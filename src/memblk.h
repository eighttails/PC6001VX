/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef MEMBLK_H_INCLUDED
#define MEMBLK_H_INCLUDED

#include <string>
#include <vector>

#include "typedef.h"
#include "device.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////

// メモリセルクラス(最小単位)
class MemCell {
public:
	enum {
		PAGEBITS = 13,				// 8KB
		PAGESIZE = 1 << PAGEBITS,
		PAGEMASK = PAGESIZE - 1
	};

protected:
	std::vector<BYTE> Data;			// データ
	bool WPt;						// ライトプロテクトフラグ

public:
	MemCell( BYTE = 0xff, bool = false );
	~MemCell();
	
	void Resize( size_t, BYTE = 0xff );				// リサイズ
	void SetData( std::fstream& );					// ROMデータをファイルから読込み
	size_t Size() const;							// サイズ取得
	
	BYTE Read( WORD ) const;						// メモリリード
	void Write( WORD, BYTE );						// メモリライト
};


// メモリセル集合体クラス(ROM/RAMチップ相当)
class MemCells {
protected:
	std::vector<MemCell> Cells;		// データ

public:
	MemCells( size_t = 1, BYTE = 0xff, bool = false );
	~MemCells();
	
	void Resize( size_t, BYTE = 0xff );				// リサイズ
	bool SetData( const P6VPATH& );					// ROMデータをファイルから読込み
	size_t Size() const;							// サイズ(メモリセル数)取得
	
	BYTE Read( DWORD ) const;						// メモリリード
	void Write( DWORD, BYTE );						// メモリライト
	
	MemCell& operator()( const int );				// メモリセル取得 - operator ()
};


// メモリブロッククラス
class MemBlock {
public:
	enum {
		PAGEBITS = 13,				// 8KB
		PAGESIZE = 1 << PAGEBITS,
		PAGEMASK = PAGESIZE - 1
	};

protected:
	using RFuncPtr = IDevice::RFuncPtr;
	using WFuncPtr = IDevice::WFuncPtr;
	using RFunc    = IDevice::RFunc;
	using WFunc    = IDevice::WFunc;
	using NFuncPtr = IDevice::NFuncPtr;
	using NFunc    = IDevice::NFunc;
	
	std::string Name;				// メモリブロック名
	MemCell* PMem;					// メモリポインタ
	NFunc FName;					// 関数ポインタ(メモリブロック名取得)
	RFunc FRead;					// 関数ポインタ(読込み)
	WFunc FWrite;					// 関数ポインタ(書込み)
	int Wait;						// アクセスウェイト

public:
	MemBlock();
	~MemBlock();
	
	void SetMemory( const std::string&, MemCell&, int = -1 );		// メモリ割当て
	void SetFunc  ( const std::string&, RFunc, WFunc, int = -1 );	// 関数割当て
	void SetFunc  ( NFunc, RFunc, WFunc, int = -1 );				// 関数割当て
	const std::string& GetName( WORD, bool = false ) const;			// メモリブロック名取得
	void SetWait( int );											// アクセスウェイト設定
	int GetWait() const;											// アクセスウェイト取得
	
	BYTE Read( WORD, int* = nullptr ) const;						// メモリリード
	void Write( WORD, BYTE, int* = nullptr ) const;					// メモリライト
};

#endif	// MEMBLK_H_INCLUDED
