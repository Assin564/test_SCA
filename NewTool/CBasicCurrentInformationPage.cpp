// CBasicCurrentInformationPage.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CBasicCurrentInformationPage.h"
#include "afxdialogex.h"


// CBasicCurrentInformationPage 对话框

IMPLEMENT_DYNAMIC(CBasicCurrentInformationPage, CMFCPropertyPage)

CBasicCurrentInformationPage::CBasicCurrentInformationPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_BASIC_CURRENT_INFORMATION_PAGE, pParent)
{

}

CBasicCurrentInformationPage::~CBasicCurrentInformationPage()
{
}

void CBasicCurrentInformationPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicCurrentInformationPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CBasicCurrentInformationPage 消息处理程序
