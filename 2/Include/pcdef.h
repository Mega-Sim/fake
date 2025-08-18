/********************************************************************
*	FILE NAME  :  DEFINE.H
*********************************************************************/
/*#ifdef	_MSC_VER	*//* Microsoft Visual C++ 5.0. */

#ifndef		__PCDEF_H
#define		__PCDEF_H

#include	"6xsystem.h"

#define	CHAR		char
#define	INT			int
#define	SHORT		short
#define	LONG		int
#define	FLOAT		float
#define	VOID		void
#define	pDOUBLE		double *


#define	FBINWRITE	"wb"
#define	FBINREAD 	"rb"


#define	AXIS_COMM_WAIT		1000L


/*-----------------------------------------------------------
*	MMC Library Version
*---------------------------------------------------------*/
#define	MMC_SW_VER				3.0
/*-----------------------------------------------------------
*	MMC COMMAND DEFINES
*---------------------------------------------------------*/
#define	PTP_MOVE					1
#define	CP_MOVE						2
#define	GET_GAIN_VALUE 				3
#define	PUT_GAIN_VALUE				4
#define	GET_VGAIN_VALUE 			5
#define	PUT_VGAIN_VALUE				6
#define	PUT_POSITION				7
#define	GET_A_POSITION				8
#define	GET_C_POSITION				9
#define	GET_ERROR     				10
#define	PUT_IN_POSITION				11
#define	GET_IN_POSITION				12
#define	PUT_ERR_LIMIT 				13
#define	GET_ERR_LIMIT				14
#define	GET_EMPTY_FRAME				15
#define	SET_BOOT_FRAME				16
#define	VEL_MOVE					17
#define	WRITE_IO					18
#define	SET_IO_BIT					19
#define	RESET_IO_BIT				20
#define	MMC_DELAY					21
#define	GET_MOTOR_TYPE				22
#define	SET_MOTOR_TYPE				23
#define	MSERVO_ON					24
#define	MSERVO_OFF					25
#define	ALARM_RESET					26
#define	ALARM_SET					27
#define	PUT_CLEAR_STATUS			28
#define	PUT_STOP_EVENT				29
#define	PUT_STOP_RATE				30
#define	GET_STOP_RATE				31
#define	PUT_E_STOP_EVENT			32
#define	PUT_E_STOP_RATE				33
#define	GET_E_STOP_RATE				34
#define	PUT_POS_EVENT   			35
#define	GET_POS_EVENT   			36
#define	PUT_NEG_EVENT   			37
#define	GET_NEG_EVENT   			38
#define	PUT_POS_LEVEL   			39
#define	GET_POS_LEVEL   			40
#define	PUT_NEG_LEVEL   			41
#define	GET_NEG_LEVEL   			42
#define	PUT_HOME_EVENT				43
#define	GET_HOME_EVENT				44
#define	PUT_HOME_LEVEL				45
#define	GET_HOME_LEVEL				46
#define	PUT_INDEX_REQUIRED			47
#define	GET_INDEX_REQUIRED			48
#define	PUT_INT_STOP   				49
#define	PUT_INT_E_STOP     			50
#define	GET_AXIS_STAT   			51
#define	GET_AXIS_SOURCE				52
#define	PUT_AMP_FAULT  				53
#define	GET_AMP_FAULT  				54
#define	PUT_AMP_FAULT_LEVEL			55
#define	GET_AMP_FAULT_LEVEL			56
#define	PUT_AMP_RESET_LEVEL			57
#define	GET_AMP_RESET_LEVEL			58
#define	PUT_DAC_OUT         		59
#define	GET_DAC_OUT					60
#define	PUT_FEEDBACK_DEVICE			61
#define	GET_FEEDBACK_DEVICE			62
#define	PUT_VOLTAGE_DEVICE			63
#define	GET_VOLTAGE_DEVICE			64
#define	PUT_AXIS_BOOT_FRAME			65
#define	PUT_FRAMES_CLEAR   			66
#define	PUT_CLOSED_LOOP				67
#define	GET_CLOSED_LOOP				68
#define	PUT_SERVO_ON_LEVEL			69
#define	GET_SERVO_ON_LEVEL			70
#define	PUT_INT_ENABLE				71
#define	GET_BUF_POSITION			72
#define	PUT_CLEAR_STOP				73
#define	MAIN_DELAY					74
#define	PUT_IO_TRIGGER				75
#define	PUT_SW_UPPER_LIMIT			76
#define	GET_SW_UPPER_LIMIT			77
#define	PUT_SW_LOWER_LIMIT			78
#define	GET_SW_LOWER_LIMIT			79
#define	READ_OUT_IO   				80
#define	PUT_VT_CONTROL				81
#define	GET_VT_CONTROL				82
#define	PUT_POS_I_MODE				83
#define	GET_POS_I_MODE				84
#define	PUT_VEL_I_MODE				85
#define	GET_VEL_I_MODE				86
#define	PUT_GEAR_RATIO				87
#define	GET_GEAR_RATIO				88
#define	PUT_SYNC_MAP_AXES			89
#define	PUT_SYNC_CONTROL_ON			90
#define	PUT_SYNC_CONTROL_OFF		91
#define	PUT_SYNC_GAIN				92
#define	GET_SYNC_GAIN				93
#define	PUT_COMPENSATION_POS		94
#define	PUT_PULSE_MODE				95
#define	GET_PULSE_MODE				96
#define	GET_E_POSITION				97
#define	GET_ES_POSITION				98
#define	PUT_PULSE_RATIO				99
#define	GET_PULSE_RATIO				100
#define	PUT_INTERPOLATION			101
#define	GET_EMPTY_INTERPOLATION		102
#define	PUT_ABS_ENCODER_RESET		103
#define	PUT_C_POSITION				104
#define	SPLINE_LINE_MOVE			105
#define	SPLINE_ARC_MOVE				106
#define	CP_LINE_MOVE				107
#define	SPLINE_MOVE					108
#define	RECT_MOVE					109
#define	PUT_INT_PCINT				110
#define	PUT_EXIST_CHK				111

