/**
@file	 VirtualDrivingControl.h
@version OHT 7.0
@brief	 ExecuteThread�� ������ Module�� �߻� �Լ�
*/

#ifndef VirtualDrivingControlH
#define VirtualDrivingControlH

#include <stdio.h>
#include "VirtualModuleControl.h"
#include "Def_PathInfo.h"			//MARK_TYPE
#include "Def_DrivingCtrlInfo.h"	//CTRL_INFO_OF_ID_READER_CMD_TYPE

typedef	enum	MANUAL_MARK_CMD_TYPE_
{
	MANUAL_MARK_CMD_DEFAULT				= 0,
	MANUAL_MARK_CMD_NODE				= 1,
	MANUAL_MARK_CMD_STATION_STOP		= 2,
	MANUAL_MARK_CMD_STATION_QR_RIGHT	= 3,
	MANUAL_MARK_CMD_STATION_QR_LEFT		= 4,
	MANUAL_MARK_CMD_STATION_STOP_BACK	= 5
}MANUAL_MARK_CMD_TYPE;


typedef	enum	MANUAL_DRIVING_CMD_TYPE_
{	
	MANUAL_DRIVING_CMD_STOP				= 0,
	MANUAL_DRIVING_CMD_FORWARD			= 1,
	MANUAL_DRIVING_CMD_BACKWARD			= 2,
	MANUAL_DRIVING_CMD_DIST				= 3,
	MANUAL_DRIVING_CMD_FRONT_AMP_ON		= 4,
	MANUAL_DRIVING_CMD_FRONT_AMP_OFF	= 5,
	MANUAL_DRIVING_CMD_REAR_AMP_ON 		= 6,
	MANUAL_DRIVING_CMD_REAR_AMP_OFF 	= 7,
	MANUAL_DRIVING_CMD_BRAKE_RELEASE 	= 8,
	MANUAL_DRIVING_CMD_BRAKE_NO_REALSE	= 9	
}MANUAL_DRIVING_CMD_TYPE;

typedef	struct	MANUAL_DRIVING_CMD_INFO_
{
	MANUAL_DRIVING_CMD_TYPE	CmdType;
	double	dAccel;	///< ���ӵ�: mm/msec2
	double	dDecel;	///< ���ӵ�: mm/msec2
	double	dSpeed;	///< �ӵ�: mm/msec
	double	dDist;	///< �Ÿ�: mm
	bool	bNew;	///<���ο� ��� ���� Ȯ��
}MANUAL_DRIVING_CMD_INFO;


//1 ---- New Way ----------------
/**
@brief	 ���� �Ŵ��� ��� �Ҵ� Ÿ��
@author  ���¿�
@date	 2013.06.28
*/
typedef	enum	DRIVING_MANUAL_CMD_TYPE_
{
	DRIVING_MANUAL_CMD_NONE						= 0,

	//2	Axis
	DRIVING_MANUAL_CMD_STATIC					,	///< Use "DRIVING_MANUAL_CMD_STATIC_VALUE"
	DRIVING_MANUAL_CMD_JOG_MOVE					,	///< Use "DRIVING_MANUAL_CMD_MOVE_VALUE"
	DRIVING_MANUAL_CMD_MARK						,	///< Use "DRIVING_MANUAL_CMD_MARK_VALUE"

	//2	Steer
	DRIVING_MANUAL_CMD_STEER				 	,	///< Use "DRIVING_MANUAL_CMD_STEER_VALUE"

	//2	Front Observation
	DRIVING_MANUAL_CMD_OBS_AREA_SET				,	///< Use "DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE"

	//2	ID Reader
	DRIVING_MANUAL_CMD_ID_READER_1				, 	///< Node
	DRIVING_MANUAL_CMD_ID_READER_2				, 	///< Tag_Station
	DRIVING_MANUAL_CMD_ID_READER_3				, 	///< QR_Left
	DRIVING_MANUAL_CMD_ID_READER_4					///< QR_Right
	
}DRIVING_MANUAL_CMD_TYPE;

/**
@brief	 ���� �Ŵ��� ��� Axis Value 1.
@author  ���¿�
@date	 2013.06.28
*/
typedef enum DRIVING_MANUAL_CMD_STATIC_VALUE_
{
	DRIVING_MANUAL_CMD_BRAKE_RELEASE			= 0,
	DRIVING_MANUAL_CMD_BRAKE_NO_REALSE			,

	DRIVING_MANUAL_CMD_AXIS1_RESET				,
	DRIVING_MANUAL_CMD_AXIS1_HOME				,
	DRIVING_MANUAL_CMD_AXIS1_AMP_ON 			,
	DRIVING_MANUAL_CMD_AXIS1_AMP_OFF			,

	DRIVING_MANUAL_CMD_AXIS2_RESET				,
	DRIVING_MANUAL_CMD_AXIS2_HOME				,
	DRIVING_MANUAL_CMD_AXIS2_AMP_ON 			,
	DRIVING_MANUAL_CMD_AXIS2_AMP_OFF
}DRIVING_MANUAL_CMD_STATIC_VALUE;

/**
@brief	 ���� �Ŵ��� ��� Axis Value 2.
@author  ���¿�
@date	 2013.06.28
*/
typedef enum DRIVING_MANUAL_CMD_JOG_MOVE_VALUE_
{
	DRIVING_MANUAL_CMD_JOG_MOVE_DIST 			= 0,
	DRIVING_MANUAL_CMD_JOG_MOVE_FORWARD			,
	DRIVING_MANUAL_CMD_JOG_MOVE_BACKWARD		,
	DRIVING_MANUAL_CMD_JOG_MOVE_STOP
}DRIVING_MANUAL_CMD_JOG_MOVE_VALUE;

