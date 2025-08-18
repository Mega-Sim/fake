#ifndef __AXISDEF_H
#define __AXISDEF_H

#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include "mmcdef.h"
#include "amc.h"
#include "internalfuncs.h"
#include <string.h>
#include <stdio.h>

// AMC Model setting
#define FOUP_7_0                    0         
#define MAC_7_0                     1                      
#define PCARD_7_0                   2         
#define PCARD_7_0_JIG               3	 
#define FOUP_V70_REV_01		    	4
#define FOUP_V70_REV_01_SERVICE		5
#define OHT_NAME_MASK                 6
#define OHT_NAME_MASK_PATROL         8

#define MAGAZINE		   			    11
#define CASSETTE   					12
#define NEO_TRAY   					13
#define NEO_MAGAZINE   				14
#define NEO_SSD   					    15
#define NEO_CASSETTE   				16	
#define FOSB_4WD	    			17
#define FOSB_2WD	    			18
#define P_FOUP						19
#define OHT_NAME_PATROL_CHINA  	20
#define EAD_CST  					21
#define NEO_SSD_TRAY				22

#define OHT_NAME_MGZ_APJT      	23
#define OHT_NAME_CST_APJT       	24
#define OHT_NAME_FOSB_APJT      	25
#define OHT_NAME_TRAY_APJT        26
#define OHT_NAME_MAC_APJT        	27

#define OHT_NAME_MODULETRAY         28

#define OHT_STD_01  				   30
#define OHT_NAME_STD_V80_SERVICE    33

#define OHT_NAME_MAC_L3			    39

#define OHT_NAME_MAC_L5    		   40
#define OHT_NAME_MAGAZINE_L5        41
#define OHT_NAME_CASSETTE_L5  	   42
#define OHT_NAME_NEO_TRAY_L5        43
#define OHT_NAME_FOSB_4WD_L5  	   44
#define OHT_NAME_BOAT_L5            45
#define OHT_NAME_RETICLE_L5  		   46
#define OHT_NAME_PATROL_L5          47

#define OHT_NAME_SDI_EVL            50
#define OHT_NAME_SDI_EVS            51
#define OHT_NAME_SDI_EVL_C   		52

#define OHT_NAME_NEO_TRAY_HIGH		55	// 210125

//////OHT 6.x 추가/////////////////////////
#define OHT_NAME_FOUP_6_0		    71
#define OHT_NAME_FOUP_6_3		    72
#define OHT_NAME_RETICLE_2_0		73
#define OHT_NAME_FOUP_6_5		    74
#define OHT_NAME_RETICLE_3_0		75
#define OHT_NAME_PATROL_6_0		    76
#define OHT_NAME_PATROL_6_3		    77
#define OHT_NAME_PATROL_6_5		    78
#define OHT_NAME_MGZ_1_0		    79
#define OHT_NAME_FOSB_1_0		    80
#define OHT_NAME_MGZ_2_0		    81
#define OHT_NAME_COMPACT_1_0		82
#define OHT_NAME_TRAY_1_0		    83
#define OHT_NAME_MAC_V60            84
////////////////////////////////////////////

#define EN_SYNCMOTION_BY_DESIRED_VEL    0
#define EN_CRUISE_MODE  1

#define PERIOD          0.0002
#define FREQ            5000
#define CNT_PER_MS      5
#define ACC_IN_RATE     0
#define ACC_IN_TIME     1
#define NUM_OF_PHASES   3
#define MS_TO_SEC       0.001

#define SCURVE_MAX_BUF_SIZE    			    FREQ	    // 1sec
#define SCURVE_MIN_BUF_SIZE      		    50		    // 10msec
#define SCURVE_MAX_SMOOTHING_FACTOR    	    0.5
#define SCURVE_MIN_SMOOTHING_FACTOR         0		

#define	EEPROM_BOOTPARAM_MAGIC_NO		0x9abcdef0
#define	EEPROM_BOOTPARAM_START_OFS		0X0000
#define	EEPROM_MAP_START_OFS			0X2000

// 2.5.x, 2.8.x 통합 버젼으로 이버젼은 2.9.x의 하나의 버젼으로 사용한다.
// pcb1.3의 경우 fpga(2.8.12)도 같이 업데이트해서 사용한다고함
// 그러면 2.5.x 버젼은 필요 없으로므 버젼 업해서 하나로 사용하기로 결정함 - 양주호 선임과 협의된 사항.
// 즉 __AMC_25x는 정의해서는 안됨.
// 2.9.8  --> 2.10.01수정건임, 마지막 버젼만 바꿈  

#if defined(__AMC_29x)
#define	VERSION_STRING	"2.09.09"	//2012.01. 09				
#define	VERSION_DSP		20909      	// WXXYY, ver:ax#, W.XX.YY
#elif defined(__AMC_SMD)
#define	VERSION_STRING	"2.10.03"	//2012.01. 09				
#define	VERSION_DSP		21003      	// WXXYY, ver:ax#, W.XX.YY	
#elif defined(__AMC_V70)
#define	VERSION_STRING	"1.71.01"	//2012.01. 09
#define	VERSION_DSP		17101     	// WXXYY, ver:ax#, W.XX.YY
#else		//__AMC_29x버젼 DEFAULT
#define	VERSION_STRING	"2.09.09"	//2012.01. 09
#define	VERSION_DSP     20909
#endif

#define	USERIN_SIZE	    8
#define	USERIN_MASK     0xFF	   // WXXYY, ver:ax#, W.XX.YY
#define	USEROUT_SIZE    8
#define	USEROUT_MASK    0xFF

