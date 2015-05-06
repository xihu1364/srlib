/************************************************************************
* FileName : ringqueue.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/15
************************************************************************/

#ifndef __RINGQUEUE_H__
#define __RINGQUEUE_H__

/************************************************************************/
/* dumpmem                                                              */
/************************************************************************/
#include <stdio.h>
#include <assert.h>

#ifndef jimic_assert
#define jimic_assert    assert
#endif // jimic_assert

/**
 * macro for address aligned to n bytes
 */
#define JIMI_ALIGNED_TO(n, alignment)   \
    (((n) + ((alignment) - 1)) & ~(size_t)((alignment) - 1))

#define JIMI_ADDR_ALIGNED_TO(p, alignment)   \
    ((void *)((((size_t)(void *)(p)) + ((alignment) - 1)) & ~(size_t)((alignment) - 1)))

#define JIMI_ADDR_ALIGNED_DOWNTO(p, alignment)   \
    ((void *)(((size_t)(void *)(p)) & ~(size_t)((alignment) - 1)))

#define JIMI_PVOID_INC(p, n)    ((void *)((size_t)(p) + 1))
#define JIMI_PVOID_DEC(p, n)    ((void *)((size_t)(p) - 1))

#define JIMI_PVOID_ADD(p, n)    ((void *)((size_t)(p) + (n)))
#define JIMI_PVOID_SUB(p, n)    ((void *)((size_t)(p) - (n)))

inline void dump_mem(void *p, size_t size, bool alignedTo /* = false */,
              unsigned int alignment /* = 16 */,
              unsigned int extraHead /* = 0 */,
              unsigned int extraTail /* = 0 */)
{
    // display memory dump
    size_t i, j;
    size_t addr;
    size_t lineTotal;
    unsigned char *cur;
    void *startAddr, *endAddr;

    jimic_assert(p != NULL);
    //jimic_assert(p != (void *)0xCCCCCCCC);

    // Whether the start address aligned to alignment?
    if (alignedTo) 
	{
        startAddr = (void *)JIMI_ADDR_ALIGNED_DOWNTO(p, alignment);
        endAddr   = (void *)JIMI_ADDR_ALIGNED_TO((size_t)p + size, alignment);
    }
    else 
	{
        startAddr = p;
        endAddr   = (void *)((size_t)p + size);
    }

    // Extend the extra lines at head or tail
    if (extraHead > 0)
        startAddr = JIMI_PVOID_SUB(startAddr, alignment * extraHead);
    if (extraTail > 0)
        endAddr   = JIMI_PVOID_ADD(endAddr, alignment * extraTail);

    // Get the total of display lines
    lineTotal = ((size_t)endAddr - (size_t)startAddr + (alignment - 1)) / alignment;

    printf("--------------------------------------------------------------\n");
    printf("  Addr = 0x%016p, Size = %u bytes\n", p, (unsigned int)size);
    printf("--------------------------------------------------------------\n");
    printf("\n");

    if (p == NULL || p == (void *)0xCCCCCCCC) 
	{
        printf("  Can not read the data from the address.\n");
        printf("\n");
        return;
    }

    printf("  Address    0  1  2  3  4  5  6  7 |  8  9  A  B  C  D  E  F\n");
    printf("--------------------------------------------------------------\n");
    printf("\n");

    addr = (size_t)startAddr;
    for (i = 0; i < lineTotal; ++i) 
	{
        // display format preview
        //printf("  %08X  00 01 02 03 04 05 06 07   08 09 0A 0B 0C 0D 0E 0F\n", (unsigned int)addr);
        printf("  %08X  ", (unsigned int)addr);
        cur = (unsigned char *)addr;
        for (j = 0; j < alignment; ++j) 
		{
            printf("%02X", (unsigned int)(*cur));
            if (j < alignment - 1) 
			{
                if ((j & 7) != 7)
                    printf(" ");
                else
                    printf("   ");
            }
            cur++;
        }
        printf("    ");
        cur = (unsigned char *)addr;
        for (j = 0; j < alignment; ++j) 
		{
            if (*cur >= 128)
                printf("?");
            else if (*cur >= 32)
                printf("%c", (unsigned char)(*cur));
            else
                printf(".");
            cur++;
        }
        printf("\n");
        addr += alignment;
    }

    printf("\n");
}
/************************************************************************/


