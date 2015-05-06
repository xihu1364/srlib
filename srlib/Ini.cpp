/************************************************************************
* FileName : SRongIni.cpp
* Autor    : xihu
* Info     : 2012-01
************************************************************************/
#include "stdafx.h"
#include "Ini.h"
#include <afx.h>



CIniFile::CIniFile()
{
	m_strPath.empty();
	m_bInitialize = FALSE;
}
CIniFile::~CIniFile()
{
}
BOOL CIniFile::Initialize(PSetIniFile pCallFunc /*= NULL*/)
{
	BOOL bRet = FALSE;
	
	//创建程序名目录
	int nRet = -1;
	char szPath[MAX_PATH] = {0};
	char szModuleName[MAX_PATH] = {0};
	GetModuleFileName(NULL,szPath,MAX_PATH);
	strncpy(szModuleName,szPath,strlen(szPath) + 1);
	PathStripPath(szModuleName);
	char* p = szPath;
	while (*p++ != '\0');
	while (1)
	{
		if (*p == '.')
		{
			*p = '\\';
			*(p + 1) = '\0';
			break;
		}
		p--;
	}
	strcat(szPath,"config.ini");

	m_strPath = szPath;

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(szPath,&fd);

	if (INVALID_HANDLE_VALUE != hFind)
	{
		m_bInitialize = TRUE;
	}
	FindClose(hFind);

	if (!m_bInitialize)
	{
		//设置配置文件默认路径
		SetPath(szPath);
		SetKeyValue("Log","Level","3");
		SetKeyValue("Log","RunSwitch","1");
		SetKeyValue("Log","DbgSwitch","0");
		SetKeyValue("Log","BinSwitch","0");
		SetKeyValue("Log","StatSwitch","0");
		SetKeyValue("Log","MaxFileSum","10");
		SetKeyValue("Log","TimeLimit","24");
		SetKeyValue("Log","FileSizeMax","1024");
		SetKeyValue("Log","PushTimeDelay","10");
		SetKeyValue("Thread","DefaultNum","20");
		SetKeyValue("Thread","RegTimeDelay ","10");

		if (NULL != pCallFunc)
		{
			pCallFunc();
		}
		m_bInitialize = TRUE;
	}
	bRet = TRUE;
	return bRet;
}
/***************************************************************************
Function	: SetPath
Description : 设置ini文件路径
Input		: strPath--ini文件路径
Output		: NA
Return		: 成功返回TRUE;否则返回FALSE
Remark	    : NA
***************************************************************************/
BOOL CIniFile::SetPath(const string& strPath)
{

    m_strPath = strPath;
    // 检查文件是否存在
	DWORD  dwFlag = ::GetFileAttributes((LPCTSTR)m_strPath.c_str());

    // 文件或者路径不存在，返回FALSE
    if( 0xFFFFFFFF == dwFlag )
	{
		char lpSrc[MAX_PATH + 1];
		memset(lpSrc,0,MAX_PATH + 1);
		strcpy(lpSrc,(LPCTSTR)m_strPath.c_str());
		LPCTSTR temp = lpSrc;
		int nLen = 0;
		int num = 0;

		while ( *(temp++) )
		{
			nLen++;
		}
		while (1)
		{
			if (*temp == '\\' )
			{
				lpSrc[nLen - num +2] = '\0';
				break;
			}
			--temp;
			num++;
		}
		//判断目录是否存在
		if (0xFFFFFFFF == ::GetFileAttributes(lpSrc))
		{
			if(!::CreateDirectory(lpSrc,NULL))
			{
				return FALSE;
			}
		}
		//创建文件
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile((LPCTSTR)m_strPath.c_str(),&fd);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			FindClose(hFind);
			return TRUE;
		}
		FindClose(hFind);
		CFile f;
		CFileException e;
		if( !f.Open( (LPCTSTR)m_strPath.c_str(), CFile::modeCreate | CFile::modeWrite, &e ) )
		{
			return FALSE;
		}
		f.Close();
	}
    return TRUE;
}
/***************************************************************************
Function	: SectionExist
Description : 检查section是否存在
Input		: strSection--需要检查的section
Output		: NA
Return		: 存在返回TRUE;否则返回FALSE
Remark	    : NA
***************************************************************************/
BOOL CIniFile::SectionExist(const string& strSection) const
{
    TCHAR chSection[MAX_SECTION];
	ZeroMemory(chSection, MAX_SECTION);
    DWORD dwRetValue = GetPrivateProfileString((LPCTSTR)strSection.c_str(),
		                                        NULL,
                                                _T(""),
                                                chSection,
                                                sizeof(chSection)/sizeof(TCHAR),
                                                (LPCTSTR)m_strPath.c_str());
    return (dwRetValue > 0);
}

