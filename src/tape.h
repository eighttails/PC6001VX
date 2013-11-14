#ifndef TAPE_H_INCLUDED
#define TAPE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "p6t2.h"
#include "sound.h"


// データの種類
#define PG_P	(0x0000)
#define PG_S	(0x0100)
#define PG_D	(0x0200)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class CMTL : public Device, public SndDev, public IDoko {
private:
	char FilePath[PATH_MAX];			// TAPEファイルフルパス
	cP6T *p6t;
	bool Relay;							// リレーの状態
	bool stron;							// ストリーム内部処理用
	
	bool Boost;							// BoostUp使う? true:使う false:使わない
	int MaxBoost60;						// BoostUp 最大倍率(N60モード)
	int MaxBoost62;						// BoostUp 最大倍率(N60m/N66モード)
	
	bool Remote( bool );				// リモート制御(PLAY,STOP)
	WORD CmtRead();						// CMT 1文字読込み
	int GetSinCurve( int );				// sin波取得
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
public:
	CMTL( VM6 *, const ID& );			// コンストラクタ
	virtual ~CMTL();					// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	bool Init( int );					// 初期化
	void Reset();						// リセット
	
	bool Mount( const char * );			// TAPE マウント
	void Unmount();						// TAPE アンマウント
	
	WORD Update();						// ストリーム更新(1byte分)
	int SoundUpdate( int );				// ストリーム更新
	
	bool IsMount() const;				// マウント済み?
	bool IsAutoStart() const;			// オートスタート?
	
	const char *GetFile() const;		// ファイルパス取得
	const char *GetName() const;		// TAPE名取得
	DWORD GetSize() const;				// ベタイメージサイズ取得
	int GetCount() const;				// カウンタ取得
	bool IsRelay() const;				// リレーの状態取得
	
	void SetBoost( bool );				// BoostUp設定
	void SetMaxBoost( int, int );		// BoostUp最大倍率設定
	bool IsBoostUp() const;				// BoostUp状態取得
	
	const P6TAUTOINFO *GetAutoStartInfo() const;	// オートスタート情報取得
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );		// どこでもSAVE
	bool DokoLoad( cIni * );		// どこでもLOAD
	// ------------------------------------------
};


class CMTS : public Device {
private:
	char FilePath[PATH_MAX];			// TAPEファイルフルパス
	
	FILE *fp;							// FILE ポインタ
	int Baud;							// ボーレート
	
public:
	CMTS( VM6 *, const ID& );			// コンストラクタ
	~CMTS();							// デストラクタ
	
	bool Init( const char * );			// 初期化
	
	bool Mount();						// TAPE マウント
	void Unmount();						// TAPE アンマウント
	
	void SetBaud( int );				// ボーレート設定
	
	void CmtWrite( BYTE );				// CMT 1文字書込み
};


#endif	// TAPE_H_INCLUDED
