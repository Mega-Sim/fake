#ifndef Def_DrivingControlH
#define Def_DrivingControlH

#include <mmsystem.h>	// To Use "timeGetTime()"
#include <stdio.h>

#include "MAP.h"

#include "Def_DrivingCmdInfo.h"		// ��� List ���� Frame
#include "Def_DrivingHWInfo.h"		// HW ���� �����ϴ� Frame
#include "Def_DrivingTimeInfo.h"	// ���� �ð� Ȯ�� Frame
#include "Def_DrivingPositionInfo.h"	// ��ġ ���� ���� Frame
#include "Def_DrivingSteerInfo.h"	// ���� ���� ���� Frame
#include "Def_DrivingEventState.h"	// �̺�Ʈ �� ���� ���� Frame
#include "Def_DrivingCtrlInfo.h"		// ���� HW ���� Frame
#include "Def_DrivingIniParam.h"		// ���� HW ���� Frame
#include "List.h"

#include "Def_CID_Protocol.h"			// VHL ���������� ���� ���� �����Ŷ ����

typedef enum DRIVING_MARK_FINAL_INFO_
{
	DRIVING_MARK_FINAL_INFO_NONE				= 0,		///< ���� ����
	DRIVING_MARK_FINAL_INFO_SLOW_REACH			= 1,		///< ���ڸ� ��ġ���� ����
	DRIVING_MARK_FINAL_INFO_JUST_REACH			= 2,		///< �ѹ��� ��Ȯ�� ��
	DRIVING_MARK_FINAL_INFO_OVER_RUN_ESTIMATED	= 3,		///< �Ѿ
	DRIVING_MARK_FINAL_INFO_OVER_RUN			= 4,		///< �Ѿ
	DRIVING_MARK_FINAL_INFO_LAST_CMD_START		= 5,		///< ������ ���� ����: ��Ȯ�� ��
	DRIVING_MARK_FINAL_INFO_LAST_CMD_RUNNING	= 6,		///< ������ ���� ����: ��Ȯ�� ��
	DRIVING_MARK_FINAL_INFO_GOOD_ARIVED			= 7		///< ���� ��ġ Mark �Ϸ�
}DRIVING_MARK_FINAL_INFO;

/**
@brief	 Path�� ���� Define
@author  ehoto
@date	 2012.11.12
*/
typedef enum DRIVING_STEERING_INFO_
{
	DRIVING_STEERING_NONE			= STEERING_NONE,		///< Steering ���� ����
	DRIVING_STEERING_LEFT			= STEERING_LEFT,		///< Left Steering
	DRIVING_STEERING_RIGHT			= STEERING_RIGHT,	///< Right Steering
	DRIVING_STEERING_MOVING			= STEERING_MOVING	///< Steering �̵� ��
}DRIVING_STEERING_INFO;

typedef enum DRIVING_STEERING_CMD_
{
	DRIVING_STEERING_CMD_NONE		= STEERING_NONE,		///< Steering ���� ����
	DRIVING_STEERING_CMD_LEFT		= STEERING_LEFT,		///< Left Steering
	DRIVING_STEERING_CMD_RIGHT		= STEERING_RIGHT	///< Right Steering
}DRIVING_STEERING_CMD;

typedef enum TRANS_MARK_START_STATUS_
{
	FRONT_OFF_REAR_OFF		= 0,		// ������ �����ϸ� ��ŷ
	FRONT_ON_REAR_OFF		= 1,		// �ڷ� 50mm �̵� �� ��ŷ
	FRONT_OFF_REAR_ON		= 2,		// Error
	FRONT_ON_REAR_ON		= 3		// �ڷ� 20mm �̵� �� ��ŷ
}TRANS_MARK_START_STATUS;

//---------------------------------------------------------------------------
/**
@brief	 ���� ���� ��ġ�� ��Ÿ����  Data
@author  ehoto
@date	 2012.12.12
*/
typedef struct POSITION_
{
	UINT	CurrNodeID;			///< ������ Update �� ���� Node
	UINT	NextNodeID;			///< ������ TAG ���� �� ���� ���� Node
	double	CurrNodeOffset;		///< ������ Update �� ���� Node �κ����� ��� �Ÿ�
	UINT	CurrStationID;		///< ������ Update �� ������ Node

	bool			Marked;		///< ��ŷ �Ϸ� ����: true /false
	MARK_TYPE	StationType;	///< ������ Type
} POSITION;


//---------------------------------------------------------------------------
/**
@brief	�̻� ���� �߻� ���� �� �߻� ���� �ð� ��� �ϴ� ����ü
@author  ���¿�
@date	 2013.04.12
*/
typedef class EVENT_TIME_CHECK_
{
public:
	bool	FlagOfAbnormal;	///< �̻� ��Ȳ �߻����� 
	DWORD	TimeOfAbnormal;	///< �̻� ��Ȳ �߻� �ð�

	void init()
	{
		FlagOfAbnormal	= false;
		TimeOfAbnormal	= timeGetTime();
	}

	EVENT_TIME_CHECK_()
	{
		init();
	}	
}EVENT_TIME_CHECK;

