#define DSP_IF_PLIMIT_GLOBALS

#include "all.h"

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_Find_Bd_Jnt
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	_dsp_Find_Bd_Jnt(int ax, int *bn, int *jnt)
{
	int error_code;
	 
	amc_error = MMC_OK;
	error_code = MMC_OK;
	
	if ((ax>=TOTAL_AXIS_NUM) || ax<0)
	{
  	  	error_code	= MMC_INVALID_AXIS;
  	 	amc_error = MMC_INVALID_AXIS;
  	}
	else
	{
		*jnt = ax;
	}

	amc_error = error_code;			
	
	return error_code;				
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_positive_sw_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_positive_sw_limit(int ax, double limit, int action)
{
	LONG	limit_l;
	
	if(action<0 || action>ABORT_EVENT)
	{
		return amc_error = MMC_ILLEGAL_PARAMETER;
	}
	if((int)limit>MMC_POS_SW_LIMIT)
	{
		return amc_error = MMC_ILLEGAL_PARAMETER;
	}
	
	limit_l=(LONG) (limit);
	
	__SetSWUpperLimit(ax, limit_l, action);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_positive_sw_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_positive_sw_limit(int ax, double* limit, int* action)
{
	LONG	li;
	char cst;

	__GetSWUpperLimit(ax, &li, &cst);
	
	*limit=(LONG)li;
	*action = cst;
	
	return	amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_negative_sw_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_negative_sw_limit(int ax, double limit, int action)
{
	LONG	limit_l;
	
	if(action<0 || action>ABORT_EVENT)
	{
		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}
	if((int)limit<MMC_NEG_SW_LIMIT)
	{
		return  amc_error=MMC_ILLEGAL_PARAMETER;
	}
	
	limit_l=(LONG)limit;
	
	__PutSWLowerLimit(ax, limit_l, action);
	
	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_negative_sw_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int	_dsp_get_negative_sw_limit (int ax, double* limit, int* action)
{
	LONG	li;
	char cst;

	__GetSWLowerLimit(ax, &li, &cst);
	
	*limit = (float)li;
	*action = cst;
	
	return	MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_accel_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_accel_limit(int ax, int* limit)
{
	int	bn,jnt;

	if(_dsp_Find_Bd_Jnt(ax,&bn,&jnt))
	{
		return	amc_error;
	}

	*limit=pCBuf[ax]->Accel_Limit;

	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_accel_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_accel_limit(int ax, int limit)
{
	int	bn,jnt;

	if(_dsp_Find_Bd_Jnt(ax,&bn,&jnt))
	{
		return	amc_error;
	}

	if(limit<1)
	{
		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}
	else if(limit>MMC_ACCEL_LIMIT)
	{
		pCBuf[ax]->Accel_Limit = MMC_ACCEL_LIMIT;
		BootPara.st_boot.Accel_Limit[jnt]=MMC_ACCEL_LIMIT;
		SetBootParaCrc(&BootPara);

		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}

	pCBuf[ax]->Accel_Limit=limit;

	BootPara.st_boot.Accel_Limit[jnt]=limit;
	
	SetBootParaCrc(&BootPara);

	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_vel_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_vel_limit(int ax, double* limit)
{
	int	bn,jnt;

	if(_dsp_Find_Bd_Jnt(ax,&bn,&jnt))	
	{
		return	amc_error;
	}
	
	*limit=(float)BootPara.st_boot.Vel_Limit[jnt];

	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_vel_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_vel_limit(int ax, double limit)
{
	int	bn,jnt;

	if(_dsp_Find_Bd_Jnt(ax,&bn,&jnt))
	{
		return	amc_error;
	}

	if(limit<=0)
	{
		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}
	else if(limit>MMC_VEL_LIMIT)
	{
		pCBuf[ax]->Vel_Limit = BootPara.st_boot.Vel_Limit[jnt] = (MMC_VEL_LIMIT);
		SetBootParaCrc(&BootPara);

		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}

	pCBuf[ax]->Vel_Limit = BootPara.st_boot.Vel_Limit[jnt] = (limit);
	
	SetBootParaCrc(&BootPara);

	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_set_error_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_set_error_limit(int ax, double limit, int action)
{
	LONG	limit_l;

	if(limit<=0.0 || limit > MMC_ERROR_LIMIT)
	{
		return	amc_error=MMC_ILLEGAL_PARAMETER;
	}
	
	limit_l=(LONG)limit;
	
	__SetErrorLimit(ax, limit_l, action);
	
	return amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_error_limit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_error_limit(int ax, double* limit, int* action)
{
	int	bn,jnt;
	int	limit_l;
	char cst;

	if(_dsp_Find_Bd_Jnt(ax,&bn,&jnt))
	{
		return	amc_error;
	}
	
	__GetErrorLimit(ax, &limit_l, &cst);
	
	*action = cst;
	
	*limit=(float)limit_l;
	
	return	amc_error=MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_out64
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int _dsp_get_out64(int* value)
{
	__GetIoOutData(&value[0], &value[1]);

	return MMC_OK;
}

#if (defined(__AMC_SMD) || defined(__AMC_V70))
////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_io_output
// Brief	: 256BIT OUTPUT출력을 읽어옴  
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK DSP
////////////////////////////////////////////////////////////////////////
int _dsp_get_io_output(int* value)
{
	__GetIoOutData_256(&value[0], &value[1], &value[2], &value[3], &value[4], &value[5], &value[6], &value[7]);
	
	return MMC_OK;
}

////////////////////////////////////////////////////////////////////////
// Function	: _dsp_get_io_count
// Brief	: 설정된 INPUT/OUTPUT PIN을 읽어옴 
// Input	:
// Return	:
// History	:
// Date		: 120831 2.9.8 SYK
////////////////////////////////////////////////////////////////////////
int _dsp_get_io_count(int* in_c, int* out_c)	
{
	int tmp_in, tmp_out;
	
	__GetIoCount(&tmp_in, &tmp_out);
	
	*in_c = tmp_in;
	*out_c = tmp_out;	
	
	return MMC_OK;
}
#endif