#include <intrin.h>
#include <cstdint>

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

#ifndef JIMI_MAX
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

#define jimi_b2(x)              (        (x) | (        (x) >>  1))
#define jimi_b4(x)              ( jimi_b2(x) | ( jimi_b2(x) >>  2))
#define jimi_b8(x)              ( jimi_b4(x) | ( jimi_b4(x) >>  4))
#define jimi_b16(x)             ( jimi_b8(x) | ( jimi_b8(x) >>  8))
#define jimi_b32(x)             (jimi_b16(x) | (jimi_b16(x) >> 16))
#define jimi_b64(x)             (jimi_b32(x) | (jimi_b32(x) >> 32))

#define jimi_next_power_of_2(x)     (jimi_b32((x) - 1) + 1)

#define MUTEX_MAX_SPIN_COUNT    1
#define SPIN_YIELD_THRESHOLD    1

struct spin_mutex_t
{
	volatile char padding1[CACHE_LINE_SIZE];
	volatile uint32_t locked;
	volatile char padding2[CACHE_LINE_SIZE - 1 * sizeof(uint32_t)];
	volatile uint32_t spin_counter;
	volatile uint32_t recurse_counter;
	volatile uint32_t thread_id;
	volatile uint32_t reserve;
	volatile char padding3[CACHE_LINE_SIZE - 4 * sizeof(uint32_t)];
};

typedef struct spin_mutex_t spin_mutex_t;


struct RingQueueHead
{
	volatile uint32_t head;
	char padding1[CACHE_LINE_SIZE - sizeof(uint32_t)];

	volatile uint32_t tail;
	char padding2[CACHE_LINE_SIZE - sizeof(uint32_t)];
};

template <typename T, uint32_t Capacity>
class RingQueueCore
{
public:
	typedef T *         item_type;

public:
	static const bool kIsAllocOnHeap = true;

public:
	RingQueueHead       info;
	volatile item_type *queue;
};

template <typename T, uint32_t Capacity = 16U,typename CoreTy = RingQueueCore<T, Capacity> >
class RingQueueBase
{
public:
	typedef uint32_t                    size_type;
	typedef uint32_t                    index_type;
	typedef T *                         value_type;
	typedef typename CoreTy::item_type  item_type;
	typedef CoreTy                      core_type;
	typedef T *                         pointer;
	typedef const T *                   const_pointer;
	typedef T &                         reference;
	typedef const T &                   const_reference;

public:
	static const size_type  kCapacity = (size_type)JIMI_MAX(jimi_next_power_of_2(Capacity), 2);
	static const index_type kMask     = (index_type)(kCapacity - 1);

public:
	RingQueueBase(bool bInitHead = false);
	~RingQueueBase();
public:
	void dump_info();
	void dump_detail();

	index_type mask() const      { return kMask;     };
	size_type capacity() const   { return kCapacity; };
	size_type length() const     { return sizes();   };
	size_type sizes() const;

	void init(bool bInitHead = false);
	int PushBack(T * item);
	T * PopFront();
protected:
	core_type       core;
	spin_mutex_t    spin_mutex;
};
template <typename T, uint32_t Capacity, typename CoreTy>
RingQueueBase<T, Capacity, CoreTy>::RingQueueBase(bool bInitHead  /* = false */)
{
	//printf("RingQueueBase::RingQueueBase();\n\n");

	init(bInitHead);
}
template <typename T, uint32_t Capacity, typename CoreTy>
RingQueueBase<T, Capacity, CoreTy>::~RingQueueBase()
{
	// Do nothing!
	_ReadWriteBarrier();

	spin_mutex.locked = 0;
}
template <typename T, uint32_t Capacity, typename CoreTy>
inline void RingQueueBase<T, Capacity, CoreTy>::init(bool bInitHead /* = false */)
{
	//printf("RingQueueBase::init();\n\n");

	if (!bInitHead) 
	{
		core.info.head = 0;
		core.info.tail = 0;
	}
	else 
	{
		memset((void *)&core.info, 0, sizeof(core.info));
	}
	_ReadWriteBarrier();

	spin_mutex.locked = 0;
	spin_mutex.spin_counter = MUTEX_MAX_SPIN_COUNT;
	spin_mutex.recurse_counter = 0;
	spin_mutex.thread_id = 0;
	spin_mutex.reserve = 0;
}

