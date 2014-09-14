#include <stdlib.h>
#include <new>

#include "log.h"
#include "replay.h"
#include "common.h"
#include "error.h"
#include "osd.h"

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
REPLAY::REPLAY( void ) : Ini(NULL), RepST(REP_IDLE), Matrix(NULL),
							MSize(0), RepFrm(0), EndFrm(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
REPLAY::~REPLAY( void )
{
	switch( RepST ){
	case REP_RECORD:	StopRecord(); break;
	case REP_REPLAY:	StopReplay(); break;
	}
	if( Matrix ) delete [] Matrix;
	if( Ini ) delete Ini;
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	msize	マトリクスサイズ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::Init( int msize )
{
	PRINTD( GRP_LOG, "[REPLAY][Init]\n" );
	
	if( Ini ) delete Ini;
	Ini = NULL;
	
	if( Matrix ) delete [] Matrix;
	Matrix = new BYTE[msize];
	if( !Matrix ) return false;
	
	RepST   = REP_IDLE;
	MSize   = msize;
	RepFrm  = 0;
	EndFrm  = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ステータス取得
//
// 引数:	なし
// 返値:	int		ステータス
////////////////////////////////////////////////////////////////
int REPLAY::GetStatus( void ) const
{
	return RepST;
}


////////////////////////////////////////////////////////////////
// リプレイ記録開始
//
// 引数:	filename	出力ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::StartRecord( const char *filename )
{
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != REP_IDLE ) throw Error::ReplayRecError;
		
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::ReplayRecError;
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	memset( Matrix, 0xff, MSize );		// キーマトリクスバッファクリア
	
	RepFrm = 0;
	RepST  = REP_RECORD;
	
	// 無事だったのでエラーなし
	Error::Reset();
	
    return true;
}


////////////////////////////////////////////////////////////////
// リプレイ記録再開
//
// 引数:	filename	出力ファイル名
// 引数:	frame       途中再開するフレーム
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::ResumeRecord( const char *filename, int frame )
{
	if( !StartRecord( filename ) ) return false;
	char buf[16];
	sprintf( buf, "%08lX", frame );
	// 指定されたフレーム以降のリプレイを削除し、そこから再開
	Ini->DeleteAfter( "REPLAY", buf );
	
	RepFrm = frame;
	return true;
}


////////////////////////////////////////////////////////////////
// リプレイ記録停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void REPLAY::StopRecord( void )
{
	if( RepST != REP_RECORD ) return;
	
	if( Ini ){
		Ini->PutEntry( "REPLAY", NULL, "EndFrm", "0x%08lX", RepFrm );
		
		Ini->Write();
		
		delete Ini;
		Ini = NULL;
	}
	
	RepST = REP_IDLE;
}


////////////////////////////////////////////////////////////////
// リプレイ1フレーム書出し
//
// 引数:	mt		キーマトリクスポインタ
// 			chg		キーマトリクス変化 true:した false:しない
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::ReplayWriteFrame( const BYTE *mt, bool chg )
{
	char stren[16],strva[256];
	
	if( ( RepST != REP_RECORD ) || !mt || !Ini ) return false;
	
	// マトリクスに変化があったら書出し
//	if( chg ){	// 常に書出すことにする
		sprintf( stren, "%08lX ", RepFrm );
		for( int i=0; i<MSize; i++ )
			sprintf( strva+i*2, "%02X", mt[i] );
		Ini->PutEntry( "REPLAY", NULL, stren, "%s", strva );
//	}
	
	RepFrm++;
	
	return true;
}


////////////////////////////////////////////////////////////////
// リプレイ再生開始
//
// 引数:	filename	入力ファイル名
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::StartReplay( const char *filename )
{
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		int st;	// ホントはDWORD
		
		if( RepST != REP_IDLE ) throw Error::ReplayPlayError;
		
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::ReplayPlayError;
		
		if( !Ini->GetInt( "REPLAY", "EndFrm", &st, EndFrm ) ) throw Error::NoReplayData;
		else                                                  EndFrm = st;
	}
	catch( std::bad_alloc ){	// new に失敗した場合
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	memset( Matrix, 0xff, MSize );	// キーマトリクスバッファクリア
	
	RepFrm = 1;
	RepST  = REP_REPLAY;
	
	// 無事だったのでエラーなし
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// リプレイ再生停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void REPLAY::StopReplay( void )
{
	if( RepST != REP_REPLAY ) return;
	
	if( Ini ){
		delete Ini;
		Ini = NULL;
	}
	
	RepST = REP_IDLE;
}


////////////////////////////////////////////////////////////////
// リプレイ1フレーム読込み
//
// 引数:	mt		キーマトリクスポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool REPLAY::ReplayReadFrame( BYTE *mt )
{
	char stren[16],strva[256];
	
	if( ( RepST != REP_REPLAY ) || !mt || !Ini ) return false;
	
	sprintf( stren, "%08lX", RepFrm );
	if( Ini->GetString( "REPLAY", stren, strva, "" ) ){
		int stl = strlen( strva ) / 2;
		for( int i=0; i<stl; i++ ){
			char dt[3] = "FF";
			strncpy( dt, &strva[i*2], 2 );
			Matrix[i] = strtol( dt, NULL, 16 );
		}
		memcpy( mt, Matrix, MSize );
	}
	
	if( ++RepFrm >= EndFrm ){
		// データ終端に達したらリプレイ終了
		StopReplay();
	}
	
	return true;
}
