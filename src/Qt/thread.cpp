// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "../thread.h"

#include <QThread>
//QThreadをcTheradのインターフェースで使えるようにするためのラッパ
class InternalTherad : public QThread
{
public:
    InternalTherad(cThread* thread, void* param)
        : thread_(thread)
        , param_(param){};
protected:
    virtual void run(){
        thread_->OnThread(param_);
        thread_->m_hThread = NULL;
    }

private:
    cThread* thread_;
    void* param_;
};

// Constructer
cThread::cThread( void )
{
    this->m_bCancel			= true;
    this->m_hThread			= NULL;
	this->m_BeginTheadParam	= NULL;
}


// Destructer
cThread::~cThread( void )
{
	bool bWaiting = this->Waiting();
    if( m_hThread && bWaiting == false ){
        ((InternalTherad*)m_hThread)->terminate();
    }
    ((InternalTherad*)m_hThread)->deleteLater();
}


// Start Thread. 
bool cThread::BeginThread ( void *lpVoid )
{
	bool bSuccess = false;
	
	if( this->m_hThread == NULL ){
		this->m_BeginTheadParam = lpVoid;
		this->m_bCancel			= false;
		
        this->m_hThread = new InternalTherad(this, lpVoid);
        ((InternalTherad*)m_hThread)->start();
        bSuccess = true;
	}
	
	return bSuccess;
}


// Wait for create thread end. ( forvOnThread end )
//inline bool cThread::Waiting (DWORD dwWaitTime)
bool cThread::Waiting( void )
{
	bool bSuccess = false;
	
	if( this->m_hThread != NULL ){
        bSuccess = ((InternalTherad*)m_hThread)->wait();
        this->m_hThread = NULL;
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
    //何もしない(InternalTherad::run()で代替)
}
