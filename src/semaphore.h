/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2022 Yumitaro
/////////////////////////////////////////////////////////////////////////////
// Based on SLibrary.
/////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/
/////////////////////////////////////////////////////////////////////////////
#ifndef SEMAPHORE_H_INCLUDED
#define SEMAPHORE_H_INCLUDED

#include <condition_variable>
#include <mutex>


/////////////////////////////////////////////////////////////////////////////
// クラス定義
/////////////////////////////////////////////////////////////////////////////
// Mutex クラス
class cMutex {
private:
	std::mutex mtx;
	
public:
	cMutex();
	~cMutex();
	
	void lock();		// Lock
	void unlock();		// Unlock
};

// RecursiveMutex クラス
class cRecursiveMutex {
private:
	std::recursive_mutex mtx;

public:
	cRecursiveMutex();
	~cRecursiveMutex();

	void lock();		// Lock
	void unlock();		// Unlock
};


// Semaphore クラス
class cSemaphore {
private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
	
public:
	cSemaphore();
	~cSemaphore();
	
	void Post();		// セマフォ加算
	void Wait();		// セマフォ待つ
};


#endif // SEMAPHORE_H_INCLUDED
