#ifndef	__AXISINT_H
#define	__AXISINT_H

#ifdef AXISINT_GLOBALS
#define AXISINT_EXT
#else
#define AXISINT_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define	GPValue(n)	(1<<n)

#define	INITIAL_GUIDE_BITS  0x40000000

#define	pDOUBLE		        double *
#define PRO_VEL_MIN_LIMIT   5000 	


////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
#if defined(__AMC_28x)
AXISINT_EXT int __5mCnt;
#endif
AXISINT_EXT int __nGuideSensorBits;
AXISINT_EXT int __nGPort;
AXISINT_EXT int __nRcvCount;
AXISINT_EXT int nSimCount;
AXISINT_EXT int nIOCount;
AXISINT_EXT int __n5msCnt;
AXISINT_EXT char check_err_time;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
AXISINT_EXT void LedOn(void);
AXISINT_EXT void LedOff(void);
AXISINT_EXT void InitDbgLed();
AXISINT_EXT void ToggleDbgLed();
AXISINT_EXT void AMC_Int5ms(void);
AXISINT_EXT void CheckAndClear_PauseRst(CONTROLBUF* pCB, int ax);
AXISINT_EXT void UpdateEventDbgStatus(char mstime, int q);
AXISINT_EXT void UpdateMotionCalDbgStatus(char point, int ax, float vel, float pos);
AXISINT_EXT void UpdateMotionMake_ResultDbgStatus(char point, char err);
AXISINT_EXT void UpdateMotionMake_BFDbgStatus(int ax, double acc, double dcc, double v, int m);
AXISINT_EXT void UpdateMotionMake_AFDbgStatus(int ax, int q_pos1, int q_pos2, int q_pos3, int q_pos4);
AXISINT_EXT void UpdateMotionMake_DbgStatusClear(void);
AXISINT_EXT void UpdateMotionCal_DbgStatusClear(void);	
AXISINT_EXT interrupt void AMC_Int1ms(void);
AXISINT_EXT void dummy_axisint_c(void);
AXISINT_EXT void __InitGuideBuffer(int np);
AXISINT_EXT void InitGuideSensorPort(int nport);

#endif

