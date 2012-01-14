#include <string.h>

#include "pc6001v.h"
#include "log.h"
#include "voice.h"
#include "osd.h"
#include "schedule.h"


// イベントID
#define	EID_FRAME	(1)



////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
VCE6::VCE6( VM6 *vm, const ID& id ) : P6DEVICE(vm,id), Device(id)
{
	SndDev::Volume = DEFAULT_VOICEVOL;
	*FilePath = '\0';
	io_E0H = io_E2H = io_E3H = 0;
	Pnum    = 0;
	Fnum    = 0;
	PReady 	= FALSE;
	Fbuf    = NULL;
	VStat   = D7752E_IDL;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
VCE6::~VCE6()
{
	if( Fbuf )    delete [] Fbuf;
}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void VCE6::EventCallback( int id, int clock )
{
	PRINTD( VOI_LOG, "[VOICE][EventCallback] -> " );
	
	switch( id ){
	case EID_FRAME:	// 1フレーム
		if( VStat & D7752E_EXT ){	// 外部句発声処理
			PRINTD( VOI_LOG, "(EXT)\n" );
			if( PReady ){	// パラメータセット完了してる?
				// フレーム数=0ならば終了処理
				if( !(ParaBuf[0]>>3) )
					AbortVoice();
				else{
					// 1フレーム分のサンプル生成
					cD7752::Synth(	ParaBuf, Fbuf );
					
					// サンプリングレートを変換してバッファに書込む
					UpConvert();
					
					// 次フレームのパラメータを受け付ける
					PReady = FALSE;
					VStat |= D7752E_REQ;
				}
			}else{			// 完了してなければエラー
				AbortVoice();		// 発声停止
				VStat = D7752E_ERR;	// ステータスをエラー発生にセット
			}
		}else{						// 内部句発声処理
			PRINTD( VOI_LOG, "(INT) " );
			// 1フレーム分のサンプルをバッファに書込む
			int num = min( IVLen - IVPos, cD7752::GetFrameSize() * SndDev::SampleRate / 10000 );
			PRINTD2( VOI_LOG, "%d/%d\n", num, IVPos );
			while( num-- )
				SndDev::cRing::Put( ( IVBuf[IVPos++] * SndDev::Volume ) / 100 );
			if( IVPos >= IVLen ){	// 最後まで発生したら発声終了
				AbortVoice();
			}
		}
		
		break;
	}
}


////////////////////////////////////////////////////////////////
// モードセット
////////////////////////////////////////////////////////////////
void VCE6::VSetMode( BYTE mode )
{
	PRINTD2( VOI_LOG, "[VOICE][VSetMode]    %02X %dms", mode, (((mode>>2)&1)+1)*10 );
	PRINTD1( VOI_LOG, " %s\n", mode&1 && mode&2 ? "X" : mode&1 ? "SLOW" : mode&2 ? "FAST" : "NORMAL" );
	
	// 音声合成開始
	cD7752::Start( mode );
	
	// ステータスクリア
	VStat = D7752E_IDL;
}


////////////////////////////////////////////////////////////////
// コマンドセット
////////////////////////////////////////////////////////////////
void VCE6::VSetCommand( BYTE comm )
{
	PRINTD1( VOI_LOG, "[VOICE][VSetCommand] %02X\n", comm );
	
	// 発声中なら止める
	AbortVoice();
	
	switch( comm ){
	case 0x00:		// 内部句選択コマンド ----------
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
		// WAVファイルを読込む
		if( !LoadVoice( comm ) ) break;
		
		// ステータスを内部句再生モードにセット
		VStat = D7752E_BSY;
		
		// フレームイベントをセットする
		vm->sche->Add( this, EID_FRAME, 10000.0/(double)cD7752::GetFrameSize(), EV_LOOP|EV_HZ );
		
		break;
		
	case 0xfe:		// 外部句選択コマンド ----------
		// フレームバッファ確保
		Fbuf = new D7752_SAMPLE[cD7752::GetFrameSize()];
		if( !Fbuf ) break;
		
		// ステータスを外部句再生モードにセット，パラメータ受付開始
		VStat = D7752E_BSY | D7752E_EXT | D7752E_REQ;
		
		// フレームイベントをセットする
		vm->sche->Add( this, EID_FRAME, 10000.0/(double)cD7752::GetFrameSize(), EV_LOOP|EV_HZ );
		
		break;
		
	case 0xff:		// ストップコマンド ----------
		break;
		
	default:		// 無効コマンド  ----------
		VStat = D7752E_ERR;	// ホント?
		break;
	}
}


////////////////////////////////////////////////////////////////
// 音声パラメータ転送
////////////////////////////////////////////////////////////////
void VCE6::VSetData( BYTE data )
{
	PRINTD1( VOI_LOG, "[VOICE][VSetData]    %02X\n", data );
	
	// 再生時のみデータを受け付ける
	if(	(VStat & D7752E_BSY)&&(VStat & D7752E_REQ) ){
		if( Fnum == 0 || Pnum ){	// 最初のフレーム?
			// 最初のパラメータだったら繰り返し数を設定
			if( Pnum == 0 ) Fnum = data>>3;
			// パラメータバッファに保存
			ParaBuf[Pnum++] = data;
			// もし1フレーム分のパラメータが溜まったら発声準備完了
			if( Pnum == 7 ){
				VStat &= ~D7752E_REQ;
				Pnum = 0;
				if( Fnum > 0 ) Fnum--;
				PReady = TRUE;
			}
		}else{						// 繰り返しフレーム?
			// パラメータバッファに保存
			// PD7752の仕様に合わせる
			for( int i=1; i<6; i++ ) ParaBuf[i] = 0;
			ParaBuf[6] = data;
			VStat &= ~D7752E_REQ;
			Pnum = 0;
			Fnum--;
			PReady = TRUE;
		}
	}
}


////////////////////////////////////////////////////////////////
// ステータスレジスタ取得
////////////////////////////////////////////////////////////////
int VCE6::VGetStatus( void )
{
	return VStat;
}


////////////////////////////////////////////////////////////////
// 発声停止
////////////////////////////////////////////////////////////////
void VCE6::AbortVoice( void )
{
	PRINTD( VOI_LOG, "[VOICE][AbortVoice]\n" );
	
	// フレームイベント停止
	vm->sche->Del( this, EID_FRAME );
	
	// 残りのパラメータはキャンセル
	Pnum = Fnum = 0;
	PReady = FALSE;
	
	// フレームバッファ開放
	if( Fbuf ){
		delete [] Fbuf;
		Fbuf = NULL;
	}
	
	// 内部句WAV開放
	FreeVoice();
	
	VStat &= ~D7752E_BSY;
}


////////////////////////////////////////////////////////////////
// サンプリングレート変換
////////////////////////////////////////////////////////////////
void VCE6::UpConvert( void )
{
	PRINTD( VOI_LOG, "[VOICE][UpConvert]\n" );
	
	// 10kHz -> 実際に再生するサンプリングレートに変換した時のサンプル数
	int samples = cD7752::GetFrameSize() * SndDev::SampleRate / 10000;
	PRINTD1( VOI_LOG, "UpConvert %d \n", samples );
	
	for( int i=0; i<samples; i++ ){
		int dat = Fbuf[i * cD7752::GetFrameSize() / samples] * 4;	// * 4 は 16bit<-14bit のため
//		SndDev::cRing::Put( ( dat * SndDev::Volume ) / 100 );
		SndDev::cRing::Put( ( dat * SndDev::Volume * 2 ) / 100 );		// 出力レベルが低いのでとりあえず2倍
	}
	PRINTD( VOI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// 内部句WAV読込み
////////////////////////////////////////////////////////////////
BOOL VCE6::LoadVoice( int index )
{
	PRINTD1( VOI_LOG, "[VOICE][LoadVoice]  No.%d -> ", index );
	
	DWORD len;		// 元データサイズ
	BYTE *buf;		// 元データバッファ
	int freq;		// 元データサンプリングレート
	
	// WAVファイルを読込む
	char filepath[PATH_MAX];
	sprintf( filepath, "%sf4%d.wav", FilePath, index );
	
	PRINTD1( VOI_LOG, "%s ->", filepath );
	
	if( !OSD_LoadWAV( filepath, &buf, &len, &freq ) ){
		PRINTD( VOI_LOG, "Error!\n" );
		return FALSE;
	}
	
	PRINTD( VOI_LOG, "OK\n" );
	
	// 発声速度変換後のサイズを計算してバッファを確保
	// 発声速度4の時,1フレームのサンプル数は160
	IVLen = (int)( (double)SndDev::SampleRate * (double)(len/2) / (double)freq
					* (double)cD7752::GetFrameSize() / (double)160 );
	
	PRINTD2( VOI_LOG, "Len:%d/%d ->", IVLen, (int)len );
	
	IVBuf = new int[IVLen];
	if( !IVBuf ){
		OSD_FreeWAV( buf );
		IVLen = 0;
		return FALSE;
	}
	
	// 発声速度変換
	// 単なる間引きなのでピッチが変わってしまうのが問題
	// FFTを使うか?
	signed short *sbuf = (signed short *)buf;
	for( int i=0; i<IVLen; i++ ){
		IVBuf[i] = sbuf[(int)(( (double)i * (double)(len/2) ) / (double)IVLen)];
	}
	
	// WAV開放
	OSD_FreeWAV( buf );
	
	// 読込みポインタ初期化
	IVPos = 0;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 内部句WAV開放
////////////////////////////////////////////////////////////////
void VCE6::FreeVoice( void )
{
	if( IVBuf ){
		delete [] IVBuf;
		IVBuf = NULL;
		IVLen = IVPos= 0;
	}
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
BOOL VCE6::Init( int srate, char *path )
{
	PRINTD( VOI_LOG, "[VOICE][Init] BufferSize:" );
	
	// WAVEファイル格納パス保存
	strncpy( FilePath, path, PATH_MAX );
	
	PRINTD1( VOI_LOG, ":%d\n", SndDev::cRing::GetSize() );
	
	return SndDev::Init( srate );
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void VCE6::Reset( void )
{
	AbortVoice();
	VStat = D7752E_IDL;
}


////////////////////////////////////////////////////////////////
// ストリーム更新
////////////////////////////////////////////////////////////////
int VCE6::SoundUpdate( int samples )
{
	PRINTD( VOI_LOG, "[VOICE][Update]\n" );
	
	return 0;
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
inline void VCE6::OutE0H( int, BYTE data ){ VSetData( data ); }
inline void VCE6::OutE2H( int, BYTE data ){ VSetMode( data ); }
inline void VCE6::OutE3H( int, BYTE data ){ VSetCommand( data ); }
inline BYTE VCE6::InE0H( int ){ return VGetStatus(); }
inline BYTE VCE6::InE2H( int ){ return io_E2H; }
inline BYTE VCE6::InE3H( int ){ return io_E3H; }


////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VCE6::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	Ini->PutEntry( "VOICE", NULL, "io_E0H",		"0x%02X",	io_E0H );
	Ini->PutEntry( "VOICE", NULL, "io_E2H",		"0x%02X",	io_E2H );
	Ini->PutEntry( "VOICE", NULL, "io_E3H",		"0x%02X",	io_E3H );
	Ini->PutEntry( "VOICE", NULL, "VStat",		"%d",		VStat );
	
	// 内部句関係
//	int IVLen;						// サンプル数
//	int *IVBuf;						// データバッファ
//	int IVPos;						// 読込みポインタ
	
	Ini->PutEntry( "VOICE", NULL, "ParaBuf0",	"0x%02X",	ParaBuf[0] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf1",	"0x%02X",	ParaBuf[1] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf2",	"0x%02X",	ParaBuf[2] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf3",	"0x%02X",	ParaBuf[3] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf4",	"0x%02X",	ParaBuf[4] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf5",	"0x%02X",	ParaBuf[5] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf6",	"0x%02X",	ParaBuf[6] );
	Ini->PutEntry( "VOICE", NULL, "Pnum",		"%d",		Pnum );
	Ini->PutEntry( "VOICE", NULL, "Fnum",		"%d",		Fnum );
	Ini->PutEntry( "VOICE", NULL, "PReady",		"%s",		PReady ? "Yes" : "No" );
	
//	D7752_SAMPLE *Fbuf;				// フレームバッファポインタ(10kHz 1フレーム)
	
	// イベント
	e.id = EID_FRAME;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "VOICE", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL VCE6::DokoLoad( cIni *Ini )
{
	int st;
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	Ini->GetInt(   "VOICE", "io_E0H",		&st,		io_E0H );	io_E0H = st;
	Ini->GetInt(   "VOICE", "io_E2H",		&st,		io_E2H );	io_E2H = st;
	Ini->GetInt(   "VOICE", "io_E3H",		&st,		io_E3H );	io_E3H = st;
	Ini->GetInt(   "VOICE", "VStat",		&VStat,		VStat );
	
	// 内部句関係(とりあえず無視)
	FreeVoice();
	
	Ini->GetInt(   "VOICE", "ParaBuf0",		&st,		ParaBuf[0] );	ParaBuf[0] = st;
	Ini->GetInt(   "VOICE", "ParaBuf1",		&st,		ParaBuf[1] );	ParaBuf[1] = st;
	Ini->GetInt(   "VOICE", "ParaBuf2",		&st,		ParaBuf[2] );	ParaBuf[2] = st;
	Ini->GetInt(   "VOICE", "ParaBuf3",		&st,		ParaBuf[3] );	ParaBuf[3] = st;
	Ini->GetInt(   "VOICE", "ParaBuf4",		&st,		ParaBuf[4] );	ParaBuf[4] = st;
	Ini->GetInt(   "VOICE", "ParaBuf5",		&st,		ParaBuf[5] );	ParaBuf[5] = st;
	Ini->GetInt(   "VOICE", "ParaBuf6",		&st,		ParaBuf[6] );	ParaBuf[6] = st;
	Ini->GetInt(   "VOICE", "Pnum",			&Pnum,		Pnum );
	Ini->GetInt(   "VOICE", "Fnum",			&Fnum,		Fnum );
	Ini->GetTruth( "VOICE", "PReady",		&PReady,	PReady );
	
//	D7752_SAMPLE *Fbuf;				// フレームバッファポインタ(10kHz 1フレーム)
	
	// イベント
	e.id = EID_FRAME;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "VOICE", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor VCE6::descriptor = {
	VCE6::indef, VCE6::outdef
};

const Device::OutFuncPtr VCE6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE0H ),
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE2H ),
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE3H )
};

const Device::InFuncPtr VCE6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE0H ),
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE2H ),
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE3H )
};
