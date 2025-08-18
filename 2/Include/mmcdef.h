#ifndef	__MMCDEF_H
#define	__MMCDEF_H

//OHT 버젼 에 따른 컴파일 선택

#define	__AMC_28x
//#define __AMC_25x
//#define __AMC_29x
//#define __AMC_SMD
#define __AMC_V70

#if (!defined(__AMC_25x) && defined(__AMC_28x) && defined(__AMC_29x) && !defined(__AMC_SMD) && !defined(__AMC_V70))
#elif (!defined(__AMC_25x) && defined(__AMC_28x) && !defined(__AMC_29x) && defined(__AMC_SMD) && !defined(__AMC_V70))
#elif (!defined(__AMC_25x) && defined(__AMC_28x) && !defined(__AMC_29x) && !defined(__AMC_SMD) && defined(__AMC_V70))
#else
[Define Duplicate error]
#endif

#define	TRUE				1
#define	FALSE				0

#define	HIGH				1
#define	LOW  				0

#define	CIR_CCW 			0
#define	CIR_CW  			1

#define	ST_NONE				0x0000
#define	ST_HOME_SWITCH		0x0001
#define	ST_POS_LIMIT		0x0002
#define	ST_NEG_LIMIT   		0x0004
#define	ST_AMP_FAULT		0x0008

#if (defined(__AMC_SMD) || defined(__AMC_29x))
#define	ST_A_LIMIT    		0x0010
#endif

#if defined(__AMC_V70)
#define	ST_SYSTEM_INSEC    	0x0010
#endif

#define	ST_V_LIMIT  			0x0020
#define	ST_X_NEG_LIMIT 			0x0040
#define	ST_X_POS_LIMIT			0x0080
#define	ST_ERROR_LIMIT			0x0100
#define	ST_PC_COMMAND  			0x0200		// Stop, E-Stop PC 명령어
#define	ST_OUT_OF_FRAMES    	0x0400
#define	ST_AMP_POWER_ONOFF  	0x0800
#define	ST_ENC_OVF				0x1000		// 2007.10.12, ckyu


#define	NO_EVENT			    0 	// ignore a condition
#define	STOP_EVENT			    1	// generate a stop event
#define	E_STOP_EVENT	    	2   // generate an e_stop event
#define	ABORT_EVENT		    	3 	// disable PID control, and disable the amplifier
#define	ENC_OVF_EVENT	    	4	// set if encoder overflow is present. actions is same as e-stop. ckyu, 2007.10.12
#define	PAUSE_EVENT		    	5	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.

#define	GAIN_NUMBER	    		5	// elements expected get/set_filter
#define	GA_P		    		0	// proportional gain
#define	GA_I		    		1	// integral gain
#define	GA_D		    		2	// derivative gain-damping term
#define	GA_F       		    	3	// velocity feed forward
#define	GA_ILIMIT 			    4	// integration summing limit

#define	MAX_ERROR_LEN			    	80 	// maximum length for error massage string 
#define	MMC_OK					    	0	// no problems
#define	MMC_NOT_INITIALIZED		    	1	// be sure to call mmc_init(...)
#define	MMC_TIMEOUT_ERR			    	2	// DPRAM Communication Error
#define	MMC_INVALID_AXIS		    	3	// axis out of range or not configured 
#define	MMC_ILLEGAL_ANALOG		    	4	// analog channel < 0 or > 4 
#define	MMC_ILLEGAL_IO				    5	// illegal I/O port 
#define	MMC_ILLEGAL_PARAMETER		    6	// move with zero accel or velocity 
#define	MMC_NO_MAP					    7 	// The map_axes(...) funcation has not been called 
#define	MMC_AMP_FAULT				    8 	// amp fault occured 
#define	MMC_ON_MOTION				    9  	// Motion is not completed 
#define	MMC_NON_EXIST				    10	// MMC Board is not exist 
#define	MMC_BOOT_OPEN_ERROR			    11	// MMC Boot File Read/Write Error
#define	MMC_CHKSUM_OPEN_ERROR		    12	// MMC Checksum File Read/Write Error
#define	MMC_WINNT_DRIVER_OPEN_ERROR	    13	// MMC Windows NT Driver Open Error
#define	FUNC_ERR					    -1	// Function Error				

#define	MMC_ILLEGAL_PARAM_MOVE_DS	    14	// move_ds, _dst 함수에서 목표 이동거리, 방향, 속도값에 이상이 있는 경우
#define	MMC_ILLEGAL_PARAM_MOVE_S	    32	// MoveDS 수행완료 전 수행되거나, 정지 상태에서 명령이 발생한 경우
#define	MMC_MOVE_LASTDS				    30	// move_ds의 마지막 n-1 번째 프로파일 실행 중임을 알림 20120301 통합 syk
#define	MMC_MUTEXLOCK_ERROR			    31	// MUTEXLOCK ERROR /dsp timeout error와 구분하기 위해 분리 20120316 2.9.2 syk
#define MMC_REVERSE_MOTION              15
#define MMC_ZERO_DISTANCE               16

#define	MMC_BOARD_NUM		        	1	// Total DSP board number // 사용할 수 있는 보드 수를 1장으로 제한
#define	BD_AXIS_NUM				        4	// DSP 1-bd axes number	
#define	TOTAL_AXIS_NUM			        (MMC_BOARD_NUM * BD_AXIS_NUM)

#define MMC_BD1					        0	    // DSP board #1
#define	MMC_BD2					        1	    // DSP board #2
#define	MMC_BD3					        2	    // DSP board #3
#define	MMC_BD4					        3	    // DSP board #4

#define	MAX_FRAME_NUM			        50      // Max Q_Buffer Number			

#define	POSITIVE					    1
#define	NEGATIVE					    0

#define	SERVO_MOTOR     0
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

#define	G_tSampling_Time	0.0002	// 0.2ms, 시간 단위

#define	MMC_ACCEL_LIMIT		1500	

//6000rpm=164PPT => 2배한 값보다 큰 속도가 검출되면 에러 처리
#define	AMC_MAX_PPT			(2.0*(6000.0/60.0)*PULSE_PER_REVOLUTION*G_tSampling_Time)	

#define	MOVEX_ACCEL_SCALER	4		//500 => 1000, 2007.12.28(오)

#define	PULSE_PER_REVOLUTION	8192	//1회전당 펄스 수

#define	MMC_PROF_MARGIN_LIMIT	    100
#define	MMC_VEL_LIMIT		        (100*8192)
#define	MMC_POS_SW_LIMIT	        2147483647
#define	MMC_NEG_SW_LIMIT	        -2147483647
#define	MMC_ERROR_LIMIT		        35000
#define	MMC_PULSE_RATIO		        255
#define	V_LIMIT_CHECK_CYCLE_CNT     50

#define	API		

#define	INT			int
#define	LONG		int
#define	CHAR		char
#define	FLOAT		float
#define	VOID		void
#define	pCHAR		char *
#define pINT		int *
#define pUINT		unsigned int *
#define	pLONG		LONG *
#define	pFLOAT		float *

#endif

