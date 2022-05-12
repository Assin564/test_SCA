// CUPS_FIRMWARE_UPGRADE.cpp: 实现文件
//

#include "pch.h"
#include "NewTool.h"
#include "CUPS_FIRMWARE_UPGRADE.h"
#include "afxdialogex.h"

#include <fstream>

#define RS232TryCnt		3
#define USBTryCnt		3

UINT USB_VID = 0x10AF;
UINT USB_PID = 0x0002;
char FLASH_CMD[16] = "CPEDGE#";
long COMMUNICAT_RATE = 2400;
long PROGRAM_RATE = 9600;
BYTE RAW_DATA_SIZE = 0x20;
long RAW_HEAD_LINE = 0x01;
long RAW_START_ADDR = 0x4000;
long RAW_END_ADDR = 0x23FFF;
long RAW_INT_START_ADDR = 0xFFDC;
long RAW_INT_END_ADDR = 0xFFFC;
long RAW_PAGE_SIZE = 0x8000;
long IAP_END_ADDR = 0xFFFF;
long IAP_START_ADDR = 0xC000;
long RAW_MASK_ADDR = 0xBFE0;


static         CUPS_FIRMWARE_UPGRADE* pthis;

// CUPS_FIRMWARE_UPGRADE 对话框

IMPLEMENT_DYNAMIC(CUPS_FIRMWARE_UPGRADE, CDialogEx)

CUPS_FIRMWARE_UPGRADE::CUPS_FIRMWARE_UPGRADE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UPS_FIRMWARE_UPGRADE_PAGE, pParent)
{

}

CUPS_FIRMWARE_UPGRADE::~CUPS_FIRMWARE_UPGRADE()
{
}

void CUPS_FIRMWARE_UPGRADE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_UPS_INTERFACE, m_select_ups_interface);
	DDX_Control(pDX, IDC_BUTTON_UPS_FILE2, m_ButtonUpsFile);
	DDX_Control(pDX, IDC_BUTTON_UPS_UPDATE_FIRMWARE, m_ButtonUpsFirmware);
	DDX_Control(pDX, IDC_EDIT_UPS_FILE, m_EditFilePath);
	DDX_Control(pDX, IDC_EDIT_UPS_PROGRESS, m_EditUpsFileUpdateProgressPercent);
	DDX_Control(pDX, IDC_PROGRESS_UPS_PROGRESS, m_ProgressUpsProgress);
	DDX_Control(pDX, IDC_EDIT_UPS_UPDATE_INFORMATION, m_EditUpsUpdateInformation);
}


BEGIN_MESSAGE_MAP(CUPS_FIRMWARE_UPGRADE, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UPS_FILE2, &CUPS_FIRMWARE_UPGRADE::OnBnClickedButtonUpsFile2)
	ON_BN_CLICKED(IDC_BUTTON_UPS_UPDATE_FIRMWARE, &CUPS_FIRMWARE_UPGRADE::OnBnClickedButtonUpsUpdateFirmware)
END_MESSAGE_MAP()


// CUPS_FIRMWARE_UPGRADE 消息处理程序


BOOL CUPS_FIRMWARE_UPGRADE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int idx = 0;
	m_select_ups_interface.InsertString(idx++, _T("USB"));
	m_select_ups_interface.InsertString(idx++, _T("COM1"));
	m_select_ups_interface.InsertString(idx++, _T("COM2"));
	m_select_ups_interface.InsertString(idx++, _T("COM4"));
	m_select_ups_interface.InsertString(idx++, _T("COM5"));
	m_select_ups_interface.InsertString(idx++, _T("COM6"));
	m_select_ups_interface.InsertString(idx++, _T("COM7"));
	m_select_ups_interface.InsertString(idx++, _T("COM8"));
	m_select_ups_interface.InsertString(idx++, _T("COM9"));
	m_select_ups_interface.InsertString(idx++, _T("COM10"));
	m_select_ups_interface.InsertString(idx++, _T("COM11"));
	m_select_ups_interface.InsertString(idx++, _T("COM12"));
	m_select_ups_interface.InsertString(idx++, _T("COM13"));
	m_select_ups_interface.InsertString(idx++, _T("COM14"));
	m_select_ups_interface.InsertString(idx++, _T("COM15"));
	m_select_ups_interface.InsertString(idx++, _T("COM16"));
	m_select_ups_interface.InsertString(idx++, _T("COM17"));
	m_select_ups_interface.InsertString(idx++, _T("COM18"));
	m_select_ups_interface.InsertString(idx++, _T("COM19"));
	m_select_ups_interface.InsertString(idx++, _T("COM20"));
	m_select_ups_interface.InsertString(idx++, _T("COM21"));
	m_select_ups_interface.InsertString(idx++, _T("COM22"));
	m_select_ups_interface.InsertString(idx++, _T("COM23"));
	m_select_ups_interface.InsertString(idx++, _T("COM24"));
	m_select_ups_interface.InsertString(idx++, _T("COM25"));
	m_select_ups_interface.InsertString(idx++, _T("COM26"));
	m_select_ups_interface.InsertString(idx++, _T("COM27"));
	m_select_ups_interface.InsertString(idx++, _T("COM28"));
	m_select_ups_interface.InsertString(idx++, _T("COM29"));
	m_select_ups_interface.InsertString(idx++, _T("COM30"));
	m_select_ups_interface.SetCurSel(0);


	m_ButtonUpsFile.EnableWindow(TRUE);
	m_ButtonUpsFirmware.EnableWindow(FALSE);

	UPSDeviceListInit();

	m_iState = 0;

	pthis = this;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


unsigned char UpsBinFileBuff[0x20000] = {0};

unsigned char UpsFlashBuff[0x26000] = {0};

