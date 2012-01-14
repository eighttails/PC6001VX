// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "typedef.h"

#include "semaphore.h"


class cThread : public cCritical {
private:
	BOOL m_bCancel;				// for Cancel().
	HTHREAD m_hThread;			// for Thread Handle.
	void *m_BeginTheadParam;
	
	static void ThreadProc( void * );	// Default thread procedure. Don't call this method in direct!
	
protected:
	virtual void OnThread( void * ) = 0;	// Virtual func. You need overwrite.
	
public:
	cThread();
	virtual ~cThread();
	
	BOOL BeginThread( void * = NULL );	// Start Thread. 
	BOOL Waiting();						// Wait for created thread end.
	
	void Cancel();						// Method for cancel.
	BOOL IsCancel();
};

#endif // THREAD_H_INCLUDED

