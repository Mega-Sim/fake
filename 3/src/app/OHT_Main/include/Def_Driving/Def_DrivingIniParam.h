#ifndef Def_DrivingIniParamH
#define Def_DrivingIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_MarkParam.h"	// MARK_DIST_PARAM, MARK_SPEED_PARAM, MARK_ACCEL_PARAM

/**
@brief	 ���� Mark �Ķ����
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_MARK_PARAM_
{
	MARK_TYPE			Type;	///< Mark Ÿ��: ���� �������� Type ( MARK_TYPE 0: MARK_TYPE_NODE (Node) / 1: MARK_TYPE_TAG_STATION (TagStation) / 2: MARK_TYPE_QR_STATION_LEFT (QR Left) / 3: MARK_TYPE_QR_STATION_RIGHT (QR Right) )
	MAKER_TYPE          MakerType;  ///< QR Sensor Maker Type
	MARK_DIST_PARAM		Dist;	///< �Ÿ�
	MARK_SPEED_PARAM	Speed;	///< �ӵ�
	MARK_ACCEL_PARAM	Accel;	///< ������
}DRIVING_MARK_PARAM;

/**
@brief	 ���� Steer �Ķ����
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_STEER_PARAM_
{
	DWORD	dwChangeTime;			///< ������ ����Ǵµ� �ʿ��� �ð�
	DWORD	dwMovingTorqueTime;		///< MovingTorque ���� �ð�
	DWORD	dwCompleteCheckTime;	///< ���� ������ �Ϸ� �� ä�͸� �ð�
	DWORD	dwCompleteLimitTime;	///< ���� ���� �ð� Limit
}DRIVING_STEER_PARAM;

/**
@brief	 ���� Link �Ķ����
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_LINK_PARAM_
{
	double	dOutNodeAreaMargin;				///< Link�� OutNode ���� ���� ����
	double	dNBranchMiddleAreaInMargin;		///< N�б� �߰��� ���� ���� ����		
}DRIVING_LINK_PARAM;

/**
@brief	 ���� ���氨�� �Ķ����
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_FRONT_DETECT_PARAM_
{
	DWORD	dwDetectFailChatteringTime;	///< Detect Fail Error Chattering �ð�
	DWORD	dwObsFailChatteringTime;	///< OBS Fail Error Chattering �ð�
	DWORD	dwObsTimeout;				///< OBS ���� �ð� �ʰ� ����
	DWORD	dwObsChangeTime;				///< OBS ���� �ð� �ʰ� ����
	double	dObsStraightDetectMargin;	///< OBS ���� ���� ���� �� ���� �� ���� ����
}DRIVING_FRONT_DETECT_PARAM;

/**
@brief	 UBG ��ȣ ���� (Fail 3) �� ���� �ߵ� ���� ���� �� Error ó���� ���� �Ķ����
@see	 AMOHTV80F-1756
*/
typedef struct DRIVING_UBG_FAIL3_ERROR_PARAM_
{
	bool bHourlyCheck;	///< �Žð� ���� ���� �� Error ó�� ����
	bool bWeeklyCheck;	///< �Žð����� ���� �� Error ó�� ����
	int nHourCount;		///< �Žð� �߻��� `UBG FAIL 3` Error ����
	int nWeekCount;		///< ���� �߻��� `UBG FAIL 3` Error ����
}DRIVING_UBG_FAIL3_ERROR_PARAM;

/**
@brief	 ���� Link �Ķ����
@author  ehoto
@date	 2013.06.23
*/
typedef struct DRIVING_FOUPCOVER_PARAM_
{
	UINT	nLineFoupChattering;		///< Link�� OutNode ���� ���� ����
	UINT	nCurveFoupChattering;		///< �б� �߰��� ���� ���� ����
	UINT	nSPCurveFoupChattering;		///< Ư�� �б� �߰��� ���� ���� ����
}DRIVING_FOUPCOVER_PARAM;

/**
@brief	 ���� ���氨�� �Ķ����
@author  ehoto
@date	 2013.06.23
*/
struct DRIVING_PARAM_SET
{
	PROPOTINAL_AXIS_PARAM		    Axis;			///< ���� �� ���� ����
	
	int 						    nMarkTypeCount;	///< Mark ���� ��
	DRIVING_MARK_PARAM			    Mark[4];		///< Mark �Ķ����: [0]�迭�� ���� ����
	
	DRIVING_STEER_PARAM			    Steer;			///< ���� ���� ����
	DRIVING_LINK_PARAM			    Link;			///< Path ���� ����
	DRIVING_FRONT_DETECT_PARAM	    FrontDetect;	///< ���� ����
	DRIVING_UBG_FAIL3_ERROR_PARAM   UbgFail3Err;	////< UBG ��ȣ ���� (UBG Fail 3) �� ���� �ߵ� ���� ����

	DRIVING_FOUPCOVER_PARAM         FoupCover;      //Foup Cover ä�͸�

	int CID_PAUSE_TIMEOVER; 					///< ����Off�� CID PAUSE TIMEOVER
	double StationMarkInPos;					//������ Tag ����ġ ��ũ���� Inpos ����
	int MapMakerCount_Driving;					//Mark�� MapMaker �ݻ����ν��� Ȯ�� �Ÿ���
	int MapMakerCount_Trans;					//Mark�� MapMaker �ݻ����ν��� Ȯ�� �Ÿ���

	double MapMakerMargin_Gap;					//Mark�� MapMaker �ݻ����ν��� Ȯ�� �Ÿ���
	bool  bVHLLifterDir;

	double ScatteredReflection;					//Station Mark �� ���ݻ���� ���Ͷ� �Ÿ�
	double ScatteredReflectionMax;					//Station Mark �� ���ݻ���� ���Ͷ� �Ÿ�

	int CIDWaitingTimeout; //�۾������ CID Occup �ݳ� ó�� Count

	bool CID_Fiber_Monitoring;
	int	 CID_SERIAL_CMD_DELAY_TURN;
	int LightGuide_TIME_OUT_MS_MTL;
	int LightGuide_TIME_OUT_MS_VHLLifter;
	int LightGuide_TIME_OUT_MS_AutoDoor;

	char	Version[8];

};



#endif
