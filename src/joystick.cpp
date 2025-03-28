/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "joystick.h"
#include "osd.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
JOY6::JOY6( void )
{
	INITARRAY( JID, -1 );
	INITARRAY( Jinfo, nullptr );
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
JOY6::~JOY6( void )
{
	// オープンされていたらクローズする
	for( int i = 0; i < MAX_JOY; i++ ){
		if( OSD_OpenedJoy( Jinfo[i] ) ){ OSD_CloseJoy( Jinfo[i] ); }
	}
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool JOY6::Init( void )
{
	// オープンされていたらクローズする
	for( int i = 0; i < MAX_JOY; i++ ){
		if( OSD_OpenedJoy( Jinfo[i] ) ){ OSD_CloseJoy( Jinfo[i] ); }
	}
	
	ScanConnect();
//	Connect( 0, 1 );
//	Connect( 1, 2 );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック接続
//
// 引数:	jno		ジョイスティック番号(0-1)
//			index	インデックス
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool JOY6::Connect( int jno, int index )
{
	if( jno < 0 || jno > 1 ){
		return false;
	}
	
	if( index <= 0 ){
		// indexに0以下の値を設定した場合は接続解除
		// -1 : 未接続 ジョイスティックを検出したら自動的に接続する
		//  0 : 未接続 ジョイスティックを検出しても何もしない
		// >0 : 接続済
		JID[jno] = index;
		return true;
	}
	
 	if( index <= min( OSD_GetJoyNum(), MAX_JOY ) ){
		if( !OSD_OpenedJoy( Jinfo[index - 1] ) ){
			Jinfo[index - 1] = OSD_OpenJoy( index - 1 );
		}
 		if( OSD_OpenedJoy( Jinfo[index - 1] ) ){
 			JID[jno] = index;
 			return true;
 		}
 	}
	JID[jno] = -1;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// インデックス取得
//
// 引数:	jno		ジョイスティック番号(0-1)
// 返値:	int		インデックス
/////////////////////////////////////////////////////////////////////////////
int JOY6::GetID( int jno )
{
	return 	( jno == 0 || jno == 1 ) ? JID[jno] : -1;
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティックスキャン
/////////////////////////////////////////////////////////////////////////////
void JOY6::ScanConnect( void )
{
	int resjoy = 0;	// 予約済み(使用済み)
	
	for( int i = 0; i < 2; i++ ){
		if( (JID[i] < 0) || (JID[i] > 0 && !OSD_OpenedJoy( Jinfo[i] )) ){
			int j = resjoy;
			while( j++ < OSD_GetJoyNum() ){
				if( Connect( i, j ) ){
					resjoy = j;
					break;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック状態取得
//
// 引数:	jno		ジョイスティック番号(0-1)
// 返値:	BYTE	ジョイスティックの状態 1:OFF 0:ON <-注意!
//				bit7: ボタン4
//				bit6: ボタン3
//				bit5: ボタン2
//				bit4: ボタン1
//				bit3: 右
//				bit2: 左
//				bit1: 上
//				bit0: 下
/////////////////////////////////////////////////////////////////////////////
BYTE JOY6::GetJoyState( int jno )
{
	BYTE ret = 0xff;
	
	if( jno < 0 || jno > 1 || JID[jno] <= 0 ){
		return ret;
	}
	
	HJOYINFO jj = Jinfo[JID[jno] - 1];
	
	int Xmove = OSD_GetJoyAxis( jj, 0 );	// X軸
	int Ymove = OSD_GetJoyAxis( jj, 1 );	// Y軸
	
	if( Xmove < INT16_MIN / 2 ){ ret &= ~4; }	// 左
	if( Xmove > INT16_MAX / 2 ){ ret &= ~8; }	// 右
	if( Ymove < INT16_MIN / 2 ){ ret &= ~1; }	// 下
	if( Ymove > INT16_MAX / 2 ){ ret &= ~2; }	// 上
	
	// ボタン
	for( int i = 0; i < min( OSD_GetJoyNumButtons( jj ), 4 ); i++ ){
		if( OSD_GetJoyButton( jj, i ) ){
			ret &= ~(0x10 << i);
		}
	}
	
	return ret;
}
