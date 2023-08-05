/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef P6VM_H_INCLUDED
#define P6VM_H_INCLUDED

#include <memory>
#include <vector>

#include "cpum.h"
#include "cpus.h"
#include "io.h"
#include "keyboard.h"
#include "pio.h"
#include "psgfm.h"
#include "schedule.h"
#include "tape.h"
#include "typedef.h"
#include "device/z80.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "breakpoint.h"
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class EL6;
class CFG6;

class CPU6;
class SUB6;
class IO6;
class MEM6;
class IRQ6;
class PIO6;
class VDG6;
class KEY6;
class PSGb;
class VCE6;
class CMTL;
class CMTS;
class DSK6;

class EVSC;
class BPoint;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// 基本仮想マシンクラス
class VM6 : public std::enable_shared_from_this<VM6> {
	
	friend class EL6;
	friend class QtEL6;
	friend class cWndStat;
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndReg;
	friend class cWndMem;
	friend class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
protected:
	// デバイスコネクタテーブル構造体
	struct DEVCONNTABLE {
		const std::vector<IOBus::Connector>* Intr;		// 割込み
		const std::vector<IOBus::Connector>* Memory;	// メモリ
		const std::vector<IOBus::Connector>* Vdg;		// VDG
		const std::vector<IOBus::Connector>* Psg;		// PSG/OPN
		const std::vector<IOBus::Connector>* M8255;		// I/O(Z80側)
		const std::vector<IOBus::Connector>* S8255;		// I/O(SUB CPU側)
		const std::vector<IOBus::Connector>* Voice;		// 音声合成
		const std::vector<IOBus::Connector>* Disk;		// DISK
		const std::vector<IOBus::Connector>* CmtL;		// CMT(LOAD)
		
		DEVCONNTABLE() :
			Intr( nullptr ), Memory( nullptr ), Vdg( nullptr ), Psg( nullptr ), M8255( nullptr ), S8255( nullptr ),
			Voice( nullptr ), Disk( nullptr ), CmtL( nullptr ) {}
	};
	
	int cclock;						// CPUクロック
	int pclock;						// PSG/OPNクロック
	
	// オブジェクトポインタ
	std::unique_ptr<EVSC> evsc;		// イベントスケジューラ
	std::unique_ptr<IO6> iom;		// I/O(Z80側)
	std::unique_ptr<IO6> ios;		// I/O(SUB CPU側)
	
	std::shared_ptr<IRQ6> intr;		// 割込み
	std::shared_ptr<CPU6> cpum;		// CPU
	std::shared_ptr<SUB6> cpus;		// SUB CPU
	std::shared_ptr<MEM6> mem;		// メモリ
	std::shared_ptr<VDG6> vdg;		// VDG
	std::shared_ptr<PSGb> psg;		// PSG/OPN
	std::shared_ptr<VCE6> voice;	// 音声合成
	std::shared_ptr<PIO6> pio;		// 8255
	std::shared_ptr<KEY6> key;		// キー
	std::shared_ptr<CMTL> cmtl;		// CMT(LOAD)
	std::shared_ptr<CMTS> cmts;		// CMT(SAVE)
	std::shared_ptr<DSK6> disk;		// DISK
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	std::unique_ptr<BPoint> bp;		// ブレークポイント
	bool MonDisp;					// モニタウィンドウ表示状態 true:表示 false:非表示
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	DEVCONNTABLE DevTable;			// デバイスコネクタテーブル
	
	virtual void AllocObjSpec() = 0;					// 機種別オブジェクト確保
	bool AllocObject( const std::shared_ptr<CFG6>& );	// 全オブジェクト確保
	
public:
	VM6();
	virtual ~VM6();
	
