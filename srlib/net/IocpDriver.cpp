/************************************************************************
* FileName  : NA
* Author    : xihu
* Info      : 2014-02
************************************************************************/
#include "stdafx.h"
#include "IocpDriver.h"
#include "IoManager.h"
#include "SysLog.h"

IocpDriver::IocpDriver() : m_bClose(FALSE),m_bInit(FALSE),m_hDealIoThread(INVALID_HANDLE_VALUE),m_hMtAcceptThread(INVALID_HANDLE_VALUE)
{
	m_nPostAcceptIoStat = 0;
}
IocpDriver::~IocpDriver()
{

}
int IocpDriver::InitDriver()
{
	int nRet = -1;

	if (m_bInit)
	{
		return nRet;
	}
	m_bInit = TRUE;

	m_pMemoryPool = CreateInstance<MemoryPool>();

	assert(NULL != m_pMemoryPool);

	m_pIoManager = CreateInstance<IoManager>();

	assert(NULL != m_pIoManager);

	m_Iocp.CreateIocp();

	m_sDriver = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	static GUID guidTransmitFile		= WSAID_TRANSMITFILE;
	static GUID guidAcceptEx			= WSAID_ACCEPTEX;
	static GUID guidGetAcceptExSockaddrs= WSAID_GETACCEPTEXSOCKADDRS;
	static GUID guidTransmitPackets		= WSAID_TRANSMITPACKETS;
	static GUID guidConnectEx			= WSAID_CONNECTEX;
	static GUID guidDisconnectEx		= WSAID_DISCONNECTEX;
	static GUID guidWSARecvMsg			= WSAID_WSARECVMSG;

	DWORD dwBytes = 0;

	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidTransmitFile,sizeof(guidTransmitFile), &TransmitFile,sizeof(TransmitFile),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidAcceptEx,sizeof(guidAcceptEx), &AcceptEx,sizeof(AcceptEx),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidGetAcceptExSockaddrs,sizeof(guidGetAcceptExSockaddrs), &GetAcceptExSockaddrs,sizeof(GetAcceptExSockaddrs),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidTransmitPackets,sizeof(guidTransmitPackets), &TransmitPackets,sizeof(TransmitPackets),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidConnectEx,sizeof(guidConnectEx), &ConnectEx,sizeof(ConnectEx),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidDisconnectEx,sizeof(guidDisconnectEx), &DisconnectEx,sizeof(DisconnectEx),&dwBytes,NULL,NULL);
	WSAIoctl(m_sDriver,SIO_GET_EXTENSION_FUNCTION_POINTER,&guidWSARecvMsg,sizeof(guidWSARecvMsg), &WSARecvMsg,sizeof(WSARecvMsg),&dwBytes,NULL,NULL);

	unsigned int nWorkId = 0;

	if (INVALID_HANDLE_VALUE == m_hDealIoThread)
	{
		for (int i = 0;i < 1;i++)
		{
			m_hDealIoThread = (HANDLE)_beginthreadex(NULL,0,DealIoImplThread,this,CREATE_SUSPENDED,(unsigned*)&nWorkId);

			if (INVALID_HANDLE_VALUE != m_hDealIoThread)
			{
				SetThreadPriority(m_hDealIoThread,THREAD_PRIORITY_ABOVE_NORMAL);

				ResumeThread(m_hDealIoThread);
			}
		}
	}
	return nRet;
}

int IocpDriver::Listen(SOCKET& sListen,int nPort,char* pIp/* = NULL*/)
{
	int nRet = -1;

	if (nPort <= 0 || sListen != INVALID_SOCKET)
	{
		return nRet;
	}
	sListen = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	struct sockaddr_in addr_in;

	memset (&addr_in,0,sizeof(struct sockaddr_in));

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(nPort);

	if (NULL == pIp)
	{
		addr_in.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		addr_in.sin_addr.s_addr = inet_addr(pIp);
	}
	nRet = ::bind(sListen,(struct sockaddr*)&addr_in,sizeof(struct sockaddr));

	if (listen(sListen,5))
	{
		TRACE("listen error : %d\n",GetLastError());

		return nRet;
	}
	if (!AssociateDevice((HANDLE)sListen,NULL))
	{
		TRACE("AssociateDevice error\n");
	}
	if (INVALID_HANDLE_VALUE == m_hMtAcceptThread)
	{
		PAcceptParam pAcceptParam = (PAcceptParam)m_pMemoryPool->Malloc(sizeof(AcceptParam));

		pAcceptParam->pIocpDriver = this;

		pAcceptParam->sListen = sListen;

		unsigned int nWorkId = 0;

		m_hMtAcceptThread = (HANDLE)_beginthreadex(NULL,0,MtAcceptIoThread,pAcceptParam,CREATE_SUSPENDED,(unsigned*)&nWorkId);

		if (INVALID_HANDLE_VALUE != m_hMtAcceptThread)
		{
			SetThreadPriority(m_hMtAcceptThread,THREAD_PRIORITY_TIME_CRITICAL);

			ResumeThread(m_hMtAcceptThread);
		}
	}
	return nRet = 0;
}

