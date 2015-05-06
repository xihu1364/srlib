/************************************************************************
* FileName : IocpDriver.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/19
************************************************************************/
#ifndef __IOCPDRIVER_H__
#define __IOCPDRIVER_H__

#include <Mswsock.h>
#pragma comment(lib,"Mswsock.lib")
#include <MSTcpIP.h>
#include <cassert>
#include "MemoryPool.h"
#include "NetDefine.h"


class win_iocp
{
public:
	win_iocp() : m_hIocp(NULL){}

	~win_iocp() 
	{ 
		if (NULL != m_hIocp)
		{ 
			CloseHandle(m_hIocp);
		}
	}
	BOOL CreateIocp() 
	{ 
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL,0,0); 

		assert(m_hIocp != NULL); 

		return m_hIocp != NULL;
	}
	BOOL AssociateDevice(HANDLE hDevice,ULONG_PTR ulCompKey)
	{
		BOOL bOk = (CreateIoCompletionPort(hDevice,m_hIocp,ulCompKey,0) == m_hIocp);

		if (!bOk)
		{
			int nError = GetLastError();

			if (nError != 87)
			{
				assert(bOk);
			}
			else
			{
				bOk = TRUE;
			}
		}
		return bOk;
	}
	BOOL PostStatus(ULONG_PTR ulCompKey,DWORD dwNumBytes = 0,OVERLAPPED *pOver = NULL)
	{
		BOOL bOk = PostQueuedCompletionStatus(m_hIocp,dwNumBytes,ulCompKey,pOver);

		assert(bOk);

		return bOk;
	}
	BOOL GetStatus(ULONG_PTR *pCompKey,PDWORD pdwNumBytes,OVERLAPPED **pOver,DWORD dwMilliseconds = INFINITE)
	{
		return GetQueuedCompletionStatus(m_hIocp,pdwNumBytes,pCompKey,pOver,dwMilliseconds);
	}
protected:

private:
	HANDLE m_hIocp;
};

class AsyncResult;
class IocpDriver;
class IoManager;

typedef int (IocpDriver::*PCALLBACKFUN)(AsyncResult*,int);

class AsyncResult : public OVERLAPPED
{
public:
	SOCKET			m_sListen;
	SOCKET			m_sDealSocket;
	PCALLBACKFUN	m_pCallBackFunc;
	char*			m_pBuf;
	int				m_nCurFlag;

	AsyncResult()
	{
		memset(this,0,sizeof(AsyncResult));
	}
};

typedef struct ACCEPTPARAM
{
	IocpDriver* pIocpDriver;
	SOCKET		sListen;

}AcceptParam,*PAcceptParam;

class IocpDriver
{
public:
	IocpDriver();

	~IocpDriver();

	int InitDriver();

	int Listen(SOCKET& sListen,int nPort,char* pIp = NULL);

	int CloseConn(SOCKET sServer,SOCKET sClient);

	int SendMessage(SOCKET sSocket,char* pBuf,int nLen);

	int Connect(SOCKET& sConn,int nPort,char* pIp);

	int SendMessage(SOCKET sServer,SOCKET sClient,char* pszBuf,int nLen) { return PostSendIo(sServer,sClient,pszBuf,nLen);}

	int GetPostIoCount() const { return m_nPostAcceptIoStat;}

protected:

	enum { MAX_ACCEPT = 10 };

	BOOL AssociateDevice(HANDLE hHandle,void* pIoClient = NULL);

	int PostSendIo(SOCKET sServer,SOCKET sClient,char* pszBuf,int nLen);

	int GetAcceptIoStat() {return m_nPostAcceptIoStat;}
	 
	static unsigned int __stdcall MtAcceptIoThread(void* pParam);

	int MtAcceptIo(SOCKET sListen);

	static unsigned int __stdcall DealIoImplThread(void* pParam);

	int DealIoImpl();

	int OnAccept(AsyncResult* pAsyncResult,int nNumBytes);

	int OnPreRecv(AsyncResult* pAsyncResult,int nNumBytes);

	int OnRecv(AsyncResult* pAsyncResult,int nNumBytes);

	int OnSend(AsyncResult* pAsyncResult,int nNumBytes);

	int OnDisconn(AsyncResult* pAsyncResult,int nNumBytes);

	int PostAcceptIo(AsyncResult* pAsyncResult);

	int PostIoMsg(SOCKET sServer,SOCKET sClient,int nIoType,char* pszBuf,int nNumBytes,int nTick = -1);
private:
	SOCKET  m_sDriver;

	win_iocp m_Iocp;

	BOOL m_bClose;

	BOOL m_bInit;

	HANDLE m_hDealIoThread;

	HANDLE m_hMtAcceptThread;

	int m_nPostAcceptIoStat;

	MemoryPool* m_pMemoryPool;

	IoManager* m_pIoManager;

	LPFN_TRANSMITFILE			TransmitFile;
	LPFN_ACCEPTEX				AcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	GetAcceptExSockaddrs;
	LPFN_TRANSMITPACKETS		TransmitPackets;
	LPFN_CONNECTEX				ConnectEx;
	LPFN_DISCONNECTEX			DisconnectEx;
	LPFN_WSARECVMSG				WSARecvMsg;
};




#endif