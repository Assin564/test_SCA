// CDiagnosisAndEventRecordPage.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CDiagnosisAndEventRecordPage.h"
#include "afxdialogex.h"


// CDiagnosisAndEventRecordPage 对话框

IMPLEMENT_DYNAMIC(CDiagnosisAndEventRecordPage, CMFCPropertyPage)

CDiagnosisAndEventRecordPage::CDiagnosisAndEventRecordPage(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_DIAGNOSIS_EVENT_PAGE, pParent)
{

}

CDiagnosisAndEventRecordPage::~CDiagnosisAndEventRecordPage()
{
}

void CDiagnosisAndEventRecordPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDiagnosisAndEventRecordPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CDiagnosisAndEventRecordPage 消息处理程序