/**
@brief	 ���� ���� ��ġ�� ��Ÿ����  Data
@author  ehoto
@date	 2012.12.12
*/
typedef class DRIVING_ABNORMAL_CHECK_INFO_
{
public:
	EVENT_TIME_CHECK	OHTDetectFail;		///< ���� ���� ���� �̻� ���� Ȯ��(Fail Signal)
	EVENT_TIME_CHECK	OBSDetectFail;		///< ��ֹ� ���� ���� �̻� ���� Ȯ��(Fail Signal)
	EVENT_TIME_CHECK	OBSDetect;			///< ��ֹ� ���� ���� Ȯ��: ��ֹ� ������ ���� �ð� ���� �� ��� ���� ó��
	EVENT_TIME_CHECK	SteeringChange;		///< ���� ���� ���� Ȯ�� : ���� ����� ���� ���� �� ��� ���� ó��
	EVENT_TIME_CHECK	OnRunning;			///< ���� ���� Ȯ�� : Ư���� ���� ���� ���� �Ǿ��� ��� ���� ó��
	EVENT_TIME_CHECK	Steering;			///< ���� ���� Ȯ�� : ���� ��Ȳ�� ���� �ʴ� ���� ���°� �ö�� ��� ���� ó��
	
	void	init()
	{
		OBSDetect.init();
		SteeringChange.init();
		OnRunning.init();
		Steering.init();
	}

	DRIVING_ABNORMAL_CHECK_INFO_()
	{
		init();
	}	
} DRIVING_ABNORMAL_CHECK_INFO;


//---------------------------------------------------------------------------
/**
@brief	 ���� Motion�� ���� Refference Data
@author  ehoto
@date	 2012.11.26
*/
typedef struct CMD_INFO_
{
	// ���� 
	DRIVING_STEERING_CMD SteeringCmd;			///< ���� ����( 0:N, 1:L, 2:R )

	// �ӵ�
	double	SpeedOfRef;				///< ������ ���� �ӵ� (!=map Speed)
	double	SpeedOfCmd;				///< ��� �ӵ�
	double	SpeedOfLimit;			///< ���� �ӵ�

	// �Ÿ�
	UINT	CountOfCmdNode;			///< �̵� ��� �� �������� ��� ����
	double	DistCmdOfRef;			///< ��� ���� �Ÿ�
	double	PositionOfSpeedDown;		///< ������ �����ϴ� ��ġ
	double	DistOfOffset;			///<��� �ΰ� �� �ݿ��ϴ� �߰� �� - ��ŷ ���� �߰��Ÿ� + �Ÿ� ���� �� ���... 
} CMD_INFO;

/**
@brief	 Node�� ���� ����
@author  ehoto
@date	 2012.11.26
*/
typedef struct LINK_INFO_
{
	UINT	EndNodeID;			///< Link ���� EndNodeID
	int		CountOfNode;			///< Node ���� (���ۺ��� ~ Node��ġ)
	
	PATH_DIRECTION_INFO	PathDirectionOfMap;	///< �������� ���� ���� ���� ���� 
	PATH_STEERING_INFO	SteeringOfMap;		///< ���� ����	L, R, N, L2R, R2L

	double	SpeedOfMap;			///< Map ���� �ӵ�
	double	DistSumOfMap;		///< End Node ���� ��ġ (���ۺ��� ~ Node��ġ)
	double	DistOfThisPath;		///< �� Path�� ����

	bool	FlagOfMapMake;		///< MapMake Path Flag ( true:MapMake ���� / false: �Ϲ� ���� ���� )
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)
} LINK_INFO;

/**
@brief	 ���� ���� ����
@author  ehoto
@date	 2012.11.26
*/
typedef struct MARK_STATUS_
{
	bool	StateOnSmallAddPossible;		///< 00. ���� Routine������ ���� ���� �����Ͽ���
	bool	StateOnStopRoutineIn;		///< 0. StopRoutine ���� ����
	bool	StateOnFirstMet;				///< 1. StopTag ���� ����
	bool	StateOnSecondMet;			///< 2. StopTag ���� ��ġ ���� ����
	bool	StateOnStoped;				///< 3. ���� �Ϸ� ����
	bool	StateOnOverRunBack;			///< 4. OverRun�� ���

	UINT	FinalReadedTargetID;			///< ������ ���� ID ��
}MARK_STATUS;

/**
@brief	 ���� ���� ����
@author  ehoto
@date	 2012.11.26
*/
typedef struct STATUS_
{
	//1 1. ID, �ӵ�, ����, �Ÿ� ----------------------
	// ID
	int		CountOfNode;					///< Node ���� (���ۺ��� ~ Node��ġ)
	UINT	IDOfNode;					///< Node IDOfNode
	UINT	IDOfNodeBCR;					///< Node IDOfNode
	double	RealPositionOfNode;			///< Position Of Tag Station(For MapMaking)
	UINT	IDOfTagStationBCR;			///< Node ID Of Tag Station(For MapMaking)
	double	RealPositionOfTagStation;	///< Position Of Tag Station(For MapMaking)
	
	// ���� 
	DRIVING_STEERING_INFO	SteeringStatus;			///< L, R, M
	EVENT_TIME_CHECK	SteeringComplete;
	bool	SteeringFirstHit;					//1	���� ����: SteeringComplete
	DWORD	TimeOfSteeringFirstHit;				//1	���� ����: SteeringComplete
	bool	PreSteeringAvailable;

	DRIVING_STEERING_INFO	FrontSteeringStatus;		///< L, R, M
	EVENT_TIME_CHECK FrontSteeringComplete;
	bool	FrontSteeringFirstHit;				//1	���� ����: FrontSteeringComplete
	DWORD 	TimeOfFrontSteeringFirstHit;			//1	���� ����: FrontSteeringComplete
	
	// �ӵ� 
	double	SpeedOfNow;						///< ���� �ӵ�
	bool	StopWithoutFrontDetect;			///< ���� ���� ���� ������ ��� true

	// �Ÿ� 
	double	PositionOfNow;					///< ������ ��ġ
	double	RealPositionOfNodePreChecked;	///< PreCheck Node�� ���� ���ڴ� ��ġ��

	UINT	CountOfLastCmd;					///< ��� ���� ������ NodeCount
	UINT	TargetNode;						///< ������

#if 0
	//1 2. �������� ��ƾ ----------------------
	BYTE	TypeOfFrontObject;   		///< ���� Ÿ�� (
	double	DistToFrontObject;			///< ���� ��ü���� ���� �Ÿ�
#endif

	//1 3. Mark Routine ----------------------	: ���� ����ü�� ���� 
	MARK_STATUS	MarkStatus;
} STATUS;

