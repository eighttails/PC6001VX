/*
	Skelton for Z-80 PC Emulator

	Author : Takeda.Toshiya
	Date   : 2004.04.26 -

	[ Device Base Class ]
*/
//  をベースにゆみたろが細工(簡略化)したものです

#ifndef P6DEVICE_H_INCLUDED
#define P6DEVICE_H_INCLUDED

#include "typedef.h"
#include "pc60.h"

class P6DEVICE {
public:
	typedef DWORD P6ID;
	
protected:
	VM6* vm;
	P6ID p6id;		// デバイスID
	
public:
	P6DEVICE( VM6* parent_vm, const P6ID& id ) : vm( parent_vm ), p6id( id ){}
	virtual ~P6DEVICE(){}
	
	const P6ID& GetID() const { return p6id; }
	virtual void EventCallback( int, int ){}	// イベントコールバック関数
};



#endif		// P6DEVICE_H_INCLUDED