#define	IO_MODE_SET					112
#define	GET_GLB_IO_INT_ENABLE		113
#define	GET_USERIO_INT_EDGE			114
#define	IO_MODE_GET					115
#define PUT_USER_IO_INT_EDGE		116

#define	GET_AXIS_VAR				120		/* added by Yun 2001.01.04 */


/*-----------------------------------------------------------
*	Q_BUFFER COMMAND DEFINES
*---------------------------------------------------------*/
#define	AX_T_MOVE			1	/* Trapezoid motion single axis	*/
#define	AX_S_MOVE			2	/* S Curve motion single axis */
#define	AX_R_MOVE			3	/* Relative motion single axis */
#define	AX_RS_MOVE			4	/* Relative motion single axis */
#define	AX_P_MOVE			5	/* Parabolic motion single axis	*/
#define	AX_DELAY			6	/* Delay */
#define	AX_IO_TRIGGER		7	/* I/O Trigger Bit */

#define	CIRCLE_MOVE			11
#define	MAIN_INTERPOLATION	12

/*-----------------------------------------------------------
*	Limit Command Define
*---------------------------------------------------------*/
#define	PUT_P_SW_LIMIT		101
#define	PUT_N_SW_LIMIT		102
#define	GET_P_SW_LIMIT		103
#define	GET_N_SW_LIMIT		104

//(오) 2007.1.10 : 복원 
/*-----------------------------------------------------------
*	Trajectory Define
*---------------------------------------------------------*/
#define	TRAPEZOID		1
#define	S_CURVE			2
#define	RELATIVE		3
#define	S_RELATIVE		4
#define	PARABOLIC		5

#define	VELOCITY		20

