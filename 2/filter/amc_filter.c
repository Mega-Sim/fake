#include <math.h>
#include <stdlib.h>

#include "../Appl-include/amc_filter.h"


#ifndef M_LN2
#define M_LN2	   0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


typedef		double		smp_type;

typedef		AMC_FILTER	biquad;






/* Below this would be biquad.c */
/* Computes a BiQuad filter on a sample */
static smp_type BiQuad(smp_type sample, biquad * b)
{
    smp_type result;

    /* compute result */
    result = b->a0 * sample + b->a1 * b->x1 + b->a2 * b->x2 -
        b->a3 * b->y1 - b->a4 * b->y2;

    /* shift x1 to x2, sample to x1 */
    b->x2 = b->x1;
    b->x1 = sample;

    /* shift y1 to y2, result to y1 */
    b->y2 = b->y1;
    b->y1 = result;

    return result;
}

/* sets up a BiQuad Filter */
// dbGain은 LPF, HPF, BPF, NOTCH 에는 영향이 없다.
// 2008.3.11, ckyu
static biquad *BiQuad_new(biquad *b, int type, smp_type dbGain, smp_type freq,
smp_type srate, smp_type bandwidth)
{
    smp_type A, omega, sn, cs, alpha, beta;
    smp_type a0, a1, a2, b0, b1, b2;
	smp_type BW_Q_SH = bandwidth;

    /* setup variables */
    A = pow(10, dbGain/40);
    omega = 2 * M_PI * freq/srate;
    sn = sin(omega);
    cs = cos(omega);
    switch (type) {
    case AFT_LPF:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = (1 - cs)/2;
        b1 = 1 - cs; 
        b2 = (1 - cs)/2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case AFT_HPF:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = (1 + cs)/2;
        b1 = -(1 + cs);
        b2 = (1 + cs)/2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case AFT_BPF:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = sn/2;
        b1 = 0;
        b2 = -sn/2;

        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
#if 0
    case BPFB:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = alpha;
        b1 = 0;
        b2 = -alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case AFT_APF:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = 1 - alpha;
        b1 = -2 * cs;
        b2 = 1 + alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
#endif
    case AFT_NOTCH:
        alpha = sn/(2.0*BW_Q_SH);
        b0 = 1;
        b1 = -2 * cs;
        b2 = 1;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case AFT_PEQ:
        alpha = sn * sinh(M_LN2 / 2 * BW_Q_SH * omega / sn);
        b0 = 1 + (alpha * A);
        b1 = -2 * cs;
        b2 = 1 - (alpha * A);
        a0 = 1 + (alpha / A);
        a1 = -2 * cs;
        a2 = 1 - (alpha / A);
        break;
#if 0
    case AFT_LSH:
        alpha = sn/2*sqrt((A + 1/A)*(1/S - 1) + 2);
        beta = 2 * sqrt(A) * alpha;
        b0 = A * ((A + 1) - (A - 1) * cs + beta);
        b1 = 2 * A * ((A - 1) - (A + 1) * cs);
        b2 = A * ((A + 1) - (A - 1) * cs - beta);
        a0 = (A + 1) + (A - 1) * cs + beta;
        a1 = -2 * ((A - 1) + (A + 1) * cs);
        a2 = (A + 1) + (A - 1) * cs - beta;
        break;
    case AFT_HSH:
        alpha = sn/2 * sqrt((A + 1/A)*(1/S - 1) + 2);
        beta = 2 * sqrt(A) * alpha;
        b0 = A * ((A + 1) + (A - 1) * cs + beta);
        b1 = -2 * A * ((A - 1) + (A + 1) * cs);
        b2 = A * ((A + 1) + (A - 1) * cs - beta);
        a0 = (A + 1) - (A - 1) * cs + beta;
        a1 = 2 * ((A - 1) - (A + 1) * cs);
        a2 = (A + 1) - (A - 1) * cs - beta;
        break;
#endif
    default:
        return NULL;
    }
    /* precompute the coefficients */
    b->a0 = b0/a0;
    b->a1 = b1/a0;
    b->a2 = b2/a0;
    b->a3 = a1/a0;
    b->a4 = a2/a0;

    /* zero initial samples */
    b->bJustInit = 1;
    b->x1 = b->x2 = 0;
    b->y1 = b->y2 = 0;

    return b;
}
/* crc==3062280887, version==4, Sat Jul  7 00:03:23 2001 */


