// ---------------------------------------------------------------------------
//	Virtual Bus Implementation
//	Copyright (c) cisc 1999.
//  をベースにゆみたろが細工(簡略化)したものです
// ---------------------------------------------------------------------------
//	$Id: device.h,v 1.21 1999/12/28 10:33:53 cisc Exp $

#ifndef DEVICE_H_INCLUDE
#define DEVICE_H_INCLUDE

#include "typedef.h"

// デフォルトポート数
#define	BANKSIZE	256

// ---------------------------------------------------------------------------
//	デバイスのインターフェース
// ---------------------------------------------------------------------------
struct IDevice {
	typedef DWORD ID;
	typedef BYTE (IDevice::*InFuncPtr)( int port );
	typedef void (IDevice::*OutFuncPtr)( int port, BYTE data );
	struct Descriptor{
		const InFuncPtr* indef;
		const OutFuncPtr* outdef;
	};
	
	virtual const ID& GetID() const = 0;
	virtual const Descriptor* GetDesc() const = 0;
};


// ---------------------------------------------------------------------------
//	Device
// ---------------------------------------------------------------------------
class Device : public IDevice {
public:
	Device( const ID& _id ) : id( _id ) {}
	virtual ~Device() {}
	
	const ID& GetID() const { return id; }
	const Descriptor* GetDesc() const { return 0; }
	
private:
	ID id;
};

// ---------------------------------------------------------------------------
// デバイスリストクラス
// ---------------------------------------------------------------------------
class DeviceList {
// 型定義
public:
	typedef IDevice::ID ID;
	
private:
	struct Node{
		IDevice* entry;
		Node* next;
		int count;
	};

// ここから本体
public:
	DeviceList() { node = 0; }
	~DeviceList();
	
	void Cleanup();
	bool Add( IDevice* t );
	bool Del( IDevice* t ) { return t->GetID() ? Del(t->GetID()) : false; }
	bool Del( const ID id );
	IDevice* Find( const ID id );
	
private:
	Node* FindNode( const ID id );
	
	Node* node;
};

// ---------------------------------------------------------------------------
//	IO 空間を提供するクラス
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
		DummyIO() : Device(0) {}
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

// ---------------------------------------------------------------------------

#define DEV_ID(a)		\
	(Device::ID((DWORD)a[0] + ((DWORD)a[1] << 8) + ((DWORD)a[2] << 16) + ((DWORD)a[3] << 24)))

// ---------------------------------------------------------------------------

#endif // DEVICE_H_INCLUDE
