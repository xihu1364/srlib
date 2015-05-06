/************************************************************************
* FileName : DBManager.h
* Autor    : xihu
* Info     : 2012-01
************************************************************************/
#ifndef __DBManager_H__
#define __DBManager_H__	

#include <hash_map>
#include <vector>
#include "Singleton.h"
#include "Lock.h"

#include "AdoCommand.h"
#include "AdoConnection.h"
#include "AdoRecordSet.h"


class DBConnector
{
public:
	CAdoConnection* GetDbConn() 
	{
		//m_cs.Lock();

		return &m_AdoConnector;
	}
	void FreeDbConn()
	{
		//m_cs.UnLock();
	}
private:
	CAdoConnection m_AdoConnector;

	CriticalSection m_cs;
};


class CDBManager
{
public:
	~CDBManager(void);

	int InitDBManager(void);

	void UnInitDBManager();

	CAdoConnection* GetDBConnnection();

	//void FreeDbConnection();

protected:
	DECLARE_SINGLETON_CLASS(CDBManager)

	CDBManager();

	char			m_szHostName[128];

	char			m_szDbName[128];

	char			m_szDbUserName[20];

	char			m_szDbPassWord[20];

	std::vector<CAdoConnection*> m_FreeDbList;
};

#endif