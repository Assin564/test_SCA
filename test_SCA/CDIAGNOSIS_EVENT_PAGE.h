#pragma once


// CDIAGNOSIS_EVENT_PAGE 对话框

class CDIAGNOSIS_EVENT_PAGE : public CDialogEx
{
	DECLARE_DYNAMIC(CDIAGNOSIS_EVENT_PAGE)

public:
	CDIAGNOSIS_EVENT_PAGE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDIAGNOSIS_EVENT_PAGE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIAGNOSIS_EVENT_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
