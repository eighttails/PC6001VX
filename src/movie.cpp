#ifndef NOAVI

#include <string>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/avassert.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#define SCALE_FLAGS SWS_BICUBIC
#endif //NOAVI

#include "common.h"
#include "log.h"
#include "movie.h"
#include "osd.h"

#ifndef NOAVI

#ifdef av_err2str
#undef av_err2str
av_always_inline char* av_err2str(int errnum)
{
	// static char str[AV_ERROR_MAX_STRING_SIZE];
	// thread_local may be better than static in multi-thread circumstance
	thread_local char str[AV_ERROR_MAX_STRING_SIZE];
	memset(str, 0, sizeof(str));
	return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#endif

// ---------------------------------------------------
// FFMpegのサンプルmuxing.cより抜粋,改変
// ---------------------------------------------------
static int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,
					   AVStream *st, AVFrame *frame)
{
	int ret;

	// send the frame to the encoder
	ret = avcodec_send_frame(c, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame to the encoder: %s\n",
				av_err2str(ret));
		return 1;
	}

	while (ret >= 0) {
		AVPacket pkt = {};

		ret = avcodec_receive_packet(c, &pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;
		else if (ret < 0) {
			fprintf(stderr, "Error encoding a frame: %s\n", av_err2str(ret));
			return 1;
		}

		/* rescale output packet timestamp values from codec to stream timebase */
		av_packet_rescale_ts(&pkt, c->time_base, st->time_base);
		pkt.stream_index = st->index;

		/* Write the compressed frame to the media file. */
		ret = av_interleaved_write_frame(fmt_ctx, &pkt);
		av_packet_unref(&pkt);
		if (ret < 0) {
			fprintf(stderr, "Error while writing output packet: %s\n", av_err2str(ret));
			return 1;
		}
	}

	return ret == AVERROR_EOF ? 1 : 0;
}

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,
					   AVCodec **codec,
					   enum AVCodecID codec_id, int source_width, int source_height, int source_samplerate)
{
	AVCodecContext *c = NULL;
	int i = 0;

	// エンコーダーを探索
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {
		fprintf(stderr, "Could not find encoder for '%s'\n",
				avcodec_get_name(codec_id));
		return;
	}

	ost->st = avformat_new_stream(oc, NULL);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
		return;
	}
	ost->st->id = oc->nb_streams-1;
	c = avcodec_alloc_context3(*codec);
	if (!c) {
		fprintf(stderr, "Could not alloc an encoding context\n");
		return;
	}
	ost->enc = c;


	switch ((*codec)->type) {
	case AVMEDIA_TYPE_AUDIO:
		c->sample_fmt  = (*codec)->sample_fmts ?
					(*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate    = 128000;
		c->sample_rate = 44100;
		if ((*codec)->supported_samplerates) {
			c->sample_rate = (*codec)->supported_samplerates[0];
			for (i = 0; (*codec)->supported_samplerates[i]; i++) {
				if ((*codec)->supported_samplerates[i] == source_samplerate)
					c->sample_rate = source_samplerate;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if ((*codec)->channel_layouts) {
			c->channel_layout = (*codec)->channel_layouts[0];
			for (i = 0; (*codec)->channel_layouts[i]; i++) {
				if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
					c->channel_layout = AV_CH_LAYOUT_STEREO;
			}
		}
		c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
		ost->st->time_base = (AVRational){ 1, c->sample_rate };
		break;

	case AVMEDIA_TYPE_VIDEO:
		c->bit_rate = 4000000;
		c->width    = source_width;
		c->height   = source_height;
		// 60FPSに設定
		ost->st->time_base = (AVRational){ 1, 60 };
		c->time_base       = ost->st->time_base;

		c->gop_size      = 12;
		c->pix_fmt       = AV_PIX_FMT_YUV420P;
		break;

	default:
		break;
	}

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
								  uint64_t channel_layout,
								  int sample_rate, int nb_samples)
{
	AVFrame *frame = av_frame_alloc();
	int ret = 0;

	if (!frame) {
		fprintf(stderr, "Error allocating an audio frame\n");
		return NULL;
	}

	frame->format = sample_fmt;
	frame->channel_layout = channel_layout;
	frame->sample_rate = sample_rate;
	frame->nb_samples = nb_samples;

	if (nb_samples) {
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0) {
			fprintf(stderr, "Error allocating an audio buffer\n");
			return NULL;
		}
	}

	return frame;
}

static void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg, int sample_rate)
{
	AVCodecContext *c = NULL;
	int nb_samples = 0;
	int ret = 0;
	AVDictionary *opt = NULL;

	c = ost->enc;

	// コーデックを初期化
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
		return;
	}

	if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = c->frame_size;

	ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout,
									   c->sample_rate, nb_samples);
	ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
									   sample_rate, nb_samples/(c->sample_rate/sample_rate));

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		return;
	}

	// サンプル変換部
	ost->swr_ctx = swr_alloc();
	if (!ost->swr_ctx) {
		fprintf(stderr, "Could not allocate resampler context\n");
		return;
	}

	// 音声フォーマットの設定
	av_opt_set_int       (ost->swr_ctx, "in_channel_count",   c->channels,       0);
	av_opt_set_int       (ost->swr_ctx, "in_sample_rate",     sample_rate,       0);
	av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
	av_opt_set_int       (ost->swr_ctx, "out_channel_count",  c->channels,       0);
	av_opt_set_int       (ost->swr_ctx, "out_sample_rate",    c->sample_rate,    0);
	av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt",     c->sample_fmt,     0);

	// サンプル変換部を初期化
	if ((ret = swr_init(ost->swr_ctx)) < 0) {
		fprintf(stderr, "Failed to initialize the resampling context\n");
		return;
	}
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame(OutputStream *ost, AVI6 *avi)
{
	AVFrame *frame = ost->tmp_frame;
	int16_t *q = (int16_t*)frame->data[0];

	if(avi->GetAudioBuffer()->ReadySize() < frame->nb_samples){
		return NULL;
	}

	// オーディオ出力
	for (int j = 0; j <frame->nb_samples; j++) {
		short dat = avi->GetAudioBuffer()->Get();
		for (int i = 0; i < ost->enc->channels; i++){
			*q++ = dat;
		}
	}

	frame->pts = ost->next_pts;
	ost->next_pts  += frame->nb_samples;

	return frame;
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_audio_frame(AVFormatContext *oc, OutputStream *ost, AVI6 *avi)
{
	AVCodecContext *c;
	AVFrame *frame;
	int ret;
	int dst_nb_samples;

	c = ost->enc;

	frame = get_audio_frame(ost, avi);

	if (frame) {
		/* convert samples from native format to destination codec format, using the resampler */
		/* compute destination number of samples */
		dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
										c->sample_rate, c->sample_rate, AV_ROUND_UP);
		av_assert0(dst_nb_samples == frame->nb_samples);

		/* when we pass a frame to the encoder, it may keep a reference to it
		  * internally;
		  * make sure we do not overwrite it here
		  */
		ret = av_frame_make_writable(ost->frame);
		if (ret < 0)
			return 1;

		/* convert to destination format */
		ret = swr_convert(ost->swr_ctx,
						  ost->frame->data, dst_nb_samples,
						  (const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0) {
			fprintf(stderr, "Error while converting\n");
			return 1;
		}
		frame = ost->frame;

		frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
		ost->samples_count += dst_nb_samples;
	}

	return frame ? write_frame(oc, c, ost->st, frame) : 1;
}

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame *picture;
	int ret;

	picture = av_frame_alloc();
	if (!picture)
		return NULL;

	picture->format = pix_fmt;
	picture->width  = width;
	picture->height = height;

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(picture, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		return NULL;
	}

	return picture;
}

