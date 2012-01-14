#include "joystick.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cJoy::cJoy( void )
{
	JID[0] = JID[1] = -1;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cJoy::~cJoy( void )
{
	// オープンされていたらクローズする
	for( int i=0; i < OSD_GetJoyNum(); i++ )
		if( OSD_OpenedJoy( i ) ) OSD_CloseJoy( i );
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL cJoy::Init( void )
{
	// オープンされていたらクローズする
	for( int i=0; i < OSD_GetJoyNum(); i++ )
		if( OSD_OpenedJoy( i ) ) OSD_CloseJoy( i );
	
	Connect( 0, 0 );
	Connect( 1, 1 );
	
	return TRUE;
}



////////////////////////////////////////////////////////////////
// ジョイスティック接続
//
// 引数:	jno		ジョイスティック番号(0-1)
//			index	インデックス
// 返値:	BOOL	TRUE:成功 FALSE:失敗
////////////////////////////////////////////////////////////////
BOOL cJoy::Connect( int jno, int index )
{
	if( jno == 0 || jno == 1 ){
		if( index >= 0 && index < OSD_GetJoyNum() ){
			if( !OSD_OpenedJoy( index ) ) OSD_OpenJoy( index );
			if( OSD_OpenedJoy( index ) ){
				JID[jno] = index;
				return TRUE;
			}
		}
		JID[jno] = -1;
	}
	
	return FALSE;
}


////////////////////////////////////////////////////////////////
// インデックス取得
//
// 引数:	jno		ジョイスティック番号(0-1)
// 返値:	int		インデックス
////////////////////////////////////////////////////////////////
int cJoy::GetID( int jno )
{
	return 	( jno == 0 || jno == 1 ) ? JID[jno] : -1;
}


////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////
BYTE cJoy::GetJoyState( int jno )
{
	BYTE ret = 0;
	
	if( jno == 0 || jno == 1 ){
		if( JID[jno] >= 0 ){
			int Xmove, Ymove;
			
			// 軸
			Xmove = OSD_GetJoyAxis( JID[jno], 0 );
			Ymove = OSD_GetJoyAxis( JID[jno], 1 );
			
			if( Xmove < -16384 ) ret |= 4;	// 左
			if( Xmove >  16384 ) ret |= 8;	// 右
			if( Ymove < -16384 ) ret |= 1;	// 下
			if( Ymove >  16384 ) ret |= 2;	// 上
			
			// ボタン
			for( int i=0; i < min( OSD_GetJoyNumButtons( JID[jno] ), 4 ); i++ )
				if( OSD_GetJoyButton( JID[jno], i ) ) ret |= 0x10<<i;
		}
	}
	
	return ~ret;
}
