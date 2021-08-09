/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef NOAVI

#include <string>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/avassert.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


#include <libavutil/imgutils.h>
}
#define SCALE_FLAGS SWS_BICUBIC
#endif // NOAVI

#include "common.h"
#include "log.h"
#include "movie.h"

#ifndef NOAVI


// AV_PIX_FMT_RGB555LE	packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0
// AV_PIX_FMT_RGB24		packed RGB 8:8:8, 24bpp, RGBRGB...
// AV_PIX_FMT_BGR24		packed RGB 8:8:8, 24bpp, BGRBGR...
// AV_PIX_FMT_BGR0		packed BGR 8:8:8, 32bpp, BGR0BGR0...
// AV_PIX_FMT_0RGB		packed RGB 8:8:8, 32bpp, 0RGB0RGB...

// ピクセルフォーマット -> ffmpegのピクセルフォーマット
AVPixelFormat GetPixelFormat( const PixelFMT pf )
{
	return pf == PX16RGB ? AV_PIX_FMT_RGB555LE :
		   pf == PX24RGB ? AV_PIX_FMT_RGB24 :
						   AV_PIX_FMT_0RGB;
}


// libavcodecのutil.cより抜粋,改変
// ---------------------------------------------------
AVHWAccel* ff_find_hwaccel( enum AVCodecID codec_id, enum AVPixelFormat pix_fmt )
{
	AVHWAccel* hwaccel = nullptr;
	
	while( (hwaccel = av_hwaccel_next( hwaccel )) ){
		if(    hwaccel->id      == codec_id
			&& hwaccel->pix_fmt == pix_fmt )
			return hwaccel;
	}
	return nullptr;
}


// ---------------------------------------------------

// FFMpegのサンプルmuxing.cより抜粋,改変
// ---------------------------------------------------
static int WriteFrame( AVFormatContext* fmt_ctx, const AVRational* time_base, AVStream* st, AVPacket* pkt )
{
	// タイムスタンプを変換
	av_packet_rescale_ts( pkt, *time_base, st->time_base );
	pkt->stream_index = st->index;
	
	// フレームを書き込み
	return av_interleaved_write_frame( fmt_ctx, pkt );
}


/////////////////////////////////////////////////////////////////////////////
/* Add an output stream. */
static bool AddStream( OutputStream& ost, AVFormatContext* oc, AVCodec*& codec,
					   enum AVCodecID codec_id, int source_width, int source_height, double rate )
{
	// エンコーダーを探索
	codec = avcodec_find_encoder( codec_id );
	if( !codec ){ return false; }
	
	ost.st = avformat_new_stream( oc, codec );
	if( !ost.st ){ return false; }
	
	ost.st->id        = oc->nb_streams-1;
	AVCodecContext* c = ost.st->codec;
	
	switch( codec->type ){
	case AVMEDIA_TYPE_AUDIO:
		c->sample_fmt  = codec->sample_fmts ? codec->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
		c->bit_rate    = 128000;
		c->sample_rate = rate;
		if( codec->supported_samplerates ){
			c->sample_rate = codec->supported_samplerates[0];
			for( int i = 0; codec->supported_samplerates[i]; i++ ){
				if( codec->supported_samplerates[i] == rate ){
					c->sample_rate = rate;
				}
			}
		}
		c->channels       = av_get_channel_layout_nb_channels( c->channel_layout );
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if( codec->channel_layouts ){
			c->channel_layout = codec->channel_layouts[0];
			for( int i = 0; codec->channel_layouts[i]; i++ ){
				if( codec->channel_layouts[i] == AV_CH_LAYOUT_STEREO ){
					c->channel_layout = AV_CH_LAYOUT_STEREO;
				}
			}
		}
		c->channels       = av_get_channel_layout_nb_channels( c->channel_layout );
		ost.st->time_base = (AVRational){ 1, c->sample_rate };
		break;
		
	case AVMEDIA_TYPE_VIDEO:
		c->codec_id       = codec_id;
		c->bit_rate       = 4000000;
		c->width          = source_width;
		c->height         = source_height;
		ost.st->time_base = (AVRational){ 1000000, (int)(rate * 1000000.0) };
		c->time_base      = ost.st->time_base;
		c->gop_size       = 12;
		c->pix_fmt        = AV_PIX_FMT_YUV420P;
		c->hwaccel        = ff_find_hwaccel( c->codec->id, c->pix_fmt );
		break;
		
	default:
		break;
	}
	
