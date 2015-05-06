/************************************************************************
* FileName : NetItem.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/

#ifndef __NETITEM_H__
#define __NETITEM_H__
#include "MemoryPool.h"
#include "ClassFactory.h"
class NetItem
{
public:
	NetItem() : m_sServer(INVALID_SOCKET),m_sClient(INVALID_SOCKET),m_nLocalPort(-1),m_nRemotePort(-1),m_pMemoryPool(NULL)
	{ 
		memset(m_szLocalIp,0,20);
		memset(m_szRemoteIp,0,20);
		m_pMemoryPool = CreateInstance<MemoryPool>();
		assert(NULL != m_pMemoryPool);
	}
	virtual ~NetItem(){}

	void SetServerSocket(SOCKET sServer)		{ m_sServer = sServer;}
	SOCKET GetServerSocket()					{ return m_sServer;}

	void SetClientSocket(SOCKET sClient)		{ m_sClient = sClient;}
	SOCKET GetClientSocket()					{ return m_sClient;}

	void SetLocalPort(int nPort)				{ m_nLocalPort = nPort;}
	int GetLocalPort()							{ return m_nLocalPort;}

	void SetRemotePort(int nPort)				{ m_nRemotePort = nPort;}
	int GetRemotePort()							{ return m_nRemotePort;}

	void SetLocalIp(const char* pszIp)			{ if (NULL != pszIp){ int nLen = strlen(pszIp);memcpy(m_szLocalIp,pszIp,nLen < 20 ? nLen : 20);}}
	char* GetLocalIp()							{return (char*)m_szLocalIp;}

	void SetRemoteIp(const char* pszIp)			{ if (NULL != pszIp){ int nLen = strlen(pszIp);memcpy(m_szRemoteIp,pszIp,nLen < 20 ? nLen : 20);}}
	char* GetRemoteIp()							{return (char*)m_szRemoteIp;}
protected:
	SOCKET m_sServer;
	SOCKET m_sClient;
	int    m_nLocalPort;
	char   m_szLocalIp[20];
	int	   m_nRemotePort;
	char   m_szRemoteIp[20];
	MemoryPool* m_pMemoryPool;
};

#endif