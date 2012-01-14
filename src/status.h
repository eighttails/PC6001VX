#ifndef STATUS_H_INCLUDED
#define STATUS_H_INCLUDED

#include "typedef.h"
#include "console.h"
#include "p6device.h"


//------------------------------------------------------
//  ステータスバークラス
//------------------------------------------------------
class cWndStat : public P6DEVICE, public ZCons {
private:
	
public:
	cWndStat( VM6 *, const P6ID& );	// コンストラクタ
	~cWndStat();					// デストラクタ
	
	BOOL Init( int );				// 初期化
	void Update();					// ウィンドウ更新
};

#endif	// STATUS_H_INCLUDED
