#ifndef __AXISDEF_OLD_H
#define __AXISDEF_OLD_H

#include	<math.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<float.h>

#include 	"amc.h"
#include <string.h>


#define		EEPROM_BOOTPARAM_MAGIC_NO		0x9abcdef0
#define		EEPROM_BOOTPARAM_START_OFS		0X0000
#define		EEPROM_MAP_START_OFS			0X2000


#define		ROM_VERSION		20942      	// ver:ax#

#define 	USERIN_SIZE	8
#define		USERIN_MASK 0xFF
#define		USEROUT_SIZE   8
#define		USEROUT_MASK 0xFF


#define		MAX_COMM_NUM	159

#define 	BOOT_SAVE_WAIT	10000000
#define		BOOT_FRAME_SIZE	196
//////////////////////////////////////////////////////////////////////////////



#define  	MMC_AXIS 		4        	// reserved 
#define  	JPC_AXIS 		MMC_AXIS	     	// number of actual axis



#define		COUNT_MAX		5 		// number of queue 
#define     COUNT_INT_MAX	20		// number of interpolation queue 
#define		MAX_Q_NUM       	50		// number of motion queue

#define		MAX_ACCEL		200

#define 	OK  			1
#define   	ERROR			0

#define 	NO				ERROR
#define 	YES				OK

#define 	ITP_MODE_STOP	0
#define 	ITP_MODE_MOVE	1

#define 	TRAPEZOID       1
#define 	SCURVE          2
#define		RELATIVE		3
#define		S_RELATIVE		4
#define 	PARABOLIC       5
#define		AX_DELAY		6
#define		AX_IO_TRIGGER	7

#define 	SAMPLING_TIME   200.0   // 5ms SAMPLING TIME
#define		MAX_SMPCNT      5		// 10 MAX SMPLING COUNT VALUE

//#define	PI	3.141592654
#define	PI	(atan2(1.0,1.0)*4.0)

// Default PID Gain
#define		P_GAIN			150		// initial P_gain  define
#define		I_GAIN			1   	     	// initial I_gain  define
#define		D_GAIN			0      	  	// initial D_gain  define
#define		F_GAIN			300    		// initial F_gain  define
#define		J_GAIN			00     	  	// initial J_gain  define
#define		I_LIMIT			1000   		// initial I_limit define

#define     AXIS_START      	    	1		// Command start     
#define     AXIS_MOVING         	2        	// jpc에서 동작중    
#define     AXIS_CHANGE         	3        	// jpc에서 동작중    
#define     SEMI_END            	4         	// command end       
#define     AXIS_END            	5       	// command end       
#define     GOAL_MOV_ERR        	6    	     	// Goal Position move error



#define		NO_EVENT		0
#define		STOP_EVENT		1
#define		E_STOP_EVENT		2
#define		ABORT_EVENT		3

#define		ST_NONE				0x0000
#define		ST_HOME_SWITCH		0x0001
#define		ST_POS_LIMIT			0x0002
#define		ST_NEG_LIMIT   			0x0004
#define		ST_AMP_FAULT     		0x0008
#define		ST_SYSTEM_INSEC    		0x0010
#define		ST_V_LIMIT  				0x0020
#define		ST_X_NEG_LIMIT 		0x0040
#define		ST_X_POS_LIMIT 			0x0080
#define		ST_ERROR_LIMIT   		0x0100
#define		ST_PC_COMMAND  		0x0200
#define		ST_OUT_OF_FRAMES  	0x0400
#define		ST_AMP_POWER_ONOFF	0x0800
#define 		ST_HWINPOSITION		0x2000	//inserted in 2000/02/22
#define		ST_SWINPOSITION		0x4000




typedef struct{
		int     	Moving_Flag;
		int     	Moving_Axis[JPC_AXIS];
		int    	Int_Moving_Timer[JPC_AXIS];
		int     	Gp_Chk_Cnt[JPC_AXIS];
		int    	Moving_Timer[JPC_AXIS];
		int    	JntCurrPos[JPC_AXIS];
		int     	Offset_Axis;
}JPCMOVSTATUS;

typedef struct{
		int     	Flag[JPC_AXIS];
		int     	Acc[JPC_AXIS];
		int     	Timer[JPC_AXIS];
}JPCPAUSE;	

typedef struct
{
		int  	Pos[JPC_AXIS];
		int    	NewPos[JPC_AXIS];
		int    	CurPos[JPC_AXIS];
		float   	Vel[JPC_AXIS];
		int	 	Acc[JPC_AXIS];
		int	 	Dcc[JPC_AXIS];
		int     	Flag[JPC_AXIS];
		int	 	Profile[JPC_AXIS];
		int     	Cmd;
} AXIS_STATUS;

