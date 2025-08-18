#ifndef Def_PositionInfoH
#define Def_PositionInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE

/**
@brief	 ��ġ ����
@author  ehoto
@date	 2013.06.19
*/
typedef struct POSITION_INFO_
{
	MARK_TYPE	MarkType;	///< 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight
	UINT		uNode;		///< ����� Node ID
	UINT		uNextNode;	///< ����� Next Node ID
	UINT		uStation;	///< ����� Station ID
	double		dOffset;		///< Node ���� Station������ �Ÿ�, ���� ��� ���� �� ������ �Ǵ� Offset"
}POSITION_INFO;

/**
@brief	 ��ġ ���� ( ���� + Mark ���� )
@author  ehoto
@date	 2013.06.19
*/
typedef struct POSITION_STATUS_
{
	bool			bMarked;				///< true: ����ġ �Ϸ� / false: ����ġ �̿Ϸ�
	UINT			uRealArrivedNode;	///< ���� �Ϸ� �� ������ ���� Node (Node ���� ���: Node�� ��ġ / Staion ���� ���: Node or NextNode) --> ���� ��� ���� �� ������ �Ǵ� Node
	POSITION_INFO	Info;				///< ���� ��ġ ����
}POSITION_STATUS;

/**
@brief	 ��ġ ����
@author  ehoto
@date	 2013.08.06
*/
//typedef struct COMMANDED_POSITION_INFO_
//{
//	MARK_TYPE	MarkType;	///< 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight
//	double		dCmdPos;	///< ��� ��ġ: �������� ���� ��ġ
//	double		dDecelDist;	///< ���� �Ÿ�: Map �ӵ� �������� �ʿ��� ���� �Ÿ�
//	double		dSpeed;		///< �ӵ�: Map �ӵ�
//	double		dDecel;		///< ���ӵ�
//	int			nCount;		///< ��� �� ������ NodeCount(Station ������ ��� �� ��� Next Node�� Count)
//}COMMANDED_POSITION_INFO;


/**
@brief	 ���� ��ġ ���� + ������ ����
@author  ehoto
@date	 2013.06.19
*/
typedef struct CURR_AND_TARGET_POSTION_STATUS_
{
	POSITION_STATUS			CurrPositionStatus;	///< ���� ��ġ ���� �� ����
	POSITION_INFO			TargetPosition;		///< ������ ��ġ ����
//	COMMANDED_POSITION_INFO	CommandedPosition;	///< �ֱ� �ΰ��� ��� ��ġ ����
}CURR_AND_TARGET_POSTION_STATUS;

#endif