/**
@brief	 ���� Refference Ȯ�� �ʿ� Event Flag
@author  ehoto
@date	 2012.11.26
*/
typedef struct EVENT_FLAG_
{
	// Point Event : 16
	UINT	Event_Node_PreChecked				: 1;	// Node �� �̸� �����Ǿ��� ��� --> ������ ����
	UINT	Event_Steering_Completed			: 1;	// ���� ���� �Ϸ� -> ���� ���� Limit ���� ����
	UINT	Event_Front_Steering_Completed		: 1;	// �� ���� ���� �Ϸ� -> ���� ���� Limit ���� ����
	UINT	Event_Steering_Changed_OnNBranch	: 1;	// N�б⿡�� ���� ���� �Ϸ� -> ���� ���� Limit ���� ����

	UINT	Event_NewNode_Arrived				: 1;	// ���ο� Node ������ ��: NodeCount �߰� / BCR �� ����
	UINT	Event_SpeedDownPoint_Passed			: 1;	// ���� ���� ������� ���

	UINT	Event_Cmd_Added						: 1;	// ��� �߰��Ǿ��� ���
	UINT	Event_Target_Updated				: 1;	// Ÿ�� ������Ʈ �Ǿ��� ���

	UINT	Event_Safety_Factor_Changed			: 1;	// ���� ���� ���� ���
	
	UINT	Event_Move_Dist_Not_Match			: 1;	// ��� �� �Ÿ��� AMC�� ���� �Ÿ��� �ٸ� ��� 
	UINT	Event_Stop_With_No_Reason			: 1;	// �������� ��

	UINT	Event_OutInNodeArea_Entered			: 1;	//Out/In Node ���� ���� ���� ��

	UINT	Event_Reserved						: 4;

	// Point Event Of Mark : 8

	UINT	MarkEvent_NextIsStop_and_SpeedDown	: 1;	// Ÿ�� ���� ��ƾ 1. --> ������ GoMoreTagLength �� ������ ��� ���� : (State_NextIsTarget||State_NextIsBeforeMergeNode) && State_OnSpeedDown
	UINT	MarkEvent_FirstMet					: 1;	// Ÿ�� ���� ��ƾ 2. --> Target ���� ��� Stop ��ġ�� ���� : �� ���ǿ��� ���� ó�� ���� ���� ���
	UINT	MarkEvent_SecondMet					: 1;	// Ÿ�� ���� ��ƾ 3. --> ���� ���� �� ���� ���� : �� ���ǿ��� ���� ������ ���� ���� ���
	UINT	MarkEvent_BackFromOverRun			: 1;	// Ÿ�� ���� ��ƾ 4. --> Overrun�Ǿ��� ��� Back ���

	UINT	Mark_Reserved						: 4;

	// State Event: 8
	UINT	State_OnSteeringChanging			: 1;	// ��Ƽ� ���� �� --> ���� �Ÿ��� ���ҿ� ���� �ӵ� ���� ������ ���� ����
	UINT	State_OnFrontSteeringCompleted		: 1;	// ��Ƽ� ���� �� --> ���� �Ÿ��� ���ҿ� ���� �ӵ� ���� ������ ���� ����
	UINT	State_OnPreChecked					: 1;	//Node�� PreCheck �Ǿ� �ִ� ��Ȳ( �ǳ�� ������ �� )
//	UINT	State_OnOutInNodeArea				: 1;	// InOut ��� ������ ���� : ������ + ������ ���
	
	UINT	State_OnSpeedDown					: 1;	// ���� ���� ���� ���
	
	UINT	MarkState_NextIsTarget				: 1;	// ���� ��尡 Ÿ���� ��� --> ������ƾ
	UINT	MarkState_NextIsPreMergeNode		: 1;	// ���� ��尡 �շ� ���� ����� ��� --> �浹 ���� �� �帧 ����
	UINT	MarkState_AvailabeToPass			: 1;	// �շ� �� Node��  Pass �����ϵ��� ���� ���� ���

	UINT	State_Reserved						: 1;
}EVENT_FLAG;

/**
@brief	 ���� Refference Ȯ�� �ʿ� Event ���� ����
@author  ehoto
@date	 2012.11.26
*/
typedef struct EVENT_CHECK_
{
	WORD	EventExist;
	BYTE	MarkEventExist;
	BYTE	StateExist;

}EVENT_CHECK;

/**
@brief	 ���� Refference Ȯ�� �ʿ� Event Flag �� ���� ���� ����
@author  ehoto
@date	 2012.11.26
*/
typedef union EVENT_
{
	EVENT_FLAG	Flag;
	EVENT_CHECK	Check;
}EVENT;

/**
@brief	 ���� ���� �� ���� ������ 
@author  ehoto
@date	 2012.11.26
*/
typedef struct SAFETY_
{
	// ���� Target �� ���� �� 
	double	DistOffsetOfPreDriving;		///< ���� ���� Ÿ�ٿ� ���� Offset �Ÿ� : �����ÿ��� ������ ���ǰ� ��߽ÿ� ���ؼ� ����

	// ���� ó�� 
	double	DistOfTotalError;			///< �� �Ÿ� ���� : ���ڴ� �� - �̻�(ideal) �Ÿ� 
	double	DistOfThisError;				///< �ֱ� ��ũ���� �߻��� �Ÿ� ����

	// �ӵ� ����
	double	SpeedLimitBySteering;		///< ������ �����ϱ� ���� �ּ� �ӵ�
	double	DistLimitBySteering;			///< ���� ������ �ִ� �Ÿ� 
	UINT	CountLimitBySteering;		///< ���⿡ ���� �ӵ� ������ ���� ���� ����� Ÿ���� ���� ��� ���� �ӵ��� ���� ����... �����ϸ鼭 �ӵ��� �� ���� �ʿ�� ����

	double	SpeedLimitByObstacle;		///< ������ �����ϱ� ���� �ּ� �ӵ�
	double	DistLimitByObstacle;			///< ��ֹ� ������ ���� �� �� �ִ� �ִ� �Ÿ�
//	UINT CountLimitByObstacle;		  			///< ���⿡ ���� �ӵ� ������ ���� ���� ����� Ÿ���� ���� ��� ���� �ӵ��� ���� ����... �����ϸ鼭 �ӵ��� �� ���� �ʿ�� ����  : ���� �� ���� ��ġ	
}SAFETY;

