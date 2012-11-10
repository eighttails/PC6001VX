// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
// CriticalSection
class cCritical {
private:
	HCRSECT mcs;
	
public:
	cCritical();
	~cCritical();
	
	void Lock();
	void UnLock();
};


// Semaphore
class cSemaphore {
private:
	HSEMAPHORE sem;
	long count;
	
public:
	cSemaphore();
	~cSemaphore();
	
	DWORD Value();
	int Post();
	int Wait();
};


#endif // SEMAPHORE_H_INCLUDED
