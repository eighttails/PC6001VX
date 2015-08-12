#ifndef NOAVI

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/avassert.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#define SCALE_FLAGS SWS_BICUBIC

#include "common.h"
#include "log.h"
#include "movie.h"
#include "osd.h"

const char* av_make_error_string(int errnum)
{
	char errbuf[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
	return (std::string(errbuf)).c_str();
}

// FFMpegのサンプルmuxing.cより抜粋,改変
// ---------------------------------------------------
static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index = st->index;

	return av_interleaved_write_frame(fmt_ctx, pkt);
}

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,
					   AVCodec **codec,
					   enum AVCodecID codec_id, int source_width, int source_height)
{
	AVCodecContext *c;
	int i;

	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {
		fprintf(stderr, "Could not find encoder for '%s'\n",
				avcodec_get_name(codec_id));
		exit(1);
	}

	ost->st = avformat_new_stream(oc, *codec);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
		exit(1);
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

		c->bit_rate = 400000;
		/* Resolution must be a multiple of two. */
		c->width    = source_width - (source_width % 2);
		c->height   = source_height - (source_height % 2);
		/* timebase: This is the fundamental unit of time (in seconds) in terms
		 * of which frame timestamps are represented. For fixed-fps content,
		 * timebase should be 1/framerate and timestamp increments should be
		 * identical to 1. */
		ost->st->time_base = (AVRational){ 1, 60 };
		c->time_base       = ost->st->time_base;

		c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
		c->pix_fmt       = AV_PIX_FMT_YUV420P;
		if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
			/* just for testing, we also add B frames */
			c->max_b_frames = 2;
		}
		if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
			/* Needed to avoid using macroblocks in which some coeffs overflow.
			 * This does not happen with normal video, it just happens here as
			 * the motion of the chroma plane does not match the luma plane. */
			c->mb_decision = 2;
		}
	break;

	default:
		break;
	}

	/* Some formats want stream headers to be separate. */
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
								  uint64_t channel_layout,
								  int sample_rate, int nb_samples)
{
	AVFrame *frame = av_frame_alloc();
	int ret;

	if (!frame) {
		fprintf(stderr, "Error allocating an audio frame\n");
		exit(1);
	}

	frame->format = sample_fmt;
	frame->channel_layout = channel_layout;
	frame->sample_rate = sample_rate;
	frame->nb_samples = nb_samples;

	if (nb_samples) {
		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0) {
			fprintf(stderr, "Error allocating an audio buffer\n");
			exit(1);
		}
	}

	return frame;
}

static void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
	AVCodecContext *c;
	int nb_samples;
	int ret;
	AVDictionary *opt = NULL;

	c = ost->st->codec;

	/* open it */
	av_dict_copy(&opt, opt_arg, 0);
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open audio codec: %s\n", av_make_error_string(ret));
		exit(1);
	}

	if (c->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
		nb_samples = 10000;
	else
		nb_samples = c->frame_size;

	ost->frame     = alloc_audio_frame(c->sample_fmt, c->channel_layout,
									   c->sample_rate, nb_samples);
	ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
									   c->sample_rate, nb_samples);
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame(OutputStream *ost)
{
	AVFrame *frame = ost->tmp_frame;
	int16_t *q = (int16_t*)frame->data[0];

	// #PENDING サンプルを実際に入れる

	frame->pts = ost->next_pts;
	ost->next_pts  += frame->nb_samples;

	return frame;
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_audio_frame(AVFormatContext *oc, OutputStream *ost)
{
	AVCodecContext *c;
	AVPacket pkt = { 0 }; // data and size must be 0;
	AVFrame *frame;
	int ret;
	int got_packet;
	int dst_nb_samples;

	av_init_packet(&pkt);
	c = ost->st->codec;

	frame = get_audio_frame(ost);

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
			exit(1);

			/* convert to destination format */
			ret = swr_convert(ost->swr_ctx,
							  ost->frame->data, dst_nb_samples,
							  (const uint8_t **)frame->data, frame->nb_samples);
			if (ret < 0) {
				fprintf(stderr, "Error while converting\n");
				exit(1);
			}
			frame = ost->frame;

		frame->pts = av_rescale_q(ost->samples_count, (AVRational){1, c->sample_rate}, c->time_base);
		ost->samples_count += dst_nb_samples;
	}

	ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
	if (ret < 0) {
		fprintf(stderr, "Error encoding audio frame: %s\n", av_make_error_string(ret));
		exit(1);
	}

	if (got_packet) {
		ret = write_frame(oc, &c->time_base, ost->st, &pkt);
		if (ret < 0) {
			fprintf(stderr, "Error while writing audio frame: %s\n",
					av_make_error_string(ret));
			exit(1);
		}
	}

	return (frame || got_packet) ? 0 : 1;
}

