#include <string.h>

#include "log.h"
#include "d88.h"
#include "common.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cD88::cD88( void )
{
	PRINTD( D88_LOG, "[D88][cD88]\n" )
	
	ZeroMemory( &d88, sizeof( D88INFO ) );
	
	Protected = FALSE;
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
BOOL cD88::Init( char *fname )
{
	PRINTD1( D88_LOG, "[D88][Init] %s\n", fname )
	
	strncpy( FileName, fname, PATH_MAX );
	
	// 読取り専用属性ならプロテクト状態で開く
	if( OSD_FileReadOnly( FileName ) ){
		d88.fp = FOPENEN( FileName, "rb" );
		Protected = TRUE;	// プロテクトシールあり
	}else{
		d88.fp = FOPENEN( FileName, "rb+" );
		Protected = FALSE;	// プロテクトシールなし
	}
	
	if( !d88.fp ){
		*FileName = 0;
		Protected = FALSE;
		return FALSE;
	}
	
	ReadHeader88();	// D88 ヘッダ読込み
	
	return TRUE;
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
		if( d88.protect ) Protected = TRUE;
		else if( Protected ) d88.protect = 0x10;
		
		// DISKの種類
		d88.type = fgetc( d88.fp );
		
		// DISKのサイズ
		d88.size = FGETDWORD( d88.fp );
		
		// トラック部のオフセットテーブル
		for( int i=0; i<164; i++ )
			d88.table[i] = FGETDWORD( d88.fp );
		
		PRINTD1( D88_LOG, " FileName : %s\n", d88.name )
		PRINTD1( D88_LOG, " Protect  : %s\n", d88.protect ? "ON" : "OFF" )
		PRINTD1( D88_LOG, " Format   : %d\n", d88.type )
		PRINTD1( D88_LOG, " Size     : %d\n", (int)d88.size )
	}
}


////////////////////////////////////////////////////////////////
// D88 セクタ情報読込み
////////////////////////////////////////////////////////////////
void cD88::ReadSector88( void )
{
//	PRINTD( D88_LOG, "[D88][ReadSector88]\n" );
	
	if( d88.fp ){
		d88.secinfo.c = fgetc( d88.fp );			// ID の C (シリンダNo 片面の場合は=トラックNo)
		d88.secinfo.h = fgetc( d88.fp );			// ID の H (ヘッダアドレス 片面の場合は=0)
		d88.secinfo.r = fgetc( d88.fp );			// ID の R (トラック内のセクタNo)
		d88.secinfo.n = fgetc( d88.fp );			// ID の N (セクタサイズ 0:256 1:256 2:512 3:1024)
		d88.secinfo.sec_nr  = FGETWORD( d88.fp );	// このトラック内に存在するセクタの数
		d88.secinfo.density = fgetc( d88.fp );		// 記録密度     0x00:倍密度   0x40:単密度
		d88.secinfo.deleted = fgetc( d88.fp );		// DELETED MARK 0x00:ノーマル 0x10:DELETED
		d88.secinfo.status  = fgetc( d88.fp );		// ステータス
		fread( d88.secinfo.reserve, sizeof(BYTE), 5, d88.fp );	// リザーブ空読み
		d88.secinfo.size   = FGETWORD( d88.fp );	// このセクタ部のデータサイズ
		d88.secinfo.data   = ftell( d88.fp );		// データへのオフセット
		d88.secinfo.offset = 0;						// 次に読込むデータのセクタ先頭からのオフセット
		
//		PRINTD1( D88_LOG, " C      : %d\n", d88.secinfo.c )
//		PRINTD1( D88_LOG, " H      : %d\n", d88.secinfo.h )
//		PRINTD1( D88_LOG, " R      : %d\n", d88.secinfo.r )
//		PRINTD1( D88_LOG, " N      : %d\n", d88.secinfo.n )
//		PRINTD1( D88_LOG, " SectNum: %d\n", d88.secinfo.sec_nr )
//		PRINTD1( D88_LOG, " Density: %s\n", d88.secinfo.density&0x40 ? "D" : "DD" )
//		PRINTD1( D88_LOG, " Del    : %s\n", d88.secinfo.deleted&0x10 ? "DELETED" : "NORMAL" )
//		PRINTD1( D88_LOG, " Stat   : %02X\n", d88.secinfo.status )
//		PRINTD1( D88_LOG, " Size   : %d\n", d88.secinfo.size )
//		PRINTD1( D88_LOG, " Offset : %d\n", (int)d88.secinfo.data )
	}
}


