/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef BREAKPOINTL_H_INCLUDED
#define BREAKPOINTL_H_INCLUDED

#include <vector>

#include "typedef.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class BPoint {
public:
	enum BPtype{ BP_NONE, BP_PC, BP_READ, BP_WRITE, BP_IN, BP_OUT };
	
private:
	struct BreakPoint {
		BPtype Type;
		WORD Addr;
		bool Break;
		
		BreakPoint() : Type(BP_NONE), Addr(0), Break(false) {}
	};
	std::vector<BreakPoint> BP;	// ブレークポイント
	
public:
	BPoint();
	~BPoint();
	
	void Set( const BPtype, WORD );			// ブレークポイントを設定
	void Delete( const int );				// ブレークポイントを削除
	BPtype GetType( const int ) const;		// ブレークポイントのタイプを取得
	WORD GetAddr( const int ) const;		// ブレークポイントのアドレスを取得
	int GetNum() const;						// ブレークポイント登録数取得
	
	bool Check( const BPtype, const WORD );	// ブレークポイントをチェック
	int GetReqNum() const;					// ブレーク要求のあったブレークポイントNo.を取得
	void Reset();							// ブレーク要求キャンセル
};

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// BREAKPOINTL_H_INCLUDED
