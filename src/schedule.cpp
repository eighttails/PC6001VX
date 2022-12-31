/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include "pc6001v.h"

#include "common.h"
#include "config.h"
#include "log.h"
#include "osd.h"
#include "schedule.h"


#define SPDSEC		(2)									// 処理速度計算期間(sec)
#define	SPDCNT		(10)								// 処理速度計算用保存領域サイズ(1000を割切れる数)
#define WRUPDATE	(SPDSEC*1000/SPDCNT)				// 処理速度更新間隔(ms)

#define FRMTICK		((double)1000.0/(double)VSYNC_HZ);	// 1フレームの時間(ms)
#define FPSSEC		(2.0)								// FPS計算期間(sec)
#define FPSCNT		((size_t)(VSYNC_HZ*FPSSEC+1.0))		// FPS計算用保存領域サイズ


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
EVSC::EVSC( void ) : VSYNC( false ), MasterClock( 0 ), NextEvent( -1 ), SaveClock( 0 )
{
	PRINTD( CONST_LOG, "[[EVSC]]\n" );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
EVSC::~EVSC( void )
{
	PRINTD( CONST_LOG, "[[~EVSC]]\n" );
}


/////////////////////////////////////////////////////////////////////////////
// イベント検索
//
// 引数:	devid		デバイスオブジェクトID
//			eid			イベントID
// 返値:	evinfo*		イベント情報ポインタ(存在しなければnullptr)
/////////////////////////////////////////////////////////////////////////////
const EVSC::evinfo* EVSC::Find( Device::ID devid, int eid ) const
{
	for( auto &i : ev ){
		if( i.devid == devid && i.id == eid ){
			return &i;
		}
	}
	return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
// 接続するデバイス候補を登録する
//
// 引数:	dev		デバイスオブジェク配列
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::Entry( std::vector<std::shared_ptr<IDevice>> dev )
{
	for( auto &i : dev ){
		if( i && !devlist.Add( i ) ){
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// イベント追加
//
// 引数:	did		デバイスオブジェクトID
//			eid		イベントID
//			hz		イベント発生周波数
//			loop	ループ指定 true:ループ false:ワンタイム
//			flag	イベントスタイル指定
//				bit0  : 繰り返し指示 0:ワンタイム 1:ループ
//				bit2,3,4: 周期指定単位 000:Hz 001:us 010:ms 100:CPUステート数
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::Add( Device::ID did, int eid, double hz, int flag )
{
	PRINTD( TIC_LOG, "[SCHE][Add] :%c%c%c%c ID:%d\n", (char)(did & 0xff), (char)(did >> 8) & 0xff, (char)(did >> 16) & 0xff, (char)(did >> 24), eid );
	
	// 登録済みの場合は一旦削除して再登録
	const evinfo* e = Find( did, eid );
	if( e ){ Del( e->devid, e->id ); }
	
	evinfo event;
	event.devid = did;
	event.id    = eid;
	
	// イベント発生周波数の設定
	switch( flag & (EV_US | EV_MS | EV_STATE) ){
	case EV_US:		// usで指示
		event.nps   = (double)1000000 / hz;
		event.Clock = (int)((double)MasterClock / event.nps);
		break;
	case EV_MS:		// msで指示
		event.nps   = (double)1000 / hz;
		event.Clock = (int)((double)MasterClock / event.nps);
		break;
	case EV_STATE:	// CPUステート数で指示
		event.nps   = (double)MasterClock / hz;
		event.Clock = (int)hz;
		break;
	default:		// Hzで指示
		event.nps   = hz;
		event.Clock = (int)((double)MasterClock / hz);
	}
	
	// 周期の設定
	if( flag & EV_LOOP ){	// ループイベント
		event.Period = event.Clock;
		if( event.Period < 1 ){ event.Period = 1; }
	}else{				// ワンタイムイベント
		event.Period = 0;
	}
	
	// 次のイベントまでのクロック数更新
	if( NextEvent < 0 ){ NextEvent = event.Clock; }
	else               { NextEvent = min( NextEvent, event.Clock ); }
	
	// イベント追加
	ev.push_back( event );
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// イベント削除
//
// 引数:	did		デバイスオブジェクトID
//			eid		イベントID
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::Del( Device::ID did, int eid )
{
	PRINTD( TIC_LOG, "[SCHE][Del] DevID:%c%c%c%c ID:%d\n", (char)(did & 0xff), (char)(did >> 8) & 0xff, (char)(did >> 16) & 0xff, (char)(did >> 24), eid );
	
	evinfo* e = (evinfo*)Find( did, eid );
	if( !e ){
		return false;
	}
	
	// 一旦無効化
	e->devid = 0;
	e->id    = 0;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// イベント更新
//
// 引数:	clk		進めるクロック数
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EVSC::Update( int clk )
{
	PRINTD( TIC_LOG, "[SCHE][Update] %d clock\n", clk );
	
	// イベントがなければ無視
	if( ev.empty() ){
		return;
	}
	
	// クロックを溜め込む
	SaveClock += clk;	// 次のイベント発生用
	
	// 次のイベント発生クロックに達していなかったら戻る
	// ただし clk=0 の場合は更新を行う
	if( NextEvent > SaveClock && clk ){
		return;
	}
	NextEvent = -1;
	
	int cnt,num;
	do{
		cnt = 0;
		num = ev.size();
		auto p = ev.begin();
		for( int i = 0; i < num; i++ ){
			// 有効なイベント?
			if( (*p).devid && (*p).id ){
				(*p).Clock -= SaveClock;
				// 更新間隔が長い場合は複数回発生する可能性あり
				// とりあえず全てこなすまで繰り返すってことでいいのか?
				if( (*p).Clock <= 0 ){
					// イベントコールバックを実行
					std::shared_ptr<IDevice>& dev = devlist.Find( (*p).devid );
					if( dev ) dev->EventCallback( (*p).id, (*p).Clock );
					
					// コールバック関数中でイベントが追加・削除される可能性があるためイテレータ再設定
					// Add : 必ず末尾に追加される
					// Del : すぐには削除せず一旦無効化
					p = ev.begin()+i;
					
					if( (*p).Period > 0 ){	// ループイベント
						(*p).Clock += (*p).Period;
						if( (*p).Clock <= 0 ){ cnt++; }	// 次のイベントも発生していたらカウント
					}else{					// ワンタイムイベント
						if( (*p).devid && (*p).id ){ Del( (*p).devid, (*p).id ); }
					}
				}
				// 次のイベントまでのクロック数更新
				if( NextEvent < 0 ){ NextEvent = (*p).Clock; }
				else               { NextEvent = min( NextEvent, (*p).Clock ); }
			}
			p++;
		}
		SaveClock = 0;
		
		// 不要イベント削除
		p = ev.begin();
		while( p != ev.end() ){
			if( !(*p).devid && !(*p).id ){ p = ev.erase( p ); }
			else						 { p++; }
		}
		
	}while( cnt > 0 );
}


/////////////////////////////////////////////////////////////////////////////
// 指定イベントをリセットする
//
// 引数:	devid	デバイスオブジェクトID
//			eid		イベントID
//			ini		カウンタ初期値(進行率で指定 1.0=100%)
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EVSC::Reset( Device::ID devid, int eid, double ini )
{
	PRINTD( TIC_LOG, "[SCHE][Reset] DevID:%c%c%c%c ID:%d %3f%%", (char)(devid & 0xff), (char)(devid >> 8) & 0xff, (char)(devid >> 16) & 0xff, (char)(devid >> 24), eid, ini );
	
	evinfo* e = (evinfo*)Find( devid, eid );
	if( !e ){
		PRINTD( TIC_LOG, "\n" );
		return;
	}
	
	// 溜め込んだクロック分を考慮
	e->Clock = (int)((double)e->Period * ( 1.0 - min( max( 0.0, ini ), 1.0 ) )) - SaveClock;
	PRINTD( TIC_LOG, " -> %d/%d clock\n", e->Clock, e->Period );
}


/////////////////////////////////////////////////////////////////////////////
// イベント発生までの残りクロック数を求める
//
// 引数:	devid	デバイスオブジェクトID
//			eid		イベントID
// 返値:	int		残りステート数(IDが無効なら0)
/////////////////////////////////////////////////////////////////////////////
int EVSC::Rest( Device::ID devid, int eid ) const
{
	const evinfo* e = Find( devid, eid );
	return e ? e->Clock : 0;
}


/////////////////////////////////////////////////////////////////////////////
// イベントの進行率を求める
//
// 引数:	devid	デバイスオブジェクトID
//			eid		イベントID
// 返値:	double	イベント進行率(1.0=100%)
/////////////////////////////////////////////////////////////////////////////
double EVSC::GetProgress( Device::ID devid, int eid ) const
{
	PRINTD( TIC_LOG, "[SCHE][GetProgress] DevID:%c%c%c%c ID:%d", (char)(devid & 0xff), (char)(devid >> 8) & 0xff, (char)(devid >> 16) & 0xff, (char)(devid >> 24), eid );
	
	const evinfo* e = Find( devid, eid );
	// イベントが存在し1周期のクロック数が設定されている?
	if( !e || e->Period <= 0 ){
		PRINTD( TIC_LOG, " Disable\n" );
		return 0;
	}
	
	PRINTD( TIC_LOG, " %d/%d SAVE:%d\n", e->Clock, e->Period, SaveClock );
	
	// 溜め込んだクロックを考慮
	double sc = (double)( (double)( e->Period - max( e->Clock - SaveClock, 0 ) ) / (double)e->Period );
	return min( max( 0.0, sc ), 1.0 );
}


/////////////////////////////////////////////////////////////////////////////
// イベント情報取得
//
// 引数:	evinfo		イベント情報構造体へのポインタ(devid,id をセットしておく)
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::GetEvinfo( evinfo* info ) const
{
	if( !info ){
		return false;
	}
	
	const evinfo* e = Find( info->devid, info->id );
	if( !e ){
		return false;
	}
	
	info->Period = e->Period;
	info->Clock  = e->Clock;
	info->nps    = e->nps;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// イベント情報設定
//
// 引数:	evinfo		イベント情報構造体へのポインタ(devid,id をセットしておく)
// 返値:	bool		true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::SetEvinfo( evinfo* info )
{
	if( !info ){
		return false;
	}
	
	// 問答無用で追加
	if( !Add( info->devid, info->id, 1, EV_HZ ) ){
		return false;
	}
	
	evinfo* e = (evinfo*)Find( info->devid, info->id );
	if( !e ){
		return false;
	}
	
	e->Period = info->Period;
	e->Clock  = info->Clock;
	e->nps    = info->nps;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// マスタクロック設定
//
// 引数:	clock	マスタクロック数
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EVSC::SetMasterClock( int clock )
{
	MasterClock = clock;

	for( auto &i : ev ){
		if( i.devid && i.nps > 0 && i.Period > 0 ){
			i.Period = (int)((double)clock / i.nps);
			if( i.Period < 1 ){ i.Period = 1; }
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// マスタクロック取得
//
// 引数:	なし
// 返値:	int		マスタクロック数
/////////////////////////////////////////////////////////////////////////////
int EVSC::GetMasterClock( void ) const
{
	return MasterClock;
}


/////////////////////////////////////////////////////////////////////////////
// VSYNCに達した?
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
bool EVSC::IsVSYNC( void ) const
{
	return VSYNC;
}


/////////////////////////////////////////////////////////////////////////////
// VSYNCを通知する
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EVSC::OnVSYNC( void )
{
	VSYNC = true;
}


/////////////////////////////////////////////////////////////////////////////
// VSYNCフラグキャンセル
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void EVSC::ReVSYNC( void )
{
	VSYNC = false;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::DokoSave( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	Ini->SetVal( "SCHEDULE", "MasterClock",	"", MasterClock );
	Ini->SetVal( "SCHEDULE", "VSYNC",		"", VSYNC );
	Ini->SetVal( "SCHEDULE", "NextEvent",	"", NextEvent );
	Ini->SetVal( "SCHEDULE", "SaveClock",	"", SaveClock );
	
	
	// イベント
	int i = 0;
	for( auto &p : ev ){
		if( p.devid && p.id ){
			BYTE id1,id2,id3,id4;
			
			DWTOB( p.devid, id4, id3, id2, id1 );
			Ini->SetEntry( "SCHEDULE", Stringf( "Event%02X", i++ ), "", "%c%c%c%c %d %d %d %lf", id1, id2, id3, id4, p.id, p.Period, p.Clock, p.nps );
		}
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool EVSC::DokoLoad( cIni* Ini )
{
	if( !Ini ){
		return false;
	}
	
	// 全てのイベントをひとまず無効にする
	ev.clear();
	
	Ini->GetVal( "SCHEDULE", "MasterClock",	MasterClock );
	Ini->GetVal( "SCHEDULE", "VSYNC",		VSYNC       );
	Ini->GetVal( "SCHEDULE", "NextEvent",	NextEvent   );	// イベント再設定時に再現される?
	Ini->GetVal( "SCHEDULE", "SaveClock",	SaveClock   );
	
	
	// イベント
	for( int i = 0; ; i++ ){
		std::string str;
		evinfo e;
		BYTE id1,id2,id3,id4;
		
		if( !Ini->GetEntry( "SCHEDULE", Stringf( "Event%02X", i ), str ) ){
			break;
		}
		sscanf( str.c_str(), "%c%c%c%c %d %d %d %lf", &id1, &id2, &id3, &id4, &e.id, &e.Period, &e.Clock, &e.nps );
		e.devid = BTODW( id1, id2, id3, id4 );
		if( e.devid && e.id && !SetEvinfo( &e ) ){
			return false;
		}
	}
	
	return true;
}









/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
SCH6::SCH6( void ) : WaitEnable( true ), PauseEnable( false ), EnableScrUpdate( 0 ),
					 SpeedRatio( 100 ), SpeedCnt1( 1 ), SpeedCnt2( 1 ),
					 MasterClock( 0 ), ClkCnt( 0 ), FPSCnt( 0 )
{
	PRINTD( CONST_LOG, "[[SCH6]]\n" );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
SCH6::~SCH6( void )
{
	PRINTD( CONST_LOG, "[[~SCH6]]\n" );
	
	Stop();
}


/////////////////////////////////////////////////////////////////////////////
// スレッド関数(オーバーライド)
//   OSD_Delay()の精度に左右されるのでけっこういい加減かも
//   49.7日以上連続稼動したOSでは不具合が生じるけど．．．いいでしょ
//
// 引数:	inst	自分自身のオブジェクトポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::OnThread( void* inst )
{
	SCH6* ti = STATIC_CAST( SCH6*, inst );	// 自分自身のオブジェクトポインタ取得
	double fnext;							// 次の画面更新時間
	DWORD now,next;
	
	
	EnableScrUpdate = 0;
	
	// 最初の待ち時間を設定
	now   = OSD_GetTicks();
	next  = now + WRUPDATE;
	fnext = (double)now + FRMTICK;
	
	while( !this->cThread::IsCancel() ){
		now = OSD_GetTicks();
		
		// 実行速度比更新
		if( now >= next ){
			next += WRUPDATE;
			
			WRClk.emplace_front( ClkCnt );
			while( WRClk.size() > SPDCNT ) WRClk.pop_back();
			ClkCnt -= WRClk.front();
		}
		
		// 画面更新タイミング更新
		if( now >= (DWORD)(fnext + 0.5) ){
			fnext += FRMTICK;
			
			// FPS更新
			if( FPSCnt ){
				FPSCnt--;
				FPSClk.emplace_front( now );
				while( (FPSClk.size() > FPSCNT) || (*FPSClk.begin() - *FPSClk.end() > FPSSEC * 1000) ) FPSClk.pop_back();
			}
			
			// 画面更新フラグを立てる
			EnableScrUpdate++;
			// タイミング調整用VSYNC Wait解除
			ti->VWaitReset();
		}else
			OSD_Delay( 0 );
	}
	
	while( !WRClk.empty() ) WRClk.pop_back();
	ClkCnt = 0;
	
	while( !FPSClk.empty() ) FPSClk.pop_back();
	FPSCnt = 0;


}


/////////////////////////////////////////////////////////////////////////////
// マスタクロック設定
//
// 引数:	mclock	マスタクロック
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::SetMasterClock( int mclock )
{
	MasterClock = mclock;
}


/////////////////////////////////////////////////////////////////////////////
// 動作開始
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
bool SCH6::Start( void )
{
	// スレッド生成
	if( !this->cThread::BeginThread( this ) ){
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 動作停止
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::Stop( void )
{
	this->cThread::Cancel();	// スレッド終了フラグ立てる
	this->cThread::Waiting();	// スレッド終了まで待つ
}


/////////////////////////////////////////////////////////////////////////////
// Wait有効フラグ取得
//
// 引数:	なし
// 返値:	有効:true 無効:false
/////////////////////////////////////////////////////////////////////////////
bool SCH6::GetWaitEnable( void ) const
{
	return WaitEnable;
}


/////////////////////////////////////////////////////////////////////////////
// Wait有効フラグ設定
//
// 引数:	en		Wait有効フラグ 有効:true 無効:false
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::SetWaitEnable( bool en )
{
	WaitEnable = en;
}


/////////////////////////////////////////////////////////////////////////////
// ポーズ有効フラグ取得
//
// 引数:	なし
// 返値:	有効:true 無効:false
/////////////////////////////////////////////////////////////////////////////
bool SCH6::GetPauseEnable( void ) const
{
	return PauseEnable;
}


/////////////////////////////////////////////////////////////////////////////
// ポーズ有効フラグ設定
//
// 引数:	en		ポーズ有効フラグ 有効:true 無効:false
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::SetPauseEnable( bool en )
{
	PauseEnable = en;
}


/////////////////////////////////////////////////////////////////////////////
// VSYNC Wait
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::VWait( void )
{
	if( !WaitEnable ){
		return;
	}
	
	SpeedCnt1++;
	
	if( (SpeedRatio > 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 < SpeedRatio ) ){
		return;
	}
	
	cSemaphore::Wait();
}


/////////////////////////////////////////////////////////////////////////////
// VSYNC Waitを解除する
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::VWaitReset( void )
{
	if( (SpeedCnt1 > 0xffffff) || (SpeedCnt2 > 0xffffff) ){
		SpeedCnt1 /= 2;
		SpeedCnt2 /= 2;
	}
	
	SpeedCnt2++;
	
	if( (SpeedRatio < 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 >= SpeedRatio ) ){
		return;
	}
	
	cSemaphore::Post();
}


/////////////////////////////////////////////////////////////////////////////
// 実行速度設定
//
// 引数:	spd		+:UP -:DOWN 0:等速
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::SetSpeedRatio( int spd )
{
	if     ( spd > 0 && SpeedRatio < 2000 ){ SpeedRatio += SpeedRatio <  200 ? 10 : 100; }
	else if( spd < 0 && SpeedRatio >   10 ){ SpeedRatio -= SpeedRatio <= 200 ? 10 : 100; }
	else if( spd == 0 )                    { SpeedRatio = 100; }
	
	SpeedCnt2 = (int)((double)VSYNC_HZ * 1000.0);
	SpeedCnt1 = (SpeedCnt2 * SpeedRatio) / 100;
}


/////////////////////////////////////////////////////////////////////////////
// 実行速度取得
//
// 引数:	なし
// 返値:	int		実行速度(%)
/////////////////////////////////////////////////////////////////////////////
int SCH6::GetSpeedRatio( void ) const
{
	return SpeedRatio;
}


/////////////////////////////////////////////////////////////////////////////
// 実行速度比取得
//
// 引数:	なし
// 返値:	int		実行速度比(%)(等倍が100)
/////////////////////////////////////////////////////////////////////////////
int SCH6::GetRatio( void ) const
{
	DWORD sum = 0;
	
	for( auto &i : WRClk ){
		sum += i;
	}
	
	return WRClk.size() ? (int)(double)( sum * 100.0 / MasterClock * 1000.0 / (WRUPDATE * WRClk.size()) + 0.5 ) : 100;
}


/////////////////////////////////////////////////////////////////////////////
// FPS取得
//
// 引数:	なし
// 返値:	double		FPS
/////////////////////////////////////////////////////////////////////////////
double SCH6::GetFPS( void ) const
{
	if( FPSClk.size() > 1 ){
		DWORD sum  = 0;
		for( auto p = FPSClk.begin() + 1; p != FPSClk.end(); ++p ){
			sum += *(p - 1) - *p;
		}
 		return (((double)FPSClk.size() - 1.0) * 1000.0) / (double)sum;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// イベント更新
//
// 引数:	clk		進めるクロック数
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::Update( int clk )
{
	ClkCnt += clk;	// 実行速度計算用
}


/////////////////////////////////////////////////////////////////////////////
// FPSカウントアップ
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void SCH6::FPSUpdate( void )
{
	FPSCnt++;
}


/////////////////////////////////////////////////////////////////////////////
// 画面更新時期を迎えた?
//
// 引数:	なし
// 返値:	bool		true:更新する false:更新しない
/////////////////////////////////////////////////////////////////////////////
bool SCH6::IsScreenUpdate( void )
{
	if( EnableScrUpdate ){
		EnableScrUpdate--;
		return true;
	}
	return false;
}




/////////////////////////////////////////////////////////////////////////////
// ** 結局使わなくなりました **
// 画面の表示状態に応じてCPUステート数を計算
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
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


