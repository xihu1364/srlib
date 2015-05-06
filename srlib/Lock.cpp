/******************************************************************
*  FileName ：SRongLock.cpp
*  Autor    ：
*  Info	    ：
*  Data     ：
*******************************************************************/
#include "stdafx.h"
#include <cassert>
#include "Lock.h"


/************************************************************************
* Fuction : CriticalSection
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&m_CriticalSection);
}
/************************************************************************
* Fuction : ~CriticalSection
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&m_CriticalSection);
}
/************************************************************************
* Fuction : Lock
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL CriticalSection::Lock(unsigned int dwTimeout)
{
	assert(dwTimeout == INFINITE);
	EnterCriticalSection(&m_CriticalSection);
	return TRUE;
}
/************************************************************************
* Fuction : UnLock
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL CriticalSection::UnLock(long lCount, long*  lpPrevCount)
{
	assert(lCount == 1);
	assert(lpPrevCount == NULL);
	LeaveCriticalSection(&m_CriticalSection);
	return TRUE;
}
/************************************************************************
* Fuction : IsLock
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL CriticalSection::IsLock()
{
	return m_CriticalSection.LockCount >= 0;
}
/************************************************************************
* Fuction : Event
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Event::Event(char* pszName):m_Event(NULL)
{

}
/************************************************************************
* Fuction : Event
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Event::~Event()
{
	if (NULL != m_Event)
	{
		CloseHandle(m_Event);
		m_Event = NULL;
	}
	
}
/************************************************************************
* Fuction : Init
* Autor   : Lock
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::Lock(unsigned int dwTimeout)
{
	BOOL bRet = FALSE;
	assert(dwTimeout == INFINITE);
	bRet = SetEvent();
	return bRet;
}
/************************************************************************
* Fuction : UnLock
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::UnLock(long lCount, long*  lpPrevCount)
{
	BOOL bRet = FALSE;
	assert(lCount == 1);
	assert(lpPrevCount == NULL);
	bRet = ResetEvent();
	return bRet;
}
/************************************************************************
* Fuction : IsLock
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::IsLock()
{
	BOOL bRet = FALSE;
	bRet = GetEventSignal();
	return bRet;
}
/************************************************************************
* Fuction : Create
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::Create(char* pszName,BOOL bInitialSignal,BOOL bManualReset)
{
	BOOL bRet = FALSE;
	bRet = Open(pszName);
	if (bRet)
	{
		//该事件已经存在
	}
	else
	{
		if (NULL != m_Event)
		{
			CloseHandle(m_Event);
			m_Event = NULL;
		}
		m_Event = ::CreateEvent(NULL,bManualReset,bInitialSignal,pszName);
		if (NULL != m_Event)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
/************************************************************************
* Fuction : Open
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::Open(char* pszName)
{
	BOOL bRet = FALSE;
	if (NULL == pszName)
	{
		return bRet;
	}
	else
	{
		if (NULL != m_Event)
		{
			CloseHandle(m_Event);
			m_Event = NULL;
		}
		m_Event = ::OpenEvent(EVENT_ALL_ACCESS,false,pszName);
		if (NULL != m_Event)
		{
			bRet = TRUE;
		}
	}
	return bRet;
}
/************************************************************************
* Fuction : Close
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::Close()
{
	BOOL bRet = FALSE;
	bRet = CloseHandle(m_Event);
	m_Event = NULL;
	return bRet;
}
/************************************************************************
* Fuction : SetEvent
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::SetEvent()
{
	BOOL bRet = FALSE;
	bRet = ::SetEvent(m_Event);
	return bRet;
}
/************************************************************************
* Fuction : ResetEvent
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::ResetEvent()
{
	BOOL bRet = FALSE;
	bRet = ::ResetEvent(m_Event);
	return bRet;
}
/************************************************************************
* Fuction : PulseEvent
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::PulseEvent()
{
	BOOL bRet = FALSE;
	bRet = ::PulseEvent(m_Event);
	return bRet;
}
/************************************************************************
* Fuction : GetEventSignal
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::GetEventSignal()
{
	BOOL bRet = FALSE;
	bRet = WaitEvent(0);
	return bRet;
}
/************************************************************************
* Fuction : GetEventHandle
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
HANDLE Event::GetEventHandle()
{
	HANDLE hRet = NULL;
	hRet = m_Event;
	return hRet;
}
/************************************************************************
* Fuction : WaitEvent
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Event::WaitEvent(unsigned int uTimeout)
{
	BOOL bRet = FALSE;
	if (NULL == m_Event)
	{
		return bRet;
	}
	unsigned int l_uWaitRet = ::WaitForSingleObject(m_Event,uTimeout);
	if (WAIT_OBJECT_0 == l_uWaitRet)
	{
		bRet = TRUE;
	}
	else if (WAIT_TIMEOUT == l_uWaitRet)
	{
		//超时...
	}
	else if (WAIT_FAILED == l_uWaitRet)
	{
		//Bad call
	}
	return bRet;
}
/************************************************************************
* Fuction : WaitEvents
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int Event::WaitEvents()
{
	int nRet = -1;
	return nRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Semaphore::Semaphore(char* pszName):m_hSemaphore(NULL),m_lMaxCount(0)
{

}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Semaphore::~Semaphore()
{

}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::Lock(unsigned int dwTimeout)
{
	BOOL bRet = FALSE;
	bRet = Enter(dwTimeout);
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::UnLock(long lCount,long* lpPrevCount)
{
	BOOL bRet = FALSE;
	bRet = Leave(lCount,lpPrevCount);
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::IsLock()
{
	BOOL bRet = FALSE;
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::Create(long lCount,char* pszName)
{
	BOOL bRet = FALSE;
	if (NULL != m_hSemaphore)
	{
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
	m_lMaxCount = lCount;
	if (NULL != pszName)
	{
		m_hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, pszName);
		if (NULL != m_hSemaphore)
		{
			//已经存在该名字的信号量
			return bRet;
		}
	}
	m_hSemaphore = ::CreateSemaphore(NULL,lCount,lCount,pszName);
	if (NULL != m_hSemaphore)
	{
		bRet = TRUE;
	}
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::Enter(unsigned int uTimeout)
{
	BOOL bRet = FALSE;
	if (NULL == m_hSemaphore)
	{
		return bRet;
	}
	unsigned int l_uWaitRet = ::WaitForSingleObject(m_hSemaphore,uTimeout);
	if (WAIT_OBJECT_0 == l_uWaitRet)
	{
		bRet = TRUE;
	}
	else if (WAIT_TIMEOUT == l_uWaitRet)
	{
		//超时...
	}
	else if (WAIT_FAILED == l_uWaitRet)
	{
		//Bad call
	}
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::Leave(long lCount,long* lpPrevCount)
{
	BOOL bRet = FALSE;
	if (NULL == m_hSemaphore)
	{
		return bRet;
	} 
	bRet = ::ReleaseSemaphore(m_hSemaphore, lCount, lpPrevCount);
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Semaphore::Release()
{
	BOOL bRet = FALSE;
	if (NULL == m_hSemaphore)
	{
		return bRet;
	}
	bRet = ::ReleaseSemaphore(m_hSemaphore, m_lMaxCount, NULL);
	if (bRet)
	{
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Mutex::Mutex(char* pszName /* = NULL */):m_hMutex(NULL)
{

}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
Mutex::~Mutex()
{

}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Mutex::Lock(unsigned int dwTimeout)
{
	BOOL bRet = FALSE;
	if (NULL == m_hMutex)
	{
		return bRet;
	}
	unsigned int l_uWaitRet = WaitForSingleObject(m_hMutex,dwTimeout);
	if (WAIT_OBJECT_0 == l_uWaitRet)
	{
		bRet = TRUE;
	}
	else if (WAIT_TIMEOUT == l_uWaitRet)
	{
		//超时...
	}
	else if (WAIT_FAILED == l_uWaitRet)
	{
		//Bad call
	}
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Mutex::UnLock(long lCount,long* lpPrevCount)
{
	BOOL bRet = FALSE;
	assert(lCount == 1);
	assert(lpPrevCount == NULL);
	bRet = Release();
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Mutex::IsLock()
{
	BOOL bRet = FALSE;
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Mutex::Create(char* pszName,BOOL bInit)
{
	BOOL bRet = FALSE;
	if (NULL != pszName)
	{
		if (NULL != m_hMutex)
		{
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
		m_hMutex = OpenMutex(SEMAPHORE_ALL_ACCESS, FALSE, pszName);
		if (NULL != m_hMutex)
		{
			//已经存在该名称的互斥量
			bRet = TRUE;
			return bRet;
		}
	}
	m_hMutex = ::CreateMutex(NULL,bInit,pszName);
	if (NULL != m_hMutex)
	{
		bRet = TRUE;
	}
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL Mutex::Release()
{
	BOOL bRet = FALSE;
	if (NULL == m_hMutex)
	{
		return bRet;
	}
	bRet = ReleaseMutex(m_hMutex);
	return bRet;
}
/************************************************************************
* Fuction : CSRongSyncObj
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSRongSyncObj::CSRongSyncObj(CSRongLockBase* pLock)
{
	assert(pLock != NULL);
	m_pLock = pLock;
	m_pLock->Lock();
}
/************************************************************************
* Fuction : ~CSRongSyncObj
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSRongSyncObj::~CSRongSyncObj()
{
	assert(m_pLock != NULL);
	m_pLock->UnLock();
}



/************************************************************************/
/*                              单写多读锁                              */
/************************************************************************/ 
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSRongMultiWriteSingleRead::CSRongMultiWriteSingleRead()
{
	InitializeCriticalSection(&(m_MRSWLock.s_CriticalSection));
	m_MRSWLock.s_bReadFlag = FALSE;
	m_MRSWLock.s_nReadCount = 0;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSRongMultiWriteSingleRead::~CSRongMultiWriteSingleRead()
{
	EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
	//空加锁，避免风险。    注：在未解锁的情况下删除锁，会导致问题
	LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
	DeleteCriticalSection(&(m_MRSWLock.s_CriticalSection));
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSRongMultiWriteSingleRead::EnableWrite()
{
	UINT iRet = 0;
	while (1)
	{
		EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
		if (!m_MRSWLock.s_bReadFlag)
		{
			m_MRSWLock.s_bReadFlag = TRUE;
			LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
			break;
		}
		LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
		Sleep(1);
	}
	while (1)
	{
		iRet = GetReadCount();
		if (iRet == 0)
		{
			break;
		}
		else
		{
			//注意，这里就会一直等读的数量为零才会结束，不然会一直循环。 
			//是否需要改下？当别人忘记释放读锁，那么就一直循环。改成循环多少次会返回设置失败？
			Sleep(1);
		}
	}
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSRongMultiWriteSingleRead::DisableWrite()
{
	EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
	m_MRSWLock.s_bReadFlag = FALSE;
	LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
BOOL CSRongMultiWriteSingleRead::GetWriteFlag()
{
	BOOL bRet = FALSE;
	EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
	bRet = m_MRSWLock.s_bReadFlag;
	LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
	return bRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
UINT CSRongMultiWriteSingleRead::GetReadCount()
{
	UINT iRet = 0;
	EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
	iRet = m_MRSWLock.s_nReadCount;
	LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
	return iRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSRongMultiWriteSingleRead::DecReadCount()
{
	EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
	if (m_MRSWLock.s_nReadCount > 0)
	{
		m_MRSWLock.s_nReadCount--;
	}
	LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSRongMultiWriteSingleRead::AddReadCount()
{
	while (1)
	{
		EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
		if (!m_MRSWLock.s_bReadFlag)
		{
			m_MRSWLock.s_nReadCount++;
			LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
			break;
		}
		LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
		Sleep(1);
	}

}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSRongMultiWriteSingleRead::ReadToWrite()
{
	UINT iRet = 0;
	while(1)
	{
		EnterCriticalSection(&(m_MRSWLock.s_CriticalSection));
		if (!m_MRSWLock.s_bReadFlag)
		{
			m_MRSWLock.s_bReadFlag = TRUE;
			if (m_MRSWLock.s_nReadCount > 0)
			{
				m_MRSWLock.s_nReadCount--;
			}
			LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
			break;
		}
		LeaveCriticalSection(&(m_MRSWLock.s_CriticalSection));
		Sleep(1);
	}
	while (1)
	{
		iRet = GetReadCount();
		if (iRet == 0)
		{
			break;
		}
		else
		{
			//注意，这里就会一直等读的数量为零才会结束，不然会一直循环。 
			//是否需要改下？当别人忘记释放读锁，那么就一直循环。改成循环多少次会返回转换失败？
			Sleep(1);
		}
	}
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
MWSR_int::MWSR_int()
{
	m_Lock.EnableWrite();
	m_nValue =(0);
	m_Lock.DisableWrite();
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
MWSR_int::~MWSR_int()
{

}
// int MWSR_int::operator = (const int& rValue)
// {
// 	int nRet = 0;
// 	m_Lock.EnableWrite(); 
// 	{
// 		m_nValue = rValue;
// 	} 
// 	m_Lock.DisableWrite();
// 	return nRet;
// }
// int MWSR_int::operator = (MWSR_int& temp)
// {
// 	int nRet = 0;
// 	m_Lock.EnableWrite(); 
// 	{
// 		this.m_nValue = temp.GetValue();
// 	} 
// 	m_Lock.DisableWrite();
// 	return nRet;
// }
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int MWSR_int::operator ++()
{
	int nRet = 0;
	m_Lock.EnableWrite(); 
	{
		nRet = ++m_nValue;
	} 
	m_Lock.DisableWrite();
	return nRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int MWSR_int::operator--()
{
	int nRet = 0;
	m_Lock.EnableWrite(); 
	{
		nRet = --m_nValue;
	} 
	m_Lock.DisableWrite();
	return nRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int MWSR_int::operator-(MWSR_int& rValue)
{
	int nRet = 0;
	nRet = m_nValue - rValue.GetValue();
	return nRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int MWSR_int::GetValue() const
{
	int nRet = 0;
	m_Lock.AddReadCount();
	{
		nRet = m_nValue;
	}
	m_Lock.DecReadCount();
	return nRet;
}
/************************************************************************
* Fuction : WriteRunLog(LEVEL_TYPE eLevel,char* pBuffer,...)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int MWSR_int::SetValue(const int& nValue)
{
	int nRet = 0;
	m_Lock.EnableWrite();
	{
		m_nValue = nValue;
	}
	m_Lock.DisableWrite();
	return nRet;
}
//////////////////////////////////////////////////////////////////end