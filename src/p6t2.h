#ifndef P6T2_H_INCLUDED
#define P6T2_H_INCLUDED

#include "typedef.h"
#include "ini.h"


// P6T ブロック情報
typedef struct{
	int STime;				// 無音部の時間(ms)
	int PTime;				// ぴー音の時間(ms)
	int Offset;				// ベタイメージ先頭からのオフセット
	int DNum;				// データサイズ
} P6TBLKINFO;


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
	
	cP6DATA *Next()  { return next; }	// 次のブロックへのポインタを返す
	cP6DATA *Before(){ return before; }	// 前のブロックへのポインタを返す
	
	int SetData( FILE *, int );			// データセット
	void SetPeriod( int, int );			// 無音部,ぴー音時間セット
	
	void GetInfo( P6TBLKINFO * );		// P6T ブロック情報取得
	int GetSize(){ return Info.DNum; }	// DATAブロックサイズ取得
	
	BYTE Read( int );					// 1Byte読込み
	
	cP6DATA *New();						// 新規DATAブロック追加
	cP6DATA *Clone();					// コピー作成
	cP6DATA *Clones();					// 全コピー作成
	
	int Writefd( FILE * );				// ファイルに書込み(データ)
};


// PARTクラス
class cP6PART {
private:
	BYTE ID;								// ID番号
	char Name[17];							// データ名(16文字+'00H')
	WORD Baud;								// ボーレート(600/1200)
	cP6DATA *Data;							// 先頭DATAブロックへのポインタ
	
	cP6PART *next;							// 次のPARTへのポインタ
	cP6PART *before;						// 前のPARTへのポインタ
	
	cP6PART *TopPart();						// 先頭PARTへのポインタを返す
	cP6PART *LastPart();					// 末尾PARTへのポインタを返す
	
	BYTE Renumber();						// ID番号リナンバー
	
public:
	cP6PART();								// コンストラクタ
	~cP6PART();								// デストラクタ
	
	cP6PART *Next()  { return next; }		// 次のPARTへのポインタを返す
	cP6PART *Before(){ return before; }		// 前のPARTへのポインタを返す
	cP6PART *Part( int );					// 任意のPARTへのポインタを返す
	
	cP6PART *New();							// 新規PART追加
	cP6PART *Link( cP6PART * );				// 末尾にPART連結
	cP6PART *Clone();						// コピー作成
	
	cP6DATA *FirstData(){ return Data; }	// 先頭DATAブロックへのポインタ取得
	BYTE GetID()   { return ID; }			// ID番号取得
	const char *GetName(){ return Name; }	// データ名取得
	WORD GetBaud() { return Baud; }			// ボーレート取得
	
	cP6DATA *NewBlock();					// 新規DATAブロック追加
	
	int GetBlocks();						// 全DATAブロック数を取得
	int GetSize();							// PARTサイズ取得
	
	int SetName( const char * );			// データ名設定
	void SetBaud( int baud ){ Baud = baud; }	// ボーレート設定
	
	BOOL Readf( FILE * );					// ファイルから全PARTを読込み
	int Writefd( FILE * );					// ファイルに書込み(データ)
	BOOL Writeff( FILE * );					// ファイルに書込み(フッタ)
};


// P6Tクラス
class cP6T : public IDoko {
private:
	char Name[17];							// データ名(16文字+'00H')
	BYTE Version;							// バージョン
	
	BOOL Start;								// オートスタートフラグ
	BYTE BASIC;								// BASICモード(PC-6001の場合は無意味)
	BYTE Page;								// ページ数
	WORD ASKey;								// オートスタートコマンドサイズ
	WORD EHead;								// 拡張情報サイズ(64KBまで)
	
	char *ask;								// オートスタートコマンド格納領域へのポインタ
	char *exh;								// 拡張情報格納領域へのポインタ
	
	cP6PART *Part;							// 先頭PARTへのポインタ
	
	int Boost;								// ボーレート倍率(標準が1)
	
	// 読込み関係ワーク
	cP6PART *rpart;							// 現在の読込みPART
	cP6DATA *rdata;							// 現在の読込みDATAブロック
	int rpt;								// 現在の読込みポインタ(DATAブロック内)
	int swait;								// 無音部の待ち回数
	int pwait;								// ぴー音の待ち回数
	
	BOOL ReadP6T( const char * );			// P6Tを読込み
	BOOL ConvP6T( const char * );			// ベタをP6Tに変換して読込み
	
public:
	cP6T();									// コンストラクタ
	virtual ~cP6T();						// デストラクタ
	
	int GetBlocks();						// 全DATAブロック数を取得
	int GetParts();							// 全PART数を取得
	
	cP6PART *GetPart( int num ){ return Part->Part(num); }	// 任意PARTへのポインタ取得
	
	int SetName( const char * );			// データ名設定
	char *GetName(){ return Name; }			//         取得
	BOOL GetStart(){ return Start; }		// オートスタートフラグ取得
	BYTE GetBASIC(){ return BASIC; }		// BASICモード取得
	BYTE GetPage() { return Page; }			// ページ数取得
	
	int GetSize();							// ベタイメージサイズ取得
	
	char *GetAutoStartStr(){ return ask; }	// オートスタート文字列取得
	WORD GetAutoStartLen(){ return ASKey; }	// オートスタート文字数取得
	
	BYTE ReadOne();							// 1文字読込み
	
	BOOL IsSWaiting();						// 無音部待ち?
	BOOL IsPWaiting();						// ぴー音待ち?
	
	BOOL Readf( char * );					// ファイルから読込み
	BOOL Writef( char * );					// ファイルに書込み
	
	int GetCount();							// TAPEカウンタ取得
	void SetCount( int );					//             設定
	
	void Reset();							// リセット(読込み関係ワーク初期化)
	
	void SetBoost( int );					// ボーレート倍率設定
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};



#endif	// P6T2_H_INCLUDED
