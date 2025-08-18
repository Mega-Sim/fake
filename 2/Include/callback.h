#ifndef		__AMC_CALLBACK_H
#define		__AMC_CALLBACK_H



enum {
	CALLBACK_INT1 = 0	// 1msIntAMC(), ISR내에서 맨처음 불려진다.
	, CALLBACK_INT5_MAIN	// 5msIntAMC1(), ISR내에서 맨 나중에 불려진다.
	, CALLBACK_INT5_AUX1	// 5msIntAMC2(), ISR내에서 맨 나중에 불려진다.
	, CALLBACK_INT5_AUX2	// 5msIntAMC3(), ISR내에서 맨 나중에 불려진다.
	, CALLBACK_INT10	// 10msIntAMC(), ISR내에서 맨 나중에 불려진다.

	, CALLBACK_STR_EXCH	// AMC_STR_EXCH()
	
	, CALLBACK_PREV_MAIN_LOOP	// AMC_APP_MAIN()
	, CALLBACK_BODY_MAIN_LOOP	// MAIN LOOP에서 호출하기위한 오프셋
	
	, CALLBACK_CALL_END
};


// 사용자프로그램의 최초 진입점이며,
// 여기에서 필요한 콜백포인터를 등록한다.
extern void __App_First();

void * __dsplib_set_callback(int ofs, void (*func)());
void __dsplib_call_callback(int ofs);

void * __dsplib_set_callback_param4(int ofs, void (*func)(void *prcv, int len, void *prtn, int *plen));
void __dsplib_call_callback_param4(int ofs, void *p1, int nrcv, void *p2, int *psend);




#endif

