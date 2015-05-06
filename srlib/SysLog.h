/************************************************************************
* FileName : SysLog.h
* Autor    : xihu
* Info     : 2012-01
************************************************************************/

#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#include "syndeque.h"
#include "Singleton.h"
#include "ClassFactory.h"

#pragma warning(disable:4996)

#define SR_RUN_LOG			WriteRunLog
#define SR_BIN_LOG			WriteBinLog
#define SR_STAT_LOG			WriteStatLog
#define SR_DBG_LOG			WriteDbgLog
#define __MT_FUNC__			MonitorFunction monitor_fun(__FUNCTION__);
#define __OPEN_MT_FUNC__	MonitorFunction::OpenMonitorFunction();
#define __CLOSE_MT_FUNC__	MonitorFunction::CloseMonitorFunction();


#define KB						1024
#define MB						1024*1024
#define HOURS					60*60*10000000    //文件时间是1/10000000秒

#define BinLogInfo				"MEMADDR	 -1--2--3--4---5--6--7--8--Hex-2--3--4---5--6--7--8- ---ASCII CODE---"


/* 日志报警等级 */    
typedef enum
{
	LEVEL_NONE			= 0,
	LEVEL_ERROR			= 1,         /* 错误*/
	LEVEL_WARNING		= 2,         /* 警告*/
	LEVEL_INFO			= 3,		 /* 提示*/
	LEVEL_BUTT
} LEVEL_TYPE;

//日志的类型
typedef enum 
{
	T_SYS_RUN			= 0,		//系统运行日志
	T_SYS_DBG			= 1,		//系统调试日志
	T_SYS_STAT			= 2,		//系统统计日志
	T_SYS_BIN			= 3,		//系统码流日志
	T_SYS_BUTT,
}LOG_TYPE;

const char LogHead[] = \
	"\n  \
	*******************************************************************************\n  \
	*                     CopyRight(C) ***** 2011-2013                            *\n  \
	* AppName : NA                                                                *\n  \
	* Author  : xihu                                                              *\n  \
	* Description : NA                                                            *\n  \
	*******************************************************************************\n";


typedef struct LOGINFO
{
	unsigned int	nThreadId;
	char			szLogTime[25];
	unsigned short	nType;
	unsigned short	nLevel;
	unsigned int	nLogLen;
	char*			pLogMsg;
	char			szTagMsg[128];
	LOGINFO()
	{
		memset(this,0,sizeof(LOGINFO));
	}
	
}LogInfo;

#define LOG_TYPE_SYN	0
#define LOG_TYPE_ASYN	1

class CSysLog
{
public:
	~CSysLog();

	void Init();

	void UnInit();

	int PushLog(unsigned short nType,unsigned short nLevel,char* pszLogMsg,unsigned int nLogLen,char* pszTagMsg = NULL);

	void SetSynType() { m_nType = LOG_TYPE_SYN;}

	int GetLogNumInQueue()				{	return m_LogDeque.GetSize();	}
	long long GetDbgCount()		const	{	return m_nInputFileDbgCount;	}
	long long GetDbgSize()		const	{	return m_nInputFileDbgSize;		}
	long long GetRunCount()		const	{	return m_nInputFileRunCount;	}
	long long GetRunSize()		const	{	return m_nInputFileRunSize;		}
	long long GetBinCount()		const	{	return m_nInputFileBinCount;	}
	long long GetBinSize()		const	{	return m_nInputFileBinSize;		}
	long long GetStatCount()	const	{	return m_nInputFileStatCount;	}
	long long GetStatSize()		const	{	return m_nInputFileStatSize;	}
	long long GetTotalCount()	const	{	return m_nInputFileTotalCount;	}
	long long GetTotalSize()	const	{	return m_nInputFileTotalSize;	}
	BOOL GetRunSwitch()			const   {	return m_bSwitchRun;			}
	BOOL GetDbgSwitch()			const   {	return m_bSwitchDbg;			}
	BOOL GetBinSwitch()			const   {	return m_bSwitchBin;			}
	BOOL GetStatSwitch()		const   {	return m_bSwitchStat;			}

protected:
	DECLARE_SINGLETON_CLASS(CSysLog)

	CSysLog();

	void LoadConfig();

	static unsigned int _stdcall LogThreadWorker(void* pParam);

	int LogWorker();

	int WriteLogRun(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen);

	int WriteLogDbg(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen);

	int WriteLogBin(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen,char* pTagMsg);

	int WriteLogStat(unsigned int nThreadId,unsigned short nLevel,char* pszLogTime,char* pszLogMsg,unsigned int nLogLen);

	int WriteLog(char* pFileName,char* pszLogTime,char* pszLevel,unsigned int nThreadId,const char* pszLogMsg,long long& nSize,char* pTagMsg = NULL);

	int GetLevelString(int nLevel,char* pszLevel);

	int GetFileName(char* pFile);

	int FindFile(char* pFileName);

	int FindUpFile(char* pFileName);

	int GetDateTime(char* pDataTime);

	int DeleteOldFile(char* pFile);

	int ForMatString(char* pDestBuf,int iMaxDestLen,char* pBuffer,...);

	int ForMatBin(string& pDestBuf,char* pSrcBuf,int iSrcLen);

