/*******************************************************************
** 文件名:	FileCounter.cpp
** 创建人:	饶建超
** 日  期:	2015-06-27
** 版  本:	1.0
** 描  述:				
********************************************************************/

#include "StdAfx.h"
#include "FileCounter.h"
#include "CodeCounterMana.h"
#include <afx.h>

CFileCounter::CFileCounter()
{
}

CFileCounter::CFileCounter(CCodeCounterMana * pMana, bool (*ConuterEndCallBack)(CCodeCounterMana * pMana,	
	stFileMsg sFileMg, string &strPath, stCounterConfig &stConfig))
{
	m_pMana = pMana;
	m_ConuterEndCallBack = ConuterEndCallBack;
}

CFileCounter::~CFileCounter()
{
}

//////////////////////////////////////////////
//		结束线程
//////////////////////////////////////////////
void CFileCounter::Close()
{
	release();
	delete this;
}

//////////////////////////////////////////////
//		清空数据
//////////////////////////////////////////////
void CFileCounter::release()
{
	m_strPath = "";
	m_stConfig.strNote = "";
	m_stConfig.strPairNote.strBegin = "";
	m_stConfig.strPairNote.strEnd = "";
}

//////////////////////////////////////////////
//		开启线程
//////////////////////////////////////////////
void CFileCounter::run()
{
	stFileMsg sFileMg;
	while (m_ConuterEndCallBack(m_pMana, sFileMg, m_strPath, m_stConfig))
	{
		sFileMg.clear();
		GetFileMsg(m_strPath.c_str(), m_stConfig, sFileMg);
	}
}

//////////////////////////////////////////////
//	UTF8编码转为多字节编码
//////////////////////////////////////////////
bool CFileCounter::UTF8ToMB(vector<char>& pmb, const char* pu8, int utf8Len)  
{  
	// convert an UTF8 string to widechar   
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);  

	WCHAR* lpszW = NULL;  
	try  
	{  
		lpszW = new WCHAR[nLen];  
	}  
	catch(bad_alloc)  
	{  
		return false;  
	}  

	int nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, lpszW, nLen);  

	if(nRtn != nLen)  
	{  
		delete[] lpszW;  
		return false;  
	}  

	// convert an widechar string to Multibyte   
	int MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);  
	if (MBLen <=0)  
	{  
		return false;  
	}  
	pmb.resize(MBLen);  
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, &*pmb.begin(), MBLen, NULL, NULL);  
	delete[] lpszW;  

	if(nRtn != MBLen)  
	{  
		pmb.clear();  
		return false;  
	}  
	return true;  
}  

