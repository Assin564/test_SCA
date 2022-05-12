// CUSB_FIRMWARE_UPGRADE.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CUSB_FIRMWARE_UPGRADE.h"
#include "afxdialogex.h"

#include <fstream>

static         CUSB_FIRMWARE_UPGRADE* pthis;

// CUSB_FIRMWARE_UPGRADE 对话框

IMPLEMENT_DYNAMIC(CUSB_FIRMWARE_UPGRADE, CDialogEx)

CUSB_FIRMWARE_UPGRADE::CUSB_FIRMWARE_UPGRADE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_USB_FIRMWARE_UPGRADE_PAGE, pParent)
{
	//m_DeviceCount = 0;

}

CUSB_FIRMWARE_UPGRADE::~CUSB_FIRMWARE_UPGRADE()
{
}

void CUSB_FIRMWARE_UPGRADE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_INTERFACE, m_DeviceList);
	DDX_Control(pDX, IDC_BUTTON_OPEN_DEVICE, m_button_open_usb_device);
	DDX_Control(pDX, IDC_BUTTON_CLOSE_DEVICE, m_button_close_usb_device);
	DDX_Control(pDX, IDC_BUTTON_SELECT_FIRMWARE, m_button_select_firmware);
	DDX_Control(pDX, IDC_BUTTON_UPDATE_FIRMWARE, m_button_update_firmware);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_FILE_PATH, m_edit_firmware_path);
	DDX_Control(pDX, IDC_EDIT_UPDATE_PERCNT, m_edit_update_percent);
	DDX_Control(pDX, IDC_PROGRESS_UPDATE_FIRMWARE, m_process_update_process);
	DDX_Control(pDX, IDC_EDIT_UPDATE_INFORMATION, m_edit_update_information);
}


BEGIN_MESSAGE_MAP(CUSB_FIRMWARE_UPGRADE, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DEVICE, &CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonOpenDevice)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonClearLog)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FIRMWARE, &CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonSelectFirmware)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_DEVICE, &CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonCloseDevice)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_FIRMWARE, &CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonUpdateFirmware)
END_MESSAGE_MAP()


// CUSB_FIRMWARE_UPGRADE 消息处理程序

void CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonOpenDevice()
{
	// TODO: 在此添加控件通知处理程序代码
	UINT HIDIdx;
	TCHAR    displayVidPid[64];

	UpdateData(TRUE);

	CString StrTemp = 0;
	for (HIDIdx = 0; HIDIdx < m_DeviceCount; HIDIdx++)
	{
		StrTemp = m_UPSVIDPIDList[HIDIdx].StrInterface;
		if(StrTemp.Find(_T("HID")) !=  (-1))
		{
			if (m_HIDControl.OpenHIDUPS(m_UPSVIDPIDList[HIDIdx].VID, m_UPSVIDPIDList[HIDIdx].PID,1))
			{
				m_HIDControl.CloseHIDUPS();
				break;
			}
		}
	}

	StrTemp = m_UPSVIDPIDList[HIDIdx].StrInterface;
	if (StrTemp.Find(_T("HID")) != (-1))
	{
		//Open HID Interface	
		if (m_HIDControl.OpenHIDUPS(m_UPSVIDPIDList[HIDIdx].VID, m_UPSVIDPIDList[HIDIdx].PID,1))
		{
			AddMessage(_T("Find the HID Device OK!"));
			_stprintf_s(displayVidPid, _T("VID: %s, PID: %s"), m_UPSVIDPIDList[HIDIdx].StrVID, m_UPSVIDPIDList[HIDIdx].StrPID);
			AddMessage(displayVidPid);
			m_DeviceOpenStatus = DEVICE_STATUS_HIDOPENED;

			if ((0x051D == m_UPSVIDPIDList[HIDIdx].VID) && (0x0002 == m_UPSVIDPIDList[HIDIdx].PID))
			{
				m_bFlashAPCUSB = TRUE;
				m_bFlashInterrupt = FALSE;
			}
			else if ((0x00FF == m_UPSVIDPIDList[HIDIdx].VID) && (0x0000 == m_UPSVIDPIDList[HIDIdx].PID))
			{
				m_bFlashAPCUSB = FALSE;
				m_bFlashInterrupt = TRUE;
			}
			else
			{
				m_bFlashAPCUSB = FALSE;
				m_bFlashInterrupt = FALSE;
			}
			SetButtonState(1);

		}
		else
		{
			if (m_HIDControl.OpenHIDUPS(0x00FF, 0x0000,1))
			{
				AddMessage(_T("Find the HID Device OK!"));
				m_DeviceOpenStatus = DEVICE_STATUS_HIDOPENED;
				SetButtonState(1);
				m_bFlashInterrupt = TRUE;
			}
			else
			{
				AddMessage(_T("Can not find the HID Device!"));
				m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
				SetButtonState(0);
				m_bFlashInterrupt = FALSE;
			}
		}
	}
	else
	{
		AddMessage(_T("Can Not Open Ihis Interface,Don't Support!"));
		m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
		SetButtonState(0);
	}

}


