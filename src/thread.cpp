// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "thread.h"

#ifdef USESDLTHREAD
#include <SDL.h>
#else
#include <windows.h>
#include <process.h>
#endif



// Constructer
cThread::cThread( void )
{
	this->m_bCancel			= FALSE;
	this->m_hThread			= NULL;
	this->m_BeginTheadParam	= NULL;
}


// Destructer
cThread::~cThread( void )
{
	BOOL bWaiting = this->Waiting();
	if( bWaiting == FALSE )
		#ifdef USESDLTHREAD
		SDL_KillThread( this->m_hThread );
		#else
		::TerminateThread( this->m_hThread, 0 );
		#endif
}


// Start Thread. 
BOOL cThread::BeginThread ( void *lpVoid )
{
	BOOL bSuccess = FALSE;
	
	if( this->m_hThread == NULL ){
		this->m_BeginTheadParam = lpVoid;
		this->m_bCancel			= FALSE;
		
		#ifdef USESDLTHREAD
                HTHREAD hThread = SDL_CreateThread( (int (*)(void *))ThreadProc, (void *)this );
		if( hThread >= 0 ){
			this->m_hThread = hThread;
			bSuccess = TRUE;
		}
		#else
		HTHREAD hThread = (HTHREAD)::_beginthread(ThreadProc, 0, reinterpret_cast<void*>(this) );
		if( hThread != (HTHREAD)(unsigned int)-1 ){
			this->m_hThread = hThread;
			::SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
			bSuccess = TRUE;
		}
		#endif
	}
	
	return bSuccess;
}


// Wait for create thread end. ( forvOnThread end )
//inline BOOL cThread::Waiting (DWORD dwWaitTime)
BOOL cThread::Waiting( void )
{
	BOOL bSuccess = FALSE;
	
	if( this->m_hThread != NULL ){
		#ifdef USESDLTHREAD
		int status = 0;
		SDL_WaitThread( this->m_hThread, &status );
		if( !status ){
		#else
		DWORD dwRet = ::WaitForSingleObject( this->m_hThread, INFINITE );
		if( dwRet == WAIT_OBJECT_0 ){
		#endif
			this->m_hThread = NULL;
			bSuccess = TRUE;
		}
	}else{
		bSuccess = TRUE;
	}
	
	return bSuccess;
}


void cThread::Cancel()
{
	this->cCritical::Lock();
	this->m_bCancel = TRUE;
	this->cCritical::UnLock();
}


BOOL cThread::IsCancel()
{
	BOOL bCancel = FALSE;
	this->cCritical::Lock();
	bCancel = this->m_bCancel;
	this->cCritical::UnLock();
	return bCancel;
}


// Default thread procedure. Don't call this method in direct!
void cThread::ThreadProc(void *lpVoid)
{
	static __thread cThread *lpThis;
	
	if( !lpThis ) lpThis = (cThread *)lpVoid;		// 自分自身のオブジェクトポインタ取得
	lpThis->OnThread( lpThis->m_BeginTheadParam );	// virtual Procedure 
	lpThis->m_hThread = NULL;
	#ifndef USESDLTHREAD
	::_endthread();
	#endif
}
