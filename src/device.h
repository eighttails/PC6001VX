#ifndef DEVICE_H_INCLUDE
#define DEVICE_H_INCLUDE

// 8888888888888888888888888888888888888
#include <map>
// 8888888888888888888888888888888888888
#include "typedef.h"

#define DEV_ID(a)	BTODW((BYTE)a[0], (BYTE)a[1], (BYTE)a[2], (BYTE)a[3])


class VM6;


// ---------------------------------------------------------------------------
//	デバイスのインターフェース
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
struct IDevice {
	typedef DWORD ID;
	typedef BYTE (IDevice::*InFuncPtr)( int port );
	typedef void (IDevice::*OutFuncPtr)( int port, BYTE data );
	struct Descriptor{
		const InFuncPtr *indef;
		const OutFuncPtr *outdef;
	};
	
	virtual const ID &GetID() const = 0;
	virtual const Descriptor *GetDesc() const = 0;
	virtual void EventCallback( int, int ) = 0;
	
	
	typedef BYTE (IDevice::*RFuncPtr)( BYTE *, WORD );
	typedef void (IDevice::*WFuncPtr)( BYTE *, WORD, BYTE );
};


// ---------------------------------------------------------------------------
//	Device
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
class Device : public IDevice {
protected:
	VM6 *vm;
	ID id;
	
public:
	Device( VM6 *_vm, const ID &_id ) : vm( _vm ), id( _id ) {}
	virtual ~Device() {}
	
	const ID &GetID() const { return id; }
	virtual const Descriptor *GetDesc() const { return 0; }
	virtual void EventCallback( int, int ){};
};


// ---------------------------------------------------------------------------
// デバイスリストクラス
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
class DeviceList {
public:
	typedef IDevice::ID ID;
	
private:
	struct Node{
		IDevice *entry;
		Node *next;
		int count;
	};
	
// 8888888888888888888888888888888888888
//	Node *node;
// 8888888888888888888888888888888888888
	std::map<int, Node> nodeMap;
	Node *FindNode( const ID );
// 8888888888888888888888888888888888888
	
public:
	DeviceList();
	~DeviceList();
	
	void Cleanup();
	bool Add( IDevice * );
	bool Del( IDevice * );
	bool Del( const ID );
	IDevice *Find( const ID );
};


#endif // DEVICE_H_INCLUDE
