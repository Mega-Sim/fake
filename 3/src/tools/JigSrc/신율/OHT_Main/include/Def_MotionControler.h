#ifndef Def_MotionControlerH
#define Def_MotionControlerH

#include <stdio.h>
#include "Def_OHT.h"

// CMD_MOVE�� CmdType define
#define	MMC_CMD_AXIS_NONE				0	///< ��� ����
#define	MMC_CMD_AXIS_RESET				1	///< �� �ʱ�ȭ
#define	MMC_CMD_AXIS_AMP_ENABLE			2	///< Servo On --> �Է� ���� ����
#define	MMC_CMD_AXIS_AMP_DISABLE		3	///< Servo Off --> �Է� ���� ����

#define	MMC_CMD_AXIS_MOVE_P				4
#define	MMC_CMD_AXIS_MOVE_N				5
#define	MMC_CMD_AXIS_MOVE_S				6
#define	MMC_CMD_AXIS_MOVE_DS			7

#define	MMC_CMD_AXIS_MOVE_TIME_P		8
#define	MMC_CMD_AXIS_MOVE_TIME_N		9
#define	MMC_CMD_AXIS_MOVE_TIME_S		10
#define	MMC_CMD_AXIS_MOVE_TIME_DS		11

#define	MMC_CMD_AXIS_SET_HOME			12	///< ���� ��ġ�� 0���� ���� --> �Է� ���� ����
#define	MMC_CMD_AXIS_SET_OFFSET			13	///< Offset �� ���� (����ġ ���ڴ�: �ԷµǴ� ��ġ�� 0���� ����) --> Distance �� �� �Է�

#define	MMC_CMD_AXIS_SET_POS_LIMIT		14	///< Positive SW Limit --> Distance �� �� �Է�
#define	MMC_CMD_AXIS_SET_NEG_LIMIT		15	///< Negative SW Limit --> Distance �� �� �Է�
#define	MMC_CMD_AXIS_RESET_LIMIT		16	///< Reset Limit --> Limit ������� ����(�Ǵ� ���� ū ������ ����) --> �Է� ���� ����

// CMD_ADDITIONAL_INFO�� CmdType define
#define	MMC_CMD_PATH_DIRECTION_NO_USE 	0	// �̻��: ���� ����
#define	MMC_CMD_PATH_DIRECTION_NORMAL	1	// ���� ����: ���� N
#define	MMC_CMD_PATH_DIRECTION_LEFT		2	// ����: ���� L
#define	MMC_CMD_PATH_DIRECTION_RIGHT	3	// ������: ���� R
#define	MMC_CMD_PATH_DIRECTION_SHORT	4	// N�б� ���: ���� S
#define	MMC_CMD_PATH_DIRECTION_BOTH		5	// ª�� S���� �: ���� B
#define	MMC_CMD_PATH_DIRECTION_NARROW	6	// ���� ������ 900mm �����϶��� ª�� S���� �: ���� V

// -------------------------------------------
//1 Parameter
// -------------------------------------------
/**
@brief	 Motion ����⿡ ������ ���� �Ķ����
@author  ���¿�
@date	 2013.05.15
*/
typedef struct _DRIVING_PARAM
{
	//2 ���� �⺻ ������
	volatile int	GearRatioOfDriving;		///< ���� Gear ��(Cts/m) --> 1m �� 1mm �̳��� ����
	
	//2 ��������
	volatile int	OHTDetectAreaDist[6];		///< ���� ���� ���� ������ ���� �Ÿ� (���� Cts)
	volatile int	OHTDetectAreaSpeed[6];	///< ���� ���� ���� ������ ���� �ӵ� (���� Cts/s)
	volatile int	OBSDetectAreaDist[6];		///< ��ֹ� ���� ���� ������ ���� �Ÿ� (���� Cts)
	volatile int	OBSDetectAreaSpeed[6];	///< ��ֹ� ���� ���� ������ ���� �ӵ� (���� Cts/s)
	volatile int	AccelOfNormalPath;		///< �������������� ���� ��
	volatile int	DecelOfNormalPath;		///< �������������� ���� ��
	volatile int	AccelOfCurvePath;		///< ����������� ���� ��
	volatile int	DecelOfCurvePath;		///< ����������� ���� ��
}DRIVING_PARAM;


