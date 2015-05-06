/******************************************************************
*  FileName ��SRongLock.h
*  Autor    ��xihu
*  Info	    ��
*  Data     ��
*******************************************************************/
#ifndef  __SRongLock_H__
#define	 __SRongLock_H__

//������
class CSRongLockBase
{
public:
	CSRongLockBase(){ };
	virtual ~CSRongLockBase() { };
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE) = 0;

	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL) = 0;

	virtual BOOL IsLock() = 0;
};

/*
	�ٽ���:  �����ڵ��������еĶ��߳�,���������ͬ�����ƣ�Ч�ʿ�.ȱ��:���������벻ͬ����,�����߳�Ϊ�ͷ�,�������̵߳ò���������ȥ�ͷ���.
*/
class CriticalSection : public CSRongLockBase
{
public:
	CriticalSection();
	virtual ~CriticalSection();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL);
	virtual BOOL IsLock();
private:
	CRITICAL_SECTION m_CriticalSection;
};

/*
	�¼�:   �����ڶ��߳�,�����.���ٽ���Ч�ʵ��ٶ���
	Win32 api��:
		OpenEvent();   //�������ƴ�һ���Ѿ��򿪵��¼� OpenEvent(EVENT_ALL_ACCESS,false,pszName)
		CreateEvent(); //����һ���¼� CreateEvent(NULL,[�Զ�]TRUE or[�ֶ�]FALSE,[��ʼ״̬]TRUE[���ź�] or FALSE[���ź�],pszName )
			����:
		CloseHandle(); //�ر��¼�
		SetEvent();    //�����¼�Ϊ���ź�״̬
		ResetEvent();  //�����¼�Ϊ���ź�״̬
		PulseEvent();  //�����ض����¼�����Ϊ���ź�״̬��Ȼ���ͷ���صĵȴ��̣߳�����ʱ�����Ϊ���ź�״̬,
					   //MSDN˵�ˣ���������ǲ��õģ���ò�Ҫʹ��
   ʹ�÷�����
   1.��������
   2.����Create���������ֶ�/�Զ�,��ʼֵ,����.
   3.SetEvent�������ź�״̬
   4.WaitEvent�ȴ��ź�
   5.ResetEvent�������ź�
*/
class Event : public CSRongLockBase
{
public:
	Event(char* pszName = NULL);
	virtual ~Event();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1, long*  lpPrevCount = NULL);
	virtual BOOL IsLock();//�ӻ���̳еĽӿڣ��ж��Ƿ����¼��ź� ��(true) ��(false)
	BOOL Create(char* pszName = NULL,BOOL bInit = FALSE,BOOL bManualReset = FALSE);
	BOOL Open(char* pszName = NULL);
	BOOL Close();
	BOOL SetEvent();
	BOOL ResetEvent();
	BOOL PulseEvent();
	BOOL GetEventSignal();
	BOOL WaitEvent(unsigned int uTimeout = INFINITE);
	HANDLE GetEventHandle();
	//�˽ӿڴ��Ժ�ʹ��  ���һ���������� Ӧ����static����
	int WaitEvents();
private:
	HANDLE m_Event;
};

/*
	�ź��������¼�������ͬ������һ��������.
	API
		OpenSemaphore();
		CreateSemaphore();
		CloseHandle();
		ReleaseSemaphore();
		WaitForSingleObject();
	ʹ�÷�����
	1.��������
	2.����Create������������ź���MaxCount
	3.����Enter��������õĴ�������MaxCount�������޵ȴ�
	4.����Leave������Enter�Ĵ�����lCount
*/
class Semaphore : public CSRongLockBase
{
public:
	Semaphore(char* pszName = NULL);
	virtual ~Semaphore();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1,long* lpPrevCount = NULL);
	virtual BOOL IsLock();
	BOOL Create(long lCount,char* pszName = NULL);
	BOOL Enter(unsigned int uTimerOut = INFINITE);
	BOOL Leave(long lCount = 1,long* lpPrevCount = NULL);
	BOOL Release();
private:
	HANDLE m_hSemaphore;
	long    m_lMaxCount;
};
//������
/*
����������һ��ʹ��������һ���߳�I D��һ���ݹ������������������Ϊ������ؼ��������ͬ��
���ǻ�����������ں˶��󣬶��ؼ�������������û���ʽ��������ζ�Ż������������ٶȱȹؼ������Ҫ����
������Ҳ��ζ�Ų�ͬ�����еĶ���߳��ܹ����ʵ���������󣬲�������ζ���߳��ڵȴ�������Դʱ�����趨һ����ʱֵ��

*/
class Mutex : public CSRongLockBase
{
public:
	Mutex(char* pszName = NULL);
	virtual ~Mutex();
	virtual BOOL Lock(unsigned int dwTimeout = INFINITE);
	virtual BOOL UnLock(long lCount = 1,long* lpPrevCount = NULL);
	virtual BOOL IsLock();
	BOOL Create(char* pszName = NULL,BOOL bInit = FALSE);
	BOOL Release();
private:
	HANDLE m_hMutex;
};

//��������--��д����
class CSRongSyncObj
{
public:
	CSRongSyncObj(CSRongLockBase* pLock);
	~CSRongSyncObj();
private:
	CSRongLockBase* m_pLock;
};

//��������--��д���
//ԭ����һ������������һ��д��ǣ�һ���ٽ�����
//��ֻ��������£����Զ�ζ�������д����д������£�����д��Ҳ���ܶ���
typedef struct tag_MultiWriteSingleRead
{
	UINT s_nReadCount;
	BOOL s_bReadFlag;
	CRITICAL_SECTION s_CriticalSection;
}SMultiWriteSingleRead;

class CSRongMultiWriteSingleRead
{
private:
	SMultiWriteSingleRead m_MRSWLock;
public:
	CSRongMultiWriteSingleRead();
	~CSRongMultiWriteSingleRead();
	//��д
	void EnableWrite();
	//����д
	void DisableWrite();
	//��ȡд���
	BOOL GetWriteFlag();
	//��ȡ�Ѷ�����
	UINT GetReadCount();
	//����һ��д
	void AddReadCount();
	//����һ��д
	void DecReadCount();
	//�Ӷ�ת����д
	void ReadToWrite();
};

class MWSR_int
{
public:
	MWSR_int();
	~MWSR_int();

	int operator==(int&);
	int operator++();
	int operator--();
	int operator+(int&);
	int operator+(MWSR_int&);
	int operator-(int&);
	int operator-(MWSR_int&);
	int operator*(int&);
	int operator/(int&);
	int GetValue()const;
	int SetValue(const int&);
	int SetValue(const MWSR_int&);
private:
	int m_nValue;
	mutable CSRongMultiWriteSingleRead m_Lock;
};



#endif