typedef struct  {
		int		N1;
		int    	N2;
		int    	N3;
		int    	N4;
		int    	Ds;
		float   	Acc_Div;
		float   	Dcc_Div;
		float   	Acc_Rate;
		float   	Dcc_Rate;
		float   	V;
}TRAJ_PARA;


typedef struct  {
		int     	Command;
		int     	Len;
		int 	 	Profile;
		float   	Vel;
		int     	Acc;
		int     	Dcc;
		int     	Axis;
		int    	Pos;
		int	 	Delay;
		int		IO_Trigger_Bit;
		int		IO_Trigger_St;
		int		Cir_Direction;
		float   	Ox;
		float   	Oy;
		float   	Angle;
		float   	L_Vel;
		int 	 	L_Acc;
		float   	L_Deg;
		float		Spl_Pos;
      int	Cir_Dir;
}AQ_BUF;

typedef	void	 (*FUNCPOINT)(void);


// 축의 상태를 저장하기위한 구조체
// AMC, 2006.11
typedef	struct {
// status variables
 	 int		jtpos;		//	00 ; current actual position
	 int		jtpos_b;		//	01 ; previous actual position

	 double	mdGoal_pos;		//	02 ; goal position 
	 double	mdRefpos;		//	04 ; interpolation position
	 double	mdRefpos_b;		//	06 ; previous interpolation position
	 double	mdBasepos;		//	08 ; base position
	 double	mdMaspos;		//	10 ; 


	 float	cal_velo;		//	12 ; calculated velocity
	 float	velo_z0;		//	13 ; current velocity
	 float	velo_z1;		//	14 ; previous velocity
	 float	error_z0;		//	15 ; current position error
	 float	error_z1;		//	16 ; previous position error
	 float  	error_z2;		//	17  
	 float  	maindt_p;		//	18 ; 10msec segment data
	 float  	maindt_b;		//	19 ; previous 10msec segment data
	 float  	maindt_bb;		//	20   

	 float	isum;			//	21 ; integrated error
	 float	j_z0;			//	22 ; current  acceleration
	 float	j_z1;			//	23 

	 float	velfbz0;		//	24 ; velocity feedback buffer
	 float	velfbz1;		//	25 ; velocity feedback buffer
	 float	velcmd;		//	26 ; velocity command 
 	 float 	velcmd_b;		//	27 ; previous velocity command 
	 float	velerrz1;		//	28 ; absolute velocity 
	 float	velerr;		//	29 ; velocity error   
	 float  	velisum;		//	30 ; velocity SUM	    
	 float	tqrcmdf;		//	31 ; torque float input
	 float	tqrcmd;		//	32 ; torque int input
	 float 	pos_ierr;		//	33 ; 
	 float 	vel_ierr;		//	34 ;
	 
	 float	dtBox;		//	35 :	distance channel
	 float	bsBox;		//	36 :	base position channel

// moniter
	 int		counter;		//	37 : inner counter value
	 int		encoder;		//	38 : encoder counter value
	 float	counter_err;	//	39 : goal_pos - inner_counter
	 float	encoder_err;	//	40 : goal_pos - encoder_counter/4
	 int		counter_vel;	//	41 : inner counter velocity
	 int		encoder_vel;	//	42 : encoder counter velocity
	 
	 
// timer or flags
	 int		timer;		//	43 ; 1 msec counter
	 int  	run_timer;		//	44                 
 	 int	  	dwell;		//	45 ; Axis Delay	
	 int   	gukan;		//	47 ; interpolation period information
	 float  	smpcnt;		//	48 ; sampling counter    
	 float	smpcnt_b;		//	49 ; previous sampling counter
	 int		g_on;			//	50 ; pwm on/off flag
	 int		offset_on;		//	51                  
 	 int	  	encoder_type;	//	52 ; Encoder or Bipolar)Analog device
	 int	  	loop_cfg;		//	53 ; Encoder Index Pulse Use/Unuse
	 int	  	voltage_type;	//	54 ; Unipolar Device or Bipolar Device
	 int	  	in_pos_flag;	//	55 ; In_Position Flag
	 int	 	in_sequence;	//	56 ; On Motioning
	 int		run_flag;		//	57 ; Run_Mode(1), Stop_Mode(0)
	 int		unianalog_sum;	//	58 ; Unipolar Analog Sum
	 int		inPosIndex;		//	59 ; flag for checking in-position 
// parameters
	 int		pgain;		//	60 ; P-gain
	 int		igain;		//	61 ; I-gain
	 int		dgain;		//	62 ; D-gain
	 int		fgain;		//	63 ; F-gain
	 int		jgain;		//	64 ; J-gain
	 int		ilimit;		//	65 ; error sum value limit
	 int		IO_Trigger;		//	66 
	 int		amp_on_level;	//	67 
	 int		velpgain;		//	68 ; velocity Pgain
	 int	  	veligain;		//	69 ; velocity Igain
	 int	  	veldgain;		//	70 ; velocity Dgain
	 int	  	velfgain;		//	71 ; velocity FFgain
	 int	  	veljgain;		//	72 ; velocity ACCgain
	 int		velilimit;		//	73 ; velocity ilimit
 	 int		motortype;		//	74 ; Servo=0 , Stepper=1
	 float  	in_position;	//	75 ; In_Position Range
	 int		control_cfg;	//	76 ; Velocity_Cont(0), Torque_Cont(1)
	 int		posimode;		//	77 ; In_Standing(0), In_Always(1)
	 int		velimode;		//	78 ; In_Standing(0), In_Always(1)
	 float		gearratio;		//	79 ; Electric Gear Ratio
	 int 		pulsemode;		//	80 ; Step Pulse Mode
	 int		micro_stepper;	//	81 ;		
	 int		pulseratio;		//	82 ;		
	 int		dac_bias;		//	83 ; Velocity Voltage Out Data
	 int		swupper_limit;	//	84 ; software upper travel limit
	 int		swlower_limit;	//	85 ; software lower travel limit	
	 int		stop_rate;		//	86 ; STOP EVENT deceleration rate	
	 int		e_stop_rate;	//	87 ; E_STOP EVENT deceleration rate	
	 int		encDir;		//	88 ; Encoder Direction
	 int		encRes;		//	89 ; Encoder Resolution
	 int		comRes;		//	90 ; Command Resolution
	 float		ratio;		//	91 ; 
	 
// servo or sensor
	 int	  	home_level;		//	92 ; Home Sensor level define
 	 int	  	fault_level;	//	93 ; Servo Driver Error level define
	 int	  	reset_level;	//	94 ; Servo Driver Reset level define
	 int	  	pos_level;		//	95 ; Positive Sensor level define
	 int	  	neg_level;		//	96 ; Negative Sensor level define
	 int		inpos_level;	//	97 ; In Position level define
	 
	 int	  	event_st;		//	98 ; Define Event
	 int	  	axis_source;	//	99 ; Axis event define
	 int	  	pos_sn_st;		//	100; Positive Sensor Event
	 int	  	neg_sn_st;		//	101; Negative Sensor Event
	 int	  	home_sn_st;		//	102; Home     Sensor Event
	 int	  	limit_err_st;	//	103; Limit Error Event	
	 int	  	int_event;		//	104; Interrupt Event define
	 int	  	amp_event;		//	105; Amp Fault Event Define
	 int		swupper_limitst;	//	106; software upper travel limit event
	 int		swlower_limitst;	//	107; software lower travel limit event
	 
	 int		delay_c;		//	108; servo check delay reduce noise
	 int		in_vcu;		//	109; servo status
	 int		in_vcu1;		//	110; servo states value
	 int		IO_Trigger_Bit;	//	111; servo states value
	 int		init_counter;	//	112;    
	 int		set_counter;	//	113; counter value set
	 int		limit_err_chk;	//	114; Envelope Error Value

	 int		sn_st;		//	115; Current Sensor Status
	 int		sn_st1;		//	116; 1 step delay Sensor Status
	 int		IO_Trigger_St;	//	117; 1 step delay Sensor Status
	 int		sn_c;			//	118; Sensor Noise Counter

	 int	  	index_req;		//	119; Encoder Index Pulse En/Dis
	 int		bianalog_sum;	//	120; Bipolar Analog Sum	
	 int		clkdac_bias;	//	121; Velocity Voltage Out Data

	 // 10sec routine variables
	 float		vm_vel;		//	122; Velocity Mode Set Velocity
  	 float		vm_prev_vel;	//	123; v_move previous slew velocity	
	 int		vm_acc;		//	124; Velocity Mode acceleration count

	 int		vm_dcc;		//	124-1; Velocity Mode dcc count, 2006.11.18(Oh)
	 
	 float		vm_delt_s;		//	125; Velocity Mode acceleration flag
	 int		vm_accflag;		//	126; Velocity Mode acceleration flag
	 int		vm_dccflag;		//	127; Velocity Mode deceleration flag
	 int		vm_count;		//	128; Velocity Mode acceleration count
	 int		vm_axis;		//	130; Velocity Mode Axis
	 int		temp;			//	131; for debug
	 int		temp1;
	 int		temp2;
	 int		temp3;
	 
	 //-- Added by AMC, 2006.11.17
	 float		V_TrackingFactor;	// default = 1.0, 속도 트랙킹을 위한 비율.
	 //-- Added by AMC, 2006.12.07(오)
	 int		dac_code;		//현재 출력되고 있는 DAC 값(Offset 및 Tracking Factor는 고려하지 않음.)


	// Bootpara에 있는 Accel_Limit를 추가함.
	// 2007.1.23, ckyu
	int			Accel_Limit;
	
	// 2007.4.13, ckyu
	int			EncoderOffset;
	
}	CONTROLBUF;

