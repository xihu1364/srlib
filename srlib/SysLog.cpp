/************************************************************************
* FileName : SysLog.cpp 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/

#include "stdafx.h"
#include <cassert>
#include "SysLog.h"
#include "Ini.h"

/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSysLog::CSysLog() : m_bClose(FALSE),m_bSwitchBin(TRUE),m_bSwitchDbg(TRUE),m_bSwitchRun(TRUE),m_bSwitchStat(TRUE),m_iMaxFileSum(0),
	m_eLevel(LEVEL_NONE),m_nInputFileBinCount(0),m_nInputFileBinSize(0),m_nInputFileDbgCount(0),
	m_nInputFileDbgSize(0),m_nInputFileRunCount(0),m_nInputFileRunSize(0),m_nInputFileStatCount(0),m_nInputFileStatSize(0),
	m_nInputFileTotalCount(0),m_nInputFileTotalSize(0),m_nPushTimeDelay(0),m_hLogThd(INVALID_HANDLE_VALUE),m_nType(LOG_TYPE_ASYN)
{
	m_LogDeque.Clear();

	memset(m_szPath,0,MAX_PATH);
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
CSysLog::~CSysLog()
{

}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSysLog::Init()
{
	char szPath[MAX_PATH] = {0};

	GetModuleFileName(NULL,szPath,MAX_PATH);

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
	strcat(szPath,"log\\");

	strcpy(m_szPath,szPath);

	int iRet = ::CreateDirectory(m_szPath,NULL);

	if (iRet != 1 && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		return;
	}
	LoadConfig();

	unsigned int nWorkId = 0;

	if (INVALID_HANDLE_VALUE == m_hLogThd)
	{
		m_hLogThd = (HANDLE)_beginthreadex(NULL,0,LogThreadWorker,this,CREATE_SUSPENDED,(unsigned*)&nWorkId);

		if (INVALID_HANDLE_VALUE != m_hLogThd)
		{
			SetThreadPriority(m_hLogThd,THREAD_PRIORITY_ABOVE_NORMAL);

			ResumeThread(m_hLogThd);
		}
	}
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSysLog::UnInit()
{
	m_bClose = TRUE;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
void CSysLog::LoadConfig()
{
	CIniFile* pIni  = CreateInstance<CIniFile>();

	if (NULL != pIni)
	{
		m_eLevel		= (LEVEL_TYPE)atoi(pIni->GetKeyValue("Log","Level").c_str());
		m_bSwitchRun	= atoi(pIni->GetKeyValue("Log","RunSwitch").c_str());
		m_bSwitchDbg	= atoi(pIni->GetKeyValue("Log","DbgSwitch").c_str());
		m_bSwitchBin	= atoi(pIni->GetKeyValue("Log","BinSwitch").c_str());
		m_bSwitchStat	= atoi(pIni->GetKeyValue("Log","StatSwitch").c_str());
		m_iMaxFileSum	= atoi(pIni->GetKeyValue("Log","MaxFileSum").c_str());
		m_iTimeLimit	= atoi(pIni->GetKeyValue("Log","TimeLimit").c_str());
		m_lFileSizeMax	= atoi(pIni->GetKeyValue("Log","FileSizeMax").c_str());
		m_nPushTimeDelay= atoi(pIni->GetKeyValue("Log","PushTimeDelay").c_str());
	}
	else
	{
		assert(0);
	}
	WriteLogRun(0,LEVEL_BUTT,0,(char*)LogHead,strlen(LogHead));

	WriteLogDbg(0,LEVEL_BUTT,0,(char*)LogHead,strlen(LogHead));

	WriteLogStat(0,LEVEL_BUTT,0,(char*)LogHead,strlen(LogHead));

	WriteLogBin(0,LEVEL_BUTT,0,(char*)LogHead,strlen(LogHead),0);
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::PushLog(unsigned short nType,unsigned short nLevel,char* pszLogMsg,unsigned int nLogLen,char* pszTagMsg /*= NULL*/)
{
	int nRet = -1;

	//防止恶意写日志
	Sleep(m_nPushTimeDelay);

	if (NULL == pszLogMsg || nLogLen <= 0)
	{
		return nRet;
	}
	LogInfo loginfo;

	if (NULL != pszTagMsg)
	{
		int nLen = strlen(pszTagMsg);

		if (nLen > 128)
		{
			nLen = 128;
		}
		strncpy(loginfo.szTagMsg,pszTagMsg,nLen);
	}
	
	GetDateTime(loginfo.szLogTime);

	loginfo.nThreadId = GetCurrentThreadId();

	loginfo.nLogLen = nLogLen;

	loginfo.nLevel = nLevel;

	loginfo.nType = nType;

	loginfo.pLogMsg = new char[nLogLen + 1];

	if (NULL == loginfo.pLogMsg)
	{
		return nRet;
	}
	memcpy(loginfo.pLogMsg,pszLogMsg,nLogLen);

	loginfo.pLogMsg[nLogLen] = '\0';

	if (m_nType == LOG_TYPE_ASYN)
	{
		m_LogDeque.PushBack(loginfo);
	}
	else
	{
		switch (loginfo.nType)
		{
		case T_SYS_RUN:
			{
				WriteLogRun(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
				break;
			}
		case T_SYS_DBG:
			{
				WriteLogDbg(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
				break;
			}
		case T_SYS_STAT:
			{
				WriteLogStat(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
				break;
			}
		case T_SYS_BIN:
			{
				WriteLogBin(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen,loginfo.szTagMsg);
				break;
			}
		default:
			{
				//assert(0);
				break;
			}
		}
		if (NULL != loginfo.pLogMsg)
		{
			delete[] loginfo.pLogMsg;

			loginfo.pLogMsg = NULL;
		}
	}
	

	return nRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::GetLevelString(int nLevel,char* pszLevel)
{
	int nRet = -1;

	if (NULL == pszLevel)
	{
		return nRet;
	}

	switch (nLevel)
	{
	case LEVEL_NONE:
		{
			break;
		}
	case LEVEL_INFO:
		{
			strcpy(pszLevel,"Info");

			nRet = 0;

			break;
		}
	case LEVEL_WARNING:
		{
			strcpy(pszLevel,"Warning");

			nRet = 0;

			break;
		}
	case LEVEL_ERROR:
		{
			strcpy(pszLevel,"Error");

			nRet = 0;

			break;
		}
	default:
		{
			break;
		}
	}
	return nRet;
}
/************************************************************************
* Fuction : FindFile
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::FindFile(char* pFileName)
{
	int iRet = -1;
	if (NULL == pFileName)
	{
		return iRet;
	}
	CFile file;

	CFileException e;

	iRet = file.Open(pFileName,CFile::modeCreate | CFile::modeWrite,&e);

	if (TRUE == iRet)
	{
		file.Close();

		iRet = 0;
	}
	return iRet;
}
/************************************************************************
* Fuction : FindUpFile(char* pFileName)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::FindUpFile(char* pFileName)
{
	int iRet = -1;
	if (NULL == pFileName)
	{
		return iRet;
	}

	char lpSrc[260] = {0};

	strcpy(lpSrc,pFileName);

	char* pTemp = lpSrc;

	int nLen = 0;

	int num = 0;

	while ( *(pTemp++) )
	{
		nLen++;
	}
	while (1)
	{
		if (*pTemp == '\\' )
		{
			lpSrc[nLen - num + 2] = '\0';

			break;
		}
		--pTemp;

		num++;
	}

	iRet = ::CreateDirectory(lpSrc,NULL);

	if (1 == iRet)
	{
		iRet = FindFile(pFileName);

		iRet = 0;
	}
	else
	{
		if (!iRet && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			return iRet;
		}
	}
	return iRet;
}
/************************************************************************
* Fuction : DeleteOldFile(char* pFile)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::DeleteOldFile(char* pFile)
{
	int iRet = -1;

	if (NULL == pFile)
	{
		return iRet;
	}
	char szOldFile[260] = {0};

	char szCurrentFilePath[260] = {0};

	strcpy(szCurrentFilePath,pFile);

	strcat(szCurrentFilePath,"\\*.log");

	WIN32_FIND_DATA findOldFile;

	WIN32_FIND_DATA findNewFile;

	int count = 1;

	HANDLE hanle = FindFirstFile(szCurrentFilePath,&findOldFile);

	if (INVALID_HANDLE_VALUE != hanle)
	{
		strcpy(szOldFile,findOldFile.cFileName);

		while (FindNextFile(hanle,&findNewFile))
		{
			count++;

			if (1 == CompareFileTime(&findOldFile.ftCreationTime,&findNewFile.ftCreationTime))
			{
				memset(szOldFile,0,260);

				strcpy(szOldFile,findNewFile.cFileName);
			}
		}
	}
	FindClose(hanle);

	if ( m_iMaxFileSum <= count)
	{
		char TempBuf[260] = {0};

		strcpy(TempBuf,pFile);

		strcat(TempBuf,szOldFile);

		remove(TempBuf);

		iRet = 0;
	}
	return iRet;
}
/************************************************************************
* Fuction : GetDateTime(char* pDataTime)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::GetDateTime(char* pDataTime)
{
	int iRet = -1;

	if (NULL == pDataTime)
	{
		return iRet;
	}

	CString strtemp;

	strtemp = CTime::GetCurrentTime().Format("%Y-%m-%d %H_%M_%S");

	strcat(pDataTime,strtemp.GetBuffer(strtemp.GetLength()));

	iRet = 0;

	return iRet;
}
/************************************************************************
* Fuction : GetFileName(char* pFile)
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::GetFileName(char* pFile)
{
	int iRet = -1;

	FindUpFile(pFile);

	char szCurrentFilePath[260] = {0};

	strcpy(szCurrentFilePath,pFile);

	strcat(szCurrentFilePath,"\\*.log");

	char szNewFile[260] = {0};

	WIN32_FIND_DATA findNextFile;

	WIN32_FIND_DATA findNewFile;

	HANDLE hanle = FindFirstFile(szCurrentFilePath,&findNewFile);

	if (INVALID_HANDLE_VALUE != hanle)
	{
		strcpy(szNewFile,findNewFile.cFileName);

		while (FindNextFile(hanle,&findNextFile))
		{
			if (1 == CompareFileTime(&findNextFile.ftCreationTime,&findNewFile.ftCreationTime))
			{
				findNewFile = findNextFile;

				memset(szNewFile,0,260);

				strcpy(szNewFile,findNewFile.cFileName);
			}
		}
		FILETIME CurrentFileTime;

		SYSTEMTIME CurrentTime;

		GetSystemTime(&CurrentTime);

		SystemTimeToFileTime(&CurrentTime,&CurrentFileTime);

		LONGLONG* pCurrentFileTime = (LONGLONG*)&CurrentFileTime;

		LONGLONG* pFileCreateFileTime = (LONGLONG*)&findNewFile.ftCreationTime;

		LONGLONG ulTime = *pCurrentFileTime - *pFileCreateFileTime;

		//判断文件创建时间是否超过指定时间
		if (ulTime < (LONGLONG)m_iTimeLimit*HOURS)
		{
			//判断文件是否超过指定大小
			if (findNewFile.nFileSizeLow < (ULONG)m_lFileSizeMax*10*KB)  
			{
				strcat(pFile,"\\");

				strcat(pFile,szNewFile);

				iRet = 0;

				FindClose(hanle);

				return iRet;
			}
		}
	}
	FindClose(hanle);

	DeleteOldFile(pFile);

	char szTimeString[260] = {0};

	GetDateTime(szTimeString);

	strcat(pFile,"\\");

	strcat(pFile,szTimeString);

	strcat(pFile,".log");

	iRet = 0;

	return iRet;
}
/************************************************************************
* Fuction : ForMatString
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::ForMatString(char* pDestBuf,int iMaxDestLen,char* pBuffer,...)
{
	int iRet = -1;

	int iListCount = 0;

	if (NULL == pDestBuf || NULL == pBuffer)
	{
		return iRet;
	}
	va_list pArgList;

	va_start(pArgList,pBuffer);

	iListCount += _vsnprintf(pDestBuf + iListCount,iMaxDestLen - iListCount,pBuffer,pArgList);

	va_end(pArgList);

	if(iListCount > (iMaxDestLen - 1)) 
	{
		iListCount = iMaxDestLen - 1;
	}
	*(pDestBuf + iListCount) = '\0';

	iRet = iListCount;

	return iRet;
}
/************************************************************************
* Fuction : StringToHex
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::StringToHex(char* pDestBuf,char* pSrcBuf,int iSrcLen)
{
	int iRet = -1;
	int i	 = 0;
	int j	 = 0;

	if (NULL == pDestBuf || NULL == pSrcBuf)
	{
		return iRet;
	}
	for(i = 0;i < iSrcLen; i++)
	{
		iRet += ForMatString(pDestBuf + iRet,256,"%02X ",(unsigned char)*(pSrcBuf + i));

		j++;

		if(4 == j)
		{
			j = 0;

			iRet += ForMatString(pDestBuf + iRet,256," ");
		}
	} 
	if(16 > iSrcLen) 
	{
		for( ;i < 16;i++)
		{
			iRet += ForMatString(pDestBuf + iRet,256," ");

			j++;

			if(4 == j)
			{
				j = 0;

				iRet += ForMatString(pDestBuf + iRet,256," ");
			}
		}
	} 
	return iRet;
}
/************************************************************************
* Fuction : StringToAsci
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::StringToAsci(char* pDestBuf,char* pSrcBuf,int iSrcLen)
{
	int iRet = -1;

	if (NULL == pDestBuf || NULL == pSrcBuf)
	{
		return iRet;
	}
	for(int i = 0; i < iSrcLen; i++) 
	{
		//ASCII 字符表中，可显示字符代码>32
		if(32 <= *(pSrcBuf + i)) 
		{
			iRet += ForMatString( pDestBuf + iRet, 256, "%c", *(pSrcBuf + i));
		}
		else
		{
			iRet += ForMatString( pDestBuf + iRet, 256, ".");
		}
	} 
	return iRet;
}
/************************************************************************
* Fuction : ForMatBin
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::ForMatBin(string& strOutput,char* pSrcBuf,int iSrcLen)
{
	int iRet = -1;
	if (NULL == pSrcBuf)
	{
		return iRet;
	}
	int nAddr		 = 0;
	int nLineCount	 = 0;
	int n			 = 0;
	int nBufferCount = iSrcLen;
	char szLine[256] = {0}; 

	if( 0 < iSrcLen)
	{
		while(1)
		{
			n = 0;

			n += ForMatString(szLine + n,256 - n,"%p   ",pSrcBuf + nAddr);

			nLineCount = 16;

			if(nBufferCount < nLineCount) 
			{
				nLineCount = nBufferCount;
			}
			n += StringToHex(szLine + n,pSrcBuf + nAddr,nLineCount);

			int iCount = 16 - nLineCount;

			if (iCount != 0)
			{
				memset(&szLine[n],32,iCount*2);
			}
			n += StringToAsci(szLine + n + iCount*2,pSrcBuf + nAddr,nLineCount);

			strOutput += szLine;

			nAddr += 16;

			nBufferCount -= 16;

			if(0 >= nBufferCount) 
			{
				break;
			}
			strOutput += "\n";
		} 
	}
	iRet = 0;

	return iRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
unsigned int CSysLog::LogThreadWorker(void* pParam)
{
	unsigned int uRet = -1;

	CSysLog* pSysLog = reinterpret_cast<CSysLog*>(pParam);

	if (NULL != pSysLog)
	{
		uRet = pSysLog->LogWorker();
	}
	return uRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::LogWorker()
{
	LogInfo loginfo;

	while (!m_bClose)
	{
		if (m_LogDeque.WaiteForQueue(loginfo,1))
		{
			switch (loginfo.nType)
			{
			case T_SYS_RUN:
				{
					WriteLogRun(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
					break;
				}
			case T_SYS_DBG:
				{
					WriteLogDbg(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
					break;
				}
			case T_SYS_STAT:
				{
					WriteLogStat(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen);
					break;
				}
			case T_SYS_BIN:
				{
					WriteLogBin(loginfo.nThreadId,loginfo.nLevel,loginfo.szLogTime,loginfo.pLogMsg,loginfo.nLogLen,loginfo.szTagMsg);
					break;
				}
			default:
				{
					//assert(0);
					break;
				}
			}
			if (NULL != loginfo.pLogMsg)
			{
				delete[] loginfo.pLogMsg;

				loginfo.pLogMsg = NULL;
			}
		}
		else
		{
			if (m_LogDeque.GetSize() <= 5000)
			{
				Sleep(1);
			}
		}
	}
	return 0;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::WriteLogRun(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen)
{
	int nRet = -1;

	if (!m_bSwitchRun || m_eLevel > nLevel)
	{
		return nRet;
	}
	char szLevel[10] = {0};

	char szFileName[260] = {0};

	long long nSize = 0;

	GetLevelString(nLevel,szLevel);

	strcpy(szFileName,m_szPath);

	strcat(szFileName,"run\\");

	GetFileName(szFileName);

	nRet = WriteLog(szFileName,pszLogTime,szLevel,nThreadId,pszLogMsg,nSize);

	if (!nRet)
	{
		m_nInputFileRunSize += nSize;
		++m_nInputFileRunCount;
	}

	return nRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::WriteLogDbg(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen)
{
	int nRet = -1;

	if (!m_bSwitchDbg || m_eLevel > nLevel)
	{
		return nRet;
	}
	char szLevel[10] = {0};

	char szFileName[260] = {0};
	long long nSize = 0;

	GetLevelString(nLevel,szLevel);

	strcpy(szFileName,m_szPath);

	strcat(szFileName,"dbg\\");

	GetFileName(szFileName);

	nRet = WriteLog(szFileName,pszLogTime,szLevel,nThreadId,pszLogMsg,nSize);

	if (!nRet)
	{
		m_nInputFileDbgSize += nSize;
		++m_nInputFileDbgCount;
	}

	return nRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::WriteLogBin(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen,char* pTagMsg)
{
	int nRet = -1;

	if (!m_bSwitchBin || m_eLevel > nLevel)
	{
		return nRet;
	}
	string strOutput("\n");

	if (NULL != pTagMsg)
	{
		ForMatBin(strOutput,pszLogMsg,nLogLen);
	}
	else
	{
		strOutput = pszLogMsg;
	}
	char szLevel[10] = {0};

	char szFileName[260] = {0};

	long long nSize = 0;

	GetLevelString(nLevel,szLevel);

	strcpy(szFileName,m_szPath);

	strcat(szFileName,"bin\\");

	GetFileName(szFileName);

	nRet = WriteLog(szFileName,pszLogTime,szLevel,nThreadId,strOutput.c_str(),nSize,pTagMsg);

	if (!nRet)
	{
		m_nInputFileBinSize += nSize;
		++m_nInputFileBinCount;
	}

	return nRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::WriteLogStat(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen)
{
	int nRet = -1;

	if (!m_bSwitchStat || m_eLevel > nLevel)
	{
		return nRet;
	}
	char szLevel[10] = {0};

	char szFileName[260] = {0};

	long long nSize = 0;

	GetLevelString(nLevel,szLevel);

	strcpy(szFileName,m_szPath);

	strcat(szFileName,"stat\\");

	GetFileName(szFileName);

	nRet = WriteLog(szFileName,pszLogTime,szLevel,nThreadId,pszLogMsg,nSize);

	if (!nRet)
	{
		m_nInputFileStatSize += nSize;
		++m_nInputFileStatCount;
	}

	return nRet;
}
/************************************************************************
* Fuction : NA
* Autor   : xihu
* Input   : 
* Output  : 
* Info    : 2012-01
************************************************************************/
int CSysLog::WriteLog(char* pFileName,char* pszLogTime,char* pszLevel,unsigned int nThreadId,const char* pszLogMsg,long long& nSize,char* pTagMsg)
{
	int nRet = -1;

	if (NULL == pFileName)
	{
		return nRet;
	}

	FILE* fp = NULL;

	fp = fopen(pFileName,"a+");

	if (fp != NULL)
	{
		if (pszLogTime == NULL)
		{
			nSize += fprintf(fp,"%s",pszLogMsg);
		}
		else
		{
			if (pTagMsg != NULL)
			{
				nSize += fprintf(fp,"[%s]:[%s][%5d]%s\n%s%s\n",pszLogTime,pszLevel,nThreadId,pTagMsg,BinLogInfo,pszLogMsg);
			}
			else
			{
				nSize += fprintf(fp,"[%s]:[%s][%5d]%s\n",pszLogTime,pszLevel,nThreadId,pszLogMsg);
			}
		}
		fclose(fp);

		fp = NULL;

		m_nInputFileTotalSize += nSize;

		++m_nInputFileTotalCount;

		nRet = 0;
	}
	return nRet;
}

BOOL MonitorFunction::_IsOpen = FALSE;