// 2.5.25v2.8.07통합, 120111, syk 함수 추가  : Put_Vel_Curve, Get_Vel_Curve(181+2)
// 110323 2.9.3 syk함수 추가 (183+2)
// 120731 2.9.7 syk AMCStatusGet2, debug 메시지 추가(block box)
// 120831 2.9.8 syk , IN/OUTPUT IO(256/256)접점 추가로 인한 함수 추가(186+2)  
// 120831 2.9.8 syk , OHT v7.0, SMD IN/OUTPUT 접점 사용 수 제한을 위한 함수 추가(188+2)  

#if defined(__AMC_29x)
#define	MAX_COMM_NUM	186
#elif defined(__AMC_SMD)
#define	MAX_COMM_NUM	191
#elif defined(__AMC_V70)
#define	MAX_COMM_NUM	204
#else		// __AMC_29x버젼 DEFAULT
#define	MAX_COMM_NUM	186
#endif

#define	BOOT_SAVE_WAIT	10000000
#define	BOOT_FRAME_SIZE	196

#define	MMC_AXIS 		4        	// reserved 
#define	JPC_AXIS 		MMC_AXIS	// number of actual axis
#define	EVENT_MSG 		30        	// reserved //120803 2.9.7 syk
#define	MOTIONMAKE_MSG	12        	// reserved //120803 2.9.7 syk
#define	MOTIONCAL_MSG 	40        	// reserved //120803 2.9.7 syk

#define	COUNT_MAX		5 		    // number of queue 
#define	MAX_Q_NUM      	50		    // number of motion queue

#define	MIN_DECEL		6		// 최소 감속 기울기, 2007.11.13(오), 2008.3.31, 3=>6, 가속 시간 5ms 이하에서 작은 위치 이동시 위치가 틀어짐.
#define	MIN_ACCEL		6		// 최소 가속 기울기, 2007.11.13(오)

#define	OK  			1
#define	ERROR			0

#define	NO				ERROR
#define	YES				OK

#define	ITP_MODE_STOP	0
#define	ITP_MODE_MOVE	1

#define	TRAPEZOID       1
#define	SCURVE          2
#define	RELATIVE		3
#define	S_RELATIVE		4
#define	PARABOLIC       5
#define	AX_DELAY		6
#define	AX_IO_TRIGGER	7

#define	PI	            (atan2(1.0,1.0)*4.0)

#define	P_GAIN			150				// initial P_gain  define
#define	I_GAIN			1   	   		// initial I_gain  define
#define	D_GAIN			0   	   	  	// initial D_gain  define
#define	F_GAIN			300    			// initial F_gain  define
#define	J_GAIN			00     	  		// initial J_gain  define
#define	I_LIMIT			1000   			// initial I_limit define

#define AXIS_START      1				// Command start     
#define AXIS_MOVING     2       		// jpc에서 동작중    
#define AXIS_CHANGE     3 	      		// jpc에서 동작중    
#define SEMI_END        4 	      		// command end       
#define AXIS_END        5 	    	  	// command end       
#define GOAL_MOV_ERR    6  		  		// Goal Position move error

#define	NO_EVENT		0
#define	STOP_EVENT		1
#define	E_STOP_EVENT	2
#define	ABORT_EVENT		3
#define	ENC_OVF_EVENT	4	// set if encoder overflow is present. actions is same as e-stop. ckyu, 2007.10.12 
#define	PAUSE_EVENT		5	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.

#define	ST_NONE             0x0000
#define	ST_HOME_SWITCH		0x0001
#define	ST_POS_LIMIT		0x0002
#define	ST_NEG_LIMIT   		0x0004
#define	ST_AMP_FAULT     	0x0008

#if (defined(__AMC_SMD) || defined(__AMC_29x))
#define	ST_A_LIMIT    		0x0010
#endif

#if defined(__AMC_V70)
#define	ST_SYSTEM_INSEC    	0x0010
#endif

#define	ST_V_LIMIT  		0x0020
#define	ST_X_NEG_LIMIT 		0x0040	
#define	ST_X_POS_LIMIT 		0x0080	// 2009.10.21(오), +와 - Limit이 동시에 발생하는 경우는 Working Limit 에러에 의한 Stop Event 상태를 의미
#define	ST_ERROR_LIMIT   	0x0100
#define	ST_PC_COMMAND  		0x0200
#define	ST_OUT_OF_FRAMES  	0x0400
#define	ST_AMP_POWER_ONOFF	0x0800
#define ST_HWINPOSITION		0x2000	// 2009.10.21(오), Abort 상태인 FPGA 내부의 초기화 값이 바뀐 경우의 이상 상태를 확인하기 위함.?
#define	ST_SWINPOSITION		0x4000
#define	ST_MUST_REBOOT		0x8000	// 2009.10.21(오), 서보 Ready 신호와 Watchdog 신호가 발생하면 사용하는 Flag

// 2009.10.22(오), 인코더 회전 방향이 초기화가 깨진 경우(FPGA와 내부 변수가 다른)
									
#if defined(__AMC_V70)
#define	REF_POWER_FPGA12		32767 	// +-12v공용, 
#define	REF_POWER_FPGA5P		13653	// 13652.91(32767*12/5)
#define	REF_POWER_FPGA3P3		9011	// 9010.92(32767*12/3.3)
#define	REF_DAC_AXISOUTPUT		20194 	// DAC 

#define	REF_POWER_R_Dac0
#define	REF_POWER_R_Dac1
#define	REF_POWER_R_Dac2
#define	REF_POWER_R_Dac3

#define	DEFAULT_POWER3P3_VAL	330
#define	DEFAULT_POWER5P_VAL		500
#define	DEFAULT_POWER12P_VAL	1200
#define	DEFAULT_POWER12M_VAL	-1200