typedef	struct {
	int	  	command;		//	0 	; main command	
	int		exram_cnt;		//  	1	; user ram address count
	int		z_index_ax;		//  	2	; LED DATA FLAG	
	int		z_index_flag;	//  	3 	; run margin timer
	int		sys_io_out;		//	4	; pwm on/off	
	int		user_io_out;	//	5-1	; 				
	int		user_io_out2;	//	5-2	; 				
	int		zr_enable;		//  	6	;			
	int		delay_time;		//	7	; For mmcDelay Function
	int		io_int_ax;		//	8	; I/O Interrupt Axis	
	int		sync_flag;		//	9	; Synchronous Motion Flag
	int		sync_m_axis;	//	10	; Master Axis For Synchronous Motion
	int		sync_s_axis;	//	11	; Slave  Axis For Synchronous Motion
	int		sync_gain;		//	12	; Synchronous Motion Gain
}    GLOBALBUF;



//
// 이 구조체는 PC측의 BOOT_FRAME_TYPE과 동일한 순서와 크기로 맞추어져 있음.
// 2007.3.13, ckyu
//
typedef struct _ST_BOOTPARA
{
	int 	Vel_Limit[JPC_AXIS]; 		//
	int   	Accel_Limit[JPC_AXIS];

	int	  	PulseRatio[JPC_AXIS];    	// system gear ratio    
	float  	GearRatio[JPC_AXIS];    	//system gear ratio    
	
	float  	HwLower_Limit[JPC_AXIS]; 	// system lower limit   
	float  	HwUpper_Limit[JPC_AXIS]; 	// system upper limit   
	int   	SwLower_Limit[JPC_AXIS]; 	// system lower limit   
	int   	SwUpper_Limit[JPC_AXIS]; 	// system upper limit   

	int   	PGain[JPC_AXIS];       		// P gain    value      
	int   	IGain[JPC_AXIS];        	// I gain    value      
	int   	DGain[JPC_AXIS];        	// D gain    value      
	int   	FGain[JPC_AXIS];        	// F gain    value      
	int		ILimit[JPC_AXIS];       	// I_LIMIT   value      

	int 		VPgain[JPC_AXIS];       	// VELP gain    value   
	int  		VIgain[JPC_AXIS];        	// VELI gain    value   
	int  		VDgain[JPC_AXIS];        	// VELD gain    value   
	int  		VFgain[JPC_AXIS];        	// VELF gain    value   
	int  		VIlimit[JPC_AXIS];     		// VELI_LIMIT   value   

	float		In_Position[JPC_AXIS];
	int 		Error_Limit[JPC_AXIS];
	char 		Motor_Type[JPC_AXIS];

	char		SwUpper_LimitSt[JPC_AXIS];
	char		SwLower_LimitSt[JPC_AXIS];

	char		Pos_Level[JPC_AXIS];
	char		Neg_Level[JPC_AXIS];
	char		Home_Level[JPC_AXIS];
	char		Amp_Level[JPC_AXIS];
	char		Amp_Reset_Level[JPC_AXIS];

	char		Pos_Limit_St[JPC_AXIS];
	char		Neg_Limit_St[JPC_AXIS];
	char		Home_Limit_St[JPC_AXIS];
	char		Error_Limit_St[JPC_AXIS];

	char		Encoder_Cfg[JPC_AXIS];
	char		Voltage_Cfg[JPC_AXIS];
	char		Home_Index[JPC_AXIS];

	char 		Stop_Rate[JPC_AXIS];
	char     E_Stop_Rate[JPC_AXIS];

	char		Control_Cfg[JPC_AXIS];
	char		Loop_Cfg[JPC_AXIS];
	char		Amp_OnLevel[JPC_AXIS];

	int 		Io_Int_Enable[JPC_AXIS];
	char		Int_Event_St[JPC_AXIS];
	char		Amp_Fault_Event[JPC_AXIS];

	char		PosImode[JPC_AXIS];
	char		VelImode[JPC_AXIS];
	char		PulseMode[JPC_AXIS];

	int 		Inpos_Level[JPC_AXIS];

	int		Dpram_Addr[JPC_AXIS];
	int  		Axis_Num;
	int  		Action_Axis_Num;

	int 		UserIO_BootMode;
	int		UserIO_BootValue;

	
	// 2006.11.16, AMC
	// offset variable added
	int		dac_bias[JPC_AXIS];
	float	V_TrackingFactor[JPC_AXIS];
/*	User I/O mode, data is needed...*/

	// 2007.4.13, ckyu
	// 인코더의 0 위치를 설정하기 위한 용도.
	int		Encoder_Offset[JPC_AXIS];

	_ST_BOOTPARA()
	{
		memset(this, 0, sizeof(_ST_BOOTPARA));
		for (int i = 0; i < JPC_AXIS; i ++)
		{
			dac_bias[i] = 0;
			V_TrackingFactor[i] = 1.0;
		}
	}

} ST_BOOTPARA;


