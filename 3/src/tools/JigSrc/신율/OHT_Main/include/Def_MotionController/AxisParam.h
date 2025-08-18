#ifndef AxisParamH
#define AxisParamH

#include <stdio.h>
#include "MinMaxCtrl.h"	//MIN_MAX_VALUE


/**
@brief	 ��� ���� �� �Ķ����
@author  ehoto
@date	 2013.06.23
*/
struct PROPOTINAL_GEAR_RATIO
{
	double	dIdealGearRatio;		///< ���� ( Ideal �ⱸ ��ġ : cts/mm )
	double	dIdeal2RealRatio;		///< ���� ���� ( ���� �� / Ideal �� )
	double	dDeviceDifference;		///< Ÿ ȣ����� ���� =  ���ظ鿡�� �� �� - ����� �� ( ��⿡�� ƼĪ ������ �� ���� ���� ���� : UserUnit )
	int		nHomeSearchCtsOffset;	///< ���� ������������ �ⱸ�� ���������� Offset (�޽�)
	
	PROPOTINAL_GEAR_RATIO()
	{
		dIdealGearRatio = 1.0;
		dIdeal2RealRatio = 1.0;
        nHomeSearchCtsOffset = 0;
	};
};

struct PROPOTINAL_GEAR_ROTATE
{
	int	nDeviceDifference;			///< Ÿ ȣ����� ���� =  ���ظ鿡�� �� �� - ����� �� ( ��⿡�� ƼĪ ������ �� ���� ���� ���� : UserUnit )
	int	nDeviceDifference_90;		////< Ÿ ȣ����� ���� =  ���ظ鿡�� �� �� - ����� �� ( ��⿡�� ƼĪ ������ �� ���� ���� ���� : UserUnit )
	int	nDeviceDifference_180;		///< Ÿ ȣ����� ���� =  ���ظ鿡�� �� �� - ����� �� ( ��⿡�� ƼĪ ������ �� ���� ���� ���� : UserUnit )
	PROPOTINAL_GEAR_ROTATE()
	{
		nDeviceDifference = 0;
		nDeviceDifference_90 = 0;
		nDeviceDifference_180 = 0;
	};
};

/**
@brief	��ġ ���� ����ü
@date    2013.07.28
@author  ���¿�
*/
struct POSITION_VALUE
{
	double	dPulse;		///< Pulse �Ÿ� ����
	double	dUserUnit;	///< ����� �Ÿ� ����
};

/**
@brief	 3�� ������ ���� �� �Ķ����
@author  ehoto
@date	 2013.07.28
*/
struct THREE_LINEAR_EQUATION_GEAR_RATIO
{
	POSITION_VALUE	PositionValue[10];		///< ��ġ �� ( Pulse / ����� ���� ) 4��
	double			dDeviceDifference;		///< Ÿ ȣ����� ���� =  ���ظ鿡�� �� �� - ����� �� ( ��⿡�� ƼĪ ������ �� ���� ���� ���� : UserUnit )
	int				nHomeSearchCtsOffset;	///< ���� ������������ �ⱸ�� ���������� Offset (�޽�)
};

/**
@brief	 �� Limit �Ķ����
@author  ehoto
@date	 2013.06.23
*/
struct AXIS_LIMIT_PARAM
{
	MIN_MAX_VALUE<double>	Accel;
	MIN_MAX_VALUE<double>	Decel;
	MIN_MAX_VALUE<double>	Speed;
	double					dAbnormalMoveSpeedMargin;
	MIN_MAX_VALUE<double>	Position;
	MIN_MAX_VALUE<double>	JogPosition;
};

/**
@brief	 �� Default �Ķ����
@author  ehoto
@date	 2013.06.23
*/
struct AXIS_DEFULT_PARAM
{
	double	dDefaultAccel;		///< �⺻ ���ӵ�
	double	dDefaultDecel;		///< �⺻ ���ӵ�
	double	dEmergencyDecel;	///< ��� ���ӵ�
};


/**
@brief	 ���� �� �Ķ����
@author  ehoto
@date	 2013.06.23
*/
struct PROPOTINAL_AXIS_PARAM
{
	PROPOTINAL_GEAR_RATIO	GearRatio;
	AXIS_LIMIT_PARAM		Limit;
	AXIS_DEFULT_PARAM		Default;
};


/**
@brief	 ���� �� �Ķ����
@author  ehoto
@date	 2013.07.08
*/
struct THREE_LINEAR_EQUATION_AXIS_PARAM
{
	THREE_LINEAR_EQUATION_GEAR_RATIO	GearRatio;
	AXIS_LIMIT_PARAM					Limit;
	AXIS_DEFULT_PARAM					Default;
};

#endif
