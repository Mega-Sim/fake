#define AXISDPR_GLOBALS

#include "all.h"

extern void FSExchange();

#pragma CODE_SECTION(".user");
////////////////////////////////////////////////////////////////////////
// Function : AMCStatusGet
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
void InitServiceFunction()
{       
    int i;
    
    for(i = 0; i < MAX_COMM_NUM+1; i++)
    {
        Service[i] = NoOperation;
    }
   
    Service[  7] = GET_Position;
    Service[  8] = PUT_Curr_Pos;
    Service[  9] = PUT_Position;
    Service[ 10] = PUT_Error_Pls;
    
    Service[ 13] = GET_Error_Limit;
    Service[ 14] = PUT_Error_Limit;
    Service[ 19] = SET_IO_Bit;
    Service[ 20] = RESET_IO_Bit;
    
    Service[ 24] = Servo_On;
    Service[ 25] = Servo_Off;
    Service[ 26] = Alarm_Reset;
    Service[ 27] = Alarm_Set;
    Service[ 28] = Clear_Status;
    
    Service[ 51] = Put_Axis_Stat;
    Service[ 52] = Put_Axis_Source;
    Service[ 59] = Get_Dac_Offset;
    Service[ 60] = Put_Dac_Offset;
    
    Service[ 66] = Get_Frames_Clear;
    Service[ 69] = Get_Amp_On_Level;
    Service[ 70] = Put_Amp_On_Level;
    
    Service[ 76] = Get_SW_Upper_Limit;
    Service[ 77] = Put_SW_Upper_Limit;
    Service[ 78] = Get_SW_Lower_Limit;
    Service[ 79] = Put_SW_Lower_Limit;
    Service[ 80] = Put_IO_Out_Data;
    
    Service[ 89] = Get_SyncMap;
    Service[ 90] = Get_SyncOn;
    
    Service[ 91] = Get_SyncOff;
    Service[ 92] = Get_SyncGain;
    Service[ 93] = Put_SyncGain;
    Service[ 97] = Put_Encoder;
    Service[ 98] = Put_SyncEncoder;
    
    Service[111] = Exist_Check;
    Service[112] = Set_Oht_Model_id;
    Service[113] = Get_Oht_Model_id;    
    
    Service[140] = DPRAMTest;
	Service[141] = Put_CleanVoltage;
    
    Service[151] = SaveLongBlockToEeprom;
    Service[153] = UploadParamToIPC;
    Service[154] = DnloadParamFromIPC;
    Service[155] = RunConfigUploadToIPC;
    Service[156] = RunConfigSaveFromIPC;
    Service[157] = SetEncoderOffset;
    Service[158] = GetEncoderOffset;
    Service[159] = _dsp_param_flush;
    Service[160] = WatchdogOperations;
    
    Service[168] = GetMovexCmd;
    Service[169] = _dsp_clear_amc_error;
    Service[170] = _dsp_get_error_status;
    
    Service[171] = FSExchange;
    Service[174] = AMCStatusGet;
    Service[178] = TraceUpdate_1;
    Service[179] = TraceUpdate;
    Service[180] = GetAMCData70;
    
    Service[181] = ReloadEncoderPos;
    Service[182] = Put_Vel_Curve;
    Service[183] = Get_Vel_Curve;
    Service[184] = Put_Actvel_Margin;
    Service[185] = Get_Actvel_Margin;
	
    Service[186] = AMCStatusGet2;    
#if (defined(__AMC_SMD) || defined(__AMC_V70))		
    Service[187] = SetIoOutput;
    Service[188] = GetIoOutput;
    Service[189] = SetIOCount;
    Service[190] = GetIOCount;    
    
    Service[191] = ReloadServopackAll;        
#endif
#if defined(__AMC_V70)
    Service[192] = GetSystempStatus;
    Service[193] = SetSystemMoniEnable;
    Service[194] = SetSystemMoniDisable;
    Service[195] = GetSystemMoniEnable;
    Service[196] = GetSystemMoniValue;
    Service[197] = SetMoniteringThresholdPercent;
    Service[198] = GetMoniteringThresholdPercent;
    Service[200] = ResetPeakHoldValue;
    Service[201] = GetMinPeakHoldValue;
    Service[202] = GetMaxPeakHoldValue;
    Service[203] = SetDecelOnWD;
    Service[204] = GetDecelOnWD;
#endif
}

#pragma CODE_SECTION(".user");
////////////////////////////////////////////////////////////////////////
// Function	: Main_to_Dsp_Command
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetAMCData70()
{		
	static int Length;
	static char *DpramBuffer = (char *)0xb0001400;	
	
	Main_to_Dsp_Command(&DpramBuffer[16*12], sizeof(AMC_CMD), &DpramBuffer[16*12], &Length);	
}

#pragma CODE_SECTION(".user");
////////////////////////////////////////////////////////////////////////
// Function	: Dpr_Int
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
interrupt void Dpr_Int( void )
{
	int	i, axchk;

    // Amc Commfail ���� Debug Log �߰�
    if(g_pLD.nDpramCnt++ > 2000000000)
    {
    	g_pLD.nDpramCnt = 0;	
    }
    Dpram_Log = SetLog(Dpram_Log, 11, true);  
    WriteLogData(Dpram_Log);

    // DPR Int Clear Enable
    if( g_nUseDprInt_AutoClear == 0)
    {
    	if(++g_nUseDprInt_AutoEnableCnt > MAX_DPRINT_ENABLE_COUNT)
    	{
    		// g_nUseDprInt_AutoClear�� ���� ���·� Dpr_Int�� MAX_DPRINT_ENABLE_COUNT�̻� ȣ��Ǿ��ٸ�
    		// ������ ���� �����̹Ƿ� �ٽ� Main�� DPram Interrupt�� ����͸� ���� AutoClear�Լ��� ������Ų��.
    		g_nUseDprInt_AutoClear = 1;
    		g_nUseDprInt_AutoEnableCnt = 0;
    		g_nDprIntClearRetryCnt = 0; //
    		
    	}
    }

	i = ReadCommand();

	if( i==0 || i>MAX_COMM_NUM)
	{
        g_pLD.cmd_index = i;
        WriteLogData(Dpram_Log);
        gbNewCmdArrived = TRUE;
        gnNewCmdIndex = -1;
		return;
	}
	
	axchk = ReadCommandAxis();
	
	gnNewCmdAxis = axchk;
	gnNewCmdIndex = i;
	gbNewCmdArrived = TRUE;
	
	sDbgCntBuff.loop_DPRAM++;

    // Amc Commfail ���� Debug Log �߰�
    Dpram_Log = SetLog(Dpram_Log, 11, false);  
    WriteLogData(Dpram_Log);
}

