#ifndef PSGFM_H_INCLUDED
#define PSGFM_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "sound.h"
#include "device/ay8910.h"
#include "device/ym2203.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// PSG系サウンドデバイスのベースクラス
class PSGb : public Device, public SndDev, public virtual IDoko {
protected:
	BYTE JoyNo;							// 読取るジョイスティックの番号(0-1)
	int Clock;							// クロック
	
	int GetUpdateSamples();				// 更新サンプル数取得
	
public:
	PSGb( VM6 *, const ID& );			// コンストラクタ
	virtual ~PSGb();					// デストラクタ
	
	virtual bool Init( int, int ) = 0;	// 初期化
	virtual void Reset(){};				// リセット
	
	virtual void EventCallback( int, int );		// イベントコールバック関数
	virtual bool SetSampleRate( int, int ) = 0;	// サンプリングレート設定
};


class PSG6 : public PSGb, public cAY8910 {
protected:
	void PreWriteReg();					// レジスタ変更前のストリーム更新
	
	// ポートアクセス関数(ジョイスティック限定)
	BYTE PortAread();
	void PortBwrite( BYTE );
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutA0H( int, BYTE );			// PSGレジスタアドレスラッチ
	void OutA1H( int, BYTE );			// PSGライトデータ
	void OutA3H( int, BYTE );			// PSGインアクティブ
	BYTE InA2H( int );					// PSGリードデータ
	
public:
	PSG6( VM6 *, const ID& );			// コンストラクタ
	virtual ~PSG6();					// デストラクタ
	
	bool Init( int, int );				// 初期化
	
	bool SetSampleRate( int, int );		// サンプリングレート設定
	
	int SoundUpdate( int );				// ストリーム更新
	
	// デバイスID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0 };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class OPN6 : public PSGb, public cYM2203 {
protected:
	void PreWriteReg();					// レジスタ変更前のストリーム更新
	
	void SetTimerA( int );				// TimerA設定
	void SetTimerB( int );				// TimerB設定
	
	// ポートアクセス関数(ジョイスティック限定)
	BYTE PortAread();
	void PortBwrite( BYTE );
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	void OutA0H( int, BYTE );			// OPNレジスタアドレスラッチ
	void OutA1H( int, BYTE );			// OPNライトデータ
	void OutA3H( int, BYTE );			// OPNインアクティブ
	BYTE InA2H( int );					// OPNリードデータ
	BYTE InA3H( int );					// OPNステータスリード
	
public:
	OPN6( VM6 *, const ID& );			// コンストラクタ
	~OPN6();							// デストラクタ
	
	bool Init( int, int );				// 初期化
	
	void EventCallback( int, int );		// イベントコールバック関数
	bool SetSampleRate( int, int );		// サンプリングレート設定
	
	int SoundUpdate( int );				// ストリーム更新
	
	// デバイスID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0,  inA3H };
};


#endif	// PSGFM_H_INCLUDED