/**
@brief	 �����縦 ���� ������ 

	Node �߰� �� Ÿ���� ���� �ݿ�
@author  ehoto
@date	 2012.11.26
*/
typedef struct TAG_
{
	bool	bGotMarkInfo;					///< false: ��ũ ������ ���� ���Ͽ���, true: ��ũ ������ �޾���

	UINT	IDofStation;						///< Station ID -- ���� ��� 0
	
	UINT	IDofPreNode;						///< ������ ���� ���
	UINT	IDofNextNode;					///< ������ ���� ���	- �Ÿ� ������ Ű�� ���� ���ɼ� ����
	UINT	CountOfFinalNode;				///< �������� ���� �޷����� ������ Node ( Staion Target �� ��� Next Node / Node Target �� ��� PreNode )
	MARK_TYPE	StationType;				///< ������ Type

	double	DistSumToTarget;					///< ��ߺ��� Target������ �Ÿ�

	double	DistOffsetFromPreNode;			///< ������ ���� ���κ����� Offset
	double	DistBetweenNode;					///< ������ ��尡 ���� ��� ���̿� ���� ��, �� ������ ���� ���� �Ÿ�
	
	double	LengthBeforeFirst;				///< ����ġ ���� �� �Ÿ� ������ ���� �� ������ �̵� �Ÿ� (-������ Final Node���� ���ҽ�Ű�� �Ÿ�)
	double	LengthFromFirst;					///< ����ġ ���� �� ���� ���� �Ÿ� : Front 2 Rear ���� 135mm
	double	LengthOfSecond;					///< ����ġ Tag�� ũ�� : Rear ���� 5mm
	
	double	AddLengthForFirst;				///< ����ġ Tag �̵��� �� ���ݾ� �����ϴ� �Ÿ� : Tag ������ 2/3 ���� (������ Tag: 90mm)
	double	AddLengthForSecond;				///< ����ġ �̵��� �� ���ݾ� �����ϴ� �Ÿ� : ����ġ ������ 1/3 ����  (������ Tag: 5*1/3mm)
	double	DistLimitOfAddLengthForFirst;	///< ����ġ Tag �̵��� �� ���ݾ� �����ϴ� ȸ���� �ִ밪
	double	DistLimitOfAddLengthForSecond;	///< ����ġ �̵��� �� ���ݾ� �����ϴ� ȸ���� �ִ밪

	double	SpeedBeforeFirst;				///< ���� ���� �Ÿ� ���� �ӵ� 
	double	SpeedOfFirst;					///< ���� ���� �Ÿ� ���� �ӵ� 
	double	SpeedOfSecond;					///< ����ġ ������ ���� �ӵ� 

	double	AccelBeforeFirst;				///< ���� ���� �Ÿ� ���� ���ӵ� : 0 �̸� ������ 
	double	AccelOfFirst;					///< ���� ���� �Ÿ� ���� ���ӵ� : 0 �̸� ������  
	double	AccelOfSecond;				///< ����ġ ������ ���� ���ӵ� : 0 �̸� ������	

	// ����ġ�� ���� �԰� ��ġ�� ����ϴ� ���  : todo:����
	double	RealPositionOfFirstEdgeCheck;	///< ����ġ ���� ������ ó�� ������ ��ġ 
	double	RealPositionOfSecondEdgeCheck;	///< ����ġ Tag�� ó�� ������ ��ġ
}TAG;

/**
@brief	 ������ ���� �Ķ����
@author  ���¿�
@date	 2013.03.29
*/
typedef struct PARA_FOR_MOVE_
{
	double	Accel;	///< ���ӵ� (mm/msec2)
	double	Decel;	///< ���ӵ� (mm/msec2)
	double	Speed;	///< �Ÿ� (mm/msec)
	double	Dist;	///< �Ÿ� (mm)

	USER_MOVE_DS_ADDITIONAL_INFO	AdditionalInfo;	///< Move DS �� Extension ������ ����ϱ� ���� ����
}PARA_FOR_MOVE;


/**
@brief	 INI �Ķ����
@author  ���¿�
@date	 2013.05.30
*/
typedef struct INI_PARA_FOR_DRIVING_
{

}INI_PARA_FOR_DRIVING;


//1	NEW WAY ADDITION

/**
@brief	�з��� ��� ���� ����
@author  ���¿�
@date	 2013.07.23
*/
typedef struct CLASSIFIED_DRIVING_CMD_SET_
{
	//2 ��� ���� List Of Information
	List<DRIVING_CMD_INFO_OF_NODE*> 		NodeList;		///< Node ������ �� ���� ���� Ȯ�� �� ���
	List<DRIVING_CMD_INFO_OF_STEER*>		SteerList;		///< ���� ���� �� ���⿡ ���� �Ÿ� �ӵ� ����
	List<DRIVING_CMD_INFO_OF_DIV*>			DivList;		///< �б� ����(CID���� ����)
	List<DRIVING_CMD_INFO_OF_DIRECTION*>	DirectionList;	///< ���������� ���� ����: Node�� ������Ʈ �Ǳ� �� Path�� ���� ������ ��ġ ����(Count/�Ÿ�)�� ����
	List<DRIVING_CMD_INFO_OF_SPEED*>		SpeedlList;		///< �ӵ� ��ȭ�� ���� ���� ����
	List<DRIVING_CMD_INFO_OF_PATH*> 		PathList;		///< ����� ����
	
	//2 ���� ������ ����
	DRIVING_CMD_INFO_OF_TARGET			TargetInfo;		///< ���� �������� ���� ����
}CLASSIFIED_DRIVING_CMD_SET;


