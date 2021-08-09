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
#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <mutex>
#include <thread>


class cThread {
private:
	bool m_bCancel;				// for Cancel().
	std::thread m_hThread;		// for Thread Handle.
	std::mutex mtx;
	
	void* m_BeginTheadParam;
	static int ThreadProc( void* );			// デフォルトスレッド関数
	
protected:
	virtual void OnThread( void* ) = 0;		// Virtual func. You need overwrite.
	
public:
	cThread();
	virtual ~cThread();
	
	bool BeginThread( void* = nullptr );	// スレッド開始
	bool Waiting();							// スレッド終了を待つ
	
	void Cancel();							// スレッド終了要求
	bool IsCancel();						// スレッド終了要求された？
};

#endif // THREAD_H_INCLUDED

