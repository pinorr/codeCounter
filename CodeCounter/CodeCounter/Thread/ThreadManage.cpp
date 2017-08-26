/*******************************************************************
** 文件名:	TreadManage.cpp
** 创建人:	饶建超
** 日  期:	2015-06-27  18:55
** 版  本:	1.0
** 描  述:	简单进程管理类
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "ThreadManage.h"
#include "../FileCounter.h"

#include "Thread.h"


ThreadManage::ThreadManage(void)
{
}

ThreadManage::~ThreadManage(void)
{
}

//////////////////////////////////////////////
//		获取线程池
//////////////////////////////////////////////
rkt::ThreadPool* ThreadManage::getThreadPool()
{
	return &m_threadPool;
}

//////////////////////////////////////////////
//		开启线程
//////////////////////////////////////////////
void ThreadManage::ThreadStart()
{
	m_threadPool.start();
}

//////////////////////////////////////////////
//		终止线程
//////////////////////////////////////////////
void ThreadManage::ThreadTerminate()
{
	m_threadPool.clear();
}

//////////////////////////////////////////////
//		线程创建
//////////////////////////////////////////////
void ThreadManage::CounterThreadCreate(int nNumCPU, CCodeCounterMana * pMana, bool (*ConuterEndCallBack)(CCodeCounterMana * pMana, 
	stFileMsg sFileMg, string &strPath, stCounterConfig &stConfig))
{	
	for (int i=0; i<nNumCPU; ++i)
	{
		CFileCounter * p = new CFileCounter(pMana, ConuterEndCallBack);
		getThreadPool()->add(p);
		m_vctCounterThread.push_back(p);		
	}
}

//////////////////////////////////////////////
//		结束所有线程
//////////////////////////////////////////////
void ThreadManage::CounterThreadClose()
{
	for (IRUNNABLEVECTOR::iterator iter = m_vctCounterThread.begin(); iter != m_vctCounterThread.end(); ++iter)
	{ 
		CFileCounter* p = (CFileCounter*)(*iter);
		if (p)
		{
			p->Close();
		}
	}
	m_vctCounterThread.clear();
}
