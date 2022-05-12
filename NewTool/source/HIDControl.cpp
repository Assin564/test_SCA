//
//////////////////////////////////////////////////////////////////////
#include "framework.h"
#include "HIDControl.h"
#include <wtypes.h>
#include <initguid.h>

#include "pch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHIDControl::CHIDControl()
{
	ReadHandle   = INVALID_HANDLE_VALUE;
	WriteHandle =  INVALID_HANDLE_VALUE;
	DeviceHandle = INVALID_HANDLE_VALUE;
	hEventObject = INVALID_HANDLE_VALUE;
	bDeviceIsOpened = FALSE;
}

CHIDControl::~CHIDControl()
{

}

BOOL CHIDControl::OpenHIDUPS(UINT UPSVID,UINT UPSPID, UINT VERSION)
{
	//Use a series of API calls to find a HID.

	HIDD_ATTRIBUTES                     Attributes;
	SP_DEVICE_INTERFACE_DATA            devInfoData;
	int                                 MemberIndex = 0;
	bool                                DeviceDetected = FALSE;
	bool                                bFinishFind = FALSE;
	int                                 Result;

	Length = 0;
	DeviceHandle = NULL;

	/*
	API function: HidD_GetHidGuid
	Get the GUID for all system HIDs.
	Returns: the GUID in HidGuid.
	*/

	HidD_GetHidGuid(&HidGuid);

	/*
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	*/
	
	hDevInfo=SetupDiGetClassDevs 
		(&HidGuid, 
		NULL, 
		NULL, 
		DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
		
	devInfoData.cbSize = sizeof(devInfoData);

	/*
	API function: SetupDiEnumDeviceInterfaces
	On return, MyDeviceInterfaceData contains the handle to a
	SP_DEVICE_INTERFACE_DATA structure for a detected device.
	Requires:
	The DeviceInfoSet returned in SetupDiGetClassDevs.
	The HidGuid returned in GetHidGuid.
	An index to specify a device.
	*/

	while(!bFinishFind)
	{
		Result=SetupDiEnumDeviceInterfaces 
			(hDevInfo, 
			0, 
			&HidGuid, 
			MemberIndex, 
			&devInfoData);

		MemberIndex++;
		
		if (Result)
		{
			//A device has been detected, so get more information about it.

			/*
			API function: SetupDiGetDeviceInterfaceDetail
			Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
			containing information about a device.
			To retrieve the information, call this function twice.
			The first time returns the size of the structure in Length.
			The second time returns a pointer to the data in DeviceInfoSet.
			Requires:
			A DeviceInfoSet returned by SetupDiGetClassDevs
			The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
				
			The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			This application doesn't retrieve or use the structure.			
			If retrieving the structure, set 
			MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			and pass the structure's address.
			*/
				
			//Get the Length value.
			//The call will return with a "buffer too small" error which can be ignored.
			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				NULL, 
				0, 
				&Length, 
				NULL);

			//Allocate memory for the hDevInfo structure, using the returned Length.
			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
				
			//Set cbSize in the detailData structure.
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			//Call the function again, this time passing it the returned buffer size.
			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				detailData, 
				Length, 
				&Required, 
				NULL);

			//Open a handle to the device.

			/*
			API function: CreateFile
			Returns: a handle that enables reading and writing to the device.
			Requires:
			The DevicePath in the detailData structure
			returned by SetupDiGetDeviceInterfaceDetail.
			*/
		
			DeviceHandle=CreateFile 
				(detailData->DevicePath,
				GENERIC_READ|GENERIC_WRITE, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, 
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, 
				0,
				NULL);

			if (INVALID_HANDLE_VALUE == DeviceHandle) 
			{
				continue;
			}


			/*
			API function: HidD_GetAttributes
			Requests information from the device.
			Requires: the handle returned by CreateFile.
			Returns: a HIDD_ATTRIBUTES structure containing
			the Vendor ID, Product ID, and Product Version Number.
			Use this information to decide if the detected device is
			the one we're looking for.
			*/

			//Set the Size to the number of bytes in the structure.
			Attributes.Size = sizeof(Attributes);

			Result = HidD_GetAttributes 
				(DeviceHandle, 
				&Attributes);
		
			//Assert Our HID device
			if(( Attributes.VendorID == UPSVID)&&(Attributes.ProductID == UPSPID)&&(Attributes.VersionNumber>=VERSION))
			{
				DeviceDetected = TRUE;
			
				GetDeviceCapabilities();
				WriteHandle=CreateFile 
						(detailData->DevicePath, 
						GENERIC_WRITE, 
						FILE_SHARE_READ|FILE_SHARE_WRITE, 
						(LPSECURITY_ATTRIBUTES)NULL,
						OPEN_EXISTING, 
						0, 
						NULL);
				PrepareForOverlappedTransfer();
				free(detailData);
				break;
			}

			//Free the memory used by the detailData structure (no longer needed).

		}
		else
		{
			if (ERROR_NO_MORE_ITEMS == GetLastError()) 
			{
				bFinishFind = TRUE;
				//SetupDiEnumDeviceInterfaces returned 0, so there are not UPS.
				DeviceDetected = FALSE;
			}

		}	
	//while end
	}
	//Free the memory reserved for hDevInfo by SetupDiClassDevs.
	SetupDiDestroyDeviceInfoList(hDevInfo);

	if(DeviceDetected == TRUE)
	{
		bDeviceIsOpened = TRUE;
	}
	else
	{
		bDeviceIsOpened = FALSE;
	}
	return DeviceDetected;

}