#define	DEFAULT_POWER3P3_ADCSCALE_VAL	41779	// (-12V ~12V, 0~65535)
#define	DEFAULT_POWER5P_ADCSCALE_VAL	46421
#define	DEFAULT_POWER12P_ADCSCALE_VAL	65535
#define	DEFAULT_POWER12M_ADCSCALE_VAL	0

#define	POWER3P3	0
#define	POWER5P		1
#define	POWER12P	2	
#define	POWER12M	3
#define	AXIS0_VEL	4
#define	AXIS1_VEL	5
#define	AXIS2_VEL	6
#define	AXIS3_VEL	7

#define POWER3P3_CHECK_TIME		500
#define POWER5P_CHECK_TIME		500
#define POWER12P_CHECK_TIME		500
#define POWER12Pm_CHECK_TIME	500

#define POWER3P3_CHECK_Percent	5
#define POWER5P_CHECK_Percent	5
#define POWER12P_CHECK_Percent	10
#define POWER12Pm_CHECK_Percent	10

#define POWER3P3_REF	330
#define POWER5P_REF		500
#define POWER12P_REF	1200
#define POWER12Pm_REF	1200

#define Power3_3V_Upper_Limit	(POWER3P3_REF + ((POWER3P3_REF*POWER3P3_CHECK_Percent)/100))
#define Power3_3V_Lower_Limit	(POWER3P3_REF - ((POWER3P3_REF*POWER3P3_CHECK_Percent)/100))

#define Power5V_Upper_Limit		(POWER5P_REF + ((POWER5P_REF*POWER5P_CHECK_Percent)/100)) 	
#define Power5V_Lower_Limit		(POWER5P_REF - ((POWER5P_REF*POWER5P_CHECK_Percent)/100)) 	

#define Power12VP_Upper_Limit	(POWER12P_REF + ((POWER12P_REF*POWER12P_CHECK_Percent)/100))
#define Power12VP_Lower_Limit	(POWER12P_REF - ((POWER12P_REF*POWER12P_CHECK_Percent)/100))

#define Power12VM_Upper_Limit   (POWER12Pm_REF + ((POWER12Pm_REF*POWER12Pm_CHECK_Percent)/100))
#define Power12VM_Lower_Limit   (POWER12Pm_REF - ((POWER12Pm_REF*POWER12Pm_CHECK_Percent)/100))

#define	PERCENT0				0
#define	PERCENTINIT_5_percent	5
#define	PERCENTINIT_10_percent	10
#define	PERCENT100				100

#define	DACPADC		0.61629		// 20194/32767
#define	VOLTPADC    0.03662		// 1200/32767   // 1200 = 12(V) * 100(scale) 
#define	PPSTPDAC	40.5665     // 819200/20194
#endif

#define	DMT_VMOVE	1
#define	DMT_MOVEP	2
#define	DMT_MOVEN	3
#define	DMT_MOVES	4
#define	DMT_MOVEDS	5
#define	DMT_PTP		6
#define	DMT_IO		7
#define	DMT_DWELL	8

typedef struct
{
    volatile int Moving_Flag;
    volatile int Moving_Axis[JPC_AXIS];
    volatile int Int_Moving_Timer[JPC_AXIS];
    volatile int Gp_Chk_Cnt[JPC_AXIS];
    volatile int Moving_Timer[JPC_AXIS];
    volatile int JntCurrPos[JPC_AXIS];
    volatile int Offset_Axis;
}JPCMOVSTATUS;

typedef struct _JPCPAUSE
{
    volatile int Flag[JPC_AXIS];
    volatile int Acc[JPC_AXIS];
    volatile int Timer[JPC_AXIS];
    volatile int WaitState[JPC_AXIS];	// 0:normal, 1:wait for reason release
    volatile int WaitCount[JPC_AXIS];

    void Init()
    {
        for (int i = 0; i < JPC_AXIS; i ++)
        {
            Flag[i] = 0;
            Acc[i] = 0;
            Timer[i] = 0;
            WaitState[i] = 0;
            WaitCount[i] = 0;
        }
    }
}JPCPAUSE;	

// 2007.10.15, ckyu
// Pause를 구현하기 위해서 마지막 실행된 명령을 저장하는 구조체
// PTPMove, velmove를 모두 저장한다.
enum { PAUSERST_NONE = 0, PAUSERST_V_MOVE, PAUSERST_V_MOVE_POS, PAUSERST_PTP_MOVE };

typedef struct 
{
	volatile int cmd[JPC_AXIS];	        // 0:nothing, 1: velmove, 2:위치값이 있는 move_ds, 3:ptpmove,  
	volatile int nTimeCmd[JPC_AXIS];	// 0: move_p, move_n, move_ds(), 1:move_pt, move_nt, move_dst() 명령
	volatile float fVel[JPC_AXIS];	    // vel move의 파라미터1,
	volatile float a1[JPC_AXIS];	    // vel move의 파라미터2
	volatile float a2[JPC_AXIS];	    // vel move의 파라미터2
	
	volatile float vPos[JPC_AXIS];		// v_move로 도달할 위치를 검사하기위한 변수저장 용도.

	volatile int Acc[JPC_AXIS];
	volatile int Dcc[JPC_AXIS];
	volatile int nPos[JPC_AXIS];
	volatile int nStartPos[JPC_AXIS];
	volatile int profile[JPC_AXIS];
	
	// vdiff : 현재속도하고, 새로지정한 속도간의 차이값, => 재시작시 새로운 acc값 결정에 이용한다.
	void SetVMoveParam(int ax, float fvel, float cora, float vdiff, int ntimecmd)
	{
		nTimeCmd[ax] = ntimecmd;
		fVel[ax] = fvel;
		a1[ax] = cora;
		vPos[ax] = vdiff;
		cmd[ax] = (__IsPauseOn(ax) ? PAUSERST_NONE : PAUSERST_V_MOVE);
	}
	
	void SetPtpMoveParam(int ax, int _profile, int npos, int acc, int dcc, float fvel)//, int nCurPos)
	{
		nTimeCmd[ax] = 0;
		profile[ax] = _profile;
		nPos[ax] = npos;
		Acc[ax] = acc;
		Dcc[ax] = dcc;
		fVel[ax] = fvel;
		cmd[ax] = (__IsPauseOn(ax) ? PAUSERST_NONE : PAUSERST_PTP_MOVE);
	}
	
	void SetVMovePosParam(int ax, float _a1, float _a2, float fvel, float npos, int ntimecmd)
	{
		nTimeCmd[ax] = ntimecmd;
		fVel[ax] = fvel;
		a1[ax] = _a1;
		a2[ax] = _a2;
		vPos[ax] = npos;
		cmd[ax] = (__IsPauseOn(ax) ? PAUSERST_NONE : PAUSERST_V_MOVE_POS);
	}
} PAUSE_RST;	// pause restart 

