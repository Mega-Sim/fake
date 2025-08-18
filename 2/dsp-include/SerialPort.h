#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__



#define _REG32(addr) (*((volatile unsigned int *)(addr)))



#define DATA_6BIT		0
#define DATA_7BIT		0x2
#define DATA_8BIT		0x3
#define STOP_1BIT		0
#define STOP_2BIT		0x4
#define PAR_NO			0
#define PAR_EVEN		0x10
#define PAR_ODD 		0x18

#define STX 0x02
#define ETX 0x03
#define ACK 0x06
#define NAK 0x25

#define	DCD_STATUS		(0x80
#define	RI_STATUS		(0x40)
#define	DSR_STATUS		(0x20)
#define	CTS_STATUS		(0x10)


// Added, 2006.11.1, Khoosen Yu
int _IsSendingStopped(int nport0_11);



void 			SERIAL_Init();
void 			SERIAL_Enable(BOOL bEnable);
BOOL			SERIAL_SetPort(int nBaudrate, int nDataBit, int nParity, int nStopBit);
int				SERIAL_ReadAByte(unsigned char *pData, int timeout);
void 			SERIAL_WriteAByte(unsigned char data);
int				SERIAL_ReceivedDataCnt();
int 			SERIAL_ReadBytes(void *pData, int nCnt, int timeout);
void			SERIAL_WriteBytes(const void *pData, int nCnt);
BOOL			SERIAL_WaitCommand(unsigned char cmd, int timeout);
BOOL			SERIAL_ReadStxEtxString(char *sString, int nLength, int nTimeout);
unsigned char	SERIAL_GenChecksum(void *pData, int nCnt);
BOOL			SERIAL_TestChecksum(void *pData, int nCnt, unsigned char chksum);

/*
timeout : msec. if value is zero, wait infinite
*/
#define			SERIAL_PORT_1	0
#define			SERIAL_PORT_2	1
#define			SERIAL_PORT_3	2
#define			SERIAL_PORT_4	3

// 2006.9.28, Khoosen Yu
#define			EXT_SERIAL_0		4
#define			EXT_SERIAL_1		5
#define			EXT_SERIAL_2		6
#define			EXT_SERIAL_3		7
#define			EXT_SERIAL_4		8
#define			EXT_SERIAL_5		9
#define			EXT_SERIAL_6		10
#define			EXT_SERIAL_7		11



void			SERIALEX_Open(int nPortNum);
void			SERIALEX_Close(int nPortNum);
BOOL			SERIALEX_SetPort(int nPortNum, int nBaudrate, int nDataBit, int nParity, int nStopBit);
void			SERIALEX_Control(int nPortNum);
unsigned char 	SERIALEX_Status(int nPortNum);
int 			SERIALEX_ReadBytes(int nPortNum, void *pData, int nCnt, int timeout);
void			SERIALEX_WriteBytes(int nPortNum, const void *pData, int nCnt = 0);

typedef struct _SER_FILE_DESC
{
	char m_sName[60];
	unsigned int m_nSize;
	unsigned char *m_pBuffer;
}SER_FILE_DESC;

BOOL SERIAL_ReceiveFile(SER_FILE_DESC *pFile, int timeout);//Timeout : 
void SERIAL_ReleaseFileDesc(SER_FILE_DESC *pFile);
BOOL SERIAL_SendFile(char* sFileName, unsigned char *buffer, unsigned int nSize, int nTimeout);

#endif
