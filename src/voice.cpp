/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "pc6001v.h"

#include "common.h"
#include "intr.h"
#include "log.h"
#include "osd.h"
#include "p6el.h"
#include "p6vm.h"
#include "schedule.h"
#include "voice.h"


// イベントID
#define	EID_FRAME	(1)



/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
VCE6::VCE6( VM6* vm, const ID& id ) : Device( vm, id ),
	FilePath( DIR_WAVE ), io_E0H( 0 ), io_E2H( 0 ), io_E3H( 0 ), VStat( D7752E_IDL ),
	Pnum( 0 ), Fnum( 0 ), PReady( false )
{
	INITARRAY( ParaBuf, 0 );
	SndDev::Volume = DEFAULT_VOICEVOL;
}

VCE60::VCE60( VM6* vm, const ID& id ) : VCE6( vm, id )
{
}

VCE62::VCE62( VM6* vm, const ID& id ) : VCE6( vm, id )
{
	// Device Description (Out)
	descs.outdef.emplace( outE0H, STATIC_CAST( Device::OutFuncPtr, &VCE62::OutE0H ) );
	descs.outdef.emplace( outE2H, STATIC_CAST( Device::OutFuncPtr, &VCE62::OutE2H ) );
	descs.outdef.emplace( outE3H, STATIC_CAST( Device::OutFuncPtr, &VCE62::OutE3H ) );
	
	// Device Description (In)
	descs.indef.emplace ( inE0H,  STATIC_CAST( Device::InFuncPtr,  &VCE62::InE0H  ) );
	descs.indef.emplace ( inE2H,  STATIC_CAST( Device::InFuncPtr,  &VCE62::InE2H  ) );
	descs.indef.emplace ( inE3H,  STATIC_CAST( Device::InFuncPtr,  &VCE62::InE3H  ) );
}

VCE64::VCE64( VM6* vm, const ID& id ) : VCE62( vm, id )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
VCE6::~VCE6()
{
}

VCE60::~VCE60()
{
}

VCE62::~VCE62()
{
}

VCE64::~VCE64()
{
}


