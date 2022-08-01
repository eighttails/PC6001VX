/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "d88.h"
#include "log.h"
#include "osd.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cD88::cD88( bool dd ) : FilePath(""), DDDrv(dd), Protected(false), trkno(0)
{
	PRINTD( D88_LOG, "[D88][cD88] %s Drive\n", DDDrv ? "1DD" : "1D" )
	fs.clear();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cD88::~cD88( void )
{
	if( fs.is_open() ) fs.close();
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool cD88::Init( const P6VPATH& filepath )
{
	std::ios_base::openmode mode;
	
	PRINTD( D88_LOG, "[D88][Init] %s\n", P6VPATH2STR( filepath ).c_str() )
	
	// 読取り専用属性ならプロテクト状態で開く
	if( OSD_FileReadOnly( filepath ) ){
		mode      = std::ios_base::in | std::ios_base::binary;
		Protected = true;	// プロテクトシールあり
	}else{
		mode      = std::ios_base::in | std::ios_base::out | std::ios_base::binary;
		Protected = false;	// プロテクトシールなし
	}
	
	if( !OSD_FSopen( fs, filepath, mode ) ){
		FilePath.clear();
		Protected = false;
		return false;
	}
	
	FilePath = filepath;
	
	ReadHeader88();	// D88 ヘッダ読込み
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// D88 ヘッダ読込み
/////////////////////////////////////////////////////////////////////////////
void cD88::ReadHeader88( void )
{
	BYTE buf[32];
	
	PRINTD( D88_LOG, "[D88][ReadHeader88]\n" )
	
	if( !fs.is_open() ) return;
	
	// DISK名
	fs.read( (char*)buf, 17 * sizeof(BYTE) );
	buf[16] = '\0';
	d88.name = (const char*)buf;
	OSD_SJIStoUTF8( d88.name );	// SJIS->UTF-8
	
	// リザーブ
	fs.read( (char*)d88.reserve, sizeof(d88.reserve) );
	
	// ライトプロテクト
	d88.protect = FSGETBYTE( fs );
	if( d88.protect ) Protected = true;
	else if( Protected ) d88.protect = 0x10;
	
	// DISKの種類
	d88.type = FSGETBYTE( fs );
	
	// DISKのサイズ
	d88.size = FSGETDWORD( fs );
	
	// トラック部のオフセットテーブル初期化
	d88.table.fill( 0 );
	
	// 1DDドライブで1Dディスクを使う時は2トラック飛びで読込む
	if( DDDrv && !(GetType() & FD_DOUBLETRACK) ){
		PRINTD( D88_LOG, " (1D disk on 1DD drive)\n" )
		for( int i = 0; i < 164; i += 2 )
			d88.table[i] = FSGETDWORD( fs );
	}else
		for( int i = 0; i < 164; i++ )
			d88.table[i] = FSGETDWORD( fs );
		
	// アクセス中のトラックNo
	trkno = 0;
	
	PRINTD( D88_LOG, " ImgName : %s\n", d88.name.c_str() )
	PRINTD( D88_LOG, " Protect : %s\n", d88.protect ? "ON" : "OFF" )
	PRINTD( D88_LOG, " Media   : %02X\n", d88.type )
	PRINTD( D88_LOG, " Size    : %d\n", (int)d88.size )
}


/////////////////////////////////////////////////////////////////////////////
// D88 セクタ情報読込み
/////////////////////////////////////////////////////////////////////////////
void cD88::ReadSector88( void )
{
	PRINTD( D88_LOG, "[D88][ReadSector88]\n" );
	
	if( !(fs.is_open() && d88.table[trkno]) ){
		ZeroMemory( &secinfo, sizeof(D88SECTOR) );
		return;
	}
	
	secinfo.c       = FSGETBYTE( fs );		// ID の C (シリンダNo 片面の場合は=トラックNo)
	secinfo.h       = FSGETBYTE( fs );		// ID の H (ヘッダアドレス 片面の場合は=0)
	secinfo.r       = FSGETBYTE( fs );		// ID の R (トラック内のセクタNo)
	secinfo.n       = FSGETBYTE( fs );		// ID の N (セクタサイズ 0:256 1:256 2:512 3:1024)
	secinfo.sec_nr  = FSGETWORD( fs );		// このトラック内に存在するセクタの数
	secinfo.density = FSGETBYTE( fs );		// 記録密度     0x00:倍密度   0x40:単密度
	secinfo.deleted = FSGETBYTE( fs );		// DELETED MARK 0x00:ノーマル 0x10:DELETED
	secinfo.status  = FSGETBYTE( fs );		// ステータス
	fs.read( (char*)secinfo.reserve, sizeof(secinfo.reserve) );	// リザーブ
	secinfo.size    = FSGETWORD( fs );		// このセクタ部のデータサイズ
	secinfo.data    = fs.tellg();			// データへのオフセット
	secinfo.offset  = 0;					// 次に読込むデータのセクタ先頭からのオフセット
	secinfo.secno++;						// アクセス中のセクタNo
	
	// Dittのバグ対応
	// 吸出し時，データサイズ=0の時に00Hが256バイト格納されるバグがあるらしい
	// データサイズが0だったら続く256バイトを調べ，全て00Hだったら読み飛ばす
	// 00H以外のデータが現れたら次のセクタのデータと判断し，読み飛ばさない。
	// 256バイト先がトラックorディスクの末尾に到達する場合も読み飛ばさない
	if( (secinfo.size == 0) &&
		(((trkno < 163) && (d88.table[trkno+1] >= secinfo.data+256)) ||
		 (d88.size >= secinfo.data+256)	) ){
		
		for( int i = 0; i < 256; i++ ){
			if( FSGETBYTE( fs ) != 0 ){
				fs.seekg( secinfo.data, std::ios_base::beg );
				break;
			}
		}
	}
	
	PRINTD( D88_LOG, " C      : %d\n", secinfo.c )
	PRINTD( D88_LOG, " H      : %d\n", secinfo.h )
	PRINTD( D88_LOG, " R      : %d\n", secinfo.r )
	PRINTD( D88_LOG, " N      : %d\n", secinfo.n )
	PRINTD( D88_LOG, " SectNum: %d/%d\n", secinfo.secno, secinfo.sec_nr )
	PRINTD( D88_LOG, " Density: %s\n", secinfo.density & 0x40 ? "S" : "D" )
	PRINTD( D88_LOG, " Del    : %s\n", secinfo.deleted & 0x10 ? "DELETED" : "NORMAL" )
	PRINTD( D88_LOG, " Stat   : %02X\n", secinfo.status )
	PRINTD( D88_LOG, " Size   : %d\n", secinfo.size )
	PRINTD( D88_LOG, " Offset : %d\n", (int)secinfo.data )
}


/////////////////////////////////////////////////////////////////////////////
// 1byte 読込み
/////////////////////////////////////////////////////////////////////////////
BYTE cD88::Get8( void )
{
	BYTE dat;
	
	PRINTD( D88_LOG, "[D88][Get8] -> " )
	
	if( !(fs.is_open() && d88.table[trkno]) ){
		PRINTD( D88_LOG, "false(0xff)\n" );
		return 0xff;
	}
	
	// セクタの終わりに到達したら次のセクタをシークする
	// 最終セクタの次は同一トラックの先頭セクタに移動
	// エラーセクタの場合は次のセクタに移動しない(Ditt!のエラー対応)
	if( secinfo.offset >= secinfo.size && !secinfo.status ){
		if( secinfo.secno > secinfo.sec_nr ) Seek( trkno );
		else										 ReadSector88();
	}
	dat = FSGETBYTE( fs );
	secinfo.offset++;
	
	PRINTD( D88_LOG, "%02X\n", dat );
	
	return dat;
}


/////////////////////////////////////////////////////////////////////////////
// 1byte 書込み
/////////////////////////////////////////////////////////////////////////////
bool cD88::Put8( BYTE dat )
{
	PRINTD( D88_LOG, "[D88][Put8] -> %02X(%02d:%02d:%02d:%02d)", dat, secinfo.c, secinfo.h, secinfo.r, secinfo.n );
	
	if( !(fs.is_open() && d88.table[trkno] && !d88.protect) ){
		PRINTD( D88_LOG, " ->NG\n" );
		return false;
	}
	
	// セクタの終わりに到達したら次のセクタをシークする
	// 最終セクタの次は同一トラックの先頭セクタに移動
	if( secinfo.offset >= secinfo.size ){
		if( secinfo.secno > secinfo.sec_nr ) Seek( trkno );
		else										 ReadSector88();
	}
	
	// r+,w+,a+ で開いたファイルに対して読込みと書込みを切り替える場合は
	// 必ず fsetpos,fseek,rewind のいずれかの関数を実行する必要があるらしい
	fs.seekp( 0, std::ios_base::cur );
	FSPUTBYTE( dat, fs );
	fs.seekg( 0, std::ios_base::cur );
	
	secinfo.offset++;
	
	PRINTD( D88_LOG, " ->OK\n" );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// シーク
/////////////////////////////////////////////////////////////////////////////
bool cD88::Seek( int trackno, int sectno )
{
	PRINTD( D88_LOG, "[D88][Seek] Track : %d Sector : %d ", trackno, sectno );
	
	if( !fs.is_open() ){
		PRINTD( D88_LOG, "-> false\n" );
		return false;
	}
	
	ZeroMemory( &secinfo, sizeof(D88SECTOR) );
	trkno          = trackno;
	secinfo.status = BIOS_MISSING_IAM;
	
	// トラックが無効ならUnformat扱い
	if( !d88.table[trkno] ){
		PRINTD( D88_LOG, "-> Unformat\n" );
		return false;
	}
	PRINTD( D88_LOG, "-> Track:%d\n", trkno );
	
	// トラックの先頭をシーク
	fs.seekg( d88.table[trkno], std::ios_base::beg );
	
	// 最初のセクタ情報読込み
	ReadSector88();
	
	// 目的のセクタを頭出し
	if( sectno > 1 ){
		fs.seekg( (long)secinfo.size, std::ios_base::cur );
		ReadSector88();
	}
	
	PRINTD( D88_LOG, "-> OK\n" );
	secinfo.status = BIOS_READY;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// セクタを探す
/////////////////////////////////////////////////////////////////////////////
bool cD88::SearchSector( BYTE c, BYTE h, BYTE r, BYTE n )
{
	PRINTD( D88_LOG, "[D88][SearchSector] C:%02X H:%02X R:%02X N:%02X ", c, h, r, n );
	
	if( Seek( trkno ) ){
		// 目的のセクタが現れるまで空読み
		while( secinfo.secno <= secinfo.sec_nr ){
			// IDをチェック
			if( ( secinfo.c == c ) && ( secinfo.h == h )
			 && ( secinfo.r == r ) && ( secinfo.n == n ) ){
				PRINTD( D88_LOG, "-> Found\n" );
				return true;
			}
			// 一致しなければ次のセクタ情報読込み
			fs.seekg( (long)secinfo.size, std::ios_base::cur );
			ReadSector88();
		}
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// 次のセクタに移動する
/////////////////////////////////////////////////////////////////////////////
bool cD88::NextSector( void )
{
	PRINTD( D88_LOG, "[D88][NextSector] Sector:%d ", secinfo.secno );
	
	if( !secinfo.sec_nr ){
		PRINTD( D88_LOG, "-> false\n" );
		return false;
	}
	
	int ssize = secinfo.size - secinfo.offset;	// 現在のセクタ終端までのデータ数
	
	if( secinfo.secno == secinfo.sec_nr )
		// 最終セクタの次は同一トラックの先頭セクタに移動
		Seek( trkno );
	else{
		// 次のセクタ先頭まで移動してセクタ情報を読込み
		fs.seekg( (long)ssize, std::ios_base::cur );
		ReadSector88();
	}
	PRINTD( D88_LOG, "-> %d\n", secinfo.secno );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のCHRN取得
/////////////////////////////////////////////////////////////////////////////
void cD88::GetID( BYTE* C, BYTE* H, BYTE* R, BYTE* N ) const
{
	PRINTD( D88_LOG, "[D88][GetID] %02X %02X %02X %02X\n", secinfo.c, secinfo.h, secinfo.r, secinfo.n );
	
	if( C ) *C = secinfo.c;
	if( H ) *H = secinfo.h;
	if( R ) *R = secinfo.r;
	if( N ) *N = secinfo.n;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のセクタサイズ取得
/////////////////////////////////////////////////////////////////////////////
WORD cD88::GetSecSize( void ) const
{
	PRINTD( D88_LOG, "[D88][GetSecSize] %d\n", secinfo.size );
	
	return secinfo.size;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のトラック番号取得
/////////////////////////////////////////////////////////////////////////////
BYTE cD88::Track( void ) const
{
	PRINTD( D88_LOG, "[D88][Track] %d\n", trkno );
	
	return trkno;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のセクタ番号取得
/////////////////////////////////////////////////////////////////////////////
BYTE cD88::Sector( void ) const
{
	PRINTD( D88_LOG, "[D88][Sector] %d\n", secinfo.secno );
	
	return secinfo.secno;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のトラック内に存在するセクタ数取得
/////////////////////////////////////////////////////////////////////////////
WORD cD88::SecNum( void ) const
{
	PRINTD( D88_LOG, "[D88][SecNum] %d\n", secinfo.sec_nr );
	
	return secinfo.sec_nr;
}


/////////////////////////////////////////////////////////////////////////////
// 現在のステータス取得
/////////////////////////////////////////////////////////////////////////////
BYTE cD88::GetSecStatus( void ) const
{
	PRINTD( D88_LOG, "[D88][GetStatus] %02X\n", secinfo.status );
	
	return secinfo.status;
}


/////////////////////////////////////////////////////////////////////////////
// ファイル名取得
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& cD88::GetFileName( void ) const
{
	return FilePath;
}


/////////////////////////////////////////////////////////////////////////////
// DISKイメージ名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& cD88::GetDiskImgName( void ) const
{
	return d88.name;
}


/////////////////////////////////////////////////////////////////////////////
// プロテクトシール状態取得
/////////////////////////////////////////////////////////////////////////////
bool cD88::IsProtect( void ) const
{
	return Protected;
}


/////////////////////////////////////////////////////////////////////////////
// メディアタイプ取得
/////////////////////////////////////////////////////////////////////////////
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