void CUPS_FIRMWARE_UPGRADE::AddMessage(CString strMsg)
{
	int iLen;
	strMsg += "\r\n";
	iLen = m_EditUpsUpdateInformation.GetWindowTextLength();
	m_EditUpsUpdateInformation.SetSel(iLen, iLen, TRUE);
	m_EditUpsUpdateInformation.ReplaceSel(strMsg, FALSE);

}

//int Remainder = 0;
void CUPS_FIRMWARE_UPGRADE::OnBnClickedButtonUpsFile2()
{
	CString         pathName;
	TCHAR szPathStr[_MAX_PATH];
	// TODO: 在此添加控件通知处理程序代码

	//get current path
	GetCurrentDirectory(_MAX_PATH, szPathStr);

	TCHAR szFilters[] = _T("Firmware Files (*.S19;*.mot;*.Bin)|*.S19;*.mot;*.Bin|All Files (*.*.*)|*.*.*|");

	// Create an Open dialog; the default file name extension is ".S19".
	CFileDialog fileDlg(TRUE, _T("*.S19;*.mot;*.Bin"), _T("*.S19;*.mot;*.Bin"), OFN_CREATEPROMPT, szFilters, this);

	if (fileDlg.DoModal() == IDOK)
	{
		m_EditUpsUpdateInformation.SetWindowTextW(_T(""));//先清空编辑框
		AddMessage(_T("------------------------------------"));
		pathName = fileDlg.GetPathName();
		m_EditFilePath.SetWindowText(pathName);
		AddMessage(_T("The UPS firmware file name is"));
		AddMessage(pathName);

		m_ButtonUpsFirmware.EnableWindow(TRUE);
	}
	else
	{
		return;
	}

	//打开S19文件
	//读取文件
	AddMessage(_T("Create UpsFlashTmp.bin file..."));
	//std::ifstream fin(pathName, std::ios::in);
	CFile readfile;
	readfile.Open(pathName, CFile::modeReadWrite | CFile::typeBinary);//打开二进制文件
	unsigned int  BinFileLength = 0;
	BinFileLength = readfile.GetLength();

	CString StrTemp = 0;
	StrTemp.Format(_T("Total bin data:%d Bytes."), BinFileLength);
	AddMessage(StrTemp);
	readfile.Read(UpsBinFileBuff, BinFileLength);
	readfile.Close();

	std::ofstream outfile;
	outfile.open("UpsFlashTmp.bin", std::ios::out);//先清除内容
	outfile.close();
	outfile.open("UpsFlashTmp.bin", std::ios::app | std::ios::binary);//需要以二进制的方式写入，不然在windows系统下写入0x0A时，会自动插入0x0D，从而导致数据错误

	
	unsigned char EraseData[TX_UPS_FRAME_LENGTH] = {0};

	FlashBinFileSize = 0;
	int FlashAddressIndex = UPSFlashStartAddress;
	//先写入0x004000 ~ 0x0043FF的数据
	while (FlashAddressIndex < UPSFlashOkFlagStartAddress)
	{
		unsigned char CheckSum = 0;
		//写入地址
		EraseData[0] = TX_UPS_FRAME_LENGTH;
		CheckSum ^= TX_UPS_FRAME_LENGTH;
		EraseData[1] = (FlashAddressIndex >> 16) & 0xFF;
		CheckSum ^= EraseData[1];
		EraseData[2] = (FlashAddressIndex >> 8) & 0xFF;
		CheckSum ^= EraseData[2];
		EraseData[3] = (FlashAddressIndex) & 0xFF;
		CheckSum ^= EraseData[3];
		EraseData[4] = TX_UPS_FLASH_LENGTH;
		CheckSum ^= TX_UPS_FLASH_LENGTH;
		//写入数据
		
		unsigned char i = 0;
		for (i = 5; i < TX_UPS_FRAME_LENGTH - 1; i++)
		{
			EraseData[i] = UpsBinFileBuff[FlashAddressIndex];
			CheckSum ^= UpsBinFileBuff[FlashAddressIndex];
			FlashAddressIndex++;
		}
		EraseData[i] = CheckSum;
		outfile.write((char*)EraseData, (std::streamsize)TX_UPS_FRAME_LENGTH);
	}
	//再写入0x004440及以后的数据
	FlashAddressIndex = UPSFlashOkFlagEndAddress;
	while (FlashAddressIndex < BinFileLength)
	{
		unsigned char CheckSum = 0;
		//写入地址
		EraseData[0] = TX_UPS_FRAME_LENGTH;
		CheckSum ^= TX_UPS_FRAME_LENGTH;
		EraseData[1] = (FlashAddressIndex >> 16) & 0xFF;
		CheckSum ^= EraseData[1];
		EraseData[2] = (FlashAddressIndex >> 8) & 0xFF;
		CheckSum ^= EraseData[2];
		EraseData[3] = (FlashAddressIndex) & 0xFF;
		CheckSum ^= EraseData[3];
		EraseData[4] = TX_UPS_FLASH_LENGTH;
		CheckSum ^= TX_UPS_FLASH_LENGTH;
		//写入数据
		
		unsigned char i = 0;
		for (i = 5; i < TX_UPS_FRAME_LENGTH - 1; i++)
		{
			if (FlashAddressIndex >= BinFileLength)//最后一帧数据需要在结尾加上0xFFL来补齐一帧长度的数据：TX_UPS_FRAME_LENGTH
			{
				EraseData[i] = 0xFF;
				CheckSum ^= 0xFF;
				FlashBinFileSize++;
			}
			else
			{
				EraseData[i] = UpsBinFileBuff[FlashAddressIndex];
				CheckSum ^= UpsBinFileBuff[FlashAddressIndex];
			}
			FlashAddressIndex++;
		}
		EraseData[i] = CheckSum;
		outfile.write((char*)EraseData, (std::streamsize)TX_UPS_FRAME_LENGTH);

	}
	//最后写入0x004400 ~ 0x004440的数据，该数据段包含烧录完成标志，所以要最后写入
	FlashAddressIndex = UPSFlashOkFlagStartAddress;
	while (FlashAddressIndex < UPSFlashOkFlagEndAddress)
	{
		unsigned char CheckSum = 0;
		//写入地址
		EraseData[0] = TX_UPS_FRAME_LENGTH;
		CheckSum ^= TX_UPS_FRAME_LENGTH;
		EraseData[1] = (FlashAddressIndex >> 16) & 0xFF;
		CheckSum ^= EraseData[1];
		EraseData[2] = (FlashAddressIndex >> 8) & 0xFF;
		CheckSum ^= EraseData[2];
		EraseData[3] = (FlashAddressIndex) & 0xFF;
		CheckSum ^= EraseData[3];
		EraseData[4] = TX_UPS_FLASH_LENGTH;
		CheckSum ^= TX_UPS_FLASH_LENGTH;
		//写入数据
		
		unsigned char i = 0;
		for (i = 5; i < TX_UPS_FRAME_LENGTH - 1; i++)
		{
			EraseData[i] = UpsBinFileBuff[FlashAddressIndex];
			CheckSum ^= UpsBinFileBuff[FlashAddressIndex];
			FlashAddressIndex++;
		}
		EraseData[i] = CheckSum;
		outfile.write((char*)EraseData, (std::streamsize)TX_UPS_FRAME_LENGTH);
	}


	FlashBinFileSize += (BinFileLength - UPSFlashStartAddress);
	AddMessage(_T("OK."));
	outfile.close();

	std::ifstream fin2;
	fin2.open("UpsFlashTmp.bin", std::ios::in | std::ios::binary);
	fin2.read((char*)UpsFlashBuff, 0x26000);
	fin2.close();

	AddMessage(_T("------------------------------------"));
}

