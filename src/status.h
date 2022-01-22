/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef STATUS_H_INCLUDED
#define STATUS_H_INCLUDED

#include <memory>

#include "console.h"
#include "p6el.h"


//---------------------------------------------------------------------------
//  ステータスバークラス
//---------------------------------------------------------------------------
class cWndStat : public ZCons {
private:
	int DrvNum;				// ドライブ数
	WORD ExCart;			// 拡張カートリッジ
	
public:
	cWndStat();
	~cWndStat();
	
	bool Init( int, int = -1, WORD = 0xffff );	// 初期化
	void Update( EL6* );						// ウィンドウ更新
};

#endif	// STATUS_H_INCLUDED