BOOL CUSB_FIRMWARE_UPGRADE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	UPSDeviceListInit();
	OtherParameterInit();


	//m_UPSCommnunicationThread = CreateThread(0, 0, UsbFirmwareUpdateThread, NULL, 0, &m_Tid);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CUSB_FIRMWARE_UPGRADE::UPSDeviceListInit()
{
	UINT i;
	TCHAR FilePathName[MAX_PATH];
	TCHAR StrDeviceSectionName[256];
	TCHAR buffer[8] = {0};
	
	// get UPS list from USBDeviceList.ini file

	GetCurrentDirectory(_MAX_PATH, FilePathName);
	_tcscat_s(FilePathName, _T("\\USBDeviceList.ini"));

	m_DeviceCount = GetPrivateProfileInt(_T("DEVICELIST"), _T("DEVICECOUNT"), 0xFFFF, (LPCTSTR)FilePathName);

	if (m_DeviceCount == 0xFFFF)
	{
		// if can not get the ini file,Use the default setting
		m_DeviceCount = 1;
		m_UPSVIDPIDList[0].VID = 0x00FF;
		m_UPSVIDPIDList[0].PID = 0x0000;
		_tcscpy_s(m_UPSVIDPIDList[0].StrVID, _T("0x00FF"));
		_tcscpy_s(m_UPSVIDPIDList[0].StrPID, _T("0x0000"));
		_tcscpy_s(m_UPSVIDPIDList[0].StrModel, _T("Bootloader"));
		_tcscpy_s(m_UPSVIDPIDList[0].StrInterface, _T("HID"));
	}
	else
	{
		for (i = 0; i < m_DeviceCount; i++)
		{
			//get VID
			_tcscpy_s(StrDeviceSectionName, _T("DEVICE"));
			//_itoa(i, buffer, 10);
			_itow_s(i, buffer, 10);//int 以10进制转换为字符串

			_tcscat_s(StrDeviceSectionName, buffer);

			m_UPSVIDPIDList[i].VID = GetPrivateProfileInt(StrDeviceSectionName, _T("VID"), 0xFFFF, (LPCTSTR)FilePathName);
			GetPrivateProfileString(StrDeviceSectionName, _T("VID"), _T(""), m_UPSVIDPIDList[i].StrVID, 255, (LPCTSTR)FilePathName);

			//get PID
			m_UPSVIDPIDList[i].PID = GetPrivateProfileInt(StrDeviceSectionName, _T("PID"), 0xFFFF, (LPCTSTR)FilePathName);
			GetPrivateProfileString(StrDeviceSectionName, _T("PID"), _T(""), m_UPSVIDPIDList[i].StrPID, 255, (LPCTSTR)FilePathName);

			//get model 
			GetPrivateProfileString(StrDeviceSectionName, _T("MODEL"), _T(""), m_UPSVIDPIDList[i].StrModel, 255, (LPCTSTR)FilePathName);
			//get interface
			GetPrivateProfileString(StrDeviceSectionName, _T("INTERFACE"), _T(""), m_UPSVIDPIDList[i].StrInterface, 255, (LPCTSTR)FilePathName);
		}
	}

	//init the combo box
	for (i = 0; i < m_DeviceCount; i++)
	{
		m_DeviceList.AddString(m_UPSVIDPIDList[i].StrModel);
	}

	m_DeviceList.SetCurSel(0);
}

