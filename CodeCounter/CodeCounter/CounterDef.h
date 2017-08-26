/*******************************************************************
** 文件名:	CounterDef.h
** 创建人:	饶建超
** 日  期:	2015-06-27  11:02
** 版  本:	1.0
** 描  述:	源代码统计头文件定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <string.h>
#include <vector>

using namespace std;

#define INI_FILE_NAME	"D:\\CodeCounterConfig.ini"
#define CODE_TYPE_NAME_MAX		10
#define CODE_TYPE_NUM_MAX		100

#define deleteSafe(x)	delete []x; x=NULL;

// 统计范围枚举
enum EM_COUTER_AREA
{
	CODE_COUNTER_FOLDER	= 0,	// 文件夹
	CODE_COUNTER_FILE,			// 文件
	CODE_COUNTER_MAX,			// 最大
};

// 文件行数相关数据
struct stFileMsg
{
	int nAllCount;			// 总行数
	int nNullCount;			// 空行行数
	int nNoteCount;			// 注释行数
	int nCodeCount;			// 有效代码行数
	int nNodeCodeCount;		// 代码&注释行数

	stFileMsg()
	{
		clear();
	}	
	stFileMsg(const stFileMsg &stTemp)
	{
		nAllCount = stTemp.nAllCount;
		nNullCount = stTemp.nNullCount;
		nNoteCount = stTemp.nNoteCount;
		nCodeCount = stTemp.nCodeCount;
		nNodeCodeCount = stTemp.nNodeCodeCount;
	}
	void clear()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 成双注释符
struct stPairChars
{
	string strBegin;	// 起始注释符
	string strEnd;		// 结束注释符
	stPairChars()
	{
		strBegin = "";
		strEnd = "";
	}
	bool bCheakConfig()
	{
		return strBegin.size() > 0 && strEnd.size() > 0;
	}
};

// 注释符数据
struct stCounterConfig
{
	string		strNote;			// 单行注释符
	stPairChars strPairNote;		// 多行注释符

	stCounterConfig()
	{
		strNote = "";
	}	
	stCounterConfig(const stCounterConfig &stTemp)
	{
		strNote = stTemp.strNote;
		strPairNote.strBegin = stTemp.strPairNote.strBegin;
		strPairNote.strEnd = stTemp.strPairNote.strEnd;
	}
	bool bCheakConfig()
	{
		return strNote.size() > 0 && strPairNote.bCheakConfig();
	}
};

// 任务数据
struct stTask
{
	string strFilePath;
	stCounterConfig stConfig;
	stTask()
	{
		strFilePath = "";
	}
	stTask(const stTask &stTemp)
	{
		strFilePath = stTemp.strFilePath;
		stConfig = stTemp.stConfig;
	}
	stTask(string filePath, stCounterConfig config)
	{
		strFilePath = filePath;
		stConfig = config;
	}
};

// 已完成的任务数据
struct stTaskDone
{
	string		strFilePath;
	stFileMsg	sFileMsg;

	stTaskDone()
	{
	}
	stTaskDone(string filePath, stFileMsg fileMsg)
	{
		strFilePath = filePath;
		sFileMsg	= fileMsg;
	}
};