// -------------------------------------------
//1 ����� (Receive)
// -------------------------------------------
/**
@brief	 Motion ������ ���� �޴� Node ����
@author  ���¿�
@date	 2013.05.15
*/
typedef struct _NODE_CHECK_DATA
{
	volatile int NodeCountOfPreCheck;	///< ��� ����
	volatile int NodeCount;				///< ��� ����
	
	// SetHome, setOffset �� �ϸ� ���� ��ġ�� �� ������ ������
	volatile int PositionOfDrivingTagPreCheck;	///< ���� ��� ���� ������ ��ġ		
	volatile int PositionOfDrivingTag;			///< ���� ��� ������ ��ġ			 
	volatile int PositionOfTransTagFirst;		///< ������ ��� ������ ��ġ(Front)
	volatile int PositionOfTransTagFinal;		///< ������ ��� ������ ��ġ(Rear)
	
}NODE_CHECK_DATA;

// -------------------------------------------
//1 ����� (Send)
// -------------------------------------------
/**
@brief	 Motion ����⿡ ������ �������� ���� data
@author  ���¿�
@date	 2013.05.15
*/
#if(USE_CHANGENODE == 1)
typedef struct _DETECT_INFO
{
	volatile char	UseFollowControlFlag;	///< ���� ���� ��� ����: true �� ���, false�� �̻��

	volatile char	OHTDetectType;				///< Link�� OHTDetectType
	volatile char	OBSDetectType;				///< Link�� OBSDetectType

	volatile int	NodeCount;				///< ��� ����
	volatile int	DistSumOfStraightPath;	///< ���� ���� �̾����� ���������� ��ġ (Cts)
	volatile int	FollowCommandType;		
	volatile int	PBSIgnoreFlag;

	volatile int	Reserved_1;
	volatile int	Reserved_2;
}DETECT_INFO;
#else
typedef struct _DETECT_INFO
{
	volatile char	UseFollowControlFlag;	///< ���� ���� ��� ����: true �� ���, false�� �̻��

	volatile char	Type;				///< Link�� OHTDetectType

	volatile int	NodeCount;				///< ��� ����
	volatile int	DistSumOfStraightPath;	///< ���� ���� �̾����� ���������� ��ġ (Cts)
	volatile int	FollowCommandType;
	volatile int	PBSIgnoreFlag;

}DETECT_INFO;
#endif

typedef struct _PAUSE_INFO
{
	volatile int	Pause;		
	volatile int	Commander;	
}PAUSE_INFO;

// for change node (jhoun.lim)
struct OPTIONAL_NODE_INFO
{
	volatile char NodeType;				//	0x05 : Tag-type Optional Node In, 0x06 : Distance-type Optional Node In, 0x07 : Combo-type Optional Node In
	volatile int DistON;				// Optional Node ������ �Ÿ�
	volatile int DistAfterON;			// Optional Node ���Ŀ� ������ �ٲ�� ��ġ
	volatile char OHTDetectTypeOpt;		// Optional Node ���� ������ Type(����)
	volatile char OBSDetectTypeOpt;		// Optional Node ���� ������ Type(��ֹ�)

