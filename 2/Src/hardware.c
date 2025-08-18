#define HARDWARE_GLOBALS

#include "all.h"

////////////////////////////////////////////////////////////////////////
// Function	: InitHardware
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitHardwareParam(void)
{
    int i;
#if (defined(__AMC_SMD) || defined(__AMC_V70))
    for( i=0 ; i < MMC_AXIS ; i++ )
    {
        ui_CurPos[i] = 0;
    }   
#endif
    
    pBootPara = (char *)(0x60000);
    
    MOTOR_DIRECTION_CONTROL = (int *) 0xA0000020;
    
    // 인코더카운트를 리턴하는 어드레스를 할당한다.
    ASIC_COUNT[0] = (int *)0xa0000040;
    ASIC_COUNT[1] = (int *)0xa0000050;
    ASIC_COUNT[2] = (int *)0xa0000060;
    ASIC_COUNT[3] = (int *)0xa0000070;
    
    //-- 속도명령을 내리는 어드레스를 할당한다.
    DA_PORT_ADDR[0] = (int *) 0xa0000030;
    DA_PORT_ADDR[1] = (int *) 0xa0000034;
    DA_PORT_ADDR[2] = (int *) 0xa0000038;
    DA_PORT_ADDR[3] = (int *) 0xa000003c;
    
    //-- 펄스 출력을 내보내는 어드레스를 할당한다.
    CLK_OUT_ADDR[0] = (int *) 0xa0000080;
    CLK_OUT_ADDR[1] = (int *) 0xa0000084;
    CLK_OUT_ADDR[2] = (int *) 0xa0000088;
    CLK_OUT_ADDR[3] = (int *) 0xa000008c;
    
    //-- 펄스형 모드일때의 출력된 펄스값을 읽어 오는데 이용한다.
    CLK_IN_ADDR[0] = (int *) 0xa0000090;
    CLK_IN_ADDR[1] = (int *) 0xa0000094;
    CLK_IN_ADDR[2] = (int *) 0xa0000098;
    CLK_IN_ADDR[3] = (int *) 0xa000009c;

    CommDPRAM=(char *)(0xb0001580);

    AxisDPRAM[0] = (char *) 0xb0001400;
    AxisDPRAM[1] = (char *) 0xb0001430;
    AxisDPRAM[2] = (char *) 0xb0001460;
    AxisDPRAM[3] = (char *) 0xb0001490;

    pDownloadDPRAM = (int*) 0xb0001400;
    
    Ack2Pc = (char *)0xb00017fe;
    Int2Pc = (char *)0xb00017ff;
    AxisInfo = (char *)0xb00017fc;
  
    //-- 사용자 출력포트로 설정(64비트: 16비트 * 4)
    ptr_user_oport = (int *) 0xa0300010;
    ptr_user_oport2 = (int *) 0xa0300014;
    ptr_user_oport3 = (int *) 0xa0300018;
    ptr_user_oport4 = (int *) 0xa030001c;
    
#if (defined(__AMC_SMD) || defined(__AMC_V70))
    //-- 사용자 출력포트로 설정(64비트: 16비트 * 4) // 120818 2.9.8 syk 추가 
    ptr_user_oport5 = (int *) 0xa0300030;
    ptr_user_oport6 = (int *) 0xa0300034;
    ptr_user_oport7 = (int *) 0xa0300038;
    ptr_user_oport8 = (int *) 0xa030003c;
    
    //-- 사용자 출력포트로 설정(64비트: 16비트 * 4) // 120818 2.9.8 syk 추가 
    ptr_user_oport9 = (int *) 0xa0300050;
    ptr_user_oport10 = (int *) 0xa0300054;
    ptr_user_oport11 = (int *) 0xa0300058;
    ptr_user_oport12 = (int *) 0xa030005c;
    
    //-- 사용자 출력포트로 설정(64비트: 16비트 * 4) // 120818 2.9.8 syk 추가 
    ptr_user_oport13 = (int *) 0xa0300070;
    ptr_user_oport14 = (int *) 0xa0300074;
    ptr_user_oport15 = (int *) 0xa0300078;
    ptr_user_oport16 = (int *) 0xa030007c;
#endif

    //-- 사용자 입력포트로 설정(64비트 : 16비트 * 4);
    ptr_user_iport = (int *) 0xa0300000;
    ptr_user_iport2 = (int *) 0xa0300004;
    ptr_user_iport3 = (int *) 0xa0300008;
    ptr_user_iport4 = (int *) 0xa030000c;
    
#if (defined(__AMC_SMD) || defined(__AMC_V70))
    //-- 사용자 입력포트로 설정(64비트 : 16비트 * 4); // 120818 2.9.8 syk 추가 
    ptr_user_iport5 = (int *) 0xa0300020;
    ptr_user_iport6 = (int *) 0xa0300024;
    ptr_user_iport7 = (int *) 0xa0300028;
    ptr_user_iport8 = (int *) 0xa030002c;
    
    //-- 사용자 입력포트로 설정(64비트 : 16비트 * 4); // 120818 2.9.8 syk 추가 
    ptr_user_iport9 = (int *) 0xa0300040;
    ptr_user_iport10 = (int *) 0xa0300044;
    ptr_user_iport11 = (int *) 0xa0300048;
    ptr_user_iport12 = (int *) 0xa030004c;
    
    //-- 사용자 입력포트로 설정(64비트 : 16비트 * 4); // 120818 2.9.8 syk 추가 
    ptr_user_iport13 = (int *) 0xa0300060;
    ptr_user_iport14 = (int *) 0xa0300064;
    ptr_user_iport15 = (int *) 0xa0300068;
    ptr_user_iport16 = (int *) 0xa030006c;
#endif
    
#if defined(__AMC_V70)
    //-- 시스템 내부 전원 모니터링 // 
    ptr_power_12p = (UINT2 *) 0xa0101028;
    ptr_power_12m = (UINT2 *) 0xa010102c;
    ptr_power_5p = (UINT2 *) 0xa0101024;
    ptr_power_3p3 = (UINT2 *) 0xa0101020;
    
    ptr_power_cutoff_12p = (UINT2 *) 0xa0101018;
    ptr_power_cutoff_12m = (UINT2 *) 0xa010101c;
    ptr_power_cutoff_5p = (UINT2 *) 0xa0101014;
    ptr_power_cutoff_3p3 = (UINT2 *) 0xa0101010;
    
    ptr_Moni_R_Dac0 = (UINT2 *) 0xa0101030;
    ptr_Moni_R_Dac1 = (UINT2 *) 0xa0101034;
    ptr_Moni_R_Dac2 = (UINT2 *) 0xa0101038;
    ptr_Moni_R_Dac3 = (UINT2 *) 0xa010103c;
    
    ptr_cutoff_enable = (char *) 0xa0101000;
    ptr_cutoff_status = (char *) 0xa0101004;

    ptr_Power_MinMaxPeak_3p3 = (UINT2 *) 0xa0101040;
    ptr_Power_MinMaxPeak_5p  = (UINT2 *) 0xa0101044;
    ptr_Power_MinMaxPeak_12p = (UINT2 *) 0xa0101048;
    ptr_Power_MinMaxPeak_12m = (UINT2 *) 0xa010104c;
    ptr_WdStopDecelRatio_Ax0 = (UINT2 *) 0xa0100030;
    ptr_WdStopDecelRatio_Ax1 = (UINT2 *) 0xa0100034;
    ptr_WdStopDecelRatio_Ax2 = (UINT2 *) 0xa0100038;
    ptr_WdStopDecelRatio_Ax3 = (UINT2 *) 0xa010003c;
#endif
    
    //-- 서보제어출력으로 할당
    ptr_sys_sctrl = (int *) 0xa0000010;    // Servo On/Off, Alarm Set/Reset
    ptr_sys_sstat = (int *) 0xa0000014;    // Servo 상태 읽기 
    ptr_sys_sabs  = (int *) 0xa000001c;    // Servo 상태 읽기 
}


