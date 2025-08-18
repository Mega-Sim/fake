#ifndef		__AMC_CALLBACK_H
#define		__AMC_CALLBACK_H



enum {
	CALLBACK_INT1 = 0	// 1msIntAMC(), ISR������ ��ó�� �ҷ�����.
	, CALLBACK_INT5_MAIN	// 5msIntAMC1(), ISR������ �� ���߿� �ҷ�����.
	, CALLBACK_INT5_AUX1	// 5msIntAMC2(), ISR������ �� ���߿� �ҷ�����.
	, CALLBACK_INT5_AUX2	// 5msIntAMC3(), ISR������ �� ���߿� �ҷ�����.
	, CALLBACK_INT10	// 10msIntAMC(), ISR������ �� ���߿� �ҷ�����.

	, CALLBACK_STR_EXCH	// AMC_STR_EXCH()
	
	, CALLBACK_PREV_MAIN_LOOP	// AMC_APP_MAIN()
	, CALLBACK_BODY_MAIN_LOOP	// MAIN LOOP���� ȣ���ϱ����� ������
	
	, CALLBACK_CALL_END
};


enum SAMPLING_FREQ {
	SAMPLING_FREQ_200 = 200			// 1ms �����ֱ�
	, SAMPLING_FREQ_1000 = 1000		// 200us �����ֱ�
};

// ��������α׷��� ���� �������̸�,
// ���⿡�� �ʿ��� �ݹ������͸� ����Ѵ�.
extern void __App_First();

void * __dsplib_set_callback(int ofs, void (*func)());
void __dsplib_call_callback(int ofs);

void * __dsplib_set_callback_param4(int ofs, void (*func)(void*prcv, int len, void *prtn, int *));
void __dsplib_call_callback_param4(int ofs, void *p1, int nrcv, void *p2, int *psend);


SAMPLING_FREQ __dsplib_get_sampling_freq();
void __dsplib_set_sampling_freq(SAMPLING_FREQ sf);



#endif

