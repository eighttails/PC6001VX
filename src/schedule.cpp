#include "pc6001v.h"
#include "config.h"
#include "log.h"
#include "osd.h"
#include "schedule.h"
#include "common.h"

#define WRUPDATE	(1000/SPDCNT)	/* 処理速度更新間隔(ms) */


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
EVSC::EVSC( int mclock ) : VSYNC(false), MasterClock(mclock), NextEvent(-1), SaveClock(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
EVSC::~EVSC( void ){}


////////////////////////////////////////////////////////////////
// イベント検索
//
// 引数:	devid		デバイスオブジェクトID
//			id			イベントID
// 返値:	evinfo *	イベント情報ポインタ(存在しなければNULL)
////////////////////////////////////////////////////////////////
const EVSC::evinfo *EVSC::Find( Device::ID devid, int id ) const
{
	EvMap::const_iterator p = ev.find(std::make_pair(devid, id));
	return p != ev.end() ? &p->second : NULL;
}


////////////////////////////////////////////////////////////////
// 接続するデバイス候補を登録する
//
// 引数:	dev		デバイスオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::Entry( Device *dev )
{
	// 追加&削除してデバイスリストに候補を登録する
	if( !dev || !Add( dev, 0, 0, 0 ) || !Del( dev, 0 ) ) return false;
	return true;

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
bool EVSC::Add( Device *dev, int id, double hz, int flag )
{
	// 登録済みの場合は一旦削除して再登録
	const evinfo *e = Find( dev->GetID(), id );
	if( e ) Del( (Device *)devlist.Find( e->devid ), e->id );

	evinfo& event = ev[std::make_pair(dev->GetID(), id)];
	devlist.Add( dev );

	event.devid  = dev->GetID();
	event.id     = id;

	// イベント発生周波数の設定
	switch( flag&(EV_US|EV_MS|EV_STATE) ){
	case EV_US:		// usで指示
		event.nps    = (double)1000000 / hz;
		event.Clock  = (int)((double)MasterClock / event.nps);
		break;
	case EV_MS:		// msで指示
		event.nps    = (double)1000 / hz;
		event.Clock  = (int)((double)MasterClock / event.nps);
		break;
	case EV_STATE:	// CPUステート数で指示
		event.nps    = (double)MasterClock / hz;
		event.Clock  = (int)hz;
		break;
	default:		// Hzで指示
		event.nps    = hz;
		event.Clock  = (int)((double)MasterClock / hz);
	}

	// 周期の設定
	if( flag&EV_LOOP ){	// ループイベント
		event.Period = event.Clock;
		if( event.Period < 1 ) event.Period = 1;
	}else				// ワンタイムイベント
		event.Period = 0;

	// 次のイベントまでのクロック数更新
	if( NextEvent < 0 ) NextEvent = event.Clock;
	else                NextEvent = min( NextEvent, event.Clock );

	return true;
}


////////////////////////////////////////////////////////////////
// イベント削除
//
// 引数:	dev		デバイスオブジェクトポインタ
//			id		イベントID
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::Del( Device *dev, int id )
{
	if( dev == NULL ) return false;

	evinfo *e = (evinfo *)Find( dev->GetID(), id );
	if( e ){
		devlist.Del( dev );
		ev.erase(std::make_pair( dev->GetID(), id ));
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// イベント発生周波数設定
//
// 引数:	devid	デバイスオブジェクトID
//			id		イベントID
//			hz		イベント発生周波数
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::SetHz( Device::ID devid, int id, double hz )
{
	evinfo *e = (evinfo *)Find( devid, id );
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
void EVSC::Update( int clk )
{
	PRINTD( TIC_LOG, "[SCHE][Update] %d clock\n", clk );
	
	// クロックを溜め込む
	SaveClock  += clk;	// 次のイベント発生用
	
	// 次のイベント発生クロックに達していなかったら戻る
	// ただし clk=0 の場合は更新を行う
	if( NextEvent > SaveClock && clk ) return;
	NextEvent = -1;
	
	int cnt;
	do{
		cnt = 0;
		for( EvMap::iterator p = ev.begin(); p != ev.end(); ++p ){
			evinfo& event = p->second;
			event.Clock -= SaveClock;
			// 更新間隔が長い場合は複数回発生する可能性あり
			// とりあえず全てこなすまで繰り返すってことでいいのか?
			if( event.Clock <= 0 ){
				// イベントコールバックを実行
				devlist.Find( event.devid )->EventCallback( event.id, event.Clock );

				if( event.Period > 0 ){	// ループイベント
					event.Clock += event.Period;
					if( event.Clock <= 0 ) cnt++;	// 次のイベントも発生していたらカウント
				}else{					// ワンタイムイベント
					Del( (Device *)devlist.Find( event.devid ), event.id );
					break;
				}
			}
			// 次のイベントまでのクロック数更新
			if( NextEvent < 0 ) NextEvent = event.Clock;
			else                NextEvent = min( NextEvent, event.Clock );
		}
		SaveClock = 0;
	}while( cnt > 0 );
}


////////////////////////////////////////////////////////////////
// 指定イベントをリセットする
//
// 引数:	devid	デバイスオブジェクトID
//			id		イベントID
//			ini		カウンタ初期値(進行率で指定 1.0=100%)
// 返値:	なし
////////////////////////////////////////////////////////////////
void EVSC::Reset( Device::ID devid, int id, double ini )
{
	PRINTD( TIC_LOG, "[SCHE][Reset] ID:%d %3f%%", id, ini );
	
	evinfo *e = (evinfo *)Find( devid, id );
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
// 引数:	devid	デバイスオブジェクトID
//			id		イベントID
// 返値:	int		残りステート数(IDが無効なら0)
////////////////////////////////////////////////////////////////
int EVSC::Rest( Device::ID devid, int id ) const
{
	const evinfo *e = Find( devid, id );
	if( e ) return e->Clock;
	else    return 0;
}


////////////////////////////////////////////////////////////////
// イベントの進行率を求める
//
// 引数:	devid	デバイスオブジェクトID
//			id		イベントID
// 返値:	double	イベント進行率(1.0=100%)
////////////////////////////////////////////////////////////////
double EVSC::GetProgress( Device::ID devid, int id ) const
{
	PRINTD( TIC_LOG, "[SCHE][GetProgress] ID:%d", id );
	
	const evinfo *e = Find( devid, id );
	// イベントが存在し1周期のクロック数が設定されている?
	if( e && e->Period > 0 ){
		PRINTD( TIC_LOG, " %d/%d SAVE:%d\n", e->Clock, e->Period, SaveClock );
		
		// 溜め込んだクロックを考慮
		double sc = (double)( (double)( e->Period - max( e->Clock - SaveClock, 0 ) ) / (double)e->Period );
		return min( max( 0.0, sc ), 1.0 );
	}else{
		PRINTD( TIC_LOG, " Disable\n" );
	}
	
	return 0;
}


////////////////////////////////////////////////////////////////
// イベント情報取得
//
// 引数:	evinfo *	イベント情報構造体へのポインタ(devid,id をセットしておく)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::GetEvinfo( evinfo *info ) const
{
	if( !info ) return false;
	
	const evinfo *e = Find( info->devid, info->id );
	if( e ){
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
// 引数:	evinfo *	イベント情報構造体へのポインタ(devid,id をセットしておく)
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::SetEvinfo( evinfo *info )
{
	if( !info ) return false;
	
	// 問答無用で追加
	if( !Add( (Device *)devlist.Find( info->devid ), info->id, 1, EV_HZ ) ) return false;
	
	evinfo *e = (evinfo *)Find( info->devid, info->id );
	if( e ){
		e->Period = info->Period;
		e->Clock  = info->Clock;
		e->nps    = info->nps;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// マスタクロック設定
//
// 引数:	clock	マスタクロック数
// 返値:	なし
////////////////////////////////////////////////////////////////
void EVSC::SetMasterClock( int clock )
{
	MasterClock = clock;
	for( EvMap::iterator p = ev.begin(); p != ev.end(); ++p ){
		evinfo& event = p->second;
		if( event.devid && event.nps > 0 && event.Period > 0 ){
			event.Period = (int)((double)clock / event.nps);
			if( event.Period < 1 ) event.Period = 1;
		}
	}
}


////////////////////////////////////////////////////////////////
// マスタクロック取得
//
// 引数:	なし
// 返値:	int		マスタクロック数
////////////////////////////////////////////////////////////////
int EVSC::GetMasterClock( void ) const
{
	return MasterClock;
}


////////////////////////////////////////////////////////////////
// VSYNCに達した?
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
bool EVSC::IsVSYNC( void ) const
{
	return VSYNC;
}


////////////////////////////////////////////////////////////////
// VSYNCを通知する
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EVSC::OnVSYNC( void )
{
	VSYNC = true;
}

////////////////////////////////////////////////////////////////
// VSYNCフラグキャンセル
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void EVSC::ReVSYNC( void )
{
	VSYNC = false;
}
////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "SCHEDULE", NULL, "MasterClock",	"%d",	MasterClock );
	Ini->PutEntry( "SCHEDULE", NULL, "VSYNC",		"%s",	VSYNC ? "Yes" : "No" );
	Ini->PutEntry( "SCHEDULE", NULL, "NextEvent",	"%d",	NextEvent );
	Ini->PutEntry( "SCHEDULE", NULL, "SaveClock",	"%d",	SaveClock );
	
	
	// イベント
	char stren[16];
	int i = 0;
	
	for( EvMap::iterator p = ev.begin(); p != ev.end(); ++p ){
		evinfo& event = p->second;
		BYTE id1,id2,id3,id4;
		DWTOB( event.devid, id4, id3, id2, id1 );
		sprintf( stren, "Event%02X", i );
		Ini->PutEntry( "SCHEDULE", NULL, stren, "%c%c%c%c %d %d %d %lf", id1, id2, id3, id4, event.id, event.Period, event.Clock, event.nps );
		i++;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool EVSC::DokoLoad( cIni *Ini )
{
	if( !Ini ) return false;
	
	// 全てのイベントをひとまず無効にする
	ev.clear();

	Ini->GetInt(    "SCHEDULE", "MasterClock",	&MasterClock,	MasterClock );
	Ini->GetTruth(  "SCHEDULE", "VSYNC",		&VSYNC, 		VSYNC );
	Ini->GetInt(    "SCHEDULE", "NextEvent",	&NextEvent,		NextEvent );	// イベント再設定時に再現される?
	Ini->GetInt(    "SCHEDULE", "SaveClock",	&SaveClock,		SaveClock );
	
	
	// イベント
	char stren[16];
	char strrs[64];
	
	for( size_t i=0; i<MAXEVENT; i++ ){
		sprintf( stren, "Event%02X", i );
		if( Ini->GetString( "SCHEDULE", stren, strrs, "" ) ){
			evinfo e;
			BYTE id1,id2,id3,id4;
			
			sscanf( strrs,"%c%c%c%c %d %d %d %lf", &id1, &id2, &id3, &id4, &e.id, &e.Period, &e.Clock, &e.nps );
			e.devid = BTODW( id1, id2, id3, id4 );
			if( !SetEvinfo( &e ) ) return false;
		}else
			break;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
SCH6::SCH6( int mclock ) : WaitEnable(true), PauseEnable(false),
	EnableScrUpdate(0),	SpeedRatio(100), SpeedCnt1(1), SpeedCnt2(1), MasterClock(mclock)
{
	INITARRAY( WRClock, 0 );
}


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
	int now,last;

	EnableScrUpdate = 0;

	ti = STATIC_CAST( SCH6 *, inst );	// 自分自身のオブジェクトポインタ取得

	// 1秒間のインターバル設定
	for( int i=0; i<VSYNC_HZ; i++ ) Vint[i] = (int)( 1000 / VSYNC_HZ );
	int Vrem = 1000 - (int)( 1000 / VSYNC_HZ ) * VSYNC_HZ;
	for( int i=0; i<Vrem; i++ ) Vint[(int)(VSYNC_HZ * i / Vrem)]++;

	// 最初の待ち時間を設定
	now  = OSD_GetTicks();
	last = now;
	int NextWait = now + Vint[VintCnt++];

	while( !this->cThread::IsCancel() ){
		// TILTモード
		UpdateTilt();

		// エミュレーション本体の処理が溜まっていなければ画面更新フラグを立てる
		if (!cSemaphore::Value()) EnableScrUpdate = 1;

		NextWait += Vint[VintCnt++];
		if( VintCnt >= VSYNC_HZ ){
			VintCnt -= VSYNC_HZ;
			// １秒間に１回は強制的に画面を更新
			EnableScrUpdate = 1;
		}
		ti->WaitReset();

#ifndef NOJOYSTICK
		// ジョイスティックをポーリング
		OSD_PushEvent(EV_JOYAXISMOTION);
#endif
		now = int(OSD_GetTicks());
		OSD_Delay( max( NextWait - now, 0 ));

		if( now - last >= WRUPDATE ){
			for( int i=SPDCNT-1; i>0; i-- )
				WRClock[i] = WRClock[i-1];
			WRClock[0] = 0;
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
	
	INITARRAY( WRClock, 0 );
}


////////////////////////////////////////////////////////////////
// Wait有効フラグ取得
//
// 引数:	なし
// 返値:	有効:true 無効:false
////////////////////////////////////////////////////////////////
bool SCH6::GetWaitEnable( void ) const
{
	return WaitEnable;
}


////////////////////////////////////////////////////////////////
// Wait有効フラグ設定
//
// 引数:	en		Wait有効フラグ 有効:true 無効:false
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::SetWaitEnable( bool en )
{
	WaitEnable = en;
}


////////////////////////////////////////////////////////////////
// ポーズ有効フラグ取得
//
// 引数:	なし
// 返値:	有効:true 無効:false
////////////////////////////////////////////////////////////////
bool SCH6::GetPauseEnable( void ) const
{
	return PauseEnable;
}


////////////////////////////////////////////////////////////////
// ポーズ有効フラグ設定
//
// 引数:	en		ポーズ有効フラグ 有効:true 無効:false
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::SetPauseEnable( bool en )
{
	PauseEnable = en;
}


////////////////////////////////////////////////////////////////
// VSYNC Wait
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::VWait( void )
{
	if( !WaitEnable ) return;
	
	SpeedCnt1++;
	
	if( (SpeedRatio > 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 < SpeedRatio ) )
		return;
	
	cSemaphore::Wait();
}


////////////////////////////////////////////////////////////////
// Waitを解除する
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::WaitReset( void )
{
	if( (SpeedCnt1 > 0xffffff) || (SpeedCnt2 > 0xffffff) ){
		SpeedCnt1 /= 2;
		SpeedCnt2 /= 2;
	}
	
	SpeedCnt2++;
	
	if( (SpeedRatio < 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 >= SpeedRatio ) )
		return;
	
	//10フレーム分くらいまでためられるようにする
	if( !(cSemaphore::Value() > 10) ) cSemaphore::Post();
	
}


////////////////////////////////////////////////////////////////
// 実行速度設定
//
// 引数:	spd		+:UP -:DOWN 0:等速
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::SetSpeedRatio( int spd )
{
	if     ( spd > 0 && SpeedRatio < 2000 ) SpeedRatio += SpeedRatio <  200 ? 10 : 100;
	else if( spd < 0 && SpeedRatio >   10 ) SpeedRatio -= SpeedRatio <= 200 ? 10 : 100;
	else if( spd == 0 )                     SpeedRatio = 100;
	
	SpeedCnt2 = VSYNC_HZ * 1000;
	SpeedCnt1 = (SpeedCnt2 * SpeedRatio) / 100;
}


////////////////////////////////////////////////////////////////
// 実行速度取得
//
// 引数:	なし
// 返値:	int		実行速度(%)
////////////////////////////////////////////////////////////////
int SCH6::GetSpeedRatio( void ) const
{
	return SpeedRatio;
}


////////////////////////////////////////////////////////////////
// 実行速度比取得
//
// 引数:	なし
// 返値:	int		実行速度比(%)(等倍が100)
////////////////////////////////////////////////////////////////
int SCH6::GetRatio( void ) const
{
	DWORD sum = 0;
	
	for( int i=1; i<SPDCNT; i++ )
		sum += WRClock[i];
	
	return (int)(double)( sum * 100.0 / MasterClock * 1000.0 / (WRUPDATE*(SPDCNT-1)) + 0.5 );
}


////////////////////////////////////////////////////////////////
// イベント更新
//
// 引数:	clk		進めるクロック数
// 返値:	なし
////////////////////////////////////////////////////////////////
void SCH6::Update( int clk )
{
	WRClock[0] += clk;	// 実行速度計算用
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
	EVSC::SetMasterClock( CpuState );
}
*/