	int StringToHex(char* pDestBuf,char* pSrcBuf,int iSrcLen);

	int StringToAsci(char* pDestBuf,char* pSrcBuf,int iSrcLen);

private:
	int					m_nType;

	BOOL				m_bSwitchRun;
						
	BOOL				m_bSwitchDbg;
						
	BOOL				m_bSwitchStat;	
					
	BOOL				m_bSwitchBin;	
					
	LEVEL_TYPE			m_eLevel;	
						
	int					m_iMaxFileSum;	
					
	int					m_iTimeLimit;	
					
	long				m_lFileSizeMax;	

	char				m_szPath[MAX_PATH];

	HANDLE				m_hLogThd;

	BOOL				m_bClose;

	SynDeque<LogInfo>	m_LogDeque;

	int					m_nPushTimeDelay;

	//统计写入文件字节数
	volatile long long	m_nInputFileDbgSize;

	volatile long long	m_nInputFileRunSize;

	volatile long long	m_nInputFileStatSize;

	volatile long long	m_nInputFileBinSize;

	volatile long long	m_nInputFileTotalSize;

	//统计写入文件日志条数
	volatile long long	m_nInputFileDbgCount;

	volatile long long	m_nInputFileRunCount;

	volatile long long	m_nInputFileStatCount;

	volatile long long	m_nInputFileBinCount;

	volatile long long	m_nInputFileTotalCount;
};

inline void WriteRunLog(unsigned short nLevel,char* pszLogMsg,...)
{
	int iListCount = 0;

	int nMaxDestLen = 8192;

	char szDestBuf[8192] = {0};

	va_list pArgList;

	va_start(pArgList,pszLogMsg);

	iListCount += _vsnprintf(szDestBuf + iListCount,nMaxDestLen - iListCount,pszLogMsg,pArgList);

	va_end(pArgList);

	if(iListCount > (nMaxDestLen - 1)) 
	{
		iListCount = nMaxDestLen - 1;
	}
	*(szDestBuf + iListCount) = '\0';

	CSysLog* pSysLog = CreateInstance<CSysLog>();

	if (NULL != pSysLog)
	{
		pSysLog->PushLog(T_SYS_RUN,nLevel,szDestBuf,iListCount);
	}
}
inline void WriteDbgLog(unsigned short nLevel,char* pszLogMsg,...)
{
	int iListCount = 0;

	int nMaxDestLen = 8192;

	char szDestBuf[8192] = {0};

	va_list pArgList;

	va_start(pArgList,pszLogMsg);

	iListCount += _vsnprintf(szDestBuf + iListCount,nMaxDestLen - iListCount,pszLogMsg,pArgList);

	va_end(pArgList);

	if(iListCount > (nMaxDestLen - 1)) 
	{
		iListCount = nMaxDestLen - 1;
	}
	*(szDestBuf + iListCount) = '\0';

	CSysLog* pSysLog = CreateInstance<CSysLog>();

	if (NULL != pSysLog)
	{
		pSysLog->PushLog(T_SYS_DBG,nLevel,szDestBuf,iListCount);
	}
}
inline void WriteStatLog(unsigned short nLevel,char* pszLogMsg,...)
{
	int iListCount = 0;

	int nMaxDestLen = 8192;

	char szDestBuf[8192] = {0};

	va_list pArgList;

	va_start(pArgList,pszLogMsg);

	iListCount += _vsnprintf(szDestBuf + iListCount,nMaxDestLen - iListCount,pszLogMsg,pArgList);

	va_end(pArgList);

	if(iListCount > (nMaxDestLen - 1)) 
	{
		iListCount = nMaxDestLen - 1;
	}
	*(szDestBuf + iListCount) = '\0';

	CSysLog* pSysLog = CreateInstance<CSysLog>();

	if (NULL != pSysLog)
	{
		pSysLog->PushLog(T_SYS_STAT,nLevel,szDestBuf,iListCount);
	}
}

inline void WriteBinLog(unsigned short nLevel,char* pszLogMsg,unsigned int nLogLen,char* pszTagMsg)
{
	CSysLog* pSysLog = CreateInstance<CSysLog>();

	if (NULL != pSysLog)
	{
		pSysLog->PushLog(T_SYS_BIN,nLevel,pszLogMsg,nLogLen,pszTagMsg);
	}
}


class MonitorFunction
{
public:
	MonitorFunction(char* pszFunctionName):_pszFunctionName(pszFunctionName),_BeginTick(0)
	{
		if (_IsOpen)
		{
			_BeginTick = GetTickCount();

			SR_DBG_LOG(LEVEL_INFO,"==========Enter %-40s               ==========",pszFunctionName);
		}
	}
	~MonitorFunction()
	{
		if (_IsOpen)
		{
			SR_DBG_LOG(LEVEL_INFO,"==========Out   %-40s UseTime : %4d==========",_pszFunctionName,GetTickCount() - _BeginTick);
		}
	}
	static void OpenMonitorFunction() { _IsOpen = TRUE;}

	static void CloseMonitorFunction(){ _IsOpen = FALSE;}
private:
	int _BeginTick;
	char* _pszFunctionName;
	static BOOL _IsOpen;
};
#endif