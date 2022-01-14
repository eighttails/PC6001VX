/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <memory>
#include <unordered_map>
#include <vector>

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "keydef.h"


// キーボードインジケータ状態
#define	KI_KANA		(0b00000001)
#define	KI_KKANA	(0b00000010)
#define	KI_CAPS		(0b00000100)
#define	KI_SHIFT	(0b00001000)
#define	KI_GRAPH	(0b00010000)
#define	KI_CTRL		(0b00100000)


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class KEY6 : public Device, public IDoko {
protected:
	std::unordered_map<PCKEYsym,P6KEYsym> K6Table;	// 仮想キーコード -> P6キーコード 変換テーブル
	std::unordered_map<P6KEYsym,BYTE> MatTable;		// P6キーコード -> マトリクス 変換テーブル
	
	// 特殊キー フラグ
	bool ON_SHIFT;	// SHIFT
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// かな
	bool ON_KKANA;	// カタカナ
	bool ON_CTRL;	// CTRL
	bool ON_STOP;	// STOP
	bool ON_CAPS;	// CAPS
	
	std::vector<BYTE> P6Matrix;	// キーマトリクス (前半:今回 後半:前回)
	std::vector<BYTE> P6Mtrx;	// キーマトリクス保存用
								//  いずれも末尾の2byteはジョイスティックの状態保存用

public:
	KEY6( VM6*, const ID& );
	virtual ~KEY6();
	
	bool Init();										// 初期化
	void Reset();										// リセット
	
	void UpdateMatrixKey( const PCKEYsym, const bool );	// キーマトリクス更新(キー)
	void UpdateMatrixJoy( const BYTE, const BYTE );		// キーマトリクス更新(ジョイスティック)
	bool ScanMatrix();									// キーマトリクススキャン
	std::vector<BYTE>& GetMatrix();						// キーマトリクス取得
	const std::vector<BYTE>& GetMatrix2() const;		// キーマトリクス(保存用)取得
	
	BYTE GetKeyJoy() const;								// カーソルキー状態取得
	BYTE GetKeyIndicator() const;						// キーボードインジケータ状態取得
	
	void SetVKeySymbols( std::vector<VKeyConv>& );		// 仮想キーコード -> P6キーコード 設定
	
	BYTE GetJoy( const int ) const;						// ジョイスティック状態取得
	
	void ChangeKana();									// 英字<->かな切換
	void ChangeKKana();									// かな<->カナ切換
	
	
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
