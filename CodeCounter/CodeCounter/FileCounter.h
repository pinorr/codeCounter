/*******************************************************************
** 文件名:	FileCounter.h
** 创建人:	饶建超
** 日  期:	2015-06-27  11:02
** 版  本:	1.0
** 描  述:	文件代码统计头文件
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

/*
算法说明：
单行注释符：a
多行注释开启符: b
多行注释终止符：e
主要有以下内个步骤：
1、读取文件一行内容，去首尾空格、TAB字符
2、如果当前行为空，则此行为空行，转步骤8
3、如果非空则先将特殊转义符：\"和\'分别转成生僻字符◎和※，以免影响步骤4的正确性
4、将""与''字符间的字符串删除
5、如果同时有单行注释符a和多行注释开启符b且a在b前面
	或位置相同(如lua的--与--[[)，则无视多行注释开启符b
6、判断当前状态
	6.1 非注释状态，查找多行注释开启符b
		6.1.1 找到开启符b,则进入注释状态,查找多行注释终止符e
			6.1.1 找到终止符e，则删除开户符b与终止符e中间的所有字符，
				  退出注释状态且转为步骤5
			6.1.2 没找到终止符e，则判断是否有代码行、注释行，继续步骤7
		6.1.2 没找到开启符b，则判断当前行是否有代码行、注释行，继续步骤7
	6.2 注释状态，查找多行注释终止符e
		6.2.1 找到终止符e，则删除开户符b与终止符e中间的所有字符，
			  退出注释状态且转为步骤5
		6.2.2 没找到终止符e，则判断是否有代码行、注释行，继续步骤7
7、由以上步骤，统计当前行是否有代码行、注释行
8、此行判断结束,继续下一行
///////////////////////////////////////////////////////////////////*/

#pragma once

#include "CounterDef.h"
#include "Thread\Thread.h"

class CCodeCounterMana;

class CFileCounter:public ::rkt::IRunnable
{
public:
	CFileCounter();
	CFileCounter(CCodeCounterMana * pMana, bool (*ConuterEndCallBack)(CCodeCounterMana * pMana, 
		stFileMsg sFileMg, string &strPath, stCounterConfig &stConfig));
	~CFileCounter(void);	

public:
	// 字符UTF-8转成多字节
	bool	UTF8ToMB(vector<char>& pmb, const char* pu8, int utf8Len);

	// 文件统计函数
	// param@ lpPath:	文件路径
	// param@ stConfig:	文件统计配置
	// param@ sFileMg:	文件统计结果引用输出
	// return: bool		是否统计成功
	bool	GetFileMsg(const char *lpPath, stCounterConfig stConfig, stFileMsg &sFileMg);

	// 结束线程
	void	Close();	

	/////////// IRunnable ///////////////////////////
	virtual void release();
	virtual void run();

private:
	// 文件路径
	string				m_strPath;

	// 文件统计相关配置
	stCounterConfig		m_stConfig;

	// 代码统计管理类指针（回调用）
	CCodeCounterMana *  m_pMana;

	// 回调函数指针变量
	bool (*m_ConuterEndCallBack)(CCodeCounterMana * pMana, stFileMsg sFileMg, 
		string &strPath, stCounterConfig &stConfig);
};