#include "pc6001v.h"
#include "log.h"
#include "tape.h"
#include "common.h"
#include "cpus.h"
#include "disk.h"
#include "intr.h"
#include "schedule.h"
#include "vdg.h"


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
#define PG_HI	(0)
#define PG_LO	(1)

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
CMTL::CMTL( VM6 *vm, const ID& id ) : P6DEVICE(vm,id), Device(id)
{
	*FilePath  = '\0';
	p6t        = NULL;
	Relay      = FALSE;
	Boost      = DEFAULT_BOOST;
	MaxBoost60 = DEFAULT_MAXBOOST60;
	MaxBoost62 = DEFAULT_MAXBOOST62;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
CMTL::~CMTL( void )
{
	Unmount();
}


////////////////////////////////////////////////////////////////
// イベントコールバック関数
//
// 引数:	id		イベントID
//			clock	クロック
// 返値:	なし
////////////////////////////////////////////////////////////////
void CMTL::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_TAPE:	// CMT割込み処理
		// 割込み処理中でない?
		if( vm->cpus->IsCmtIntrReady() ){
			// CMTアップデート
			WORD rd = Update();
			// データならCMT割込み発生
			if( rd & PG_D ){
				vm->cpus->ReqCmtIntr( rd & 0xff );
			}
		}
		break;
	}
}


////////////////////////////////////////////////////////////////
// リモート制御(PLAY,STOP)
////////////////////////////////////////////////////////////////
BOOL CMTL::Remote( BOOL relay )
{
    PRINTD1( TAPE_LOG, "[TAPE][Relay] -> %s\n", relay ? "TRUE" : "FALSE" );
	
	// リレーの状態を保存
	Relay = relay;
	
	// リレーの状態によりイベント追加or削除
	if( relay ){	// ON
		// 画面サイズによりBoostUp最大倍率設定
		// 本当はBASICモード(N60,N60m/N66)によって判断するべき
		// あまり厳密ではないけどいいことにする
		int bst = Boost ? ( vm->vdg->GetWinSize() ? MaxBoost60 : MaxBoost62 ) : 1;
		
		if( IsMount() ) p6t->SetBoost( bst );
		
		if( !vm->sche->Add( this, EID_TAPE, DEFAULT_CMT_HZ * bst, EV_LOOP|EV_HZ ) ) return FALSE;
	}else{			// OFF
		if( !vm->sche->Del( this, EID_TAPE ) ) return FALSE;
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
BOOL CMTL::Init( int srate )
{
	// P6T情報 領域初期化(リセット時には無効)
	p6t = NULL;
	
	return SndDev::Init( srate );
}


////////////////////////////////////////////////////////////////
// リセット
////////////////////////////////////////////////////////////////
void CMTL::Reset( void )
{
	// TAPEをリセット
	if( p6t ) p6t->Reset();
}


////////////////////////////////////////////////////////////////
// オートスタート文字列設定
////////////////////////////////////////////////////////////////
void CMTL::SetAutoStart( int model )
{
	// オートスタート有効?
	if( p6t && p6t->GetStart() ){
		char buf[256] = "";
		
		// キーバッファに書込み
		switch( model ){
		case 60:	// PC-6001
			sprintf( buf, "%c%c", p6t->GetPage()+'0', 0x0d );
			break;
		case 62:	// PC-6001mk2
			switch( p6t->GetBASIC() ){
			case 3:
			case 4:
			case 5:
				if( vm->disk->GetDrives() )	// ??? 実際は?
					sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, p6t->GetBASIC()+'0', 0x17, 20, 0x0d, p6t->GetPage()+'0', 0x0d, 0x17, 110 );
				else
					sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, p6t->GetBASIC()+'0', 0x17, 20,       p6t->GetPage()+'0', 0x0d, 0x17, 110 );
				break;
			default:
				sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, p6t->GetBASIC()+'0', 0x17, 20, p6t->GetPage()+'0', 0x0d, 0x17, 10 );
			}
			break;
		case 66:	// PC-6601
			switch( p6t->GetBASIC() ){
			case 3:
			case 4:
			case 5:
				if( vm->disk->IsMount( 0 ) )
					sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, p6t->GetBASIC()+'0', 0x17, 20, 0x0d, p6t->GetPage()+'0', 0x0d, 0x17, 110 );
				else
					sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, p6t->GetBASIC()+'0', 0x17, 20,       p6t->GetPage()+'0', 0x0d, 0x17, 110 );
				break;
			default:
				sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, p6t->GetBASIC()+'0', 0x17, 20, p6t->GetPage()+'0', 0x0d, 0x17, 10 );
			}
			break;
