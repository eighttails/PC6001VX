/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef DEVICE_H_INCLUDE
#define DEVICE_H_INCLUDE

#include <functional>
#include <map>
#include <memory>

#include "typedef.h"


#define DEV_ID(a)	BTODW((BYTE)a[0], (BYTE)a[1], (BYTE)a[2], (BYTE)a[3])


class VM6;
class MemCell;

// --------------------------------------------------------------------------
//	デバイスのインターフェース
//	  Original     : cisc
//	  Modification : Yumitaro
// --------------------------------------------------------------------------
struct IDevice {
	using ID         = DWORD;
	
	// I/Oアクセス関数
	using InFuncPtr  = BYTE (IDevice::*)( int );
	using OutFuncPtr = void (IDevice::*)( int, BYTE );
	
	// メモリアクセス関数
	using RFuncPtr   = BYTE (IDevice::*)( MemCell*, WORD, int* );
	using WFuncPtr   = void (IDevice::*)( MemCell*, WORD, BYTE, int* );
	using RFunc      = std::function<BYTE( MemCell*, WORD, int* )>;
	using WFunc      = std::function<void( MemCell*, WORD, BYTE, int* )>;
	RFunc FR( RFuncPtr fn ){ return std::bind( fn, std::ref(*this), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ); }
	WFunc FW( WFuncPtr fn ){ return std::bind( fn, std::ref(*this), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ); }
	
	// メモリブロック名取得用
	using NFuncPtr   = const std::string& (IDevice::*)( WORD, bool );
	using NFunc      = std::function<const std::string&( WORD, bool )>;
	NFunc FN( NFuncPtr fn ){ return std::bind( fn, std::ref(*this), std::placeholders::_1, std::placeholders::_2 ); }
	
	struct Descriptor{
		std::map<int, InFuncPtr>  indef;
		std::map<int, OutFuncPtr> outdef;
	};
	
	virtual const ID& GetID() const = 0;
	virtual const Descriptor& GetDescriptors() const = 0;
	virtual void EventCallback( int, int ) = 0;
};


// --------------------------------------------------------------------------
//	Device
//	  Original     : cisc
//	  Modification : Yumitaro
// --------------------------------------------------------------------------
class Device : public IDevice {
protected:
	VM6* vm;
	
	ID id;
	Descriptor descs;

public:
	Device( VM6*, const ID& );
	virtual ~Device();
	
	const ID& GetID() const override;
	const Descriptor& GetDescriptors() const override { return descs; }
	virtual void EventCallback( int, int ) override;
};


// --------------------------------------------------------------------------
// デバイスリストクラス
//	  Original     : cisc
//	  Modification : Yumitaro
// --------------------------------------------------------------------------
class DeviceList {
public:
	using ID = IDevice::ID;

private:
	struct Node{
		std::shared_ptr<IDevice> entry;
		int count;
	};
	
	std::map<int, Node> NodeMap;
	std::shared_ptr<IDevice> dummydev;

public:
	DeviceList();
	~DeviceList();
	
	bool Add( const std::shared_ptr<IDevice>& );
	bool Del( const ID );
	std::shared_ptr<IDevice>& Find( const ID );
};


#endif // DEVICE_H_INCLUDE
