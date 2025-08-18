#ifndef		__AMC_APP_H
#define		__AMC_APP_H

#include "amc_type.h"
#include "callback.h"
#include "eeprom.h"
#include "crc32.h"
#include "i2c.h"
#include "i2creg.h"
#include "watchdog.h"

#include "amc_def.h"
#include "amc_kernel.h"
#include "amc_filter.h"

/*-----------------------------------------------------------
*	Event defines
*---------------------------------------------------------*/
#define		NO_EVENT			0 	/* ignore a condition */
#define		STOP_EVENT			1	/* generate a stop event */
#define 	E_STOP_EVENT		2 	/* generate an e_stop event */
#define		ABORT_EVENT			3 	/* disable PID control, and disable the amplifier */
#define		ENC_OVF_EVENT		4	/* set if encoder overflow is present. actions is same as e-stop. ckyu, 2007.10.12 */
#define		PAUSE_EVENT			5	// stop this command. if reason is removed then restart the command pushed on queue. event check bit is actived.

#define		PF_VMOVE			8
#define		PF_VMOVE_POS		9

int _dsp_set_control(int ax, int ctrl);
int _dsp_set_closed_loop(int ax, int cfg);
int _dsp_set_unipolar(int ax, int device);
int _dsp_set_feedback(int ax, int cfg);
int _dsp_set_servo(int ax);
int _dsp_set_amp_enable(int ax, int state);


// stop 관련
int _dsp_set_stop(int ax);
int _dsp_set_stop_rate(int ax, int rate);
int _dsp_fset_stop_rate(int ax, int rate);
int _dsp_get_stop_rate(int ax, int * rate);
int _dsp_fget_stop_rate(int ax, int * rate);

// e-stop관련
int _dsp_set_e_stop(int ax);
int _dsp_set_e_stop_rate(int ax, int rate);
int _dsp_fset_e_stop_rate(int ax, int rate);
int _dsp_get_e_stop_rate(int ax, int * rate);
int _dsp_fget_e_stop_rate(int ax, int * rate);



int _dsp_set_home_sn_event(int ax, int event);
int _dsp_get_home_sn_event(int ax, int *pevent);
int _dsp_set_positive_level(int ax, int level);	
int _dsp_set_positive_limit(int ax, int event);			//EStop Event 지정






int _dsp_start_move( int ax, double pos, double vel, int acc);
int _dsp_set_position(int ax, double dval);
int _dsp_get_counter(int ax, double * pd);
int _dsp_get_sync_position(double * pos_m, double * pos_s);

int _dsp_set_command(int ax, double pos);
int _dsp_get_command(int ax, double * pos);
int _dsp_get_error(int ax, double * error);
int _dsp_get_com_velocity(int ax);
int _dsp_get_act_velocity(int ax);
int _dsp_get_com_velocity_rpm(int ax);
int _dsp_get_act_velocity_rpm(int ax);






void _dsp_set_encoder_direction(int ax, int ccwcw);
int _dsp_get_encoder_direction(int ax);
#define	_dsp_fset_encoder_direction(a,c)	_dsp_set_encoder_direction(a,c)
#define	_dsp_fget_encoder_direction(a)		_dsp_get_encoder_direction(a)
























int _dsp_io_trigger(int ax, int bitNo, int state);
//int _dsp_get_analog (int channel, int * value);
//int _dsp_set_dac_offset(int axis, int voltage);
//int _dsp_get_dac_offset(int axis, int * voltage);

int	_dsp_Find_Analog_Channel(int ChannelNo);
int _dsp_get_dac_output(int axis, int * voltage);	// dac_code
int _dsp_set_dac_output(int axis, int voltage);		// dac_code
int _dsp_set_analog_offset(int axis, int hexacode);	// dac bias
int _dsp_get_analog_offset(int axis, int * hexacode);// dac bias

int _dsp_set_amp_enable_level(int axno,int level);
int	_dsp_fset_amp_enable_level(int axno,int level);
int _dsp_get_amp_enable_level(int axno,int * level);
int _dsp_get_amp_enable(int axno,int * state);
int _dsp_amp_fault_reset(int axno);
int _dsp_fget_control(int axno,int * control);
int _dsp_fset_control(int axno,int control);
int _dsp_set_pulse_ratio(int axno,int pgratio);
int _dsp_get_pulse_ratio(int axno,int * pgratio);
int _dsp_get_control(int axno,int * control);
int _dsp_set_electric_gear(int axno, double ratio);
int _dsp_fset_electric_gear(int axno, double ratio);
int _dsp_get_electric_gear(int axno, double * ratio);

