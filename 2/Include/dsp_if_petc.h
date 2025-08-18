#ifndef	__DSP_IF_PETC_H
#define	__DSP_IF_PETC_H

#ifdef DSP_IF_PETC_GLOBALS
#define DSP_IF_PETC_EXT
#else
#define DSP_IF_PETC_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define	INT			int
#define	pFLOAT		float *
#define	pDOUBLE		double *
#define	pint		int *
#define	pUint		unsigned int *

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
DSP_IF_PETC_EXT void ProcessBootPara(int ax, int val, int comm);
DSP_IF_PETC_EXT int _dsp_set_amp_enable_level(int axno, int level);
DSP_IF_PETC_EXT int _dsp_get_amp_enable_level(int axno, pint level);
DSP_IF_PETC_EXT int _dsp_set_amp_enable(int axno, int state);
#if defined(__AMC_V70)
DSP_IF_PETC_EXT int _dsp_system_moni_enable(char axno, char state);
DSP_IF_PETC_EXT int _dsp_get_system_monitering_value(char axno, int* val, int* rval, int* cval);
DSP_IF_PETC_EXT int _dsp_set_monitering_Threshold_percent(char axno, char pcnt);
DSP_IF_PETC_EXT int _dsp_get_monitering_Threshold_percent(char axno, char* pcnt);
DSP_IF_PETC_EXT int _dsp_system_status(char* status);
DSP_IF_PETC_EXT int _dsp_get_system_moni_enable(char* state);
#endif
DSP_IF_PETC_EXT int _dsp_get_amp_enable(int axno, int* state);
DSP_IF_PETC_EXT int _dsp_amp_fault_reset(int axno);
DSP_IF_PETC_EXT int _dsp_amp_fault_set(int axno);
DSP_IF_PETC_EXT int _dsp_set_sync_map_axes(int Master, int Slave);
DSP_IF_PETC_EXT int _dsp_set_sync_control(int condition);
DSP_IF_PETC_EXT int _dsp_get_sync_control(int* condition);
DSP_IF_PETC_EXT int _dsp_set_sync_gain(float syncgain);
DSP_IF_PETC_EXT int _dsp_get_sync_gain(float* syncgain);
DSP_IF_PETC_EXT int _dsp_version_chk(int bn, int* ver);
DSP_IF_PETC_EXT int _dsp_get_tracking_factor(int axno, double* pfFactor);
DSP_IF_PETC_EXT int _dsp_set_tracking_factor(int axno, double fFactor);
DSP_IF_PETC_EXT int _dsp_get_dac_output(int axis, int* voltage);
DSP_IF_PETC_EXT int _dsp_set_dac_output(int axis, int voltage);
DSP_IF_PETC_EXT int _dsp_set_analog_offset(int axis, int voltage);
DSP_IF_PETC_EXT int _dsp_get_analog_offset(int axis, int* voltage);
DSP_IF_PETC_EXT void _dsp_set_bit(INT bitNo);
DSP_IF_PETC_EXT void _dsp_reset_bit(INT bitNo);
DSP_IF_PETC_EXT void __SetWorkingLimit(int ax, int e_stop_rate);
DSP_IF_PETC_EXT int _dsp_amc_init(VOID);
DSP_IF_PETC_EXT int _dsp_get_error(int ax, double* error);
DSP_IF_PETC_EXT int _dsp_get_com_velocity(int ax);
DSP_IF_PETC_EXT int _dsp_get_act_velocity(int ax);
DSP_IF_PETC_EXT int _dsp_get_com_velocity_rpm(int ax);
DSP_IF_PETC_EXT int _dsp_get_act_velocity_rpm(int ax);

#endif

