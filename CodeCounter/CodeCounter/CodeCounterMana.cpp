
#include "stdafx.h"
#include "CodeCounterMana.h"

#define MIN_PROCESSOR_NUM		1
#define MAX_PROCESSOR_NUM		32

//////////////////////////////////////////////
//		新增处理任务数据到任务队列中
//////////////////////////////////////////////
void CCodeCounterMana::AddTask(string filePath, stCounterConfig stConfig)
{	
	if (!stConfig.bCheakConfig() || filePath.size() == 0)
	{
		return;
	}
	bool bAdd = m_queueListWait.spin_push(stTask(filePath, stConfig));
	if (bAdd)
	{
		m_nLeftNum.AddValue(1);
	}	
}

//////////////////////////////////////////////
//		开始多线程处理任务
//////////////////////////////////////////////
bool CCodeCounterMana::Start()
{
	if (m_queueListWait.sizes() > 0)
	{
		// 获取CPU核心数n，最佳性能n个运算线程并行运算
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		int nNewPro = min(MAX_PROCESSOR_NUM, max(MIN_PROCESSOR_NUM, info.dwNumberOfProcessors));
		m_CTreadManage.CounterThreadCreate(nNewPro, this, ConuterEndCallBack);
		m_CTreadManage.ThreadStart();
		return true;
	}
	return false;
}

//////////////////////////////////////////////
//		终止多线程任务
//////////////////////////////////////////////
void CCodeCounterMana::End()
{
	m_CTreadManage.CounterThreadClose();
	m_CTreadManage.ThreadTerminate();
}

//////////////////////////////////////////////
//		队列中任务是否已全部处理完毕
//////////////////////////////////////////////
bool CCodeCounterMana::bFinished()
{
	return m_nLeftNum.GetValue() == 0 && m_queueListDone.sizes() == 0;
}

//////////////////////////////////////////////
//		获取处理完毕的任务数据
//////////////////////////////////////////////
void CCodeCounterMana::GetDoneTaskData(vector<stTaskDone> &vctTaskDone)
{
	vctTaskDone.clear();
	stTaskDone stDoneTemp;
	while (m_queueListDone.spin_pop(stDoneTemp))
	{
		vctTaskDone.push_back(stDoneTemp);
	}
}

//////////////////////////////////////////////
// 任务数据回调接口
//param@ pMana：	单例指针
//param@ sFileMg：	文件处理结果（传入上次处理的文件结果）
//param@ strPath：	文件路径（传入上次处理文件路径，且传回下一个需要处理的文件路径）
//param@ stConfig：	文件对应配置（strPath对应文件的统计配置）
//return bool		是否回调成功
//////////////////////////////////////////////
bool CCodeCounterMana::ConuterEndCallBack(CCodeCounterMana * pMana, stFileMsg sFileMg, 
	string &strPath, stCounterConfig &stConfig)
{
	if (pMana == NULL)
	{
		return false;
	}
	if (strPath.size() > 0)
	{
		stTaskDone stDoneTemp(strPath, sFileMg);
		pMana->m_queueListDone.spin_push(stDoneTemp);
	}	
	stTask stTaskTemp;
	if (pMana->m_queueListWait.spin_pop(stTaskTemp))
	{
		strPath = stTaskTemp.strFilePath;
		stConfig = stTaskTemp.stConfig;	
		pMana->m_nLeftNum.AddValue(-1);
		return true;
	}
	return false;
}