//		case 64:	// PC-6001mk2SR
//		case 68:	// PC-6601SR
		}
		memcpy( &buf[strlen(buf)], p6t->GetAutoStartStr(), p6t->GetAutoStartLen() );
		
		// 自動キー入力設定
		if( vm->SetAutoKey( buf, strlen(buf) ) );
	}
}


////////////////////////////////////////////////////////////////
// TAPEマウント
////////////////////////////////////////////////////////////////
BOOL CMTL::Mount( char *filename )
{
	PRINTD1( TAPE_LOG, "[TAPE][Mount] %s\n", filename  );
	
	// もしマウント済みであればアンマウントする
	if( p6t ){
		delete p6t;
		p6t = NULL;
	}
	
	// P6T確保
	p6t = new cP6T;
	if( !p6t ) return FALSE;
	
	// ファイルから読込み
	if( !p6t->Readf( filename ) ){
		delete p6t;
		p6t = NULL;
		return FALSE;
	}
	
	// ファイルパス保存
	strncpy( FilePath, filename, PATH_MAX );
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
// TAPEアンマウント
////////////////////////////////////////////////////////////////
void CMTL::Unmount( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Unmount]\n" );
	
	if( p6t ){
		delete p6t;
		p6t = NULL;
		*FilePath = '\0';
	}
}


////////////////////////////////////////////////////////////////
// CMT 1文字読込み
// 戻り値：上位はデータ種(ぴー,無音,データ) 下位はデータ
// リレーON & CMT OPEN が前提
////////////////////////////////////////////////////////////////
WORD CMTL::CmtRead( void )
{
	PRINTD( TAPE_LOG, "[TAPE][CmtRead] " );
	
	// TAPEがマウントされていなければ無音
	if( !p6t ) return PG_S;
	
	// 無音部待ち?
	if( p6t->IsSWaiting() ){
		PRINTD( TAPE_LOG, "swait\n" );
		return PG_S;
	}
	
	// ぴー音待ち?
	if( p6t->IsPWaiting() ){
		PRINTD( TAPE_LOG, "pwait\n" );
		return PG_P;
	}
	
	// データ読込み
	PRINTD( TAPE_LOG, "read\n" );
	return PG_D | p6t->ReadOne();
}


////////////////////////////////////////////////////////////////
// マウント済み?
////////////////////////////////////////////////////////////////
BOOL CMTL::IsMount( void )
{
	if( p6t ) return TRUE;
	else      return FALSE;
}


////////////////////////////////////////////////////////////////
// オートスタート?
////////////////////////////////////////////////////////////////
BOOL CMTL::IsAutoStart( void )
{
	if( p6t ) return p6t->GetStart();
	else      return FALSE;
}


////////////////////////////////////////////////////////////////
// ファイルパス取得
////////////////////////////////////////////////////////////////
char *CMTL::GetFile()
{
	return FilePath;
}


////////////////////////////////////////////////////////////////
// TAPE名取得
////////////////////////////////////////////////////////////////
char *CMTL::GetName( void )
{
	if( p6t ) return p6t->GetName();
	else      return (char *)"";
}


////////////////////////////////////////////////////////////////
// ベタイメージサイズ取得
////////////////////////////////////////////////////////////////
DWORD CMTL::GetSize( void )
{
	if( p6t ) return p6t->GetSize();
	else      return 0;
}


////////////////////////////////////////////////////////////////
// カウンタ取得
////////////////////////////////////////////////////////////////
int CMTL::GetCount( void )
{
	if( p6t ) return p6t->GetCount();
	else      return 0;
}


////////////////////////////////////////////////////////////////
// リレーの状態取得
////////////////////////////////////////////////////////////////
BOOL CMTL::IsRelay( void )
{
	return Relay;
}


////////////////////////////////////////////////////////////////
// BoostUp設定
////////////////////////////////////////////////////////////////
void CMTL::SetBoost( BOOL boost )
{
	if( Boost != boost ){
		Boost = boost;
		// リレーONだったら一旦止めて再開
		if( Relay ){
			Remote( FALSE );
			Remote( TRUE );
		}
	}
}


////////////////////////////////////////////////////////////////
// BoostUp最大倍率設定
////////////////////////////////////////////////////////////////
void CMTL::SetMaxBoost( int max60, int max62 )
{
	if( max60 > 0 ) MaxBoost60 = max60;
	if( max62 > 0 ) MaxBoost62 = max62;
}


