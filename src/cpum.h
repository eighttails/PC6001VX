/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef CPUM_H_INCLUDED
#define CPUM_H_INCLUDED

#include <memory>

#include "device.h"
#include "ini.h"
#include "device/z80.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class CPU6 : public Device, public cZ80, public IDoko {
private:
	BYTE Fetch( WORD, int* ) override;			// フェッチ(M1)
	BYTE ReadMemNW( WORD ) override;			// メモリアクセス(ウェイトなし)
	BYTE ReadMem( WORD ) override;				// メモリアクセス(ウェイトあり) Read
	void WriteMem( WORD, BYTE ) override;		// メモリアクセス(ウェイトあり) Write
	BYTE ReadIO( int ) override;				// I/Oポートアクセス Read
	void WriteIO( int, BYTE ) override;			// I/Oポートアクセス Write
	int GetIntrVector() override;				// 割込みチェック＆ベクタ取得
	bool IsBUSREQ() override;					// バスリクエスト区間停止フラグ取得
	
public:
	CPU6( VM6*, const ID& );
	~CPU6();
	
	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};

#endif		// CPUM_H_INCLUDED
