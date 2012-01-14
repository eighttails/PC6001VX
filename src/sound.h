#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "typedef.h"
#include "semaphore.h"


// ストリームの最大数
// PSG + TAPE + VOICE = 3
#define	MAXSTREAM	(3)
// バッファサイズの倍率
#define	MULTI		(2)

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// リングバッファオブジェクト
//   内部的にはサイズがMULTI倍されるが対外的には等倍のように振舞う
//   読み書きは全領域に対して行なわれるがサイズ取得は等倍
//   つまりオーバーフローを防止しつつレスポンスを保てる...はず
class cRing : public cCritical {
private:
	int *Buffer;			// バッファへのポインタ
	
	int Size;				// バッファサイズ(データ数)
	int Wpt;				// 書込みポインタ
	int Rpt;				// 読込みポインタ
	int Num;				// 未読データ数
	
public:
	cRing();								// コンストラクタ
	virtual ~cRing();						// デストラクタ
	
	BOOL InitBuffer( int );					// バッファ初期化
	
	virtual int Get( int = -1 );			// 読込み
	virtual BOOL Put( int );				// 書込み
	
	int ReadySize();						// 未読データ数取得
	int FreeSize();							// 残りバッファ取得
	int GetSize();							// バッファサイズ取得
};


// サウンドデバイスオブジェクト
class SndDev : public cRing {
protected:
	int SampleRate;			// サンプルレート
	int Volume;				// 音量
	int LPF_Mem;			// ローパスフィルタ用ワーク
	int LPF_k;				// ローパスフィルタ係数
	
	int LPF( int );							// ローパスフィルタ
	
public:
	SndDev();								// コンストラクタ
	virtual ~SndDev();						// デストラクタ
	
	virtual BOOL Init( int );				// 初期化
	
	int Get( int = -1 );					// 読込み
	
	void SetLPF( int );						// ローパスフィルタ カットオフ周波数設定
	virtual void SetVolume( int );			// 音量設定
	virtual int SoundUpdate( int );			// ストリーム更新
};


// サウンドオブジェクト
class SND6 : public cRing {
private:
	SndDev *RB[MAXSTREAM];	// ストリームポインタ配列
	int Volume;				// マスター音量
	int SampleRale;			// サンプリングレート
	
public:
	SND6();									// コンストラクタ
	~SND6();								// デストラクタ
	
	BOOL Init( void *, void (*)(void *, BYTE *, int ), int, int );	// 初期化
	
	BOOL ConnectStream( SndDev * );			// ストリーム接続
	
	void Play();							// 再生
	void Pause();							// 停止
	
	int GetSampleRate();					// サンプリングレート取得
	void SetVolume( int );					// マスター音量設定
	
	int PreUpdate( int, cRing * = NULL );	// サウンド事前更新関数
	void Update( BYTE *, int );				// サウンド更新関数
};

#endif	// SOUND_H_INCLUDED
