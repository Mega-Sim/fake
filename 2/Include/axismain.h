#ifndef	__AXISMAIN_H
#define	__AXISMAIN_H

#ifdef AXISMAIN_GLOBALS
#define AXISMAIN_EXT
#else
#define AXISMAIN_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define POSITIVE        1
#define	NEGATIVE		0

#define SERVO_MOTOR     0
#define	STEPPER         1
#define	MICRO_STEPPER   2

#define	FB_ENCODER		0
#define	FB_UNIPOLAR		1
#define	FB_BIPOLAR		2

#define	OPEN_LOOP		0
#define	CLOSED_LOOP		1

#define	V_CONTROL		0
#define	T_CONTROL		1

#define	IN_STANDING		0
#define	IN_ALWAYS		1

#define	TWO_PULSE		0
#define	SIGN_PULSE		1

#define TIME_2_SEC		10000

// Check DPRAM Interrupt and Clear
// g_ForceClearIntStatus defintion
#define FC_INT_CLR_NORMAL     0  //Interrupt is being issued normally.
#define FC_INT_CLR_CHECKING   1  //check if force Interrupt clear monitoring is being done.
#define FC_INT_CLR_DONE       2  //check if force Interrupt clear is done
#define FC_INT_CLR_NOCMD      3  //check if Dram Interrupt is not being issued

#define MAIN_WHILE_LOOP_RUN_TIME    1610    // main while loop run time = 1.61usec
#define DPRINT_CLEAR_TIMEOUT_MS      300    // DPRAM ���ͷ�Ʈ Release(Clear) �Ǵ� ���� �ð�
                                            // (300ms : 16msec���� DprInt�� 10���̻� ���� �� ���� �Ǵ�)
#define DPRINT_CLEAR_TIMEOUT     	((DPRINT_CLEAR_TIMEOUT_MS*1000)/MAIN_WHILE_LOOP_RUN_TIME*1000)
                                           // 300ms = DPRINT_CLEAR_TIMEOUT(186335cnt)
#define MAX_DPRINT_CLEAR_COUNT      10     // �ִ� DPR Interrupt ���� Clear ī��Ʈ ����
#define MAX_DPRINT_ENABLE_COUNT     3000   // �ִ� DPR Interrupt Count to enable Auto Clear after Force Clear
#define DPRINT_BIT                  6      // GPVAL (0x01b00008)�� bit6: Dpram ���ͷ�Ʈ �߻� ����


////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
AXISMAIN_EXT int fpga_ver;
AXISMAIN_EXT DBG_CMD_BUFFER	sDbgCmdBuff[MMC_AXIS];
AXISMAIN_EXT DBG_CMD_BUFFER	move_sDbgCmdBuff[MMC_AXIS];
AXISMAIN_EXT DBG_CNT_BUFFER	sDbgCntBuff;
AXISMAIN_EXT DBG_MSG_BUFFER	sDbgMsgBuff[MMC_AXIS];
AXISMAIN_EXT DBG_EVENT_MSG_BUFFER sEventMsgBuff[EVENT_MSG][MMC_AXIS];		
AXISMAIN_EXT DBG_MOTIONMAKE_MSG_BUFFER sMotionMakeMsgBuff[MOTIONMAKE_MSG];	
AXISMAIN_EXT DBG_MOTIONCAL_MSG_BUFFER sMotionCalMsgBuff[MOTIONCAL_MSG];	

// ICon ������ ����� ��Ʈ
AXISMAIN_EXT volatile int* ptr_icon_oport1;
AXISMAIN_EXT volatile int* ptr_icon_oport2;
AXISMAIN_EXT volatile int* ptr_icon_iport;
AXISMAIN_EXT volatile int* ptr_icon_lamp;

// Check dpram interrupt
AXISMAIN_EXT volatile int g_nOldLoopDpramCnt;         // While�������� ���� �ִ� DPram Interrupt counter��
AXISMAIN_EXT volatile int g_nDprIntActiveHoldCnt;     // DPR Int Active ���� ȸ��
AXISMAIN_EXT volatile int g_anIntStatesReg;           // GPVAL�� ���� 16bit�� backup
AXISMAIN_EXT volatile int g_nDprIntClearRetryCnt;     // Force Ack2PC ���� Ƚ��
AXISMAIN_EXT volatile char g_nDprIntState;            // Interrupt���� 1:OFF, 0:ON --> Active Low
AXISMAIN_EXT volatile int g_nUseDprInt_AutoClear;     // DPram Interrupt üũ Enable ����(Off�� ����, Dpr_Int 100�� ���� �� ����)
AXISMAIN_EXT volatile int g_nUseDprInt_AutoEnableCnt; // DPram Interrupt�� ���� �ð� ����(����) => ����͸� ����
AXISMAIN_EXT volatile int g_ForceClearIntStatus;      // ForceClearIntStatus

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
AXISMAIN_EXT void InitBdParameter(void);
AXISMAIN_EXT void SetAliveCheckFlag(int ok);
AXISMAIN_EXT void NdokMain(void);
AXISMAIN_EXT void InitBgSt(int ax);
AXISMAIN_EXT void InitPara(void);
AXISMAIN_EXT void NdokOnSerialEvent(const unsigned char* sStr, int nLen);
AXISMAIN_EXT void NdokOnIdleEvent();
AXISMAIN_EXT void NdokOnLowIdleEvent();
AXISMAIN_EXT void AbortEventStopProcess();

#endif

