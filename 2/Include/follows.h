#ifndef	__FOLLOWS_H
#define	__FOLLOWS_H

#ifdef FOLLOWS_GLOBALS
#define FOLLOWS_EXT
#else
#define FOLLOWS_EXT extern
#endif

const unsigned short crc16tab[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};


////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////v
#define HOKUYO							0
#define SICK							1

#define MONITORING_SENSOR_NUM           4
   
#define AMC_AXIS				        4

#define SECTOMSEC   			        1000.0	

#define SOFTMOTION_TIME					1000

// CMD_MOVE의 CmdType define
#define	AMC_CMD_AXIS_NONE			    0	// 명령 없음
#define	AMC_CMD_AXIS_RESET			    1	// 축 초기화
#define	AMC_CMD_AXIS_AMP_ENABLE		    2	// Servo On
#define	AMC_CMD_AXIS_AMP_DISABLE	    3	// Servo Off
#define	AMC_CMD_AXIS_MOVE_P			    4
#define	AMC_CMD_AXIS_MOVE_N			    5
#define	AMC_CMD_AXIS_MOVE_S			    6
#define	AMC_CMD_AXIS_MOVE_DS		    7
#define	AMC_CMD_AXIS_MOVE_TIME_P	    8
#define	AMC_CMD_AXIS_MOVE_TIME_N	    9
#define	AMC_CMD_AXIS_MOVE_TIME_S	    10
#define	AMC_CMD_AXIS_MOVE_TIME_DS	    11
#define	AMC_CMD_AXIS_SET_HOME		    12  // 현재 위치를 0으로 만듬
#define	AMC_CMD_AXIS_SET_OFFSET		    13	// Offset 값 적용 (절대치 엔코더: 입력되는 위치를 0으로 만듬) --> Distance 에 값 입력
#define	AMC_CMD_AXIS_SET_POS_LIMIT		14	// Positive SW Limit --> Distance 에 값 입력
#define	AMC_CMD_AXIS_SET_NEG_LIMIT		15	// Negative SW Limit --> Distance 에 값 입력
#define	AMC_CMD_AXIS_RESET_LIMIT		16	// Reset Limit --> Limit 사용하지 않음(또는 제일 큰 값으로 설정) --> 입력 인자 없음

// CMD_ADDITIONAL_INFO의 CmdType define
#define	OHT_DETECT_CMD_PATH_DIRECTION_NO_USE 		0	// 미사용: 예전 없음
#define	OHT_DETECT_CMD_PATH_DIRECTION_NORMAL		1	// 직진 구간: 예전 N
#define	OHT_DETECT_CMD_PATH_DIRECTION_LEFT			2	// 왼쪽: 예전 L
#define	OHT_DETECT_CMD_PATH_DIRECTION_RIGHT 		3	// 오른쪽: 예전 R
#define	OHT_DETECT_CMD_PATH_DIRECTION_SHORT	    	4	// N분기 사용: 예전 S
#define	OHT_DETECT_CMD_PATH_DIRECTION_BOTH			5	// 짧은 S자형 곡선: 예전 B
#define	OHT_DETECT_CMD_PATH_DIRECTION_NARROW		6	// 레일 간격이 900mm 이하일때의 짧은 S자형 곡선: 예전 V
#define	OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL	7		
#define	OHT_DETECT_CMD_PATH_DIRECTION_RESERVED2		8
#define	OHT_DETECT_CMD_PATH_DIRECTION_RESERVED3		9
#define	OHT_DETECT_CMD_PATH_DIRECTION_RESERVED4		10
#define OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT    	11
#define OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT   	12
#define OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT   13
#define OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT  14
#define OHT_DETECT_CMD_PATH_DIRECTION_RESERVED5		15

#define	OHT_DETECT_NO_USE 	            0xF 
#define	OHT_DETECT_NORMAL_VALUE	        0xE 
#define	OHT_DETECT_LEFT_VALUE	        0xD 
#define	OHT_DETECT_RIGHT_VALUE	        0xC 
#define	OHT_DETECT_SHORT_VALUE	        0xB 
#define	OHT_DETECT_BOTH_VALUE	        0xA 
#define	OHT_DETECT_NARROW_VALUE         0x9 
#define	OHT_DETECT_HILL_VALUE			0x8
#define	OHT_DETECT_RESERVED2			0x7
#define	OHT_DETECT_RESERVED3			0x6
#define	OHT_DETECT_RESERVED4			0x5
#define	OHT_DETECT_NL_VALUE				0x4 
#define	OHT_DETECT_NR_VALUE         	0x3
#define OHT_DETECT_DOUBLE_LEFT   		0x2
#define OHT_DETECT_DOUBLE_RIGHT  		0x1
#define OHT_DETECT_RESERVED5			0

