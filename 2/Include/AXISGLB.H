#ifndef	__AXISGLB_H
#define	__AXISGLB_H

#include "pcdef.h"
#include "axisdef.h"
#include "trace.h"

extern UBOOTPARA BootPara;

extern float G_fSampling_Time;

extern GLOBALBUF GBuf;
extern CONTROLBUF CBuf[MMC_AXIS];
extern GLOBALBUF *pGBuf;
extern CONTROLBUF *pCBuf[MMC_AXIS];

extern Queue CQueue[MMC_AXIS];
extern Queue *AxisQueue[MMC_AXIS];

extern MQueue MCQueue[MMC_AXIS];
extern MQueue* MAxisQueue[MMC_AXIS];

extern JPCPAUSE Pause;

extern AQ_BUF Q_Buf[JPC_AXIS][MAX_Q_NUM];
extern PAUSE_RST PauseRstrt;

extern int Q_out_num[JPC_AXIS];
extern int Q_in_num[JPC_AXIS];

extern float Pid_Dividend;

extern float Pos_Limit;

extern char dbg_point[3];		

extern int userio_mode_select;

extern int Boot_Checksum_Flag;

extern BG_STATE gBgSt[JPC_AXIS];
extern ISR_STATE gIsrSt[JPC_AXIS];
extern PROFILE gBgPrf[JPC_AXIS];
extern PROFILE gIsrPrf[JPC_AXIS];

extern int amc_error;
extern int amc_move_err;		

extern COMM_DPRAM_TYPE *CommDpram[MMC_BOARD_NUM];
extern AXIS_DPRAM_TYPE *AxisDpram[TOTAL_AXIS_NUM];

extern MOVE_FRAME_TYPE Mf;
extern MOTION_DISP Mdisp[TOTAL_AXIS_NUM];
extern SYNC_MOTION SyncMotion;

extern int POWER3P3_Status_Cnt;
extern int POWER5P_Status_Cnt;
extern int POWER12P_Status_Cnt;
extern int POWER12M_Status_Cnt;

#endif