/**
@brief	��� ����� ���� ����
@author  ���¿�
@date	 2016.06.03
*/
typedef struct CLASSIFIED_DRIVING_CHANGE_
{
	//2 ���� ������ ����
	int 								nCheckNodeCount;
	double								dEncoderFinal;

	DRIVING_CMD_INFO_OF_TARGET			TargetInfo;		///< ���� �������� ���� ����
}CLASSIFIED_DRIVING_CHANGE;


/**
@brief	��ġ ���� ����
@author  ���¿�
@date	 2013.07.23
*/
typedef struct DRIVING_POSITION_INFO_SET_
{
	CURR_AND_TARGET_POSTION_STATUS	CurrentAndTarget;	///< ���� OHT ��ġ ����, ������ ����
	DRIVING_COMPENSATION_INFO		Compensation;		///< �Ÿ� ���� ����
	DRIVING_MARK_INFO				Mark;				///< ���� ����� Mark ����:	���� �� CurrentAndTarget.CurrPositionStatus ������ ���� 
	DRIVING_CTRL_REFERENCE			CmdRefInfo;	
	bool							bFirstNIDIsCorrect;		///< ó�� ���� Node �� ID ���� Ȯ�� ���: Node�� 0~10mm Offset�� Station  ��� / ����(Station ���� ���� ���)
	bool							bPreCheckIsOverFirstNode;	///< ��� �� PreCheck ������  ù Node�� �Ѿ �ִ� ��� 
}DRIVING_POSITION_INFO_SET;

/**
@brief	���� ���� ����
@author  ���¿�
@date	 2013.07.23
*/
typedef struct DRIVING_INFO_SET_
{
	DRIVING_HW_INFO_SET				HwInfo;	

	DRIVING_TIME_INFO_SET			Time;
	DRIVING_POSITION_INFO_SET		Position;
	STEER_INFO						Steer;

	DRIVING_EVENT_AND_STATE			EventAndState;
	DRIVING_CTRL_INFO_SET			Control;
}DRIVING_INFO_SET;


/**
@brief	���� Step ���� ����
@author  ���¿�
@date	 2013.07.23
*/
typedef enum DRIVING_CTRL_STEP_
{
	DRIVING_CTRL_STEP_NONE			= 0,	///< ���� ����

	DRIVING_CTRL_STEP_GO_PASSBOX	,
	DRIVING_CTRL_STEP_ELEVETOR_RIDE 	,	///< Elevetor ��� : �ö�
	DRIVING_CTRL_STEP_ELEVETOR_OFF		,
	DRIVING_CTRL_STEP_GO_INIT		,	///< ���� ��� : �ʱ�ȭ
	DRIVING_CTRL_STEP_GO_PREPARE	,	///< ���� ��� : ���� �غ�
	DRIVING_CTRL_STEP_GO_FIRST_NODE	,	///< ���� ��� : Node ��ó Station ��� Node Count ������ �����ϱ� ���� ó��
	DRIVING_CTRL_STEP_GO_RUN		,	///< ���� ��� : ����
	DRIVING_CTRL_STEP_GO_MARK		,	///< ���� ��� : ���� ��ƾ
	DRIVING_CTRL_STEP_GO_CHECK_STOP	,	///< ���� ��� : ���� Ȯ�� ��ƾ
	DRIVING_CTRL_STEP_GO_COMPLETE	,	///< ���� ��� : �Ϸ� Ȯ�� 

	DRIVING_CTRL_STEP_MANUAL_INIT		,	///< Manual ��� : �ʱ�ȭ
	DRIVING_CTRL_STEP_MANUAL_AMP_ON		,	///< Manual ��� : AMP ON
	DRIVING_CTRL_STEP_MANUAL_MOVE_CMD	,	///< Manual ��� : ���� ��� 
	DRIVING_CTRL_STEP_MANUAL_CHECK_STOP	,	///< Manual ��� : ���� Ȯ��

	DRIVING_CTRL_STEP_MANUAL_MARK_INIT			,	///< Manual Mark ��� : �ʱ�ȭ
	DRIVING_CTRL_STEP_MANUAL_MARK_PREPARE		,	///< Manual Mark ��� : ���� �غ�
	DRIVING_CTRL_STEP_MANUAL_MARK_RUN			,	///< Manual Mark ��� : ���� ��� 
	DRIVING_CTRL_STEP_MANUAL_MARK_CHECK_STOP	,	///< Manual Mark ��� :  ���� Ȯ�� ��ƾ
	DRIVING_CTRL_STEP_MANUAL_MARK_COMPLETE		,	///< Manual Mark ��� : �Ϸ� Ȯ�� 
	

}DRIVING_CTRL_STEP;

typedef enum PASSBOX_CTRL_STEP_
{
	PASSBOX_CTRL_STEP_NONE						= 0,
	PASSBOX_CTRL_STEP_INIT						,
	PASSBOX_CTRL_STEP_GO_SIGNAL_CHECK			,	
	PASSBOX_CTRL_STEP_SIGNAL_OFF				,	
	PASSBOX_CTRL_STEP_ES_SIGNAL_CHECK			,		
	PASSBOX_CTRL_STEP_PB_AVBL_SIGNAL_CHECK		,		
	PASSBOX_CTRL_STEP_ABNORMAL_SIGNAL_CHECK		,			
	PASSBOX_CTRL_STEP_OHT_STATUS_ON				,
	PASSBOX_CTRL_STEP_CLOSE_ON_CHECK			,	
	PASSBOX_CTRL_STEP_DOOR_SELECT				,		
	PASSBOX_CTRL_STEP_OPEN_REQ					,		
	PASSBOX_CTRL_STEP_BUSY_ON_SIGNAL_CHECK		,			
	PASSBOX_CTRL_STEP_READY_ON					,				
	PASSBOX_CTRL_STEP_CLOSE_OFF_CHECK			,					
	PASSBOX_CTRL_STEP_BUSY_OFF_CHECK			,						
	PASSBOX_CTRL_STEP_PASSBOX_DOOR_OPEN_CHECK	,								
	PASSBOX_CTRL_STEP_COMPLETE	
}PASSBOX_CTRL_STEP;

