/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <cstring>

#include "pc6001v.h"

#include "log.h"
#include "osd.h"
#include "p6t2.h"


// P6T形式フォーマットVer.2
//  基本的には「ベタイメージ+フッタ+ベタイメージサイズ(4byte)」という構造
//  フッタにはDATAブロック情報が含まれる。
//  DATAブロックは無音部を区切りとして1と数える。
//  BASICなど「ヘッダ＋データ」という構造の場合，DATAブロックは2個となるが
//  ID番号を同一にすることで関連付けて管理できる。
//
// [フッタ]
//  header (2byte) : "P6"
//  ver    (1byte) : バージョン
//  dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
//  start  (1byte) : オートスタートフラグ(0:無効 1:有効)
//  basic  (1byte) : BASICモード(PC-6001の場合は無意味)
//  page   (1byte) : ページ数
//  askey  (2byte) : オートスタートコマンド文字数
//  ...コマンドがある場合はこの後にaskey分続く
//  exhead (2byte) : 拡張情報サイズ(64KBまで)
//  ...拡張情報がある場合はこの後にexhead分続く
//
// [DATAブロック]
//  header (2byte) : "TI"
//  id     (1byte) : ID番号(DATAブロックを関連付ける)
//  name  (16byte) : データ名(15文字+'00H')
//  baud   (2byte) : ボーレート(600/1200)
//  stime  (2byte) : 無音部の時間(ms)
//  ptime  (2byte) : ぴー音の時間(ms)
//  offset (4byte) : ベタイメージ先頭からのオフセット
//  size   (4byte) : データサイズ