typedef struct _UBOOTPARA
{
	int		nMagic;
	int		nBlockSize;
	char	nBoardNo;
	char	nAxisNo;
	char	nVerMaj;
	char	nVerMin;
	ST_BOOTPARA st_boot;
	
	int		nCRC32;
	
	// Added by AMC
	// 2006.11.17
	int GetBodySize() { return sizeof(_UBOOTPARA) - sizeof(int); }
	void * GetBodyTopPtr() { return (void *) &nMagic; }
	void SetCRC32(int ncrc) { nCRC32 = ncrc; }
	int  GetCRC32() { return nCRC32; }
	int	 IsValidParam(int ncrc)
	{
		if (nMagic != EEPROM_BOOTPARAM_MAGIC_NO) return -1;
		if (ncrc != nCRC32) return -1;
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




extern	UBOOTPARA BootPara;


extern	int		IEEE_conv[9];
extern	int		*ptr_IEEE;

extern	JPCPAUSE       Pause;
extern	AQ_BUF			Q_Buf[JPC_AXIS][MAX_Q_NUM];
extern	TRAJ_PARA      Traj[JPC_AXIS];
extern	int 		Last_Pos[JPC_AXIS];

extern	int     virtual_pos[JPC_AXIS];

extern	int      acc_traj[JPC_AXIS];
extern	int      dcc_traj[JPC_AXIS];

extern 	int     acc_time[JPC_AXIS];
extern	int     dcc_time[JPC_AXIS];

extern	int     f_dest_pls[JPC_AXIS];
extern	int     f_cur_pls[JPC_AXIS];
extern	float    f_cur_pls_b[JPC_AXIS];
extern	float    f_delt_s[JPC_AXIS];

extern	float		vm_cur_pls[JPC_AXIS];
extern	float		vm_cur_pls_b[JPC_AXIS];

extern	float 	p_offs[MAX_ACCEL+1][MAX_ACCEL+1];

extern	int		Q_out_num[JPC_AXIS];
extern	int		Q_in_num[JPC_AXIS]; 

extern	int		Interpolation_Flag[JPC_AXIS];
extern	int		Dwell_Flag[JPC_AXIS];
extern	int		Dwell_Value[JPC_AXIS];



extern GLOBALBUF 	*pGBuf;
extern CONTROLBUF *pCBuf[MMC_AXIS];


// Program Dependent Data       (Variables num =  6 EA)
extern	int		mask;



extern int iomask;
extern int userio_mode_select;




extern	int Boot_Checksum_Flag;
extern	int	DISABLE_DPINT;
extern	int	FOR_CTR_INT;
extern  int g_i;
extern int g_ii;
extern int g_iii;
extern	int CLEAR_IF;



extern int ITTP;
extern int lITPCmd[JPC_AXIS];
extern int lITPSt[JPC_AXIS][2];
extern int nITPMode[JPC_AXIS];

extern int ABS_command;

// Hardware /////////////////////////////////////
/*--
extern volatile char	*CommDPRAM;
extern volatile char	*AxisDPRAM[MMC_AXIS];
extern volatile char * pBootPara;
extern int int_change;
--*/

//--
// 플랫폼에서 필요한 인터럽트를 활성화 하므로 생략한다.
// 2006.11.2, AMC
#define	AllIntEnable()	
/////////////////////////////////////////////////


#endif