/////////////////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VCE6::EventCallback( int id, int clock )
{
	PRINTD( VOI_LOG, "[VOICE][EventCallback] -> " );
	
	switch( id ){
	case EID_FRAME:	// 1フレーム
		if( VStat & D7752E_EXT ){	// 外部句発声処理
			PRINTD( VOI_LOG, "(EXT)\n" );
			if( PReady ){	// パラメータセット完了してる?
				// フレーム数=0ならば終了処理
				if( !(ParaBuf[0] >> 3) ){
					AbortVoice();
				}else{
					// 1フレーム分のサンプル生成
					cD7752::Synth( ParaBuf, Fbuf );
					
					// サンプリングレートを変換してバッファに書込む
					UpConvert();
					
					// 次フレームのパラメータを受け付ける
					PReady = false;
					VStat |= D7752E_REQ;
					ReqIntr();		// 音声合成割込み
				}
			}else{			// 完了してなければエラー
				AbortVoice();		// 発声停止
				VStat = D7752E_ERR;	// ステータスをエラー発生にセット
			}
		}else{						// 内部句発声処理
			PRINTD( VOI_LOG, "(INT) " );
			// 1フレーム分のサンプルをバッファに書込む
			int num = min( IVBuf.size(), cD7752::GetFrameSize() * SndDev::SampleRate / 10000 );
			PRINTD( VOI_LOG, "%d/%lld\n", num, IVBuf.size() );
			while( num-- ){
				SndDev::cRing::Put( IVBuf.front() );
				IVBuf.pop();
			}
			if( IVBuf.empty() ){	// 最後まで発生したら発声終了
				AbortVoice();
			}
		}
		
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// モードセット
/////////////////////////////////////////////////////////////////////////////
void VCE6::VSetMode( BYTE mode )
{
	PRINTD( VOI_LOG, "[VOICE][VSetMode]    %02X %dms", mode, (((mode >> 2) & 1) + 1) * 10 );
	PRINTD( VOI_LOG, " %s\n", mode & 1 && mode & 2 ? "X" : mode & 1 ? "SLOW" : mode & 2 ? "FAST" : "NORMAL" );
	
	// 音声合成開始
	cD7752::Start( mode );
	
	// ステータスクリア
	VStat = D7752E_IDL;
}


/////////////////////////////////////////////////////////////////////////////
// コマンドセット
/////////////////////////////////////////////////////////////////////////////
void VCE6::VSetCommand( BYTE comm )
{
	PRINTD( VOI_LOG, "[VOICE][VSetCommand] %02X\n", comm );
	
	// 発声中なら止める
	AbortVoice();
	
	switch( comm ){
	case 0x00:		// 内部句選択コマンド ----------
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
		// WAVファイルを読込む
		if( !LoadVoice( comm ) ){
			break;
		}
		
		// ステータスを内部句再生モードにセット
		VStat = D7752E_BSY;
		
		// フレームイベントをセットする
		vm->EventAdd( Device::GetID(), EID_FRAME, 10000.0 / (double)cD7752::GetFrameSize(), EV_LOOP | EV_HZ );
		
		break;
		
	case 0xfe:		// 外部句選択コマンド ----------
		// フレームバッファ初期化
		std::queue<D7752_SAMPLE>().swap( Fbuf );
		
		// フレームイベントをセットする
		vm->EventAdd( Device::GetID(), EID_FRAME, 10000.0 / (double)cD7752::GetFrameSize(), EV_LOOP | EV_HZ );
		
		// ステータスを外部句再生モードにセット，パラメータ受付開始
		VStat = D7752E_BSY | D7752E_EXT | D7752E_REQ;
		
		ReqIntr();		// 音声合成割込み
		break;
		
	case 0xff:		// ストップコマンド ----------
		break;
		
	default:		// 無効コマンド  ----------
		VStat = D7752E_ERR;	// ホント?
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// 音声パラメータ転送
/////////////////////////////////////////////////////////////////////////////
void VCE6::VSetData( BYTE data )
{
	PRINTD( VOI_LOG, "[VOICE][VSetData]    %02X\n", data );
	
	// 再生時のみデータを受け付ける
	if(	(VStat & D7752E_BSY) && (VStat & D7752E_REQ) ){
		if( Fnum == 0 || Pnum ){	// 最初のフレーム?
			// 最初のパラメータだったら繰り返し数を設定
			if( Pnum == 0 ){ Fnum = data >> 3; }
			// パラメータバッファに保存
			ParaBuf[Pnum++] = data;
			// もし1フレーム分のパラメータが溜まったら発声準備完了
			if( Pnum == 7 ){
				VStat &= ~D7752E_REQ;
				Pnum = 0;
				if( Fnum > 0 ){ Fnum--; }
				PReady = true;
			}else
				ReqIntr();		// 音声合成割込み
		}else{						// 繰り返しフレーム?
			// パラメータバッファに保存
			// PD7752の仕様に合わせる
			for( int i = 1; i < 6; i++ ) ParaBuf[i] = 0;
			ParaBuf[6] = data;
			VStat &= ~D7752E_REQ;
			Pnum = 0;
			Fnum--;
			PReady = true;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// ステータスレジスタ取得
/////////////////////////////////////////////////////////////////////////////
int VCE6::VGetStatus( void )
{
	return VStat;
}


/////////////////////////////////////////////////////////////////////////////
// 発声停止
/////////////////////////////////////////////////////////////////////////////
void VCE6::AbortVoice( void )
{
	PRINTD( VOI_LOG, "[VOICE][AbortVoice]\n" );
	
	// フレームイベント停止
	vm->EventDel( Device::GetID(), EID_FRAME );
	
	// 残りのパラメータはキャンセル
	Pnum = Fnum = 0;
	PReady = false;
	
	VStat &= ~D7752E_BSY;
}


/////////////////////////////////////////////////////////////////////////////
// サンプリングレート変換
/////////////////////////////////////////////////////////////////////////////
void VCE6::UpConvert( void )
{
	PRINTD( VOI_LOG, "[VOICE][UpConvert]\n" );
	
	// 10kHz -> 実際に再生するサンプリングレートに変換した時のサンプル数
	int samples = cD7752::GetFrameSize() * SndDev::SampleRate / 10000;
	PRINTD( VOI_LOG, "UpConvert %d \n", samples );
	
	int pos = -1;
	int dat = 0;
	for( int i = 0; i < samples; i++ ){
		int npos;
		if( (npos = i * cD7752::GetFrameSize() / samples) != pos ){
			pos = npos;
			dat = Fbuf.front() * 4;	// * 4 は 16bit<-14bit のため
			Fbuf.pop();
		}
		SndDev::cRing::Put( dat * 2 );		// 出力レベルが低いのでとりあえず2倍
	}
	
	PRINTD( VOI_LOG, "\n" );
}


/////////////////////////////////////////////////////////////////////////////
// 内部句WAV読込み
/////////////////////////////////////////////////////////////////////////////
bool VCE6::LoadVoice( int index )
{
	PRINTD( VOI_LOG, "[VOICE][LoadVoice]  No.%d -> ", index );
	
	DWORD len;		// 元データサイズ
	BYTE* buf;		// 元データバッファ
	int freq;		// 元データサンプリングレート
	
	// WAVファイルを読込む
	P6VPATH tpath;
	OSD_AddPath( tpath, FilePath, STR2P6VPATH( Stringf( "f4%d.wav", index ) ) );
	
	PRINTD( VOI_LOG, "%s ->", P6VPATH2STR( tpath ).c_str() );
	
	if( !OSD_LoadWAV( tpath, &buf, &len, &freq ) ){
		PRINTD( VOI_LOG, "Error!\n" );
		return false;
	}
	
	PRINTD( VOI_LOG, "OK\n" );
	
	// 発声速度変換後のサイズを計算してバッファを確保
	// 発声速度4の時,1フレームのサンプル数は160
	int IVLen = (int)( (double)SndDev::SampleRate * (double)(len / 2) / (double)freq
					* (double)cD7752::GetFrameSize() / (double)160 );
	
	PRINTD( VOI_LOG, "Len:%d/%d ->", IVLen, (int)len );
	
	FreeVoice();
	
	// 発声速度変換
	// 単なる間引きなのでピッチが変わってしまうのが問題
	// FFTを使うか?
	for( int i = 0; i < IVLen; i++ )
		IVBuf.emplace( ((signed short*)buf)[(int)(( (double)i * (double)(len / 2) ) / (double)IVLen)] );
	
	// WAV開放
	OSD_FreeWAV( buf );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 内部句WAV開放
/////////////////////////////////////////////////////////////////////////////
void VCE6::FreeVoice( void )
{
	// バッファ初期化
	std::queue<int32_t>().swap( IVBuf );
}


/////////////////////////////////////////////////////////////////////////////
// 音声合成割込み要求
/////////////////////////////////////////////////////////////////////////////
void VCE6::ReqIntr( void )
{
}

void VCE64::ReqIntr( void )
{
	vm->IntReqIntr( IREQ_VOICE );
}


/////////////////////////////////////////////////////////////////////////////
// リセット
/////////////////////////////////////////////////////////////////////////////
void VCE6::Reset( void )
{
	AbortVoice();
	VStat = D7752E_IDL;
}


/////////////////////////////////////////////////////////////////////////////
// WAVEファイル格納パス設定
//
// 引数:	path	WAVEファイル格納パスへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void VCE6::SetPath( const P6VPATH& path )
{
	FilePath = path;
}


/////////////////////////////////////////////////////////////////////////////
// ストリーム更新
//
// 引数:	samples	更新するサンプル数(0:処理クロック分)
// 返値:	int		更新したサンプル数
/////////////////////////////////////////////////////////////////////////////
int VCE6::SoundUpdate( int samples )
{
	int length = max( 0, samples - SndDev::cRing::ReadySize() );
	
	PRINTD( VOI_LOG, "[VOICE][SoundUpdate] Samples: %d -> %d\n", samples, length );
	
	for( int i = 0; i < length; i++ ){
		// バッファに書込み
		SndDev::cRing::Put( 0 );	// 手抜き
	}
	
	return length;
}


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
void VCE62::OutE0H( int, BYTE data ){ VSetData( data ); }
void VCE62::OutE2H( int, BYTE data ){ VSetMode( data ); }
void VCE62::OutE3H( int, BYTE data ){ VSetCommand( data ); }
BYTE VCE62::InE0H( int ){ return VGetStatus(); }
BYTE VCE62::InE2H( int ){ return io_E2H; }
BYTE VCE62::InE3H( int ){ return io_E3H; }


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool VCE6::DokoSave( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->SetVal( "VOICE", "io_E0H",		"", "0x%02X", io_E0H );
	Ini->SetVal( "VOICE", "io_E2H",		"", "0x%02X", io_E2H );
	Ini->SetVal( "VOICE", "io_E3H",		"", "0x%02X", io_E3H );
	Ini->SetVal( "VOICE", "VStat",		"", VStat );
	
	// 内部句関係(とりあえず無視)
	
	Ini->SetVal( "VOICE", "ParaBuf0",	"",	"0x%02X", ParaBuf[0] );
	Ini->SetVal( "VOICE", "ParaBuf1",	"",	"0x%02X", ParaBuf[1] );
	Ini->SetVal( "VOICE", "ParaBuf2",	"",	"0x%02X", ParaBuf[2] );
	Ini->SetVal( "VOICE", "ParaBuf3",	"",	"0x%02X", ParaBuf[3] );
	Ini->SetVal( "VOICE", "ParaBuf4",	"",	"0x%02X", ParaBuf[4] );
	Ini->SetVal( "VOICE", "ParaBuf5",	"",	"0x%02X", ParaBuf[5] );
	Ini->SetVal( "VOICE", "ParaBuf6",	"",	"0x%02X", ParaBuf[6] );
	Ini->SetVal( "VOICE", "Pnum",		"", Pnum   );
	Ini->SetVal( "VOICE", "Fnum",		"", Fnum   );
	Ini->SetVal( "VOICE", "PReady",		"", PReady );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool VCE6::DokoLoad( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->GetVal( "VOICE", "io_E0H",	io_E0H );
	Ini->GetVal( "VOICE", "io_E2H",	io_E2H );
	Ini->GetVal( "VOICE", "io_E3H",	io_E3H );
	Ini->GetVal( "VOICE", "VStat",	VStat  );
	
	// 内部句関係(とりあえず無視)
	FreeVoice();
	
	Ini->GetVal( "VOICE", "ParaBuf0",	ParaBuf[0] );
	Ini->GetVal( "VOICE", "ParaBuf1",	ParaBuf[1] );
	Ini->GetVal( "VOICE", "ParaBuf2",	ParaBuf[2] );
	Ini->GetVal( "VOICE", "ParaBuf3",	ParaBuf[3] );
	Ini->GetVal( "VOICE", "ParaBuf4",	ParaBuf[4] );
	Ini->GetVal( "VOICE", "ParaBuf5",	ParaBuf[5] );
	Ini->GetVal( "VOICE", "ParaBuf6",	ParaBuf[6] );
	Ini->GetVal( "VOICE", "Pnum",		Pnum       );
	Ini->GetVal( "VOICE", "Fnum",		Fnum       );
	Ini->GetVal( "VOICE", "PReady",		PReady     );
	
	return true;
}