/*-----------------------------------------------------------
*	Limit Sensor
*---------------------------------------------------------*/
#define	NEG_SWITCH		1
#define	HOME_SWITCH		2
#define	POS_SWITCH		3

#define	AMP_FAULT		1

/*-----------------------------------------------------------
*	Boot Frame Save or None
*---------------------------------------------------------*/
#define	BOOT_SAVE		1
#define	TEMPORARY		0

#include	"mmcdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*	PLIB.C		*/
INT		MMCMutexLock (void);
INT		MMCMutexUnlock (void);
INT		CommWrite(INT,INT);
INT		CDIWrite(INT, INT, INT);
INT		CDIRead(INT, pINT, INT);
INT		CDLWrite(INT ,LONG, INT);
INT		CDLRead(INT ,pLONG, INT);
INT		CDFWrite(INT, FLOAT, INT);
INT		CDFRead(INT, pFLOAT, INT);
INT		CDFDWrite(INT ax, float pos, INT comm);
INT		CDI3Write(INT, LONG, INT, INT);
INT		CDI3Read(INT, pLONG, pINT, INT);
INT		MMCCommCheck(INT, pINT, INT, INT);

/*	PLIB1.C		*/
INT		CDINoBootWrite(INT ,INT , INT);
INT		CDFNoBootWrite(INT ,FLOAT, INT);
INT		CDFDNoBootWrite(INT, float, INT);
INT		CDI3NoBootWrite(INT ,LONG, INT, INT);

#ifdef __cplusplus
}
#endif

typedef struct _CommDPRAMType 
{
		UINT2    	Command;      		/*	0	-	1	*/
		UINT2   	Len;               	/*	2	-	3	*/
		FLOAT   Vel[BD_AXIS_NUM]; 	/*	4	-	35	*/
		INT2    	Acc[BD_AXIS_NUM]; 	/*	36	-	51	*/
		INT2    	Axis[BD_AXIS_NUM]; 	/*	52	-	67	*/
		INT   	Pos[BD_AXIS_NUM];  	/*	68	-	99	*/
		FLOAT   Ox;          		/*	100	-	103	*/
		FLOAT   Oy;              	/*	104	-	107	*/
		FLOAT   Angle;             	/*	108	-	111	*/
		FLOAT   L_Vel;      		/*	112	-	115	*/
		INT2		L_Acc;   			/*	116	-	117	*/
		INT2		Cir_Dir;    		/*	118	-	119	*/
		FLOAT   L_Deg;            	/*	120	-	123	*/

		INT	Io_outValue;    	/*	124	-	127	*/
		INT	Io_inValue;       	/*	128	-	131	*/
		INT2		AnalogChannel[4];  	/*	132	-	139	*/
		INT	Delay;				/*	140	-	143	*/
		INT2		Axis_Num;      		/*	144	-	145	*/
		INT2		Ver;				/*	146	-	147	*/
		INT2		Dcc[BD_AXIS_NUM]; 	/*	148	-	163	*/
		INT2		Temp_Acc;         	/*	164	-	165	*/
		INT2		Temp_Dcc;           /*	166	-	167	*/
		FLOAT	Temp_Vel;         	/*	168	-	171	*/
#if defined(__AMC_29x)		
		FLOAT	Spl_Pos[30][3];		/*	172	-  531	*/
#endif
#if (defined(__AMC_SMD) || defined(__AMC_V70))
		FLOAT	Spl_Pos[25][3];		/*	172	-  471	*/  
		
		INT		IoinValueR;			/* 472 ~ 475 */			// //120831 2.9.8 SYK io추가를 위해 분할.
		INT		IoinValue1_1;		/* 476 ~ 479 */
		INT		IoinValue2_1;		/* 480 ~ 483 */
				
		INT		Io_outValue3;		/* 484 ~ 487 */			// //120831 2.9.8 SYK io추가를 위해 분할.
		INT		IoinValue3;			/* 488 ~ 491 */
		INT		Io_outValue4;		/* 492 ~ 495 */
		INT		IoinValue4;			/* 496 ~ 499 */
		INT		Io_outValue5;		/* 500 ~ 503 */
		INT		IoinValue5;			/* 504 ~ 507 */								
		INT		Io_outValue6;		/* 508 ~ 511 */
		INT		IoinValue6;			/* 512 ~ 515 */
		INT		Io_outValue7;		/* 516 ~ 519 */
		INT		IoinValue7;			/* 520 ~ 523 */
		INT		Io_outValue8;		/* 524 ~ 527 */
		INT		IoinValue8;			/* 528 ~ 531 */
#endif		
		INT		Io_outValue2;		/* 532 ~ 535 */
		INT		IoinValue2;			/* 536 ~ 539 */
} COMM_DPRAM_TYPE;					/*	0x300 -  space	*/

