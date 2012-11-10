#ifndef TAPE_H_INCLUDED
#define TAPE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "p6t2.h"
#include "sound.h"
#include "p6device.h"


// データの種類
#define PG_P	(0x0000)
#define PG_S	(0x0100)
#define PG_D	(0x0200)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class CMTL : public P6DEVICE, public Device, public SndDev, public IDoko {
private:
	char FilePath[PATH_MAX];	// TAPEファイルフルパス
	cP6T *p6t;
	bool Relay;					// リレーの状態
	
	bool Boost;					// BoostUp使う? true:使う false:使わない
	int MaxBoost60;				// BoostUp 最大倍率(N60モード)
	int MaxBoost62;				// BoostUp 最大倍率(N60m/N66モード)
	
	bool Remote( bool );			// リモート制御(PLAY,STOP)
	WORD CmtRead();					// CMT 1文字読込み
	int GetSinCurve( int );			// sin波取得
	
	// デバイス定義
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/Oアクセス関数
	void OutB0H( int, BYTE );
	
public:
	CMTL( VM6 *, const ID& );		// コンストラクタ
	virtual ~CMTL();				// デストラクタ
	
	void EventCallback( int, int );	// イベントコールバック関数
	
	bool Init( int );				// 初期化
	void Reset();					// リセット
	
	void SetAutoStart( int );		// オートスタート文字列設定
	
	bool Mount( char * );			// TAPE マウント
	void Unmount();					// TAPE アンマウント
	
	WORD Update();					// ストリーム更新(1byte分)
	int SoundUpdate( int );			// ストリーム更新
	
	bool IsMount();					// マウント済み?
	bool IsAutoStart();				// オートスタート?
	
	char *GetFile();				// ファイルパス取得
	char *GetName();				// TAPE名取得
	DWORD GetSize();				// ベタイメージサイズ取得
	int GetCount();					// カウンタ取得
	bool IsRelay();					// リレーの状態取得
	
	void SetBoost( bool );			// BoostUp設定
	void SetMaxBoost( int, int );	// BoostUp最大倍率設定
	bool IsBoostUp();				// BoostUp状態取得
	
	// デバイスID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );		// どこでもSAVE
	bool DokoLoad( cIni * );		// どこでもLOAD
	// ------------------------------------------
};


class CMTS : public P6DEVICE {
private:
	char FilePath[PATH_MAX];		// TAPEファイルフルパス
	
	FILE *fp;						// FILE ポインタ
	int Baud;						// ボーレート
	
public:
	CMTS( VM6 *, const P6ID& );		// コンストラクタ
	~CMTS();						// デストラクタ
	
	bool Init( char * );			// 初期化
	
	bool Mount();					// TAPE マウント
	void Unmount();					// TAPE アンマウント
	
	void SetBaud( int );			// ボーレート設定
	
	void CmtWrite( BYTE );			// CMT 1文字書込み
};


#endif	// TAPE_H_INCLUDED