typedef struct
{
    volatile int Pos[JPC_AXIS];
    volatile int NewPos[JPC_AXIS];
    volatile int CurPos[JPC_AXIS];
    volatile float Vel[JPC_AXIS];
    volatile int Acc[JPC_AXIS];
    volatile int Dcc[JPC_AXIS];
    volatile int Flag[JPC_AXIS];
    volatile int Profile[JPC_AXIS];
    volatile int Cmd;
} AXIS_STATUS;

typedef struct  
{
    volatile int N1;
    volatile int N2;
    volatile int N3;
    volatile int N4;
    volatile int Ds;
    volatile float Acc_Div;
    volatile float Dcc_Div;
    volatile float Acc_Rate;
    volatile float Dcc_Rate;
    volatile float V;
}TRAJ_PARA;

typedef struct  
{
    volatile int Command;
    volatile int Len;		// 2007.12.17, vmove_pos에서 mode를 저장하기위한 용도로 이용, ckyu
    volatile int Profile;
    volatile float Vel;
    volatile int Acc;
    volatile int Dcc;
    volatile int Axis;
    volatile int Pos;
    volatile int Delay;
    volatile int IO_Trigger_Bit;
    volatile int IO_Trigger_St;
    volatile int Cir_Direction;
    volatile float Ox;
    volatile float Oy;
    volatile float Angle;
    volatile float L_Vel;
    volatile int L_Acc;
    volatile float L_Deg;
    volatile float Spl_Pos;
    volatile float adv_vel;	// vmove_pos를 위한 변수
    volatile float adv_acc;	// vmove_pos를 위한 변수
    volatile float adv_pos;	// vmove_pos를 위한 변수

    volatile float cmd_a1;	// a1
    volatile float cmd_a2;	// a2
    volatile float cmd_v;	// v
    volatile float cmd_pos;	// pos
    volatile int Cir_Dir;
}AQ_BUF;

typedef	void	 (*FUNCPOINT)(void);

