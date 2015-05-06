/************************************************************************
* FileName : syndeque.h
* Autor    : xihu
* Info     : 2012-01
************************************************************************/

#ifndef __SYNDEQUE_H__
#define __SYNDEQUE_H__


#include <deque>
using namespace std;


template <class T>
class SynDeque
{
public:
	SynDeque();
	~SynDeque();
	void PushBack(T& obj);
	void PopFront(T& obj);
	int GetSize();
	void Clear();
	BOOL WaiteForQueue(T& obj,int nWaitMilliseconds);
private:
	deque<T>			m_deque;
	CRITICAL_SECTION	m_cs;
	HANDLE				m_hevent;
};

template <class T>
SynDeque<T>::SynDeque():m_hevent(NULL)
{
	InitializeCriticalSection(&m_cs);
	m_hevent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_deque.empty();
}
template <class T>
SynDeque<T>::~SynDeque()
{
	DeleteCriticalSection(&m_cs);
	if (NULL != m_hevent)
	{
		CloseHandle(m_hevent);
		m_hevent = NULL;
	}
	m_deque.empty();
}
template <class T>
void SynDeque<T>::PushBack(T& obj)
{
	try
	{
		EnterCriticalSection(&m_cs);
		m_deque.push_back(obj);
		SetEvent(m_hevent);

	}
	catch(...)
	{
		LeaveCriticalSection(&m_cs);
	}
	LeaveCriticalSection(&m_cs);
}
template <class T>
void SynDeque<T>::PopFront(T& obj)
{
	try
	{
		EnterCriticalSection(&m_cs);
		if (m_deque.size() > 0)
		{
			obj = m_deque.front();

			m_deque.pop_front();

			if (!m_deque.size())
			{
				ResetEvent(m_hevent);
			}
		}
	}
	catch(...)
	{
		LeaveCriticalSection(&m_cs);
	}
	LeaveCriticalSection(&m_cs);
}
template <class T>
void SynDeque<T>::Clear()
{
	try
	{
		EnterCriticalSection(&m_cs);
		m_deque.empty();
		if (!m_deque.size())
		{
			ResetEvent(m_hevent);
		}
	}
	catch(...)
	{
		LeaveCriticalSection(&m_cs);
	}
	LeaveCriticalSection(&m_cs);
}
template <class T>
BOOL SynDeque<T>::WaiteForQueue(T& obj,int nWaitMilliseconds)
{
	BOOL bRet = FALSE;
	unsigned int l_uWaitRet = ::WaitForSingleObject(m_hevent,nWaitMilliseconds);

	if (WAIT_OBJECT_0 == l_uWaitRet)
	{
		PopFront(obj);
		bRet = TRUE;
	}
	return bRet;
}
template <class T>
int SynDeque<T>::GetSize()
{
	int nRet = -1;
	try
	{
		EnterCriticalSection(&m_cs);
		nRet = (int)m_deque.size();
	}
	catch(...)
	{
		LeaveCriticalSection(&m_cs);
	}
	LeaveCriticalSection(&m_cs);
	return nRet;
}

#endif