void CUPS_FIRMWARE_UPGRADE::OnButtonOpen()
{
	UpdateData(TRUE);
	CString strInterface;

	m_select_ups_interface.GetLBText(m_select_ups_interface.GetCurSel(), strInterface);//获取接口下拉框当前选择的接口的文本
	
	strInterface = strInterface.Mid(0,3);
	if (strInterface.Compare(_T("USB")) == 0)
	{
		m_InterfaceType = INTERFACE_TYPE_USB;
		m_FirmwareType = FIRMWARE_TYPE_UPS;
	}
	else if (strInterface.Compare(_T("COM")) == 0)
	{
		m_InterfaceType = INTERFACE_TYPE_RS232;
		m_FirmwareType = FIRMWARE_TYPE_UPS;
	}
	else
	{
		AddMessage(_T("Please select interface"));
	}

	if (m_InterfaceType == INTERFACE_TYPE_USB)
	{
		AddMessage(_T("Open The HID Interface!"));

		if (m_HIDControl.OpenHIDUPS(USB_VID, USB_PID,1))
		{
			AddMessage(_T("Find the HID Device OK!"));
			m_DeviceOpenStatus = DEVICE_STATUS_HIDOPENED;
			m_ButtonUpsFile.EnableWindow(TRUE);
			m_ButtonUpsFirmware.EnableWindow(TRUE);
		}
		else
		{
			AddMessage(_T("Can not find the HID Device!"));
			m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
		}
	}
	else if (m_InterfaceType == INTERFACE_TYPE_RS232)
	{
		CString StrMessage;

		m_SerControl.Close();
		m_select_ups_interface.GetLBText(m_select_ups_interface.GetCurSel(), strInterface);//获取接口下拉框当前选择的接口的文本

		if (strInterface.Compare(_T("COM1")) == 0)
		{
			ComSel = 1;
		}
		else if (strInterface.Compare(_T("COM2")) == 0)
		{
			ComSel = 2;
		}
		else if (strInterface.Compare(_T("COM3")) == 0)
		{
			ComSel = 3;
		}
		else if (strInterface.Compare(_T("COM4")) == 0)
		{
			ComSel = 4;
		}
		else if (strInterface.Compare(_T("COM5")) == 0)
		{
			ComSel = 5;
		}
		else if (strInterface.Compare(_T("COM6")) == 0)
		{
			ComSel = 6;
		}
		else if (strInterface.Compare(_T("COM7")) == 0)
		{
			ComSel = 7;
		}
		else if (strInterface.Compare(_T("COM8")) == 0)
		{
			ComSel = 8;
		}
		else if (strInterface.Compare(_T("COM9")) == 0)
		{
			ComSel = 9;
		}
		else if (strInterface.Compare(_T("COM10")) == 0)
		{
			ComSel = 10;
		}
		else if (strInterface.Compare(_T("COM11")) == 0)
		{
			ComSel = 11;
		}
		else if (strInterface.Compare(_T("COM12")) == 0)
		{
			ComSel = 12;
		}
		else if (strInterface.Compare(_T("COM13")) == 0)
		{
			ComSel = 13;
		}
		else if (strInterface.Compare(_T("COM14")) == 0)
		{
			ComSel = 14;
		}
		else if (strInterface.Compare(_T("COM15")) == 0)
		{
			ComSel = 15;
		}
		else if (strInterface.Compare(_T("COM16")) == 0)
		{
			ComSel = 16;
		}
		else if (strInterface.Compare(_T("COM17")) == 0)
		{
			ComSel = 17;
		}
		else if (strInterface.Compare(_T("COM18")) == 0)
		{
			ComSel = 18;
		}
		else if (strInterface.Compare(_T("COM19")) == 0)
		{
			ComSel = 19;
		}
		else if (strInterface.Compare(_T("COM20")) == 0)
		{
			ComSel = 20;
		}
		else if (strInterface.Compare(_T("COM21")) == 0)
		{
			ComSel = 21;
		}
		else if (strInterface.Compare(_T("COM22")) == 0)
		{
			ComSel = 22;
		}
		else if (strInterface.Compare(_T("COM23")) == 0)
		{
			ComSel = 23;
		}
		else if (strInterface.Compare(_T("COM24")) == 0)
		{
			ComSel = 24;
		}
		else if (strInterface.Compare(_T("COM25")) == 0)
		{
			ComSel = 25;
		}
		else if (strInterface.Compare(_T("COM26")) == 0)
		{
			ComSel = 26;
		}
		else if (strInterface.Compare(_T("COM27")) == 0)
		{
			ComSel = 27;
		}
		else if (strInterface.Compare(_T("COM28")) == 0)
		{
			ComSel = 28;
		}
		else if (strInterface.Compare(_T("COM29")) == 0)
		{
			ComSel = 29;
		}
		else if (strInterface.Compare(_T("COM30")) == 0)
		{
			ComSel = 30;
		}

		//if(m_SerControl.Open(ComSel, 2400))		// check if the COM port is opened 
		if (m_SerControl.Open(ComSel, COMMUNICAT_RATE))
		{
			StrMessage.Format(_T("Com%d Is Finded"), ComSel);
			AddMessage(StrMessage);
			m_DeviceOpenStatus = DEVICE_STATUS_RS232OPENED;
			m_ButtonUpsFile.EnableWindow(TRUE);
			m_ButtonUpsFirmware.EnableWindow(TRUE);
		}
		else
		{
			StrMessage.Format(_T("Can Not Open Com%d,Please Choose the right Com Port"), ComSel);
			AfxMessageBox(StrMessage);
			StrMessage.Format(_T("Can Not Open Com%d !"), ComSel);
			AddMessage(StrMessage);
			m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
		}
	}
	else	//Other Interface don't support	
	{
		AddMessage(_T("Can Not Open Ihis Interface,Don't Support!"));
		m_DeviceOpenStatus = DEVICE_STATUS_NOOPENED;
	}
}

