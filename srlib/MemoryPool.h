/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <assert.h>
#include <afx.h>
#include <cmath>
#include "Singleton.h"


using namespace std;

enum MemType
{
	Mem_0_Bit_Size		= 0,
	Mem_16_Bit_Size		= 16,
	Mem_32_Bit_Size		= 32,
	Mem_64_Bit_Size		= 64,
	Mem_128_Bit_Size	= 128,
	Mem_256_Bit_Size	= 256,
	Mem_512_Bit_Size	= 512,
	Mem_1k_Bit_Size		= 1024,	
	Mem_2k_Bit_Size		= 2*1024,
	Mem_4k_Bit_Size		= 4*1024,
	Mem_8k_Bit_Size		= 8*1024,
	Mem_16k_Bit_Size	= 16*1024,	
	Mem_32k_Bit_Size	= 32*1024,							
	Mem_64k_Bit_Size	= 64*1024,							
	Mem_128k_Bit_Size	= 128*1024,							
	Mem_256k_Bit_Size	= 256*1024,	
	Mem_512k_Bit_Size	= 512*1024,	
	Mem_1m_Bit_Size		= 1024*1024,
};


struct MemBlock
{
	MemBlock(int nMenLen) : m_nMemLen(nMenLen),m_pPreBlock(NULL),m_pNextBlock(NULL){}
	int			m_nMemLen;
	MemBlock*	m_pPreBlock;
	MemBlock*	m_pNextBlock;
};

