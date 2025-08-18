#ifndef Def_DrivingTimeInfoH
#define Def_DrivingTimeInfoH

#include <stdio.h>
#include "Def_TimeCheck.h"	//TIME_CHECK

/**
@brief	 ���� ���� �ð� ���� �׸�
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_FRONT_OBSERVE_
{
	TIME_CHECK	OHTDetectFail;	///< ���� ���� ���� Fail Flag �� ���� �ð�
	TIME_CHECK	OBSDetectFail;	///< ��ֹ� ���� ���� Fail Flag �� ���� �ð�
	TIME_CHECK	OBSDetect;		///< ��ֹ� ���� Flag �� ���� �ð�
}DRIVING_TIME_FRONT_OBSERVE;

/**
@brief	 ���� �ð� ���� �׸�
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_STEER_
{
	TIME_CHECK	SteeringChangeCmded;	///< ���� ���� ��� ���� Flag �� ���� �ð�: ä�͸����� ���� �Ϸ� �� false�� ���� 
	TIME_CHECK	SteeringNotMatch;		///< ���� ���� �̻� Flag �� ���� �ð�
	TIME_CHECK	FrontSteeringArrived;	///< ���� ���� ���� Flag �� ���� �ð�: ä�͸� �� Timeout ó��
	TIME_CHECK	RearSteeringArrived;	///< �Ĺ� ���� ���� Flag �� ���� �ð�: ä�͸� �� Timeout ó��
	TIME_CHECK	FrontSteeringDeparted;	///< ���� ������ ����� �ް� ��ġ �̵� ���� Flag �� ���� �ð�( ��ũ ���� ���� ���� )
	TIME_CHECK	RearSteeringDeparted;	///< �Ĺ� ������ ����� �ް� ��ġ �̵� ���� Flag �� ���� �ð�( ��ũ ���� ���� ���� )
}DRIVING_TIME_STEER;

/**
@brief	 �̻� ���� �ð� ���� �׸�
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_STOP_
{
	TIME_CHECK	OnAbnormalStop;	///< �̻� ���� Flag �� ���� �ð�
	TIME_CHECK	OnAbnormalRetry;	///< �̻� ���� Flag �� ���� �ð�
}DRIVING_TIME_STOP;

/**
@brief	 �ð� ���� ���� �׸�
@author  ehoto
@date	 2013.06.19
*/
typedef struct DRIVING_TIME_INFO_SET_
{
	DRIVING_TIME_FRONT_OBSERVE		FrontObserve;	///< ���� ����
	DRIVING_TIME_STEER				Steer;			///< ���� ����
	DRIVING_TIME_STOP				Stop;			///< ���� ����
}DRIVING_TIME_INFO_SET;

#endif

