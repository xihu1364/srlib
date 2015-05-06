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
#include "Singleton.h"

enum AllocatorIdx
{
	Mem_Block_Base        = 4,

	Allocator_Idx_Invalid = -1,
	Allocator_Idx_0       = 0,      // 1 << (0+Mem_Block_Base)    // 16,
	Allocator_Idx_1       = 1,      // 1 << (1+Mem_Block_Base)    // 32,
	Allocator_Idx_2       = 2,      // 1 << (2+Mem_Block_Base)    // 64,
	Allocator_Idx_3       = 3,      // 1 << (3+Mem_Block_Base)    // 128,
	Allocator_Idx_4       = 4,      // 1 << (4+Mem_Block_Base)    // 256,
	Allocator_Idx_5       = 5,      // 1 << (5+Mem_Block_Base)    // 512,
	Allocator_Idx_6       = 6,      // 1 << (6+Mem_Block_Base)    // 1k,
	Allocator_Idx_7       = 7,      // 1 << (7+Mem_Block_Base)    // 2k,
	Allocator_Idx_8       = 8,      // 1 << (8+Mem_Block_Base)    // 4k,
	Allocator_Idx_9       = 9,      // 1 << (9+Mem_Block_Base)    // 8k,
	Allocator_Idx_10      = 10,     // 1 << (10+Mem_Block_Base)   // 16k,
	Allocator_Idx_11      = 11,     // 1 << (11+Mem_Block_Base)   // 32k,
	Allocator_Idx_12      = 12,     // 1 << (12+Mem_Block_Base)   // 64k,
	Allocator_Idx_13      = 13,     // 1 << (13+Mem_Block_Base)   // 128k,
	Allocator_Idx_14      = 14,     // 1 << (14+Mem_Block_Base)   // 256k,
	Allocator_Idx_15      = 15,     // 1 << (15+Mem_Block_Base)   // 512k,
	Allocator_Idx_16      = 16,     // 1 << (16+Mem_Block_Base)   // 1m,
	Allocator_Idx_17      = 17,     // 1 << (17+Mem_Block_Base)   // 2m,
	Allocator_Idx_18      = 18,     // 1 << (18+Mem_Block_Base)   // 4m,

	Allocator_Idx_Max,
};

enum MemType
{
	Mem_User_Byte_Size_Type = -1,   // 0xFFFFFFFF,

	Mem_16_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_0),       // 16,
	Mem_32_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_1),       // 32,
	Mem_64_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_2),       // 64,
	Mem_128_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_3),       // 128,
	Mem_256_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_4),       // 256,
	Mem_512_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_5),       // 512,
	Mem_1k_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_6),       // 1k,
	Mem_2k_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_7),       // 2k,
	Mem_4k_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_8),       // 4k,
	Mem_8k_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_9),       // 8k,
	Mem_16k_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_10),      // 16k,
	Mem_32k_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_11),      // 32k,
	Mem_64k_Byte_Size_Type  = 1 << (Mem_Block_Base + Allocator_Idx_12),      // 64k,
	Mem_128k_Byte_Size_Type = 1 << (Mem_Block_Base + Allocator_Idx_13),      // 128k,
	Mem_256k_Byte_Size_Type = 1 << (Mem_Block_Base + Allocator_Idx_14),      // 256k,
	Mem_512k_Byte_Size_Type = 1 << (Mem_Block_Base + Allocator_Idx_15),      // 512k,
	Mem_1m_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_16),      // 1m,
	Mem_2m_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_17),      // 2m,
	Mem_4m_Byte_Size_Type   = 1 << (Mem_Block_Base + Allocator_Idx_18),      // 4m,

	Mem_Min_Byte_Size_Type  = Mem_16_Byte_Size_Type,
	Mem_Max_Byte_Size_Type  = Mem_4m_Byte_Size_Type,
};

