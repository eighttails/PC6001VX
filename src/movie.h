#ifndef MOVIE_H_INCLUDED
#define MOVIE_H_INCLUDED

#include "sound.h"
#include "vsurface.h"

// RECT相当
typedef struct {
	long left;		// LONG
	long top;		// LONG
	long right;		// LONG
	long bottom;	// LONG
} RECT6;

// RGBQUAD相当
typedef struct {
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	reserved;
} RGBPAL6;

// BITMAPINFOHEADER相当
typedef struct {
	DWORD  biSize;
	long   biWidth;			// LONG
	long   biHeight;		// LONG
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	long   biXPelsPerMeter;	// LONG
	long   biYPelsPerMeter;	// LONG
	DWORD  biClrUsed;
	DWORD  biClrImportant;
} BMPINFOHEADER6;

// MainAVIHeader相当
typedef struct {
	DWORD dwMicroSecPerFrame;
	DWORD dwMaxBytesPerSec;
	DWORD dwReserved1;
	DWORD dwFlags;
	DWORD dwTotalFrames;
	DWORD dwInitialFrames;
	DWORD dwStreams;
	DWORD dwSuggestedBufferSize;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwReserved[4];
} MAINAVIHEADER6;

// AVIStreamHeader相当
typedef struct {
	DWORD fccType;		// FOURCC
	DWORD fccHandler;	// FOURCC
	DWORD dwFlags;
	DWORD dwPriority;
	DWORD dwInitialFrames;
	DWORD dwScale;
	DWORD dwRate;
	DWORD dwStart;
	DWORD dwLength;
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;
	RECT6 rcFrame;
} AVISTRMHEADER6;

// AVIINDEXENTRY相当
typedef struct {
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
} AVIINDEXENTRY6;

// WAVEFORMATEX相当
typedef struct {
	WORD  wFormatTag;
	WORD  nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD  wBitsPerSample;
	WORD  cbSize;
} WAVEFORMATEX6;



////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class AVI6 {
protected:
	FILE *vfp;
	MAINAVIHEADER6 vmh;
	AVISTRMHEADER6 vsh, ash;
	BMPINFOHEADER6 vbf;
	RGBPAL6 pal[256];
	WAVEFORMATEX6 awf;
	
	int ABPP;					// bpp (8,16,24)
	bool AVIRLE;				// RLEフラグ  true:RLE  false:ベタ
	
	DWORD PosMOVI;
	
	DWORD RiffSize;
	DWORD MoviSize;
	
	cRing ABuf;					// オーディオバッファ
	DWORD anum;					// オーディオサンプル数カウント用
	
	bool WriteHeader();						// ヘッダチャンク書出し
	bool WriteIndexr();						// インデックスチャンク書出し
	
	void putBMPINFOHEADER6( BMPINFOHEADER6 * );
	void putMAINAVIHEADER6( MAINAVIHEADER6 * );
	void putAVISTRMHEADER6( AVISTRMHEADER6 * );
	void putAVIINDEXENTRY6( AVIINDEXENTRY6 * );
	void putWAVEFORMATEX6( WAVEFORMATEX6 * );
	
public:
	AVI6();									// コンストラクタ
	~AVI6();								// デストラクタ
	
	bool Init();							// 初期化
	
	bool StartAVI( char *, VSurface *, int, int, bool );	// ビデオキャプチャ開始
	
	void StopAVI();							// ビデオキャプチャ停止
	bool IsAVI();							// ビデオキャプチャ中?
	
	bool AVIWriteFrame( VSurface * );		// AVI1フレーム書出し
	
	cRing *GetAudioBuffer();				// オーディオバッファ取得
};


#endif	// MOVIE_H_INCLUDED
