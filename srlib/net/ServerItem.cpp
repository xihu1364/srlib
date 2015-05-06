/************************************************************************
* FileName : ServerItem.cpp 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#include "stdafx.h"
#include "ServerItem.h"
#include "NetDefine.h"
#include <process.h>
#include "ClassFactory.h"
#include "MemoryPool.h"
#include "IoManager.h"
#include <functional>

ServerItem::ServerItem() : m_bClose(FALSE),m_hDealIoMsgThread(INVALID_HANDLE_VALUE),m_lCount(0)
{

}
ServerItem::~ServerItem()
{

}
int ServerItem::StartServer()
{
	int nRet = -1;

	m_uClientTimeOut = 200*1000;	//连接超时时间200秒

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
void ServerItem::OnAccept(ClientItem*& pClientItem)
{
	pClientItem = (ClientItem*)m_pMemoryPool->Malloc(sizeof(ClientItem));

	new(pClientItem) ClientItem(this);

	//pClientItem = new ClientItem();
}
int ServerItem::SendMessage(SOCKET sClient,char* pBuf,int nLen)
{
	int nRet = -1;

	IoManager* pIoManager = CreateInstance<IoManager>();

	assert(NULL != pIoManager);

	nRet = pIoManager->SendMessage(GetServerSocket(),sClient,pBuf,nLen);

	return nRet;
}

unsigned int __stdcall ServerItem::DealIoMsgQueueThread(void* pParam)
{
	int nRet = -1;

	ServerItem* pServerItem = (ServerItem*)pParam;

	if (NULL == pServerItem)
	{
		return nRet;
	}
	pServerItem->DealIoMsgQueue();

	return nRet = 0;
}


void ServerItem::DealIoMsgQueue()
{
	PIoMsg pIoMsg = NULL;

	while (!m_bClose)
	{
//  	pIoMsg = m_MsgQueue.PopFront();
// 
// 		ProcessIoMsg(pIoMsg);
//  
//  		if (NULL != pIoMsg)
//  		{
//  			if (NULL != pIoMsg->_pszBuf)
//  			{
//  				m_pMemoryPool->Free(pIoMsg->_pszBuf);
//  			}
//  			m_pMemoryPool->Free(pIoMsg);
//  		}

		do 
		{
			if (pIoMsg != NULL)
			{
				if (NULL != pIoMsg->_pszBuf)
				{
					m_pMemoryPool->Free(pIoMsg->_pszBuf);
				}
				m_pMemoryPool->Free(pIoMsg);
			}
			pIoMsg = m_MsgQueue.PopFront();

			ProcessIoMsg(pIoMsg);

		} while (pIoMsg != NULL);

		unsigned int uTimeOut = 0;

		ClientList::iterator ClientIt;

		for (ClientIt = m_ClientList.begin();ClientIt != m_ClientList.end();)
		{
			if (ClientIt->second->GetVerifyFlag())
			{
				uTimeOut = m_uClientTimeOut;
			}
			else
			{
				uTimeOut = m_uClientTimeOut/10;
			}
			if (GetTickCount() - ClientIt->second->GetLastMsgTime() > uTimeOut)
			{
				CloseConn(ClientIt->second->GetClientSocket());

				m_ClientList.erase(ClientIt++);

				continue;
			}
			++ClientIt;
		}
		Sleep(1);
	}
}
void ServerItem::ProcessIoMsg(IoMsg* pIoMsg)
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
				ClientItem* pClientItem = NULL;

				OnAccept(pClientItem);

				assert(NULL != pClientItem);

				shared_ptr<ClientItem> spClient(pClientItem,tr1::bind(&MemoryPool::Free,m_pMemoryPool,tr1::placeholders::_1));

				SOCKADDR_IN* pAddress = (SOCKADDR_IN*)pIoMsg->_pszBuf;

				spClient->SetClientSocket(pIoMsg->_sClient);

				spClient->SetServerSocket(pIoMsg->_sServer);

				spClient->SetRemoteIp(inet_ntoa(pAddress->sin_addr));

				spClient->SetRemotePort(ntohs(pAddress->sin_port));

				spClient->RecordMsgTime();

				AddClient(pIoMsg->_sClient,spClient);

				break;
			}
		case IO_ON_CONNECT:
			{
				break;
			}
		case IO_ON_DISCONN:
			{
				DelClient(pIoMsg->_sClient);

				break;
			}
		case IO_ON_RECV:
			{
				InterlockedIncrement(&m_lCount);

				shared_ptr<ClientItem> spClient = FindClient(pIoMsg->_sClient);

				if (spClient)
				{
					spClient->RecordMsgTime();

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

void ServerItem::CloseConn(SOCKET sClient)
{
	IoManager* pIoManager = CreateInstance<IoManager>();

	assert(NULL != pIoManager);

	pIoManager->CloseConn(m_sServer,sClient);
}
void ServerItem::PostIoMsg(IoMsg* pIoMsg)
{
	while (m_MsgQueue.PushBack(pIoMsg) == -1)
	{
		Sleep(0);
	}
}

int ServerItem::AddClient(SOCKET sClient,shared_ptr<ClientItem> spClient)
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
shared_ptr<ClientItem> ServerItem::FindClient(SOCKET sClient)
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
int ServerItem::DelClient(SOCKET sClient)
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