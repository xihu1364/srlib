/************************************************************************
* FileName : threadpool.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/16
************************************************************************/

#ifndef __TTHREADPOOL_H__
#define __TTHREADPOOL_H__

#include <windows.h>
#include <memory>
#include "ringqueue.h"
#include "MemoryPool.h"
#include "NetDefine.h"


typedef struct MSGDATA MsgData,*PMsgData;



using namespace std;

class ITaskObj
{
public:
	virtual int RunProcess(PMsgData pMsg = NULL)
	{
		TRACE("RunProcess run\n");

		return 0;
	}
};

typedef struct TASKPARAM
{
	weak_ptr<ITaskObj> wpTask;
	PMsgData pMsg;
}TaskParam;


class ThreadPool
{
public:
	int InitThreadPool(int nThreadDefualtNum = 14,int nThreadMaxNum = 200);
	int UnInitThreadPool();

	int AddTask(weak_ptr<ITaskObj> wpTask,PMsgData pMsg = NULL);

	int GetFreeThreadCount() const { return (int)m_nFreeNum;}
	int GetTotalThreadCount()const { return (int)m_nTotalNum;}
	int GetBusyThreadCount() const { return (int)(m_nTotalNum - m_nFreeNum);}
	int GetTaskCount()		 const { return m_TaskList.sizes();}

protected:
	static UINT APIENTRY ControlThreadProc(LPVOID lpParam);
	static UINT APIENTRY RunThreadProc(LPVOID lpParam);

private:
	volatile long m_nTotalNum;
	volatile long m_nFreeNum;
	volatile long m_nBusyNum;
	RingQueue<TaskParam> m_TaskList;

	BOOL m_bClose;

	long m_nThreadDefualtNum;
	long m_nThreadMaxNum;
	MemoryPool* m_pMemoryPool;

};
inline int ThreadPool::InitThreadPool(int nThreadDefualtNum/* = 14*/,int nThreadMaxNum /*= 200*/)
{
	int nRet = -1;

	m_nThreadDefualtNum = nThreadDefualtNum;

	m_nThreadMaxNum = nThreadMaxNum;

	m_bClose = FALSE;

	m_nTotalNum = 0;

	m_nFreeNum = 0;

	m_pMemoryPool = CreateInstance<MemoryPool>();

	assert(NULL != m_pMemoryPool);

	UINT dwThreadID = 0;

	HANDLE hThread = (HANDLE)_beginthreadex(NULL,0,&ControlThreadProc,this,CREATE_SUSPENDED,(UINT*)&dwThreadID);

	if (hThread != INVALID_HANDLE_VALUE)
	{
		SetThreadPriority(hThread,THREAD_PRIORITY_ABOVE_NORMAL);

		ResumeThread(hThread);

		CloseHandle(hThread);

		nRet = 0;
	}
	return nRet;
}
inline int ThreadPool::UnInitThreadPool()
{
	m_bClose = TRUE;

	return 0;
}

inline int ThreadPool::AddTask(weak_ptr<ITaskObj> wpTask,PMsgData pMsg/* = NULL*/)
{
	int nRet = -1;

	TaskParam* pTaskParam = (TaskParam*)m_pMemoryPool->Malloc(sizeof(TaskParam));

	new(pTaskParam) TaskParam();

	assert(NULL != pTaskParam);

	pTaskParam->wpTask = wpTask;

	if (pMsg != NULL)
	{
		pTaskParam->pMsg = (MsgData*)m_pMemoryPool->Malloc(sizeof(MsgData));

		memcpy(pTaskParam->pMsg,pMsg,sizeof(MsgData));
	}
	else
	{
		pTaskParam->pMsg = NULL;
	}
	nRet = m_TaskList.PushBack(pTaskParam);

	return nRet;
}

inline UINT ThreadPool::ControlThreadProc(LPVOID lpParam)
{
	ThreadPool* pThis = (ThreadPool*)lpParam;

	while (!pThis->m_bClose)
	{
		if (pThis->m_nFreeNum < pThis->m_nThreadDefualtNum)
		{
			if (pThis->m_nTotalNum < pThis->m_nThreadMaxNum)
			{
				UINT dwThreadID = 0;

				HANDLE hThread = (HANDLE)_beginthreadex(NULL,0,&RunThreadProc,lpParam,CREATE_SUSPENDED,(UINT*)&dwThreadID);

				if (hThread != INVALID_HANDLE_VALUE)
				{
// 					if (pThis->m_bUserDB)
// 					{
// 						CDBManager* pDBManager = CreateInstance<CDBManager>();
// 
// 						assert(NULL != pDBManager);
// 
// 						pDBManager->AddDbConnThread(dwThreadID);
// 					}
					ResumeThread(hThread);

					CloseHandle(hThread);

					InterlockedIncrement(&pThis->m_nTotalNum);

					InterlockedIncrement(&pThis->m_nFreeNum);

					continue;
				}
			}
		}
		else
		{
			Sleep(1);
		}
	}
	return 0;
}
inline UINT ThreadPool::RunThreadProc(LPVOID lpParam)
{
	ThreadPool* pThis = (ThreadPool*)lpParam;

	TaskParam* pTaskParam = NULL;

	while (!pThis->m_bClose)
	{
		pTaskParam = pThis->m_TaskList.PopFront();

		if (pTaskParam)
		{
			InterlockedDecrement(&pThis->m_nFreeNum);

			if (!pTaskParam->wpTask.expired())
			{
				shared_ptr<ITaskObj> spTask = pTaskParam->wpTask.lock();

				spTask->RunProcess(pTaskParam->pMsg);
			}
			if (pTaskParam->pMsg != NULL)
			{
				if (pTaskParam->pMsg->pMsgData != NULL)
				{
					pThis->m_pMemoryPool->Free(pTaskParam->pMsg->pMsgData);
				}
				pThis->m_pMemoryPool->Free(pTaskParam->pMsg);
			}
			pThis->m_pMemoryPool->Free(pTaskParam);

			pTaskParam = NULL;

			if (pThis->m_nFreeNum >= pThis->m_nThreadDefualtNum)
			{
				InterlockedDecrement(&pThis->m_nTotalNum);

				break;
			}
			InterlockedIncrement(&pThis->m_nFreeNum);
		}
		else
		{
			Sleep(1);

			continue;
		}
	}
	//CDBManager* pDBManager = CreateInstance<CDBManager>();

	//assert(NULL != pDBManager);

	//pDBManager->DelDbConnThread(GetCurrentThreadId());

	return 0;
}

#endif