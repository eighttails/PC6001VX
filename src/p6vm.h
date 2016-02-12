#ifndef P6VM_H_INCLUDED
#define P6VM_H_INCLUDED

#include "typedef.h"
#include "io.h"
#include "schedule.h"

#include "device/z80.h"

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "breakpoint.h"
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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


// 基本仮想マシンクラス
class VM6 {
	friend class QtEL6;

	friend class EL6;
	friend class DSP6;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
protected:
	// デバイスコネクタテーブル構造体
	struct DEVCONNTABLE {
		const IOBus::Connector *Intr;		// 割込み
		const IOBus::Connector *Memory;		// メモリ
		const IOBus::Connector *Vdg;		// VDG
		const IOBus::Connector *Psg;		// PSG/OPN
		const IOBus::Connector *M8255;		// I/O(Z80側)
		const IOBus::Connector *S8255;		// I/O(SUB CPU側)
		const IOBus::Connector *Voice;		// 音声合成
		const IOBus::Connector *Disk;		// DISK
		const IOBus::Connector *CmtL;		// CMT(LOAD)
		const IOBus::Connector *Soldier;	// 戦士のカートリッジ
		
		DEVCONNTABLE() : Intr(NULL), Memory(NULL), Vdg(NULL), Psg(NULL), M8255(NULL), S8255(NULL),
						 Voice(NULL), Disk(NULL), CmtL(NULL), Soldier(NULL) {}
	};
	
	
	int cclock;					// CPUクロック
	int pclock;					// PSG/OPNクロック
	
	// オブジェクトポインタ
	EL6 *el;					// エミュレータレイヤ
	EVSC *evsc;					// イベントスケジューラ
	IO6 *iom;					// I/O(Z80側)
	IO6 *ios;					// I/O(SUB CPU側)
	IRQ6 *intr;					// 割込み
	CPU6 *cpum;					// CPU
	SUB6 *cpus;					// SUB CPU
	MEM6 *mem;					// メモリ
	VDG6 *vdg;					// VDG
	PSGb *psg;					// PSG/OPN
	VCE6 *voice;				// 音声合成
	PIO6 *pio;					// 8255
	KEY6 *key;					// キー
	CMTL *cmtl;					// CMT(LOAD)
	CMTS *cmts;					// CMT(SAVE)
	DSK6 *disk;					// DISK
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	BPoint *bp;					// ブレークポイント
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	DEVCONNTABLE DevTable;		// デバイスコネクタテーブル
	
	virtual bool AllocObjSpecific() = 0;		// 機種別オブジェクト確保
	
public:
	VM6( EL6 * );								// コンストラクタ
	virtual ~VM6();								// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_soldier[];	// 戦士のカートリッジ
	
