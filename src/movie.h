/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef MOVIE_H_INCLUDED
#define MOVIE_H_INCLUDED

#include <vector>

#include "osd.h"
#include "sound.h"
#include "vsurface.h"

struct AVFormatContext;
struct AVCodec;
struct AVStream;
struct AVFrame;

// FFMpegのサンプルmuxing.cより抜粋
// a wrapper around a single output AVStream
typedef struct OutputStream {
	AVStream* st;
	
	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;
	
	AVFrame* frame;
	AVFrame* tmp_frame;
	
	struct SwsContext* sws_ctx;
	struct SwrContext* swr_ctx;
} OutputStream;


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class AVI6 : public cSemaphore {
protected:
	bool isAVI;
	
	AVFormatContext* oc;
	AVCodec* audio_codec;
	AVCodec* video_codec;
	OutputStream video_st;
	OutputStream audio_st;
	
	std::vector<BYTE> Sbuf;		// イメージデータバッファ
	cRing ABuf;					// オーディオバッファ
	
	VRect ss;					// キャプチャフレーム設定
	PixelFMT pixfmt;			// ピクセルフォーマット
	int req;					// フレーム出力リクエスト数
	
	mutable cMutex Mutex;

public:
	AVI6();
	~AVI6();
	
	bool StartAVI( const P6VPATH&, int, int, double, int, int );	// ビデオキャプチャ開始
	void StopAVI();							// ビデオキャプチャ停止
	
	bool IsAVI();							// ビデオキャプチャ中?
	void Request();							// フレーム出力リクエスト
	int GetRequest();						// フレーム出力リクエスト数取得
	
	bool AVIWriteFrame( HWINDOW );			// AVI1フレーム書出し
	
	cRing *GetAudioBuffer();				// オーディオバッファ取得
	DWORD GetUpdateSample();				// 追加更新が必要なサンプル数取得
};

#endif	// MOVIE_H_INCLUDED
