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
    
    // ���ڴ�ī��Ʈ�� �����ϴ� ��巹���� �Ҵ��Ѵ�.
    ASIC_COUNT[0] = (int *)0xa0000040;
    ASIC_COUNT[1] = (int *)0xa0000050;
    ASIC_COUNT[2] = (int *)0xa0000060;
    ASIC_COUNT[3] = (int *)0xa0000070;
    
    //-- �ӵ������ ������ ��巹���� �Ҵ��Ѵ�.
    DA_PORT_ADDR[0] = (int *) 0xa0000030;
    DA_PORT_ADDR[1] = (int *) 0xa0000034;
    DA_PORT_ADDR[2] = (int *) 0xa0000038;
    DA_PORT_ADDR[3] = (int *) 0xa000003c;
    
    //-- �޽� ����� �������� ��巹���� �Ҵ��Ѵ�.
    CLK_OUT_ADDR[0] = (int *) 0xa0000080;
    CLK_OUT_ADDR[1] = (int *) 0xa0000084;
    CLK_OUT_ADDR[2] = (int *) 0xa0000088;
    CLK_OUT_ADDR[3] = (int *) 0xa000008c;
    
    //-- �޽��� ����϶��� ��µ� �޽����� �о� ���µ� �̿��Ѵ�.
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
  
    //-- ����� �����Ʈ�� ����(64��Ʈ: 16��Ʈ * 4)
    ptr_user_oport = (int *) 0xa0300010;
    ptr_user_oport2 = (int *) 0xa0300014;
    ptr_user_oport3 = (int *) 0xa0300018;
    ptr_user_oport4 = (int *) 0xa030001c;
    
#if (defined(__AMC_SMD) || defined(__AMC_V70))
    //-- ����� �����Ʈ�� ����(64��Ʈ: 16��Ʈ * 4) // 120818 2.9.8 syk �߰� 
    ptr_user_oport5 = (int *) 0xa0300030;
    ptr_user_oport6 = (int *) 0xa0300034;
    ptr_user_oport7 = (int *) 0xa0300038;
    ptr_user_oport8 = (int *) 0xa030003c;
    
    //-- ����� �����Ʈ�� ����(64��Ʈ: 16��Ʈ * 4) // 120818 2.9.8 syk �߰� 
    ptr_user_oport9 = (int *) 0xa0300050;
    ptr_user_oport10 = (int *) 0xa0300054;
    ptr_user_oport11 = (int *) 0xa0300058;
    ptr_user_oport12 = (int *) 0xa030005c;
    
    //-- ����� �����Ʈ�� ����(64��Ʈ: 16��Ʈ * 4) // 120818 2.9.8 syk �߰� 
    ptr_user_oport13 = (int *) 0xa0300070;
    ptr_user_oport14 = (int *) 0xa0300074;
    ptr_user_oport15 = (int *) 0xa0300078;
    ptr_user_oport16 = (int *) 0xa030007c;
#endif

    //-- ����� �Է���Ʈ�� ����(64��Ʈ : 16��Ʈ * 4);
    ptr_user_iport = (int *) 0xa0300000;
    ptr_user_iport2 = (int *) 0xa0300004;
    ptr_user_iport3 = (int *) 0xa0300008;
    ptr_user_iport4 = (int *) 0xa030000c;
    
#if (defined(__AMC_SMD) || defined(__AMC_V70))
    //-- ����� �Է���Ʈ�� ����(64��Ʈ : 16��Ʈ * 4); // 120818 2.9.8 syk �߰� 
    ptr_user_iport5 = (int *) 0xa0300020;
    ptr_user_iport6 = (int *) 0xa0300024;
    ptr_user_iport7 = (int *) 0xa0300028;
    ptr_user_iport8 = (int *) 0xa030002c;
    
    //-- ����� �Է���Ʈ�� ����(64��Ʈ : 16��Ʈ * 4); // 120818 2.9.8 syk �߰� 
    ptr_user_iport9 = (int *) 0xa0300040;
    ptr_user_iport10 = (int *) 0xa0300044;
    ptr_user_iport11 = (int *) 0xa0300048;
    ptr_user_iport12 = (int *) 0xa030004c;
    
    //-- ����� �Է���Ʈ�� ����(64��Ʈ : 16��Ʈ * 4); // 120818 2.9.8 syk �߰� 
    ptr_user_iport13 = (int *) 0xa0300060;
    ptr_user_iport14 = (int *) 0xa0300064;
    ptr_user_iport15 = (int *) 0xa0300068;
    ptr_user_iport16 = (int *) 0xa030006c;
