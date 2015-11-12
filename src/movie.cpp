#ifndef NOAVI

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/avassert.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
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
const char* MakeErrorString(int errnum)
{
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
	return (std::string(errbuf)).c_str();
}

const char* TS2Str(int64_t ts)
{
	char buf[AV_TS_MAX_STRING_SIZE];
	av_ts_make_string(&buf[0], ts);
	return (std::string(buf)).c_str();
}

const char* Ts2TimeStr(int64_t ts, AVRational *tb)
{
	char buf[AV_TS_MAX_STRING_SIZE];
	av_ts_make_time_string(&buf[0], ts, tb);
	return (std::string(buf)).c_str();
}

static void LogPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

	printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
		   TS2Str(pkt->pts), Ts2TimeStr(pkt->pts, time_base),
		   TS2Str(pkt->dts), Ts2TimeStr(pkt->dts, time_base),
		   TS2Str(pkt->duration), Ts2TimeStr(pkt->duration, time_base),
		   pkt->stream_index);
}

// FFMpegのサンプルmuxing.cより抜粋,改変
// ---------------------------------------------------
static int WriteFrame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
	// タイムスタンプを変換
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index = st->index;

	// フレームを書き込み
	//LogPacket(fmt_ctx, pkt);
	return av_interleaved_write_frame(fmt_ctx, pkt);
}

/* Add an output stream. */
static void AddStream(OutputStream *ost, AVFormatContext *oc,
					   AVCodec **codec,
					   enum AVCodecID codec_id, int source_width, int source_height)
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

	ost->st = avformat_new_stream(oc, *codec);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
		return;
	}
	ost->st->id = oc->nb_streams-1;
	c = ost->st->codec;

	switch ((*codec)->type) {
	case AVMEDIA_TYPE_AUDIO:
		c->sample_fmt  = (*codec)->sample_fmts ?
			(*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate    = 64000;
		c->sample_rate = 44100;
		if ((*codec)->supported_samplerates) {
			c->sample_rate = (*codec)->supported_samplerates[0];
			for (i = 0; (*codec)->supported_samplerates[i]; i++) {
				if ((*codec)->supported_samplerates[i] == 44100)
					c->sample_rate = 44100;
			}
		}
		c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if ((*codec)->channel_layouts) {
			c->channel_layout = (*codec)->channel_layouts[0];
			for (i = 0; (*codec)->channel_layouts[i]; i++) {
				if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
					c->channel_layout = AV_CH_LAYOUT_STEREO;
			}
		}
		c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
		ost->st->time_base = (AVRational){ 1, c->sample_rate };
		break;

	case AVMEDIA_TYPE_VIDEO:
		c->codec_id = codec_id;

		c->bit_rate = 1000000;
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
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

static AVFrame *AllocAudioFrame(enum AVSampleFormat sample_fmt,
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

static void OpenAudio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg, int sample_rate)
{
	AVCodecContext *c = NULL;
	int nb_samples = 0;
	int ret = 0;
	AVDictionary *opt = NULL;

	c = ost->st->codec;

	// コーデックを初期化
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open audio codec: %s\n", MakeErrorString(ret));
		return;
	}

	if (c->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = c->frame_size;

	ost->frame     = AllocAudioFrame(c->sample_fmt, c->channel_layout,
									   c->sample_rate, nb_samples);
	ost->tmp_frame = AllocAudioFrame(AV_SAMPLE_FMT_S16, c->channel_layout,
									   sample_rate, nb_samples/(c->sample_rate/sample_rate));

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

static AVFrame *GetAudioFrame(OutputStream *ost, AVI6 *avi)
{
	AVFrame *frame = ost->tmp_frame;
	int16_t *q = (int16_t*)frame->data[0];

	if(avi->GetAudioBuffer()->ReadySize() < frame->nb_samples){
		return NULL;
	}

	// オーディオ出力
	for (int j = 0; j <frame->nb_samples; j++) {
		short dat = avi->GetAudioBuffer()->Get();
		for (int i = 0; i < ost->st->codec->channels; i++){
			*q++ = dat;
		}
	}

	frame->pts = ost->next_pts;
	ost->next_pts  += frame->nb_samples;

	return frame;
}

static int WriteAudioFrame(AVFormatContext *oc, OutputStream *ost, AVI6 *avi)
{
	AVCodecContext *c = NULL;
	AVPacket pkt = { 0 };
	AVFrame *frame = NULL;
	int ret = 0;
	int got_packet = 0;
	int dst_nb_samples = 0;

	av_init_packet(&pkt);
	c = ost->st->codec;

	frame = GetAudioFrame(ost, avi);

	if (frame) {
		// フォーマット変換後のサンプル数を決定
		dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, frame->sample_rate) + frame->nb_samples,
										c->sample_rate, c->sample_rate, AV_ROUND_UP);
		//av_assert0(dst_nb_samples == frame->nb_samples);

		// フレームを書き込み可能にする
		ret = av_frame_make_writable(ost->frame);
		if (ret < 0)
			exit(1);

		// 音声フォーマットを変換
		ret = swr_convert(ost->swr_ctx,
						  ost->frame->data, dst_nb_samples,
						  (const uint8_t **)frame->data, frame->nb_samples);
		if (ret < 0) {
			fprintf(stderr, "Error while converting\n");
			return 0;
		}
		frame = ost->frame;

		frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
		ost->samples_count += dst_nb_samples;

		ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
		if (ret < 0) {
			fprintf(stderr, "Error encoding audio frame: %s\n", MakeErrorString(ret));
			return 0;
		}

		if (got_packet) {
			ret = WriteFrame(oc, &c->time_base, ost->st, &pkt);
			if (ret < 0) {
				fprintf(stderr, "Error while writing audio frame: %s\n",
						MakeErrorString(ret));
				return 0;
			}
		}
	}

	return (frame || got_packet) ? 0 : 1;
}

static AVFrame *AllocPicture(enum AVPixelFormat pix_fmt, int width, int height)
{
	AVFrame *picture = 0;
	int ret = 0;

	picture = av_frame_alloc();
	if (!picture)
		return NULL;

	picture->format = pix_fmt;
	picture->width  = width;
	picture->height = height;

	// 画像バッファを確保
	ret = av_frame_get_buffer(picture, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		return NULL;
	}

	return picture;
}

static void OpenVideo(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
	int ret = 0;
	AVCodecContext *c = ost->st->codec;
	AVDictionary *opt = NULL;

	av_dict_copy(&opt, opt_arg, 0);

	// コーデックを初期化
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open video codec: %s\n", MakeErrorString(ret));
		return;
	}

	// フレームを初期化
	ost->frame = AllocPicture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}

	// 画像フォーマットの変換元(OSD_GetWindowImage)のフォーマットがBGR0(0は未使用)であることを想定
	ost->tmp_frame = NULL;
	ost->tmp_frame = AllocPicture(AV_PIX_FMT_BGR0, c->width, c->height);
	if (!ost->tmp_frame) {
		fprintf(stderr, "Could not allocate temporary picture\n");
		return;
	}
}

