#include "6xSystem.h"
#include "hardware.h"

#include "i2c.h"

#include "i2creg.h"

#include "callback.h"


#include <string.h>

#define INTCODE_RDR		0x04
#define INTCODE_XDR		0x05


#define		BIT_FREE		14
#define		BIT_STT			13
#define		BIT_STP			11
#define		BIT_MST			10
#define		BIT_TRX			9
#define		BIT_RM			7
#define		BIT_DLB			6
#define		BIT_FDF			3

#define		BIT_BB			12


#define NRST				(1<<5)

void (*SetVect(int irq, void (*func)()))();
void EnableInterrupt(int bit,int enable);

volatile int m_nAddress;

extern void ExtraISR0_5ms();
extern void ExtraISR1_5ms();


interrupt void I2C_IsrSendBytes0()
{
	unsigned int intr_src;
	
	while (1)
	{
		intr_src = 0x7 & I2CISR0;
		if (intr_src == 0) break;
		
		switch(intr_src)
		{
		case 1:	// arbitration lost
			__dsplib_call_callback(CALLBACK_INT5_AUX1);
//			ExtraISR0_5ms();
			break;
		case 2: // no-ack condition is detected
			break;
		case 3:	// registers are ready
			// 아래의 1줄이 추가되었음.
			// 2007.5.28, ckyu
			return;
			
		case 4:	// receive data is ready
			break;
		case INTCODE_XDR:
			I2C_Triggering0();
			__dsplib_call_callback(CALLBACK_INT5_AUX1);
//			ExtraISR0_5ms();
			break;
		}
	}
	
}

interrupt void I2C_IsrSendBytes1()
{
	unsigned int intr_src;
	
	while (1)
	{
		intr_src = 0x7 & I2CISR1;
		if (intr_src == 0) break;
		
		switch(intr_src)
		{
		case 1:	// arbitration lost
			__dsplib_call_callback(CALLBACK_INT5_AUX2);
//			ExtraISR1_5ms();
			break;
		case 2: // no-ack condition is detected
			break;
		case 3:	// registers are ready
			// 아래의 1줄이 추가되었음.
			// 2007.5.28, ckyu
			return;
			
		case 4:	// receive data is ready
			break;
		case INTCODE_XDR:
			I2C_Triggering1();
			__dsplib_call_callback(CALLBACK_INT5_AUX2);
//			ExtraISR1_5ms();
			break;
		}
	}
	
}

//#define		ORG_I2C_CODE

void I2C_Init0()
{
	I2CMDR0 &= ~NRST;
	
	I2COAR0 = 0x5a;
	I2CSAR0 = 0x5b;
	
	I2CPSC0 = 10;
	I2CCLKL0 = 5;		// low = 1clk
	I2CCLKH0 = 5;		// high = 1clk
	
	I2CMDR0 = 0x2e20;
/*
		(1 << BIT_FREE) |	// FREE = 1
		(1 << BIT_STT) |	// STT = 1
		(1 << BIT_STP) |	// STP = 1
		(1 << BIT_MST) | 	// MST = 1
		(1 << BIT_TRX) |	// TRX = 1
		(1 << BIT_FDF) |	// FDF = 1
//		(1 << BIT_RM)  |	// RM = 1
		0 ;
*/

	I2CCNT0 = 1;

	MUXH &= 0xFFFF03FF;
	MUXH |= 0x00005800;
	
	SetVect(12, I2C_IsrSendBytes0);
	EnableInterrupt(12, 1);

	I2CIER0 = 0x1f;		// all interrupt
	I2CMDR0 |= NRST;
}

void I2C_Init1()
{
	I2CMDR1 &= ~NRST;
	
	I2COAR1 = 0x5a;
	I2CSAR1 = 0x5b;
	
	I2CPSC1 = 10;
	I2CCLKL1 = 5;		// low = 1clk
	I2CCLKH1 = 5;		// high = 1clk
	
	I2CMDR1 = 0x2e20;
	I2CCNT1 = 1;

	MUXH &= 0xFFE0FFFF;
	MUXH |= 0x00170000;
	
	SetVect(13, I2C_IsrSendBytes1);
	EnableInterrupt(13, 1);

	I2CIER1 = 0x1f;		// all interrupt
	I2CMDR1 |= NRST;
}

void I2C_Triggering0()
{
	
	// clear flag which cleared by manually.
	I2CMDR0 &= ~NRST;
	I2CDXR0 = 0x00;
	while (I2CSTR0 & (1 << BIT_BB));
	I2CMDR0 &= (1 << BIT_STP);
	I2CMDR0 |= (1 << BIT_STT);
	I2CMDR0 |= NRST;
}

void I2C_Triggering1()
{
	
	// clear flag which cleared by manually.
	I2CMDR1 &= ~NRST;
	I2CDXR1 = 0x00;
	while (I2CSTR1 & (1 << BIT_BB));
	I2CMDR1 &= (1 << BIT_STP);
	I2CMDR1 |= (1 << BIT_STT);
	I2CMDR1 |= NRST;
}