void CUSB_FIRMWARE_UPGRADE::OtherParameterInit()
{
	//init static CHIDTestProgramDlg point,Other thread will reference it
	pthis = this;
	////init log file handle
	//m_hLogFile = INVALID_HANDLE_VALUE;


	m_bReady = FALSE;
	//m_bCancelUpdate = FALSE;

	////auto send thread flag init
	//bInAutoThread = false;
	//bStopAutoSend = true;

	////for runonce
	//SetProp(m_hWnd, g_szPropName, g_hValue);

	m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
	//ComSel = 0;

	m_bFlashInterrupt = FALSE;
	m_bFlashAPCUSB = FALSE;

	pthis->m_edit_update_percent.SetWindowText(_T("0%"));

	m_process_update_process.SetRange(0,100);//设置进度条范围 0 - 100
	m_process_update_process.SetPos(0);//设置当前进度为 0

	SetButtonState(0);

}

void CUSB_FIRMWARE_UPGRADE::AddMessage(CString strMsg)
{
	int iLen;
	strMsg += "\r\n";
	iLen = m_edit_update_information.GetWindowTextLength();
	m_edit_update_information.SetSel(iLen, iLen, TRUE);
	m_edit_update_information.ReplaceSel(strMsg, FALSE);

}

void CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonClearLog()
{
	// TODO: 在此添加控件通知处理程序代码

	//清空文本框
	GetDlgItem(IDC_EDIT_UPDATE_INFORMATION)->SetWindowText(_T(""));
}

void CUSB_FIRMWARE_UPGRADE::SetButtonState(int State)
{
	switch (State)
	{
	//打开设备前的按键状态：
	case 0:
		m_button_open_usb_device.EnableWindow(TRUE);
		m_DeviceList.EnableWindow(TRUE);
		m_button_close_usb_device.EnableWindow(FALSE);
		m_button_select_firmware.EnableWindow(FALSE);
		m_button_update_firmware.EnableWindow(FALSE);
		break;
	//成功打开设备后的按键状态：
	case 1:
		m_button_open_usb_device.EnableWindow(FALSE);
		m_DeviceList.EnableWindow(FALSE);
		m_button_close_usb_device.EnableWindow(TRUE);
		m_button_select_firmware.EnableWindow(TRUE);
		m_button_update_firmware.EnableWindow(FALSE);
		break;
	//选择烧录固件后的按键状态：
	case 2:
		m_button_open_usb_device.EnableWindow(FALSE);
		m_DeviceList.EnableWindow(FALSE);
		m_button_close_usb_device.EnableWindow(TRUE);
		m_button_select_firmware.EnableWindow(TRUE);
		m_button_update_firmware.EnableWindow(TRUE);
		break;
	//烧录固件时的按键状态：
	case 3:
		m_button_open_usb_device.EnableWindow(FALSE);
		m_DeviceList.EnableWindow(FALSE);
		m_button_close_usb_device.EnableWindow(FALSE);
		m_button_select_firmware.EnableWindow(FALSE);
		m_button_update_firmware.EnableWindow(FALSE);
		break;
	default:
		break;
	}
}

unsigned char S19buff[100] = {};
unsigned char buff[15000] = {};

