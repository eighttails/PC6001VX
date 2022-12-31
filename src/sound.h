/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <memory>
#include <deque>

#include "semaphore.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
// リングバッファオブジェクト
class cRing {
private:
	std::deque<int32_t> Buffer;	// バッファ
	int Size;					// バッファサイズ(データ数)
	mutable cMutex Mutex;
	
public:
	cRing();
	virtual ~cRing();
	
	bool InitBuffer( int );					// バッファ初期化
	
	virtual int Get();						// 読込み
	virtual void Put( int );				// 書込み
	
	int ReadySize() const;					// 未読データ数取得
	int GetSize() const;					// バッファサイズ取得
};


// サウンドデバイスオブジェクト
class SndDev : public cRing {
protected:
	int SampleRate;				// サンプルレート
	int Volume;					// 音量
	int LPF_Mem;				// ローパスフィルタ用ワーク
	int LPF_fc;					// ローパスフィルタカットオフ周波数
	
	int LPF( int );							// ローパスフィルタ
	
public:
	SndDev();
	virtual ~SndDev();
	
	int Get() override;						// 読込み
	
	void SetLPF( int );						// ローパスフィルタ カットオフ周波数設定
	virtual bool SetSampleRate( int, int );	// サンプリングレート設定
	virtual void SetVolume( int );			// 音量設定
	virtual int SoundUpdate( int );			// ストリーム更新
};


// サウンドオブジェクト
class SND6 : public cRing {
private:
	std::vector<std::shared_ptr<SndDev>> sdev;	// ストリームポインタ配列
	int Volume;					// マスター音量
	int SampleRate;				// サンプリングレート
	int BSize;					// バッファサイズ(倍率)
	CBF_SND CbFunc;				// コールバック関数へのポインタ
	void* CbData;				// コールバック関数に渡すデータ
	
public:
	SND6();
	~SND6();
	
	bool Init( void*, CBF_SND, int, int );	// 初期化
	
	bool ConnectStream( const std::shared_ptr<SndDev>& );	// ストリーム接続
	
	void Play();							// 再生
	void Pause();							// 停止
	
	bool SetSampleRate( int, int = 0 );		// サンプリングレート設定
	int GetSampleRate();					// サンプリングレート取得
	
	int GetBufferSize();					// バッファサイズ(倍率)取得
	int OverflowSamples();					// バッファから溢れたサンプル数取得
	
	void SetVolume( int );					// マスター音量設定
	
	int PreUpdate( int, cRing* = nullptr );	// サウンド事前更新関数
	#ifndef NOCALLBACK	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void Update( BYTE*, int );				// サウンド更新関数(Callback)
	#else				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void Update();							// サウンド更新関数(Push)
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};

#endif	// SOUND_H_INCLUDED
