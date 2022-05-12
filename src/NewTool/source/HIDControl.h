// CHIDControl.h: interface for the CHIDControl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CHIDCONTROL_H__
#define __CHIDCONTROL_H__


extern "C"
{
#include "hidsdi.h"
#include "setupapi.h"
}

#define   MAX_HID_INPUTREPORT_LENGTH    100
class CHIDControl  
{
private:
//the HID parameter declare

    HIDP_CAPS       Capabilities;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HANDLE          DeviceHandle ;
	HANDLE          hEventObject ;
	  
	GUID            HidGuid;
	OVERLAPPED      HIDOverlapped;
	char            HIDInputReport[MAX_HID_INPUTREPORT_LENGTH];//用Maxlength 来定义
	ULONG           Length;
	DWORD           NumberOfBytesRead;
	HANDLE          ReadHandle;
	HANDLE			WriteHandle;
	ULONG           Required;
	CString			ValueToDisplay;
	CString	        ReceivedByte;
	CString         VendorID;
	CString         ProductID;
	CString         VersionNumber;
	HANDLE			hDevInfo ;

	BOOL            bDeviceIsOpened;

public:
	CHIDControl();
	virtual ~CHIDControl();

	BOOL OpenHIDUPS(UINT UPSVID,UINT UPSPID, UINT VERSION);
	void CloseHIDUPS();

	BOOL WriteUPS(char *Command,int TxLenght);
	BOOL ReadUPS(char * ReceiveBuffer,long *number);
	BOOL DeviceIsOpened();

	USHORT  ObtainOutputReportLength();
	USHORT  ObtainFeatureReportLength();


private:
	void GetDeviceCapabilities();
	BOOL PrepareForOverlappedTransfer();

};

#endif // __CHIDCONTROL_H__
