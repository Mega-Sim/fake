/******************************************************************************
**                                                                           **
**  Watchdog.c                                                               **
**     : 함수의 각부분에서 모니터링이 필요한루틴의 동작을 확인하는 용도.     **
**                                                                           **
**  Programm                                                                 **
**     : ckyu                                                                **
**     : 2007.10.2                                                           **
**                                                                           **
**  Revision history                                                         **
**     - 1.0 : 2007.10.2                                                     **
**                                                                           **
******************************************************************************/

#include "watchdog.h"



#define		REG32(a)		(*((int *) a))
#define		REG16(a)		(*((short *) a))
#define		REG8(a)			(*((char *) a))


#define		WS_ADDR_ENABLE				(0xA0100000)
#define		WS_ADDR_STATUS				(0xA0100004)
#define		WS_ADDR_CLR_MAINLOOP		(0xA0100008)
#define		WS_ADDR_CLR_HIGHERMAIN		(0xA010000C)
#define		WS_ADDR_CLR_CMD_INTERPRETE	(0xA0100010)
#define		WS_ADDR_CLR_CMD_CONTROL		(0xA0100014)


enum {
	WDT_CMD_ENABLE_WDT		= 1,
	WDT_CMD_DISABLE_WDT		= 2,
	WDT_CMD_GET_WDT_STATUS	= 3,
	WDT_CMD_SET_WDT_STATUS	= 4,
	WDT_CMD_TRIGGER_WDT_STATUS	= 5//, 2011.5.5
};





int __DisableWatchdog(int nWdtId)
{
	int _b = REG16(WS_ADDR_ENABLE);
	
	switch (nWdtId)
	{
	case WDT_MAINLOOP:		_b &= ~0x01; break;
	case WDT_EXTRA:		_b &= ~0x02; break;
	case WDT_SUBCONTROL:	_b &= ~0x04; break;
	case WDT_CONTROL:	_b &= ~0x08; break;
	}
	REG16(WS_ADDR_ENABLE) = _b;
	return _b;
}

int __EnableWatchdog(int nWdtId)
{
	int _b = REG16(WS_ADDR_ENABLE);
	
	switch (nWdtId)
	{
	case WDT_MAINLOOP:		_b |= 0x01; break;
	case WDT_EXTRA:			_b |= 0x02; break;
	case WDT_SUBCONTROL:	_b |= 0x04; break;
	case WDT_CONTROL:		_b |= 0x08; break;
	}
	REG16(WS_ADDR_ENABLE) = _b;
	return _b;
}

// 0x80: 워치독 발생으로 서보 오프
// 0x08: 200us wdt
// 0x04: 1ms wdt
// 0x02: 5ms wdt
// 0x01: 10ms wdt => main
unsigned char __GetWatchdogStatus()
{
	return REG8(WS_ADDR_STATUS);
}

void __SetWatchdogStatus(unsigned char ucpattern)
{
	REG8(WS_ADDR_STATUS) = ucpattern;
}


// 한번에 한개씩의 워치독을 리셋한다.
void __ClrWatchdog(int ofs)
{
	switch (ofs)
	{
	case WDT_MAINLOOP:		REG8(WS_ADDR_CLR_MAINLOOP) = 0; break;
	case WDT_EXTRA:			REG8(WS_ADDR_CLR_HIGHERMAIN) = 0; break;
	case WDT_SUBCONTROL:	REG8(WS_ADDR_CLR_CMD_INTERPRETE) = 0; break;
	case WDT_CONTROL:		REG8(WS_ADDR_CLR_CMD_CONTROL) = 0; break;
	}
}






void _dsp_disable_wdt_reason(int a)
{
	__DisableWatchdog(a);
}

void _dsp_enable_wdt_reason(int a)
{
	__EnableWatchdog(a);
}

unsigned char _dsp_get_wdt_status()
{
	return __GetWatchdogStatus();
}

void _dsp_set_wdt_status(unsigned char a)
{
	__SetWatchdogStatus(a);
}

void _dsp_clr_wdt_reason(unsigned char a)
{
	__ClrWatchdog(a);
}
