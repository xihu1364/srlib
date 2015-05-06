/************************************************************************
* FileName : ServerItem.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#ifndef __SERVERITEM_H__
#define __SERVERITEM_H__
#pragma warning(disable:4005)

#include <hash_map>
#include <memory>
#include "Lock.h"
#include "NetItem.h"
#include "ringqueue.h"
#include "NetDefine.h"
#include "ClientItem.h"




class MemoryPool;

using namespace std::tr1;

class ServerItem : public NetItem
{
	typedef std::hash_map<SOCKET,shared_ptr<ClientItem> > ClientList;
	typedef RingQueue<IoMsg> MsgQueue;
public:
	ServerItem();

	virtual ~ServerItem();

	void PostIoMsg(IoMsg* pIoMsg);

	int StartServer();

	virtual void OnAccept(ClientItem*& pClientItem);

	virtual int SendMessage(SOCKET sClient,char* pBuf,int nLen);

	void CloseConn(SOCKET sClient);

	int GetClientNum()					{ return m_ClientList.size();}

	int GetMsgQueueSize()				{ return m_MsgQueue.sizes();}

	long GetThroughPut()				{ static long lLast = 0;long lRet = 0;lRet = m_lCount - lLast;lLast = m_lCount;return lRet;} 

protected:
	static unsigned int __stdcall DealIoMsgQueueThread(void* pParam);

	void DealIoMsgQueue();

	int AddClient(SOCKET sClient,shared_ptr<ClientItem> spClient);

	shared_ptr<ClientItem> FindClient(SOCKET sClient);

	int DelClient(SOCKET sClient);

	void ProcessIoMsg(IoMsg* pIoMsg);

protected:
	ClientList m_ClientList;
	CriticalSection m_ClientListLock;

	HANDLE m_hDealIoMsgThread;

	BOOL m_bClose;

	MsgQueue m_MsgQueue;

	long m_lCount;

	unsigned int m_uClientTimeOut;
};
#endif