void CHIDControl::CloseHIDUPS()
{
	if(DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
	}

	if(ReadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(ReadHandle);
	}

	if(WriteHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(WriteHandle);
	}
}

void CHIDControl::GetDeviceCapabilities()
{
	//Get the Capabilities structure for the device.
	PHIDP_PREPARSED_DATA	PreparsedData;

	/*
	API function: HidD_GetPreparsedData
	Returns: a pointer to a buffer containing the information about the device's capabilities.
	Requires: A handle returned by CreateFile.
	There's no need to access the buffer directly,
	but HidP_GetCaps and other API functions require a pointer to the buffer.
	*/

	HidD_GetPreparsedData(DeviceHandle, &PreparsedData);

	/*
	API function: HidP_GetCaps
	Learn the device's capabilities.
	For standard devices such as joysticks, you can find out the specific
	capabilities of the device.
	For a custom device, the software will probably know what the device is capable of,
	and the call only verifies the information.
	Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
	Returns: a Capabilities structure containing the information.
	*/
	
	HidP_GetCaps(PreparsedData, &Capabilities);

	//No need for PreparsedData any more, so free the memory it's using.
	HidD_FreePreparsedData(PreparsedData);
}

BOOL CHIDControl::PrepareForOverlappedTransfer()
{
	//Get another handle to the device for the overlapped ReadFiles.
	ReadHandle=CreateFile 
		(detailData->DevicePath, 
		GENERIC_READ, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		(LPSECURITY_ATTRIBUTES)NULL, 
		OPEN_EXISTING, 
		FILE_FLAG_OVERLAPPED, 
		NULL);

	if (INVALID_HANDLE_VALUE != ReadHandle) 	
	{	
		if (INVALID_HANDLE_VALUE == hEventObject)
		{
			hEventObject = CreateEvent 
				(NULL, 
				TRUE, 
				TRUE, 
				_T(""));

			//Set the members of the overlapped structure.
			HIDOverlapped.hEvent = hEventObject;
			HIDOverlapped.Offset = 0;
			HIDOverlapped.OffsetHigh = 0;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CHIDControl::WriteUPS(char *Command,int TxLenght)
{
	const unsigned short int	MAXREPORTSIZE = 128;
    BOOL    bWriteSuccess = FALSE; 
	DWORD	BytesWritten = 0;
	CHAR	OutputReport[MAXREPORTSIZE];
	ULONG	Result;
	CString	strBytesWritten = _T("");
	CString temp;
	static BOOL WriteWay;
	long    error;
	int     i;
	int     WriteTimes;

	WriteTimes=TxLenght/(Capabilities.OutputReportByteLength -1);

	if(TxLenght%(Capabilities.OutputReportByteLength -1) != 0)
	{
		WriteTimes = WriteTimes + 1;
	}

	for(i=0; i<WriteTimes; i++)
	{
		memset(OutputReport,'\0',MAXREPORTSIZE);
		OutputReport[0]=0;
		memcpy(OutputReport+1,Command +(Capabilities.OutputReportByteLength - 1)*i,Capabilities.OutputReportByteLength-1);

		Result = WriteFile 
			(WriteHandle, 
			OutputReport, 
			Capabilities.OutputReportByteLength, 
			&BytesWritten,
			(LPOVERLAPPED) &HIDOverlapped); 
		
		Result = WaitForSingleObject 
				(hEventObject, 
				2000);
 
		switch (Result)
		{
			case WAIT_OBJECT_0:
			{
				bWriteSuccess = TRUE;
				if(i<WriteTimes-1)
				{
					Sleep(35);
				}
				break;
			}
			case WAIT_TIMEOUT:
			{
				//Cancel the Write operation.
				error= GetLastError();
				temp.Format(_T("%d"),error);
					
				CloseHandle(WriteHandle);
				Result = CancelIo(WriteHandle);
				bWriteSuccess = FALSE;
				break;
			}
			default:
			{
				CloseHandle(WriteHandle);
				Result = CancelIo(WriteHandle);
				bWriteSuccess = FALSE;
				break;
			}	
		}

		if(bWriteSuccess == FALSE)
		{
			break;		
		}
	}

	return bWriteSuccess;
}

BOOL CHIDControl::ReadUPS(char * ReceiveBuffer,long *number)
{
	//Read a report from the device.
	DWORD	Result;
	char    * pReadBufer;
	BOOL    bReadSuccess = FALSE;
	BOOL    ReadComplete = FALSE;
	CString num;
	long    error;
	static  USHORT	i=1;
	BYTE    Index=0;

	memset(HIDInputReport,'\0',MAX_HID_INPUTREPORT_LENGTH);
	pReadBufer = ReceiveBuffer;
	ResetEvent(hEventObject);

	while(!ReadComplete)
	{
		memset(HIDInputReport,'\0',MAX_HID_INPUTREPORT_LENGTH);

		Result = ReadFile 
			(ReadHandle, 
			HIDInputReport, 
			Capabilities.InputReportByteLength, 
			&NumberOfBytesRead,
			(LPOVERLAPPED) &HIDOverlapped); 

		Result = WaitForSingleObject(hEventObject, 3000);
 
		switch (Result)
		{
		case WAIT_OBJECT_0:
			{
				USHORT	i;

				for (i=1; i < Capabilities.InputReportByteLength; i++)
				{
					if(HIDInputReport[i] !='\r')
					{
						pReadBufer[Index++] = HIDInputReport[i];
					}
					else
					{
						pReadBufer[Index++] = HIDInputReport[i];
						pReadBufer[Index] = '\0';
						ReadComplete = TRUE;
						break;
					}
				}
				/*************************/
				NumberOfBytesRead=0;
				GetOverlappedResult(ReadHandle,&HIDOverlapped,&NumberOfBytesRead,TRUE);
				//*number=NumberOfBytesRead;
				//num.Format("%d",*number);
				//pMainWnd->m_InfoList.InsertString(pMainWnd->m_InfoList.GetCount(),num);
				/************************/
				bReadSuccess = TRUE;	
				break;
			}
		case WAIT_TIMEOUT:
			{
				CloseHandle(ReadHandle);
				error= GetLastError();
				num.Format(_T("%d"),error);
				memset(HIDInputReport,'\0',MAX_HID_INPUTREPORT_LENGTH);
				bReadSuccess = FALSE;	
				break;
			}
		default:
			{
				CloseHandle(ReadHandle);
				bReadSuccess = FALSE;
				break;
			}
		}
		
		ResetEvent(hEventObject);
		if(bReadSuccess == FALSE)
		{
			break;
		}
	}
	
	return bReadSuccess;
}

BOOL CHIDControl::DeviceIsOpened()
{

	return bDeviceIsOpened;
}

USHORT  CHIDControl::ObtainOutputReportLength()
{

	return Capabilities.OutputReportByteLength;
}

USHORT  CHIDControl::ObtainFeatureReportLength()
{

	return Capabilities.FeatureReportByteLength;
}