int IocpDriver::CloseConn(SOCKET sServer,SOCKET sClient)
{
	int nRet = -1;

	nRet = closesocket(sClient);

	//nRet = shutdown(sClient,SD_BOTH);

	//nRet = CancelIo((HANDLE)sClient);

	//nRet = DisconnectEx(sClient,NULL,TF_REUSE_SOCKET,0);

	//nRet = GetLastError();

	return nRet;
}
int IocpDriver::SendMessage(SOCKET sSocket,char* pBuf,int nLen)
{
	int nRet = -1;

	return nRet;
}
int IocpDriver::Connect(SOCKET& sConn,int nPort,char* pIp)
{
	int nRet = -1;

	sConn = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

	struct sockaddr_in addr_in;

	memset (&addr_in, 0, sizeof(struct sockaddr_in));

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(nPort);
	addr_in.sin_addr.s_addr = inet_addr(pIp);

	nRet = connect(sConn,(struct sockaddr *)&addr_in,sizeof(struct sockaddr));

	if (nRet == 0)
	{
		AssociateDevice((HANDLE)sConn,NULL);

		AsyncResult* pAsyncResult = (AsyncResult*)m_pMemoryPool->Malloc(sizeof(AsyncResult));

		new(pAsyncResult) AsyncResult();				

		pAsyncResult->m_sListen = INVALID_SOCKET;

		pAsyncResult->m_sDealSocket = sConn;

		pAsyncResult->m_pCallBackFunc = &IocpDriver::OnPreRecv;

		WSABUF wsabuf = {0};

		DWORD dwFlag = 0;

		if (0 != WSARecv(pAsyncResult->m_sDealSocket,&wsabuf,1,NULL,&dwFlag,pAsyncResult,NULL) && WSAGetLastError() != WSA_IO_PENDING)
		{
			TRACE("WSARecv Error : %d\n",WSAGetLastError());

			if (WSAGetLastError() != 997)
			{
				if (pAsyncResult->m_pBuf != NULL)
				{
					m_pMemoryPool->Free(pAsyncResult->m_pBuf);

					pAsyncResult->m_pBuf = NULL;
				}
				//pMemoryPool->Free(pAsyncResult);
				DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

				pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

				BOOL bRet = DisconnectEx(pAsyncResult->m_sDealSocket,pAsyncResult,dwFlags,0);

				if (!bRet)
				{
					m_pMemoryPool->Free(pAsyncResult);

					pAsyncResult = NULL;
				}
			}
		}
	}
	return nRet;
}