//typedef	enum	MANUAL_DRIVING_STEP_TYPE_
//{
//	DRIVING_CTRL_STEP_MANUAL_INIT			= 0,
//	DRIVING_CTRL_STEP_MANUAL_AMP_ON			= 1,
//	DRIVING_CTRL_STEP_MANUAL_MOVE_CMD		= 2,
//	DRIVING_CTRL_STEP_MANUAL_CHECK_STOP		= 3
//}MANUAL_DRIVING_STEP_TYPE;


// ���������Ϳ� �� ���� ���� ����
	/* [CID�� �ʱ� ����]
	   CID-O�� Operating Mode (O6)�� ��1(CID)�� ���� ��
	   Select On, ��ġ�� ������ O6�� ��1�� ���� �� �켱 Select On.
	   CID�� �ʱ���� 1(CID)�̰�, VHL�� �̵��Ͽ� LIFTER������ �����ϸ�
	   CID-O�� I5�� '1'�� �������� ����, ������ ���� ������ �� �� �ִ� */

// Select Off�� ��  Operating Mode (O6)�� ��0(MTL/Lifter)�� ���� �� Select On.
// Select ON-> OFF-> ON�Ͻ� ��,OFF�ð��� �ּ� 10ms �̻��� ����.
typedef enum VHL_ELEVETOR_RIDE_CTRL_STEP_
{
	VHL_ELEVETOR_RIDE_CTRL_STEP_NONE						= 0,	// �����ϰ� ���� �ʴ�
	VHL_ELEVETOR_RIDE_CTRL_STEP_CID_INIT					,		// IO�� ���� ������ ���� ��ȯ��Ų�� 
	
	VHL_ELEVETOR_RIDE_CTRL_STEP_SERIAL_INIT					,		// �ø��� ����� �ʱ�ȭ Start
	VHL_ELEVETOR_RIDE_CTRL_STEP_SERIAL_WAIT					, 		// �ø��� ����� �ʱ�ȭ ������

	VHL_ELEVETOR_RIDE_CTRL_STEP_INPUT_REQUEST				,		// �����㰡 ��û��
	VHL_ELEVETOR_RIDE_CTRL_STEP_INPUT_CONFIRM_WAIT		    ,		// �����㰡 ���δ��
	VHL_ELEVETOR_RIDE_CTRL_STEP_COMPLETE				    ,		// Process OK.
	

} VHL_ELEVETOR_RIDE_CTRL_STEP;


// ������ ���� �����㰡 ��û����
typedef enum VHL_ELEVETOR_PREPASS_STEP_
{
	VHL_ELEVETOR_PREPASS_STEP_NONE		 = 0,	// �����㰡 �������� ����
	VHL_ELEVETOR_PREPASS_STEP_TOUCH			,	// �������̾��� (�Ϸ������ ����)
	VHL_ELEVETOR_PREPASS_STEP_COMPLETE		,	// �����㰡 �Ϸ�. RIDE���� ���ʿ�. �ٷ� ������ ��
} VHL_ELEVETOR_PREPASS_STEP;


// ���������Ϳ��� ���� ���� ���� ����
typedef enum VHL_ELEVETOR_OFF_CTRL_STEP_
{
	VHL_ELEVETOR_OFF_CTRL_STEP_NONE							= 0,	// �����ϰ� ���� �ʴ�
	VHL_ELEVETOR_OFF_CTRL_STEP_INIT							,
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_REQUEST_INIT			,		// �������� �������� ����ʹٰ� ��û�Ѵ� (��/�� ���� �� ��������)
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_RESPONSE				,		// ��û�� ���� ������ üũ�Ѵ�
	VHL_ELEVETOR_OFF_CTRL_STEP_OUTPUT_COMPLETE				,		// ������ �ȴٰ� ������ ����. ������Ʈ �ӽ� ����
} VHL_ELEVETOR_OFF_CTRL_STEP;

typedef enum MTL_TakeOut_CTRL_STEP_
{
	MTL_TakeOut_CTRL_STEP_NONE						= 0,
	MTL_TakeOut_CTRL_STEP_INIT						,
	MTL_TakeOut_CTRL_STEP_OCSReport_CHECK			,
	MTL_TakeOut_CTRL_STEP_TakeOutREQ_ON				,
	MTL_TakeOut_CTRL_STEP_TakeOutOk_SIGNAL_CHECK	,
	MTL_TakeOut_CTRL_STEP_TakeOutREQ_OFF     		,
	MTL_TakeOut_CTRL_STEP_FrontMoveOK_SIGNAL_CHECK	,
	MTL_TakeOut_CTRL_STEP_OHT_MTL_Insert_Start		,
	MTL_TakeOut_CTRL_STEP_OHT_MTL_Insert_End		,
	MTL_TakeOut_CTRL_STEP_OHT_Inposition_CHECK		,
	MTL_TakeOut_CTRL_STEP_TakeOutCompt_CHECK		,
	MTL_TakeOut_CTRL_STEP_COMPLETE                  ,
	MTL_TakeOut_CTRL_NG_COMPLETE                    ,

}MTL_TakeOut_CTRL_STEP;

