/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "pc6001v.h"

#include "disk.h"
#include "intr.h"
#include "log.h"
#include "osd.h"
#include "p6el.h"
#include "p6vm.h"
#include "schedule.h"
#include "tape.h"


// P6T形式フォーマットVer.2
//  基本的には「ベタイメージ+フッタ+ベタイメージサイズ(4byte)」という構造
//  フッタにはDATAブロック情報が含まれる。
//  DATAブロックは無音部を区切りとして1と数える。
//  BASICなど「ヘッダ＋データ」という構造の場合，DATAブロックは2個となるが
//  ID番号を同一にすることで関連付けて管理できる。
//
// [フッタ]
//  header (2byte) : "P6"
//  ver    (1byte) : バージョン
//  dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
//  start  (1byte) : オートスタートフラグ(0:無効 1:有効)
//  basic  (1byte) : BASICモード(PC-6001の場合は無意味)
//  page   (1byte) : ページ数
//  askey  (2byte) : オートスタートコマンド文字数
//  ...コマンドがある場合はこの後にaskey分続く
//  exhead (2byte) : 拡張情報サイズ(64KBまで)
//  ...拡張情報がある場合はこの後にexhead分続く
//
// [DATAブロック]
//  header (2byte) : "TI"
//  id     (1byte) : ID番号(DATAブロックを関連付ける)
//  name  (16byte) : データ名(15文字+'00H')
//  baud   (2byte) : ボーレート(600/1200)
//  stime  (2byte) : 無音部の時間(ms)
//  ptime  (2byte) : ぴー音の時間(ms)
//  offset (4byte) : ベタイメージ先頭からのオフセット
//  size   (4byte) : データサイズ


// イベントID
#define	EID_TAPE	(1)

// 周波数
#define PG_HI		(0)
#define PG_LO		(1)

// テープデータ 1バイトあたりのビット数
#define BitsPerByte()	(1+8+StopBit)
// 1秒毎のテープデータ送信バイト数
#define CMT_HZ()		(DEFAULT_BAUD/BitsPerByte())


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CMTL::CMTL( VM6* vm, const ID& id ) : Device( vm, id ),
	FilePath( "" ), Relay( false ), stron( false ),
	Boost( DEFAULT_BOOST ), MaxBoost60( DEFAULT_MAXBOOST60 ),
	MaxBoost62( DEFAULT_MAXBOOST62 ), StopBit( DEFAULT_STOPBIT )
{
	// Device Description (Out)
	descs.outdef.emplace( outB0H, STATIC_CAST( Device::OutFuncPtr, &CMTL::OutB0H ) );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CMTL::~CMTL( void )
{
	Unmount();
}


/////////////////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void CMTL::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_TAPE:	// CMT割込み処理
		// 割込み処理中でない?
		if( vm->CpusIsCmtIntrReady() ){
			// CMTアップデート
			WORD rd = Update();
			// データならCMT割込み発生
			if( rd & PG_D ){
				vm->CpusReqCmtIntr( rd & 0xff );
			}
		}
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// TAPEマウント
/////////////////////////////////////////////////////////////////////////////
bool CMTL::Mount( const P6VPATH& filepath )
{
	PRINTD( TAPE_LOG, "[TAPE][Mount] %s\n", P6VPATH2STR( filepath ).c_str()  );
	
	// 一旦アンマウントする
	Unmount();
	
	// ファイルから読込み
	if( !cP6T::Readf( filepath ) ){
		Unmount();
		return false;
	}
	
	// ファイルパス保存
	FilePath = filepath;
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// TAPEアンマウント
/////////////////////////////////////////////////////////////////////////////
void CMTL::Unmount( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Unmount]\n" );
	
	if( !FilePath.empty() ){
		cP6T::Clear();
		FilePath.clear();
	}
}


/////////////////////////////////////////////////////////////////////////////
// ストリーム更新(1byte分)
/////////////////////////////////////////////////////////////////////////////
WORD CMTL::Update( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Update]\n" );
	
	// TAPEイメージがマウントされていなかったら無音
	if( !IsMount() ){
		return PG_S;
	}
	
	// 1byte分のデータを作る
	int length = SndDev::SampleRate / CMT_HZ();
	int bdata  = length / BitsPerByte();	// 1bitあたりのデータ数
	
	WORD rd = CmtRead();	// CMT 1文字読込み
	
	switch( rd & 0xff00 ){	// データ形式は？
	case PG_P:	// ぴー音の場合
		// 高音にセット
		while( length-- ) SndDev::cRing::Put( GetSinCurve( PG_HI ) );
		stron = true;	// ストリーム更新許可
		
		break;
	case PG_S:	// 無音部の場合
		while( length-- ) SndDev::cRing::Put( 0 );
		
		break;
	case PG_D:	// データの場合
		// スタートビット 1bit
		// データビット   8bits
		// ストップビット 2-10bits (default:3bits)
		
		// スタートビット
		for( int i = 0; i < bdata; i++ ){
			SndDev::cRing::Put( GetSinCurve( PG_LO ) );
		}
		// データビット
		for( int j = 7; j >= 0; j-- ){
			int hilo = (rd >> j) & 1 ? PG_HI : PG_LO;
			for( int i = 0; i < bdata; i++ ){
				SndDev::cRing::Put( GetSinCurve( hilo ) );
			}
		}
		// ストップビット
		for( length -= bdata * 9; length > 0; length-- ){
			SndDev::cRing::Put( GetSinCurve( PG_HI ) );
		}
		break;
	}
	
	return rd;
}


