#ifndef P6T2_H_INCLUDED
#define P6T2_H_INCLUDED

#include "typedef.h"
#include "ini.h"


// P6T ブロック情報
struct P6TBLKINFO {
	int STime;				// 無音部の時間(ms)
	int PTime;				// ぴー音の時間(ms)
	int Offset;				// ベタイメージ先頭からのオフセット
	int DNum;				// データサイズ
	
	P6TBLKINFO() : STime(0), PTime(0), Offset(0), DNum(0) {}
};


// DATAブロッククラス
class cP6DATA {
private:
	P6TBLKINFO Info;					// P6T ブロック情報
	BYTE *Data;							// データへのポインタ
	
	cP6DATA *next;						// 次のDATAブロックへのポインタ
	cP6DATA *before;					// 前のDATAブロックへのポインタ
	
	cP6DATA *TopBlock();				// 先頭ブロックへのポインタを返す
	cP6DATA *LastBlock();				// 末尾ブロックへのポインタを返す
	
public:
	cP6DATA();							// コンストラクタ
	~cP6DATA();							// デストラクタ
	
	cP6DATA *Next();					// 次のブロックへのポインタを返す
	cP6DATA *Before();					// 前のブロックへのポインタを返す
	
	cP6DATA *New();						// 新規DATAブロック追加
	cP6DATA *Clone();					// コピー作成
	cP6DATA *Clones();					// 全コピー作成
	
	P6TBLKINFO *GetInfo();				// P6T ブロック情報取得
	
	int SetData( FILE *, int );			// データセット
	void SetPeriod( int, int );			// 無音部,ぴー音時間セット
	
	BYTE Read( int );					// 1Byte読込み
	
	int Writefd( FILE * );				// ファイルに書込み(データ)
};


// P6T PART情報
struct P6TPRTINFO {
	BYTE ID;				// ID番号
	char Name[17];			// データ名(16文字+'00H')
	WORD Baud;				// ボーレート(600/1200)
	
	P6TPRTINFO() : ID(0), Baud(1200)
	{
		INITARRAY( Name, 0 );
	}
};
// PARTクラス
class cP6PART {
private:
	P6TPRTINFO Info;						// P6T PART情報
	cP6DATA *Data;							// 先頭DATAブロックへのポインタ
	
	cP6PART *next;							// 次のPARTへのポインタ
	cP6PART *before;						// 前のPARTへのポインタ
	
	cP6PART *TopPart();						// 先頭PARTへのポインタを返す
	cP6PART *LastPart();					// 末尾PARTへのポインタを返す
	
	BYTE Renumber();						// ID番号リナンバー
	
public:
	cP6PART();								// コンストラクタ
	~cP6PART();								// デストラクタ
	
	cP6PART *Next();						// 次のPARTへのポインタを返す
	cP6PART *Before();						// 前のPARTへのポインタを返す
	cP6PART *Part( int );					// 任意のPARTへのポインタを返す
	
	cP6PART *New();							// 新規PART追加
	cP6PART *Clone();						// コピー作成
	cP6PART *Link( cP6PART * );				// 末尾にPART連結
	
	cP6DATA *NewBlock();					// 新規DATAブロック追加
	
	int GetBlocks();						// 全DATAブロック数を取得
	int GetSize();							// PARTサイズ取得
	
	cP6DATA *FirstData();					// 先頭DATAブロックへのポインタ取得
	
	P6TPRTINFO *GetInfo();					// P6T PART情報取得
	
	int SetName( const char * );			// データ名設定
	void SetBaud( int );					// ボーレート設定
	
	bool Readf( FILE * );					// ファイルから全PARTを読込み
	int Writefd( FILE * );					// ファイルに書込み(データ)
	bool Writeff( FILE * );					// ファイルに書込み(フッタ)
};


// オートスタート情報
struct P6TAUTOINFO {
	bool Start;				// オートスタートフラグ
	BYTE BASIC;				// BASICモード(PC-6001の場合は無意味)
	BYTE Page;				// ページ数
	WORD ASKey;				// オートスタートコマンドサイズ
	char *ask;				// オートスタートコマンド格納領域へのポインタ
	
	P6TAUTOINFO() : Start(false), BASIC(1), Page(1), ASKey(0), ask(NULL) {}
};
// P6Tクラス
class cP6T : public IDoko {
private:
	char Name[17];							// データ名(16文字+'00H')
	BYTE Version;							// バージョン
	
	P6TAUTOINFO Ainfo;						// オートスタート情報
	
	WORD EHead;								// 拡張情報サイズ(64KBまで)
	char *exh;								// 拡張情報格納領域へのポインタ
	
	cP6PART *Part;							// 先頭PARTへのポインタ
	
	// 読込み関係ワーク
	cP6PART *rpart;							// 現在の読込みPART
	cP6DATA *rdata;							// 現在の読込みDATAブロック
	int rpt;								// 現在の読込みポインタ(DATAブロック内)
	int swait;								// 無音部の待ち回数
	int pwait;								// ぴー音の待ち回数
	
	bool ReadP6T( const char * );			// P6Tを読込み
	bool ConvP6T( const char * );			// ベタをP6Tに変換して読込み
	
public:
	cP6T();									// コンストラクタ
	virtual ~cP6T();						// デストラクタ
	
	int GetBlocks() const;					// 全DATAブロック数を取得
	int GetParts() const;					// 全PART数を取得
	
	cP6PART *GetPart( int ) const;			// 任意PARTへのポインタ取得
	
	int SetName( const char * );			// データ名設定
	const char *GetName() const;			//         取得
	
	const P6TAUTOINFO *GetAutoStartInfo() const;	// オートスタート情報取得
	
	int GetSize() const;					// ベタイメージサイズ取得
	
	BYTE ReadOne();							// 1文字読込み
	
	bool IsSWaiting();						// 無音部待ち?
	bool IsPWaiting();						// ぴー音待ち?
	
	bool Readf( const char * );				// ファイルから読込み
	bool Writef( const char * );			// ファイルに書込み
	
	int GetCount() const;					// TAPEカウンタ取得
	void SetCount( int );					//             設定
	
	void Reset();							// リセット(読込み関係ワーク初期化)
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};



#endif	// P6T2_H_INCLUDED
