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
	
	char TapeName[MAX_PATH];			// ファイル名
	bool TapeIsAutoStart;				// オートスタート?
	bool TapeIsOpen;					// オープン?
	int TapeSize;						// サイズ
	int TapeCount;						// カウンタ
	
	int DrvNum;							// ドライブ数
	char DiskName[2][MAX_PATH];			// ファイル名
	bool DiskIsSystem[2];				// システムディスク?
	bool DiskIsProtect[2];				// プロテクト?
	bool DiskIsAccess[2];				// アクセス中?
	
	int	KeyIndicator;					// かな&CAPS インジケータ
	
	int	ReplayStatus;					// リプレイステータス
	
public:
	cWndStat( VM6 * );					// コンストラクタ
	~cWndStat();						// デストラクタ
	
	bool Init( int, int = -1 );			// 初期化
	void Update();						// ウィンドウ更新
	
	void TapeMount( char *, bool, int );		// TAPE マウント
	void TapeUnmount();							// TAPE アンマウント
	
	void DiskMount( int, char *, bool, bool );	// DISK マウント
	void DiskUnmount( int );					// DISK アンマウント
	
	void SetReplayStatus( int );				// リプレイステータスセット
};

#endif	// STATUS_H_INCLUDED
