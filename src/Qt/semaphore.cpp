// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include <QSemaphore>
#include <QMutex>

#include "../semaphore.h"


cCritical::cCritical( void )
{
	mcs = (HCRSECT)new QMutex();
}


cCritical::~cCritical( void )
{
	delete ((QMutex *)mcs);
}


void cCritical::Lock( void )
{
	if( mcs ) ((QMutex *)mcs)->lock();
}


void cCritical::UnLock( void )
{
	if( mcs ) ((QMutex *)mcs)->unlock();
}

cSemaphore::cSemaphore( void )
{
	sem = (HSEMAPHORE)new QSemaphore( 0 );
	count = 0;
}


cSemaphore::~cSemaphore( void )
{
	if( sem ) delete((QSemaphore *)sem);
}


DWORD cSemaphore::Value( void )
{
	return ((QSemaphore *)sem)->available();
}


int cSemaphore::Post( void )
{
	((QSemaphore *)sem)->release();
	return ((QSemaphore *)sem)->available();
}


int cSemaphore::Wait( void )
{
	((QSemaphore *)sem)->acquire();
	return ((QSemaphore *)sem)->available();
}

