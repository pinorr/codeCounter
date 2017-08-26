
// CodeCounterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CodeCounter.h"
#include "CodeCounterDlg.h"
#include "CodeCounterMana.h"
#include "afxdialogex.h"
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCodeCounterDlg 对话框

CCodeCounterDlg::CCodeCounterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCodeCounterDlg::IDD, pParent)
	, m_cstrPath(_T(""))
	, m_nNullCount(0)
	, m_nCodeCount(0)
	, m_nNoteCount(0)
	, m_nNoteCodeCount(0)
	, m_nAllCount(0)
	, m_cstrTest(_T(""))
	, m_cstrAddType(_T(""))
	, m_cstrAddNote(_T(""))
	, m_cstrAddBeginNote(_T(""))
	, m_cstrAddEndNote(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
	new CCodeCounterMana;
}

void CCodeCounterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_FOLDER, m_radioCounterType);
	DDX_Text(pDX, IDC_EDIT_CODE_PATH, m_cstrPath);
	DDX_Text(pDX, IDC_EDIT_NULL, m_nNullCount);
	DDX_Text(pDX, IDC_EDIT_CODE, m_nCodeCount);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_nNoteCount);
	DDX_Text(pDX, IDC_EDIT_NOTE_CODE, m_nNoteCodeCount);
	DDX_Text(pDX, IDC_EDIT_ALL, m_nAllCount);
	DDX_Control(pDX, IDC_LIST_SHOW, m_List);
	DDX_Text(pDX, IDC_STATIC_TEST, m_cstrTest);
	DDX_Control(pDX, IDC_LIST_TYPE, m_lstBoxType);
	DDX_Text(pDX, IDC_EDIT_ADD_TYPE, m_cstrAddType);
	DDX_Text(pDX, IDC_EDIT_ADDNOTE, m_cstrAddNote);
	DDX_Text(pDX, IDC_EDIT_ADD_BEGINNOTE, m_cstrAddBeginNote);
	DDX_Text(pDX, IDC_EDIT_ADD_ENDNOTE, m_cstrAddEndNote);
}