int _dsp_set_step_mode(int axno, int mode);
int _dsp_fset_step_mode(int axis, int mode);
int _dsp_get_step_mode(int axno, int * mode);
int _dsp_fget_step_mode(int axis,int *mode);
int _dsp_set_sync_map_axes(int Master, int Slave);
int _dsp_set_sync_control(int condition);
int _dsp_get_sync_control(int * condition);
int _dsp_set_sync_gain(int syncgain);
int _dsp_get_sync_gain(int * syncgain);

//int _dsp_fset_sync_gain(int syncgain);
//int _dsp_fget_sync_gain(int * syncgain);
int _dsp_compensation_pos(int len, int * axes, double * c_pos, int * c_acc);
int _dsp_version_chk(int bn, int * ver);
//int _dsp_get_offset(int axno, int * ofs);
//int _dsp_fget_analog_offset(int axis, int * ofs);
//int _dsp_set_offset(int axno, int ofs);
//int _dsp_fset_analog_offset(int axno, int ofs);
int _dsp_get_tracking_factor(int axno, double * pfFactor);
int _dsp_fget_tracking_factor(int axno, double * pfFactor);
int _dsp_set_tracking_factor(int axno, double fFactor);
int _dsp_fset_tracking_factor(int axis, double fFactor);
int _dsp_home_switch(int axis);
int _dsp_pos_switch(int	axis);
int _dsp_neg_switch(int	axis);
int _dsp_amp_fault_switch(int axis);
//int _dsp_set_io_mode(int port, int value);
//int _dsp_get_io_mode(int port, int * value);

void _dsp_get_io64(int *pdat);		// read 64 input bits
int _dsp_set_io64(int port, int *pval);	// write 64 output bits
int _dsp_get_out64(int * value);	// 출력되고있는 64bit 값을 리턴한다.

#if (defined(__AMC_SMD) || defined(__AMC_V70))
void _dsp_get_io_input(int *pdat);
int _dsp_set_io_output(int port, int *pval);	//120831 2.9.8 SYK	OUTPUT 값 쓰기 함수
int _dsp_get_io_output(pINT value);				//120831 2.9.8 SYK  OUTPUT 값 읽기 함수�
int _dsp_set_io_count(int in_c, int out_c);		//120831 2.9.8 SYK  INPUT/OUTPUT PIN 설정 
int _dsp_get_io_count(pINT in_c, pINT out_c);	//120831 2.9.8 SYK  INPUT/OUTPUT PIN 설정값 읽어오기

int _dsp_reload_servopack_all();				//121204 2.11.2 SYK servo encoder reload 
#endif

//int _dsp_get_bit(int bitno);
//int _dsp_get_outbit(int bitno);

int _dsp_get_bit(int bitno, pINT chk_err);		// 통합 버젼 120228 syk 리턴값과 error 코드 분리 
int _dsp_get_outbit(int bitno, pINT chk_err);	// 통합 버젼 120228 syk 리턴값과 error 코드 분리

void _dsp_set_bit(int bitNo);
void _dsp_reset_bit(int bitNo);
int _dsp_SetReset_Bit_IO(int bitNo, int flag);

//int _dsp_Find_IO_Bit(int bitNo);
//int _dsp_Find_IO_Port (int portNo);
//int _dsp_fio_interrupt_pcirq_eoi(int bn);
//int _dsp_io_interrupt_pcirq(int bn, int state);
//int _dsp_io_interrupt_enable(int bn, int state);
int _dsp_dwell(int ax, int duration);
int _dsp_io_trigger(int ax, int bitNo, int state);
int _dsp_io_interrupt_on_stop(int ax, int state);
//int _dsp_set_vel_limit(int ax, double limit)
//int	_dsp_Find_Bd_Jnt (int ax, int *bn, int *jnt);

int _dsp_set_positive_sw_limit(int ax, double limit, int action);
int _dsp_fset_positive_sw_limit(int ax, double limit, int action);
int _dsp_get_positive_sw_limit(int ax, double * limit, int * action);
int _dsp_fget_positive_sw_limit(int axis, double * limit, int *action);

int _dsp_set_negative_sw_limit(int ax, double limit, int action);
int _dsp_fset_negative_sw_limit(int ax, double limit, int action);
int	_dsp_get_negative_sw_limit (int ax, double * limit, int * action);
int _dsp_fget_negative_sw_limit(int axis, double * limit, int *action);

int _dsp_get_accel_limit(int ax, int * limit);
int _dsp_set_accel_limit(int ax, int limit);
int _dsp_get_vel_limit(int ax, double * limit);
int _dsp_set_vel_limit(int ax, double limit);