typedef enum MTL_TakeIn_CTRL_STEP_
{
	MTL_TakeIn_CTRL_STEP_NONE						= 0,
	MTL_TakeIn_CTRL_STEP_INIT						,
	MTL_TakeIn_CTRL_STEP_OHTTakeIn_OK_CHECK			,
	MTL_TakeIn_CTRL_STEP_TakeIn_SIGNAL_CHECK		,
	MTL_TakeIn_CTRL_STEP_OHT_MTL_TakeIn_Start		,
	MTL_TakeIn_CTRL_STEP_OHT_MTL_TakeIn_End	   		,
	MTL_TakeIn_CTRL_STEP_OHT_Inposition_CHECK  		,
	MTL_TakeIn_CTRL_STEP_TakeInCompt_CHECK	   		,
	MTL_TakeIn_CTRL_STEP_COMPLETE					,
	MTL_TakeIn_CTRL_STEP_OCSReport_Error			,
}MTL_TakeIN_CTRL_STEP;



// shkim delete_reserve
// 2016.10.15  CID��� ���� State���� (auto)
// shkim
#if 0
typedef enum CID_STEP_
{
	CID_STEP_NONE								= 0,   	// �ƹ��͵� ���� �ʰ� ������ ����üũ�� �ϴ� ���̴�
	// CID_STEP_LR_SETTING							 ,		// Left, Right�� �����Ѵ�. �� ���¿� �и��� ������ ������ ���� (5.1.2.15)�ÿ��� Link�� �����ϰ�
														// �����ݳ� �� ACKȮ���� �¿� Select�� �ؾ� �ϴ� ��, �� ���¿� �Բ� �������� ���� �����̴�.
	// CID_STEP_LINK_WAITING						 ,		// ��ũ ON�� ��, Link�� �̷�������� ���Ѻ��� ���̴�
														//      (����ó�� : �����ð� �Ǵ� �Ÿ����� ������� ������ �����Ѵ�)
														//      (����ó�� : ���� �����ÿ��� �����ð��� ���°��� ī��Ʈ�� �����Ͽ� ��ٸ����� �Ѵ�)

	CID_STEP_CONDITIONAL_CHK_RESUME					 ,  // 3
	CID_STEP_CONDITIONAL_CHK_PAUSE					 ,  // 4
	CID_STEP_CONDITIONAL_CHK_RESERVE				 ,  // 5
	// CID_STEP_MOVING_PLAN_RESERV_SLOPE			 ,	// 6

	CID_STEP_MOVING_PLAN_CHK					 ,   // 7   // ���� �������� CID End��� �Ǵ� �� ���ı������� �Ǵ��Ͽ� �������� CID Start��忡
														// ������ ���̶�� ������ ���� ���õ����� ���ʿ��ϹǷ� ���� ����� ��������
														// �� �ܰ迡�� ��� Ȯ���ϸ� ����Ѵ�

	CID_STEP_CONDITIONAL_CHK_RUNNING				,	 //8   // (1) �������� ���·μ�, ���� ��ġ�� CID End Node���� ���θ� �ݺ������� Ȯ���Ѵ�
														// (2) ���� �� ���¿��� ������ ������ �Ͼ�� �����ݳ� �� �ݳ� Ȯ���ϰ�, LR�������� ���� �Ѵ�

	CID_STEP_N_BRANCH_SELECT_OFF				 ,   // 9
	CID_STEP_N_BRANCH_SELECT_ON				 	 ,   // 10

	// CID_STEP_COMPLETE							 ,	 // 11	// CID������ ��� ����Ǿ� I/O�� Ŭ�����ϰ� ���º����� �����Ѵ�

}CID_STEP;
#endif
// ---------------------------------------------------------
/**
@brief	CID ���� Step ���� ����
@author  ������
@date	 2016.10.30
*/
typedef enum CID_REVISED_STEP_
{
	// CID_STEP_INITIALIZATION							= 0,
	CID_STEP_CONNECTING							= 1,	// 1
	CID_STEP_CHECKING_PATH						,		// 2
	CID_STEP_OCCUPYING_PATH						,		// 3
	CID_STEP_COMPLETION							,		// 4
	// CID_STEP_DISCONNECTING						,	// 5
	CID_STEP_ABNORMAL							,	// 5
	CID_STEP_WAITING_FOR_PATH					,		// 7
	CID_STEP_COMPLETION_DUE_TO_JOB_CHANGE		,
	
} CID_REVISED_STEP;


/*
	CID Status�ӽ� ���� ����
*/
typedef enum executeCID_CMD_
{
	executeCID_NONE					= 0,
	executeCID_N_Branch_Select_Off	   ,
	executeCID_N_Branch_Select_ON	   ,

} executeCID_CMD;


/*
	�¿� �Ϲ� CID, �¿� ���� CID�� ����
*/
typedef enum CID_DIRECTION_
{
	CID_DIRECTION_LEFT			= 0, // 0
	CID_DIRECTION_RIGHT			 	,// 1
	CID_DIRECTION_COMMON_LEFT		,// 2
	CID_DIRECTION_COMMON_RIGHT		,// 3
	CID_DIRECTION_NOT_LINK			,// 4
} CID_DIRECTION;

#define CID_LAYOUT_TYPE_NORMAL	1
#define CID_LAYOUT_TYPE_SLOPE	2


// 2016.10.15  CID��� ���� State���� (auto)
// shkim
// �ó����� : OHT Manual Mode �϶� (76p)
/*
	-  ���� Node�� ����, CID�����̸� L �Ǵ� R�� �Ҵ� (O1 or O2) 
	-  Operation Mode�� CID�� ���� (O6�� 1�� ����)	
	-  Abnormal (O5�� 1�� ����) 
	- Link�� Ȯ�εǸ� ������ ON
*/