/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
BOOL IocpDriver::AssociateDevice(HANDLE hHandle,void* pIoClient)
{
	return (BOOL)m_Iocp.AssociateDevice(hHandle,(ULONG_PTR)pIoClient);
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
// int IocpDriver::StopWork()
// {
// 	__MT_FUNC__
// 
// 	int nRet = -1;
// 
// 	if (!m_bStart)
// 	{
// 		return nRet;
// 	}
// 	m_bClose = TRUE;
// 
// 	if (INVALID_HANDLE_VALUE != m_hDealIoThread)
// 	{
// 		CloseHandle(m_hDealIoThread);
// 
// 		m_hDealIoThread = INVALID_HANDLE_VALUE;
// 	}
// 	m_bStart = TRUE;
// 
// 	nRet = 0;
// 
// 	return nRet;
// }
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
unsigned int __stdcall IocpDriver::MtAcceptIoThread(void* pParam)
{
	__MT_FUNC__

	int nRet = -1;

	PAcceptParam pAcceptParam = static_cast<PAcceptParam>(pParam);

	if (NULL == pAcceptParam)
	{
		return nRet;
	}
	nRet = pAcceptParam->pIocpDriver->MtAcceptIo(pAcceptParam->sListen);

	if (NULL != pAcceptParam)
	{
		pAcceptParam->pIocpDriver->m_pMemoryPool->Free(pAcceptParam);
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::MtAcceptIo(SOCKET sListen)
{
	__MT_FUNC__

	HANDLE hAcceptEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);

	WSAEventSelect(sListen,hAcceptEvent,FD_ACCEPT);

	while(!m_bClose)
	{	
		DWORD dwError = ::WaitForSingleObjectEx(hAcceptEvent,INFINITE,TRUE);

		if(dwError == WAIT_FAILED || dwError == WAIT_IO_COMPLETION)
		{
			break;
		}
		else if( dwError == WAIT_OBJECT_0 )
		{
			for(int i = 0;i != MAX_ACCEPT;++i)
			{
				AsyncResult* pAsyncResult = (AsyncResult*)m_pMemoryPool->Malloc(sizeof(AsyncResult));

				new(pAsyncResult) AsyncResult();				

				pAsyncResult->m_sListen = sListen;

				pAsyncResult->m_sDealSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);

				assert(pAsyncResult->m_sDealSocket != INVALID_SOCKET);

				PostAcceptIo(pAsyncResult);
			}
		}
	}
	return 0;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
unsigned int __stdcall IocpDriver::DealIoImplThread(void* pParam)
{
	__MT_FUNC__

	int nRet = -1;

	IocpDriver* pIocpDriver = (IocpDriver*)pParam;

	if (pIocpDriver == NULL)
	{
		return nRet;
	}
	nRet = pIocpDriver->DealIoImpl();

	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::DealIoImpl()
{
	__MT_FUNC__

	ULONG_PTR pCompKey = NULL;

	DWORD dwNumBytes = 0;

	OVERLAPPED *pOverlapped = NULL;

	while (!m_bClose)
	{
		if (m_Iocp.GetStatus(&pCompKey,&dwNumBytes,&pOverlapped,INFINITE))
		{
			int nTick = GetTickCount();

			if (pOverlapped == NULL && NULL != pCompKey)
			{
				break;
			}
			AsyncResult* pAsyncResult = static_cast<AsyncResult*>(pOverlapped);

			if (pAsyncResult->m_pCallBackFunc != NULL)
			{
				(this->*(pAsyncResult->m_pCallBackFunc))(pAsyncResult,dwNumBytes);
			}
			else
			{
				int err = GetLastError();

				TRACE("GetStatus : GetLastError = %d\n",err);

				int nRet = shutdown(pAsyncResult->m_sDealSocket,SD_BOTH);

				err = GetLastError();

				if (err == 10038)
				{
					PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_DISCONN,NULL,0);
				}
				else
				{
					TRACE("GetStatus-shutdown : GetLastError = %d\n",err);
				}
				nRet = CancelIo((HANDLE)pAsyncResult->m_sDealSocket);

				PostAcceptIo(pAsyncResult);
			}

			//TRACE("m_Iocp.GetStatus UseTime : %d\n",GetTickCount() - nTick);
		}
		else
		{
			int nErr = GetLastError();

			if (NULL != pOverlapped)
			{
				AsyncResult* pAsyncResult = static_cast<AsyncResult*>(pOverlapped);

				BOOL bRet;

				if (nErr == 64)
				{
					if (pAsyncResult->m_pBuf != NULL)
					{
						m_pMemoryPool->Free(pAsyncResult->m_pBuf);

						pAsyncResult->m_pBuf = NULL;
					}
					//		pMemoryPool->Free(pAsyncResult);
					DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

					//pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

					bRet = DisconnectEx(pAsyncResult->m_sDealSocket,NULL,dwFlags,0);

					if (bRet)
					{
						PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_DISCONN,NULL,0);

						PostAcceptIo(pAsyncResult);
					}
					else
					{
						TRACE("bRet = DisconnectEx == TRUE\n");

						m_pMemoryPool->Free(pAsyncResult);

						pAsyncResult = NULL;
					}
				}
				else
				{
					if (pAsyncResult->m_pCallBackFunc != NULL)
					{
						(this->*(pAsyncResult->m_pCallBackFunc))(pAsyncResult,0);
					}
					else
					{
						//未设置回调函数
					}
				}
			}
			else
			{
				assert(0);
			}
		}
	}
	return 0;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::OnAccept(AsyncResult* pAsyncResult,int nNumBytes)
{
	__MT_FUNC__

	int nRet = -1;

	assert(pAsyncResult != NULL);

	if (NULL == pAsyncResult)
	{
		return nRet;
	}
	m_nPostAcceptIoStat--;

	// 设置超时
	struct TCPKeepAlive
	{
		u_long onoff;
		u_long keepalivetime;
		u_long keepaliveinterval;				
	};
	TCPKeepAlive inKeepAlive = {0};

	DWORD dwInLen = sizeof(TCPKeepAlive);

	TCPKeepAlive outKeepAlive = {0};

	DWORD dwOutLen = sizeof(TCPKeepAlive);

	DWORD dwBytesReturn = 0;

	inKeepAlive.onoff = 1;

	inKeepAlive.keepalivetime = 30000;

	inKeepAlive.keepaliveinterval = 2000;	

	WSAIoctl(pAsyncResult->m_sDealSocket,SIO_KEEPALIVE_VALS,&inKeepAlive,dwInLen,&outKeepAlive,dwOutLen,&dwBytesReturn,NULL,NULL);

	int nSockBufLen = 1024*32*4;

	if (setsockopt(pAsyncResult->m_sDealSocket,SOL_SOCKET,SO_RCVBUF,(char*)&nSockBufLen,sizeof(nSockBufLen)))
	{
		int nErr = GetLastError();

		assert(0);
	}
	SOCKADDR_IN *pLocalAddr = NULL, *pRemoteAddr = NULL;

	int szLocalLen = 0, szRemoteLen = 0;

	GetAcceptExSockaddrs(pAsyncResult->m_pBuf,0,sizeof(SOCKADDR_IN) + 16,sizeof(SOCKADDR_IN) + 16,reinterpret_cast<SOCKADDR **>(&pLocalAddr),&szLocalLen,reinterpret_cast<SOCKADDR **>(&pRemoteAddr),&szRemoteLen);

	AssociateDevice((HANDLE)pAsyncResult->m_sDealSocket);

	//TRACE("接受连接：Client = %p,socket = %d\n",pIoClient,pIoClient->GetSocket());

	PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_ACCEPT,(char*)pRemoteAddr,sizeof(SOCKADDR_IN));

	m_pMemoryPool->Free(pAsyncResult->m_pBuf);

	pAsyncResult->m_pBuf = NULL;

	pAsyncResult->m_pCallBackFunc = &IocpDriver::OnPreRecv;

	WSABUF wsabuf = {0};

	DWORD dwFlag = 0;

	if (0 != WSARecv(pAsyncResult->m_sDealSocket,&wsabuf,1,NULL,&dwFlag,pAsyncResult,NULL) && WSAGetLastError() != WSA_IO_PENDING)
	{
		TRACE("WSARecv Error : %d\n",WSAGetLastError());

		if (WSAGetLastError() != 997)
		{
			if (pAsyncResult->m_pBuf != NULL)
			{
				m_pMemoryPool->Free(pAsyncResult->m_pBuf);

				pAsyncResult->m_pBuf = NULL;
			}
			//pMemoryPool->Free(pAsyncResult);
			DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

			pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

			BOOL bRet = DisconnectEx(pAsyncResult->m_sDealSocket,pAsyncResult,dwFlags,0);

			if (!bRet)
			{
				m_pMemoryPool->Free(pAsyncResult);
			
				pAsyncResult = NULL;
			}
		}
		return nRet;
	}
	return nRet = 0;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::OnPreRecv(AsyncResult* pAsyncResult,int nNumBytes)
{
	__MT_FUNC__

	int nRet = -1;
	
	assert(nNumBytes == 0);

	if (nNumBytes != 0)
	{
		TRACE("nNumBytes != 0\n");

		return nRet; 
	}
	SOCKET sClient = pAsyncResult->m_sDealSocket;

	if (pAsyncResult->m_pBuf != NULL)
	{
		m_pMemoryPool->Free(pAsyncResult->m_pBuf);

		pAsyncResult->m_pBuf = NULL;
	}
	pAsyncResult->m_pBuf = (char*)m_pMemoryPool->Malloc(PACKET_MAX);

	WSABUF wsabuf = {0};

	wsabuf.buf = pAsyncResult->m_pBuf;

	wsabuf.len = PACKET_MAX;

	pAsyncResult->m_pCallBackFunc = &IocpDriver::OnRecv;
	
	DWORD dwFlag = 0;

	if (0 != WSARecv(pAsyncResult->m_sDealSocket,&wsabuf,1,NULL,&dwFlag,pAsyncResult,NULL) && WSAGetLastError() != WSA_IO_PENDING)
	{
		TRACE("WSARecv Error : %d\n",WSAGetLastError());

		if (WSAGetLastError() != 997)
		{
			if (pAsyncResult->m_pBuf != NULL)
			{
				m_pMemoryPool->Free(pAsyncResult->m_pBuf);

				pAsyncResult->m_pBuf = NULL;
			}
			//pMemoryPool->Free(pAsyncResult);
			DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

			pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

			BOOL bRet = DisconnectEx(pAsyncResult->m_sDealSocket,pAsyncResult,dwFlags,0);

			if (!bRet)
			{
				m_pMemoryPool->Free(pAsyncResult);

				pAsyncResult = NULL;
			}
		}
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::OnRecv(AsyncResult* pAsyncResult,int nNumBytes)
{
	__MT_FUNC__

	int nRet = -1;

	unsigned int uCurTick = 0;

	if (nNumBytes == 0)		//断线
	{
		//TRACE("关闭连接：Client = %p,socket = %d\n",pIoClient,pIoClient->GetSocket());
		if (pAsyncResult->m_pBuf != NULL)
		{
			m_pMemoryPool->Free(pAsyncResult->m_pBuf);

			pAsyncResult->m_pBuf = NULL;
		}
//		pMemoryPool->Free(pAsyncResult);
		DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

		pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

		BOOL bRet = DisconnectEx(pAsyncResult->m_sDealSocket,pAsyncResult,dwFlags,0);

		if (!bRet)
		{
			PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_DISCONN,NULL,0);

			PostAcceptIo(pAsyncResult);
		}
	}
	else if (nNumBytes > 0)
	{
		nNumBytes += pAsyncResult->m_nCurFlag;

		char* pRecvBuf = pAsyncResult->m_pBuf;

		while (TRUE)
		{
			if (nNumBytes < 2)
			{
				break;
			}
			WORD wLen = *(WORD*)pRecvBuf;

			assert(wLen > 0 && wLen <= PACKET_MAX);

			if (wLen > nNumBytes)
			{
				break;
			}
			int nTick = GetTickCount();

			PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_RECV,pRecvBuf,wLen,nTick);

			pRecvBuf += wLen;

			nNumBytes -= wLen;
		}
		pAsyncResult->m_nCurFlag = nNumBytes;

		memmove(pAsyncResult->m_pBuf,pRecvBuf,nNumBytes);

		WSABUF wsabuf = {0};

		wsabuf.buf = pAsyncResult->m_pBuf + nNumBytes;

		wsabuf.len = PACKET_MAX - nNumBytes;

		DWORD dwFlag = 0;

		if (0 != WSARecv(pAsyncResult->m_sDealSocket,&wsabuf,1,NULL,&dwFlag,pAsyncResult,NULL) && WSAGetLastError() != WSA_IO_PENDING)
		{
			TRACE("WSARecv Error : %d\n",WSAGetLastError());

			if (WSAGetLastError() != 997)
			{
				if (pAsyncResult->m_pBuf != NULL)
				{
					m_pMemoryPool->Free(pAsyncResult->m_pBuf);

					pAsyncResult->m_pBuf = NULL;
				}
				//pMemoryPool->Free(pAsyncResult);
				DWORD dwFlags = 1 ? TF_REUSE_SOCKET : 0;

				pAsyncResult->m_pCallBackFunc = &IocpDriver::OnDisconn;

				BOOL bRet = DisconnectEx(pAsyncResult->m_sDealSocket,pAsyncResult,dwFlags,0);

				if (!bRet)
				{
					m_pMemoryPool->Free(pAsyncResult);

					pAsyncResult = NULL;
				}
			}
			return nRet;
		}
	}
	else
	{
		assert(0);
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::OnSend(AsyncResult* pAsyncResult,int nNumBytes)
{
	__MT_FUNC__

	int nRet = -1;

	PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_SEND,pAsyncResult->m_pBuf,nNumBytes);

	if (pAsyncResult->m_pBuf != NULL)
	{
		m_pMemoryPool->Free(pAsyncResult->m_pBuf);

		pAsyncResult->m_pBuf = NULL;
	}
	m_pMemoryPool->Free(pAsyncResult);

	pAsyncResult = NULL;

	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::OnDisconn(AsyncResult* pAsyncResult,int nNumBytes)
{
	__MT_FUNC__

	int nRet = -1;

	if (NULL != pAsyncResult)
	{
		nRet = shutdown(pAsyncResult->m_sDealSocket,SD_BOTH);

		int err = GetLastError();

		//TRACE("shutdown err : %d\n",err);

		nRet = CancelIo((HANDLE)pAsyncResult->m_sDealSocket);

		PostIoMsg(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,IO_ON_DISCONN,NULL,0);

		PostAcceptIo(pAsyncResult);
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::PostAcceptIo(AsyncResult* pAsyncResult)
{
	__MT_FUNC__

	int nRet = -1;

	assert(pAsyncResult != NULL);

	if (NULL == pAsyncResult)
	{
		return nRet;
	}
	if (pAsyncResult->m_pBuf != NULL)
	{
		m_pMemoryPool->Free(pAsyncResult->m_pBuf);

		pAsyncResult->m_pBuf = NULL;
	}
	//最多acceptio为20个
	if (m_nPostAcceptIoStat >= 20 && NULL != pAsyncResult)
	{
		closesocket(pAsyncResult->m_sDealSocket);

		m_pMemoryPool->Free(pAsyncResult);

		pAsyncResult = NULL;

		return nRet;
	}
	pAsyncResult->m_pBuf = (char*)m_pMemoryPool->Malloc((sizeof(sockaddr_in) + 16)*2);

	pAsyncResult->m_pCallBackFunc = &IocpDriver::OnAccept;

	if (!AcceptEx(pAsyncResult->m_sListen,pAsyncResult->m_sDealSocket,pAsyncResult->m_pBuf,0,sizeof(SOCKADDR_IN) + 16,sizeof(SOCKADDR_IN) + 16,0,pAsyncResult))
	{
		int nError = GetLastError();

		if (nError != ERROR_IO_PENDING)
		{
			TRACE("AcceptEx error : %d!\n",nError);

			if (pAsyncResult != NULL)
			{
				if (pAsyncResult->m_pBuf != NULL)
				{
					m_pMemoryPool->Free(pAsyncResult->m_pBuf);

					pAsyncResult->m_pBuf = NULL;
				}
				m_pMemoryPool->Free(pAsyncResult);

				pAsyncResult = NULL;
			}
			return nRet;
		}
	}
	m_nPostAcceptIoStat++;

	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::PostSendIo(SOCKET sServer,SOCKET sClient,char* pszBuf,int nLen)
{
	__MT_FUNC__

	int nRet = -1;

	AsyncResult* pAsyncResult = (AsyncResult*)m_pMemoryPool->Malloc(sizeof(AsyncResult));

	new(pAsyncResult) AsyncResult();				

	pAsyncResult->m_sListen = sServer;

	pAsyncResult->m_sDealSocket = sClient;

	pAsyncResult->m_pCallBackFunc = &IocpDriver::OnSend;

	pAsyncResult->m_pBuf = (char*)m_pMemoryPool->Malloc(nLen);

	memcpy(pAsyncResult->m_pBuf,pszBuf,nLen);

	WSABUF wsabuf = {0,0};

	wsabuf.len = nLen;

	wsabuf.buf = pAsyncResult->m_pBuf;

	DWORD dwSize = 0;

	nRet = WSASend(sClient,&wsabuf,1,&dwSize,0,pAsyncResult,NULL);

	if (nRet == -1)
	{
		int nError = GetLastError();

		if (nError != 997)
		{
			TRACE("WSASend error : %d\n",nError);

			if (pAsyncResult->m_pBuf != NULL)
			{
				m_pMemoryPool->Free(pAsyncResult->m_pBuf);

				pAsyncResult->m_pBuf = NULL;
			}
			m_pMemoryPool->Free(pAsyncResult);

			pAsyncResult = NULL;
		}
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/21
************************************************************************/
int IocpDriver::PostIoMsg(SOCKET sServer,SOCKET sClient,int nIoType,char* pszBuf,int nNumBytes,int nTick/* = -1*/)
{
	__MT_FUNC__

	int nRet = -1;

	nRet = m_pIoManager->PostIoMsg(sServer,sClient,nIoType,0,pszBuf,nNumBytes,nTick);
	
	return nRet = 0;
}