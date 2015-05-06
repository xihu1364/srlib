/************************************************************************
* FileName : IoManager.cpp 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#include "stdafx.h"
#include <WINSOCK2.H>
#pragma comment(lib, "ws2_32.lib")
#include <process.h>
#include "IoManager.h"
#include "ClassFactory.h"


IoManager::IoManager() : m_hDealIoMsgThread(INVALID_HANDLE_VALUE),m_bInit(FALSE),m_bClose(FALSE),m_pMemoryPool(NULL),m_lCount(0)
{
	m_uKeepAliveTime = 5*1000;
}

IoManager::~IoManager()
{

}

int IoManager::InitIoManager()
{
	int nRet = -1;

	if (m_bInit)
	{
		return nRet;
	}
	m_bInit = TRUE;

#pragma region 初始化socket库

	WSAData data;

	int error = WSAStartup(MAKEWORD(2, 2), &data);

	if (0 != error)
	{
		return nRet;
	}
	if(HIBYTE(data.wVersion) != 2 && LOBYTE(data.wVersion))    
	{         
		WSACleanup();     

		return nRet;     
	}
	m_IocpDirver.InitDriver();

#pragma endregion
	if (m_pMemoryPool == NULL)
	{
		m_pMemoryPool = CreateInstance<MemoryPool>();

		assert(m_pMemoryPool != NULL);
	}
#pragma region 处理IoMsg线程

	if (INVALID_HANDLE_VALUE == m_hDealIoMsgThread)
	{
		unsigned int nWorkId = 0;

		m_hDealIoMsgThread = (HANDLE)_beginthreadex(NULL,0,DealIoMsgQueueThread,this,CREATE_SUSPENDED,(unsigned*)&nWorkId);

		if (INVALID_HANDLE_VALUE != m_hDealIoMsgThread)
		{
			SetThreadPriority(m_hDealIoMsgThread,THREAD_PRIORITY_TIME_CRITICAL);

			ResumeThread(m_hDealIoMsgThread);
		}
	}
#pragma endregion

	return nRet;
}
int IoManager::UnInitIoManager()
{
	if (m_hDealIoMsgThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDealIoMsgThread);

		m_bClose = TRUE;
	}
	return 0;
}
int IoManager::CloseConn(SOCKET sServer,SOCKET sClient)
{
	int nRet = -1;

	if (sServer == INVALID_SOCKET)
	{
		DelClient(sClient);
	}
	nRet = m_IocpDirver.CloseConn(sServer,sClient);

	return nRet;
}

unsigned int __stdcall IoManager::DealIoMsgQueueThread(void* pParam)
{
	int nRet = -1;

	IoManager* pIoManager = (IoManager*)pParam;

	if (NULL == pIoManager)
	{
		return nRet;
	}
	pIoManager->DealIoMsgQueue();

	return nRet = 0;
}

void IoManager::DealIoMsgQueue()
{
	PIoMsg pIoMsg = NULL;

	while (!m_bClose)
	{
		do 
		{
			if (pIoMsg != NULL)
			{
				if (NULL != pIoMsg->_pszBuf)
				{
					m_pMemoryPool->Free(pIoMsg->_pszBuf);
				}
				m_pMemoryPool->Free(pIoMsg);

				pIoMsg = NULL;
			}
			pIoMsg = m_MsgQueue.PopFront();

			ProcessIoMsg(pIoMsg);

		} while (pIoMsg != NULL);

		ClientList::iterator ClientIt;

		for (ClientIt = m_ClientList.begin();ClientIt != m_ClientList.end();)
		{
			unsigned int nCurTime = GetTickCount();
			unsigned int nTime = ClientIt->second->GetLastMsgTime();
			unsigned int nTT = nCurTime - nTime;

			if (GetTickCount() - ClientIt->second->GetLastMsgTime() >= m_uKeepAliveTime)
			{
				ClientIt->second->SendPackage(MSG_KEEP_ALIVE,NULL,0);

				continue;
			}
			++ClientIt;
		}
		Sleep(1);
	}
}
void IoManager::ProcessIoMsg(IoMsg* pIoMsg)
{
	if (NULL != pIoMsg)
	{
		switch (pIoMsg->_nIoType)
		{
		case IO_ON_CREATE:
			{
				break;
			}
		case IO_ON_ACCEPT:
			{

				break;
			}
		case IO_ON_CONNECT:
			{
				break;
			}
		case IO_ON_DISCONN:
			{
				shared_ptr<ClientItem> spClient = FindClient(pIoMsg->_sClient);

				if (spClient)
				{
					spClient->OnDisconn();
				}
				DelClient(pIoMsg->_sClient);

				break;
			}
		case IO_ON_RECV:
			{
				InterlockedIncrement(&m_lCount);

				shared_ptr<ClientItem> spClient = FindClient(pIoMsg->_sClient);

				if (spClient)
				{
					//spClient->RecordMsgTime();

					spClient->GroupPackage(pIoMsg);
				}
				break;
			}
		case IO_SEND:
			{
				break;
			}
		case IO_ON_SEND:
			{
				break;
			}
		case IO_ERROR:
			{
				break;
			}
		default:
			{
				break;
			}
		}
	}
}
int IoManager::SendMessage(SOCKET sServer,SOCKET sClient,char* pBuf,int nLen)
{
	int nRet = -1;

	nRet = m_IocpDirver.SendMessage(sServer,sClient,pBuf,nLen);

	return nRet;
}

int IoManager::PostIoMsg(SOCKET sListen,SOCKET sClient,int nIoType,unsigned int nResult,char* pszBuf,int nNumBytes,int nTick/* = -1*/)
{
	int nRet = -1;

	shared_ptr<ServerItem> spServer = FindServer(sListen);

	IoMsg* pIoMsg = m_pMemoryPool->New<IoMsg>();

	assert(pIoMsg != NULL);

	pIoMsg->_sServer = sListen;

	pIoMsg->_sClient = sClient;

	pIoMsg->_nIoType = nIoType;

	pIoMsg->_nTick = nTick;

	pIoMsg->_nResult = nResult;

	if (nNumBytes > 0)
	{
		pIoMsg->_pszBuf = (char*)m_pMemoryPool->Malloc(nNumBytes);

		assert(pIoMsg->_pszBuf != NULL);

		memcpy(pIoMsg->_pszBuf,pszBuf,nNumBytes);
	}
	pIoMsg->_nLen = nNumBytes;

	if (spServer.get() != NULL)
	{
		spServer->PostIoMsg(pIoMsg);
	}
	else
	{
		m_MsgQueue.PushBack(pIoMsg);
	}
	return nRet;
}
int IoManager::AddServer(SOCKET sServer,shared_ptr<ServerItem> spServer)
{
	int nRet = -1;

	if (INVALID_SOCKET == sServer)
	{
		return nRet;
	}
	CSRongSyncObj LockObj(&m_ServerListLock);

	std::pair< ServerList::iterator,bool > ret;

	ret = m_ServerList.insert(ServerList::value_type(sServer,spServer));

	if (ret.second)
	{
		nRet = 0;
	}
	return nRet;
}
shared_ptr<ServerItem> IoManager::FindServer(SOCKET sServer)
{
	shared_ptr<ServerItem> spServer;

	CSRongSyncObj LockObj(&m_ServerListLock);

	ServerList::iterator it;

	it = m_ServerList.find(sServer);

	if (it != m_ServerList.end())
	{
		spServer = it->second;
	}
	return spServer;
}
int IoManager::DelServer(SOCKET sServer)
{
	int nRet = -1;

	CSRongSyncObj LockObj(&m_ServerListLock);

	ServerList::iterator it;

	it = m_ServerList.find(sServer);

	if (it != m_ServerList.end())
	{
		m_ServerList.erase(it);

		nRet = 0;
	}
	return nRet;
}
int IoManager::AddClient(SOCKET sClient,shared_ptr<ClientItem> spClient)
{
	int nRet = -1;

	if (INVALID_SOCKET == sClient)
	{
		return nRet;
	}
	//CSRongSyncObj LockObj(&m_ClientListLock);

	std::pair< ClientList::iterator,bool > ret;

	ret = m_ClientList.insert(ClientList::value_type(sClient,spClient));

	if (ret.second)
	{
		nRet = 0;
	}
	return nRet;
}
shared_ptr<ClientItem> IoManager::FindClient(SOCKET sClient)
{
	shared_ptr<ClientItem> spClient;

	//CSRongSyncObj LockObj(&m_ClientListLock);

	ClientList::iterator it;

	it = m_ClientList.find(sClient);

	if (it != m_ClientList.end())
	{
		spClient = it->second;
	}
	return spClient;
}
int IoManager::DelClient(SOCKET sClient)
{
	int nRet = -1;

	//CSRongSyncObj LockObj(&m_ClientListLock);

	ClientList::iterator it;

	it = m_ClientList.find(sClient);

	if (it != m_ClientList.end())
	{
		m_ClientList.erase(it);

		nRet = 0;
	}
	return nRet;
}
void IoManager::DumpNode()
{
	printf("========================DumpNode========================\n");
	printf("ServerNum : %d    ",m_ServerList.size());
	printf("PostAcceptIoNum : %d   ",m_IocpDirver.GetPostIoCount());
	printf("ClientNum : %d\n",m_ClientList.size());
	printf("--------------------------------------------------------\n");
	ServerList::iterator itServer;

	for (itServer = m_ServerList.begin();itServer != m_ServerList.end();++itServer)
	{
		printf("Server[%d] Port : %d Num : %d Msg : %d TP : %d \n",itServer->second->GetServerSocket(),itServer->second->GetLocalPort(),itServer->second->GetClientNum(),itServer->second->GetMsgQueueSize(),itServer->second->GetThroughPut());
	}
	printf("========================DumpNode========================\n");
}