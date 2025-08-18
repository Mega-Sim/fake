#define AXISCTRL_GLOBALS

#include "all.h"

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ClearSWPauseFlag
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ClearSWPauseFlag(int ax)
{
	Pause.Flag[ax]=0;
	
	WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,(pCBuf[ax]->in_pos_flag=1));
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence=0));

	if((SyncMotion.Flag == true) && (ax == 0))
	{
		gIsrSt[0].bMoving      = FALSE;
		gIsrSt[1].bMoving      = FALSE;				
	}
	else
	{
		gIsrSt[ax].bMoving      = FALSE;	
	}
	
	gIsrSt[ax].bPrcsingProf = FALSE;
	pCBuf[ax]->nPrfCtr = 0;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ClearPauseFlag
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ClearPauseFlag(int ax)
{
    Pause.WaitState[ax] = 0;
	Pause.Flag[ax]=0;
	
	WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,(pCBuf[ax]->in_pos_flag=1));
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence=0));

	if((SyncMotion.Flag == true) && (ax == 0))
	{
		gIsrSt[0].bMoving      = FALSE;
		gIsrSt[1].bMoving      = FALSE;				
	}
	else
	{
		gIsrSt[ax].bMoving      = FALSE;	
	}
	
	gIsrSt[ax].bPrcsingProf = FALSE;
	pCBuf[ax]->nPrfCtr = 0; 

	__SetPosLimit(ax);
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: InitIsrVars
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitIsrVars(int ax, int pos )
{
    CONTROLBUF *pCB = pCBuf[ax];

    pCB->nPrfCtr = 0;
    pCB->dtBoxRaw = 0.0;

    pCB->fProfFinalVel = 0.0;
    pCB->fOnTheFlyElapsedPos = 0.0;
    pCB->fAccRaw = 0.0;
    pCB->fBgTgtVel = 0.0;
    pCB->fBgAcc = 0.0;

    pCB->dPosCmd = pos;
    pCB->dPosCmdRaw = pos;
    
	if(ax==0)	
	{
		FollowsPOS = pos;	
		FollowsPOS_Prev = FollowsPOS;	
	}		
	
    pCB->dBgPosCmd = pos;
    pCB->dProfFinalPos = pos;

    InitScurveParams(ax);
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: InitIsrSt
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitIsrSt(int ax)
{
    gIsrSt[ax].bHaltEvent = FALSE;
    gIsrSt[ax].bHoldingVel = FALSE;
    
	if((SyncMotion.Flag == true) && (ax == 0))
	{
		gIsrSt[0].bMoving      = FALSE;
		gIsrSt[1].bMoving      = FALSE;				
	}
	else
	{
		gIsrSt[ax].bMoving      = FALSE;	
	}

    gIsrSt[ax].bCheckInPos = FALSE;
    gIsrSt[ax].bSyncEndPos = FALSE;
    gIsrSt[ax].bPrcsingProf = FALSE;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: GetTotalProfCount
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetTotalProfCount(int ax)
{
    int i=0, sum=0;
    
    // Amc Commfail 관련 Debug Log 추가
    Dpram_Log = SetLog(Dpram_Log, 2, true);  
    WriteLogData(Dpram_Log);
    
    while(i < gIsrPrf[ax].nPhasesLeft)
    {
        sum += gIsrPrf[ax].Phase[i].nAcc;
        i++;
    }

    // Amc Commfail 관련 Debug Log 추가
    Dpram_Log = SetLog(Dpram_Log, 2, false);  
    WriteLogData(Dpram_Log);
    
    return sum;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: StartNewProfile
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void StartNewProfile(int ax)
{
    int end;
    CONTROLBUF *pCB = pCBuf[ax];
    
    gIsrPrf[ax] = gBgPrf[ax];
    end = gIsrPrf[ax].nPhasesLeft-1;
    
    pCB->dProfFinalPos = gIsrPrf[ax].dGoalPos;
    pCB->fProfFinalVel = gIsrPrf[ax].Phase[end].fEndVel;
    gIsrSt[ax].bSyncEndPos = gIsrPrf[ax].Phase[end].bEndByPos;

   	gIsrSt[ax].bPrcsingProf = TRUE;

	if((SyncMotion.Flag == true) && (ax == 0))
	{
		gIsrSt[0].bMoving      = TRUE;
		gIsrSt[1].bMoving      = TRUE;				
	}
	else
	{
		gIsrSt[ax].bMoving      = TRUE;	
	}

    gBgSt[ax].bNewProfileReady = FALSE;
    gBgSt[ax].bHaltTriggered = FALSE;

    WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCB->in_sequence=1));
    WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,pCB->in_pos_flag=0);
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: GenerateVelocity
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GenerateVelocity(int ax, BOOL bFreshStart)
{
	static volatile float fVel0[JPC_AXIS];
	static volatile int counter[JPC_AXIS];
	volatile int nAcc;
	volatile float fVel1, fVelZ1;

	PHASE* pPh = NULL;
	CONTROLBUF* pCB = pCBuf[ax];
	BOOL bEndofPhase = FALSE;

	fVelZ1 = pCB->dtBoxRaw; 

	if(((gIsrPrf[ax].nPhasesLeft > 0) && (gIsrSt[ax].bPrcsingProf)) && (motionProfileMakeingStatus==0))
	{
		if (bFreshStart == TRUE)
		{
			counter[ax] = 1;
			fVel0[ax]   = pCB->dtBoxRaw;
		}

		pPh = &(gIsrPrf[ax].Phase[gIsrPrf[ax].nCurPhase]);
		fVel1 = pPh->fEndVel;
		nAcc = pPh->nAcc;

		if(counter[ax] < nAcc)
		{
			pCB->dtBoxRaw = fVel0[ax] + (fVel1 - fVel0[ax]) * ((float)counter[ax] / nAcc);
			ApplyLimitsFloat(	pCB->dtBoxRaw , fVel0[ax], fVel1);
			counter[ax]++;
		}
		else
		{
			pCB->dtBoxRaw = fVel1;
			fVel0[ax] = fVel1;
			counter[ax] = 1;
			bEndofPhase = TRUE;

            // Amc Commfail 관련 Debug Log 추가
            Dpram_Log = SetLog(Dpram_Log, 3, true);  
            WriteLogData(Dpram_Log);

			while(gIsrPrf[ax].nPhasesLeft)     
			{
				gIsrPrf[ax].nPhasesLeft--;
				gIsrPrf[ax].nCurPhase++;

				if (gIsrPrf[ax].Phase[gIsrPrf[ax].nCurPhase].nAcc > 0)
				{
					break;
				}
			}

            // Amc Commfail 관련 Debug Log 추가
            Dpram_Log = SetLog(Dpram_Log, 3, false);  
            WriteLogData(Dpram_Log);
		}
	}

	// Position command
	pCB->dPosCmdRaw += (pCB->dtBoxRaw + fVelZ1) * 0.5;

	if ((bEndofPhase) && (pPh != NULL) && (pPh->bEndByPos))
	{
		pCB->dPosCmdRaw = pPh->dEndPos;
	}

	// Acceleration
	pCB->fAccRaw = pCB->dtBoxRaw - fVelZ1;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: UpdateDbgStatus
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void UpdateDbgStatus(int q)
{
	short sval;
	
	sDbgMsgBuff[q].jtpos = pCBuf[q]->jtpos;
	sDbgMsgBuff[q].mfGoal_pos = (float) pCBuf[q]->mdRefpos;
	sDbgMsgBuff[q].event_st = pCBuf[q]->event_st;
	sDbgMsgBuff[q].axis_source = pCBuf[q]->axis_source;
	sDbgMsgBuff[q].dac_code = pCBuf[q]->dac_code;
	sDbgMsgBuff[q].vm_accflag = pCBuf[q]->vm_accflag;
	sDbgMsgBuff[q].vm_count = pCBuf[q]->nPrfCtr;
	sDbgMsgBuff[q].vm_bMoveDs = pCBuf[q]->vm_bMoveDs;
	sDbgMsgBuff[q].vm_bPosWait = gIsrSt[q].bSyncEndPos;
	sDbgMsgBuff[q].vm_WaitPos = pCBuf[q]->dProfFinalPos;
	sDbgMsgBuff[q].vm_adv_aord = 0;
	sDbgMsgBuff[q].velcmd = pCBuf[q]->velcmd;
	sDbgMsgBuff[q].velerr = pCBuf[q]->velerr;
	sDbgMsgBuff[q].tqrcmd = pCBuf[q]->tqrcmd;
	sDbgMsgBuff[q].dtBox = (float) pCBuf[q]->dtBox;
	
	sDbgMsgBuff[q].cmd = sDbgCmdBuff[q].cmd;
	sDbgMsgBuff[q].acc = sDbgCmdBuff[q].acc;
	sDbgMsgBuff[q].dcc = sDbgCmdBuff[q].dcc;
	sDbgMsgBuff[q].vel = sDbgCmdBuff[q].vel;
	sDbgMsgBuff[q].pos = sDbgCmdBuff[q].pos;
	sDbgMsgBuff[q].qhead = Q_in_num[q];
	sDbgMsgBuff[q].qtail = Q_out_num[q];
	
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
	
	sDbgMsgBuff[q].wdt_status = sval;
	sDbgMsgBuff[q].servo_status = GetAxisStatus(q);
	
	switch (q)
	{
		case 0:	// main 루프 카운터
			sDbgMsgBuff[q].loop_cnt = sDbgCntBuff.loop_main; 
			break;
			
		case 1: // 200us 인터럽트 카운터
			sDbgMsgBuff[q].loop_cnt = sDbgCntBuff.loop_200us; 
			break;
			
		case 2: // 1ms 인터럽트 카운터
			sDbgMsgBuff[q].loop_cnt = sDbgCntBuff.loop_1ms; 
			break;
			
		case 3: // IPC와의 통신 회수 카운터
			sDbgMsgBuff[q].loop_cnt = sDbgCntBuff.loop_DPRAM; 
			break;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: SetAbortEvent
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetAbortEvent(int ax)
{
	int i=0;
	pCBuf[ax]->g_on = 1;
	
	ServoOff( ax );
	
	pCBuf[ax]->axis_source |= (ST_AMP_POWER_ONOFF);
	pCBuf[ax]->axis_source &= (~ST_OUT_OF_FRAMES);

	pCBuf[ax]->vm_accflag=0;
	pCBuf[ax]->vm_dccflag=0;

	InitBgSt(ax);
	InitIsrSt(ax);    

	Pause.Flag[ax]=0;
	WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,(pCBuf[ax]->in_pos_flag=1));
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence=0));

	Q_out_num[ax]=Q_in_num[ax]=0;
	
	for(i=0; i<MAX_Q_NUM; i++)
	{
		Q_Buf[ax][i].Command=0;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: CheckInPos
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckInPos(void)
{
	int	q;

    for(q=0; q<JPC_AXIS; q++)
    {
        if ((abs(pCBuf[q]->error_z0) <= pCBuf[q]->in_position) && (gIsrSt[q].bCheckInPos))	
        {
            if( ((float)pCBuf[q]->jtpos + (float)pCBuf[q]->working_limit) > 2147483647)
            {
                pCBuf[q]->move_upper_limit = 2147483647;
            }
            else
            {
                pCBuf[q]->move_upper_limit = pCBuf[q]->jtpos + pCBuf[q]->working_limit;
            }

            if( ((float)pCBuf[q]->jtpos - (float)pCBuf[q]->working_limit) < -2147483647)
            {
                pCBuf[q]->move_lower_limit =- 2147483647;
            }
            else 
            {
                pCBuf[q]->move_lower_limit = pCBuf[q]->jtpos - pCBuf[q]->working_limit;
            }

            pCBuf[q]->vel_max_limit = (float)pCBuf[q]->Vel_Limit * 0.1;
            pCBuf[q]->vel_min_limit = (float)pCBuf[q]->Vel_Limit * -0.1;

            pCBuf[q]->vm_bMoveDs = 0;
            pCBuf[q]->checked_back_motion =0;
            pCBuf[q]->vm_prev_vel = 0.0;
            pCBuf[q]->in_sequence = 0;

            WriteChannel1(AxisDPRAM[q],AD_In_Sequence, 0);

            WriteChannel1(AxisDPRAM[q],AD_In_Pos_Flag, (pCBuf[q]->in_pos_flag=1));

            gIsrSt[q].bCheckInPos = FALSE;
        }
    }
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ProcessErrEvents
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ProcessErrEvents( int ax )
{
	CONTROLBUF *pCB = pCBuf[ax];

	if((pCB->event_st != NO_EVENT) && (!pCBuf[ax]->g_on))
	{
		if(pCB->event_st == ABORT_EVENT)
		{
            if(SyncMotion.Flag && (flag4AbortEventStop == 0) && ((SyncMotion.Master == ax) || (SyncMotion.Slave == ax)))
			{
				if((SyncMotion.Master == ax) && (pCBuf[SyncMotion.Slave]->event_st == NO_EVENT) || 
                   (SyncMotion.Slave == ax) && (pCBuf[SyncMotion.Master]->event_st == NO_EVENT) )
				{
					flag4AbortEventStop = 1;
					firstExecute4AbortEventStop = 0;
					count4AbortEventStop = 0;
				}
				else
				{
					ServoOff(SyncMotion.Master);
					ServoOff(SyncMotion.Slave); 
					SetAbortEvent(SyncMotion.Master);
					SetAbortEvent(SyncMotion.Slave);
					
					pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st = pCB->event_st; 
				}				
			}
			else
			{
	            ServoOff(ax);	
                SetAbortEvent(ax);
			}
		}
		else
		{
			if (!gIsrSt[ax].bHaltEvent)
			{
				if (pCB->event_st == E_STOP_EVENT)
				{
		            if(SyncMotion.Flag && ((SyncMotion.Master == ax) || (SyncMotion.Slave == ax)) )
					{
		                gBgSt[SyncMotion.Master].fDecelTime4Halt = pCB->e_stop_rate;
	   	                gBgSt[SyncMotion.Slave].fDecelTime4Halt = pCB->e_stop_rate;
		            }
					else
					{
		                gBgSt[ax].fDecelTime4Halt = pCB->e_stop_rate;
					}
				}					
				else // STOP_EVENT, PAUSE_EVENT
	            {
		            if(SyncMotion.Flag && ((SyncMotion.Master == ax) || (SyncMotion.Slave == ax)) )
					{
		                gBgSt[SyncMotion.Master].fDecelTime4Halt = pCB->stop_rate;
	   	                gBgSt[SyncMotion.Slave].fDecelTime4Halt = pCB->stop_rate;
		            }
					else
					{
		                gBgSt[ax].fDecelTime4Halt = pCB->stop_rate;
					}
	            }

		        if(SyncMotion.Flag && ((SyncMotion.Master == ax) || (SyncMotion.Slave == ax)) )
				{						
		            gBgSt[SyncMotion.Master].bHaltTriggered = TRUE;
		            gIsrSt[SyncMotion.Master].bHaltEvent = TRUE;

		            gBgSt[SyncMotion.Slave].bHaltTriggered = TRUE;
		            gIsrSt[SyncMotion.Slave].bHaltEvent = TRUE;
				}
				else
				{
		            gBgSt[ax].bHaltTriggered = TRUE;
		            gIsrSt[ax].bHaltEvent = TRUE;
				}
			}
		}
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: Control_Int200us
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
interrupt void Control_Int200us( void )
{
	int ax;
	short sval;
	int now_v, pre_v, diff_v;	

	CONTROLBUF * pCB;
	BOOL bNewProfile = FALSE;
	
	RsvrOnOff(0, 1);

    // Amc Commfail 관련 Debug Log 추가
    if(g_pLD.n200usCnt++ > 2000000000)
    {
    	g_pLD.n200usCnt = 0;
    } 
    Dpram_Log = SetLog(Dpram_Log, 1, true);  
    WriteLogData(Dpram_Log);

	if(Count_200us_INT_4_Debug++ > 2000000000)		
	{        
		Count_200us_INT_4_Debug = 0;        
	}	

	if((gIsrSt[0].bMoving == true) && (Follows_MoveDS_Flag != true))
	{
		if((double)((pCBuf[0]->dtBox * 5)/113.4) >= 0.5)
		{
			Follows_MoveDS_Flag = true;
		}
	}
	
	_dsp_clr_wdt_reason(WDT_CONTROL);
	
	if(_dsp_get_wdt_status() >= 0x80)
	{
		for(ax = 0; ax<JPC_AXIS; ax++)
		{
			pCBuf[ax]->event_st = ABORT_EVENT;
			pCBuf[ax]->axis_source |= ST_MUST_REBOOT;
		}
	}

	// __dsplib_call_callback(CALLBACK_INT1);

    ax = 0;
    
    // Amc Commfail 관련 Debug Log 추가
    Dpram_Log = SetLog(Dpram_Log, 4, true);  
    WriteLogData(Dpram_Log);
    do
    {
		bNewProfile = FALSE;

        pCB = pCBuf[ax];
		
        // UpdateDbgStatus(ax);
		
		if(_dsp_get_encoder_direction(ax) != GetEncoderHwDirection(ax))
		{
			pCB->event_st = ABORT_EVENT;
			pCB->axis_source |= ST_HWINPOSITION;

			continue;
		}

		if(pCB->g_on != 0)
		{
			WriteChannel1(AxisDPRAM[ax], AD_In_Sequence, (pCB->in_sequence=0));

			continue;
		}

#if EN_SYNCMOTION_BY_DESIRED_VEL
	    if((SyncMotion.Flag) && (ax == SyncMotion.Slave))
	    {
	        //Calculate target position
	        pCB->dtBox_b = pCB->dtBox;
	        pCB->dtBox = pCBuf[SyncMotion.Master]->dtBox;
	        pCB->dPosCmd = pCBuf[SyncMotion.Master]->dPosCmd;	        
	    }
	    else
#endif 
	    {
	        // Process Profile
	        if((gBgSt[ax].bNewProfileReady) && (motionProfileMakeingStatus==0))
	        {
	            // From org code
                // Pause.WaitState[ax] = 0;

                StartNewProfile(ax);
				
                pCB->nPrfCtr = GetTotalProfCount(ax);
                pCB->nPrfCtr += pCBuf[ax]->nScurveBufSize - 1;

	            bNewProfile = TRUE;
	        }

#if EN_CRUISE_MODE
	        now_v = (int)(pCB->dtBox * 1000);
	        pre_v = (int)(pCB->dtBox_b * 1000);
	        diff_v = now_v - pre_v;

	        if(diff_v > Offset_Pos)
	        {
	        	if(acc_count[ax]++ >= Acc_Offset)
	        	{
	        		currentSpeedMode[ax] = ACC_MODE;
	        	    acc_count[ax] = Acc_Offset;
                    if(ax==0)
                    {
                        stop_mode_cnt = 0;
                    }
                    if((SoftMotionFlag != true) && (ax==0))
                    {
        	            cruise_mode_set = true; 
        	            cruise_mode_cnt = 0;
                    }
	        	}
	        	else
	        	{
	        		cruise_count[ax] = 0;
	        		dcc_count[ax] = 0;
	        	}
	        }	
	        else if(diff_v < Offset_Neg)
	        {
	        	if(dcc_count[ax]++ >= Dcc_Offset)
	        	{
	        		currentSpeedMode[ax] = DCC_MODE;
	        	    dcc_count[ax] = Dcc_Offset;
                    if(ax==0)
                    {
                        stop_mode_cnt = 0;
                    }
                    if((SoftMotionFlag != true) && (ax==0))
                    {
                        cruise_mode_set = false;  
                        cruise_mode_cnt = 0;
                    }
	        	}
	        	else
	        	{
	        		cruise_count[ax] = 0;
	        		acc_count[ax] = 0;
        		}
	        }
	       	else if((diff_v <= Offset_Pos) && (diff_v >= Offset_Neg))
	        {
	        	if(cruise_count[ax]++ >= Cruise_Offset)
	        	{
	        		currentSpeedMode[ax] = CRUISE_MODE;
	        		cruise_count[ax] = Cruise_Offset;
                    if(ax==0)
                    {
                        stop_mode_cnt = 0;
                        if(cruise_mode_cnt++ >= Cruise_Mode_Offset) 
                        {
                            cruise_mode_cnt = Cruise_Mode_Offset;
                        }
                    }
	        	}
				else	        		
				{
	        		acc_count[ax] = 0;
	        		dcc_count[ax] = 0;
	        	}
	        }
	        else
	        {
	        	currentSpeedMode[ax] = NONE_MODE;
	        	cruise_count[ax] = 0;
	        	acc_count[ax] = 0;
	        	dcc_count[ax] = 0;
	        }

            // oht가 뒤로(현재 속도가 0이하인 경우) 이동하는 경우 ACC_MODE와 DCC_MODE를 반대로 입력함
            if(now_v < 0)
            {
                if(currentSpeedMode[ax] == ACC_MODE)
                {
                    currentSpeedMode[ax] = DCC_MODE;
                }
                else if(currentSpeedMode[ax] == DCC_MODE)
                {
                    currentSpeedMode[ax] = ACC_MODE;
                }
            }
#endif

	        if(gIsrSt[ax].bMoving == FALSE)
	        {
#if EN_CRUISE_MODE
	        	currentSpeedMode[ax] = STOP_MODE;
	        	cruise_count[ax] = 0;
	        	acc_count[ax] = 0;
	        	dcc_count[ax] = 0;

                if(ax==0)
                {
                    cruise_mode_set = true;  
                    cruise_mode_cnt = 0;   
                    if(stop_mode_cnt++ >= Stop_Mode_Offset) 
                    {
                        stop_mode_cnt = Stop_Mode_Offset;
                    }
                }
#endif

				Motion_Flag[ax] = MoveNOP;

	            continue;
	        }
        
	        // Target velocity
	        pCB->dtBox_b = pCB->dtBox;
	              
	        GenerateVelocity(ax, bNewProfile);
	        
            pCB->dtBox = ApplyScurveFilter(ax, pCB->dtBoxRaw);
            
	        if(pCB->nPrfCtr == 1)
	        {
	            pCB->dtBox = pCB->fProfFinalVel;

    	        if(pCB->dtBox == 0.0)
	            {
	                pCB->fScurveSum = 0.0; 
    	        }
	        }

	        // Position Cmd
	        pCB->dPosCmd += (pCB->dtBox + pCB->dtBox_b)*0.5;

	        if((pCB->nPrfCtr == 1) && (gIsrSt[ax].bSyncEndPos))
	        {
	            pCB->dPosCmd = pCB->dProfFinalPos;
	            gIsrSt[ax].bSyncEndPos = FALSE;

	        }        

	        // Set appropriate status flags
	        if((pCB->nPrfCtr == 0) && (gIsrSt[ax].bPrcsingProf))
	        {
		        pCB->dPosCmd = (int)pCB->dPosCmd;
				
	            pCB->vm_accflag = pCB->vm_dccflag = 0;
				
				gIsrSt[ax].bPrcsingProf = FALSE;

	            if(pCB->dtBox_b == 0.0)
	            {
	                gIsrSt[ax].bCheckInPos = TRUE;

					if((SyncMotion.Flag == true) && (ax == 0))
					{
						gIsrSt[0].bMoving	   = FALSE;
						gIsrSt[1].bMoving	   = FALSE; 			
					}
					else
					{
						gIsrSt[ax].bMoving		= FALSE;	
					}             
                }

				Motion_Flag[ax] = MoveNOP;
                
	        }
	    
	        if(pCB->nPrfCtr > 0) 
	        {
	            pCB->nPrfCtr--;
	        }
	        
	        if((SyncMotion.Flag == true) && (ax == 0))
	        {
	        	pCBuf[SyncMotion.Slave]->dPosCmd	= pCBuf[SyncMotion.Master]->dPosCmd;
	        	pCBuf[SyncMotion.Slave]->dPosCmdRaw = pCBuf[SyncMotion.Master]->dPosCmdRaw;
	        	
	        	pCBuf[SyncMotion.Slave]->dtBox		= pCBuf[SyncMotion.Master]->dtBox;
	        	pCBuf[SyncMotion.Slave]->dtBoxRaw	= pCBuf[SyncMotion.Master]->dtBoxRaw;
	        	
	        	pCBuf[SyncMotion.Slave]->dtBox_b	= pCBuf[SyncMotion.Master]->dtBox_b;	   		        	        	
	        }
	    }
    }while(++ax < JPC_AXIS);
    Dpram_Log = SetLog(Dpram_Log, 4, false);  
    WriteLogData(Dpram_Log);
	
	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{
		pCB = pCBuf[ax];

		ControlPosition(pCB, ax);
		ControlVelocity(pCB, ax);

        if(gBgSt[ax].bStopUpdatingBgVar == FALSE)
        {
            pCB->dBgPosCmd = pCB->dPosCmdRaw;
            pCB->fBgTgtVel = pCB->dtBoxRaw;
            pCB->fBgAcc = pCB->fAccRaw;
        }  
	}

	if(SyncMotion.Flag)
	{
		pCB = pCBuf[SyncMotion.Master];
		GenerateOutput(pCB, SyncMotion.Master);
		
		pCB = pCBuf[SyncMotion.Slave];
		GenerateOutput(pCB, SyncMotion.Slave);
	}
	
	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{
		if(SyncMotion.Flag && ((ax == SyncMotion.Master) || (ax == SyncMotion.Slave))) 
		{
			continue;
		}
		
		pCB = pCBuf[ax];
		GenerateOutput(pCB, ax);
	}

	// 200601 yjchoe
	// 2wd Vehicle 구분 - 에러 예외 처리
	if((Model_OHT_ID == NEO_TRAY) || (Model_OHT_ID == NEO_MAGAZINE)
			|| (Model_OHT_ID == NEO_SSD) || (Model_OHT_ID == NEO_CASSETTE) || (Model_OHT_ID == FOSB_2WD))
	{
		b2wdFlag = true;
	}
	else	b2wdFlag = false;

	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{
		if(ax != 1 || !b2wdFlag)	// 2wd Vehicle 구분 - 에러 예외 처리
			ProcessErrEvents(ax);
	}

	if(flag4AbortEventStop == 1)
	{
		count4AbortEventStop++;
	}
	
	CheckInPos();

	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{
		pCB = pCBuf[ax];

		if(ax != 1 || !b2wdFlag)	// 2wd Vehicle 구분 - 에러 예외 처리
		{
			CheckServo(pCB, ax);
			CheckSensor(pCB, ax);
			CheckSWInPos(pCB, ax);
		}
		// CheckSWPauseCondition(pCB, ax);

		WriteChannel2(AxisDPRAM[ax] , AD_AxisSource , (short) pCB->axis_source );
	}

	for(ax = 0 ; ax<JPC_AXIS ; ax++)
	{
		WriteChannel(AxisDPRAM[ax], AD_LongType2, _dsp_get_com_velocity(ax));
		WriteChannel(AxisDPRAM[ax], AD_FloatType2, _dsp_get_act_velocity(ax));
		
		sval = _dsp_get_com_velocity_rpm(ax);
		WriteChannel2(AxisDPRAM[ax], AD_Vel, sval);
		
		sval = _dsp_get_act_velocity_rpm(ax);
		WriteChannel2(AxisDPRAM[ax], AD_ActVel, sval);
	}

	// WriteChannel(CommDPRAM, CD_Delay, pGBuf->delay_time);

	__GetIoInData_256(&IO_Input[0], &IO_Input[1], &IO_Input[2], &IO_Input[3], &IO_Input[4], &IO_Input[5], &IO_Input[6], &IO_Input[7]);	
	
	WriteChannel(CommDPRAM, CD_Io_inValue, IO_Input[0]);	
	WriteChannel(CommDPRAM, CD_Io_inValue2, IO_Input[1]);

#if (defined(__AMC_SMD) || defined(__AMC_V70))
	WriteChannel(CommDPRAM, CD_Io_inValue1_1, IO_Input[0]);
	WriteChannel(CommDPRAM, CD_Io_inValue2_1, IO_Input[1]);
	WriteChannel(CommDPRAM, CD_Io_inValue3, IO_Input[2]);
	WriteChannel(CommDPRAM, CD_Io_inValue4, IO_Input[3]);
	WriteChannel(CommDPRAM, CD_Io_inValue5, IO_Input[4]);
	WriteChannel(CommDPRAM, CD_Io_inValue6, IO_Input[5]);
	WriteChannel(CommDPRAM, CD_Io_inValue7, IO_Input[6]);
	WriteChannel(CommDPRAM, CD_Io_inValue8, IO_Input[7]);	
#endif			

#if defined(__AMC_V70)	
	//__Get_Monitering_Power();		
	//Check_Monitering_System();	
#endif		

    *((volatile short *) 0xa0300000) = 0;
    *((volatile short *) 0xa0300020) = 0;
    *((volatile short *) 0xa0300040) = 0;
    *((volatile short *) 0xa0300060) = 0;

	if(++trace_cnt_iteration >= TRACE_CNT_ITER)
	{
		for(ax = 0 ; ax<JPC_AXIS ; ax++)
		{
			pCB = pCBuf[ax];
			Trace_Execute(pCB, ax);
		}
		trace_cnt_iteration = 0;
	}	

	Debug_Flag = 0;

    // Amc Commfail 관련 Debug Log 추가
    Dpram_Log = SetLog(Dpram_Log, 1, false);  
    WriteLogData(Dpram_Log);
    
    RsvrOnOff(0, 0);
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: GetValueWithLimit
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float GetValueWithLimit(float val , float limit)
{
	float ret;
	
	if(val >= limit)
	{
		ret = limit;
	}
	else if(val <= -limit)
	{
		ret = -limit;
	}
	else
	{
		ret = val;
	}
	
	return ret;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ControlVelocity
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ControlVelocity( CONTROLBUF * pCB, int ax )
{
	float velcmd_b, velfbz0, velcmd, p_x_err, ierr, i_x_isum, derr, sum, velerr, vel_ierr;
	
	if( pCB->offset_on == 1 )
	{
		return;			
	}

	if( (pCB->motortype==MOTOR_SERVO)&&(pCB->control_cfg!=CONTROL_VEL) && (pCB->encoder_type==ENCODER_INCREMENTAL)  )     
	{
		pCB->velerrz1 = pCB->velerr;

		// P Control
		velfbz0  = pCB->velfbz0;				// 현재 모터의 속도
		velcmd	 = pCB->velcmd;					// 1ms 위치 제어 루틴에서 계산한 속도 명령
		
		if( pCB->loop_cfg != LOOP_OPEN )
		{			
			velerr = velcmd - velfbz0;
			pCB->velerr = velerr;
			p_x_err = (float)pCB->velpgain * velerr;
		}
		else
		{
			p_x_err = (float)pCB->velpgain * (velcmd - pCB->velcmd_b);	// Open Loop(오), 이전 명령과의 차이, 즉 가속도.
		}

		// I Control 
		velcmd_b = pCB->velcmd_b;				// 이전 속도 명령
		vel_ierr = velcmd_b - velfbz0;			// 이전 속도와 현재 속도의 에러
		pCB->vel_ierr = vel_ierr;
		if( ( pCB->velimode == 1 )||( !gIsrSt[ax].bMoving ) )
		{
			if( pCB->loop_cfg != LOOP_OPEN )
			{
				ierr = pCB->velisum + pCB->vel_ierr;
			}
			else
			{
				ierr = pCB->velisum + velerr;	// Open 루프인 경우 속도 명령에 대한 에러만 고려 
			}
			pCB->velisum = GetValueWithLimit( ierr , (float)pCB->velilimit );	// ISUM VALUE UPDATE
			i_x_isum = (float)pCB->veligain * pCB->velisum;						// Vel IGAIN * ISUM VALUE
			sum = p_x_err + i_x_isum;											// Vel p+i data
		}
		else
		{
			pCB->velisum = 0.0;
			sum = p_x_err;						
		}
		
		// D Control 
		derr = pCB->velerr - pCB->velerrz1;
		sum += (float)pCB->veldgain * derr;	

		// F Control 
		sum += (float)pCB->velfgain * pCB->j_z0;

		pCB->tqrcmd = GetValueWithLimit( (Pid_Dividend * sum) , Pos_Limit );
	}
	else
	{
		// Open Loop에 속도 모드인 경우에 수행됨.
		pCB->tqrcmd = pCB->velcmd;
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckSWLimit
// Brief	: 2011.7.20 다시 원복. 항상 limit을 체크하도록
//            2011.9.26 아래 Limit 체크만 2.5.16 버전에 추가한 것임. Source 설정 이상 현상까지는 수정하였음
//            2011.10.13 2.8.03, 2.8.04 Source 설정 이상 현상까지는 수정하였음
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckSWLimit( CONTROLBUF * pCB , int val, int ax)
{	
	int slave_axis=SyncMotion.Slave;

	// SW upper limit check, 2011.7.25 인코더 값만으로도 확인이 가능하도록 추가?
	if( pCB->swupper_limit < val || pCB->swupper_limit < pCB->jtpos )
	{
		if( pCB->limit_swupper_chk_cnt >= 5 ) 			// 2.5.25v2.8.07통합, 120111, syk, software upper travel limit 필터링 적용(노이즈 방지 대책)
		{
			pCB->axis_source |= ST_X_POS_LIMIT;

  			if( pCB->event_st < pCB->swupper_limitst )
			{
				pCB->event_st = pCB->swupper_limitst;
			}

			// 2011.7.22, Sync 모션에서 master 축에 리미트 에러가 발생하면 Slave 축에도 event를 발생. 
			if( SyncMotion.Flag && (SyncMotion.Master == ax) )
			{
				// pCBuf[slave_axis]->axis_source |= ST_X_POS_LIMIT; // 2.5.25v2.8.07통합, 120111, syk 전자 요청(master의 axis_source를 slave에 공유하지 말것)
				if( pCBuf[slave_axis]->event_st < pCB->event_st )
				{
					pCBuf[slave_axis]->event_st = pCB->event_st;	// 마스터 축의 이벤트를 저장함.
				}
			}
		}
		else
		{
			pCB->limit_swupper_chk_cnt++;		
		}
	}
	else
	{
		pCB->limit_swupper_chk_cnt=0;
	}
	
	// SW lower limit check, 2011.7.25 인코더 값만으로도 확인이 가능하도록 추가?
	if( val < pCB->swlower_limit || pCB->jtpos < pCB->swlower_limit )
	{
		if( pCB->limit_swlower_chk_cnt >= 5 )				 // 2.5.25v2.8.07통합, 120111, syk, software lower travel limit 필터링 적용(노이즈 방지 대책)
		{
			pCB->axis_source |= ST_X_NEG_LIMIT;
            
			if( pCB->event_st < pCB->swlower_limitst )
			{
				pCB->event_st = pCB->swlower_limitst;
			}
	
			// 2011.7.22, Sync 모션에서 master 축에 리미트 에러가 발생하면 Slave 축에도 event를 발생. 
			if( SyncMotion.Flag && (SyncMotion.Master == ax) )
			{
				if( pCBuf[slave_axis]->event_st < pCB->event_st )
				{
					pCBuf[slave_axis]->event_st = pCB->event_st;	// 마스터 축의 이벤트를 저장함.
				}
			}
		}
		else
		{
			pCB->limit_swlower_chk_cnt++;
		}
		
	}
	else
	{
		pCB->limit_swlower_chk_cnt = 0;	
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckSWPauseCondition
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckSWPauseCondition(CONTROLBUF * pCB , int ax)
{
	if (__IsPauseCondition(ax))
	{
		pCB->event_st = PAUSE_EVENT;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: PowerOffState
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void PowerOffState( CONTROLBUF * pCB, int ax )
{
	pCB->mdRefpos = pCB->jtpos;
	pCB->mdRefpos_b = pCB->jtpos;
	pCB->dPosCmd = pCB->jtpos;
	pCB->dPosCmdRaw = pCB->jtpos;

	if(ax==0)
	{
		FollowsPOS =  pCB->jtpos;
		FollowsPOS_Prev = FollowsPOS;			
	}
	
	pCB->velo_z0 = 0.0;
	pCB->error_z0 = 0.0;
	pCB->error_z1 = 0.0;
	pCB->isum = 0.0;
	pCB->velcmd = 0.0;
	pCB->velcmd_b = 0.0;
	pCB->velisum = 0.0;

	pCB->velerr = 0.0;
	pCB->velerrz1 = 0.0;
	pCB->pos_ierr = 0.0;
	pCB->vel_ierr = 0.0;
	pCB->j_z0 = 0.0;

	pCB->limit_err_chk_cnt = 0;			// 2009.11.3(오)
	pCB->limit_curve_chk_cnt = 0;		// 2.5.25v2.8.07통합, 120111, syk 용도 변경
	pCB->limit_swupper_chk_cnt = 0;		// 2.5.25v2.8.07통합, 120111, syk
	pCB->limit_swlower_chk_cnt = 0;		// 2.5.25v2.8.07통합, 120111, syk 	
	
	pCB->vm_prev_vel = 0.0;				// 통합, 120302, syk // move_ds 변경사항 적용에 따른 clear, 이전 vm관련 상태 clear					

    pCB->dtBoxRaw = 0.0;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: Trace_Execute
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Trace_Execute(CONTROLBUF * pCB, int ax)
{
	int traceFlag;

	traceFlag = gIsrSt[ax].bMoving;

	if(trace_on_flag[ax] > 0)
	{
		trace_on_flag[ax]--;
	}

	if(((traceFlag) || (trace_on_flag[ax])) && (!Trace_Update_Status))
	{
		Enqueue(AxisQueue[ax], pCB->jtpos);                         
		Enqueue(AxisQueue[ax], pCB->dPosCmd);		     
		Enqueue(AxisQueue[ax], pCB->dPosCmdRaw);               		
		Enqueue(AxisQueue[ax], pCB->dtBox * FREQ);     //속도 변수 단위를 pulse/count --> pulse/s로 수정  250515           
		Enqueue(AxisQueue[ax], pCB->velfbz0 * FREQ);               
		
		Enqueue(AxisQueue[ax], pCB->dtBoxRaw * FREQ);                 				
		Enqueue(AxisQueue[ax], pCB->dProfFinalPos);	     		
		Enqueue(AxisQueue[ax], Target_Velocity_by_Sensor); 		
		Enqueue(AxisQueue[ax], pCB->nPrfCtr);	         		
		Enqueue(AxisQueue[ax], Count_200us_INT_4_Debug);		     		
				
		Enqueue(AxisQueue[ax], pCB->dac_code);	
		Enqueue(AxisQueue[ax], Motion_Flag[ax]);
		if(ax == 1) Enqueue(AxisQueue[ax], EncTrc); //EncTrc is logged at Driving Rear as it works during moving status
		else        Enqueue(AxisQueue[ax], Model_FILTER_4_NODECNT);
		Enqueue(AxisQueue[ax], (((NodeCount<<16) & 0xffff0000) | (PreCheckCount&0xffff)));    		
		Enqueue(AxisQueue[ax], (((NodeCountState[0].KeepEdgeCounter<<16) & 0xffff0000) | (NodeCountState[3].KeepEdgeCounter&0xffff)));                        
	
		Enqueue(AxisQueue[ax], (((Follows_MoveDS_Flag<<16) & 0xffff0000) | (OHT_PBS_IgnoreFlag&0xffff)));                        
		Enqueue(AxisQueue[ax], Follow_Enable_Flag);
		Enqueue(AxisQueue[ax], currentSpeedMode[ax]);
		Enqueue(AxisQueue[ax], (((SoftMotionFlag<<16) & 0xffff0000) | (count_SoftMotion&0xffff)));    
		Enqueue(AxisQueue[ax], IO_Input[ax]);		     	
		
		
		if(ax == 0)			Enqueue(AxisQueue[0], pGBuf->user_io_out);		     		
		else if(ax == 1)	Enqueue(AxisQueue[1], pGBuf->user_io_out2);		     		
		else if(ax == 2)	Enqueue(AxisQueue[2], pGBuf->user_io_out3);		     				
		else				Enqueue(AxisQueue[3], pGBuf->user_io_out4);

		Enqueue(AxisQueue[ax], (((Debug_Flag << 16) & 0xffff0000) | (MoveCondition & 0xffff)));

		if (ax == 1) Enqueue(AxisQueue[ax], Model_OHT_ID);
		else Enqueue(AxisQueue[ax], AMC_Pause_Control);	

		Enqueue(AxisQueue[ax], pCB->axis_source);		     		
		Enqueue(AxisQueue[ax], pCB->event_st);		     		

		if(traceFlag)
		{
			trace_on_flag[ax] = (int)(((Trace_Data_Buffer_Size-1)/Trace_Data_Count_M)*0.2);			
		}            
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ControlPosition
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ControlPosition( CONTROLBUF * pCB, int ax )
{
	double cmd, myCal_vel, myPos_ierr;
	double myGoal;

	pCB->velcmd_b = pCB->velcmd;		// 현재 속도 명령값을 이전 속도 명령으로 저장

	pCB->error_z1 = pCB->error_z0;		// 현재 위치 에러값을 이전 위치 에러값으로 저장
	
	pCB->mdRefpos_b = pCB->mdRefpos;	// 현재 인터폴레이션 위치값을 이전 인터폴레이션 위치값으로 저장

	pCB->jtpos_b  = pCB->jtpos;			//현재 모터 위치를 이전 모터 위치값으로 저장

	pCB->jtpos 	= ReadCurPos( pCB, ax );// 현재 모터 위치를 읽은 후
	pCB->velfbz0 = (float)( pCB->jtpos - pCB->jtpos_b );	// 현재 모터의 속도를 계산한다.

	if(( pCB->offset_on != 0 )||( pCB->g_on != 0 ))
	{
		// 서보 Off 상태이거나 DAC를 직접 출력하는 상태이면 리턴
		PowerOffState( pCB, ax );

        InitScurveParams(ax);

		WritePulseCommand(ax, 0, 0, 0, 0, 0);	// 2009.10.9(오), 문제는 없으나 안정을 위한 코드
		
		return;
	}

    myGoal = pCB->dPosCmd;

	CheckSWLimit( pCB , (int)myGoal, ax);

	myCal_vel = myGoal - pCB->mdRefpos;			// 속도 계산
	pCB->error_z0 = myGoal - pCB->jtpos;		// 목표 위치와 모터의 현재 위치를 이용한 에러 펄스 계산
	myPos_ierr = pCB->mdRefpos_b - pCB->jtpos;	// 현재 위치와 현재 모터 위치와의 에러 펄스

	pCB->mdRefpos = myGoal;
	
 	pCB->encoder_err = myGoal - pCB->encoder;	// 인코더 펄스와 1:1로 사용하기 위함.(오)

	pCB->j_z0 	= myCal_vel - pCB->velo_z0; 	// 이전 속도와 현재 속도의 차로 가속도 기울기 구함.
	pCB->velo_z0 = myCal_vel;					// 목표 속도값 갱신

	pCB->pos_ierr = myPos_ierr;					// 현재 발생한 에러 펄스값(현재 속도에서 현 위치를 뺀 값)

	cmd = ControlServo( pCB, ax );
	pCB->velcmd = GetValueWithLimit( cmd , Pos_Limit );	// 2007.1.20(오) 16비트 DAC를 사용하기 때문에 Limit 값을 변경함. 

	CheckEnvelope( pCB, ax );

	return;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckEnvelope
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckEnvelope( CONTROLBUF * pCB, int ax )
{
	if(((abs(pCB->encoder_vel)* G_fSampling_Time ) > (pCB->actvel_margin + pCB->profile_limit)) && (pCB->actvel_marginst != 0) && (pCB->g_on == 0))
	{	
		if( pCB->limit_curve_chk_cnt >= pCB->actvel_margintime )  
		{
			pCB->axis_source |= ST_V_LIMIT;
			
			if( pCB->actvel_marginst >= pCB->event_st )
			{
				pCB->event_st = pCB->actvel_marginst;
			}

			//Sync 모션에서 master 축에 velocity 리미트 에러가 발생하면 Slave 축에도 event를 발생. 
			
			int slave_axis=SyncMotion.Slave;
			
			if( SyncMotion.Flag && (SyncMotion.Master == ax) )
			{
				// pCBuf[slave_axis]->axis_source |= ST_V_LIMIT;	//2.8.6(단기적용), 120103, syk 전자 요청(master의 axis_source를 slave에 공유하지 말것)
				
				if( pCBuf[slave_axis]->event_st < pCB->event_st )
				{
					pCBuf[slave_axis]->event_st = pCB->event_st;//마스터 축의 설정 이벤트로 처리
				}
			}
		}
		else
		{
			pCB->limit_curve_chk_cnt++;
		}
	}
	else
	{
		pCB->limit_curve_chk_cnt = 0;   
	}
	
#if !EN_SYNCMOTION_BY_DESIRED_VEL
	if( SyncMotion.Flag && (SyncMotion.Slave == ax) )
	{
		return;
	}	
#endif

	if( (pCB->limit_err_chk <= abs((int)pCB->error_z0)) || ( (pCB->limit_err_chk <= abs((int)pCB->encoder_err)) && (pCB->motortype == MOTOR_STEPPER) ) )
	{
		pCB->limit_err_chk_cnt++;

		if( pCB->limit_err_chk_cnt >= 5 )
		{
			pCB->axis_source |= ST_ERROR_LIMIT;

			if( pCB->limit_err_st >= pCB->event_st )
			{
				pCB->event_st = pCB->limit_err_st;
			}
			
			if( SyncMotion.Flag && (SyncMotion.Master == ax) )
			{
				if( pCBuf[SyncMotion.Slave]->event_st < pCB->event_st )
				{
					pCBuf[SyncMotion.Slave]->event_st = pCB->event_st;					
				}
			}

			if( SyncMotion.Flag && (SyncMotion.Slave == ax) )
			{
				if( pCBuf[SyncMotion.Master]->event_st < pCB->event_st )
				{
					pCBuf[SyncMotion.Master]->event_st = pCB->event_st;
				}
			}
		}
	}
	else
	{
		pCB->limit_err_chk_cnt = 0;		
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckServo
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckServo( CONTROLBUF * pCB, int ax )
{
	int val, st, bAmpFault = 0;
	int srdy, bRdyFail = 0;
	
	if ( pCB->bMustReboot )
	{
		pCB->axis_source |= ST_MUST_REBOOT;

		AmpFaultSet( pCB, ax );	
		return;
	}
	
	val = ReadServoAlarm( ax );
	if (pCB->fault_level == val) 
	{
		bAmpFault = 1;
	}

	srdy = ReadServoSRdy(ax);
	
	if (pCB->inpos_level == srdy)
	{
		AmpFaultSet( pCB, ax );
		bRdyFail = 1;
	}
	
	if (bAmpFault && bRdyFail)
	{
		pCB->bMustReboot = 1;
	}

	if( pCB->g_on != 0 ) 
	{
		return;
	}
	
	st = pCB->in_vcu1;				// BACKUP SERVO STATUS
	pCB->in_vcu1 = val;
	
	if( pCB->in_vcu1 != st )
	{
		pCB->delay_c = 1;
	}
	else
	{
		if( 10 >= pCB->delay_c )
		{
			pCB->delay_c++;
		}
		else
		{
			if( pCB->fault_level == 0 )
			{
				if( 0 == val )	
				{
					AmpFaultSet( pCB, ax );
				}
				else
				{
					AmpFaultReset( pCB, ax );
				}
			}
			else
			{
				if( 1 == val )
				{
					AmpFaultSet( pCB, ax );
				}
				else
				{
					AmpFaultReset( pCB, ax );
				}
			}
		}
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: AmpFaultReset
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AmpFaultReset( CONTROLBUF * pCB, int ax )
{
	if( pCB->amp_event == 0 )
	{
		pCB->axis_source &= (~ST_AMP_FAULT);
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: AmpFaultSet
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AmpFaultSet( CONTROLBUF * pCB, int ax)
{
	pCB->axis_source |= ST_AMP_FAULT;
	
	if( pCB->amp_event >= pCB->event_st )
	{
		pCB->event_st = pCB->amp_event;		
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckSensor
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckSensor( CONTROLBUF * pCB, int ax )
{
	int st, sn_c;

	st = pCB->sn_st1;			
	pCB->sn_st1 = ReadSensor( ax );			// store ax servo state

	if( pCB->sn_st1 != st )
	{				
		// State transition occur
		pCB->sn_c = 1;
	}
	else
	{
		if( (sn_c = (pCB->sn_c+1)) <= 10 )
		{
			// Noise Check(Continue 10 msec)
			pCB->sn_c = sn_c;
		}
		else
		{
			ChkPosLimit( pCB , st , ax);
			ChkHomeLimit( pCB , st, ax);
			ChkNegLimit( pCB , st, ax);
		}
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ChkPosLimit
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkPosLimit( CONTROLBUF * pCB , int st, int ax )
{
	int slave_axis=SyncMotion.Slave;
	int master_axis=SyncMotion.Master;
	
	if( SyncMotion.Flag && (SyncMotion.Master == ax || SyncMotion.Slave == ax) )	// 동기 축 부분
	{
		if( pCBuf[master_axis]->pos_level == 0 )	// Active Low, 센서 동작 레벨은 마스터 축 기준
		{
			if( (POS_SENSOR & pCBuf[master_axis]->sn_st1) == 0 || (POS_SENSOR & pCBuf[slave_axis]->sn_st1) == 0)	
			{
				ChkPosSet( pCB, ax );
			}
			else
			{
				ChkPosReset( pCB, ax );
			}
		}
		else	// Active High
		{
			if( (POS_SENSOR & pCBuf[master_axis]->sn_st1) != 0 || (POS_SENSOR & pCBuf[slave_axis]->sn_st1) != 0)
			{
				ChkPosSet( pCB, ax );
			}
			else
			{
				ChkPosReset( pCB, ax );
			}
		}
	}
	else	// 동기 축이 아닌 부분
	{
		if( pCB->pos_level == 0 )	// Active Low
		{
			if( (POS_SENSOR & st) == 0 )
			{
				ChkPosSet( pCB, ax );
			}
			else
			{
				ChkPosReset( pCB, ax );
			}
		}
		else	// Active High
		{
			if( (POS_SENSOR & st) != 0 )
			{
				ChkPosSet( pCB, ax );
			}
			else
			{
				ChkPosReset( pCB, ax );
			}
		}
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ChkPosReset
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkPosReset( CONTROLBUF * pCB, int ax )
{
	if( pCB->pos_sn_st == 0 )
	{
		pCB->axis_source &= (~ST_POS_LIMIT);
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ChkPosSet
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkPosSet( CONTROLBUF * pCB, int ax )
{
	pCB->axis_source |= ST_POS_LIMIT;

	if( pCB->pos_sn_st >= pCB->event_st )
	{
		pCB->event_st = pCB->pos_sn_st;
	}
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ChkNegLimit
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkNegLimit( CONTROLBUF * pCB , int st, int ax )
{
	// 2011.7.22, Sync 모션에서 master 축에 리미트 에러가 발생하면 Slave 축에도 event를 발생. 
	int slave_axis=SyncMotion.Slave;
	int master_axis=SyncMotion.Master;
	
	if( SyncMotion.Flag && (SyncMotion.Master == ax || SyncMotion.Slave == ax) )	// 동기 축 부분
	{
		if( pCBuf[master_axis]->neg_level == 0 )	// Active Low, 센서 동작 레벨은 마스터 축 기준
		{
			if( (NEG_SENSOR & pCBuf[master_axis]->sn_st1) == 0 || (NEG_SENSOR & pCBuf[slave_axis]->sn_st1) == 0)
			{
				ChkNegSet( pCB, ax );
			}
			else
			{
				ChkNegReset( pCB, ax );
			}
		}
		else	// Active High
		{
			if( (NEG_SENSOR & pCBuf[master_axis]->sn_st1) != 0 || (NEG_SENSOR & pCBuf[slave_axis]->sn_st1) != 0)	
			{
				ChkNegSet( pCB, ax );
			}
			else
			{
				ChkNegReset( pCB, ax );
			}
		}
	}
	else
	{
		if( pCB->neg_level == 0 )
		{
			if( (NEG_SENSOR & st) == 0 )	
			{
				ChkNegSet( pCB, ax );
			}
			else
			{
				ChkNegReset( pCB, ax );
			}
		}
		else
		{
			if( (NEG_SENSOR & st) != 0 )
			{
				ChkNegSet( pCB, ax );
			}
			else
			{
				ChkNegReset( pCB, ax );
			}
		}	
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ChkNegReset
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkNegReset( CONTROLBUF * pCB, int ax )
{
	if( pCB->neg_sn_st == 0 )
	{
		pCB->axis_source &= (~ST_NEG_LIMIT);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ChkNegSet
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkNegSet( CONTROLBUF * pCB, int ax )
{
	pCB->axis_source |= ST_NEG_LIMIT;

	if( pCB->neg_sn_st >= pCB->event_st )
	{
		pCB->event_st = pCB->neg_sn_st;		
	}
	return;
}

////////////////////////////////////////////////////////////////////////
// Function	: ChkHomeLimit
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkHomeLimit( CONTROLBUF * pCB , int st, int ax )
{
	if( pCB->home_level == 0 )
	{
		if( (HOME_SENSOR & st) == 0 )
		{
			ChkHomeSet( pCB, ax );
		}
		else
		{
			ChkHomeReset( pCB, ax );
		}
	}
	else
	{
		if( (HOME_SENSOR & st) != 0 )
		{
			ChkHomeSet( pCB, ax );
		}
		else
		{
			ChkHomeReset( pCB, ax );
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ChkHomeReset
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkHomeReset( CONTROLBUF * pCB, int ax )
{
	if( pCB->home_sn_st == 0 )
	{
		pCB->axis_source &= (~ST_HOME_SWITCH);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: ChkHomeSet
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ChkHomeSet( CONTROLBUF * pCB, int ax)
{
	pCB->axis_source |= ST_HOME_SWITCH;
	
	if( pCB->home_sn_st >= pCB->event_st )
	{
		pCB->event_st = pCB->home_sn_st;
	}
	
	return;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ReadCurPos
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ReadCurPos(CONTROLBUF * pCB, int ax)
{
	int encoder;
	
	encoder = pCB->encoder;           // back up the previous encoder value
	pCB->encoder = __ReadEncoder(ax); // READ encoder values from FPGA ASIC

	Setposition_count[ax]--;
	if(Setposition_count[ax] <= 0)
	{		
		Setposition_count[ax] = 0;	
	}			

#if defined(__AMC_28x)	// FIX Encoder read error
	#define ENC_READ_ERR_ALLOW_CNT 1
	#define ENC_READ_ERR_CLEAR_CNT 500
	#define ENC_TOTAL_ERR_CLEAR_CNT 5000
	static int enc_read_error_cnt[4] = {0,};
	static int enc_read_good_cnt[4] = {0,};
	static int total_error_cnt[4] = {0,};
	static int total_error_clear_cnt[4] = {0,};

	if((abs(pCB->encoder - encoder) > AMC_MAX_PPT*2) && (pCB->g_on==0) && (Setposition_count[ax]==0)/* && (enc_read_error_cnt[ax] < ENC_READ_ERR_ALLOW_CNT)*/)
	{
		pCB->encoder = encoder; // will put n-1's enc value if n's encoder value is too big as a wrong value.
		enc_read_error_cnt[ax]++;   // this fixing of enc value is allowed only one time. So two consecutive wrong enc values will make an error as before.
		enc_read_good_cnt[ax] = 0;
		total_error_cnt[ax]++;
		total_error_clear_cnt[ax] = 0;
		if(total_error_cnt[ax] >= 5)
		{
			total_error_cnt[ax] = 0;
			pCB->axis_source |= ST_ENC_OVF;
			pCB->event_st |= ENC_OVF_EVENT;
		}
	}
	else
	{
		if(total_error_clear_cnt[ax] < ENC_TOTAL_ERR_CLEAR_CNT) total_error_clear_cnt[ax]++;   //10초 동안 발생하지 않으면 total_error_cnt clear시킴 
		else total_error_cnt[ax] = 0;
		//if(enc_read_good_cnt[ax] < ENC_READ_ERR_CLEAR_CNT) enc_read_good_cnt[ax]++; // after checking good count of 5 consecutive values, the error count will be clear.
		//else enc_read_error_cnt[ax] = 0;	                                    // Since then, fixing encoder read error will be allowed again.
	}
#endif

	pCB->encoder_vel = pCB->encoder - encoder;

#if defined(__AMC_28x) // CHECK Encoder value read or corrected
	char encoder_error;
	encoder_error = 0;	    // 에러 없음. 2011.5.5
	if((abs(pCB->encoder_vel) > AMC_MAX_PPT*2) && (pCB->g_on==0) && (Setposition_count[ax]==0))	// Test Code *2 => 확인할 것
	{
		encoder_error = 1;	// 인코더에 이상이 발생한 상태, +/- 모두 체크하기 위해 abs(). 2011.5.5
	}
#endif

	// 2007.10.12, ckyu
	// 이전읽은 인코더값과 이번 인코더값에서 오버플로우가 발생했는지를 검사한다.
#if defined(__AMC_28x)
	if ( (fabs(pCB->jtpos_b - pCB->encoder)  > 0x7fffffff) || encoder_error==1 )
	{
		pCB->axis_source |= ST_ENC_OVF;
		pCB->event_st |= ENC_OVF_EVENT;
	}
#else
	if ( fabs(pCB->jtpos_b - pCB->encoder)  > 0x7fffffff )
	{
		pCB->axis_source |= ST_ENC_OVF;
		pCB->event_st |= ENC_OVF_EVENT;
	}
#endif	

	return pCB->encoder;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: ControlServo
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float ControlServo( CONTROLBUF * pCB, int ax )
{
	float sum, isum;
	
	// P Control 
	if( pCB->loop_cfg == LOOP_OPEN )
	{
		pCB->error_z0 = 0.0;
		sum = (float)pCB->pgain * pCB->velo_z0;		// 속도 명령 * P게인
	}
	else
	{
		sum = (float)pCB->pgain * pCB->error_z0;	// 속도 에러 * P게인
	}

	// I CONTROL
	if( (pCB->igain==0)||((pCB->igain!=0)&&(pCB->posimode!=1)&&(gIsrSt[ax].bMoving)) )
	{
		pCB->isum = 0.0;    						// ISUM BUFFER CLEAR BECAUSE OF IGAIN 0
	}
	else
	{
		if( pCB->loop_cfg == LOOP_CLOSED )
		{
			isum = pCB->isum + pCB->pos_ierr;
		}
		else
		{
			isum = pCB->isum + pCB->error_z0;
		}
		
		pCB->isum = GetValueWithLimit( isum , pCB->ilimit );
		sum += (float)pCB->igain * pCB->isum;			
	}

	// D CONTROL
	if( pCB->loop_cfg == LOOP_CLOSED )
	{
		sum += (float)pCB->dgain * (pCB->error_z0 - pCB->error_z1);
	}

	// FF VELOCITY CONTROL
	sum += (float)pCB->fgain * pCB->velo_z0;

	sum *= Pid_Dividend;

	return sum;
}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: GenerateOutput
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GenerateOutput( CONTROLBUF * pCB, int ax )
{
	int iVal, iTqr, tmp_iTqr, i_p_prof_mrg_vel, i_n_prof_mrg_vel;
	float tmp_V_TrackingFactor;
	int follow_m=0; //2.9.7 120803 syk 후륜 과속 보완  

	if((ax == 2) && (Model_OHT_Service==1))
	{
	    if(dcleanVoltage <= CleanVoltageMinValue)
	    {
	        dcleanVoltage = CleanVoltageMinValue;
	    }
	    
        if(dcleanVoltage >= CleanVoltageMaxValue)
        {
            dcleanVoltage = CleanVoltageMaxValue;
        }
	    
	    iTqr = (int)(dcleanVoltage * CleanVoltageScaleValue);
	    
		iTqr =  ~iTqr + 1;
		
		WriteDAPort(ax, iTqr);		 
		return;
	}	
		
	if( pCB->offset_on == 1 )
	{
		iVal = pCB->dac_bias;    // 최중 출력 제한을 위해.. writedaport함수 내에있는 코드 위치 변경 pCB->V_TrackingFactor안곱해도 될듯. 테스트용임.
		WriteDAPort( ax, iVal );
		return;
	}

	if( pCB->g_on == 0 )
	{
#if EN_SYNCMOTION_BY_DESIRED_VEL
        iTqr = (int)(pCB->tqrcmd);
#else
		// sync mode인 경우 Slave축을 Master 축 데이터로 출력
		if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )
		{
			iTqr = (int)(pCBuf[SyncMotion.Master]->tqrcmd);
		}
		else
		{
			iTqr = (int)(pCB->tqrcmd);	
		}
#endif			
		// V_TrackingFactor
		tmp_V_TrackingFactor =pCB->V_TrackingFactor;
		
		if((tmp_V_TrackingFactor < 0.97) || (tmp_V_TrackingFactor > 1.03))  // 20120316 syk 2.9.02 V_TrackingFactor의 값이 이상한 값으로 설정될 경우 폭주 가능성이 있어 제한함. 97% ~ 103%
		{
			tmp_V_TrackingFactor = 1.0;
			pCB->V_TrackingFactor = 1.0;
		}
		
		tmp_iTqr = iTqr;				 
		iTqr = (int)(tmp_iTqr * tmp_V_TrackingFactor) + pCB->dac_bias;		// 최중 출력 제한을 위해.. writedaport함수 내에있는 코드 위치 변경

		if(pCB->curve_limitst==1)											//20120316 syk 2.9.02 profile margin 적용 					
		{
			// sync mode인 경우 Slave축을 Master 축 데이터로 출력
			if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )
			{
				 i_p_prof_mrg_vel = ((int)(open_loop_gain*pCBuf[SyncMotion.Master]->velo_z0) + pCBuf[SyncMotion.Slave]->curve_limit); // slave의 경우 master의 속도로 motion을 하므로 속도는 master의 속도로 margin을 잡음.
				 i_n_prof_mrg_vel = ((int)(open_loop_gain*pCBuf[SyncMotion.Master]->velo_z0) - pCBuf[SyncMotion.Slave]->curve_limit);					
			
				if(iTqr >i_p_prof_mrg_vel) 
				{
					iTqr = i_p_prof_mrg_vel;
				}
				
				if(iTqr <i_n_prof_mrg_vel) 
				{
					iTqr = i_n_prof_mrg_vel;						
				}
			}
			else 
			{
				i_p_prof_mrg_vel = ((int)(open_loop_gain*pCB->velo_z0) + pCB->curve_limit); // slave의 경우 master의 속도로 motion을 하므로 속도는 master의 속도로 margin을 잡음.
				i_n_prof_mrg_vel = ((int)(open_loop_gain*pCB->velo_z0) - pCB->curve_limit);					
			
				if(iTqr >i_p_prof_mrg_vel)
				{
					iTqr = i_p_prof_mrg_vel;
				}
				
				if(iTqr <i_n_prof_mrg_vel)  
				{
					iTqr = i_n_prof_mrg_vel;
				}
			}
		}
		else
		{
			if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )
			{
				 i_p_prof_mrg_vel = (int)(open_loop_gain*pCBuf[SyncMotion.Master]->velo_z0);
				 i_n_prof_mrg_vel = 0;
			}
			else 
			{
				i_p_prof_mrg_vel = (int)(open_loop_gain*pCB->velo_z0);
				i_n_prof_mrg_vel = 0;
			}			
		}

		if(abs(i_p_prof_mrg_vel) > abs(i_n_prof_mrg_vel))
		{
			tmp_iTqr = abs(i_p_prof_mrg_vel);
			pCB->profile_limit = (int)((tmp_iTqr * 81920)/2019.4);  			
		}
		else
		{
			tmp_iTqr = abs(i_n_prof_mrg_vel);
			pCB->profile_limit = (int)((tmp_iTqr * 81920)/2019.4);  			
		}

		// 토오크 모드인 경우는 다른 수식으로 정리가 필요함.	
		// 아래 계산에서 계산 결과가 32비트를 넘기 때문에 float로 연산하였음. 		
		// 20120229 통합 버젼 syk 32767의 값은 회로상 19.47v(3.3v*5.9)의 값일 때 이고,
		// 서보팩스펙이 3000RPM(409600)일 경우 6V임,그러면 10097, 819200일 경우는 20194임.

		if(ax != 2)
		{
			if( iTqr > ((float)pCBuf[ax]->Vel_Limit * 2019.4) / 81920)
			{
				iTqr = ((float)pCBuf[ax]->Vel_Limit * 2019.4) / 81920;
			}
			else if( iTqr < -((float)pCBuf[ax]->Vel_Limit * 2019.4) / 81920)
			{
				iTqr = -((float)pCBuf[ax]->Vel_Limit * 2019.4) / 81920;
			}
		}
		
		if((pCB->limit_curve_chk_cnt >= (pCB->actvel_margintime/2)) && (pCB->actvel_marginst != 0))  // 2.9.7 120803 syk 후륜 과속 보완 
		{			
			if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )
			{	
				follow_m = (pCBuf[SyncMotion.Master]->encoder_vel* G_fSampling_Time );	 		
				iTqr = (follow_m * 2019.4) / 81920;			
			}
		}

		if (BootPara.st_boot.Encoder_direction[ax] == CIR_CCW)
		{
			iTqr =  ~iTqr + 1;		// 2's 보수, 극성 반전을 위함.
		}
		
		WriteDAPort( ax, iTqr);		
	}
	else
	{
		WriteDAPort( ax, pCB->dac_bias );   // 최중 출력 제한을 위해.. writedaport함수 내에있는 코드 위치 변경
	}

}

#pragma CODE_SECTION(".user_ctrl")
////////////////////////////////////////////////////////////////////////
// Function	: CheckSWInPos
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckSWInPos( CONTROLBUF * pCB, int ax )
{
	float error, encoder;
	
	encoder = (int)((float)pCB->encoder * pCB->ratio);
	error = (int) (pCB->mdRefpos- encoder);

	pCB->inPosIndex <<= 1;
	
	if( abs(error)+abs(pCB->encoder_vel) <= pCB->in_position )
	{
		pCB->inPosIndex |= 0x1;
	}

	if( (pCB->inPosIndex&0x1F) == 0x1F )	
	{
		pCB->axis_source |= ST_SWINPOSITION;
	}
	else
	{
		pCB->axis_source &= (~ST_SWINPOSITION);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: CheckInPosSignal
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void CheckInPosSignal( CONTROLBUF * pCB, int ax )
{
	int in_pos = ReadInPos( ax );
	
	if( in_pos != 1 )	
	{
		pCB->axis_source &= (~ST_HWINPOSITION);
	}
	else			
	{
		pCB->axis_source |= ST_HWINPOSITION;
	}
}		

////////////////////////////////////////////////////////////////////////
// Function	: WriteLogData
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int WriteLogData(int Step)
{
	int ofs=0;
	static char *LogDpramBufBase = (char *)0xb00015D0; // 추가여유dpram (0xb00015D0 ~ 0xb000164f, len=0x80=128bytes)	

	g_pLD.nSignature = Step;
		
	for(ofs=0; ofs<LOG_DATA_SIZE; ofs++)
	{
		*(LogDpramBufBase + ofs) = *((char *)(&g_pLD) + ofs);
	}		
	
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Function	: SetLog
// Brief	:
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int SetLog(int log, int pos, bool setdata)
{
    int data = 0;

    if(pos > 0)
    {
        if(setdata)
            data = log | (0x00000001 << pos-1);
        else
            data = log & (~(0x00000001 << pos-1));
    }
    return data; 
}

