/************************************************************************
* FileName : SRongIni.h
* Autor    : xihu
* Info     : 2012-01
************************************************************************/

#ifndef __SRONGINI_H__
#define __SRONGINI_H__

#pragma warning(disable:4996)

#include "Singleton.h"
#include <string>
#include <vector>
#include <map>


using namespace std;

#define APP_INI_PATH		".\\config\\XXX.ini"
#define MAX_SECTION			256				//Section��󳤶�
#define MAX_KEY				256				//KeyValues��󳤶�
#define MAX_ALLSECTIONS		65535			//����Section����󳤶�
#define MAX_ALLKEYS			65535			//����KeyValue����󳤶�

typedef void (*PSetIniFile)();
/************************************************************************
*	ClassName :	CIniFile
*   Autor	  : xihu
*	Info	  : �����ļ���
*	Date	  : 2011-12-30
************************************************************************/
class CIniFile
{
public:

	virtual ~CIniFile();

	BOOL Initialize(PSetIniFile pCallFunc = NULL);

	//����ini�ļ�·��
	BOOL SetPath(const string& strPath);

	string& GetPath() { return m_strPath;}

	//���section�Ƿ����
	BOOL SectionExist(const string& strSection) const;

	//��ָ����Section��Key��ȡKeyValue
	string GetKeyValue(const string& strSection,const string& strKey) const;

	//����Section��Key�Լ�KeyValue
	BOOL SetKeyValue(const string& strSection,const string& strKey,const string& strKeyValue) const;

	//ɾ��ָ��Section�µ�һ��Key
	BOOL DeleteKey(const string& strSection,const string& strKey) const;

	//ɾ��ָ����Section�Լ����µ�����Key
	BOOL DeleteSection(const string& strSection) const;

	//������е�Section
	int GetAllSections(vector<string>& strArrSection) const;

	//����ָ��Section�õ����µ�����Key��KeyValue
	int GetAllKeysAndValues(const string& strSection,map<string, string>& mapKey2Value) const;

private:
	CIniFile();
	DECLARE_SINGLETON_CLASS(CIniFile)
private:
	string	m_strPath;					// ini�ļ�·��
	BOOL    m_bInitialize;				// �Ƿ��ʼ���������ļ�
}; 




#endif // __SRONGINI_H__
