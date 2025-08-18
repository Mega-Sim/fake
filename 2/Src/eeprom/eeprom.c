#include "6xSystem.h"
#include "serialport.h"
#include "system.h"

#include "eeprom.h"


// designed for 24FC1025 (1Mbits eeprom)


#define		MAKE_SLAVE_ADDR(slave_addr, addr0_16)		(slave_addr | (((addr0_16 >> 16) & 0x01) ? 0x08 : 0))
#include "mcasp_reg.h"

#define		EEPROM_DELAY			13		// for 1us 5=> 13, 2011.5.5
#define		EEPROM_SLAVE_ADDR		(0xa2)	// 1010BXXr, or 1010BXXw

#define		EEPROM_DATA_RD_MODE		(McPDIR0 &= ~0x40000000)
#define		EEPROM_DATA_WR_MODE		(McPDIR0 |= 0x40000004)

#define		EEPROM_SET_SDA			(McPDOUT0 |= 0x40000000)
#define		EEPROM_CLR_SDA			(McPDOUT0 &= ~0x40000000)
#define		EEPROM_SET_SCL			(McPDOUT0 |= 0x00000004)
#define		EEPROM_CLR_SCL			(McPDOUT0 &= ~0x00000004)

#define		EEPROM_DATA_VALUE		((McPDIN0 >> 30) & 0x01)

#define		min(a,b)		((a) > (b) ? (b) : (a))
void __Start_eeprom()
{
	EEPROM_DATA_WR_MODE;
	
	// stop condition
	EEPROM_SET_SDA;	
	EEPROM_SET_SCL;
	MicroDelay(EEPROM_DELAY);
	
	EEPROM_CLR_SDA;
	MicroDelay(EEPROM_DELAY);
	EEPROM_CLR_SCL;
	MicroDelay(EEPROM_DELAY);
}

int __GetAck_eeprom()
{
	int nVal;

	EEPROM_CLR_SDA;
	MicroDelay(EEPROM_DELAY);
	EEPROM_DATA_RD_MODE;
	MicroDelay(EEPROM_DELAY);
	
	EEPROM_SET_SCL;
	int ns = GetTicCount();
	do { 
		MicroDelay(EEPROM_DELAY);
		nVal = EEPROM_DATA_VALUE; 
		if (msec(GetTicCount() - ns) > 1) break;
	} while (nVal);

	EEPROM_CLR_SCL;
	MicroDelay(EEPROM_DELAY);
	EEPROM_DATA_WR_MODE;
	MicroDelay(EEPROM_DELAY);
	
	return nVal;
}

unsigned char __Writeeeprom(char ch)
{
	int i;
	EEPROM_DATA_WR_MODE;
	MicroDelay(EEPROM_DELAY);
	EEPROM_CLR_SCL;
	MicroDelay(EEPROM_DELAY);	//--
	for (i = 7; i >= 0; i --)
	{
		if ((ch >> i) & 0x01)	EEPROM_SET_SDA;
		else					EEPROM_CLR_SDA;
		MicroDelay(EEPROM_DELAY);
		
		EEPROM_SET_SCL;
		MicroDelay(EEPROM_DELAY);
		EEPROM_CLR_SCL;
		MicroDelay(EEPROM_DELAY);
	}
	return __GetAck_eeprom();
}

unsigned char __Readeeprom(int nAck)
{
	int i;
	unsigned char uc = 0;
	EEPROM_DATA_RD_MODE;
	MicroDelay(EEPROM_DELAY);	//--
	for (i = 0; i < 8; i ++)
	{
		EEPROM_SET_SCL;
		MicroDelay(EEPROM_DELAY);

		uc <<= 1;
		uc |= EEPROM_DATA_VALUE;

		EEPROM_CLR_SCL;
		MicroDelay(EEPROM_DELAY);
	}
	
	if (nAck)	EEPROM_SET_SDA;
	else		EEPROM_CLR_SDA;

	EEPROM_DATA_WR_MODE;
	
	EEPROM_SET_SCL;
	MicroDelay(EEPROM_DELAY);
	EEPROM_CLR_SCL;
	MicroDelay(EEPROM_DELAY);
	
	EEPROM_DATA_RD_MODE;
	
//	i = __GetAck_eeprom();
	return uc;
}

void __Stop_eeprom()
{
	EEPROM_DATA_WR_MODE;
	MicroDelay(EEPROM_DELAY);

	EEPROM_CLR_SDA;
	MicroDelay(EEPROM_DELAY);

	EEPROM_SET_SCL;
	MicroDelay(EEPROM_DELAY);
	EEPROM_SET_SDA;
	MicroDelay(EEPROM_DELAY);
}