typedef	struct 
{
    volatile int jtpos;                 // current actual position
    volatile int jtpos_b;		        // previous actual position

    volatile double mdRefpos;		    // interpolation position, 이전 목표 위치
    volatile double mdRefpos_b;	        // previous interpolation position

    volatile float	velo_z0;		    // current velocity,목표 속도
    volatile float	error_z0;		    // current position error
    volatile float	error_z1;		    // previous position error

    volatile float	isum;			    // integrated error
    volatile float	j_z0;			    // current  acceleration

    volatile float	velfbz0;		    // velocity feedback buffer

    volatile float	velcmd;		        // velocity command 
    volatile float velcmd_b;		    // previous velocity command 
    volatile float	velerrz1;		    // absolute velocity 
    volatile float	velerr;		        // velocity error   
    volatile float velisum;		        // velocity SUM	    
    volatile float	tqrcmd;		        // torque int input
    volatile float pos_ierr;		
    volatile float vel_ierr;		

    volatile double dtBox;		        // distance channel, 2007.11.2

    // moniter
    volatile int encoder;		        // encoder counter value
    volatile float encoder_err;	        // goal_pos - encoder_counter/4
    volatile int encoder_vel;	        // encoder counter velocity


    // timer or flags
    volatile int g_on;			        // pwm on/off flag
    volatile int offset_on;		
    volatile int encoder_type;	        // Encoder or Bipolar)Analog device
    volatile int loop_cfg;		        // Encoder Index Pulse Use/Unuse
    volatile int voltage_type;	        // Unipolar Device or Bipolar Device
    volatile int in_pos_flag;	        // In_Position Flag
    volatile int in_sequence;	        // On Motioning
    volatile int inPosIndex;		    // flag for checking in-position 

    // parameters
    volatile unsigned int pgain;	    // P-gain
    volatile unsigned int igain;	    // I-gain
    volatile unsigned int dgain;	    // D-gain
    volatile unsigned int fgain;	    // F-gain
    volatile unsigned int jgain;	    // J-gain
    volatile unsigned int ilimit;	    // error sum value limit
    volatile int amp_on_level;	        
    volatile unsigned int velpgain;	    // velocity Pgain
    volatile unsigned int veligain;	    // velocity Igain
    volatile unsigned int veldgain;	    // velocity Dgain
    volatile unsigned int velfgain; 	// velocity FFgain
    volatile unsigned int veljgain; 	// velocity ACCgain
    volatile unsigned int velilimit;	// velocity ilimit
    volatile int motortype;		        // Servo=0 , Stepper=1
    volatile float in_position;	        // In_Position Range
    volatile int control_cfg;	        // Velocity_Cont(0), Torque_Cont(1)
    volatile int posimode;		        // In_Standing(0), In_Always(1), i gain을 언제 사용할 것인가를 선택하기 위한 레지스터
    volatile int velimode;		        // In_Standing(0), In_Always(1)
    volatile int dac_bias;		        // Velocity Voltage Out Data
    volatile int swupper_limit;	        // software upper travel limit
    volatile int swlower_limit;	        // software lower travel limit	
    volatile unsigned int stop_rate;		        // STOP EVENT deceleration rate	
    volatile unsigned int e_stop_rate;	        // E_STOP EVENT deceleration rate	
    volatile int encDir;		        // Encoder Direction
    volatile int encRes;		        // Encoder Resolution
    volatile int comRes;		        // Command Resolution
    volatile float ratio;		

    // servo or sensor
    volatile int home_level;		    // Home Sensor level define
    volatile int fault_level;	        // Servo Driver Error level define
    volatile int reset_level;	        // Servo Driver Reset level define
    volatile int pos_level;		        // Positive Sensor level define
    volatile int neg_level;		        // Negative Sensor level define
    volatile int inpos_level;  	        // In Position level define

    volatile int event_st;		        // Define Event
    volatile int axis_source;	        // Axis event define
    volatile int pos_sn_st;		        // Positive Sensor Event
    volatile int neg_sn_st;		        // Negative Sensor Event
    volatile int home_sn_st;		    // Home Sensor Event
    volatile int limit_err_st; 	        // Limit Error Event	
    volatile int amp_event;		        // Amp Fault Event Define
    volatile int swupper_limitst;	    // software upper travel limit event
    volatile int swlower_limitst;	    // software lower travel limit event

    volatile int delay_c;		        // servo check delay reduce noise
    volatile int in_vcu1;		        // servo states value
    volatile int IO_Trigger_Bit;	    // servo states value
    volatile int init_counter; 
    volatile int set_counter;	        // counter value set
    volatile int limit_err_chk;	        // Envelope Error Value

    volatile int sn_st;		            // Current Sensor Status
    volatile int sn_st1;		        // 1 step delay Sensor Status
    volatile int IO_Trigger_St;     	// 1 step delay Sensor Status
    volatile int sn_c;			        // Sensor Noise Counter

    volatile int index_req;		        // Encoder Index Pulse En/Dis
    volatile int clkdac_bias;	        // Velocity Voltage Out Data

    // 10sec routine variables
    volatile float	vm_prev_vel;	    // v_move previous slew velocity	
    volatile int vm_acc;
    volatile int vm_dcc;		        // Velocity Mode dcc count, 2006.11.18(Oh)

    volatile float	vm_delt_s;		    // Velocity Mode acceleration flag
    volatile int vm_accflag;		    // Velocity Mode acceleration flag
    volatile int vm_dccflag;		    // Velocity Mode deceleration flag
    volatile int temp;			        // for debug
    volatile int vm_bMoveDs;

    // 2012.1.9, overspeed, ohs move_ds의 마지막 위치 보정임을 나타내며,
    // 마지막 보정값이 임의의 부호를 가질 수 있어 
    // 마지막 보정할 때에는 다음 move_ds가 실행되지 않도록 하기 위해 사용 //2012.2.15 // 2.5.26 // 2.8.08
    volatile int vm_bLastDs;	

    //-- Added by AMC, 2006.11.17
    volatile float V_TrackingFactor;	// default = 1.0, 속도 트랙킹을 위한 비율.
    //-- Added by AMC, 2006.12.07(오)
    volatile int dac_code;		        // 현재 출력되고 있는 DAC 값(Offset 및 Tracking Factor는 고려하지 않음.)

    // Bootpara에 있는 Accel_Limit를 추가함.
    // 2007.1.23, ckyu
    volatile unsigned int Accel_Limit;

    // 2007.10.19, ckyu
    // BootPara에 있는 Vel_Limit를 저장하는 변수가 없어서 추가함.
    volatile unsigned int Vel_Limit;

    // 2007.4.13, ckyu
    volatile int EncoderOffset;

    // 동작중 이상 현상이 발생할 경우 Abort Event를 발생시키기 위해 추가한 변수들
    // 2008.5.30(오)
    volatile int move_upper_limit;	//	현재 모션에서 + 방향으로 이동할 수 있는 최고 위치
    volatile int move_lower_limit;	//	현재 모션에서 - 방향으로 이동할 수 있는 최저 위치
    
    // e-stop rate 에서 자동으로 계산토록 변경한다.
    // 공식) 8192 * 100. * e_stop / 1000
    // 100.: 6000 rpm이면 초당 100회전
    // e_stop / 1000 : 내부 stop 관련 정지루틴의 주기는 1ms 이므로 
    // 2008.6.2, ckyu
    // 최대 5회전 을 넘지 않도록 한다.
    volatile int working_limit;		// INI 파일에서 설정 가능, 현재 모션에서 이동 가능한 작업 영역을 나타내는 펄스값

    // 모션 동작중에 속도 파형의 이상 현상을 방지하기 위해 추가된 부분
    volatile int vel_max_limit;		// 현재 모션에서 이동할 최대 속도?
    volatile int vel_min_limit;		// 현재 모션에서 이동할 최저 속도?
    volatile float vel_limit_ratio;	// 정지 제어시 사용할 최대 속도와의 비율, 모든 속도는 이 속도로 제한됨. 

    volatile int bMustReboot;	

    volatile int limit_err_chk_cnt;	// 2009.11.3(오), 에러 리미트값이 벗어난 회수를 카운트하여  Event 처리하려고 만들었음.?
    volatile int acc_rate;	        // 2010.11.2, 가속도 상태를 나타내는 Flag로 1ms 루틴에서 계산해야 함.
    volatile double	dtBox_b;	    // 이전 dtBox 값  -->  // 120725 2.9.06 syk dtBox_b용도변경 : move_ds명령에 의해 프로파일 생성할 때 사용된 dtbox값 저장.
    volatile int curve_limitst;			// 2.5.25v2.8.07통합, 120111, syk, curve구간의 과속 방지 이벤트	 // 용도 변경 : profile의 상하 bnad margin의 사용 유무
    volatile int curve_limit;			// 2.5.25v2.8.07통합, 120111, syk, curve구간의 과속 방지 limit값  // 용도 변경 : profile의 상하 bnad margin	
    volatile int limit_curve_chk_cnt;	// 2.5.25v2.8.07통합, 120111, syk, curve구간의 과속 횟수 
    volatile int limit_swlower_chk_cnt;	// 2.5.25v2.8.07통합, 120111, syk, software lower travel limit 횟수
    volatile int limit_swupper_chk_cnt;	// 2.5.25v2.8.07통합, 120111, syk, software upper travel limit 횟수

    volatile int actvel_margin;			// 2.9.3, 120323, syk, actvel margin 값 
    volatile int actvel_marginst;		// 2.9.3, 120323, syk, actvel margin 값 초과시 event
    volatile int actvel_margintime;	 	// 2.9.3, 120323, syk, actvel margin 값 초과시 유지시간 
    volatile int profile_limit;	 	    // 2.9.3, 120323, syk, actvel margin 값 초과시 유지시간 	 
    volatile int ds_profile_point;	 	// 2.9.6, 120726, syk, move_ds명령시 프로파일 생성 위치값을 갖음.
    volatile int checked_back_motion;   // 2.9.6, 120726, syk, move_ds명령시 후진하는 개수 체크.

#if (defined(__AMC_SMD) || defined(__AMC_V70))	 
    volatile int VersatileIO;	 		// 120831 2.9.8 SYK 0축 : input io count, 1축 : output io count 2축 : reserve 3축 : reserve  
#endif

#if defined(__AMC_V70)
    volatile int Moni_R_Adc;			// DAC OUTPUT 출력 ADC한 값(FPGA에서 읽어옴)
    volatile int Moni_R_Dac;			// DAC OUTPUT 출력 ADC한 값(FPGA에서 읽어옴)
    volatile int dsp_dac_out;	 		// DAC OUTPUT 출력(DSP에서 FPGA에 출력한 값) 

    volatile int dac_code_b;	 		// 현재-1(200us전) 출력되고 있는 DAC 값
#endif 	 

    volatile int nPrfCtr;

    volatile float fProfFinalVel;
    volatile float fAccRaw;
    volatile float fOnTheFlyElapsedPos;
    volatile float fBgAcc;

    volatile double dProfFinalPos;
    volatile double dBgPosCmd;
    volatile double fBgTgtVel;
    volatile double dPosCmd;

    volatile float fScurveBuf[SCURVE_MAX_BUF_SIZE];
    volatile double fScurveSum;
    volatile int nScurveBufIdx;
    volatile int nScurveBufSize;

    volatile float fScurveRatio;

    volatile float dtBoxRaw;
    volatile double dPosCmdRaw;
} CONTROLBUF;