#define	OBS_DETECT_CMD_PATH_DIRECTION_NO_USE 		0	
#define	OBS_DETECT_CMD_PATH_DIRECTION_NORMAL		1	
#define	OBS_DETECT_CMD_PATH_DIRECTION_LEFT			2	
#define	OBS_DETECT_CMD_PATH_DIRECTION_RIGHT 		3	
#define	OBS_DETECT_CMD_PATH_DIRECTION_SHORT	    	4	
#define	OBS_DETECT_CMD_PATH_DIRECTION_BOTH			5	
#define	OBS_DETECT_CMD_PATH_DIRECTION_NARROW		6	
#define	OBS_DETECT_CMD_PATH_DIRECTION_RESERVED1		7		
#define	OBS_DETECT_CMD_PATH_DIRECTION_RESERVED2		8
#define	OBS_DETECT_CMD_PATH_DIRECTION_RESERVED3		9
#define	OBS_DETECT_CMD_PATH_DIRECTION_RESERVED4		10
#define OBS_DETECT_CMD_PATH_DIRECTION_N_LEFT    	11
#define OBS_DETECT_CMD_PATH_DIRECTION_N_RIGHT   	12
#define OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT   13
#define OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT  14
#define OBS_DETECT_CMD_PATH_DIRECTION_RESERVED5		15

// Sick
#define	SICK_OBS_DETECT_NORMAL_VALUE        	0x0 
#define	SICK_OBS_DETECT_BOTH_VALUE				0x1 
#define	SICK_OBS_DETECT_LEFT_VALUE				0x2 
#define	SICK_OBS_DETECT_RIGHT_VALUE				0x3 
#define	SICK_OBS_DETECT_RESERVED1				0x4 
#define	SICK_OBS_DETECT_RESERVED2				0x5 
#define	SICK_OBS_DETECT_RESERVED3				0x6 
#define	SICK_OBS_DETECT_RESERVED4				0x7 
#define	SICK_OBS_DETECT_RESERVED5				0x8 
#define	SICK_OBS_DETECT_RESERVED6				0x9
#define	SICK_OBS_DETECT_RESERVED7				0xA 
#define	SICK_OBS_DETECT_RESERVED8				0xB 
#define	SICK_OBS_DETECT_RESERVED9				0xC 
#define	SICK_OBS_DETECT_RESERVED10				0xD 
#define	SICK_OBS_DETECT_RESERVED11				0xE 
#define	SICK_OBS_DETECT_RESERVED12				0xF 

// HYOKUYO
#define	HOKUYO_OBS_DETECT_NO_USE 				0xF 
#define	HOKUYO_OBS_DETECT_NORMAL_VALUE	        0xE 
#define	HOKUYO_OBS_DETECT_BOTH_VALUE			0xD 
#define	HOKUYO_OBS_DETECT_LEFT_VALUE			0xC 
#define	HOKUYO_OBS_DETECT_RIGHT_VALUE			0xB 
#define	HOKUYO_OBS_DETECT_RESERVED1		        0xA 
#define	HOKUYO_OBS_DETECT_RESERVED2				0x9 
#define	HOKUYO_OBS_DETECT_RESERVED3				0x8 
#define	HOKUYO_OBS_DETECT_RESERVED4				0x7 
#define	HOKUYO_OBS_DETECT_RESERVED5	       	 	0x6 
#define	HOKUYO_OBS_DETECT_RESERVED6				0x5 
#define	HOKUYO_OBS_DETECT_RESERVED7				0x4 
#define	HOKUYO_OBS_DETECT_RESERVED8				0x3 
#define	HOKUYO_OBS_DETECT_RESERVED9	        	0x2 
#define	HOKUYO_OBS_DETECT_RESERVED10			0x1 
#define	HOKUYO_OBS_DETECT_RESERVED11			0 

#define WATCHDOG_DETECT_LEVEL			1000		// 1 sec