class MemTypeAllocator
{
public:
	MemTypeAllocator(int nBlockType): m_nBlockType(nBlockType),m_nTotalBlockNum(0),m_nFreeBlockNum(0),m_pBeginBlock(NULL),m_pEndBlock(NULL){};
	~MemTypeAllocator(){};
	void* AllocMemory();
	void FreeMemory(void* pFreeMemory);
	int GetTotalBlockCount();
	int GetFreeBlockCount();
	int GetBlockType();
	void DumpNode();
private:
	static const int BlockMax = 100;
	int			m_nBlockType;
	long		m_nTotalBlockNum;
	long		m_nFreeBlockNum;
	MemBlock*	m_pBeginBlock;
	MemBlock*	m_pEndBlock;
};
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void* MemTypeAllocator::AllocMemory()
{
	void* pRet = NULL;

	if (m_pEndBlock != NULL)
	{
		assert(m_pEndBlock->m_pNextBlock == NULL);

		MemBlock* pTond = m_pEndBlock;

		pRet = (char*)m_pEndBlock + sizeof(MemBlock);

		m_pEndBlock = m_pEndBlock->m_pPreBlock;

		if (m_pEndBlock != NULL)
		{
			m_pEndBlock->m_pNextBlock = pTond->m_pNextBlock;
		}
		else
		{
			m_pBeginBlock = NULL;
		}
		pTond->m_pPreBlock = NULL;

		m_nFreeBlockNum--;
	}
	else
	{
		void* pMemory = malloc(m_nBlockType + sizeof(MemBlock));

		MemBlock* pNewBlock = (MemBlock*)pMemory;

		assert(pNewBlock != NULL);

		pNewBlock->m_nMemLen = m_nBlockType;

		pNewBlock->m_pNextBlock = NULL;

		pNewBlock->m_pPreBlock = NULL;

		pRet = (char*)pNewBlock + sizeof(MemBlock);

		m_nTotalBlockNum++;
	}
	return pRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemTypeAllocator::FreeMemory(void* pFreeMemory)
{
	assert(pFreeMemory != NULL);

	if (pFreeMemory == NULL)
	{
		return; 
	}
	MemBlock* pMemBlock = (MemBlock*)((char*)pFreeMemory - sizeof(MemBlock));

	memset(pFreeMemory,0,pMemBlock->m_nMemLen);

	if (m_nFreeBlockNum > BlockMax)
	{
		free(pMemBlock);

		pMemBlock = NULL;

		m_nTotalBlockNum--;
	}
	else
	{
		if (NULL == m_pBeginBlock)
		{
			m_pBeginBlock = pMemBlock;

			m_pEndBlock = pMemBlock;
		}
		else
		{
			pMemBlock->m_pNextBlock = m_pBeginBlock;

			m_pBeginBlock->m_pPreBlock = pMemBlock;

			m_pBeginBlock = pMemBlock;
		}
		m_nFreeBlockNum++;
	}
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemTypeAllocator::GetTotalBlockCount()
{
	return m_nTotalBlockNum;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemTypeAllocator::GetFreeBlockCount()
{
	return m_nFreeBlockNum;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemTypeAllocator::GetBlockType()
{
	return m_nBlockType;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemTypeAllocator::DumpNode()
{
	MemBlock* pTond = m_pBeginBlock;

	while (pTond)
	{
		//TRACE("Node:%p->",pTond);

		pTond = pTond->m_pNextBlock;
	}
	//TRACE("NULL\n");

	printf("BlockType : %d TotalBlockNum : %d FreeBlockNum : %d  UserBlockNum : %d \n",m_nBlockType,m_nTotalBlockNum,m_nFreeBlockNum,m_nTotalBlockNum - m_nFreeBlockNum);
}

class MemoryPool
{
public:
	~MemoryPool(){};

	void InitMemoryPool();
	void UnInitMemoryPool();

	void* Malloc(int nSize);
	void Free(void* pMemory);

	long GetTotalMemorySize();
	long GetFreeMemorySize();

	int GetTotalBlockCountForType(int nType);
	int GetFreeBlockCountForType(int nType);

	int GetBlockTypeCount();

	void DunpMemoryPool();

	int FindType(int nElement);

	template<class T>
	T* New();

	template<class T>
	void Delete(T* pObj);
private:
	MemoryPool(){};

	DECLARE_SINGLETON_CLASS(MemoryPool)
protected:
	int FitSize(int nSize);

	int FindElement(int nType);

private:
	static const int			MemTypeMax = 17;

	MemTypeAllocator*			m_arrayAllocator[MemTypeMax]; 

	CRITICAL_SECTION			m_cs;
};
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::FindElement(int nType)
{
	int nRet = -1;

	if (nType > 0 && nType <= Mem_1m_Bit_Size)
	{
		nRet = (int)(log10(nType*1.0)/log10(2.0) - 4);
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::FindType(int nElement)
{
	int nRet = -1;

	if (nElement >= 0 && nElement < MemTypeMax)
	{
		nRet = (int)pow(2.0,nElement + 4);
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemoryPool::InitMemoryPool()
{
	InitializeCriticalSection(&m_cs);

	for (int i = 0;i < MemTypeMax;i++)
	{
		int nType = FindType(i);

		m_arrayAllocator[i] = new MemTypeAllocator(nType);
	}
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemoryPool::UnInitMemoryPool()
{
	DeleteCriticalSection(&m_cs);

	for (int i = 0;i < MemTypeMax;i++)
	{
		delete m_arrayAllocator[i];

		m_arrayAllocator[i] = NULL;
	}
}

/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::FitSize(int nSize)
{
	int nRet = -1;

	if (nSize <= 0)
	{
		return nRet;
	}
	else if (nSize >0 && nSize <= Mem_16_Bit_Size)
	{ 
		nRet = Mem_16_Bit_Size;
	}
	else if (nSize <= Mem_32_Bit_Size) 
	{ 
		nRet = Mem_32_Bit_Size;
	}
	else if (nSize <= Mem_64_Bit_Size) 
	{ 
		nRet = Mem_64_Bit_Size; 
	}
	else if (nSize <= Mem_128_Bit_Size) 
	{ 
		nRet = Mem_128_Bit_Size; 
	}
	else if (nSize <= Mem_256_Bit_Size) 
	{ 
		nRet = Mem_256_Bit_Size; 
	}
	else if (nSize <= Mem_512_Bit_Size) 
	{ 
		nRet = Mem_512_Bit_Size; 
	}
	else if (nSize <= Mem_1k_Bit_Size) 
	{ 
		nRet = Mem_1k_Bit_Size; 
	}
	else if (nSize <= Mem_2k_Bit_Size) 
	{ 
		nRet = Mem_2k_Bit_Size; 
	} 
	else if (nSize <= Mem_4k_Bit_Size) 
	{ 
		nRet = Mem_4k_Bit_Size; 
	} 
	else if (nSize <= Mem_8k_Bit_Size) 
	{ 
		nRet = Mem_8k_Bit_Size; 
	} 
	else if (nSize <= Mem_16k_Bit_Size) 
	{  
		nRet = Mem_16k_Bit_Size;
	} 
	else if (nSize <= Mem_32k_Bit_Size)
	{  
		nRet = Mem_32k_Bit_Size;
	} 
	else if (nSize <= Mem_64k_Bit_Size)
	{  
		nRet = Mem_64k_Bit_Size;
	} 
	else if (nSize <= Mem_128k_Bit_Size)
	{  
		nRet = Mem_128k_Bit_Size;
	} 
	else if (nSize <= Mem_256k_Bit_Size)
	{  
		nRet = Mem_256k_Bit_Size;
	} 
	else if (nSize <= Mem_512k_Bit_Size) 
	{ 
		nRet = Mem_512k_Bit_Size; 
	} 
	else if (nSize <= Mem_1m_Bit_Size) 
	{ 
		nRet = Mem_1m_Bit_Size;
	} 
	else
	{  
		//如果申请大于1M的内存直接申请
		//....
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void* MemoryPool::Malloc(int nSize)
{
	void* pRet = NULL;

	int nType = FitSize(nSize);

	if (nType > 0)
	{
		int nElement = FindElement(nType);

		if (nElement >= 0)
		{
			EnterCriticalSection(&m_cs);

			pRet = m_arrayAllocator[nElement]->AllocMemory();

			LeaveCriticalSection(&m_cs);
		}
		else
		{
			assert(0);
		}
	}
	else if (nType = 0)
	{
		return pRet;
	}
	else
	{
		void* pTond = malloc(nSize + sizeof(MemBlock));

		assert(pTond != NULL);

		*(int*)pTond = -1;	//自行分配内存

		pRet = (char*)pTond + sizeof(MemBlock);
	}
	return pRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemoryPool::Free(void* pMemory)
{
	if (pMemory == NULL)
	{
		return;
	}
	int* pType = (int*)((char*)pMemory - sizeof(MemBlock));

	assert(pType != NULL);

	int nElement = FindElement(*pType);

	if (nElement >= 0)
	{
		EnterCriticalSection(&m_cs);

		m_arrayAllocator[nElement]->FreeMemory(pMemory);

		LeaveCriticalSection(&m_cs);
	}
	else
	{
		void* pTond = (char*)pMemory - sizeof(MemBlock);

		free(pTond);
	}
	return;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline long MemoryPool::GetTotalMemorySize()
{
	long lRet = -1;

	for (int i = 0;i < MemTypeMax;i++)
	{
		lRet += m_arrayAllocator[i]->GetTotalBlockCount()*m_arrayAllocator[i]->GetBlockType();
	}
	return lRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline long MemoryPool::GetFreeMemorySize()
{
	long lRet = -1;

	for (int i = 0;i < MemTypeMax;i++)
	{
		lRet += m_arrayAllocator[i]->GetFreeBlockCount()*m_arrayAllocator[i]->GetBlockType();
	}
	return lRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::GetTotalBlockCountForType(int nType)
{
	int nRet = -1;

	int nElement = FindElement(nType);

	if (nElement >= 0)
	{
		nRet = m_arrayAllocator[nElement]->GetTotalBlockCount();
	}
	else
	{
		nRet = 0;
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::GetFreeBlockCountForType(int nType)
{
	int nRet = -1;
	
	int nElement = FindElement(nType);

	if (nElement >= 0)
	{
		nRet = m_arrayAllocator[nElement]->GetFreeBlockCount();
	}
	else
	{
		nRet = 0;
	}
	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::GetBlockTypeCount()
{
	int nRet = -1;

	nRet = MemTypeMax;

	return nRet;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemoryPool::DunpMemoryPool()
{
	for (int i = 0;i < MemTypeMax;i++)
	{
		if (m_arrayAllocator[i]->GetTotalBlockCount() > 0)
		{
			m_arrayAllocator[i]->DumpNode();
		}
	}
}
template<class T>
inline T* MemoryPool::New()
{
	T* pRet = NULL;

	int nSize = sizeof(T);

	pRet = (T*)Malloc(nSize);

	if (NULL == pRet)
	{
		return pRet;
	}
	new(pRet) T();

	return pRet;
}

template<class T>
inline void MemoryPool::Delete(T* pObj)
{
	if (NULL == pObj)
	{
		return;
	}
	pObj->~T();

	Free(pObj);

	return;
}

#endif