#ifndef BREAKPOINTL_H_INCLUDED
#define BREAKPOINTL_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ブレークポイントの数
#define	NR_BP	(10)

class BPoint {
public:
	enum BPtype{ BP_NONE, BP_PC, BP_READ, BP_WRITE, BP_IN, BP_OUT, EndofBPtype };
	
private:
	struct BreakPoint {
		BPtype Type;
		WORD Addr;
		
		BreakPoint() : Type(BP_NONE), Addr(0) {}
	};
	BreakPoint BP[NR_BP];	// ブレークポイント
	
	// ブレークポイントの情報
	int BPNo;
	WORD BPAddr;
	BYTE BPData;
	const char *BPStr;
	
	// ブレーク要求
	bool ReqBreak;
	int ReqBPNum;
	
public:
	BPoint();
	~BPoint();
	
	bool ExistBreakPoint() const;						// ブレークポイントの有無をチェック
	bool CheckBreakPoint( BPtype, WORD, BYTE, const char * );	// ブレークポイントをチェック
	
	void ClearStatus();									// ブレークポイントの情報をクリア
	
	BPtype GetType( int ) const;						// ブレークポイントのタイプを取得
	void SetType( int, BPtype );						// ブレークポイントのタイプを設定
	WORD GetAddr( int ) const;							// ブレークポイントのアドレスを取得
	void SetAddr( int, WORD );							// ブレークポイントのアドレスを設定
	
	bool IsReqBreak() const;							// ブレーク要求あり?
	int GetReqBPNum() const;							// ブレーク要求のあったブレークポイントNo.を取得
	void ResetBreak();									// ブレーク要求キャンセル
};

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// BREAKPOINTL_H_INCLUDED
