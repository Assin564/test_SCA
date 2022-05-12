#pragma once


// CBASIC_CURRENT_PAGE 对话框

class CBASIC_CURRENT_PAGE : public CDialogEx
{
	DECLARE_DYNAMIC(CBASIC_CURRENT_PAGE)

public:
	CBASIC_CURRENT_PAGE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBASIC_CURRENT_PAGE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BASIC_CURRENT_INFORMATION_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