#define VELOCITY_0_0_MPS	0 
#define VELOCITY_0_1_MPS	11343 
#define VELOCITY_0_15_MPS	17014
#define VELOCITY_0_2_MPS	22686 
#define VELOCITY_0_3_MPS	34029 
#define VELOCITY_0_4_MPS	45372 
#define VELOCITY_0_5_MPS	56716 
#define VELOCITY_0_6_MPS	68059 
#define VELOCITY_0_7_MPS	79402 
#define VELOCITY_0_8_MPS	90745 
#define VELOCITY_0_9_MPS	102088 
#define VELOCITY_1_0_MPS	113431 
#define VELOCITY_1_1_MPS	124774 
#define VELOCITY_1_2_MPS	136117 
#define VELOCITY_1_3_MPS	147461 
#define VELOCITY_1_4_MPS	158804 
#define VELOCITY_1_5_MPS	170147 
#define VELOCITY_1_6_MPS	181490 
#define VELOCITY_1_7_MPS	192833 
#define VELOCITY_1_8_MPS	204176 
#define VELOCITY_1_9_MPS	215519 
#define VELOCITY_2_0_MPS	226862 
#define VELOCITY_2_1_MPS	238205 
#define VELOCITY_2_2_MPS	249549 
#define VELOCITY_2_3_MPS	260892 
#define VELOCITY_2_4_MPS	272235 
#define VELOCITY_2_5_MPS	283578 
#define VELOCITY_2_6_MPS	294921 
#define VELOCITY_2_7_MPS	306264 
#define VELOCITY_2_8_MPS	317607 
#define VELOCITY_2_9_MPS	328950 
#define VELOCITY_3_0_MPS	340294 
#define VELOCITY_3_1_MPS	351637 
#define VELOCITY_3_2_MPS	362980 
#define VELOCITY_3_3_MPS	374323 
#define VELOCITY_3_4_MPS	385666 
#define VELOCITY_3_5_MPS	397009 
#define VELOCITY_3_6_MPS	408352 
#define VELOCITY_3_7_MPS	419695 
#define VELOCITY_3_8_MPS	431038 
#define VELOCITY_3_9_MPS	442382 
#define VELOCITY_4_0_MPS	453725 
#define VELOCITY_4_1_MPS	465068 
#define VELOCITY_4_2_MPS	476411 
#define VELOCITY_4_3_MPS	487754 
#define VELOCITY_4_4_MPS	499097 
#define VELOCITY_4_5_MPS	510440 
#define VELOCITY_4_6_MPS	521783 
#define VELOCITY_4_7_MPS	533127 
#define VELOCITY_4_8_MPS	544470 
#define VELOCITY_4_9_MPS	555813 
#define VELOCITY_5_0_MPS	567156 
#define VELOCITY_5_1_MPS	578499 
#define VELOCITY_5_2_MPS	589842 

#define SPEED_4_0_MPS2		453724.73		
#define SPEED_3_5_MPS2		397009.14		
#define SPEED_3_3_MPS2		374322.90		
#define SPEED_3_0_MPS2		340293.55
#define SPEED_2_5_MPS2		283577.96
#define SPEED_2_0_MPS2		226862.37
#define SPEED_1_7_MPS2		192833.01
#define SPEED_1_2_MPS2		136117.42
#define SPEED_1_5_MPS2		170146.77
#define SPEED_1_0_MPS2		113431.18
#define SPEED_0_7_MPS2      79401.82
#define SPEED_0_5_MPS2		56715.59

#define SPEED_0_04_MS_CPI				0.91

#define DIST_ARRIVED_LEVEL1				5000
#define DIST_ARRIVED_LEVEL2				1000

#define OHT_SPEED_GRADE_3_BIT           0x8
#define OHT_SPEED_GRADE_4_BIT           0x9
#define OHT_SPEED_GRADE_5_BIT           0xD
#define OHT_SPEED_GRADE_6_BIT           0xC
#define OHT_SPEED_GRADE_7_BIT           0xE
#define OHT_SPEED_GRADE_8_BIT           0xF

// Sick
#define SICK_OBS_SPEED_GRADE_3_BIT      0x8 // b 1000
#define SICK_OBS_SPEED_GRADE_5_BIT      0x9 // b 1001
#define SICK_OBS_SPEED_GRADE_7_BIT      0xB // b 1011
#define SICK_OBS_SPEED_GRADE_8_BIT      0xF // b 1111
#define SICK_OBS_SPEED_GRADE_STEP_OUT	0x7 // b 0111

// HYOKUYO 
#define HOKUYO_OBS_SPEED_GRADE_3_BIT   		0x8 // b 1000
#define HOKUYO_OBS_SPEED_GRADE_5_BIT        0xC // b 1100
#define HOKUYO_OBS_SPEED_GRADE_7_BIT        0xE // b 1110
#define HOKUYO_OBS_SPEED_GRADE_8_BIT        0xF // b 1111
#define HOKUYO_OBS_SPEED_GRADE_STEP_OUT		0xA // b 1010

#define MOVE_S_DCC_BY_SENSOR_NORMAL 	SPEED_3_5_MPS2
#define MOVE_S_DCC_BY_SENSOR_CURVE	 	SPEED_2_5_MPS2

#define MOVE_DS_DCC_BY_SENSOR_CURVE  	SPEED_2_5_MPS2
#define MOVE_DS_ACC_BY_SENSOR_CURVE  	SPEED_1_0_MPS2

