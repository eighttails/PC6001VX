#ifndef REPLAY_H_INCLUDED
#define REPLAY_H_INCLUDED

#include "typedef.h"
#include "ini.h"

#define	REP_IDLE	(0)
#define	REP_RECORD	(1)
#define	REP_REPLAY	(2)


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class REPLAY {
protected:
	cIni *Ini;
	
	int RepST;					// ステータス
	
	BYTE *Matrix;				// キーマトリクスバッファポインタ
	int MSize;					// キーマトリクスサイズ
	
	DWORD RepFrm;				// フレームNo.カウンタ
	DWORD EndFrm;				// リプレイ終了フレーム
	
public:
	REPLAY();								// コンストラクタ
	~REPLAY();								// デストラクタ
	
	BOOL Init( int );						// 初期化
	
	BOOL GetStatus();						// ステータス取得
	
	BOOL StartRecord( char * );				// リプレイ記録開始
	void StopRecord();						// リプレイ記録停止
	BOOL ReplayWriteFrame( BYTE *, BOOL );	// リプレイ1フレーム書出し
	
	BOOL StartReplay( char * );				// リプレイ再生開始
	void StopReplay();						// リプレイ再生止
	BOOL ReplayReadFrame( BYTE * );			// リプレイ1フレーム読込み
	
};


#endif	// REPLAY_H_INCLUDED
