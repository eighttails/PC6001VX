/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef P6T2_H_INCLUDED
#define P6T2_H_INCLUDED

#include <fstream>
#include <list>
#include <string>
#include <vector>

#include "typedef.h"


// P6T ブロック情報
struct P6TBLKINFO {
	BYTE ID;				// ID番号
	char Name[16];			// データ名(15文字+'00H')
	WORD Baud;				// ボーレート(600/1200)
	int StopBit;			// ストップビット
	int STime;				// 無音部の時間(ms)
	int PTime;				// ぴー音の時間(ms)
	DWORD Offset;			// ベタイメージ先頭からのオフセット
	
	P6TBLKINFO() : ID(0), Baud(1200), StopBit(0), STime(0), PTime(0), Offset(0)
	{
		INITARRAY( Name, 0 );
	}
};

// DATAブロッククラス
class cP6DATA {
private:
	P6TBLKINFO Info;						// P6T ブロック情報
	std::vector<BYTE> Data;					// データ
	std::vector<BYTE>::const_iterator rp;	// データリード用イテレータ
	
public:
	cP6DATA();
	~cP6DATA();
	
	void Rewind();									// 巻戻し
	
	const P6TBLKINFO& GetInfo();					// P6T ブロック情報取得
	
	int SetName( const std::string& );				// データ名設定
	void SetBaud( int );							// ボーレート設定
	void SetStopBit( int );							// ストップビット設定
	void SetPeriod( int, int );						// 無音部,ぴー音時間セット
	
	DWORD GetSize() const;							// データサイズ取得
	void SetSize( DWORD );							//             設定
	DWORD GetCount();								// 読込みカウンタ取得
	void SetCount( DWORD );							//               設定
	bool IsEnd() const;								// データ末尾?
	
	BYTE Read();									// 1Byte読込み
	
	bool Readfd( std::fstream& );					// ファイルから読込み(データ)
	bool Readff( std::fstream& );					// ファイルから読込み(フッタ)
	bool Writefd( std::fstream& );					// ファイルに書込み(データ)
	bool Writeff( std::fstream& );					// ファイルに書込み(フッタ)
	
};


// オートスタート情報
struct P6TAUTOINFO {
	bool Start;				// オートスタートフラグ
	BYTE BASIC;				// BASICモード(PC-6001の場合は無意味)
	BYTE Page;				// ページ数
	std::vector<char> ask;	// オートスタートコマンド
	
	P6TAUTOINFO() : Start(false), BASIC(1), Page(1) {}
};

// P6Tクラス
class cP6T {
private:
	std::string Name;						// データ名
	BYTE Version;							// バージョン
	P6TAUTOINFO Ainfo;						// オートスタート情報
	std::vector<BYTE> exh;					// 拡張情報
	std::list<cP6DATA> Data;				// DATAブロックリスト
	std::list<cP6DATA>::iterator rd;		// DATAブロック用イテレータ
	
	bool ReadP6T( const P6VPATH& );					// P6Tを読込み
	bool ConvP6T( const P6VPATH& );					// ベタをP6Tに変換して読込み
	
protected:
	int swait;								// 無音部の待ちbit数
	int pwait;								// ぴー音の待ちbit数
	
public:
	cP6T();
	virtual ~cP6T();
	
	void Clear();									// 初期化
	void Rewind();									// 巻戻し
	
	int SetName( const std::string& );				// データ名設定
	const std::string& GetName() const;				//         取得
	
	bool IsSWaiting( int );							// 無音部待ち?
	bool IsPWaiting( int );							// ぴー音待ち?
	bool IsAutoStart() const;						// オートスタート?
	const P6TAUTOINFO& GetAutoStartInfo() const;	// オートスタート情報取得
	DWORD GetBetaSize();							// ベタイメージサイズ取得
	
	int GetCount() const;							// TAPEカウンタ取得
	void SetCount( DWORD );							//             設定
	
	BYTE ReadOne();									// 1文字読込み
	
	bool Readf( const P6VPATH& );					// ファイルから読込み
	bool Writef( const P6VPATH& );					// ファイルに書込み
};

#endif	// P6T2_H_INCLUDED
