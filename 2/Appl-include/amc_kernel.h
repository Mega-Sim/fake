#ifndef		__AMC_KERNEL_H
#define		__AMC_KERNEL_H


// 1ms timer based delay
void Delay(int ms);

void MicroDelay(int us);


#if 0
// nPortNum : 4~11
//			  0~3 : (reserved)
// Open후 원하는 직렬포트 설정을 SetPort()로 해야 한다.
void			SERIALEX_Open(int nPortNum);


// nPortNum : 4~11
//			  0~3 : (reserved)
// 사용하지 않는 포트는 Close()시켜야 하며,
// 재사용할 때는 Open() -> SetPort()의 순서로 초기화 해야 한다.
// 2007.12.3, ckyu
void			SERIALEX_Close(int nPortNum);

// nBaudRate : 9600 ~ 115200
// nDataBit : 7, 8
// nParity : 0:none, 1:even, 1:odd
// nStopBit : 1 or 2
BOOL			SERIALEX_SetPort(int nPortNum, int nBaudrate, int nDataBit, int nParity, int nStopBit);

// timeout < 0 : wait infinite until nCnt reached
// timeout == 0 : return immediatedly with received char
// timeout > 0 : wait for timeout milliseconds
// RETURN : # of received chars.
int 			SERIALEX_ReadBytes(int nPortNum, const void *pData, int nCnt, int timeout);

// pData : Pointer what for sending
// nCnt == 0 : count the string of pData, and send it.
// nCnt > 0 : send nCnt chars.
void			SERIALEX_WriteBytes(int nPortNum, const void *pData, int nCnt = 0);


int		SERIAL_ReadBytes(void *p, int len, int timeout);
void	SERIAL_WriteBytes(const void *p, int len);
#endif


#endif

