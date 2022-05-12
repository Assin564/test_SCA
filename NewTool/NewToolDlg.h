
// NewToolDlg.h: 头文件
//

#pragma once

#include "CBASIC_CURRENT_PAGE.h"

#include "CDIAGNOSIS_EVENT_PAGE.h"

#include	"CUSB_FIRMWARE_UPGRADE.h"

#include	"CUPS_FIRMWARE_UPGRADE.h"


// CNewToolDlg 对话框
class CNewToolDlg : public CDialogEx
{
// 构造
public:
	CNewToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

	CBASIC_CURRENT_PAGE Page_BasicCurrentInformation;
	CDIAGNOSIS_EVENT_PAGE Page_DiagnosisEvent;
	CUSB_FIRMWARE_UPGRADE Page_UsbFirmwareGrade;
	CUPS_FIRMWARE_UPGRADE Page_UpsFirmwareGrade;


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEWTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
};