/***************************************************************************
Function	: GetKeyValue
Description : 从指定的Section和Key读取KeyValue
Input		: strSection--指定的section strKey--指定的key
Output		: NA
Return		: 返回KeyValue
Remark	    : NA
***************************************************************************/
string CIniFile::GetKeyValue(const string& strSection,
							  const string& strKey) const
{
    TCHAR chKey[MAX_KEY];
	ZeroMemory(chKey, MAX_KEY);
	string strKeyValue;
    DWORD dwRetValue = GetPrivateProfileString((LPCTSTR)strSection.c_str(),
		                                       (LPCTSTR)strKey.c_str(),
                                               _T(""),
                                               chKey,
                                               sizeof(chKey)/sizeof(TCHAR),
                                               (LPCTSTR)m_strPath.c_str());
	if(dwRetValue > 0)
	{
		strKeyValue = chKey;
	}
    return strKeyValue;
}
/***************************************************************************
Function	: SetKeyValue
Description : 设置Section、Key以及KeyValue
Input		: strSection--指定的section
              strKey--指定的key
			  strKeyValue--指定的value
Output		: NA
Return		: 成功返回TRUE;否则返回FALSE
Remark	    : 若Section或者Key不存在则创建
***************************************************************************/
BOOL CIniFile::SetKeyValue(const string& strSection,const string& strKey,
						   const string& strKeyValue) const
{
	BOOL bRetValue = WritePrivateProfileString((LPCTSTR)strSection.c_str(),
		                                        (LPCTSTR)strKey.c_str(),
                                                (LPCTSTR)strKeyValue.c_str(),
                                                (LPCTSTR)m_strPath.c_str());
	return bRetValue;
}
/***************************************************************************
Function	: DeleteKey
Description : 删除指定Section下的一个Key
Input		: strSection--指定的section strKey--指定的key
Output		: NA
Return		: 成功返回TRUE;否则返回FALSE
Remark	    : 
***************************************************************************/
BOOL CIniFile::DeleteKey(const string& strSection,const string& strKey) const
{
    BOOL bRetValue = WritePrivateProfileString((LPCTSTR)strSection.c_str(),
		                                       (LPCTSTR)strKey.c_str(),
                                               NULL, // 这里写NULL,则删除Key
                                               (LPCTSTR)m_strPath.c_str());
	return bRetValue;
}
/***************************************************************************
Function	: DeleteSection
Description : 删除指定的Section以及其下的所有Key
Input		: strSection--指定的section
Output		: NA
Return		: 成功返回TRUE;否则返回FALSE
Remark	    : 
***************************************************************************/
BOOL CIniFile::DeleteSection(const string& strSection) const
{
    BOOL bRetValue = WritePrivateProfileString((LPCTSTR)strSection.c_str(),
		                                        NULL,
                                                NULL,// 这里都写NULL,则删除Section
                                                (LPCTSTR)m_strPath.c_str());
	return bRetValue;

}
/***************************************************************************
Function	: GetAllSections
Description : 获得所有的Section
Input		: NA
Output		: strArrSection -- 获得的section列表
Return		: 返回Section数目
Remark	    : 
***************************************************************************/
int CIniFile::GetAllSections(vector<string>& strArrSection) const
{
	strArrSection.clear(); //清空原来的数据
	//
    TCHAR chAllSections[MAX_ALLSECTIONS];   
    ZeroMemory(chAllSections, MAX_ALLSECTIONS);
    DWORD dwRetValue = GetPrivateProfileSectionNames(chAllSections,
		                                             MAX_ALLSECTIONS,
                                                     m_strPath.c_str());
	if(0 == dwRetValue)
	{
		return 0;
	}
	int i = 0;
    for(; i < (MAX_ALLSECTIONS - 1); i++)
    {
        if((NULL == chAllSections[i]) && (NULL == chAllSections[i + 1]))
        {
			break;
        }
    }
    i++; 
	TCHAR chTempSection[MAX_SECTION];
	ZeroMemory(chTempSection, MAX_SECTION);
	int iPos = 0;
    for(int j = 0; j < i; j++) 
    {
        chTempSection[iPos++] = chAllSections[j]; 
        if( NULL == chAllSections[j] )
        {
            (void )strArrSection.push_back(chTempSection);
            ZeroMemory(chTempSection, MAX_SECTION);
            iPos = 0;
        }
    }
    return (int )strArrSection.size();
}
/***************************************************************************
Function	: GetAllKeysAndValues
Description : 根据指定Section得到其下的所有Key和KeyValue
Input		: strSection--指定的section
Output		: strArrKey -- 获得的key列表
strArrKeyValue -- 获得的keyValue列表
Return		: 返回Key的数目
Remark	    : 
***************************************************************************/
int CIniFile::GetAllKeysAndValues(const string& strSection,
								  map<string, string>& mapKey2Value) const
{    
	TCHAR chAllKeysAndValues[MAX_ALLKEYS];
	ZeroMemory(chAllKeysAndValues, MAX_ALLKEYS);
    DWORD dwRetValue = GetPrivateProfileSection(strSection.c_str(),
		                                        chAllKeysAndValues,
                                                MAX_ALLKEYS,
                                                m_strPath.c_str());
	if( 0 == dwRetValue )
	{
		return 0;
	}
	int i = 0;
    for(; i < MAX_ALLSECTIONS - 1; i++)
    {
        if( (NULL == chAllKeysAndValues[i] ) && (NULL == chAllKeysAndValues[i+1])) 
        {
			break;
        }
    }
    i++;
    mapKey2Value.clear();
	TCHAR chTempkeyAndValue[MAX_KEY];
	ZeroMemory(chTempkeyAndValue, MAX_KEY);
	int iPos = 0;
	string strTempKey,strTempK,strTempV;
    for(int j = 0; j < i; j++) 
    {
        chTempkeyAndValue[iPos++] = chAllKeysAndValues[j];
        if( NULL == chAllKeysAndValues[j] )
        {
			strTempKey = chTempkeyAndValue;
			size_t intK = -1;
			size_t intV = -1;
			intK = strTempKey.find('=');
			strTempK = strTempKey.substr( 0, intK );
			intV = intK + 1;
			strTempV = strTempKey.substr( intV, -1 );
			mapKey2Value[strTempK] = strTempV;
            ZeroMemory(chTempkeyAndValue, MAX_KEY);
            iPos = 0; 
        } 

    }
    return (int )mapKey2Value.size();
}
//////////////////////////////////////////////////////////////////////////end