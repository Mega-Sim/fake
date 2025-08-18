#ifndef Def_AxisCtrlInfoH
#define Def_AxisCtrlInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "MotionAxis.h"		// USER_MOVE_DS_ADDITIONAL_INFO

/**
@brief	 ���� ���
@author  ���¿�
@date	 2013.06.19
*/
typedef enum AXIS_CTRL_CMD_TYPE_
{
	AXIS_CTRL_CMD_NONE		= 0,	///< ��� ����
	AXIS_CTRL_CMD_STATIC	= 1, 	///< ���� ���
	AXIS_CTRL_CMD_MOVE_DS	= 2, 	///< moveDS
	AXIS_CTRL_CMD_MOVE_S	= 3, 	///< moveS
	AXIS_CTRL_CMD_MOVE_P	= 4, 	///< moveP
	AXIS_CTRL_CMD_MOVE_N	= 5 	///< moveN
}AXIS_CTRL_CMD_TYPE;

/**
@brief	 �񵿱� �� ���� ���� ��� �� (���� �غ�/�Ϸ� ��)
@author  ���¿�
@date	 2013.06.19
*/
typedef struct STATIC_ASYNC_AXIS_CTRL_INFO_
{
	WORD	NodeCountReset		: 1;	///< 0: ��� ���� / 1: ��� ����

	WORD	ResetCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	AmpCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	SetHomeCmdOn	: 1;	///< 0: ��� ���� / 1: ��� ����
	
	WORD	Reserved	: 11;
}STATIC_ASYNC_AXIS_CTRL_INFO;

/**
@brief	 ���� �� ���� ���� ��� �� (���� �غ�/�Ϸ� ��)
@author  ���¿�
@date	 2013.06.19
*/
typedef struct STATIC_SYNC_AXIS_CTRL_INFO_
{
	WORD	NodeCountReset		: 1;	///< 0: ��� ���� / 1: ��� ����

	WORD	Axis1ResetCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	Axis1AmpCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	Axis1AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	Axis1SetHomeCmdOn	: 1;	///< 0: ��� ���� / 1: ��� ����
	
	WORD	Axis2ResetCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	Axis2AmpCmdOn		: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	Axis2AmpOn			: 1;	///< 0: Amp Off / 1: Amp On
	WORD	Axis2SetHomeCmdOn	: 1;	///< 0: ��� ���� / 1: ��� ����

	WORD	AxisSetTorqueLimit			: 1;	///< 0: ��� ���� / 1: ��� ����
	WORD	AxisSetTorqueLimitRelease	: 1;	///< 0: ��� ���� / 1: ��� ����

	WORD	Reserved			: 5;
	
}STATIC_SYNC_AXIS_CTRL_INFO;

/**
@brief	 ���� ��� ��
@author  ���¿�
@date	 2013.06.19
*/
typedef struct DYNAMIC_AXIS_CTRL_INFO_
{
	double                      dAccel;              ///< ���ӵ�(����: mm/msec2)
	double                      dDecel;              ///< ���ӵ�(����: mm/msec2)
	double                      dSpeed;              ///< �ӵ�(����: mm/msec)
	double                      dDist;               ///< �Ÿ�(����: mm)
	USER_MOVE_DS_ADDITIONAL_INFO     AdditionalInfo;    ///< �߰� ����
}DYNAMIC_AXIS_CTRL_INFO;


/**
@brief	 ���� ���
@author  ���¿�
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_ASYNC_AXIS_MOVE_
{
	AXIS_CTRL_CMD_TYPE				Type;		///< 0: AXIS_CTRL_CMD_NONE (��� ����) 1: AXIS_CTRL_CMD_STATIC (���� ���) 2: AXIS_CTRL_CMD_MOVE_DS (moveDS) 3: AXIS_CTRL_CMD_MOVE_S (moveS) 4: AXIS_CTRL_CMD_MOVE_P (moveP) 5: AXIS_CTRL_CMD_MOVE_N (moveN)
	STATIC_ASYNC_AXIS_CTRL_INFO		Static;		///< ���� ���
	DYNAMIC_AXIS_CTRL_INFO			Dynamic;	///< ���� ���
}CTRL_INFO_OF_ASYNC_AXIS_MOVE;

/**
@brief	 ���� ���
@author  ���¿�
@date	 2013.06.19
*/
typedef struct CTRL_INFO_OF_SYNC_AXIS_MOVE_
{
	AXIS_CTRL_CMD_TYPE				byType;		///< 0: AXIS_CTRL_CMD_NONE (��� ����) 1: AXIS_CTRL_CMD_STATIC (���� ���) 2: AXIS_CTRL_CMD_MOVE_DS (moveDS) 3: AXIS_CTRL_CMD_MOVE_S (moveS) 4: AXIS_CTRL_CMD_MOVE_P (moveP) 5: AXIS_CTRL_CMD_MOVE_N (moveN)
	STATIC_SYNC_AXIS_CTRL_INFO		Static;		///< ���� ���
	DYNAMIC_AXIS_CTRL_INFO			Dynamic;	///< ���� ���
}CTRL_INFO_OF_SYNC_AXIS_MOVE;

#endif
