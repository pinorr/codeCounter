/*******************************************************************
** 文件名:	CodeCounterMana.h
** 创建人:	饶建超
** 日  期:	2015-06-27  11:02
** 版  本:	1.0
** 描  述:	代码统计管理类（单例模式）
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "CounterDef.h"
#include "Singleton.h"
#include "Thread/ringQueue.h"
#include "Thread/ThreadManage.h"

using namespace rkt;

/*	统计管理类 原理说明：
1、管理类存有两个多线程安全队列：等待任务队列WaitQueue、已完成任务队列DoneQueue
2、外部调用AddTask，新增任务信息到WaitQueue保存
3、外部调用Start，会根据当前电脑CPU核心数，开启合适线程数，开始多线程运算
4、每个线程调用ConuterEndCallBack传回上次任务处理后的信息，并将其保存到DoneQueue，
   同时获得下一任务数据。以此循环，直到所有任务完成为止
5、外部定时调用GetDoneTaskData，主动来获取存在DoneQueue中已处理的数据
6、所有数据处理完成后，外部调用End终止所有新建线程

ps: 多线程安全队列是可自动扩容及内存回收的队列
///////////////////////////////////////////////////////////////////////////////*/

class CCodeCounterMana : public SingletonEx<CCodeCounterMana>
{
public:
	CCodeCounterMana() :m_nLeftNum(0)
	{}
	~CCodeCounterMana()
	{}
	// 新增处理任务数据
	void	AddTask(string filePath, stCounterConfig stConfig);
	// 开始多线程处理任务
	bool	Start();
	// 终止多线程任务
	void	End();
	// 队列中任务是否已全部处理完毕
	bool	bFinished();
	// 获取处理完毕的任务数据
	void	GetDoneTaskData(vector<stTaskDone> &vctTaskDone);
	// 任务数据回调接口
	static bool	ConuterEndCallBack(CCodeCounterMana * pMana, stFileMsg sFileMg, 
		string &strPath, stCounterConfig &stConfig);

public:
	ThreadManage			m_CTreadManage;			// 多线程管理器
	RingQueue<stTask>		m_queueListWait;		// 等待完成的任务列表
	RingQueue<stTaskDone>	m_queueListDone;		// 已完成的任务列表
	atomic_m<int>			m_nLeftNum;				// 剩余的任务数
};