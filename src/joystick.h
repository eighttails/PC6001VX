#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cJoy {
protected:
	int JID[2];						// インデックス
	
public:
	cJoy();							// コンストラクタ
	virtual ~cJoy();				// デストラクタ
	
	BOOL Init();					// 初期化
	
	BOOL Connect( int, int );		// ジョイスティック接続
	int GetID( int );				// インデックス取得
	BYTE GetJoyState( int );		// ジョイスティックの状態取得
};

#endif	// JOYSTICK_H_INCLUDED
