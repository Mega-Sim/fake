#define AXISMAIN_GLOBALS

#include "all.h"

extern void InitializeFileSystemFunction();
extern void __dsplib_callback_init();
extern volatile int	*ptr_sys_sabs;

////////////////////////////////////////////////////////////////////////
// Function	: InitBdParameter
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitBdParameter(void)
{
	int i;

	for( i=0 ; i<4 ; i++ )
	{
		pCBuf[i]->Vel_Limit = MMC_VEL_LIMIT;

		pCBuf[i]->swlower_limit = MMC_NEG_SW_LIMIT;
		pCBuf[i]->swupper_limit = MMC_POS_SW_LIMIT;

		if( i==4 )	// Open Loop 제어
		{ 
			pCBuf[i]->pgain = 12000*2;	// BootPara.st_boot.PGain[i];       	/* P gain    value      */
			pCBuf[i]->igain = 0;		// BootPara.st_boot.IGain[i];        	/* I gain    value      */
			pCBuf[i]->dgain = 0;		// BootPara.st_boot.DGain[i];        	/* D gain    value      */
			pCBuf[i]->fgain = 0;		// BootPara.st_boot.FGain[i];        	/* F gain    value      */
			pCBuf[i]->ilimit = 1000;	// BootPara.st_boot.ILimit[i];       	/* I_LIMIT   value      */

			pCBuf[i]->velpgain = 0;		// BootPara.st_boot.VPgain[i];       	/* VELP gain    value   */
			pCBuf[i]->veligain = 0; 	// BootPara.st_boot.VIgain[i];        	/* VELI gain    value   */
			pCBuf[i]->veldgain = 0;		// BootPara.st_boot.VDgain[i];        	/* VELD gain    value   */
			pCBuf[i]->velfgain = 0;		// BootPara.st_boot.VFgain[i];        	/* VELF gain    value   */
			pCBuf[i]->velilimit = 1000;	// BootPara.st_boot.VIlimit[i];     	/* VELI_LIMIT   value   */
		}
		else		// Closed Loop 제어
		{
			pCBuf[i]->pgain = 200;		// BootPara.st_boot.PGain[i];       	/* P gain    value      */
			pCBuf[i]->igain = 20;		// BootPara.st_boot.IGain[i];        	/* I gain    value      */
			pCBuf[i]->dgain = 0;		// BootPara.st_boot.DGain[i];        	/* D gain    value      */
			pCBuf[i]->fgain = 0;		// BootPara.st_boot.FGain[i];        	/* F gain    value      */
			pCBuf[i]->ilimit = 1000;	// BootPara.st_boot.ILimit[i];       	/* I_LIMIT   value      */

			pCBuf[i]->velpgain = 1500;	// BootPara.st_boot.VPgain[i];       	/* VELP gain    value   */
			pCBuf[i]->veligain = 10; 	// BootPara.st_boot.VIgain[i];        	/* VELI gain    value   */
			pCBuf[i]->veldgain = 0;		// BootPara.st_boot.VDgain[i];        	/* VELD gain    value   */
			pCBuf[i]->velfgain = 0;		// BootPara.st_boot.VFgain[i];        	/* VELF gain    value   */
			pCBuf[i]->velilimit = 1000;	// BootPara.st_boot.VIlimit[i];     	/* VELI_LIMIT   value   */
		}

		pCBuf[i]->in_position = (float)150.0;		// BootPara.st_boot.In_Position[i];
		pCBuf[i]->limit_err_chk = MMC_ERROR_LIMIT;	// BootPara.st_boot.Error_Limit[i];

		pCBuf[i]->motortype = 0;

		pCBuf[i]->swupper_limitst = NO_EVENT;		// BootPara.st_boot.SwUpper_LimitSt[i];
		pCBuf[i]->swlower_limitst = NO_EVENT;		// BootPara.st_boot.SwLower_LimitSt[i];

		pCBuf[i]->pos_level = LOW;					// BootPara.st_boot.Pos_Level[i];
		pCBuf[i]->neg_level = LOW;					// BootPara.st_boot.Neg_Level[i];
		pCBuf[i]->home_level = LOW;					// BootPara.st_boot.Home_Level[i];
		pCBuf[i]->fault_level = HIGH;				// BootPara.st_boot.Amp_Level[i];
		pCBuf[i]->reset_level = LOW;				// BootPara.st_boot.Amp_Reset_Level[i];
		pCBuf[i]->amp_on_level = LOW;				// BootPara.st_boot.Amp_OnLevel[i]; 

		pCBuf[i]->pos_sn_st = NO_EVENT;				// BootPara.st_boot.Pos_Limit_St[i]; //2008.5.20(오) Abort => No_Event
		pCBuf[i]->neg_sn_st = NO_EVENT;				// BootPara.st_boot.Neg_Limit_St[i];
		pCBuf[i]->home_sn_st = NO_EVENT;			// BootPara.st_boot.Home_Limit_St[i]; 
		
		pCBuf[i]->limit_err_st = NO_EVENT;			// BootPara.st_boot.Error_Limit_St[i];
		pCBuf[i]->amp_event = NO_EVENT;				// ABORT_EVENT;//BootPara.st_boot.Amp_Fault_Event[i];

		pCBuf[i]->encoder_type = 0;					// BootPara.st_boot.Encoder_Cfg[i];
		pCBuf[i]->voltage_type = 0;					// BootPara.st_boot.Voltage_Cfg[i];
		pCBuf[i]->index_req = 0;					// BootPara.st_boot.Home_Index[i];
		
		pCBuf[i]->stop_rate = 100;					// BootPara.st_boot.Stop_Rate[i];
		pCBuf[i]->e_stop_rate = 150;				// BootPara.st_boot.E_Stop_Rate[i];

		pCBuf[i]->control_cfg = 1;					// CONTROL_TRQ;//0;	//BootPara.st_boot.Control_Cfg[i];

		pCBuf[i]->loop_cfg = 0;						// BootPara.st_boot.Loop_Cfg[i];

		pCBuf[i]->posimode = IN_ALWAYS;				// BootPara.st_boot.PosImode[i];
		pCBuf[i]->velimode = IN_ALWAYS;				// BootPara.st_boot.VelImode[i];

		pCBuf[i]->V_TrackingFactor = 1.0;
		pCBuf[i]->dac_bias = 0;
		pCBuf[i]->dac_code = 0;
		
		_dsp_amp_fault_reset(i);
	}
}	
	
