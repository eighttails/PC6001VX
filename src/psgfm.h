/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef PSGFM_H_INCLUDED
#define PSGFM_H_INCLUDED

#include <memory>

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "semaphore.h"
#include "sound.h"
#include "device/ay8910.h"
#include "device/ym2203.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
// PSG系サウンドデバイスのベースクラス
class PSGb : public Device, public SndDev, public IDoko {
protected:
	BYTE JoyNo;		// 読取るジョイスティックの番号(0-1)
	int Clock;		// クロック
	int Smpls;		// 生成サンプル数カウンタ
	mutable cMutex Mutex;
	
	int GetUpdateSamples();						// 更新サンプル数取得
	
public:
	PSGb( VM6*, const ID& );
	virtual ~PSGb();
	
	virtual bool Init( int, int ) = 0;			// 初期化
	virtual void Reset(){};						// リセット
	
	// デバイスID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0,  inA3H };
};


class PSG60 : public PSGb, public cAY8910 {
protected:
	void PreWriteReg() override;				// レジスタ変更前のストリーム更新
	
	// ポートアクセス関数(ジョイスティック限定)
	BYTE PortAread() override;
	void PortBwrite( BYTE ) override;
	
	// I/Oアクセス関数
	void OutA0H( int, BYTE );					// PSGレジスタアドレスラッチ
	void OutA1H( int, BYTE );					// PSGライトデータ
	void OutA3H( int, BYTE );					// PSGインアクティブ
	BYTE InA2H( int );							// PSGリードデータ
	
public:
	PSG60( VM6*, const ID& );
	virtual ~PSG60();
	
	bool Init( int, int ) override;				// 初期化
	
	void EventCallback( int, int ) override;	// イベントコールバック関数
	bool SetSampleRate( int, int ) override;	// サンプリングレート設定
	int SoundUpdate( int ) override;			// ストリーム更新
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class OPN64 : public PSGb, public cYM2203 {
protected:
	void PreWriteReg() override;				// レジスタ変更前のストリーム更新
	
	void SetTimerA( int ) override;				// TimerA設定
	void SetTimerB( int ) override;				// TimerB設定
	
	// ポートアクセス関数(ジョイスティック限定)
	BYTE PortAread() override;
	void PortBwrite( BYTE ) override;
	
	void OutA0H( int, BYTE );					// OPNレジスタアドレスラッチ
	void OutA1H( int, BYTE );					// OPNライトデータ
	void OutA3H( int, BYTE );					// OPNインアクティブ
	BYTE InA2H( int );							// OPNリードデータ
	BYTE InA3H( int );							// OPNステータスリード
	
public:
	OPN64( VM6*, const ID& );
	~OPN64();
	
	bool Init( int, int ) override;				// 初期化
	
	void EventCallback( int, int ) override;	// イベントコールバック関数
	bool SetSampleRate( int, int ) override;	// サンプリングレート設定
	int SoundUpdate( int ) override;			// ストリーム更新
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


#endif	// PSGFM_H_INCLUDED