	if( oc->oformat->flags & AVFMT_GLOBALHEADER ){
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
/* audio output */
static AVFrame* AllocAudioFrame( enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples )
{
	AVFrame* frame = av_frame_alloc();
	if( !frame ){ return nullptr; }
	
	frame->format         = sample_fmt;
	frame->channel_layout = channel_layout;
	frame->sample_rate    = sample_rate;
	frame->nb_samples     = nb_samples;
	
	if( nb_samples && (av_frame_get_buffer( frame, 0 ) < 0) ){
		return nullptr;
	}
	
	return frame;
}


/////////////////////////////////////////////////////////////////////////////
static bool OpenAudio( AVFormatContext* oc, AVCodec* codec, OutputStream& ost, AVDictionary* opt_arg, int sample_rate )
{
	int nb_samples = 0;
	AVCodecContext* c = ost.st->codec;
	
	// コーデックを初期化
//	int ret = 0;
//	AVDictionary* opt = nullptr;
//	av_dict_copy( &opt, opt_arg, 0 );
//	ret = avcodec_open2( c, codec, &opt );
//	av_dict_free( &opt );
//	if( ret < 0 ){ return; }
	if( avcodec_open2( c, codec, &opt_arg ) < 0 ){ return false; }
	
	nb_samples = (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE) ? 10000 : c->frame_size;
	
	ost.frame     = AllocAudioFrame( c->sample_fmt,     c->channel_layout, c->sample_rate, nb_samples );
	ost.tmp_frame = AllocAudioFrame( AV_SAMPLE_FMT_S16, c->channel_layout, sample_rate,    nb_samples/(c->sample_rate/sample_rate) );
	
	// フレームを書き込み可能にする
	av_frame_make_writable( ost.frame );
	av_frame_make_writable( ost.tmp_frame );
	
	
	// サンプル変換部
	ost.swr_ctx = swr_alloc();
	if( !ost.swr_ctx ){ return false; }
	
	// 音声フォーマットの設定
	av_opt_set_int       ( ost.swr_ctx, "in_channel_count",  c->channels,       0 );
	av_opt_set_int       ( ost.swr_ctx, "in_sample_rate",    sample_rate,       0 );
	av_opt_set_sample_fmt( ost.swr_ctx, "in_sample_fmt",     AV_SAMPLE_FMT_S16, 0 );
	av_opt_set_int       ( ost.swr_ctx, "out_channel_count", c->channels,       0 );
	av_opt_set_int       ( ost.swr_ctx, "out_sample_rate",   c->sample_rate,    0 );
	av_opt_set_sample_fmt( ost.swr_ctx, "out_sample_fmt",    c->sample_fmt,     0 );
	
	// サンプル変換部を初期化
	if( swr_init( ost.swr_ctx ) < 0 ){ return false; }
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
static AVFrame* GetAudioFrame( OutputStream& ost, AVI6* avi )
{
	AVCodecContext* c = ost.st->codec;
	AVFrame* frame    = ost.tmp_frame;
	int16_t* q        = (int16_t*)frame->data[0];
	
	if( avi->GetAudioBuffer()->ReadySize() < frame->nb_samples ){
		return nullptr;
	}
	
	// オーディオ出力
	for( int j = 0; j <frame->nb_samples; j++ ){
		short dat = avi->GetAudioBuffer()->Get();
		for( int i = 0; i < ost.st->codec->channels; i++ ){
			*q++ = dat;
		}
	}
	
	frame->pts = ost.next_pts;
	ost.next_pts += frame->nb_samples;
	
	// フォーマット変換後のサンプル数を決定
	int dst_nb_samples = 0;
	dst_nb_samples = av_rescale_rnd( swr_get_delay( ost.swr_ctx, frame->sample_rate ) + frame->nb_samples,
									 c->sample_rate, c->sample_rate, AV_ROUND_UP );
	
	// 音声フォーマットを変換
	if( swr_convert( ost.swr_ctx, ost.frame->data, dst_nb_samples, (const uint8_t **)frame->data, frame->nb_samples ) < 0 ){
		return nullptr;
	}
	ost.samples_count += dst_nb_samples;
	ost.frame->pts = av_rescale_q( ost.samples_count, (AVRational){ 1, c->sample_rate }, c->time_base );
	
	return ost.frame;
}


/////////////////////////////////////////////////////////////////////////////
static int WriteAudioFrame( AVFormatContext* oc, OutputStream& ost, AVI6* avi )
{
	AVPacket pkt;
	AVCodecContext* c = ost.st->codec;
	AVFrame* frame    = GetAudioFrame( ost, avi );
	
	if( !frame || !frame->pts ){ return 1; }
	
	if( avcodec_send_frame( c, frame ) < 0 ){ return 0; }
	
	av_init_packet( &pkt );
	if( (avcodec_receive_packet( c, &pkt ) < 0) || (WriteFrame( oc, &c->time_base, ost.st, &pkt ) < 0) ){
		return 0;
	}
	
	return 0;
}




/////////////////////////////////////////////////////////////////////////////
static AVFrame* AllocPicture( enum AVPixelFormat pix_fmt, int width, int height )
{
	AVFrame* picture = av_frame_alloc();
	if( !picture ){ return nullptr; }
	
	picture->format = pix_fmt;
	picture->width  = width;
	picture->height = height;
	
	// 画像バッファを確保
	if( av_frame_get_buffer( picture, 32 ) < 0 ){	// 32byte aligned
		return nullptr;
	}
	
	return picture;
}


/////////////////////////////////////////////////////////////////////////////
static bool OpenVideo( AVFormatContext* oc, AVCodec* codec, OutputStream& ost, AVDictionary* opt_arg, enum AVPixelFormat pix_fmt )
{
	AVCodecContext* c = ost.st->codec;
	
	// コーデックを初期化
//	int ret = 0;
//	AVDictionary* opt = nullptr;
//	av_dict_copy( &opt, opt_arg, 0 );
//	ret = avcodec_open2( c, codec, &opt );
//	av_dict_free( &opt );
//	if( ret < 0 ){ return; }
	if( avcodec_open2( c, codec, &opt_arg ) < 0 ){ return false; }
	
	// フレームを初期化
	ost.frame = AllocPicture( c->pix_fmt, c->width, c->height );
	if( !ost.frame ){ return false; }
	
	// 画像フォーマットの変換元(OSD_GetWindowImage)のフォーマットに合わせて初期化
	ost.tmp_frame = AllocPicture( pix_fmt, c->width, c->height );
	if( !ost.tmp_frame ){ return false; }
	
	av_frame_make_writable( ost.frame );
	av_frame_make_writable( ost.tmp_frame );
	
	// スケーラの設定
	if( !ost.sws_ctx ){
		ost.sws_ctx = sws_getContext( c->width, c->height, pix_fmt, c->width, c->height, c->pix_fmt,
									   SCALE_FLAGS, nullptr, nullptr, nullptr);
		if( !ost.sws_ctx ){ return false; }
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
static AVFrame* GetVideoFrame( OutputStream& ost, std::vector<BYTE>& src_img, enum AVPixelFormat pix_fmt )
{
	AVCodecContext* c = ost.st->codec;
	
	// ウィンドウから画像をコピー
	// 変換元(OSD_GetWindowImage)の画像データは4byte aligned
	av_image_fill_arrays( ost.tmp_frame->data, ost.tmp_frame->linesize, src_img.data(), pix_fmt, c->width, c->height, 4 );
	
	// 変換
	sws_scale( ost.sws_ctx, (const uint8_t * const *)ost.tmp_frame->data, ost.tmp_frame->linesize,
			   0, c->height, ost.frame->data, ost.frame->linesize );
	
	ost.frame->pts = ost.next_pts++;
	
	return ost.frame;
}


/////////////////////////////////////////////////////////////////////////////
static int WriteVideoFrame( AVFormatContext* oc, OutputStream& ost, std::vector<BYTE>& src_img, enum AVPixelFormat pix_fmt )
{
	AVPacket pkt;
	AVCodecContext* c = ost.st->codec;
	AVFrame* frame    = GetVideoFrame( ost, src_img, pix_fmt );
	
	if( !frame ){ return 0; }
	
	if( avcodec_send_frame( c, frame ) < 0 ){ return 0; }
	
	av_init_packet( &pkt );
	if( (avcodec_receive_packet( c, &pkt ) < 0) || (WriteFrame( oc, &c->time_base, ost.st, &pkt ) < 0) ){
		return 0;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
static void CloseStream( OutputStream& ost )
{
	avcodec_close( ost.st->codec );
	av_frame_free( &ost.frame );
	av_frame_free( &ost.tmp_frame );
	sws_freeContext( ost.sws_ctx );
	ost.sws_ctx = nullptr;
	swr_free( &ost.swr_ctx );
	ost.swr_ctx = nullptr;
}
// ---------------------------------------------------
#endif // NOAVI




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : isAVI(false), oc(nullptr),
	audio_codec(nullptr), video_codec(nullptr), video_st(), audio_st(), pixfmt(PX32ARGB), req(0)
{}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
AVI6::~AVI6( void )
{
	StopAVI();
}


/////////////////////////////////////////////////////////////////////////////
// ビデオキャプチャ開始
//
// 引数:	filename	出力ファイルパス
//			sw			スクリーンの幅
//			sh			スクリーンの高さ
//			vrate		フレームレート(fps)
//			arate		音声サンプリングレート(Hz)
//			bpp			色深度(16,24,32)
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool AVI6::StartAVI( const P6VPATH& filepath, int sw, int sh, double vrate, int arate, int bpp )
{
#ifndef NOAVI
	std::lock_guard<cMutex> lock( Mutex );
	
	AVDictionary* opt = nullptr;;
	
	// FFMpegの初期化
	av_register_all();
	
	// キャプチャフレーム設定
	ss.x = 0;
	ss.y = 0;
	ss.w = sw;
	ss.h = sh;
	
	// ピクセルフォーマット設定
	pixfmt = bpp == 16 ? PX16RGB :
			 bpp == 24 ? PX24RGB :
						 PX32ARGB;
	
	// フレーム出力リクエスト初期化
	req = 0;
	
	// イメージデータバッファ作成
	Sbuf.resize( ((sw * ( bpp / 8 ) + 3) & ~3) * sh );
	
	// オーディオバッファ作成
	ABuf.InitBuffer( arate / vrate * 2 );
	
	// 出力コンテキスト作成
	avformat_alloc_output_context2( &oc, nullptr, nullptr, P6VPATH2STR( filepath ).c_str() );
	if( !oc ){ return false; }
	
	AVOutputFormat* fmt = oc->oformat;
	
	// 音声、ビデオストリームを作成
	if( fmt->video_codec != AV_CODEC_ID_NONE ){
		// ビデオコーデックにはVP9を選択。
		fmt->video_codec = AV_CODEC_ID_VP9;
		if( !AddStream( video_st, oc, video_codec, fmt->video_codec, sw, sh, vrate ) ){
			return false;
		}
	}
	if( fmt->audio_codec != AV_CODEC_ID_NONE ){
		// FFmpegのOpusは48KHzしか扱えないため、強制的にVORBISにする。
		fmt->audio_codec = AV_CODEC_ID_VORBIS;
		if( !AddStream( audio_st, oc, audio_codec, fmt->audio_codec, sw, sh, arate ) ){
			return false;
		}
	}
	
	if( !OpenVideo( oc, video_codec, video_st, opt, GetPixelFormat( pixfmt ) ) ){
		return false;
	}
	if( !OpenAudio( oc, audio_codec, audio_st, opt, arate ) ){
		return false;
	}
	av_dump_format( oc, 0, P6VPATH2STR( filepath ).c_str(), 1 );
	
	// ファイルを開く
	if( !(fmt->flags & AVFMT_NOFILE) ){
		if( avio_open( &oc->pb, P6VPATH2STR( filepath ).c_str(), AVIO_FLAG_WRITE ) < 0 ){
			return false;
		}
	}
	
	// ストリームヘッダを書き込み
	if( avformat_write_header( oc, &opt ) < 0 ){
		return false;
	}
	
	isAVI = true;
	return true;
#else
	return false;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// ビデオキャプチャ停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void AVI6::StopAVI( void )
{
#ifndef NOAVI
	std::lock_guard<cMutex> lock( Mutex );
	
	if( oc ){
		// ストリームトレイラ書込み
		av_write_trailer( oc );
		
		CloseStream( video_st );
		CloseStream( audio_st );
		
		avio_closep( &oc->pb );
		avformat_free_context( oc );
		
		oc    = nullptr;
		isAVI = false;
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// ビデオキャプチャ中?
//
// 引数:	なし
// 返値:	bool	true:キャプチャ中 false:ひま
/////////////////////////////////////////////////////////////////////////////
bool AVI6::IsAVI( void )
{
	return isAVI;
}


/////////////////////////////////////////////////////////////////////////////
// フレーム出力リクエスト
/////////////////////////////////////////////////////////////////////////////
void AVI6::Request( void )
{
#ifndef NOAVI
	std::lock_guard<cMutex> lock( Mutex );
	
	req++;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// フレーム出力リクエスト数取得
/////////////////////////////////////////////////////////////////////////////
int AVI6::GetRequest( void )
{
#ifndef NOAVI
	std::lock_guard<cMutex> lock( Mutex );
#endif
	
	return isAVI ? req : 0;
}


/////////////////////////////////////////////////////////////////////////////
// AVI1フレーム書出し
//
// 引数:	wh		ウィンドウハンドル
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool AVI6::AVIWriteFrame( HWINDOW wh )
{
#ifndef NOAVI
	std::lock_guard<cMutex> lock( Mutex );
	
	if( !isAVI || !wh || !req ){ return false; }
	
	req--;
	
	if( !OSD_GetWindowImage( wh, Sbuf, &ss, pixfmt ) ) return false;
	
	int encode_video = 1, encode_audio = 1;
	while (encode_video || encode_audio) {
		/* select the stream to encode */
		if (encode_video &&
				(!encode_audio || av_compare_ts( video_st.next_pts, video_st.st->codec->time_base,
												 audio_st.next_pts, audio_st.st->codec->time_base ) <= 0)) {
			WriteVideoFrame( oc, video_st, Sbuf, GetPixelFormat( pixfmt ) );
			encode_video = 0;
		} else {
			encode_audio = !WriteAudioFrame( oc, audio_st, this );
		}
	}
	
	return true;
#else
	return false;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// オーディオバッファ取得
//
// 引数:	なし
// 返値:	cRing *		バッファオブジェクトへのポインタ
/////////////////////////////////////////////////////////////////////////////
cRing *AVI6::GetAudioBuffer( void )
{
	return &ABuf;
}


/////////////////////////////////////////////////////////////////////////////
// 追加更新が必要なサンプル数取得
//
// 引数:	なし
// 返値:	DWORD		サンプル数
/////////////////////////////////////////////////////////////////////////////
DWORD AVI6::GetUpdateSample( void )
{
	std::lock_guard<cMutex> lock( Mutex );
	
	return 0;	// 後で書く
}