#pragma CODE_SECTION(".user");
////////////////////////////////////////////////////////////////////////
// Function	: DoCmdIfExist
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void DoCmdIfExist()
{
	if (gbNewCmdArrived == TRUE)
	{
		gbNewCmdArrived = FALSE;
		
		if ((gnNewCmdIndex>=0 && gnNewCmdIndex<=MAX_COMM_NUM) && (gnNewCmdAxis >= 0 && gnNewCmdAxis < JPC_AXIS))
		{
			(*Service[gnNewCmdIndex])();
		}

		EchoCommand( gnNewCmdIndex );
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: NoOperation
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void NoOperation(void)
{
	int	ax;

	ax = ReadCommandAxis();
	
	WriteChannel1( AxisDPRAM[ax], AD_In_Sequence, (pCBuf[ax]->in_sequence=1) );//3
}

////////////////////////////////////////////////////////////////////////
// Function	: SPL_Line_Move
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SPL_Line_Move(void)
{
}

////////////////////////////////////////////////////////////////////////
// Function	: SPL_Arc_Move
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SPL_Arc_Move(void)
{
}
////////////////////////////////////////////////////////////////////////
// Function	: SPL_Move
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SPL_Move( void )
{
}

////////////////////////////////////////////////////////////////////////
// Function	: TraceUpdate
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void TraceUpdate_1(void)
{
	int* DPRAMBUF = (int *)0xb00014C0;
	int i, j, ax;

	Trace_Update_Status = true;

	for( j=0 ; j<JPC_AXIS ; j++ )
	{
		trace_on_flag[j] = 0;
	}			

	ax = ReadCommandAxis();
		
	for(i=0 ; i<(Trace_Data_Transfer_Buffer_Size_C-1) ; i++)
	{
		if(!MQIsEmpty(MAxisQueue[ax]))
		{
			DPRAMBUF[i] = MDequeue(MAxisQueue[ax]);			
		} 
		else
		{
			break;
		}
	}
	
	DPRAMBUF[Trace_Data_Transfer_Buffer_Size_C-1] = i;	

	if(  i  != (Trace_Data_Transfer_Buffer_Size_C-1) )
	{
		Trace_Update_Status = false;
		
		for( j=0 ; j<JPC_AXIS ; j++ )
		{
			trace_on_flag[j] = 0;
		}	
	}	
}

////////////////////////////////////////////////////////////////////////
// Function	: TraceUpdate
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void TraceUpdate(void)
{
	int* DPRAMBUF = (int *)0xb00014C0;
	int i, j, ax;

	Trace_Update_Status = true;
	
	for( j=0 ; j<JPC_AXIS ; j++ )
	{
		trace_on_flag[j] = 0;
	}		

	ax = ReadCommandAxis();
		
	for(i=0 ; i<(Trace_Data_Transfer_Buffer_Size_M-1) ; i++)
	{
		if(!QIsEmpty(AxisQueue[ax]))
		{
			DPRAMBUF[i] = Dequeue(AxisQueue[ax]);			
		}
		else
		{
			break;
		}
	}
	
	DPRAMBUF[Trace_Data_Transfer_Buffer_Size_M-1] = i;	

	if(  i  != (Trace_Data_Transfer_Buffer_Size_M-1) )
	{
		Trace_Update_Status = false;
		
		for( j=0 ; j<JPC_AXIS ; j++ )
		{
			trace_on_flag[j] = 0;
		}						
	}
}
	
////////////////////////////////////////////////////////////////////////
// Function	: GET_Position
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GET_Position(void)
{
	int	ax, pos;

	ax = ReadCommandAxis();
	pos = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	
	if(pCBuf[ax]->g_on != 0 )
	{
		__SetPosition(ax, pos);

		if((ax == 0) && (pos == 0))
		{
			Final_Arrival_Flag = 0;
			
	 		AMC_Stop_count = 0;

			OHT_DETECT_N_VALUE_Flag = false;
			OHT_Detect_AreaSetting_Flag = false;
			OHT_Detect_AreaSetting_Flag2 = false;
			
			PauseFlag = PAUSED_NONE;

			PBS_Ignore_Flag = false;
			OHT_PBS_IgnoreSettingFlag = false;

			Follows_MoveDS_Flag = false;
	 		Follow_Enable_Flag = false;		
			
			Rear_servo_Off_flag = false;
			MoveDS_Wait_Count_After_Servo_On_Flag = false;			
			MoveDS_Wait_Count_After_Servo_On = 0;		
		}
	}	
}

////////////////////////////////////////////////////////////////////////
// Function	: PUT_Curr_Pos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PUT_Curr_Pos(void)
{
	int	ax, lpos;
	
	ax = ReadCommandAxis();
    lpos = __GetCurPos(ax);

	WriteChannel(AxisDPRAM[ax],AD_LongType1, lpos);
}

////////////////////////////////////////////////////////////////////////
// Function	: 
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PUT_Position(void)
{
	int	ax, ipos;
	
	ax = ReadCommandAxis();
	ipos = __GetPos(ax);

	WriteChannel(AxisDPRAM[ax],AD_LongType1, ipos);
}

////////////////////////////////////////////////////////////////////////
// Function	: 
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PUT_Error_Pls(void)
{
	int	ax;
	double ferr;
	
	ax = ReadCommandAxis();

	_dsp_get_error(	ax, &ferr);
	
	WriteChannel(AxisDPRAM[ax],AD_FloatType1,TMS2IEEE(ferr));
}

////////////////////////////////////////////////////////////////////////
// Function	: 
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GET_Error_Limit(void)
{
	int	ax, lchk;
	char cst;

	ax = ReadCommandAxis();
	lchk = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	cst = ReadChannel1(AxisDPRAM[ax],AD_CharType1);

	__SetErrorLimit(ax, lchk, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: PUT_Error_Limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PUT_Error_Limit(void)
{
	int	ax,lchk;
	char cst;
	
	ax = ReadCommandAxis();
	__GetErrorLimit(ax, &lchk, &cst);

	WriteChannel(AxisDPRAM[ax],AD_LongType1, lchk);
	WriteChannel1(AxisDPRAM[ax],AD_CharType1, cst);
}

#if (defined(__AMC_SMD) || defined(__AMC_V70))
////////////////////////////////////////////////////////////////////////
// Function	: SetIoOutput
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetIoOutput(void)	
{
	int	ld1, ld2, ld3, ld4, ld5, ld6, ld7, ld8;
	
	ld1 = ReadChannel(CommDPRAM, CD_Io_outValue);
	ld2 = ReadChannel(CommDPRAM, CD_Io_outValue2);
	ld3 = ReadChannel(CommDPRAM, CD_Io_outValue3);
	ld4 = ReadChannel(CommDPRAM, CD_Io_outValue4);
	ld5 = ReadChannel(CommDPRAM, CD_Io_outValue5);
	ld6 = ReadChannel(CommDPRAM, CD_Io_outValue6);
	ld7 = ReadChannel(CommDPRAM, CD_Io_outValue7);
	ld8 = ReadChannel(CommDPRAM, CD_Io_outValue8);
		
	__SetIoOutData_256(ld1, ld2, ld3, ld4, ld5, ld6, ld7, ld8);	
}

////////////////////////////////////////////////////////////////////////
// Function	: SetIOCount
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetIOCount(void)
{
	int	ld1, ld2;
	
	ld1 = ReadChannel(CommDPRAM, CD_Io_outValue7);
	ld2 = ReadChannel(CommDPRAM, CD_Io_outValue8);
	
	__SetIoCount(ld1, ld2);	
}

////////////////////////////////////////////////////////////////////////
// Function	: GetIoOutput
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetIoOutput(void)
{
	int dat[8];
	
	amc_error= _dsp_get_io_output(dat);	
	
	WriteChannel(CommDPRAM, CD_Io_outValue, dat[0] );
	WriteChannel(CommDPRAM, CD_Io_outValue2, dat[1] );
	WriteChannel(CommDPRAM, CD_Io_outValue3, dat[2] );
	WriteChannel(CommDPRAM, CD_Io_outValue4, dat[3] );
	WriteChannel(CommDPRAM, CD_Io_outValue5, dat[4] );			
	WriteChannel(CommDPRAM, CD_Io_outValue6, dat[5] );
	WriteChannel(CommDPRAM, CD_Io_outValue7, dat[6] );
	WriteChannel(CommDPRAM, CD_Io_outValue8, dat[7] );			
}

////////////////////////////////////////////////////////////////////////
// Function	: GetIOCount
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetIOCount(void)	
{
	int dat1,dat2;
	
	amc_error=  _dsp_get_io_count(&dat1, &dat2);
	
	WriteChannel(CommDPRAM, CD_Io_outValue7, dat1 );
	WriteChannel(CommDPRAM, CD_Io_outValue8, dat2 );	
}

////////////////////////////////////////////////////////////////////////
// Function	: make_io_mask
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void make_io_mask()		
{	
	int io_count;

	for(io_count =0; io_count< 8; io_count++)
	{
		in_mask[io_count]= 0;	
		out_mask[io_count]=	0;		
	}
	
 	for(io_count =0; io_count < pCBuf[0]->VersatileIO; io_count++)	// input io
 	{
 		in_mask[io_count/32] |= (1<< (io_count%32));
 	}

 	for(io_count =0; io_count< pCBuf[1]->VersatileIO; io_count++)	// output io
 	{
 		out_mask[io_count/32] |= (1<< (io_count%32));
 	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ReloadServopackAll
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ReloadServopackAll()
{
	_ReloadServopackTiming(11);
}
#endif

////////////////////////////////////////////////////////////////////////
// Function	: SET_IO_Bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SET_IO_Bit(void)
{
	int bit;
	
	bit = ReadChannel(CommDPRAM, CD_Io_outValue);
	
	_dsp_set_bit(bit);
}

////////////////////////////////////////////////////////////////////////
// Function	: RESET_IO_Bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void RESET_IO_Bit(void)
{
	int bit;
	
	bit = ReadChannel(CommDPRAM, CD_Io_outValue);
	
	_dsp_reset_bit(bit);
}

////////////////////////////////////////////////////////////////////////
// Function	: Servo_On
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Servo_On(void)
{
	int	ax;

	ax = ReadCommandAxis();

	_dsp_set_amp_enable(ax, TRUE);
}

////////////////////////////////////////////////////////////////////////
// Function	: Servo_Off
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Servo_Off(void)
{
	int	ax;

	ax = ReadCommandAxis();
	
	_dsp_set_amp_enable(ax, FALSE);
}

#if defined(__AMC_V70)
////////////////////////////////////////////////////////////////////////
// Function	: SetSystemMoniEnable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetSystemMoniEnable(void)
{
	int	ax;
	char axno;
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	
	__system_monitering_enable(axno, TRUE);	
}

////////////////////////////////////////////////////////////////////////
// Function	: SetSystemMoniDisable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetSystemMoniDisable(void)
{
	int	ax;
	char axno;
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	
	__system_monitering_enable(axno, FALSE);	
}

////////////////////////////////////////////////////////////////////////
// Function	: SetMoniteringThresholdPercent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetMoniteringThresholdPercent(void)
{
	int	ax;
	char axno,percent;
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	percent	= ReadChannel1(AxisDPRAM[ax], AD_CharType1);
	
	__s_monitering_threshold_percent(axno, percent);
}

////////////////////////////////////////////////////////////////////////
// Function	: GetMoniteringThresholdPercent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetMoniteringThresholdPercent(void)
{
	int	ax;
	char axno,percent;
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	__g_monitering_threshold_percent(axno, &percent);
	
	WriteChannel1(AxisDPRAM[ax],AD_CharType1, percent);	
}

////////////////////////////////////////////////////////////////////////
// Function	: GetSystemMoniValue
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetSystemMoniValue(void)
{
	int	ax, value, fpga_data, dac_data;
	char axno;
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	
	__g_system_monitering_value(axno, &value, &fpga_data, &dac_data);
	
	WriteChannel(AxisDPRAM[ax],AD_LongType1,value);	
	WriteChannel2(AxisDPRAM[ax],AD_IntType4,(INT2)fpga_data);	
	WriteChannel2(AxisDPRAM[ax],AD_IntType5,(INT2)dac_data);			
}

////////////////////////////////////////////////////////////////////////
// Function	: GetSystempStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetSystempStatus(void)
{
	int	ax;
	char status;
	
	ax = ReadCommandAxis();
	
	_dsp_system_status(&status);
	
	WriteChannel1(AxisDPRAM[ax],AD_CharType2, status);
}

////////////////////////////////////////////////////////////////////////
// Function	: GetSystemMoniEnable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetSystemMoniEnable(void)
{
	int ax = 1;
	char moni_status;
	
	_dsp_get_system_moni_enable(&moni_status);
	
	WriteChannel1(AxisDPRAM[ax],AD_CharType2, (char)moni_status);
}

// �߰��� on 2018.05.23 --> ���� ����͸� ���� �� ���� ���� & WatchDog �߻� �� Decel Stop Ratio ��� �߰�.
/* // FPGA �Լ��� ����.
40401	0XA010 1004	"Read : ���� ���� �̻� ���� Status
   ���� �̻��� ���� �Ǹ� '1'�� Set
Write '0' : Status Clear"	"

bit0 = '1' : 3.3V ���� �̻�� �߻�, '0' : ����
bit1 = '1' : 5V ���� �̻�� �߻�, '0' : ����
bit2 = '1' : +12V ���� �̻�� �߻�, '0' : ����
bit3 = '1' : -12V ���� �̻�� �߻�, '0' : ����"	R/W	0000
*/

/*
[31:16] Max Peak :  unsigned short (0x0000 ~ 0xFFFF : 0 ~ 65536 : -12 ~ +12V)
[15:0] Min Peak :  unsigned short (0x0000 ~ 0xFFFF : 0 ~ 65536 : -12 ~ +12V)
- "0" �� Write �ϸ� Clear ��	R/C

40410	0XA010 1040	Max / Min Peak Hold 3.3V ����
40411	0XA010 1044	Max / Min Peak Hold 5V ����
40412	0XA010 1048	Max / Min Peak Hold +12V ����
40413	0XA010 104C	Max / Min Peak Hold -12V ����
*/

void ResetPeakHoldValue(void)	// 200  --> 3.3V, 5V, +12V, -12V ���� Reset ����.
{
	int	ax;
	char axno; // ���⼭�� ���ȣ�� �ƴ�, Volt Type�� ����.
	int value = 0; // 0�� write �ϸ� Clear ��.

	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);

	__s_system_monitering_peak_hold_value(axno, value); // clear --> min:FALSE, max:TRUE
}

void GetMinPeakHoldValue(void)	// 201  --> 3.3V, 5V, +12V, -12V ������ ���� ���ŵ� �ִ� Peak Hold Voltage, raw, dac �� ��ȯ
{
	int	ax;
	char axno; // ���⼭�� ���ȣ�� �ƴ�, Volt Type�� ����.
	int value; // value
	int fpga_data; // raw

	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);

	__g_system_monitering_peak_hold_value(axno, &value, &fpga_data, FALSE); // min:FALSE, max:TRUE

	WriteChannel(AxisDPRAM[ax],AD_LongType1,value);
	WriteChannel2(AxisDPRAM[ax],AD_IntType4,(INT2)fpga_data);
}



void GetMaxPeakHoldValue(void)	// 202  --> 3.3V, 5V, +12V, -12V ������ ���� ���ŵ� �ִ� Peak Hold Voltage, raw, dac �� ��ȯ
{
	int	ax;
	char axno; // ���⼭�� ���ȣ�� �ƴ�, Volt Type�� ����.
	int value; // value
	int fpga_data; // raw

	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);

	__g_system_monitering_peak_hold_value(axno, &value, &fpga_data, TRUE); // min:FALSE, max:TRUE

	WriteChannel(AxisDPRAM[ax],AD_LongType1,value);
	WriteChannel2(AxisDPRAM[ax],AD_IntType4,(INT2)fpga_data);
}

/*
'- Watchdog �߻��� �ް��� �� �ڿ� Servo Off �ϴ� ���
16bit unsigned : 3.2ms(200us*16) ���� ������ ��(dac�� ����)�� �߿���� ȯ���Ͽ� �Է��ؾ���
���� �Ϸ� �Ŀ� SERVO OFF ��
-- 3000rpm = 6(V) = 10097(DAC)
-- �ִ�ӵ� 10097 �϶� 3.2ms ���� 33�� ������ �ϸ� 306Cycle �ɸ�
-- 306 Cycle * 3.2ms = 979.2ms (���� ���� �ð�)
-- ""0"" �� Write �ϸ� ���� ��� ���������� ���� �� (���� ���� �ٷ� Servo Off ���)"	R/W (Read �Ǵ� Write ��������)

4000C	0XA010 0030	Axis1 Watchdog ESTOP ���Ӻ�	"[7 : 0] : unsigned char
4000D	0XA010 0034	Axis2 Watchdog ESTOP ���Ӻ�
4000E	0XA010 0038	Axis3 Watchdog ESTOP ���Ӻ�
4000F	0XA010 003C	Axis4 Watchdog ESTOP ���Ӻ�
*/
void SetDecelOnWD(void)	// 203  --> Ratio = 0 �� �� �ش� ��� ��� ����(Disable)
{
	int ax;
	char axno;
	float fDecelRatio;
	double fFpgaDecelRatio = 0.0; // ����Ʈ�� 0(���� ����. Servo OFF�� ����)
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	fDecelRatio = IEEE2TMS(ReadChannel(AxisDPRAM[ax],AD_FloatType1)); // ���� : float ���� IEEE2TMS ��ȯ�ؾ� ��.
	
	// (char)fFpgaDecelRatio = [0..255]
	fFpgaDecelRatio = (double)fDecelRatio * (double)((double)10097.0/(double)3.5) * (double)((double)16.0/(double)5000.0); // 10097(6V), 3.5(�ִ� �ӵ� : Meter/Sec^2), 16(���ش� ���), 5000(200us) --> nDecelRatio=3.5(M/S^2) �϶� �� 33 �� ��.
		
	__s_system_decelstop_ratio(axno, fFpgaDecelRatio);
}

void GetDecelOnWD(void)		// 204 --> �ະ�� ������ Wd Decel Ratio ��ȯ.
{
	int	ax;
	char axno;
	float fDecelRatio;
	double fFpgaDecelRatio = 0.0; //dac�� �ִ� ��33.0(3.5M/S^2����) [0..255]
	
	ax = ReadCommandAxis();
	axno = ReadChannel1(AxisDPRAM[ax], AD_CharType2);
	__g_system_decelstop_ratio(axno, &fFpgaDecelRatio);

	fDecelRatio = (float)(fFpgaDecelRatio * (double)((double)3.5/(double)10097.0) * (double)((double)5000.0/(double)16.0));	
	WriteChannel(AxisDPRAM[ax],AD_FloatType1, TMS2IEEE(fDecelRatio));	
}

void SetDecelOnWD_By_AMC(int ax, float fDecelOnWd)
{
	double fFpgaDecelRatio = 0.0; // ����Ʈ�� 0(���� ����. Servo OFF�� ����)
	fFpgaDecelRatio = (double)fDecelOnWd * (double)(10097.0/3.5) * (double)(16.0/5000.0); // 10097(6V), 3.5(�ִ� �ӵ� : Meter/Sec^2), 16(���ش� ���), 5000(200us) --> nDecelRatio=3.5(M/S^2) �϶� �� 33 �� ��.
	__s_system_decelstop_ratio(ax, fFpgaDecelRatio);	
}

void ResetPeakHoldValue_By_AMC(void)	
{
	for(int i = 0; i<4; i++)
	{
		// Min/Max Voltage Reset
		__s_system_monitering_peak_hold_value(i, 0); // clear --> min:FALSE, max:TRUE
	}
}

#endif

////////////////////////////////////////////////////////////////////////
// Function	: Alarm_Reset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Alarm_Reset(void)
{
	int	ax;
	
	ax = ReadCommandAxis();
	
	_dsp_amp_fault_reset(ax);
}

////////////////////////////////////////////////////////////////////////
// Function	: Alarm_Set
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Alarm_Set(void)
{
	int	ax;
	
	ax = ReadCommandAxis();
	
	_dsp_amp_fault_set(ax);
}

////////////////////////////////////////////////////////////////////////
// Function	: Clear_Status
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Clear_Status(void)
{
	int	ax;
	
	ax = ReadCommandAxis();
	
	_dsp_clear_status(ax);

	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence));
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Axis_Stat
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_Axis_Stat(void)
{
	int	ax, state;
	
	ax = ReadCommandAxis();
	
	state = _dsp_axis_state(ax);
	
	WriteChannel2(AxisDPRAM[ax],AD_IntType1, state);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Axis_Source
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_Axis_Source(void)
{
	int	ax, src;
	
	ax = ReadCommandAxis();
	src = _dsp_axis_source(ax);
	
	WriteChannel2(AxisDPRAM[ax],AD_IntType1, src);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Dac_Offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_Dac_Offset(void)
{
	int	ax;
	
	ax = ReadCommandAxis();
	
	if (ReadChannel2(AxisDPRAM[ax],AD_IntType3) == 0)
	{
		_dsp_set_dac_output(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
	}
	else
	{
		_dsp_set_analog_offset(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Dac_Offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_Dac_Offset(void)
{
	int	ax,volt;
	
	ax = ReadCommandAxis();

	if (ReadChannel2(AxisDPRAM[ax],AD_IntType3) == 0)
	{
		_dsp_get_dac_output(ax, &volt);
	}
	else
	{
		_dsp_get_analog_offset(ax, &volt);
	}

	WriteChannel2(AxisDPRAM[ax],AD_IntType1, volt);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Frames_Clear
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_Frames_Clear(void)
{
	int	ax;

	ax = ReadCommandAxis();
	
	_dsp_frames_clear(ax);
	
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,0);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Amp_On_Level
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_Amp_On_Level(void)
{
	int	ax;
	
	ax = ReadCommandAxis();
	
	_dsp_set_amp_enable_level(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Amp_On_Level
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_Amp_On_Level(void)
{
	int	ax, lvl;
	
	ax = ReadCommandAxis();
	
	_dsp_get_amp_enable_level(ax, &lvl);
	
	WriteChannel2(AxisDPRAM[ax],AD_IntType1, lvl);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SW_Upper_Limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SW_Upper_Limit(void)
{
	int	ax = ReadCommandAxis();
	int li = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	char cst = ReadChannel1(AxisDPRAM[ax],AD_CharType1);

	__SetSWUpperLimit(ax, li, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_SW_Upper_Limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_SW_Upper_Limit(void)
{
	int	ax, li;
	char cst;
	
	ax = ReadCommandAxis();

	__GetSWUpperLimit(ax, &li, &cst);

	WriteChannel(AxisDPRAM[ax],AD_LongType1, li);
	WriteChannel1(AxisDPRAM[ax],AD_CharType1, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SW_Lower_Limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SW_Lower_Limit(void)
{
	int ax = ReadCommandAxis();
	int li = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	char cst = ReadChannel1(AxisDPRAM[ax],AD_CharType1);
	
	__PutSWLowerLimit(ax, li, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_SW_Lower_Limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_SW_Lower_Limit(void)
{
	int ax = ReadCommandAxis();
	int li;
	char cst;
	
	__GetSWLowerLimit(ax, &li, &cst);
	
	WriteChannel(AxisDPRAM[ax],AD_LongType1,li);
	WriteChannel1(AxisDPRAM[ax],AD_CharType1, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_IO_Out_Data
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_IO_Out_Data(void)
{
	int dat[2];

	amc_error= _dsp_get_out64(dat);
	
	WriteChannel(CommDPRAM, CD_Io_outValue, dat[0] );
	WriteChannel(CommDPRAM, CD_Io_outValue2, dat[1] );
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SyncMap
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SyncMap(void)
{
	_dsp_set_sync_map_axes(ReadChannel2(CommDPRAM,CD_Axis), ReadChannel2(CommDPRAM,CD_Axis+2));
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SyncOn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SyncOn(void)
{
	SyncMotion.Flag = 1;
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SyncOff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SyncOff(void)		
{
	SyncMotion.Flag = 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_SyncGain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Get_SyncGain(void)
{
	int	ax;
	float syncgain, tmp;
	
	ax = ReadCommandAxis();
	tmp = IEEE2TMS(ReadChannel(AxisDPRAM[ax], AD_FloatType1));

	if(tmp > 1.03)
	{
		syncgain = 1.03;
	}
	else if(tmp < 0.97)
	{
		syncgain = 0.97;
	}
	else
	{
		syncgain = tmp;
	}
	
	_dsp_set_sync_gain(syncgain);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_SyncGain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_SyncGain(void)	
{
	int	ax;
	float sgain;
	
	ax = ReadCommandAxis();
	
	_dsp_get_sync_gain(&sgain);
	
	WriteChannel(AxisDPRAM[ax],AD_FloatType1, TMS2IEEE(sgain));
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Encoder
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_Encoder(void)
{
	int	ax;
	int	pos;
	
	ax = ReadCommandAxis();
	
	pos= pCBuf[ax]->jtpos;
	
	WriteChannel(AxisDPRAM[ax],AD_LongType1,pos);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_SyncEncoder
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_SyncEncoder(void)
{
	int	pos_m,pos_s;

	__GetSyncEncoder(&pos_m, &pos_s);

	WriteChannel(AxisDPRAM[SyncMotion.Master],AD_LongType1, pos_m);
	WriteChannel(AxisDPRAM[SyncMotion.Slave],AD_LongType1, pos_s);
}

////////////////////////////////////////////////////////////////////////
// Function	: Exist_Check
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Exist_Check(void)
{
	WriteChannel2(CommDPRAM,CD_AxisNum,(int)JPC_AXIS);//2
	WriteChannel2(CommDPRAM,CD_Ver,(int)VERSION_DSP);
}

////////////////////////////////////////////////////////////////////////
// Function	: IEEE2TMS
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float IEEE2TMS(int x)
{
	float *pf = (float *) &x;
	
	return *pf;
}

////////////////////////////////////////////////////////////////////////
// Function	: TMS2IEEE
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int TMS2IEEE(float x)
{
	int *pi = (int *) &x;
	
	return *pi;
}

////////////////////////////////////////////////////////////////////////
// Function	: Copy_Para_2_BootPara
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Copy_Para_2_BootPara(void)
{
	int i;
	
	for( i=0 ; i<JPC_AXIS ; i++ )
	{
		BootPara.st_boot.Vel_Limit[i] = pCBuf[i]->Vel_Limit;
		BootPara.st_boot.Accel_Limit[i] = pCBuf[i]->Accel_Limit;		// reserved 
		BootPara.st_boot.PulseRatio[i] = 0;
		BootPara.st_boot.GearRatio[i] = 1; 

		BootPara.st_boot.HwLower_Limit[i] = 0.0; 						// reserved 
		BootPara.st_boot.HwUpper_Limit[i] = 0.0; 						// reserved 
		BootPara.st_boot.SwLower_Limit[i] = pCBuf[i]->swlower_limit; 	// system lower limit 
		BootPara.st_boot.SwUpper_Limit[i] = pCBuf[i]->swupper_limit; 	// system upper limit   

		BootPara.st_boot.PGain[i] = pCBuf[i]->pgain;       				// P gain 
		BootPara.st_boot.IGain[i] = pCBuf[i]->igain;        			// I gain
		BootPara.st_boot.DGain[i] = pCBuf[i]->dgain;        			// D gain
		BootPara.st_boot.FGain[i] = pCBuf[i]->fgain;        			// F gain
		BootPara.st_boot.ILimit[i] = pCBuf[i]->ilimit;       			// I_LIMIT

		BootPara.st_boot.VPgain[i] = pCBuf[i]->velpgain;       			// VELP 
		BootPara.st_boot.VIgain[i] = pCBuf[i]->veligain;        		// VELI gain 
		BootPara.st_boot.VDgain[i] = pCBuf[i]->veldgain;        		// VELD gain 
		BootPara.st_boot.VFgain[i] = pCBuf[i]->velfgain;        		// VELF gain 
		BootPara.st_boot.VIlimit[i] = pCBuf[i]->velilimit;     			// VELI_LIMIT

		BootPara.st_boot.In_Position[i] = pCBuf[i]->in_position;
		BootPara.st_boot.Error_Limit[i] = pCBuf[i]->limit_err_chk;

		if( pCBuf[i]->motortype == 1)
		{
			BootPara.st_boot.Motor_Type[i] = 1;
		}
		else
		{
			BootPara.st_boot.Motor_Type[i] = 0;
		}

		BootPara.st_boot.SwUpper_LimitSt[i] = pCBuf[i]->swupper_limitst;
		BootPara.st_boot.SwLower_LimitSt[i] = pCBuf[i]->swlower_limitst;

		BootPara.st_boot.Pos_Level[i] = pCBuf[i]->pos_level;
		BootPara.st_boot.Neg_Level[i] = pCBuf[i]->neg_level;
		BootPara.st_boot.Home_Level[i] = pCBuf[i]->home_level;
		BootPara.st_boot.Amp_Level[i] = pCBuf[i]->fault_level;
		BootPara.st_boot.Amp_Reset_Level[i] = pCBuf[i]->reset_level;

		BootPara.st_boot.Pos_Limit_St[i] = pCBuf[i]->pos_sn_st;
		BootPara.st_boot.Neg_Limit_St[i] = pCBuf[i]->neg_sn_st;
		BootPara.st_boot.Home_Limit_St[i] = pCBuf[i]->home_sn_st; 
		BootPara.st_boot.Error_Limit_St[i] = pCBuf[i]->limit_err_st;

		BootPara.st_boot.Encoder_Cfg[i] = pCBuf[i]->encoder_type;
		BootPara.st_boot.Voltage_Cfg[i] = pCBuf[i]->voltage_type;
		BootPara.st_boot.Home_Index[i] = pCBuf[i]->index_req;

		BootPara.st_boot.Stop_Rate[i] = pCBuf[i]->stop_rate;
		BootPara.st_boot.E_Stop_Rate[i] = pCBuf[i]->e_stop_rate;

		BootPara.st_boot.Control_Cfg[i] = pCBuf[i]->control_cfg;
		BootPara.st_boot.Loop_Cfg[i] = pCBuf[i]->loop_cfg;
		BootPara.st_boot.Amp_OnLevel[i] = pCBuf[i]->amp_on_level;

		BootPara.st_boot.Io_Int_Enable[i] = 0;	
		BootPara.st_boot.Int_Event_St[i] = 0;
		BootPara.st_boot.Amp_Fault_Event[i] = pCBuf[i]->amp_event;

		BootPara.st_boot.PosImode[i] = pCBuf[i]->posimode;
		BootPara.st_boot.VelImode[i] = pCBuf[i]->velimode;
		BootPara.st_boot.PulseMode[i] = 0;

		BootPara.st_boot.Inpos_Level[i] = 0;	

		BootPara.st_boot.Dpram_Addr[i] = 0;		
		
		BootPara.st_boot.dac_bias[i] = pCBuf[i]->dac_bias;
		BootPara.st_boot.V_TrackingFactor[i] = pCBuf[i]->V_TrackingFactor;

	}
	
	BootPara.st_boot.Axis_Num = 0;				
	BootPara.st_boot.Action_Axis_Num = 0;		

	BootPara.st_boot.UserIO_BootMode = userio_mode_select;
	BootPara.st_boot.UserIO_BootValue = pGBuf->user_io_out;

	SetBootParaCrc(&BootPara);
}

////////////////////////////////////////////////////////////////////////
// Function	: Copy_BootPara_2_Para
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Copy_BootPara_2_Para(void)
{
	int i;
	
	for( i=0 ; i<JPC_AXIS ; i++ )
	{
		pCBuf[i]->bMustReboot = 0;
		
		pCBuf[i]->Vel_Limit = BootPara.st_boot.Vel_Limit[i];
		pCBuf[i]->Accel_Limit = BootPara.st_boot.Accel_Limit[i];
		pCBuf[i]->swlower_limit = BootPara.st_boot.SwLower_Limit[i]; 	// system lower limit 
		pCBuf[i]->swupper_limit = BootPara.st_boot.SwUpper_Limit[i]; 	// system upper limit


		pCBuf[i]->pgain = BootPara.st_boot.PGain[i];       				// P gain
		pCBuf[i]->igain = BootPara.st_boot.IGain[i];        			// I gain
		pCBuf[i]->dgain = BootPara.st_boot.DGain[i];        			// D gain 
		pCBuf[i]->fgain = BootPara.st_boot.FGain[i];        			// F gain 
		pCBuf[i]->ilimit = BootPara.st_boot.ILimit[i];       			// I_LIMIT  

		pCBuf[i]->velpgain = BootPara.st_boot.VPgain[i];     		  	// VELP gain    
		pCBuf[i]->veligain = BootPara.st_boot.VIgain[i];      		  	// VELI gain   
		pCBuf[i]->veldgain = BootPara.st_boot.VDgain[i];        		// VELD gain   
		pCBuf[i]->velfgain = BootPara.st_boot.VFgain[i];       		 	// VELF gain   
		pCBuf[i]->velilimit = BootPara.st_boot.VIlimit[i];     			// VELI_LIMIT   

		pCBuf[i]->in_position = BootPara.st_boot.In_Position[i];
		pCBuf[i]->limit_err_chk = BootPara.st_boot.Error_Limit[i];

		if( BootPara.st_boot.Motor_Type[i] == 1 )
		{
			pCBuf[i]->motortype = 1;
		}
		else
		{
			pCBuf[i]->motortype = 0;
		}

		pCBuf[i]->swupper_limitst = BootPara.st_boot.SwUpper_LimitSt[i];
		pCBuf[i]->swlower_limitst = BootPara.st_boot.SwLower_LimitSt[i];

		pCBuf[i]->pos_level = BootPara.st_boot.Pos_Level[i];
		pCBuf[i]->neg_level = BootPara.st_boot.Neg_Level[i];
		pCBuf[i]->home_level = BootPara.st_boot.Home_Level[i];
		pCBuf[i]->fault_level = BootPara.st_boot.Amp_Level[i];
		pCBuf[i]->reset_level = BootPara.st_boot.Amp_Reset_Level[i];
		pCBuf[i]->amp_on_level = BootPara.st_boot.Amp_OnLevel[i]; 

		pCBuf[i]->pos_sn_st = BootPara.st_boot.Pos_Limit_St[i];
		pCBuf[i]->neg_sn_st = BootPara.st_boot.Neg_Limit_St[i];
		pCBuf[i]->home_sn_st = BootPara.st_boot.Home_Limit_St[i]; 
		pCBuf[i]->limit_err_st = BootPara.st_boot.Error_Limit_St[i];

		pCBuf[i]->amp_event = BootPara.st_boot.Amp_Fault_Event[i];

		pCBuf[i]->encoder_type = BootPara.st_boot.Encoder_Cfg[i];
		pCBuf[i]->voltage_type = BootPara.st_boot.Voltage_Cfg[i];
		pCBuf[i]->index_req = BootPara.st_boot.Home_Index[i];

		pCBuf[i]->stop_rate = BootPara.st_boot.Stop_Rate[i];
		pCBuf[i]->e_stop_rate = BootPara.st_boot.E_Stop_Rate[i];

		pCBuf[i]->control_cfg = BootPara.st_boot.Control_Cfg[i];	// Torque

		pCBuf[i]->loop_cfg = BootPara.st_boot.Loop_Cfg[i];

		pCBuf[i]->posimode = BootPara.st_boot.PosImode[i];
		pCBuf[i]->velimode = BootPara.st_boot.VelImode[i];

		pCBuf[i]->inpos_level = BootPara.st_boot.Inpos_Level[i];	// SRDYAC

		pCBuf[i]->V_TrackingFactor = BootPara.st_boot.V_TrackingFactor[i];

		pCBuf[i]->dac_bias = BootPara.st_boot.dac_bias[i];
		pCBuf[i]->dac_code = 0;

		pCBuf[i]->EncoderOffset = BootPara.st_boot.Encoder_Offset[i];

		// pCBuf[i]->fScurveRatio = BootPara.st_boot.ScurveSmoothingFactor[i];
		
		_dsp_amp_fault_reset(i);

		__SetWorkingLimit(i, pCBuf[i]->e_stop_rate);
		
		pCBuf[i]->vel_limit_ratio = 0.5;
	}

	userio_mode_select = BootPara.st_boot.UserIO_BootMode | int_change;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetInt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetInt(int ofs)
{
	int nmagic = ReadEepromChar(ofs + 0) & 0xff;
	
	nmagic |= ((int)ReadEepromChar(ofs + 1) & 0xff) << 8;
	nmagic |= ((int)ReadEepromChar(ofs + 2) & 0xff) << 16;
	nmagic |= ((int)ReadEepromChar(ofs + 3) & 0xff) << 24;

	return nmagic;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetShort
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
short __GetShort(int ofs)
{
	int ns = ReadEepromChar(ofs + 0) & 0xff;
	
	ns |= ((int)ReadEepromChar(ofs + 1) & 0xff) << 8;

	return ns;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetChar
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
unsigned char __GetChar(int ofs) 
{ 
	char ch = ReadEepromChar(ofs);
	
	return (ch & 0xff); 
}

////////////////////////////////////////////////////////////////////////
// Function	: __PutInt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __PutInt(int ofs, int nval)
{
	WriteEepromInt(ofs + 0, nval & 0xffffffff);
}

////////////////////////////////////////////////////////////////////////
// Function	: __PutShort
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __PutShort(int ofs, short sn)
{
	WriteEepromShort(ofs + 0, sn & 0xffff);
}

////////////////////////////////////////////////////////////////////////
// Function	: __PutChar
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __PutChar(int ofs, char ch)
{
	WriteEepromChar(ofs, ch & 0xff);
}

////////////////////////////////////////////////////////////////////////
// Function	: ReadBootParam
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadBootParam(UBOOTPARA *pBParam)
{
	UBOOTPARA bP;
	char *p = (char *) bP.GetBodyTopPtr();
	int nSize = sizeof(UBOOTPARA), sz;

	sz = ReadEepromString(EEPROM_BOOTPARAM_START_OFS, p, nSize);
	
	if( sz != nSize)
	{
		return -2;
	}

	int ncrc = _crc32_get_crc((char *) &bP, bP.GetBodySize());
	
	if (bP.IsValidParam(ncrc) < 0) 
	{
		return -1;
	}
	
	memcpy(pBParam, &bP, nSize);

	return 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: SaveBootParam
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SaveBootParam(UBOOTPARA *pBParam)
{
	char *p = (char *) pBParam;
	int nSize = sizeof(UBOOTPARA);
	int ncrc;

	ncrc = _crc32_get_crc((char *) pBParam->GetBodyTopPtr(), pBParam->GetBodySize());
	
	pBParam->SetCRC32(ncrc);

	WriteEepromString(EEPROM_BOOTPARAM_START_OFS, p, nSize);
}

////////////////////////////////////////////////////////////////////////
// Function	: InitBootParam
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitBootParam(UBOOTPARA *pb)
{
	memset(pb, 0, sizeof(UBOOTPARA));
	pb->nMagic = EEPROM_BOOTPARAM_MAGIC_NO;
	pb->nBlockSize = sizeof(UBOOTPARA);
	pb->nBoardNo = 1;
	pb->nAxisNo = JPC_AXIS;
	pb->nVerMaj = 1;
	pb->nVerMin = 0;
	
	for (int i = 0; i < JPC_AXIS; i ++)
	{
		pb->st_boot.PulseRatio[i] = 8;
		pb->st_boot.GearRatio[i] = 1;
		pb->st_boot.HwLower_Limit[i] = MMC_NEG_SW_LIMIT;
		pb->st_boot.HwUpper_Limit[i] = MMC_POS_SW_LIMIT;
		pb->st_boot.SwLower_Limit[i] = MMC_NEG_SW_LIMIT;
		pb->st_boot.SwUpper_Limit[i] = MMC_POS_SW_LIMIT;
		pb->st_boot.Vel_Limit[i] = MMC_VEL_LIMIT;
		
		if (i == 0 || i == 1)
		{
			pb->st_boot.PGain[i] = 12000;
			pb->st_boot.IGain[i] = 0;
			pb->st_boot.DGain[i] = 0;
			pb->st_boot.FGain[i] = 0;
			pb->st_boot.ILimit[i] = 1000;
	
			pb->st_boot.VPgain[i] = 0;
			pb->st_boot.VIgain[i] = 0;
			pb->st_boot.VDgain[i] = 0;
			pb->st_boot.VFgain[i] = 0;
			pb->st_boot.VIlimit[i] = 1000;
		} 
		else
		{
			pb->st_boot.PGain[i] = 1200;
			pb->st_boot.IGain[i] = 20;
			pb->st_boot.DGain[i] = 0;
			pb->st_boot.FGain[i] = 0;
			pb->st_boot.ILimit[i] = 1000;
	
			pb->st_boot.VPgain[i] = 1000;
			pb->st_boot.VIgain[i] = 10;
			pb->st_boot.VDgain[i] = 0;
			pb->st_boot.VFgain[i] = 0;
			pb->st_boot.VIlimit[i] = 3276800;
		}

		pb->st_boot.In_Position[i] = 100.;
		pb->st_boot.Error_Limit[i] = MMC_ERROR_LIMIT;
		pb->st_boot.Motor_Type[i] = 0;
		pb->st_boot.Vel_Limit[i] = MMC_VEL_LIMIT;
		pb->st_boot.Accel_Limit[i] = MMC_ACCEL_LIMIT;

		pb->st_boot.Pos_Level[i] = LOW;
		pb->st_boot.Neg_Level[i] = LOW;
		pb->st_boot.Home_Level[i] = LOW;
		pb->st_boot.Amp_Level[i] = HIGH;
		pb->st_boot.Amp_Reset_Level[i] = LOW;
		pb->st_boot.Amp_OnLevel[i] = LOW;

		pb->st_boot.Pos_Limit_St[i] = NO_EVENT;
		pb->st_boot.Neg_Limit_St[i] = NO_EVENT;
		pb->st_boot.Home_Limit_St[i] = NO_EVENT;
		
		pb->st_boot.Int_Event_St[i] = NO_EVENT;
		pb->st_boot.Error_Limit_St[i] = NO_EVENT;
		pb->st_boot.Amp_Fault_Event[i] = NO_EVENT;

		pb->st_boot.SwLower_LimitSt[i] = NO_EVENT;
		pb->st_boot.SwUpper_LimitSt[i] = NO_EVENT;

		pb->st_boot.Encoder_Cfg[i] = 0;
		pb->st_boot.Voltage_Cfg[i] = 0;
		pb->st_boot.Home_Index[i] = 0;

		pb->st_boot.Stop_Rate[i] = 10;
		pb->st_boot.E_Stop_Rate[i] = 25;

		pb->st_boot.Control_Cfg[i] = 0;
		pb->st_boot.Loop_Cfg[i] = 0; 	

		pb->st_boot.PosImode[i] = IN_ALWAYS;
		pb->st_boot.VelImode[i] = IN_ALWAYS;
		pb->st_boot.PulseMode[i] = TWO_PULSE;

		pb->st_boot.Io_Int_Enable[i] = 0;
		
		pb->st_boot.V_TrackingFactor[i] = 1.0;
		pb->st_boot.dac_bias[i] = 0;
		pb->st_boot.Encoder_Offset[i] = 0;
		
		pb->st_boot.Encoder_direction[i] = CIR_CCW;
		pb->st_boot.Motor_Pause[i] = 0;
		pb->st_boot.Motor_Pause_Level[i] = 1;
		pb->st_boot.Motor_Pause_CheckBit[i] = 0;
		
		pb->st_boot.naPositionNotchFreq[i] = 0;
		pb->st_boot.naPositionLPFFreq[i] = 1000;
		pb->st_boot.naVelocityNotchFreq[i] = 0;
		pb->st_boot.naVelocityLPFFreq[i] = 1000;

		pb->st_boot.ScurveSmoothingFactor[i] = 0.1;
	}
	
	SetBootParaCrc(pb);	
}

////////////////////////////////////////////////////////////////////////
// Function	: SetBootParaCrc
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
UINT SetBootParaCrc(UBOOTPARA *pbp)
{
	unsigned int ncrc;
	
	ncrc = _crc32_get_crc((char *) pbp->GetBodyTopPtr(), pbp->GetBodySize());
	pbp->SetCRC32(ncrc);
	
	return ncrc;
}

////////////////////////////////////////////////////////////////////////
// Function	: SaveLongBlockToEeprom
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SaveLongBlockToEeprom()
{
	_SaveLongBlockToEeprom();
}

////////////////////////////////////////////////////////////////////////
// Function	: _SaveLongBlockToEeprom
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _SaveLongBlockToEeprom()
{
	int nBlkID, uiOfs;
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	
	nBlkID = ReadChannel2(pMapStartBlock, MAP_OFS_BLKID2);
	
	if (nBlkID == 0)
	{
		mmHdr.uiCrc32 = ReadChannel(pMapStartBlock, MAP_OFS_CRC4);
		mmHdr.uiTotLen = ReadChannel(pMapStartBlock, MAP_OFS_TOTLEN4);
		mmHdr.uiBlkLen = ReadChannel2(pMapStartBlock, MAP_OFS_BLKLEN2);
		
		if (m_pcaBuff) 
		{
			free(m_pcaBuff);
		}
		m_pcaBuff = (char *) malloc(mmHdr.uiTotLen);
	}
	
	uiOfs = nBlkID * 256;
	
	if (uiOfs > mmHdr.uiTotLen) 
	{
		return -1;
	}

	{
		int nEnd = min(mmHdr.uiTotLen - uiOfs, 256);
		
		for (int j = 0; j < nEnd; j ++)
		{
			m_pcaBuff[uiOfs] = ReadChannel1(pMapStartBlock + MAP_OFS_DATA256, j);
			uiOfs ++;
		}
	}
	
	if (uiOfs == mmHdr.uiTotLen)
	{
		int ulCrc = _crc32_get_crc(m_pcaBuff, mmHdr.uiTotLen);
		
		if (ulCrc != mmHdr.uiCrc32)
		{
			free(m_pcaBuff);
			m_pcaBuff = NULL;			
			return -2;
		}

		WriteEepromString(uiEepromSaveAddr, (char *) &mmHdr, sizeof(mmHdr));
		WriteEepromString(uiEepromSaveAddr + sizeof(mmHdr), m_pcaBuff, mmHdr.uiTotLen);
		
		free(m_pcaBuff);
		m_pcaBuff = NULL;
		return 1;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////
// Function	: LoadLongBlockFromEeprom
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int LoadLongBlockFromEeprom(UINT uiEepromAddr,char *psMap, UINT *puiCrc, UINT *puiLen, int nReadBytes)
{
	MAP_MAIN_HEADER mmHdr;
	
	ReadEepromString(uiEepromAddr, (char *) &mmHdr, sizeof(mmHdr));

	*puiLen = mmHdr.uiTotLen;
	*puiCrc = mmHdr.uiCrc32;
	
	if (mmHdr.uiTotLen > nReadBytes)
	{
		return -1;
	}
	
	ReadEepromString(uiEepromAddr + sizeof(mmHdr), psMap, mmHdr.uiTotLen);

	ULONG ulCrc = _crc32_get_crc(psMap, mmHdr.uiTotLen);
	
	if (ulCrc != mmHdr.uiCrc32) 
	{
		return -2;
	}
	
	return 1;
}

////////////////////////////////////////////////////////////////////////
// Function	: RunConfigUploadToIPC
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void RunConfigUploadToIPC()
{
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	int nBlkID = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKID2);
	
	if (nBlkID == 0)
	{
		eDumpHdr.ulStartAddress = ReadChannel(pMapStartBlock, EDUMP_OFS_SAVEADDR4);
		eDumpHdr.ulTotLen = ReadChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4);

		if (peDumpBody) 
		{
			free(peDumpBody);
		}
		peDumpBody = (char *) malloc(mmHdr.uiTotLen);
		
		ReadEepromString(eDumpHdr.ulStartAddress, peDumpBody, eDumpHdr.ulTotLen);
		
		eDumpHdr.ulCrc = _crc32_get_crc(peDumpBody, eDumpHdr.ulTotLen);
		
		int blklen = (eDumpHdr.ulTotLen / 256);
		
		if (blklen * 256 < eDumpHdr.ulTotLen) 
		{
			blklen += 1;
		}

		WriteChannel2(pMapStartBlock, EDUMP_OFS_BLKLEN2, blklen);
		WriteChannel(pMapStartBlock, EDUMP_OFS_CRC4, eDumpHdr.ulCrc);
	}
	
	{
		long uiOfs = nBlkID * 256;
		int nEnd = min(eDumpHdr.ulTotLen - uiOfs, 256);
		
		for (int i = 0; i < nEnd; i ++)
		{
			WriteChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i, peDumpBody[uiOfs++]);
		}
		
		if (uiOfs >= eDumpHdr.ulTotLen)
		{
			if (peDumpBody) 
			{
				free(peDumpBody);
			}
			peDumpBody = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: RunConfigSaveFromIPC
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void RunConfigSaveFromIPC()
{
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	int nBlkID = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKID2);
	
	if (nBlkID == 0)
	{
		eDumpHdr.ulCrc = 0;
		eDumpHdr.ulTotLen = 0;
		eDumpHdr.uiBlkLen = 0;
		eDumpHdr.ulCrc = ReadChannel(pMapStartBlock, EDUMP_OFS_CRC4);
		eDumpHdr.ulStartAddress = ReadChannel(pMapStartBlock, EDUMP_OFS_SAVEADDR4);
		eDumpHdr.ulTotLen = ReadChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4);
		eDumpHdr.uiBlkLen = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKLEN2);
		
		if (peDumpBody) 
		{
			free(peDumpBody);
		}
		peDumpBody = (char *) malloc(eDumpHdr.ulTotLen);
	}
	
	{
		int uiOfs = nBlkID * 256;
		int nEnd = min(eDumpHdr.ulTotLen - uiOfs, 256);
		
		for (int i = 0; i < nEnd; i ++)
		{
			peDumpBody[uiOfs++] = ReadChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i);
		}
	
		WriteChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4, 0);
		
		if (uiOfs >= eDumpHdr.ulTotLen)
		{
			unsigned long ulCrc = _crc32_get_crc(peDumpBody, eDumpHdr.ulTotLen);
			
			if (ulCrc != eDumpHdr.ulCrc)
			{
				WriteChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4, -1);

			} 
			else
			{
				WriteEepromString(eDumpHdr.ulStartAddress, peDumpBody, eDumpHdr.ulTotLen);
				
				free(peDumpBody);
				peDumpBody = NULL;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: UploadParamToIPC
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void UploadParamToIPC()
{
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	int nBlkID = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKID2);
	
	if (nBlkID == 0)
	{
		eDumpHdr.ulTotLen = sizeof(BootPara);
		
		if (peDumpBody) 
		{
			free(peDumpBody);
		}
		
		peDumpBody = (char *) malloc(eDumpHdr.ulTotLen);
		
		eDumpHdr.ulCrc = _crc32_get_crc(peDumpBody, BootPara.GetBodySize());
		
		memcpy(peDumpBody, BootPara.GetBodyTopPtr(), eDumpHdr.ulTotLen);
		
		int blklen = (eDumpHdr.ulTotLen / 256);
		
		if (blklen * 256 < eDumpHdr.ulTotLen) 
		{
			blklen += 1;
		}
		
		WriteChannel2(pMapStartBlock, EDUMP_OFS_BLKLEN2, blklen);
		WriteChannel(pMapStartBlock, EDUMP_OFS_CRC4, eDumpHdr.ulCrc);
		WriteChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4, eDumpHdr.ulTotLen);
	}
	
	{
		long uiOfs = nBlkID * 256;
		int nEnd = min(eDumpHdr.ulTotLen - uiOfs, 256);
		
		for (int i = 0; i < nEnd; i ++)
		{
			WriteChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i, peDumpBody[uiOfs++]);
		}
		
		if (uiOfs >= eDumpHdr.ulTotLen)
		{
			if (peDumpBody)
			{
				free(peDumpBody);
			}
			peDumpBody = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: DnloadParamFromIPC
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void DnloadParamFromIPC()
{
	UBOOTPARA *pUBP;
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	int nBlkID = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKID2);
	
	if (nBlkID == 0)
	{
		eDumpHdr.ulStartAddress = 0;
		eDumpHdr.ulCrc = ReadChannel(pMapStartBlock, EDUMP_OFS_CRC4);
		eDumpHdr.ulTotLen = ReadChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4);
		eDumpHdr.uiBlkLen = ReadChannel2(pMapStartBlock, EDUMP_OFS_BLKLEN2);
		
		if (peDumpBody) 
		{
			free(peDumpBody);
		}
		peDumpBody = (char *) malloc(eDumpHdr.ulTotLen);
	}
	
	{
		int uiOfs = nBlkID * 256;
		int nEnd = min(eDumpHdr.ulTotLen - uiOfs, 256);
		
		for (int i = 0; i < nEnd; i ++)
		{
			peDumpBody[uiOfs++] = ReadChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i);
		}
	
		WriteChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4, 0);
		if (uiOfs >= eDumpHdr.ulTotLen)
		{
			unsigned long ulCrc = _crc32_get_crc(peDumpBody, eDumpHdr.ulTotLen);
			
			if (ulCrc != eDumpHdr.ulCrc)
			{
				WriteChannel(pMapStartBlock, EDUMP_OFS_TOTLEN4, -1);

			}
			else
			{
				pUBP = (UBOOTPARA *) peDumpBody;
				memcpy(BootPara.GetBodyTopPtr(), pUBP, BootPara.GetBodySize());
				BootPara.nCRC32 = ulCrc;
				
				SaveBootParam(&BootPara);
				
				Copy_BootPara_2_Para();
			
				free(peDumpBody);
				peDumpBody = NULL;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: WatchdogOperations
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void WatchdogOperations()
{
	int	cmd, nwhere, nrtn;
	
	// command 1:enable, 2:disable, 3:getstatus, 4:setstatus, 5:trigger
	cmd = ReadChannel2(AxisDPRAM[0],AD_IntType1);		// watchdog reason
	nwhere = ReadChannel2(AxisDPRAM[0],AD_IntType2);	// watchdog reason
	
	switch (cmd)
	{
		case 1:	// enable
			_dsp_enable_wdt_reason(nwhere); 
			break;
			
		case 2: // disable
			_dsp_disable_wdt_reason(nwhere); 
			break;
			
		case 3: // getstatus
			nrtn = _dsp_get_wdt_status();
			WriteChannel2(AxisDPRAM[0], AD_IntType1, nrtn);
			break;
			
		case 4: // setstatus
			_dsp_set_wdt_status(nwhere);
			break;
			
		case 5: // triggering
			_dsp_clr_wdt_reason(nwhere);
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: GetMovexCmd
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetMovexCmd()
{
	char * pMapStartBlock = MAP_DPRAM_ADDRESS + 0x10;
	MOVE_X_PARAM mvxParam;
	char *pPtr = (char *) &mvxParam;

	volatile double FollowsACC_tmp_GetMovexCmd;
	volatile double FollowsDCC_tmp_GetMovexCmd;
	volatile double FollowsVEL_tmp_GetMovexCmd;
	volatile int FollowsPOS_tmp_GetMovexCmd;	
	
	check_pccmd = 0x80;
	amc_move_err =0;

	for (int i = 0; i < sizeof(MOVE_X_PARAM); i ++)
	{
		*pPtr++ = ReadChannel1(pMapStartBlock, i);
	}

    if (pCBuf[mvxParam.ax]->event_st)
    {
        amc_error = MMC_ON_MOTION;
        return;
    }
    
    if (pCBuf[mvxParam.ax]->g_on != 0)
    {
        return;
    }

    if (SyncMotion.Flag)
    {
        if(SyncMotion.Slave == mvxParam.ax)
        {
            return;
        }
        else if((SyncMotion.Master == mvxParam.ax) && (pCBuf[SyncMotion.Slave]->g_on != 0))
        {
            return;
        }
    }

	switch (mvxParam.cmd)
	{
		case CMD_MOVE_P:
			if (mvxParam.mode == 1)
			{
				amc_move_err=_dsp_move_pt(mvxParam.ax, mvxParam.a[0]/SECTOMSEC, mvxParam.v[0]);
			}
			else
			{
				amc_move_err=_dsp_move_p(mvxParam.ax, mvxParam.a[0], mvxParam.v[0]);
			}
			break;
			
    	case CMD_MOVE_N:
			if (mvxParam.mode == 1)
			{
				amc_move_err=_dsp_move_nt(mvxParam.ax, mvxParam.a[0]/SECTOMSEC, mvxParam.v[0]);
			}
			else
			{
				amc_move_err=_dsp_move_n(mvxParam.ax, mvxParam.a[0], mvxParam.v[0]);
			}
			break;
			
	    case CMD_MOVE_S:
			if (mvxParam.mode == 1)
			{
				MoveS_Flag = 1;            						
				Follow_Enable_Flag = false;				
				amc_move_err=_dsp_move_st(mvxParam.ax, (mvxParam.a[0]/SECTOMSEC));
			}
			else
			{
				MoveS_Flag = 2;            			
				Follow_Enable_Flag = false;								
				amc_move_err=_dsp_move_s(mvxParam.ax, mvxParam.a[0]);
			}
			break;
		
    	case CMD_MOVE_DS:
			Marking_Process_Flag[mvxParam.ax] = false;
			
			if (mvxParam.mode == 1)
			{
	            if(mvxParam.ax == 0)
	            {
		            Check_Front_Rear_servo_On();

					FollowsACC_tmp_GetMovexCmd = (mvxParam.a[0]/SECTOMSEC);
					FollowsDCC_tmp_GetMovexCmd = (mvxParam.a[1]/SECTOMSEC);
					FollowsVEL_tmp_GetMovexCmd = mvxParam.v[0];
					FollowsPOS_tmp_GetMovexCmd = mvxParam.m[0];
					
					MoveDS_Flag = 1;							
					amc_move_err=_dsp_move_dst(mvxParam.ax,	FollowsACC_tmp_GetMovexCmd, FollowsDCC_tmp_GetMovexCmd, FollowsVEL_tmp_GetMovexCmd, FollowsPOS_tmp_GetMovexCmd);

					Follow_Enable_Flag = FALSE;

					FollowsPOS = FollowsPOS_tmp_GetMovexCmd;					
					FollowsVEL = FollowsVEL_tmp_GetMovexCmd;															
					FollowsACC = SPEED_2_0_MPS2;
					FollowsDCC = SPEED_3_0_MPS2;
					
					FollowsDCC_CMD = FollowsDCC;					

					FollowsPOS_Prev = FollowsPOS;					
					FollowsVEL_Prev = FollowsVEL;										
					FollowsACC_Prev = FollowsACC;
					FollowsDCC_Prev = FollowsDCC;
					
					Follow_Enable_Flag = TRUE;					
				}
				else
				{
					MoveDS_Flag = 2;							
					amc_move_err=_dsp_move_dst(mvxParam.ax,	(mvxParam.a[0]/SECTOMSEC), (mvxParam.a[1]/SECTOMSEC), mvxParam.v[0], mvxParam.m[0]);
				}									
			}
			else
			{
	            if(mvxParam.ax == 0)
	            {
		            Check_Front_Rear_servo_On();

					FollowsACC_tmp_GetMovexCmd = mvxParam.a[0]; 
					FollowsDCC_tmp_GetMovexCmd = mvxParam.a[1];
					FollowsVEL_tmp_GetMovexCmd = mvxParam.v[0];
					FollowsPOS_tmp_GetMovexCmd = mvxParam.m[0];
					
					MoveDS_Flag = 3;				
					_dsp_move_ds(mvxParam.ax, FollowsACC_tmp_GetMovexCmd, FollowsDCC_tmp_GetMovexCmd, FollowsVEL_tmp_GetMovexCmd, FollowsPOS_tmp_GetMovexCmd);

					Follow_Enable_Flag = FALSE;

					FollowsPOS = FollowsPOS_tmp_GetMovexCmd;					
					FollowsVEL = FollowsVEL_tmp_GetMovexCmd;															
					FollowsACC = FollowsACC_tmp_GetMovexCmd;
					FollowsDCC = FollowsDCC_tmp_GetMovexCmd;

					FollowsDCC_CMD = FollowsDCC;

					FollowsPOS_Prev = FollowsPOS;					
					FollowsVEL_Prev = FollowsVEL;					
					FollowsACC_Prev = FollowsACC;
					FollowsDCC_Prev = FollowsDCC;
					
					Follow_Enable_Flag = TRUE;					
				}
				else
				{
					MoveDS_Flag = 4;				
					amc_move_err=_dsp_move_ds(mvxParam.ax,	mvxParam.a[0], mvxParam.a[1], mvxParam.v[0], mvxParam.m[0]);
				}									
			}
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_clear_amc_error
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_clear_amc_error()
{
	amc_error = MMC_OK;
	amc_move_err= MMC_OK;  
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_error_status
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_get_error_status()
{
	int ax;
	
	ax = ReadCommandAxis();

	WriteChannel(AxisDPRAM[ax],AD_LongType1, amc_move_err); // 20120704 2.9.6 syk 
	
	amc_move_err= MMC_OK;    								// 20120704 2.9.6 syk 
}

////////////////////////////////////////////////////////////////////////
// Function	: AMCStatusGet
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AMCStatusGet()
{
	char *ptr = (char *) &sDbgMsgBuff;
	char *pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);
	
	memcpy(pd, ptr, 256);
}

////////////////////////////////////////////////////////////////////////
// Function	: InitDbgCmdBuff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitDbgCmdBuff(int ax)
{
	sDbgCmdBuff[ax].InitCmd();
}

////////////////////////////////////////////////////////////////////////
// Function	: PushDbgCmdBuff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PushDbgCmdBuff(int ax, int cmd, float acc, float dcc, float vel, int pos)
{
	char i=0,j=3;
	
	for(i=0; i<3; i++, j--)
	{
		sDbgCmdBuff[j].cmd = sDbgCmdBuff[j-1].cmd;
		sDbgCmdBuff[j].acc = sDbgCmdBuff[j-1].acc;
		sDbgCmdBuff[j].dcc = sDbgCmdBuff[j-1].dcc;
		sDbgCmdBuff[j].vel = sDbgCmdBuff[j-1].vel;
		sDbgCmdBuff[j].pos = sDbgCmdBuff[j-1].pos;
	}

	sDbgCmdBuff[0].cmd = cmd;
	sDbgCmdBuff[0].acc = acc;
	sDbgCmdBuff[0].dcc = dcc;
	sDbgCmdBuff[0].vel = vel;
	sDbgCmdBuff[0].pos = pos;
}

////////////////////////////////////////////////////////////////////////
// Function	: move_PushDbgCmdBuff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void move_PushDbgCmdBuff(int ax, int cmd, float acc, float dcc, float vel, int pos)
{
	move_sDbgCmdBuff[ax].cmd = cmd;
	move_sDbgCmdBuff[ax].acc = acc;
	move_sDbgCmdBuff[ax].dcc = dcc;
	move_sDbgCmdBuff[ax].vel = vel;
	move_sDbgCmdBuff[ax].pos = pos;
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_CleanVoltage
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Put_CleanVoltage(void)
{
    dcleanVoltage = IEEE2TMS(ReadChannel(AxisDPRAM[0], AD_FloatType1));
}

////////////////////////////////////////////////////////////////////////
// Function	: DPRAMTest
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void DPRAMTest()
{
	char *psStr = (char *) MAP_DPRAM_ADDRESS;

	psStr[3] = 0;	
	
	while (*psStr < 5)
	{
		switch (*psStr)
		{
			case 0:	
				break;
			
			case 1:
				psStr[2] = DPRAMTEST_VALUE2;
				if ((psStr[1] & 0xff) != DPRAMTEST_VALUE1) 
				{
					psStr[3] = 1;
				}
				break;
				
			case 2:
				if ((psStr[1] & 0xff) != DPRAMTEST_VALUE1) 
				{
					psStr[3] = 1;
				}
				break;
				
			case 3:
				psStr[2] = DPRAMTEST_VALUE2;
				break;
				
			case 4:
				psStr[2] = DPRAMTEST_VALUE2;
				if ((psStr[2]  & 0xff)!= DPRAMTEST_VALUE2)
				{
					psStr[3] = 1;
				}
				psStr[1] = DPRAMTEST_VALUE1;
				if ((psStr[1]  & 0xff)!= DPRAMTEST_VALUE1) 
				{
					psStr[3] = 1;
				}
				break;
		}
	}
	
	*psStr = 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: ReloadEncoderPos
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ReloadEncoderPos()
{
	int	ax;
	
	ax = ReadCommandAxis();
	_ReadServopackPosition(ax, 11, 0);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Vel_Curve
// Brief		: 2.5.25v2.8.07����, 120111, syk �Լ� �߰� // (2.9.02 syk 120316)�뵵 ���� : profile ���� bnad margin ���� �б� 
// Input		:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void	Put_Vel_Curve(void)	 
{
	int	ax;
	int li;
	char cst;
	
	ax = ReadCommandAxis();

	__GetVelCurve(ax, &li, &cst);

	WriteChannel(AxisDPRAM[ax],AD_LongType1, li);
	WriteChannel1(AxisDPRAM[ax],AD_CharType1, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Vel_Curve
// Brief		: 2.5.25v2.8.07����, 120111, syk �Լ� �߰� // (2.9.02 syk 120316)�뵵 ���� : profile ���� bnad margin ����
// Input		:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void	Get_Vel_Curve(void)
{
	
	int ax = ReadCommandAxis();
	int li = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	char cst = ReadChannel1(AxisDPRAM[ax],AD_CharType1);
	
	__SetVelCurve(ax, li, cst);
}

////////////////////////////////////////////////////////////////////////
// Function	: Put_Actvel_Margin
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 2.9.3, 120323, syk �Լ� �߰� 
////////////////////////////////////////////////////////////////////////
void	Put_Actvel_Margin(void)	
{
	int	ax;
	int li;
	int time;
	char cst;
	
	ax = ReadCommandAxis();

	__GetActvelMargin(ax, &li, &cst, &time);

	WriteChannel1(AxisDPRAM[ax],AD_CharType1, cst);
	WriteChannel2(AxisDPRAM[ax],AD_IntType1, time);	
	WriteChannel(AxisDPRAM[ax],AD_LongType1, li);	
}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Actvel_Margin
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 2.9.3, 120323, syk �Լ� �߰�
////////////////////////////////////////////////////////////////////////
void	Get_Actvel_Margin(void)	 
{
	int ax = ReadCommandAxis();
	int li = ReadChannel(AxisDPRAM[ax],AD_LongType1);
	int time = ReadChannel2(AxisDPRAM[ax],AD_IntType1);;	
	char cst = ReadChannel1(AxisDPRAM[ax],AD_CharType1);
	
	__SetActvelMargin(ax, li, cst, time);
}

////////////////////////////////////////////////////////////////////////
// Function	: Set_Oht_Model_id
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: kskim
////////////////////////////////////////////////////////////////////////
void Set_Oht_Model_id(void)	 
{
	Model_OHT_ID = ReadChannel2(AxisDPRAM[0], AD_IntType1);
	
	Model_NODE_COUNT_IGNORE_UPPER_LIMIT_INIT =  140;    
	Model_NODE_COUNT_IGNORE_UPPER_LIMIT =  140;

	if((Model_OHT_ID==FOUP_7_0) || (Model_OHT_ID==MAC_7_0))
    {	
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 50;
        Model_DRIVINGFRONTNUM = 52;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;		

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;		

		Model_OHT_Service = 0;

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;
    }
	else if(Model_OHT_ID==FOUP_V70_REV_01)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;		

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;	

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }    
	else if(Model_OHT_ID==FOUP_V70_REV_01_SERVICE)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;		

		Model_OHT_Service = 1;		

		Model_FILTER_4_NODECNT = 5;
			
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;			

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;	

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }    	
	else if((Model_OHT_ID==PCARD_7_0) || (Model_OHT_ID==PCARD_7_0_JIG))
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }    
	else if((Model_OHT_ID==MAGAZINE) || (Model_OHT_ID==CASSETTE) || (Model_OHT_ID==FOSB_4WD))
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
	
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = SPEED_3_0_MPS2;  

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }   
	else if((Model_OHT_ID==NEO_TRAY) || (Model_OHT_ID==NEO_SSD))
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_0_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_2_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;								
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_2_MPS2;

        Model_FOLLOWSDCC = SPEED_1_2_MPS2;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }
	else if((Model_OHT_ID==NEO_MAGAZINE) || (Model_OHT_ID==NEO_CASSETTE) || (Model_OHT_ID==FOSB_2WD))
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;								
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_5_MPS2;

        Model_FOLLOWSDCC = SPEED_1_5_MPS2;

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }    	
	else if(Model_OHT_ID==OHT_NAME_MASK)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_1_5_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_5_MPS2;

        Model_FOLLOWSDCC = SPEED_1_5_MPS2;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.3;		
		pCBuf[1]->fScurveRatio = 0.3;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }	
	else if(Model_OHT_ID == OHT_NAME_MASK_PATROL)
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_1_5_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_5_MPS2;

        Model_FOLLOWSDCC = SPEED_1_5_MPS2;
		
		Model_OHT_Service = 1;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.3;		
		pCBuf[1]->fScurveRatio = 0.3;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;			

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}	
	else if(Model_OHT_ID==OHT_NAME_PATROL_CHINA)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

		Model_OHT_Service = 1;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;	

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }	
	else if(Model_OHT_ID==OHT_STD_01)
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 3;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_0_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_2_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_3_3_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_4_1_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_5_2_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 5000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 114.7341619;

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }    	   	
	else if(Model_OHT_ID==OHT_NAME_STD_V80_SERVICE)
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

		Model_OHT_Service = 1;		

		Model_FILTER_4_NODECNT = 3;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_0_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_2_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_3_3_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_4_1_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_5_2_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 5000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 114.7341619;

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }	
	else if(Model_OHT_ID==EAD_CST)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_0_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_2_0_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_5_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_0_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_1_5_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_0_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_0_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 3000;        
    }		
	else if(Model_OHT_ID==NEO_SSD_TRAY)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_0_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_2_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;								
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_2_MPS2;

        Model_FOLLOWSDCC = SPEED_1_2_MPS2;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }		
	else if(Model_OHT_ID==P_FOUP)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_0_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_5_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_0_9_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_1_2_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_1_6_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_2_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 3000;        
    }	
	else if(Model_OHT_ID==OHT_NAME_MGZ_APJT)	
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
	
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;						
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = SPEED_3_0_MPS2;  

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;			

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}
	else if(Model_OHT_ID==OHT_NAME_CST_APJT)	
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 50;
        Model_DRIVINGFRONTNUM = 52;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;		

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;		

		Model_OHT_Service = 0;

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;	

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}
	else if(Model_OHT_ID==OHT_NAME_FOSB_APJT)	
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 50;
        Model_DRIVINGFRONTNUM = 52;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;		

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;		

		Model_OHT_Service = 0;

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}
	else if(Model_OHT_ID==OHT_NAME_TRAY_APJT)	
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_0_MPS2; 
		Model_MOVE_S_DCC_BY_SENSOR_CURVE  = SPEED_1_2_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;								
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_2_MPS2;

        Model_FOLLOWSDCC = SPEED_1_2_MPS2;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;			

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}	
	else if(Model_OHT_ID==OHT_NAME_MAC_APJT)	
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 50;
        Model_DRIVINGFRONTNUM = 52;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;		

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;		

		Model_OHT_Service = 0;

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
	}		
	else if(Model_OHT_ID==OHT_NAME_MAC_L3)
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

        Model_OHT_Service = 0;		            

		Model_FILTER_4_NODECNT = 5;		

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;	

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;  	
	}	
	else if((Model_OHT_ID==OHT_NAME_MAC_L5)	||
             (Model_OHT_ID==OHT_NAME_MAGAZINE_L5) ||
             (Model_OHT_ID==OHT_NAME_CASSETTE_L5) ||
             (Model_OHT_ID==OHT_NAME_FOSB_4WD_L5) ||             
             (Model_OHT_ID==OHT_NAME_BOAT_L5) ||             
             (Model_OHT_ID==OHT_NAME_RETICLE_L5) ||             
             (Model_OHT_ID==OHT_NAME_PATROL_L5))
	{
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;					

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;	

        if(Model_OHT_ID == OHT_NAME_PATROL_L5)
        {
            Model_OHT_Service = 1;		
        }
        else
        {
            Model_OHT_Service = 0;		            
        }

		Model_FILTER_4_NODECNT = 5;		

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;				

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;	

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;		

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;      
	}
	else if((Model_OHT_ID==OHT_NAME_NEO_TRAY_L5) || (Model_OHT_ID==OHT_NAME_NEO_TRAY_HIGH))
	{
		Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
	    Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
	    Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
	    Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
	    Model_OHT_DETECT_READ_BIT_SCALE = 0;
	    Model_OBS_DETECT_READ_BIT_SCALE = 28;

	    Model_DRIVINGPRECHECK = 52;
	    Model_DRIVINGFRONTNUM = 51;
	    Model_TRANSFRONT = 53;
	    Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = true;
		Model_OBS_SENSOR_MAKER = HOKUYO;

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_2_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

	    Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

	    if(Model_OHT_ID == OHT_NAME_PATROL_L5)
	    {
	    	Model_OHT_Service = 1;
	    }
	    else
	    {
	    	Model_OHT_Service = 0;
	    }

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;
		pCBuf[1]->fScurveRatio = 0.1;
		pCBuf[2]->fScurveRatio = 0.1;
		pCBuf[3]->fScurveRatio = 0.5;

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_7_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

		Model_AXIS_STOP_COUNT_LIMIT = 3000;

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;

		Model_PULSETOMM = 113.3736777;

	    Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;
	}
    else if(Model_OHT_ID == OHT_NAME_MODULETRAY)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = SICK;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }
    else if((Model_OHT_ID == OHT_NAME_SDI_EVL)
        || (Model_OHT_ID == OHT_NAME_SDI_EVS) || (Model_OHT_ID == OHT_NAME_SDI_EVL_C))
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 52;
        Model_DRIVINGFRONTNUM = 51;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		// �ش� ���� �߷� ������ ���� ���� ��/���ӵ� ���
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_1_5_MPS2;	//SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_1_2_MPS2;	//SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_1_2_MPS2;	//SPEED_1_5_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        

        // �ش� ���� �߷� ������ ���� ���� ��/���ӵ� ���
        DYNAMIC_ACC_1st = SPEED_1_0_MPS2;
        DYNAMIC_ACC_2nd = SPEED_1_0_MPS2;
        DYNAMIC_ACC_3th = SPEED_1_0_MPS2;
        DYNAMIC_ACC_4th = SPEED_0_7_MPS2;
    }
	//----------------------6.x OHT �߰�----------------------------------------------------
	else if((Model_OHT_ID == OHT_NAME_FOUP_6_0) ||
			(Model_OHT_ID == OHT_NAME_FOUP_6_3) ||
			(Model_OHT_ID == OHT_NAME_RETICLE_2_0) ||
			(Model_OHT_ID == OHT_NAME_PATROL_6_0) ||
			(Model_OHT_ID == OHT_NAME_PATROL_6_3) ||
			(Model_OHT_ID == OHT_NAME_MAC_V60))

    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x0F000000;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000003C;
        Model_DRIVINGPRECHECK = 48;
        Model_DRIVINGFRONTNUM = 43;
        Model_TRANSFRONT = 48;
        Model_TRANSREAR = 48;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 24;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 2;
        Model_OHT_DETECT_READ_BIT_SCALE = 20;
        Model_OBS_DETECT_READ_BIT_SCALE = 27;
		
		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_3_0_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 20; //5  240816 CIS
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_5_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_5_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_3_0_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;
        
        Model_NODE_COUNT_IGNORE_UPPER_LIMIT_INIT =  100;    
		Model_NODE_COUNT_IGNORE_UPPER_LIMIT =  140;        
    }
    else if((Model_OHT_ID == OHT_NAME_FOUP_6_5) ||
            (Model_OHT_ID == OHT_NAME_RETICLE_3_0) ||
            (Model_OHT_ID == OHT_NAME_PATROL_6_5) ||
            (Model_OHT_ID == OHT_NAME_MGZ_1_0) ||
            (Model_OHT_ID == OHT_NAME_FOSB_1_0) ||
            (Model_OHT_ID == OHT_NAME_MGZ_2_0) )

    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x0F000000;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000003C;
        Model_DRIVINGPRECHECK = 44;
        Model_DRIVINGFRONTNUM = 43;
        Model_TRANSFRONT = 48;
        Model_TRANSREAR = 48;

        Model_OHT_DETECT_WRITE_BIT_SCALE = 24;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 2;
        Model_OHT_DETECT_READ_BIT_SCALE = 20;
        Model_OBS_DETECT_READ_BIT_SCALE = 27;

        Model_OHT_SENSOR_ENABLE = true;
        Model_OBS_SENSOR_MAKER = HOKUYO;

        Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
        Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
        Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;
        Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_3_0_MPS2;

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

        Model_OHT_Service = 0;

        Model_FILTER_4_NODECNT = 20; //5  240816 CIS

        pCBuf[0]->fScurveRatio = 0.1;
        pCBuf[1]->fScurveRatio = 0.1;
        pCBuf[2]->fScurveRatio = 0.1;
        pCBuf[3]->fScurveRatio = 0.5;

        Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
        Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_0_MPS;
        Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_5_MPS;
        Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_5_MPS;
        Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_1_MPS;
        Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_5_MPS;
        Model_SPEED_GRADE_7_VELOCITY = VELOCITY_3_0_MPS;
        Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

        Model_AXIS_STOP_COUNT_LIMIT = 3000;

        Model_DIST_ARRIVED_LEVEL1 = 3000;
        Model_DIST_ARRIVED_LEVEL2 = 1000;

        Model_PULSETOMM = 113.3736777;

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;
        
        Model_NODE_COUNT_IGNORE_UPPER_LIMIT_INIT =  140;    
		Model_NODE_COUNT_IGNORE_UPPER_LIMIT =  200;
    }
	else if(Model_OHT_ID == OHT_NAME_TRAY_1_0) 
	{
		Model_OHT_DETECT_WRITE_BIT_MASK = 0x0F000000;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000003C;
        Model_DRIVINGPRECHECK = 44;
        Model_DRIVINGFRONTNUM = 43;
        Model_TRANSFRONT = 48;
        Model_TRANSREAR = 48;

        Model_OHT_DETECT_WRITE_BIT_SCALE = 24;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 2;
        Model_OHT_DETECT_READ_BIT_SCALE = 20;
        Model_OBS_DETECT_READ_BIT_SCALE = 27;
		
		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_5_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_5_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_5_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_3_0_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;
	}
	else if(Model_OHT_ID == OHT_NAME_COMPACT_1_0)
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x00000F00;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x00000078;
        Model_DRIVINGPRECHECK = 54;
        Model_DRIVINGFRONTNUM = 53;
        Model_TRANSFRONT = 0;
        Model_TRANSREAR = 0;
		
        Model_OHT_DETECT_WRITE_BIT_SCALE = 8;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 3;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;
		
		Model_OHT_SENSOR_ENABLE = true;		
		Model_OBS_SENSOR_MAKER = HOKUYO;
		
		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_2_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_3_0_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
		
		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;
		
		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_5_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_1_5_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_3_0_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;		

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;		

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;                
    }
	//----------------------6.x OHT �߰�----------------------------------------------------
    else
    {
        Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
        Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
        Model_OHT_DETECT_READ_BIT_SCALE = 0;
        Model_OBS_DETECT_READ_BIT_SCALE = 28;

        Model_DRIVINGPRECHECK = 50;
        Model_DRIVINGFRONTNUM = 52;
        Model_TRANSFRONT = 53;
        Model_TRANSREAR = 54;

		Model_OHT_SENSOR_ENABLE = false;		
		Model_OBS_SENSOR_MAKER = HOKUYO;		

		Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
		Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
		Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
		Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;		

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;			

		Model_OHT_Service = 0;		

		Model_FILTER_4_NODECNT = 5;

		pCBuf[0]->fScurveRatio = 0.1;		
		pCBuf[1]->fScurveRatio = 0.1;				
		pCBuf[2]->fScurveRatio = 0.1;				
		pCBuf[3]->fScurveRatio = 0.5;						

		Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
		Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
		Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
		Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
		Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
		Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
		Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
		Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;	

		Model_AXIS_STOP_COUNT_LIMIT = 3000;		

		Model_DIST_ARRIVED_LEVEL1 = 3000;
		Model_DIST_ARRIVED_LEVEL2 = 1000;				

		Model_PULSETOMM = 113.3736777;				

        Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;        
    }	    

	if(Model_OHT_ID==OHT_NAME_STD_V80_SERVICE)
	{
		LitzCableIO[0] 		= 40;	// LitzCableDetectError
		LitzCableIO[1] 		= 50;	// LitzCableDetectWarning
		LitzCableIO[2] 		= 5;	// LitzCableTempErrorRight
		LitzCableIO[3] 		= 6;	// LitzCableTempWarningRight
		LitzCableIO[4] 		= 7;	// LitzCableTempErrorLeft
		LitzCableIO[5] 		= 8;	// LitzCableTempWarningLeft
		LitzCableIO[6] 		= 3;	// VisionDetect
		LitzCableIO[7] 		= 41;	// RailMisDetectLowLeft
		LitzCableIO[8] 		= 43;	// RailMisDetectHighLeft
		LitzCableIO[9] 		= 71;	// RailMisDetectLowRight
		LitzCableIO[10] 	= 72;	// RailMisDetectHighRight
	}
	else if(Model_OHT_ID == OHT_NAME_MASK_PATROL)
	{
		LitzCableIO[0] 		= 50;	// LitzCableDetectError
		LitzCableIO[1] 		= 40;	// LitzCableDetectWarning
		LitzCableIO[2] 		= 11;	// LitzCableTempErrorRight
		LitzCableIO[3] 		= 12;	// LitzCableTempWarningRight
		LitzCableIO[4] 		= 13;	// LitzCableTempErrorLeft
		LitzCableIO[5] 		= 14;	// LitzCableTempWarningLeft
		LitzCableIO[6] 		= 3;	// VisionDetect
		LitzCableIO[7] 		= 41;	// RailMisDetectLowLeft
		LitzCableIO[8] 		= 43;	// RailMisDetectHighLeft
		LitzCableIO[9] 		= 60;	// RailMisDetectLowRight
		LitzCableIO[10] 	= 61;	// RailMisDetectHighRight
	}
	else if(Model_OHT_ID==OHT_NAME_PATROL_CHINA)
	{
		LitzCableIO[0] 		= 40;	// LitzCableDetectError
		LitzCableIO[1] 		= 50;	// LitzCableDetectWarning
		LitzCableIO[2] 		= 11;	// LitzCableTempErrorRight
		LitzCableIO[3] 		= 12;	// LitzCableTempWarningRight
		LitzCableIO[4] 		= 13;	// LitzCableTempErrorLeft
		LitzCableIO[5] 		= 14;	// LitzCableTempWarningLeft
		LitzCableIO[6] 		= 3;	// VisionDetect
		LitzCableIO[7] 		= 41;	// RailMisDetectLowLeft
		LitzCableIO[8] 		= 43;	// RailMisDetectHighLeft
		LitzCableIO[9] 		= 60;	// RailMisDetectLowRight
		LitzCableIO[10] 	= 61;	// RailMisDetectHighRight
	}
	else if((Model_OHT_ID==FOUP_V70_REV_01_SERVICE) || (Model_OHT_ID==OHT_NAME_PATROL_L5))
	{
		LitzCableIO[0] 		= 40;	// LitzCableDetectError
		LitzCableIO[1] 		= 50;	// LitzCableDetectWarning
		LitzCableIO[2] 		= 5;	// LitzCableTempErrorRight
		LitzCableIO[3] 		= 6;	// LitzCableTempWarningRight
		LitzCableIO[4] 		= 7;	// LitzCableTempErrorLeft
		LitzCableIO[5] 		= 8;	// LitzCableTempWarningLeft
		LitzCableIO[6] 		= 3;	// VisionDetect
		LitzCableIO[7] 		= 41;	// RailMisDetectLowLeft
		LitzCableIO[8] 		= 43;	// RailMisDetectHighLeft
		LitzCableIO[9] 		= 60;	// RailMisDetectLowRight
		LitzCableIO[10] 	= 61;	// RailMisDetectHighRight
	}

	// Mapping I/O ���� Ȯ��
	PatrolVHLIOCnt = 1;	// ī��Ʈ �� ���� �ʱ�ȭ, ���� ����� �� �ٽ� ī��Ʈ�Ͽ� ���� ������ -> I/O �����Ⱚ ����

	for(int i = 0; i < 32; i++)
	{
		if(LitzCableIO[i] == 0)	break;
		PatrolVHLIOCnt++;
	}

}

////////////////////////////////////////////////////////////////////////
// Function	: Get_Actvel_Margin
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 2.9.3, 120323, syk �Լ� �߰�
////////////////////////////////////////////////////////////////////////
void Get_Oht_Model_id(void)	 
{
	WriteChannel2(AxisDPRAM[0],AD_IntType1, Model_OHT_ID);	
}

////////////////////////////////////////////////////////////////////////
// Function	: AMCStatusGet2
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AMCStatusGet2()		
{
	int ax;
	char cmd, *ptr, *pd, copy_point=0;

	ax = ReadCommandAxis();
	cmd = ReadChannel1(AxisDPRAM[ax], AD_CharType2);

	switch(cmd)
	{
		case 1 :
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-1;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-2;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-3;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-4;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;				
			
		case 2 :
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-5;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-6;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-7;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-8;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 3 :	
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-9;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-10;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-11;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[2]+MOTIONMAKE_MSG)-12;
			copy_point %= MOTIONMAKE_MSG;
			ptr= (char *) &sMotionMakeMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;	

		case 4 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-1;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-2;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-3;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-4;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
		case 5 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-5;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-6;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-7;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-8;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;				
			
		case 6 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-9;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-10;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-11;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-12;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 7 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-13;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-14;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-15;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-16;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 8 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-17;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-18;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-19;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-20;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 9 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-21;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-22;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-23;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-24;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 10 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-25;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-26;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-27;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-28;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 11 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-29;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-30;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-31;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-32;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;		
			
		case 12 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-33;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-34;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-35;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-36;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;			
			
		case 13 :
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-37;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, DIV_LENGTH);
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-38;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+DIV_LENGTH);					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = (dbg_point[1]+MOTIONCAL_MSG)-39;
			copy_point %= MOTIONCAL_MSG;
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*2));					
			memcpy(pd, ptr, DIV_LENGTH);	
			
			copy_point = dbg_point[1];
			ptr= (char *) &sMotionCalMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256+(DIV_LENGTH*3));						
			memcpy(pd, ptr, DIV_LENGTH);
			break;
							
		case 14 :
			copy_point = (dbg_point[0]+EVENT_MSG)-1;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;			
			
		case 15 :
			copy_point = (dbg_point[0]+EVENT_MSG)-2;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 16 :
			copy_point = (dbg_point[0]+EVENT_MSG)-3;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 17 :
			copy_point = (dbg_point[0]+EVENT_MSG)-4;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
	
		case 18 :
			copy_point = (dbg_point[0]+EVENT_MSG)-5;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 19 :
			copy_point = (dbg_point[0]+EVENT_MSG)-6;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 20 :
			copy_point = (dbg_point[0]+EVENT_MSG)-7;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 21 :
			copy_point = (dbg_point[0]+EVENT_MSG)-8;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 22 :
			copy_point = (dbg_point[0]+EVENT_MSG)-9;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 23 :
			copy_point = (dbg_point[0]+EVENT_MSG)-10;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 24 :
			copy_point = (dbg_point[0]+EVENT_MSG)-11;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 25 :
			copy_point = (dbg_point[0]+EVENT_MSG)-12;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 26 :
			copy_point = (dbg_point[0]+EVENT_MSG)-13;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 27 :
			copy_point = (dbg_point[0]+EVENT_MSG)-14;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 28 :
			copy_point = (dbg_point[0]+EVENT_MSG)-15;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 29 :
			copy_point = (dbg_point[0]+EVENT_MSG)-16;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 30 :
			copy_point = (dbg_point[0]+EVENT_MSG)-17;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 31 :
			copy_point = (dbg_point[0]+EVENT_MSG)-18;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 32 :
			copy_point = (dbg_point[0]+EVENT_MSG)-19;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 33 :
			copy_point = (dbg_point[0]+EVENT_MSG)-20;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 34 :
			copy_point = (dbg_point[0]+EVENT_MSG)-21;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;			
			
		case 35 :
			copy_point = (dbg_point[0]+EVENT_MSG)-22;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 36 :
			copy_point = (dbg_point[0]+EVENT_MSG)-23;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 37 :
			copy_point = (dbg_point[0]+EVENT_MSG)-24;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 38 :
			copy_point = (dbg_point[0]+EVENT_MSG)-25;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 39 :
			copy_point = (dbg_point[0]+EVENT_MSG)-26;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 40 :
			copy_point = (dbg_point[0]+EVENT_MSG)-27;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		case 41 :
			copy_point = (dbg_point[0]+EVENT_MSG)-28;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;	
			
		case 42 :
			copy_point = (dbg_point[0]+EVENT_MSG)-29;
			copy_point %= EVENT_MSG;
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;
			
		case 43 :
			copy_point = dbg_point[0];
			ptr= (char *) &sEventMsgBuff[copy_point];
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);										
			memcpy(pd, ptr, 256);
			break;		
			
		default :
			ptr = (char *) &sDbgMsgBuff;
			pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);
			memcpy(pd, ptr, 256);
			break;
	}	
}

