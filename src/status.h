#ifndef STATUS_H_INCLUDED
#define STATUS_H_INCLUDED

#include "typedef.h"
#include "console.h"
#include "p6vm.h"


//------------------------------------------------------
//  ステータスバークラス
//------------------------------------------------------
class cWndStat : public ZCons {
private:
	VM6* vm;
	
	int DrvNum;							// ドライブ数
	int	ReplayStatus;					// リプレイステータス
	
public:
	cWndStat( VM6 * );					// コンストラクタ
	~cWndStat();						// デストラクタ
	
	bool Init( int, int = -1 );			// 初期化
	void Update();						// ウィンドウ更新
	
	void SetReplayStatus( int );		// リプレイステータスセット
};

#endif	// STATUS_H_INCLUDED