	bool AllocObject( CFG6 * );					// 全オブジェクト確保
	bool Init( CFG6 * );						// 初期化
	void Reset();								// リセット
	int Emu();									// 1命令実行
	int GetCPUClock() const;					// CPUクロック取得
	
	
	// P6デバイス用関数群
	// EL
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool ElIsMonitor() const;					// モニタモード?
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// EVSC
	bool EventAdd( Device *, int, double, int );	// イベント追加
	bool EventDel( Device *, int );				// イベント削除
	void EventUpdate( int );					// イベント更新
	void EventReset( Device::ID, int, double=0 );	// 指定イベントをリセットする
	double EventGetProgress( Device::ID, int );	// イベントの進行率を求める
	bool EventGetInfo( EVSC::evinfo * );		// イベント情報取得
	bool EventSetInfo( EVSC::evinfo * );		// イベント情報設定
	void EventOnVSYNC();						// VSYNCを通知する
	// IO6
	BYTE IomIn( int, int * = NULL );			// IN関数
	BYTE IosIn( int, int * = NULL );			// IN関数
	void IomOut( int, BYTE, int * = NULL );		// OUT関数
	void IosOut( int, BYTE, int * = NULL );		// OUT関数
	// IRQ6
	int	IntIntrCheck();							// 割込みチェック
	void IntReqIntr( DWORD );					// 割込み要求
	void IntCancelIntr( DWORD );				// 割込み撤回
	bool IntGetTimerIntr();						// タイマ割込みスイッチ取得
	// CPU6
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int CpumDisasm( char *, WORD );				// 1ライン逆アセンブル
	void CpumGetRegister( cZ80::Register * );	// レジスタ値取得
	void CpumSetRegister( cZ80::Register * );	// レジスタ値設定
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// SUB6
	void CpusReqKeyIntr( int, BYTE );			// キー割込み要求
	void CpusExtIntr();							// 外部割込み要求
	void CpusReqCmtIntr( BYTE );				// CMT READ割込み要求
	bool CpusIsCmtIntrReady();					// CMT割込み発生可?
	// MEM6
	BYTE MemFetch( WORD, int * = NULL );		// フェッチ(M1)
	BYTE MemRead( WORD, int * = NULL );			// メモリリード
	void MemWrite( WORD, BYTE, int * = NULL );	// メモリライト
	void MemSetCGBank( bool );					// CG ROM BANK を切り替える
	BYTE MemReadMainRom( WORD ) const;			// 直接読込み
	BYTE MemReadIntRam ( WORD ) const;			// 直接読込み
	BYTE MemReadExtRom ( WORD ) const;			// 直接読込み
	BYTE MemReadExtRam ( WORD ) const;			// 直接読込み
	BYTE MemReadCGrom1 ( WORD ) const;			// 直接読込み
	BYTE MemReadCGrom2 ( WORD ) const;			// 直接読込み
	BYTE MemReadCGrom3 ( WORD ) const;			// 直接読込み
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const char *MemGetReadMemBlk( int ) const;	// メモリブロック取得(Read)
	const char *MemGetWriteMemBlk( int ) const;	// メモリブロック取得(Write)
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// VDG6
	bool VdgGetCrtDisp() const;					// CRT表示状態取得
	void VdgSetCrtDisp( bool );					// CRT表示状態設定
	bool VdgGetWinSize() const;					// ウィンドウサイズ取得
	bool VdgIsBusReqStop() const;				// バスリクエスト区間停止フラグ取得
	bool VdgIsBusReqExec() const;				// バスリクエスト区間実行フラグ取得
	WORD VdgGetVramAddr() const;				// VRAMアドレス取得
	WORD VdgGerAttrAddr() const;				// ATTRアドレス取得
	bool VdgIsSRmode() const;					// SRモード取得
	bool VdgIsSRGVramAccess( WORD, bool ) const;	// SRのG-VRAMアクセス?
	WORD VdgSRGVramAddr( WORD ) const;			// SRのG-VRAMアドレス取得
	// PIO6
	BYTE PioReadA();							// PartA リード
	// KEY6
	BYTE KeyGetKeyJoy() const;					// カーソルキー状態取得
	BYTE KeyGetJoy( int ) const;				// ジョイスティック状態取得
	BYTE KeyGetKeyIndicator() const;			// キーボードインジケータ状態取得
	void KeyChangeKana();						// 英字<->かな切換
	void KeyChangeKKana();						// かな<->カナ切換
	// CMTL
	bool CmtlIsMount() const;					// マウント済み?
	bool CmtlIsAutoStart() const;				// オートスタート?
	const char *CmtlGetFile() const;			// ファイルパス取得
	const char *CmtlGetName() const;			// TAPE名取得
	DWORD CmtlGetSize() const;					// ベタイメージサイズ取得
	int CmtlGetCount() const;					// カウンタ取得
	// CMTS
	bool CmtsMount();							// TAPE マウント
	void CmtsUnmount();							// TAPE アンマウント
	void CmtsSetBaud( int );					// ボーレート設定
	void CmtsCmtWrite( BYTE );					// CMT 1文字書込み
	// DSK6
	bool DskIsMount( int ) const;				// マウント済み?
	bool DskIsSystem( int ) const;				// システムディスク?
	bool DskIsProtect( int ) const;				// プロテクト?
	bool DskInAccess( int ) const;				// アクセス中?
	const char *DskGetFile( int ) const;		// ファイルパス取得
	const char *DskGetName( int ) const;		// DISK名取得
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// BPoint
	bool BpExistBreakPoint() const;				// ブレークポイントの有無をチェック
	bool BpCheckBreakPoint( BPoint::BPtype, WORD, BYTE, const char * );	// ブレークポイントをチェック
	void BpClearStatus();						// ブレークポイントの情報をクリア
	BPoint::BPtype BpGetType( int ) const;		// ブレークポイントのタイプを取得
	void BpSetType( int, BPoint::BPtype );		// ブレークポイントのタイプを設定
	WORD BpGetAddr( int ) const;				// ブレークポイントのアドレスを取得
	void BpSetAddr( int, WORD );				// ブレークポイントのアドレスを設定
	bool BpIsReqBreak() const;					// ブレーク要求あり?
	int BpGetReqBPNum() const;					// ブレーク要求のあったブレークポイントNo.を取得
	void BpResetBreak();						// ブレーク要求キャンセル
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};


// PC-6001 仮想マシンクラス
class VM60 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM60( EL6 * );								// コンストラクタ
	~VM60();									// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_intr[];		// 割込み
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// PSG
	const static IOBus::Connector c_8255m[];	// I/O(Z80側)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU側)
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
	const static IOBus::Connector c_soldier[];	// 戦士のカートリッジ
	
};


// PC-6001A 仮想マシンクラス
class VM61 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM61( EL6 * );								// コンストラクタ
	~VM61();									// デストラクタ
};


// PC-6001mk2 仮想マシンクラス
class VM62 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM62( EL6 * );								// コンストラクタ
	~VM62();									// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_intr[];		// 割込み
	const static IOBus::Connector c_memory[];	// メモリ
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// PSG
	const static IOBus::Connector c_8255m[];	// I/O(Z80側)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU側)
	const static IOBus::Connector c_voice[];	// 音声合成
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
};


// PC-6601 仮想マシンクラス
class VM66 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM66( EL6 * );								// コンストラクタ
	~VM66();									// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_disk[];		// DISK
};


// PC-6001mk2SR 仮想マシンクラス
class VM64 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM64( EL6 * );								// コンストラクタ
	~VM64();									// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_intr[];		// 割込み
	const static IOBus::Connector c_memory[];	// メモリ
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// OPN
	const static IOBus::Connector c_8255m[];	// I/O(Z80側)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU側)
	const static IOBus::Connector c_voice[];	// 音声合成
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
};


// PC-6601SR 仮想マシンクラス
class VM68 : public VM6 {
private:
	bool AllocObjSpecific();					// 機種別オブジェクト確保
	
public:
	VM68( EL6 * );								// コンストラクタ
	~VM68();									// デストラクタ
	
	// デバイスコネクタ
	const static IOBus::Connector c_disk[];		// DISK
};


#endif		// P6VM_H_INCLUDED