typedef	struct 
{
    int	command;		//	0 	; main command	
    int exram_cnt;		//  1	; user ram address count
    int	z_index_ax;		//  2	; LED DATA FLAG	
    int	z_index_flag;	//  3 	; run margin timer
    int	sys_io_out;		//	4	; pwm on/off	
    int	user_io_out;	//	5-1	; 				
    int	user_io_out2;	//	5-2	; 				
    int	zr_enable;		//  6	;			
    int	delay_time;		//	7	; For mmcDelay Function
    int	io_int_ax;		//	8	; I/O Interrupt Axis	
    int	sync_gain;		//	12	; Synchronous Motion Gain

#if (defined(__AMC_SMD) || defined(__AMC_V70))		
    int	user_io_out3;	//	5-3	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output 			
    int	user_io_out4;	//	5-4	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output 
    int	user_io_out5;	//	5-5	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output  				
    int	user_io_out6;	//	5-6	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output 
    int	user_io_out7;	//	5-7	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output  				
    int	user_io_out8;	//	5-8	; // 120831 2.9.8 SYK  추가 접점에 대한 IO output 	
#endif
} GLOBALBUF;

// 이 구조체는 PC측의 BOOT_FRAME_TYPE과 동일한 순서와 크기로 맞추어져 있음.
// 2007.3.13, ckyu
typedef struct _ST_BOOTPARA
{
    unsigned int Vel_Limit[JPC_AXIS]; 		
    unsigned int Accel_Limit[JPC_AXIS];

    unsigned int PulseRatio[JPC_AXIS];    	// system gear ratio    
    float GearRatio[JPC_AXIS];    	        // system gear ratio    

    float HwLower_Limit[JPC_AXIS]; 	        // system lower limit   
    float HwUpper_Limit[JPC_AXIS]; 	        // system upper limit   
    int SwLower_Limit[JPC_AXIS]; 	        // system lower limit   
    int SwUpper_Limit[JPC_AXIS]; 	        // system upper limit   

    unsigned int PGain[JPC_AXIS];       	// P gain    value      
    unsigned int IGain[JPC_AXIS];        	// I gain    value      
    unsigned int DGain[JPC_AXIS];        	// D gain    value      
    unsigned int FGain[JPC_AXIS];        	// F gain    value      
    unsigned int ILimit[JPC_AXIS];       	// I_LIMIT   value      

    unsigned int VPgain[JPC_AXIS];       	// VELP gain    value   
    unsigned int VIgain[JPC_AXIS];        	// VELI gain    value   
    unsigned int VDgain[JPC_AXIS];        	// VELD gain    value   
    unsigned int VFgain[JPC_AXIS];        	// VELF gain    value   
    unsigned int VIlimit[JPC_AXIS];    		// VELI_LIMIT   value   

    float In_Position[JPC_AXIS];
    unsigned int Error_Limit[JPC_AXIS];
    char Motor_Type[JPC_AXIS];

    char SwUpper_LimitSt[JPC_AXIS];
    char SwLower_LimitSt[JPC_AXIS];

    char Pos_Level[JPC_AXIS];
    char Neg_Level[JPC_AXIS];
    char Home_Level[JPC_AXIS];
    char Amp_Level[JPC_AXIS];
    char Amp_Reset_Level[JPC_AXIS];

    char Pos_Limit_St[JPC_AXIS];
    char Neg_Limit_St[JPC_AXIS];
    char Home_Limit_St[JPC_AXIS];
    char Error_Limit_St[JPC_AXIS];

    char Encoder_Cfg[JPC_AXIS];
    char Voltage_Cfg[JPC_AXIS];
    char Home_Index[JPC_AXIS];

    unsigned int Stop_Rate[JPC_AXIS];
    unsigned int E_Stop_Rate[JPC_AXIS];

    char Control_Cfg[JPC_AXIS];
    char Loop_Cfg[JPC_AXIS];
    char Amp_OnLevel[JPC_AXIS];

    int Io_Int_Enable[JPC_AXIS];
    char Int_Event_St[JPC_AXIS];
    char Amp_Fault_Event[JPC_AXIS];

    char PosImode[JPC_AXIS];
    char VelImode[JPC_AXIS];
    char PulseMode[JPC_AXIS];

    int Inpos_Level[JPC_AXIS];	// SRDY신호의 극성 지정, 2008.11.19, ckyu

    int	Dpram_Addr[JPC_AXIS];
    int Axis_Num;
    int Action_Axis_Num;

    int UserIO_BootMode;
    int	UserIO_BootValue;


    // 2006.11.16, AMC
    // offset variable added
    int	dac_bias[JPC_AXIS];
    float V_TrackingFactor[JPC_AXIS];

    // 2007.4.13, ckyu
    // 인코더의 0 위치를 설정하기 위한 용도.
    int	Encoder_Offset[JPC_AXIS];

    // 2007.10.10
    int	Encoder_direction[JPC_AXIS];
    int	Motor_Pause[JPC_AXIS];
    int	Motor_Pause_Level[JPC_AXIS];
    int	Motor_Pause_CheckBit[JPC_AXIS];

    // 2008.3.5, 필터관련 변수 추가함. ckyu.
    // PC측 라이브러리의 pcdef.h (BOOT_FRAME_TYPE)에 추가된 사항임.
    int	naPositionNotchFreq[JPC_AXIS];	// 0이면 필터링 안함
    int	naPositionLPFFreq[JPC_AXIS];	// 0이면 필터링 안함
    int	naVelocityNotchFreq[JPC_AXIS];	// Torque 관련, 0이면 필터링 안함
    int	naVelocityLPFFreq[JPC_AXIS];	// Torque 관련, 0이면 필터링 안함

    float ScurveSmoothingFactor[JPC_AXIS];    

    _ST_BOOTPARA()
    {
        memset(this, 0, sizeof(_ST_BOOTPARA));
        for (int i = 0; i < JPC_AXIS; i ++)
        {
            dac_bias[i] = 0;
            V_TrackingFactor[i] = 1.0;
            Encoder_direction[i] = CIR_CCW;
            Motor_Pause[i] = 0;	        // off
            Motor_Pause_Level[i] = 1;	// Active High
            Motor_Pause_CheckBit[i] = 0;

            // 필터링관련 변수를 초기화 한다.
            naPositionNotchFreq[i] = 0;
            naPositionLPFFreq[i] = 0;
            naVelocityNotchFreq[i] = 0;
            naVelocityLPFFreq[i] = 0;

            ScurveSmoothingFactor[i] = 0.1;
        }
    }       
} ST_BOOTPARA;

