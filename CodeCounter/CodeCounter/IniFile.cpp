// m_IniFile.cpp: implementation of the CIniFile class. 
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h" 
#include "IniFile.h"

#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
#define new DEBUG_NEW 
#endif

////////////////////////////////////////////////////////////////////// 
// Construction/Destruction 
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile() 
{ 
	m_sFileName = "ini.ini"; 
}

CIniFile::~CIniFile() 
{ 
	if(m_FileContainer.GetSize() > 0) 
	{ 
		m_FileContainer.RemoveAll(); 
	} 
}

void CIniFile::SetFileName(CString FileName) 
{ 
	m_sFileName = FileName; 
}

void CIniFile::SetInt(CString Section, CString Item, int Value) 
{ 
	CString strtemp; 
	//  strtemp.Format("%d",Value); 
	//保存到m_FileContainer 
	WriteFileString(Section, Item, strtemp); 
	//保存到ini文件 
	WriteIniFile(); 
}

void CIniFile::SetString(CString Section, CString Item, CString Value) 
{ 
	//保存到m_FileContainer 
	WriteFileString(Section, Item, Value); 
	//保存到ini文件 
	WriteIniFile(); 
}

CString CIniFile::GetString(CString Section, CString Item, CString Value) 
{ 
	return GetFileString(Section, Item, Value); 
}

int CIniFile::GetInt(CString Section, CString Item, int Value) 
{ 
	CString strtemp; 
	strtemp.Format("%d",Value); 
	return atoi(GetFileString(Section, Item, strtemp)); 
	//return GetFileString(Section, Item, strtemp); 
}

BOOL CIniFile::ReadIniFile() 
{ 
	m_bFileExist = m_IniFile.Open(m_sFileName, CFile::modeRead);

	if(!m_bFileExist) 
	{ 
		return FALSE; 
	}

	CString strline; 
	m_FileContainer.RemoveAll();

	//将m_IniFile文件数据读到m_FileContainer 
	while(m_IniFile.ReadString(strline)) 
	{ 
		m_FileContainer.Add(strline); 
	}

	m_IniFile.Close(); 
	return TRUE; 
}

BOOL CIniFile::WriteIniFile() 
{ 
	m_bFileExist=m_IniFile.Open(m_sFileName, CFile::modeCreate | CFile::modeWrite); 
	if(!m_bFileExist) 
		return FALSE; 

	CString strline;

	//将m_FileContainer写到m_IniFile文件 
	for(int i = 0; i< m_FileContainer.GetSize(); i++) 
	{ 
		strline = m_FileContainer[i]; 
		m_IniFile.WriteString(strline+"/n"); 
	}

	m_IniFile.Close();

	return m_bFileExist; 
}

CString CIniFile::GetFileString(CString Section, CString Item, CString Value) 
{ 
	ReadIniFile();//打开文件

	if(m_bFileExist == FALSE || m_FileContainer.GetSize() < 0) 
		return Value;//文件打开出错或文件为空，返回默认值

	int i = 0; 
	int iFileLines = m_FileContainer.GetSize(); 
	CString strline,str;

	while(i<iFileLines) 
	{ 
		strline = m_FileContainer.GetAt(i++);

		strline.TrimLeft(); 
		if(strline.GetAt(0)=='[')//查找Section，第一个必须为[ 
		{ 
			str=strline.Left(strline.Find("]"));//去掉]右边 
			str=str.Right(str.GetLength()-str.Find("[")-1);//去掉[左边

			str.TrimLeft(); 
			str.TrimRight();

			if(Section == str)//找到Section 
			{ 
				while(i<iFileLines) 
				{ 
					strline = m_FileContainer.GetAt(i++); 
					strline.TrimLeft();

					if(strline.GetAt(0)=='[') 
						return Value;//如果到达下一个[]，即找不到,返回默认值 

					str = strline.Left(strline.Find("="));//去掉=右边

					str.TrimLeft(); 
					str.TrimRight();

					if(Item == str)//找到Item 
					{ 
						str=strline.Right(strline.GetLength()-strline.Find("=")-1);//去掉=左边

						str.TrimLeft(); 
						str.TrimRight();

						return str; 
					} 
				} 
				return Value;//找不到,返回默认值 
			}

		}

	} 
	return Value;//找不到,返回默认值 
}

void CIniFile::WriteFileString(CString Section, CString Item, CString Value) 
{ 
	int i = 0; 
	int iFileLines = m_FileContainer.GetSize(); 
	CString strline,str;

	while(i<iFileLines) 
	{ 
		strline = m_FileContainer.GetAt(i++);

		strline.TrimLeft(); 
		if(strline.GetAt(0)=='[')//查找Section，第一个必须为[ 
		{ 
			str=strline.Left(strline.Find("]"));//去掉]右边 
			str=str.Right(str.GetLength()-str.Find("[")-1);//去掉[左边

			str.TrimLeft(); 
			str.TrimRight();

			if(Section == str)//找到Section 
			{ 
				while(i<iFileLines) 
				{ 
					strline = m_FileContainer.GetAt(i++); 
					strline.TrimLeft();

					if(strline.GetAt(0)=='[')//如果到达下一个[]，即找不到Item 
					{ 
						break; 
					} 

					str = strline.Left(strline.Find("="));//去掉=右边

					str.TrimLeft(); 
					str.TrimRight();

					if(Item == str)//找到Item 
					{ 
						strline = Item + "=" + Value; 
						m_FileContainer[i-1] = strline; 
						return; 
					} 
				} 
				//找不到Item 
				strline = Item + "=" + Value; 
				m_FileContainer.InsertAt(i-1, strline); 
				return;

			}

		}

	} 
	//找不到Section 
	//直接在最后加入Section，Item，Value 
	m_FileContainer.Add("[" + Section + "]"); 
	m_FileContainer.Add(Item + "=" + Value); 
	return; 
}