float forder(smp_type Apass, smp_type Astop, smp_type Fpass, 
                                         smp_type Fstop, smp_type Fsamp)
{
    smp_type stop_power, pass_power, frequency_pass, 
                                           frequency_stop, temp0, temp1;
    float N;
    stop_power = pow(10.0,Astop/20.0)-1;
    pass_power = pow(10.0,Apass/20.0)-1;
    frequency_pass = tan(M_PI*Fpass/Fsamp);
    frequency_stop = tan(M_PI*Fstop/Fsamp);
    
    temp0 = log(frequency_stop/frequency_pass);
    temp1 = log(sqrt(stop_power/pass_power));
    
    N= log(sqrt(stop_power/pass_power))/log(frequency_stop/frequency_pass);
    return ceil(N); 
}


#if 0
// 원래의 간단한 필터 소스
LPAMC_FILTER __FilterInit(LPAMC_FILTER lpFilter, int fType, int nCutOffFreq, int fs)
{
	double dQ = 0.707;
	lpFilter->nFreq = nCutOffFreq;
	
	if (nCutOffFreq == 0) return NULL;
	if (fType == AFT_NOTCH || fType == AFT_BPF) dQ = 0.5;
	return BiQuad_new(lpFilter, fType, 0., nCutOffFreq, fs, dQ);
}

double __Filtering( double sample, LPAMC_FILTER lpFilter)
{
	if (lpFilter->nFreq == 0) return sample;
	return BiQuad(sample, lpFilter);
}

#else


// 필터링 상황이 변경되었을 때 플래그를 설정한 후 이후 다른 한 값을 추종해 가도록 한다.
LPAMC_FILTER __FilterInit(LPAMC_FILTER lpFilter, int fType, int nCutOffFreq, int fs)
{
	double dQ = 0.707;
	
	lpFilter->nFilterType = fType;

	if (lpFilter->nFreq != 0 && nCutOffFreq == 0)
		lpFilter->dJustRatio = -1.;	// 필터링 안된 값 쪽으로 이동한다.
	else if (lpFilter->nFreq != nCutOffFreq)
		lpFilter->dJustRatio = 1.;	// 필터링된 값 쪽으로 이동한다.

	lpFilter->dIncValue = 0.01;
	lpFilter->nFreq = nCutOffFreq;
//	if (nCutOffFreq == 0) return NULL;
	
	if (fType == AFT_NOTCH || fType == AFT_BPF) dQ = 0.5;
	return BiQuad_new(lpFilter, fType, 0., nCutOffFreq, fs, dQ);
}

double __Filtering( double sample, LPAMC_FILTER lpFilter)
{
/*
	// 노치필터의 경우 
	if (lpFilter->nFilterType == AFT_NOTCH && lpFilter->nFreq == 0) return sample;
*/
	if (lpFilter->bJustInit)
	{
		// 필터링을 새로 시작했을 때 충격을 적게 하기 위해서 적용한다.
		lpFilter->bJustInit = 0;
		lpFilter->x1 = lpFilter->x2 = sample;
		lpFilter->y1 = lpFilter->y2 = sample;
	}

	double val = BiQuad(sample, lpFilter);
	
	if (lpFilter->nFreq != 0) 
	{
		if (0. < lpFilter->dJustRatio)
		{
			// 필터링 된쪽으로 이동한다.
		    val = (sample * lpFilter->dJustRatio + val * (1. - lpFilter->dJustRatio));
		    lpFilter->dJustRatio -= lpFilter->dIncValue;
		}
	} else {
		if (lpFilter->dJustRatio < 0.)
		{
			// 필터링 안된쪽으로 이동한다.
			// (-) 이므로 계산하는 수식이 약간 달라짐
		    val = (sample * (1 + lpFilter->dJustRatio) - val * lpFilter->dJustRatio);
		    lpFilter->dJustRatio += lpFilter->dIncValue;
		} else
			val = sample;
	}
	return val;
}
#endif

int __GetFilteringFreq(LPAMC_FILTER lpFilter)
{
	if (lpFilter != NULL) return lpFilter->nFreq;
	return 0;
}

void __InitInternalDelay(LPAMC_FILTER lpFilter)
{
	lpFilter->x1 = lpFilter->x2 = 0;
	lpFilter->y1 = lpFilter->y2 = 0;
}

