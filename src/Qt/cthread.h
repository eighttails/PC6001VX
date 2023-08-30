// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#ifndef CTHREAD_H_
#define CTHREAD_H_

#include "typedef.h"
#include <QMutex>

class cThread {
	friend class InternalThread;
private:
	QMutex m_Mutex;
	bool m_bCancel;				// for Cancel().
	HTHREAD m_hThread;			// for Thread Handle.
	void *m_BeginThreadParam;

	static void ThreadProc( void * );		// デフォルトスレッド関数

protected:
	virtual void OnThread( void * ) = 0;	// Virtual func. You need overwrite.

public:
	cThread();
	virtual ~cThread();

	bool BeginThread( void * = nullptr );	// スレッド開始
	bool Waiting();						// スレッド終了を待つ

	void Cancel();						// スレッド終了要求
	bool IsCancel();					// スレッド終了要求された？
	void yield();						// スレッドを明け渡す
};

#endif // CTHREAD_H_

