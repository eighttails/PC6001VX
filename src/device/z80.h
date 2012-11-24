#ifndef Z80_H_INCLUDED
#define Z80_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cZ80 {
public:
	// ペアレジスタ データ型定義
	typedef union {
	#if BYTEORDER == BIG_ENDIAN
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
		BYTE IFF,IFF2;			// IFF,IFF2
		BYTE IM;				// 割込モード
		BYTE Halt;				// HALT フラグ
	};
	
protected:
	typedef int8_t	offset;
	
	PAIR AF, BC, DE, HL;	// 汎用レジスタ
	PAIR IX, IY, PC, SP;	// 専用レジスタ
	PAIR AF1,BC1,DE1,HL1;	//  裏 レジスタ
	BYTE I, R;				// 特殊レジスタ
	BYTE R_saved;			// R reg 保存用
	BYTE IFF,IFF2;			// IFF,IFF2
	BYTE IM;				// 割込モード
	BYTE Halt;				// HALT フラグ
	
	int mstate;				// メモリアクセスウェイト ステート数
	
	// フェッチ(M1)
	virtual BYTE Fetch( WORD, int * )   = 0;
	
	// メモリアクセス(ウェイトなし)
	virtual BYTE ReadMemNW( WORD )      = 0;
	
	// メモリアクセス(ウェイトあり)
	virtual BYTE ReadMem( WORD )        = 0;
	virtual void WriteMem( WORD, BYTE ) = 0;
	
	// I/Oポートアクセス
	virtual BYTE ReadIO( int )          = 0;
	virtual void WriteIO( int, BYTE )   = 0;
	
	// 割込みベクタ取得
	virtual int GetIntrVector()         = 0;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	void PrintfHead( char *, WORD, int );	// アドレス/インストラクションコードの表示
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
public:
	cZ80();						// コンストラクタ
	virtual ~cZ80();			// デストラクタ
	
	void Reset();				// リセット
	int Exec();					// 1命令実行
	
	virtual bool IsBUSREQ();	// BUSREQ取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	// デバッグ用
	int Disasm( char *, WORD );		// 1ライン逆アセンブル
	void GetRegister( Register * );	// レジスタ値取得
	void SetRegister( Register * );	// レジスタ値設定
	WORD GetPC();					// PCレジスタ値取得
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
};


#endif		// Z80_H_INCLUDED