void TcharToChar(const TCHAR* tchar, char* _char)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CUPS_FIRMWARE_UPGRADE::OnBnClickedButtonUpsUpdateFirmware()
{
	// TODO: 在此添加控件通知处理程序代码
	long  ReadNumber = 0;
	int   CmdCnt = 0;
	BYTE  byArray[16];
	BYTE  byBaudRate[] = { 0x03, 'B', 0x01 };

	OnButtonOpen();//打开串口或者USB
	m_bReady = FALSE;
	while (FLASH_CMD[CmdCnt])
	{
		CmdCnt++;
	}

	memset(InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);

	if (FLASH_CMD[CmdCnt - 1] == '#')
	{
		FLASH_CMD[CmdCnt - 1] = '\r';
	}
	else
	{
		MessageBox(_T("the FLASHCMD is error"), _T("ERROR"), MB_OK);
		AddMessage(_T("Please modify the FLASHCMD in config file first!"));
		return;
	}

	switch (m_DeviceOpenStatus)
	{
	case DEVICE_STATUS_NOOPENED:
		FLASH_CMD[CmdCnt - 1] = '#';
		MessageBox(_T("Open the Device error"), _T("ERROR"), MB_OK);
		AddMessage(_T("Please choose the right interface first!"));
		break;
	case DEVICE_STATUS_HIDOPENED:
		if (m_HIDControl.DeviceIsOpened() == TRUE)
		{
			for (int i = 0; i < USBTryCnt; i++)
			{
				if (m_HIDControl.WriteUPS(FLASH_CMD, CmdCnt))
				{
					memset(InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);
					if (m_HIDControl.ReadUPS(InputReport, &ReadNumber))
					{
						if (strstr(InputReport, "#FH\r") != NULL)
						{
							
							if ((PROGRAM_RATE == 9600) || (PROGRAM_RATE == 76800))
							{
								memset(byArray, '\0', 16);
								CvrtToCByteArray(byArray, (BYTE*)byBaudRate, 3);
								if (m_HIDControl.WriteUPS((char*)byArray, 8))
								{
									memset(InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);
									if (m_HIDControl.ReadUPS(InputReport, &ReadNumber))
									{
										if (strstr(InputReport, "A\r") != NULL)
										{
											AddMessage(_T("Send Baud Rate Command OK"));
											m_bReady = TRUE;
											FLASH_CMD[CmdCnt - 1] = '#';
											break;
										}
									}
								}
							}
							else
							{
								m_bReady = TRUE;
								FLASH_CMD[CmdCnt - 1] = '#';
								break;
							}
						}
					}
				}
			}

			if (m_bReady)
			{
				AddMessage(_T("UPS ready for firmware update"));

				m_ButtonUpsFile.EnableWindow(FALSE);
				m_ButtonUpsFirmware.EnableWindow(FALSE);

				UpdateData(TRUE);
				m_FirmwareUpdateTread = CreateThread(0, 0, UPSFirmwareUpdateByUSBThread, NULL, 0, &m_Tid);      //create the update thread
			}
			else
			{
				AddMessage(_T("UPS no ready for firmware update"));
				m_HIDControl.CloseHIDUPS();
				AfxMessageBox(_T("Firmware update check ready error"), MB_OK);
			}
		}
		else
		{
			AddMessage(_T("Can't communicate with the UPS,please check the USB cable connection"));
		}

		break;

	case DEVICE_STATUS_RS232OPENED:
		if (CheckSerUPSReadyAndUpdate(&m_Tid, CmdCnt))
		{
			AddMessage(_T("UPS firmware update completed"));
		}
		else
		{
			FLASH_CMD[CmdCnt - 1] = '#';
			AddMessage(_T("UPS firmware update fault"));
		}
		break;
	default:
		FLASH_CMD[CmdCnt - 1] = '#';
		AddMessage(_T("Please open device first!"));
		break;
	}
	FLASH_CMD[CmdCnt - 1] = '#';

}