////////////////////////////////////////////////////////////////
// 1byte 読込み
////////////////////////////////////////////////////////////////
BYTE cD88::Getc88( void )
{
	PRINTD( D88_LOG, "[D88][Getc88] -> " )
	
	if( d88.fp ){
		BYTE dat = fgetc( d88.fp );
		d88.secinfo.offset++;
		
		PRINTD1( D88_LOG, "%02X\n", dat );
		
		// セクタの終わりに到達したら次のセクタをシークする
		// (ほんと?せめて同一トラック内に限定しといたほうがいい?)
		if( d88.secinfo.offset >= d88.secinfo.size ) ReadSector88();
		
		return dat;
	}
	PRINTD( D88_LOG, "FALSE(0xff)\n" );
	
	return 0xff;
}


////////////////////////////////////////////////////////////////
// 1byte 書込み
////////////////////////////////////////////////////////////////
BOOL cD88::Putc88( BYTE dat )
{
	PRINTD5( D88_LOG, "[D88][Putc88] -> %02X(%02d:%02d:%02d:%02d)", dat, d88.secinfo.c, d88.secinfo.h, d88.secinfo.r, d88.secinfo.n );
	
	if( d88.fp && !d88.protect ){
		fseek( d88.fp, 0, SEEK_CUR );	// ごまかしっぽい
		fputc( dat, d88.fp );
		fseek( d88.fp, 0, SEEK_CUR );	// これも
		d88.secinfo.offset++;
		
		// セクタの終わりに到達したら次のセクタをシークする
		// (ほんと?せめて同一トラック内に限定しといたほうがいい?)
		if( d88.secinfo.offset >= d88.secinfo.size ) ReadSector88();
		
		PRINTD( D88_LOG, " ->OK\n" );
		
		return TRUE;
	}
	PRINTD( D88_LOG, " ->NG\n" );
	
	return FALSE;
}


////////////////////////////////////////////////////////////////
// シーク
////////////////////////////////////////////////////////////////
BOOL cD88::Seek88( int trackno, int sectno )
{
	PRINTD2( D88_LOG, "[D88][Seek88] Track : %d Sector : %d", trackno, sectno );
	
	if( d88.fp ){
		// トラックが無効ならエラー
		if( !d88.table[trackno] ) return FALSE;
		PRINTD1( D88_LOG, " -> Track:%d\n", trackno );
		
		// トラックの先頭をシーク
		fseek( d88.fp, d88.table[trackno], SEEK_SET );
		
		// 最初のセクタ情報読込み
		ReadSector88();
		if( d88.secinfo.sec_nr < sectno ) return FALSE;	// セクタ番号は有効?
		PRINTD1( D88_LOG, " -> Sector:%d", sectno );
		
		// 目的のセクタが現れるまで空読み
		int TryCnt = 2;	// トライ回数カウンタ
		while( d88.secinfo.r != sectno ){
			// トラック2周して見つからなければエラー
			if( !TryCnt ) return FALSE;
			fseek( d88.fp, (long)d88.secinfo.size, SEEK_CUR );
			ReadSector88();	// 次のセクタ情報読込み
			if( d88.secinfo.r == d88.secinfo.sec_nr ) TryCnt--;
		}
		PRINTD( D88_LOG, " -> OK\n" );
		
		return TRUE;
	}
	PRINTD( D88_LOG, " -> FALSE\n" );
	
	return FALSE;
}


////////////////////////////////////////////////////////////////
// 現在のCHRN取得
////////////////////////////////////////////////////////////////
DWORD cD88::GetCHRN( void )
{
	PRINTD( D88_LOG, "[D88][GetCHRN]\n" );
	
	return d88.secinfo.c<<24 || d88.secinfo.h<<16 || d88.secinfo.r<<8 || d88.secinfo.n;
}


////////////////////////////////////////////////////////////////
// ファイル名取得
////////////////////////////////////////////////////////////////
char *cD88::GetFileName( void )
{
	return FileName;
}


////////////////////////////////////////////////////////////////
// DISKイメージ名取得
////////////////////////////////////////////////////////////////
char *cD88::GetDiskImgName( void )
{
	return (char *)d88.name;
}


////////////////////////////////////////////////////////////////
// プロテクトシール状態取得
////////////////////////////////////////////////////////////////
BOOL cD88::IsProtect( void )
{
	return Protected;
}