#define SLOPE_DCC			SPEED_3_0_MPS2		


#define AXIS_F_DRIVING					0
#define AXIS_R_DRIVING	                1
#define AXIS_HOIST		                2
#define AXIS_SLIDING	                3

#define AXIS_STOP_COUNT_LIMIT							    5000	// 5 sec
#define MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON 		500		// 0.5 sec
#define MOVE_DS_RESTART_COUNT_LIMIT_AFTER_STOP			500		// 0.5 sec

#define forcedNodeCountDistLimit						200

#define DETECT_INFO_TYPE_ONLY_UBG		0
#define DETECT_INFO_TYPE_ONLY_PBS		1
#define DETECT_INFO_TYPE_BOTH			2

#define PAUSED_NONE		              	0
#define PAUSED_BY_TP		       		1
#define PAUSED_BY_OCS      				2

#define STATUS_NORMAL					0
#define STATUS_OBS_STOP					1
#define STATUS_PBS_STOP					2
#define STATUS_OHT_PAUSED    			3

#define OHT_DETECT_N_CHANGE_OFFST		1000

#define ACC_MODE     1
#define CRUISE_MODE  2
#define DCC_MODE    -1
#define STOP_MODE    0
#define NONE_MODE   -2

#define ACC_CHANGE_RATIO	0.5
//#define ACC_CHANGE_SPEED1	1.2
//#define ACC_CHANGE_SPEED2	3.3


// Axis Command Structure --> AMC CMD Structure
// NONE : 0
// MoveP: 1 // MoveN: 2 // MoveS: 3 // MoveDS: 4
// MoveP_T: 5 // MoveN_T: 6 // MoveS_T: 7 // MoveN_T: 8
// Reset: 9 // setHome: 10
typedef struct _CMD_AXIS
{
	volatile char CmdType;	// Move (P,N,S,DS - TimeBase, AccelBase), reset, SetHome 등등 
	volatile int Accel;		// 가속도(Cts/s) or 시간(msec)
	volatile int Decel;		// 가속도(Cts/s) or 시간(msec)
	volatile int Speed;		// 속도(Cts/s)
	volatile int Distance;	// 거리(Cts)
	volatile int TriggerCtrlInfo;
}CMD_AXIS;

// 근접제어를 위한 변수
typedef struct _DETECT_INFO
{
	volatile char UseFollowControlFlag;	///< 근접 제어 사용 여부: true 면 사용, false면 미사용 
	volatile char Type;					///< Link의 Type
	volatile int NodeCount;				///< 노드 개수
	volatile int DistSumOfStraightPath;	///< 직진 길이 이어지는 지점까지의 위치 (Cts)
	volatile int FollowCommandType;		
	volatile int PBSIgnoreFlag;
}DETECT_INFO;

typedef struct _PAUSE_INFO
{
	volatile int	Pause;		
	volatile int	Commander;	
}PAUSE_INFO;


// AMC CMD Structure
typedef struct _AMC_CMD
{
	volatile int Cmd;			///< bit 단위 명령 : CMD_TYPE bitfield 참조
	volatile int Output[4];		///< Output 정보
	CMD_AXIS CmdAxis[4];		///< 축 명령 정보
	DETECT_INFO DetectInfo;		///< 전방 감지 Type 설정을 위한 정보
	PAUSE_INFO		PauseInfo;
    volatile int crc;
}AMC_CMD;

// AMC to DLL Data
// AMC_CMD_BIT_ANALYSIS
typedef enum _AMC_CMD_TYPE
{
	AMC_CMD_TYPE_SET_OUTBIT	= 0,
	AMC_CMD_TYPE_CLEAR_NODE_COUNT,
	AMC_CMD_TYPE_MOVE_AXIS1,
	AMC_CMD_TYPE_MOVE_AXIS2,
	AMC_CMD_TYPE_MOVE_AXIS3,
	AMC_CMD_TYPE_MOVE_AXIS4,
	AMC_CMD_TYPE_SET_DETECT_INFO,
	AMC_CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE,
	AMC_CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE,		
	AMC_CMD_TYPE_COMPENSATE_REAR_ENCODER			
}AMC_CMD_TYPE;

typedef struct _NODE_CHECK_DATA
{
	volatile int NodeCountOfPreCheck;	///< 노드 개수
	volatile int NodeCount;				///< 노드 개수
	
	// SetHome, setOffset 시 하면 현재 위치를 뺀 값으로 재저장
	volatile int PositionOfDrivingTagPreCheck;	///< 주행 노드 사전 감지된 위치		
	volatile int PositionOfDrivingTag;			///< 주행 노드 감지된 위치			 
	volatile int PositionOfTransTagFirst;		///< 이적재 노드 감지된 위치(Front)
	volatile int PositionOfTransTagFinal;		///< 이적재 노드 감지된 위치(Rear)
	
}NODE_CHECK_DATA;

