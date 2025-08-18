#define INTERNALFUNCS_GLOBALS

#include "all.h"

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_param_flush
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_param_flush()
{
	SaveBootParam(&BootPara);
	
	Copy_BootPara_2_Para();
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_encoder_offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_set_encoder_offset(int ax, int ofs)
{
	if(pCBuf[ax]->g_on != 0 )
	{
		__SetEncoderOffset(ax, ofs);
		
		BootPara.st_boot.Encoder_Offset[ax] = ofs;
		
		SetBootParaCrc(&BootPara);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_fset_encoder_offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void _dsp_fset_encoder_offset(int ax, int ofs)
{
	_dsp_set_encoder_offset(ax, ofs);
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_encoder_offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_encoder_offset(int ax)
{
	return __GetEncoderOffset(ax);
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetPosition
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetPosition(int ax, int pos)
{
	pCBuf[ax]->init_counter=pos;
	pCBuf[ax]->dtBox = 0.0; 
	pCBuf[ax]->dtBoxRaw = 0.0;

	Setposition_count[ax] = 5;

	if(ax==0)
	{			
		FollowsPOS = pos;
		FollowsPOS_Prev = FollowsPOS;
	}	
	
    InitScurveParams(ax);
	
	if( pCBuf[ax]->motortype )
	{
		MUST_ENTER_CRITICAL_SECTION;

		WriteEncoder(ax, pCBuf[ax]->init_counter);		
		
		pCBuf[ax]->jtpos = pos;
		pCBuf[ax]->jtpos_b = pos;

		pCBuf[ax]->mdRefpos = pos;
		pCBuf[ax]->mdRefpos_b = pos;
		pCBuf[ax]->dPosCmd = pos;
		pCBuf[ax]->dPosCmdRaw = pos;

		MUST_LEAVE_CRITICAL_SECTION;
	}
	else
	{
		MUST_ENTER_CRITICAL_SECTION;

		WriteEncoder(ax, pCBuf[ax]->init_counter);		
		
		pCBuf[ax]->jtpos = pos;
		pCBuf[ax]->jtpos_b = pos;

		pCBuf[ax]->mdRefpos = pos;
		pCBuf[ax]->mdRefpos_b = pos;
		pCBuf[ax]->dPosCmd = pos;
		pCBuf[ax]->dPosCmdRaw = pos;

		MUST_LEAVE_CRITICAL_SECTION;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetSyncEncoder
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetSyncEncoder(int *ppos_m, int *ppos_s)
{
	*ppos_m = pCBuf[SyncMotion.Master]->jtpos; 	// 200us 에서 읽은 위치 데이터
	*ppos_s = pCBuf[SyncMotion.Slave]->jtpos;	// 200us 에서 읽은 위치 데이터
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetCurPos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetCurPos(int ax)
{
	return pCBuf[ax]->jtpos;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetPos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetPos(int ax)
{
	return (int) ceil( pCBuf[ax]->mdRefpos );
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetErrorPls
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float __GetErrorPls(int ax)
{
	return pCBuf[ax]->pos_ierr;
}
////////////////////////////////////////////////////////////////////////
// Function	: __SetErrorLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetErrorLimit(int ax, int lchk, char cst)
{
	pCBuf[ax]->limit_err_chk = lchk;
	pCBuf[ax]->limit_err_st = cst;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetErrorLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetErrorLimit(int ax, int *plchk, char *pcst)
{
	*plchk = pCBuf[ax]->limit_err_chk;
	*pcst = pCBuf[ax]->limit_err_st;
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetPosLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetPosLimit(int ax)
{
	if( ((float)pCBuf[ax]->jtpos + (float)pCBuf[ax]->working_limit) > 2147483647) 
	{
		pCBuf[ax]->move_upper_limit = 2147483647;									// 이동 가능한 최고 위치 
	}
	else
	{
		pCBuf[ax]->move_upper_limit = pCBuf[ax]->jtpos + pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
	}

	if( ((float)pCBuf[ax]->jtpos - (float)pCBuf[ax]->working_limit) < -2147483647)
	{
		pCBuf[ax]->move_lower_limit = -2147483647;									// 이동 가능한 최저 위치 
	}
	else
	{
		pCBuf[ax]->move_lower_limit = pCBuf[ax]->jtpos - pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
	}

	pCBuf[ax]->vel_max_limit = (float)pCBuf[ax]->Vel_Limit * 0.1;	// 최대 속도값
	pCBuf[ax]->vel_min_limit = (float)pCBuf[ax]->Vel_Limit * -0.1;	// 최저 속도값

}

////////////////////////////////////////////////////////////////////////
// Function	: __ServoOn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ServoOn(int ax)
{
	int amp_onoff_check=0;	// 120217 2.8.08 syk
	
	if(pCBuf[ax]->g_on != 0)
	{
#if (defined(__AMC_SMD) || defined(__AMC_29x))
		if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )	// OHT 모드에 rear축인 경우 
		{
			if (__GetAxisSource(SyncMotion.Slave) & (ST_MUST_REBOOT | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
				return;
			}
			
			if(pCBuf[SyncMotion.Slave]->event_st != 0) 
			{
				return;	
			}
						
			if (__GetAxisSource(SyncMotion.Master) & (ST_MUST_REBOOT | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
    			if( pCBuf[SyncMotion.Slave]->event_st < pCBuf[SyncMotion.Master]->event_st )  
				{
        			pCBuf[SyncMotion.Slave]->event_st = pCBuf[SyncMotion.Master]->event_st;
                }
				return;
			}
			
		    if(pCBuf[SyncMotion.Master]->event_st != 0)
		    {
       			if( pCBuf[SyncMotion.Slave]->event_st < pCBuf[SyncMotion.Master]->event_st )  
				{
        			pCBuf[SyncMotion.Slave]->event_st = pCBuf[SyncMotion.Master]->event_st;
                }
				return;			
		    }
		}
		else if ( SyncMotion.Flag && (SyncMotion.Master == ax) )
		{
			if (__GetAxisSource(SyncMotion.Master) & ( ST_MUST_REBOOT | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
				return;
			}
			
		    if(pCBuf[SyncMotion.Master]->event_st != 0) 
			{
				return;	
			}
		    	
			if (__GetAxisSource(SyncMotion.Slave) & ( ST_MUST_REBOOT | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{	
    			if( pCBuf[SyncMotion.Master]->event_st < pCBuf[SyncMotion.Slave]->event_st ) 
				{
        			pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st;			
 				}
				return;
			}
		
			if(pCBuf[SyncMotion.Slave]->event_st != 0)
			{	
    			if( pCBuf[SyncMotion.Master]->event_st < pCBuf[SyncMotion.Slave]->event_st )  
				{
        			pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st;			
				}
				return;
			}							
		}
		else
		{
			if (__GetAxisSource(ax) & ( ST_MUST_REBOOT | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) )
			{
				return;		
			}
		    
		    if(pCBuf[ax]->event_st != 0)
			{
				return;			 	
			}
		}
#endif

#if defined(__AMC_V70)
		if ( SyncMotion.Flag && (SyncMotion.Slave == ax) )	// OHT 모드에 rear축인 경우 
		{
			if (__GetAxisSource(SyncMotion.Slave) & ( ST_MUST_REBOOT | ST_SYSTEM_INSEC | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
				return;
			}
			
			if(pCBuf[SyncMotion.Slave]->event_st != 0) 
			{
				return;	
			}
						
			if (__GetAxisSource(SyncMotion.Master) & ( ST_MUST_REBOOT | ST_SYSTEM_INSEC | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
    			if( pCBuf[SyncMotion.Slave]->event_st < pCBuf[SyncMotion.Master]->event_st )  
				{
        			pCBuf[SyncMotion.Slave]->event_st = pCBuf[SyncMotion.Master]->event_st;
				}
				return;
			}
			
		    if(pCBuf[SyncMotion.Master]->event_st != 0)
		    {
       			if( pCBuf[SyncMotion.Slave]->event_st < pCBuf[SyncMotion.Master]->event_st )  
				{
        			pCBuf[SyncMotion.Slave]->event_st = pCBuf[SyncMotion.Master]->event_st;
				}
				return;			
		    }
		}
		else if ( SyncMotion.Flag && (SyncMotion.Master == ax) )
		{
			if (__GetAxisSource(SyncMotion.Master) &  ( ST_MUST_REBOOT | ST_SYSTEM_INSEC | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{
				return;
			}		
			
		    if(pCBuf[SyncMotion.Master]->event_st != 0) 
			{
				return;	
			}
		    	
			if (__GetAxisSource(SyncMotion.Slave) & ( ST_MUST_REBOOT | ST_SYSTEM_INSEC | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) ) 
			{	
    			if( pCBuf[SyncMotion.Master]->event_st < pCBuf[SyncMotion.Slave]->event_st )  
				{
        			pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st;			
				}
				return;
			}
		
			if(pCBuf[SyncMotion.Slave]->event_st != 0)
			{	
    			if( pCBuf[SyncMotion.Master]->event_st < pCBuf[SyncMotion.Slave]->event_st )  
				{
        			pCBuf[SyncMotion.Master]->event_st = pCBuf[SyncMotion.Slave]->event_st;	
 				}
				return;
			}							
		}
		else
		{
			if (__GetAxisSource(ax) & ( ST_MUST_REBOOT | ST_SYSTEM_INSEC | ST_V_LIMIT | ST_AMP_FAULT | ST_X_NEG_LIMIT | ST_X_POS_LIMIT | ST_ERROR_LIMIT | ST_ENC_OVF) )
			{
				return;		
			}
		    
		    if(pCBuf[ax]->event_st != 0) 
			{
				return;			 	
			}
		}
#endif		

		pCBuf[ax]->axis_source &= (~ST_AMP_POWER_ONOFF);	// Amp Enable 상태로 변경, 2010.8.4=> 2011.8.18 적용

		pCBuf[ax]->mdRefpos = pCBuf[ax]->jtpos;

		pCBuf[ax]->delay_c = 0;

		__SetPosLimit(ax);

		WriteChannel2(AxisDPRAM[ax], AD_AxisSource, pCBuf[ax]->axis_source);
		
		pCBuf[ax]->g_on=0;		// 2011.10.13, 위치 변경
		
		ServoOn( ax );			// 2008.12.6(오) 위에서 마지막 설정 후로 이동

		// 120217 2.8.08 syk pc에서의 amp on/off 기능 추가로 인해 amp onoff상태 fpga에서 확인 필요// 
		if(fpga_ver > 20810)
		{
			amp_onoff_check = 1 << (ax + 16);
			if((*ptr_sys_sctrl & amp_onoff_check))
			{	
				pCBuf[ax]->axis_source |= (ST_AMP_POWER_ONOFF);	
				pCBuf[ax]->g_on=-1;					
			}
		}		
	}
	
	return;
}

////////////////////////////////////////////////////////////////////////
// Function	: __ServoOff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ServoOff(int ax)
{
	pCBuf[ax]->g_on=-1;
	ServoOff( ax );
	
	pCBuf[ax]->axis_source |= (ST_AMP_POWER_ONOFF);
	pCBuf[ax]->vm_accflag=0;
	pCBuf[ax]->vm_dccflag=0;

	pCBuf[ax]->dtBox = 0.0;
	pCBuf[ax]->dtBoxRaw = 0.0;
	pCBuf[ax]->vm_bMoveDs = 0;		// Wait position 모드를 해제.
	pCBuf[ax]->checked_back_motion =0;

	InitIsrSt(ax);
    InitBgSt(ax);
    InitIsrVars(ax, pCBuf[ax]->jtpos);
	
    InitScurveParams(ax);
	
	WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,(pCBuf[ax]->in_pos_flag=1));
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence=0));
	WriteChannel2(AxisDPRAM[ax], AD_AxisSource, pCBuf[ax]->axis_source);

	_dsp_frames_clear(ax);			// 20120727 2.9.06 syk  dsp user 프로그램에서 servo off 하는 경우 축 정보를 dpram을 통해 얻어 옴에 의해, 다른 축의 frame을 clear해 버리는 문제.
	
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,0);	
}

////////////////////////////////////////////////////////////////////////
// Function	: __ResetAlarm
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ResetAlarm( int ax )
{
	int i;
	
	i = 1 << (ax + 4);
	
	if(!pCBuf[ax]->reset_level)
	{
		i=~i;
		pGBuf->sys_io_out = i&pGBuf->sys_io_out;
	}
	else
	{
		pGBuf->sys_io_out = i | pGBuf->sys_io_out;
	}
	
	*ptr_sys_sctrl = pGBuf->sys_io_out;

	pCBuf[ax]->axis_source &= (~ST_AMP_FAULT);	
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetAlarm
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetAlarm( int ax )
{
	int i;
	
	i = 1 << (ax + 4);

	if(pCBuf[ax]->reset_level)
	{
		i=~i;
		pGBuf->sys_io_out = i & pGBuf->sys_io_out;
	}
	else
	{
		pGBuf->sys_io_out = i | pGBuf->sys_io_out;
	}
	
	*ptr_sys_sctrl = pGBuf->sys_io_out;
}

////////////////////////////////////////////////////////////////////////
// Function	: __ClearStatus
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __ClearStatus(int ax)
{
	if(SyncMotion.Flag)		// OHT 모드에서는 Rebooting 조건을 해제하기 위해, 2010.11.13
	{
		pCBuf[ax]->bMustReboot = 0;
	}
	
	pCBuf[ax]->axis_source &= 0x0E00;	// Turn ON => ST_PC_COMMAND,ST_OUT_OF_FRAMES
	pCBuf[ax]->axis_source &= (~ST_PC_COMMAND);

	pCBuf[ax]->event_st=NO_EVENT;
	WriteChannel1(AxisDPRAM[ax],AD_In_Sequence,(pCBuf[ax]->in_sequence=0));
	WriteChannel1(AxisDPRAM[ax],AD_In_Pos_Flag,(pCBuf[ax]->in_pos_flag=1));

	// 2008.1.22(오), Event 후 move_x 함수에서 사용하는 변수들에 이상이 있는 경우 동작하지 않아서 초기화
	pCBuf[ax]->vm_prev_vel = 0.0;
	pCBuf[ax]->nPrfCtr = 0;				// 통합, 120302, syk // move_ds 변경사항 적용에 따른 clear, 이전 vm관련 상태 clear	
	
	pCBuf[ax]->dtBox = 0.0;
	pCBuf[ax]->dtBoxRaw = 0.0;
	pCBuf[ax]->checked_back_motion =0;	
	
	// 2008.5.20, stop()후 move_ds()를 한번이상 누른상태에서 clear_status()를 하면 진행하는 문제
	int qpos = Q_in_num[ax] = Q_out_num[ax];
	AQ_BUF *pQueue = &Q_Buf[ax][qpos];
	pQueue->Command = 0;
	
	Pause.WaitState[ax] = 0;
	Pause.Flag[ax] = 0;

#if defined(__AMC_V70)	
	*ptr_cutoff_status = 0;		// Status clear for monitoring voltage
#endif

	InitIsrSt(ax);
    InitBgSt(ax);
    InitScurveParams(ax);

	// 2008.12.8(오), 최고 작업 영역을 확인하여 벗어나는 경우만 제한
	if( ((float)pCBuf[ax]->jtpos + (float)pCBuf[ax]->working_limit) > 2147483647)
	{
		pCBuf[ax]->move_upper_limit = 2147483647;									// 이동 가능한 최고 위치 
	}
	else
	{
		pCBuf[ax]->move_upper_limit = pCBuf[ax]->jtpos + pCBuf[ax]->working_limit;	// 이동 가능한 최고 위치 
	}

	// 2008.12.8(오), 최저 작업 영역을 확인하여 벗어나는 경우만 제한
	if( ((float)pCBuf[ax]->jtpos - (float)pCBuf[ax]->working_limit) < -2147483647) 
	{
		pCBuf[ax]->move_lower_limit = -2147483647;									// 이동 가능한 최저 위치 
	}
	else 
	{
		pCBuf[ax]->move_lower_limit = pCBuf[ax]->jtpos - pCBuf[ax]->working_limit;	// 이동 가능한 최저 위치 
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __IsPauseOn
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __IsPauseOn(int ax)
{
	if (!BootPara.st_boot.Motor_Pause[ax]) 
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __IsPauseCondition
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __IsPauseCondition(int ax)
{
	if (!BootPara.st_boot.Motor_Pause[ax]) 
	{
		return 0;
	}
	else
	{
    	return 1;
    }
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetDacOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetDacOffset(int ax)
{
	return pCBuf[ax]->dac_bias;		// dac_bias => ->dac_code로 수정, 2007.1.23(오)=> 다시 bias로 2007.11.22(오)
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetFramesClear
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetFramesClear(int ax)
{
	int i;
	
	Q_out_num[ax]=Q_in_num[ax]=0;

	for(i=0; i<MAX_Q_NUM; i++)
	{
		Q_Buf[ax][i].Command=0;
	}
	
	pCBuf[ax]->in_sequence=0;
	pCBuf[ax]->axis_source &= (~ST_OUT_OF_FRAMES);
}

////////////////////////////////////////////////////////////////////////
// Function	: __PutDacOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __PutDacOffset(int ax, int nd)
{
	pCBuf[ax]->dac_bias = nd;
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetSWUpperLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetSWUpperLimit(int ax, int li, char cst)
{
	pCBuf[ax]->swupper_limit= li;
	pCBuf[ax]->swupper_limitst= cst;
	pCBuf[ax]->limit_swupper_chk_cnt = 0;			// 2.5.25v2.8.07통합, 120111, syk software upper travel limit 설정시 cnt 초기화 	
	
	BootPara.st_boot.SwUpper_Limit[ax] = li;
	BootPara.st_boot.SwUpper_LimitSt[ax] = cst;		// 2011.8.18
	
	SetBootParaCrc(&BootPara);						// 2011.4.20, ckyu // 자동으로 crc를 계산한다.
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetSWUpperLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetSWUpperLimit(int ax, int *pli, char *pcst)
{
	*pli = pCBuf[ax]->swupper_limit;
	*pcst = pCBuf[ax]->swupper_limitst;
}

////////////////////////////////////////////////////////////////////////
// Function	: __PutSWLowerLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 2.5.25v2.8.07통합, 120111, syk software lower travel limit 설정시 cnt 초기화 			
////////////////////////////////////////////////////////////////////////
void __PutSWLowerLimit(int ax, int li, char cst)
{
	pCBuf[ax]->swlower_limit= li;
	pCBuf[ax]->swlower_limitst= cst;
	pCBuf[ax]->limit_swlower_chk_cnt = 0;			
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetVelCurve
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void __SetVelCurve(int ax, int li, char cst)
{
	pCBuf[ax]->curve_limit= li;
	pCBuf[ax]->curve_limitst= cst;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetVelCurve
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void __GetVelCurve(int ax, int *pli, char *pcst) 
{
	*pli = pCBuf[ax]->curve_limit;
	*pcst = pCBuf[ax]->curve_limitst;
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetActvelMargin
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void __SetActvelMargin(int ax, int li, char cst, int time)
{
	pCBuf[ax]->actvel_margin= li;
	pCBuf[ax]->actvel_marginst= cst;
	pCBuf[ax]->actvel_margintime= time;	
	pCBuf[ax]->limit_curve_chk_cnt = 0;		
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetActvelMargin
// Brief		: 
// Input		:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void __GetActvelMargin(int ax, int *pli, char *pcst, int *ptime) 
{
	*pli = pCBuf[ax]->actvel_margin;
	*pcst = pCBuf[ax]->actvel_marginst;
	*ptime = pCBuf[ax]->actvel_margintime;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetIoOutData
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetIoOutData(int *pl1, int *pl2)
{
	*pl1 = pGBuf->user_io_out;
	*pl2 = pGBuf->user_io_out2;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetAxisStat
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetAxisStat(int ax) 
{
    return 	pCBuf[ax]->event_st; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetAxisSource
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetAxisSource(int ax) 
{ 
    return pCBuf[ax]->axis_source; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetAmpOnLevel
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetAmpOnLevel(int ax, int amp) 
{
    pCBuf[ax]->amp_on_level = amp; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetAmpOnLevel
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetAmpOnLevel(int ax) 
{ 
    return pCBuf[ax]->amp_on_level; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetTrackingFactor
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
float __GetTrackingFactor(int ax) 
{
    return pCBuf[ax]->V_TrackingFactor; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetTrackingFactor
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetTrackingFactor(int ax, float tfactor) 
{
    pCBuf[ax]->V_TrackingFactor = tfactor; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetIoInData
// Brief	: DSP의 입력 64비트를 읽어서 리턴한다.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetIoInData(int *pl1, int *pl2)
{
	int i1, i2;

	i1 = (*ptr_user_iport) & 0xFFFF;
	i1 |= ( (*ptr_user_iport2) & 0xFFFF ) << 16;
	i2 = (*ptr_user_iport3) & 0xFFFF;
	i2 |= ( (*ptr_user_iport4) & 0xFFFF ) << 16;
	
	*pl1 = i1;
	*pl2 = i2;
}

#if (defined(__AMC_SMD) || defined(__AMC_V70))
////////////////////////////////////////////////////////////////////////
// Function	: __GetIoOutData_256
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetIoOutData_256(int *pl1, int *pl2, int *pl3, int *pl4, int *pl5, int *pl6, int *pl7, int *pl8) // 120831 2.9.8 256BIT OUTPUT 출력 읽는 함수
{
	// 현재 출력되고 있는 출력값을 리턴해야 한다. => 다른 함수 이름으로 get_io(..)를 구성해야 함.(오)2006.11.30
	*pl1 = (pGBuf->user_io_out & out_mask[0]);
	*pl2 = (pGBuf->user_io_out2 & out_mask[1]);
	*pl3 = (pGBuf->user_io_out3 & out_mask[2]);
	*pl4 = (pGBuf->user_io_out4 & out_mask[3]);
	*pl5 = (pGBuf->user_io_out5 & out_mask[4]);
	*pl6 = (pGBuf->user_io_out6 & out_mask[5]);
	*pl7 = (pGBuf->user_io_out7 & out_mask[6]);
	*pl8 = (pGBuf->user_io_out8 & out_mask[7]);			
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetIoCount
// Brief	: 설정된 INPUT/OUTPUT PIN 수 읽는 함수
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8
////////////////////////////////////////////////////////////////////////
void __GetIoCount(int *pl1, int *pl2)	
{

	*pl1 = pCBuf[0]->VersatileIO;
	*pl2 = pCBuf[1]->VersatileIO; 
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetIoInData_256
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetIoInData_256(int *pl1, int *pl2, int *pl3, int *pl4, int *pl5, int *pl6, int *pl7, int *pl8) // 120831 2.9.8 256BIT INPUT 읽는 함수
{
	int i1, i2, i3, i4, i5, i6, i7, i8;

	i1 = (*ptr_user_iport) & 0xFFFF;
	i1 |= ( (*ptr_user_iport2) & 0xFFFF ) << 16;
	i2 = (*ptr_user_iport3) & 0xFFFF;
	i2 |= ( (*ptr_user_iport4) & 0xFFFF ) << 16;
	i3 = (*ptr_user_iport5) & 0xFFFF;
	i3 |= ( (*ptr_user_iport6) & 0xFFFF ) << 16;
	i4 = (*ptr_user_iport7) & 0xFFFF;
	i4 |= ( (*ptr_user_iport8) & 0xFFFF ) << 16;
	i5 = (*ptr_user_iport9) & 0xFFFF;
	i5 |= ( (*ptr_user_iport10) & 0xFFFF ) << 16;
	i6 = (*ptr_user_iport11) & 0xFFFF;
	i6 |= ( (*ptr_user_iport12) & 0xFFFF ) << 16;
	i7 = (*ptr_user_iport13) & 0xFFFF;
	i7 |= ( (*ptr_user_iport14) & 0xFFFF ) << 16;
	i8 = (*ptr_user_iport15) & 0xFFFF;
	i8 |= ( (*ptr_user_iport16) & 0xFFFF ) << 16;						
							
	*pl1 = (i1 & in_mask[0]);
	*pl2 = (i2 & in_mask[1]);
	*pl3 = (i3 & in_mask[2]);
	*pl4 = (i4 & in_mask[3]);
	*pl5 = (i5 & in_mask[4]);
	*pl6 = (i6 & in_mask[5]);
	*pl7 = (i7 & in_mask[6]);
	*pl8 = (i8 & in_mask[7]);		
	
}
#endif

////////////////////////////////////////////////////////////////////////
// Function	: __PutSyncMap
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __PutSyncMap(int syn1, int syn2)
{
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetSWLowerLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __GetSWLowerLimit(int ax, int *pli, char *pcst)
{
	*pli = pCBuf[ax]->swlower_limit;
	*pcst = pCBuf[ax]->swlower_limitst;
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetEncoderOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetEncoderOffset(int ax, int ofs)
{
	pCBuf[ax]->EncoderOffset = ofs;
}

////////////////////////////////////////////////////////////////////////
// Function	: __GetEncoderOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int __GetEncoderOffset(int ax)
{
	return pCBuf[ax]->EncoderOffset;
}

////////////////////////////////////////////////////////////////////////
// Function	: SetEncoderOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void SetEncoderOffset()
{
	int ax, ofs;
	
	ax = ReadCommandAxis();
	ofs = ReadChannel(AxisDPRAM[ax], AD_LongType1);
	
	if (pCBuf[ax]->g_on != 0)	// 2011.10.8, 서보 off 상태에서는 설정되지 않도록
	{
		__SetEncoderOffset(ax, ofs);
		_dsp_fset_encoder_offset(ax, ofs);
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: GetEncoderOffset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void GetEncoderOffset()
{
	int ax, ofs;
	
	ax = ReadCommandAxis();
	ofs = __GetEncoderOffset(ax);
	
	WriteChannel(AxisDPRAM[ax], AD_LongType1, ofs);
}

////////////////////////////////////////////////////////////////////////
// Function	: RsvrOnOff
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void RsvrOnOff(int ax, int bOn)
{
	int i = 1 << (ax + 8);

	if(bOn)
	{
		pGBuf->sys_io_out |= i;
	}
	else
	{
		pGBuf->sys_io_out &= ~i;
	}

	*ptr_sys_sctrl = pGBuf->sys_io_out;
}

#if defined(__AMC_V70)
////////////////////////////////////////////////////////////////////////
// Function	: __Get_Monitering_Power
// Brief	: DPRAM에 접근하여 각 축의 DAC 값 Reading(200us 주기로 읽음)
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __Get_Monitering_Power()		
{
	UINT2 Moni_Dac_FPGA;
	char sign=0;

	{
		Moni_Dac_FPGA = (*ptr_Moni_R_Dac0) & 0xFFFF;
		pCBuf[0]->Moni_R_Adc = (int)Moni_Dac_FPGA;		

		if(Moni_Dac_FPGA < 0x8000)	// -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			Moni_Dac_FPGA = 32768 - Moni_Dac_FPGA; 
			sign = -1;
		}
		else
		{
			Moni_Dac_FPGA = Moni_Dac_FPGA - 32768; 
			sign = 1;		
		}
		
		// adc값을 dac 출력 기준으로 scale 변환 		
		Moni_Dac_FPGA = Moni_Dac_FPGA * DACPADC;		// 20194(DAC 출력)로 절대치 scale변환
		pCBuf[0]->Moni_R_Dac = Moni_Dac_FPGA * sign;	// 부호 적용 
		pCBuf[0]->dsp_dac_out = pCBuf[0]->dac_code_b; 
	}

	{
		Moni_Dac_FPGA = (*ptr_Moni_R_Dac1) & 0xFFFF;
		pCBuf[1]->Moni_R_Adc = (int)Moni_Dac_FPGA;	
		
		if(Moni_Dac_FPGA < 0x8000)	// -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			Moni_Dac_FPGA = 32768 - Moni_Dac_FPGA; 
			sign = -1;
		}
		else
		{
			Moni_Dac_FPGA = Moni_Dac_FPGA - 32768; 
			sign = 1;		
		}
		
		// adc값을 dac 출력 기준으로 scale 변환 
		Moni_Dac_FPGA = Moni_Dac_FPGA * DACPADC;		// 20194(DAC 출력)로 절대치 scale변환
		pCBuf[1]->Moni_R_Dac = Moni_Dac_FPGA * sign;	// 부호 적용
		pCBuf[1]->dsp_dac_out = pCBuf[1]->dac_code_b; 		
	}

	{
		Moni_Dac_FPGA = (*ptr_Moni_R_Dac2) & 0xFFFF;
		pCBuf[2]->Moni_R_Adc = (int)Moni_Dac_FPGA;				
		
		if(Moni_Dac_FPGA < 0x8000)	// -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			Moni_Dac_FPGA = 32768 - Moni_Dac_FPGA; 
			sign = -1;
		}
		else
		{
			Moni_Dac_FPGA = Moni_Dac_FPGA - 32768; 
			sign = 1;		
		}
		
		Moni_Dac_FPGA = Moni_Dac_FPGA * DACPADC;		// 20194(DAC 출력)로 절대치 scale변환
		pCBuf[2]->Moni_R_Dac = Moni_Dac_FPGA * sign;	// 부호 적용
		pCBuf[2]->dsp_dac_out = pCBuf[2]->dac_code_b; 			
	}

	{
		Moni_Dac_FPGA = (*ptr_Moni_R_Dac3) & 0xFFFF;
		pCBuf[3]->Moni_R_Adc = (int)Moni_Dac_FPGA;			
					
		if(Moni_Dac_FPGA < 0x8000)	// -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			Moni_Dac_FPGA = 32768 - Moni_Dac_FPGA; 
			sign = -1;
		}
		else
		{
			Moni_Dac_FPGA = Moni_Dac_FPGA - 32768; 
			sign = 1;		
		}
		
		Moni_Dac_FPGA = Moni_Dac_FPGA * DACPADC;		// 20194(DAC 출력)로 절대치 scale변환
		pCBuf[3]->Moni_R_Dac = Moni_Dac_FPGA * sign;	// 부호 적용
		pCBuf[3]->dsp_dac_out = pCBuf[3]->dac_code_b; 					
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: Check_Monitering_System
// Brief	: 모니터링 기능이 Enable되어 있고, 이상이 있는 경우 axis_source, state 설정
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Check_Monitering_System()		
{
	char c_cutoff_en;	
	char c_cutoff_st;	
    int	value, fpga_data, dac_data;
	int power3p3_val, power5p_val, power12p_val, power12m_val;
	volatile char Power3_3V_cutoffen, Power5V_cutoffen, Power12VP_cutoffen, Power12VM_cutoffen;
	volatile int ConditionV;
	int i;	

	_dsp_get_system_moni_enable(&c_cutoff_en);
	_dsp_system_status(&c_cutoff_st);	

	for(i = 0; i<4; i++)
	{
		__g_system_monitering_value(i, &value, &fpga_data, &dac_data);

		switch (i)
		{
			case 0:	// 3.3V
				power3p3_val = value & 0xffff;
				break;
				
			case 1: // 5V
				power5p_val = value & 0xffff;
				break;
				
			case 2: // +12V
				power12p_val = value & 0xffff;
				break;
				
			case 3: // -12V
				power12m_val = -value & 0xffff;
				break;
		}
	}

	VCC_Moniter = ((power5p_val << 16) & 0xffff0000) | (power3p3_val & 0x0000ffff);
	VCC_Moniter_2 = ((power12m_val << 16) & 0xffff0000) | (power12p_val & 0x0000ffff);

	Power3_3V_cutoffen = (c_cutoff_en & (1<<POWER3P3));
	ConditionV = (!Power3_3V_cutoffen && ((power3p3_val > Power3_3V_Upper_Limit) || (power3p3_val < Power3_3V_Lower_Limit)));
	
	if((Power3_3V_cutoffen && (c_cutoff_st & (1<<POWER3P3))) || ConditionV)
	{
	    if(POWER3P3_Status_Cnt++ > POWER3P3_CHECK_TIME)
	    {
	    	for(i=0 ; i<BD_AXIS_NUM ; i++)
	    	{
	    		pCBuf[i]->axis_source |= ST_SYSTEM_INSEC;
	    		
    			if( pCBuf[i]->event_st < ABORT_EVENT )	pCBuf[i]->event_st = ABORT_EVENT ;		 
			}

            POWER3P3_Status_Cnt--;
        }            
        else
        {
            *ptr_cutoff_status = (*ptr_cutoff_status) & ~(1<<POWER3P3);            
        }
	}
	else
	{
		POWER3P3_Status_Cnt = 0;
	}

	Power5V_cutoffen = (c_cutoff_en & (1<<POWER5P));
	ConditionV = (!Power5V_cutoffen && ((power5p_val > Power5V_Upper_Limit) || (power5p_val < Power5V_Lower_Limit)));

	if((Power5V_cutoffen && (c_cutoff_st & (1<<POWER5P))) || ConditionV)			
	{
	    if(POWER5P_Status_Cnt++ > POWER5P_CHECK_TIME)
	    {
	    	for(i=0 ; i<BD_AXIS_NUM ; i++)
	    	{
	    		pCBuf[i]->axis_source |= ST_SYSTEM_INSEC;
	    		
    			if( pCBuf[i]->event_st < ABORT_EVENT )	pCBuf[i]->event_st = ABORT_EVENT ;		 
			}
			
            POWER5P_Status_Cnt--;
        }            
        else
        {
            *ptr_cutoff_status = (*ptr_cutoff_status) & ~(1<<POWER5P);            
        }
	}
	else
	{
		POWER5P_Status_Cnt = 0;
	}

	Power12VP_cutoffen = (c_cutoff_en & (1<<POWER12P));
	ConditionV = (!Power12VP_cutoffen && ((power12p_val > Power12VP_Upper_Limit) || (power12p_val < Power12VP_Lower_Limit)));
	
	if((Power12VP_cutoffen && (c_cutoff_st & (1<<POWER12P))) || ConditionV)				
	{
        if(POWER12P_Status_Cnt++ > POWER12P_CHECK_TIME)
	    {
	    	for(i=0 ; i<BD_AXIS_NUM ; i++)
	    	{
	    		pCBuf[i]->axis_source |= ST_SYSTEM_INSEC;
	    		
    			if( pCBuf[i]->event_st < ABORT_EVENT )	pCBuf[i]->event_st = ABORT_EVENT ;		 
			}

            POWER12P_Status_Cnt--;
        }            
        else
        {
			*ptr_cutoff_status = (*ptr_cutoff_status) & ~(1<<POWER12P);
        }
	}
	else
	{
		POWER12P_Status_Cnt = 0;
	}

	Power12VM_cutoffen = (c_cutoff_en & (1<<POWER12M));
	ConditionV = (!Power12VM_cutoffen && ((power12m_val > Power12VM_Upper_Limit) || (power12m_val < Power12VM_Lower_Limit)));
	
	if((Power12VM_cutoffen && (c_cutoff_st & (1<<POWER12M))) || ConditionV)				
	{
	    if(POWER12M_Status_Cnt++ > POWER12Pm_CHECK_TIME)
	    {
	    	for(i=0 ; i<BD_AXIS_NUM ; i++)
	    	{
	    		pCBuf[i]->axis_source |= ST_SYSTEM_INSEC;
	    		
    			if( pCBuf[i]->event_st < ABORT_EVENT )	pCBuf[i]->event_st = ABORT_EVENT ;		 
			}

            POWER12M_Status_Cnt--;
        }            
        else
        {
            *ptr_cutoff_status = (*ptr_cutoff_status) & ~(1<<POWER12M);
        }
	}
	else
	{
		POWER12M_Status_Cnt = 0;
	}
	
	//VCC_Moniter_2 = ((POWER12M_Status_Cnt << 24) & (0xff<<24)) |((POWER12P_Status_Cnt << 16) & (0xff<<16)) |((POWER5P_Status_Cnt << 8) & (0xff<<8)) | (POWER3P3_Status_Cnt & 0xff) ;

}

////////////////////////////////////////////////////////////////////////
// Function	: __system_monitering_enable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __system_monitering_enable(char moni_nu, char en_d)
{
	char tmp;

	// +12,-12,+5,+3.3전원 0,1,2,3
	if(en_d ==TRUE)	
	{
		tmp = (*ptr_cutoff_status) & 0x0F; 		// 모니터링 상태 clear
		tmp &= ~(1<<moni_nu);
		(*ptr_cutoff_status) = tmp & 0x0F; 	

		tmp = (*ptr_cutoff_enable) & 0x0F; 		// 모니터링 enable
		tmp |= (1<<moni_nu);
		(*ptr_cutoff_enable) = tmp & 0x0F; 
	}
	else
	{
		tmp = (*ptr_cutoff_enable) & 0x0F; 		// 모니터링 disable	
		tmp &= ~(1<<moni_nu);
		(*ptr_cutoff_enable) = tmp & 0x0F; 

		tmp = (*ptr_cutoff_status) & 0x0F; 		// 모니터링 상태 clear
		tmp &= ~(1<<moni_nu);
		(*ptr_cutoff_status) = tmp & 0x0F; 	
	}		
}

////////////////////////////////////////////////////////////////////////
// Function	: __g_system_monitering_value
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __g_system_monitering_value(char  moni_nu, int *value, int *raw_val, int *compare_val)	// value = 0~3(전압*100), 4~7(각 축adc값(32767)을 dac(20194)로 변환 한값)
{																							// raw_val = 0~7(fpga를 통해 읽어온 값(adc값))
	UINT2 i_value;																			// compare_val = 0~3(=0), 4~7(DSP DAC출력 값)	
	char sign;
	float f_val;
	
	if(moni_nu ==0)	
	{
		i_value	= (*ptr_power_3p3) & 0xFFFF;
		*raw_val = (int)i_value;

		// scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value; 
			sign =-1;
		}
		else
		{
			i_value = i_value - 32768; 
			sign =1;		
		}
		
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val*sign;			// 부호 적용
		*compare_val = 0;								
	}
	else if(moni_nu == 1)	
	{
		i_value	= (*ptr_power_5p) & 0xFFFF;
		*raw_val = (int)i_value;			
		
		// scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value; 
			sign =-1;
		}
		else
		{
			i_value = i_value - 32768; 
			sign = 1;		
		}
			
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용		
		*compare_val = 0;						
	}
	else if(moni_nu == 2)	
	{
		i_value	= (*ptr_power_12p) & 0xFFFF;
		*raw_val = (int)i_value;			
		
		// scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value; 
			sign =-1;
		}
		else
		{
			i_value = i_value - 32768; 
			sign = 1;		
		}
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용	
		*compare_val = 0;						
	}
	else if(moni_nu ==3)	
	{
		i_value	= (*ptr_power_12m) & 0xFFFF;
		*raw_val = (int)i_value;			
		
		// scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value; 
			sign =-1;
		}
		else
		{
			i_value = i_value - 32768; 
			sign =1;		
		}
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용
		*compare_val = 0;			
	}
	else if(moni_nu < 8)
	{
		*value = (int)pCBuf[moni_nu-4]->Moni_R_Dac;			
		*raw_val = (int)pCBuf[moni_nu-4]->Moni_R_Adc & 0xFFFF;	
		*compare_val = pCBuf[moni_nu-4]->dsp_dac_out;				
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __s_monitering_threshold_percent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __s_monitering_threshold_percent(char  moni_nu, char pcnt)
{
	UINT2 s_val;
	float f_val;
	float f_div;	
	
	f_div = (float)pcnt/PERCENT100;
	
	if(moni_nu ==0)	
	{
		f_val= ceil(REF_POWER_FPGA3P3 *f_div);
		s_val= (UINT2)f_val;
		(*ptr_power_cutoff_3p3) = (s_val & 0x7FFF);		
	}
	else if(moni_nu ==1)	
	{
		f_val= ceil(REF_POWER_FPGA5P *f_div);
		s_val= (UINT2)f_val;
		(*ptr_power_cutoff_5p) = (s_val & 0x7FFF);		
	}
	else if(moni_nu ==2)	
	{
		f_val= ceil(REF_POWER_FPGA12 *f_div);
		s_val= (UINT2)f_val;
		(*ptr_power_cutoff_12p) = (s_val & 0x7FFF);		
	}
	else if(moni_nu ==3)	
	{
		f_val= ceil(REF_POWER_FPGA12 *f_div);
		s_val= (UINT2)f_val;
		(*ptr_power_cutoff_12m) = (s_val & 0x7FFF);			
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: __g_monitering_threshold_percent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __g_monitering_threshold_percent(char  moni_nu, char *pcnt)
{
	UINT2 s_val;
	float f_div;	
	
	if(moni_nu ==0)	
	{
		s_val = (*ptr_power_cutoff_3p3) & 0x7FFF;	
		f_div = (float)s_val/REF_POWER_FPGA3P3;
		*pcnt = (char)(f_div*PERCENT100);	
	}
	else if(moni_nu ==1)	
	{
		s_val = (*ptr_power_cutoff_5p) & 0x7FFF;	
		f_div = (float)s_val/REF_POWER_FPGA5P;
		*pcnt = (char)(f_div*PERCENT100);		
	}
	else if(moni_nu ==2)	
	{
		s_val = (*ptr_power_cutoff_12p) & 0x7FFF;	
		f_div = (float)s_val/REF_POWER_FPGA12;
		*pcnt = (char)(f_div*PERCENT100);	
	}
	else if(moni_nu ==3)	
	{
		s_val = (*ptr_power_cutoff_12m) & 0x7FFF;	
		f_div = (float)s_val/REF_POWER_FPGA12;
		*pcnt = (char)(f_div*PERCENT100);		
	}
}

/*
[31:16] Max Peak :  unsigned short (0x0000 ~ 0xFFFF : 0 ~ 65536 : -12 ~ +12V)
[15:0] Min Peak :  unsigned short (0x0000 ~ 0xFFFF : 0 ~ 65536 : -12 ~ +12V)
- "0" 을 Write 하면 Clear 됨	R/C (Read 또는 Clear 레지스터)

40410	0XA010 1040	Max / Min Peak Hold 3.3V 전원
40411	0XA010 1044	Max / Min Peak Hold 5V 전원
40412	0XA010 1048	Max / Min Peak Hold +12V 전원
40413	0XA010 104C	Max / Min Peak Hold -12V 전원
*/

void __s_system_monitering_peak_hold_value(char moni_nu, int raw_val)
{
	// value=0 일때(clear) 만 가능	--> fixme:
	switch (moni_nu)
	{
		case 0:
			*ptr_Power_MinMaxPeak_3p3 = raw_val; break;
		case 1:
			*ptr_Power_MinMaxPeak_5p = raw_val; break;
		case 2:
			*ptr_Power_MinMaxPeak_12p = raw_val; break;
		case 3:
			*ptr_Power_MinMaxPeak_12m = raw_val; break;
		default:
			break;
	}
}

void __g_system_monitering_peak_hold_value(char moni_nu, int *value, int *raw_val, BOOL bMax) // min:FALSE, max:TRUE
{
	UINT2 i_value;
	char sign;
	float f_val;

	if(moni_nu ==0)
	{
		i_value	= bMax ? (((*ptr_Power_MinMaxPeak_3p3)>>16) & 0xFFFF) : ((*ptr_Power_MinMaxPeak_3p3) & 0xFFFF);
		*raw_val = (int)i_value;

		//scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value;
			sign =-1;
		}
		else	//
		{
			i_value = i_value - 32768;
			sign =1;
		}

		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val*sign;			// 부호 적용
		//*compare_val = 0;
	}
	else if(moni_nu ==1)
	{
		i_value	= bMax ? (((*ptr_Power_MinMaxPeak_5p)>>16) & 0xFFFF) : ((*ptr_Power_MinMaxPeak_5p) & 0xFFFF);
		*raw_val = (int)i_value;
		//scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value;
			sign =-1;
		}
		else	//
		{
			i_value = i_value - 32768;
			sign =1;
		}

		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용
		//*compare_val = 0;
	}
	else if(moni_nu ==2)
	{
		i_value	= bMax ? (((*ptr_Power_MinMaxPeak_12p)>>16) & 0xFFFF) : ((*ptr_Power_MinMaxPeak_12p) & 0xFFFF);
		*raw_val = (int)i_value;
		//scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value;
			sign =-1;
		}
		else	//
		{
			i_value = i_value - 32768;
			sign =1;
		}
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용
		//*compare_val = 0;
	}
	else if(moni_nu ==3)
	{
		i_value	= bMax ? (((*ptr_Power_MinMaxPeak_12m)>>16) & 0xFFFF) : ((*ptr_Power_MinMaxPeak_12m) & 0xFFFF);
		*raw_val = (int)i_value;
		//scale 변경
		if(i_value <0x8000) // -부호 0x0000 ~ 0xffff ( -12V ~ +12V, -12V : 0x0000, 0V : 0x8000, 12V : 0xffff)
		{
			i_value = 32768 - i_value;
			sign =-1;
		}
		else	//
		{
			i_value = i_value - 32768;
			sign =1;
		}
		f_val = (float)i_value * VOLTPADC;	// scale변환
		*value = (int)f_val *sign;			// 부호 적용
		//*compare_val = 0;
	}
	/*
	else if(moni_nu < 8) // 4,5,6,7
	{
		*value = (int)pCBuf[moni_nu-4]->Moni_R_Dac;
		*raw_val = (int)pCBuf[moni_nu-4]->Moni_R_Adc & 0xFFFF;
		*compare_val = pCBuf[moni_nu-4]->dsp_dac_out;
	}
	*/
}

void __s_system_decelstop_ratio(char axno, double fDecelRatio)
{
	UINT2 s_val;
	double f_val;

	if(axno ==0)	
	{
		f_val= ceil(fDecelRatio);
		s_val= (UINT2)f_val;
		(*ptr_WdStopDecelRatio_Ax0) = (s_val & 0xFF);
	}
	else if(axno ==1)	
	{
		f_val= ceil(fDecelRatio);
		s_val= (UINT2)f_val;
		(*ptr_WdStopDecelRatio_Ax1) = (s_val & 0xFF);
	}
	else if(axno ==2)	
	{
		f_val= ceil(fDecelRatio);
		s_val= (UINT2)f_val;
		(*ptr_WdStopDecelRatio_Ax2) = (s_val & 0xFF);
	}
	else if(axno ==3)	
	{
		f_val= ceil(fDecelRatio);
		s_val= (UINT2)f_val;
		(*ptr_WdStopDecelRatio_Ax3) = (s_val & 0xFF);		
	}
}

void __g_system_decelstop_ratio(char  axno, double *pfDecelRatio)
{
	UINT2 s_val;
	double f_val;	
	
	if(axno ==0)	
	{
		s_val = (*ptr_WdStopDecelRatio_Ax0) & 0xFF;
		f_val = (double)s_val;
		*pfDecelRatio = (double)(f_val);	
	}
	else if(axno ==1)	
	{
		s_val = (*ptr_WdStopDecelRatio_Ax1) & 0xFF;
		f_val = (double)s_val;
		*pfDecelRatio = (char)(f_val);		
	}
	else if(axno ==2)	
	{
		s_val = (*ptr_WdStopDecelRatio_Ax2) & 0xFF;
		f_val = (double)s_val;
		*pfDecelRatio = (char)(f_val);	
	}
	else if(axno ==3)	
	{
		s_val = (*ptr_WdStopDecelRatio_Ax3) & 0xFF;
		f_val = (double)s_val;
		*pfDecelRatio = (char)(f_val);		
	}
}

#endif