struct S19_BLOCK
{
	unsigned char DataLength;
	unsigned char FlashAddress[4];
	unsigned char Data[16];
}S19_Block;
//ASCII码转16进制
unsigned char ascii2value(unsigned char c)
{
	if ('0' <= c && c <= '9')
		return c - '0';
	else if ('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if ('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else abort();
}
//16进制转ASCIIS
unsigned char val2char(unsigned char c)
{
	if (0 <= c && c <= 9)
		return c + '0';
	else if (10 <= c && c <= 15)
		return c - 10 + 'A';
	else abort();
}

void S19AsciiStringToHexBuff(unsigned char* pHexbuff, unsigned char* pAsciiString, int Number)
{
	int i = 0;
	for (i = 0; i < Number; i++)
	{
		pHexbuff[i] = (ascii2value(pAsciiString[i * 2]) << 4) | (ascii2value(pAsciiString[i * 2 + 1]));
	}

	for (; i < 16; i++)
	{
		pHexbuff[i] = 0xFF;
	}
}

void CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonSelectFirmware()
{
	// TODO: 在此添加控件通知处理程序代码
	CString         pathName;
	TCHAR szPathStr[_MAX_PATH];

	AddMessage(_T("------------------------------------"));

	//get current path
	GetCurrentDirectory(_MAX_PATH, szPathStr);

	TCHAR szFilters[] = _T("USB FirmWare Files (*.S19)|*.S19|All Files (*.*)|*.*|");

	// Create an Open dialog; the default file name extension is ".S19".
	CFileDialog fileDlg(TRUE, _T("*.S19"), _T("*.S19"), OFN_CREATEPROMPT, szFilters, this);

	// Display the file dialog. When the user clicks OK, fileDlg.DoModal()
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		pathName = fileDlg.GetPathName();

		m_edit_firmware_path.SetWindowText(pathName);
		AddMessage(_T("The USB firmware file name is"));
		AddMessage(pathName);

		pthis->SetButtonState(2);
	}
	else
	{
		return;
	}


	
	//打开S19文件
	//读取文件
	pthis->AddMessage(_T("Create tmp.bin file..."));
	std::ifstream fin(pathName, std::ios::in);

	std::ofstream outfile;
	outfile.open("tmp.bin", std::ios::out);//先清除内容
	outfile.close();
	outfile.open("tmp.bin", std::ios::app | std::ios::binary);//需要以二进制的方式写入，不然在windows系统下写入0x0A时，会自动插入0x0D，从而导致数据错误

	FlashBinFileSize = 0;
	unsigned char EraseData[] = { 0x14, 0x08, 0x00, 0x50, 0x00, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
														0x14, 0x08, 0x00, 0x54, 0x00, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	
	/*先写入头两个芯片flash的扇区的首地址
		让芯片的Bootloader在烧录前先擦除这两个扇区的数据用以擦除地址0x080057F0的烧录完成标志的数据，表示开始烧录且没有烧录完*/
	outfile.write((char*)EraseData, sizeof(EraseData));

	//先写入大于0x08005800地址的数据到bin文件中
	int i = 0;
	fin.getline((char*)S19buff, sizeof(S19buff));//按行获取S19文件的数据
	while (S19buff[i] != 0)
	{
		if (S19buff[i] == 'S')
		{
			if (S19buff[i + 1] == '3')
			{
				S19AsciiStringToHexBuff(&S19_Block.DataLength, &S19buff[i + 2], 1);//一行S19的数据长度
				S19AsciiStringToHexBuff(S19_Block.FlashAddress, &S19buff[i + 4], 4);//获得烧录地址
				S19AsciiStringToHexBuff(S19_Block.Data, &S19buff[i + 12], S19_Block.DataLength - 5);//获得烧录数据

				int FlashAddress = S19_Block.FlashAddress[0] << 24 | S19_Block.FlashAddress[1] << 16 | S19_Block.FlashAddress[2] << 8 | S19_Block.FlashAddress[3];
				if (FlashAddress >= 0x08005800)//先写入大于0x08005800地址的数据到bin文件中
				{
					char Length = 0;
					Length = 0x14;
					outfile.write((char*)&Length, 1);
					outfile.write((char*)S19_Block.FlashAddress, 4);
					Length = 0x10;
					outfile.write((char*)&Length, 1);
					outfile.write((char*)S19_Block.Data, 16);

					FlashBinFileSize += 16;
				}

				fin.getline((char*)S19buff, sizeof(S19buff));
			}
			else if (S19buff[i + 1] == '7')//S19文件结束标志
			{
				break;
			}
		}
	}

	//再写入0x08005000 ~ 0x08005800地址的数据到bin文件中，因为0x080057F0地址记录了USB软体烧录完成标志，需要最后烧录
	fin.close();
	fin.open(pathName, std::ios::in);
	i = 0;
	fin.getline((char*)S19buff, sizeof(S19buff));
	while (S19buff[i] != 0)
	{
		if (S19buff[i] == 'S')
		{
			if (S19buff[i + 1] == '3')
			{
				S19AsciiStringToHexBuff(&S19_Block.DataLength, &S19buff[i + 2], 1);//一行S19的数据长度
				S19AsciiStringToHexBuff(S19_Block.FlashAddress, &S19buff[i + 4], 4);//获得烧录地址
				S19AsciiStringToHexBuff(S19_Block.Data, &S19buff[i + 12], S19_Block.DataLength - 5);//获得烧录数据

				int FlashAddress = S19_Block.FlashAddress[0] << 24 | S19_Block.FlashAddress[1] << 16 | S19_Block.FlashAddress[2] << 8 | S19_Block.FlashAddress[3];
				if (FlashAddress >= 0x08005000 && FlashAddress < 0x08005800)
				{
					char Length = 0;
					Length = 0x14;
					outfile.write((char*)&Length, 1);
					outfile.write((char*)S19_Block.FlashAddress, 4);
					Length = 0x10;
					outfile.write((char*)&Length, 1);
					outfile.write((char*)S19_Block.Data, 16);

					FlashBinFileSize += 16;
				}

				fin.getline((char*)S19buff, sizeof(S19buff));
			}
			else if (S19buff[i + 1] == '7')//S19文件结束标志
			{
				break;
			}
		}
	}

	pthis->AddMessage(_T("OK."));
	outfile.close();

	//读取二进制文件
	pthis->AddMessage(_T("Create tmp_text.txt file..."));
	std::ifstream fin2;
	fin2.open("tmp.bin", std::ios::in | std::ios::binary);
	fin2.read((char*)buff, 15000);

	std::ofstream outfile2("tmp_text.txt", std::ios::out);//| std::ios::trunc
	outfile2.close();
	outfile2.open("tmp_text.txt", std::ios::app | std::ios::binary);

	unsigned char temp[2] = { 0 };
	int k = 0;
	while (1)
	{
		temp[0] = val2char(buff[k] >> 4);
		temp[1] = val2char(buff[k] & 0x0F);

		outfile2.write((char*)temp, 2);

		k += 1;
		if (k % 22 == 0)//
		{
			if (buff[k] != 0x14)
			{
				break;
			}
			temp[0] = '\r';
			outfile2.write((char*)temp, 1);
		}
		if ((k % 22 == 1) || (k % 22 == 5) || (k % 22 == 6))
		{
			temp[0] = '-';
			outfile2.write((char*)temp, 1);
		}
	}

	pthis->AddMessage(_T("OK."));
	fin2.close();
	outfile2.close();

	CString StrTemp = 0;
	StrTemp.Format(_T("Total flash data:%d Bytes."), FlashBinFileSize);
	AddMessage(StrTemp);

	m_edit_update_percent.SetWindowText(_T("0%"));

	AddMessage(_T("------------------------------------"));

}

void CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonCloseDevice()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	switch (m_DeviceOpenStatus)
	{
	case DEVICE_STATUS_NOOPENED:
		AddMessage(_T("Please open device first!"));
		break;
	case DEVICE_STATUS_HIDOPENED:
		m_HIDControl.CloseHIDUPS();
		for (i = 0; i < MAX_HID_INPUTREPORT_LENGTH; i++)
		{
			InputReport[i] = 0;
		}
		AddMessage(_T("Close HID UPS OK!"));
		break;
	//case DEVICE_STATUS_RS232OPENED:
	//	m_SerControl.Close();
	//	AddMessage(_T("Close Serial UPS OK!"));
	//	break;
	default:
		AddMessage(_T("Please open device first!"));
		break;
	}

	SetButtonState(0);

	AddMessage(_T("------------------------------------"));
}