/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cP6DATA::cP6DATA( void ) : rp(Data.cbegin())
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cP6DATA::~cP6DATA( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 巻戻し
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::Rewind( void )
{
	rp = Data.cbegin();
}


/////////////////////////////////////////////////////////////////////////////
// P6T ブロック情報取得
/////////////////////////////////////////////////////////////////////////////
const P6TBLKINFO& cP6DATA::GetInfo( void )
{
	return Info;
}


/////////////////////////////////////////////////////////////////////////////
// データ名設定
/////////////////////////////////////////////////////////////////////////////
int cP6DATA::SetName( const std::string& name )
{
	ZeroMemory( Info.Name, sizeof(Info.Name) );
	std::strncpy( Info.Name, name.c_str(), sizeof(Info.Name) - 1 );
	
	return std::strlen( Info.Name );
}


/////////////////////////////////////////////////////////////////////////////
// ボーレート設定
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::SetBaud( int baud )
{
	Info.Baud = baud;
}


/////////////////////////////////////////////////////////////////////////////
// ストップビット設定
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::SetStopBit( int bits )
{
	Info.StopBit = bits;
}


/////////////////////////////////////////////////////////////////////////////
// 無音部,ぴー音時間セット
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::SetPeriod( int stime, int ptime )
{
	PRINTD( P6T2_LOG, "[cP6DATA][SetPeriod] s:%d p:%d\n", stime, ptime );
	
	Info.STime = stime;	// 無音部の時間(ms)セット
	Info.PTime = ptime;	// ぴー音の時間(ms)セット
}


/////////////////////////////////////////////////////////////////////////////
// データサイズ取得
/////////////////////////////////////////////////////////////////////////////
DWORD cP6DATA::GetSize( void ) const
{
	return Data.size();
}


/////////////////////////////////////////////////////////////////////////////
// データサイズ設定
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::SetSize( DWORD size )
{
	Data.resize( size );
}


/////////////////////////////////////////////////////////////////////////////
// 読込みカウンタ取得
/////////////////////////////////////////////////////////////////////////////
DWORD cP6DATA::GetCount( void )
{
	return rp - Data.cbegin();
}


/////////////////////////////////////////////////////////////////////////////
// 読込みカウンタ設定
/////////////////////////////////////////////////////////////////////////////
void cP6DATA::SetCount( DWORD cnt )
{
	if( Data.size() > cnt ){ rp = Data.cbegin() + cnt; }
}


/////////////////////////////////////////////////////////////////////////////
// データ末尾?
/////////////////////////////////////////////////////////////////////////////
bool cP6DATA::IsEnd( void ) const
{
	return rp == Data.cend();
}


/////////////////////////////////////////////////////////////////////////////
// 1Byte読込み
/////////////////////////////////////////////////////////////////////////////
BYTE cP6DATA::Read( void )
{
	return IsEnd() ? 0 : *(rp++);
}


/////////////////////////////////////////////////////////////////////////////
// ファイルから読込み(フッタ)
/////////////////////////////////////////////////////////////////////////////
bool cP6DATA::Readff( std::fstream& fs )
{
	// 識別子 "TI"(0x4954) ?
	if( !(fs.is_open() && fs.good()) || FSGETWORD( fs ) != 0x4954 ){
		return false;
	}
	
	Info.ID = FSGETBYTE( fs );
	fs.read( Info.Name, sizeof(Info.Name) );
	Info.Baud   = FSGETWORD( fs );
	Info.STime  = FSGETWORD( fs );
	Info.PTime  = FSGETWORD( fs );
	Info.Offset = FSGETDWORD( fs );
	Data.resize( FSGETDWORD( fs ) );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルから読込み(データ)
/////////////////////////////////////////////////////////////////////////////
bool cP6DATA::Readfd( std::fstream& fs )
{
	if( !(fs.is_open() && fs.good()) ){
		return false;
	}
	
	DWORD fps = fs.tellg();		// 現在位置保存
	fs.seekg( Info.Offset, std::ios_base::beg );
	fs.read( (char*)Data.data(), Data.size() );
	fs.seekg( fps, std::ios_base::beg );	// ファイルポインタを戻す
	
	Rewind();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルに書込み(フッタ)
/////////////////////////////////////////////////////////////////////////////
bool cP6DATA::Writeff( std::fstream& fs )
{
	FSPUTBYTE( 'T', fs );
	FSPUTBYTE( 'I', fs );
	FSPUTBYTE( Info.ID, fs );
	fs.write( Info.Name, sizeof(Info.Name) );
	FSPUTWORD( Info.Baud, fs )
	FSPUTWORD( Info.STime, fs );
	FSPUTWORD( Info.PTime, fs );
	FSPUTDWORD( Info.Offset, fs );
	FSPUTDWORD( Data.size(), fs );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルに書込み(データ)
/////////////////////////////////////////////////////////////////////////////
bool cP6DATA::Writefd( std::fstream& fs )
{
	Info.Offset = fs.tellp();
	fs.write( (const char*)Data.data(), Data.size() );
	
	return true;
}




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cP6T::cP6T( void ) : Name(""), Version(0),rd(Data.begin()), swait(0), pwait(0)
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cP6T::~cP6T( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
void cP6T::Clear( void )
{
	Name.clear();
	Version = 0;
	Ainfo.Start = false;
	exh.clear();
	Data.clear();
	rd = Data.begin();
	swait = pwait = 0;
}


/////////////////////////////////////////////////////////////////////////////
// 巻戻し
/////////////////////////////////////////////////////////////////////////////
void cP6T::Rewind( void )
{
	PRINTD( P6T2_LOG, "[cP6T][Rewind]\n" );
	
	rd = Data.begin();		// 現在の読込みDATAブロック
	for( auto &i : Data )	// 全DATAブロック巻戻し
		i.Rewind();
	
	const P6TBLKINFO& binfo = Data.front().GetInfo();
	swait = ( binfo.STime * DEFAULT_BAUD ) / 1000;	// 無音部の待ちbit数
	pwait = ( binfo.PTime * DEFAULT_BAUD ) / 1000;	// ぴー音の待ちbit数
}


/////////////////////////////////////////////////////////////////////////////
// データ名設定
/////////////////////////////////////////////////////////////////////////////
int cP6T::SetName( const std::string& name )
{
	Name = name;
	
	return Name.length();
}


/////////////////////////////////////////////////////////////////////////////
// データ名取得
/////////////////////////////////////////////////////////////////////////////
const std::string& cP6T::GetName( void ) const
{
	return Name;
}


/////////////////////////////////////////////////////////////////////////////
// 無音部待ち?
/////////////////////////////////////////////////////////////////////////////
bool cP6T::IsSWaiting( int bpb )
{
	if( swait <= 0 ){
		return false;
	}
	
	swait -= bpb;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ぴー音待ち?
/////////////////////////////////////////////////////////////////////////////
bool cP6T::IsPWaiting( int bpb )
{
	if( pwait <= 0 ){
		return false;
	}
	
	pwait -= bpb;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// オートスタート?
/////////////////////////////////////////////////////////////////////////////
bool cP6T::IsAutoStart( void ) const
{
	return Ainfo.Start;
}


/////////////////////////////////////////////////////////////////////////////
// オートスタート情報取得
/////////////////////////////////////////////////////////////////////////////
const P6TAUTOINFO& cP6T::GetAutoStartInfo( void ) const
{
	return Ainfo;
}


/////////////////////////////////////////////////////////////////////////////
// ベタイメージサイズ取得
/////////////////////////////////////////////////////////////////////////////
DWORD cP6T::GetBetaSize( void )
{
	DWORD size = 0;
	
	for( auto& i : Data )
		size += i.GetSize();
	
	return size;
}


/////////////////////////////////////////////////////////////////////////////
// TAPEカウンタ取得
/////////////////////////////////////////////////////////////////////////////
int cP6T::GetCount( void ) const
{
	if( Data.empty() ){
		return 0;
	}
	
	return rd->GetInfo().Offset + rd->GetCount();
}


/////////////////////////////////////////////////////////////////////////////
// TAPEカウンタ設定
/////////////////////////////////////////////////////////////////////////////
void cP6T::SetCount( DWORD cnt )
{
	if( GetBetaSize() <= cnt ){
		return;
	}
	
	// 全DATAブロック巻戻し
	for( auto &i : Data )
		i.Rewind();
	
	rd = Data.begin();
	for( auto& i : Data ){
		if( i.GetInfo().Offset + i.GetSize() >= cnt ){
			break;
		}
		rd++;
	}
	
	rd->SetCount( cnt - rd->GetInfo().Offset );
}


/////////////////////////////////////////////////////////////////////////////
// 1文字読込み
/////////////////////////////////////////////////////////////////////////////
BYTE cP6T::ReadOne( void )
{
	BYTE dat = rd->Read();
	
	// 最後のデータまで読んだら巻戻して次のDATAブロックへ
	if( rd->IsEnd() ){
		(rd++)->Rewind();
		
		// 最後のDATAブロックまで読んだら先頭へ
		if( rd == Data.end() ){
			rd = Data.begin();
		}
		
		// ブロック情報取得
		const P6TBLKINFO& binfo = rd->GetInfo();
		swait = ( binfo.STime * DEFAULT_BAUD ) / 1000;	// 無音部の待ちbit数
		pwait = ( binfo.PTime * DEFAULT_BAUD ) / 1000;	// ぴー音の待ちbit数
	}
	
	return dat;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルから読込み
/////////////////////////////////////////////////////////////////////////////
bool cP6T::Readf( const P6VPATH& filepath )
{
	PRINTD( P6T2_LOG, "[cP6T][Readf] [%s]\n", P6VPATH2STR( filepath ).c_str() );
	
	// P6T読込み→ベタ読込み
	if( !ReadP6T( filepath ) && !ConvP6T( filepath ) ){
		return false;
	}
	
	Rewind();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルに書込み
/////////////////////////////////////////////////////////////////////////////
bool cP6T::Writef( const P6VPATH& filepath )
{
	PRINTD( P6T2_LOG, "[cP6T][Writef] [%s]\n", P6VPATH2STR( filepath ).c_str() );
	
	std::fstream fs;
	
	if( !OSD_FSopen( fs, filepath, std::ios_base::out | std::ios_base::binary ) ){
		return false;
	}
	
	// ベタイメージ書込み&サイズ取得
	DWORD beta = 0;
	for( auto& i : Data ){
		beta += i.Writefd( fs );
	}
	
	// P6T フッタ書込み
	FSPUTBYTE( 'P', fs );
	FSPUTBYTE( '6', fs );
	FSPUTBYTE( Version, fs );
	FSPUTBYTE( Data.size(), fs );
	FSPUTBYTE( Ainfo.Start ? 1 : 0, fs );
	FSPUTBYTE( Ainfo.BASIC, fs );
	FSPUTBYTE( Ainfo.Page, fs );
	FSPUTWORD( Ainfo.ask.size(), fs );
	if( !Ainfo.ask.empty() ){
		fs.write( Ainfo.ask.data(), Ainfo.ask.size() );
	}
	FSPUTWORD( exh.size(), fs );
	if( !exh.empty() ){
		fs.write( (const char*)exh.data(), exh.size() );
	}
	
	// 全DATAブロックフッタ書込み
	for( auto& i : Data ){
		i.Writeff( fs );
	}
	
	// ベタイメージサイズ書込み
	FSPUTDWORD( beta, fs );
	
	fs.close();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// P6Tを読込み
/////////////////////////////////////////////////////////////////////////////
bool cP6T::ReadP6T( const P6VPATH& filepath )
{
	PRINTD( P6T2_LOG, "[cP6T][ReadP6T] [%s]\n", P6VPATH2STR( filepath ).c_str() );
	
	std::fstream fs;
	
	if( !OSD_FSopen( fs, filepath, std::ios_base::in | std::ios_base::binary ) ){
		return false;
	}
	
	// ベタイメージサイズ取得
	fs.seekg( -4, std::ios_base::end );
	DWORD beta = FSGETDWORD( fs );
	// ベタイメージサイズがファイルサイズを超えていたらエラー
	if( beta > (DWORD)fs.tellg() ){
		fs.close();
		return false;
	}
	
	// フッタの先頭に移動
	fs.seekg( beta, std::ios_base::beg );
	
	// 識別子 "P6"(0x3650) でなければエラー
	if( FSGETWORD( fs ) != 0x3650 ){
		fs.close();
		return false;
	}
	
	Version     = FSGETBYTE( fs );					// バージョン
	BYTE bk		= FSGETBYTE( fs );					// 含まれるDATAブロック数
	Ainfo.Start = FSGETBYTE( fs ) ? true : false;	// オートスタートフラグ
	Ainfo.BASIC = FSGETBYTE( fs );					// BASICモード
	Ainfo.Page  = FSGETBYTE( fs );					// ページ数
	
	// オートスタートコマンド
	Ainfo.ask.resize( FSGETWORD( fs ) );
	if( !Ainfo.ask.empty() ){
		fs.read( Ainfo.ask.data(), Ainfo.ask.size() );
		Ainfo.ask.emplace_back( '\0' );
	}
	
	// 拡張情報
	exh.resize( FSGETWORD( fs ) );
	if( !exh.empty() ){
		fs.read( (char*)exh.data(), exh.size() );
	}
	
	// DATAブロックを読込み
	while( bk-- ){
		// DATAブロック追加
		// C++14
		Data.emplace_back();
		cP6DATA& b = Data.back();
		// C++17
//		cP6DATA& b = Data.emplace_back();
		
		// ファイルから読込み(フッタ&データ)
		if( !b.Readff( fs ) || !b.Readfd( fs ) ){
			Data.pop_back();
			fs.close();
			return false;
		}
	}
	
	fs.close();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ベタをP6Tに変換して読込み
/////////////////////////////////////////////////////////////////////////////
bool cP6T::ConvP6T( const P6VPATH& filepath )
{
	PRINTD( P6T2_LOG, "[cP6T][ConvP6T] [%s]\n", P6VPATH2STR( filepath ).c_str() );
	
	std::fstream fs;
	
	// P6T情報設定
	SetName( OSD_GetFileNamePart( filepath ) );	// データ名はファイル名
	Version     = 2;						// バージョン(とりあえず2)
	Ainfo.Start = false;					// オートスタートフラグ(無効)
	Ainfo.BASIC = 1;						// BASICモード(PC-6001の場合は無意味)(とりあえず1だが無意味)
	Ainfo.Page  = 1;						// ページ数(とりあえず1だが無意味)
	Ainfo.ask.clear();						// オートスタートコマンド(なし)
	exh.clear();							// 拡張情報(なし)
	
	// DATAブロックを作成
	Data.clear();
	
	// C++14
	Data.emplace_back();
	cP6DATA& b = Data.back();
	// C++17
//	cP6DATA& b = Data.emplace_back();
	
	b.SetName( "BetaImage" );				// データ名はとりあえず"BetaImage"
	b.SetBaud( DEFAULT_BAUD );				// ボーレート
	b.SetStopBit( 0 );						// ストップビット(0:DEFAULT)
	b.SetPeriod( 3400, 6800 );				// 無音部(3400ms),ぴー音(6800ms)時間セット
	b.SetSize( OSD_GetFileSize( filepath ) );	// データサイズ = ベタイメージサイズ
	
	if( !OSD_FSopen( fs, filepath, std::ios_base::in | std::ios_base::binary ) ){
		SetName( "" );
		Data.clear();
		return false;
	}
	
	b.Readfd( fs );							// ベタイメージ読込み
	fs.close();
	
	return true;
}

