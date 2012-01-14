#ifndef CPUS_H_INCLUDED
#define CPUS_H_INCLUDED

#include "typedef.h"
#include "ini.h"

#include "p6device.h"


// CMTステータス
#define	CMTCLOSE	(0)
#define	LOADOPEN	(1)
#define	SAVEOPEN	(2)

// I/Oポート番号
#define	IO8049_BUS	(0x00)	/* バスポート */
#define	IO8049_P1	(0x01)	/* Port1 */
#define	IO8049_P2	(0x02)	/* Port2 */
#define	IO8049_T0	(0x03)	/* T0(OUT) */
#define	IO8049_INT	(0x04)	/* ~INT(IN) */


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class SUB6 : public P6DEVICE, public IDoko {
private:
	int CmtStatus;						// CMTステータス
	int Status8049;						// 8049動作ステータス
	
	DWORD IntrFlag;						// 割込み要求フラグ
	
	BYTE KeyCode;						// キーコード(割込み用)
	BYTE JoyCode;						// ゲーム用キーコード(割込み用)
	BYTE CmtData;						// CMTリードデータ(割込み用)
	BYTE SioData;						// RS232C受信データ(割込み用)
	
	BYTE ReadIO( int );					// I/Oポートアクセス Read
	void WriteIO( int, BYTE );			// I/Oポートアクセス Write
	
	void WriteExt( BYTE );				// 外部メモリ出力(8255のPortA)
	BYTE ReadExt();						// 外部メモリ入力(8255のPortA)
	
	BOOL GetT0();						// テスト0ステータス取得
	BOOL GetT1();						// テスト1ステータス取得
	BOOL GetINT();						// 外部割込みステータス取得
	
	void OutVector();					// 割込みベクタ出力
	void OutData()	;					// 割込みデータ出力
	
	
public:
	SUB6( VM6 *, const P6ID& );			// コンストラクタ
	~SUB6();							// デストラクタ
	
	void EventCallback( int, int );		// イベントコールバック関数
	
	void Reset();						// リセット
	
	// キーボード関連
	void ReqKeyIntr( int, BYTE );		// キー割込み要求
	void ReqJoyIntr();					// ゲーム用キー割込み要求
	
	// CMT関連
	void ReqCmtIntr( BYTE );			// CMT READ割込み要求
	int GetCmtStatus();					// CMTステータス取得
	BOOL IsCmtIntrReady();				// CMT割込み発生可?
	
	void ExtIntr();						// 外部割込み要求
	
	// ------------------------------------------
	BOOL DokoSave( cIni * );	// どこでもSAVE
	BOOL DokoLoad( cIni * );	// どこでもLOAD
	// ------------------------------------------
};


#endif	// CPUS_H_INCLUDED
