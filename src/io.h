#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include "typedef.h"
#include "device.h"

// デフォルトポート数
#define	BANKSIZE	256

////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------------
//	IOBus : I/O空間を提供するクラス
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
class IOBus {
// 型定義
public:
	typedef Device::InFuncPtr InFuncPtr;
	typedef Device::OutFuncPtr OutFuncPtr;
	
	struct InBank{
		IDevice* device;
		InFuncPtr func;
		InBank* next;
	};
	struct OutBank{
		IDevice* device;
		OutFuncPtr func;
		OutBank* next;
	};
	
	enum ConnectRule{
		end = 0, portin = 1, portout = 2
	};
	struct Connector{
		DWORD bank;
		BYTE rule;
		BYTE id;
	};
	
private:
	class DummyIO : public Device{
	public:
		DummyIO() : Device(NULL,0) {}
		~DummyIO() {}
		
		BYTE dummyin( int );
		void dummyout( int, BYTE );
	};

// ここから本体
public:
	IOBus();
	~IOBus();
	
	bool Init( DeviceList* devlist = 0, int bs = BANKSIZE );
	
	bool ConnectIn( int bank, IDevice* device, InFuncPtr func );
	bool ConnectOut( int bank, IDevice* device, OutFuncPtr func );
	
	InBank* GetIns() { return ins; }
	OutBank* GetOuts() { return outs; }
	BYTE* GetFlags() { return flags; }
	
	bool Connect( IDevice* device, const Connector* connector );
	bool Disconnect( IDevice* device );
	BYTE In( int port );
	void Out( int port, BYTE data );
	
private:
	InBank* ins;
	OutBank* outs;
	BYTE* flags;
	DeviceList* devlist;
	
	int banksize;
	
	static DummyIO dummyio;
};
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
	
	bool Init( int );						// 初期化
	
	bool Connect( IDevice*, const IOBus::Connector* );	// デバイス接続
	bool Disconnect( IDevice* );						// デバイス切断
	
	BYTE In( int, int * = NULL );			// IN関数
	void Out( int, BYTE, int * = NULL );	// OUT関数
	
	void SetInWait( int, int );				// IN ウェイト設定
	void SetOutWait( int, int );			// OUTウェイト設定
	int GetInWait( int );					// IN ウェイト取得
	int GetOutWait( int );					// OUTウェイト取得
};


#endif		// IO_H_INCLUDED
