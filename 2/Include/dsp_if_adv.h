#ifndef	__DSP_IF_ADV_H
#define	__DSP_IF_ADV_H

#ifdef DSP_IF_ADV_GLOBALS
#define DSP_IF_ADV_EXT
#else
#define DSP_IF_ADV_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define MAX_ACC_PPS2    	(1020363.0)  						// 9.0 m/s^2
#define MAX_ACC         	(MAX_ACC_PPS2 * PERIOD * PERIOD)	// pulse/cnt^2
#define INT_WITH_MIN1(a) 	(((int)(a) < 1) ? 1 : ((int)(a)))
#define ZP0001MPS			0.0023
#define ACC_DURING_DCC_MODE    SPEED_1_0_MPS2      
#define	SGN(a)				(((a)<0.) ? -1 : 1)

#define MoveNOP	0
#define MoveP	1
#define MoveN	2
#define MoveS	3
#define MoveD	4

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
DSP_IF_ADV_EXT char error_position;
DSP_IF_ADV_EXT int Motion_Flag[4];

DSP_IF_ADV_EXT int motionProfileMakeingStatus;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
DSP_IF_ADV_EXT void InitScurveParams(int ax);
DSP_IF_ADV_EXT void InitScurveBuffer(int ax, float initVal);
DSP_IF_ADV_EXT double ApplyScurveFilter(int ax, float fVel);
DSP_IF_ADV_EXT int ApplyLimitsInt(int val, int low, int high);
DSP_IF_ADV_EXT float ApplyLimitsFloat(float val, float low, float high);
DSP_IF_ADV_EXT int MovePNS(int mode, int ax, float fAcc, float fTgtVelPps);
DSP_IF_ADV_EXT float CalcAccOrDs(float fV0, float fV1, float fDsOrAcc);
DSP_IF_ADV_EXT float CalcVel4TriProf(float fA1, float fA2, float fV0, float fV2, float fDs);
DSP_IF_ADV_EXT float CalcAccCnt(float fV0, float fV1, float fDs);
DSP_IF_ADV_EXT float CalcDistance(float fV0, float fV1, int nAccCnt);
DSP_IF_ADV_EXT int MoveDS(char mode, int ax, float fAcc1, float fAcc2, float fMaxSpd, int nTgtPos);
DSP_IF_ADV_EXT int _dsp_move_pt(int ax, double acc, double vel);
DSP_IF_ADV_EXT int _dsp_move_nt(int ax, double acc, double vel);
DSP_IF_ADV_EXT int _dsp_move_st(int ax, double acc);
DSP_IF_ADV_EXT int _dsp_move_dst(int ax, double a1, double a2, double v, int m);
DSP_IF_ADV_EXT int _dsp_move_p(int ax, double a, double vel);
DSP_IF_ADV_EXT int _dsp_move_n(int ax, double a, double vel);
DSP_IF_ADV_EXT int _dsp_move_s(int ax, double a);
DSP_IF_ADV_EXT int _dsp_move_ds(int ax, double a1, double a2, double v, int m);
DSP_IF_ADV_EXT int __dsp_move_p(int mode, int ax, double a_param, double vel);
DSP_IF_ADV_EXT int __dsp_move_n(int mode, int ax, double a_param, double vel);
DSP_IF_ADV_EXT int __dsp_move_s(int mode, int ax, double a_param);
DSP_IF_ADV_EXT int __dsp_move_ds(int mode, int ax, double a1, double a2, double v, int m);

#endif

