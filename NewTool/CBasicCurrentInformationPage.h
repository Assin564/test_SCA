#pragma once


// CBasicCurrentInformationPage 对话框

class CBasicCurrentInformationPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CBasicCurrentInformationPage)

public:
	CBasicCurrentInformationPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBasicCurrentInformationPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BASIC_CURRENT_INFORMATION_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
