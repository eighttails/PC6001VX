/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "keydef.h"
#include "romaji.h"
#include "semaphore.h"


// キーボードインジケータ状態
#define	KI_KANA		(0b00000001)
#define	KI_KKANA	(0b00000010)
#define	KI_ROMAJI	(0b00000100)
#define	KI_CAPS		(0b00001000)
#define	KI_SHIFT	(0b00010000)
#define	KI_GRAPH	(0b00100000)
#define	KI_CTRL		(0b01000000)


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class KEY6 : public Device, public IDoko, public Romaji {
protected:
	std::unordered_map<PCKEYsym,P6KEYsym> K6Table;	// 仮想キーコード -> P6キーコード 変換テーブル
	std::unordered_map<P6KEYsym,BYTE> MatTable;		// P6キーコード -> マトリクス 変換テーブル
	
	// 特殊キー フラグ
	bool ON_SHIFT;	// SHIFT
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// かな
	bool ON_KKANA;	// カタカナ
	bool ON_ROMAJI;	// ローマ字入力
	bool ON_CTRL;	// CTRL
	bool ON_STOP;	// STOP
	bool ON_CAPS;	// CAPS
	
	std::vector<BYTE> P6Matrix0;	// キーマトリクス (前半:今回 後半:前回)
	std::vector<BYTE> P6Matrix1;	// キーマトリクス保存用
									//  いずれも末尾の2byteはジョイスティックの状態保存用
	mutable cMutex Mutex;
	
	P6KeyScan iakey;	// キーマトリクス 前回キー保存
	bool ak_KANA;		// かな状態保存
	bool ak_KKANA;		// カタカナ状態保存

public:
	KEY6( VM6*, const ID& );
	virtual ~KEY6();
	
	bool Init();										// 初期化
	void Reset();										// リセット
	
	void UpdateMatrixP6Key( const P6KEYsym, const bool );	// キーマトリクス更新(キー,P6キーコード)
	void UpdateMatrixKey( const PCKEYsym, const bool );	// キーマトリクス更新(キー,仮想キーコード)
	void UpdateMatrixKeyChrRelease();					// キーマトリクス更新(キー,文字コード)自動キー入力用リリース
	void UpdateMatrixKeyChr( const WORD );				// キーマトリクス更新(キー,文字コード)自動キー入力用
	void UpdateMatrixJoy( const BYTE, const BYTE );		// キーマトリクス更新(ジョイスティック)
	void ScanMatrix();									// キーマトリクススキャン
	std::vector<BYTE>& GetMatrix0();					// キーマトリクス取得
	const std::vector<BYTE>& GetMatrix1() const;		// キーマトリクス(保存用)取得
	
	BYTE GetKeyJoy() const;								// カーソルキー状態取得
	BYTE GetKeyIndicator() const;						// キーボードインジケータ状態取得
	
	void SetVKeySymbols( std::vector<VKeyConv>& );		// 仮想キーコード -> P6キーコード 設定
	
	BYTE GetJoy( const int ) const;						// ジョイスティック状態取得
	
	void ChangeKana();									// 英字<->かな切換
	void ChangeKKana();									// かな<->カナ切換
	void ChangeRomaji();								// ローマ字入力切換
	
	void PushMod();										// モディファイア保存
	void PopMod();										// モディファイア復帰
	bool GetLastKeyReleased();							// 前回キーリリース済み？
	
	int RomajiConvert( const PCKEYsym );				// ローマ字かな変換
	const std::string& RomajiGetResult();				// ローマ字かな変換結果取得
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


class KEY60 : public KEY6 {
public:
	KEY60( VM6*, const ID& );
	~KEY60();
};

class KEY62 : public KEY6 {
public:
	KEY62( VM6*, const ID& );
	~KEY62();
};


#endif	// KEYBOARD_H_INCLUDED
