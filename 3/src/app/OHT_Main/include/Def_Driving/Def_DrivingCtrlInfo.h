#ifndef Def_DrivingCtrlInfoH
#define Def_DrivingCtrlInfoH


#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64
#include "Def_AxisCtrlInfo.h"	// AXIS_CTRL_CMD_TYPE, STATIC_ASYNC_AXIS_CTRL_INFO, DYNAMIC_AXIS_CTRL_INFO, CTRL_INFO_OF_ASYNC_AXIS_MOVE, CTRL_INFO_OF_SYNC_AXIS_MOVE
//#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_Driving.h"	//USER_DETECT_INFO


/**
@brief	 ���� ���
@author  ���¿�
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_STEER_
{
	BYTE	FrontCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	BYTE	FrontDirection	: 1;	///< 0:���� / 1:������
	BYTE	FrontTorque		: 2;	///< 0:Free / 1:Stop / 2:Moving / 3:����
	
	BYTE	RearCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	BYTE	RearDirection	: 1;	///< 0:���� / 1:������
	BYTE	RearTorque		: 2;	///< 0:Free / 1:Stop / 2:Moving / 3:����
	
}CTRL_INFO_OF_STEER;


/**
@brief	 ID Reader ���
@author  ���¿�
@date	 2013.06.19
*/
typedef enum CTRL_INFO_OF_ID_READER_CMD_TYPE_
{
	CTRL_INFO_OF_ID_READER_CMD_NONE    				= 0,
	CTRL_INFO_OF_ID_READER_CMD_ENABLE				,
	CTRL_INFO_OF_ID_READER_CMD_DISABLE				,
	CTRL_INFO_OF_ID_READER_CMD_RESTART				,
	CTRL_INFO_OF_ID_READER_CMD_TRIGGER_ON 			,
	CTRL_INFO_OF_ID_READER_CMD_READ_DATA			,
	CTRL_INFO_OF_ID_READER_CMD_TRIGGER_OFF			,
	CTRL_INFO_OF_ID_READER_CMD_READ_SEQUENTIALLY	,			///< 100ms �� delay ����
}CTRL_INFO_OF_ID_READER_CMD_TYPE;



/**
@brief	 ���� ���� ���
@author  ���¿�
@date	 2013.07.31
*/
typedef struct CTRL_INFO_OF_FRONT_DETECT_
{
	bool bCmdOn;				///< false: ��� ���� / true: ��� ����
	USER_DETECT_INFO Info;	///<���� ��
	
}CTRL_INFO_OF_FRONT_DETECT;



/**
@brief	 ����� ��� ����
@author  ���¿�
@date	 2013.06.19
*/
typedef struct DRIVING_CTRL_INFO_SET_
{
	CTRL_INFO_OF_STEER				Steer;		///< ���� ���
	CTRL_INFO_OF_SYNC_AXIS_MOVE		Axis;		///< ���� ���
	CTRL_INFO_OF_FRONT_DETECT		FrontObservation;	///< ���� ���� ���
	CTRL_INFO_OF_ID_READER_CMD_TYPE	IdReader[14];	///< ID Reader
}DRIVING_CTRL_INFO_SET;

#endif
