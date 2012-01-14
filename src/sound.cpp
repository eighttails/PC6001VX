#include <new>
#include <memory.h>

#include "pc6001v.h"
#include "log.h"
#include "sound.h"
#include "error.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cRing::cRing( void )
{
	Buffer = NULL;
	Size = Wpt = Rpt = Num = 0;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cRing::~cRing( void )
{
	if( Buffer ) delete[] Buffer;
}


////////////////////////////////////////////////////////////////
// バッファ初期化
//
// 引数:	size		サンプル数
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL cRing::InitBuffer( int size )
{
	PRINTD1( SND_LOG, "[cRing][Init] Size:%d\n", size );
	
	if( Buffer ) delete [] Buffer;
	
	Size = size*MULTI;	// 実際のバッファサイズはMULTI倍
	Wpt = Rpt = Num = 0;
	
	try{
		Buffer = new int[Size];
		memset( Buffer, 0, Size );
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		Error::SetError( Error::MemAllocFailed );
		return FALSE;
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 読込み
//
// 引数:	idx		インデックス(-1:1データ読んでポインタ進める)
//								(0-:idxから1データ読む ポインタ進めない)
// 返値:	int		データ
////////////////////////////////////////////////////////////////
int cRing::Get( int idx )
{
	if( Num ){
		int data = 0;
		cCritical::Lock();
		if( idx < 0 ){
			data = Buffer[Rpt++];
			if( Rpt == Size ) Rpt = 0;
			Num--;
		}else{
			if( idx < Size ){
				int pt = Rpt + idx;
				if( pt >= Size ) pt -= Size;
				data = Buffer[pt];
			}
		}
		cCritical::UnLock();
		return data;
	}else{
		return 0;
	}
}


////////////////////////////////////////////////////////////////
// 書込み
//
// 引数:	data		データ
// 返値:	BOOL		TRUE:成功 FALSE:失敗(バッファいっぱい)
////////////////////////////////////////////////////////////////
BOOL cRing::Put( int data )
{
	if( Num < Size ){
		cCritical::Lock();
		Buffer[Wpt++] = data;
		if( Wpt == Size ) Wpt = 0;
		Num++;
		cCritical::UnLock();
		return TRUE;
	}else
		return FALSE;
}


////////////////////////////////////////////////////////////////
// 未読データ数取得
//
// 引数:	なし
// 返値:	int			未読サンプル数
////////////////////////////////////////////////////////////////
int cRing::ReadySize( void )
{
	return Num;
}


////////////////////////////////////////////////////////////////
// 残りバッファ取得
//
// 引数:	なし
// 返値:	int			残りバッファサンプル数
////////////////////////////////////////////////////////////////
int cRing::FreeSize( void )
{
	return Size - Num;
}


////////////////////////////////////////////////////////////////
// バッファサイズ取得
//
// 引数:	なし
// 返値:	int			全バッファサンプル数
////////////////////////////////////////////////////////////////
int cRing::GetSize( void )
{
	return Size;
}




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SndDev::SndDev( void )
{
	SampleRate = DEFAULT_SAMPLE_RATE;
	Volume     = 0;
	LPF_Mem    = 0;
	LPF_k      = 0x8000;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SndDev::~SndDev( void ){}


////////////////////////////////////////////////////////////////
// 初期化
// 引数:	rate	サンプリングレート
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
int SndDev::Init( int rate )
{
	SampleRate = rate;
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// ローパスフィルタ カットオフ周波数設定
//
// 引数:	fc		カットオフ周波数
// 返値:	なし
////////////////////////////////////////////////////////////////
void SndDev::SetLPF( int fc )
{
	if( fc == 0 )
		LPF_k = 0x8000;
	else
		LPF_k = (int)((2.0*M_PI*(double)fc*(double)0x8000)/(double)SampleRate);
}


////////////////////////////////////////////////////////////////
// ローパスフィルタ
//
// [参考] 初歩のディジタルフィルタ
// http://kmkz.jp/mtm/mag/lab/digitalfilter.htm
//
// 引数:	src		元データ
// 返値:	int		フィルタ適用後のデータ
////////////////////////////////////////////////////////////////
int SndDev::LPF( int src )
{
	LPF_Mem += ( ( src - LPF_Mem ) * LPF_k ) / 0x8000;
	return LPF_Mem;
}


////////////////////////////////////////////////////////////////
// 読込み(オーバーライド)
//
// 引数:	idx		インデックス(-1:1データ読んでポインタ進める)
//								(0-:idxから1データ読む ポインタ進めない)
// 返値:	int		データ
////////////////////////////////////////////////////////////////
int SndDev::Get( int idx )
{
	return LPF( cRing::Get( idx ) );
}


////////////////////////////////////////////////////////////////
// 音量設定
//
// 引数:	vol		音量(0-100)
// 返値:	なし
////////////////////////////////////////////////////////////////
void SndDev::SetVolume( int vol )
{
	Volume = min( max( vol, 0 ), 100 );
}


////////////////////////////////////////////////////////////////
// ストリーム更新
//
// 引数:	samples	新するサンプル数
// 返値:	int		更新したサンプル数
////////////////////////////////////////////////////////////////
int SndDev::SoundUpdate( int samples )
{
	return 0;
}


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SND6::SND6( void )
{
	for( int i=0; i<MAXSTREAM; i++ ) RB[i] = NULL;
	Volume     = DEFAULT_MASTERVOL;
	SampleRale = DEFAULT_SAMPLE_RATE;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SND6::~SND6( void )
{
	Pause();
	// オーディオデバイスを閉じる
	OSD_CloseAudio();
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	p6			自分自身へのオブジェクトポインタ
//			callback	コールバック関数へのポインタ
//			rate		サンプリングレート
//			size		バッファサイズ(倍率)
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL SND6::Init( void *p6, void (*callback)(void *, BYTE *, int ), int rate, int size )
{
	PRINTD( SND_LOG, "[SND6][Init]\n" );
	
	for( int i=0; i<MAXSTREAM; i++ ) RB[i] = NULL;
	
	// バッファサイズ(サンプル数)
	int samples = rate * size / VSYNC_HZ;
	
	// バッファ初期化
	if( !cRing::InitBuffer( samples ) ) return FALSE;
	
	// オーディオデバイスを開く
	if( !OSD_OpenAudio( p6, callback, rate, samples ) ) return FALSE;
	
	SampleRale = rate;
	
	PRINTD1( SND_LOG, " SampleRate : %d\n", rate );
	PRINTD1( SND_LOG, " BufferSize : %d\n", samples );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// ストリーム接続
//
// 引数:	SndDev *	バッファポインタ
// 返値:	BOOL		TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL SND6::ConnectStream( SndDev *buf )
{
	PRINTD( SND_LOG, "[SND6][ConnectStream]\n" );
	
	if( !buf ) return FALSE;
	
	for( int i=0; i<MAXSTREAM; i++ ){
		if( !RB[i] ){
			// バッファ初期化
			if( !buf->InitBuffer( SampleRale ) ) return FALSE;
			RB[i] = buf;
			return TRUE;
		}
	}
	
	return FALSE;
}


////////////////////////////////////////////////////////////////
// 再生
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SND6::Play( void )
{
	PRINTD( SND_LOG, "[SND6][Play]\n" );
	
	OSD_StartAudio();
}


////////////////////////////////////////////////////////////////
// 停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SND6::Pause( void )
{
	PRINTD( SND_LOG, "[SND6][Pause]\n" );
	
	OSD_StopAudio();
}


////////////////////////////////////////////////////////////////
// サンプリングレート取得
//
// 引数:	なし
// 返値:	int		サンプリングレート
////////////////////////////////////////////////////////////////
int SND6::GetSampleRate( void )
{
	return SampleRale;
}


////////////////////////////////////////////////////////////////
// マスター音量設定
//
// 引数:	vol		音量(0-100)
// 返値:	なし
////////////////////////////////////////////////////////////////
void SND6::SetVolume( int vol )
{
	Volume = min( max( vol, 0 ), 100 );
}


////////////////////////////////////////////////////////////////
// サウンド事前更新関数
//
// 引数:	samples		サンプル数
//			exbuf		外部バッファポインタ
// 返値:	int			更新したサンプル数
////////////////////////////////////////////////////////////////
int SND6::PreUpdate( int samples, cRing *exbuf )
{
	int exsam = 0;
	
	PRINTD3( SND_LOG,"PreUpdate %d %d %d \n", RB[0]->ReadySize(), RB[1]->ReadySize(), RB[2]->ReadySize() );
	
	for( int i=0; i<MAXSTREAM; i++ )
		if( RB[i] ) exsam = min( max( exsam, RB[i]->ReadySize() ), samples );
	
	for( int i=0; i<exsam; i++ ){
		int dat = 0;
		for( int j=0; j<MAXSTREAM; j++ )
			if( RB[j] ) dat += RB[j]->Get();
		
		dat = ( dat * Volume ) / 100;
		dat = min( max( dat, -32768 ), 32767 );
		
		cRing::Put( (signed short)dat );
		// 外部バッファが存在すれば書込み
		if( exbuf ) exbuf->Put( (signed short)dat );
	}
	
	return exsam;
}


////////////////////////////////////////////////////////////////
// サウンド更新関数
//
// 引数:	stream		ストリーム書込みバッファへのポインタ
//			samples		サンプル数
// 返値:	なし
////////////////////////////////////////////////////////////////
void SND6::Update( BYTE *stream, int samples )
{
	signed short *str = (signed short *)stream;
	
	PRINTD3( SND_LOG, "[SND6][Update] Stream:%08X Samples:%d / %d\n", (int)stream, samples, cRing::ReadySize() );
	
	for( int i=0; i<samples; i++ ){
		*(str++) = (signed short)cRing::Get();
	}
}