void Init_eeprom()
{
	McPFUNC0 |= 0x04;	// Set GPIO for SCL pin
	McPFUNC0 |= 0x40000000;	// set GPIO for SDA pin
	
	EEPROM_DATA_WR_MODE;
	
	EEPROM_SET_SDA;
	EEPROM_SET_SCL;	
}


int EepromReady(UINT usAddr)
{
	int nrtn;
//Flash Write 하는 타이밍(5ms) 동안 ACK가 High를 유지함 
	__Start_eeprom();
	nrtn = __Writeeeprom(MAKE_SLAVE_ADDR(EEPROM_SLAVE_ADDR, usAddr) | 0);//EEPROM_SLAVE_ADDR | 0);
	__Stop_eeprom();	//Start 조건부터 다시 시작
	return nrtn;
}

//extern int m_bEepromError;
int WaitEepromReady(UINT uiAddr)
{
	int ncnt = 0;
	while (EepromReady(uiAddr))
	{
		if (ncnt ++ > 15)
		{
//			m_bEepromError = 1;
			return -1;
		}
	}
//	m_bEepromError = 0;
	return 0;
}


int WriteEepromString(unsigned int usAddr, char *ps, int nBytes)
{
	int nTotWrite = 0;
	int nEnd, i;	//2011.5.5

	while (0 < nBytes)
	{
		// 만약 더 쓰기할 데이터가 있으면
		if (WaitEepromReady(usAddr) < 0) return nTotWrite;
		
		nEnd = EEPROM_PAGE_SIZE - (usAddr & (EEPROM_PAGE_SIZE - 1));
		nEnd = min(nBytes, nEnd);
		
		__Start_eeprom();
		__Writeeeprom(MAKE_SLAVE_ADDR(EEPROM_SLAVE_ADDR, usAddr) | 0);	// write
		__Writeeeprom(usAddr >> 8);	// write
		__Writeeeprom(usAddr & 0xff);	// write

		for (i = 0; i < nEnd; i ++)	//2011.5.5
		{
			__Writeeeprom(*ps ++ & 0xff);
		};// while (1);
		__Stop_eeprom();

		nBytes -= nEnd;
		usAddr += nEnd;
		nTotWrite += nEnd;
	}
	return nTotWrite;
}

int ReadEepromString(unsigned int usAddr, char *ps, int nRead)
{
	int ncnt;
	
	// 24LC256인 경우의 코드임.
	int bBreakCond;
	ncnt = 0;
	while ((nRead - ncnt) > 0)
	{
		// 만약 더 쓰기할 데이터가 있으면
		if (WaitEepromReady(usAddr) < 0) return ncnt;
		__Start_eeprom();
		__Writeeeprom(MAKE_SLAVE_ADDR(EEPROM_SLAVE_ADDR, usAddr) | 0);	// write
		__Writeeeprom(usAddr >> 8);	// write
		__Writeeeprom(usAddr & 0xff);	// write
		
		__Start_eeprom();
		__Writeeeprom(MAKE_SLAVE_ADDR(EEPROM_SLAVE_ADDR, usAddr) | 1);	// read
		
		do {
			// 지정한 바이트를 모두 읽었거나,
			// 0x0000~0xffff범위 또는 0x10000~0x1ffff범위를 넘어서게될때 
			// 브레이크가 일어난다.
			bBreakCond = ((ncnt == nRead-1) || ((usAddr + 1) % 0x10000) == 0);
			*ps ++ = __Readeeprom(bBreakCond ? 1 : 0) & 0xff;
			ncnt ++;
			usAddr++;
			if (bBreakCond == 1) 
				break;
		} while (1);
		__Stop_eeprom();	
	}
	
	return ncnt;
}













void WriteEepromChar(unsigned int usAddr0_17, unsigned char ucValue)
{
	WriteEepromString(usAddr0_17, (char *) &ucValue, 1);
}

void WriteEepromShort(unsigned int usAddr0_17, unsigned short usValue)
{
	WriteEepromString(usAddr0_17, (char *) &usValue, sizeof(unsigned short));
}

// 4Bytes
void WriteEepromInt(unsigned int usAddr0_17, unsigned int uiValue)
{
	WriteEepromString(usAddr0_17, (char *) &uiValue, sizeof(int));
}


unsigned char ReadEepromChar(unsigned int usAddr0_17)
{
	unsigned char uc;
	ReadEepromString(usAddr0_17, (char *) &uc, 1);
	return uc;
}

unsigned short ReadEepromShort(unsigned int usAddr0_17)
{
	unsigned short us;
	ReadEepromString(usAddr0_17, (char *) &us, sizeof(short));
	return us;
}


unsigned int ReadEepromInt(unsigned int usAddr0_17)
{
	unsigned int ui;
	ReadEepromString(usAddr0_17, (char *) &ui, sizeof(int));
	return ui;
}