////////////////////////////////////////////////////////////////
// BoostUp状態取得
////////////////////////////////////////////////////////////////
BOOL CMTL::IsBoostUp( void )
{
	return Boost;
}


////////////////////////////////////////////////////////////////
// ストリーム更新
////////////////////////////////////////////////////////////////
WORD CMTL::Update( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Update]\n" );
	
	WORD rd = PG_S;
	
	// TAPEイメージオープン?
	if( p6t ){
		// 1byte分のデータを作る 10ms(基本)
		int length = GetSampleRate() / DEFAULT_CMT_HZ;
		int bdata;
		
		rd = CmtRead();		// CMT 1文字読込み
		
		switch( rd & 0xff00 ){	// データ形式は？
		case PG_P:	// ぴー音の場合
			// 高音にセット
			while( length-- ) SndDev::cRing::Put( GetSinCurve( PG_HI ) );
			
			break;
		case PG_S:	// 無音部の場合
			while( length-- ) SndDev::cRing::Put( 0 );
			
			break;
		case PG_D:	// データの場合
			// スタートビット 1bit
			// データビット   8bits
			// ストップビット 3bits
			
			bdata = length/12;	// 1bitあたりのデータ数
			// スタートビット
			for( int i=0; i<bdata; i++ ){
				SndDev::cRing::Put( GetSinCurve( PG_LO ) );
			}
			// データビット
			for( int j=7; j>=0; j-- ){
				int hilo = (rd>>j)&1 ? PG_HI : PG_LO;
				for( int i=0; i<bdata; i++ ){
					SndDev::cRing::Put( GetSinCurve( hilo ) );
				}
			}
			// ストップビット
			for( length -= bdata*9; length > 0; length-- ){
				SndDev::cRing::Put( GetSinCurve( PG_HI ) );
			}
			break;
		}
	}
	
	return rd;
}


////////////////////////////////////////////////////////////////
// sin波取得
////////////////////////////////////////////////////////////////
int CMTL::GetSinCurve( int fq )
{
	static const int sinc[] = {
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767
 	};
	static int n=0;
	
	// サンプリングレートと周波数で間引きの間隔を決める
	// データテーブルは44100Hz,1200Hzの数値
	n += (fq == PG_HI ? 2:1 ) * 44100 / GetSampleRate();
	
	// テーブルサイズは72(sizeof(sinc))
	if( n >= (int)(sizeof(sinc)/sizeof(int)) ) n -= (int)(sizeof(sinc)/sizeof(int));
	
	int ret = ( sinc[n] * SndDev::Volume ) / 100;
	
	return ret;
}


////////////////////////////////////////////////////////////////
// サンプリングレート取得
////////////////////////////////////////////////////////////////
int CMTL::GetSampleRate( void )
{
	return vm->snd->GetSampleRate();
}




