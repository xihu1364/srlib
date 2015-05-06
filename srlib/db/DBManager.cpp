/************************************************************************
* FileName : DBManager.cpp
* Autor    : xihu
* Info     : 2012-01
************************************************************************/
#include "stdafx.h"
#include <cassert>
#include "DBManager.h"
#include "AdoCommand.h"
#include "AdoConnection.h"
#include "AdoRecordSet.h"

#include "SysLog.h"
#include "Ini.h"
#include "MemoryPool.h"

/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2014/03/31
************************************************************************/
CDBManager::CDBManager(void)
{

}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2014/03/31
************************************************************************/
CDBManager::~CDBManager(void)
{

}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2014/03/31
************************************************************************/
int CDBManager::InitDBManager(void)
{
	CoInitialize(NULL);

	CIniFile* pIniFile = CreateInstance<CIniFile>();

	strcpy(m_szHostName,pIniFile->GetKeyValue("DB","DbHost").c_str());

	strcpy(m_szDbName,pIniFile->GetKeyValue("DB","DbName").c_str());

	strcpy(m_szDbUserName,pIniFile->GetKeyValue("DB","DbUser").c_str());

	strcpy(m_szDbPassWord,pIniFile->GetKeyValue("DB","DbPwd").c_str());

	MemoryPool* pMemoryPool = CreateInstance<MemoryPool>();

	assert(NULL != pMemoryPool);

	for (int i = 0;i < 200;i++)
	{
		CAdoConnection* pDbConn = (CAdoConnection*)pMemoryPool->Malloc(sizeof(CAdoConnection));

		new(pDbConn) CAdoConnection();

		if (!pDbConn->ConnectSQLServer(m_szHostName,m_szDbName,m_szDbUserName,m_szDbPassWord))
		{
			assert(0);
		}
		m_FreeDbList.push_back(pDbConn);
	}
	return 0;
}
void CDBManager::UnInitDBManager()
{
	MemoryPool* pMemoryPool = CreateInstance<MemoryPool>();

	assert(NULL != pMemoryPool);

	for (int i = 0;i < 20;i++)
	{
		pMemoryPool->Free(m_FreeDbList[i]);
	}
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2014/03/31
************************************************************************/
CAdoConnection* CDBManager::GetDBConnnection()
{
	CoInitialize(NULL);

	CAdoConnection* pAdoConn = NULL;

	int nThreadId = GetCurrentThreadId();

	int nConnId = nThreadId / 4 % 200;

	//TRACE("Thread %d ID %d GetDb\n",nThreadId,nConnId);

	pAdoConn = m_FreeDbList[nConnId];

	_RecordsetPtr pRet = pAdoConn->Execute("select @@version");		//查询数据库连接是否正常

	if (NULL == pRet)
	{
		if (!pAdoConn->ConnectSQLServer(m_szHostName,m_szDbName,m_szDbUserName,m_szDbPassWord))
		{
			SR_RUN_LOG(LEVEL_ERROR,"重新连接数据库失败！ 错误 %d",GetLastError());

			return NULL;
		}                                                  
	}
	return pAdoConn;
}


