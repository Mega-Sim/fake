#ifndef	__AXISCTRL_H
#define	__AXISCTRL_H

#ifdef AXISCTRL_GLOBALS
#define AXISCTRL_EXT
#else
#define AXISCTRL_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
// BSET and BCLR Macro

#define YOUNG_TEST                              1
#define EVENT_ST_BIT_MASK                       0x000000ff    

#define BSET( reg, bits )   (reg |= (bits))         // Bit Set
#define BCLR( reg, bits )   (reg &= (~( bits )))    // Bit Clear 

#define EVENT_SET( reg, bits )  (reg = ((reg & (~(EVENT_ST_BIT_MASK))) | (bits & EVENT_ST_BIT_MASK)))  
#define EVENT_BSET( reg, bits )  (reg = ((reg & (~(EVENT_ST_BIT_MASK))) | ((1<<bits) & EVENT_ST_BIT_MASK)))  
#define EVENT_BCLR( reg, bits )  (reg &= (~(1<<bits)))  
#define EVENT_ALL_CLR( reg )  (reg &= (~(EVENT_ST_BIT_MASK)))  

// Control status flag => AMC_Control_Flag

// BG_STATE
#define B_ON_THE_FLY_CMD                        0x00000001 // NOTUSED
#define B_SETTING_PARAM                         0x00000002 // NOTUSED
#define B_NEW_PROFILE_READY                     0x00000004
#define B_STOP_UPDATING_BG_VAR                  0x00000008
#define B_HALT_TRIGGERED                        0x00000010
#define B_SKIP_SMOOTHING                        0x00000020
//ISR_STATE
#define B_HALT_EVENT                            0x00000040
#define B_HOLDING_VEL                           0x00000080
#define B_MOVING                                0x00000100
#define B_CHECK_IN_POS                          0x00000200
#define B_SYNC_END_POS                          0x00000400
#define B_PRCSING_PROF                          0x00000800

#define IN_SEQUENCE                             0x00001000  
#define IN_POS_FLAG                             0x00002000
#define N_SCURVE_SKIP_FLAG                      0x00004000
#define B_END_BY_POS                            0x00008000

// axis_source
#define ST_HOME_SWITCH_FLAG                     0x00010000
#define ST_POS_LIMIT_FLAG                       0x00020000
#define ST_NEG_LIMIT_FLAG                       0x00040000
#define ST_AMP_FAULT_FLAG                       0x00080000
#if (defined(__AMC_SMD) || defined(__AMC_29x))
#define	ST_A_LIMIT_FLAG    		                0x00100000
#endif
#if defined(__AMC_V70)
#define ST_SYSTEM_INSEC_FLAG                    0x00100000
#endif
#define ST_V_LIMIT_FLAG                         0x00200000
#define ST_X_NEG_LIMIT_FLAG                     0x00400000
#define ST_X_POS_LIMIT_FLAG                     0x00800000
#define ST_ERROR_LIMIT_FLAG                     0x01000000
#define ST_PC_COMMAND_FLAG                      0x02000000
#define ST_OUT_OF_FRAMES_FLAG                   0x04000000
#define ST_AMP_POWER_ONOFF_FLAG                 0x08000000
#define ST_ENC_OVF_FLAG                         0x10000000
#define ST_HWINPOSITION_FLAG                    0x20000000
#define ST_SWINPOSITION_FLAG                    0x40000000
#define ST_MUST_REBOOT_FLAG                     0x80000000
#define ST_NONE_FLAG                            0x00000000  


#define	STOP_EVENT_BIT_SCALE		            0
#define	E_STOP_EVENT_BIT_SCALE	                1
#define	ABORT_EVENT_BIT_SCALE		            2
#define	ENC_OVF_EVENT_BIT_SCALE	                3	// set if encoder overflow is present. actions is same as e-stop. ckyu, 2007.10.12 
#define	PAUSE_EVENT_BIT_SCALE            		4	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.
#define	Test_Flag_BIT_SCALE               		5	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.


// Control status flag => AMC_Public_Control_Flag
//0x000000001~ 0x00000007 3bit event_st
#define	NO_EVENT_FLAG		                    0x00000001
#define	STOP_EVENT_FLAG		                    0x00000002
#define	E_STOP_EVENT_FLAG	                    0x00000004
#define	ABORT_EVENT_FLAG		                0x00000008
#define	ENC_OVF_EVENT_FLAG	                    0x00000010	// set if encoder overflow is present. actions is same as e-stop. ckyu, 2007.10.12 
#define	PAUSE_EVENT_FLAG                		0x00000020	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.
#define	Test_Flag                       		0x00000040	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.

#define MOTOR_TYPE_FLAG                         0x00000100
#define BIT6                                    0x00000200
#define BIT7                                    0x00000400
#define BIT8                                    0x00000800

// AMC_Public_Control_Flag
#define MOVEDS_WAIT_COUNT_AFTER_STOP_FLAG       0x00010000
#define MOVEDS_WAIT_COUNT_AFTER_SERVO_ON_FLAG   0x00020000
#define OHT_DETECT_AREASETTING_FLAG             0x00040000
#define REAR_SERVO_OFF_FLAG                     0x00080000
#define FOLLOWS_MOVEDS_FLAG                     0x00100000
#define GB_NEW_CMD_ARRIVED                      0x00200000
#define OHT_PBS_IGNORE_SETTING_FLAG             0x00400000

#define MOTOR_SERVO     0
#define MOTOR_STEPPER   1

