#include <new>
#include <vector>
#include <memory.h>

#include "pc6001v.h"
#include "error.h"
#include "log.h"
#include "osd.h"
#include "sound.h"


// バッファサイズの倍率
#define	MULTI		(2)
//   内部的にはサイズがMULTI倍されるが対外的には等倍のように振舞う
//   読み書きは全領域に対して行なわれるがサイズ取得は等倍
//   つまりオーバーフローを防止しつつレスポンスを保てる...はず
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cRing::cRing( void ) : Buffer(NULL), Size(0), Wpt(0), Rpt(0), Num(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cRing::~cRing( void )
{
	if( Buffer ) delete [] Buffer;
}


////////////////////////////////////////////////////////////////
// バッファ初期化
//
// 引数:	size		サンプル数
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cRing::InitBuffer( int size )
{
	PRINTD( SND_LOG, "[cRing][Init] Size:%d\n", size );
	
	if( Buffer ) delete [] Buffer;
	
	Size = size * MULTI;	// 実際のバッファサイズはMULTI倍
	Wpt = Rpt = Num = 0;
	
	try{
		Buffer = new int[Size];
		memset( Buffer, 0, Size );
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// 読込み
//
// 引数:	なし
// 返値:	int		データ
////////////////////////////////////////////////////////////////
int cRing::Get( void )
{
	if( Num ){
		cCritical::Lock();
		int data = Buffer[Rpt++];
		if( Rpt == Size ) Rpt = 0;
		Num--;
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
// 返値:	bool		true:成功 false:失敗(バッファいっぱい)
////////////////////////////////////////////////////////////////
bool cRing::Put( int data )
{
	if( Num < Size ){
		cCritical::Lock();
		Buffer[Wpt++] = data;
		if( Wpt == Size ) Wpt = 0;
		Num++;
		cCritical::UnLock();
		return true;
	}else
		return false;
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
SndDev::SndDev( void ) : SampleRate(DEFAULT_SAMPLE_RATE),
							Volume(0), LPF_Mem(0), LPF_fc(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SndDev::~SndDev( void ){}


////////////////////////////////////////////////////////////////
// 初期化
// 引数:	rate	サンプリングレート
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SndDev::Init( int rate )
{
	SampleRate = rate;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ローパスフィルタ カットオフ周波数設定
//
// 引数:	fc		カットオフ周波数
// 返値:	なし
////////////////////////////////////////////////////////////////
void SndDev::SetLPF( int fc )
{
	LPF_fc = fc;
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
	int lpf_k = LPF_fc ? (int)((2.0*M_PI*(double)LPF_fc*(double)0x8000)/(double)SampleRate) : 0x8000;
	
	LPF_Mem += ( ( src - LPF_Mem ) * lpf_k ) / 0x8000;
	return LPF_Mem;
}


////////////////////////////////////////////////////////////////
// 読込み(オーバーライド)
//
// 引数:	なし
// 返値:	int		データ
////////////////////////////////////////////////////////////////
int SndDev::Get( void )
{
	return LPF( this->cRing::Get() );
}


////////////////////////////////////////////////////////////////
// サンプリングレート設定
//
// 引数:	rate	サンプリングレート
//			size	バッファサイズ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SndDev::SetSampleRate( int rate, int size )
{
	if( SampleRate == rate ) return false;
	
	SampleRate = rate;
	
	if( !InitBuffer( size ) ) return false;
	
	return true;
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
SND6::SND6( void ) : Volume(DEFAULT_MASTERVOL), SampleRate(DEFAULT_SAMPLE_RATE), BSize(SOUND_BUFFER_SIZE),
						CbFunc(NULL), CbData(NULL)
{
	INITARRAY( RB, NULL );
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
// 引数:	cbdata		コールバック関数に渡すデータ
//			callback	コールバック関数へのポインタ
//			rate		サンプリングレート
//			size		バッファサイズ(倍率)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SND6::Init( void *cbdata, void (*callback)(void *, BYTE *, int ), int rate, int size )
{
	PRINTD( SND_LOG, "[SND6][Init]\n" );
	
	for( int i=0; i<MAXSTREAM; i++ ) RB[i] = NULL;
	
	// バッファサイズ(サンプル数)
	int samples = rate / VSYNC_HZ;
	
	// バッファ初期化
	if( !this->cRing::InitBuffer( samples * size ) ) return false;
	
	// オーディオデバイスを開く
	if( !OSD_OpenAudio( cbdata, callback, rate, samples ) ) return false;
	
	CbData     = cbdata;
	CbFunc     = callback;
	SampleRate = rate;
	BSize      = size;
	
	PRINTD( SND_LOG, " SampleRate : %d\n", rate );
	PRINTD( SND_LOG, " BufferSize : %d\n", samples * size );
	
	return true;
}


////////////////////////////////////////////////////////////////
// ストリーム接続
//
// 引数:	SndDev *	バッファポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SND6::ConnectStream( SndDev *buf )
{
	PRINTD( SND_LOG, "[SND6][ConnectStream]\n" );
	
	if( !buf ) return false;
	
	// バッファサイズ(サンプル数)
	int samples = SampleRate * BSize / VSYNC_HZ;
	
	for( int i=0; i<MAXSTREAM; i++ ){
		if( !RB[i] ){
			// バッファ初期化
			if( !buf->InitBuffer( samples ) ) return false;
			RB[i] = buf;
			return true;
		}
	}
	
	return false;
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
// サンプリングレート設定
//
// 引数:	rate	サンプリングレート
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool SND6::SetSampleRate( int rate )
{
	SampleRate = rate;
	
	bool pflag = OSD_AudioPlaying();
	OSD_CloseAudio();
	
	// バッファサイズ(サンプル数)
	int samples = rate / VSYNC_HZ;
	
	// バッファ初期化
	if( !this->cRing::InitBuffer( samples * BSize ) ) return false;
	
	for( int i=0; i<MAXSTREAM; i++ )
		if( RB[i] && !RB[i]->SetSampleRate( rate, samples * BSize ) ) return false;
	
	// オーディオデバイスを開く
	if( !OSD_OpenAudio( CbData, CbFunc, rate, samples ) ) return false;
	if( pflag ) Play();
	
	return true;
}


////////////////////////////////////////////////////////////////
// サンプリングレート取得
//
// 引数:	なし
// 返値:	int		サンプリングレート
////////////////////////////////////////////////////////////////
int SND6::GetSampleRate( void )
{
	return SampleRate;
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
// バッファサイズ(倍率)取得
//
// 引数:	なし
// 返値:	int		バッファサイズ(倍率)
////////////////////////////////////////////////////////////////
int SND6::GetBufferSize( void )
{
	return BSize;
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
	
	PRINTD( SND_LOG,"PreUpdate" );
	
	for( int i=0; i<MAXSTREAM; i++ )
		if( RB[i] ){
			PRINTD( SND_LOG," [%d]%d", i, RB[i]->ReadySize() );
			exsam = min( max( exsam, RB[i]->ReadySize() ), samples );
		}
	PRINTD( SND_LOG,"\n" );
	
	for( int i=0; i<exsam; i++ ){
		int dat = 0;
		for( int j=0; j<MAXSTREAM; j++ )
			if( RB[j] ) dat += RB[j]->Get();
		
		dat = ( dat * Volume ) / 100;
		dat = min( max( dat, INT16_MIN ), INT16_MAX );
		
		this->cRing::Put( (int16_t)dat );
		// 外部バッファが存在すれば書込み
		if( exbuf ) exbuf->Put( (int16_t)dat );
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
void SND6::Update()
{
    int size = this->cRing::ReadySize();

    std::vector<int16_t> stream;
    stream.reserve(size);

    for( int i=0; i<size; i++ ){
        stream.push_back((int16_t)this->cRing::Get());
    }
    OSD_WriteAudioStream((BYTE*)&stream[0], stream.size() * sizeof(int16_t));
}
