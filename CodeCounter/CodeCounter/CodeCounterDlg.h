
// CodeCounterDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "CounterDef.h"
#include "FileCounter.h"
#include <map>
#include "afxcmn.h"
#include "Thread/spinLock.h"
#include "Thread/ThreadManage.h"

#define MAX_FIND_FILE_NUM		9999

// CCodeCounterDlg 对话框
class CCodeCounterDlg : public CDialogEx
{
	enum EM_TIMER_INDEX
	{
		EM_TIMER_SHOW_LIST,
	};
typedef map<CString, stCounterConfig>	MAP_CONFIG;
// 构造
public:
	CCodeCounterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CODECOUNTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnScan();
	afx_msg void OnBnClickedRadioFolder();
	afx_msg void OnBnClickedRadioFile();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();

public:
	CButton m_radioCounterType;
	CString m_cstrPath;
	DWORD m_nNullCount;
	DWORD m_nCodeCount;
	DWORD m_nNoteCount;
	DWORD m_nNoteCodeCount;
	DWORD m_nAllCount;
	CListCtrl m_List;
	CString m_cstrTest;
	CListBox m_lstBoxType;
	CString m_cstrAddType;
	CString m_cstrAddNote;
	CString m_cstrAddBeginNote;
	CString m_cstrAddEndNote;

private:	
	// 浏览得到一个文件夹目录路径
	CString		GetDirectory(void);
	// 从文件夹下得到所有文件列表
	void		FindFile(char *lpPath, vector<CString> &vctFilePath, size_t nMaxNum = MAX_FIND_FILE_NUM);
	// 从ini文件中读取配置
	void		ReadIniFile();
	// 保存配置到ini文件中
	void		WriteIniFile();
	// 显示文件列表详细数据
	void		ShowListData(CString cstrFileName, const stFileMsg stData);
	// 清空数据
	void		Clear();

private:
	EM_COUTER_AREA		m_nCounterType;		// 统计类型
	vector<CString>		m_vctFileList;		// 找到的文件列表
	CStringArray		m_strArray;			// 文件类型列表
	MAP_CONFIG			m_mapTypeConfig;	// 统计配置列表
	CFileCounter		m_CFileCounter;		// 源码统计类
	int					m_nCountNum;		// 统计的文件数
};