////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
CMTS::CMTS( VM6 *vm, const P6ID& id ) : P6DEVICE(vm,id)
{
	fp   = NULL;
	Baud = 1200;
        *FilePath = NULL;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
CMTS::~CMTS( void )
{
	Unmount();
}


////////////////////////////////////////////////////////////////
// 初期化
////////////////////////////////////////////////////////////////
BOOL CMTS::Init( char *filename )
{
	if( *filename ){
		// ファイルパス保存
		strncpy( FilePath, filename, PATH_MAX );
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////
// TAPEマウント
////////////////////////////////////////////////////////////////
BOOL CMTS::Mount( void )
{
	if( fp ) fclose( fp );
	
	fp = FOPENEN( FilePath, "wb" );
	
	if( fp ) return TRUE;
	else     return FALSE;
}


////////////////////////////////////////////////////////////////
// TAPEアンマウント
////////////////////////////////////////////////////////////////
void CMTS::Unmount( void )
{
	if( fp ){
		// フッタを付ける(とりあえずベタで)
		fflush( fp );
		int beta = ftell( fp );	// ベタイメージサイズ取得
		
		// [フッタ]
		fputc( 'P', fp );	// header (2byte) : "P6"
		fputc( '6', fp );
		fputc( 2, fp );		// ver    (1byte) : バージョン
		fputc( 1, fp );		// dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
		fputc( 0, fp );		// start  (1byte) : オートスタートフラグ(0:無効 1:有効)
		fputc( 1, fp );		// basic  (1byte) : BASICモード(PC-6001の場合は無意味)
		fputc( 1, fp );		// page   (1byte) : ページ数
		fputc( 0, fp );		// askey  (2byte) : オートスタートコマンド文字数
		fputc( 0, fp );		// ...コマンドがある場合はこの後にaskey分続く
		fputc( 0, fp );		// exhead (2byte) : 拡張情報サイズ(64KBまで)
		fputc( 0, fp );		// ...拡張情報がある場合はこの後にexhead分続く
		
		// [DATAブロック]
		fputc( 'T', fp );				// header (2byte) : "TI"
		fputc( 'I', fp );
		fputc( 0, fp );					// id     (1byte) : ID番号(DATAブロックを関連付ける)
		for( int i=0; i<16; i++ ) fputc( 0, fp );	// name  (16byte) : データ名(15文字+'00H')
		FPUTWORD( Baud, fp );			// baud   (2byte) : ボーレート(600/1200)
		fputc( 0x48, fp );				// stime  (2byte) : 無音部の時間(ms)
		fputc( 0x0d, fp );
		fputc( 0x48, fp );				// ptime  (2byte) : ぴー音の時間(ms)
		fputc( 0x0d, fp );
		fputc( 0, fp );					// offset (4byte) : ベタイメージ先頭からのオフセット
		fputc( 0, fp );
		fputc( 0, fp );
		fputc( 0, fp );
		FPUTDWORD( beta, fp );			// size   (4byte) : データサイズ
		
		// [ベタイメージサイズ]
		FPUTDWORD( beta, fp );
		
		fclose( fp );
		fp = NULL;
		*FilePath = '\0';
	}
}


////////////////////////////////////////////////////////////////
// ボーレート設定
////////////////////////////////////////////////////////////////
void CMTS::SetBaud( int b )
{
	Baud = b;
}


////////////////////////////////////////////////////////////////
// CMT 1文字書込み
////////////////////////////////////////////////////////////////
void CMTS::CmtWrite( BYTE data )
{
	if( fp ) fputc( data, fp );
}


////////////////////////////////////////////////////////////////
// I/Oアクセス関数
////////////////////////////////////////////////////////////////
void CMTL::OutB0H( int, BYTE data ){ Remote( data&0x08 ? TRUE : FALSE ); }


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
BOOL CMTL::DokoSave( cIni *Ini )
{
	cSche::evinfo e;
	char stren[16];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	Ini->PutEntry( "TAPE", NULL, "Relay",	"%s",	Relay ? "Yes" : "No" );
	Ini->PutEntry( "TAPE", NULL, "BoostUp",	"%s",	Boost ? "Yes" : "No" );
	
	// イベント
	e.id = EID_TAPE;
	if( vm->sche->GetEvinfo( &e ) ){
		sprintf( stren, "Event%08X", e.id );
		Ini->PutEntry( "TAPE", NULL, stren, "%d %d %d %lf", e.Active ? 1 : 0, e.Period, e.Clock, e.nps );
	}
	
	// TAPEがマウントされてなければ何もしないで戻る
	if( !p6t ) return TRUE;
	
	// マウントされていたらP6TオブジェクトをSAVE
	Ini->PutEntry( "TAPE", NULL, "FilePath",	"%s",	FilePath );
	
	return p6t->DokoSave( Ini );
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
BOOL CMTL::DokoLoad( cIni *Ini )
{
	int yn;
	cSche::evinfo e;
	char stren[16];
	char strrs[64];
	
	e.device = this;
	
	if( !Ini ) return FALSE;
	
	Ini->GetTruth( "TAPE", "Relay",		&Relay,	Relay );
	Ini->GetTruth( "TAPE", "BoostUp",	&Boost,	Boost );
	
	// イベント
	e.id = EID_TAPE;
	sprintf( stren, "Event%08X", e.id );
	if( Ini->GetString( "TAPE", stren, strrs, "" ) ){
		sscanf( strrs,"%d %d %d %lf", &yn, &e.Period, &e.Clock, &e.nps );
		e.Active = yn ? TRUE : FALSE;
		if( !vm->sche->SetEvinfo( &e ) ) return FALSE;
	}
	
	Ini->GetString( "TAPE", "FilePath", FilePath, "" );
	if( *FilePath ){
		if( !Mount( FilePath ) ) return FALSE;
		if( !p6t->DokoLoad( Ini ) ) return FALSE;
	}else
		Unmount();
	
	
	return TRUE;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor CMTL::descriptor = {
	CMTL::indef, CMTL::outdef
};

const Device::OutFuncPtr CMTL::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &CMTL::OutB0H )
};

const Device::InFuncPtr CMTL::indef[] = { NULL };
