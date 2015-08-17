#ifndef MOVIE_H_INCLUDED
#define MOVIE_H_INCLUDED

#ifndef NOAVI

#include "sound.h"
#include "vsurface.h"

struct AVOutputFormat;
struct AVFormatContext;
struct AVCodec;
struct AVStream;
struct AVFrame;
struct AVDictionary;

// FFMpegのサンプルmuxing.cより抜粋
// a wrapper around a single output AVStream
typedef struct OutputStream {
	AVStream *st;

	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;

	AVFrame *frame;
	AVFrame *tmp_frame;

	//OCfloat t, tincr, tincr2;

	struct SwsContext *sws_ctx;
	struct SwrContext *swr_ctx;
} OutputStream;

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class AVI6 {
protected:


	int ABPP;					// 色深度 (16,24,32)
	std::vector<BYTE> Sbuf;		// イメージデータバッファ
//	BYTE *Sbuf;					// イメージデータバッファポインタ
	
//	DWORD PosMOVI;
	
//	DWORD RiffSize;
//	DWORD MoviSize;
	AVOutputFormat *fmt;
	AVFormatContext *oc;
	AVCodec *audio_codec;
	AVCodec *video_codec;
	AVDictionary *opt;
	OutputStream video_st;
	OutputStream audio_st;

	cRing ABuf;					// オーディオバッファ
	//DWORD anum;					// オーディオサンプル数カウント用

public:
	AVI6();									// コンストラクタ
	~AVI6();								// デストラクタ
	
	bool Init();							// 初期化
	
	bool StartAVI( const char *, int, int, int, int, int );	// ビデオキャプチャ開始
	
	void StopAVI();							// ビデオキャプチャ停止
	bool IsAVI();							// ビデオキャプチャ中?
	
	bool AVIWriteFrame( HWINDOW );			// AVI1フレーム書出し
	
	cRing *GetAudioBuffer();				// オーディオバッファ取得
};

#endif	// NOAVI
#endif	// MOVIE_H_INCLUDED
