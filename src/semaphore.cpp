/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
// Based on SLibrary.
/////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/
/////////////////////////////////////////////////////////////////////////////
#include "semaphore.h"


/////////////////////////////////////////////////////////////////////////////
// Mutexl クラス
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cMutex::cMutex( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cMutex::~cMutex( void )
{
	mtx.unlock();
}


/////////////////////////////////////////////////////////////////////////////
// Lock
/////////////////////////////////////////////////////////////////////////////
void cMutex::lock( void )
{
	mtx.lock();
}


/////////////////////////////////////////////////////////////////////////////
// Unlock
/////////////////////////////////////////////////////////////////////////////
void cMutex::unlock( void )
{
	mtx.unlock();
}




/////////////////////////////////////////////////////////////////////////////
// Semaphore クラス
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
cSemaphore::cSemaphore( void ) : count( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
cSemaphore::~cSemaphore( void )
{
	{
		std::unique_lock<std::mutex> lock( mtx );
		count = 1;
	}
	cv.notify_all();
}


/////////////////////////////////////////////////////////////////////////////
// セマフォ加算
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cSemaphore::Post( void )
{
	{
		std::unique_lock<std::mutex> lock( mtx );
		count++;
	}
	cv.notify_one();
}


/////////////////////////////////////////////////////////////////////////////
// セマフォ待つ
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void cSemaphore::Wait( void )
{
	std::unique_lock<std::mutex> lock( mtx );
	
	cv.wait( lock, [&]{ return count > 0; } );
	count = 0;
}