void CUSB_FIRMWARE_UPGRADE::OnBnClickedButtonUpdateFirmware()
{
	// TODO: 在此添加控件通知处理程序代码

	long	number;
	BYTE	bTxLen = 0;
	BOOL	FlashEnable = FALSE;
	int		i, j, k;
	BYTE	bWriteCmdPhase1;

	if (m_HIDControl.DeviceIsOpened() == FALSE)
	{
		AddMessage(_T("Please open device first!"));
		SetButtonState(0);
		return;
	}

	for (i = 0; i < 1; i++)
	{
		bTxLen = BYTE(pthis->m_HIDControl.ObtainFeatureReportLength()) - 1;

		bWriteCmdPhase1 = 0;

		//if (m_bFlashAPCUSB == TRUE)
		//{
		//	if (m_HIDControl.WriteAPCUPS("CPF070\r", bTxLen))
		//	{
		//		bWriteCmdPhase1 = 1;
		//	}
		//}
		//else
		//{
			if (m_HIDControl.WriteUPS("CPF070\r", 7))
			{
				bWriteCmdPhase1 = 1;
			}
		//}

		if (bWriteCmdPhase1 == 1)
		{
			for (j = 0; j < 3; j++)
			{
				Sleep(200);

				//if (m_bFlashAPCUSB == TRUE)
				//{
				//	m_HIDControl.ReadAPCUPS(InputReport, bTxLen + 1);
				//}
				//else
				{
					m_HIDControl.ReadUPS(InputReport, &number);
				}

				if (strstr(InputReport, "FLH") != NULL)
				{
					m_HIDControl.CloseHIDUPS();
					if (m_bFlashInterrupt == FALSE)
					{
						AddMessage(_T("Waiting for USB Ready #0"));
						Sleep(3500);
					}

					for (k = 0; k < 3; k++)
					{
						if (m_HIDControl.OpenHIDUPS(0x00FF, 0x0000,1))
						{
							AddMessage(_T("Find the HID Device OK!"));
							m_DeviceOpenStatus = DEVICE_STATUS_HIDOPENED;
							SetButtonState(1);
							m_bReady = TRUE;
							Sleep(3000);
							break;
						}
						else
						{
							Sleep(500);
						}
					}
				}

				if (m_bReady == TRUE)
				{
					break;
				}
			}

			if (m_bReady == TRUE)
			{
				break;
			}
		}
	}


	if (m_bReady == 0)
	{
		AddMessage(_T("USB no ready for firmware update#1"));
		AfxMessageBox(_T("USB Firmware update check ready error"), MB_OK);
		return;
	}

	if (m_bFlashInterrupt == FALSE)
	{
		if (m_HIDControl.WriteUPS("CPF070\r", 7))
		{
			m_HIDControl.ReadUPS(InputReport, &number);
			if (strstr(InputReport, "FLH") != NULL)
			{
				FlashEnable = TRUE;
			}
			else
			{
				AddMessage(_T("USB no ready for firmware update#3"));
				AfxMessageBox(_T("USB Firmware update check ready error"), MB_OK);
			}
		}
		else
		{
			AddMessage(_T("USB no ready for firmware update#2"));
			AfxMessageBox(_T("USB Firmware update check ready error"), MB_OK);
		}
	}
	else
	{
		FlashEnable = TRUE;
	}

	if (FlashEnable == TRUE)
	{
		AddMessage(_T("USB ready for firmware update"));

		SetButtonState(3);

		// create the update thread
		UpdateData(TRUE);
		m_UPSCommnunicationThread = CreateThread(0, 0x1000, UsbFirmwareUpdateThread, NULL, 0, &m_Tid);
	}
}

