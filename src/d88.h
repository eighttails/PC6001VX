#ifndef D88_H_INCLUDED
#define D88_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cD88 {
private:
	// D88 セクタ情報構造体
	typedef struct{
		BYTE c;				// ID の C (シリンダNo 片面の場合は=トラックNo)
		BYTE h;				// ID の H (ヘッダアドレス 片面の場合は=0)
		BYTE r;				// ID の R (トラック内のセクタNo)
		BYTE n;				// ID の N (セクタサイズ 0:256 1:256 2:512 3:1024)
		WORD sec_nr;		// このトラック内に存在するセクタの数
		BYTE density;		// 記録密度     0x00:倍密度   0x40:単密度
		BYTE deleted;		// DELETED MARK 0x00:ノーマル 0x10:DELETED
		BYTE status;		// ステータス
		BYTE reserve[5];	// リザーブ
		WORD size;			// このセクタ部のデータサイズ
		DWORD data;			// データへのオフセット
		WORD offset;		// 次に読込むデータのセクタ先頭からのオフセット
	} D88SECTOR;
	
	// D88 情報構造体
	typedef struct{
		BYTE name[17];		// ディスクの名前(ASCII + '\0')
		BYTE reserve[9];	// リザーブ
		BYTE protect;		// ライトプロテクト  0x00:なし 0x10:あり
		BYTE type;			// ディスクの種類    0x00:2D   0x10:2DD  0x20:2HD
		DWORD size;			// ディスクのサイズ(ヘッダ部+全トラック部)
		DWORD table[164];	// トラック部のオフセットテーブル(Track 0-163)
		D88SECTOR secinfo;	// セクタ情報
		FILE *fp;			// FILE ポインタ
	} D88INFO;
	
	D88INFO d88;				// D88 情報
	char FileName[PATH_MAX];	// ファイル名バッファ
	
	BOOL Protected;				// プロテクトシール
	
	void ReadHeader88();		// D88 ヘッダ読込み
	void ReadSector88();		// D88 セクタ情報読込み
	
public:
	cD88();						// コンストラクタ
	~cD88();					// デストラクタ
	
	BOOL Init( char * );		// 初期化
	
	BYTE Getc88();				// 1byte 読込み
	BOOL Putc88( BYTE );		// 1byte 書込み
	BOOL Seek88( int, int );	// シーク
	DWORD GetCHRN();			// 現在のCHRN取得
	
	char *GetFileName();		// ファイル名取得
	char *GetDiskImgName();		// DISKイメージ名取得
	BOOL IsProtect();			// プロテクトシール状態取得
};

#endif	// D88_H_INCLUDED
