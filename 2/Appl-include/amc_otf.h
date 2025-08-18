#ifndef		__OTF_FILTER_H
#define		__OTF_FILTER_H


#include "amc_filter.h"

typedef		AMC_FILTER		OTF_FILTER;
typedef		LPAMC_FILTER	LPOTF_FILTER;

#ifdef		__cplusplus
extern "C" {
#endif

LPOTF_FILTER __otf_Init(LPOTF_FILTER lpFilter, int nFc, int nGT200, int fs = 5000); // 200보다 크게 설정한다.
double __otf_ing( double sample, LPOTF_FILTER lpFilter);

#ifdef		__cplusplus
}
#endif		// end of #ifdef __cplusplus


#endif