typedef struct _AMC_DATA
{
	// Input/Output Data
	volatile int Input[4];						///< InPut 정보
	volatile int Output[4];						///< Output 정보

	// Axis Data
	volatile int Source[4];						///< 축별 source
	volatile int State[4];						///< 축별 state
	volatile char Running[4];					///< 축별 움직이는 중 확인
	volatile char InPosition[4];				///< 축별 도착 완료 확인
	volatile char AmpEnable[4];					///< 축별 동작 대기 상태 확인(Servo On, ~~)
	volatile int Position[4];					///< 축별 위치 (Cts)
	volatile int Velocity[4];					///< 축별 속도 (Cts/sec)
	volatile int FollowingError[4];				///< 축별 에러 (Cts)
	volatile int PositionOfSmallAddCheck[4];	///< Small Add 실행 시 센서 감지 위치값 ( 미감지,미사용 시 0.0 / 감지 위치가 0일 경우 1로 받음 )  (Cts)

	// AMC Log
	volatile int TargetVel[4];					///< Sample 당 속도 Reffernce
	volatile int TargetPos[4];					///< Sample 당 위치
	volatile int FinalPos[4];					///< 종착지 위치
	
	volatile int AxisFlag1[4];
	volatile int AxisFlag2[4];
	
	volatile int VoltageMonitor[2];	
	volatile int PauseState;	
	
	NODE_CHECK_DATA NodeCheckData;				///< OHTv7 을 위한 추가 정보
}AMC_DATA;

typedef struct _DRIVING_PARAM
{
	//2 주행 기본 설정값
	volatile int GearRatioOfDriving;			///< 주행 Gear 비(Cts/m) --> 1m 에 1mm 이내의 오차
	
	//2 근접제어
	volatile int OHTDetectAreaDist[6];			///< 대차 감지 센서 영역당 설정 거리 (단위 Cts)
	volatile int OHTDetectAreaSpeed[6];			///< 대차 감지 센서 영역당 설정 속도 (단위 Cts/s)
	volatile int OBSDetectAreaDist[6];			///< 장애물 감지 센서 영역당 설정 거리 (단위 Cts)
	volatile int OBSDetectAreaSpeed[6];			///< 장애물 감지 센서 영역당 설정 속도 (단위 Cts/s)
	volatile int AccelOfNormalPath;				///< 직진구간에서의 가속 값
	volatile int DecelOfNormalPath;				///< 직진구간에서의 감속 값
	volatile int AccelOfCurvePath;				///< 곡선구간에서의 가속 값
	volatile int DecelOfCurvePath;				///< 곡선구간에서의 감속 값
}DRIVING_PARAM;

typedef struct _CMD_ANALYSIS
{	
	unsigned int SetOutBit					: 1;		///< I/O의 Output 설정 : ①
	unsigned int ClearNodeCount				: 1;		///< Node Count Clear : ②
	unsigned int MoveAxis1					: 1;		///< 1축 이동 명령 : ①
	unsigned int MoveAxis2					: 1;		///< 2축 이동 명령 : ①
	unsigned int MoveAxis3					: 1;		///< 3축 이동 명령 : ①
	unsigned int MoveAxis4					: 1;		///< 4축 이동 명령 : ①
	unsigned int SetDetectInfo				: 1;		///< 전방 감지 Type 설정 : ②
	unsigned int SetGearRatioOfDriving		: 1;		// 제거 예정 /< 주행 Gear 비 설정 : ①
	unsigned int SetDecelLimitOfDriving		: 1;		// 제거 예정 /< 감속 Limit 설정 : ①
	unsigned int CompensateRearEncoder		: 1;		///< Rear값을 Front 값으로 맞춰주는 명령 : ②
	unsigned int PauseControl				: 1;		
	unsigned int Reserved					: 21;
}CMD_ANALYSIS;

typedef union _CMD_ANALYSIS_UNION
{
	CMD_ANALYSIS CmdAnalysis;	///< AMC_CMD.Cmd의 bitfield
	int Cmd;
}CMD_ANALYSIS_UNION;