template <typename T, uint32_t Capacity, typename CoreTy>
void RingQueueBase<T, Capacity, CoreTy>::dump_info()
{
	//ReleaseUtils::dump(&core.info, sizeof(core.info));
	dump_mem(&core.info, sizeof(core.info), false, 16, 0, 0);
}
template <typename T, uint32_t Capacity, typename CoreTy>
void RingQueueBase<T, Capacity, CoreTy>::dump_detail()
{
#if 0
	printf("---------------------------------------------------------\n");
	printf("RingQueueBase.p.head = %u\nRingQueueBase.p.tail = %u\n\n", core.info.p.head, core.info.p.tail);
	printf("RingQueueBase.c.head = %u\nRingQueueBase.c.tail = %u\n",   core.info.c.head, core.info.c.tail);
	printf("---------------------------------------------------------\n\n");
#else
	printf("RingQueueBase: (head = %u, tail = %u)\n",core.info.head, core.info.tail);
#endif
}
template <typename T, uint32_t Capacity, typename CoreTy>
inline typename RingQueueBase<T, Capacity, CoreTy>::size_type RingQueueBase<T, Capacity, CoreTy>::sizes() const
{
	index_type head, tail;

	_ReadWriteBarrier();

	head = core.info.head;

	tail = core.info.tail;

	return (size_type)/*(*/(head - tail)/* <= kMask) ? (head - tail) : (size_type)-1*/;
}

