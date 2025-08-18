#define AXISINT_GLOBALS

#include "all.h"

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: LedOn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void LedOn(void)
{
    GPVAL &= ~GPValue(9); 
}
    
////////////////////////////////////////////////////////////////////////
// Function	: LedOff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void LedOff(void)
{
    GPVAL |= GPValue(9); 
}

////////////////////////////////////////////////////////////////////////
// Function	: InitDbgLed
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitDbgLed()
{
	GPEN |= GPValue(9);		// enable GP9
	GPDIR |= GPValue(9);	// GP9 is output
	LedOn();
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ToggleDbgLed
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ToggleDbgLed()
{
	static int bf = 0;
	
	bf ^= 1;
	
	if (bf == 1)
	{
		LedOn(); 
	}
	else
	{
		LedOff();
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: AMC_Int5ms
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AMC_Int5ms(void)
{
	int ax;

	_dsp_clr_wdt_reason(WDT_EXTRA);

	if (_dsp_get_wdt_status() >= 0x80)
	{
		for(ax = 0; ax < JPC_AXIS; ax++) 
		{
			pCBuf[ax]->event_st = ABORT_EVENT;
			pCBuf[ax]->axis_source |= ST_MUST_REBOOT;            
		}
	}

	if (__n5msCnt < 0)
	{
		InitDbgLed();
		__n5msCnt = 0;
	}
	
	if (++__n5msCnt >= 100)
	{
		__n5msCnt = 0;
		ToggleDbgLed();
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: CheckAndClear_PauseRst
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckAndClear_PauseRst(CONTROLBUF* pCB, int ax)
{
	if (pCB->event_st != PAUSE_EVENT)
    {
    	PauseRstrt.cmd[ax] = PAUSERST_NONE;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateEventDbgStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void UpdateEventDbgStatus(char mstime, int q)
{
	short sval;
	
	sEventMsgBuff[mstime][q].jtpos = pCBuf[q]->jtpos;
	sEventMsgBuff[mstime][q].mfGoal_pos = (float) pCBuf[q]->mdRefpos;
	sEventMsgBuff[mstime][q].event_st = pCBuf[q]->event_st;
	sEventMsgBuff[mstime][q].axis_source = pCBuf[q]->axis_source;
	sEventMsgBuff[mstime][q].dac_code = pCBuf[q]->dac_code;
	sEventMsgBuff[mstime][q].vm_accflag = pCBuf[q]->vm_accflag;
	sEventMsgBuff[mstime][q].vm_count = pCBuf[q]->nPrfCtr;
	sEventMsgBuff[mstime][q].vm_flag = 1;
	sEventMsgBuff[mstime][q].vm_bMoveDs = pCBuf[q]->vm_bMoveDs;
	sEventMsgBuff[mstime][q].vm_bPosWait = gIsrSt[q].bSyncEndPos;
	sEventMsgBuff[mstime][q].vm_WaitPos = pCBuf[q]->dProfFinalPos;
	sEventMsgBuff[mstime][q].vm_adv_aord = 0;
	sEventMsgBuff[mstime][q].velerr = pCBuf[q]->velerr;
	sEventMsgBuff[mstime][q].tqrcmd = pCBuf[q]->tqrcmd;
	sEventMsgBuff[mstime][q].dtBox = (float) pCBuf[q]->dtBox;
	
	sEventMsgBuff[mstime][q].encoder_vel = pCBuf[q]->encoder_vel;
	sEventMsgBuff[mstime][q].profile_limit = pCBuf[q]->profile_limit;
	sEventMsgBuff[mstime][q].limit_curve_chk_cnt = (char)pCBuf[q]->limit_curve_chk_cnt;
	sEventMsgBuff[mstime][q].mdBasePos = 0.0;
	sEventMsgBuff[mstime][q].actvel_margin = pCBuf[q]->actvel_margin;	
	
	
	sEventMsgBuff[mstime][q].qhead = Q_in_num[q];
	sEventMsgBuff[mstime][q].qtail = Q_out_num[q];
	
	// wdt reason
	sval = _dsp_get_wdt_status() & 0x0f;
	if( pCBuf[q]->jtpos > pCBuf[q]->move_upper_limit || pCBuf[q]->jtpos < pCBuf[q]->move_lower_limit) 
    {
		sval |= 0x20;
	}
	if (sDbgCmdBuff[q].encoder_type != pCBuf[q]->encoder_type) 
    {
		sval |= 0x40;
	}
	if (sDbgCmdBuff[q].circcw != _dsp_get_encoder_direction(q))
	{
		sval |= 0x80;
	}
	
	sEventMsgBuff[mstime][q].wdt_status = sval;
	sEventMsgBuff[mstime][q].servo_status = GetAxisStatus(q);
	
	switch (q)
	{
    	case 0 :
			sEventMsgBuff[mstime][q].val = dbg_point[2];	
			sEventMsgBuff[mstime][q].loop_cnt = sDbgCntBuff.loop_main; 
			break;
			
    	case 1 :
			sEventMsgBuff[mstime][q].val = dbg_point[1];	
			sEventMsgBuff[mstime][q].loop_cnt = sDbgCntBuff.loop_200us; 
			break;
		
    	case 2 :
			sEventMsgBuff[mstime][q].val = 0;	
			sEventMsgBuff[mstime][q].loop_cnt = sDbgCntBuff.loop_1ms; 
			break;
			
    	case 3 :
			sEventMsgBuff[mstime][q].val = check_count_200us;	
			sEventMsgBuff[mstime][q].loop_cnt = sDbgCntBuff.loop_DPRAM; 
			break;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionCalDbgStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 120803 2.9.7 syk
////////////////////////////////////////////////////////////////////////
void UpdateMotionCalDbgStatus(char point, int ax, float vel, float pos)
{
	sMotionCalMsgBuff[dbg_point[1]].buffer_no1 = dbg_point[1];	
	sMotionCalMsgBuff[dbg_point[1]].cal_sort = point;
	sMotionCalMsgBuff[dbg_point[1]].ax = (char)ax;
	sMotionCalMsgBuff[dbg_point[1]].vel = vel;
	sMotionCalMsgBuff[dbg_point[1]].pos = (int)pos;
	sMotionCalMsgBuff[dbg_point[1]].vm_prev_vel = pCBuf[ax]->vm_prev_vel;
	sMotionCalMsgBuff[dbg_point[1]].vm_vel = gIsrPrf[ax].Phase[gIsrPrf[ax].nCurPhase].fEndVel;
	sMotionCalMsgBuff[dbg_point[1]].vm_count = pCBuf[ax]->nPrfCtr;
	sMotionCalMsgBuff[dbg_point[1]].vm_acc = pCBuf[ax]->vm_acc;
	sMotionCalMsgBuff[dbg_point[1]].vm_dcc = pCBuf[ax]->vm_dcc;
	sMotionCalMsgBuff[dbg_point[1]].vm_accflag = (char)pCBuf[ax]->vm_accflag;
	sMotionCalMsgBuff[dbg_point[1]].vm_dccflag = (char)pCBuf[ax]->vm_dccflag;
	sMotionCalMsgBuff[dbg_point[1]].qhead = (char)Q_in_num[ax];
	sMotionCalMsgBuff[dbg_point[1]].qtail = (char)Q_out_num[ax];
	sMotionCalMsgBuff[dbg_point[1]].ds_cal_point = 0;
	sMotionCalMsgBuff[dbg_point[1]].dtbox = (float)pCBuf[ax]->dtBox;
	sMotionCalMsgBuff[dbg_point[1]].fvel = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].virtual_pos = (float)pCBuf[ax]->mdRefpos;
	sMotionCalMsgBuff[dbg_point[1]].time = sDbgCntBuff.loop_1ms;
	sMotionCalMsgBuff[dbg_point[1]].dac_bias = pCBuf[ax]->dac_bias;
	
	dbg_point[1]++;
	
	if(dbg_point[1]>=MOTIONCAL_MSG)
	{
    	dbg_point[1]=0;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionMake_ResultDbgStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 120803 2.9.7 syk
////////////////////////////////////////////////////////////////////////
void UpdateMotionMake_ResultDbgStatus(char point, char err)
{
	sMotionMakeMsgBuff[dbg_point[2]].buffer_no = dbg_point[2]; 	
	sMotionMakeMsgBuff[dbg_point[2]].motion_sort = (point+check_pccmd);  
	sMotionMakeMsgBuff[dbg_point[2]].err = err;
	sMotionMakeMsgBuff[dbg_point[2]].err_point = error_position;
		
	check_pccmd = 0;
	
	dbg_point[2]++;
	
	if(dbg_point[2]>=MOTIONMAKE_MSG) 
	{
    	dbg_point[2]=0;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionMake_BFDbgStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 120803 2.9.7 syk
////////////////////////////////////////////////////////////////////////
void UpdateMotionMake_BFDbgStatus(int ax, double acc, double dcc, double v, int m)
{
	sMotionMakeMsgBuff[dbg_point[2]].ax = (char)ax;
	sMotionMakeMsgBuff[dbg_point[2]].acc = (float)acc;
	sMotionMakeMsgBuff[dbg_point[2]].dcc = (float)dcc;
	sMotionMakeMsgBuff[dbg_point[2]].vel = (float)v;
	sMotionMakeMsgBuff[dbg_point[2]].pos = m;
	sMotionMakeMsgBuff[dbg_point[2]].virtual_pos = (float)pCBuf[ax]->mdRefpos;
	sMotionMakeMsgBuff[dbg_point[2]].dtBox = (float)pCBuf[ax]->dtBox;
	sMotionMakeMsgBuff[dbg_point[2]].vm_count = pCBuf[ax]->nPrfCtr;
	sMotionMakeMsgBuff[dbg_point[2]].vm_accflag = (char)pCBuf[ax]->vm_accflag;
	sMotionMakeMsgBuff[dbg_point[2]].vm_dccflag = (char)pCBuf[ax]->vm_dccflag;
	sMotionMakeMsgBuff[dbg_point[2]].vm_adv_aord = 0;
	sMotionMakeMsgBuff[dbg_point[2]].time = sDbgCntBuff.loop_1ms;
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionMake_AFDbgStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 120803 2.9.7
////////////////////////////////////////////////////////////////////////
void UpdateMotionMake_AFDbgStatus(int ax, int q_pos1, int q_pos2, int q_pos3, int q_pos4)
{
	sMotionMakeMsgBuff[dbg_point[2]].ds_profile_point = pCBuf[ax]->ds_profile_point;
	sMotionMakeMsgBuff[dbg_point[2]].qhead = (char)Q_in_num[ax];
	sMotionMakeMsgBuff[dbg_point[2]].qtail = (char)Q_out_num[ax];
	sMotionMakeMsgBuff[dbg_point[2]].q_pos1 = q_pos1;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos2 = q_pos2;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos3 = q_pos3;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos4 = q_pos4;
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionMake_DbgStatusClear
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void UpdateMotionMake_DbgStatusClear(void)
{
	sMotionMakeMsgBuff[dbg_point[2]].motion_sort = 0;  
	sMotionMakeMsgBuff[dbg_point[2]].err = 0;
	sMotionMakeMsgBuff[dbg_point[2]].err_point = 0;
	sMotionMakeMsgBuff[dbg_point[2]].ax = 0;
	sMotionMakeMsgBuff[dbg_point[2]].acc = 0.0;
	sMotionMakeMsgBuff[dbg_point[2]].dcc = 0.0;
	sMotionMakeMsgBuff[dbg_point[2]].vel = 0.0;
	sMotionMakeMsgBuff[dbg_point[2]].pos = 0;
	sMotionMakeMsgBuff[dbg_point[2]].virtual_pos = 0.0;
	sMotionMakeMsgBuff[dbg_point[2]].dtBox = 0.0;
	sMotionMakeMsgBuff[dbg_point[2]].vm_count = 0;
	sMotionMakeMsgBuff[dbg_point[2]].vm_accflag = 0;
	sMotionMakeMsgBuff[dbg_point[2]].vm_dccflag = 0;
	sMotionMakeMsgBuff[dbg_point[2]].vm_adv_aord = 0;
	sMotionMakeMsgBuff[dbg_point[2]].time = 0;
	sMotionMakeMsgBuff[dbg_point[2]].ds_profile_point = 0;
	sMotionMakeMsgBuff[dbg_point[2]].qhead = 0;
	sMotionMakeMsgBuff[dbg_point[2]].qtail = 0;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos1 = 0;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos2 = 0;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos3 = 0;
	sMotionMakeMsgBuff[dbg_point[2]].q_pos4 = 0;
	sMotionMakeMsgBuff[dbg_point[2]].buffer_no = 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: UpdateMotionCal_DbgStatusClear
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void UpdateMotionCal_DbgStatusClear(void)	
{
	sMotionCalMsgBuff[dbg_point[1]].buffer_no1 = 0;	
	sMotionCalMsgBuff[dbg_point[1]].cal_sort = 0;
	sMotionCalMsgBuff[dbg_point[1]].ax = 0;
	sMotionCalMsgBuff[dbg_point[1]].vel = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].pos = 0;
	sMotionCalMsgBuff[dbg_point[1]].vm_prev_vel = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].vm_vel = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].vm_count = 0;
	sMotionCalMsgBuff[dbg_point[1]].vm_acc = 0;
	sMotionCalMsgBuff[dbg_point[1]].vm_dcc = 0;
	sMotionCalMsgBuff[dbg_point[1]].vm_accflag = 0;
	sMotionCalMsgBuff[dbg_point[1]].vm_dccflag = 0;
	sMotionCalMsgBuff[dbg_point[1]].qhead = 0;
	sMotionCalMsgBuff[dbg_point[1]].qtail = 0;
	sMotionCalMsgBuff[dbg_point[1]].ds_cal_point = 0;
	sMotionCalMsgBuff[dbg_point[1]].dtbox = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].fvel = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].virtual_pos = 0.0;
	sMotionCalMsgBuff[dbg_point[1]].time = 0;
	sMotionCalMsgBuff[dbg_point[1]].dac_bias = 0;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: AMC_Int1ms
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
interrupt void AMC_Int1ms( void )
{
	int	ax;

    // Amc Commfail 관련 Debug Log 추가
    if(g_pLD.n1msCnt++ > 2000000000)
    {
    	g_pLD.n1msCnt = 0;
    }
    Dpram_Log = SetLog(Dpram_Log, 20, true);  
    WriteLogData(Dpram_Log);

	// sDbgCntBuff.loop_1ms++;
	
	_dsp_clr_wdt_reason(WDT_SUBCONTROL);

	if(_dsp_get_wdt_status() >= 0x80)
	{
		for(ax = 0; ax < JPC_AXIS; ax++) 
		{
			pCBuf[ax]->event_st = ABORT_EVENT;
			pCBuf[ax]->axis_source |= ST_MUST_REBOOT;			
		}
	}
/*
	if( (pCBuf[0]->event_st != MMC_OK) || (pCBuf[1]->event_st != MMC_OK) || (pCBuf[2]->event_st != MMC_OK) || (pCBuf[3]->event_st != MMC_OK) )	
	{
		check_err_time++;
	}
	else
	{
		check_err_time=0;	
	}

	if(check_err_time <6)
	{	
		dbg_point[0]++;
		
		if(dbg_point[0]>=EVENT_MSG)
		{
			dbg_point[0]=0;
		}
	}
	else
	{
		check_err_time = 7;
	}

	for( ax=0 ; ax<JPC_AXIS ; ax++ )
	{
		if(check_err_time <6)
		{
		 	UpdateEventDbgStatus(dbg_point[0], ax);		
		}
	}

	check_count_200us=0;
*/	
	
#if defined(__AMC_28x)
	if (++__5mCnt >= 5)
	{
		__5mCnt = 0;
		AMC_Int5ms();
	}
#endif

	Follows1ms();

    // Amc Commfail 관련 Debug Log 추가
    Dpram_Log = SetLog(Dpram_Log, 20, false);  
WriteLogData(Dpram_Log);

}

#pragma CODE_SECTION(".user_int")
////////////////////////////////////////////////////////////////////////
// Function	: dummy_axisint_c
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void dummy_axisint_c( void )
{
}

////////////////////////////////////////////////////////////////////////
// Function	: __InitGuideBuffer
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __InitGuideBuffer(int np) 
{
	char str[16];
	while (SERIALEX_ReadBytes(np, str, 16, 0));
	
	__nGuideSensorBits = INITIAL_GUIDE_BITS;
	__nRcvCount = 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: InitGuideSensorPort
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitGuideSensorPort(int nport)
{
	__nGPort = nport;

	__InitGuideBuffer(__nGPort);

	SERIALEX_Open(__nGPort);
	SERIALEX_SetPort(__nGPort, 19200, 8, 0, 1);
}

