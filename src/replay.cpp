/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "log.h"
#include "replay.h"
#include "common.h"
#include "error.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
REPLAY::REPLAY( void ) : RepST(ST_IDLE), RepFrm(0), EndFrm(0)
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
REPLAY::~REPLAY( void )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	switch( RepST ){
	case ST_REPLAYREC:	StopRecord(); break;
	case ST_REPLAYPLAY:	StopReplay(); break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::Init( void )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	PRINTD( GRP_LOG, "[REPLAY][Init]\n" );
	
	cIni::Init();
	
	RepST   = ST_IDLE;
	RepFrm  = 0;
	EndFrm  = 0;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ステータス取得
//
// 引数:	なし
// 返値:	DWORD		ステータス
/////////////////////////////////////////////////////////////////////////////
DWORD REPLAY::GetStatus( void ) const
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	return RepST;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ記録開始
//
// 引数:	filepath	出力ファイルパス
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::StartRecord( const P6VPATH& filepath )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != ST_IDLE ){
			throw Error::ReplayRecError;
		}
		
		if( !cIni::Read( filepath ) ){
			throw Error::ReplayRecError;
		}
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		cIni::Init();
		return false;
	}
	
	RepFrm = 0;
	RepST  = ST_REPLAYREC;
	
	// 無事だったのでエラーなし
	Error::Clear();
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// リプレイ保存再開
//
// 引数:	filepath	出力ファイルパス
// 引数:	frame       途中再開するフレーム
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::ResumeRecord( const P6VPATH& filepath, DWORD frame )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	if( !StartRecord( filepath ) ){
		return false;
	}
	
	// 指定されたフレーム以降のリプレイを削除し、そこから再開
	cIni::DeleteAfter( "REPLAY", Stringf( "%08ld", frame ) );
	RepFrm = frame;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ記録停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void REPLAY::StopRecord( void )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	if( RepST != ST_REPLAYREC ){
		return;
	}
	
	cIni::SetEntry( "REPLAY", "EndFrm", "", "%ld", RepFrm );
	cIni::Write();
	cIni::Init();
	
	RepST = ST_IDLE;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ1フレーム書出し
//
// 引数:	mt		キーマトリクス
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::ReplayWriteFrame( const std::vector<BYTE>& mt )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	std::string strva;
	
	if( RepST != ST_REPLAYREC ){
		return false;
	}
	
	// 変化があったフレームだけをリプレイに書き出すと
	// キー入力の記録が歯抜けになり、
	// REPLAY::DeleteAfter()が正しく動かなくなり
	// 「途中保存から再開」を実行するとリプレイが破綻する。
	// 残帯対応としてVXでは従来どおり全フレーム書き出すようにする。
	// 本家側で対応されるまでコメントアウトしておく。

//	// キーマトリクスの変化を確認
//	int sz = (int)mt.size() / 2;
//	int i;
//	for( i = 0; i < sz; i++ ) try{
//		if( mt.at( i ) != mt.at( i + sz ) ){
//			break;
//		}
//	}
//	catch( std::out_of_range& ){}
	
//	// 最初のフレームもしくはキーマトリクスに変化があれば書出し
//	if( RepFrm == 0 || i < sz ){
		for( auto &m : mt ){
			strva += Stringf( "%02X", m );
		}
		cIni::SetEntry( "REPLAY", Stringf( "%08ld", RepFrm ), "", strva.c_str() );
//	}
	
	RepFrm++;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ再生開始
//
// 引数:	filepath	入力ファイルパス
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::StartReplay( const P6VPATH& filepath )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != ST_IDLE ){
			throw Error::ReplayPlayError;
		}
		
		if( !cIni::Read( filepath ) ){
			throw Error::ReplayPlayError;
		}
		if( !cIni::GetVal( "REPLAY", "EndFrm", EndFrm ) ){
			throw Error::NoReplayData;
		}
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		cIni::Init();
		return false;
	}
	
	RepFrm = 0;
	RepST  = ST_REPLAYPLAY;
	
	// 無事だったのでエラーなし
	Error::Clear();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ再生停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void REPLAY::StopReplay( void )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	if( RepST != ST_REPLAYPLAY ){
		return;
	}
	
	cIni::Init();
	
	RepST = ST_IDLE;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ1フレーム読込み
//
// 引数:	mt		キーマトリクス
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::ReplayReadFrame( std::vector<BYTE>& mt )
{
	std::lock_guard<cRecursiveMutex> lock( Mutex );
	std::string strva;
	
	if( RepST != ST_REPLAYPLAY ){
		return false;
	}
	
	if( cIni::GetEntry( "REPLAY", Stringf( "%08ld", RepFrm++ ), strva ) ){
		strva.resize( mt.size() * 2, 'F' );
		int i = 0;
		for( auto &m : mt ){
			m = std::stoul( strva.substr( i++ * 2, 2 ), nullptr, 16 );
		}
	}
	
	if( RepFrm >= EndFrm ){
		// データ終端に達したらリプレイ終了
		StopReplay();
	}
	
	return true;
}
