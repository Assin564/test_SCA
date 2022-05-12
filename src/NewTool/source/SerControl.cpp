// Serial.cpp

#include "framework.h"
#include "SerControl.h"

#include "pch.h"

CSerControl::CSerControl()
{
	memset(&m_OverlappedRead, '\0', sizeof(OVERLAPPED));
 	memset(&m_OverlappedWrite, '\0', sizeof(OVERLAPPED));
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

CSerControl::~CSerControl()
{
	Close();
}

void CSerControl::ClearReceiveBuffer(void)
{
	memset(&m_OverlappedRead, '\0', sizeof(OVERLAPPED));
}

void CSerControl::ClearSendBuffer(void)
{
 	memset(&m_OverlappedWrite, '\0', sizeof(OVERLAPPED));
}

BOOL CSerControl::Open(int nPort, int nBaud)
{
	//char szPort[15];
	//char szComParams[50];

	CString sPort;
	CString sComParams;

	DCB dcb;
	COMMTIMEOUTS CommTimeOuts;
	unsigned char ucSet;
	DWORD dwError;

	if(m_bOpened)
	{
		return(TRUE);
	}

	sPort.Format(_T("\\\\.\\COM%d"), nPort);
	//wsprintf(szPort, "\\\\.\\COM%d", nPort);
	m_hIDComDev = CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if(m_hIDComDev == NULL) 
	{
		return( FALSE );
	}

	memset(&m_OverlappedRead, '\0', sizeof(OVERLAPPED));
 	memset(&m_OverlappedWrite, '\0', sizeof(OVERLAPPED));

	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(m_hIDComDev, &CommTimeOuts);

	sComParams.Format(_T("COM%d:%d,n,8,1"), nPort, nBaud);
	//wsprintf(szComParams, "COM%d:%d,n,8,1", nPort, nBaud);

	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hIDComDev, &dcb);
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	
	ucSet = (unsigned char)((FC_RTSCTS & FC_DTRDSR) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_RTSCTS) != 0);
	ucSet = (unsigned char)((FC_RTSCTS & FC_XONXOFF) != 0);

	if(!SetCommState( m_hIDComDev, &dcb )
		|| !SetupComm( m_hIDComDev, 10000, 10000 )
		|| m_OverlappedRead.hEvent == NULL 
		|| m_OverlappedWrite.hEvent == NULL)
	{
		dwError = GetLastError();

		if(m_OverlappedRead.hEvent != NULL)
		{
			CloseHandle(m_OverlappedRead.hEvent);
		}
		if(m_OverlappedWrite.hEvent != NULL)
		{
			CloseHandle(m_OverlappedWrite.hEvent);
		}

		CloseHandle(m_hIDComDev);
		return(FALSE);
	}

	m_bOpened = TRUE;

	return(m_bOpened);

}

BOOL CSerControl::Close(void)
{

	if(!m_bOpened || m_hIDComDev == NULL) 
	{
		return(TRUE);
	}

	if(m_OverlappedRead.hEvent != NULL) 
	{
		CloseHandle(m_OverlappedRead.hEvent);
	}
	if(m_OverlappedWrite.hEvent != NULL)
	{
		CloseHandle(m_OverlappedWrite.hEvent);
	}
	CloseHandle(m_hIDComDev);
	m_hIDComDev = NULL;
	m_bOpened = FALSE;

	return(TRUE);
}

BOOL CSerControl::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);
	if(!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if(WaitForSingleObject(m_OverlappedWrite.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return(TRUE);

}

int CSerControl::SendData(const char *buffer, int size)
{
	DWORD dwBytesWritten = 0;
	int i;

	if(!m_bOpened || m_hIDComDev == NULL)
	{
		return(0);
	}

	for(i = 0; i < size; i++ )
	{
		WriteCommByte(buffer[i]);
		dwBytesWritten++;
	}

	return((int)dwBytesWritten);

}

int CSerControl::ReadDataWaiting(void)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat;

	if(!m_bOpened || m_hIDComDev == NULL)
	{
		return(0);
	}

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);

	return((int)ComStat.cbInQue);

}

int CSerControl::ReadData(void *buffer, int limit)
{
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	if(!m_bOpened || m_hIDComDev == NULL)
	{
		return(0);
	}

	ClearCommError(m_hIDComDev, &dwErrorFlags, &ComStat);
	if(!ComStat.cbInQue)
	{
		return(0);
	}

	dwBytesRead = (DWORD)ComStat.cbInQue;
	if(limit < (int)dwBytesRead)
	{
		dwBytesRead = (DWORD)limit;
	}

	bReadStatus = ReadFile(m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead);
	if(!bReadStatus)
	{
		if(GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedRead.hEvent, 2000);
			return((int)dwBytesRead);
		}

		return(0);
	}

	return((int)dwBytesRead);
}


BOOL CSerControl::ReceiveDataByLength(char *RecevBuff, int Length, int TimeSpan)
{
	CTime startTimeIdle = CTime::GetCurrentTime();
	CTime endTimeIdle;
	CTimeSpan elapsedTimeIdle=0;
	int BufferIndex = 0;

	BufferIndex = 0;
	while(elapsedTimeIdle < TimeSpan)
	{
		endTimeIdle = CTime::GetCurrentTime();
		elapsedTimeIdle = endTimeIdle - startTimeIdle;

		if(ReadData(&(RecevBuff[BufferIndex]),1)==1)
		{
			BufferIndex++;
			if(BufferIndex >= Length)
			{
				break;
			}
		}
	}

	if(elapsedTimeIdle < TimeSpan)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CSerControl::ReceiveDataByFlag( char *RecevBuff, int BufferLength, char Flag, int TimeSpan)
{
	CTime startTimeIdle = CTime::GetCurrentTime();
	CTime endTimeIdle;
	CTimeSpan elapsedTimeIdle=0;
	int BufferIndex = 0;
	BOOL bufferfull = FALSE;

	startTimeIdle = CTime::GetCurrentTime();

	while( elapsedTimeIdle < TimeSpan)
	{
		endTimeIdle = CTime::GetCurrentTime();
		elapsedTimeIdle = endTimeIdle - startTimeIdle;

		if(ReadData(&(RecevBuff[BufferIndex]),1)==1)
		{
			if(RecevBuff[BufferIndex] == 0x00)
			{
			}
			else if(RecevBuff[BufferIndex] == Flag)
			{
				break;
			}
			else
			{
				BufferIndex++;

				if(BufferIndex >= BufferLength)
				{
					bufferfull = TRUE;
					break;
				}
			}
		}
	}
		
	if((elapsedTimeIdle < TimeSpan)&&(bufferfull == FALSE))
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}
