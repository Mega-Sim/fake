#include "axisdef.h"
#include "pcdef.h"
#include "trace.h"

#pragma DATA_SECTION(".user");

UBOOTPARA BootPara;

float G_fSampling_Time;

GLOBALBUF GBuf = {0, };
CONTROLBUF CBuf[MMC_AXIS] = {0,};
GLOBALBUF *pGBuf = &GBuf;
CONTROLBUF* pCBuf[MMC_AXIS]={ &CBuf[0], &CBuf[1], &CBuf[2], &CBuf[3] };

Queue CQueue[MMC_AXIS] = {0,};
Queue* AxisQueue[MMC_AXIS]={ &CQueue[0], &CQueue[1], &CQueue[2], &CQueue[3] };

MQueue MCQueue[MMC_AXIS] = {0,};
MQueue* MAxisQueue[MMC_AXIS]={ &MCQueue[0], &MCQueue[1], &MCQueue[2], &MCQueue[3] };

JPCPAUSE Pause = {0, };

AQ_BUF Q_Buf[JPC_AXIS][MAX_Q_NUM];
PAUSE_RST PauseRstrt = {0, };

int	Q_out_num[JPC_AXIS];
int	Q_in_num[JPC_AXIS];

float Pid_Dividend;

float Pos_Limit;

char dbg_point[3];		

int userio_mode_select;

int Boot_Checksum_Flag;

BG_STATE gBgSt[JPC_AXIS];
ISR_STATE gIsrSt[JPC_AXIS];
PROFILE gBgPrf[JPC_AXIS];
PROFILE gIsrPrf[JPC_AXIS];

int	amc_error=0;
int	amc_move_err=0; 		

COMM_DPRAM_TYPE	*CommDpram[MMC_BOARD_NUM];
AXIS_DPRAM_TYPE	*AxisDpram[TOTAL_AXIS_NUM];

MOVE_FRAME_TYPE	Mf;
MOTION_DISP	Mdisp[TOTAL_AXIS_NUM];
SYNC_MOTION	SyncMotion;

int POWER3P3_Status_Cnt;
int POWER5P_Status_Cnt;
int POWER12P_Status_Cnt;
int POWER12M_Status_Cnt;