/**************************************************************/
/* video output */

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
	ret = av_frame_get_buffer(picture, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		exit(1);
	}

	return picture;
}

static void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
	int ret;
	AVCodecContext *c = ost->st->codec;
	AVDictionary *opt = NULL;

	av_dict_copy(&opt, opt_arg, 0);

	/* open the codec */
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		fprintf(stderr, "Could not open video codec: %s\n", av_make_error_string(ret));
		exit(1);
	}

	/* allocate and init a re-usable frame */
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	 * picture is needed too. It is then converted to the required
	 * output format. */
	ost->tmp_frame = NULL;
	ost->tmp_frame = alloc_picture(AV_PIX_FMT_BGR0, c->width, c->height);
	if (!ost->tmp_frame) {
		fprintf(stderr, "Could not allocate temporary picture\n");
		exit(1);
	}
}

static AVFrame *get_video_frame(OutputStream *ost, BYTE* src_img)
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
			exit(1);
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

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_video_frame(AVFormatContext *oc, OutputStream *ost, BYTE* src_img)
{
	int ret;
	AVCodecContext *c;
	AVFrame *frame;
	int got_packet = 0;

	c = ost->st->codec;

	frame = get_video_frame(ost, src_img);

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

		/* encode the image */
		ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
		if (ret < 0) {
			fprintf(stderr, "Error encoding video frame: %s\n", av_make_error_string(ret));
			exit(1);
		}

		if (got_packet) {
			ret = write_frame(oc, &c->time_base, ost->st, &pkt);
		} else {
			ret = 0;
		}
	}

	if (ret < 0) {
		fprintf(stderr, "Error while writing video frame: %s\n", av_make_error_string(ret));
		exit(1);
	}

	return (frame || got_packet) ? 0 : 1;
}

