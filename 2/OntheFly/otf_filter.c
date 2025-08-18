#include <math.h>
#include <stdlib.h>

#include "../Appl-include/amc_otf.h"


#ifndef M_LN2
#define M_LN2	   0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


typedef		double		smp_type;

typedef		OTF_FILTER	biquad;






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
// dbGain�� LPF, HPF, BPF, NOTCH ���� ������ ����.
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
//    b->x1 = b->x2 = 0;
//    b->y1 = b->y2 = 0;

    return b;
}
/* crc==3062280887, version==4, Sat Jul  7 00:03:23 2001 */



// ���͸� ��Ȳ�� ����Ǿ��� �� �÷��׸� ������ �� ���� �ٸ� �� ���� ������ ������ �Ѵ�.
LPAMC_FILTER __otf_Init(LPOTF_FILTER lpFilter, int nCutOffFreq, int nGT200, int fs)
{
	// ���͸��� ��� ������ �̵��Ͽ��ٰ�, ��� ���͸� �ȵ� �Է� ������ �̵��Ѵ�.
	double dQ = 0.707;
	
	lpFilter->nFilterType = AFT_LPF;

	if (nCutOffFreq == 0)
		lpFilter->dJustRatio = -1.;	// ���͸� �ȵ� �� ������ �̵��Ѵ�.
	else// if (lpFilter->nFreq != nCutOffFreq)
		lpFilter->dJustRatio = 1.;	// ���͸��� �� ������ �̵��Ѵ�.

	if (nGT200 < 200) nGT200 = 200;
	lpFilter->dIncValue = 1. / nGT200;
		
	lpFilter->nFreq = nCutOffFreq;
	
	return BiQuad_new(lpFilter, AFT_LPF, 0., nCutOffFreq, fs, dQ);
}

double __otf_ing( double sample, LPOTF_FILTER lpFilter)
{
	if (lpFilter->bJustInit)
	{
		// ���͸��� ���� �������� �� ����� ���� �ϱ� ���ؼ� �����Ѵ�.
		lpFilter->bJustInit = 0;
//		lpFilter->x1 = lpFilter->x2 = sample;
		lpFilter->y1 = lpFilter->y2 = sample;

		lpFilter->x1 = lpFilter->x2 = 0;
		lpFilter->y2 = lpFilter->y1;
		lpFilter->y1 = (lpFilter->y1 + sample) / 2;
	}
	double val = BiQuad(sample, lpFilter);

	if (lpFilter->dJustRatio <= -1) val = sample;
	else
	{
		if (0. < lpFilter->dJustRatio)
		{
			// ���͸� �Ȱ���� �ϴ� ���󰣴�.
//			    val = (sample * lpFilter->dJustRatio + val * (1. - lpFilter->dJustRatio));
		} else {
			// ������ ���͸� �ȵ� �� ������ �̵��Ѵ�.
		    val = (val * (1. + lpFilter->dJustRatio) - sample * lpFilter->dJustRatio);
		    lpFilter->dJustRatio -= lpFilter->dIncValue;
		}
	    lpFilter->dJustRatio -= lpFilter->dIncValue;
	}
	
	
	return val;
}
