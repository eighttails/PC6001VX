/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef Z80_H_INCLUDED
#define Z80_H_INCLUDED

#include <string>

#include "typedef.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class cZ80 {
public:
	// ペアレジスタ データ型定義
	typedef union {
	#if BYTEORDER == P6V_BIG_ENDIAN
	 	struct { BYTE h,l; } B;
	#else
		struct { BYTE l,h; } B;
	#endif
				 WORD        W;
	} PAIR;
	
	// レジスタ群 構造体定義
	struct Register {
		PAIR AF, BC, DE, HL;	// 汎用レジスタ
		PAIR IX, IY, PC, SP;	// 専用レジスタ
		PAIR AF1,BC1,DE1,HL1;	//  裏 レジスタ
		BYTE I, R;				// 特殊レジスタ
		BYTE R_saved;			// R reg 保存用
		bool IFF, IFF2;			// IFF,IFF2
		BYTE IM;				// 割込モード
		bool Halt;				// HALT フラグ
		BYTE IntVec;			// 割込みベクタ
		
		Register() : AF({0xff,0xff}), BC({0xff,0xff}), DE({0xff,0xff}), HL({0xff,0xff}),
					 IX({0xff,0xff}), IY({0xff,0xff}), PC({0,0}), SP({0xff,0xff}),
					 AF1({0xff,0xff}), BC1({0xff,0xff}), DE1({0xff,0xff}), HL1({0xff,0xff}),
					 I(0), R(0), R_saved(0), IFF(false), IFF2(false), IM(0), Halt(false), IntVec(0xff) {}
	};
	
protected:
	using offset = int8_t;
	
	PAIR AF, BC, DE, HL;	// 汎用レジスタ
	PAIR IX, IY, PC, SP;	// 専用レジスタ
	PAIR AF1,BC1,DE1,HL1;	//  裏 レジスタ
	BYTE I, R;				// 特殊レジスタ
	BYTE R_saved;			// R reg 保存用
	bool IFF, IFF2;			// IFF,IFF2
	BYTE IM;				// 割込モード
	bool Halt;				// HALT フラグ
	BYTE IntVec;			// 割込みベクタ
	
	int mstate;				// メモリアクセスウェイト ステート数
	
	virtual BYTE Fetch( WORD, int* )    = 0;	// フェッチ(M1)
	virtual BYTE ReadMemNW( WORD )      = 0;	// メモリアクセス(ウェイトなし)
	virtual BYTE ReadMem( WORD )        = 0;	// メモリアクセス(ウェイトあり) Read
	virtual void WriteMem( WORD, BYTE ) = 0;	// メモリアクセス(ウェイトあり) Write
	virtual BYTE ReadIO( int )          = 0;	// I/Oポートアクセス Read
	virtual void WriteIO( int, BYTE )   = 0;	// I/Oポートアクセス Write
	virtual int GetIntrVector()         = 0;	// 割込みチェック＆ベクタ取得
	virtual bool IsBUSREQ()             = 0;	// バスリクエスト区間停止フラグ取得
	
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void PrintfHead( std::string&, WORD, int );	// アドレス/インストラクションコードの表示
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
public:
	cZ80();						// Constructor
	virtual ~cZ80();			// Destructor
	
	void Reset();				// リセット
	int Exec();					// 1命令実行
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int Disasm( std::string&, WORD );	// 1ライン逆アセンブル
	void GetRegister( Register& );		// レジスタ値取得
	void SetRegister( Register& );		// レジスタ値設定
	WORD GetPC();						// PCレジスタ値取得
	BYTE GetIntVec();					// 割込みベクタ取得
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};


#endif		// Z80_H_INCLUDED
