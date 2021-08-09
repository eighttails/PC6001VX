/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <filesystem>
#include <fstream>
#include <string>

#include "log.h"
#include "osd.h"


/////////////////////////////////////////////////////////////////////////////
// パス名処理関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// パスの末尾にデリミタを追加
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddDelimiter( P6VPATH& path )
{
	path /= "";
}


/////////////////////////////////////////////////////////////////////////////
// パスの末尾のデリミタを削除
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_DelDelimiter( P6VPATH& path )
{
	if( path.filename().empty() ) path = path.parent_path();
}


/////////////////////////////////////////////////////////////////////////////
// 相対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_RelativePath( P6VPATH& path )
{
	if( path.empty() ) return;
	
	std::error_code ec;
	P6VPATH p = std::filesystem::proximate( path, OSD_GetConfigPath(), ec );
	if( ec ) return;
	
	// ../なら絶対パス化
	if( P6VPATH2STR( p ).length() >= 2 && P6VPATH2STR( p ).substr( 0, 2 ) == ".." )
		OSD_AbsolutePath( p );
	
	path = p;
}


/////////////////////////////////////////////////////////////////////////////
// 絶対パス化
//
// 引数:	path			パス
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AbsolutePath( P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_AbsolutePath] %s -> ", P6VPATH2STR( path ).c_str() );
	
	if( path.empty() ) return;
	
	P6VPATH p = path;
	
	// 既に絶対パスなら正規化のみ実施
	if( p.is_relative() && !p.has_root_name() )	// Windowsの場合, "C:"は is_relative()==true らしい
		p = OSD_GetConfigPath() / p;
	
	// パスを結合して正規化
	path = std::filesystem::weakly_canonical( p );
	
	PRINTD( OSD_LOG, "%s\n", P6VPATH2STR( path ).c_str() );
}


/////////////////////////////////////////////////////////////////////////////
// パス結合
//
// 引数:	cpath			結合後パス
//			path1			パス1
//			path2			パス2
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void OSD_AddPath( P6VPATH& cpath, const P6VPATH& path1, const P6VPATH& path2 )
{
	cpath = path1 / path2;
}


/////////////////////////////////////////////////////////////////////////////
// パスからフォルダ名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFolderNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFolderNamePart]\n" );
	
	P6VPATH p = path;
	
	return P6VPATH2STR( p.remove_filename() );
}


/////////////////////////////////////////////////////////////////////////////
// パスからファイル名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNamePart( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );
	
	return P6VPATH2STR( path.filename() );
}


/////////////////////////////////////////////////////////////////////////////
// パスから拡張子名を取得
//
// 引数:	path			パス
// 返値:	std::string		取得した文字列(UTF-8)
/////////////////////////////////////////////////////////////////////////////
const std::string OSD_GetFileNameExt( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNameExt]\n" );
	
	std::string ext = P6VPATH2STR( path.extension() );
	ext.erase( ext.begin() );
	return ext;
}


/////////////////////////////////////////////////////////////////////////////
// 拡張子名を変更
//
// 引数:	path			パス
//			ext				新しい拡張子への参照
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_ChangeFileNameExt( P6VPATH& path, const std::string& ext )
{
	PRINTD( OSD_LOG, "[OSD][OSD_ChangeFileNameExt] %s -> %s\n", OSD_GetFileNameExt( path ).c_str(), ext.c_str() );
	
	path.replace_extension( STR2P6VPATH( ext ) );
	return OSD_GetFileNameExt( path ) == ext;
}




/////////////////////////////////////////////////////////////////////////////
// ファイル操作関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// ファイルを開く
//
// 引数:	path			パス
//			mode			モード文字列への参照
// 返値:	FILE*			ファイルポインタ
/////////////////////////////////////////////////////////////////////////////
FILE* OSD_Fopen( const P6VPATH& path, const std::string& mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_Fopen] %s(%s) ", P6VPATH2STR( path ).c_str(), mode.c_str() );
	
	char str[PATH_MAX+1];
	
	// Windowsの場合 native()はwchar_tなのでcharに変換
	// POSIXの場合 native()は多分UTF-8なのだけど 処理共通化のために一旦wstring()にしてからcharに戻す
	std::wcstombs( str, path.wstring().c_str(), sizeof(str) );
	return fopen( str, mode.c_str() );
}


