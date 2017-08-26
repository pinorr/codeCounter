//////////////////////////////////////////////////////////////////*/ 
// IniFile.h: interface for the CIniFile class. 
// 
//////////////////////////////////////////////////////////////////////
#pragma once;

#include <afx.h>

class CIniFile   
{ 
public: 
    void SetFileName(CString FileName);

    void WriteFileString(CString Section, CString Item, CString Value); 
    CString GetFileString(CString Section, CString Item, CString Value);

    CString GetString(CString Section, CString Item, CString Value); 
    int GetInt(CString Section, CString Item, int Value);

    void SetString(CString Section, CString Item, CString Value); 
    void SetInt(CString Section, CString Item, int Value);

    BOOL WriteIniFile(); 
    BOOL ReadIniFile();

    CIniFile(); 
    virtual ~CIniFile();

private:
    //ini file 
    CString			m_sFileName; 
    CStdioFile		m_IniFile; 
    BOOL			m_bFileExist; 
    CStringArray	m_FileContainer;

}; 