static AVFrame *GetVideoFrame(OutputStream *ost, BYTE* src_img)
{
	AVCodecContext *c = ost->st->codec;

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
	avpicture_fill((AVPicture*)ost->tmp_frame, src_img, AV_PIX_FMT_BGR0, c->width, c->height);

	sws_scale(ost->sws_ctx,
			  (const uint8_t * const *)ost->tmp_frame->data, ost->tmp_frame->linesize,
			  0, c->height, ost->frame->data, ost->frame->linesize);


	ost->frame->pts = ost->next_pts++;

	return ost->frame;
}

static int WriteVideoFrame(AVFormatContext *oc, OutputStream *ost, BYTE* src_img)
{
	int ret = 0;
	AVCodecContext *c = NULL;
	AVFrame *frame = NULL;
	int got_packet = 0;

	c = ost->st->codec;

	frame = GetVideoFrame(ost, src_img);

	if (oc->oformat->flags & AVFMT_RAWPICTURE) {
		/* a hack to avoid data copy with some raw video muxers */
		AVPacket pkt;
		av_init_packet(&pkt);

		if (!frame)
			return 1;

		pkt.flags        |= AV_PKT_FLAG_KEY;
		pkt.stream_index  = ost->st->index;
		pkt.data          = (uint8_t *)frame;
		pkt.size          = sizeof(AVPicture);

		pkt.pts = pkt.dts = frame->pts;
		av_packet_rescale_ts(&pkt, c->time_base, ost->st->time_base);

		ret = av_interleaved_write_frame(oc, &pkt);
	} else {
		AVPacket pkt = { 0 };
		av_init_packet(&pkt);

		// 映像をエンコード
		ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
		if (ret < 0) {
			fprintf(stderr, "Error encoding video frame: %s\n", MakeErrorString(ret));
			return 0;
		}

		if (got_packet) {
			ret = WriteFrame(oc, &c->time_base, ost->st, &pkt);
		} else {
			ret = 0;
		}
	}

	if (ret < 0) {
		fprintf(stderr, "Error while writing video frame: %s\n", MakeErrorString(ret));
		return 0;
	}

	return (frame || got_packet) ? 0 : 1;
}

static void CloseStream(AVFormatContext *oc, OutputStream *ost)
{
	avcodec_close(ost->st->codec);
	av_frame_free(&ost->frame);
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	ost->sws_ctx = NULL;
	swr_free(&ost->swr_ctx);
}
// ---------------------------------------------------
#endif //NOAVI

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : isAVI(false), ABPP(32), fmt(NULL), oc(NULL),
	audio_codec(NULL), video_codec(NULL), opt(NULL), video_st(), audio_st()
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

	// FFMpegの初期化
	av_register_all();
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
	if (!oc) return false;

	fmt = oc->oformat;

	// 音声、ビデオストリームを作成
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		// ビデオコーデックにVP9を選択されると画像が崩れるため、暫定措置として強制的にVP8にする。
		fmt->video_codec = AV_CODEC_ID_VP8;
		AddStream(&video_st, oc, &video_codec, fmt->video_codec, sw, sh);
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		// オーディオコーデックにOPUSを選択されると落ちるため、暫定措置として強制的にVORBISにする。
		fmt->audio_codec = AV_CODEC_ID_VORBIS;
		AddStream(&audio_st, oc, &audio_codec, fmt->audio_codec, sw, sh);
	}

	OpenVideo(oc, video_codec, &video_st, opt);
	OpenAudio(oc, audio_codec, &audio_st, opt, arate);

	av_dump_format(oc, 0, filename, 1);

	int ret = 0;
	// ファイルを開く
	if (!(fmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
		if (0 > ret) {
			return false;
		}
	}

	// ストリームヘッダを書き込み
	ret = avformat_write_header(oc, &opt);
	if (0 > ret) {
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
		CloseStream(oc, &video_st);
		CloseStream(oc, &audio_st);
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
				(!encode_audio || av_compare_ts(video_st.next_pts, video_st.st->codec->time_base,
												audio_st.next_pts, audio_st.st->codec->time_base) <= 0)) {
			WriteVideoFrame(oc, &video_st, &Sbuf[0]);
			encode_video = 0;
		} else {
			encode_audio = !WriteAudioFrame(oc, &audio_st, this);
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