typedef struct _UBOOTPARA
{
	int nMagic;
	int	nBlockSize;
	char nBoardNo;
	char nAxisNo;
	char nVerMaj;
	char nVerMin;
	ST_BOOTPARA st_boot;
	
	int	nCRC32;
	
	int GetBodySize() 
	{
	    return sizeof(_UBOOTPARA) - sizeof(int); 
	}
	void* GetBodyTopPtr() 
	{
	    return (void *) &nMagic; 
	}
	void SetCRC32(int ncrc) 
	{
	    nCRC32 = ncrc; 
	}
	int GetCRC32() 
	{
	    return nCRC32; 
	}
	int IsValidParam(int ncrc)
	{
		if (nMagic != EEPROM_BOOTPARAM_MAGIC_NO)
		{
    		return -1;
    	}
		if (ncrc != nCRC32) 
	    {
    		return -1;
    	}
		return 0;
	}
	
	_UBOOTPARA()
	{
		nMagic = EEPROM_BOOTPARAM_MAGIC_NO;
		nBlockSize = sizeof(_UBOOTPARA);
		nBoardNo = 1;
		nAxisNo = JPC_AXIS;
		nVerMaj = 1;
		nVerMin = 0;
	}
} UBOOTPARA;

typedef struct 
{
	int circcw;			// 회전방향
	int encoder_type;	// 절대치, 0:인크리멘털 인코더
	
	int cmd;			// 모션명령의 종류
	float acc;			// 가속기울기
	float dcc;			// 감속 기울기
	float vel;			// 목표 최대 속도
	int pos;			// 목표점
	
	void InitCmd ()
	{
		cmd = pos = 0;
		acc = dcc = vel = 0.;
	}
} DBG_CMD_BUFFER;

