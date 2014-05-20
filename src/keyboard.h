#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "keydef.h"



// キーボードインジケータ状態
#define	KI_KANA		(1)
#define	KI_KKANA	(2)
#define	KI_CAPS		(4)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class KEY6 : public Device, public IDoko {
protected:
	P6KEYsym K6Table[KVC_LAST];		// 仮想キーコード -> P6キーコード 変換テーブル
	BYTE MatTable[KP6_LAST];		// P6キーコード -> マトリクス 変換テーブル
	
	// 特殊キー フラグ
	bool ON_SHIFT;	// SHIFT
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// かな
	bool ON_KKANA;	// カタカナ
	bool ON_CTRL;	// CTRL
	bool ON_STOP;	// STOP
	bool ON_CAPS;	// CAPS
	
	BYTE P6Matrix[16*2];		// キーマトリクス (前半:今回 後半:前回)
	BYTE P6Mtrx[16*2];			// キーマトリクス保存用
								//  いずれも末尾の2byteはジョイスティックの状態保存用
public:
	KEY6( VM6 *, const ID& );			// コンストラクタ
	virtual ~KEY6();					// デストラクタ
	
	bool Init( int );					// 初期化
	
	void UpdateMatrixKey( int, bool );	// キーマトリクス更新(キー)
	void UpdateMatrixJoy( BYTE, BYTE );	// キーマトリクス更新(ジョイスティック)
	bool ScanMatrix();					// キーマトリクススキャン
	int GetMatrixSize() const;			// キーマトリクスサイズ取得
	BYTE *GetMatrix();					// キーマトリクスポインタ取得
	const BYTE *GetMatrix2() const;		// キーマトリクスポインタ(保存用)取得
	
	BYTE GetKeyJoy() const;				// カーソルキー状態取得
	BYTE GetKeyIndicator() const;		// キーボードインジケータ状態取得
	
	void SetVKeySymbols( VKeyConv * );	// 仮想キーコード -> P6キーコード 設定
	
	BYTE GetJoy( int ) const;			// ジョイスティック状態取得
	
	void ChangeKana();					// 英字<->かな切換
	void ChangeKKana();					// かな<->カナ切換
	
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class KEY60 : public KEY6 {
public:
	KEY60( VM6 *, const ID& );		// コンストラクタ
	~KEY60();							// デストラクタ
};


#endif	// KEYBOARD_H_INCLUDED
