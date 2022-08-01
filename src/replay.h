/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef REPLAY_H_INCLUDED
#define REPLAY_H_INCLUDED

#include <vector>

#include "typedef.h"
#include "ini.h"


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
class REPLAY : public cIni {
protected:
	DWORD RepST;				// ステータス
	DWORD RepFrm;				// フレームNo.カウンタ
	DWORD EndFrm;				// リプレイ終了フレーム
	
public:
	REPLAY();
	~REPLAY();
	
	bool Init();												// 初期化
	
	DWORD GetStatus() const;									// ステータス取得
	
	bool StartRecord( const P6VPATH& );							// リプレイ記録開始
	bool ResumeRecord( const P6VPATH&, int );					// リプレイ記録再開
	void StopRecord();											// リプレイ記録停止
	bool ReplayWriteFrame( const std::vector<BYTE>&, bool );	// リプレイ1フレーム書出し
	
	bool StartReplay( const P6VPATH& );							// リプレイ再生開始
	void StopReplay();											// リプレイ再生止
	bool ReplayReadFrame( std::vector<BYTE>& );					// リプレイ1フレーム読込み
	
};


#endif	// REPLAY_H_INCLUDED
