#include <stdio.h>

#include "callback.h"
#include "amc_app.h"
#include "watchdog.h"


//#include "hardware.h"//2007.11.16(오)

#include "dsp_if_filter.h"//2008.3.5(오)
#include "amc_filter.h"


void Appl_Body_Main();
void Appl_Prev_Main();

int check_loop=0;
extern DBG_CNT_BUFFER sDbgCntBuff;
int loop_count=1615;

int check_200us=0;
extern int nSimCount;
int motion2_end=0;
int motion3_end=0;
int motion4_end=0;
int motion5_end=0;
int motion6_end=0;
int motion_go;
int err;
int body_on_motion=0;
int body_on_motion_count=0;
int motion_start=0;
extern int check_1ms;
int base = 55;	
int change_pos = 25868;
int change_pos1 = 26900;//26950;


int inout_check =0;
int main_count=0;
int motion_act= -1;
void Appl_Int1()//200us 인터럽트
{

}

int main_motion_s=0;
void Appl_Body_Main()
//void Appl_Int10()
{

}

//void Appl_Body_Main()
void Appl_Int10()//200us 인터럽트
{
	return;
}


void __App_First()
{
	__dsplib_set_callback(CALLBACK_PREV_MAIN_LOOP, Appl_Prev_Main);//main 루프에 바로 들어가기 전에 한번 불려지는 함수
	__dsplib_set_callback(CALLBACK_BODY_MAIN_LOOP, Appl_Body_Main);//main 루프의 while문 안에서 계속 불려지는 함수
	__dsplib_set_callback(CALLBACK_INT10, Appl_Int10);
	__dsplib_set_callback(CALLBACK_INT1, Appl_Int1);
//	__dsplib_set_callback(WDT_SUBCONTROL, Appl_Int1);
}


void Appl_Prev_Main()
{

}