#endif
    
#if defined(__AMC_V70)
    //-- �ý��� ���� ���� ����͸� // 
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
    
    //-- ��������������� �Ҵ�
    ptr_sys_sctrl = (int *) 0xa0000010;    // Servo On/Off, Alarm Set/Reset
    ptr_sys_sstat = (int *) 0xa0000014;    // Servo ���� �б� 
    ptr_sys_sabs  = (int *) 0xa000001c;    // Servo ���� �б� 
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

	// �ܼ���Ʈ �ʱ�ȭ
	// port num = 2
	SERIAL_Init();
    SERIAL_SetPort(115200, 8, 0, 1);		
	SERIAL_Enable(TRUE);

	// ���̵弾����ȣ�� �����ϱ����� ������Ʈ���ʱ�ȭ �Ѵ�.
	// port num = 1;
    // 2007.11.23 �ø��� ��Ʈ ���ͷ�Ʈ�� ���� ���� �ӽ÷� ������.
    // SMR������ ����ؾ� �ϴ� ��Ʈ��.	
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
    // FPGA ���� ���濡 ���� ���ڴ��� �׻� 200us ���ͷ�Ʈ ��ƾ������ �д´�. FPGA 3.x ����
	// �ӵ� ������ �ʿ��� ī���� ���� �д´�.
    // pCBuf[ax]->TCounter = (int)(*MT_COUNTER[ax]);//���ӿ� �޽� �� ī���Ͱ� ����
    // pCBuf[ax]->SUM_TCounter = (int)(*(MT_COUNTER[ax] + 1));//M���� �޽��� ī������ ��ü �ð� 
    // pCBuf[ax]->MCounter = abs( (int)*(MT_COUNTER[ax] + 2) );//���� �ֱ⿡�� ī������ ���ڴ� �޽� ��

	int temp_cnt, encoder_cnt[3];
	int wrong_cnt = 0;

 	encoder_cnt[0] = *ASIC_COUNT[ax];
 	encoder_cnt[1] = *ASIC_COUNT[ax];
 	encoder_cnt[2] = *ASIC_COUNT[ax];


 	// ū ������ ���� �� �߰� ��̰��� ����
 	// 4500RPM, �ִ� 128.9�޽� �̵� ����
 	// 2.5.21 ���� ������ FPGA���� ���ڴ� ���� �� Glitch �߻���.
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
	// �� ������ ��?������ �ʱ�ȭ �Ѵ�.
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
	
	// ���� ��ȣ�� �ΰ��� �� ȸ���� �� ������ �о ������ ī���Ϳ� �����Ѵ�.
	
	ResetEncoderTiming(11);		// ���ڴ� 1ȸ�� ���� �������� ���ش� ����

    // Ȯ���ؾ� �� �ʱ�ȭ �κ�(��)
    // ȸ������ �޽��� ��� �ʱ�ȭ �Ѵ�.
    // �ΰ����� ��� �ʱ�ȭ�ϴ°��� ��Ȯ���� ���� Ȯ���Ͽ� �����ؾ� �Ѵ�.
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
	return (temp&0xFF); //Clear Int�� Echocommand ���� ���� return�߰�
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
	    // offset�� ����� ���� ����ϵ��� ���� ���. 2007.11.2
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
	
	// �Ʒ��� ����Ǿ���. 2007.12.3, ckyu
	// 4:200us, 5:1ms, 6:5/10ms, 7:DPRAM, 8:10ms , serialinterrupt.
	// Emul. Ser.  Dpram   5/10ms 1ms   200us
	// X01001 00100 00111   X00110 00101 00010
	// 0010 0100 1000 0111  0001 1000 1010 0010
	
    // MUXL = 0x248718a2;
    //	MUXH &= ~0xff000000;
    //	MUXH |= 0x1c000000;

    // 2011.5.5 sghan ���ͷ�Ʈ ��������
	SetVect(6, Control_Int200us);	// 1ms interrupt, FPGA���� DSP�� 200us �޽��� �޾� ����
	SetVect(7, Dpr_Int);		    // DPRAM Interrupt
	SetVect(8, AMC_Int1ms);		    // 5ms interrupt, FPGA���� DSP�� 200us �޽��� �޾� ����

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
// Brief	: ���� ���� 120228 syk ���ϰ��� error �ڵ� �и� 
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
// Brief	: ���� ���� 120228 syk ���ϰ��� error �ڵ� �и�
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
	
	amc_error=_dsp_get_out64(dat);  // 2102.07.01 syk ,_dsp_get_out64���� amc_error���� ������ ���� �ִ� ���� �����Կ� ���� �̺κп� �߰��Ͽ��� 
	
	return __get_bit(bitno, dat);
}
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70)) 		// 256BIT���� Ȯ��
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
// Brief	: 64BIT  --> 256BIT���� Ȯ��
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
// Brief	: 64BIT  --> 256BIT���� Ȯ��
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
	amc_error=_dsp_get_io_output(dat);  // 2102.07.01 syk ,_dsp_get_out64���� amc_error���� ������ ���� �ִ� ���� �����Կ� ���� �̺κп� �߰��Ͽ��� 
	
	return __get_bit(bitno, dat);
}
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70))      // 120831 2.9.8 SYK 256BIT  INPUT �б� 
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
	pCBuf[ax]->encoder = pos;	// 2011.5.5. ���ڴ� ������ üũ���� �� ������ ���� ���ڴ� ���� �޶� ������ �߻���.
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
	int daval = val; // ���� ����� limit ������ ���� writeda�� ȣ���ϴ� ������ ��ġ ���� // ���� �ִ� ��� dac_bias�� ���� �̻��� �� ��� limit������ �ȵ�.

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
	// 2006.11.3, ����� �޽���¸��� ����� ����� �ʿ�����Ƿ� �ּ�ó����, AMC
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
// Brief	: little_endian���� ����Ǿ� �־�� �Ѵ�.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
short ReadChannel2( char *addr, int offset)
{
	volatile char *address;
	unsigned short val, i;  // 2011.10.13, 2.08.04�� �����ϸ鼭 �ٽ� ����, 2.05.22���� �ٽ� �����ϵ��� �Ѵ�.

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
// Brief	: little_endian���� ����Ǿ� �־�� �Ѵ�.
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
// Brief	: 1����Ʈ �＼��
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
// Brief	: 2����Ʈ �＼��
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
		j = i* 8;   // ����Ʈ ������ �����ϱ� ����.
		*(addr+offset+i) = (data>>j);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: WriteChannel
// Brief	: 4����Ʈ �＼��
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
		j = i*8;    // ����Ʈ ������ �����ϱ� ����.
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
	// 16��Ʈ�� ����ؾ� �ϹǷ� 4���� ���ļ� ����Ѵ�.
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

#if (defined(__AMC_SMD) || defined(__AMC_V70))	// 120818 2.9.8 syk IO���� �߰��� ���� �Լ� ����
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
// Brief	: INPUT/OUTPUT PIN�� �����Լ�
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
void __SetIoOutData_256( int ld1, int ld2, int ld3, int ld4, int ld5, int ld6, int ld7, int ld8)	//120831 2.9.8 SYK 256BIT OUTPUT ��� �Լ�?
{
	// 16��Ʈ�� ����ؾ� �ϹǷ� 4���� ���ļ� ����Ѵ�.
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
// Brief	: DSP�Լ� : 256BIT OUTPUT ��� �Լ�					
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
// Brief	: DSP�Լ� : INPUT/OUTPUT PIN ���� �Լ� 
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
// Brief	: bEnAlready=1 => Incremental�� 0���� �ʱ�ȭ, ����ġ�� ���������� ��ġ�� ����.
//            bEnAlready=0 => Absolute�� ReLoad�ϰ� ��ũ����Ż�� �׳� �����Ѵ�.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _ReadServopackPosition(int ax, int nValidEncBitNo, int bEnAlready)
{
	// 4���� ī��Ʈ���� �о ������ ī��Ʈ �������Ϳ� �����Ѵ�.
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
			return;     // ��ũ����Ż�� ���� �Ʒ� ����ġ �д� ������ ������ �ʿ䰡 ����.
		}
		
		_dsp_set_amp_enable(ax, FALSE);
		
		// Incremental�ΰ�� ������Ű�� ���ؼ� ���� ��ġ�� ������ ���´�.
		uiCurPos = _REG32(ASIC_COUNT[i]);
		
		// 2010.2.10(��), ó�� �ʱ�ȭ �κ��� �ƴ� ���� �߿� ����ġ�� �ٽ� �д� ���
		// sen_output ��°��� �ش� ��Ʈ�� 0�� �Ǿ� �ٸ� ���� ī��Ʈ�� ���� �ʴ´�.
		// �̸� �����ϱ� ���Ͽ� ���� sen_output�� �������� ����Ѵ�.
		aux_sen_output = sen_output;
		
		// SEN stop	   
		// sen_output = 0x0505000f;//2010.2.11(��), ���� sen_output �����͸� �̿��ؾ� ��ũ����Ż�� ��� ���� ���� ������.
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
	
		// �̽ð����� �������� ��ġ������ ���ŵȴ�.
		Delay(2000);
	}
	
	{
		nval1 = _REG32(ASIC_COUNT[i] + 1);	// ��ȸ�� ������ 1
		nval2 = _REG32(ASIC_COUNT[i] + 2);	// ��ȸ�� ������ 2
		
        // 2010.08.05 sghan ���ͷ�Ʈ ��������
        // *INTR_1MS_PULSE_ADDR = -1;		// 2009.9.17, 200uS ���ͷ�Ʈ Ʈ���� ��ȣ�� �ΰ��ؾ� 1ȸ�� �����Ͱ� ��ġ��.
		nval3 = _REG32(ASIC_COUNT[i]);		// 1ȸ�� ���� ������
		
		// 11��Ʈ���� unsigned�� ���� ���´�.
		// 2007.6.27, ckyu
        // nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);  // 2007.12.14(��), ���ڴ� ī���� ���������� 8191 �̻��� ���� ���ü� �����Ƿ� ����ũ�� ���ش�.

		// 2008.1.7, ckyu
		// �Ʒ��� ���·� �ڵ����� �ʴ°�� release ��忡�� �����
		// �񱳹��� ���� �бⰡ ���������� ������� �ʴ°�찡 �߻��Ѵ�.
		ctemp = ((nval1 >> 8) & 0xff);
		enc_sign = 1;
		
		if (ctemp == '-') 
		{
    		enc_sign = -1;
    	}

		if (ctemp != '+' && ctemp != '-')
		{
			// 2010.2.10(��), ó�� �ʱ�ȭ �κ��� �ƴ� ���� �߿� ����ġ�� �ٽ� �д� ���
			// sen_output ��°��� �ش� ��Ʈ�� 0�� �Ǿ� �ٸ� ���� ī��Ʈ�� ���� �ʴ´�.
			// �̸� �����ϱ� ���Ͽ� ���� sen_output�� �������� ����Ѵ�.
			if (bEnAlready == 0) 
			{
    			sen_output = aux_sen_output;
    		}

			sen_output &= senmask[i];
			
			_REG32(ptr_sys_sabs) = sen_output;  // �ش� ���� SEN ��ȣ�� Low�� ����� �Ϲ� ���ڴ��� �����ϵ���
			
			// PwrOn �ΰ�� (bEnReady == 1)���� 0���� �ʱ�ȭ �ϰ�
			// �׷��� ���� �ʱ�ȭ�� ���� ���簪�� �ٽ� ���� ��Ų��.
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
		
		// ȸ������ ASCII �ڵ忡�� HEXA �����͸� ��� ���� ����
		nval1 &= 0x7f7f0000;	// 8��° ��Ʈ�� �и�Ʈ�� �����ϰ� 7��Ʈ �ڵ�� 
		enc1_counter = ((nval1>>16 & 0xff) - '0')*10000 + ((nval1>>24 & 0xff) - '0')*1000;
		
		nval2 &= 0x007f7f7f;	// 8��° ��Ʈ�� �и�Ʈ�� �����ϰ� 7��Ʈ �ڵ�� 
		enc1_counter += ((nval2 & 0xff) - '0')*100 + ((nval2>>8 & 0xff) - '0')*10 + ((nval2>>16) - '0');
		
		// 17��Ʈ ���ڴ��� ����� ����� �ʱ�ȭ
		// ��ȣ���� 16��Ʈ�� ��ȯ�Ѵ�.
		enc1_counter *= enc_sign;
		sprintf(str, "%d: %d turns + %d ", i+1, enc1_counter, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// ��ȣ�� �����ϵ��� �� ��
		// ��ȣ���� 15��Ʈ�� ��ȯ�ϰ�
		// �ֻ��� ��Ʈ�� �÷� ���´�.
		// nval4 = enc1_counter / 2;
		// nval4 <<= 17;	// �ֻ��� 15��Ʈ�� �����ϵ��� �ߴ�.
		
		// nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);
		// nval3 <<= (15 - nValidEncBitNo);
		
		// �߽�ī�� ����̹��� �⺻ ������ 1ȸ���� 13��Ʈ, ȸ������ ��ȣ �ִ� 16��Ʈ
		// ��ȣ���� 15��Ʈ�� ��ȯ�ϰ�
		// �ֻ��� ��Ʈ�� �÷� ���´�.
		nval4 = enc1_counter; // / 2;	// ȸ���� ������
		nval4 <<= (2 + nValidEncBitNo);	// �ֻ��� 19��Ʈ�� �����ϵ��� �ߴ�.
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

		// 2007.12.14(��), ���⿡ ���� ���� ȸ������ �����ϰ� ī��Ʈ�ϱ� ����
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
	// 2011.7.20 SEN ��ȣ ����� ��� ���� �ѿ��� 3���Ŀ� �ޱ� ������ �̸� ����� ������ ���� �߻�
	// ���� �������� 1.82�� ������ ������ ������ 1.5�ʸ� ����ϳ� ������ �ֱ� ���Ͽ� 2.4�ʸ� �߰��Ͽ���. 
	_REG32(ptr_sys_sabs) = 0x05050000;
	Delay(3300);//2470ms=3289

	// 2007.12.14(��), ���⿡ ���� ���� ȸ������ �����ϰ� ī��Ʈ�ϱ� ����
    // for (i = 0; i < AMC_AXIS; i ++)
    // {
    //      SetEncoderHwDirection(i, CIR_CCW);
    // }

	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// ���ڴ� ī���͸� 0���� 
	}

    // Yaskawa ������ ����ġ ���� : P +/- D5 D4 D3 D2 D1(ȸ���� 5����Ʈ, ���� ����Ʈ ���� ���) CR �� 8����Ʈ�� ����
    // ������ ������ 7��Ʈ, ���� 1��Ʈ�� �и�Ƽ, ¦�� �и�Ʈ ���
    // FPGA �������� ���� : 
    // 1�� 1ȸ�� ���� ���ڴ� ī���� : 0xA0000040
    // 1�� ȸ���� ������ ���� : 0xA0000044 => D4 D5 +/- P
    // 1�� ȸ���� ������ ���� : 0xA0000048 => CR D3 D2 D1
    // ȸ���� �����͸� �̿��ؼ� ���ڴ� ī���Ϳ� ȸ������ ����Ͽ� �缳���Ѵ�.
    // Yaskawa ������ ��ȸ���� ���������� ������ �� CCW ������.

    // ī���� ���� ȸ�� ���� ���� : CW(������)�� ���� ī���� �ٿ�, CCW�̸� Up ī��Ʈ

    // 2008.5.27(��)
    // ��ũ����Ż ���ڴ��� ��� ����� ���ԵǴ� ��� ����ġ ���ڴ��� ����Ǹ鼭 �̻��� ���� �� ����.
    // �̸� �����ϱ� ���Ͽ� ���� 28��Ʈ�� �ڵ尡 0x0505000�� ��쿡�� �����Ͱ� Write�ǵ��� �����Ͽ���. 
   	// SEN start
	_REG32(ptr_sys_sabs) = 0x0505000f;
	sen_output = 0x0505000f;

    // ��ũ����Ż ���ڴ��� �߰��ϸ鼭 �ʿ��� ��ƾ

	Delay(1);		// FPGA ���� ������ �ð� ����(13uS) ?���� �ʿ�//

    // 2007.12.14(��), SEN start ������ ���ڴ� ���� ī��Ʈ �ɼ��� �����Ƿ� �ٽ� 0���� ���� �Ѵ�.(���� ���ڴ� ������ �ʿ����)
	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// ���ڴ� ī���͸� 0���� 
	}	

	// 2007.6.25, ckyu
	// ���������� 1000 �̾�����, ASIC_COUNT[i]���� �߸� �����Ǵ� �� ����
	// �����ð��� 2000���� �÷���.
	// ����� ����
	
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
	// 4���� ī��Ʈ���� �о ������ ī��Ʈ �������Ϳ� �����Ѵ�.
	int senmask[4] = { 0x0505000e, 0x0505000d, 0x0505000b, 0x05050007 };
	int i = ax;
	char str[200];
	int nval1, nval2, nval3, nval4, enc1_counter;
	char ctemp;
	
	{
		nval1 = _REG32(ASIC_COUNT[i] + 1);	// ��ȸ�� ������ 1
		nval2 = _REG32(ASIC_COUNT[i] + 2);	// ��ȸ�� ������ 2
        // 2010.08.05 sghan ���ͷ�Ʈ ��������
        // *INTR_1MS_PULSE_ADDR = -1;		// 2009.9.17, 200uS ���ͷ�Ʈ Ʈ���� ��ȣ�� �ΰ��ؾ� 1ȸ�� �����Ͱ� ��ġ��.
		nval3 = _REG32(ASIC_COUNT[i]);		// 1ȸ�� ���� ������
		
		// 11��Ʈ���� unsigned�� ���� ���´�.
		// 2007.6.27, ckyu
        // nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);	//2007.12.14(��), ���ڴ� ī���� ���������� 8191 �̻��� ���� ���ü� �����Ƿ� ����ũ�� ���ش�.

		// 2008.1.7, ckyu
		// �Ʒ��� ���·� �ڵ����� �ʴ°�� release ��忡�� �����
		// �񱳹��� ���� �бⰡ ���������� ������� �ʴ°�찡 �߻��Ѵ�.
		ctemp = ((nval1 >> 8) & 0xff);
		
		enc_sign = 1;
		if (ctemp == '-') 
		{
    		enc_sign = -1;
    	}

		if (ctemp != '+' && ctemp != '-')
		{
			sen_output &= senmask[i];
			_REG32(ptr_sys_sabs) = sen_output ; // �ش� ���� SEN ��ȣ�� Low�� ����� �Ϲ� ���ڴ��� �����ϵ���
			
			// PwrOn �ΰ�� (bEnReady == 1)���� 0���� �ʱ�ȭ �ϰ�
			// �׷��� ���� �ʱ�ȭ�� ���� ���簪�� �ٽ� ���� ��Ų��.

			_REG32(ASIC_COUNT[i]) = ui_CurPos[i];

			sprintf(str, "Incremental Motor #%d, %x, pos=%u\r\n", i, sen_output, ui_CurPos[i]);
			SERIAL_WriteBytes(str, strlen(str));
			return;
		}

		sprintf(str, "ABS Motor #%d(%08X, %08X, %08X), ", i, nval1, nval2, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// ȸ������ ASCII �ڵ忡�� HEXA �����͸� ��� ���� ����
		nval1 &= 0x7f7f0000;	//8��° ��Ʈ�� �и�Ʈ�� �����ϰ� 7��Ʈ �ڵ�� 
		enc1_counter = ((nval1>>16 & 0xff) - '0')*10000 + ((nval1>>24 & 0xff) - '0')*1000;
		
		nval2 &= 0x007f7f7f;	//8��° ��Ʈ�� �и�Ʈ�� �����ϰ� 7��Ʈ �ڵ�� 
		enc1_counter += ((nval2 & 0xff) - '0')*100 + ((nval2>>8 & 0xff) - '0')*10 + ((nval2>>16) - '0');
		
		// 17��Ʈ ���ڴ��� ����� ����� �ʱ�ȭ
		// ��ȣ���� 16��Ʈ�� ��ȯ�Ѵ�.
		enc1_counter *= enc_sign;
		sprintf(str, "%d: %d turns + %d ", i+1, enc1_counter, nval3);
		SERIAL_WriteBytes(str, strlen(str));
		
		// ��ȣ�� �����ϵ��� �� ��
		// ��ȣ���� 15��Ʈ�� ��ȯ�ϰ�
		// �ֻ��� ��Ʈ�� �÷� ���´�.
		// nval4 = enc1_counter / 2;
		// nval4 <<= 17;	// �ֻ��� 15��Ʈ�� �����ϵ��� �ߴ�.
		
		// nval3 &= ((1 << (2 + nValidEncBitNo)) - 1);
		// nval3 <<= (15 - nValidEncBitNo);
		
		// �߽�ī�� ����̹��� �⺻ ������ 1ȸ���� 13��Ʈ, ȸ������ ��ȣ �ִ� 16��Ʈ
		// ��ȣ���� 15��Ʈ�� ��ȯ�ϰ�
		// �ֻ��� ��Ʈ�� �÷� ���´�.
		nval4 = enc1_counter; // / 2;	// ȸ���� ������
		nval4 <<= (2 + nValidEncBitNo);	// �ֻ��� 19��Ʈ�� �����ϵ��� �ߴ�.
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
		// ���� ������ ���ڴ��� ���ش��� 8192/ȸ�� (0x1fff)
		// ���ڴ� ī���Ϳ� ȸ������ �߰��� ���ο� �� �ε�
		// ��ȣ�� ����ؼ� �߰��� ��
		_REG32(ASIC_COUNT[i]) = nval4;

		sprintf(str, "= %d\r\n", nval4);
		SERIAL_WriteBytes(str, strlen(str));

		// 2007.12.14(��), ���⿡ ���� ���� ȸ������ �����ϰ� ī��Ʈ�ϱ� ����
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
	// 2011.7.20 SEN ��ȣ ����� ��� ���� �ѿ��� 3���Ŀ� �ޱ� ������ �̸� ����� ������ ���� �߻�
	// ���� �������� 1.82�� ������ ������ ������ 1.5�ʸ� ����ϳ� ������ �ֱ� ���Ͽ� 2.4�ʸ� �߰��Ͽ���. 
	_REG32(ptr_sys_sabs) = 0x05050000;
	
	Delay(4000);    // 2470ms=3289

	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// ���ڴ� ī���͸� 0���� 
	}
	
    // Yaskawa ������ ����ġ ���� : P +/- D5 D4 D3 D2 D1(ȸ���� 5����Ʈ, ���� ����Ʈ ���� ���) CR �� 8����Ʈ�� ����
    // ������ ������ 7��Ʈ, ���� 1��Ʈ�� �и�Ƽ, ¦�� �и�Ʈ ���
    // FPGA �������� ���� : 
    // 1�� 1ȸ�� ���� ���ڴ� ī���� : 0xA0000040
    // 1�� ȸ���� ������ ���� : 0xA0000044 => D4 D5 +/- P
    // 1�� ȸ���� ������ ���� : 0xA0000048 => CR D3 D2 D1
    // ȸ���� �����͸� �̿��ؼ� ���ڴ� ī���Ϳ� ȸ������ ����Ͽ� �缳���Ѵ�.
    // Yaskawa ������ ��ȸ���� ���������� ������ �� CCW ������.

    // ī���� ���� ȸ�� ���� ���� : CW(������)�� ���� ī���� �ٿ�, CCW�̸� Up ī��Ʈ

    // 2008.5.27(��)
    // ��ũ����Ż ���ڴ��� ��� ����� ���ԵǴ� ��� ����ġ ���ڴ��� ����Ǹ鼭 �̻��� ���� �� ����.
    // �̸� �����ϱ� ���Ͽ� ���� 28��Ʈ�� �ڵ尡 0x0505000�� ��쿡�� �����Ͱ� Write�ǵ��� �����Ͽ���. 
    // SEN start
	_REG32(ptr_sys_sabs) = 0x0505000f;
	sen_output = 0x0505000f;

    // ��ũ����Ż ���ڴ��� �߰��ϸ鼭 �ʿ��� ��ƾ

	Delay(1);		// FPGA ���� ������ �ð� ����(13uS) ?���� �ʿ�//

    // 2007.12.14(��), SEN start ������ ���ڴ� ���� ī��Ʈ �ɼ��� �����Ƿ� �ٽ� 0���� ���� �Ѵ�.(���� ���ڴ� ������ �ʿ����)
	for (i = 0; i < MMC_AXIS; i++)
	{
		_REG32(ASIC_COUNT[i]) = 0;		// ���ڴ� ī���͸� 0���� 
	}	

	// 2007.6.25, ckyu
	// ���������� 1000 �̾�����, ASIC_COUNT[i]���� �߸� �����Ǵ� �� ����
	// �����ð��� 2000���� �÷���.
	// ����� ����
	Delay(2000);
	
	for (i = 0; i < MMC_AXIS; i ++)
	{
		_ReadServopackALLPosition(i, nValidEncBitNo);
		pCBuf[i]->bMustReboot =0;
	}		
}
#endif

