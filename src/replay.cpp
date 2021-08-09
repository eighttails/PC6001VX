/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
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
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != ST_IDLE ) throw Error::ReplayRecError;
		
		if( !cIni::Read( filepath ) ) throw Error::ReplayRecError;
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
bool REPLAY::ResumeRecord( const P6VPATH& filepath, int frame )
{
	if( !StartRecord( filepath ) ) return false;
	
	// 指定されたフレーム以降のリプレイを削除し、そこから再開
	cIni::DeleteAfter( "REPLAY", Stringf( "%08lX", frame ) );
	
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
	if( RepST != ST_REPLAYREC ) return;
	
	cIni::SetEntry( "REPLAY", "EndFrm", "", "0x%08lX", RepFrm );
	cIni::Write();
	cIni::Init();
	
	RepST = ST_IDLE;
}


/////////////////////////////////////////////////////////////////////////////
// リプレイ1フレーム書出し
//
// 引数:	mt		キーマトリクス
// 			chg		キーマトリクス変化 true:した false:しない
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool REPLAY::ReplayWriteFrame( const std::vector<BYTE>& mt, bool chg )
{
	std::string strva;
	
	if( RepST != ST_REPLAYREC ) return false;
	
	// マトリクスを書出し
	for( auto &i : mt )
		strva += Stringf( "%02X", i );
	cIni::SetEntry( "REPLAY", Stringf( "%08lX ", RepFrm ), "", strva.c_str() );
	
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
	// とりあえずエラー設定
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != ST_IDLE ) throw Error::ReplayPlayError;
		
		if( !cIni::Read( filepath ) ) throw Error::ReplayPlayError;
		if( !cIni::GetVal( "REPLAY", "EndFrm", EndFrm ) ) throw Error::NoReplayData;
	}
	catch( Error::Errno i ){	// 例外発生
		Error::SetError( i );
		cIni::Init();
		return false;
	}
	
	RepFrm = 1;
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
	if( RepST != ST_REPLAYPLAY ) return;
	
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
	std::string strva;
	
	if( RepST != ST_REPLAYPLAY ) return false;
	
	if( cIni::GetEntry( "REPLAY", Stringf( "%08lX", RepFrm ), strva ) ){
		strva.resize( mt.size() * 2, 'F' );
		int i = 0;
		for( auto &m : mt )
			m = std::stoul( strva.substr( i++ * 2, 2 ), nullptr, 16 );
	}
	
	if( ++RepFrm >= EndFrm )
		// データ終端に達したらリプレイ終了
		StopReplay();
	
	return true;
}