//USB固件烧录线程
/*
线程过程：
1.先把烧录文件.s19转换为可以烧录的.bin文件
2.开始烧录：把.bin的数据逐条发给通讯板的Bootloader
*/
DWORD __stdcall CUSB_FIRMWARE_UPGRADE::UsbFirmwareUpdateThread(PVOID nouse)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(500);

	pthis->AddMessage(_T("------------------------------------"));

	int Index = 0;
	unsigned char TxData[32] = {0};
	unsigned char bCheckSum = 0;
	BOOL	UpdateOK = 0;
	long	number = 0;
	
	int  FlashAddr;
	int FileByte = 0;
	int TotalFileByte = 0; 
	int FlashPercent = 0;
	int FlashPercentOld = 0;

	CString StrTemp = 0;

	TotalFileByte = pthis->FlashBinFileSize;

	pthis->m_process_update_process.SetPos(0);
	while (1)
	{
		memset(TxData, '\0', 32);
		//一帧数据的长度
		TxData[0] = 23;
		bCheckSum ^= TxData[0];
		//要写入数据的芯片的地址
		TxData[1] = buff[Index + 1];
		bCheckSum ^= TxData[1];
		TxData[2] = buff[Index + 2];
		bCheckSum ^= TxData[2];
		TxData[3] = buff[Index + 3];
		bCheckSum ^= TxData[3];
		TxData[4] = buff[Index + 4];
		bCheckSum ^= TxData[4];
		//要写入芯片的数据长度
		TxData[5] = buff[Index + 5];
		bCheckSum ^= TxData[5];
		//16个字节数据
		for (int j = 0; j < 16; j++)
		{
			TxData[j + 6] = buff[Index + j + 6];
			bCheckSum ^= TxData[j + 6];
		}
		TxData[22] = bCheckSum;
		bCheckSum = 0;

		FlashAddr = TxData[1] * 0x1000000
			+ TxData[2] * 0x10000
			+ TxData[3] * 0x100
			+ TxData[4];
		FileByte += 16;
		
		if ((FlashAddr >= APP_LOADED_ID_PAGE_START_ADR) && (FileByte <= MAX_FIRMWARE_BYTE))
		{
			if (pthis->m_HIDControl.WriteUPS((char*)TxData, 23))
			{
				pthis->m_HIDControl.ReadUPS(pthis->InputReport, &number);

				if (pthis->InputReport[0] == 'A')
				{
					//计算并显示更新程序的进度
					if (TotalFileByte > 0)
					{
						FlashPercent = FileByte *100 / TotalFileByte;
						if (FlashPercentOld < FlashPercent)
						{
							StrTemp.Format(_T(" %d %%"), FlashPercent);
							pthis->m_edit_update_percent.SetWindowText(StrTemp);
							FlashPercentOld = FlashPercent;

							/*pthis->m_process_update_process.SetStep(1);
							pthis->m_process_update_process.StepIt();*/

							pthis->m_process_update_process.SetPos(FlashPercent);
						}
					}

					StrTemp.Format(_T(" 0x%x Data OK."), FlashAddr);
					pthis->AddMessage(StrTemp);
					
					memset(pthis->InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);
					if (((buff[Index + 6] == 0x45) && (buff[Index + 7] == 0x44) && (buff[Index + 8] == 0x47) && (buff[Index + 9] == 0x45) && (buff[Index + 20] == 0x5A) && (buff[Index + 21] == 0x5A)))
					{
						StrTemp.Format(_T("Total flash data：%d bytes."), FileByte - 32);
						pthis->AddMessage(StrTemp);
						UpdateOK = TRUE;//结尾
						break;
					}
					else
					{
						Index += 22;
					}
				}
				else
				{
					pthis->AddMessage(_T("Send the USB firmware data Err"));
					UpdateOK = FALSE;
					memset(pthis->InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);
					break;
				}
			}
			else
			{
				pthis->AddMessage(_T("Write report error"));
				UpdateOK = FALSE;
				break;
			}
		}
		else
		{
			pthis->AddMessage(_T("Flash address error"));
			UpdateOK = FALSE;
			break;
		}
	}

	if (UpdateOK == TRUE)
	{
		TxData[0] = 0x0A;
		TxData[1] = 0x00;
		TxData[2] = 0x00;
		TxData[3] = 0x00;
		TxData[4] = 0x00;
		TxData[5] = 0x03;
		TxData[6] = 'E';
		TxData[7] = 'O';
		TxData[8] = 'F';
		TxData[9] = 0x45;

		if (pthis->m_HIDControl.WriteUPS((char*)TxData, 10))
		{
			pthis->AddMessage(_T("Send EOF Command OK"));
		}
		else
		{
			pthis->AddMessage(_T("Send EOF Command ERROR!"));
		}
	}
	pthis->AddMessage(_T("------------------------------------"));

	pthis->SetButtonState(2);


}