/////////////////////////////////////////////////////////////////////////////
// ファイルストリームを開く
//
// 引数:	fs				ファイルストリームへの参照
//			path			パス
//			mode			モード
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FSopen( std::fstream& fs, const P6VPATH& path, const std::ios_base::openmode mode )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FSopen] %s\n", P6VPATH2STR( path ).c_str() );
	
	fs.open( path, mode );
	return fs.is_open() && fs.good();
}


/////////////////////////////////////////////////////////////////////////////
// フォルダを作成
//
// 引数:	path			パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_CreateFolder( const P6VPATH& path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_CreateFolder] %s\n", P6VPATH2STR( path ).c_str() );
	
	try{
		std::error_code ec;
		
		P6VPATH tpath = path;
		OSD_DelDelimiter( tpath );	// デリミタ付きだと作成されない?
		OSD_AbsolutePath( tpath );
		PRINTD( OSD_LOG, "-> %s\n", P6VPATH2STR( tpath ).c_str() );
		
		// 設定ファイルパスより外側には作成しない
		if( P6VPATH2STR( tpath ).compare( 0, P6VPATH2STR( OSD_GetConfigPath() ).length(), P6VPATH2STR( OSD_GetConfigPath() ) ) ) return false;
		
		return std::filesystem::create_directories( tpath, ec );
	}
	catch( std::filesystem::filesystem_error& ){
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルの存在チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:存在する false:存在しない
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileExist( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileExist] %s\n", P6VPATH2STR( fullpath ).c_str() );
	
	try{
		return std::filesystem::exists( std::filesystem::status( fullpath ) );
	}
	catch( std::filesystem::filesystem_error& ){
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルサイズ取得
//
// 引数:	fullpath		パス
// 返値:	DWORD			ファイルサイズ
/////////////////////////////////////////////////////////////////////////////
DWORD OSD_GetFileSize( const P6VPATH& fullpath )
{
	try{
		return std::filesystem::file_size( fullpath );
	}
	catch( std::filesystem::filesystem_error& ){
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルの読取り専用チェック
//
// 引数:	fullpath		パス
// 返値:	bool			true:読取り専用 false:読み書き
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly] %s\n", P6VPATH2STR( fullpath ).c_str() );
	
	try{
		std::filesystem::perms perm = std::filesystem::status( fullpath ).permissions();
		return ( perm & ( std::filesystem::perms::owner_write |
						  std::filesystem::perms::group_write |
						  std::filesystem::perms::others_write ) ) == std::filesystem::perms::none ? true : false;
	}
	catch( std::filesystem::filesystem_error& ){
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルを削除
//
// 引数:	fullpath		パス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileDelete( const P6VPATH& fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileDelete] %s\n", P6VPATH2STR( fullpath ).c_str() );
	
	try{
		return std::filesystem::remove( fullpath );
	}
	catch( std::filesystem::filesystem_error& ){
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ファイルを探す
//
// 引数:	path			パス
//			file			探すファイル名
//			folders			見つかったパスを格納するvectorへの参照
//			size			ファイルサイズ (0:チェックしない)
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FindFile( const P6VPATH& path, const P6VPATH& file, std::vector<P6VPATH>& files, size_t size )
{
	std::string sfile = OSD_GetFileNamePart( file );
	std::transform( sfile.begin(), sfile.end(), sfile.begin(), ::tolower );	// 小文字
	
	for( const std::filesystem::directory_entry& ent : std::filesystem::recursive_directory_iterator( path ) ){
		if( is_regular_file( ent.path() ) ){
			std::string tfile = OSD_GetFileNamePart( ent.path() );
			std::transform( tfile.begin(), tfile.end(), tfile.begin(), ::tolower );	// 小文字
			if( tfile == sfile && (!size || OSD_GetFileSize( ent.path() ) == size) ){
				files.emplace_back( ent.path() );
			}
		}
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイル名を変更
//
// 引数:	fullpath1		変更元のパス
//			fullpath2		変更するパス
// 返値:	bool			true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool OSD_FileRename( const P6VPATH& fullpath1, const P6VPATH& fullpath2 )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileRename] %s -> %s\n", P6VPATH2STR( fullpath1 ).c_str(), P6VPATH2STR( fullpath2 ).c_str() );
	
	try{
		std::filesystem::rename( fullpath1, fullpath2 );
		return true;
	}
	catch( std::filesystem::filesystem_error& ){
		return false;
	}
}

