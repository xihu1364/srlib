/************************************************************************
* FileName : ClientItem.cpp 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#include "stdafx.h"
#include "ClassFactory.h"
#include "ClientItem.h"
#include "ServerItem.h"
#include "NetDefine.h"
#include "zlib.h"
#include "IoManager.h"
#ifdef _DEBUG
#pragma comment(lib,"zlib_d.lib")
#else
#pragma comment(lib,"zlib.lib")
#endif // _DEBUG


void ClientItem::GroupPackage(IoMsg* pIoMsg)
{
	assert(NULL != pIoMsg);

	IMsgPacket* pPacket = (IMsgPacket*)pIoMsg->_pszBuf;

	if (pPacket->msgHead.nMsgCmd == 1)
	{
		return;
	}
	MsgData msgData;

	MapGroupPackage::iterator it;

	it = m_GroupPackage.find(pPacket->msgHead.nMsgCmd);

	if (it == m_GroupPackage.end())
	{
		msgData.nDataLen = pPacket->msgHead.usMsgLen - MSG_HEAD_LEN;

		//msgData.pMsgData = new char[msgData.nDataLen];

		msgData.pMsgData = (char*)m_pMemoryPool->Malloc(msgData.nDataLen);

		msgData.nCmd = pPacket->msgHead.nMsgCmd;

		memcpy(msgData.pMsgData,pPacket->msgData,msgData.nDataLen);

		if (pPacket->msgHead.nMsgDataType & OP_IMCOMPLETE)
		{
			m_GroupPackage.insert(std::hash_map<int,MSGDATA>::value_type(pPacket->msgHead.nMsgCmd,msgData));

			return;
		}
		else if (pPacket->msgHead.nMsgDataType & OP_COMPLETE)
		{
			//ok
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		msgData.nDataLen = pPacket->msgHead.usMsgLen - MSG_HEAD_LEN + it->second.nDataLen;

		msgData.nCmd = pPacket->msgHead.nMsgCmd;

		//msgData.pMsgData = new char[msgData.nDataLen];

		msgData.pMsgData = (char*)m_pMemoryPool->Malloc(msgData.nDataLen);

		memcpy(msgData.pMsgData,it->second.pMsgData,it->second.nDataLen);

		memcpy(msgData.pMsgData + it->second.nDataLen,pPacket->msgData,pPacket->msgHead.usMsgLen - MSG_HEAD_LEN);

		//delete[] it->second.pMsgData;

		m_pMemoryPool->Free(it->second.pMsgData);

		if (pPacket->msgHead.nMsgDataType & OP_IMCOMPLETE)
		{
			it->second.nDataLen = msgData.nDataLen;

			it->second.pMsgData = msgData.pMsgData;

			return;
		}
		else if (pPacket->msgHead.nMsgDataType & OP_COMPLETE)
		{
			m_GroupPackage.erase(it);
		}
		else
		{
			assert(0);
		}
	}
	if (pPacket->msgHead.nMsgDataType & OP_COMPRESS)
	{
		int nDeCodeLen = pPacket->msgHead.nMsgDataLen;

		//char* pDeCodeBuf = new char[nDeCodeLen];

		char* pDeCodeBuf = (char*)m_pMemoryPool->Malloc(nDeCodeLen);

		assert(pDeCodeBuf != NULL);

		memset(pDeCodeBuf,0x00,nDeCodeLen);

		int nErr = uncompress((Bytef*)pDeCodeBuf,(uLongf*)&nDeCodeLen,(Bytef*)msgData.pMsgData,(uLongf)msgData.nDataLen);

		if (nErr)
		{
			assert(0);
		}
		msgData.nDataLen = nDeCodeLen;

		//delete[] msgData.pMsgData;

		m_pMemoryPool->Free(msgData.pMsgData);

		msgData.pMsgData = pDeCodeBuf;
	}
	else if (pPacket->msgHead.nMsgDataType & OP_UNCOMPRESS)
	{
		//ok
	}
	else
	{
		assert(0);
	}
	if (m_pServerItem == NULL)
	{
		RunProcess(&msgData);
	}
	else
	{
		ThreadPool* pThreadPool = CreateInstance<ThreadPool>();

		shared_ptr<ClientItem> spClient = shared_from_this();

		weak_ptr<ClientItem> wpClient = spClient;

		if (!m_bActive)
		{
			pThreadPool->AddTask(wpClient,NULL);

			m_bActive = TRUE;
		}
		m_MsgQueue.PushBack(msgData);
	}
}
int ClientItem::RunProcess(PMsgData pMsg/* = NULL*/)
{
	int nRet = -1;

	assert(pMsg == NULL);

	MsgData msgDate;

	while (m_MsgQueue.WaiteForQueue(msgDate,2000))
	{
		ProcessMessage(&msgDate);

		if (msgDate.pMsgData != NULL)
		{
			m_pMemoryPool->Free(msgDate.pMsgData);

			msgDate.pMsgData = NULL;
		}
		msgDate.Clear();
	}
	m_bActive = FALSE;

	return nRet;
}
int ClientItem::ProcessMessage(PMsgData pMsg/* = NULL*/)
{
	int nRet = -1;

	return nRet;
}
int ClientItem::SendPackage(unsigned int nMsg,char* pBuf,int nLen)
{
	int nRet = -1;

	IMsgPacket msgPacket;

	msgPacket.msgHead.nMsgCmd = nMsg;

	if (nLen < 0)
	{
		assert(0);
	}
	else if (nLen == 0)
	{
		msgPacket.msgHead.nMsgDataType = OP_UNCOMPRESS | OP_COMPLETE;

		msgPacket.msgHead.nMsgDataLen = 0;

		msgPacket.msgHead.usMsgLen = MSG_HEAD_LEN;

		SendMessage((char*)&msgPacket,msgPacket.msgHead.usMsgLen);
	}
	else
	{
		int nDestLen = 0;

		char* pDestData = NULL;

		char* pTagData = NULL;

		if (m_bDataCompress)
		{
			msgPacket.msgHead.nMsgDataType |= OP_COMPRESS;

			int nError = 0;

			nDestLen = compressBound(nLen);

			pDestData = (char*)m_pMemoryPool->Malloc(nDestLen);

			nError = compress((Bytef*)pDestData,(uLongf*)&nDestLen,(Bytef*)pBuf,nLen);

			assert(nError == 0);
		}
		else
		{
			msgPacket.msgHead.nMsgDataType |= OP_UNCOMPRESS;

			pDestData = pBuf;

			nDestLen = nLen;
		}
		pTagData = pDestData;

		if (nDestLen <= MSG_DATA_LEN)
		{
			msgPacket.msgHead.nMsgDataType |= OP_COMPLETE;

			memcpy(msgPacket.msgData,pTagData,nDestLen);

			msgPacket.msgHead.nMsgDataLen = nLen;

			msgPacket.msgHead.usMsgLen = MSG_HEAD_LEN + nDestLen;

			SendMessage((char*)&msgPacket,msgPacket.msgHead.usMsgLen);

			//TRACE("Ñ¹ËõÊý¾Ý%s\n",pTagData);
		}
		else
		{
			int nTag = nDestLen;

			int nSendLen = 0;

			while (nTag > 0)
			{
				if (nTag <= MSG_DATA_LEN)
				{
					msgPacket.msgHead.nMsgDataType ^= OP_IMCOMPLETE;

					msgPacket.msgHead.nMsgDataType |= OP_COMPLETE;
				}
				else
				{
					msgPacket.msgHead.nMsgDataType |= OP_IMCOMPLETE;
				}
				nSendLen = nTag > MSG_DATA_LEN ? MSG_DATA_LEN : nTag;

				memcpy(msgPacket.msgData,pTagData,nSendLen);

				msgPacket.msgHead.nMsgDataLen = nLen;

				msgPacket.msgHead.usMsgLen = MSG_HEAD_LEN + nSendLen;

				SendMessage((char*)&msgPacket,msgPacket.msgHead.usMsgLen);

				pTagData += nSendLen;

				nTag -= nSendLen;
			}
		}
		if (m_bDataCompress)
		{
			m_pMemoryPool->Free(pDestData);
		}
	}
	return nRet;
}

int ClientItem::SendMessage(char* pBuf,int nLen)
{
	int nRet = -1;

	IoManager* pIoManager = CreateInstance<IoManager>();

	if (NULL == m_pServerItem)
	{
		RecordMsgTime();

		pIoManager->SendMessage(INVALID_SOCKET,m_sClient,pBuf,nLen);
	}
	else
	{
		m_pServerItem->SendMessage(GetClientSocket(),pBuf,nLen);
	}
	return nRet;
}
int ClientItem::Disconn()
{
	int nRet = -1;

	IoManager* pIoManager = CreateInstance<IoManager>();

	if (NULL == m_pServerItem)
	{
		pIoManager->CloseConn(INVALID_SOCKET,m_sClient);

		nRet = 0;
	}
	return nRet;
}