////////////////////////////////////////////////////////////////////////
// Function	: InitHardware
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitHardware( void )
{
    InitHardwareParam();
    
	*((volatile short *) 0xa0300000) = 0;				// I/O port(UIO0) enable( amp & alarm enable )

#if (defined(__AMC_SMD) || defined(__AMC_V70))	
	*((volatile short *) 0xa0300020) = 0;				// 120831 2.9.8 SYK //I/O port(UIO1)- 64~127 enable( amp & alarm enable )
	*((volatile short *) 0xa0300040) = 0;				// 120831 2.9.8 SYK //I/O port(UIO2)- 128~191 enable( amp & alarm enable )
	*((volatile short *) 0xa0300060) = 0;				// 120831 2.9.8 SYK //I/O port(UIO3)- 192~255 enable( amp & alarm enable )	
#endif		
	
	*ptr_sys_sctrl = 0xFFFF;		// SYSTEM I/O PORT OUTDATA
	
	pGBuf->exram_cnt = 0;			// EXT-RAM count clear
	pGBuf->sys_io_out = 0xFFFF;	// SERVO ALARM PRESET/POWER OFF
	pGBuf->user_io_out = 0x0;	// USER I/O DATA
	pGBuf->user_io_out2 = 0x0;	// USER I/O DATA
	pGBuf->io_int_ax = 0;
	
#if defined(__AMC_V70)
	__system_monitering_enable(POWER3P3, false);
	__s_monitering_threshold_percent(POWER3P3, POWER3P3_CHECK_Percent);

	__system_monitering_enable(POWER5P, false);
	__s_monitering_threshold_percent(POWER5P, POWER5P_CHECK_Percent);

	__system_monitering_enable(POWER12P, false);
	__s_monitering_threshold_percent(POWER12P, POWER12P_CHECK_Percent);

	__system_monitering_enable(POWER12M, false);
	__s_monitering_threshold_percent(POWER12M, POWER12Pm_CHECK_Percent);
#endif	

	Init_eeprom();
	
	InitMotorParameters();
	ControlIdle();
	ClearDPRInt();		// clear dual port ram interrupt
	ResetPulseOut();
	InitPIDGain();		// initialize gains
	InitDA();			// 0v dac output port addresses
	InitPosition();		// initialize counter addresses

	SelectFlashROM();

	// 콘솔포트 초기화
	// port num = 2
	SERIAL_Init();
    SERIAL_SetPort(115200, 8, 0, 1);		
	SERIAL_Enable(TRUE);

	// 가이드센서신호를 수신하기위한 직렬포트를초기화 한다.
	// port num = 1;
    // 2007.11.23 시리얼 포트 인터럽트를 막기 위해 임시로 막았음.
    // SMR에서는 사용해야 하는 포트임.	
	{
		char str[100];
		sprintf(str, "%s %s (Cantops %s)\r\n", AMC_VERSION, VERSION_STRING, __DATE__);
		SERIAL_WriteBytes(str, strlen(str));
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __ReadEncoder
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __ReadEncoder( int ax )
{
    // FPGA 구조 변경에 따라 인코더는 항상 200us 인터럽트 루틴에서만 읽는다. FPGA 3.x 버전
	// 속도 측정에 필요한 카운터 값을 읽는다.
    // pCBuf[ax]->TCounter = (int)(*MT_COUNTER[ax]);//저속용 펄스 폭 카운터값 저장
    // pCBuf[ax]->SUM_TCounter = (int)(*(MT_COUNTER[ax] + 1));//M개의 펄스를 카운팅한 전체 시간 
    // pCBuf[ax]->MCounter = abs( (int)*(MT_COUNTER[ax] + 2) );//제어 주기에서 카운팅한 인코더 펄스 수

	int temp_cnt, encoder_cnt[3];
	int wrong_cnt = 0;

 	encoder_cnt[0] = *ASIC_COUNT[ax];
 	encoder_cnt[1] = *ASIC_COUNT[ax];
 	encoder_cnt[2] = *ASIC_COUNT[ax];


 	// 큰 순서로 소팅 후 중간 어레이값을 취함
 	// 4500RPM, 최대 128.9펄스 이동 가능
 	// 2.5.21 버전 이하의 FPGA에서 인코더 리딩 시 Glitch 발생함.
	if(encoder_cnt[0] > encoder_cnt[1]) 
	{
		temp_cnt = encoder_cnt[0];
		encoder_cnt[0] = encoder_cnt[1];
		encoder_cnt[1] = temp_cnt;
	}

	if(encoder_cnt[0] > encoder_cnt[2]) 
	{
		temp_cnt = encoder_cnt[0];
		encoder_cnt[0] = encoder_cnt[2];
		encoder_cnt[2] = temp_cnt;
	}

	if(encoder_cnt[1] > encoder_cnt[2]) 
	{
		temp_cnt = encoder_cnt[1];
		encoder_cnt[1] = encoder_cnt[2];
		encoder_cnt[2] = temp_cnt;
	}


    if(ax == 0) // This if-part will be removed after the cause of wrong encoder is figured out
    {
    	if(encoder_cnt[0] != encoder_cnt[1]) // The encoder_cnt[1] is the accepted value for real using
    	{                                    // One of wrong encoder values is logged
    		EncTrc = encoder_cnt[0] - pCBuf[ax]->EncoderOffset;
    		wrong_cnt++;
    	}
    	if(encoder_cnt[2] != encoder_cnt[1])
    	{
    		EncTrc = encoder_cnt[2] - pCBuf[ax]->EncoderOffset;
    		wrong_cnt++;
    	}

    	if(wrong_cnt == 0)
    	{
    		EncTrc = 0xFFFFFFFF;         // 0xFFFFFFFF means each encoder value is the same(normal situation).
    	}
    	else if(wrong_cnt == 2)          // to check if all of the three encoder values are not the same.
    	{                                // if MSB of EncTrc is set, it will mean every encoder value read is different.
    		EncTrc = EncTrc | 0x80000000;
    	}
    }
	
 	return encoder_cnt[1] - pCBuf[ax]->EncoderOffset;
}

////////////////////////////////////////////////////////////////////////
// Function	: InitMotorParameters
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitMotorParameters()
{
	// 각 모터의 리?레벨을 초기화 한다.
	int i;
	
	for (i = 0; i < MMC_AXIS; i ++)
	{
		pCBuf[i]->reset_level = 1;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ResetEncCounter
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ResetEncCounter( void )
{
	int i;
	
	// 센스 신호를 인가한 후 회전수 및 각도를 읽어서 내부의 카운터에 설정한다.
	
	ResetEncoderTiming(11);		// 인코더 1회전 내의 데이터인 분해능 정보

    // 확인해야 할 초기화 부분(오)
    // 회전량과 펄스를 모두 초기화 한다.
    // 두가지를 모두 초기화하는것이 정확한지 이후 확인하여 적용해야 한다.
	for( i=0 ; i<MMC_AXIS ; i++ )
    {
		pCBuf[i]->init_counter = 0;
	}
	pGBuf->zr_enable = 0xFF;
}

////////////////////////////////////////////////////////////////////////
// Function	: ControlIdle
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ControlIdle( void )
{
	int i;
	
	for( i=0 ; i<MMC_AXIS ; i++ )
	{
		pCBuf[i]->g_on = 1;
		pCBuf[i]->axis_source = ST_AMP_POWER_ONOFF;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ClearDPRInt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ClearDPRInt( void )
{
	static int temp;
	
	temp = *Ack2Pc;
	return (temp&0xFF); //Clear Int시 Echocommand 수행 위해 return추가
}

////////////////////////////////////////////////////////////////////////
// Function	: ResetPulseOut
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ResetPulseOut( void )
{
	int i;
	
	for( i=0 ; i<MMC_AXIS ; i++ )
    {
		*CLK_OUT_ADDR[i] = 0;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: InitPIDGain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitPIDGain( void )
{
	int i;
	
	for( i=0 ; i<MMC_AXIS ; i++ )
	{
		pCBuf[i]->pgain = P_GAIN;
		pCBuf[i]->igain = I_GAIN;
		pCBuf[i]->dgain = D_GAIN;
		pCBuf[i]->fgain = F_GAIN;
		pCBuf[i]->jgain = J_GAIN;
		pCBuf[i]->ilimit = I_LIMIT;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: InitDA
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitDA( void )
{
	int ax;
	
	for( ax=0 ; ax<MMC_AXIS ; ax++ )
	{
	    // offset을 고려한 값을 출력하도록 수정 요망. 2007.11.2
		*DA_PORT_ADDR[ax] = 0x8000;	
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: InitPosition
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitPosition( void )
{
	int i;
	int val;
	float fval;
	
	for( i=0 ; i<JPC_AXIS ; i++ )
	{
		pCBuf[i]->jtpos = pCBuf[i]->jtpos_b = 0;
		val = pCBuf[i]->jtpos; 
		fval = -val;

		pCBuf[i]->mdRefpos = fval;
		pCBuf[i]->mdRefpos_b = fval;
		pCBuf[i]->dPosCmd = fval;
		pCBuf[i]->dPosCmdRaw = fval;
		if(i==0)
		{
			FollowsPOS = fval;
			FollowsPOS_Prev = FollowsPOS;
		}
		pCBuf[i]->init_counter = pCBuf[i]->jtpos;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: SetTimer
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetTimer( void )
{
	GPEN |= 0xe0;		// set to 0, GPINT4~7 is interrupt input 
	GPDIR &= ~0xf0;
	
	// Adjust Interrupt priority.
	// 4:Serial, 5:1ms, 6:5ms, 7:DPRAM, 8:10ms interrupt.
	MUXL = 0x24e61444;
	
	// 아래로 변경되었음. 2007.12.3, ckyu
	// 4:200us, 5:1ms, 6:5/10ms, 7:DPRAM, 8:10ms , serialinterrupt.
	// Emul. Ser.  Dpram   5/10ms 1ms   200us
	// X01001 00100 00111   X00110 00101 00010
	// 0010 0100 1000 0111  0001 1000 1010 0010
	
    // MUXL = 0x248718a2;
    //	MUXH &= ~0xff000000;
    //	MUXH |= 0x1c000000;

    // 2011.5.5 sghan 인터럽트 구조변경
	SetVect(6, Control_Int200us);	// 1ms interrupt, FPGA에서 DSP의 200us 펄스를 받아 생성
	SetVect(7, Dpr_Int);		    // DPRAM Interrupt
	SetVect(8, AMC_Int1ms);		    // 5ms interrupt, FPGA에서 DSP의 200us 펄스를 받아 생성

	EnableInterrupt(6, 1);
	EnableInterrupt(7, 1);
	EnableInterrupt(8, 1);
}

////////////////////////////////////////////////////////////////////////
// Function	: SelectFlashROM
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SelectFlashROM( void )
{
	char temp;
	
	// Reset Command for Am29F160D
	*(char*)0x900 = 0xF0;
	
	// Autoselect Command for Am29F160D
	*(char*)0xAAA = 0xAA;
	*(char*)0x555 = 0x55;
	*(char*)0xAAA = 0x90;
	
	temp = *(char*)0x1000;
	
	if( (temp&0xFF) == 0x01 )
	{
	    // if flash ROM's manufaturer is AMD
		FlashType = 1;
	}
	// Reset Command for Am29F160D
	*(char*)0x900 = 0xF0;
}

////////////////////////////////////////////////////////////////////////
// Function	: ServoOn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ServoOn( int ax )
{
	int i;
	
	i=1<<ax;
	
	if(!pCBuf[ax]->amp_on_level)
	{
		i=~i;
		pGBuf->sys_io_out = i&pGBuf->sys_io_out;
	}
	else
	{
		pGBuf->sys_io_out = i|pGBuf->sys_io_out;
	}
	
	*ptr_sys_sctrl=pGBuf->sys_io_out;
}

////////////////////////////////////////////////////////////////////////
// Function	: ServoOff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ServoOff( int ax )
{
	int	i;

	i=1<<ax;
	
	if(pCBuf[ax]->amp_on_level)
	{
		i=~i;
		pGBuf->sys_io_out = i&pGBuf->sys_io_out;
	}
	else
	{
		pGBuf->sys_io_out = i|pGBuf->sys_io_out;
	}
	*ptr_sys_sctrl=pGBuf->sys_io_out;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadCommand
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	ReadCommand(void)
{
	int	val;
	
	val = (*Ack2Pc)&0xFF;
	*Ack2Pc = 0;
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadCommandAxis
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadCommandAxis( void )
{
	return (int)(*AxisInfo & 0x00FF);
}

////////////////////////////////////////////////////////////////////////
// Function	: EchoCommand
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	EchoCommand( int x )
{
	int xx;

	xx = x;
	
	*Int2Pc = (xx & 0xFF);
	
	return xx;
}

////////////////////////////////////////////////////////////////////////
// Function	: GetEncoderHwDirection
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetEncoderHwDirection(int ax)
{
    unsigned int val = *MOTOR_DIRECTION_CONTROL;
    
	val = (val >> (ax * 8)) & 0x80;
	
	if (val == 0x80) 
	{
    	return CIR_CCW;
    }
    else
    {
    	return CIR_CW;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: SetEncoderHwDirection
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int SetEncoderHwDirection(int ax, int cwccw)
{
    unsigned int val = *MOTOR_DIRECTION_CONTROL;
    int mask = 0x80 << (ax * 8);
    
	val &= ~mask;
	
	int newval = 0x00;
	
	if (cwccw == CIR_CCW) 
	{
    	newval = 0x80;
    }
    
	val |= newval << (ax * 8);
	
	*MOTOR_DIRECTION_CONTROL = val;
		
	return cwccw;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_encoder_direction
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_encoder_direction(int ax)
{
	if (BootPara.st_boot.Encoder_direction[ax] == CIR_CW)
	{
    	return CIR_CW;
    }
    else
    {
    	return CIR_CCW;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadUserIn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadUserIn( void )
{
	int indata;
	
	indata = (*ptr_user_iport);
	indata |= ( (*ptr_user_iport2) & 0xFFFF ) << 16;
	
	return indata;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_io64
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_get_io64(int *pdat)
{
    __GetIoInData(&pdat[0], &pdat[1]); 
}

#if defined(__AMC_29x)		
////////////////////////////////////////////////////////////////////////
// Function	: __get_bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK 64BIT  
////////////////////////////////////////////////////////////////////////
int __get_bit(int bit, int *pdat)
{
	if (bit < 32) 
	{
    	return (pdat[0] >> bit) & 0x01; // 2010.10.31 [2] => [0]
    }
    else
    {
    	return (pdat[1] >> (bit-32)) & 0x01;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_bit
// Brief	: 통합 버젼 120228 syk 리턴값과 error 코드 분리 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_bit(int bitno, pINT chk_err)	
{
	int dat[2];
	
	if (bitno < 0 || bitno > 63)
	{
		*chk_err = FUNC_ERR;
		return 0;
	}

	*chk_err = MMC_OK;	
	
	_dsp_get_io64(dat);
	
	return __get_bit(bitno, dat);
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_outbit
// Brief	: 통합 버젼 120228 syk 리턴값과 error 코드 분리
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_outbit(int bitno, pINT chk_err)	
{
	int dat[2];
	
	if (bitno < 0 || bitno > 63)
	{
		*chk_err = FUNC_ERR;
		return 0;
	}

	*chk_err = MMC_OK;	
	
	amc_error=_dsp_get_out64(dat);  // 2102.07.01 syk ,_dsp_get_out64에서 amc_error전역 변수에 값을 넣는 것을 삭제함에 의해 이부분에 추가하였음 
	
	return __get_bit(bitno, dat);
}
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70)) 		// 256BIT까지 확장
////////////////////////////////////////////////////////////////////////
// Function	: __get_bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __get_bit(int bit, int *pdat)
{
	if (bit < 32)		return (pdat[0]>>(bit -   0)) & 0x01;
	else if(bit < 64)	return (pdat[1]>>(bit -  32)) & 0x01;
	else if(bit < 96)	return (pdat[2]>>(bit -  64)) & 0x01;
	else if(bit < 128)	return (pdat[3]>>(bit -  96)) & 0x01;
	else if(bit < 160)	return (pdat[4]>>(bit - 128)) & 0x01;
	else if(bit < 192)	return (pdat[5]>>(bit - 160)) & 0x01;
	else if(bit < 224)	return (pdat[6]>>(bit - 192)) & 0x01;
	else if(bit < 256)	return (pdat[7]>>(bit - 224)) & 0x01;
	else return 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_bit
// Brief	: 64BIT  --> 256BIT까지 확장
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK  
////////////////////////////////////////////////////////////////////////
int _dsp_get_bit(int bitno, pINT chk_err)
{
	int dat[8];
	
	if (bitno < 0 || bitno > 255)
	{
		*chk_err = MMC_ILLEGAL_IO;
		return 0;
	}

	*chk_err = MMC_OK;	
	
	_dsp_get_io_input(dat);
	
	return __get_bit(bitno, dat);
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_outbit
// Brief	: 64BIT  --> 256BIT까지 확장
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK  
////////////////////////////////////////////////////////////////////////
int _dsp_get_outbit(int bitno, pINT chk_err)	
{
	int dat[8];
	
	if (bitno < 0 || bitno > 255)
	{
		*chk_err = MMC_ILLEGAL_IO;
		return 0;
	}

	*chk_err = MMC_OK;	
	amc_error=_dsp_get_io_output(dat);  // 2102.07.01 syk ,_dsp_get_out64에서 amc_error전역 변수에 값을 넣는 것을 삭제함에 의해 이부분에 추가하였음 
	
	return __get_bit(bitno, dat);
}
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70))      // 120831 2.9.8 SYK 256BIT  INPUT 읽기 
////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_io_input
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_get_io_input(int *pdat)
{
    __GetIoInData_256(&pdat[0], &pdat[1], &pdat[2], &pdat[3], &pdat[4], &pdat[5], &pdat[6], &pdat[7]); 
}
#endif

////////////////////////////////////////////////////////////////////////
// Function	: WriteEncoder
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteEncoder(int ax, int pos)
{
	*ASIC_COUNT[ax] = (int)pos;
	pCBuf[ax]->encoder = pos;	// 2011.5.5. 인코더 노이즈 체크에서 이 변수와 실제 인코더 값이 달라 에러가 발생함.
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteFlashMemory
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
char WriteFlashMemory( char * flash_address, char data )
{
	char r_data, temp1, temp2;
	unsigned int status;
	int count = 0;
	
	if( FlashType == 0 )
	{
		*flash_address = 0x50;	// status reg. clear
		*flash_address = 0x40;
		*flash_address = data;
	
        do{
            *flash_address = 0x70;
            
			if( ++count>0xFFF ) 
			{
    			break;
    		}
    		
			status = *flash_address;
			
		}while( (unsigned int)0x80 != status );
	
		*flash_address = 0xFF;
		r_data = * flash_address;
	}
	else
	{
		// Write Command for Am29F160D
		*(char*)0xAAA = 0xAA;
		*(char*)0x555 = 0x55;
		*(char*)0xAAA = 0xA0;
		
		*flash_address = data;
		do{
			temp1 = (*flash_address) & 0x60;
			temp2 = (*flash_address) & 0x60;
			
			if( ++count>0xFFF )
			{
    			break;
    		}
		} while( temp1 != temp2 );
	
		// Reset Command for Am29F160D
		*(char*)0x900 = 0xF0;
		
		// Read Command
		r_data = * flash_address;
	}
	
	return r_data;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadServoAlarm
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadServoAlarm( int nzax )
{
	int data;
	
	data = *ptr_sys_sstat;
	data = data >> (16 + 4 * nzax);
	
	return data & 0x01;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadServoSRdy
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadServoSRdy( int nzax )
{
	int data;
	
	data = *ptr_sys_sstat;
	data = ~data >> (12 + nzax);
	
	return data & 0x01;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadSensor
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadSensor( int ax )
{
	int io, val;
	
	io = *ptr_sys_sstat;	    // Read Sensor
	io >>= ax;
	
	val = io & 0x01;		    // Home
	val |= (io >> 3) & 0x02;    // +Limit
	val |= (io >> 6) & 0x04;    // -Limit

	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteDAPort
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteDAPort( int ax, int val )
{
	int daval = val; // 최종 출력의 limit 제한을 위해 writeda를 호출하는 곳으로 위치 변경 // 여기 있는 경우 dac_bias의 값이 이상해 진 경우 limit제한이 안됨.

	daval += DAC_BASE;

	if (daval > 0xffff) 
	{
    	daval = 0xffff;
    }
	else if (daval < 0)
	{
    	daval = 0;
    }
	
	*DA_PORT_ADDR[ax] = daval;

#if defined(__AMC_V70)	
	pCBuf[ax]->dac_code_b = (pCBuf[ax]->dac_code * -1); 
#endif	
	pCBuf[ax]->dac_code = daval - DAC_BASE;
}

////////////////////////////////////////////////////////////////////////
// Function	: WritePulseCommand
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WritePulseCommand( int ax, int value, unsigned int attr, int r1, int r2, int upperCounter )
{
	// 2006.11.3, 현재는 펄스출력모드와 관계된 제어는 필요없으므로 주석처리함, AMC
	// *PULSE_CTRL[ax] = attr;
	*CLK_OUT_ADDR[ax] = (0x3FF&value) | attr;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadInPos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadInPos( int ax )
{
	int in_pos;
	
	in_pos = *ptr_sys_sstat;
	in_pos = in_pos >> (12 + ax);
	
	return ( in_pos & 1 );
}

////////////////////////////////////////////////////////////////////////
// Function	: GetAxisStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetAxisStatus(int ax)
{
	int in_pos;
	int uch;
	
	in_pos = *ptr_sys_sstat;
	
	// error code
	uch = (in_pos >> (4*(ax+3))) & 0xf0;
	
	// home
	uch |= (in_pos >> ax) & 0x01;
	
	// limit+
	uch |= (in_pos >> (ax + 3)) & 0x02;
	
	// limit-
	uch |= (in_pos >> (ax + 6)) & 0x04;
	
	// inpos
	uch |= (in_pos >> (ax + 9)) & 0x08; 
	
	return uch;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadChannel1
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
char ReadChannel1( char *addr, int offset)
{
	char val;
	volatile char *pc = (volatile char *) addr;
	
	val = pc[offset] & 0xFF;
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadChannel2
// Brief	: little_endian으로 저장되어 있어야 한다.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
short ReadChannel2( char *addr, int offset)
{
	volatile char *address;
	unsigned short val, i;  // 2011.10.13, 2.08.04를 개선하면서 다시 수정, 2.05.22에서 다시 수정하도록 한다.

	val = 0;
	address = addr + offset;
	
	for( i=0 ; i < 2 ; i++ )
	{
		val |= (*address & 0xFF) << (i * 8);
		address ++;
	}
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadChannel
// Brief	: little_endian으로 저장되어 있어야 한다.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	ReadChannel(char *addr, int offset)
{
	volatile char * address;
	int val, i;

	val = 0;
	address = addr + offset;
	
	for( i = 0 ; i < 4 ; i++ )
	{
		val |= (*address & 0xFF) << (i * 8);
		address ++;
	}
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteChannel1
// Brief	: 1바이트 억세스
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteChannel1(char *addr, int offset, char data)
{
	*(addr+offset) = data;
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteChannel2
// Brief	: 2바이트 억세스
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteChannel2(char *addr, int offset, short data)
{
	int i,j;
	
	for( i=0 ; i<2 ; i++ )
	{
		j = i* 8;   // 바이트 단위로 정렬하기 위함.
		*(addr+offset+i) = (data>>j);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteChannel
// Brief	: 4바이트 억세스
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteChannel(char *addr, int offset, int data)
{
	int i,j;
	
	for( i=0 ; i<4 ; i++ )
	{
		j = i*8;    // 바이트 단위로 정렬하기 위함.
		*(addr+offset+i) = (data>>j);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadMem1
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	ReadMem1(char *addr,int offset)
{
	int val;
	
	val = (*(addr+offset))&0xFF;
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadMem2
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	ReadMem2(char *addr,int offset)
{
	int temp,val,i,j;
	
	val = 0;
	temp = 0;

	for( i=0 ; i<2 ; i++ )
	{
		temp = (*(addr+offset+i)) & 0xFF;
		j = i*8;
		temp = temp<<j;
		val |= temp;
	}
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadMem
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	ReadMem(char *addr,int offset)
{
	int temp, val, i,j;
	
	val = 0;
	temp = 0;

	for( i=0 ; i<4 ; i++ )
	{
		temp = (*(addr+offset+i)) & 0xFF;
		j = i*8;
		temp = temp<<j;
		val |= temp;
	}
	
	return val;
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteMem1
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteMem1(volatile char *addr, int offset, int data)
{
	*(addr+offset) = data;
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteMem2
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteMem2(volatile char *addr, int offset, int data)
{
	int i,j;
	
	for( i=0 ; i<2 ; i++ )
	{
		j = i* 8;
		*(addr+offset+i) = (data>>j);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteMem
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WriteMem(volatile char *addr, int offset, int data)
{
	int i,j;
	
	for( i=0 ; i<4 ; i++ )
	{
		j = i*8;
		*(addr+offset+i) = (data>>j);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetIoOutData
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetIoOutData( int ld1, int ld2 )
{
	// 16비트씩 출력해야 하므로 4번에 걸쳐서 출력한다.
	*ptr_user_oport = (pGBuf->user_io_out = ld1) & 0xffff;
	*(ptr_user_oport2) = (ld1 >> 16) & 0xffff;
	*(ptr_user_oport3) = (pGBuf->user_io_out2 = ld2) & 0xffff;
	*(ptr_user_oport4) = (ld2 >> 16) & 0xffff;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_io64
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_io64(int port, int *pval)
{
	__SetIoOutData(pval[0], pval[1]);
	
	return 0;
}


#if defined(__AMC_29x)
////////////////////////////////////////////////////////////////////////
// Function	: __SetIoBit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetIoBit(int npos)
{
	if (npos < 32)
	{
		pGBuf->user_io_out |= 1 << (npos);
		__SetIoOutData(pGBuf->user_io_out, pGBuf->user_io_out2);
	}
	else
	{
		pGBuf->user_io_out2 |= 1 << (npos - 32);
		__SetIoOutData(pGBuf->user_io_out, pGBuf->user_io_out2);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __ResetIoBit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ResetIoBit(int npos)
{
	if (npos < 32)
	{
		pGBuf->user_io_out &= ~(1 << (npos));
		__SetIoOutData(pGBuf->user_io_out, pGBuf->user_io_out2);
	}
	else
	{
		pGBuf->user_io_out2 &= ~(1 << (npos - 32));
		__SetIoOutData(pGBuf->user_io_out, pGBuf->user_io_out2);
	}
}
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70))	// 120818 2.9.8 syk IO접점 추가로 인해 함수 변경
////////////////////////////////////////////////////////////////////////
// Function	: __SetIoBit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetIoBit(int npos)
{
	if (npos < 32)
	{
		pGBuf->user_io_out |= 1 << (npos);
		*ptr_user_oport = pGBuf->user_io_out & 0xffff;
		*(ptr_user_oport2) = (pGBuf->user_io_out >> 16) & 0xffff;		
	}
	else if(npos < 64)
	{
		pGBuf->user_io_out2 |= 1 << (npos-32);
		*(ptr_user_oport3) = pGBuf->user_io_out2 & 0xffff;
		*(ptr_user_oport4) = (pGBuf->user_io_out2 >> 16) & 0xffff;	
	}
	else if(npos < 96)
	{
		pGBuf->user_io_out3 |= 1 << (npos-64);
		*(ptr_user_oport5) = pGBuf->user_io_out3 & 0xffff;
		*(ptr_user_oport6) = (pGBuf->user_io_out3 >> 16) & 0xffff;		
	}
	else if(npos < 128)
	{
		pGBuf->user_io_out4 |= 1 << (npos-96);
		*(ptr_user_oport7) = pGBuf->user_io_out4 & 0xffff;
		*(ptr_user_oport8) = (pGBuf->user_io_out4 >> 16) & 0xffff;		
	}
	else if(npos < 160)
	{
		pGBuf->user_io_out5 |= 1 << (npos-128);
		*(ptr_user_oport9) = pGBuf->user_io_out5 & 0xffff;
		*(ptr_user_oport10) = (pGBuf->user_io_out5 >> 16) & 0xffff;			
	}
	else if(npos < 192)
	{
		pGBuf->user_io_out6 |= 1 << (npos-160);
		*(ptr_user_oport11) = pGBuf->user_io_out6 & 0xffff;
		*(ptr_user_oport12) = (pGBuf->user_io_out6 >> 16) & 0xffff;		
	}	
	else if(npos < 224)
	{
		pGBuf->user_io_out7 |= 1 << (npos-192);
		*(ptr_user_oport13) = pGBuf->user_io_out7 & 0xffff;
		*(ptr_user_oport14) = (pGBuf->user_io_out7 >> 16) & 0xffff;		
	}
	else if(npos < 256)
	{
		pGBuf->user_io_out8 |= 1 << (npos-224);
		*(ptr_user_oport15) = pGBuf->user_io_out8 & 0xffff;
		*(ptr_user_oport16) = (pGBuf->user_io_out8 >> 16) & 0xffff;		
	}
	else ;
}

////////////////////////////////////////////////////////////////////////
// Function	: __ResetIoBit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ResetIoBit(int npos)
{
	if (npos < 32)
	{
		pGBuf->user_io_out &= ~(1 << (npos));
		*ptr_user_oport = pGBuf->user_io_out & 0xffff;
		*(ptr_user_oport2) = (pGBuf->user_io_out >> 16) & 0xffff;		
	}
	else if(npos < 64)
	{
		pGBuf->user_io_out2 &= ~(1 << (npos-32));
		*(ptr_user_oport3) = pGBuf->user_io_out2 & 0xffff;
		*(ptr_user_oport4) = (pGBuf->user_io_out2 >> 16) & 0xffff;	
	}
	else if(npos < 96)
	{
		pGBuf->user_io_out3 &= ~(1 << (npos-64));
		*(ptr_user_oport5) = pGBuf->user_io_out3 & 0xffff;
		*(ptr_user_oport6) = (pGBuf->user_io_out3 >> 16) & 0xffff;		
	}
	else if(npos < 128)
	{
		pGBuf->user_io_out4 &= ~(1 << (npos-96));
		*(ptr_user_oport7) = pGBuf->user_io_out4 & 0xffff;
		*(ptr_user_oport8) = (pGBuf->user_io_out4 >> 16) & 0xffff;		
	}
	else if(npos < 160)
	{
		pGBuf->user_io_out5 &= ~(1 << (npos-128));
		*(ptr_user_oport9) = pGBuf->user_io_out5 & 0xffff;
		*(ptr_user_oport10) = (pGBuf->user_io_out5 >> 16) & 0xffff;			
	}
	else if(npos < 192)
	{
		pGBuf->user_io_out6 &= ~(1 << (npos-160));
		*(ptr_user_oport11) = pGBuf->user_io_out6 & 0xffff;
		*(ptr_user_oport12) = (pGBuf->user_io_out6 >> 16) & 0xffff;		
	}	
	else if(npos < 224)
	{
		pGBuf->user_io_out7 &= ~(1 << (npos-192));
		*(ptr_user_oport13) = pGBuf->user_io_out7 & 0xffff;
		*(ptr_user_oport14) = (pGBuf->user_io_out7 >> 16) & 0xffff;		
	}
	else if(npos < 256)
	{
		pGBuf->user_io_out8 &= ~(1 << (npos-224));
		*(ptr_user_oport15) = pGBuf->user_io_out8 & 0xffff;
		*(ptr_user_oport16) = (pGBuf->user_io_out8 >> 16) & 0xffff;		
	}
	else ;
} 

////////////////////////////////////////////////////////////////////////
// Function	: __SetIoCount
// Brief	: INPUT/OUTPUT PIN수 설정함수
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK
////////////////////////////////////////////////////////////////////////
void __SetIoCount(int ld1, int ld2)		  
{
	pCBuf[0]->VersatileIO = ld1;	// input io count
	pCBuf[1]->VersatileIO = ld2;	// output io count

	make_io_mask();	
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetIoOutData_256
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetIoOutData_256( int ld1, int ld2, int ld3, int ld4, int ld5, int ld6, int ld7, int ld8)	//120831 2.9.8 SYK 256BIT OUTPUT 출력 함수?
{
	// 16비트씩 출력해야 하므로 4번에 걸쳐서 출력한다.
	*ptr_user_oport = (pGBuf->user_io_out = ld1) & 0xffff;
	*(ptr_user_oport2) = (ld1 >> 16) & 0xffff;
	*(ptr_user_oport3) = (pGBuf->user_io_out2 = ld2) & 0xffff;
	*(ptr_user_oport4) = (ld2 >> 16) & 0xffff;
	
	*(ptr_user_oport5) = (pGBuf->user_io_out3 = ld3) & 0xffff;
	*(ptr_user_oport6) = (ld3 >> 16) & 0xffff;
	*(ptr_user_oport7) = (pGBuf->user_io_out4 = ld4) & 0xffff;
	*(ptr_user_oport8) = (ld4 >> 16) & 0xffff;
	
	*(ptr_user_oport9) = (pGBuf->user_io_out5 = ld5) & 0xffff;
	*(ptr_user_oport10) = (ld5 >> 16) & 0xffff;
	*(ptr_user_oport11) = (pGBuf->user_io_out6 = ld6) & 0xffff;
	*(ptr_user_oport12) = (ld6 >> 16) & 0xffff;
	
	*(ptr_user_oport13) = (pGBuf->user_io_out7 = ld7) & 0xffff;
	*(ptr_user_oport14) = (ld7 >> 16) & 0xffff;
	*(ptr_user_oport15) = (pGBuf->user_io_out8 = ld8) & 0xffff;
	*(ptr_user_oport16) = (ld8 >> 16) & 0xffff;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_io_output
// Brief	: DSP함수 : 256BIT OUTPUT 출력 함수					
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK 
////////////////////////////////////////////////////////////////////////
int _dsp_set_io_output(int port, int *pval)	
{
	__SetIoOutData_256(pval[0], pval[1], pval[2], pval[3], pval[4], pval[5], pval[6], pval[7]);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_io_count
// Brief	: DSP함수 : INPUT/OUTPUT PIN 설정 함수 
// Input	:
// Return	:
// History	:
// Date		: //120831 2.9.8 SYK 
////////////////////////////////////////////////////////////////////////
int _dsp_set_io_count(int in_c, int out_c)  
{
	if((in_c < 0) || (in_c > 256))
	{
    	return	MMC_ILLEGAL_IO;
    }
    
	if((out_c < 0) || (out_c > 256))	
	{
    	return	MMC_ILLEGAL_IO;		
    }
	
	__SetIoCount(in_c,out_c);	
	return 0;
}
#endif

////////////////////////////////////////////////////////////////////////
// Function	: _ReadServopackPosition
// Brief	: bEnAlready=1 => Incremental은 0으로 초기화, 절대치는 정상적으로 위치값 갱신.
//            bEnAlready=0 => Absolute만 ReLoad하고 인크리멘탈은 그냥 리턴한다.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _ReadServopackPosition(int ax, int nValidEncBitNo, int bEnAlready)
{
	// 4축의 카운트값을 읽어서 내부의 카운트 레지스터에 복사한다.
	int senmask[4] = { 0x0505000e, 0x0505000d, 0x0505000b, 0x05050007 };
	int i = ax;
	char str[200];
	int nval1, nval2, nval3, nval4, enc1_counter, aux_sen_output;
	char ctemp;
	unsigned int uiCurPos = 0;

	if (bEnAlready == 0)
	{
		if( (sen_output & (1 << ax)) ==0 )	
		{
			return;     // 인크리멘탈인 경우는 아래 절대치 읽는 절차를 수행할 필요가 없음.
		}
		
		_dsp_set_amp_enable(ax, FALSE);
		
		// Incremental인경우 복구시키기 위해서 현재 위치를 저장해 놓는다.
		uiCurPos = _REG32(ASIC_COUNT[i]);
		
		// 2010.2.10(오), 처음 초기화 부분이 아닌 동작 중에 절대치를 다시 읽는 경우
		// sen_output 출력값이 해당 비트만 0이 되어 다른 축은 카운트가 되지 않는다.
		// 이를 보완하기 위하여 이전 sen_output을 마지막에 출력한다.
		aux_sen_output = sen_output;
		
		// SEN stop	   
		// sen_output = 0x0505000f;//2010.2.11(오), 이전 sen_output 데이터를 이용해야 인크리멘탈의 경우 이전 값을 유지함.
		sen_output &= ~(1 << ax);
		_REG32(ptr_sys_sabs) = sen_output;
		Delay(100);
		
		// encoder clear;
		_REG32(ASIC_COUNT[i]) = 0;
		
		// SEN start
		sen_output |= (1 << ax);
		_REG32(ptr_sys_sabs) = sen_output;
		
		// encoder clear;
		_REG32(ASIC_COUNT[i]) = 0;
	
		// 이시간동안 서보팩의 위치정보가 수신된다.
		Delay(2000);
	}
	
	{
		nval1 = _REG32(ASIC_COUNT[i] + 1);	// 다회전 데이터 1
		nval2 = _REG32(ASIC_COUNT[i] + 2);	// 다회전 데이터 2
		
        // 2010.08.05 sghan 인터럽트 구조변경
        // *INTR_1MS_PULSE_ADDR = -1;		// 2009.9.17, 200uS 인터럽트 트리거 신호를 인가해야 1회전 데이터가 랫치됨.
		nval3 = _REG32(ASIC_COUNT[i]);		// 1회전 내의 데이터
		
		// 11비트만을 unsigned로 남겨 놓는다.
		// 2007.6.27, ckyu
        // nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);  // 2007.12.14(한), 엔코더 카운터 오동작으로 8191 이상의 값이 들어올수 있으므로 마스크를 없앤다.

		// 2008.1.7, ckyu
		// 아래의 형태로 코딩하지 않는경우 release 모드에서 실행시
		// 비교문에 의한 분기가 정상적으로 수행되지 않는경우가 발생한다.
		ctemp = ((nval1 >> 8) & 0xff);
		enc_sign = 1;
		
		if (ctemp == '-') 
		{
    		enc_sign = -1;
    	}

		if (ctemp != '+' && ctemp != '-')
		{
			// 2010.2.10(오), 처음 초기화 부분이 아닌 동작 중에 절대치를 다시 읽는 경우
			// sen_output 출력값이 해당 비트만 0이 되어 다른 축은 카운트가 되지 않는다.
			// 이를 보완하기 위하여 이전 sen_output을 마지막에 출력한다.
			if (bEnAlready == 0) 
			{
    			sen_output = aux_sen_output;
    		}

			sen_output &= senmask[i];
			
			_REG32(ptr_sys_sabs) = sen_output;  // 해당 축의 SEN 신호를 Low로 만들어 일반 인코더로 동작하도록
			
			// PwrOn 인경우 (bEnReady == 1)에는 0으로 초기화 하고
			// 그렇지 않은 초기화의 경우는 현재값을 다시 복구 시킨다.
			if (bEnAlready == 0) 
			{
				_REG32(ASIC_COUNT[i]) = uiCurPos;
			}
			else 
			{
				_REG32(ASIC_COUNT[i]) = 0;	//nval3;	//0
			}
			
			sprintf(str, "Incremental Motor #%d, %x, pos=%u\r\n", i, sen_output, uiCurPos);
			SERIAL_WriteBytes(str, strlen(str));
			return;
		}

		sprintf(str, "ABS Motor #%d(%08X, %08X, %08X), ", i, nval1, nval2, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// 회전수의 ASCII 코드에서 HEXA 데이터를 얻기 위한 계산식
		nval1 &= 0x7f7f0000;	// 8번째 비트인 패리트를 제거하고 7비트 코드로 
		enc1_counter = ((nval1>>16 & 0xff) - '0')*10000 + ((nval1>>24 & 0xff) - '0')*1000;
		
		nval2 &= 0x007f7f7f;	// 8번째 비트인 패리트를 제거하고 7비트 코드로 
		enc1_counter += ((nval2 & 0xff) - '0')*100 + ((nval2>>8 & 0xff) - '0')*10 + ((nval2>>16) - '0');
		
		// 17비트 인코더를 사용한 경우의 초기화
		// 부호포함 16비트로 변환한다.
		enc1_counter *= enc_sign;
		sprintf(str, "%d: %d turns + %d ", i+1, enc1_counter, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// 부호를 유지하도록 한 후
		// 부호포함 15비트로 변환하고
		// 최상위 비트로 올려 놓는다.
		// nval4 = enc1_counter / 2;
		// nval4 <<= 17;	// 최상위 15비트를 점유하도록 했다.
		
		// nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);
		// nval3 <<= (15 - nValidEncBitNo);
		
		// 야스카와 드라이버의 기본 설정이 1회전은 13비트, 회전수는 부호 있는 16비트
		// 부호포함 15비트로 변환하고
		// 최상위 비트로 올려 놓는다.
		nval4 = enc1_counter; // / 2;	// 회전수 데이터
		nval4 <<= (2 + nValidEncBitNo);	// 최상위 19비트를 점유하도록 했다.
		// sprintf(str, "shift=%08X(val3=%08X), ", nval4, nval3);
		// SERIAL_WriteBytes(str, strlen(str));
		
		if (_dsp_get_encoder_direction(i) == CIR_CW)
		{
			nval4 = -nval4 + nval3;
            // nval4 = nval4 + (8192-nval3);
        }
		else
		{
			nval4 = nval4 + nval3;
		}
		
		_REG32(ASIC_COUNT[i]) = nval4;

		sprintf(str, "= %d\r\n", nval4);
		SERIAL_WriteBytes(str, strlen(str));

		// 2007.12.14(오), 방향에 관계 없이 회전수를 동일하게 카운트하기 위해
        // int ccwcw = _dsp_get_encoder_direction(i);
        // SetEncoderHwDirection(i, ccwcw);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ResetEncoderTiming
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ResetEncoderTiming(int nValidEncBitNo)
{
	int i;
	
	// Motor Disable
	_REG32(ptr_sys_sctrl) = 0x00ff; //Active Low
	
	// SEN Stop
	// 2011.7.20 SEN 신호 출력의 경우 서보 팩에서 3초후에 받기 때문에 미리 출력이 나가면 에러 발생
	// 메인 루프에서 1.82초 정도의 지연이 있으니 1.5초면 충분하나 마진을 주기 위하여 2.4초를 추가하였음. 
	_REG32(ptr_sys_sabs) = 0x05050000;
	Delay(3300);//2470ms=3289

	// 2007.12.14(오), 방향에 관계 없이 회전수를 동일하게 카운트하기 위해
    // for (i = 0; i < AMC_AXIS; i ++)
    // {
    //      SetEncoderHwDirection(i, CIR_CCW);
    // }

	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// 인코더 카운터를 0으로 
	}

    // Yaskawa 모터의 절대치 구성 : P +/- D5 D4 D3 D2 D1(회전수 5바이트, 상위 디지트 먼저 출력) CR 의 8바이트로 구성
    // 데이터 구성은 7비트, 상위 1비트는 패리티, 짝수 패리트 사용
    // FPGA 레지스터 구성 : 
    // 1축 1회전 내의 인코더 카운터 : 0xA0000040
    // 1축 회전량 데이터 상위 : 0xA0000044 => D4 D5 +/- P
    // 1축 회전량 데이터 하위 : 0xA0000048 => CR D3 D2 D1
    // 회전량 데이터를 이용해서 인코더 카운터에 회전수를 계산하여 재설정한다.
    // Yaskawa 모터의 정회전은 부하측에서 보았을 때 CCW 방향임.

    // 카운터 값과 회전 방향 관계 : CW(부하측)로 돌면 카운터 다운, CCW이면 Up 카운트

    // 2008.5.27(오)
    // 인크리멘탈 인코더의 경우 노이즈가 유입되는 경우 절대치 인코더로 변경되면서 이상이 생길 수 있음.
    // 이를 보완하기 위하여 상위 28비트의 코드가 0x0505000인 경우에만 데이터가 Write되도록 보완하였음. 
   	// SEN start
	_REG32(ptr_sys_sabs) = 0x0505000f;
	sen_output = 0x0505000f;

    // 인크리멘탈 인코더를 추가하면서 필요한 루틴

	Delay(1);		// FPGA 내부 로직의 시간 지연(13uS) ?문에 필요//

    // 2007.12.14(한), SEN start 이전에 엔코더 값이 카운트 될수가 있으므로 다시 0으로 리셋 한다.(위의 엔코더 리셋은 필요없음)
	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// 인코더 카운터를 0으로 
	}	

	// 2007.6.25, ckyu
	// 수정전에는 1000 이었으나, ASIC_COUNT[i]값이 잘못 설정되는 것 같아
	// 지연시간을 2000으로 늘렸음.
	// 디버깅 목적
	
	Delay(2000);
	
	for (i = 0; i < MMC_AXIS; i ++)
    {
		_ReadServopackPosition(i, nValidEncBitNo, 1);
	}
}

#if (defined(__AMC_SMD) || defined(__AMC_V70)) // 20121204 SYK 2.11.2
////////////////////////////////////////////////////////////////////////
// Function	: _dsp_reload_servopack_all
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_reload_servopack_all()						
{
	_ReloadServopackTiming(11);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: _ReadServopackALLPosition
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _ReadServopackALLPosition(int ax, int nValidEncBitNo)
{
	// 4축의 카운트값을 읽어서 내부의 카운트 레지스터에 복사한다.
	int senmask[4] = { 0x0505000e, 0x0505000d, 0x0505000b, 0x05050007 };
	int i = ax;
	char str[200];
	int nval1, nval2, nval3, nval4, enc1_counter;
	char ctemp;
	
	{
		nval1 = _REG32(ASIC_COUNT[i] + 1);	// 다회전 데이터 1
		nval2 = _REG32(ASIC_COUNT[i] + 2);	// 다회전 데이터 2
        // 2010.08.05 sghan 인터럽트 구조변경
        // *INTR_1MS_PULSE_ADDR = -1;		// 2009.9.17, 200uS 인터럽트 트리거 신호를 인가해야 1회전 데이터가 랫치됨.
		nval3 = _REG32(ASIC_COUNT[i]);		// 1회전 내의 데이터
		
		// 11비트만을 unsigned로 남겨 놓는다.
		// 2007.6.27, ckyu
        // nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);	//2007.12.14(한), 엔코더 카운터 오동작으로 8191 이상의 값이 들어올수 있으므로 마스크를 없앤다.

		// 2008.1.7, ckyu
		// 아래의 형태로 코딩하지 않는경우 release 모드에서 실행시
		// 비교문에 의한 분기가 정상적으로 수행되지 않는경우가 발생한다.
		ctemp = ((nval1 >> 8) & 0xff);
		
		enc_sign = 1;
		if (ctemp == '-') 
		{
    		enc_sign = -1;
    	}

		if (ctemp != '+' && ctemp != '-')
		{
			sen_output &= senmask[i];
			_REG32(ptr_sys_sabs) = sen_output ; // 해당 축의 SEN 신호를 Low로 만들어 일반 인코더로 동작하도록
			
			// PwrOn 인경우 (bEnReady == 1)에는 0으로 초기화 하고
			// 그렇지 않은 초기화의 경우는 현재값을 다시 복구 시킨다.

			_REG32(ASIC_COUNT[i]) = ui_CurPos[i];

			sprintf(str, "Incremental Motor #%d, %x, pos=%u\r\n", i, sen_output, ui_CurPos[i]);
			SERIAL_WriteBytes(str, strlen(str));
			return;
		}

		sprintf(str, "ABS Motor #%d(%08X, %08X, %08X), ", i, nval1, nval2, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// 회전수의 ASCII 코드에서 HEXA 데이터를 얻기 위한 계산식
		nval1 &= 0x7f7f0000;	//8번째 비트인 패리트를 제거하고 7비트 코드로 
		enc1_counter = ((nval1>>16 & 0xff) - '0')*10000 + ((nval1>>24 & 0xff) - '0')*1000;
		
		nval2 &= 0x007f7f7f;	//8번째 비트인 패리트를 제거하고 7비트 코드로 
		enc1_counter += ((nval2 & 0xff) - '0')*100 + ((nval2>>8 & 0xff) - '0')*10 + ((nval2>>16) - '0');
		
		// 17비트 인코더를 사용한 경우의 초기화
		// 부호포함 16비트로 변환한다.
		enc1_counter *= enc_sign;
		sprintf(str, "%d: %d turns + %d ", i+1, enc1_counter, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// 부호를 유지하도록 한 후
		// 부호포함 15비트로 변환하고
		// 최상위 비트로 올려 놓는다.
		// nval4 = enc1_counter / 2;
		// nval4 <<= 17;	// 최상위 15비트를 점유하도록 했다.
		
		// nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);
		// nval3 <<= (15 - nValidEncBitNo);
		
		// 야스카와 드라이버의 기본 설정이 1회전은 13비트, 회전수는 부호 있는 16비트
		// 부호포함 15비트로 변환하고
		// 최상위 비트로 올려 놓는다.
		nval4 = enc1_counter; // / 2;	// 회전수 데이터
		nval4 <<= (2 + nValidEncBitNo);	// 최상위 19비트를 점유하도록 했다.
		// sprintf(str, "shift=%08X(val3=%08X), ", nval4, nval3);
		// SERIAL_WriteBytes(str, strlen(str));
		
		if (_dsp_get_encoder_direction(i) == CIR_CW)
		{
			nval4 = -nval4 + nval3;
            // nval4 = nval4 + (8192-nval3);
        }
		else
		{
			nval4 = nval4 + nval3;
		}
		// 현재 설정된 인코더의 분해능은 8192/회전 (0x1fff)
		// 인코더 카운터에 회전수를 추가한 새로운 값 로딩
		// 부호를 고려해서 추가할 것
		_REG32(ASIC_COUNT[i]) = nval4;

		sprintf(str, "= %d\r\n", nval4);
		SERIAL_WriteBytes(str, strlen(str));

		// 2007.12.14(오), 방향에 관계 없이 회전수를 동일하게 카운트하기 위해
        // int ccwcw = _dsp_get_encoder_direction(i);
        // SetEncoderHwDirection(i, ccwcw);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: _ReloadServopackTiming
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _ReloadServopackTiming(int nValidEncBitNo)
{
	int i;
	
	// Motor Disable
    // _REG32(ptr_sys_sctrl) = 0x00ff;  // Active Low

	for(i=0; i < MMC_AXIS; i++)
	{
		_dsp_set_amp_enable(i, FALSE);
		ui_CurPos[i]= _REG32(ASIC_COUNT[i]);
	}
		
	// SEN Stop
	// 2011.7.20 SEN 신호 출력의 경우 서보 팩에서 3초후에 받기 때문에 미리 출력이 나가면 에러 발생
	// 메인 루프에서 1.82초 정도의 지연이 있으니 1.5초면 충분하나 마진을 주기 위하여 2.4초를 추가하였음. 
	_REG32(ptr_sys_sabs) = 0x05050000;
	
	Delay(4000);    // 2470ms=3289

	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// 인코더 카운터를 0으로 
	}
	
    // Yaskawa 모터의 절대치 구성 : P +/- D5 D4 D3 D2 D1(회전수 5바이트, 상위 디지트 먼저 출력) CR 의 8바이트로 구성
    // 데이터 구성은 7비트, 상위 1비트는 패리티, 짝수 패리트 사용
    // FPGA 레지스터 구성 : 
    // 1축 1회전 내의 인코더 카운터 : 0xA0000040
    // 1축 회전량 데이터 상위 : 0xA0000044 => D4 D5 +/- P
    // 1축 회전량 데이터 하위 : 0xA0000048 => CR D3 D2 D1
    // 회전량 데이터를 이용해서 인코더 카운터에 회전수를 계산하여 재설정한다.
    // Yaskawa 모터의 정회전은 부하측에서 보았을 때 CCW 방향임.

    // 카운터 값과 회전 방향 관계 : CW(부하측)로 돌면 카운터 다운, CCW이면 Up 카운트

    // 2008.5.27(오)
    // 인크리멘탈 인코더의 경우 노이즈가 유입되는 경우 절대치 인코더로 변경되면서 이상이 생길 수 있음.
    // 이를 보완하기 위하여 상위 28비트의 코드가 0x0505000인 경우에만 데이터가 Write되도록 보완하였음. 
    // SEN start
	_REG32(ptr_sys_sabs) = 0x0505000f;
	sen_output = 0x0505000f;

    // 인크리멘탈 인코더를 추가하면서 필요한 루틴

	Delay(1);		// FPGA 내부 로직의 시간 지연(13uS) ?문에 필요//

    // 2007.12.14(한), SEN start 이전에 엔코더 값이 카운트 될수가 있으므로 다시 0으로 리셋 한다.(위의 엔코더 리셋은 필요없음)
	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// 인코더 카운터를 0으로 
	}	

	// 2007.6.25, ckyu
	// 수정전에는 1000 이었으나, ASIC_COUNT[i]값이 잘못 설정되는 것 같아
	// 지연시간을 2000으로 늘렸음.
	// 디버깅 목적
	Delay(2000);
	
	for (i = 0; i < MMC_AXIS; i ++)
	{
		_ReadServopackALLPosition(i, nValidEncBitNo);
		pCBuf[i]->bMustReboot =0;
	}		
}
#endif