static void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
	int ret = 0;
	AVCodecContext *c = ost->enc;
	AVDictionary *opt = NULL;

	av_dict_copy(&opt, opt_arg, 0);

	// マルチスレッドエンコード設定
	c->thread_count = av_cpu_count();
	av_dict_set(&opt, "row-mt", "1", AV_OPT_SEARCH_CHILDREN);
	av_dict_set(&opt, "frame-parallel", "1", AV_OPT_SEARCH_CHILDREN);
	av_dict_set(&opt, "quality", "realtime", AV_OPT_SEARCH_CHILDREN);
	av_dict_set(&opt, "cpu-used", "8", AV_OPT_SEARCH_CHILDREN);

	// コーデックを初期化
	ret = avcodec_open2(c, codec, &opt);

	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
		return;
	}

	// フレームを初期化
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}

	// 画像フォーマットの変換元(OSD_GetWindowImage)のフォーマットがBGR0(0は未使用)であることを想定
	ost->tmp_frame = alloc_picture(AV_PIX_FMT_BGR0, c->width, c->height);
	if (!ost->tmp_frame) {
		fprintf(stderr, "Could not allocate temporary picture\n");
		return;
	}

	/* copy the stream parameters to the muxer */
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		exit(1);
	}
}

static AVFrame *get_video_frame(OutputStream *ost, BYTE* src_img)
{
	AVCodecContext *c = ost->enc;
	/* when we pass a frame to the encoder, it may keep a reference to it
	 * internally; make sure we do not overwrite it here */
	if (av_frame_make_writable(ost->frame) < 0)
		return NULL;

	if (!ost->sws_ctx) {
		ost->sws_ctx = sws_getContext(c->width, c->height,
									  AV_PIX_FMT_BGR0,
									  c->width, c->height,
									  c->pix_fmt,
									  SCALE_FLAGS, NULL, NULL, NULL);
		if (!ost->sws_ctx) {
			fprintf(stderr,
					"Could not initialize the conversion context\n");
			return NULL;
		}
	}
	// 画像をここでコピーする
	av_frame_make_writable(ost->tmp_frame);
	av_image_fill_arrays(ost->tmp_frame->data, ost->tmp_frame->linesize, src_img, AV_PIX_FMT_BGR0, c->width, c->height, 32);

	sws_scale(ost->sws_ctx,
			  (const uint8_t * const *)ost->tmp_frame->data, ost->tmp_frame->linesize,
			  0, c->height, ost->frame->data, ost->frame->linesize);


	ost->frame->pts = ost->next_pts++;

	return ost->frame;
}

