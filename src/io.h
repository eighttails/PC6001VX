#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include "typedef.h"
#include "device.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class IO6 {
private:
	// オブジェクトポインタ
	IOBus *io;								// I/Oバス
	DeviceList *dl;							// デバイスリスト
	
	int Iwait[BANKSIZE];					// IN ウェイトテーブル
	int Owait[BANKSIZE];					// OUTウェイトテーブル
	
public:
	IO6();									// コンストラクタ
	~IO6();									// デストラクタ
	
	BOOL Init( int );						// 初期化
	
	BOOL Connect( IDevice*, const IOBus::Connector* );	// デバイス接続
	BOOL Disconnect( IDevice* );						// デバイス切断
	
	BYTE In( int, int * = NULL );			// IN関数
	void Out( int, BYTE, int * = NULL );	// OUT関数
	
	void SetInWait( int, int );				// IN ウェイト設定
	void SetOutWait( int, int );			// OUTウェイト設定
	int GetInWait( int );					// IN ウェイト取得
	int GetOutWait( int );					// OUTウェイト取得
};


#endif		// IO_H_INCLUDED
