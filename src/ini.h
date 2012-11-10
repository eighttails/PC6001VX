#ifndef INI_H_INCLUDED
#define INI_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cNode {
public:
	enum NodeType{ NODE_NONE, NODE_COMMENT, NODE_SECTION, NODE_ENTRY };
	
private:
	cNode *PrevNode, *NextNode;
	
public:
	cNode( cNode * );			// コンストラクタ
	~cNode();					// デストラクタ
	
	cNode *AddNode();			// ノードを追加
	
	void SetMember( NodeType, const char * );
	
	NodeType NodeID;	// Node ID
	char *Comment;		// Comments
	char *Section;		// Sections
	char *Entry;		// Entries
	char *Value;
	
	cNode *GetPrevNode(){ return PrevNode; }
	cNode *GetNextNode(){ return NextNode; }
	void SetPrevNode( cNode *node ){ PrevNode = node; }
	void SetNextNode( cNode *node ){ NextNode = node; }
	
};

class cIni {
protected:
	cNode *IniNode;				// 先頭ノードへのポインタ
	char FileName[PATH_MAX];	// ファイル名
	
	bool Ready;
	
public:
	cIni();						// コンストラクタ
	virtual ~cIni();			// デストラクタ
	
	bool Init( char * );		// 初期化
	
	bool IsReady(){ return Ready; }
	
	bool Write();															// 書込み
	bool GetString( const char *, const char *, char *, const char * );		// 文字列読込み
	bool GetInt( const char *, const char *, int *, const int );			// 数値読込み
	bool GetTruth( const char *, const char *, bool *, const bool );		// YesNo読込み
	bool PutEntry( const char *, const char *, const char *, const char *, ... );	// エントリ追加
	
};



// どこでもSAVE用インターフェイス
struct IDoko
{
	virtual bool DokoLoad( cIni * ) = 0;
	virtual bool DokoSave( cIni * ) = 0;
};

#endif	// INI_H_INCLUDED
