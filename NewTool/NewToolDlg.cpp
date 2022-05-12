
// NewToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "NewTool.h"
#include "NewToolDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNewToolDlg 对话框



CNewToolDlg::CNewToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEWTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNewToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


#pragma warning( push )
#pragma warning( disable : 26454 )

BEGIN_MESSAGE_MAP(CNewToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CNewToolDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()

#pragma warning( pop )

// CNewToolDlg 消息处理程序

BOOL CNewToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	CRect tabRect;    // 标签控件客户区的Rect   
	// 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 25;
	tabRect.bottom -= 1;

	//m_tab.InsertItem(0, _T("BasicCurrentInformation"));    // 插入第一个标签
	//m_tab.InsertItem(1, _T("DiagnosisEvent"));    // 插入第二个标签

	//设置标签显示的内容
	m_tab.InsertItem(0, _T("UPS 信息"));    // 插入第一个标签
	m_tab.InsertItem(1, _T("错误分析和事件"));    // 插入第二个标签
	m_tab.InsertItem(2, _T("USB 固件更新"));    // 插入第三个标签
	m_tab.InsertItem(3, _T("UPS 固件更新"));    // 插入第四个标签


	Page_BasicCurrentInformation.Create(IDD_BASIC_CURRENT_INFORMATION_PAGE, &m_tab);    // 创建第一个标签页   
	Page_DiagnosisEvent.Create(IDD_DIAGNOSIS_EVENT_PAGE, &m_tab); // 创建第二个标签页  
	Page_UsbFirmwareGrade.Create(IDD_USB_FIRMWARE_UPGRADE_PAGE, &m_tab);//创建第三个标签页
	Page_UpsFirmwareGrade.Create(IDD_UPS_FIRMWARE_UPGRADE_PAGE, &m_tab);//创建第四个标签页
	

	// 根据调整好的tabRect放置t1子对话框，并设置为显示   
	Page_BasicCurrentInformation.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	// 根据调整好的tabRect放置t2子对话框，并设置为隐藏   
	Page_DiagnosisEvent.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	//根据调整好的tabRect放置t3子对话框，并设置为隐藏   
	Page_UsbFirmwareGrade.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
	//根据调整好的tabRect放置t4子对话框，并设置为隐藏   
	Page_UpsFirmwareGrade.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNewToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNewToolDlg::OnPaint()
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
HCURSOR CNewToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CNewToolDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CRect tabRect;    // 标签控件客户区的Rect   

	// 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 20;
	tabRect.bottom -= 1;

	//根据当前标签页显示对应的界面
	int nSel = m_tab.GetCurSel();
	CWnd* ps[] = { &Page_BasicCurrentInformation,&Page_DiagnosisEvent,&Page_UsbFirmwareGrade,&Page_UpsFirmwareGrade };
	for (int i = 0; i < sizeof(ps) / sizeof(ps[0]); i++)
		ps[i]->ShowWindow(i == nSel ? SW_SHOW : SW_HIDE);
	*pResult = 0;

}
