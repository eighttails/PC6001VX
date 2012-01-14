#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "typedef.h"
#include "ini.h"
#include "keydef.h"

#include "p6device.h"


// キーボードインジケータ状態
#define	KI_KANA		(1)
#define	KI_KKANA	(2)
#define	KI_CAPS		(4)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class KEY6 : public P6DEVICE, public IDoko {
protected:
	P6KEYsym K6Table[KVC_LAST];		// 仮想キーコード -> P6キーコード 変換テーブル
	BYTE MatTable[KP6_LAST];		// P6キーコード -> マトリクス 変換テーブル
	
	// 特殊キー フラグ
	BOOL ON_SHIFT;	// SHIFT
	BOOL ON_GRAPH;	// GRAPH
	BOOL ON_KANA;	// かな
	BOOL ON_KKANA;	// カタカナ
	BOOL ON_CTRL;	// CTRL
	BOOL ON_STOP;	// STOP
	BOOL ON_CAPS;	// CAPS
	
	BYTE P6Matrix[16*2];		// キーマトリクス (前半:今回 後半:前回)
	BYTE P6Mtrx[16*2];			// キーマトリクス保存用
								//  いずれも末尾の2byteはジョイスティックの状態保存用

public:
	KEY6( VM6 *, const P6ID& );			// コンストラクタ
	virtual ~KEY6();					// デストラクタ
	
	BOOL Init( int );					// 初期化
	
	void UpdateMatrixKey( int, BOOL );	// キーマトリクス更新(キー)
	void UpdateMatrixJoy();				// キーマトリクス更新(ジョイスティック)
	BOOL ScanMatrix();					// キーマトリクススキャン
	int GetMatrixSize();				// キーマトリクスサイズ取得
	BYTE *GetMatrix();					// キーマトリクスポインタ取得
	BYTE *GetMatrix2();					// キーマトリクスポインタ(保存用)取得
	
	BYTE GetKeyJoy();					// カーソルキー状態取得
	BYTE GetKeyIndicator();				// キーボードインジケータ状態取得
	
	void SetVKeySymbols( VKeyConv * );	// 仮想キーコード -> P6キーコード 設定
	
	BYTE GetJoy( int );					// ジョイスティック状態取得
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


class KEY60 : public KEY6 {
public:
	KEY60( VM6 *, const P6ID& );		// コンストラクタ
	~KEY60();							// デストラクタ
};


#endif	// KEYBOARD_H_INCLUDED