// AMC_CMD_BIT_ANALYSIS
typedef enum _MOTION_CONTROLLER_CMD_TYPE
{
	CMD_TYPE_SET_OUTBIT					= 0,
	CMD_TYPE_CLEAR_NODE_COUNT			,
	CMD_TYPE_MOVE_AXIS1					,
	CMD_TYPE_MOVE_AXIS2					,
	CMD_TYPE_MOVE_AXIS3					,
	CMD_TYPE_MOVE_AXIS4					,
	CMD_TYPE_SET_DETECT_INFO			,
	CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING	,
	CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING	,
	CMD_TYPE_COMPENSATE_REAR_ENCODER	,
	CMD_TYPE_PAUSE_CONTROL
}MOTION_CONTROLLER_CMD_TYPE;

//2	Trigger Command
typedef struct _TRIGGER_CTRL_INFO
{	
	unsigned int StartOnSpeedDown		: 1;		///<명령 인가 시점( 0:바로 시작, 1:이전 프로파일의 감속 시점에 명령 인가
	unsigned int UseCurrSpeed			: 2;		///<사용할 속도( 0: 명령 받은 속도, 1: 현재 속도, 2:Min(명령속도, 현재속도), 4:MAX(명령속도, 현재속도) )
	unsigned int UseSmallAdd			: 1;		///<Small Add ( 0: NoUse, 1: Use )
	unsigned int TriggerIONumber		: 8;		///< I/O 번호 (0~255): ②
	unsigned int IsTriggerOnRisingEdge	: 1;		///<Triger Type ( 0: Falling Edge, 1: Rising Edge )
	unsigned int AdditionalDist			: 19;		///< 1회 추가 거리 ( 0~2^19 / 방향 정보는 최종 타겟 거리에서 획득): 524288cts --> 주행: 4599mm
}TRIGGER_CTRL_INFO;

// AMC_CMD.Cmd 를 쉽게 만들고 분석하기 위한 union
typedef union _TRIGGER_CTRL_INFO_UNION
{
	TRIGGER_CTRL_INFO TriggerInfo;	///< AMC_CMD.Cmd의 bitfield
	int Cmd;
}TRIGGER_CTRL_INFO_UNION;

struct sSensorMonitor
{
	volatile int State;         	// 현재 센서가 어떤 상태인가.
	volatile int SensorNum;
	volatile int KeepEdgeCounter;
	volatile int OldSensorValue;
	volatile int LowerLimit;		// 하한 리밋
	volatile int UpperLimit;		// 상한 리밋
	volatile int EdgePos;			// 엣지가 걸린 Pos
	volatile int SensorStatus;		// 현재 센서가 On인 상태인지
	volatile int LastEdgePnt;		// 이전 Edge저장 
	
	sSensorMonitor()
	{
		SensorNum = 0;
		SensorStatus = 0;
		SensorNum= -1;
		KeepEdgeCounter	= 0;	
	}
};

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
FOLLOWS_EXT volatile int AMC_Follow_Control_Status;
FOLLOWS_EXT volatile int AMC_Pause_Control;
FOLLOWS_EXT volatile int PauseCmd;
FOLLOWS_EXT volatile int PauseBy;
FOLLOWS_EXT volatile int PauseFlag;
FOLLOWS_EXT volatile int Model_OHT_ID;
FOLLOWS_EXT volatile int Model_OHT_DETECT_WRITE_BIT_MASK;
FOLLOWS_EXT volatile int Model_OBS_DETECT_WRITE_BIT_MASK;
FOLLOWS_EXT volatile int Model_DRIVINGPRECHECK;
FOLLOWS_EXT volatile int Model_DRIVINGFRONTNUM;
FOLLOWS_EXT volatile int Model_TRANSFRONT;
FOLLOWS_EXT volatile int Model_TRANSREAR;
FOLLOWS_EXT volatile int Model_OHT_DETECT_WRITE_BIT_SCALE;
FOLLOWS_EXT volatile int Model_OBS_DETECT_WRITE_BIT_SCALE;
FOLLOWS_EXT volatile int Model_OHT_DETECT_READ_BIT_SCALE;
FOLLOWS_EXT volatile int Model_OBS_DETECT_READ_BIT_SCALE;
FOLLOWS_EXT volatile int Model_FOLLOWSDCC;

FOLLOWS_EXT volatile int Model_OBS_SENSOR_MAKER;
FOLLOWS_EXT volatile int Model_OHT_SENSOR_ENABLE;

FOLLOWS_EXT volatile int Model_OHT_Service;

FOLLOWS_EXT volatile int Model_FILTER_4_NODECNT;

FOLLOWS_EXT volatile int Model_AXIS_STOP_COUNT_LIMIT;

FOLLOWS_EXT volatile int Model_DIST_ARRIVED_LEVEL1;
FOLLOWS_EXT volatile int Model_DIST_ARRIVED_LEVEL2;

FOLLOWS_EXT volatile int OHT_DETECT_N_VALUE_Flag;
FOLLOWS_EXT volatile int OHT_DETECT_N_Distance;

