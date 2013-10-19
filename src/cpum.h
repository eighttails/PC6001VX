#ifndef CPUM_H_INCLUDED
#define CPUM_H_INCLUDED

#include "device.h"
#include "ini.h"
#include "device/z80.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class CPU6 : public Device, public cZ80, public IDoko {
private:
	BYTE Fetch( WORD, int * );		// フェッチ(M1)
	
	BYTE ReadMemNW( WORD );			// メモリアクセス(ウェイトなし)
	
	BYTE ReadMem( WORD );			// メモリアクセス(ウェイトあり) Read
	void WriteMem( WORD, BYTE );	// メモリアクセス(ウェイトあり) Write
	
	BYTE ReadIO( int );				// I/Oポートアクセス Read
	void WriteIO( int, BYTE );		// I/Oポートアクセス Write
	
	int GetIntrVector();			// 割込みベクタ取得
	
public:
	CPU6( VM6 *, const ID& );		// コンストラクタ
	~CPU6();						// デストラクタ
	
	bool IsBUSREQ();				// BUSREQ取得
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

#endif		// CPUM_H_INCLUDED
