#include <stdlib.h>
#include <stdarg.h>

#include "ini.h"
#include "common.h"
#include "osd.h"

#define MAX_LINE	256
#define TABW		4
#define TABN		4


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cNode::cNode( cNode *pn ) : PrevNode(pn), NodeID(NODE_NONE),
							Comment(NULL), Section(NULL), Entry(NULL), Value(NULL)
{
	if( PrevNode ){
		NextNode = PrevNode->GetNextNode();
		PrevNode->SetNextNode( this );
	}else
		NextNode = NULL;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cNode::~cNode( void )
{
	if( Comment ) delete [] Comment;
	if( Section ) delete [] Section;
	if( Entry   ) delete [] Entry;
	if( Value   ) delete [] Value;
	
	// つながっているノードがあれば削除
	if( NextNode ) delete NextNode;
}


////////////////////////////////////////////////////////////////
// ノードを追加
////////////////////////////////////////////////////////////////
cNode *cNode::AddNode( void )
{
	return new cNode( this );
}


////////////////////////////////////////////////////////////////
// データをセット(処理前の文字列を渡す)
////////////////////////////////////////////////////////////////
void cNode::SetMember( NodeType id, const char *str )
{
	char tstr[MAX_LINE+1];
	char *tpstr, *st;
	
	strncpy( tstr, str, MAX_LINE );
	
	NodeID = id;
	switch( NodeID ){
	case NODE_COMMENT:	// Comment
		// ';'の次の文字がコメントの先頭
//		tpstr = strchr( tstr, ';' );
//		if( !tpstr ) tpstr = tstr;
//		else         tpstr++;
		
		// 末尾の改行を削除
		while( str[strlen(tstr)-1] == '\n' )
			tstr[strlen(tstr)-1] = '\0';
		
		if( Comment ) delete [] Comment;
		Comment = new char[strlen(tstr)+1];
		strcpy( Comment, tstr );
		break;
		
	case NODE_SECTION:	// Section
		// ']'のところを末尾にする
		if( strchr( tstr, ']' ) ) *strchr( tstr, ']' ) = '\0';
		// '['の次の文字が先頭
		if( strchr( tstr, '[' ) ) tpstr = strchr( tstr, '[' ) + 1;
		else                      tpstr = tstr;
		if( Section ) delete [] Section;
		Section = new char[strlen( tpstr ) + 1];
		strcpy( Section, tpstr );
		break;
		
	case NODE_ENTRY:	// Entry
		// 値 ---
		// '='の次の文字が値の先頭
		tpstr = strchr( tstr, '=' ) + 1;
		
		// 最後が改行なら'\0'に置き換える
		if( tpstr[strlen(tpstr)-1] == '\n' )
			tpstr[strlen(tpstr)-1] = '\0';
		// 値の前の' 'と'\t'を無視
		while( *tpstr == ' ' || *tpstr == '\t')
			tpstr++;
		
		// 最後の'\"'より後に';'があれば それはコメント
		if( strrchr( tpstr, '\"' ) ) st = strchr( strrchr( tpstr, '\"' ), ';' );
		else                         st = strchr( tpstr, ';' );
		if( st ){
			this->SetMember( NODE_COMMENT, st+1 );
			NodeID = NODE_ENTRY;
			*st = '\0';
		}
		
		// 値の後の' 'と'\t'を無視
		while( tpstr[strlen(tpstr)-1] == ' ' || tpstr[strlen(tpstr)-1] == '\t' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		// 頭の'\"'を飛ばす
		if( *tpstr == '\"' ) tpstr++;
		// 後の'\"'を末尾にする
		if( tpstr[strlen(tpstr)-1] == '\"' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		if( Value ) delete [] Value;
		Value = new char[strlen( tpstr ) + 1];
		strcpy( Value, tpstr );
		
		// エントリ ---
		// '='のところを末尾にする
		*strchr( tstr, '=' ) = '\0';
		tpstr = tstr;
		// エントリ前の' 'と'\t'を無視
		while( *tpstr == ' ' || *tpstr == '\t')
			tpstr++;
		// エントリ後の' 'と'\t'を無視
		while( tpstr[strlen(tpstr)-1] == ' ' || tpstr[strlen(tpstr)-1] == '\t' )
			tpstr[strlen(tpstr)-1] = '\0';
		
		if( Entry ) delete [] Entry;
		Entry = new char[strlen( tpstr ) + 1];
		strcpy( Entry, tpstr );
		break;
		
	case NODE_NONE:
	default:
		NodeID = NODE_NONE;
		
	}
	
}


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cIni::cIni( void )
{
	Ready   = false;
	IniNode = NULL;
	
	*FileName = '\0';
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cIni::~cIni( void )
{
	if( IniNode ) delete IniNode;
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
bool cIni::Init( const char *filename )
{
	char str[MAX_LINE+1];
	FILE *fp;
	cNode *node;
	
	// ファイル名を保存
	strncpy( FileName, filename, PATH_MAX-1 );
	
	// 最初のノード確保(ダミー)
	if( IniNode ) delete IniNode;
	IniNode = node = new cNode( NULL );
	if( !IniNode ) return false;
	
	// INIファイルを開く
	fp = FOPENEN( FileName, "rt" );
	if( !fp ) return false;
	
	// 先頭から読む
	fseek( fp, 0, SEEK_SET );
	// データが無くなるまで繰り返し
	while( fgets( str, MAX_LINE, fp ) ){
		// ノード確保
		node = node->AddNode();
		if( !node ){
			fclose( fp );
			return false;
		}
		
		// 先頭が';'だったらコメント行
		if( *str == ';' )
			node->SetMember( cNode::NODE_COMMENT, str+1 );
		else
		// '['と']'を含み'='を含まないならセクション
		if( strchr( str, '[' ) && strchr( str, ']' ) && !strchr( str, '=' ) )
			node->SetMember( cNode::NODE_SECTION, str );
		else
		// '='を含むならエントリ
		if( strchr( str, '=' ) )
			node->SetMember( cNode::NODE_ENTRY, str );
		else
		// どれでもなければコメント
			node->SetMember( cNode::NODE_COMMENT, str );
	}
	fclose( fp );
	
	Ready = true;
	
	return true;
}


////////////////////////////////////////////////////////////////
// 書込み
////////////////////////////////////////////////////////////////
bool cIni::Write( void )
{
	FILE *fp;
	cNode *node;
	
	if( !Ready ) return false;
	
	// INIファイルを開く
	fp = FOPENEN( FileName, "wt" );
	if( !fp ) return false;
	
	node = IniNode;
	do{
		switch( node->NodeID ){
		case cNode::NODE_COMMENT:	// Comment
            if( strlen(node->Comment) ) fprintf( fp, ";%s\n", TRANS(node->Comment) );
			else                        fprintf( fp, "\n" );
			break;
			
		case cNode::NODE_SECTION:	// Section
			fprintf( fp, "[%s]\n", node->Section );
			break;
			
		case cNode::NODE_ENTRY:	// Entry
			fprintf( fp, "%s", node->Entry );
			for( int i=TABN*TABW-(int)strlen(node->Entry); i>0; i-=TABW )
				fprintf( fp, "\t" );
				
			fprintf( fp, "= %s", node->Value );
				
			if( node->Comment ){
				for( int i=TABN*TABW-((int)strlen(node->Value)+2); i>0; i-=TABW )
					fprintf( fp, "\t" );
                fprintf( fp, "\t;%s\n", TRANS(node->Comment) );
			}else
				fprintf( fp, "\n" );
			
			break;
			
		case cNode::NODE_NONE:
		default:
			break;
		}
	}while( (node = node->GetNextNode()) );
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// 文字列読込み
////////////////////////////////////////////////////////////////
bool cIni::GetString( const char *section, const char *entry, char *val, const char *def )
{
	cNode *node, *tnode;
	bool Found  = false;
	
	// まずはデフォルト値をセット
	strcpy( val, def );
	
	if( !Ready ) return false;
	
	// セクションを探す
	node = IniNode;
	do{
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode()) && !Found );
	// セクションが見つからないかエントリがなければエラー
	if( !Found || !node ) return false;
	Found = false;
	
	// エントリを探す
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_ENTRY )
			if( !stricmp( node->Entry, entry ) ) Found = true;
	}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
	// エントリが見つからなければエラー
	if( !Found ) return false;
	
	// 値を保存
	strcpy( val, tnode->Value );
	
	return true;
}


////////////////////////////////////////////////////////////////
// 数値読込み
////////////////////////////////////////////////////////////////
bool cIni::GetInt( const char *section, const char *entry, int *val, const int def )
{
	bool res;
	char str[MAX_LINE+1];
	
	// まずはデフォルト値をセット
	*val = def;
	
	if( (res = GetString( section, entry, str, "" )) )
		*val = strtol( str, NULL, 0 );
	
	return res;
}


////////////////////////////////////////////////////////////////
// YesNo読込み
////////////////////////////////////////////////////////////////
bool cIni::GetTruth( const char *section, const char *entry, bool *val, const bool def )
{
	bool res;
	char str[MAX_LINE+1];
	
	// まずはデフォルト値をセット
	*val = def;
	
	if( (res = GetString( section, entry, str, "" )) ){
		if( !strcmp( str, "1" ) || !stricmp( str, "yes" ) || !stricmp( str, "on" ) || !stricmp( str, "true" ) )
			*val = true;
		else
		if( !strcmp( str, "0" ) || !stricmp( str, "no" ) || !stricmp( str, "off" ) || !stricmp( str, "false" ) )
			*val = false;
		else
			return false;
	}
	return res;
}


////////////////////////////////////////////////////////////////
// エントリ追加
////////////////////////////////////////////////////////////////
bool cIni::PutEntry( const char *section, const char *comment, const char *entry, const char *val, ... )
{
	char rstr[MAX_LINE+1];
	cNode *node, *tnode;
	bool Found = false;
	
	if( !Ready ) return false;
	
	// 引数処理
	va_list arg;
	va_start( arg, val );
	vsprintf( rstr, val, arg );
	va_end( arg );
	
	
	// セクションを探す
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );
	
	// セクションが見つからなければ追加
	if( !Found ){
		// セクションノード追加
		node = tnode->AddNode();
		if( !node ) return false;
		node->SetMember( cNode::NODE_SECTION, section );
		// エントリノード追加
		node = node->AddNode();
		if( !node ) return false;
	}else{
	// セクションが見つかった場合
		Found = false;
		
		// エントリを探す
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;
		// エントリが見つからなければ追加
		if( !Found ){
			// 次のセクションに達した場合は前セクションの最後のノードを選択
			if( tnode->NodeID == cNode::NODE_SECTION ) node = tnode->GetPrevNode();
			
			// エントリノード追加
			node = node->AddNode();
			if( !node ) return false;
		}
	}
	
	// エントリにコメントありなら追加
    if( comment ) node->SetMember( cNode::NODE_COMMENT, comment );
	
	// エントリを保存
	char tstr[MAX_LINE+1];
	sprintf( tstr, "%s=%s", entry, rstr );
	node->SetMember( cNode::NODE_ENTRY, tstr );
	
    return true;
}

////////////////////////////////////////////////////////////////
// エントリ削除
////////////////////////////////////////////////////////////////
bool cIni::DeleteBefore(const char *section, const char *entry)
{
	cNode *node, *tnode;
	bool Found = false;

	if( !Ready ) return false;

	// セクションを探す
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );

	// セクションが見つからなければ何もしない
	if( !Found ){
		return false;
	} else {
		// セクションが見つかった場合
		Found = false;

		// エントリを探す
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;

		// エントリが見つからなければ何もしない
		if( !Found ){
			return false;
		} else {
			// エントリが見つかった場合はそれより前を削除(指定されたエントリを含む)
			tnode = node;
			do{
				tnode = tnode->GetPrevNode();
			}while( tnode && tnode->NodeID != cNode::NODE_SECTION );
			tnode->GetPrevNode()->SetNextNode(node->GetNextNode());
			node->GetPrevNode()->SetNextNode(NULL);
			delete tnode;
		}
	}
}

bool cIni::DeleteAfter(const char *section, const char *entry)
{
	cNode *node, *tnode;
	bool Found = false;

	if( !Ready ) return false;

	// セクションを探す
	node = IniNode;
	do{
		tnode = node;
		if( node->NodeID == cNode::NODE_SECTION )
			if( !stricmp( node->Section, section ) ) Found = true;
	}while( (node = node->GetNextNode() ) && !Found );

	// セクションが見つからなければ何もしない
	if( !Found ){
		return false;
	} else {
		// セクションが見つかった場合
		Found = false;

		// エントリを探す
		do{
			tnode = node;
			if( node->NodeID == cNode::NODE_ENTRY )
				if( !stricmp( node->Entry, entry ) ) Found = true;
		}while( (node = node->GetNextNode()) && tnode->NodeID != cNode::NODE_SECTION && !Found  );
		node = tnode;

		// エントリが見つからなければ何もしない
		if( !Found ){
			return false;
		} else {
			// エントリが見つかった場合はそれより後を削除(指定されたエントリを含む)
			tnode = node;
			do{
				tnode = tnode->GetNextNode();
			}while( tnode && tnode->NodeID != cNode::NODE_SECTION );
			if(tnode){
				tnode->GetPrevNode()->SetNextNode(NULL);
			}
			node->GetPrevNode()->SetNextNode(tnode);
			delete node;
		}
	}
}