struct MemBlock
{
	explicit MemBlock(int nBlockSize) : m_nBlockSizeType(nBlockSize),m_pPreBlock(NULL),m_pNextBlock(NULL){}
	int         m_nBlockSizeType;  // refer MemType
	MemBlock*	m_pPreBlock;
	MemBlock*	m_pNextBlock;
};

class MemTypeAllocator
{
public:
	explicit MemTypeAllocator(int nAllocBound);
	~MemTypeAllocator();
	void* AllocMemory();
	void FreeMemory(void* pFreeMemory);
	int GetTotalBlockCount();
	int GetFreeBlockCount();
	int GetBlockBound();
	void DumpNode();
private:
	// max free block count
	enum eMaxFreeSlots{ MaxFreeSlots = 20, };
	size_t     m_nBlockBound;        // allocate boundary [16B 32B ... 4MB]
	long       m_nTotalBlockNum;     // total block number
	long       m_nFreeBlockNum;      // available block number
	MemBlock*  m_pBeginBlock;
	MemBlock*  m_pEndBlock;
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

	if ( NULL != m_pEndBlock )
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

		--m_nFreeBlockNum;
	}
	else
	{
		void* pMemory = malloc(m_nBlockBound + sizeof(MemBlock));

		MemBlock* pNewBlock = (MemBlock*)pMemory;

		assert(pNewBlock != NULL);

		pNewBlock->m_nBlockSizeType = m_nBlockBound;

		pNewBlock->m_pNextBlock = NULL;

		pNewBlock->m_pPreBlock = NULL;

		pRet = (char*)pNewBlock + sizeof(MemBlock);

		++m_nTotalBlockNum;
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
	if (NULL == pFreeMemory)
	{
		assert(!(NULL == pFreeMemory));
		return; 
	}

	MemBlock* pMemBlock = (MemBlock*)((char*)pFreeMemory - sizeof(MemBlock));

	if (m_nFreeBlockNum > MaxFreeSlots)
	{
		free(pMemBlock);

		pMemBlock = NULL;

		--m_nTotalBlockNum;
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
			assert(NULL == m_pBeginBlock->m_pPreBlock);

			pMemBlock->m_pNextBlock = m_pBeginBlock;

			m_pBeginBlock->m_pPreBlock = pMemBlock;

			m_pBeginBlock = pMemBlock;
		}
		++m_nFreeBlockNum;
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
inline int MemTypeAllocator::GetBlockBound()
{
	return m_nBlockBound;
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

	printf("BlockType : %d TotalBlockNum : %d FreeBlockNum : %d  UserBlockNum : %d \n",m_nBlockBound,m_nTotalBlockNum,m_nFreeBlockNum,m_nTotalBlockNum - m_nFreeBlockNum);
}

class MemoryPool
{
public:
	~MemoryPool(){ }

	void InitMemoryPool();
	void UnInitMemoryPool();

	void* Malloc(int nSize);
	void Free(void* pMemory);

	template<class T> T* New();
	template<class T> void Delete(T* pObj);

	long GetTotalMemorySize();
	long GetFreeMemorySize();

	int GetTotalBlockCountAtBound(int nType);
	int GetFreeBlockCountAtBound(int nType);

	// how many kinds of block size
	int GetBlockTypeCount();

	void DumpMemoryPool();

	// adjust nSize to allocate boundary (16B, 32B ...)
	// return -1 means user allocate boundary
	int UpSize2Boundary(int nSize);

	// get type by allocate boundary . nBlockBound [16B 32B ... 4MB] type [0,Allocator_Idx_Max) 
	// return -1 means user allocate boundary
	int BlockBound2Slot(int nBlockBound);

	// get allocate boundary by type. nType [0,Allocator_Idx_Max) bound [16B,1MB]
	// return -1 means user allocate boundary
	int Slot2BlockBound(int nType);

private:
	MemoryPool(){};

	DECLARE_SINGLETON_CLASS(MemoryPool)

private:
	// allocator for each block size
	MemTypeAllocator*  m_arrayAllocator[Allocator_Idx_Max];
	CRITICAL_SECTION   m_cs;
};
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline int MemoryPool::UpSize2Boundary(int nSize)
{
	assert(0 != nSize);

	int nRet = Mem_User_Byte_Size_Type;
	if ( 0 < nSize && nSize < Mem_Min_Byte_Size_Type )
	{
		nRet = Mem_Min_Byte_Size_Type;
	}
	else if (nSize <= Mem_Max_Byte_Size_Type) 
	{
		// nSize is some boundary just return
		if ( 0 == (nSize & (nSize-1)) )
		{
			nRet = nSize;
		}
		else
		{
			// div 16
			nSize = nSize >> Mem_Block_Base;

			// find the highest bit
			int nHighBit = 0;
			while ( nSize && (nSize >>= 1) )
				++nHighBit;

			// left shift the highest bit, nRet is double nSize
			nRet = 1 << (Mem_Block_Base+nHighBit+1);
		}
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
inline int MemoryPool::BlockBound2Slot(int nBlockBound)
{
	int nRet = Allocator_Idx_Invalid;
	if (0 < nBlockBound && nBlockBound <= Mem_Max_Byte_Size_Type)
	{
		nBlockBound = nBlockBound >> Mem_Block_Base;
		nRet = 0;
		while ( nBlockBound && (nBlockBound >>= 1) )
			++nRet;
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
inline int MemoryPool::Slot2BlockBound(int nIdx)
{
	int nRet = Mem_User_Byte_Size_Type;

	if (nIdx >= Allocator_Idx_0 && nIdx < Allocator_Idx_Max)
	{
		nRet = 1 << (nIdx + Mem_Block_Base);
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

	for (int i = 0;i < Allocator_Idx_Max; ++i)
	{
		int nType = Slot2BlockBound(i);
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

	for (int i = 0;i < Allocator_Idx_Max; ++i)
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
inline void* MemoryPool::Malloc(int nSize)
{
	void* pRet = NULL;

	int nBlockBoundary = UpSize2Boundary(nSize);
	if ( nBlockBoundary > 0 )
	{
		int nElement = BlockBound2Slot(nBlockBoundary);
		if (nElement > 0)
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
	else
	{
		void* pAllocated = malloc(nSize + sizeof(MemBlock));
		assert(pAllocated != NULL);

		MemBlock* pMemBlock = (MemBlock*)pAllocated;
		pMemBlock->m_nBlockSizeType = Mem_User_Byte_Size_Type;

		pRet = (char*)pAllocated + sizeof(MemBlock);
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
	MemBlock* pMemBlock = (MemBlock*)((char*)pMemory - sizeof(MemBlock));
	int nType = pMemBlock->m_nBlockSizeType;

	int nElement = BlockBound2Slot(nType);
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

	for (int i = 0;i < Allocator_Idx_Max; ++i)
	{
		lRet += m_arrayAllocator[i]->GetTotalBlockCount()*m_arrayAllocator[i]->GetBlockBound();
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

	for (int i = 0;i < Allocator_Idx_Max; ++i)
	{
		lRet += m_arrayAllocator[i]->GetFreeBlockCount()*m_arrayAllocator[i]->GetBlockBound();
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
inline int MemoryPool::GetTotalBlockCountAtBound(int nBlockBound)
{
	int nRet = -1;

	int nElement = BlockBound2Slot(nBlockBound);
	if (nElement >= 0)
	{
		nRet = m_arrayAllocator[nElement]->GetTotalBlockCount();
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
inline int MemoryPool::GetFreeBlockCountAtBound(int nBlockBound)
{
	int nRet = -1;

	int nElement = BlockBound2Slot(nBlockBound);
	if (nElement >= 0)
	{
		nRet = m_arrayAllocator[nElement]->GetFreeBlockCount();
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
	return Allocator_Idx_Max;
}
/************************************************************************
* Function : NA
* Author   : xihu
* Input    : 
* Output   : 
* Info     : 2014/04/03
************************************************************************/
inline void MemoryPool::DumpMemoryPool()
{
	for (int i = 0;i < Allocator_Idx_Max; ++i)
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