#define CONTROL_VEL     0
#define CONTROL_TRQ     1

#define ENCODER_INCREMENTAL 0
#define ENCODER_UNIPOLAR	1
#define ENCODER_BIPOLAR		2

#define LOOP_OPEN       0
#define LOOP_CLOSED		1

#define MONIRAM_SIZE    0x0DF0

#define NON_SENSOR		0x0FFF8
#define HOME_SENSOR		0x01		// home sensor
#define POS_SENSOR		0x02		// limit+ sensor
#define NEG_SENSOR		0x04		// limit- sensor

#define PULSE_PLUS			0x8000	// 0: CCW, 1: CW
#define PULSE_MINUS			0x0000
#define PULSE_SIGN_MASK		0x8000	// D15로 비트 이동
#define PULSE_MICROSTEPPER	0x8000	// 아직 사용하지 않음.
#define PULSE_STEPPER		0x0000
#define PULSE_CWCCW			0x4000
#define PULSE_SIGNPULSE		0x0000	// Sign, Pulse 모드만 사용

#define CleanVoltageMinValue    0.5
#define CleanVoltageMaxValue    11.0
#define CleanVoltageScaleValue  (20000/12)

#define Offset_Pos				1
#define Offset_Neg				-1

#define Acc_Offset              150         // Time = Offset * 200us = 30ms
#define Dcc_Offset              150         // Time = Offset * 200us = 30ms
#define Cruise_Offset           150         // Time = Offset * 200us = 30ms
#define Cruise_Mode_Offset      5000        // Time = Offset * 200us = 1000ms
#define Stop_Mode_Offset        5000        // Time = Offset * 200us = 1000ms

// Amc Commfail 관련 Debug Log 추가
#define LOG_DATA_SIZE 36
typedef struct _LOG_DATA
{
	volatile int nSignature;				// test characters (0xABCD1234)
	volatile int n200usCnt;
    volatile int nDpramCnt;
    volatile int n1msCnt;
    volatile int cmd_index;
    volatile int nMainLoopCnt;
    volatile int nDprIntClrCnt;
    volatile int status_DprIntClr;
    volatile int nAutoClear;
    // Log 추가 필요시 추가
}LOG_DATA;

AXISCTRL_EXT volatile LOG_DATA g_pLD;

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
AXISCTRL_EXT volatile char check_count_200us;
AXISCTRL_EXT volatile float open_loop_gain; 
AXISCTRL_EXT volatile int VCC_Moniter;
AXISCTRL_EXT volatile int VCC_Moniter_2;
AXISCTRL_EXT volatile int Setposition_count[4];
AXISCTRL_EXT volatile int Count_200us_INT_4_Debug;
AXISCTRL_EXT volatile int Dpram_Log;
AXISCTRL_EXT volatile int count4AbortEventStop;
AXISCTRL_EXT volatile int flag4AbortEventStop;
AXISCTRL_EXT volatile int firstExecute4AbortEventStop;
AXISCTRL_EXT volatile int EncTrc;

AXISCTRL_EXT int IO_Input[8];

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
AXISCTRL_EXT void ClearSWPauseFlag(int ax);
AXISCTRL_EXT void ClearPauseFlag(int ax);
AXISCTRL_EXT void InitIsrVars(int ax, int pos);
AXISCTRL_EXT void InitIsrSt(int ax);
AXISCTRL_EXT int GetTotalProfCount(int ax);
AXISCTRL_EXT void StartNewProfile(int ax);
AXISCTRL_EXT void GenerateVelocity(int ax, unsigned int bFreshStart);
AXISCTRL_EXT void UpdateDbgStatus(int q);
AXISCTRL_EXT void SetAbortEvent(int ax);
AXISCTRL_EXT void CheckInPos(void);
AXISCTRL_EXT void ProcessErrEvents(int ax);
AXISCTRL_EXT interrupt void Control_Int200us(void);
AXISCTRL_EXT float GetValueWithLimit(float val , float limit);
AXISCTRL_EXT void ControlVelocity(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void CheckSWLimit(CONTROLBUF* pCB , int val, int ax);
AXISCTRL_EXT void CheckSWPauseCondition(CONTROLBUF* pCB , int ax);
AXISCTRL_EXT void PowerOffState(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void Trace_Execute(CONTROLBUF * pCB, int ax);
AXISCTRL_EXT void ControlPosition(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void CheckEnvelope(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void CheckServo(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void AmpFaultReset(CONTROLBUF* pCB, int ax);
AXISCTRL_EXT void AmpFaultSet(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void CheckSensor(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void ChkPosLimit(CONTROLBUF* pCB , int st, int ax );
AXISCTRL_EXT void ChkPosReset(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void ChkPosSet(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void ChkNegLimit(CONTROLBUF* pCB , int st, int ax);
AXISCTRL_EXT void ChkNegReset(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void ChkNegSet(CONTROLBUF* pCB, int ax );
AXISCTRL_EXT void ChkHomeLimit( CONTROLBUF * pCB , int st, int ax );
AXISCTRL_EXT void ChkHomeReset( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT void ChkHomeSet( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT int ReadCurPos( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT float ControlServo( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT void GenerateOutput( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT void CheckSWInPos( CONTROLBUF * pCB, int ax );
AXISCTRL_EXT void CheckInPosSignal( CONTROLBUF * pCB, int ax );
// Amc Commfail 관련 Debug Log 추가
AXISCTRL_EXT int WriteLogData(int Step);
AXISCTRL_EXT int SetLog(int log, int pos, bool setdata);
#endif