static int write_video_frame(AVFormatContext *oc, OutputStream *ost, BYTE* src_img)
{
	return write_frame(oc, ost->enc, ost->st, get_video_frame(ost, src_img));
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
	avcodec_free_context(&ost->enc);
	av_frame_free(&ost->frame);
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	swr_free(&ost->swr_ctx);
}
// ---------------------------------------------------
#endif //NOAVI

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : isAVI(false), ABPP(32), fmt(NULL), oc(NULL),
	audio_codec(NULL), video_codec(NULL), opt(NULL), video_st({}), audio_st({})
{}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
AVI6::~AVI6( void )
{
	if( IsAVI() ) StopAVI();
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::Init( void )
{
#ifndef NOAVI
	PRINTD( GRP_LOG, "[MOVIE][Init]\n" );
	// av_log_set_level(AV_LOG_DEBUG);
#endif
	return true;
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ開始
//
// 引数:	filename	出力ファイル名
//			sw			スクリーンの幅
//			sh			スクリーンの高さ
//			vrate		フレームレート(fps)
//			arate		音声サンプリングレート(Hz)
//			bpp			色深度(16,24,32)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::StartAVI( const char *filename, int sw, int sh, int vrate, int arate, int bpp )
{
#ifndef NOAVI
	cCritical::Lock();
	Init();
	
	ABPP = bpp;

	// オーディオバッファ作成
	ABuf.InitBuffer( arate / vrate * 2 );
	
	// 出力コンテキスト作成
	avformat_alloc_output_context2(&oc, NULL, NULL, filename);
	if (!oc) {
		cCritical::UnLock();
		return false;
	}
	fmt = oc->oformat;

	// 音声、ビデオストリームを作成
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		// ビデオコーデックにはVP9を選択。
		fmt->video_codec = AV_CODEC_ID_VP9;
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, sw, sh, arate);
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		// FFmpegのOpusは48KHzしか扱えないため、強制的にVORBISにする。
		fmt->audio_codec = AV_CODEC_ID_VORBIS;
		add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec, sw, sh, arate);
	}

	open_video(oc, video_codec, &video_st, opt);
	open_audio(oc, audio_codec, &audio_st, opt, arate);

	av_dump_format(oc, 0, filename, 1);

	int ret = 0;
	// ファイルを開く
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
		if (0 > ret) {
			cCritical::UnLock();
			return false;
		}
	}

	// ストリームヘッダを書き込み
	ret = avformat_write_header(oc, &opt);
	if (0 > ret) {
		cCritical::UnLock();
		return false;
	}

	isAVI = true;
	cCritical::UnLock();
	return true;
#else
	return false;
#endif
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void AVI6::StopAVI( void )
{
#ifndef NOAVI
	if(oc){
		cCritical::Lock();
		isAVI = false;
		av_write_trailer(oc);
		close_stream(oc, &video_st);
		close_stream(oc, &audio_st);
		avio_closep(&oc->pb);
		avformat_free_context(oc);
		oc = NULL;
		cCritical::UnLock();
	}
#endif
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ中?
//
// 引数:	なし
// 返値:	bool	true:キャプチャ中 false:ひま
////////////////////////////////////////////////////////////////
bool AVI6::IsAVI( void )
{
	return isAVI;
}


////////////////////////////////////////////////////////////////
// AVI1フレーム書出し
//
// 引数:	wh		ウィンドウハンドル
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::AVIWriteFrame( HWINDOW wh )
{
#ifndef NOAVI
	if( !wh ) return false;
	
	cCritical::Lock();

	int xx = OSD_GetWindowWidth(wh);
	int yy = OSD_GetWindowHeight(wh);

	// 途中でウィンドウサイズが変わっていたら記録を中断
	if(xx != video_st.tmp_frame->width || yy != video_st.tmp_frame->height){
		StopAVI();
		cCritical::UnLock();
		return false;
	}

	Sbuf.resize(xx * yy * ABPP / 4);

	VRect ss;
	ss.x = 0;
	ss.y = 0;
	ss.w = xx;
	ss.h = yy;
	if( !OSD_GetWindowImage( wh, (void **)&Sbuf, &ss ) ) return false;

	int encode_video = 1, encode_audio = 1;
	while (encode_video || encode_audio) {
		/* select the stream to encode */
		if (encode_video &&
				(!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
												audio_st.next_pts, audio_st.enc->time_base) <= 0)) {
			write_video_frame(oc, &video_st, &Sbuf[0]);
			encode_video = 0;
		} else {
			encode_audio = !write_audio_frame(oc, &audio_st, this);
		}
	}
	cCritical::UnLock();
	return true;
#else
	return false;
#endif
}


////////////////////////////////////////////////////////////////
// オーディオバッファ取得
//
// 引数:	なし
// 返値:	cRing *		バッファオブジェクトへのポインタ
////////////////////////////////////////////////////////////////
cRing *AVI6::GetAudioBuffer( void )
{
	return &ABuf;
}




