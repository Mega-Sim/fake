#ifndef Def_DrivingPositionInfoH
#define Def_DrivingPositionInfoH

#include <stdio.h>
#include "Def_Variables.h"		// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"		// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE
#include "Def_PositionInfo.h"	// POSITION_INFO, POSITION_STATUS, CURR_AND_TARGET_POSTION_STATUS


/**
@brief	 ����ġ �Ϸ� �� �����ϴ� ��
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_MARK_INFO_
{
	POSITION_STATUS		PositionStatus;	///< Mark �Ϸ�� �� map ���� ��ġ ����
	double          	dFrontPosition;	///< ����ġ �� ������ ���ڴ� �� (����ġ �� �̻� �̵� ���� �� ���Ǵ� ���� ��)
	double          	dRearPosition;	///< ����ġ �� �ķ��� ���ڴ� �� (����ġ �� �̻� �̵� ���� �� ���Ǵ� ���� ��)
}DRIVING_MARK_INFO;	



/**
@brief	 �Ÿ� ���� �׸�
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_COMPENSATION_INFO_
{
	double	dDistErrorSum;			///< ���� �Ÿ� ����: ���ڴ��� - Map ���� ��
	double  dLinkDistError;			///< Link ������ �Ÿ��� ����( ���ڴ��� - Map���� �� ) ( ���� �Ÿ� ���� - ���� ���� �Ÿ� ���� )

	double  dPreArriveOffset;		///< ��� Station Offset( ���� ���� �Ϸ� �� ���� : MARK_INFO.PositionInfo.Offset)
	
	double  dCompensatedPosition;	///< Map ���� ��ġ( ���ڴ��� - ���� �Ÿ� ������ + ��� Station Offset )
	
	double  dWearRatio;				///< ������: ��Ÿ� ���� �� ���� �Ÿ� ��� �߻��� ���� ���� ( ���� �Ÿ� ���� / Map ���� ���� �Ÿ� : ���� �Ÿ� 250m �̻� -: ���� ��� Wheel ���� / +: ���� ��� Wheel ���� �߻� )
}DRIVING_COMPENSATION_INFO;


/**
@brief	 ��� ó���� ���� ���� ����(���� ��ɿ��� �����Ǵ� ��
@author  ���¿�
@date	 2013.08.08
*/
typedef struct DRIVING_CTRL_REFERENCE_
{
	double	dCmdSpeed;		///< ��� ������ �ӵ�
	double	dRefSpeed;		///< ���� �ӵ� (Map)
	double	dCmdDecel;		///< ��� ������ ���ӵ�
	int		nKeyCount;		///< ��� ������ ��ġ�� Node Count
	int		nCount;			///< ��� ������ ��ġ�� Node Count
	double	dDist;			///< ��� ������ �� �Ÿ� (���� ��)
	double	dSpeedDownPos;	///< ���� ���� �� ��ġ (���� ��)
}DRIVING_CTRL_REFERENCE;


#endif
