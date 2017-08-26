/*******************************************************************
** 文件名:	TreadManage.h
** 创建人:	饶建超
** 日  期:	2015-06-27  18:55
** 版  本:	1.0
** 描  述:	简单线程管理器
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include <vector>
#include "Thread.h"
#include "../CounterDef.h"

class CFileCounter;
class CCodeCounterMana;

// 线程管理器
class ThreadManage
{
public:
	typedef std::vector<::rkt::IRunnable*>	IRUNNABLEVECTOR;

public:
	ThreadManage(void);
	~ThreadManage(void);

	// 获取线程池
	rkt::ThreadPool* getThreadPool();
	// 开启线程
	void ThreadStart();
	// 终止线程
	void ThreadTerminate();

	// 线程创建
	void CounterThreadCreate(int nNumCPU, CCodeCounterMana * pMana, bool (*ConuterEndCallBack)(CCodeCounterMana * pMana, 
		stFileMsg sFileMg, string &strPath, stCounterConfig &stConfig));
	// 结束所有线程
	void CounterThreadClose();	

private:
	// 线程池
	rkt::ThreadPool m_threadPool;
	// 线程句柄列表
	IRUNNABLEVECTOR m_vctCounterThread;
};

