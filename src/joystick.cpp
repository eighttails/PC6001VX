#include "joystick.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
JOY6::JOY6( void )
{
	INITARRAY( JID, -1 );
	INITARRAY( Jinfo, NULL );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
JOY6::~JOY6( void )
{
	// オープンされていたらクローズする
	for( int i=0; i < MAX_JOY; i++ )
		if( Jinfo[i] && OSD_OpenedJoy( i ) ) OSD_CloseJoy( Jinfo[i] );
}


////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool JOY6::Init( void )
{
	// オープンされていたらクローズする
	for( int i=0; i < MAX_JOY; i++ )
		if( Jinfo[i] && OSD_OpenedJoy( i ) ) OSD_CloseJoy( Jinfo[i] );
	
	Connect( 0, 0 );
	Connect( 1, 1 );
	
	return true;
}



////////////////////////////////////////////////////////////////
// ジョイスティック接続
//
// 引数:	jno		ジョイスティック番号(0-1)
//			index	インデックス
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool JOY6::Connect( int jno, int index )
{
	if( ( jno == 0 || jno == 1 ) && index >= 0 && index < min( OSD_GetJoyNum(), MAX_JOY ) ){
		if( !OSD_OpenedJoy( index ) )
			Jinfo[index] = OSD_OpenJoy( index );
		
		if( OSD_OpenedJoy( index ) ){
			JID[jno] = index;
			return true;
		}
		JID[jno] = -1;
		Jinfo[index] = NULL;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////
// インデックス取得
//
// 引数:	jno		ジョイスティック番号(0-1)
// 返値:	int		インデックス
////////////////////////////////////////////////////////////////
int JOY6::GetID( int jno )
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
BYTE JOY6::GetJoyState( int jno )
{
	BYTE ret = 0;
	
	if( jno == 0 || jno == 1 ){
		if( JID[jno] >= 0 ){
			int Xmove, Ymove;
			
			// 軸
			Xmove = OSD_GetJoyAxis( Jinfo[JID[jno]], 0 );
			Ymove = OSD_GetJoyAxis( Jinfo[JID[jno]], 1 );
			
			if( Xmove < INT16_MIN/2 ) ret |= 4;	// 左
			if( Xmove > INT16_MAX/2 ) ret |= 8;	// 右
			if( Ymove < INT16_MIN/2 ) ret |= 1;	// 下
			if( Ymove > INT16_MAX/2 ) ret |= 2;	// 上
			
			// ボタン
			for( int i=0; i < min( OSD_GetJoyNumButtons( Jinfo[JID[jno]] ), 4 ); i++ )
				if( OSD_GetJoyButton( Jinfo[JID[jno]], i ) ) ret |= 0x10<<i;
		}
	}
	
	return ~ret;
}