	bool Init( const std::shared_ptr<CFG6>& );			// 初期化
	void Reset();										// リセット
	int Emu();											// 1命令実行
	int GetCPUClock() const;							// CPUクロック取得
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool IsMonitor() const;								// モニタモード?
	void SetMonitor( bool );							// モニタモード設定
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	
	// P6デバイス用関数群
	// EVSC
	bool EventAdd( Device::ID, int, double, int );		// イベント追加
	bool EventDel( Device::ID, int );					// イベント削除
	void EventUpdate( int );							// イベント更新
	void EventReset( Device::ID, int, double=0 );		// 指定イベントをリセットする
	double EventGetProgress( Device::ID, int );			// イベントの進行率を求める
	bool EventGetInfo( EVSC::evinfo* );					// イベント情報取得
	bool EventSetInfo( EVSC::evinfo* );					// イベント情報設定
	void EventOnVSYNC();								// VSYNCを通知する
	// IO6
	BYTE IomIn( int, int* = nullptr );					// IN関数
	BYTE IosIn( int, int* = nullptr );					// IN関数
	void IomOut( int, BYTE, int* = nullptr );			// OUT関数
	void IosOut( int, BYTE, int* = nullptr );			// OUT関数
	// IRQ6
	int	IntIntrCheck();									// 割込みチェック＆ベクタ取得
	void IntReqIntr( DWORD );							// 割込み要求
	void IntCancelIntr( DWORD );						// 割込み撤回
	bool IntGetTimerIntr();								// タイマ割込みスイッチ取得
	// SUB6
	void CpusReqKeyIntr( int, BYTE );					// キー割込み要求
	void CpusExtIntr();									// 外部割込み要求
	void CpusReqCmtIntr( BYTE );						// CMT READ割込み要求
	bool CpusIsCmtIntrReady();							// CMT割込み発生可?
	// MEM6
	BYTE MemFetch( WORD, int* = nullptr );				// フェッチ(M1)
	BYTE MemRead( WORD, int* = nullptr );				// メモリリード
	void MemWrite( WORD, BYTE, int* = nullptr );		// メモリライト
	void MemSetCGBank( bool );							// CG ROM BANK 選択
	BYTE MemReadIntRam( WORD ) const;					// 直接読込み
	BYTE MemReadExtRam( WORD ) const;					// 直接読込み
	BYTE MemReadCGrom1( WORD ) const;					// 直接読込み
	BYTE MemReadCGrom2( WORD ) const;					// 直接読込み
	BYTE MemReadCGrom3( WORD ) const;					// 直接読込み
	// VDG6
//	bool VdgGetCrtDisp() const;							// CRT表示状態取得
	void VdgSetCrtDisp( bool );							// CRT表示状態設定
	bool VdgGetWinSize() const;							// ウィンドウサイズ取得
	bool VdgIsBusReqStop() const;						// バスリクエスト区間停止フラグ取得
	bool VdgIsBusReqExec() const;						// バスリクエスト区間実行フラグ取得
	bool VdgIsSRmode() const;							// SRモード取得
	bool VdgIsSRBitmap( WORD ) const;					// SRビットマップモード取得
	WORD VdgSRGVramAddr( WORD ) const;					// SRのG-VRAMアドレス取得(ビットマップモード)
	// PIO6
	BYTE PioReadA();									// PartA リード
	// KEY6
	BYTE KeyGetKeyJoy() const;							// カーソルキー状態取得
	BYTE KeyGetJoy( int ) const;						// ジョイスティック状態取得
	void KeyChangeKana();								// 英字<->かな切換
	void KeyChangeKKana();								// かな<->カナ切換
	// CMTS
	bool CmtsMount();									// TAPE マウント
	void CmtsUnmount();									// TAPE アンマウント
	void CmtsSetBaud( int );							// ボーレート設定
	void CmtsWriteOne( BYTE );							// 1文字書込み
};


// PC-6001 仮想マシンクラス
class VM60 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM60();
	~VM60();
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_intr;		// 割込み
	static const std::vector<IOBus::Connector> c_vdg;		// VDG
	static const std::vector<IOBus::Connector> c_psg;		// PSG
	static const std::vector<IOBus::Connector> c_8255m;		// I/O(Z80側)
	static const std::vector<IOBus::Connector> c_8255s;		// I/O(SUB CPU側)
	static const std::vector<IOBus::Connector> c_disk;		// DISK
	static const std::vector<IOBus::Connector> c_cmtl;		// CMT(LOAD)
};


// PC-6001A 仮想マシンクラス
class VM61 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM61();
	~VM61();
};


// PC-6001mk2 仮想マシンクラス
class VM62 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM62();
	~VM62();
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_intr;		// 割込み
	static const std::vector<IOBus::Connector> c_memory;	// メモリ
	static const std::vector<IOBus::Connector> c_vdg;		// VDG
	static const std::vector<IOBus::Connector> c_psg;		// PSG
	static const std::vector<IOBus::Connector> c_8255m;		// I/O(Z80側)
	static const std::vector<IOBus::Connector> c_8255s;		// I/O(SUB CPU側)
	static const std::vector<IOBus::Connector> c_voice;		// 音声合成
	static const std::vector<IOBus::Connector> c_disk;		// DISK
	static const std::vector<IOBus::Connector> c_cmtl;		// CMT(LOAD)
};


// PC-6601 仮想マシンクラス
class VM66 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM66();
	~VM66();
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_disk;	// DISK
};


// PC-6001mk2SR 仮想マシンクラス
class VM64 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM64();
	~VM64();
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_intr;		// 割込み
	static const std::vector<IOBus::Connector> c_memory;	// メモリ
	static const std::vector<IOBus::Connector> c_vdg;		// VDG
	static const std::vector<IOBus::Connector> c_psg;		// OPN
	static const std::vector<IOBus::Connector> c_8255m;		// I/O(Z80側)
	static const std::vector<IOBus::Connector> c_8255s;		// I/O(SUB CPU側)
	static const std::vector<IOBus::Connector> c_voice;		// 音声合成
	static const std::vector<IOBus::Connector> c_disk;		// DISK
	static const std::vector<IOBus::Connector> c_cmtl;		// CMT(LOAD)
};


// PC-6601SR 仮想マシンクラス
class VM68 : public VM6 {
private:
	void AllocObjSpec() override;				// 機種別オブジェクト確保
	
public:
	VM68();
	~VM68();
	
	// デバイスコネクタ
	static const std::vector<IOBus::Connector> c_disk;	// DISK
};


#endif		// P6VM_H_INCLUDED