typedef struct 
{
	int loop_main;		// main 루프 카운터?
	int loop_200us;		// 200us 인터럽트 카운터
	int loop_1ms;		// 1ms 인터럽트 카운터?
	int loop_DPRAM;		// DPRAM 인터럽트 카운터
	
	void InitCmd ()
	{
		loop_main = loop_200us = 0;
		loop_1ms = loop_DPRAM = 0;
	}
} DBG_CNT_BUFFER;

// DPRAM에 저장될 상태를 저장하기위한 구조체
typedef struct _DBG_MSG_BUFFER
{
    int jtpos;
    float mfGoal_pos;
    float acc;
    float dcc;
    float vel;
    float vm_WaitPos;
    float velcmd;
    float velerr;
    float tqrcmd;
    float dtBox;
    int pos;
    short axis_source;
    short dac_code;
    char event_st;
    char vm_accflag;
    char vm_count;
    char vm_flag;
    char vm_bMoveDs;
    char vm_bPosWait;
    char vm_adv_aord;
    
    char cmd;
    char qhead;
    char qtail;
    char wdt_status;	// wdt_status()에 다른값이 들어 있다.
    char servo_status;	// H/W에서 읽은 신호선들의 값
 
    int loop_cnt;		// 2011.7.21(오) AMC 내부의 주요 루프의 동작 회수를 카운트하기 위한 용도?
    
    _DBG_MSG_BUFFER()
    {
        memset(this, 0, sizeof(_DBG_MSG_BUFFER));
    }
} DBG_MSG_BUFFER;

// 120803 2.9.7 syk 모션 관련 메시지(block box 기능)
typedef struct _DBG_EVENT_MSG_BUFFER 
{
    int jtpos;
    float mfGoal_pos;
	int profile_limit;
	int actvel_margin;
	float mdBasePos;
    float vm_WaitPos;
	int val;
    float velerr;
    float tqrcmd;
    float dtBox;
	int encoder_vel;
    short axis_source;
    short dac_code;
    char event_st;
    char vm_accflag;
    char vm_count;
    char vm_flag;
    char vm_bMoveDs;
    char vm_bPosWait;
    char vm_adv_aord;
    
	char limit_curve_chk_cnt;
    char qhead;
    char qtail;
    char wdt_status;	// wdt_status()에 다른값이 들어 있다.
    char servo_status;	// H/W에서 읽은 신호선들의 값
 
    int loop_cnt;		// 2011.7.21(오) AMC 내부의 주요 루프의 동작 회수를 카운트하기 위한 용도?
    
    _DBG_EVENT_MSG_BUFFER()
    {
        memset(this, 0, sizeof(_DBG_EVENT_MSG_BUFFER));
    }
} DBG_EVENT_MSG_BUFFER;

// 120803 2.9.7 syk 모션 생성관련 메시지
typedef struct _DBG_MOTIONMAKE_MSG_BUFFER 
{
    float acc;
    float dcc;
    float vel;
    float virtual_pos;    
    float dtBox;    
    int pos;
	int ds_profile_point;
    int vm_count;
  	int q_pos1;
  	int q_pos2;
  	int q_pos3;
  	int q_pos4;  
	int time; 
	  	   	
	char tmp1;  
	char tmp2; 
	char vm_accflag;	
	char vm_dccflag;
	char vm_adv_aord;	
    char qhead;
    char qtail;
  	char err;
  	char err_point; 
	char motion_sort;
	char ax;  
	char buffer_no;   		   
    
    _DBG_MOTIONMAKE_MSG_BUFFER()
    {
        memset(this, 0, sizeof(_DBG_MOTIONMAKE_MSG_BUFFER));
    }
} DBG_MOTIONMAKE_MSG_BUFFER;

// 120803 2.9.7 syk 모션 계산관련 메시지
typedef struct _DBG_MOTIONCAL_MSG_BUFFER 
{
    float dtbox;
    float fvel;
    float virtual_pos; 
    int time;   
    float vel;
    int pos;
    float vm_prev_vel;
    float vm_vel;
    int vm_count; 
    int vm_acc;
    int vm_dcc;
    int dac_bias;
    int tmp4; 
    int tmp5;           
    
    char vm_accflag;
    char vm_dccflag;
    char qhead;
    char qtail;   
    char ds_cal_point;
 	char cal_sort;
	char ax; 
	char buffer_no1; 	   
	
	
    _DBG_MOTIONCAL_MSG_BUFFER()
    {
        memset(this, 0, sizeof(_DBG_MOTIONCAL_MSG_BUFFER));
    }
} DBG_MOTIONCAL_MSG_BUFFER;

typedef struct _BG_STATE
{
    volatile int bOnTheFlyCmd;
    volatile int bSettingParam;
    volatile int bNewProfileReady;
    volatile int bStopUpdatingBgVar;
    volatile int bHaltTriggered;
    volatile float fDecelTime4Halt;          // sec
}BG_STATE;

typedef struct _ISR_STATE
{
    volatile int bHaltEvent;
    volatile int bHoldingVel;
    volatile int bMoving;
    volatile int bCheckInPos;
    volatile int bSyncEndPos;
    volatile int bPrcsingProf;
}ISR_STATE;

typedef struct _PHASE
{
    volatile int nAcc;
    volatile float fEndVel;
    volatile double dEndPos;
    volatile int bEndByPos;
}PHASE;

typedef struct _PROFILE
{
    volatile int nPhasesLeft;
    volatile int nCurPhase;
    volatile double dBasePos;
    volatile double dGoalPos;
    volatile float fBaseVel;
    PHASE Phase[NUM_OF_PHASES];
}PROFILE;

#endif