/**
@brief	 ���� �Ŵ��� ��� Axis Value 3.
@author  ���¿�
@date	 2013.06.28
*/
typedef MARK_TYPE DRIVING_MANUAL_CMD_MARK_VALUE; 

/**
@brief	 ���� �Ŵ��� ��� Steer
@author  ���¿�
@date	 2013.06.28
*/
typedef enum DRIVING_MANUAL_CMD_STEER_VALUE_
{
	DRIVING_MANUAL_CMD_STEER_RESET				= 0,	///< ���� ��ġ�� ������ ���߰� ���� ��ũ�� 
	DRIVING_MANUAL_CMD_STEER_LEFT				,		///< Moving Torque�� �������� �̵� ��Ų �� ���� �� ������ũ�� ����
	DRIVING_MANUAL_CMD_STEER_LEFT_TRACTION		,		///< Traction Torque�� �������� �̵� ��Ų �� ���� �� ������ũ�� ����
	DRIVING_MANUAL_CMD_STEER_RIGHT				,		///< Moving Torque�� ���������� �̵� ��Ų �� ���� �� ������ũ�� ����
	DRIVING_MANUAL_CMD_STEER_RIGHT_TRACTION 			///< Traction Torque�� ���������� �̵� ��Ų �� ���� �� ������ũ�� ����
}DRIVING_MANUAL_CMD_STEER_VALUE;

/**
@brief	 ���� �Ŵ��� ��� Front Observation
@author  ���¿�
@date	 2013.06.28
*/
typedef enum DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE_
{
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_0		= 0,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_1		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_2		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_3		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_4		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_5		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_6		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_7		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_8		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_9		,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_10	,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_11	,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_12	,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_13	,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_14	,
	DRIVING_MANUAL_CMD_FRONT_OBSERVATION_AREA_15	
}DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE;

/**
@brief	 ���� �Ŵ��� ��� ID Reader
@author  ���¿�
@date	 2013.06.28
*/
typedef	CTRL_INFO_OF_ID_READER_CMD_TYPE DRIVING_MANUAL_CMD_FRONT_ID_READER_VALUE;

#if 0
typedef enum DRIVING_MANUAL_CMD_ID_READER_VALUE_
{
	DRIVING_MANUAL_CMD_ID_READER_TRIGER_ON			= 0,
	DRIVING_MANUAL_CMD_ID_READER_TRIGER_OFF			,
	DRIVING_MANUAL_CMD_ID_READER_READ_DATA			,
	DRIVING_MANUAL_CMD_ID_READER_READ_SEQUENTIALLY			
}DRIVING_MANUAL_CMD_FRONT_ID_READER_VALUE;
#endif

/**
@brief	 ���� �Ŵ��� ��� ���� ����ü
@author  ���¿�
@date	 2013.06.28
*/
typedef	struct	DRIVING_MANUAL_CMD_INFO_
{
	DRIVING_MANUAL_CMD_TYPE	CmdType;
	int		nValue;	///< ��
	
	double	dAccel;	///< ���ӵ�: mm/msec2
	double	dDecel;	///< ���ӵ�: mm/msec2
	double	dSpeed;	///< �ӵ�: mm/msec
	double	dDist;	///< �Ÿ�: mm
}DRIVING_MANUAL_CMD_INFO;

/**
@brief	 �Ŵ��󿡼� ����� ����� class
@author  ���¿�
@date	 2013.06.28
*/
class VirtualDrivingControl:public VirtualModuleControl
{
	public:
		VirtualDrivingControl(void):VirtualModuleControl()	/// ������, ���� �ʱ�ȭ
		{
			
		}
		virtual ~VirtualDrivingControl(void){;} /// ���� �Ҹ��� 

	public:
		virtual int		Manual_Mark		(MANUAL_MARK_CMD_TYPE Type)  	= 0;
		virtual int		Manual_Driving	(MANUAL_DRIVING_CMD_INFO Info)  = 0;
		virtual int		AutoTaeching_Driving	(MANUAL_DRIVING_CMD_INFO Info)  = 0;
//		virtual void 	Manual_DrivingAbnormalCheck()  = 0;
//		virtual int	Manual_STEERING(MANUAL_MARK_CMD_TYPE pType)  = 0;
		virtual int 	Manual_IdReader(DRIVING_MANUAL_CMD_MARK_VALUE MarkType
												, DRIVING_MANUAL_CMD_FRONT_ID_READER_VALUE CmdType
												, int* pnResultID, double* pdResultOffset)	= 0;	///< ID Reader ���� ���
		virtual int	Manual_FrontObservation(DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE Level)	= 0;	///< ���� ���� ���� ���


		
		//1	New Way
		virtual int ExecuteManual(DRIVING_MANUAL_CMD_INFO Info) 	= 0;	///< Manual ����� �����ϴ� Portal �Լ�

		virtual int GetMapData(List<EXECUTE_RESPONSE_MAPMAKING_INFO*>* pListOfMapData,
								volatile MAP_INFO_ON_ADDING_UNION *pMapInfoOnAdding) = 0; 	///< MapMaking ������ �޾ƿ��� �Լ�
		virtual EXECUTE_POSITION_INFO	GetPositionInfo() = 0;
};
//---------------------------------------------------------------------------


#endif


