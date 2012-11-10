#ifndef SCHEDULE_H_INCLUDED
#define SCHEDULE_H_INCLUDED

#include "typedef.h"

#include "device.h"
#include "ini.h"
#include "p6device.h"
#include "thread.h"


// 最大イベント数
#define	MAXEVENT	(32)

// イベントスタイルフラグ
// bit0: 繰り返し指示
#define	EV_ONETIME	(0x00)	/* ワンタイム */
#define	EV_LOOP		(0x01)	/* ループ */
// bit1,2: 周期指定単位
#define	EV_HZ		(0x00)	/* Hz */
#define	EV_US		(0x02)	/* us */
#define	EV_MS		(0x04)	/* ms */
#define	EV_STATE	(0x08)	/* CPUステート数 */


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cSche : public IDoko {
public:
	// イベント情報構造体
	struct evinfo {
		P6DEVICE* device;	// イベントデバイスオブジェクトポインタ
		int id;				// イベントID
		bool Active;		// イベント有効フラグ true:有効 false:停止
		int Period;			// 1周期のクロック数
		int Clock;			// 残りクロック数
		double nps;			// イベント発生周波数(Hz)
		
		evinfo () : device(NULL), id(0), Active(false), Period(0), Clock(0), nps(0) {}
	};
	
protected:
	evinfo ev[MAXEVENT];	// イベント情報
	
	bool VSYNC;				// VSYNCフラグ true:VSYNCに達した false:達してない
	
	int MasterClock;		// マスタクロック数(基本的にはCPUステート数/秒になるはず)
	int NextEvent;			// 次のイベントが発生するまでのクロック数
	int SaveClock;			// クロックを溜め込む
	
	evinfo *Find( P6DEVICE *, int );			// イベント検索
	void SetMasterClock( int );					// マスタクロック数を設定
	
public:
	cSche( int );								// コンストラクタ
	virtual ~cSche();							// デストラクタ
	
	bool Add( P6DEVICE *, int, double, int );	// イベント追加
	bool Del( P6DEVICE *, int );				// イベント削除
	
	bool SetHz( P6DEVICE *, int, double );		// イベント発生周波数設定
	
	void Update( int );							// イベント更新
	void Reset( P6DEVICE *, int, double=0 );	// 指定イベントをリセットする
	
	int Rest( P6DEVICE *, int );				// イベント発生までの残りクロック数を求める
	double Scale( P6DEVICE *, int );			// イベントの進行率を求める
	
	bool GetEvinfo( evinfo * );					// イベント情報取得
	bool SetEvinfo( evinfo * );					// イベント情報設定
	
	int GetMasterClock();						// マスタークロック取得
	
	bool IsVSYNC();								// VSYNCに達した?
	void OnVSYNC();								// VSYNCを通知する
	void ReVSYNC();								// VSYNCフラグキャンセル
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// どこでもSAVE
	bool DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};

class SCH6 : public cThread, public cSemaphore {
protected:
	bool WaitEnable;		// Wait有効フラグ
	int EnableScrUpdate;	// 画面更新フラグ
	
	// 処理速度計算用
	int MasterClock;		// マスタクロック数
	DWORD WRClock;			// 処理クロック保存(計測完了)
	DWORD WRClockTmp;		// 処理クロック保存(計測中)
	
	void OnThread( void * );					// スレッド関数
	
public:
	SCH6( int );								// コンストラクタ
	virtual ~SCH6();							// デストラクタ
	
//	void CalcCpuState();						// 画面の表示状態に応じてCPUステート数を計算
	
	bool Start();								// 動作開始
	void Stop();								// 動作停止
	
	bool GetWaitEnable();						// Wait有効フラグ取得
	void SetWaitEnable( bool );					// Wait有効フラグ設定
	
	void VWait();								// VSYNC Wait
	void WaitReset();							// Waitを解除する
	
	void Update( int );							// イベント更新
	int GetRatio();								// 実行速度比取得
	
	bool IsScreenUpdate();						// 画面更新時期を迎えた?
};

#endif	// SCHEDULE_H_INCLUDED
