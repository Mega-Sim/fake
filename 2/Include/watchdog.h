#ifndef		__AMC_WATCHDOG_H
#define		__AMC_WATCHDOG_H

enum { 
	WDT_MAINLOOP 		= 1	// main loop
	, WDT_EXTRA 		= 2	// 5ms => DPRAM으로 변경해야 함.
	, WDT_SUBCONTROL	= 3	// 1ms
	, WDT_CONTROL		= 4	// 200us
};

void _dsp_disable_wdt_reason(int a);
void _dsp_enable_wdt_reason(int a);
unsigned char _dsp_get_wdt_status();
void _dsp_set_wdt_status(unsigned char a);
void _dsp_clr_wdt_reason(unsigned char a);


#endif