static void close_stream(AVFormatContext *oc, OutputStream *ost)
{
	avcodec_close(ost->st->codec);
	av_frame_free(&ost->frame);
	av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	swr_free(&ost->swr_ctx);
}
// ---------------------------------------------------


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : ABPP(32), fmt(NULL), oc(NULL),
	audio_codec(NULL), video_codec(NULL), opt(NULL), video_st{0}, audio_st{0},
	anum(0) {}


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
	PRINTD( GRP_LOG, "[MOVIE][Init]\n" );

	ABPP     = 32;

	// FFMpegの初期化
	av_register_all();
	
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
	Init();
	
	ABPP = bpp;

	// オーディオバッファ作成
	ABuf.InitBuffer( arate / vrate * 2 );
	// カウンタ初期化
	anum = 0;
	
	// 出力コンテキスト作成
	avformat_alloc_output_context2(&oc, NULL, NULL, filename);
	if (!oc) return false;

	fmt = oc->oformat;

	/* Add the audio and video streams using the default format codecs
		 * and initialize the codecs. */
	if (fmt->video_codec != AV_CODEC_ID_NONE) {
		add_stream(&video_st, oc, &video_codec, fmt->video_codec, sw, sh);
	}
	if (fmt->audio_codec != AV_CODEC_ID_NONE) {
		//#PENDING あとで戻す
		//add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec, sw, sh);
	}

	open_video(oc, video_codec, &video_st, opt);
	//#PENDING あとで戻す
	//open_audio(oc, audio_codec, &audio_st, opt);

	av_dump_format(oc, 0, filename, 1);

	// ファイルを開く
	if (!(fmt->flags & AVFMT_NOFILE)) {
		if (0 > avio_open(&oc->pb, filename, AVIO_FLAG_WRITE)) {
			return false;
		}
	}

	// ストリームヘッダを書き込み
	if (0 > avformat_write_header(oc, &opt)) {
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void AVI6::StopAVI( void )
{
	if(oc){
		av_write_trailer(oc);
		close_stream(oc, &video_st);
		//#PENDING あとで戻す
		//close_stream(oc, &audio_st);
		avio_close(oc->pb);
		avformat_free_context(oc);
		oc = NULL;
	}
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ中?
//
// 引数:	なし
// 返値:	bool	true:キャプチャ中 false:ひま
////////////////////////////////////////////////////////////////
bool AVI6::IsAVI( void )
{
	return oc ? true : false;
}


////////////////////////////////////////////////////////////////
// AVI1フレーム書出し
//
// 引数:	wh		ウィンドウハンドル
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::AVIWriteFrame( HWINDOW wh )
{
	if( !wh ) return false;
	
	int xx = OSD_GetWindowWidth(wh);
	int yy =  OSD_GetWindowHeight(wh);

	Sbuf.resize(xx * yy * ABPP / 4);

	VRect ss;
	ss.x = 0;
	ss.y = 0;
	ss.w = xx;
	ss.h = yy;
	if( !OSD_GetWindowImage( wh, (void **)&Sbuf, &ss ) ) return false;
	
#if 0
	switch( ABPP ){
	case 16:	// 16bitの場合
		for( int y = yy - 1; y >= 0; y-- ){
			DWORD *src = (DWORD *)Sbuf + vmh.dwWidth * y;
			for( int x=0; x < xx; x++ ){
				WORD dat = (((*src)&RMASK32)>>(RSHIFT32+3))<<10 |
						   (((*src)&GMASK32)>>(GSHIFT32+3))<<5 |
						   (((*src)&BMASK32)>>(BSHIFT32+3));
				FPUTWORD( dat, vfp );
				src++;
			}
		}
		break;
		
	case 24:	// 24bitの場合
		for( int y = yy - 1; y >= 0; y-- ){
			DWORD *src = (DWORD *)Sbuf + vmh.dwWidth * y;
			for( int x=0; x < xx; x++ ){
				FPUTBYTE( ((*src)&BMASK32)>>BSHIFT32, vfp );
				FPUTBYTE( ((*src)&GMASK32)>>GSHIFT32, vfp );
				FPUTBYTE( ((*src)&RMASK32)>>RSHIFT32, vfp );
				src++;
			}
		}
		break;
		
	case 32:	// 32bitの場合
		for( int y = yy - 1; y >= 0; y-- )
			fwrite( (BYTE *)((DWORD *)Sbuf + vmh.dwWidth * y), sizeof(DWORD), xx, vfp );
	}
#endif

	write_video_frame(oc, &video_st, &Sbuf[0]);
	//write_audio_frame(oc, &audio_st);

	// オーディオ出力
	if( ABuf.ReadySize() > 0 ){
		anum += ABuf.ReadySize();
		//FPUTDWORD( CID01WB, vfp );
		//FPUTDWORD( ABuf.ReadySize()*2, vfp );
		while( ABuf.ReadySize() > 0 ){
			short dat = ABuf.Get();
			//FPUTWORD( dat, vfp );
		}
	}
	
	return true;
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




#endif //NOAVI
