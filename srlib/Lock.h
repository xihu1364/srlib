/******************************************************************
*  FileName ：SRongLock.h
*  Autor    ：xihu
*  Info	    ：
*  Data     ：
*******************************************************************/
#ifndef  __SRongLock_H__
#define	 __SRongLock_H__

//锁基类
class CSRongLockBase
{
public:
	CSRongLockBase(){ };
	virtual ~CSRongLockBase() { };
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE) = 0;

	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL) = 0;

	virtual BOOL IsLock() = 0;
};

/*
	临界区:  适用于单个进程中的多线程,相对于其他同步机制，效率快.缺点:不能运用与不同进程,当本线程为释放,则其他线程得不到锁，和去释放锁.
*/
class CriticalSection : public CSRongLockBase
{
public:
	CriticalSection();
	virtual ~CriticalSection();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL);
	virtual BOOL IsLock();
private:
	CRITICAL_SECTION m_CriticalSection;
};

/*
	事件:   适用于多线程,多进程.比临界区效率的速度慢
	Win32 api有:
		OpenEvent();   //根据名称打开一个已经打开的事件 OpenEvent(EVENT_ALL_ACCESS,false,pszName)
		CreateEvent(); //创建一个事件 CreateEvent(NULL,[自动]TRUE or[手动]FALSE,[初始状态]TRUE[有信号] or FALSE[无信号],pszName )
			参数:
		CloseHandle(); //关闭事件
		SetEvent();    //设置事件为有信号状态
		ResetEvent();  //设置事件为无信号状态
		PulseEvent();  //设置特定的事件对象为有信号状态，然后释放相关的等待线程，再置时间对象为无信号状态,
					   //MSDN说了，这个函数是不好的，最好不要使用
   使用方法：
   1.创建对象
   2.调用Create，并设置手动/自动,初始值,名称.
   3.SetEvent设置有信号状态
   4.WaitEvent等待信号
   5.ResetEvent设置无信号
*/
class Event : public CSRongLockBase
{
public:
	Event(char* pszName = NULL);
	virtual ~Event();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL);
	virtual BOOL IsLock();//从基类继承的接口，判断是否有事件信号 有(true) 无(false)
	BOOL Create(char* pszName = NULL,BOOL bInit = FALSE,BOOL bManualReset = FALSE);
	BOOL Open(char* pszName = NULL);
	BOOL Close();
	BOOL SetEvent();
	BOOL ResetEvent();
	BOOL PulseEvent();
	BOOL GetEventSignal();
	BOOL WaitEvent(unsigned int uTimeout = INFINITE);
	HANDLE GetEventHandle();
	//此接口待以后使用  提高一个公共方法 应该是static类型
	int WaitEvents();
private:
	HANDLE m_Event;
};

/*
	信号量：与事件基本相同，但多一个计数量.
	API
		OpenSemaphore();
		CreateSemaphore();
		CloseHandle();
		ReleaseSemaphore();
		WaitForSingleObject();
	使用方法：
	1.创建对象
	2.调用Create，并设置最大信号量MaxCount
	3.调用Enter，如果调用的次数大于MaxCount，则无限等待
	4.调用Leave将调用Enter的次数减lCount
*/
class Semaphore : public CSRongLockBase
{
public:
	Semaphore(char* pszName = NULL);
	virtual ~Semaphore();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1,long* lpPrevCount = NULL);
	virtual BOOL IsLock();
	BOOL Create(long lCount,char* pszName = NULL);
	BOOL Enter(unsigned int uTimerOut = INFINITE);
	BOOL Leave(long lCount = 1,long* lpPrevCount = NULL);
	BOOL Release();
private:
	HANDLE m_hSemaphore;
	long    m_lMaxCount;
};
//互斥量
/*
互斥对象包含一个使用数量，一个线程I D和一个递归计数器。互斥对象的行为特性与关键代码段相同，
但是互斥对象属于内核对象，而关键代码段则属于用户方式对象。这意味着互斥对象的运行速度比关键代码段要慢。
但是这也意味着不同进程中的多个线程能够访问单个互斥对象，并且这意味着线程在等待访问资源时可以设定一个超时值。

*/
class Mutex : public CSRongLockBase
{
public:
	Mutex(char* pszName = NULL);
	virtual ~Mutex();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1,long* lpPrevCount = NULL);
	virtual BOOL IsLock();
	BOOL Create(char* pszName = NULL,BOOL bInit = FALSE);
	BOOL Release();
private:
	HANDLE m_hMutex;
};

//锁对象类--单写单读
class CSRongSyncObj
{
public:
	CSRongSyncObj(CSRongLockBase* pLock);
	~CSRongSyncObj();
private:
	CSRongLockBase* m_pLock;
};

//锁对象类--单写多读
//原理：有一个读计数器，一个写标记，一个临界区锁
//在只读的情况下，可以多次读，不能写，在写的情况下，不能写，也不能读。
typedef struct tag_MultiWriteSingleRead
{
	UINT s_nReadCount;
	BOOL s_bReadFlag;
	CRITICAL_SECTION s_CriticalSection;
}SMultiWriteSingleRead;

class CSRongMultiWriteSingleRead
{
private:
	SMultiWriteSingleRead m_MRSWLock;
public:
	CSRongMultiWriteSingleRead();
	~CSRongMultiWriteSingleRead();
	//可写
	void EnableWrite();
	//不可写
	void DisableWrite();
	//获取写标记
	BOOL GetWriteFlag();
	//获取已读数量
	UINT GetReadCount();
	//增加一个写
	void AddReadCount();
	//减少一个写
	void DecReadCount();
	//从读转换成写
	void ReadToWrite();
};

class MWSR_int
{
public:
	MWSR_int();
	~MWSR_int();

	int operator==(int&);
	int operator++();
	int operator--();
	int operator+(int&);
	int operator+(MWSR_int&);
	int operator-(int&);
	int operator-(MWSR_int&);
	int operator*(int&);
	int operator/(int&);
	int GetValue()const;
	int SetValue(const int&);
	int SetValue(const MWSR_int&);
private:
	int m_nValue;
	mutable CSRongMultiWriteSingleRead m_Lock;
};



#endif