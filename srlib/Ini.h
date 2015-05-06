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
#define MAX_SECTION			256				//Section最大长度
#define MAX_KEY				256				//KeyValues最大长度
#define MAX_ALLSECTIONS		65535			//所有Section的最大长度
#define MAX_ALLKEYS			65535			//所有KeyValue的最大长度

typedef void (*PSetIniFile)();
/************************************************************************
*	ClassName :	CIniFile
*   Autor	  : xihu
*	Info	  : 配置文件类
*	Date	  : 2011-12-30
************************************************************************/
class CIniFile
{
public:

	virtual ~CIniFile();

	BOOL Initialize(PSetIniFile pCallFunc = NULL);

	//设置ini文件路径
	BOOL SetPath(const string& strPath);

	string& GetPath() { return m_strPath;}

	//检查section是否存在
	BOOL SectionExist(const string& strSection) const;

	//从指定的Section和Key读取KeyValue
	string GetKeyValue(const string& strSection,const string& strKey) const;

	//设置Section、Key以及KeyValue
	BOOL SetKeyValue(const string& strSection,const string& strKey,const string& strKeyValue) const;

	//删除指定Section下的一个Key
	BOOL DeleteKey(const string& strSection,const string& strKey) const;

	//删除指定的Section以及其下的所有Key
	BOOL DeleteSection(const string& strSection) const;

	//获得所有的Section
	int GetAllSections(vector<string>& strArrSection) const;

	//根据指定Section得到其下的所有Key和KeyValue
	int GetAllKeysAndValues(const string& strSection,map<string, string>& mapKey2Value) const;

private:
	CIniFile();
	DECLARE_SINGLETON_CLASS(CIniFile)
private:
	string	m_strPath;					// ini文件路径
	BOOL    m_bInitialize;				// 是否初始化了配置文件
}; 




#endif // __SRONGINI_H__
