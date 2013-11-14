#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include "typedef.h"

#define	MAX_JOY			(8)			// ジョイスティック最大接続数

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class JOY6 {
protected:
	int JID[2];						// インデックス
	HJOYINFO Jinfo[MAX_JOY];		// ジョイスティック情報
	
public:
	JOY6();							// コンストラクタ
	virtual ~JOY6();				// デストラクタ
	
	bool Init();					// 初期化
	
	bool Connect( int, int );		// ジョイスティック接続
	int GetID( int );				// インデックス取得
	BYTE GetJoyState( int );		// ジョイスティックの状態取得
};

#endif	// JOYSTICK_H_INCLUDED
