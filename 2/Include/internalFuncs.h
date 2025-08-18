#ifndef __INTERNAL_FUNCS_H
#define	__INTERNAL_FUNCS_H

#ifdef INTERNALFUNCS_GLOBALS
#define INTERNAL_FUNCS_EXT
#else
#define INTERNAL_FUNCS_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
INTERNAL_FUNCS_EXT void _dsp_param_flush();
INTERNAL_FUNCS_EXT void _dsp_set_encoder_offset(int ax, int ofs);
INTERNAL_FUNCS_EXT void _dsp_fset_encoder_offset(int ax, int ofs);
INTERNAL_FUNCS_EXT int _dsp_get_encoder_offset(int ax);
INTERNAL_FUNCS_EXT void __SetPosition(int ax, int pos);
INTERNAL_FUNCS_EXT void __GetSyncEncoder(int *ppos_m, int *ppos_s);
INTERNAL_FUNCS_EXT int __GetCurPos(int ax);
INTERNAL_FUNCS_EXT int __GetPos(int ax);
INTERNAL_FUNCS_EXT float __GetErrorPls(int ax);
INTERNAL_FUNCS_EXT void __SetErrorLimit(int ax, int lchk, char cst);
INTERNAL_FUNCS_EXT void __GetErrorLimit(int ax, int *plchk, char *pcst);
INTERNAL_FUNCS_EXT void __SetPosLimit(int ax);
INTERNAL_FUNCS_EXT void __ServoOn(int ax);
INTERNAL_FUNCS_EXT void __ServoOff(int ax);
INTERNAL_FUNCS_EXT void __ResetAlarm( int ax );
INTERNAL_FUNCS_EXT void __SetAlarm( int ax );
INTERNAL_FUNCS_EXT void __ClearStatus(int ax);
INTERNAL_FUNCS_EXT int __IsPauseOn(int ax);
INTERNAL_FUNCS_EXT int __IsPauseCondition(int ax);
INTERNAL_FUNCS_EXT int __GetDacOffset(int ax);
INTERNAL_FUNCS_EXT void __SetFramesClear(int ax);
INTERNAL_FUNCS_EXT void __PutDacOffset(int ax, int nd);
INTERNAL_FUNCS_EXT void __SetSWUpperLimit(int ax, int li, char cst);
INTERNAL_FUNCS_EXT void __GetSWUpperLimit(int ax, int *pli, char *pcst);
INTERNAL_FUNCS_EXT void __PutSWLowerLimit(int ax, int li, char cst);
INTERNAL_FUNCS_EXT void __SetVelCurve(int ax, int li, char cst);
INTERNAL_FUNCS_EXT void __GetVelCurve(int ax, int *pli, char *pcst) ;
INTERNAL_FUNCS_EXT void __SetActvelMargin(int ax, int li, char cst, int time);
INTERNAL_FUNCS_EXT void __GetActvelMargin(int ax, int *pli, char *pcst, int *ptime) ;
INTERNAL_FUNCS_EXT void __GetIoOutData(int *pl1, int *pl2);
INTERNAL_FUNCS_EXT void __GetIoInData(int *pl1, int *pl2);
INTERNAL_FUNCS_EXT int __GetAxisStat(int ax);
INTERNAL_FUNCS_EXT int __GetAxisSource(int ax);
INTERNAL_FUNCS_EXT void __SetAmpOnLevel(int ax, int amp);
INTERNAL_FUNCS_EXT int __GetAmpOnLevel(int ax);
INTERNAL_FUNCS_EXT float __GetTrackingFactor(int ax);
INTERNAL_FUNCS_EXT void __SetTrackingFactor(int ax, float tfactor);
#if (defined(__AMC_SMD) || defined(__AMC_V70))
INTERNAL_FUNCS_EXT void __GetIoOutData_256(int *pl1, int *pl2, int *pl3, int *pl4, int *pl5, int *pl6, int *pl7, int *pl8);
INTERNAL_FUNCS_EXT void __GetIoCount(int *pl1, int *pl2);
INTERNAL_FUNCS_EXT void __GetIoInData_256(int *pl1, int *pl2, int *pl3, int *pl4, int *pl5, int *pl6, int *pl7, int *pl8);
#endif
INTERNAL_FUNCS_EXT void __PutSyncMap(int syn1, int syn2);
INTERNAL_FUNCS_EXT void __GetSWLowerLimit(int ax, int *pli, char *pcst);
INTERNAL_FUNCS_EXT void __SetEncoderOffset(int ax, int ofs);
INTERNAL_FUNCS_EXT int __GetEncoderOffset(int ax);
INTERNAL_FUNCS_EXT void SetEncoderOffset();
INTERNAL_FUNCS_EXT void GetEncoderOffset();
INTERNAL_FUNCS_EXT void RsvrOnOff(int ax, int bOn);
#if defined(__AMC_V70)
INTERNAL_FUNCS_EXT void __Get_Monitering_Power();
INTERNAL_FUNCS_EXT void Check_Monitering_System();
INTERNAL_FUNCS_EXT void __system_monitering_enable(char moni_nu, char en_d);
INTERNAL_FUNCS_EXT void __g_system_monitering_value(char  moni_nu, int *value, int *raw_val, int *compare_val);
INTERNAL_FUNCS_EXT void __s_monitering_threshold_percent(char  moni_nu, char pcnt);
INTERNAL_FUNCS_EXT void __g_monitering_threshold_percent(char  moni_nu, char *pcnt);
INTERNAL_FUNCS_EXT void __s_system_monitering_peak_hold_value(char axno, int value);
INTERNAL_FUNCS_EXT void __g_system_monitering_peak_hold_value(char moni_nu, int *pValue, int *pFpga_data, BOOL bMax); // min:FALSE, max:TRUE
INTERNAL_FUNCS_EXT void __s_system_decelstop_ratio(char axno, double fDecelRatio);
INTERNAL_FUNCS_EXT void __g_system_decelstop_ratio(char  moni_nu, double *pfDecelRatio);
#endif

#endif

