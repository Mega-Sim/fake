#ifndef __DSP_IF_PLIMIT_H
#define	__DSP_IF_PLIMIT_H

#ifdef DSP_IF_PLIMIT_GLOBALS
#define DSP_IF_PLIMIT_EXT
#else
#define DSP_IF_PLIMIT_EXT extern
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
DSP_IF_PLIMIT_EXT int _dsp_Find_Bd_Jnt(int ax, int *bn, int *jnt);
DSP_IF_PLIMIT_EXT int _dsp_set_positive_sw_limit(int ax, double limit, int action);
DSP_IF_PLIMIT_EXT int _dsp_get_positive_sw_limit(int ax, double* limit, int* action);
DSP_IF_PLIMIT_EXT int _dsp_set_negative_sw_limit(int ax, double limit, int action);
DSP_IF_PLIMIT_EXT int _dsp_get_negative_sw_limit (int ax, double* limit, int* action);
DSP_IF_PLIMIT_EXT int _dsp_get_accel_limit(int ax, int* limit);
DSP_IF_PLIMIT_EXT int _dsp_set_accel_limit(int ax, int limit);
DSP_IF_PLIMIT_EXT int _dsp_get_vel_limit(int ax, double* limit);
DSP_IF_PLIMIT_EXT int _dsp_set_vel_limit(int ax, double limit);
DSP_IF_PLIMIT_EXT int _dsp_set_error_limit(int ax, double limit, int action);
DSP_IF_PLIMIT_EXT int _dsp_get_error_limit(int ax, double* limit, int* action);
DSP_IF_PLIMIT_EXT int _dsp_get_out64(int* value);
#if (defined(__AMC_SMD) || defined(__AMC_V70))
DSP_IF_PLIMIT_EXT int _dsp_get_io_output(int* value);
DSP_IF_PLIMIT_EXT int _dsp_get_io_count(int* in_c, int* out_c);
#endif

#endif

