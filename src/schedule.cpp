#include "pc6001v.h"
#include "config.h"
#include "log.h"
#include "schedule.h"
#include "osd.h"


#define WRUPDATE	(1000)	/* 処理速度更新間隔(ms) */


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cSche::cSche( int mclock ) : VSYNC(false), MasterClock(mclock), NextEvent(-1), SaveClock(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cSche::~cSche( void ){}


////////////////////////////////////////////////////////////////
// イベント検索
//
// 引数:	dev			デバイスオブジェクトポインタ
//			id			イベントID
// 返値:	evinfo *	イベント情報ポインタ(存在しなければNULL)
////////////////////////////////////////////////////////////////
cSche::evinfo *cSche::Find( P6DEVICE *dev, int id )
{
	for( int i=0; i<MAXEVENT; i++ )
		if( ev[i].device == dev && ev[i].id == id ) return &ev[i];
	
	return NULL;
}


////////////////////////////////////////////////////////////////
// マスタクロック数を設定
//
// 引数:	clock	マスタクロック数
// 返値:	なし
////////////////////////////////////////////////////////////////
void cSche::SetMasterClock( int clock )
{
	MasterClock = clock;
	for( int i=0; i<MAXEVENT; i++ )
		if( ev[i].device && ev[i].nps > 0 && ev[i].Period > 0 ){
			ev[i].Period = (int)((double)clock / ev[i].nps);
			if( ev[i].Period < 1 ) ev[i].Period = 1;
		}

}


////////////////////////////////////////////////////////////////
// イベント追加
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
//			hz		イベント発生周波数
//			loop	ループ指定 true:ループ false:ワンタイム
//			flag	イベントスタイル指定
//				bit0  : 繰り返し指示 0:ワンタイム 1:ループ
//				bit2,3,4: 周期指定単位 000:Hz 001:us 010:ms 100:CPUステート数
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::Add( P6DEVICE *dev, int id, double hz, int flag )
{
	// 登録済みの場合は一旦削除して再登録
	evinfo *e = Find( dev, id );
	if( e ) Del( e->device, e->id );
	
	for( int i=0; i<MAXEVENT; i++ ){
		if( !ev[i].device ){
			ev[i].device = dev;
			ev[i].id     = id;
			ev[i].Active = true;
			
			// イベント発生周波数の設定
			switch( flag&(EV_US|EV_MS|EV_STATE) ){
			case EV_US:		// usで指示
				ev[i].nps    = (double)1000000 / hz;
				ev[i].Clock  = (int)((double)MasterClock / ev[i].nps);
				break;
			case EV_MS:		// msで指示
				ev[i].nps    = (double)1000 / hz;
				ev[i].Clock  = (int)((double)MasterClock / ev[i].nps);
				break;
			case EV_STATE:	// CPUステート数で指示
				ev[i].nps    = (double)MasterClock / hz;
				ev[i].Clock  = (int)hz;
				break;
			default:		// Hzで指示
				ev[i].nps    = hz;
				ev[i].Clock  = (int)((double)MasterClock / hz);
			}
			
			// 周期の設定
			if( flag&EV_LOOP ){	// ループイベント
				ev[i].Period = ev[i].Clock;
				if( ev[i].Period < 1 ) ev[i].Period = 1;
			}else				// ワンタイムイベント
				ev[i].Period = 0;
			
			// 次のイベントまでのクロック数更新
			if( NextEvent < 0 ) NextEvent = ev[i].Clock;
			else                NextEvent = min( NextEvent, ev[i].Clock );
			
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////
// イベント削除
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::Del( P6DEVICE *dev, int id )
{
	evinfo *e = Find( dev, id );
	if( e ){
		e->device = NULL;
		e->Active = false;
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// イベント発生周波数設定
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
//			hz		イベント発生周波数
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::SetHz( P6DEVICE *dev, int id, double hz )
{
	evinfo *e = Find( dev, id );
	if( e ){
		e->nps    = hz;
		e->Period = (int)((double)MasterClock / hz);
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// イベント更新
//
// 引数:	clk		進めるクロック数
// 返値:	なし
////////////////////////////////////////////////////////////////
void cSche::Update( int clk )
{
//	PRINTD( TIC_LOG, "[SCHE][Update] %d clock\n", clk );
	
	// クロックを溜め込む
	SaveClock  += clk;	// 次のイベント発生用
//	WRClockTmp += clk;	// 実行速度計算用
	
	// 次のイベント発生クロックに達していなかったら戻る
	// ただし clk=0 の場合は更新を行う
	if( NextEvent > SaveClock && clk ) return;
	NextEvent = -1;
	
	int cnt;
	do{
		cnt = 0;
		for( int i=0; i<MAXEVENT; i++ ){
			// 有効なイベント?
			if( ev[i].Active ){
				ev[i].Clock -= SaveClock;
				// 更新間隔が長い場合は複数回発生する可能性あり
				// とりあえず全てこなすまで繰り返すってことでいいのか?
				if( ev[i].Clock <= 0 ){
					// イベントコールバックを実行
					ev[i].device->EventCallback( ev[i].id, ev[i].Clock );
					
					if( ev[i].Period > 0 ){	// ループイベント
						ev[i].Clock += ev[i].Period;
						if( ev[i].Clock <= 0 ) cnt++;	// 次のイベントも発生していたらカウント
					}else{					// ワンタイムイベント
						Del( ev[i].device, ev[i].id );
						break;
					}
				}
				// 次のイベントまでのクロック数更新
				if( NextEvent < 0 ) NextEvent = ev[i].Clock;
				else                NextEvent = min( NextEvent, ev[i].Clock );
			}
		}
		SaveClock = 0;
	}while( cnt > 0 );
}


////////////////////////////////////////////////////////////////
// 指定イベントをリセットする
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
//			ini		カウンタ初期値(進行率で指定 1.0=100%)
// 返値:	なし
////////////////////////////////////////////////////////////////
void cSche::Reset( P6DEVICE *dev, int id, double ini )
{
	PRINTD( TIC_LOG, "[SCHE][Reset] ID:%d %3f%", id, ini );
	
	evinfo *e = Find( dev, id );
	if( e ){
		// 溜め込んだクロック分を考慮
		e->Clock = (int)((double)e->Period * ( 1.0 - min( max( 0.0, ini ), 1.0 ) )) - SaveClock;
		
		PRINTD( TIC_LOG, " -> %d/%d clock", e->Clock, e->Period );
	}
	
	PRINTD( TIC_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// イベント発生までの残りクロック数を求める
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
// 返値:	int		残りステート数(IDが無効なら0)
////////////////////////////////////////////////////////////////
int cSche::Rest( P6DEVICE *dev, int id )
{
	evinfo *e = Find( dev, id );
	if( e ) return e->Clock;
	else    return 0;
}


////////////////////////////////////////////////////////////////
// イベントの進行率を求める
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
// 返値:	double	イベント進行率(1.0=100%)
////////////////////////////////////////////////////////////////
double cSche::Scale( P6DEVICE *dev, int id )
{
	PRINTD( TIC_LOG, "[SCHE][Scale] ID:%d", id );
	
	evinfo *e = Find( dev, id );
	// イベントが存在し1周期のクロック数が設定されている?
	if( e && e->Period > 0 ){
		PRINTD( TIC_LOG, " %d/%d SAVE:%d\n", e->Clock, e->Period, SaveClock );
		
		// 溜め込んだクロックを考慮
		double sc = (double)( (double)( e->Period - max( e->Clock - SaveClock, 0 ) ) / (double)e->Period );
		return min( max( 0.0, sc ), 1.0 );
	}else
		return 0;
}


////////////////////////////////////////////////////////////////
// イベント情報取得
//
// 引数:	evinfo *	イベント情報構造体へのポインタ(device,id をセットしておく)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::GetEvinfo( evinfo *info )
{
	if( !info ) return false;
	
	evinfo *e = Find( info->device, info->id );
	if( e ){
		info->Active = e->Active;
		info->Period = e->Period;
		info->Clock  = e->Clock;
		info->nps    = e->nps;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// イベント情報設定
//
// 引数:	evinfo *	イベント情報構造体へのポインタ
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::SetEvinfo( evinfo *info )
{
	if( !info ) return false;
	
	// 問答無用で追加
	if( !Add( info->device, info->id, 1, EV_HZ ) ) return false;
	
	evinfo *e = Find( info->device, info->id );
	if( e ){
		e->Active = info->Active;
		e->Period = info->Period;
		e->Clock  = info->Clock;
		e->nps    = info->nps;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// マスタークロック取得
//
// 引数:	なし
// 返値:	int		マスタークロック数
////////////////////////////////////////////////////////////////
int cSche::GetMasterClock( void )
{
	return MasterClock;
}


////////////////////////////////////////////////////////////////
// VSYNCに達した?
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
bool cSche::IsVSYNC( void )
{
	return VSYNC;
}


////////////////////////////////////////////////////////////////
// VSYNCを通知する
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void cSche::OnVSYNC( void )
{
	VSYNC = true;
}

////////////////////////////////////////////////////////////////
// VSYNCフラグキャンセル
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void cSche::ReVSYNC( void )
{
	VSYNC = false;
}
////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "SCHEDULE", NULL, "MasterClock",	"%d",	MasterClock );
	Ini->PutEntry( "SCHEDULE", NULL, "VSYNC",		"%s",	VSYNC ? "Yes" : "No" );
	Ini->PutEntry( "SCHEDULE", NULL, "NextEvent",	"%d",	NextEvent );
	Ini->PutEntry( "SCHEDULE", NULL, "SaveClock",	"%d",	SaveClock );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool cSche::DokoLoad( cIni *Ini )
{
	if( !Ini ) return false;
	
	// 全てのイベントをひとまず無効にする
	// イベント情報は各モジュール毎に再設定する
	for( int i=0; i<MAXEVENT; i++ ){
		ev[i].device = NULL;
		ev[i].Active = false;
	}
	
	Ini->GetInt(    "SCHEDULE", "MasterClock",	&MasterClock,	MasterClock );
	Ini->GetTruth(  "SCHEDULE", "VSYNC",		&VSYNC, 		VSYNC );
	Ini->GetInt(    "SCHEDULE", "NextEvent",	&NextEvent,		NextEvent );	// イベント再設定時に再現される?
	Ini->GetInt(    "SCHEDULE", "SaveClock",	&SaveClock,		SaveClock );
	
	return true;
}









////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SCH6::SCH6( int mclock ) : WaitEnable(true), EnableScrUpdate(0),
	MasterClock(mclock), WRClock(0), WRClockTmp(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
SCH6::~SCH6( void )
{
	Stop();
}


////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
//   OSD_Delay()の精度に左右されるのでけっこういい加減かも
//   49.7日以上連続稼動したOSでは不具合が生じるけど．．．いいでしょ
//
// 引数:	inst	自分自身のオブジェクトポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::OnThread( void *inst )
{
    SCH6 *ti;
	int Vint[VSYNC_HZ];
	int VintCnt = 0;
    DWORD now,last;

	EnableScrUpdate = 0;
	
    ti = STATIC_CAST( SCH6 *, inst );	// 自分自身のオブジェクトポインタ取得
	
	// 1秒間のインターバル設定
	for( int i=0; i<VSYNC_HZ; i++ ) Vint[i] = (int)( 1000 / VSYNC_HZ );
	int Vrem = 1000 - (int)( 1000 / VSYNC_HZ ) * VSYNC_HZ;
	for( int i=0; i<Vrem; i++ ) Vint[(int)(VSYNC_HZ * i / Vrem)]++;
	

    while( !this->cThread::IsCancel() ){
        DWORD start = OSD_GetTicks();
        ti->WaitReset();
        DWORD end = OSD_GetTicks();

        OSD_Delay( Vint[VintCnt++] - (end - start) );
        if( VintCnt >= VSYNC_HZ ) {
            VintCnt = 0;
            WRClock     = WRClockTmp;
            WRClockTmp  = 0;
            last       += WRUPDATE;
        }
    }
}


////////////////////////////////////////////////////////////////
// 動作開始
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
bool SCH6::Start( void )
{
	// スレッド生成
	if( !this->cThread::BeginThread( this ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// 動作停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::Stop( void )
{
	this->cThread::Cancel();	// スレッド終了フラグ立てる
	this->cThread::Waiting();	// スレッド終了まで待つ
	
	WRClock = WRClockTmp = 0;
}


////////////////////////////////////////////////////////////////
// Wait有効フラグ取得
//
// 引数:	なし
// 返値:	有効:true 無効:false
////////////////////////////////////////////////////////////////
bool SCH6::GetWaitEnable( void )
{
	return WaitEnable;
}


////////////////////////////////////////////////////////////////
// Wait有効フラグ設定
//
// 引数:	Wait有効フラグ 有効:true 無効:false
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::SetWaitEnable( bool we )
{
	WaitEnable = we;
}


////////////////////////////////////////////////////////////////
// VSYNC Wait
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::VWait( void )
{
	if( WaitEnable ) cSemaphore::Wait();
}


////////////////////////////////////////////////////////////////
// Waitを解除する
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::WaitReset( void )
{
	if( !cSemaphore::Value() ) cSemaphore::Post();
	
	// 画面更新フラグを立てる
	EnableScrUpdate++;
}


////////////////////////////////////////////////////////////////
// 実行速度比取得
//
// 引数:	なし
// 返値:	int		実行速度比(%)(等倍が100)
////////////////////////////////////////////////////////////////
int SCH6::GetRatio( void )
{
	return (int)(double)( WRClock * 100.0 / MasterClock * 1000.0 / WRUPDATE + 0.5 );
}


////////////////////////////////////////////////////////////////
// イベント更新
//
// 引数:	clk		進めるクロック数
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::Update( int clk )
{
	WRClockTmp += clk;	// 実行速度計算用
}


////////////////////////////////////////////////////////////////
// 画面更新時期を迎えた?
//
// 引数:	なし
// 返値:	bool		true:更新する false:更新しない
////////////////////////////////////////////////////////////////
bool SCH6::IsScreenUpdate( void )
{
	if( EnableScrUpdate ){
		EnableScrUpdate--;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// ** 結局使わなくなりました **
// 画面の表示状態に応じてCPUステート数を計算
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
// PC-6001mk2SRのマニュアルから引用 (なぜにSR? たまたま手元にこれしかなかったから)

// 表示周波数
// 水平 15.70kHz(63.70usec)
// 垂直 59.92 Hz(16.69msec)

// 水平タイミング (単位:usec)
// 表示モード 水平周期 バックポーチ 表示期間 フロントポーチ  計
// 32文字		4.87	13.74		35.76		9.33		63.70
// 40,80文字	4.87	 9.27		44.69		4.86		63.69

// 垂直タイミング (単位:msec)
// 表示モード 垂直周期 バックポーチ 表示期間 フロントポーチ  計
// 192H			0.25	 2.49		12.23		1.72		16.69
// 200H			0.25	 2.23		12.74		1.47		16.69
// 204H			0.25	 2.11		12.99		1.34		16.69

// ドットクロック,ライン数で置き換える
// 水平タイミング
// 表示モード 水平周期 バックポーチ 表示期間 フロントポーチ  計
// 32文字		34			98			256		67			455
// 40,80文字	34	 		66			320		35			455

// 垂直タイミング
// 表示モード 垂直周期 バックポーチ 表示期間 フロントポーチ  計
// 192H			3			34			192		33			262
// 200H			3		 	30			200		29			262
// 204H			4(?)		33			204		21			262


// まとめるとこんな感じ
// 水平トータル期間   455 Clk (ドットクロック 7.16MHz? 3.58MHzの両エッジ?)
// 水平表示期間       256 320 Clk
// 水平非表示期間     199 135 Clk
// 垂直トータルライン 262 Line
// 垂直表示ライン     192 200 204 Line
// 垂直非表示ライン    70  62  58 Line

// [表示期間]のみCPU停止と考えると，稼働率はこんな感じ
// でもSRはそんなに止まらないらしい
// N60   : 58.769% ( 199*192 + 455*70 ) / ( 262*455 )
// N60m  : 46.313% ( 135*200 + 455*62 ) / ( 262*455 )
// N66SR : 45.239% ( 135*204 + 455*58 ) / ( 262*455 )

// CPUクロックで考えると
// CPUクロック/s      3993600
// CPUクロック/VSYNC    66560 (VSYNC:60Hzとする)
// CPUクロック/1Line     1109 (66560/262)
//   稼動:停止/1Line  486:623 331:778
// CPUクロック/VBlank  146368  155240

/*
#define	Ref1	(double)( (double)( 199*192 + 455*70 ) / (double)( 262*455 ) )
#define	Ref2	(double)( (double)( 135*200 + 455*62 ) / (double)( 262*455 ) )

void SCH6::CalcCpuState( void )
{
	bool disp = vm->vdg->GetCrtDisp();
	bool win  = vm->vdg->GetWinSize();
	
	// 1秒あたりのステート数を求める
	int CpuState;
	if( disp ) CpuState = (int)( (win ? Ref1 : Ref2) * (double)CpuClock );
	else       CpuState = CpuClock;

CpuState = CpuClock;
	
	// このままだと速すぎるので適当に調整
//	CpuState = CpuState * 86 / 100;
	
	// マスタクロック数を設定
	cSche::SetMasterClock( CpuState );
}
*/


