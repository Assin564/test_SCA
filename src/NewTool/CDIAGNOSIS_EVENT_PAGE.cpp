// CDIAGNOSIS_EVENT_PAGE.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CDIAGNOSIS_EVENT_PAGE.h"
#include "afxdialogex.h"


// CDIAGNOSIS_EVENT_PAGE 对话框

IMPLEMENT_DYNAMIC(CDIAGNOSIS_EVENT_PAGE, CDialogEx)

CDIAGNOSIS_EVENT_PAGE::CDIAGNOSIS_EVENT_PAGE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIAGNOSIS_EVENT_PAGE, pParent)
{

}

CDIAGNOSIS_EVENT_PAGE::~CDIAGNOSIS_EVENT_PAGE()
{
}

void CDIAGNOSIS_EVENT_PAGE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDIAGNOSIS_EVENT_PAGE, CDialogEx)
END_MESSAGE_MAP()


// CDIAGNOSIS_EVENT_PAGE 消息处理程序
