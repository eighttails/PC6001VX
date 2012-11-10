#ifndef P6VM_H_INCLUDED
#define P6VM_H_INCLUDED

#include "typedef.h"


class EL6;

class CPU6;
class SUB6;
class IO6;
class MEM6;
class VDG6;
class INT6;
class KEY6;
class DSK6;
class CMTL;
class CMTS;
class PSG6;
class PIO6;
class VCE6;

class BPoint;
class cSche;
class CFG6;


// 基本仮想マシンクラス
class VM6 {
	
	friend class EL6;
	friend class DSP6;
	
	friend class VDG6;
	friend class VDG60;
	friend class VDG62;
	friend class SUB6;
	friend class SUB60;
	friend class SUB62;
	friend class PIO6;
	friend class CPU6;
	friend class PSG6;
	friend class INT6;
	friend class VCE6;
	friend class CMTL;
	friend class DSK6;
	friend class DSK60;
	friend class DSK66;
	friend class KEY6;
	friend class cWndStat;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	friend class cWndReg;
	friend class cWndMem;
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
protected:
	int clock;					// CPUクロック
	
	// オブジェクトポインタ
	EL6 *el;					// エミュレータレイヤ
	
	cSche *evsc;				// イベントスケジューラ
	IO6 *iom;					// I/O(Z80)
	IO6 *ios;					// I/O(SUB CPU)
	INT6 *intr;					// 割込み
	CPU6 *cpum;					// CPU
	SUB6 *cpus;					// SUB CPU
	MEM6 *mem;					// メモリ
	VDG6 *vdg;					// VDG
	PSG6 *psg;					// PSG
	VCE6 *voice;				// 音声合成
	PIO6 *pio;					// 8255
	KEY6 *key;					// キー
	CMTL *cmtl;					// CMT
	CMTS *cmts;					// CMT(SAVE)
	DSK6 *disk;					// DISK
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	BPoint *bp;					// ブレークポイント
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ //
	
	bool InitCommon( CFG6 * );				// 共通部分初期化
	
public:
	VM6( EL6 * );							// コンストラクタ
	virtual ~VM6();							// デストラクタ
	
	virtual bool Init( CFG6 * ) = 0;		// 初期化
	
	void Reset();							// リセット
	int Emu();								// 1命令実行
	int GetCPUClock();						// CPUクロック取得
};

// PC-6001 仮想マシンクラス
class VM60 : public VM6 {
public:
	VM60( EL6 * );							// コンストラクタ
	~VM60();								// デストラクタ
	
	bool Init( CFG6 * );					// 初期化
};

// PC-6001mk2 仮想マシンクラス
class VM62 : public VM6 {
public:
	VM62( EL6 * );							// コンストラクタ
	~VM62();								// デストラクタ
	
	bool Init( CFG6 * );					// 初期化
};

// PC-6601 仮想マシンクラス
class VM66 : public VM6 {
public:
	VM66( EL6 * );							// コンストラクタ
	~VM66();								// デストラクタ
	
	bool Init( CFG6 * );					// 初期化
};

#endif		// P6VM_H_INCLUDED