////////////////////////////////////////////////////////////////////////
// Function	: SetAliveCheckFlag
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetAliveCheckFlag(int ok)
{
	unsigned char *puc = (unsigned char *) ALIVE_CHK_ADDR1;
	
	if (ok)
	{
		*puc ++ = ALIVE_CHK_BYTE1;
		*puc = ALIVE_CHK_BYTE2;
	}
	else
	{
		*puc ++ = 0;
		*puc = 0;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: AbortEventStopProcess
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AbortEventStopProcess()
{
	int i;
	
	if(flag4AbortEventStop == 1)
	{
		if(firstExecute4AbortEventStop == 0)
		{
			firstExecute4AbortEventStop = 1;
			
			MoveS_Flag = 3;
			Follow_Enable_Flag = false;
			
			SoftMotionFlag = false;
			count_SoftMotion = 0;		

             SoftStopMotionFlag = false;
            count_Soft_StopMotion = 0;
			
			if(pCBuf[SyncMotion.Master]->event_st == ABORT_EVENT)
			{
				ServoOff(SyncMotion.Master);
				SyncMotion.Flag = false;
	
				for(i=0 ; i<5 ; i++)
				{
					if(_dsp_move_s(SyncMotion.Slave, VELOCITY_3_0_MPS) == MMC_OK)
					{
						break;
					}		   
					else
					{
						Delay(1);
					}
				}
				
				if(i>=5)
				{
					forcedMoveStopCommand = true;
					_dsp_move_s(SyncMotion.Slave, VELOCITY_3_0_MPS);
					forcedMoveStopCommand = false;							
				} 
			}
			else
			{
				ServoOff(SyncMotion.Slave);
				
				for(i=0 ; i<5 ; i++)
				{
					if(_dsp_move_s(SyncMotion.Master, VELOCITY_3_0_MPS) == MMC_OK)
					{
						break;
					}		   
					else
					{
						Delay(1);
					}
				}
				
				if(i >= 5)
				{
					forcedMoveStopCommand = true;
					_dsp_move_s(SyncMotion.Master, VELOCITY_3_0_MPS);
					forcedMoveStopCommand = false;							
				} 
			}
		}
	
		if(count4AbortEventStop >= TIME_2_SEC)	 
		{
			if(!pCBuf[0]->g_on)
			{
				ServoOff(0);
				SetAbortEvent(0);
			}
			if(!pCBuf[1]->g_on) 
			{
				ServoOff(1);			
				SetAbortEvent(1);
			}
	
			pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st = ABORT_EVENT;
			
			flag4AbortEventStop = 0;
			count4AbortEventStop = 0;
			firstExecute4AbortEventStop = 0;
		}
	}
}


#pragma CODE_SECTION(".user_main")
////////////////////////////////////////////////////////////////////////
// Function	: CheckDprIntClearAndForceClear
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckDprIntClearAndForceClear(void) // DPR Int Clear
{
	g_pLD.nAutoClear = g_nUseDprInt_AutoClear;
    if(g_nUseDprInt_AutoClear)
    {
		// 1. DPRAM 인터럽트 루틴이 일정 시간(DPRINT_CLEAR_TIMEOUT) 지났음에도 진입이 안되었다면
        if(sDbgCntBuff.loop_DPRAM == g_nOldLoopDpramCnt) //
        {
            g_nDprIntActiveHoldCnt++;
            if(g_nDprIntActiveHoldCnt > DPRINT_CLEAR_TIMEOUT) // 지정된 시간 또는 회수가 지났음에도 DPRAM Interrupt 가 High 상태를 유지한다면
            {  	                                              // 문제가 생긴 것으로 간주 하고 자동 Clear 시도
            	// 2. DPRAM 인터럽트 상태 체크
            	g_anIntStatesReg = GPVAL & 0xFFFF;
                g_nDprIntState = (g_anIntStatesReg >> DPRINT_BIT) & 0x1;

                if(g_nDprIntState==0) // DprInt 상태가 ON (Active Low) --> Dpr Int 가 발생/활성화(ON) 된 경우
                {
                    if(g_nDprIntClearRetryCnt < MAX_DPRINT_CLEAR_COUNT) // Clear 재시도 횟수
                    {
                	// 3. Ack2PC 강제로 날림
                        EchoCommand(ClearDPRInt());

                	// 4-1. Dpr Int 클리어 시도 회수(연속) 증가
                        g_nDprIntClearRetryCnt++;
                        g_ForceClearIntStatus = FC_INT_CLR_CHECKING;
                        // Amc Commfail 관련 Debug Log 추가                        
                        if(g_pLD.nDprIntClrCnt++ > 2000000000)
                        {
                        	g_pLD.nDprIntClrCnt = 0;
                        }
                    }
                    else // DPR Int Clear 명령을 강제로 여러번 날렸음에도 Clear 가 안되었다면 별도로 에러 처리
                    {
                    	// 4-2. 에러 처리 (외부 LED 등으로 출력 표시, 예: 특정 LED 토글)
                    	g_nUseDprInt_AutoClear = 0; // MAX_DPRINT_CLEAR_COUNT회 이상 clear해도 DprInt가 발생하지 않으면 더 이상 Clear시키지 않는다.
                    	g_nUseDprInt_AutoEnableCnt = 0; //이후 Dpr_Int가 이 count만큼 정상 수행 되는 경우, g_nUseDprInt_AutoClear를 Enable                    	
                    	//LED !! Clear retry소진
                    	g_ForceClearIntStatus = FC_INT_CLR_DONE;
                    }
                }
                else // (g_nDprIntState==1) : DprInt 상태가 OFF (Normal High) --> Dpr Int 가 발생하지 않은 경우(상위 PC에서 DPRAM 명령 안 온 경우)
                {
                    g_nDprIntClearRetryCnt = 0; // Dpr Int 상태가 1(Clear) 되었으면 RetryCount RESET.
                    g_nUseDprInt_AutoEnableCnt = 0; //이후 Dpr_Int가 이 count만큼 정상 수행 되는 경우, g_nUseDprInt_AutoClear를 Enable
                    //LED !! 명령이 안 들어옴
                    g_ForceClearIntStatus = FC_INT_CLR_NOCMD;
                }
            }
        }
        else // 정상적으로 DPRAM Int 발생되면 관련 변수 RESET
        {
            g_nDprIntActiveHoldCnt = 0;
            g_nDprIntClearRetryCnt = 0;
            g_ForceClearIntStatus = FC_INT_CLR_NORMAL;
        }
        g_nOldLoopDpramCnt = sDbgCntBuff.loop_DPRAM;   // sDbgCntBuff.loop_DPRAM : Dpr_Int()가 호출 되면 1증가
    }

    g_pLD.status_DprIntClr = g_ForceClearIntStatus;

    /*
    //For Debugging
    switch (g_ForceClearIntStatus)
    {
	case FC_INT_CLR_NORMAL: 
		RsvrOnOff(1, 0);
		RsvrOnOff(2, 1);
		RsvrOnOff(3, 1);
		break;
	case FC_INT_CLR_CHECKING:
		RsvrOnOff(1, 1);
		RsvrOnOff(2, 0);
		RsvrOnOff(3, 1);						
		break;
	case FC_INT_CLR_DONE:
		RsvrOnOff(1, 1);
		RsvrOnOff(2, 1);
		RsvrOnOff(3, 0);
		break;
	case FC_INT_CLR_NOCMD:
		RsvrOnOff(1, 0);
		RsvrOnOff(2, 0);
		RsvrOnOff(3, 0);
		break;
	default : 
		RsvrOnOff(1, 1);
		RsvrOnOff(2, 1);
		RsvrOnOff(3, 1);
		break;
    }
    */
}

////////////////////////////////////////////////////////////////////////
// Function	: NdokMain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void NdokMain( void )
{
	int	ax;
	char i_dbg;
	char str[100];

	SetAliveCheckFlag(0);	

	InitializeFileSystemFunction();

	InitHardware();

	InitPara();

	InitServiceFunction();

	*ptr_icon_oport1 = 0x0000;

	_dsp_amc_init();
	
	__dsplib_callback_init();
	
	ClearDPRInt();

	SetTimer();	

	fpga_ver = (_REG32(0xA0000000)>>16) & 0xffff;
	
	SERIAL_WriteBytes("DSP F/W Ver." VERSION_STRING, 19);
	sprintf(str, "\r\nFPGA Ver. %1d.%02d.%02d\r\n", fpga_ver/10000, (fpga_ver%10000)/100, fpga_ver%100);
	SERIAL_WriteBytes(str, strlen(str));
	SERIAL_WriteBytes("AMC Start!", 10);
	
	_dsp_disable_wdt_reason(WDT_MAINLOOP);
	_dsp_disable_wdt_reason(WDT_EXTRA);
	_dsp_disable_wdt_reason(WDT_SUBCONTROL);
	_dsp_disable_wdt_reason(WDT_CONTROL);
	_dsp_set_wdt_status(0);
	
	for( ax=0 ; ax<JPC_AXIS ; ax++ )
	{ 
		_dsp_clear_status(ax);
		PushDbgCmdBuff(ax, 0, 1000.0, 1000.0, 409600.0, 0);
	}
	
	for( i_dbg=0 ; i_dbg<MOTIONMAKE_MSG ; i_dbg++ )
	{ 
		dbg_point[2]= i_dbg;
		UpdateMotionMake_DbgStatusClear();		
	}
	
	for( i_dbg=0 ; i_dbg<MOTIONCAL_MSG ; i_dbg++ )
	{ 
		dbg_point[1] = i_dbg;
		UpdateMotionCal_DbgStatusClear();		
	}		
	
	dbg_point[0] = 0;
	dbg_point[1] = 0;
	dbg_point[2] = 0;	
			
	SetAliveCheckFlag(1);
	
	while( 1 )
	{
		sDbgCntBuff.loop_main++;
	    // Amc Commfail 관련 Debug Log 추가
	    if(g_pLD.nMainLoopCnt++ > 2000000000)
	    {
	    	g_pLD.nMainLoopCnt = 0;
	    }

	    Dpram_Log = SetLog(Dpram_Log, 5, true);
	    WriteLogData(Dpram_Log);

		_dsp_clr_wdt_reason(WDT_MAINLOOP);
		
		if ( _dsp_get_wdt_status() != 0x00 )
		{
			for (ax = 0; ax < JPC_AXIS; ax++) 
			{
				pCBuf[ax]->event_st = ABORT_EVENT;
				pCBuf[ax]->axis_source |= ST_MUST_REBOOT;
			}
		}
		
		if ( *ptr_sys_sabs != sen_output )
		{
			for (ax = 0; ax < JPC_AXIS; ax++) 
			{
				pCBuf[ax]->event_st = ABORT_EVENT;
				pCBuf[ax]->axis_source |= (ST_HWINPOSITION | ST_MUST_REBOOT);
			}
		}
		
		// DPR Int Clear
		CheckDprIntClearAndForceClear();		

		DoCmdIfExist();

		AbortEventStopProcess();

		Dpram_Log = SetLog(Dpram_Log, 6, true);
		WriteLogData(Dpram_Log);

        ax=0;
        do
        {
            if(gBgSt[ax].bHaltTriggered)
            {
                gBgSt[ax].bHaltTriggered = FALSE;
                 
                MovePNS(ACC_IN_TIME, ax, (float)(gBgSt[ax].fDecelTime4Halt/1000), 0.0);
            }
        }while(++ax < AMC_AXIS);

        Dpram_Log = SetLog(Dpram_Log, 6, false);
        WriteLogData(Dpram_Log);

        Dpram_Log = SetLog(Dpram_Log, 5, false);
        WriteLogData(Dpram_Log);

	}
}

#pragma CODE_SECTION(".user_main");
////////////////////////////////////////////////////////////////////////
// Function	: InitBgSt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitBgSt(int ax)
{
    gBgSt[ax].bHaltTriggered = FALSE;
    gBgSt[ax].bNewProfileReady = FALSE;
    gBgSt[ax].bStopUpdatingBgVar = FALSE;
    gBgSt[ax].fDecelTime4Halt = SPEED_3_0_MPS2;
}

////////////////////////////////////////////////////////////////////////
// Function	: InitPara
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitPara(void)
{
	int	i,j;
	
	i = ReadChannel1(AxisDPRAM[0],CD_Int2Pc);

	for( i=0 ; i<JPC_AXIS ; i++ )
	{
		Q_in_num[i]=Q_out_num[i]=0;
		for(j=0; j<MAX_Q_NUM; j++)
		{
			Q_Buf[i][j].Command=0;
		}
	}

	SyncMotion.Master = 0;
	SyncMotion.Slave = 1;
	SyncMotion.Flag = 0;
	pGBuf->delay_time=0;
	pGBuf->user_io_out = 0;
	pGBuf->user_io_out2 = 0;

#if (defined(__AMC_SMD) || defined(__AMC_V70))	
	// 120831 2.9.8 SYK	
	pGBuf->user_io_out3 = 0;	
	pGBuf->user_io_out4 = 0;
	pGBuf->user_io_out5 = 0;
	pGBuf->user_io_out6 = 0;
	pGBuf->user_io_out7 = 0;
	pGBuf->user_io_out8 = 0;
#endif			

	for( i=0; i<JPC_AXIS ; i++ )
	{
		WriteChannel2( CommDPRAM , (CD_Axis+(2*i)) , 0 );

		pCBuf[i]->event_st=NO_EVENT;

		pCBuf[i]->curve_limit = 2465;	  		// 100,000pps margin (100000*20194)/819200
		pCBuf[i]->curve_limitst = 0;  			// margin설정 ON

		pCBuf[i]->limit_curve_chk_cnt = 0;	
		pCBuf[i]->actvel_margin= 100000;		// 2.9.3 110323 syk 100000pps
		pCBuf[i]->actvel_marginst= NO_EVENT;	// 2.9.3 110323 syk 이벤트 초기값 
		pCBuf[i]->actvel_margintime= 50;		// 2.9.3 110323 syk 연속 초과 타임(10ms)

		pCBuf[i]->limit_swupper_chk_cnt = 0;	// 2.5.25v2.8.07통합, 120111, syk, software upper travel limit 초기화  
		pCBuf[i]->limit_swlower_chk_cnt = 0;	// 2.5.25v2.8.07통합, 120111, syk, software lower travel limit 초기화  							

		pCBuf[i]->offset_on=0;
		Pause.Flag[i]=NO_EVENT;
		
		pCBuf[i]->comRes = 2048;
		pCBuf[i]->encRes = 8192;
		pCBuf[i]->encDir = 1;
		pCBuf[i]->ratio = 1;					// 0.25

		WriteChannel1(AxisDPRAM[i],AD_In_Sequence,(pCBuf[i]->in_sequence=0));
		WriteChannel1(AxisDPRAM[i],AD_In_Pos_Flag,(pCBuf[i]->in_pos_flag=1));

		pCBuf[i]->temp = 0;
		pCBuf[i]->vm_bMoveDs = 0;
		pCBuf[i]->checked_back_motion =0;			
		
		pCBuf[i]->EncoderOffset = 0;
		pCBuf[i]->V_TrackingFactor = 1.0;

#if (defined(__AMC_SMD) || defined(__AMC_V70))		
		pCBuf[i]->VersatileIO = 256;			// 120831 2.9.8 SYK	
#endif		

		InitIsrSt(i);
		InitIsrVars(i, 0);
		InitBgSt(i);
		
		QueueInit(AxisQueue[i]);
		MQueueInit(MAxisQueue[i]);
		
        InitScurveParams(i);

		pCBuf[i]->fScurveRatio = 0.1;
		
		trace_on_flag[i] = 0;
		Marking_Process_Flag[i] = false;
		UseSmallAdd_For_Marking[i] = false;

        currentSpeedMode[i] = STOP_MODE;
        
  		cruise_count[i] = 0;
   		acc_count[i] = 0;
   		dcc_count[i] = 0;      		

		Motion_Flag[i] = MoveNOP;

		Setposition_count[i] = 0;
    }

    cruise_mode_cnt = 0;
    cruise_mode_set = true;
    stop_mode_cnt = 0;

    Pre_Target_Velocity_by_Sensor = 0;
	dcleanVoltage = 0.5;

	AMC_Follow_Control_Status = STATUS_NORMAL;
	AMC_Pause_Control = false;
	PauseCmd = false;
	PauseBy = false;	
	PauseFlag = PAUSED_NONE;
	
	TorqueLimit_Request = 0;
	TorqueLimit_Response = 0;

	Path_Slope_Status = 0;
	Servo_Off_type = 0;	

	Final_Arrival_Flag = 0;

	Model_OHT_Service = 0;
	
	Model_FILTER_4_NODECNT = 5;	

	Model_OHT_ID = -1234;
	
	forcedMoveStopCommand = false;
	followsMoveSFailCnt = 0;
	
	PatrolVHLIOCnt = 1;

	// V8.0 Patrol LitzCable
	for(i=0; i<32; i++)
	{
		LitzCableStatusClear[i] = false;
		PreLitzCableStatus[i] = 1;
		LitzCableStatus[i] = false;
		LitzCableIO[i] = 0;
	}
	
	UBG_Setting_flag = 0;

    Model_OHT_DETECT_WRITE_BIT_MASK = 0x000003C0;
    Model_OBS_DETECT_WRITE_BIT_MASK = 0x0000001E;
    Model_DRIVINGPRECHECK = 50;
    Model_DRIVINGFRONTNUM = 52;
    Model_TRANSFRONT = 53;
    Model_TRANSREAR = 54;
    Model_OHT_DETECT_WRITE_BIT_SCALE = 6;
    Model_OBS_DETECT_WRITE_BIT_SCALE = 1;
    Model_OHT_DETECT_READ_BIT_SCALE = 0;
    Model_OBS_DETECT_READ_BIT_SCALE = 28;
    Model_FOLLOWSDCC = SPEED_3_5_MPS2;

	Model_OHT_SENSOR_ENABLE = true;		
	Model_OBS_SENSOR_MAKER = HOKUYO;	

	Model_MOVE_S_DCC_BY_SENSOR_NORMAL = SPEED_3_5_MPS2;
	Model_MOVE_S_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;
	Model_MOVE_DS_ACC_BY_SENSOR_CURVE = SPEED_1_0_MPS2;		
	Model_MOVE_DS_DCC_BY_SENSOR_CURVE = SPEED_2_5_MPS2;

	Model_SPEED_GRADE_1_VELOCITY = VELOCITY_0_0_MPS;
	Model_SPEED_GRADE_2_VELOCITY = VELOCITY_0_2_MPS;
	Model_SPEED_GRADE_3_VELOCITY = VELOCITY_0_3_MPS;
	Model_SPEED_GRADE_4_VELOCITY = VELOCITY_0_7_MPS;
	Model_SPEED_GRADE_5_VELOCITY = VELOCITY_1_4_MPS;
	Model_SPEED_GRADE_6_VELOCITY = VELOCITY_2_1_MPS;
	Model_SPEED_GRADE_7_VELOCITY = VELOCITY_2_5_MPS;
	Model_SPEED_GRADE_8_VELOCITY = VELOCITY_3_5_MPS;

	ACC_CHANGE_SPEED1 = 1.2;
	ACC_CHANGE_SPEED2 = 3.3;
	ACC_CHANGE_SPEED3 = 5.0;

	DYNAMIC_ACC_1st = SPEED_2_0_MPS2;
	DYNAMIC_ACC_2nd = SPEED_2_0_MPS2;
	DYNAMIC_ACC_3th = SPEED_1_0_MPS2;
	DYNAMIC_ACC_4th = SPEED_0_7_MPS2;

	Model_AXIS_STOP_COUNT_LIMIT = 3000;	

	Model_DIST_ARRIVED_LEVEL1 = 3000;
	Model_DIST_ARRIVED_LEVEL2 = 1000;		

	Model_PULSETOMM = 113.3736777;		

    Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON = 500;    
	
	OHT_DETECT_N_VALUE_Flag	= false;

	Count_200us_INT_4_Debug = 0;
	
	Trace_Update_Status = false;
	trace_cnt_iteration = 0;

	Target_Velocity_by_Sensor = Model_SPEED_GRADE_8_VELOCITY;

    POWER3P3_Status_Cnt = 0;
    POWER5P_Status_Cnt = 0;
    POWER12P_Status_Cnt = 0;
    POWER12M_Status_Cnt = 0;
    
	G_fSampling_Time = 5000;

	Pid_Dividend = 1.953125e-3;
    Pos_Limit = 0x7fff;
    
    userio_mode_select = 0; 

	flag4AbortEventStop = 0;
	count4AbortEventStop = 0;
	firstExecute4AbortEventStop = 0;	

    error_position = 0;
    check_count_200us = 0;
    open_loop_gain = 120.5;     // 120316 2.9.02 syk open loop gain(61700 * Pid_Dividend)

    m_pcaBuff = NULL;
    peDumpBody = NULL;
    uiEepromSaveAddr = EEPROM_MAP_START_OFS;
    check_pccmd = 0;

    ptr_icon_oport1 =  (int *)(0xa0300080);
    ptr_icon_oport2 =  (int *)(0xa0300084);
    ptr_icon_iport  =  (int *)(0xa0300088);
    ptr_icon_lamp   =  (int *)(0xa030008c);
    
    int_change = 0x100000;
    gbNewCmdArrived = FALSE;
    gnNewCmdIndex = 0;
    gnNewCmdAxis = 0;

    __nGuideSensorBits = 0;
    __nGPort = 1;   
    __nRcvCount = 0;
    nSimCount = 0;
    nIOCount = 0;
    __n5msCnt = -1;
    check_err_time=0;
    __5mCnt = 0;

	Follows_MoveDS_Flag = FALSE;

	AMC_Stop_count = 0;
	Rear_servo_Off_flag = FALSE;

	motionProfileMakeingStatus = 0;

    OHT_Detect_ChangeNodeValue = 0;              
    OHT_Detect_AreaSetting = OHT_DETECT_NO_USE;
    OHT_Detect_AreaSetting_For_Curve = OHT_Detect_AreaSetting;
    OHT_Detect_AreaSetting_Flag = FALSE;
    OHT_Detect_AreaSetting_Flag2 = false;

	OHT_PBS_IgnoreSettingFlag = false;
	OHT_PBS_IgnoreFlag = false;
	OHT_PBS_IgnoreChangeNodeValue = 0;
	PBS_Ignore_Flag = false;

	Sensor_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;

	FollowsDCC_CMD = SPEED_3_0_MPS2;	

	tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;
	OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;

	MoveDS_Wait_Count_After_Servo_On = 0;
	MoveDS_Wait_Count_After_Servo_On_Flag = FALSE;

	MoveDS_Wait_Count_After_Stop = 0;
	MoveDS_Wait_Count_After_Stop_Flag = FALSE;

    FollowsACC_Prev = 0;
    FollowsDCC_Prev = 0;
    FollowsVEL_Prev = 0;
    FollowsPOS_Prev = 0;

    SoftMotionFlag = false;
    count_SoftMotion = 0;    

    SoftStopMotionFlag = false;
    count_Soft_StopMotion = 0;

	OHT_DETECT_N_VALUE_Flag = false;
	PauseFlag = PAUSED_NONE;
	Follow_Enable_Flag = false;		

	Debug_Flag = 0;

    FlashType = 0;
    NodeCount = 0;
    PreCheckCount = 0;
    PositionOfDrivingTag = 0;
    PositionOfTransTagFirst = 0;
    PositionOfTransTagFinal = 0;
    PositionOfPreCheck = 0;
    FlagFirstNode = 1;

    // 200601 yjchoe, 2wd Select Flag
    b2wdFlag = false;

    // Amc Commfail 관련 Debug Log 추가
    g_pLD.nSignature = 0;
    g_pLD.n200usCnt = 0;
    g_pLD.n1msCnt = 0;
    g_pLD.nDpramCnt = 0;
    g_pLD.cmd_index = 0;
    g_pLD.nMainLoopCnt = 0;
    g_pLD.nDprIntClrCnt = 0;
    g_pLD.status_DprIntClr = 0;
    Dpram_Log = 0;

    // Amc DprInt retry 관련 초기 변수
    g_nOldLoopDpramCnt = 0;
    g_nDprIntActiveHoldCnt = 0;
    g_anIntStatesReg = 0;
    g_nDprIntClearRetryCnt = 0;
    g_nDprIntState = 0;
    g_nUseDprInt_AutoClear = 0;
    g_nUseDprInt_AutoEnableCnt = 0;
    g_ForceClearIntStatus = FC_INT_CLR_NOCMD;

    // FPGA 감속 정지 관련 감속도 설정 (2018.06.21 by JSH)
	SetDecelOnWD_By_AMC(0, 3.5);
    SetDecelOnWD_By_AMC(1, 3.5);
    SetDecelOnWD_By_AMC(2, 0.0);
    SetDecelOnWD_By_AMC(3, 0.0);

    // FPGA 최소/최대 전압 Reset (2018.06.21 by JSH)
	ResetPeakHoldValue_By_AMC();

	for( i=0 ; i<0xA4 ; i++ )
	{
		WriteChannel1( CommDPRAM, i, (char)0 );
	}

	if( Boot_Checksum_Flag == 1 )
	{
		if (ReadBootParam(&BootPara) == 0) 
		{
			Copy_BootPara_2_Para();
		}
	}
	
	if (ReadBootParam(&BootPara) != 0) 
	{
		char *psz = "Set Boot Param. with initial value. \r\n";
		
		SERIAL_WriteBytes(psz, strlen(psz));
		
		InitBootParam(&BootPara);
		SaveBootParam(&BootPara);
	}
	
	Copy_BootPara_2_Para();

#if defined(__AMC_29x)
	__SetIoOutData(BootPara.st_boot.UserIO_BootValue, 0);
#endif

#if (defined(__AMC_SMD) || defined(__AMC_V70))	
	make_io_mask();	
	__SetIoOutData_256(BootPara.st_boot.UserIO_BootValue, 0, 0, 0, 0, 0, 0, 0);		// 120818 2.9.8 syk FPGA IO 초기화.
#endif
	
	for (i = 0; i < JPC_AXIS; i ++)
	{
		int ccwcw = _dsp_get_encoder_direction(i);
		
		SetEncoderHwDirection(i, ccwcw);
		
		sDbgCmdBuff[i].circcw = ccwcw;
		sDbgCmdBuff[i].encoder_type = pCBuf[i]->encoder_type;
	}

	Pause.Init();
	
	WriteChannel2(CommDPRAM,CD_AxisNum,(int)JPC_AXIS);
	WriteChannel2(CommDPRAM,CD_Ver,(int)VERSION_DSP);

#if (defined(__AMC_SMD) || defined(__AMC_V70))	
	Delay(4000);
#endif	

	ResetEncCounter();
}

////////////////////////////////////////////////////////////////////////
// Function	: NdokOnSerialEvent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void NdokOnSerialEvent(const unsigned char* sStr, int nLen)
{
}

////////////////////////////////////////////////////////////////////////
// Function	: NdokOnIdleEvent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void NdokOnIdleEvent()
{
}

////////////////////////////////////////////////////////////////////////
// Function	: NdokOnLowIdleEvent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void NdokOnLowIdleEvent()
{
}

