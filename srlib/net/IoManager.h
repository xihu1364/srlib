/************************************************************************
* FileName : IoManager.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__

#include <hash_map>
#include <memory>
#include "Lock.h"
#include "Singleton.h"
#include "ServerItem.h"
#include "ClientItem.h"
#include "MemoryPool.h"
#include "ringqueue.h"
#include "NetDefine.h"
#include "IocpDriver.h"
#include <functional>

class IoManager
{
	typedef std::hash_map<SOCKET,shared_ptr<ServerItem> > ServerList;
	typedef std::hash_map<SOCKET,shared_ptr<ClientItem> > ClientList;
	typedef RingQueue<IoMsg> MsgQueue;
public:
	~IoManager();

	int InitIoManager();

	int UnInitIoManager();

	template<class T>
	int CreateServer(int nPort,int nServerType = 0,char* pszIp = NULL);

	template<class T>
	T* ConnectServer(char* pszIp,int nPort);

	int PostIoMsg(SOCKET sListen,SOCKET sClient,int nIoType,unsigned int nResult,char* pszBuf,int nNumBytes,int nTick = -1);

	int SendMessage(SOCKET sServer,SOCKET sClient,char* pBuf,int nLen);

	int CloseConn(SOCKET sServer,SOCKET sClient);

	void DumpNode();
protected:
	static unsigned int __stdcall DealIoMsgQueueThread(void* pParam);

	void DealIoMsgQueue();

	int AddServer(SOCKET sServer,shared_ptr<ServerItem> spServer);

	shared_ptr<ServerItem> FindServer(SOCKET sServer);

	int DelServer(SOCKET sServer);

	int AddClient(SOCKET sClient,shared_ptr<ClientItem> spClient);

	shared_ptr<ClientItem> FindClient(SOCKET sClient);

	int DelClient(SOCKET sClient);

	void ProcessIoMsg(IoMsg* pIoMsg);

private:
	DECLARE_SINGLETON_CLASS(IoManager)

	IoManager();

	MsgQueue m_MsgQueue;

	ServerList m_ServerList;
	CriticalSection m_ServerListLock;

	ClientList m_ClientList;
	CriticalSection m_ClientListLock;

	MemoryPool* m_pMemoryPool;

	HANDLE m_hDealIoMsgThread;

	BOOL m_bClose;

	BOOL m_bInit;

	long m_lCount;

	unsigned int m_uKeepAliveTime;

	IocpDriver m_IocpDirver;
};

template<class T>
int IoManager::CreateServer(int nPort,int nServerType /*= 0*/,char* pszIp/* = NULL*/)
{
	int nRet = -1;

	T* pServer = (T*)m_pMemoryPool->Malloc(sizeof(T));

	new(pServer)  T();

	shared_ptr<T> spServer(pServer,tr1::bind(&MemoryPool::Free,m_pMemoryPool,tr1::placeholders::_1));

	SOCKET sListen = INVALID_SOCKET;

	nRet = m_IocpDirver.Listen(sListen,nPort,pszIp);

	if (nRet)
	{
		return nRet;
	}
	spServer->SetServerSocket(sListen);

	spServer->SetLocalPort(nPort);

	spServer->SetLocalIp(pszIp);

	spServer->StartServer();

	nRet = AddServer(sListen,spServer);

	PostIoMsg(NULL,NULL,IO_ON_CREATE,0,NULL,0);

	return nRet;
}

template<class T>
T* IoManager::ConnectServer(char* pszIp,int nPort)
{
	T* pClient = NULL;

	int nRet = -1;

	SOCKET sConn = INVALID_SOCKET;

	pClient = (T*)m_pMemoryPool->Malloc(sizeof(T));

	new(pClient)  T();

	shared_ptr<T> spClient(pClient,tr1::bind(&MemoryPool::Free,m_pMemoryPool,tr1::placeholders::_1));

	nRet = m_IocpDirver.Connect(sConn,nPort,pszIp);

	if (nRet == 0)
	{
		spClient->SetClientSocket(sConn);

		spClient->SetRemotePort(nPort);

		spClient->SetRemoteIp(pszIp);

		spClient->RecordMsgTime();

		AddClient(sConn,spClient);
	}
	else
	{
		pClient = NULL;
	}
	return pClient;
}

#endif