#ifndef Def_MarkParamH
#define Def_MarkParamH

#include <stdio.h>

/**
@brief	 Mark �Ÿ� �Ķ����
@author  ���¿�
@date	 2013.06.23
*/
typedef struct MARK_DIST_PARAM_
{
	double		dMarginToTarget;			///< Target������ ����	
	double		dDistFromFirstToSecond;		///< First ���� �� Second������ �Ÿ�	
	double		dLengthOfSecond;			///< Second ���� �Ÿ�	
	double		dAddDistAfetrFirstMet;		///< First ���� �� �̵� �Ÿ�(Small Add)	
	double		dAddDistAfterSecondMet;		///< Second ���� �� �̵� �Ÿ�(Small Add)	
	double		dForwardMoveDistLimit;		///< �߰� ���� Limit	
	double		dBackMoveDistLimit;			///< ���� Limit	
	double		dMarkInPosLimit;			///< ����ġ ��ġ ��Ż ���� Limit	
	double		dMinLengthToNext;			///< ���� ���� Node/Station�� �ּ� ����	
	double		dIgnoreTagDist;				///< ����Tag �Ÿ�
}MARK_DIST_PARAM;

/**
@brief	 Mark �ӵ� �Ķ����
@author  ���¿�
@date	 2013.06.23
*/
typedef struct MARK_SPEED_PARAM_
{
	double		dOfFirstMet;			///< First ���� ���������� �ӵ�	
	double		dOfSecondMet;		///< Second ���� ���������� �ӵ�	
}MARK_SPEED_PARAM;

/**
@brief	 Mark �����ӵ� �Ķ����
@author  ���¿�
@date	 2013.06.23
*/
typedef struct MARK_ACCEL_PARAM_
{
	double		dOfFirstMet;			///< First ���� ���������� �����ӵ�	
	double		dOfSecondMet;		///< Second ���� ���������� �����ӵ�	
}MARK_ACCEL_PARAM;

#endif