typedef enum MANUAL_CID_
{
	MANUAL_CID_MODE			 = 0,	// 1) �ʱ���¿��� CID���� ��ȯ.  Operation Mode�� CID�� ���� (O6�� 1�� ����).
	
	MANUAL_CID_SETTING			,	// 2) �⺻���� �ǽ�. Abnormal ��Ʈ (O5�� 1�� ����)
	
	MANUAL_CID_DIRECTION_CHK	,	// ���������� �����Ѵ� (MAP�Ǵ� ������ �̿�)
									// - Left or Right���� (Left = O1, Right = O2) : AllOff�� �ش������ �Ҵ�
									
	MANUAL_CID_RESERVED_ON		,	// ������ �Ҵ� (O3 = 1). 
									// ����Ȯ�� �ܰ� (MANUAL_CID_DIRECTION_CHK) �� �̵��Ͽ� �ݺ� �����Ѵ�
	MANUAL_CID_RESERVED_OFF		,

	MANUAL_CID_ABNORMAL_MODE_ON	,

	MANUAL_CID_ABNORMAL_MODE_OFF,

	MANUAL_CID_LEFT_SELECT		,

	MANUAL_CID_RIGHT_SELECT		,
	
	MANUAL_CID_COMMON_TAG		,
}MANUAL_CID;

#define DELAY_SERIAL_READ	    			70




// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack

// CIDȮ�� ������ ���� ������ �־�� ����ü ����
typedef struct
{
	int nDetectStatus;			//UBG ���� ����
	int tmpCurrentNodeID;
	int tmpPreNodeID;	//(int)m_PositionInfo.CurrPreCheckNode : �������� ��� ID
	int tmpNextNodeID;	//���� �̵��� ��尡 �����ϸ� NOde ID, ������ 0
	int tmpCurrentCMDID;
	int tmpCurrentStationID;
	UINT uLimitNode;
	UINT CMD_StationID;
	unsigned char Safety_Level;
	bool bIgnolOverRun;
//	int tmpNextNodeIDafterResetNode;

	//int tmpStopID;

	// DWORD   dwCIDMonitoringTrigerTime;
	//int     bCountOfCID;
	//int     bCountOfCIDLink;        //��ũ ���� ����
	//int     bCountOfCIDDualOccup;  	//�������� ����
	//int     bCountOfCIDComptACK;  	//�Ϸ�ó�� ���� ����

	unsigned int CIDStopNode1_arr;
	unsigned int CIDStopNode2_arr;
	unsigned int CIDStartNode1_arr;
	unsigned int CIDStartNode2_arr;
	unsigned int CIDResetNode1_arr;
	unsigned int CIDResetNode2_arr;
	unsigned int CIDResetNode_arr;
	unsigned int CIDResetNode_Pre_arr;
	int CIDStopNode1;		//���� ��尡 Stop Node����. Stop node�̸� node ID �� ��.
	int CIDStopNode2;		//���� ��尡 Stop Node����. Stop node�̸� node ID �� ��.
	int CIDStartNode1;
	int CIDStartNode2;
	int CIDResetNode1;      //StopNode_1�� ���� Reset Node_1
	int CIDResetNode2;      //StopNode_2�� ���� Reset Node_2
	int CIDResetNode;       //���� ��尡 Reset Node����. Reset node�̸� node ID �� ��.
	int CIDResetNode_Pre;       //Pre ��尡 Reset Node����. Reset node�̸� node ID �� ��.
	int CIDResetNodeType;	//���� ��尡 Reset Node �� ��� Reset Node Type.
	int CIDResetNodeType_Pre; //Pre ��尡 Reset Node �� ��� Reset Node Type.

	int LayoutType_Stop1;	//Stop1���� ã�� LayoutType
	int LayoutType_Stop2;	//Stop2���� ã�� LayoutType
	int LayoutType_Reset;	//Reset���� ã�� LayoutType

	//L
	int tmpStartNode1_1;
	int tmpStartNode1_2;
	int tmpStartNode1_3;
	int tmpStartNode1_4;
	int tmpStartNode1_5;
	int	tmpStopNode1;    // CID ���� ��ȣ ó���� ���� �߰� (2016.12.2)
	int tmpResetNode1;
	int tmpResetNodeType1;
	int tmpCIDStopNode1;
	int CommStartNode1_Offset;

	//R
	int tmpStartNode2_1;
	int tmpStartNode2_2;
	int tmpStartNode2_3;
	int tmpStartNode2_4;
	int tmpStartNode2_5;
	int	tmpStopNode2;    // CID ���� ��ȣ ó���� ���� �߰� (2016.12.2)
	int tmpResetNode2;
	int tmpResetNodeType2;
	int tmpCIDStopNode2;
	int CommStartNode2_Offset;

	int tmpCurrentResetNode;//�̻��...
	int tmpCurrentResetNodeType;// �� ���� �κ� �ּ� ���� 
	
	// 4�б� ó���� ����
	bool bLoadUnload;	// ������ ����. true : ������ �Ѵ�. false : ������ ���� ���� 
	int  tmpLastCMDID;	// ���� Ÿ��

	// 0:�б� ����, 1:���ʺб�, 2:�����ʺб�
	PATH_DIVERGENCE_INFO NodeDivInfo;	// ���� ����� �б�����
	PATH_DIVERGENCE_INFO NextDivInfo;	// ���� ����� �б�����

} CID_CTRL_INFO;
#pragma pack(pop) // Pack ����



typedef struct
{
	// 4�б� ó���� ����
	bool bLoadUnload;	// ������ ����. true : ������ �Ѵ�. false : ������ ���� ���� 
	int  tmpLastCMDID;	// ���� Ÿ��

	// 0:�б� ����, 1:���ʺб�, 2:�����ʺб�
	PATH_DIVERGENCE_INFO NodeDivInfo;	// ���� ����� �б�����
	PATH_DIVERGENCE_INFO NextDivInfo;	// ���� ����� �б�����
} CID_4WAY_INFO;

// -------------------------------------------
#endif