FOLLOWS_EXT volatile double Model_PULSETOMM;

FOLLOWS_EXT volatile double Model_SPEED_GRADE_1_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_2_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_3_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_4_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_5_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_6_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_7_VELOCITY;
FOLLOWS_EXT volatile double Model_SPEED_GRADE_8_VELOCITY;

FOLLOWS_EXT volatile int Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON;

FOLLOWS_EXT volatile int Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
FOLLOWS_EXT volatile int Model_MOVE_S_DCC_BY_SENSOR_CURVE;
FOLLOWS_EXT volatile int Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
FOLLOWS_EXT volatile int Model_MOVE_DS_DCC_BY_SENSOR_CURVE;

FOLLOWS_EXT volatile int UBG_Setting_flag;

FOLLOWS_EXT volatile int SoftMotionFlag;
FOLLOWS_EXT volatile int SoftStopMotionFlag;
FOLLOWS_EXT volatile int count_SoftMotion;
FOLLOWS_EXT volatile int count_Soft_StopMotion;

FOLLOWS_EXT volatile int MoveDS_Flag;
FOLLOWS_EXT volatile int MoveS_Flag;
FOLLOWS_EXT volatile int NodeCount;
FOLLOWS_EXT volatile int PreCheckCount;
FOLLOWS_EXT volatile int PositionOfDrivingTag;       // 주행 노드 감지된 위치
FOLLOWS_EXT volatile int PositionOfTransTagFirst;    // 이적재 노드 감지된 위치(Front)
FOLLOWS_EXT volatile int PositionOfTransTagFinal;    // 이적재 노드 감지된 위치(Rear)
FOLLOWS_EXT volatile int PositionOfPreCheck;         // Precheck센서 감지된 위치 
FOLLOWS_EXT volatile int FlagFirstNode;              // 첫번째 노드에서 설정하는 관계 플래그
FOLLOWS_EXT volatile int OHT_Detect_ChangeNodeValue;              
FOLLOWS_EXT volatile int OHT_Detect_AreaSetting;
FOLLOWS_EXT volatile int OHT_Detect_AreaSetting_For_Curve;
FOLLOWS_EXT volatile int OHT_Detect_AreaSetting_Flag;
FOLLOWS_EXT volatile int OHT_Detect_AreaSetting_Flag2;
FOLLOWS_EXT volatile int OHT_PBS_IgnoreSettingFlag;
FOLLOWS_EXT volatile int OHT_PBS_IgnoreFlag;
FOLLOWS_EXT volatile int OHT_PBS_IgnoreChangeNodeValue;
FOLLOWS_EXT volatile int PBS_Ignore_Flag;
FOLLOWS_EXT volatile int OBS_Detect_AreaSetting;
FOLLOWS_EXT volatile int tmp_OBS_Detect_AreaSetting;
FOLLOWS_EXT volatile double OHT_velocity_Level;
FOLLOWS_EXT volatile double OBS_velocity_Level; 
FOLLOWS_EXT volatile int AMC_Stop_count; 
FOLLOWS_EXT volatile int Rear_servo_Off_flag; 
FOLLOWS_EXT volatile double Sensor_velocity_Level; 
FOLLOWS_EXT volatile double FollowsACC;
FOLLOWS_EXT volatile double tmpFollowsACC;
FOLLOWS_EXT volatile double FollowsDCC;
FOLLOWS_EXT volatile double FollowsDCC_CMD;
FOLLOWS_EXT volatile double FollowsVEL;
FOLLOWS_EXT volatile int FollowsPOS;
FOLLOWS_EXT volatile double FollowsACC_Prev;
FOLLOWS_EXT volatile double FollowsDCC_Prev;
FOLLOWS_EXT volatile double FollowsVEL_Prev;
FOLLOWS_EXT volatile int FollowsPOS_Prev;
FOLLOWS_EXT volatile int Follows_MoveDS_Flag;
FOLLOWS_EXT volatile int OBS_Distance;
FOLLOWS_EXT volatile int OBS_Ignore_Distance;
FOLLOWS_EXT volatile double Target_Velocity_by_Sensor;
FOLLOWS_EXT volatile double Target_velocity_cmd;
FOLLOWS_EXT volatile int MoveDS_Wait_Count_After_Servo_On;
FOLLOWS_EXT volatile int MoveDS_Wait_Count_After_Servo_On_Flag;
FOLLOWS_EXT volatile int MoveDS_Wait_Count_After_Stop_Flag;
FOLLOWS_EXT volatile int MoveDS_Wait_Count_After_Stop;
FOLLOWS_EXT volatile double CurrentSpeed4NodeFilter;
FOLLOWS_EXT sSensorMonitor NodeCountState[MONITORING_SENSOR_NUM];
FOLLOWS_EXT TRIGGER_CTRL_INFO_UNION TriggerCtrlInfoParsing[4];
FOLLOWS_EXT volatile double ACC_For_Marking[4];
FOLLOWS_EXT volatile double DCC_For_Marking[4];
FOLLOWS_EXT volatile double VEL_For_Marking[4];
FOLLOWS_EXT volatile int POS_For_Marking[4];
FOLLOWS_EXT volatile int POS_For_Marking_Final[4];
FOLLOWS_EXT volatile int UseSmallAdd_For_Marking[4];
FOLLOWS_EXT volatile int IoNumber_For_Marking[4];
FOLLOWS_EXT volatile int IoType_For_Marking[4];
FOLLOWS_EXT volatile int DistanceAddition_For_Marking[4];
FOLLOWS_EXT volatile int DistanceLimit_For_Marking[4];
FOLLOWS_EXT volatile int JtPos_During_Marking[4];
FOLLOWS_EXT volatile int Marking_Process_Flag[4];
FOLLOWS_EXT volatile int Marking_Process_Trigger_Flag[4];
FOLLOWS_EXT volatile int DirectionFlag_For_Marking[4];
FOLLOWS_EXT volatile int FlagIO_For_Marking[4];
FOLLOWS_EXT volatile int Follow_Enable_Flag;
FOLLOWS_EXT volatile int OHT_Sensor_value;
FOLLOWS_EXT volatile int OBS_Sensor_value;