	volatile int Reserved1;
	volatile int Reserved2;
};
// -------------------------------------------
// Axis Command Structure --> AMC CMD Structure
// NONE : 0
// MoveP: 1 // MoveN: 2 // MoveS: 3 // MoveDS: 4
// MoveP_T: 5 // MoveN_T: 6 // MoveS_T: 7 // MoveN_T: 8
// Reset: 9 // setHome: 10
typedef struct _CMD_AXIS
{
	volatile char	CmdType;			///< Move (P,N,S,DS - TimeBase, AccelBase)	/ reset / SetHome ��� 
	volatile int	Accel;			///< ���ӵ�(Cts/s2) or �ð�(msec)
	volatile int	Decel;			///< ���ӵ�(Cts/s2) or �ð�(msec)
	volatile int	Speed;			///< �ӵ�(Cts/s)
	volatile int	Distance;		///< Target �Ÿ�(Cts)

	volatile int	TriggerCtrlInfo;	///< Trigger Control �ϴ� ����: �ִ� Target���� ���� �� �̵� ����� 1ms ������ �ΰ� --> Ư�� Ʈ���ſ� ���� ������ TRIGGER_CTRL_INFO ����ü ����
}CMD_AXIS;


// -------------------------------------------
//1 Analyze & Make Data
// -------------------------------------------

// AMC_CMD.Cmd�� bitfield (AMC Command Type Structure)
// ó�� ����� 2������ �ִ�
//  �� �����ؾ� �ϴ� ���� �ִ� ���: ���� ���� �ٸ��� ON
//  �� �����ؾ� �ϴ� ���� ���� ���: ����� ������ ON
typedef struct _CMD_ANALYSIS
{	
	unsigned int SetOutBit					: 1;		///< I/O�� Output ���� : ��
	unsigned int ClearNodeCount				: 1;		///< Node Count Clear : ��
	unsigned int MoveAxis1					: 1;		///< 1�� �̵� ��� : ��
	unsigned int MoveAxis2					: 1;		///< 2�� �̵� ��� : ��
	unsigned int MoveAxis3					: 1;		///< 3�� �̵� ��� : ��
	unsigned int MoveAxis4					: 1;		///< 4�� �̵� ��� : ��
	unsigned int SetDetectInfo				: 1;		///< ���� ���� Type ���� : ��
	unsigned int SetToreqLimit				: 1;		//1	���� ���� /< ���� Gear �� ���� : ��
	unsigned int SetToreqLimitRelease		: 1;		//1	���� ���� /< ���� Limit ���� : ��
	unsigned int CompensateRearEncoder		: 1;		///< Rear���� Front ������ �����ִ� ��� : ��

	unsigned int PauseControl				: 1;		///< Pause���  : ��
	unsigned int SetSlopeTime				: 1;		///< ���� ���� ���� ��� : ��
	unsigned int SetSlopeTimeRelease		: 1;		///< ���� ���� ����  ��� : ��
	unsigned int SetTargetPos				: 1;		///< Ÿ����ġ�� ��ó�ϰ�� ���� ��� : ��
	unsigned int SetTargetPosRelease		: 1;		///< Ÿ����ġ ���� ��� : ��
	unsigned int SetFirstNodeCountAdd		: 1;        ///< ������ ù���� ��� ī��Ʈ 1�� ���� ���
	unsigned int SetVHLDetectIgnor		 	: 1;        ///< �������� ���� ���� ���
	unsigned int SetVHLDetectIgnorRelease	: 1;        ///< �������� ���� ���� ���
	unsigned int SetDetectStopDecelDownRelease		: 1;///< ���� ���� ���ӵ�(0.5m/s^2) ���� ���� ���
	unsigned int SetDetectStopDecelDown	 			: 1;///< ���� ���� ���ӵ�(0.5m/s^2) ���� ���� ���
	unsigned int SetVHLDetectIOSet	 			: 1;///< ���� ���� ���ӵ�(0.5m/s^2) ���� ���� ���

	unsigned int Reserved					: 11;
}CMD_ANALYSIS;

// AMC_CMD.Cmd �� ���� ����� �м��ϱ� ���� union
typedef union _CMD_ANALYSIS_UNION
{
	CMD_ANALYSIS CmdAnalysis;	///< AMC_CMD.Cmd�� bitfield
	int Cmd;
}CMD_ANALYSIS_UNION;


