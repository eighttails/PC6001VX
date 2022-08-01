/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <fstream>

#include "ini.h"


#define MAX_LINE	1024
#define TABW		4
#define TABN		4


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cNode::cNode( void ) : NodeID(NODE_NONE), Comment(""), Section(""), Entry(""), Value("")
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cNode::~cNode( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// データをセット(処理前の文字列を渡す)
/////////////////////////////////////////////////////////////////////////////
void cNode::SetMember( NodeType id, const std::string& sstr )
{
	std::string	str, val, com;
	size_t len;
	
	str = sstr;
	
	// 先頭の空白,改行を削除
	len = str.find_first_not_of( " \t\n" );
	if( len != std::string::npos )
		str.erase( str.begin(), str.begin() + len );
	
	// 末尾の空白,改行を削除
	len = str.find_last_not_of( " \t\n" );
	if( len != std::string::npos )
		str.erase( str.begin() + len + 1, str.end() );
	
	
	NodeID = id;
	switch( NodeID ){
	case NODE_COMMENT:	// Comment
		// ';'の次の文字がコメントの先頭
		if( str.front() == ';' )
			str.erase( str.begin() );
		
		Comment = str;
		break;
		
	case NODE_SECTION:	// Section
		// '['の次の文字が先頭
		if( str.front() == '[' )
			str.erase( str.begin() );
		
		// ']'のところを末尾にする
		len = str.find_first_of( "] \t\n" );
		if( len != std::string::npos )
			str.erase( str.begin() + len, str.end() );
		
		Section = str;
		break;
		
	case NODE_ENTRY:	// Entry
		// 値 ---
		// '='がなければ戻る
		len = str.find_first_of( "=" );
		if( len == std::string::npos ) break;
		
		// '='の次の文字が値の先頭
		// 先頭から'='まで削除
		val = str;
		val.erase( val.begin(), val.begin() + len + 1 );
		
		// 値の前の' 'と'\t'を削除
		len = val.find_first_not_of( " \t" );
		if( len != std::string::npos )
			val.erase( val.begin(), val.begin() + len );
		
		// 最後の'\"'より後に';'があれば それはコメント
		com = val;
		len = com.find_last_of( "\"" );
		if( len != std::string::npos )
			com.erase( com.begin(), com.begin() + len );
		len = com.find_first_of( ";" );
		if( len != std::string::npos )
			com.erase( com.begin(), com.begin() + len + 1 );
		else
			com.clear();
		
		if( !com.empty() ){
			Comment = com;
			// 値からコメント部分を削除
			val.erase( val.begin() + val.rfind( ';' + com ), val.end() );
			// 値の後の' 'と'\t'を削除
			len = val.find_last_not_of( " \t" );
			if( len != std::string::npos )
				val.erase( val.begin() + len + 1, val.end() );
		}
		
		// 値の先頭の'\"'を削除
		if( val.front() == '\"' )
			val.erase( val.front() );
		
		// 値の末尾の'\"'を削除
		if( val.back() == '\"' )
			val.erase( val.back() );
		
		Value = val;
		
		// エントリ ---
		// '='の前の文字までがエントリ
		len = str.find_first_of( "=" );
		str.erase( str.begin() + len, str.end() );
		
		// エントリの後の' 'と'\t'を削除
		len = str.find_last_not_of( " \t" );
		if( len != std::string::npos )
			str.erase( str.begin() + len + 1, str.end() );
		
		Entry = str;
		break;
		
	case NODE_NONE:
	default:
		NodeID = NODE_NONE;
		
	}
}


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cIni::cIni( void ) : IniPath("")
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cIni::~cIni( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// ノード検索
/////////////////////////////////////////////////////////////////////////////
std::list<cNode>::iterator cIni::FindNode( const std::string& section, const std::string& entry )
{
	if( IniNode.empty() ) return IniNode.end();
	
	// セクションを探す
	auto node = std::find_if( IniNode.begin(), IniNode.end(), [&]( cNode& n ){
				return( n.NodeID == cNode::NODE_SECTION && n.Section == section );
			} );
	// セクションが見つからなければエラー
	if( node == IniNode.end() ) return node;
	
	
	// エントリを探す
	node = std::find_if( ++node, IniNode.end(), [&]( cNode& n ){
				return( (n.NodeID == cNode::NODE_ENTRY && n.Entry == entry) || n.NodeID == cNode::NODE_SECTION );
			} );
	// セクション内にエントリが見つからなければエラー
	if( node == IniNode.end() || node->NodeID == cNode::NODE_SECTION ) return IniNode.end();
	
	return node;
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
void cIni::Init()
{
	IniPath.clear();
	IniNode.clear();
}


/////////////////////////////////////////////////////////////////////////////
// INIファイル読込み
/////////////////////////////////////////////////////////////////////////////
bool cIni::Read( const P6VPATH& path )
{
	std::fstream fs;
	std::string	str;
	size_t len;
	
	// INIファイルを開く
	if( !OSD_FSopen( fs, path, std::ios_base::in ) ){
		IniPath.clear();
		return false;
	}
	
	// ファイル名を保存
	IniPath = path;
	
	// データが無くなるまで繰り返し
	// 最初の1行読込む
	std::getline( fs, str );
	while( !fs.eof() ){
		// 先頭の空白,改行を削除
		len = str.find_first_not_of( " \t\n" );
		if( len != std::string::npos )
			str.erase( str.begin(), str.begin() + len );
		
		// 末尾の空白,改行を削除
		len = str.find_last_not_of( " \t\n" );
		if( len != std::string::npos )
			str.erase( str.begin() + len + 1, str.end() );
		
		if( !str.empty() ){	// 空行は読み飛ばす
			// ノード追加
			IniNode.emplace_back();
			auto node = --IniNode.end();
			
			// 先頭が';'だったらコメント行
			if( str.front() == ';' )
				node->SetMember( cNode::NODE_COMMENT, str );
			else
			// 先頭が'['だったらセクション
			if( str.front() == '[' )
				node->SetMember( cNode::NODE_SECTION, str );
			else
			// '='を含むならエントリ
			if( str.find( '=' ) != std::string::npos )
				node->SetMember( cNode::NODE_ENTRY, str );
			else
			// どれでもなければコメント
				node->SetMember( cNode::NODE_COMMENT, str );
		}
		
		// 次の1行読込む
		std::getline( fs, str );
	}
	
	fs.close();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// INIファイル書込み
/////////////////////////////////////////////////////////////////////////////
bool cIni::Write( void )
{
	std::fstream fs;
	
	// INIファイルを開く
	if( !OSD_FSopen( fs, IniPath, std::ios_base::out ) )
		return false;
	
	for( auto &node : IniNode ){
		switch( node.NodeID ){
		case cNode::NODE_COMMENT:	// Comment
			if( !node.Comment.empty() )
				fs << ";" << node.Comment;
			fs << std::endl;
			break;
			
		case cNode::NODE_SECTION:	// Section
			fs << std::endl << "[" << node.Section << "]" << std::endl;
			break;
			
		case cNode::NODE_ENTRY:	// Entry
			fs << node.Entry;
			for( int i = TABN * TABW - (int)node.Entry.length(); i > 0; i -= TABW )
				fs << "\t";
			fs << "= " << node.Value;
			
			if( !node.Comment.empty() ){
				for( int i = TABN * TABW - ((int)node.Value.length() + 2); i > 0; i -= TABW )
					fs << "\t";
				fs << "\t;" << node.Comment;
			}
			fs << std::endl;
			break;
			
		case cNode::NODE_NONE:
		default:
			break;
		}
	}
	
	fs.close();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// エントリ読込み(文字列)
/////////////////////////////////////////////////////////////////////////////
bool cIni::GetEntry( const std::string& section, const std::string& entry, std::string& val )
{
	// エントリを探す
	auto node = FindNode( section, entry );
	if( node == IniNode.end() ){
		return false;
	}
	
	// 値を保存
	val = node->Value;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// エントリ書込み(文字列)
/////////////////////////////////////////////////////////////////////////////
bool cIni::SetEntry( const std::string& section, const std::string& entry, const std::string& comment, const std::string& text, ... )
{
	char rstr[MAX_LINE+1];
	std::string str;
	std::va_list arg;
	
	// C的可変長引数展開
	va_start( arg, text );
	std::vsnprintf( rstr, sizeof(rstr), text.c_str(), arg );
	va_end( arg );
	
	// セクションを探す
	auto node = std::find_if( IniNode.begin(), IniNode.end(), [&]( cNode& n ){
				return( n.NodeID == cNode::NODE_SECTION && n.Section == section );
			} );
	// セクションが見つからなければ追加
	if( node == IniNode.end() ){
		// セクションノード追加
		IniNode.emplace_back();
		node = --IniNode.end();
		node->SetMember( cNode::NODE_SECTION, section);
		// エントリノード追加
		IniNode.emplace_back();
		node = --IniNode.end();
	}else{
	// セクションが見つかった場合
		// エントリを探す
		node = std::find_if( ++node, IniNode.end(), [&]( cNode& n ){
					return( (n.NodeID == cNode::NODE_ENTRY && n.Entry == entry) || n.NodeID == cNode::NODE_SECTION );
				} );
		// エントリが見つからなければ追加
		if( node == IniNode.end() || node->NodeID == cNode::NODE_SECTION ){
			node = IniNode.emplace( node );
		}
	}
	
	// エントリにコメントありなら追加
	if( !comment.empty() ) node->SetMember( cNode::NODE_COMMENT, comment );
	
	// エントリを保存
	node->SetMember( cNode::NODE_ENTRY, entry + '=' + rstr );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// エントリ読込み
/////////////////////////////////////////////////////////////////////////////
// YesNo(テンプレート特殊化)
template <> bool cIni::GetVal<bool>( const std::string& section, const std::string& entry, bool& yn )
{
	std::string str;
	
	// エントリを探す
	if( !GetEntry( section, entry, str ) ){
		return false;
	}
	
	if( !StriCmp( str, "1" ) || !StriCmp( str, "yes" ) || !StriCmp( str, "on" ) || !StriCmp( str, "true" ) ){
		yn = true;
	}else
	if( !StriCmp( str, "0" ) || !StriCmp( str, "no" ) || !StriCmp( str, "off" ) || !StriCmp( str, "false" ) ){
		yn = false;
	}
	
	return yn;
}

// Path(テンプレート特殊化)
template <> bool cIni::GetVal<P6VPATH>( const std::string& section, const std::string& entry, P6VPATH& path )
{
	std::string tval = P6VPATH2STR( path );
	
	// エントリを探す
	if( !GetEntry( section, entry, tval ) ) return false;
	
	path = STR2P6VPATH( tval );
	OSD_AbsolutePath( path );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// エントリ書込み
/////////////////////////////////////////////////////////////////////////////
// YesNo(テンプレート特殊化)
template <> bool cIni::SetVal<bool>( const std::string& section, const std::string& entry, const std::string& comment, const bool& yn )
{
	return SetEntry( section, entry, comment, yn ? "Yes" : "No" );
}

// Path(テンプレート特殊化)
template <> bool cIni::SetVal<P6VPATH>( const std::string& section, const std::string& entry, const std::string& comment, const P6VPATH& path )
{
	return SetEntry( section, entry, comment, P6VPATH2STR( path ).c_str() );
}


/////////////////////////////////////////////////////////////////////////////
// エントリ削除(前)
/////////////////////////////////////////////////////////////////////////////
bool cIni::DeleteBefore( const std::string& section, const std::string& entry )
{
	// エントリを探す
	auto node = FindNode( section, entry );
	if( node == IniNode.end() ) return false;
	
	// エントリが見つかった場合はそれより前を削除(指定されたエントリを含む)
	IniNode.erase( IniNode.begin(), node );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// エントリ削除(後)
/////////////////////////////////////////////////////////////////////////////
bool cIni::DeleteAfter( const std::string& section, const std::string& entry )
{
	// エントリを探す
	auto node = FindNode( section, entry );
	if( node == IniNode.end() ) return false;
	
	// エントリが見つかった場合はそれより後を削除(指定されたエントリを含む)
	IniNode.erase( node, IniNode.end() );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルパス取得
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& cIni::GetFilePath( void  ) const
{
	return IniPath;
}
