
/*******************************************************************
** 文件名:	ringQueue.h
** 创建人:	饶建超
** 日  期:	2015-06-27
** 版  本:	1.0
** 描  述:	自旋锁的线程安全队列
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "spinLock.h"
#include "atomic.h"

//#define SHOW_DEBUG_RINGQUEUE

struct RingQueueHead
{
public:
	RingQueueHead():head(0),tail(0)	{}
	volatile size_t head;
	volatile size_t tail;
};

template <typename T>
class RingQueue
{
#define LIMIT_MIN_QUEUE_SIZE			128			//128
#define LIMIT_MAX_QUEUE_SIZE			4194304		//4M
#define LIMIT_RECOV_TIME_MS				1000		//ms

public:
	RingQueue(size_t initNum = 1024U)
	{
		initNum = min(initNum, LIMIT_MAX_QUEUE_SIZE);
		initNum = max(initNum, LIMIT_MIN_QUEUE_SIZE);
		initNum = GetMinPowOfNum2(initNum);
		nInitCapacity = initNum;
		nCapacity	= initNum;
		nMask		= nCapacity-1;
		queue		= new T[initNum];
		timeNote	= GetTickCount();
	};
	~RingQueue(){};

public:
	void tryClear();
	size_t mask() const       { return nMask;     };
	size_t capacity() const   { return nCapacity; };
	size_t length() const     { return sizes();   };
	size_t sizes() const;
	bool   resize(size_t nNewSize);

	bool spin_push(T item);
	bool spin_pop(T& item);

	inline size_t GetMinPowOfNum2(int nNum);

protected:
	size_t				nInitCapacity;
	volatile size_t		nCapacity;
	volatile size_t		nMask;
	RingQueueHead		info;
	T*					queue;
	spin_mutex_s		spinLock;
	int					timeNote;
};

template <typename T>
inline bool RingQueue<T>::spin_push(T item)
{
	size_t head, tail;
	while(!spinLock.TryGetLock(true)){};
	while(1)
	{
		head = info.head;
		tail = info.tail;		
		if ((head - tail) <= nMask)	// 容量还未满
		{
			size_t next = head + 1;
			info.head = next;
			queue[head & nMask] = item;	
			break;
		}
		// 容量已满，扩容（限制最大值容量）
//		if (nCapacity < LIMIT_MAX_QUEUE_SIZE/sizeof(T))
		if (nCapacity < LIMIT_MAX_QUEUE_SIZE)
		{
			// 扩容为原来的2倍	
#ifdef SHOW_DEBUG_RINGQUEUE
			printf("Add Buffer size:%d -> size:%d\n", nCapacity, nCapacity*2);
#endif			
			timeNote = GetTickCount();
			size_t index_flag = tail&nMask;	//(tail&nMask == tail&nMask)
			T* queueTemp = new T[nCapacity * 2];
			// 浅拷贝
		//	memcpy(queueTemp, queue, sizeof(T)*nCapacity);
		//	memcpy(queueTemp+nCapacity, queue, sizeof(T)*nCapacity);
			// 深度拷贝
			for (size_t i=0; i<nCapacity; ++i)
			{
				*(queueTemp + i) = *(queueTemp + nCapacity + i) = *(queue + i);
			}
			delete [] queue;
			nCapacity *= 2;
			nMask = nCapacity - 1;
			queue = queueTemp;
		}
		else
		{
			spinLock.UnLock();
			return false;
		}
	}	
	spinLock.UnLock();
	return true;
}

template <typename T>
inline bool RingQueue<T>::spin_pop(T & item)
{
	size_t head, tail;
	while(!spinLock.TryGetLock(true)){};
	head = info.head;
	tail = info.tail;
	if ((tail == head) || (tail > head && (head - tail) > nMask)) {		
		spinLock.UnLock();
		return false;
	}
	info.tail = tail + 1;
	item = queue[tail & nMask];	
	size_t dis = head-tail;		
	// 容量过大时，回收一半内存(内存回收不能太频繁 设时间间隔)
	if (nCapacity > nInitCapacity && dis < nCapacity/4 )
	{
		int timeNow = GetTickCount();
		if (timeNow - timeNote  > LIMIT_RECOV_TIME_MS)
		{
			timeNote = timeNow;
#ifdef SHOW_DEBUG_RINGQUEUE
			printf("Del Buffer size:%d -> size:%d\n", nCapacity, nCapacity/2);
#endif			
			size_t index_tail = tail&nMask;
			size_t index_head = head&nMask;
			T* queueTemp = new T[nCapacity/2];
			// 拷贝原始数据
			if (min(index_tail, index_head) >= nCapacity/2)	{
				// 浅拷贝
			//	memcpy(queueTemp, queue+nCapacity/2, sizeof(T)*(nCapacity/2));	
				// 深度拷贝
				for (size_t i=0; i<nCapacity/2; ++i)
				{
					*(queueTemp + i) = *(queue + nCapacity/2 + i);
				}
			}
			else if (max(index_tail, index_head) < nCapacity/2)	{
				// 浅拷贝
			//	memcpy(queueTemp, queue, sizeof(T)*(nCapacity/2));	
				// 深度拷贝
				for (size_t i=0; i<nCapacity/2; ++i)
				{
					*(queueTemp + i) = *(queue + i);
				}
			}
			else if(index_head > index_tail){
				// 浅拷贝
			//	memcpy(queueTemp+nCapacity/4, queue+nCapacity/4, sizeof(T)*(nCapacity/4));	
			//	memcpy(queueTemp, queue+nCapacity/2, sizeof(T)*(nCapacity/4));
				// 深度拷贝
				for (size_t i=0; i<nCapacity/4; ++i)
				{
					*(queueTemp + nCapacity/4 + i) = *(queue + nCapacity/4 + i);
					*(queueTemp + i) = *(queue + nCapacity/2 + i);
				}
			}
			else{
				// 浅拷贝
			//	memcpy(queueTemp, queue, sizeof(T)*(nCapacity/4));	
			//	memcpy(queueTemp+nCapacity/4, queue+3*nCapacity/4, sizeof(T)*(nCapacity/4));
				// 深度拷贝
				for (size_t i=0; i<nCapacity/4; ++i)
				{
					*(queueTemp + i) = *(queue + i);
					*(queueTemp + nCapacity/4 + i) = *(queue + 3*nCapacity/4 + i);					
				}
			}

			delete [] queue;
			nCapacity /= 2;
			nMask = nCapacity - 1;
			queue = queueTemp;
		}
	}
	spinLock.UnLock();
	return true;
}

template <typename T>
void RingQueue<T>::tryClear()
{
	while(!spinLock.TryGetLock(true)){};
	if (nCapacity == nInitCapacity || info.head != info.tail)
	{
		spinLock.UnLock();
		return;
	}	
	info.head = 0;
	info.tail = 0;
#ifdef SHOW_DEBUG_RINGQUEUE
	printf("reset size:%d -> size:%d\n", nCapacity, nInitCapacity);
#endif	
	nCapacity = nInitCapacity;
	nMask = nCapacity - 1;
	T* queueTemp = new T[nCapacity];
	delete [] queue;
	queue = queueTemp;	
	spinLock.UnLock();
}

template <typename T>
inline size_t RingQueue<T>::sizes() const
{
	size_t head, tail;
	_ReadWriteBarrier();

	head = info.head;
	tail = info.tail;

	return (size_t)((head - tail) <= nMask) ? (head - tail) : (size_t)-1;
}

template <typename T>
bool resize(size_t nNewSize)
{
	if (size() != 0)
	{
		return false;
	}
	delete [] queue;
	nNewSize = min(nNewSize, LIMIT_MAX_QUEUE_SIZE);
	nNewSize = max(nNewSize, LIMIT_MIN_QUEUE_SIZE);
	nNewSize = GetMinPowOfNum2(nNewSize);
	nInitCapacity = nNewSize;
	nCapacity	= nNewSize;
	nMask		= nCapacity-1;
	queue		= new T[nNewSize];
	timeNote	= GetTickCount();
}

template <typename T>
inline size_t RingQueue<T>::GetMinPowOfNum2(int nNum)
{
	size_t nSizeB = 0;
	while ((nNum & (nNum-1)) != 0)
	{
		nNum &= (nNum-1);
		++nSizeB;
	}
	if (nSizeB > 0)	
	{
		nNum = nNum<<1;
	}
	return nNum;
}