int _dsp_set_positive_limit(int ax, int act);
int _dsp_fset_positive_limit(int ax, int act);
int _dsp_get_positive_limit(int ax, int * act);
int _dsp_fget_positive_limit(int ax, int *act);

int _dsp_set_negative_limit(int ax, int act);
int _dsp_fset_negative_limit(int ax, int act);
int _dsp_get_negative_limit(int ax, int * act);
int _dsp_fget_negative_limit(int ax, int *act);

int _dsp_set_error_limit(int ax, double limit, int action);
int _dsp_fset_error_limit(int ax, double limit, int action);
int _dsp_get_error_limit(int ax, double * limit, int * action);
int _dsp_fget_error_limit(int ax, double * limit, int *action);

int _dsp_set_positive_level(int ax, int level);
int _dsp_get_positive_level(int ax, int * level);
int _dsp_set_negative_level(int ax, int level);
int _dsp_get_negative_level(int ax, int * level);

int _dsp_set_home_level(int ax, int level);
int _dsp_get_home_level(int ax, int * level);
int _dsp_set_home(int ax, int action);
int _dsp_get_home(int ax, int * action);

int _dsp_fset_amp_fault_level(int ax, int level);
int _dsp_set_amp_fault_level(int ax, int level);
int _dsp_get_amp_fault_level(int ax, int * level);
int _dsp_fset_amp_fault(int ax, int action);

int _dsp_set_amp_fault(int ax, int action);
int _dsp_get_amp_fault(int ax, int * action);

int _dsp_get_unipolar(int ax, int * device);
int _dsp_get_feedback(int ax, int * cfg);
int _dsp_get_closed_loop(int ax, int * cfg);
//int _dsp_set_motor_type(int ax, int mtype);

int _dsp_get_stepper(int ax);
int _dsp_fset_amp_reset_level(int ax, int level);
int _dsp_set_amp_reset_level(int ax, int level);
int _dsp_get_amp_reset_level(int ax, int * level);

//int _dsp_get_buf_position(int ax);
int _dsp_set_index_required(int ax, int index);
int _dsp_get_index_required(int ax, int * index);
int _dsp_move( int ax, double pos,double vel, int acc);
int _dsp_start_r_move( int ax, double pos, double vel, int acc);
int _dsp_r_move( int ax, double pos,double vel, int acc);
int _dsp_start_s_move( int ax, double pos, double vel, int acc);
int _dsp_s_move( int ax, double pos,double vel, int acc);
int _dsp_start_rs_move( int ax, double pos, double vel, int acc);
int _dsp_rs_move( int ax, double pos, double vel, int acc);

int	_dsp_start_p_move( int ax, double pos, double vel, int acc);
int	_dsp_p_move( int ax, double pos, double vel, int acc);
int _dsp_start_t_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_t_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_start_ts_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_ts_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_start_tr_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_tr_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_start_trs_move( int ax, double pos, double vel, int acc, int dcc);
int _dsp_trs_move( int ax, double pos, double vel, int acc, int dcc);

int _dsp_start_move_all( int len, int * ax, double * pos, double * vel, int * acc);
int _dsp_move_all( int len, int * ax, double * pos, double * vel, int * acc);
int _dsp_start_s_move_all( int len, int * ax, double * pos, double * vel, int * acc);
int _dsp_s_move_all( int len, int * ax, double * pos, double * vel, int * acc);
int _dsp_start_t_move_all( int len, int * ax, double * pos, double * vel, int * acc,int * dcc);
int _dsp_t_move_all( int len, int * ax, double * pos, double * vel, int * acc, int * dcc);
int _dsp_start_ts_move_all( int len, int * ax, double * pos, double * vel, int * acc,int * dcc);
int _dsp_ts_move_all( int len, int * ax, double * pos, double * vel, int * acc, int * dcc);
int _dsp_wait_for_done( int ax);
int _dsp_wait_for_all( int len, int * ax);
int _dsp_map_axes( int n_axes, int * map_array);
int _dsp_set_move_speed(double  speed);
int _dsp_set_move_accel(int  accel);
int _dsp_set_arc_division(double  degrees);
int _dsp_all_done();

int _dsp_move_2(double x, double y);
int _dsp_move_3(double x, double y, double z);
int _dsp_move_4(double x, double y, double z, double w);
int _dsp_smove_2(double x, double y);
int _dsp_smove_3(double x, double y, double z);
int _dsp_smove_4(double x,double y, double z, double w);
int _dsp_smove_n(double *  x);
int _dsp_arc_2(double x_center, double y_center, double angle);
int _dsp_spl_line_move2(double * pnt);
int _dsp_spl_line_move3(double * pnt);
int _dsp_spl_line_move(int len, double * pnt1);
int _dsp_spl_arc_move2(double x_center, double y_center, double * pnt, double vel, int acc, int cdir);
int _dsp_spl_arc_move3(double x_center, double y_center, double * pnt, double vel, int acc, int cdir);
int _dsp_spl_arc_move(int len, double x_center, double y_center, double * pnt, double vel, int acc, int cdir);

