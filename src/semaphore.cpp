// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "semaphore.h"

#define USESDLCS					// SDLのクリティカルセクション使用
#define USESDLSEMAPHORE				// SDLのセマフォ使用


#ifdef USESDLCS
#include <SDL.h>
cCritical::cCritical( void )
{
    mcs = (HCRSECT)SDL_CreateMutex();
}


cCritical::~cCritical( void )
{
    SDL_DestroyMutex( (SDL_mutex *)mcs );
}


void cCritical::Lock( void )
{
    if( mcs ) SDL_LockMutex( (SDL_mutex *)mcs );
}


void cCritical::UnLock( void )
{
    if( mcs ) SDL_UnlockMutex( (SDL_mutex *)mcs );
}


#else

cCritical::cCritical( void )
{
    mcs = (HCRSECT) new CRITICAL_SECTION;
    InitializeCriticalSection( (LPCRITICAL_SECTION)mcs );
}


cCritical::~cCritical( void )
{
    if( mcs ) DeleteCriticalSection( (LPCRITICAL_SECTION)mcs );
}


void cCritical::Lock( void )
{
    if( mcs ) EnterCriticalSection( (LPCRITICAL_SECTION)mcs );
}


void cCritical::UnLock( void )
{
    if( mcs ) LeaveCriticalSection( (LPCRITICAL_SECTION)mcs );
}
#endif


#ifdef USESDLSEMAPHORE
#include <SDL.h>
cSemaphore::cSemaphore( void )
{
    sem = (HSEMAPHORE)SDL_CreateSemaphore( 0 );
    count = 0;
}


cSemaphore::~cSemaphore( void )
{
    if( sem ) SDL_DestroySemaphore( (SDL_sem *)sem );
}


DWORD cSemaphore::Value( void )
{
    return SDL_SemValue( (SDL_sem *)sem );
}


int cSemaphore::Post( void )
{
    return SDL_SemPost( (SDL_sem *)sem );
}


int cSemaphore::Wait( void )
{
    return SDL_SemWait( (SDL_sem *)sem );
}


#else
cSemaphore::cSemaphore( void )
{
    sem = (HSEMAPHORE)CreateSemaphore( NULL, 0, 32*1024, NULL );
    count = 0;
}


cSemaphore::~cSemaphore( void )
{
    if( sem ) CloseHandle( (HANDLE)sem );
}


DWORD cSemaphore::Value( void )
{
    return count;
}


int cSemaphore::Post( void )
{
    InterlockedIncrement( &count );
    if( !ReleaseSemaphore( (HANDLE)sem, 1, NULL ) ){
        InterlockedDecrement( &count );
        return -1;
    }
    return 0;
}


int cSemaphore::Wait( void )
{
    int ret = -1;

    switch( WaitForSingleObject( (HANDLE)sem, INFINITE ) ){
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