// AMC_CMD_BIT_ANALYSIS
typedef enum _MOTION_CONTROLLER_CMD_TYPE
{
	CMD_TYPE_SET_OUTBIT							= 0,
	CMD_TYPE_CLEAR_NODE_COUNT					,
	CMD_TYPE_MOVE_AXIS1							,
	CMD_TYPE_MOVE_AXIS2							,
	CMD_TYPE_MOVE_AXIS3							,
	CMD_TYPE_MOVE_AXIS4							,
	CMD_TYPE_SET_DETECT_INFO					,
	CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE			,
	CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE	,
	CMD_TYPE_COMPENSATE_REAR_ENCODER			,
	CMD_TYPE_PAUSE_CONTROL                      ,
	CMD_TYPE_SET_SLOPE_TIME						,
	CMD_TYPE_SET_SLOPE_TIME_RELEASE             ,
	CMD_TYPE_SET_TARGET_POS						,
	CMD_TYPE_SET_TARGET_POS_RELEASE             ,
 	CMD_TYPE_SET_FIRSTNODE_ADD		            ,
	CMD_TYPE_SET_VHLDETECTIGNOR		            ,
	CMD_TYPE_SET_VHLDETECTIGNOR_RELEASE         ,
	CMD_TYPE_SET_DETECTSTOPDECELDOWN_RELEASE    ,
	CMD_TYPE_SET_DETECTSTOPDECELDOWN            ,
}MOTION_CONTROLLER_CMD_TYPE;


//2	Trigger Command
typedef struct _TRIGGER_CTRL_INFO
{	
	unsigned int StartOnSpeedDown		: 1;		///<��� �ΰ� ����( 0:�ٷ� ����, 1:���� ���������� ���� ������ ��� �ΰ�
	unsigned int UseCurrSpeed			: 2;		///<����� �ӵ�( 0: ��� ���� �ӵ�, 1: ���� �ӵ�, 2:Min(��ɼӵ�, ����ӵ�), 4:MAX(��ɼӵ�, ����ӵ�) )
	unsigned int UseSmallAdd			: 1;		///<Small Add ( 0: NoUse, 1: Use )
	unsigned int TriggerIONumber		: 8;		///< I/O ��ȣ (0~255): ��
	unsigned int IsTriggerOnRisingEdge	: 1;		///<Triger Type ( 0: Falling Edge, 1: Rising Edge )
	unsigned int AdditionalDist		: 19;	///< 1ȸ �߰� �Ÿ� ( 0~2^19 / ���� ������ ���� Ÿ�� �Ÿ����� ȹ��): 524288cts --> ����: 4599mm
}TRIGGER_CTRL_INFO;


// AMC_CMD.Cmd �� ���� ����� �м��ϱ� ���� union
typedef union _TRIGGER_CTRL_INFO_UNION
{
	TRIGGER_CTRL_INFO TriggerInfo;	///< AMC_CMD.Cmd�� bitfield
	int Cmd;
}TRIGGER_CTRL_INFO_UNION;


//2	VoltageMonitor
typedef struct VOLTAGE_MONITOR_INFO_
{	
	unsigned int Percent_5V	: 8;		///< 5��Ʈ ������ Percent: 0~255%
	unsigned int Percent_M12V	: 8;		///< -12��Ʈ ������ Percent: 0~255%
	unsigned int Percent_P12V	: 8;		///< 12��Ʈ ������ Percent: 0~255%
	unsigned int Percent_3P3V	: 8;		///< 24��Ʈ ������ Percent: 0~255%
}VOLTAGE_MONITOR_INFO;


// AMC_CMD.Cmd �� ���� ����� �м��ϱ� ���� union
//typedef union VOLTAGE_MONITOR_INFO_UNION_
//{
//	VOLTAGE_MONITOR_INFO VoltageInfo;	///< ���� ������ bitfield
//	int Data;
//}VOLTAGE_MONITOR_INFO_UNION;
//


#endif