template <typename T, uint32_t Capacity, typename CoreTy>
inline int RingQueueBase<T, Capacity, CoreTy>::PushBack(T * item)
{
	index_type head, tail, next;
	int32_t pause_cnt;
	uint32_t loop_count, yield_cnt, spin_count;
	static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;  

	_ReadWriteBarrier();   

	if (InterlockedExchange((volatile LONG *)(&spin_mutex.locked), (LONG)(1U)) != 0U) 
	{
		loop_count = 0;
		spin_count = 1;
		do 
		{
			if (loop_count < YIELD_THRESHOLD) 
			{
				for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) 
				{
					_mm_pause();      
				}
				spin_count *= 2;
			}
			else 
			{
				yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
				if ((yield_cnt & 3) == 3) 
				{
					Sleep(0);
				}
				else 
				{
					if (!SwitchToThread()) 
					{
						Sleep(0);
					}
				}
#else
				if ((yield_cnt & 63) == 63) 
				{
					Sleep(1);
				}
				else if ((yield_cnt & 3) == 3) 
				{
					Sleep(0);
				}
				else 
				{
					if (!SwitchToThread()) 
					{
						Sleep(0);
					}
				}
#endif
			}
			loop_count++;
		} while (InterlockedCompareExchange(&spin_mutex.locked,1U,0U) != 0U);
	}
	head = core.info.head;
	tail = core.info.tail;
	if ((head - tail) > kMask) 
	{
		_ReadWriteBarrier();  

		spin_mutex.locked = 0;

		return -1;
	}
	next = head + 1;
	core.info.head = next;

	core.queue[head & kMask] = item;

	_ReadWriteBarrier(); 

	spin_mutex.locked = 0;      

	return 0;
}
template <typename T, uint32_t Capacity, typename CoreTy>
inline T * RingQueueBase<T, Capacity, CoreTy>::PopFront()
{
    index_type head, tail, next;
    value_type item;
    int32_t pause_cnt;
    uint32_t loop_count, yield_cnt, spin_count;
    static const uint32_t YIELD_THRESHOLD = SPIN_YIELD_THRESHOLD;

    _ReadWriteBarrier(); 

    if (InterlockedExchange(&spin_mutex.locked, 1U) != 0U) 
	{
        loop_count = 0;
        spin_count = 1;
        do 
		{
            if (loop_count < YIELD_THRESHOLD) 
			{
                for (pause_cnt = spin_count; pause_cnt > 0; --pause_cnt) 
				{
                    _mm_pause();
                }
                spin_count *= 2;
            }
            else 
			{
                yield_cnt = loop_count - YIELD_THRESHOLD;
#if defined(__MINGW32__) || defined(__CYGWIN__)
                if ((yield_cnt & 3) == 3) 
				{
                    Sleep(0);
                }
                else 
				{
                    if (!SwitchToThread()) 
					{
                        Sleep(0);
                    }
                }
#else
                if ((yield_cnt & 63) == 63) 
				{
                    Sleep(1);
                }
                else if ((yield_cnt & 3) == 3)
				{
                    Sleep(0);
                }
                else 
				{
                    if (!SwitchToThread()) 
					{
                        Sleep(0);
                    }
                }
#endif
            }
            loop_count++;
        } while (InterlockedCompareExchange(&spin_mutex.locked,1U,0U) != 0U);
    }
    head = core.info.head;
    tail = core.info.tail;

    if ((tail == head) || (tail > head && (head - tail) > kMask)) 
	{
        _ReadWriteBarrier(); 

        spin_mutex.locked = 0;

        return (value_type)NULL;
    }
    next = tail + 1;
    core.info.tail = next;

    item = core.queue[tail & kMask];

    _ReadWriteBarrier(); 

    spin_mutex.locked = 0;

    return item;
}

template <typename T, uint32_t Capacity = 1024U>
class RingQueue : public RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >
{
public:
	typedef uint32_t                    size_type;
	typedef uint32_t                    index_type;
	typedef T *                         value_type;
	typedef T *                         pointer;
	typedef const T *                   const_pointer;
	typedef T &                         reference;
	typedef const T &                   const_reference;

	typedef RingQueueCore<T, Capacity>   core_type;

	static const size_type kCapacity = RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >::kCapacity;

public:
	RingQueue(bool bFillQueue = true, bool bInitHead = false);
	~RingQueue();

public:
	void dump_detail();

protected:
	void init_queue(bool bFillQueue = true);
};

template <typename T, uint32_t Capacity>
RingQueue<T, Capacity>::RingQueue(bool bFillQueue /* = true */,
	bool bInitHead  /* = false */)
	: RingQueueBase<T, Capacity, RingQueueCore<T, Capacity> >(bInitHead)
{
	//printf("RingQueue::RingQueue();\n\n");

	init_queue(bFillQueue);
}

template <typename T, uint32_t Capacity>
RingQueue<T, Capacity>::~RingQueue()
{
	// If the queue is allocated on system heap, release them.
	if (RingQueueCore<T, Capacity>::kIsAllocOnHeap) 
	{
		delete [] this->core.queue;
		this->core.queue = NULL;
	}
}

template <typename T, uint32_t Capacity>
inline void RingQueue<T, Capacity>::init_queue(bool bFillQueue /* = true */)
{
	//printf("RingQueue::init_queue();\n\n");

	value_type *newData = new T *[kCapacity];
	if (newData != NULL) 
	{
		this->core.queue = newData;
		if (bFillQueue) 
		{
			memset((void *)this->core.queue, 0, sizeof(value_type) * kCapacity);
		}
	}
}

template <typename T, uint32_t Capacity>
void RingQueue<T, Capacity>::dump_detail()
{
	printf("RingQueue: (head = %u, tail = %u)\n",
		this->core.info.head, this->core.info.tail);
}

#endif