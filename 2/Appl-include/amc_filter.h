#ifndef		__AMC_FILTER_H
#define		__AMC_FILTER_H



enum  {
	AFT_LPF,
	AFT_HPF,
	AFT_BPF,
	AFT_NOTCH,
	AFT_APF,		// All Pass Filter
	AFT_PEQ,		// Peaking band EQ filter
	AFT_LSH,		// Low shelf filter
	AFT_HSH			// High shelf filter
};


typedef struct _AMC_FILTER 
{
	double a0, a1, a2, a3, a4;
	double x0, x1, x2, y1, y2;

	double dJustRatio;	// 고정된 큰수에서(예 100) 0으로 움직인다.
	double dIncValue;		// 최초 설정할 비율 값
	int bJustInit;

	int nFreq;
	int nFilterType;
	
	_AMC_FILTER()
	{
		a0 = a1 = a2 = a3 = a4 = 0.;
		x0 = x1 = x2 = y1 = y2 = 0.;
		dJustRatio = 0.;
		bJustInit = 1;
	}	
} AMC_FILTER, *LPAMC_FILTER;


#ifdef		__cplusplus
extern "C" {
#endif

LPAMC_FILTER __FilterInit(LPAMC_FILTER lpFilter, int fType, int nCutOffFreq, int fs = 5000);
double __Filtering( double sample, LPAMC_FILTER lpFilter);
int __GetFilteringFreq(LPAMC_FILTER lpFilter);
void __InitInternalDelay(LPAMC_FILTER lpFilter);


#ifdef		__cplusplus
}
#endif		// end of #ifdef __cplusplus


#endif


