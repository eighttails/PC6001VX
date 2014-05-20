#include <string.h>

#include "log.h"
#include "d88.h"
#include "common.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cD88::cD88( bool dd ) : DDDrv(dd), Protected(false)
{
	PRINTD( D88_LOG, "[D88][cD88] %s Drive\n", DDDrv ? "1DD" : "1D" )
	
	INITARRAY( FileName, '\0' );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cD88::~cD88( void )
{
	if( d88.fp ) fclose( d88.fp );
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cD88::Init( const char *fname )
{
	PRINTD( D88_LOG, "[D88][Init] %s\n", fname )
	
	strncpy( FileName, fname, PATH_MAX );
	
	// 読取り専用属性ならプロテクト状態で開く
	if( OSD_FileReadOnly( FileName ) ){
		d88.fp = FOPENEN( FileName, "rb" );
		Protected = true;	// プロテクトシールあり
	}else{
		d88.fp = FOPENEN( FileName, "rb+" );
		Protected = false;	// プロテクトシールなし
	}
	
	if( !d88.fp ){
		*FileName = 0;
		Protected = false;
		return false;
	}
	
	ReadHeader88();	// D88 ヘッダ読込み
	
	return true;
}


////////////////////////////////////////////////////////////////
// D88 ヘッダ読込み
////////////////////////////////////////////////////////////////
void cD88::ReadHeader88( void )
{
	PRINTD( D88_LOG, "[D88][ReadHeader88]\n" )
	
	if( d88.fp ){
		// DISK名
		fread( d88.name, sizeof(BYTE), 17, d88.fp );
		d88.name[16] = '\0';
		
		// リザーブ空読み
		fread( d88.reserve, sizeof(BYTE), 9, d88.fp );
		
		// ライトプロテクト
		d88.protect = fgetc( d88.fp );
		if( d88.protect ) Protected = true;
		else if( Protected ) d88.protect = 0x10;
		
		// DISKの種類
		d88.type = fgetc( d88.fp );
		
		// DISKのサイズ
		d88.size = FGETDWORD( d88.fp );
		
		// トラック部のオフセットテーブル
		ZeroMemory( &d88.table, sizeof(d88.table) );
		// 1DDドライブで1Dディスクを使う時は2トラック飛びで読込む
		if( DDDrv && !(GetType()&FD_DOUBLETRACK) ){
			PRINTD( D88_LOG, " (1D disk on 1DD drive)\n" )
			for( int i=0; i<164; i+=2 )
				d88.table[i] = FGETDWORD( d88.fp );
		}else
			for( int i=0; i<164; i++ )
				d88.table[i] = FGETDWORD( d88.fp );
			
		// アクセス中のトラックNo
		d88.trkno = 0;
		
		PRINTD( D88_LOG, " FileName : %s\n", d88.name )
		PRINTD( D88_LOG, " Protect  : %s\n", d88.protect ? "ON" : "OFF" )
		PRINTD( D88_LOG, " Media    : %02X\n", d88.type )
		PRINTD( D88_LOG, " Size     : %d\n", (int)d88.size )
	}
}


////////////////////////////////////////////////////////////////
// D88 セクタ情報読込み
////////////////////////////////////////////////////////////////
void cD88::ReadSector88( void )
{
	PRINTD( D88_LOG, "[D88][ReadSector88]\n" );
	
	if( d88.fp && d88.table[d88.trkno] ){
		d88.secinfo.c       = fgetc( d88.fp );		// ID の C (シリンダNo 片面の場合は=トラックNo)
		d88.secinfo.h       = fgetc( d88.fp );		// ID の H (ヘッダアドレス 片面の場合は=0)
		d88.secinfo.r       = fgetc( d88.fp );		// ID の R (トラック内のセクタNo)
		d88.secinfo.n       = fgetc( d88.fp );		// ID の N (セクタサイズ 0:256 1:256 2:512 3:1024)
		d88.secinfo.sec_nr  = FGETWORD( d88.fp );	// このトラック内に存在するセクタの数
		d88.secinfo.density = fgetc( d88.fp );		// 記録密度     0x00:倍密度   0x40:単密度
		d88.secinfo.deleted = fgetc( d88.fp );		// DELETED MARK 0x00:ノーマル 0x10:DELETED
		d88.secinfo.status  = fgetc( d88.fp );		// ステータス
		fread( d88.secinfo.reserve, sizeof(BYTE), 5, d88.fp );	// リザーブ空読み
		d88.secinfo.size    = FGETWORD( d88.fp );	// このセクタ部のデータサイズ
		d88.secinfo.data    = ftell( d88.fp );		// データへのオフセット
		d88.secinfo.offset  = 0;					// 次に読込むデータのセクタ先頭からのオフセット
		d88.secinfo.secno++;						// アクセス中のセクタNo
		
		// Dittのバグ対応
		// 吸出し時，データサイズ=0の時に00Hが256バイト格納されるバグがあるらしい
		// データサイズが0だったら続く256バイトを調べ，全て00Hだったら読み飛ばす
		// 00H以外のデータが現れたら次のセクタのデータと判断し，読み飛ばさない。
		// 256バイト先がトラックorディスクの末尾に到達する場合も読み飛ばさない
		if( (d88.secinfo.size == 0) &&
			(((d88.trkno < 163) && (d88.table[d88.trkno+1] >= d88.secinfo.data+256)) ||
			 (d88.size >= d88.secinfo.data+256)	) ){
			
			for( int i=0; i<256; i++ ){
				if( fgetc( d88.fp ) != 0 ){
					fseek( d88.fp, d88.secinfo.data, SEEK_SET );
					break;
				}
			}
		}
	}else{
		ZeroMemory( &d88.secinfo, sizeof(D88SECTOR) );
	}
	
	PRINTD( D88_LOG, " C      : %d\n", d88.secinfo.c )
	PRINTD( D88_LOG, " H      : %d\n", d88.secinfo.h )
	PRINTD( D88_LOG, " R      : %d\n", d88.secinfo.r )
	PRINTD( D88_LOG, " N      : %d\n", d88.secinfo.n )
	PRINTD( D88_LOG, " SectNum: %d/%d\n", d88.secinfo.secno, d88.secinfo.sec_nr )
	PRINTD( D88_LOG, " Density: %s\n", d88.secinfo.density&0x40 ? "S" : "D" )
	PRINTD( D88_LOG, " Del    : %s\n", d88.secinfo.deleted&0x10 ? "DELETED" : "NORMAL" )
	PRINTD( D88_LOG, " Stat   : %02X\n", d88.secinfo.status )
	PRINTD( D88_LOG, " Size   : %d\n", d88.secinfo.size )
	PRINTD( D88_LOG, " Offset : %d\n", (int)d88.secinfo.data )
}


////////////////////////////////////////////////////////////////
// 1byte 読込み
////////////////////////////////////////////////////////////////
BYTE cD88::Get8( void )
{
	BYTE dat;
	
	PRINTD( D88_LOG, "[D88][Get8] -> " )
	
	if( d88.fp && d88.table[d88.trkno] ){
		// セクタの終わりに到達したら次のセクタをシークする
		// 最終セクタの次は同一トラックの先頭セクタに移動
		// エラーセクタの場合は次のセクタに移動しない(Ditt!のエラー対応)
		if( d88.secinfo.offset >= d88.secinfo.size && !d88.secinfo.status ){
			if( d88.secinfo.secno > d88.secinfo.sec_nr ) Seek( d88.trkno );
			else										 ReadSector88();
		}
		dat = fgetc( d88.fp );
		d88.secinfo.offset++;
		
		PRINTD( D88_LOG, "%02X\n", dat );
		
		return dat;
	}
	PRINTD( D88_LOG, "false(0xff)\n" );
	
	return 0xff;
}


////////////////////////////////////////////////////////////////
// 1byte 書込み
////////////////////////////////////////////////////////////////
bool cD88::Put8( BYTE dat )
{
	PRINTD( D88_LOG, "[D88][Put8] -> %02X(%02d:%02d:%02d:%02d)", dat, d88.secinfo.c, d88.secinfo.h, d88.secinfo.r, d88.secinfo.n );
	
	if( d88.fp && d88.table[d88.trkno] && !d88.protect ){
		// セクタの終わりに到達したら次のセクタをシークする
		// 最終セクタの次は同一トラックの先頭セクタに移動
		if( d88.secinfo.offset >= d88.secinfo.size ){
			if( d88.secinfo.secno > d88.secinfo.sec_nr ) Seek( d88.trkno );
			else										 ReadSector88();
		}
		
		// r+,w+,a+ で開いたファイルに対して読込みと書込みを切り替える場合は
		// 必ず fsetpos,fseek,rewind のいずれかの関数を実行する必要があるらしい
		fseek( d88.fp, 0, SEEK_CUR );
		fputc( dat, d88.fp );
		fseek( d88.fp, 0, SEEK_CUR );
		
		d88.secinfo.offset++;
		
		PRINTD( D88_LOG, " ->OK\n" );
		
		return true;
	}
	PRINTD( D88_LOG, " ->NG\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// シーク
////////////////////////////////////////////////////////////////
bool cD88::Seek( int trackno, int sectno )
{
	PRINTD( D88_LOG, "[D88][Seek] Track : %d Sector : %d ", trackno, sectno );
	
	if( d88.fp ){
		ZeroMemory( &d88.secinfo, sizeof(D88SECTOR) );
		d88.trkno          = trackno;
		d88.secinfo.status = BIOS_MISSING_IAM;
		
		// トラックが無効ならUnformat扱い
		if( !d88.table[d88.trkno] ){
			PRINTD( D88_LOG, "-> Unformat\n" );
			return false;
		}
		PRINTD( D88_LOG, "-> Track:%d\n", d88.trkno );
		
		// トラックの先頭をシーク
		fseek( d88.fp, d88.table[d88.trkno], SEEK_SET );
		
		// 最初のセクタ情報読込み
		ReadSector88();
		
		// 目的のセクタを頭出し
		if( sectno > 1 ){
			fseek( d88.fp, (long)d88.secinfo.size, SEEK_CUR );
			ReadSector88();
		}
		
		PRINTD( D88_LOG, "-> OK\n" );
		d88.secinfo.status = BIOS_READY;
		
		return true;
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// セクタを探す
////////////////////////////////////////////////////////////////
bool cD88::SearchSector( BYTE c, BYTE h, BYTE r, BYTE n )
{
	PRINTD( D88_LOG, "[D88][SearchSector] C:%02X H:%02X R:%02X N:%02X ", c, h, r, n );
	
	if( Seek( d88.trkno ) ){
		// 目的のセクタが現れるまで空読み
		while( d88.secinfo.secno <= d88.secinfo.sec_nr ){
			// IDをチェック
			if( ( d88.secinfo.c == c ) && ( d88.secinfo.h == h )
			 && ( d88.secinfo.r == r ) && ( d88.secinfo.n == n ) ){
				PRINTD( D88_LOG, "-> Found\n" );
				return true;
			}
			// 一致しなければ次のセクタ情報読込み
			fseek( d88.fp, (long)d88.secinfo.size, SEEK_CUR );
			ReadSector88();
		}
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// 次のセクタに移動する
////////////////////////////////////////////////////////////////
bool cD88::NextSector( void )
{
	PRINTD( D88_LOG, "[D88][NextSector] Sector:%d ", d88.secinfo.secno );
	
	if( d88.secinfo.sec_nr ){
		int ssize = d88.secinfo.size - d88.secinfo.offset;	// 現在のセクタ終端までのデータ数
		
		if( d88.secinfo.secno == d88.secinfo.sec_nr )
			// 最終セクタの次は同一トラックの先頭セクタに移動
			Seek( d88.trkno );
		else{
			// 次のセクタ先頭まで移動してセクタ情報を読込み
			fseek( d88.fp, (long)ssize, SEEK_CUR );
			ReadSector88();
		}
		PRINTD( D88_LOG, "-> %d\n", d88.secinfo.secno );
		
		return true;
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// 現在のCHRN取得
////////////////////////////////////////////////////////////////
void cD88::GetID( BYTE *C, BYTE *H, BYTE *R, BYTE *N ) const
{
	PRINTD( D88_LOG, "[D88][GetID] %02X %02X %02X %02X\n", d88.secinfo.c, d88.secinfo.h, d88.secinfo.r, d88.secinfo.n );
	
	if( C ) *C = d88.secinfo.c;
	if( H ) *H = d88.secinfo.h;
	if( R ) *R = d88.secinfo.r;
	if( N ) *N = d88.secinfo.n;
}


////////////////////////////////////////////////////////////////
// 現在のセクタサイズ取得
////////////////////////////////////////////////////////////////
WORD cD88::GetSecSize( void ) const
{
	PRINTD( D88_LOG, "[D88][GetSecSize] %d\n", d88.secinfo.size );
	
	return d88.secinfo.size;
}


////////////////////////////////////////////////////////////////
// 現在のトラック番号取得
////////////////////////////////////////////////////////////////
BYTE cD88::Track( void ) const
{
	PRINTD( D88_LOG, "[D88][Track] %d\n", d88.trkno );
	
	return d88.trkno;
}


////////////////////////////////////////////////////////////////
// 現在のセクタ番号取得
////////////////////////////////////////////////////////////////
BYTE cD88::Sector( void ) const
{
	PRINTD( D88_LOG, "[D88][Sector] %d\n", d88.secinfo.secno );
	
	return d88.secinfo.secno;
}


////////////////////////////////////////////////////////////////
// 現在のトラック内に存在するセクタ数取得
////////////////////////////////////////////////////////////////
WORD cD88::SecNum( void ) const
{
	PRINTD( D88_LOG, "[D88][SecNum] %d\n", d88.secinfo.sec_nr );
	
	return d88.secinfo.sec_nr;
}


////////////////////////////////////////////////////////////////
// 現在のステータス取得
////////////////////////////////////////////////////////////////
BYTE cD88::GetSecStatus( void ) const
{
	PRINTD( D88_LOG, "[D88][GetStatus] %02X\n", d88.secinfo.status );
	
	return d88.secinfo.status;
}


////////////////////////////////////////////////////////////////
// ファイル名取得
////////////////////////////////////////////////////////////////
const char *cD88::GetFileName( void ) const
{
	return FileName;
}


////////////////////////////////////////////////////////////////
// DISKイメージ名取得
////////////////////////////////////////////////////////////////
const char *cD88::GetDiskImgName( void ) const
{
	return (const char *)d88.name;
}


////////////////////////////////////////////////////////////////
// プロテクトシール状態取得
////////////////////////////////////////////////////////////////
bool cD88::IsProtect( void ) const
{
	return Protected;
}


////////////////////////////////////////////////////////////////
// メディアタイプ取得
////////////////////////////////////////////////////////////////
int cD88::GetType( void ) const
{
	int ret = FDUNKNOWN;
	
	switch( d88.type ){
	// 本当はこうだけど
//	case 0x00: ret = FD2D;  break;	// 2D
//	case 0x10: ret = FD2DD; break;	// 2DD
//	case 0x20: ret = FD2HD; break;	// 2HD
	// P6の場合はこう
	case 0x00: ret = FD1D;  break;	// 1D
	case 0x10: ret = FD1DD; break;	// 1DD
	}
	return ret;
}
