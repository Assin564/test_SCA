// CBASIC_CURRENT_PAGE.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CBASIC_CURRENT_PAGE.h"
#include "afxdialogex.h"


// CBASIC_CURRENT_PAGE 对话框

IMPLEMENT_DYNAMIC(CBASIC_CURRENT_PAGE, CDialogEx)

CBASIC_CURRENT_PAGE::CBASIC_CURRENT_PAGE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BASIC_CURRENT_INFORMATION_PAGE, pParent)
{

}

CBASIC_CURRENT_PAGE::~CBASIC_CURRENT_PAGE()
{
}

void CBASIC_CURRENT_PAGE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBASIC_CURRENT_PAGE, CDialogEx)
END_MESSAGE_MAP()


// CBASIC_CURRENT_PAGE 消息处理程序