void CUPS_FIRMWARE_UPGRADE::UPSDeviceListInit()
{
	UINT i;
	TCHAR FilePathName[_MAX_PATH];
	TCHAR StrDeviceSectionName[256];
	TCHAR buffer[8];

	GetCurrentDirectory(_MAX_PATH, FilePathName);
	_tcscat_s(FilePathName, _T("\\IAPConfigFile.ini"));

	m_DeviceCount = GetPrivateProfileInt(_T("DEVICELIST"), _T("DEVICECOUNT"), 0xFFFF, (LPCTSTR)FilePathName);

	if (m_DeviceCount == 0xFFFF)
	{
		AfxMessageBox(_T("IAP Config File is not exist"), MB_OK | MB_ICONINFORMATION);
		AddMessage(_T("A config file is needed for IAP progress"));

		m_ButtonUpsFile.EnableWindow(FALSE);
		m_ButtonUpsFirmware.EnableWindow(FALSE);
	}
	else
	{
		for (i = 0; i < m_DeviceCount; i++)
		{
			_tcscpy_s(StrDeviceSectionName, _T("DEVICE"));
			_itow_s(i, buffer, 10);//int 以10进制转换为字符串
			_tcscat_s(StrDeviceSectionName, buffer);

			m_UPSVIDPIDList[i].VID = GetPrivateProfileInt(StrDeviceSectionName, _T("VID"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].PID = GetPrivateProfileInt(StrDeviceSectionName, _T("PID"), 0xFFFF, (LPCTSTR)FilePathName);
			GetPrivateProfileString(StrDeviceSectionName, _T("FLASHCMD"), _T(""), m_UPSVIDPIDList[i].SflashCMD, 255, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].CommunicateRate = GetPrivateProfileInt(StrDeviceSectionName, _T("COMMUNICATBDRATE"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].ProgrogramRate = GetPrivateProfileInt(StrDeviceSectionName, _T("PROGRAMBDRATE"), 0xFFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].RawDataSize = GetPrivateProfileInt(StrDeviceSectionName, _T("RAWDATASIZE"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].HeadLine = GetPrivateProfileInt(StrDeviceSectionName, _T("HEADLINE"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].RomHead = GetPrivateProfileInt(StrDeviceSectionName, _T("ROMHEAD"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].RomTail = GetPrivateProfileInt(StrDeviceSectionName, _T("ROMTAIL"), 0xFFFF, (LPCTSTR)FilePathName);
			GetPrivateProfileString(StrDeviceSectionName, _T("IAPCODEPROTECT"), _T(""), m_UPSVIDPIDList[i].SCodePretect, 255, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].IntStart = GetPrivateProfileInt(StrDeviceSectionName, _T("INTSTART"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].IntEnd = GetPrivateProfileInt(StrDeviceSectionName, _T("INTEND"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].PageSize = GetPrivateProfileInt(StrDeviceSectionName, _T("PAGESIZE"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].IapEnd = GetPrivateProfileInt(StrDeviceSectionName, _T("IAPEND"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].IapStart = GetPrivateProfileInt(StrDeviceSectionName, _T("IAPSTART"), 0xFFFF, (LPCTSTR)FilePathName);
			m_UPSVIDPIDList[i].IapMark = GetPrivateProfileInt(StrDeviceSectionName, _T("IAPMARK"), 0xFFFF, (LPCTSTR)FilePathName);


			USB_VID = m_UPSVIDPIDList[i].VID;
			USB_PID = m_UPSVIDPIDList[i].PID;
			//memcpy(FLASH_CMD, m_UPSVIDPIDList[i].SflashCMD, 16);
			TcharToChar(m_UPSVIDPIDList[i].SflashCMD, FLASH_CMD);
			COMMUNICAT_RATE = m_UPSVIDPIDList[i].CommunicateRate;
			PROGRAM_RATE = m_UPSVIDPIDList[i].ProgrogramRate;
			RAW_DATA_SIZE = m_UPSVIDPIDList[i].RawDataSize;
			RAW_HEAD_LINE = m_UPSVIDPIDList[i].HeadLine;
			RAW_START_ADDR = m_UPSVIDPIDList[i].RomHead & 0xFFFFF0;
			RAW_END_ADDR = m_UPSVIDPIDList[i].RomTail & 0xFFFFFF;

			RAW_INT_START_ADDR = m_UPSVIDPIDList[i].IntStart & 0xFFFFF0;
			RAW_INT_END_ADDR = m_UPSVIDPIDList[i].IntEnd & 0xFFFFF0;
			RAW_PAGE_SIZE = m_UPSVIDPIDList[i].PageSize;
			IAP_END_ADDR = m_UPSVIDPIDList[i].IapEnd & 0xFFFFF0;
			IAP_START_ADDR = m_UPSVIDPIDList[i].IapStart & 0xFFFFF0;
			RAW_MASK_ADDR = m_UPSVIDPIDList[i].IapMark & 0xFFFFF0;

			if (RAW_PAGE_SIZE
				&& RAW_INT_END_ADDR
				&& RAW_INT_START_ADDR
				&& IAP_END_ADDR
				&& IAP_START_ADDR
				&& RAW_START_ADDR
				&& RAW_END_ADDR
				&& RAW_DATA_SIZE)
			{
			}
			else
			{
				AfxMessageBox(_T("IAP Config File is error"), MB_OK | MB_ICONINFORMATION);
				AddMessage(_T("Please check the config file"));
				m_ButtonUpsFile.EnableWindow(FALSE);
				m_ButtonUpsFirmware.EnableWindow(FALSE);
			}

			if ((PROGRAM_RATE != 76800 && PROGRAM_RATE != 9600 && PROGRAM_RATE != 2400) || (COMMUNICAT_RATE != 9600 && COMMUNICAT_RATE != 2400 && COMMUNICAT_RATE != 1200))
			{
				AfxMessageBox(_T("IAP Config File is error"), MB_OK | MB_ICONINFORMATION);
				AddMessage(_T("Please check the modify the Baud rate"));
			}
		}
	}
}

void CUPS_FIRMWARE_UPGRADE::CvrtToCByteArray(BYTE* byArray, BYTE* pByte, int nSize)
{
	int i;

	for (i = 0; i < nSize; i++)
	{
		byArray[i] = pByte[i];
	}
}

DWORD __stdcall CUPS_FIRMWARE_UPGRADE::UPSFirmwareUpdateByUSBThread(PVOID nouse)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(6000);

	BOOL     UpdateOK = FALSE;
	long     index = 0;
	CString  indexstr;
	CString  number;
	long     numb = 0;

	long	    Index = 0;
	int FileByte = 0;
	int TotalFileByte = 0;

	int FlashPercent = 0;
	int FlashPercentOld = 0;

	CString StrTemp = 0;

	pthis->AddMessage(_T("------------------------------------"));

	TotalFileByte = pthis->FlashBinFileSize;
	StrTemp.Format(_T("Total flash size:%d"), TotalFileByte);
	pthis->AddMessage(StrTemp);

	//init the progress information
	pthis->m_iState = 0;

	pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
	pthis->m_ProgressUpsProgress.SetPos(0);

	pthis->m_nRetransmitCount = 0;

	//disable send UPS command	
	pthis->m_ButtonUpsFile.EnableWindow(FALSE);
	pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);
	while (1)
	{
		if (!UpdateOK)//如果没有烧录完成
		{
			//刷新进度条
			if (TotalFileByte > 0)
			{
				FlashPercent = FileByte * 100 / TotalFileByte;
				if (FlashPercentOld < FlashPercent)
				{
					StrTemp.Format(_T(" %d %%"), FlashPercent);
					pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(StrTemp);
					FlashPercentOld = FlashPercent;
					pthis->m_ProgressUpsProgress.SetPos(FlashPercent);
				}
			}
			//发送烧录数据
			if (pthis->m_HIDControl.WriteUPS((char*)&UpsFlashBuff[Index], UpsFlashBuff[Index]))
			{
				memset(pthis->InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);
				//发送数据成功那个，开始接收数据
				if (pthis->m_HIDControl.ReadUPS(pthis->InputReport, &numb))
				{
					if (strstr(pthis->InputReport, "A\r") != NULL)
					{
						indexstr.Format(_T("%x ok"), ((unsigned int)UpsFlashBuff[Index + 1] << 16 | UpsFlashBuff[Index + 2] << 8 | UpsFlashBuff[Index + 3]));
						pthis->AddMessage(indexstr);
						FileByte += UpsFlashBuff[Index + 4];
						index++;
						Index += TX_UPS_FRAME_LENGTH;
						if (FileByte >= TotalFileByte)//如果烧录的数据为总烧录数据，表示所有数据烧录完成
						{
							UpdateOK = TRUE;
						}
						continue;
					}
					else
					{
						pthis->AddMessage(_T("Send the UPS firmware data Err"));
						AfxMessageBox(_T("Receive Error !"));
						UpdateOK = FALSE;
						pthis->m_HIDControl.CloseHIDUPS();
						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);
						pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
						pthis->m_ProgressUpsProgress.SetPos(0);

						return (1);
					}
				}
				else//接收数据失败
				{
					pthis->m_nRetransmitCount++;
					if (pthis->m_nRetransmitCount >= 3)
					{
						pthis->AddMessage(_T("read data Err"));
						pthis->m_HIDControl.CloseHIDUPS();
						UpdateOK = FALSE;

						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);
						pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
						pthis->m_ProgressUpsProgress.SetPos(0);
						return(1);
					}
				}
			}
			else//发送数据失败
			{
				pthis->m_nRetransmitCount++;
				if (pthis->m_nRetransmitCount >= 3)
				{
					pthis->AddMessage(_T("Write UPS error"));
					pthis->m_HIDControl.CloseHIDUPS();
					UpdateOK = FALSE;
					pthis->m_ButtonUpsFile.EnableWindow(TRUE);
//					pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
					pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
					pthis->m_ProgressUpsProgress.SetPos(0);

					return (1);
				}
			}
		}
		else//如果烧录完成，发送结束指令
		{
			//BYTE	byEof9[] = { 0x09, 0x00, 0x00, 0x00,0x04, 'E', 'O', 'F',0x46 };
			BYTE	byEof9[] = { 0x08, 0x00, 0x00, 0x00,0x04, 'E', 'O', 'F',0x47 };
			if (pthis->m_HIDControl.WriteUPS((char*)byEof9, 9))
			{
				pthis->AddMessage(_T("Send EOF Command OK"));
				pthis->AddMessage(_T("UPS firmware update successful!"));

				AfxMessageBox(_T("Programming successfully !"), MB_OK | MB_ICONINFORMATION);
				pthis->m_ButtonUpsFile.EnableWindow(TRUE);
//				pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
				break;
			}
		}

	}

	/*
	while (!UpdateOK)
	{
		switch (pthis->m_iState)
		{
			case 0:
			{// Tx
				//如果全部数据发送完毕
				if (FileByte >= TotalFileByte)
				{
					BYTE	byEof9[] = { 0x08, 0x00, 0x00, 0x00,0x04, 'E', 'O', 'F',0x47 };
					BYTE	byEof8[] = { 0x08, 0x03, 0x00, 0x00, 'E', 'O', 'F',0x47 };

					//	CByteArray	byArray;

					if (pthis->m_HIDControl.WriteUPS((char*)byArray, 9))
					{
						pthis->AddMessage(_T("Send EOF Command OK"));
						pthis->AddMessage(_T("UPS firmware update successful!"));

						AfxMessageBox(_T("Programming successfully !"), MB_OK | MB_ICONINFORMATION);
						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
						UpdateOK = TRUE;
						break;
					}
				}
				else//如果没有烧录完成，刷新进度条
				{
					if (TotalFileByte > 0)
					{
						FlashPercent = FileByte * 100 / TotalFileByte;
						if (FlashPercentOld < FlashPercent)
						{
							StrTemp.Format(_T(" %d %%"), FlashPercent);
							pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(StrTemp);
							FlashPercentOld = FlashPercent;
							pthis->m_ProgressUpsProgress.SetPos(FlashPercent);
						}
					}
				}

				if (pthis->m_HIDControl.WriteUPS((char *)&UpsFlashBuff[Index], UpsFlashBuff[Index]))
				{
					pthis->m_iState = 1;
				}
				else
				{
					pthis->m_nRetransmitCount++;
					pthis->m_iState = 0;
						
					if (pthis->m_nRetransmitCount >= 3)
					{
						pthis->AddMessage(_T("Write UPS error"));
						pthis->m_HIDControl.CloseHIDUPS();
						UpdateOK = FALSE;
						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
						pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
						pthis->m_ProgressUpsProgress.SetPos(0);

						return (1);
					}
					UpdateOK = FALSE;
				}
				
				
			}
			break;

			case 1: // Rx
			{
				{
					memset(pthis->InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);

					if (pthis->m_HIDControl.ReadUPS(pthis->InputReport, &numb))
					{
						if (strstr(pthis->InputReport, "A\r") != NULL)
						{
								indexstr.Format(_T("%x ok"), ((unsigned int)UpsFlashBuff[Index + 1] << 16 | UpsFlashBuff[Index + 2] << 8 | UpsFlashBuff[Index + 3]));
								pthis->AddMessage(indexstr);
								FileByte += UpsFlashBuff[Index + 4];
								index++;
								Index += TX_UPS_FRAME_LENGTH;
						}
						else
						{
							pthis->AddMessage(_T("Send the UPS firmware data Err"));
							AfxMessageBox(_T("Receive Error !"));
							UpdateOK = FALSE;
							pthis->m_HIDControl.CloseHIDUPS();

							pthis->m_ButtonUpsFile.EnableWindow(TRUE);
							pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);

							pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
							pthis->m_ProgressUpsProgress.SetPos(0);

							return (1);
						}
					}
					else
					{
						pthis->m_nRetransmitCount++;
						pthis->m_iState = 0;
						
						if (pthis->m_nRetransmitCount >= 6)
						{
							pthis->AddMessage(_T("read data Err"));
							pthis->m_HIDControl.CloseHIDUPS();
							UpdateOK = FALSE;
							 
							pthis->m_ButtonUpsFile.EnableWindow(TRUE);
							pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);
							pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
							pthis->m_ProgressUpsProgress.SetPos(0);
							return(1);
						}
						UpdateOK = FALSE;
					}
				}
			}
			break;

			default:
				break;
		}
	}
	*/
	//reset the progress information
	pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
	pthis->m_ProgressUpsProgress.SetPos(0);

	pthis->m_ButtonUpsFile.EnableWindow(TRUE);
	pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);

	pthis->m_bReady = FALSE;
	pthis->AddMessage(_T("------------------------------------"));

	return(1);
}

BOOL CUPS_FIRMWARE_UPGRADE::CheckSerUPSReadyAndUpdate(PVOID nouse, int cnt)
{
	char RecevUPTP[30];  //store the received data in this buffer
	int j = 0;

	// To get the answer
	for (j = 0; j < RS232TryCnt; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			if (m_SerControl.SendData(FLASH_CMD, cnt) == cnt)
			{
				break;
			}
		}

		if (m_SerControl.ReceiveDataByFlag(RecevUPTP, 30, '\r', 2))
		{
			if (strstr(RecevUPTP, "#FH\r") != NULL)
			{
				FLASH_CMD[cnt - 1] = '#';
				pthis->AddMessage(_T("UPS ready for firmware update"));
				pthis->m_ButtonUpsFile.EnableWindow(FALSE);
				pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);
				m_SerControl.Close();
				if (PROGRAM_RATE == 76800)
				{
					m_SerControl.Open(ComSel, 76800);
				}
				if (PROGRAM_RATE == 9600)
				{
					m_SerControl.Open(ComSel, 9600);
				}
				else
				{
					m_SerControl.Open(ComSel, 2400);
				}
				UpdateData(TRUE);
				m_UPSFirmwareUpdateComThread = CreateThread(0, 0, UPSFirmwareUpdateByCOMThread, NULL, 0, &m_Tid);
				break;
			}
		}
	}

	if (j == RS232TryCnt)
	{
		FLASH_CMD[cnt - 1] = '#';
		pthis->AddMessage(_T("Can't communicate with UPS, please check the RS232 cable connection."));
		MessageBox(_T("Firmware update check ready error"), _T("ERROR"), MB_OK);
		m_SerControl.Close();
		return FALSE;
	}

	return TRUE;
}

DWORD __stdcall CUPS_FIRMWARE_UPGRADE::UPSFirmwareUpdateByCOMThread(PVOID nouse)
{

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(6000);

	BYTE	byArray[148];
	CHAR	ReceiveBuff[32];
	long    index = 0;
	long	    Index = 0;
	CString indexstr;

	BOOL	UpdateOK = FALSE;
	int		iBackCou = 0;
	
	int FileByte = 0;
	int TotalFileByte = 0;

	int FlashPercent = 0;
	int FlashPercentOld = 0;

	CString StrTemp = 0;

	pthis->AddMessage(_T("------------------------------------"));

	TotalFileByte = pthis->FlashBinFileSize;
	StrTemp.Format(_T("Total flash size:%d"), TotalFileByte);
	pthis->AddMessage(StrTemp);
	//init the progress information
	pthis->m_iState = 0;

	pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
	pthis->m_ProgressUpsProgress.SetPos(0);

	pthis->m_nRetransmitCount = 0;

	//disable send UPS command	
	pthis->m_ButtonUpsFile.EnableWindow(FALSE);
	pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);

	while (!UpdateOK)
	{
		switch (pthis->m_iState)
		{
			case 0:// Tx
			{
				if (FileByte >= TotalFileByte)//如果全部数据发送完毕
				{
					BYTE	byEof9[] = { 0x09, 0x00, 0x00, 0x00,0x03, 'E', 'O', 'F',0x46};
					BYTE	byEof8[] = { 0x08, 0x03, 0x00, 0x00, 'E', 'O', 'F',0x47 };

					if (RAW_END_ADDR > 0xFFFF)
					{
						pthis->CvrtToCByteArray(byArray, (BYTE*)byEof9, 9);
					}
					else
					{
						pthis->CvrtToCByteArray(byArray, (BYTE*)byEof8, 8);
					}

					if (pthis->m_SerControl.SendData((char*)byArray, 9))
					{
						pthis->m_SerControl.Close();
						pthis->AddMessage(_T("Send EOF Command OK"));
						pthis->AddMessage(_T("UPS firmware update successful!"));

						AfxMessageBox(_T("Programming successfully !"), MB_OK | MB_ICONINFORMATION);
						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
						UpdateOK = TRUE;
						break;
					}
				}
				else //如果没有发送完成，刷新进度条
				{
					if (TotalFileByte > 0)
					{
						FlashPercent = FileByte * 100 / TotalFileByte;
						if (FlashPercentOld < FlashPercent)
						{
							StrTemp.Format(_T(" %d %%"), FlashPercent);
							pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(StrTemp);
							FlashPercentOld = FlashPercent;
							pthis->m_ProgressUpsProgress.SetPos(FlashPercent);
						}
					}
				}
			
			
				//indexstr.Format(_T("%d Wr"), index);
				//pthis->AddMessage(indexstr);

				if (pthis->m_SerControl.SendData((char*)&UpsFlashBuff[Index], UpsFlashBuff[Index]))
				{
					
					pthis->m_iState = 1;
				}
				else
				{
					pthis->m_nRetransmitCount++;
					pthis->m_iState = 0;
				
					if (pthis->m_nRetransmitCount >= 3)
					{
						pthis->AddMessage(_T("Write UPS error"));
						pthis->m_SerControl.Close();
						UpdateOK = FALSE;
						pthis->m_ButtonUpsFile.EnableWindow(TRUE);
						pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
						pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
						pthis->m_ProgressUpsProgress.SetPos(0);
						return (1);
					}
					UpdateOK = FALSE;
				}
			}
			break;

			case 1: // Rx
			{
				//if (RAW_DATA_SIZE == 64 || RAW_DATA_SIZE == 32 || RAW_DATA_SIZE == 16)
				{
//					memset(pthis->InputReport, '\0', MAX_HID_INPUTREPORT_LENGTH);

					memset(ReceiveBuff, '\0', 32);

					if (pthis->m_SerControl.ReceiveDataByLength(ReceiveBuff, 2, 8))
					{
						if (strstr(ReceiveBuff, "A\r") != NULL)
						{
							indexstr.Format(_T("%x ok"), ((unsigned int)UpsFlashBuff[Index + 1]<<16| UpsFlashBuff[Index + 2]<<8| UpsFlashBuff[Index + 3]));
							pthis->AddMessage(indexstr);
							FileByte += UpsFlashBuff[Index + 4];
							index++;
							Index += TX_UPS_FRAME_LENGTH;
							pthis->m_iState = 0;
						}
						else
						{
							pthis->m_nRetransmitCount++;
							pthis->m_iState = 0;

							if (pthis->m_nRetransmitCount >= 3)
							{
								pthis->AddMessage(_T("Send the UPS firmware data Err"));
								AfxMessageBox(_T("Receive Error !"));
								pthis->m_SerControl.Close();
								//pthis->m_SerControl.Open(pthis->ComSel,2400);
								//pthis->m_SerControl.Open(pthis->ComSel,COMMUNICAT_RATE);
								UpdateOK = FALSE;
								pthis->m_ButtonUpsFile.EnableWindow(TRUE);
//								pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
								pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
								pthis->m_ProgressUpsProgress.SetPos(0);

								return (1);
							}

							UpdateOK = FALSE;
							memset(ReceiveBuff, '\0', 32);
							break;
						}
					}
					else
					{
						pthis->m_nRetransmitCount++;
						pthis->m_iState = 0;
						
						if (pthis->m_nRetransmitCount >= 3)
						{
							pthis->AddMessage(_T("read data Err"));
							pthis->m_SerControl.Close();
							UpdateOK = FALSE;
							pthis->m_ButtonUpsFile.EnableWindow(TRUE);
//							pthis->m_ButtonUpsFirmware.EnableWindow(TRUE);
							pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
							pthis->m_ProgressUpsProgress.SetPos(0);
							return(1);
						}
						UpdateOK = FALSE;
						memset(ReceiveBuff, '\0', 32);
					}
				}
			}
			break;

			default:
				break;
		}
	}

	//reset the progress information
	pthis->m_EditUpsFileUpdateProgressPercent.SetWindowText(_T("0 %"));
	pthis->m_ProgressUpsProgress.SetPos(0);

	//pthis->m_UpdateFw.EnableWindow(FALSE);
	pthis->m_ButtonUpsFile.EnableWindow(TRUE);
	pthis->m_ButtonUpsFirmware.EnableWindow(FALSE);

	pthis->m_bReady = FALSE;

	pthis->AddMessage(_T("------------------------------------"));
	pthis->m_SerControl.Close();
	//pthis->m_SerControl.Open(pthis->ComSel,2400);
	//pthis->m_SerControl.Open(pthis->ComSel,COMMUNICAT_RATE);

	return UpdateOK;
}