//////////////////////////////////////////////
//	统计文件相关信息
// param@ lpPath:	文件路径
// param@ stConfig:	文件统计配置
// param@ sFileMg:	文件统计结果引用输出
// return: bool		是否统计成功			
//////////////////////////////////////////////
bool CFileCounter::GetFileMsg(const char *lpPath, stCounterConfig stConfig, stFileMsg &sFileMg)
{
	if (lpPath == NULL || !stConfig.bCheakConfig())
	{
		return false;
	}
	CStdioFile myFile;
	if(myFile.Open(lpPath, CFile::typeText|CFile::modeRead))
	{
		myFile.SeekToBegin();
		CString str;
		// 是否在多行注释状态中
		bool bNoteBegined = false;
		bool bBegin = true;
		while (myFile.ReadString(str))
		{
			if (bBegin)
			{
				vector<char> vctTemp;
				UTF8ToMB(vctTemp, (LPSTR)(LPCTSTR)str, str.GetLength());
				bBegin = false;
				if (vctTemp.size() > 0)
				{
					str = (char *)&vctTemp[0];
				}	
			}
			++sFileMg.nAllCount;			
			// 去空格、TAB字符（也可加入配置中配置）
			str.TrimLeft(_T("\t "));
			str.TrimRight(_T("\t "));
			if (str.GetLength() == 0)
			{
				++sFileMg.nNullCount;
			}
			else
			{
				bool bNoteLine = false;		// 记录该行是否有注释段
				bool bCodeLine = false;		// 记录该行是否有代码段
				bool bDeleted = false;		// 记录字符串是否已删除一部分
				do 
				{		
					bDeleted = false;
					str.Replace("\\\"", "◎");
					str.Replace("\\\'", "※");
					// 去掉""字符串					
					int nDelBegin = str.Find("\"");		// 注意转义字符\'与\"
					int nDelEnd = -1;
					if (nDelBegin >= 0)
					{
						nDelEnd = str.Find("\"", nDelBegin+1);
						if (nDelEnd > nDelBegin)
						{
							str.Delete(nDelBegin, nDelEnd-nDelBegin+1);
						}
					}
					// 去掉''字符串					
					nDelBegin = str.Find("\'");		// 注意转义字符\'与\"
					nDelEnd = -1;
					if (nDelBegin >= 0)
					{
						nDelEnd = str.Find("\'", nDelBegin+1);
						if (nDelEnd > nDelBegin)
						{
							str.Delete(nDelBegin, nDelEnd-nDelBegin+1);
						}
					}
					
					// 以前的代码没有开启多行注释
					if (!bNoteBegined)
					{
						// 查找多行注释开启符、单行注释符
						int nBeginIndex = str.Find(stConfig.strPairNote.strBegin.c_str());
						int nNoteIndex = str.Find(stConfig.strNote.c_str());
						// 单行注释符在多行注释符前面，则多行注释符无效
						if (nNoteIndex >= 0 && nNoteIndex < nBeginIndex)
						{
							if (nNoteIndex > 0)
							{
								bCodeLine = true;
							}							
							bNoteLine = true;						
						}
						else
						{												
							if (nBeginIndex >=0)		// 开启多行注释状态
							{													
								bNoteBegined = true;								
								bNoteLine = true;
								if (nBeginIndex != 0)	// 发现代码段
								{
									bCodeLine = true;
								}
								// 查找多行终止符
								int nEndIndex = str.Find(stConfig.strPairNote.strEnd.c_str());
								// 在本行发现终止符且与开始符没有重叠，则关闭注释状态
								if (nEndIndex >=0 && nEndIndex-nBeginIndex >= (int)stConfig.strPairNote.strBegin.size())
								{
									str = str.Mid(nEndIndex + stConfig.strPairNote.strEnd.size());
									bDeleted = true;
									bNoteBegined = false;
									str.TrimLeft(_T("\t "));
								}
							}
							else if (nNoteIndex >=0)	// 发现单行注释
							{
								bNoteLine = true;
								if (nNoteIndex != 0)		// 发现代码段
								{
									bCodeLine = true;
								}
							}
							else
							{
								bCodeLine = true;
							}
						}								
					}
					else	// 当前处于注释代码状态
					{
						bNoteLine = true;
						// 查找多行注释终止符、单行注释符
						int nEndIndex = str.Find(stConfig.strPairNote.strEnd.c_str());
						int nNoteIndex = str.Find(stConfig.strNote.c_str());
						if (nEndIndex >=0)
						{
							str = str.Mid(nEndIndex + stConfig.strPairNote.strEnd.size());
							bDeleted = true;
							bNoteBegined = false;
							str.TrimLeft(_T("\t "));
						}
					}
				} while (str.GetLength() != 0 && bDeleted );			
				if (bNoteLine)
				{
					++sFileMg.nNoteCount;
				}
				if (bCodeLine)
				{
					++sFileMg.nCodeCount;
				}	
				if (bNoteLine && bCodeLine)
				{
					++sFileMg.nNodeCodeCount;
				}
			}
		}		
	}
	else
	{
		myFile.Close();
		return false;
	}
	myFile.Close();
	return true;
}