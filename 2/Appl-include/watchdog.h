#ifndef		__AMC_WATCHDOG_H
#define		__AMC_WATCHDOG_H

enum { 
	WDT_MAINLOOP 		= 1
	, WDT_EXTRA 		= 2	// 10ms
	, WDT_SUBCONTROL	= 3	// 5ms
	, WDT_CONTROL		= 4	// 1ms
};

void _dsp_disable_wdt_reason(int a);
void _dsp_enable_wdt_reason(int a);
unsigned char _dsp_get_wdt_status();
void _dsp_set_wdt_status(unsigned char a);
void _dsp_clr_wdt_reason(unsigned char a);


#endif
