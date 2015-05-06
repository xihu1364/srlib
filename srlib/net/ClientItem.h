/************************************************************************
* FileName : ClientItem.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#ifndef __CLIENTITEM_H__
#define __CLIENTITEM_H__
#pragma warning(disable:4005)
#include <hash_map>
#include "NetItem.h"
#include "threadpool.h"
#include "syndeque.h"

class ServerItem;

class ClientItem : public NetItem,public ITaskObj,public enable_shared_from_this<ClientItem>
{
	typedef std::hash_map<int,MsgData> MapGroupPackage;
public:
	ClientItem(ServerItem* pServerItem) : m_pServerItem(pServerItem),m_bDataCompress(TRUE),m_bVerify(FALSE),m_bActive(FALSE){}
	virtual ~ClientItem(){}

	virtual void GroupPackage(IoMsg* pIoMsg);

	virtual int RunProcess(PMsgData pMsg = NULL);

	virtual int ProcessMessage(PMsgData pMsg = NULL);

	virtual int SendPackage(unsigned int nMsg,char* pBuf,int nLen);

	virtual int OnDisconn()				{ return 0;};

	int Disconn();

	void RecordMsgTime()				{ m_uLastMsgTime = GetTickCount();}

	unsigned int GetLastMsgTime()		{ return m_uLastMsgTime;}

	void SetVerifyFlag(BOOL bVerify)	{ m_bVerify = bVerify;}

	BOOL GetVerifyFlag() const			{ return m_bVerify;}

protected:
	int SendMessage(char* pBuf,int nLen);

protected:
	MapGroupPackage m_GroupPackage;

	ServerItem* m_pServerItem;

	BOOL m_bDataCompress;

	unsigned int m_uLastMsgTime;

	BOOL m_bVerify;

	volatile BOOL m_bActive;

	SynDeque<MsgData> m_MsgQueue;
};
#endif