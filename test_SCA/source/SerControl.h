// Serial.h

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

class CSerControl
{

public:
	CSerControl();
	~CSerControl();

	BOOL Open(int nPort, int nBaud);
	BOOL Close(void);

	int ReadData(void *, int);
	int SendData(const char *, int);
	int ReadDataWaiting(void);

	void ClearReceiveBuffer(void);
	void ClearSendBuffer(void);

	BOOL IsOpened(void)
	{ 
		return(m_bOpened);
	}
	BOOL ReceiveDataByFlag(char *RecevBuff,int BufferLength, char Flag, int TimeSpan);
	BOOL ReceiveDataByLength(char *RecevBuff, int Length, int TimeSpan);

protected:
	HANDLE m_hIDComDev;
	BOOL m_bOpened;

	OVERLAPPED m_OverlappedRead;
	OVERLAPPED m_OverlappedWrite;

	BOOL WriteCommByte(unsigned char ucByte);
};

#endif
