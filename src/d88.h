#ifndef D88_H_INCLUDED
#define D88_H_INCLUDED

#include "typedef.h"


// Disk BIOS Status
#define BIOS_READY					(0x00)
#define BIOS_WRITE_PROTECT			(0x70)
#define BIOS_ID_CRC_ERROR			(0xa0)
#define BIOS_DATA_CRC_ERROR			(0xb0)
#define BIOS_NO_DATA				(0xc0)
#define BIOS_MISSING_IAM			(0xe0)
#define BIOS_MISSING_DAM			(0xf0)
////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cD88 {
private:
	// D88 セクタ情報構造体
	struct D88SECTOR {
		BYTE c;				// ID の C (シリンダNo 片面の場合は=トラックNo)
		BYTE h;				// ID の H (ヘッダアドレス 片面の場合は=0)
		BYTE r;				// ID の R (トラック内のセクタNo)
		BYTE n;				// ID の N (セクタサイズ 0:128 1:256 2:512 3:1024)
		WORD sec_nr;		// このトラック内に存在するセクタの数
		BYTE density;		// 記録密度     0x00:倍密度   0x40:単密度
		BYTE deleted;		// DELETED MARK 0x00:ノーマル 0x10:DELETED
		BYTE status;		// ステータス
		BYTE reserve[5];	// リザーブ
		WORD size;			// このセクタ部のデータサイズ
		DWORD data;			// データへのオフセット
		WORD offset;		// 次に読込むデータのセクタ先頭からのオフセット
		WORD secno;			// アクセス中のセクタNo
		
		D88SECTOR() : c(0), h(0), r(0), n(0), sec_nr(0), density(0), deleted(0),
						status(0), size(0), data(0), offset(0), secno(0)
		{
			INITARRAY( reserve, 0 );
		}
	};
	
	// D88 情報構造体
	struct D88INFO {
		BYTE name[17];		// ディスクの名前(ASCII + '\0')
		BYTE reserve[9];	// リザーブ
		BYTE protect;		// ライトプロテクト  0x00:なし 0x10:あり
		BYTE type;			// ディスクの種類    0x00:2D   0x10:2DD  0x20:2HD
		DWORD size;			// ディスクのサイズ(ヘッダ部+全トラック部)
		DWORD table[164];	// トラック部のオフセットテーブル(Track 0-163)
		D88SECTOR secinfo;	// セクタ情報
		FILE *fp;			// FILE ポインタ
		int trkno;			// アクセス中のトラックNo
		
		D88INFO() : protect(0), type(0), size(0), fp(NULL), trkno(0)
		{
			INITARRAY( name, 0 );
			INITARRAY( reserve, 0 );
			INITARRAY( table, 0 );
		}
	};
	
	D88INFO d88;					// D88 情報
	char FileName[PATH_MAX];		// ファイル名バッファ
	
	bool Protected;					// プロテクトシール
	
	void ReadHeader88();			// D88 ヘッダ読込み
	void ReadSector88();			// D88 セクタ情報読込み
	
public:
	cD88();							// コンストラクタ
	~cD88();						// デストラクタ
	
	bool Init( const char * );		// 初期化
	
	BYTE Get8();					// 1byte 読込み
	bool Put8( BYTE );				// 1byte 書込み
	bool Seek( int, int = -1 );		// シーク
	bool SearchSector( BYTE, BYTE, BYTE, BYTE );	// セクタを探す
	bool NextSector();				// 次のセクタに移動する
	
	void GetID( BYTE *, BYTE *, BYTE *, BYTE * );	// 現在のCHRN取得
	WORD GetSecSize();				// 現在のセクタサイズ取得
	BYTE GetSecStatus();			// 現在のステータス取得
	
	char *GetFileName();			// ファイル名取得
	char *GetDiskImgName();			// DISKイメージ名取得
	bool IsProtect();				// プロテクトシール状態取得
	
	BYTE Track();					// 現在のトラック番号取得
	BYTE Sector();					// 現在のセクタ番号取得
	WORD SecNum();					// 現在のトラック内に存在するセクタ数取得
};

#endif	// D88_H_INCLUDED
