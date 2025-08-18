#ifndef Def_DrivingHWInfoH
#define Def_DrivingHWInfoH

#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64

typedef enum DRIVING_IO_STEER_DIRECTION_CMD_
{
	DRIVING_IO_STEER_DIRECTION_CMD_LEFT	=0,
	DRIVING_IO_STEER_DIRECTION_CMD_RIGHT
}DRIVING_IO_STEER_DIRECTION_CMD;

typedef enum DRIVING_IO_STEER_TORQUE_CMD_
{
	DRIVING_IO_STEER_TORQUE_CMD_FREE		=0,
	DRIVING_IO_STEER_TORQUE_CMD_STOP		,
	DRIVING_IO_STEER_TORQUE_CMD_MOVING		,
	DRIVING_IO_STEER_TORQUE_CMD_TRACTION	,
}DRIVING_IO_STEER_TORQUE_CMD;

/**
@brief	 ���� �� H/W I/O ����
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_IO_
{
	UINT	BrakeRelease			: 1;	///< �극��ũ Ǯ������ true

	UINT	Axis1AmpOn          	: 1;	///< Motor Enable �Ǿ� ������ true
	UINT	Axis1Running        	: 1;	///< �������̸� true
	UINT	Axis1InPosition     	: 1;	///< ��� ��ġ�� �����ϸ� true
	UINT	Axis1AxisDone       	: 1;	///< ���� ��� �Ϸ�� true

	UINT	Axis2AmpOn          	: 1;	///< Motor Enable �Ǿ� ������ true
	UINT	Axis2Running        	: 1;	///< �������̸� true
	UINT	Axis2InPosition     	: 1;	///< ��� ��ġ�� �����ϸ� true
	UINT	Axis2AxisDone       	: 1;	///< ���� ��� �Ϸ�� true

	UINT	NodePreCheckOn      	: 1;	///< Node ���� ���� ���� On�Ǹ� true
	UINT	NodeOn              	: 1;	///< Node ���� ���� On�Ǹ� true
	UINT	TagStationFrontOn   	: 1;	///< Front �����Ǹ� true
	UINT	TagStationRearOn    	: 1;	///< Rear �����Ǹ� true

	UINT	ObsStatus           	: 2;	///< 0:�̰��� / 1:���� ���� / 2:���� ���� / 3:�̻� (DETECT_STATUS)
	UINT	DetectStatus        	: 2;	///< 0:�̰��� / 1:���� ���� / 2:���� ���� / 3:�̻� (DETECT_STATUS)

	UINT	SteerFrontLeftInOn      : 1;	///< ���� Input ���ʾ� ����
	UINT	SteerFrontRightInOn     : 1;	///< ���� Input �����ʾ� ����
	UINT	SteerRearLeftInOn       : 1;	///< ���� Input ���ʵ� ����
	UINT	SteerRearRightInOn      : 1;	///< ���� Input �����ʵ� ����
	UINT	SteerFrontTorque        : 2;	///< 0:Free / 1:Stop / 2:Moving / 3:����
	UINT	SteerFrontDirCmd        : 1;	///< ���� ��� ( 0:���� / 1:������ )
	UINT	SteerRearTorque         : 2;	///< 0:Free / 1:Stop / 2:Moving / 3:����
	UINT	SteerRearDirCmd         : 1;	///< ���� ��� ( 0:���� / 1:������ )
	
	UINT	Reserved				: 5;	///< ����
}DRIVING_IO;



/**
@brief	 ���� ���
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_AXIS_
{
	double	dCurrPos;			///< ���� ���� ��ġ
	double  dFinalPos;           ///< ���� ��ǥ ��ġ
	double  dSpeed;              ///< ���� �ӵ�
	double  dFollowError;        ///< ���� Following Error
	double  dSmallAddCheckPos;   ///< ���� �ҷ� �̵� ��� ��
	int     nError;              ///< ���� ����
}DRIVING_AXIS;



/**
@brief	 Node ����
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_NODE_
{
	int		nNodeCount;			///< Node �߰� ������ ���� ��� ���� (Rising Edge ����)
	double  dNodePosition;       ///< Node �߰� ������ ���ڴ� ��
	int     nPreCheckCount;      ///< Node ���� �߰� ������ ���� ��� ���� (Rising Edge ����)
	double  dPreCheckPosition;   ///< Node ���� �߰� ������ ���ڴ� ��
	int     nBcrId;             	///< Node ID ���� ��
}DRIVING_NODE;


/**
@brief	 Station ����
@author  ehoto
@date	 2013.06.19
*/
typedef struct TAG_STATION_
{
	double	dFrontPosition;	///< Tag Front �߰� ������ ���ڴ� �� (Rising Edge ����)
	double  dRearPosition;   ///< Tag Rear �߰� ������ ���ڴ� �� (Rising Edge ����)
	int     nBcrId;        	///< Tag ID ���� ��
}TAG_STATION;


/**
@brief	 Station ����
@author  ehoto
@date	 2013.06.19
*/
typedef struct QR_STATION_
{
	int		nQrId;		///< QR Left ID
	double  dQrOffset;   ///< QR Left ID �߰� �� Offset
}QR_STATION;

/**
@brief	 ���� ���
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_HW_INFO_SET_
{
	DRIVING_IO		IO;
	DRIVING_AXIS	Axis1;
	DRIVING_AXIS	Axis2;
	DRIVING_NODE	Node;
	TAG_STATION		TagStaion;
	QR_STATION		QrStaionLeft;
	QR_STATION		QrStaionRight;
}DRIVING_HW_INFO_SET;


#endif
