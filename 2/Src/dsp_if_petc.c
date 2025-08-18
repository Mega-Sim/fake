#define DSP_IF_PETC_GLOBALS

#include "all.h"

////////////////////////////////////////////////////////////////////////
// Function	: ProcessBootPara
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void ProcessBootPara(int ax, int val, int comm)
{
	if (comm==PUT_HOME_LEVEL)			BootPara.st_boot.Home_Level[ax]=(CHAR)val;
	else if(comm==PUT_POS_LEVEL)		BootPara.st_boot.Pos_Level[ax]=(CHAR)val;
	else if(comm==PUT_NEG_LEVEL)		BootPara.st_boot.Neg_Level[ax]=(CHAR)val;
	else if(comm==PUT_AMP_FAULT)		BootPara.st_boot.Amp_Fault_Event[ax]=(CHAR)val;
	else if(comm==PUT_AMP_FAULT_LEVEL)	BootPara.st_boot.Amp_Level[ax]=(CHAR)val;
	else if(comm==PUT_AMP_RESET_LEVEL)	BootPara.st_boot.Amp_Reset_Level[ax]=(CHAR)val;
	else if(comm==PUT_POS_EVENT)		BootPara.st_boot.Pos_Limit_St[ax]=(CHAR)val;
	else if(comm==PUT_NEG_EVENT)		BootPara.st_boot.Neg_Limit_St[ax]=(CHAR)val;
	else if(comm==PUT_HOME_EVENT)		BootPara.st_boot.Home_Limit_St[ax]=(CHAR)val;
	else if(comm==PUT_STOP_RATE)		BootPara.st_boot.Stop_Rate[ax]=(CHAR)val;
	else if(comm==PUT_E_STOP_RATE)		BootPara.st_boot.E_Stop_Rate[ax]=(CHAR)val;
	else if(comm==PUT_FEEDBACK_DEVICE)	BootPara.st_boot.Encoder_Cfg[ax]=(CHAR)val;
	else if(comm==PUT_VOLTAGE_DEVICE)	BootPara.st_boot.Voltage_Cfg[ax]=(CHAR)val;
	else if(comm==PUT_INDEX_REQUIRED)	BootPara.st_boot.Home_Index[ax]=(CHAR)val;
	else if(comm==PUT_CLOSED_LOOP)		BootPara.st_boot.Loop_Cfg[ax]=(CHAR)val;
	else if(comm==PUT_SERVO_ON_LEVEL)	BootPara.st_boot.Amp_OnLevel[ax]=(CHAR)val;
	else if(comm==PUT_INT_ENABLE)		BootPara.st_boot.Io_Int_Enable[ax] = (val==0) ? 0 : 1;
	else if(comm==PUT_INT_STOP)			BootPara.st_boot.Int_Event_St[ax] = (val==0) ? 0 : 1;
	else if(comm==PUT_INT_E_STOP)		BootPara.st_boot.Int_Event_St[ax] = (val==0) ? 0 : 2;
	else if(comm==PUT_INT_PCINT)		BootPara.st_boot.Int_Event_St[ax] = (val==0) ? 0 : 3;
	else if(comm==PUT_VT_CONTROL)		BootPara.st_boot.Control_Cfg[ax]=(CHAR)val;
	else if(comm==PUT_POS_I_MODE)     	BootPara.st_boot.PosImode[ax]=(CHAR)val;
	else if(comm==PUT_VEL_I_MODE)     	BootPara.st_boot.VelImode[ax]=(CHAR)val;
	else if(comm==PUT_PULSE_MODE)		BootPara.st_boot.PulseMode[ax]=(CHAR)val;
	else if(comm==PUT_PULSE_RATIO)		BootPara.st_boot.PulseRatio[ax]=val;
	
	// 2011.4.20, ckyu
	// 자동으로 crc를 계산한다.
	
	SetBootParaCrc(&BootPara);
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_amp_enable_level
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_amp_enable_level(INT axno,INT level)
{
	__SetAmpOnLevel(axno, level);
	ProcessBootPara(axno, level, PUT_SERVO_ON_LEVEL);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_amp_enable_level
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_amp_enable_level(INT axno,pINT level)
{
	*level = __GetAmpOnLevel(axno);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_amp_enable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_amp_enable(INT axno,INT state)
{
	if (state == TRUE)
	{
		__ServoOn(axno);
	}
	else
	{
		__ServoOff(axno);
	}
	
	return MMC_OK;
}

#if defined(__AMC_V70)
////////////////////////////////////////////////////////////////////////
// Function	: _dsp_system_moni_enable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_system_moni_enable(char axno,char state)
{
	if(axno<POWER3P3 || axno>POWER12M)
	{
		return	MMC_ILLEGAL_PARAMETER;
	}

	if(state<FALSE || state> TRUE) 
	{
		return   MMC_ILLEGAL_PARAMETER;
	}
	
	__system_monitering_enable(axno, state);	
	
	return MMC_OK;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_system_monitering_value
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_system_monitering_value(char axno, int *val, int *rval, int *cval)
{
	int i_val, i_rval, i_cval;

	if(axno<POWER3P3 || axno>AXIS3_VEL)
	{
		return	MMC_ILLEGAL_PARAMETER;
	}

	__g_system_monitering_value(axno, &i_val, &i_rval, &i_cval);

	*val = i_val;
	*rval = i_rval & 0xffff;  // unsigned short type 변환.
	*cval = i_cval;
	
	return MMC_OK;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_monitering_Threshold_percent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_monitering_Threshold_percent(char axno, char pcnt) 
{
	if(axno<POWER3P3 || axno>POWER12M)
	{
		return	MMC_ILLEGAL_PARAMETER;
	}
	
	if(pcnt<PERCENT0 || pcnt>PERCENT100)
	{
		return	MMC_ILLEGAL_PARAMETER;
	}	

	__s_monitering_threshold_percent(axno, pcnt);
	return MMC_OK;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_monitering_Threshold_percent
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_monitering_Threshold_percent(char axno, char *pcnt) 
{
	char c_pcnt;
	
	if(axno<POWER3P3 || axno>POWER12M)
	{
		return	MMC_ILLEGAL_PARAMETER;
	}
	
	__g_monitering_threshold_percent(axno, &c_pcnt);
	
	*pcnt = c_pcnt;
	
	return MMC_OK;	
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_system_status
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_system_status(char *status)
{
	char tmp_fpga;
	
	tmp_fpga= (*ptr_cutoff_status) & 0x0F;	// 전원(+12,-12,+5,+3.3) 상태 이상여부 체크
 	*status = tmp_fpga;
		
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_system_moni_enable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_system_moni_enable(char *state)
{
	char tmp_fpga;
	
	tmp_fpga   = (*ptr_cutoff_enable) & 0x0F;	// 전원(+12,-12,+5,+3.3) 상태 이상여부 체크
	*state = tmp_fpga;

	return MMC_OK;
}
#endif

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_amp_enable
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_amp_enable(INT axno, pINT state)
{
	*state = TRUE;
	
	if (pCBuf[axno]->axis_source & ST_AMP_POWER_ONOFF)
	{
		*state = FALSE;
	}
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_amp_fault_reset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_amp_fault_reset(INT axno)
{
	__ResetAlarm( axno );
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_amp_fault_set
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_amp_fault_set(INT axno)
{
	__SetAlarm( axno );
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_sync_map_axes
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_sync_map_axes(INT Master, INT Slave)
{

	if( Master == Slave )
	{
	    return	amc_error=MMC_ILLEGAL_PARAMETER;
	}

	SyncMotion.Master=Master;
	SyncMotion.Slave =Slave;
	
	return	amc_error;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_sync_control
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_sync_control(INT condition)
{
	SyncMotion.Flag = condition;
	
	return	amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_sync_control
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_sync_control(pINT condition)
{
	*condition = SyncMotion.Flag;
	
	return	amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_sync_gain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_sync_gain(float syncgain)
{
	SyncMotion.Gain = syncgain;
	pCBuf[SyncMotion.Slave]->V_TrackingFactor = syncgain;

	BootPara.st_boot.V_TrackingFactor[SyncMotion.Slave] = syncgain;
	SetBootParaCrc(&BootPara);

	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_sync_gain
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_sync_gain(pFLOAT syncgain)
{
	*syncgain = pCBuf[SyncMotion.Slave]->V_TrackingFactor;
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_version_chk
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_version_chk(INT bn, pINT ver)
{
	*ver = 0;
	*ver = ReadChannel2(CommDPRAM, CD_Ver);
	
	return	MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_tracking_factor
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_get_tracking_factor(INT axno, pDOUBLE pfFactor)
{
	*pfFactor = __GetTrackingFactor(axno);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_tracking_factor
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
INT _dsp_set_tracking_factor(INT axno, double fFactor)
{
	__SetTrackingFactor(axno, fFactor);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_dac_output
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 2006.12.07(오)
////////////////////////////////////////////////////////////////////////
INT _dsp_get_dac_output(INT axis, pINT voltage)
{
	*voltage = pCBuf[axis]->dac_code;
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_dac_output
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
INT _dsp_set_dac_output(INT axis, INT voltage)
{
	pCBuf[axis]->dac_code = voltage;

	// 2007.1.23(오), DAC 출력 전압을 직접 사용하면 Motion 제어 루프를 사용하지 않도록 해야 한다. 
	if(voltage == 0)	
	{
		pCBuf[axis]->offset_on=0;
	}
	else
	{
		pCBuf[axis]->offset_on=1;
	}
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_analog_offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
INT _dsp_set_analog_offset(INT axis, INT voltage)
{
	pCBuf[axis]->dac_bias = voltage;
	BootPara.st_boot.dac_bias[axis] = voltage;
	
	SetBootParaCrc(&BootPara);		// 자동으로 crc를 계산한다. // 2011.4.20, ckyu

	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_analog_offset
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
INT _dsp_get_analog_offset(INT axis, pINT voltage)
{
	*voltage = pCBuf[axis]->dac_bias;
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void _dsp_set_bit(INT bitNo)
{
	__SetIoBit(bitNo);
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_reset_bit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		: 
////////////////////////////////////////////////////////////////////////
void _dsp_reset_bit(INT bitNo)
{
	__ResetIoBit(bitNo);
}

////////////////////////////////////////////////////////////////////////
// Function	: __SetWorkingLimit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void __SetWorkingLimit(int ax, int e_stop_rate)
{
	// INI 파일의 최고 속도를 이용하여 E-Stop Rate로 감속할 거리를 Working Limit의 Offset 으로 취한다.
	// pCBuf[ax]->working_limit = 8192. * min(5., 100. * e_stop_rate / 1000.);	//모터 2회전 이상 위치를 벗어나는 경우 에러를 처리하도록
	// if (pCBuf[ax]->working_limit <= 0) pCBuf[ax]->working_limit = 8192. * 5;
	pCBuf[ax]->working_limit = (pCBuf[ax]->Vel_Limit * 100 ) / 1000.0;
	
	if(pCBuf[ax]->working_limit < pCBuf[ax]->encRes * 2)
	{
		pCBuf[ax]->working_limit = pCBuf[ax]->encRes * 2;
	}
	
	if(pCBuf[ax]->working_limit > pCBuf[ax]->encRes * 10)
	{
		pCBuf[ax]->working_limit = pCBuf[ax]->encRes * 10;
	}
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_amc_init
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_amc_init(VOID)
{
	int	i;

	amc_error = MMC_OK;
	
	for (i=0; i < MMC_AXIS; i++)
	{
		_dsp_amp_fault_set (i);
		_dsp_set_amp_enable (i, FALSE);	
	}
	
	return	amc_error;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_error
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_error(int ax, double* error)
{
	float	error_f;
	
	error_f = __GetErrorPls(ax); 
	*error = error_f;
	
	return	MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_com_velocity
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_com_velocity(int ax)
{
	int	comv_i;
	
	comv_i = pCBuf[ax]->velo_z0 * G_fSampling_Time;
	
	return comv_i;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_act_velocity
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_act_velocity(int ax)
{
	int	actv_i;
	
	actv_i = pCBuf[ax]->velfbz0 * G_fSampling_Time;
	
	return actv_i;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_com_velocity_rpm
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_com_velocity_rpm(int ax)
{
	int	comv_i_rpm;
	
	comv_i_rpm = (_dsp_get_com_velocity(ax) * 60.) / pCBuf[ax]->encRes;
	
	return comv_i_rpm;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_act_velocity_rpm
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_act_velocity_rpm(int ax)
{
	int	actv_i_rpm;
	
	actv_i_rpm = (_dsp_get_act_velocity(ax) * 60. ) / pCBuf[ax]->encRes;
	
	return actv_i_rpm;
}

