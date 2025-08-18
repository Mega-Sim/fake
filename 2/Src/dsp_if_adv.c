#define DSP_IF_ADV_GLOBALS

#include "all.h"

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: InitScurveParams
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitScurveParams(int ax)
{
    CONTROLBUF* pCB = pCBuf[ax];
    
    pCB->nScurveBufSize = 0;
    pCB->nScurveBufIdx = 0;
    pCB->fScurveSum = 0.0;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: InitScurveBuffer
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void InitScurveBuffer(int ax, float initVal)
{
    int i;
	CONTROLBUF* pCB = pCBuf[ax];

    pCB->fScurveSum = initVal* pCB->nScurveBufSize;
    pCB->nScurveBufIdx = 0;
    i=0;
    
    while(i < pCB->nScurveBufSize)
    {
        pCB->fScurveBuf[i++] = initVal;
    }
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ApplyScurveFilter
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
double ApplyScurveFilter(int ax, float fVel)
{
    CONTROLBUF* pCB = pCBuf[ax];

    if(pCB->nScurveBufSize > SCURVE_MIN_BUF_SIZE)
    {
        pCB->fScurveSum = pCB->fScurveSum + fVel - pCB->fScurveBuf[pCB->nScurveBufIdx];
        pCB->fScurveBuf[pCB->nScurveBufIdx] = fVel;
        
        (pCB->nScurveBufIdx)++;
		
        if (pCB->nScurveBufIdx >= pCB->nScurveBufSize)
        {
            pCB->nScurveBufIdx = 0;
        }
        
        fVel = pCB->fScurveSum / pCB->nScurveBufSize;
    }
	
    return fVel;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ApplyLimitsInt
// Brief	: 
// Input	: 
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int ApplyLimitsInt(int val, int low, int high)
{
    return ((val>high) ? high : ((val<low) ? low : val));
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: ApplyLimitsFloat
// Brief	: 
// Input	: 
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float ApplyLimitsFloat(float val, float low, float high)
{
    return ((val>high) ? high : ((val<low) ? low : val));
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: MovePNS
// Brief	: 
// Input	: fAcc = +ve value (Pls/Sec^2), 
//         	  fTgtVelPps = +,-,0  (pls/sec)
//
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int MovePNS(int mode, int ax, float fAcc, float fTgtVelPps)
{
    volatile int i, ret = MMC_OK;
    volatile int nAccCnt;
    volatile float fCurVel, fCurPos, fTgtVel, fMaxSpd;
    
    CONTROLBUF * pCB = pCBuf[ax];
    PHASE *pPhase = &(gBgPrf[ax].Phase[0]);

	if(fTgtVelPps > 0.0)
	{		
		MEnqueue(MAxisQueue[ax], MoveP);  
	}		
	else if(fTgtVelPps < 0.0)
	{
		MEnqueue(MAxisQueue[ax], MoveN);  		
	}		
	else
	{
		MEnqueue(MAxisQueue[ax], MoveS);  		
	}		

    MEnqueue(MAxisQueue[ax], mode);  
    MEnqueue(MAxisQueue[ax], fAcc*1000);  
    MEnqueue(MAxisQueue[ax], fTgtVelPps);  

	if(forcedMoveStopCommand != true)
	{
		if(motionProfileMakeingStatus == 0)
		{
			motionProfileMakeingStatus = 1;
		}
		else
		{
			MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);	
		    MEnqueue(MAxisQueue[ax], MoveS_Flag);  						
			MEnqueue(MAxisQueue[ax], 0);	  
		
		   	for(i=0; i<13; i++)
	        {
		        MEnqueue(MAxisQueue[ax], 11); 
	        }
	        return MMC_ILLEGAL_PARAM_MOVE_DS;
		}	
    }    	        
    
    gBgSt[ax].bNewProfileReady = false;

	gBgSt[ax].bStopUpdatingBgVar = TRUE;

	fCurVel = pCB->fBgTgtVel;
	fCurPos = pCB->dBgPosCmd;

	gBgSt[ax].bStopUpdatingBgVar = FALSE;

	fTgtVelPps = GetValueWithLimit(fTgtVelPps, pCB->Vel_Limit);
	fTgtVel	= fTgtVelPps * PERIOD;	

    MEnqueue(MAxisQueue[ax], fCurVel);  
    MEnqueue(MAxisQueue[ax], fCurPos);  
    MEnqueue(MAxisQueue[ax], pCBuf[ax]->jtpos);  
    MEnqueue(MAxisQueue[ax], MoveS_Flag);  
    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
    MEnqueue(MAxisQueue[ax], 0);      

/*
    if(fCurVel == fTgtVel)
    { 
        for(i=0 ; i<11 ; i++)
        {
            MEnqueue(MAxisQueue[ax], 1);  
        }
            
        return ret;
    }
*/    

    fMaxSpd = (fabsf(fCurVel) > fabsf(fTgtVel)) ? fabsf(fCurVel) : fabsf(fTgtVel);

    if(mode == ACC_IN_RATE) // Acceration -> Accel Count
    {
        nAccCnt = ceilf(fMaxSpd * FREQ * FREQ / fAcc); 
    }
    else
    {	
    	nAccCnt = ceilf(fAcc * FREQ);
    }

    nAccCnt = ApplyLimitsInt(nAccCnt, 0, pCB->Accel_Limit * CNT_PER_MS);
	
    if(fMaxSpd != 0.0)
	{		
	    nAccCnt = INT_WITH_MIN1(fabsf(fTgtVel - fCurVel) * nAccCnt / fMaxSpd);
	}		
	else
	{		
	    nAccCnt = 1;
	}		
	
    pPhase->nAcc = nAccCnt;
    pPhase->fEndVel = fTgtVel;
    pPhase->dEndPos = 0.0;
    pPhase->bEndByPos = FALSE;

    MEnqueue(MAxisQueue[ax], pPhase->nAcc);  
    MEnqueue(MAxisQueue[ax], pPhase->fEndVel*1000);  

    for(i=0 ; i<8 ; i++)
    {
        MEnqueue(MAxisQueue[ax], 0);  
    }

    gBgPrf[ax].nPhasesLeft = 1;
    gBgPrf[ax].nCurPhase = 0;

    if(!gIsrSt[ax].bMoving)
    {
		pCB->fScurveRatio = ApplyLimitsFloat(pCB->fScurveRatio, SCURVE_MIN_SMOOTHING_FACTOR, SCURVE_MAX_SMOOTHING_FACTOR);
	    pCB->nScurveBufSize = ApplyLimitsInt((int)(pCB->fScurveRatio * nAccCnt), 0, SCURVE_MAX_BUF_SIZE);
        InitScurveBuffer(ax, fCurVel);
    }
    
    if(fTgtVelPps > 0.0)
	{		
		Motion_Flag[ax] = MoveP;
	}		
	else if(fTgtVelPps < 0.0)
	{
		Motion_Flag[ax] = MoveN;
	}		
	else
	{
		Motion_Flag[ax] = MoveS;
	}	

    gBgSt[ax].bNewProfileReady = TRUE;

	MoveDS_Flag = 0;	
	motionProfileMakeingStatus = 0;
 
    return ret;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: CalcAccOrDs
// Brief	: Calculates 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float CalcAccOrDs(float fV0, float fV1, float fDsOrAcc)
{
    if (fDsOrAcc == 0.0)
    {
        return 0.0;
    }
    else
    {
        return ((fV1-fV0)*(fV1+fV0)/(2.0*fDsOrAcc));
    }
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: CalcVel4NoCvCase
// Brief	: Calculates the target velocity for accel+decel case i.e.
//            no constant velocity phase. Cv = const. vel.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float CalcVel4TriProf(float fA1, float fA2, float fV0, float fV2, float fDs)
{
    float fV1;

    if ((fDs == 0) || (fA1 == 0) || (fA2 == 0) || (fA1 == fA2) || (SGN(fA1)==SGN(fA2)))
    {
        fV1 = 0.0; 
    }
    else
    {
        if ((fV0 == 0) && (fV2 == 0))
        { 
            fV1 = fDs * 2 * fA1 * fA2 / (fA2 - fA1);
        }
        else
        {
            fV1 = ( fDs + fV0*fV0/(2*fA1) - fV2*fV2/(2*fA2) ) * (2*fA1*fA2) / (fA2 - fA1);
        }

        fV1 = SGN(fDs) * sqrt(fV1);
        if ( fabsf(fV1) <= fabsf(fV0) ) // fV1 must be larger than fV0 for triangular profile
        {
            fV1 = 0.0;
	    }
    }
    return fV1;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: CalcAccCnt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float CalcAccCnt(float fV0, float fV1, float fDs)
{
    return (2 * fDs / (fV0 + fV1));
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: CalcDistance
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float CalcDistance(float fV0, float fV1, int nAccCnt)
{
    return (0.5 * nAccCnt * (fV0 + fV1));
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: MoveDS
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int MoveDS(char mode, int ax, float fAcc1, float fAcc2, float fMaxSpd, int nTgtPos)
{
	volatile int i, j, ret = MMC_OK;
	volatile int nPhNum = 0;
	volatile int nAccCnt1, nAccCnt2, acc[NUM_OF_PHASES];
	volatile float fTemp, fDs, fAcc1Ds, fAcc2Ds;
	volatile float fCurTgtVel, fMaxVel, vel[NUM_OF_PHASES];
	volatile double dCurPosCmd, pos[NUM_OF_PHASES];
	volatile int dAccLimitCalForType1;

    // For debug
    // char str[100];
    
    CONTROLBUF *pCB = pCBuf[ax];
    PHASE *pPh;

    MEnqueue(MAxisQueue[ax], MoveD);  
    MEnqueue(MAxisQueue[ax], mode);  
    MEnqueue(MAxisQueue[ax], fAcc1*1000);  
    MEnqueue(MAxisQueue[ax], fAcc2*1000);  
    MEnqueue(MAxisQueue[ax], fMaxSpd);  
    MEnqueue(MAxisQueue[ax], nTgtPos);  

	if(motionProfileMakeingStatus == 0)
	{
		motionProfileMakeingStatus = 1;
	}
	else
	{
		MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);	
	    MEnqueue(MAxisQueue[ax], MoveDS_Flag);  		
		MEnqueue(MAxisQueue[ax], 0);
	
	   	for(j=0; j<11; j++)
        {
	        MEnqueue(MAxisQueue[ax], 11); 
        }
        return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	
    gBgSt[ax].bNewProfileReady = false;

	for(i=0; i<NUM_OF_PHASES; i++)
	{
		pos[i] = 0;
		acc[i] = 0;
		vel[i] = 0;
	}

    if(SyncMotion.Flag && ((SyncMotion.Master == ax)||(SyncMotion.Slave == ax)))
	{
    	// 200601 yjchoe
    	// 2wd Vehicle 구분 - 에러 예외 처리
	    if((pCBuf[SyncMotion.Master]->g_on != 0) || ((pCBuf[SyncMotion.Slave]->g_on != 0) && !b2wdFlag))
    	{
        	for(j=0; j<14; j++)
	        {
    	        MEnqueue(MAxisQueue[ax], 1); 
	        }
	        
			motionProfileMakeingStatus = 0;
	        return MMC_AMP_FAULT;
    	}
	}
	else
	{
	    if(pCBuf[ax]->g_on != 0)
    	{
        	for(j=0; j<14; j++)
	        {
    	        MEnqueue(MAxisQueue[ax], 1); 
	        }
	        
			motionProfileMakeingStatus = 0;	        
	        return MMC_AMP_FAULT;
    	}
	}
    
    // UpdateMotionMake_BFDbgStatus(ax, fAcc1, fAcc2, fMaxSpd, nTgtPos);

	if((fAcc1 <= 0.) || (fAcc2 <= 0.))
	{
        for(j=0; j<14 ; j++)
        {
            MEnqueue(MAxisQueue[ax], 2); 
        }

		motionProfileMakeingStatus = 0;        
        return MMC_ILLEGAL_PARAM_MOVE_DS;
	}

	if(fMaxSpd <= 0.) 
	{
        for(j=0; j<14 ; j++)
        {
            MEnqueue(MAxisQueue[ax], 3); 
        }
        
		motionProfileMakeingStatus = 0;
        return MMC_ILLEGAL_PARAM_MOVE_DS;
	}

	if(fMaxSpd < PRO_VEL_MIN_LIMIT)			// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정
	{
		 fMaxSpd = PRO_VEL_MIN_LIMIT;	
	}

	// PauseRstrt.SetVMovePosParam(ax, fAcc1, fAcc2, fMaxSpd, nTgtPos, mode);
	
	// PushDbgCmdBuff(ax, DMT_MOVEDS, (float) fAcc1, (float) fAcc2, fMaxSpd, nTgtPos);
	// move_PushDbgCmdBuff(ax, DMT_MOVEDS, (float) fAcc1, (float) fAcc2, fMaxSpd, nTgtPos);	// 2012.2.2 2.5.26 syk 사용자의 move_x명령시 parameter 저장

    gBgSt[ax].bStopUpdatingBgVar = TRUE;

	dCurPosCmd = pCB->dBgPosCmd;
    fCurTgtVel = pCB->fBgTgtVel;
	
    gBgSt[ax].bStopUpdatingBgVar = FALSE;

    gBgPrf[ax].dBasePos = dCurPosCmd;
	gBgPrf[ax].dGoalPos = (double)nTgtPos;
	gBgPrf[ax].fBaseVel = fCurTgtVel;

    fDs = (float)(nTgtPos-dCurPosCmd);

    // Velocity (Speed)
    fMaxSpd = ApplyLimitsFloat(fMaxSpd, 0, pCB->Vel_Limit);
    fMaxVel = SGN(fDs) * fMaxSpd * PERIOD;

    MEnqueue(MAxisQueue[ax], (fCurTgtVel*1000));  
    MEnqueue(MAxisQueue[ax], dCurPosCmd);  
    MEnqueue(MAxisQueue[ax], pCBuf[ax]->jtpos);  
    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
    MEnqueue(MAxisQueue[ax], MoveDS_Flag);  

    if((int)fDs == 0)
    {
    	for(j=0 ; j<9 ; j++)
		{
			MEnqueue(MAxisQueue[ax], 6); 
		}	
		motionProfileMakeingStatus = 0;	        		
        return MMC_OK;
    }

    // Check that you are trying to move in the same direction
	if(gIsrSt[ax].bMoving)
	{	
		if(fabs(fCurTgtVel) > 1 && pCB->dtBox > 1)    // 1 pulse/count
		{	
			if((fMaxVel*fCurTgtVel < 0.0) && (fMaxVel*pCB->dtBox < 0.0))
			{ 
				for(j=0; j<9 ; j++)
				{
					MEnqueue(MAxisQueue[ax], 7); 
				}		
				motionProfileMakeingStatus = 0;	        		
				return MMC_REVERSE_MOTION;
			}
		}
	}	

    // Convert units for acceleration
    fTemp = (fabsf(fCurTgtVel)*FREQ > fMaxSpd) ? (fabsf(fCurTgtVel)*FREQ) : (fMaxSpd); // pls/sec

    if(mode == ACC_IN_TIME)
    {
        fAcc1 = fTemp / fAcc1;
        fAcc2 = fTemp / fAcc2;
    }

	if((ax==0) || (ax==1))
	{
        if((currentSpeedMode[ax] == DCC_MODE) && ((fabsf(fCurTgtVel)*FREQ) < fMaxSpd))
        {
            fAcc1 = ApplyLimitsFloat(fAcc1, SPEED_0_5_MPS2, ACC_DURING_DCC_MODE);
        }
        else
        {
            fAcc1 = ApplyLimitsFloat(fAcc1, SPEED_0_5_MPS2, SPEED_3_5_MPS2);            
        }
        
        fAcc2 = ApplyLimitsFloat(fAcc2, SPEED_0_5_MPS2, SPEED_3_5_MPS2);	        
	}		

    // pls/sec^2 -> pls/cnt^2, apply limits
    fAcc1 *= SGN(fDs)*PERIOD*PERIOD;
    fAcc2 *= -SGN(fDs)*PERIOD*PERIOD;

    if(fabsf(fCurTgtVel) > fabsf(fMaxVel))
    {
        fAcc1 = fAcc2;
    }
    
    //Do not use equality for float type eg. if (fMaxVel == fCurTgtVel)
    if(fabsf(fMaxVel-fCurTgtVel) < ZP0001MPS)
    {
        fAcc1 = 0;
    }
	
    if(fabsf(fMaxVel-fCurTgtVel) < fabsf(fAcc1))
    {
        fAcc1 = fMaxVel - fCurTgtVel;
    }

	if(fabsf(fMaxVel) < fabsf(fAcc2))
	{
		fAcc2 = fMaxVel;
	}

    if((fMaxVel*fAcc2) == 0) 
    {
    	for(j=0; j<9; j++)
		{
			MEnqueue(MAxisQueue[ax], 8); 
		}	
		motionProfileMakeingStatus = 0;	        		
        return MMC_ILLEGAL_PARAM_MOVE_DS;   // 속도나 가속도 a2가 0인 경우 
    }

    fAcc1Ds = CalcAccOrDs(fCurTgtVel, fMaxVel, fAcc1); 
    fAcc2Ds = CalcAccOrDs(fMaxVel, 0.0, fAcc2);

    if(fabsf(fDs) < fabsf(fAcc1Ds + fMaxVel + fAcc2Ds))
    {
        fTemp = CalcVel4TriProf(fAcc1, fAcc2, fCurTgtVel, 0.0, fDs);

        if (fTemp == 0.0)
        {
            // Do the decel (Type1)
            nAccCnt2 = INT_WITH_MIN1(CalcAccCnt(fCurTgtVel, 0.0, fDs));

			if((ax==0) || (ax==1))
			{				
				dAccLimitCalForType1 = (int)(((double)fCurTgtVel*FREQ)/((double)SPEED_4_0_MPS2/FREQ));
			}
			else if(ax==2)
			{
				dAccLimitCalForType1 = (int)(((double)fCurTgtVel*FREQ)/((double)107827.0/FREQ));				
			}

			if(((ax==0)||(ax==1)) && (Marking_Process_Flag[0] == true) && (fCurTgtVel > SPEED_0_04_MS_CPI) && (nAccCnt2 < (dAccLimitCalForType1-1)))
			{
				for(j=0; j<9; j++)
				{
					MEnqueue(MAxisQueue[ax], 9); 
				}	
				motionProfileMakeingStatus = 0;	        
				return MMC_ILLEGAL_PARAM_MOVE_DS;  
			}
					
			if((ax==2) && (Marking_Process_Flag[2] == true) && (nAccCnt2 < (dAccLimitCalForType1-1)))
			{
				for(j=0; j<9; j++)
				{
					MEnqueue(MAxisQueue[ax], 9); 
				}	
				motionProfileMakeingStatus = 0;	        				
				return MMC_ILLEGAL_PARAM_MOVE_DS;  
			}			

            acc[0] = nAccCnt2;    
            vel[0] = 0.0;
            pos[0] = nTgtPos;
            
            nPhNum = 1;
        }
        else
        {
            // Do the accel + decel (No const. vel.)
            if(fabsf(fTemp) < fabsf(fMaxVel))
            {
                fMaxVel = fTemp;
            }
            nAccCnt1 = INT_WITH_MIN1((fMaxVel - fCurTgtVel)/fAcc1);

            vel[0] = fMaxVel;   
            vel[1] = 0.0;

            pos[0] = dCurPosCmd + CalcDistance(fCurTgtVel, fMaxVel, nAccCnt1);
            pos[1] = nTgtPos;

            acc[0] = nAccCnt1;
            acc[1] = INT_WITH_MIN1(CalcAccCnt(fMaxVel, 0.0, pos[1]-pos[0]));

            nPhNum = 2;
        }
    }
    else
    {
	    if(fabsf(fMaxVel-fCurTgtVel) < ZP0001MPS)
        {
            nAccCnt2 = INT_WITH_MIN1((0.0-fMaxVel)/fAcc2);
            fTemp = fDs - CalcDistance(fMaxVel, 0.0, nAccCnt2);
            acc[0] = INT_WITH_MIN1(fTemp/fMaxVel);

            // Do the const. vel + decel
            pos[0] = dCurPosCmd + acc[0]*fMaxVel;
            pos[1] = nTgtPos;
    
            vel[0] = fMaxVel;
            vel[1] = 0.0;

            acc[1] = INT_WITH_MIN1(CalcAccCnt(fMaxVel, 0.0, pos[1]-pos[0]));

            nPhNum = 2;     
        }
        else
        {
            // Do the trapezoidal
            nAccCnt1 = INT_WITH_MIN1((fMaxVel - fCurTgtVel)/fAcc1);
            pos[0] = dCurPosCmd + CalcDistance(fCurTgtVel, fMaxVel, nAccCnt1);
            acc[1] = INT_WITH_MIN1((nTgtPos - pos[0] - fAcc2Ds)/fMaxVel);

            pos[1] = pos[0] + acc[1]*fMaxVel;
            pos[2] = nTgtPos;                     

            vel[0] = fMaxVel;   
            vel[1] = fMaxVel;   
            vel[2] = 0.0;

            acc[0] = nAccCnt1;     
            acc[2] = INT_WITH_MIN1(CalcAccCnt(fMaxVel, 0.0, pos[2]-pos[1]));

            nPhNum = 3;
        }
    }

	// UpdateMotionMake_AFDbgStatus(ax, pos[0], pos[1], pos[2], nTgtPos); //120803 2.9.7 syk

    if(ret == MMC_OK)
    {
		gBgPrf[ax].nCurPhase = 0;
		gBgPrf[ax].nPhasesLeft = nPhNum;

        for(i=0; i<nPhNum;i++)
        {
            pPh = &(gBgPrf[ax].Phase[i]);

			if(acc[i] == 0)
			{
			   	for(j=0; j<9; j++)
				{
					MEnqueue(MAxisQueue[ax], 10); 
				}	
				motionProfileMakeingStatus = 0;	        				
		        return MMC_ILLEGAL_PARAM_MOVE_DS;
			}
			
            pPh->nAcc = acc[i];
            pPh->fEndVel = vel[i];
            pPh->dEndPos = pos[i];

            MEnqueue(MAxisQueue[ax], pPh->nAcc);  
            MEnqueue(MAxisQueue[ax], pPh->fEndVel*1000);  
            MEnqueue(MAxisQueue[ax], pPh->dEndPos);  

            if (i == nPhNum-1) 
            { 
                pPh->bEndByPos  = TRUE; 
            }
            else
            {
                pPh->bEndByPos  = FALSE;
            }
         
        }
        
        if(nPhNum == 1)
        {
            for( j=0; j<6; j++)
            {
                MEnqueue(MAxisQueue[ax], 0);  
            }
        }
        else if(nPhNum == 2)
        {
            for( j=0; j<3; j++)
            {
                MEnqueue(MAxisQueue[ax], 0);  
            }
        }

        if(!gIsrSt[ax].bMoving)
        {
            pCB->fScurveRatio = ApplyLimitsFloat(pCB->fScurveRatio, SCURVE_MIN_SMOOTHING_FACTOR, SCURVE_MAX_SMOOTHING_FACTOR);
            nAccCnt1 = (fabsf(acc[0]) < fabsf(acc[nPhNum-1])) ? (fabsf(acc[0])) : (fabsf(acc[nPhNum-1])) ;
            pCB->nScurveBufSize = ApplyLimitsInt((int)(pCB->fScurveRatio * nAccCnt1), 0, SCURVE_MAX_BUF_SIZE);
            InitScurveBuffer(ax, fCurTgtVel);
        }

		if(ax==0)
		{			
			Check_Front_Rear_servo_On();
		}			
        // pCBuf[ax]->vm_bMoveDs = 1;

		Motion_Flag[ax] = MoveD;        
		
        gBgSt[ax].bNewProfileReady = TRUE;
		MoveDS_Wait_Count_After_Servo_On_Flag = FALSE;
		
		MoveS_Flag = 0;
    }

    // For debug
    // sprintf(str,"\n\rS_CURVE => [%f]\n\r",pCB->fScurveRatio);
    // SERIAL_WriteBytes(str, strlen(str));

	motionProfileMakeingStatus = 0;	            
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_pt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_pt(int ax, double acc, double vel)
{
	// acc는 초이고, 내부에서는 ms 단위의 가속도를 사용하기 때문에 단위 변환 필요.
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if(pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;			// 20120704 2.9.6 syk 	   

		MEnqueue(MAxisQueue[ax], 111);
		MEnqueue(MAxisQueue[ax], acc);		
		MEnqueue(MAxisQueue[ax], vel);				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}			

		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	   
	}

	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정
	error_code = __dsp_move_p(1, ax, acc, vel);

	amc_error = error_code;

	return error_code;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_nt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_nt(int ax, double acc, double vel)
{
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;			// 20120704 2.9.6 syk 	   

		MEnqueue(MAxisQueue[ax], 222);
		MEnqueue(MAxisQueue[ax], acc);		
		MEnqueue(MAxisQueue[ax], vel);				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		
		
		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	
	}

	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정
	error_code = __dsp_move_n(1, ax, acc, vel);

	amc_error = error_code;

	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_st
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_st(int ax, double acc)
{
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;			// 20120704 2.9.6 syk 	   

		MEnqueue(MAxisQueue[ax], 333);
		MEnqueue(MAxisQueue[ax], acc);		
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<16;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}			
		
		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	 
	}

	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정
	error_code = __dsp_move_s(1, ax, acc);

	amc_error = error_code;

	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_dst
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_dst(int ax, double a1, double a2, double v, int m)
{
	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
//	char point;
	int	error_code;
	int j;	
		
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;				// 20120704 2.9.6 syk 	   
		// point =4;
		// UpdateMotionMake_ResultDbgStatus(point,(char)error_code);	// 120803 2.9.7 syk	

		MEnqueue(MAxisQueue[ax], 444);
	    MEnqueue(MAxisQueue[ax], a1);  
	    MEnqueue(MAxisQueue[ax], a2);  
	    MEnqueue(MAxisQueue[ax], v);  		
	    MEnqueue(MAxisQueue[ax], m);  
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<13;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}					

		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	   
	}

	if (v <= 0.)	// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정(-vel illegal)
	{
		error_code = MMC_ILLEGAL_PARAM_MOVE_DS;
		amc_error = error_code;				// 20120704 2.9.6 syk 	 
		// point =5;
		// UpdateMotionMake_ResultDbgStatus(point,(char)error_code);	 // 120803 2.9.7 syk   
		
		MEnqueue(MAxisQueue[ax], 444);
	    MEnqueue(MAxisQueue[ax], a1);  
	    MEnqueue(MAxisQueue[ax], a2);  
	    MEnqueue(MAxisQueue[ax], v);  		
	    MEnqueue(MAxisQueue[ax], m);  
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<13;j++)		
		{			
			MEnqueue(MAxisQueue[ax], 2);  		
		}					
		
		return MMC_ILLEGAL_PARAM_MOVE_DS;		
	}
	
	if(v < PRO_VEL_MIN_LIMIT)	// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정
	{
		v = PRO_VEL_MIN_LIMIT;	
	}
	
	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정		
	error_code = __dsp_move_ds(1, ax, (float)a1, (float)a2, (float)v, m);
	
    amc_error = error_code;
	
	//2008.3.25(오), 정상적으로 __dsp_move_ds() 실행된 경우에만 정지용 Flag 설정.
	if(	error_code == MMC_OK ) 
	{
		pCBuf[ax]->checked_back_motion =0;		
		pCBuf[ax]->vm_bMoveDs = 1;
		
		// 2008.5.30(오)
		// 현 모션에서 모터 오동작 방지를 위한 최소 작업 영역을 설정한다.
		int pulse;
		pulse = m - (int)(pCBuf[ax]->mdRefpos);	// 현재 위치에서 목표 위치를 뺀 가야할 거리
		
		if( pulse >= 0 ) 
		{
			// 위치 리미트값을 설정
			// 2008.12.8(오), 최고 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)m + (float)pCBuf[ax]->working_limit) > 2147483647)
			{
				pCBuf[ax]->move_upper_limit = 2147483647;					// 이동 가능한 최고 위치 
			}
			else
			{
				pCBuf[ax]->move_upper_limit = m + pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
			}
		
			//2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)pCBuf[ax]->mdRefpos - (float)pCBuf[ax]->working_limit) < -2147483647) 
			{
				pCBuf[ax]->move_lower_limit = -2147483647;					// 이동 가능한 최저 위치 
			}
			else 
			{
				pCBuf[ax]->move_lower_limit = (int)pCBuf[ax]->mdRefpos - pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
			}

			if( pCBuf[ax]->dtBox * G_fSampling_Time <= v )	// +방향에서 가속하는 경우
			{
				pCBuf[ax]->vel_max_limit = v * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
				pCBuf[ax]->vel_min_limit = -v * pCBuf[ax]->vel_limit_ratio;			// 최저 속도값
			}
			else 											// +방향에서 0 속도로 감속하는 경우, 정지할 때 속도는 최고 속도의 -0.5배로 
			{
				pCBuf[ax]->vel_max_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
				pCBuf[ax]->vel_min_limit = -pCBuf[ax]->dtBox * G_fSampling_Time * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
			}
		}
		else
		{
			// 위치 리미트값을 설정

			// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)m - (float)pCBuf[ax]->working_limit) > 2147483647)
			{
				pCBuf[ax]->move_lower_limit = 2147483647;					// 이동 가능한 최고 위치 
			}
			else
			{
				pCBuf[ax]->move_lower_limit = m - pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
			}
	
			// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)pCBuf[ax]->mdRefpos + (float)pCBuf[ax]->working_limit) < -2147483647) 
			{
				pCBuf[ax]->move_upper_limit = -2147483647;					// 이동 가능한 최저 위치 
			}
			else
			{
				pCBuf[ax]->move_upper_limit = pCBuf[ax]->mdRefpos + pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
			}

			// 2008.5.31(오)
			// 속도 모드에서 감속할 때 최대/최소 속도를 설정하기 위함.
			// 단위는 펄스/ms, 최대값은 819.2(6000rpm, +12V, 속도 모드에 한함)
			// v 값은 항상 +임, dtBox는 -임.
			if( fabs(pCBuf[ax]->dtBox * G_fSampling_Time) <= v )	// -방향에서 가속하는 경우
			{
				pCBuf[ax]->vel_max_limit = v * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
				pCBuf[ax]->vel_min_limit = -v * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최저 속도값
			}
			else // -방향에서 0 속도로 감속하는 경우
			{
				pCBuf[ax]->vel_max_limit = -pCBuf[ax]->dtBox * G_fSampling_Time * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
				pCBuf[ax]->vel_min_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
			}
		}  
		
		if( pCBuf[ax]->vel_max_limit > pCBuf[ax]->Vel_Limit )
		{
			pCBuf[ax]->vel_max_limit = pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
		}
		if( pCBuf[ax]->vel_min_limit < -pCBuf[ax]->Vel_Limit )
		{
			pCBuf[ax]->vel_min_limit = -pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
		}
	}
	
	// point = 6;
	//UpdateMotionMake_ResultDbgStatus(point,(char)error_code);	// 120803 2.9.7 syk
	
	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_p
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_p(int ax, double a, double vel)
{
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;				// 20120704 2.9.6 syk 	   

		MEnqueue(MAxisQueue[ax], 11);
	    MEnqueue(MAxisQueue[ax], a);  		
	    MEnqueue(MAxisQueue[ax], vel);  		
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  		
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		
		
		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	   
	}

	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정
	error_code = __dsp_move_p(0, ax, a, vel);

	amc_error = error_code;

	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_n
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_n(int ax, double a, double vel)
{
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;				// 20120704 2.9.6 syk 	   
		
		MEnqueue(MAxisQueue[ax], 22);
	    MEnqueue(MAxisQueue[ax], a);  		
	    MEnqueue(MAxisQueue[ax], vel);  		
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		

		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	   
	}

	// 20120704 2.9.6 syk dsp(user)함수에서 amc_error 초기화 시키는 문제로 수정	
	error_code = __dsp_move_n(0, ax, a, vel);

	amc_error = error_code;

	return error_code;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_s
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_s(int ax, double a)
{
	int	error_code;
	int j;

	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;					// 20120704 2.9.6 syk 	   
		
		MEnqueue(MAxisQueue[ax], 33);
	    MEnqueue(MAxisQueue[ax], a);  		
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<16;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		

		return MMC_ON_MOTION;
		// 2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08	 
	}

	error_code = __dsp_move_s(0, ax, a);

	amc_error = error_code;

	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_move_ds
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_move_ds(int ax, double a1, double a2, double v, int m)
{
	// 2008.5.20, ckyu
	// STOP이벤트가 발생된 상태에서 인가되는 move_x() 명령들은 먹지 않도록 한다.
	// char point;
	int	error_code;
	int j;

	// UpdateMotionMake_DbgStatusClear();		// 120803 2.9.7 syk		
	
	if (pCBuf[ax]->event_st)
	{
		error_code = MMC_ON_MOTION;
		amc_error = error_code;				// 20120704 2.9.6 syk 	   
		// point = 1;
		// UpdateMotionMake_ResultDbgStatus(point,(char)error_code);	    // 120803 2.9.7 syk	 

		MEnqueue(MAxisQueue[ax], 44);
	    MEnqueue(MAxisQueue[ax], a1);  				
	    MEnqueue(MAxisQueue[ax], a2);  				
	    MEnqueue(MAxisQueue[ax], v);  				
	    MEnqueue(MAxisQueue[ax], m);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<13;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		
	   
		return MMC_ON_MOTION;
		//2012.1.13, MMC_OK => MMC_ON_MOTION //2012.2.15 // 2.5.26 // 2.8.08
	}

	if (v <= 0.) 			 				// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정(-vel illegal)
	{
	 	error_code = MMC_ILLEGAL_PARAM_MOVE_DS;
	    amc_error = error_code;				// 20120704 2.9.6 syk 	 
	    // point = 2;
	    // UpdateMotionMake_ResultDbgStatus(point,(char)error_code);		// 120803 2.9.7 syk  

		MEnqueue(MAxisQueue[ax], 44);
	    MEnqueue(MAxisQueue[ax], a1);  				
	    MEnqueue(MAxisQueue[ax], a2);  				
	    MEnqueue(MAxisQueue[ax], v);  				
	    MEnqueue(MAxisQueue[ax], m);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<13;j++)
		{			
			MEnqueue(MAxisQueue[ax], 2);  		
		}		
	    
		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	
	if(v < PRO_VEL_MIN_LIMIT)				// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정
	{
		 v = PRO_VEL_MIN_LIMIT;	
	}
			
	error_code = __dsp_move_ds(0, ax, a1, a2, v, m);
	
	amc_error  = error_code;

	// 2008.3.25(오), 정상적으로 __dsp_move_ds() 실행된 경우에만 정지용 Flag 설정.
	if(	error_code == MMC_OK ) 
	{
		pCBuf[ax]->checked_back_motion =0;		
	
		// 2008.5.30(오)
		// 현 모션에서 모터 오동작 방지를 위한 최소 작업 영역을 설정한다.
		int pulse;
		
		pulse = m - (int)(pCBuf[ax]->mdRefpos);	// 현재 위치에서 목표 위치를 뺀 가야할 거리
		
		if( pulse >= 0 ) 						// +방향으로 이동하는 경우
		{	
			// 위치 리미트값을 설정
			// 2008.12.8(오), 최고 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)m + (float)pCBuf[ax]->working_limit) > 2147483647)
			{
				pCBuf[ax]->move_upper_limit = 2147483647;					// 이동 가능한 최고 위치 
			}
			else
			{
				pCBuf[ax]->move_upper_limit = m + pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
			}
		
			// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)pCBuf[ax]->mdRefpos - (float)pCBuf[ax]->working_limit) < -2147483647)
			{
				pCBuf[ax]->move_lower_limit =-2147483647;					// 이동 가능한 최저 위치 
			}
			else
			{
				pCBuf[ax]->move_lower_limit = (int)pCBuf[ax]->mdRefpos - pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
			}

			// 2008.5.31(오), 속도 리미트값을 설정하는 루틴
			// 속도 모드에서 감속할 때 최대/최소 속도를 설정하기 위함.
			// 단위는 펄스/ms, 최대값은 819.2(6000rpm, +12V, 속도 모드에 한함)
			// v 값은 항상 +임, dtBox는 -임.
			if( pCBuf[ax]->dtBox * G_fSampling_Time <= v )					// + 방향에서 가속하는 경우
			{
				pCBuf[ax]->vel_max_limit = v * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
				pCBuf[ax]->vel_min_limit = -v * pCBuf[ax]->vel_limit_ratio;			// 최저 속도값
			}
			else	// + 방향에서 0 속도로 감속하는 경우, 정지할 때 속도는 최고 속도의 -0.5배로 
			{
				pCBuf[ax]->vel_max_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
				pCBuf[ax]->vel_min_limit = -pCBuf[ax]->dtBox * G_fSampling_Time * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
			}
		}
		else		// -방향으로 이동하는 경우
		{
			// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)m - (float)pCBuf[ax]->working_limit) > 2147483647) 
			{
				pCBuf[ax]->move_lower_limit = 2147483647;					// 이동 가능한 최고 위치 
			}
			else
			{
				pCBuf[ax]->move_lower_limit = m - pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
			}
	
			// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
			if( ((float)pCBuf[ax]->mdRefpos + (float)pCBuf[ax]->working_limit) < -2147483647) 
			{
				pCBuf[ax]->move_upper_limit =- 2147483647;					// 이동 가능한 최저 위치 
			}
			else
			{
				pCBuf[ax]->move_upper_limit = pCBuf[ax]->mdRefpos + pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
			}

			// 2008.5.31(오)
			// 속도 모드에서 감속할 때 최대/최소 속도를 설정하기 위함.
			// 단위는 펄스/ms, 최대값은 819.2(6000rpm, +12V, 속도 모드에 한함)
			// v값은 항상 +임, dtBox는 -임.
			if( fabs(pCBuf[ax]->dtBox * G_fSampling_Time) <= v )	// - 방향에서 가속하는 경우
			{
				pCBuf[ax]->vel_max_limit = v * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
				pCBuf[ax]->vel_min_limit = -v * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최저 속도값
			}
			else	// - 방향에서 0 속도로 감속하는 경우
			{
				pCBuf[ax]->vel_max_limit = -pCBuf[ax]->dtBox * G_fSampling_Time * pCBuf[ax]->vel_limit_ratio;			// 최대 속도값
				pCBuf[ax]->vel_min_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
			}
		}  
		
		if( pCBuf[ax]->vel_max_limit > pCBuf[ax]->Vel_Limit ) 
		{
			pCBuf[ax]->vel_max_limit = pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
		}
		
		if( pCBuf[ax]->vel_min_limit < -pCBuf[ax]->Vel_Limit )
		{
			pCBuf[ax]->vel_min_limit = -pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
		}
	}
	
    // point = 3;
    // UpdateMotionMake_ResultDbgStatus(point,(char)error_code);	// 120803 2.9.7 syk
    
	return error_code;
}

////////////////////////////////////////////////////////////////////////
// Function	: __dsp_move_p
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __dsp_move_p(int mode, int ax, double a_param, double vel)
{
	double a;
	int j;	
	
	// 2012.2.28
	// 서보 Enable 상태가 아니면 Motion 함수가 실행되지 않도록 하기 위함.
	if( pCBuf[ax]->g_on != 0 )
	{
		MEnqueue(MAxisQueue[ax], 1111);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		
		return MMC_AMP_FAULT;
	}	
		
	if (mode == 0)
	{
		a = a_param / 1000;
	}
	else
	{
		a = a_param;
	}
	
	// 추가할 내용 
	// 속도의 부호가 바뀌는 경우 함수 에러 처리, 기존 명령은 정상적으로 진행되어야 함. 2007.11.8(오)
	// 2008.1.22(오) PTP Motion에서 정지 상태에서 dtBox가 -1로 되는 경우가 있음.
	if(pCBuf[ax]->dtBox < 0.0)
	{
		MEnqueue(MAxisQueue[ax], 1111);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 2);  		
		}		
		return MMC_ILLEGAL_PARAM_MOVE_DS;	
	}
	
	if( vel <= 0 )
	{
		MEnqueue(MAxisQueue[ax], 1111);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 3);  		
		}		
		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}	

	if( a <= 0 )
	{
		MEnqueue(MAxisQueue[ax], 1111);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 4);  		
		}		
		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	
	if(vel < PRO_VEL_MIN_LIMIT)	// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정
	{
	    vel = PRO_VEL_MIN_LIMIT;	
	}
	
    if ( vel > pCBuf[ax]->Vel_Limit ) 
	{
		vel = pCBuf[ax]->Vel_Limit;
	}

	// 2008.6.2, ckyu
	// 디버깅을 위한 상태를 보내기위해서 수행하는 명령을 저장한다.

	// 2008.3.25(오), 실행 위치를 본 함수가 정상적으로 실행될 때에만 vm_bMoveDs Flag를 해제하도록 뒤로 이동. 
	pCBuf[ax]->vm_bMoveDs = 0;
	pCBuf[ax]->checked_back_motion =0;	
	pCBuf[ax]->ds_profile_point	=0;	
	
	if(MovePNS(mode, ax, (float)a_param, (float)vel) != MMC_OK)
	{
		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}

	// 2008.5.31(오)
	// 속도 모드에서 감속할 때 최대/최소 속도를 설정하기 위함.
	// 단위는 펄스/ms, 최대값은 819.2(6000rpm, +12V, 속도 모드에 한함)
	if( pCBuf[ax]->dtBox * G_fSampling_Time <= vel )	// + 방향에서 가속하는 경우
	{
		pCBuf[ax]->vel_max_limit = vel * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
		
		if( pCBuf[ax]->dtBox != 0 )
		{
			pCBuf[ax]->vel_min_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 - pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
		}
		else
		{
			pCBuf[ax]->vel_min_limit = -1.0 * (float)pCBuf[ax]->Vel_Limit * pCBuf[ax]->vel_limit_ratio;				// 최저 속도값
		}
	}
	else	// +방향에서 0이 아닌 속도로 감속하는 경우
	{
		if( pCBuf[ax]->dtBox != 0 )
		{
			pCBuf[ax]->vel_max_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
		}
		else
		{
			pCBuf[ax]->vel_max_limit = (float)pCBuf[ax]->Vel_Limit * pCBuf[ax]->vel_limit_ratio;					// 최저 속도값
		}
		pCBuf[ax]->vel_min_limit = vel * (1.0 - pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
	}
	
	if( pCBuf[ax]->vel_max_limit > pCBuf[ax]->Vel_Limit ) 
	{
		pCBuf[ax]->vel_max_limit = pCBuf[ax]->Vel_Limit;						// 리미트값을 취한다.
	}
	
	if( pCBuf[ax]->vel_min_limit < -pCBuf[ax]->Vel_Limit )
	{
		pCBuf[ax]->vel_min_limit = -pCBuf[ax]->Vel_Limit;						// 리미트값을 취한다.
	}

	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: __dsp_move_n
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __dsp_move_n(int mode, int ax, double a_param, double vel)
{
	double a;
	int j;
	
	// 2012.2.28
	// 서보 Enable 상태가 아니면 Motion 함수가 실행되지 않도록 하기 위함.
	if( pCBuf[ax]->g_on != 0 )
	{
		MEnqueue(MAxisQueue[ax], 2222);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		
		return MMC_AMP_FAULT;
	}		

	if (mode == 0)
	{
		a = a_param / 1000;
	}
	else
	{
		a = a_param;
	}
	
	// 추가할 내용 
	// 속도의 부호가 바뀌는 경우 함수 에러 처리, 기존 명령은 정상적으로 진행되어야 함. 2007.11.8(오)
	if(pCBuf[ax]->dtBox > 0.0)
	{
		MEnqueue(MAxisQueue[ax], 2222);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 2);  		
		}		

		return MMC_ILLEGAL_PARAM_MOVE_DS;	
	}

	if( vel <= 0 )
	{
		MEnqueue(MAxisQueue[ax], 2222);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  		
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 3);  		
		}		

		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	
	if(vel < PRO_VEL_MIN_LIMIT)		// 2012.03.08 2.9.2 syk velocity 1000PPS이하 설정
	{
		 vel = PRO_VEL_MIN_LIMIT;	
	}	
	
	if ( vel > pCBuf[ax]->Vel_Limit ) 
	{
		vel = pCBuf[ax]->Vel_Limit;	// 2007.12.28(오), 과속 방지 기능
	}

	if( a <= 0 )
	{
		MEnqueue(MAxisQueue[ax], 2222);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], vel);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<14;j++)
		{			
			MEnqueue(MAxisQueue[ax], 4);  		
		}		

		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	
	// 2008.6.2, ckyu
	// 디버깅을 위한 상태를 보내기위해서 수행하는 명령을 저장한다.
	
	// 2008.3.25(오), 실행 위치를 본 함수가 정상적으로 실행될 때에만 vm_bMoveDs Flag를 해제하도록 뒤로 이동. 
	pCBuf[ax]->vm_bMoveDs = 0;
	pCBuf[ax]->checked_back_motion =0;	
	pCBuf[ax]->ds_profile_point	=0;	
	
	if(MovePNS(mode, ax, a_param, -vel) != MMC_OK)
	{
		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}	    

	if( fabs(pCBuf[ax]->dtBox * G_fSampling_Time) <= vel )	// - 방향에서 가속하는 경우
	{
		if( pCBuf[ax]->dtBox != 0 ) 
		{
			pCBuf[ax]->vel_max_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 - pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
		}
		else
		{
			pCBuf[ax]->vel_max_limit = 1.0 * (float)pCBuf[ax]->Vel_Limit * pCBuf[ax]->vel_limit_ratio;
		}
		pCBuf[ax]->vel_min_limit = -vel * (1.0 + pCBuf[ax]->vel_limit_ratio);										// 최저 속도값
	}
	else // -방향에서 0이 아닌 속도로 감속하는 경우
	{
		pCBuf[ax]->vel_max_limit = -vel* (1.0 - pCBuf[ax]->vel_limit_ratio);									// 최대 속도값
		pCBuf[ax]->vel_min_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
	}
	
	if( pCBuf[ax]->vel_max_limit > pCBuf[ax]->Vel_Limit )
	{
		pCBuf[ax]->vel_max_limit = pCBuf[ax]->Vel_Limit;		// 리미트값을 취한다.
	}
	
	if( pCBuf[ax]->vel_min_limit < -pCBuf[ax]->Vel_Limit ) 
	{
		pCBuf[ax]->vel_min_limit = -pCBuf[ax]->Vel_Limit;		// 리미트값을 취한다.
	}

	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: __dsp_move_s
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __dsp_move_s(int mode, int ax, double a_param)
{
	double a;
	int j;
	
	// 2012.2.28
	// 서보 Enable 상태가 아니면 Motion 함수가 실행되지 않도록 하기 위함.
	if( pCBuf[ax]->g_on != 0 )
	{
		MEnqueue(MAxisQueue[ax], 3333);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  						
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 1);  		
		}		

		return MMC_AMP_FAULT;
	}

	if (mode == 0) 
	{
		a = a_param / 1000;
	}
	else
	{
		a = a_param;
	}
	
 	if(!gIsrSt[ax].bMoving)
 	{
		MEnqueue(MAxisQueue[ax], 3333);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug);  						
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 2);  		
		}		

 	 	//return MMC_OK;
		return MMC_ILLEGAL_PARAM_MOVE_S;
 	}
	
	// 추가할 내용 
	if( a <= 0 )
	{
		MEnqueue(MAxisQueue[ax], 3333);
	    MEnqueue(MAxisQueue[ax], mode);  				
	    MEnqueue(MAxisQueue[ax], a_param);  				
	    MEnqueue(MAxisQueue[ax], Count_200us_INT_4_Debug); 
	    MEnqueue(MAxisQueue[ax], 0);  	    	    
		
		for(j=0;j<15;j++)
		{			
			MEnqueue(MAxisQueue[ax], 3);  		
		}		

		return MMC_ILLEGAL_PARAM_MOVE_DS;
	}
	else 		// 2007.12.28, oh 신규 추가
	{
		if( mode == 0 )		// 가속도를 사용
		{
			if( (int)(pCBuf[ax]->Vel_Limit/a) > pCBuf[ax]->Accel_Limit * MOVEX_ACCEL_SCALER )
			{
				a = pCBuf[ax]->Vel_Limit/(pCBuf[ax]->Accel_Limit * MOVEX_ACCEL_SCALER);
			}
		}
		else	// 가속 시간을 사용
		{
			// 2012.2.28 a는 초 단위, Accel_Limit은 ms 단위라서 Scaling 변경
			if(a*1000.0 > pCBuf[ax]->Accel_Limit * MOVEX_ACCEL_SCALER)
			{
				a = (pCBuf[ax]->Accel_Limit * MOVEX_ACCEL_SCALER)/1000.0;
			}
		}
	}
	
	pCBuf[ax]->vm_bMoveDs = 0;	// 2008.1.17(오)
	pCBuf[ax]->checked_back_motion =0;	
	pCBuf[ax]->ds_profile_point	=0;	
	
	if(MovePNS(mode, ax, a_param, 0.0) != MMC_OK)
	{
		return MMC_ILLEGAL_PARAM_MOVE_DS;		
	}	
	
	if( pCBuf[ax]->dtBox >= 0.0 )	// +방향으로 회전한 경우
	{
		pCBuf[ax]->vel_max_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
		pCBuf[ax]->vel_min_limit = -1.0 * (float)pCBuf[ax]->Vel_Limit * pCBuf[ax]->vel_limit_ratio;				// 최대 속도값
	}
	else // -방향으로 회전한 경우
	{
		pCBuf[ax]->vel_max_limit = (float)pCBuf[ax]->Vel_Limit * pCBuf[ax]->vel_limit_ratio;					// 최대 속도값
		pCBuf[ax]->vel_min_limit = pCBuf[ax]->dtBox * G_fSampling_Time * (1.0 + pCBuf[ax]->vel_limit_ratio);	// 최대 속도값
	}
	
	if( pCBuf[ax]->vel_max_limit > pCBuf[ax]->Vel_Limit )
	{
		pCBuf[ax]->vel_max_limit = pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
	}
	
	if( pCBuf[ax]->vel_min_limit < -pCBuf[ax]->Vel_Limit ) 
	{
		pCBuf[ax]->vel_min_limit = -pCBuf[ax]->Vel_Limit;	// 리미트값을 취한다.
	}
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: __dsp_move_ds
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __dsp_move_ds(int mode, int ax, double a1, double a2, double v, int m)
{
    int ret = MMC_OK;
	
	ret = MoveDS(mode, ax, (float)a1, (float)a2, (float)v, m);
	
    return ret;
}