BEGIN_MESSAGE_MAP(CCodeCounterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SCAN, &CCodeCounterDlg::OnBnClickedBtnScan)
	ON_BN_CLICKED(IDC_RADIO_FOLDER, &CCodeCounterDlg::OnBnClickedRadioFolder)
	ON_BN_CLICKED(IDC_RADIO_FILE, &CCodeCounterDlg::OnBnClickedRadioFile)
	ON_BN_CLICKED(IDOK, &CCodeCounterDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CCodeCounterDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCodeCounterDlg 消息处理程序

BOOL CCodeCounterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 默认选择文件夹
	m_nCounterType = CODE_COUNTER_FOLDER;
	m_radioCounterType.SetCheck(TRUE);

	// 加载配置
	ReadIniFile();

	// 初始化表头
	m_List.InsertColumn(0,"文件名");  
	m_List.InsertColumn(1,"文件类型");  
	m_List.InsertColumn(2,"总行数");  
	m_List.InsertColumn(3,"纯代码行数"); 
	m_List.InsertColumn(4,"纯注释行数"); 
	m_List.InsertColumn(5,"空行数");  
	m_List.InsertColumn(6,"注释&代码"); 
	// 设置列的宽度。
	CRect rect;  
	m_List.GetClientRect(rect); 
	for (int i = 0; i<7; ++i)
	{
		m_List.SetColumnWidth(i, rect.Width() / 7);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCodeCounterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCodeCounterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCodeCounterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////
//		浏览得到一个文件夹目录路径
//////////////////////////////////////////////
CString CCodeCounterDlg::GetDirectory()
{
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = _T("选择文件夹目录");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
		return _T("");
	CString strDirectoryPath;
	SHGetPathFromIDList(idl, strDirectoryPath.GetBuffer(MAX_PATH));
	strDirectoryPath.ReleaseBuffer();
	if(strDirectoryPath.IsEmpty())
		return _T("");
	if(strDirectoryPath.Right(1)!="\\")
		strDirectoryPath+="\\";

	return strDirectoryPath;
}

//////////////////////////////////////////////
//		得到路径下的所有文件路径列表
//////////////////////////////////////////////
void CCodeCounterDlg::FindFile(char * lpPath, vector<CString> &vctFilePath, size_t nMaxNum)
{
	if (strlen(lpPath) <= 1)
	{
		return;
	}
    char szFind[MAX_PATH];
	char szFileName[MAX_PATH];
    WIN32_FIND_DATA FindFileData;

    strcpy_s(szFind,lpPath);
    strcat_s(szFind,"\\*.*");

    HANDLE hFind=::FindFirstFile((CString)szFind, &FindFileData);
    if(INVALID_HANDLE_VALUE == hFind)  
	{
		return;
	}
    while(TRUE)
    {
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(FindFileData.cFileName[0]!='.')
            {
				char szFile[MAX_PATH];
                strcpy_s(szFile,lpPath);
                strcat_s(szFile,"\\");
                strcat_s(szFile,(char *)FindFileData.cFileName);
                FindFile(szFile, vctFilePath);
            }
        }
        else
        {		
			sprintf_s(szFileName,"%s\\%s", lpPath, (CHAR*)(LPCTSTR)FindFileData.cFileName);
			vctFilePath.push_back(CString(szFileName));
			if (nMaxNum <= vctFilePath.size())
			{
				return;
			}
        }
        if(!FindNextFile(hFind, &FindFileData))   
			break;
    }
    FindClose(hFind);
}

//////////////////////////////////////////////
//		读取配置文件
//////////////////////////////////////////////
void CCodeCounterDlg::ReadIniFile()
{
	// 向D盘写入程序数据库连接ini文件信息，默认设置如下  
	CFileFind finder;  
	CString cstrIniFile = _T(INI_FILE_NAME);
	BOOL bFind = finder.FindFile(cstrIniFile); 
	if( bFind )
	{
		int nAllTypeNamesMaxSize = CODE_TYPE_NAME_MAX*CODE_TYPE_NUM_MAX+1;   
		char *pszSectionNames = new char[nAllTypeNamesMaxSize];
		DWORD dwCopied = 0; 
		dwCopied = ::GetPrivateProfileSectionNames(pszSectionNames, 
			nAllTypeNamesMaxSize, cstrIniFile); 

		m_strArray.RemoveAll();
		char *pSection = pszSectionNames;    
		do
		{    
			CString szSection(pSection);    
			if (szSection.GetLength() < 1)    
			{   
				break; 
			}    
			m_strArray.Add(szSection);    
			pSection = pSection + szSection.GetLength() + 1; // next section name  
		}while (pSection && pSection<pszSectionNames+nAllTypeNamesMaxSize);    
		deleteSafe(pszSectionNames);
		//m_mapTypeConfig
		for (int i=0; i<m_strArray.GetCount(); ++i)
		{
			stCounterConfig stTemp;
			CString cstrType = m_strArray.GetAt(i);		
			// 读取单行注释符
			CString cstrNote = "Note";
			char chrRead[32];
			int nGet = GetPrivateProfileString(cstrType,cstrNote,"",chrRead, sizeof(chrRead),
				(LPSTR)(LPCTSTR)cstrIniFile);
			if (nGet == 0)
			{
				break;
			}
			stTemp.strNote = chrRead;
			// 读取多行注释符
			CString cstrNoteBegin = "PairNoteBegin", cstrNoteEnd = "PairNoteEnd";
			char chrReadBegin[32], chrReadEnd[32];
			int nGetBegin = GetPrivateProfileString(cstrType,cstrNoteBegin,"",chrReadBegin, sizeof(chrReadBegin),
				(LPSTR)(LPCTSTR)cstrIniFile);
			int nGetEnd = GetPrivateProfileString(cstrType,cstrNoteEnd,"",chrReadEnd, sizeof(chrReadEnd),
				(LPSTR)(LPCTSTR)cstrIniFile);
			if (nGetBegin == 0 || nGetEnd == 0)
			{
				break;
			}
			stPairChars stPairTemp;
			stPairTemp.strBegin = chrReadBegin;
			stPairTemp.strEnd = chrReadEnd;
			stTemp.strPairNote = stPairTemp;
			m_mapTypeConfig[cstrType] = stTemp;
		}		
	}
	else	// 无配置则新建一个，默认支持: cpp;h;c;hpp;lua
	{
		stCounterConfig stTemp;
		stTemp.strNote = "//";
		stTemp.strPairNote.strBegin = "/*";
		stTemp.strPairNote.strEnd = "*/";
		m_mapTypeConfig[_T("cpp")] = stTemp;
		m_mapTypeConfig[_T("h")] = stTemp;
		m_mapTypeConfig[_T("c")] = stTemp;
		m_mapTypeConfig[_T("hpp")] = stTemp;
		stTemp.strNote = "--";
		stTemp.strPairNote.strBegin = "--[[";
		stTemp.strPairNote.strEnd = "]]";
		m_mapTypeConfig[_T("lua")] = stTemp;
		WriteIniFile();
	}
	// 显示类型列表并设置默认全选中
	MAP_CONFIG::iterator iter = m_mapTypeConfig.begin();
	for (; iter != m_mapTypeConfig.end(); ++iter)
	{
		int nIndex = m_lstBoxType.AddString(iter->first);
		//m_lstBoxType.SetCurSel(nIndex);
		m_lstBoxType.SetSel(nIndex, true);
	}	
}

//////////////////////////////////////////////
//		显示文件统计列表详细数据
//////////////////////////////////////////////
void CCodeCounterDlg::ShowListData(CString cstrFileName, const stFileMsg stData)
{
	if (cstrFileName.GetLength() == 0)
	{
		return;
	}
	int nIndex = cstrFileName.ReverseFind('\\');
	CString cstrName = cstrFileName.Mid(nIndex+1);
	nIndex = cstrName.ReverseFind('.');
	CString cstrType = cstrName.Mid(nIndex+1);
	cstrName = cstrName.Left(nIndex);
	// 设置数据
	CString cstrTemp;
	int nListNum = m_List.GetItemCount();
	m_List.InsertItem(nListNum, cstrName);  
	m_List.SetItemText(nListNum, 1, cstrType); 
	cstrTemp.Format("%d", stData.nAllCount);
	m_List.SetItemText(nListNum, 2, cstrTemp);
	cstrTemp.Format("%d", stData.nCodeCount-stData.nNodeCodeCount);
	m_List.SetItemText(nListNum, 3, cstrTemp);
	cstrTemp.Format("%d", stData.nNoteCount-stData.nNodeCodeCount);
	m_List.SetItemText(nListNum, 4, cstrTemp);
	cstrTemp.Format("%d", stData.nNullCount);
	m_List.SetItemText(nListNum, 5, cstrTemp);
	cstrTemp.Format("%d", stData.nNodeCodeCount);
	m_List.SetItemText(nListNum, 6, cstrTemp);

	// 总数统计
	m_nAllCount		+= stData.nAllCount;
	m_nNullCount	+= stData.nNullCount;
	m_nNoteCount	+= stData.nNoteCount-stData.nNodeCodeCount;
	m_nCodeCount	+= stData.nCodeCount-stData.nNodeCodeCount;
	m_nNoteCodeCount += stData.nNodeCodeCount;

}

// 清空数据
void CCodeCounterDlg::Clear()
{
	m_List.DeleteAllItems();
	m_nAllCount = 0;
	m_nNullCount = 0;
	m_nNoteCount = 0;
	m_nCodeCount = 0;
	m_nNoteCodeCount = 0;
	m_nCountNum = 0;
}

//////////////////////////////////////////////
//		写配置文件
//////////////////////////////////////////////
void CCodeCounterDlg::WriteIniFile()
{
	// 向D盘写入程序数据库连接ini文件信息，默认设置如下  
	CFileFind finder;   
	// 查找是否存在ini文件
	// 若不存在，则生成一个新的默认设置的ini文件
	CString cstrIniFile = _T(INI_FILE_NAME);
	BOOL ifFind = finder.FindFile(cstrIniFile);  
	if( !ifFind )
	{
		CFile file;  
		file.Open(cstrIniFile,CFile::modeCreate|CFile::modeWrite);
	}
	MAP_CONFIG::iterator iter = m_mapTypeConfig.begin();
	for (;iter != m_mapTypeConfig.end(); ++iter)
	{
		WritePrivateProfileString(iter->first, "Note", 
			iter->second.strNote.c_str(), (LPSTR)(LPCTSTR)cstrIniFile);
		WritePrivateProfileString(iter->first, "PairNoteBegin",
			iter->second.strPairNote.strBegin.c_str(), (LPSTR)(LPCTSTR)cstrIniFile);
		WritePrivateProfileString(iter->first, "PairNoteEnd", 
			iter->second.strPairNote.strEnd.c_str(), (LPSTR)(LPCTSTR)cstrIniFile);
	}
}

void CCodeCounterDlg::OnBnClickedBtnScan()
{	
	UpdateData(TRUE);	
	m_vctFileList.clear();
	if (m_nCounterType == CODE_COUNTER_FILE)		// 文件路径
	{
		CFileDialog dlg(true);
		if (dlg.DoModal() == IDOK)
		{
			CString cstrTemp = dlg.GetPathName();
			m_vctFileList.push_back(cstrTemp);
			m_cstrPath = cstrTemp;
		}
	} 
	else if(m_nCounterType == CODE_COUNTER_FOLDER)	// 文件夹路径
	{
		m_cstrPath = GetDirectory();
		FindFile((LPSTR)(LPCTSTR)m_cstrPath, m_vctFileList);
	}
	
/*	CString cstrTest = "   		 HI Test!";
	m_nNoteCount = cstrTest.GetLength();
	DelNullChar(cstrTest);
	m_nNoteCodeCount = cstrTest.GetLength();
	MessageBox(cstrTest);
	//*/
	UpdateData(false);
}

void CCodeCounterDlg::OnBnClickedRadioFolder()
{
	m_nCounterType = CODE_COUNTER_FOLDER;
}

void CCodeCounterDlg::OnBnClickedRadioFile()
{
	m_nCounterType = CODE_COUNTER_FILE;
}

void CCodeCounterDlg::OnBnClickedOk()
{		
	int nItemCount = m_lstBoxType.GetCount();
	int * pArray = new int[nItemCount];
	memset(pArray, 0, nItemCount*sizeof(int));
	int nChoosedNum = m_lstBoxType.GetSelItems(nItemCount, pArray);
	if (nChoosedNum == 0)
	{
		MessageBox("请选择至少一种文件类型");
	}
	set<CString> setTypeChoose;
	for (int i =0; i<nChoosedNum; ++i)
	{
		CString cstrTemp;
		m_lstBoxType.GetText(pArray[i], cstrTemp);
		setTypeChoose.insert(cstrTemp);
	}
	delete []pArray; pArray = NULL;
	
	if (m_vctFileList.size() > 0 && CCodeCounterMana::getInstance().bFinished())
	{				
		for (size_t i=0; i<m_vctFileList.size(); ++i)
		{
			// 得到文件名
			CString cstrType = m_vctFileList[i];
			int nIndex = cstrType.ReverseFind('.');			
			cstrType = cstrType.Mid(nIndex+1);
			set<CString>::iterator iterChoose = setTypeChoose.find(cstrType);
			if (iterChoose == setTypeChoose.end())
			{
				continue;
			}
			// 从配置中查到对应该文件类型
			MAP_CONFIG::iterator iterFind = m_mapTypeConfig.find(cstrType);
			if (iterFind != m_mapTypeConfig.end())
			{
				CCodeCounterMana::getInstance().AddTask(m_vctFileList[i].GetBuffer(0), iterFind->second);
			}			
		}
		bool bStarted = CCodeCounterMana::getInstance().Start();
		if (bStarted)
		{
			KillTimer(EM_TIMER_SHOW_LIST);
			Clear();
			SetTimer(EM_TIMER_SHOW_LIST, 50, NULL);
		}
		UpdateData(false);
	}
}


void CCodeCounterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == EM_TIMER_SHOW_LIST)
	{
		vector<stTaskDone> vctDone;
		CCodeCounterMana::getInstance().GetDoneTaskData(vctDone);
		for (size_t i=0; i<vctDone.size(); ++i)
		{
			ShowListData(_T(vctDone[i].strFilePath.c_str()), vctDone[i].sFileMsg);	
			++m_nCountNum;
		}
		m_cstrTest.Format("文件总数：%d 计算中...", m_nCountNum);
		if (CCodeCounterMana::getInstance().bFinished())
		{
			KillTimer(nIDEvent);
			CCodeCounterMana::getInstance().End();
			m_cstrTest.Format("文件总数：%d 已结束", m_nCountNum);
		}
		UpdateData(false);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CCodeCounterDlg::OnBnClickedButton1()
{
	UpdateData(true);
	// TODO: 在此添加控件通知处理程序代码
	if (m_cstrAddType.GetLength() > 0 && m_cstrAddNote.GetLength() > 0 &&
		m_cstrAddBeginNote.GetLength() > 0 && m_cstrAddEndNote.GetLength() > 0)
	{
		MAP_CONFIG::iterator iter = m_mapTypeConfig.find(m_cstrAddType);
		if (iter != m_mapTypeConfig.end())
		{
			iter->second.strNote = m_cstrAddNote.GetBuffer(0);
			iter->second.strPairNote.strBegin = m_cstrAddBeginNote.GetBuffer(0);
			iter->second.strPairNote.strEnd = m_cstrAddEndNote.GetBuffer(0);
			WriteIniFile();
			MessageBox("覆盖现有类型配置成功！");
		}
		else
		{
			stCounterConfig stTemp;
			stTemp.strNote = m_cstrAddNote.GetBuffer(0);
			stTemp.strPairNote.strBegin = m_cstrAddBeginNote.GetBuffer(0);
			stTemp.strPairNote.strEnd = m_cstrAddEndNote.GetBuffer(0);
			m_mapTypeConfig[m_cstrAddType] = stTemp;
			m_lstBoxType.AddString(m_cstrAddType);
			WriteIniFile();
			MessageBox("添加成功！");
		}		
	}
	else
	{
		MessageBox("左边字符不能有空！添加失败！");
	}
	UpdateData(false);
}