int _dsp_in_sequence(int ax);
int _dsp_in_motion(int ax);
int _dsp_in_position(int ax);
int _dsp_motion_done(int ax);
int _dsp_axis_done(int ax);
int _dsp_axis_state(int ax);
int _dsp_axis_source(int ax);

#if defined(__AMC_V70)
int _dsp_system_status(char *status);
int _dsp_get_system_moni_enable(char *state);
int _dsp_system_moni_enable(char axno,char state);
int _dsp_get_system_monitering_value(char axno, int *val, int *rval, int *cval);
int _dsp_set_monitering_Threshold_percent(char axno, char pcnt); 
int _dsp_get_monitering_Threshold_percent(char axno, char *pcnt); 
#endif

int _dsp_clear_status(int ax);
int _dsp_frames_clear(int ax);
int frames_left(int ax);
#define	_dsp_frames_left(a)		frames_left(a)

//void _dsp_set_sw_pause_event_st(int ax, int bSet);	// pause임을 설정한다.
//int _dsp_set_sw_pause_checkbit(int ax, int nBit, int bLvl);	// 0~63, 검사할비트와 active레벨을 설정한다.
void _dsp_set_sw_pause(int ax, int bOn);			// pause 검사여부를 설정한다.
//int _dsp_fset_sw_pause_checkbit(int ax, int nBit, int bLvl);	// 0~63
//void _dsp_fset_sw_pause(int ax, int bOn);
//void _dsp_get_sw_pause(int ax, int *pbOn);
//void _dsp_get_sw_pause_event(int ax, int *pbOn);	// 0:no pause, 1:pause
//void _dsp_get_sw_pause_checkbit(int ax, int *pnBit, int *pbLvl);
//#define		_dsp_fget_sw_pause_checkbit(a,b,c)	_dsp_get_sw_pause_checkbit((a),(b),(c))
//#define		_dsp_fget_sw_pause(a,b)				_dsp_get_sw_pause((a),(b))
void _dsp_clear_amc_error();

int	_dsp_amc_init();
int _dsp_amp_fault_set(int ax);
int _dsp_amc_init ();
int _dsp_get_position(int ax, double * ppos);

void _dsp_set_encoder_offset(int ax, int ofs);
int _dsp_get_encoder_offset(int ax);

void	_dsp_param_flush();

int _dsp_set_stepper(int);
int _dsp_fset_stepper(int);
int _dsp_get_micro_stepper(int ax);
int _dsp_set_micro_stepper(int ax);
int _dsp_fset_micro_stepper(int ax);

int _dsp_get_servo(int);
int _dsp_fset_servo(int);
int _dsp_fset_feedback(int, int);
int _dsp_fset_closed_loop(int, int);
int _dsp_fset_unipolar(int, int);

int _dsp_set_p_integration(int ax, int mode);
int _dsp_get_p_integration(int, int *);
int _dsp_set_v_integration(int, int);
int _dsp_get_v_integration(int, int *);



int _dsp_move_pt(int ax, double a, double vel);
int _dsp_move_nt(int ax, double a, double vel);
int _dsp_move_st(int ax, double a);

int _dsp_move_dst(
	int ax, 
	double a1, double a2, // 가속기울기(unit:a), 감속기울기(unit:a)
	double v, 			// 최대속도(pulse/sec)
	int m);				// 멈출위치
int _dsp_move_p(int ax, double a, double vel);
int _dsp_move_n(int ax, double a, double vel);
int _dsp_move_s(int ax, double a);

int _dsp_move_ds(
	int ax, 
	double a1, double a2, // 가속기울기(unit:a), 감속기울기(unit:a)
	double v, 			// 최대속도(pulse/sec)
	int m);				// 멈출위치	
	
	
#if 0
void _dsp_move_dc(
	int ax, 
	double a1, double a2, 
	double v1, double v2, 
	int m);
	
void _dsp_move_dds(
	int ax, 
	double a1, double a2, double a3, 
	double v1, double v2, 
	int m1, int m2);
	
void _dsp_move_ddds(
	int ax, 
	double a1, double a2, double a3, double a4,
	double v1, double v2, double v3,
	int m1, int m2, int m3);
#endif

void _dsp_clear_amc_error();















int	_dsp_Find_MMC_Num(int axes);

#endif