typedef struct _AxisDpramType 
{
	INT2    	Board_Exist;    		/*	0 	-	1 	*/
	CHAR	In_Pos_Flag; 			/*	2 	-	 	*/
	CHAR	In_Sequence;			/*	3  - 	  	*/
	INT2		AxisSource;				/*	4	-	5	*/
	INT2		AxisState;				/*	6 	-	7 	*/
	INT2 	Vel;           			/*	8 	-	9 	*/
	INT2		Actual_Vel;				/*	10	-	11	*/
	CHAR	Frames_Q;				/*	12	-	  	*/
	CHAR	Frames_Interpolation;	/*	13	-	  	*/
	CHAR	Char_Type[2];			/*	14	-	15	*/
	INT2		Int_Type[5];			/*	16	-	25	*/
	INT		Long_Type;  			/*	26	-	29	*/
	FLOAT	Float_Type;				/*	30	-	33	*/
	INT		Long_Type1;  			/*	34	-	37	*/
	FLOAT	Float_Type1;			/*	38	-	41	*/
} AXIS_DPRAM_TYPE;  				/*	42	-	48	*/






typedef struct _MoveFrameType {
	float	L_Vel;   						/* CP Motion Velocity 	*/
	INT   	L_Acc;    	               /* CP Motion Accel    	*/
	float	L_Deg; 	                  /* ARC Motion Degrees  	*/
	float  	Ox;
	float  	Oy;
	float  	Angle;
	FLOAT		Deg_Div;
	float 	Vel[TOTAL_AXIS_NUM];			/* joint speed data		*/
	INT		Len;
	INT    	Acc[TOTAL_AXIS_NUM];       /* acceleration data 	*/
	INT    	Dcc[TOTAL_AXIS_NUM];       /* acceleration data 	*/
	INT		Axis[TOTAL_AXIS_NUM];
	float  	Pos[TOTAL_AXIS_NUM];
	INT		MapFunc;
	INT		Cir_Dir;
}MOVE_FRAME_TYPE;

typedef struct _MotionDisp {
	LONG		delay;
	float	pos[2];
	float	vel;
	INT		acc;
	INT		dcc;
	INT		traj;
	INT  		flag;
	INT  		axis;
	INT		cnt;
	INT		cordx[4];
	INT		cordy[4];
}MOTION_DISP;

typedef struct _SyncMotion {
	INT		Master;
	INT		Slave;
	INT  		Flag;
	FLOAT  		Gain;
}SYNC_MOTION;

extern	CHAR				*Ack2Dsp[];
extern	CHAR				*Int2Dsp[];
extern	CHAR				*DpramExistChk[MMC_BOARD_NUM][2];
extern	unsigned LONG		Dpram_Addr[MMC_BOARD_NUM][BD_AXIS_NUM];

extern	INT					mmc_error;
extern	INT					MMC_Bd_Num;
extern	INT					Int2Dsp_Flag[];
extern	INT					Power_Flag[];
extern	unsigned CHAR		ChkSum_Parity[];
extern	LONG				Virtual_Pos[];
extern	CHAR				*Error_Msg[];
extern	INT					Version_Info[];





#endif
