#pragma once


// CDiagnosisAndEventRecordPage 对话框

class CDiagnosisAndEventRecordPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CDiagnosisAndEventRecordPage)

public:
	CDiagnosisAndEventRecordPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDiagnosisAndEventRecordPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIAGNOSIS_EVENT_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
