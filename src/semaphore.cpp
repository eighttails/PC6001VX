// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "semaphore.h"


#ifdef USESDLCS
cCritical::cCritical( void ){ this->mcs = SDL_CreateMutex(); }
cCritical::~cCritical( void ){ SDL_DestroyMutex( this->mcs ); }
void cCritical::Lock( void ){ SDL_LockMutex( this->mcs ); }
void cCritical::UnLock( void ){ SDL_UnlockMutex( this->mcs ); }
#else
cCritical::cCritical( void ){ ::InitializeCriticalSection( &(this->mcs) ); }
cCritical::~cCritical( void ){ ::DeleteCriticalSection( &(this->mcs) ); }
void cCritical::Lock( void ){ ::EnterCriticalSection( &(this->mcs) ); }
void cCritical::UnLock( void ){ ::LeaveCriticalSection( &(this->mcs) ); }
#endif


#ifdef USESDLSEMAPHORE
cSemaphore::cSemaphore( void ){ sem = SDL_CreateSemaphore( 0 ); count = 0; }
cSemaphore::~cSemaphore( void ){ if( sem ) SDL_DestroySemaphore( sem ); }
DWORD cSemaphore::Value( void ){ return SDL_SemValue( sem ); }
int cSemaphore::Post( void ){ return SDL_SemPost( sem ); }
int cSemaphore::Wait( void ){ return SDL_SemWait( sem ); }
#else
cSemaphore::cSemaphore( void )
{
	sem = CreateSemaphore( NULL, 0, 32*1024, NULL );
	count = 0;
}


cSemaphore::~cSemaphore( void )
{
	if( sem ) CloseHandle( sem );
}


DWORD cSemaphore::Value( void )
{
	return count;
}


int cSemaphore::Post( void )
{
	InterlockedIncrement( &count );
	if( !ReleaseSemaphore( sem, 1, NULL ) ){
		InterlockedDecrement( &count );
		return -1;
	}
	return 0;
}


int cSemaphore::Wait( void )
{
	int ret = -1;
	
	switch( WaitForSingleObject( sem, INFINITE ) ){
    case WAIT_OBJECT_0:
		InterlockedDecrement( &count );
		ret = 0;
		break;
		
    case WAIT_TIMEOUT:
		ret = 1;
		break;
	}
	
	return ret;
}


#endif
