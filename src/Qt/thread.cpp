// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "../thread.h"
#include <QThreadStorage>

#ifdef USESDLTHREAD
#include <SDL.h>
#else
#include <windows.h>
#include <process.h>
#endif



// Constructer
cThread::cThread( void )
{
	this->m_bCancel			= false;
	this->m_hThread			= NULL;
	this->m_BeginTheadParam	= NULL;
}


// Destructer
cThread::~cThread( void )
{
	bool bWaiting = this->Waiting();
	if( bWaiting == false )
		#ifdef USESDLTHREAD
        SDL_KillThread( (SDL_Thread*)this->m_hThread );
		#else
		::TerminateThread( this->m_hThread, 0 );
		#endif
}


// Start Thread. 
bool cThread::BeginThread ( void *lpVoid )
{
	bool bSuccess = false;
	
	if( this->m_hThread == NULL ){
		this->m_BeginTheadParam = lpVoid;
		this->m_bCancel			= false;
		
		#ifdef USESDLTHREAD
                HTHREAD hThread = SDL_CreateThread( (int (*)(void *))ThreadProc, (void *)this );
		if( hThread >= 0 ){
			this->m_hThread = hThread;
			bSuccess = true;
		}
		#else
		HTHREAD hThread = (HTHREAD)::_beginthread(ThreadProc, 0, reinterpret_cast<void*>(this) );
		if( hThread != (HTHREAD)(unsigned int)-1 ){
			this->m_hThread = hThread;
			::SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
			bSuccess = true;
		}
		#endif
	}
	
	return bSuccess;
}


// Wait for create thread end. ( forvOnThread end )
//inline bool cThread::Waiting (DWORD dwWaitTime)
bool cThread::Waiting( void )
{
	bool bSuccess = false;
	
	if( this->m_hThread != NULL ){
		#ifdef USESDLTHREAD
		int status = 0;
        SDL_WaitThread( (SDL_Thread*)this->m_hThread, &status );
		if( !status ){
		#else
		DWORD dwRet = ::WaitForSingleObject( this->m_hThread, INFINITE );
		if( dwRet == WAIT_OBJECT_0 ){
		#endif
			this->m_hThread = NULL;
			bSuccess = true;
		}
	}else{
		bSuccess = true;
	}
	
	return bSuccess;
}


void cThread::Cancel()
{
	this->cCritical::Lock();
	this->m_bCancel = true;
	this->cCritical::UnLock();
}


bool cThread::IsCancel()
{
	bool bCancel = false;
	this->cCritical::Lock();
	bCancel = this->m_bCancel;
	this->cCritical::UnLock();
	return bCancel;
}


// Default thread procedure. Don't call this method in direct!
void cThread::ThreadProc(void *lpVoid)
{
    // 呼び元のthisポインタをスレッド局所記憶(TLS)として利用する。
    // TLSを利用するためのコードはコンパイラ依存のため、Qtにその差異を吸収させる。
    // QThreadStorageはスレッド終了時に指しているポインタをdeleteする仕様のため、
    // このユースケースでは都合が悪い。
    // ここではダブルポインタを格納することで逃げる。
    static QThreadStorage<cThread **> tlsThis;
    cThread** ptr = new cThread*((cThread*)lpVoid);
    tlsThis.setLocalData(ptr);
    (*tlsThis.localData())->OnThread( (*tlsThis.localData())->m_BeginTheadParam );	// virtual Procedure
    (*tlsThis.localData())->m_hThread = NULL;
#ifndef USESDLTHREAD
    ::_endthread();
#endif
}
