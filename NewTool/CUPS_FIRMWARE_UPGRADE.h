#pragma once

#include "source\SerControl.h"
#include "source\HIDControl.h"
#include "source\HScrollListBox.h"

struct UPS_LIST
{
	UINT VID;
	UINT PID;
	TCHAR SflashCMD[16];
	TCHAR SCodePretect[8];
	UINT CommunicateRate;
	UINT ProgrogramRate;
	UINT RawDataSize;
	UINT HeadLine;
	UINT RomHead;
	UINT RomTail;
	UINT Protect;
	UINT IntStart;
	UINT IntEnd;
	UINT PageSize;
	UINT IapStart;
	UINT IapEnd;
	UINT IapMark;
};

#define	MAX_UPS_LIST_COUNT					128

#define	INTERFACE_TYPE_USB						1
#define	INTERFACE_TYPE_RS232					2

//
#define	FIRMWARE_TYPE_UPS						1
#define	FIRMWARE_TYPE_USB						2

#define	DEVICE_STATUS_NOOPENED			0
#define	DEVICE_STATUS_HIDOPENED			1
#define	DEVICE_STATUS_RS232OPENED		2

#define TX_UPS_FRAME_LENGTH					70 //一帧烧录数据的总长度
#define TX_UPS_FLASH_LENGTH					64//一帧烧录数据中需要烧进芯片的数据的长度


#define UPSFlashStartAddress						0x004000
#define UPSFlashOkFlagStartAddress			0x004400
#define UPSFlashOkFlagEndAddress				0x004440
#define UPSMaxAddress									0x200000

// CUPS_FIRMWARE_UPGRADE 对话框

class CUPS_FIRMWARE_UPGRADE : public CDialogEx
{
	DECLARE_DYNAMIC(CUPS_FIRMWARE_UPGRADE)

public:
	CUPS_FIRMWARE_UPGRADE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUPS_FIRMWARE_UPGRADE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPS_FIRMWARE_UPGRADE_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	void AddMessage(CString strMsg);//在显示文本框后面插入文本
	void OnButtonOpen(void);//打开串口或者USB接口

	int m_InterfaceType;//烧录使用的接口
	int m_FirmwareType;//烧录的软体的类型，如UPS软体，USB软体

protected:
	int					FlashBinFileSize;//记录要烧录到芯片的数据的大小
	CHIDControl	m_HIDControl;
	CSerControl		m_SerControl;
	UINT				ComSel;
	BOOL				m_bReady;

	UINT				m_DeviceCount;
	UPS_LIST			m_UPSVIDPIDList[MAX_UPS_LIST_COUNT];
	char					InputReport[MAX_HID_INPUTREPORT_LENGTH];  //HID read buffer

	void					CvrtToCByteArray(BYTE* byArray, BYTE* pByte, int nSize);

	int					m_DeviceOpenStatus;
	int					m_iState;
	int					m_nRetransmitCount;

	HANDLE          m_FirmwareUpdateTread;
	HANDLE          m_UPSFirmwareUpdateComThread;
	ULONG			m_Tid;

	static DWORD __stdcall UPSFirmwareUpdateByUSBThread(PVOID nouse);
	static DWORD __stdcall UPSFirmwareUpdateByCOMThread(PVOID nouse);

	BOOL CheckSerUPSReadyAndUpdate(PVOID nouse, int cnt);

	void UPSDeviceListInit();

public:
	virtual BOOL OnInitDialog();
	// 下拉框，显示烧录用的接口
	CComboBox m_select_ups_interface;
	// 按键：选择UPS烧录文件
	CButton m_ButtonUpsFile;
	// 按键：开始更新固件
	CButton m_ButtonUpsFirmware;
	// 编辑框：显示烧录文件的路径
	CEdit m_EditFilePath;
	// 编辑框：显示UPS烧录文件的烧录进度百分比
	CEdit m_EditUpsFileUpdateProgressPercent;
	// 进度条：UPS烧录文件的烧录进度
	CProgressCtrl m_ProgressUpsProgress;
	// 编辑框：显示UPS烧录过程时的信息
	CEdit m_EditUpsUpdateInformation;
	afx_msg void OnBnClickedButtonUpsFile2();
	afx_msg void OnBnClickedButtonUpsUpdateFirmware();
};
