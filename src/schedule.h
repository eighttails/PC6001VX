/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef SCHEDULE_H_INCLUDED
#define SCHEDULE_H_INCLUDED

#include <vector>
#include <deque>

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "csemaphore.h"
#include "cthread.h"


// イベントスタイルフラグ
// bit0: 繰り返し指示
#define	EV_ONETIME	(0x00)	// ワンタイム
#define	EV_LOOP		(0x01)	// ループ
// bit1,2: 周期指定単位
#define	EV_HZ		(0x00)	// Hz
#define	EV_US		(0x02)	// us
#define	EV_MS		(0x04)	// ms
#define	EV_STATE	(0x08)	// CPUステート数


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
// VMイベントスケジューラクラス
class EVSC : public IDoko {
public:
	// イベント情報構造体
	struct evinfo {
		Device::ID devid;		// イベントデバイスオブジェクトID
		int id;					// イベントID
		int Period;				// 1周期のクロック数
		int Clock;				// 残りクロック数
		double nps;				// イベント発生周波数(Hz)

		evinfo() : devid(0), id(0), Period(0), Clock(0), nps(0) {}
	};

protected:
	std::vector<evinfo> ev;		// イベント情報
	DeviceList devlist;			// デバイスリスト

	bool VSYNC;					// VSYNCフラグ true:VSYNCに達した false:達してない

	int MasterClock;			// マスタクロック数(基本的にはCPUステート数/秒になるはず)
	int NextEvent;				// 次のイベントが発生するまでのクロック数
	int SaveClock;				// クロックを溜め込む

	const evinfo* Find( Device::ID, int ) const;	// イベント検索

public:
	EVSC();
	virtual ~EVSC();

	bool Entry( std::vector<std::shared_ptr<IDevice>> );		// 接続するデバイス候補を登録する

	bool Add( Device::ID, int, double, int );	// イベント追加
	bool Del( Device::ID, int );				// イベント削除

	void Update( int );							// イベント更新
	void Reset( Device::ID, int, double=0 );	// 指定イベントをリセットする

	int Rest( Device::ID, int ) const;			// イベント発生までの残りクロック数を求める
	double GetProgress( Device::ID, int ) const;	// イベントの進行率を求める

	bool GetEvinfo( evinfo* ) const;			// イベント情報取得
	bool SetEvinfo( evinfo* );					// イベント情報設定

	void SetMasterClock( int );					// マスタクロック設定
	int GetMasterClock() const;					// マスタクロック取得

	bool IsVSYNC() const;						// VSYNCに達した?
	void OnVSYNC();								// VSYNCを通知する
	void ReVSYNC();								// VSYNCフラグキャンセル

	// ----------------------------------------------------------------------
	bool DokoSave( cIni* ) override;	// どこでもSAVE
	bool DokoLoad( cIni* ) override;	// どこでもLOAD
	// ----------------------------------------------------------------------
};


// エミュレータスケジューラクラス
class SCH6 : public cThread, public cSemaphore {
protected:
	bool WaitEnable;			// Wait有効フラグ
	bool PauseEnable;			// ポーズ有効フラグ
	int EnableScrUpdate;		// 画面更新フラグ

	int SpeedRatio;				// 実行速度
	int SpeedCnt1;				// 実行速度調整用カウンタ1
	int SpeedCnt2;				// 実行速度調整用カウンタ2

	// 処理速度計算用
	int MasterClock;			// マスタクロック数
	DWORD ClkCnt;				// 処理クロックカウンタ
	std::deque<DWORD> WRClk;	// 処理クロック保存

	// FPS計算用
	DWORD FPSCnt;				// 描画フレーム数カウンタ
	std::deque<DWORD> FPSClk;	// 描画クロック保存

	void OnThread( void* ) override;			// スレッド関数

public:
	SCH6();
	virtual ~SCH6();

	void SetMasterClock( int );					// マスタクロック設定

	bool Start();								// 動作開始
	void Stop();								// 動作停止

	bool GetWaitEnable() const;					// Wait有効フラグ取得
	void SetWaitEnable( bool );					//               設定

	bool GetPauseEnable() const;				// ポーズ有効フラグ取得
	void SetPauseEnable( bool );				//                 設定

	void VWait();								// VSYNC Wait
	void VWaitReset();							// VSYNC Waitを解除する

	void SetSpeedRatio( int );					// 実行速度設定
	int GetSpeedRatio() const;					//         取得

	int GetRatio() const;						// 実行速度比取得
	double GetFPS() const;						// FPS取得

	void Update( int );							// イベント更新
	void FPSUpdate();							// FPSカウントアップ

	bool IsScreenUpdate();						// 画面更新時期を迎えた?
};

#endif	// SCHEDULE_H_INCLUDED
