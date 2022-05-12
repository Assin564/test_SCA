#pragma once

#include "source\SerControl.h"
#include "source\HIDControl.h"
#include "source\HScrollListBox.h"

extern "C"
{
#include "source\hidsdi.h"
#include "setupapi.h"
}

//device opened status define
#define   DEVICE_STATUS_NOOPENED        0
#define   DEVICE_STATUS_HIDOPENED       1
#define   DEVICE_STATUS_RS232OPENED     2

#define   MAX_UPS_COMMAND_COUNT         13
#define   MAX_UPS_LIST_COUNT            128


#define APP_LOADED_ID_PAGE_START_ADR		0x08005000
#define APP_LOADED_ID_PAGE_END_ADR			0x080057FF
#define APP_LOADED_ID_IAP_MARK_ADR			0x080057F0

#define MAX_FIRMWARE_BYTE 0x1B000

struct UPSLIST
{
	UINT VID;
	UINT PID;
	TCHAR StrVID[16];
	TCHAR StrPID[16];
	TCHAR StrModel[64];
	TCHAR StrInterface[64];
};


// CUSB_FIRMWARE_UPGRADE 对话框

class CUSB_FIRMWARE_UPGRADE : public CDialogEx
{
	DECLARE_DYNAMIC(CUSB_FIRMWARE_UPGRADE)

public:
	CUSB_FIRMWARE_UPGRADE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUSB_FIRMWARE_UPGRADE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USB_FIRMWARE_UPGRADE_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

	//init function
	void UPSDeviceListInit();

	void OtherParameterInit();

	void AddMessage(CString strMsg);//在显示文本框后面插入文本

	void SetButtonState(int State);//设置按键能否操作

protected:
	BOOL            m_bFlashInterrupt;
	BOOL            m_bFlashAPCUSB;

	UINT            m_DeviceCount;
	UPSLIST         m_UPSVIDPIDList[MAX_UPS_LIST_COUNT];
	CHIDControl     m_HIDControl;

	int             m_DeviceOpenStatus;

	char            InputReport[MAX_HID_INPUTREPORT_LENGTH];  //HID read buffer

	BOOL            m_bReady;

	HANDLE m_UPSCommnunicationThread;
	ULONG  m_Tid;
	static DWORD __stdcall UsbFirmwareUpdateThread(PVOID nouse);

	int FlashBinFileSize;//记录要烧录到芯片的数据的大小

public:
	afx_msg void OnBnClickedButtonOpenDevice();
	//下拉框：显示设备列表
	CComboBox m_DeviceList;
	// 按键：打开设备
	CButton m_button_open_usb_device;
	// 按键：关闭设备
	CButton m_button_close_usb_device;
	// 按键：选择要烧录的固件
	CButton m_button_select_firmware;
	// 按键：开始烧录固件
	CButton m_button_update_firmware;
	// 显示框：显示选择的固件的路径
	CEdit m_edit_firmware_path;
	// 显示框：显示烧录进度百分比
	CEdit m_edit_update_percent;
	// 进度框：显示烧录进度
	CProgressCtrl m_process_update_process;
	// 显示框：显示烧录过程的细节部分
	//CHScrollListBox m_edit_update_information;
	CEdit m_edit_update_information;




	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonClearLog();
	afx_msg void OnBnClickedButtonSelectFirmware();
	afx_msg void OnBnClickedButtonCloseDevice();
	afx_msg void OnBnClickedButtonUpdateFirmware();
};