/////////////////////////////////////////////////////////////////////////////
// ストリーム更新
//
// 引数:	samples	更新するサンプル数
// 返値:	int		更新したサンプル数
/////////////////////////////////////////////////////////////////////////////
int CMTL::SoundUpdate( int samples )
{
	int length = max( 0, samples - SndDev::cRing::ReadySize() );
	
	PRINTD( TAPE_LOG, "[TAPE][SoundUpdate] Samples: %d -> %d\n", samples, length );
	
	for( int i = 0; i < length; i++ ){
		// バッファに書込み
		SndDev::cRing::Put( stron ? GetSinCurve( PG_HI ) : 0 );	// 手抜き
	}
	
	return length;
}


/////////////////////////////////////////////////////////////////////////////
// ファイルパス取得
/////////////////////////////////////////////////////////////////////////////
const P6VPATH& CMTL::GetFile( void ) const
{
	return FilePath;
}


/////////////////////////////////////////////////////////////////////////////
// マウント済み?
/////////////////////////////////////////////////////////////////////////////
bool CMTL::IsMount( void ) const
{
	return !FilePath.empty();
}


/////////////////////////////////////////////////////////////////////////////
// リレーの状態取得
/////////////////////////////////////////////////////////////////////////////
bool CMTL::IsRelay( void ) const
{
	return Relay;
}