FOLLOWS_EXT volatile int Debug_Flag;
FOLLOWS_EXT volatile int MoveCondition;

FOLLOWS_EXT volatile int forcedMoveStopCommand;
FOLLOWS_EXT volatile int followsMoveSFailCnt;

FOLLOWS_EXT volatile int TorqueLimit_Request;
FOLLOWS_EXT volatile int TorqueLimit_Response;

FOLLOWS_EXT volatile int Path_Slope_Status;

FOLLOWS_EXT volatile int Servo_Off_type;

FOLLOWS_EXT volatile int Final_Arrival_Flag;

FOLLOWS_EXT volatile int currentSpeedMode[4];

FOLLOWS_EXT volatile int cruise_count[4];
FOLLOWS_EXT volatile int acc_count[4];
FOLLOWS_EXT volatile int dcc_count[4];

FOLLOWS_EXT volatile int cruise_mode_cnt;
FOLLOWS_EXT volatile int cruise_mode_set;
FOLLOWS_EXT volatile int stop_mode_cnt;

FOLLOWS_EXT volatile double Pre_Target_Velocity_by_Sensor;
FOLLOWS_EXT volatile int nCruiseTime;
FOLLOWS_EXT volatile int nStopTime;

// Clear Command
FOLLOWS_EXT volatile char LitzCableStatusClear[32];
// PreLitzCable
FOLLOWS_EXT volatile char PreLitzCableStatus[32];
// LitzCableStatus
FOLLOWS_EXT volatile char LitzCableStatus[32];
// LitzCableIO
FOLLOWS_EXT volatile char LitzCableIO[32];

FOLLOWS_EXT	volatile int DYNAMIC_ACC_1st;
FOLLOWS_EXT	volatile int DYNAMIC_ACC_2nd;
FOLLOWS_EXT	volatile int DYNAMIC_ACC_3th;
FOLLOWS_EXT	volatile int DYNAMIC_ACC_4th;

FOLLOWS_EXT volatile int Model_NODE_COUNT_IGNORE_UPPER_LIMIT_INIT;    
FOLLOWS_EXT volatile int Model_NODE_COUNT_IGNORE_UPPER_LIMIT;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
FOLLOWS_EXT double Get_Velocity_by_Area(void);
FOLLOWS_EXT int GetBit(int Source, int BitNum);
FOLLOWS_EXT int GetPos(void);
FOLLOWS_EXT int GetCurrentPos(int Axis);
FOLLOWS_EXT void AxisCmdDo(AMC_CMD *AmcCmd, int axis);
FOLLOWS_EXT void Main_to_Dsp_Command(char *Command, int Size, char *ReturnMessage, int *MessageLength);
FOLLOWS_EXT void State_Init(void);
FOLLOWS_EXT void State_WaitEdge(int SensorNum);
FOLLOWS_EXT void State_KeepEdge(int SensorNum);
FOLLOWS_EXT void Check_Rear_servo_Off(void);
FOLLOWS_EXT void Check_Front_Rear_servo_On(void);
FOLLOWS_EXT void Follows1ms(void);
FOLLOWS_EXT unsigned short crc16_ccitt(const char *buf, int len);

#endif

