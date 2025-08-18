/**
@file    HWSetStatus.h
@version OHT 7.0
@brief   ���� ����ü ���� Header File
*/

#ifndef HWSetStatusH
#define HWSetStatusH

/**
@brief   HW Thread�� Status Thread�� ����� ���� ����ü
@author  zzang9un
@date    2012.10.18
*/
typedef struct HWSetStatus_
{
    int ErrorCode; ///< Error Code
    //OHTCMD Cmd_Remocon; ///< Remocon���� ���� Command
} HWSetStatus;

#endif  //HWSetStatusH