/////////////////////////////////////////////////////////////////////////////
// BoostUp設定
/////////////////////////////////////////////////////////////////////////////
void CMTL::SetBoost( bool boost )
{
	if( Boost != boost ){
		Boost = boost;
		// リレーONだったら一旦止めて再開
		if( Relay ){
			Remote( false );
			Remote( true );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// BoostUp最大倍率設定
/////////////////////////////////////////////////////////////////////////////
void CMTL::SetMaxBoost( int max60, int max62 )
{
	if( max60 > 0 ){ MaxBoost60 = max60; }
	if( max62 > 0 ){ MaxBoost62 = max62; }
}


/////////////////////////////////////////////////////////////////////////////
// BoostUp状態取得
/////////////////////////////////////////////////////////////////////////////
bool CMTL::IsBoostUp( void ) const
{
	return Boost;
}


/////////////////////////////////////////////////////////////////////////////
// ストップビット数設定
/////////////////////////////////////////////////////////////////////////////
void CMTL::SetStopBit( int bits )
{
	StopBit = min( max( MIN_STOPBIT, bits ), MAX_STOPBIT );
}


/////////////////////////////////////////////////////////////////////////////
// ストップビット数取得
/////////////////////////////////////////////////////////////////////////////
int CMTL::GetStopBit( void ) const
{
	return StopBit;
}


/////////////////////////////////////////////////////////////////////////////
// CMT 1文字読込み
// 戻り値：上位はデータ種(ぴー,無音,データ) 下位はデータ
// リレーON & CMT OPEN が前提
/////////////////////////////////////////////////////////////////////////////
WORD CMTL::CmtRead( void )
{
	PRINTD( TAPE_LOG, "[TAPE][CmtRead] " );
	
	// TAPEがマウントされていなければ無音
	if( FilePath.empty() ){
		return PG_S;
	}
	
	// 無音部待ち?
	if( cP6T::IsSWaiting( BitsPerByte() ) ){
		PRINTD( TAPE_LOG, "swait\n" );
		return PG_S;
	}
	
	// ぴー音待ち?
	if( cP6T::IsPWaiting( BitsPerByte() ) ){
		PRINTD( TAPE_LOG, "pwait\n" );
		return PG_P;
	}
	
	// データ読込み
	PRINTD( TAPE_LOG, "read\n" );
	return PG_D | cP6T::ReadOne();
}


/////////////////////////////////////////////////////////////////////////////
// リモート制御(PLAY,STOP)
/////////////////////////////////////////////////////////////////////////////
bool CMTL::Remote( bool relay )
{
    PRINTD( TAPE_LOG, "[TAPE][Relay] -> %s\n", relay ? "true" : "false" );
	
	stron = false;
	
	// リレーの状態を保存
	Relay = relay;
	
	// リレーの状態によりイベント追加or削除
	if( relay ){	// ON
		// 画面サイズによりBoostUp最大倍率設定
		// 本当はBASICモード(N60,N60m/N66)によって判断するべき
		// あまり厳密ではないけどいいことにする
		// SRはmk2/66と同じにしてみる
		int bst = Boost ? ( vm->VdgGetWinSize() ? MaxBoost60 : MaxBoost62 ) : 1;
		
		if( !vm->EventAdd( Device::GetID(), EID_TAPE, CMT_HZ() * bst, EV_LOOP | EV_HZ ) ){
			return false;
		}
	}else{			// OFF
		if( !vm->EventDel( Device::GetID(), EID_TAPE ) ){
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// sin波取得
/////////////////////////////////////////////////////////////////////////////
int CMTL::GetSinCurve( int fq )
{
	static const int sinc[] = {
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767
 	};
	static int n = 0;
	
	// サンプリングレートと周波数で間引きの間隔を決める
	// データテーブルは44100Hz,1200Hzの数値
	n += (fq == PG_HI ? 2:1 ) * 44100 / SndDev::SampleRate;
	
	// テーブルサイズは72(sizeof(sinc))
	if( n >= COUNTOF(sinc) ){ n -= COUNTOF(sinc); }
	
	return sinc[n];
}




/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CMTS::CMTS( VM6* vm, const ID& id ) : Device( vm, id ), FilePath( "" ), Baud( 1200 )
{
	fs.clear();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CMTS::~CMTS( void )
{
	Unmount();
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////
bool CMTS::Init( const P6VPATH& filepath )
{
	if( !filepath.empty() ){
		// ファイルパス保存
		FilePath = filepath;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// TAPEマウント
/////////////////////////////////////////////////////////////////////////////
bool CMTS::Mount( void )
{
	if( fs.is_open() ){ fs.close(); }
	
	return OSD_FSopen( fs, FilePath, std::ios_base::out | std::ios_base::binary );
}


/////////////////////////////////////////////////////////////////////////////
// TAPEアンマウント
/////////////////////////////////////////////////////////////////////////////
void CMTS::Unmount( void )
{
	if( !fs.is_open() ){
		return;
	}
	
	// フッタを付ける(とりあえずベタで)
	fs.flush();
	DWORD beta = fs.tellp();	// ベタイメージサイズ取得
	
	// [フッタ]
	FSPUTBYTE( 'P', fs );	// header (2byte) : "P6"
	FSPUTBYTE( '6', fs );
	FSPUTBYTE( 2, fs );		// ver    (1byte) : バージョン
	FSPUTBYTE( 1, fs );		// dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
	FSPUTBYTE( 0, fs );		// start  (1byte) : オートスタートフラグ(0:無効 1:有効)
	FSPUTBYTE( 1, fs );		// basic  (1byte) : BASICモード(PC-6001の場合は無意味)
	FSPUTBYTE( 1, fs );		// page   (1byte) : ページ数
	FSPUTBYTE( 0, fs );		// askey  (2byte) : オートスタートコマンド文字数
	FSPUTBYTE( 0, fs );		// ...コマンドがある場合はこの後にaskey分続く
	FSPUTBYTE( 0, fs );		// exhead (2byte) : 拡張情報サイズ(64KBまで)
	FSPUTBYTE( 0, fs );		// ...拡張情報がある場合はこの後にexhead分続く
	
	// [DATAブロック]
	FSPUTBYTE( 'T', fs );	// header (2byte) : "TI"
	FSPUTBYTE( 'I', fs );
	FSPUTBYTE( 0, fs );		// id     (1byte) : ID番号(DATAブロックを関連付ける)
	for( int i = 0; i < 16; i++ ) FSPUTBYTE( 0, fs );	// name  (16byte) : データ名(15文字+'00H')
	FSPUTWORD( Baud, fs );	// baud   (2byte) : ボーレート(600/1200)
	FSPUTBYTE( 0x48, fs );	// stime  (2byte) : 無音部の時間(ms)
	FSPUTBYTE( 0x0d, fs );
	FSPUTBYTE( 0x48, fs );	// ptime  (2byte) : ぴー音の時間(ms)
	FSPUTBYTE( 0x0d, fs );
	FSPUTBYTE( 0, fs );		// offset (4byte) : ベタイメージ先頭からのオフセット
	FSPUTBYTE( 0, fs );
	FSPUTBYTE( 0, fs );
	FSPUTBYTE( 0, fs );
	FSPUTDWORD( beta, fs );	// size   (4byte) : データサイズ
	
	// [ベタイメージサイズ]
	FSPUTDWORD( beta, fs );
	
	fs.close();
	FilePath.clear();
}


/////////////////////////////////////////////////////////////////////////////
// ボーレート設定
/////////////////////////////////////////////////////////////////////////////
void CMTS::SetBaud( int b )
{
	Baud = b;
}


/////////////////////////////////////////////////////////////////////////////
// 1文字書込み
/////////////////////////////////////////////////////////////////////////////
void CMTS::WriteOne( BYTE data )
{
	if( fs.is_open() ){
		FSPUTBYTE( data, fs );
	}
}


/////////////////////////////////////////////////////////////////////////////
// I/Oアクセス関数
/////////////////////////////////////////////////////////////////////////////
void CMTL::OutB0H( int, BYTE data ){ Remote( data & 0x08 ? true : false ); }


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
/////////////////////////////////////////////////////////////////////////////
bool CMTL::DokoSave( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->SetVal( "TAPE", "Relay",	"",	Relay );
	Ini->SetVal( "TAPE", "BoostUp",	"",	Boost );
	Ini->SetVal( "TAPE", "StopBit",	"",	StopBit );
	
	// TAPEがマウントされてなければ何もしないで戻る
	if( !IsMount() ){
		return true;
	}
	
	// マウントされていたらP6TオブジェクトをSAVE
	P6VPATH tpath = FilePath;
	OSD_RelativePath( tpath );
	Ini->SetVal( "TAPE", "FilePath",	"", tpath );
	
	// P6T
	Ini->SetVal( "P6T", "Counter",		"", cP6T::GetCount() );
	Ini->SetVal( "P6T", "swait",		"", cP6T::swait );
	Ini->SetVal( "P6T", "pwait",		"", cP6T::pwait );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
/////////////////////////////////////////////////////////////////////////////
bool CMTL::DokoLoad( cIni* Ini )
{
	int st = 0;
	P6VPATH fpath = "";
	
	if( !Ini ){
		return false;
	}
	
	Ini->GetVal( "TAPE", "Relay",		Relay );
	Ini->GetVal( "TAPE", "BoostUp",		Boost );
	Ini->GetVal( "TAPE", "StopBit",		StopBit );
	
	Ini->GetVal( "TAPE", "FilePath",	fpath );
	if( !fpath.empty() ){
		if( !Mount( fpath ) ){
			return false;
		}
		
		// P6T
		Ini->GetVal( "P6T",	"Counter",	st );
		cP6T::SetCount( st );
		Ini->GetVal( "P6T",	"swait",	cP6T::swait );
		Ini->GetVal( "P6T",	"pwait",	cP6T::pwait );
	}else
		Unmount();
	
	
	return true;
}

