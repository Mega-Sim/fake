#ifndef Def_DrivingEventStateH
#define Def_DrivingEventStateH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64

/**
@brief	 ���� Event
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_EVENT_
{
	UINT	CmdAdded			: 1;	///< 1-1. ��� �߰�
	UINT	TargetChanged		: 1;	///< 1-2. ������ ����
	UINT	PreCheck			: 1;	///< 1-4. Node�� 350mm �� �����Ǿ��� ��� ( NodeCount: PreCheck = Check +1 / ���� ��ġ���� (350-����mm) �� �̵� / �ӵ� ������ �߰� ���� �ʿ� )
	UINT	Check				: 1;	///< 1-8. Node�� ������ ��� ( NodeCount: Check = PreChec )

	UINT	SpeedDownPassed		: 1;	///< 2-1. ���� ������ ����ϴ� ���� �߻�
	UINT	FrontSteerMoved		: 1;	///< 2-2. ��ɿ� ���� ������ �������� �����ð� ��� �� ( Moving Torque �� Stop Torque )
	UINT	RearSteerMoved		: 1;	///< 2-4. ��ɿ� �Ĺ� ������ �������� �����ð� ��� �� ( Moving Torque �� Stop Torque )
	UINT	FrontSteerCompleted	: 1;	///< 2-8. ���� ������ ���� ������ �Ϸ� �Ǵ� ���� ( Chattering: 100ms / ���� ���� �Ÿ� ���� �� +(350 - ����)mm )
	
	UINT	SteerAllCompleted	: 1;	///< 3-1. ���Ĺ� ���� ������ �Ϸ� �Ǵ� ���� ( Chattering: 100ms )
	UINT	PushedByGuide		: 1;	///< 3-2. ChangeAreaByGuide ���� ( ������ ����Ǵ� ���� )
	UINT	StopWithNoReason	: 1;	///< 3-4. ��� �� �Ÿ��� �������� �� �̻� ���� ( ��ֹ�/���� ������ ���� ���� ��� / Small AddMove �� ���� ) �� ����
	UINT	DistNotMatch		: 1;	///< 3-8. ��� �� �Ÿ��� �ݿ����� ���� ��� �� ����

	UINT	MarkCmd				: 1;	///< 4-1. ��ũ ��ƾ ���鼭 ���� ���� ��� �߻� ��

	UINT	FrontDetectionChanged: 1;	///< 5-1. ���� ���� ���� ���� ��
	
	UINT	Reserved			: 18;	///< ����

}DRIVING_EVENT;


/**
@brief	 ���� State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_MARK_STATE_
{
	WORD	OnRoutineIn         : 1;	///< Mark Routine ����
	WORD	FirstMet            : 1;	///< Node: PreCheck ���� ��(NodeCount) / Tag Station: Front ���� ��(I/O) / QR: QR ó�� ������ ���
	WORD	SecondMet           : 1;	///< Node: Check ���� ��(NodeCount) / Tag Station: Rear ���� ��(I/O) / QR: QR �Ÿ� ����
	WORD	IdCheck             : 1;	///< ID  Ȯ�� �ǽ� 
	WORD	Arrived             : 1;	///< ���� ��ġ �����Ͽ� ���� ��

	WORD	OverRun             : 1;	///< �Ѿ�� ���
	WORD	Recovery            : 1;	///< �Ѿ �� ����

	WORD	OnMoveCmd           : 1;	///< �����̴� ��� �ΰ� �� true, �����̸� false�� ����

	WORD	FollowCtrlOffArea	: 1;	///< ������� Off �ϴ� ����

	WORD	Reserved			: 7;
}DRIVING_MARK_STATE;


/**
@brief	 ���� State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_STATE_
{
	UINT	NodePreChecked			: 1;	///< Node Count�� PreCheck>Check ��� / ������ Mark �Ǿ��� ��� ( Node Mark: ���� Node�� 350+40mm �̳� ���� ) / Station Mark: ���� Node�� PreCheck�� Check ���� ����
	UINT	OutInNodeDiffSteer      : 1;	///< Out/In Node ���� ������ �ٸ� ����� ���

	UINT	SteerMoving             : 1;	///< ���� ���� ��
	UINT	SteerFrontCompleted     : 1;	///< �� ���� �̵��� �Ϸ� ( Mark�ƴ� ���: ���� ���� �Ÿ� ���� �� +(350 - ����)mm )
	UINT	SteerAllCompleted       : 1;	///< ���� ���� �̵� �Ϸ� ( ���� �� ������ �� ���� �� ���� ��� �� ���� )
	UINT	SteerChangeAreaByGuide  : 1;	///< ���̵� ���Ͽ� ���� ������ ���� ���� ���� ( N�б� �߰�( ����+700 ~ ����-500 ) )
	
	UINT	StopByFrontDetect       : 1;	///< ���� ������ ���� ���� �� ( ��ֹ�, ���� ������ ���� ���� �� )
	UINT	SpeedDownByFrontDetect  : 1;	///< ���� ������ ���� ���� �� ( ��ֹ�, ���� ������ ���� ���� �� )
	
	UINT	SpeedDownPassed        	: 1;	///< ���ӽ��� ���� �������
	UINT	MarkArea	        	: 1;	///< Mark���� ���� �������
	UINT 	DualStageStbArea		: 1;
	UINT	Reserved            	: 21;	///< ����
}DRIVING_STATE;


/**
@brief	 ���� State
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_EVENT_AND_STATE_
{
	DRIVING_EVENT		Event;
	DRIVING_STATE		State;
	DRIVING_MARK_STATE	MarkState;		
}DRIVING_EVENT_AND_STATE;


#endif
