/**
@file    EXECUTE_INFO.h
@version OHT 7.0
@brief   ���� ����ü ���� Header File
*/

#ifndef ExecuteInfoH
#define ExecuteInfoH

#include "List.h"
#include "MAP.h"
#include "../Common/Def_Protocol.h"
#include "../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

#include "Def_PathInfo.h"
#include "Def_Variables.h"
#include "Def_DrivingCmdInfo.h"




/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_MODE_
{
	EXECUTE_MODE_MANUAL	= 0,		///< Manual Mode (�ʱ�ȭ ���µ� Manual)
	EXECUTE_MODE_AUTO	= 1		///< Auto Mode
}EXECUTE_MODE;


/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_UNIT_
{
	EXECUTE_UNIT_NONE		= 0,		///< �ʱ�ȭ ����
	EXECUTE_UNIT_DRIVING	= 1,		///< ���� ��� -> Driving Ctrl		A/M
	EXECUTE_UNIT_TRANS		= 2,		///< ������ ��� -> Trans Ctrl		A/M
	EXECUTE_UNIT_MANUAL		= 3		///< Manual ��� -> Manual Ctrl	_/M
}EXECUTE_UNIT;

/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_DRIVING_CMD_TYPE_
{
	EXECUTE_DRIVING_CMD_NONE= 0,		///< �ʱ�ȭ ����
	EXECUTE_DRIVING_CMD_INIT,		///< ���� ��� -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_GO,			///< ���� ��� -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_GOBACK,	   	///< ���� ���� ��� -> Driving Ctrl		A/M
	EXECUTE_DRIVING_CMD_MAPMAKE,	///< Map Make ��� -> Driving Ctrl	_/M
	EXECUTE_DRIVING_CMD_CLEAN,		///< Manual ��� -> Manual Ctrl	A/M
	EXECUTE_DRIVING_CMD_SET_STATION_OFFSET,	///< Station Offset ���� -> Driving Ctrl	_/M
	EXECUTE_DRIVING_CMD_MARK,            ///< Mark ��� -> Driving Ctrl			_/M
	EXECUTE_DRIVING_CMD_FLOOR,            ///< FLOOR �̵� ��� -> Driving Ctrl			_/M
	EXECUTE_DRIVING_CMD_MOVETEST  ///< FLOOR �̵� ��� -> Driving Ctrl			_/M
//	EXECUTE_DRIVING_CMD_MARK = 5            ///< Mark ��� -> Driving Ctrl			_/M
}EXECUTE_DRIVING_CMD_TYPE;


/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_QUICK_CMD_
{
	EXECUTE_QUICK_CMD_NONE			= 0,		///< �ʱ�ȭ ����		A/M
	EXECUTE_QUICK_CMD_STOP			= 1,		///< ��� ����		A/M     ~ OCS_Estop / Status Check ���� Internal Cmd �� E-stop / 
	EXECUTE_QUICK_CMD_PAUSE_OCS		= 2,		///< �Ͻ� ����		A/M     ~ OCS_Pause  
	EXECUTE_QUICK_CMD_RESUME_OCS	= 3,		///< �� ����		A/M     ~ OCS_Resume 	
	EXECUTE_QUICK_CMD_CANCEL		= 4,		///< ���� ��� Clear	A/M     ~ OCS �� ��� ������ ���
	EXECUTE_QUICK_CMD_RESET			= 5,         ///< ���� ��� Clear	A/M     ~ OCS �� ��� ������ ���
	EXECUTE_QUICK_CMD_PAUSE_TP		= 6,		///< �Ͻ� ����		A/M
	EXECUTE_QUICK_CMD_RESUME_TP		= 7,			///< �� ����		A/M
	EXECUTE_QUICK_CMD_TURN_OFF 		= 8,        ///< �⵿ Off
	EXECUTE_QUICK_CMD_TURN_ON       = 9
}EXECUTE_QUICK_CMD;

/**
@brief   Excute Thread �� D,T,M ���� ��ɿ� ���� ����
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_ACK_
{
	EXECUTE_ACK_NO_ERR				= 0,		///< ���� ���� 			A/M
	EXECUTE_ACK_BUSY				= 1,		///< ��� ���� �� ���� ����	A/M
	EXECUTE_ACK_NOT_AVAILABLE_CMD	= 2,		///< ó���� �� ���� ���		A/M
	EXECUTE_ACK_ON_ERR				= 3
}EXECUTE_ACK;

/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_RESPONSE_TYPE_
{
	EXECUTE_RESPONCE_NONE		= 0,		///< �ʱ�ȭ ����					A/M
	EXECUTE_RESPONCE_MAPMAKING	= 1,		///< MapMaking ���� -> Driving Ctrl		A/M
	EXECUTE_RESPONCE_TRANS		= 2		///< ƼĪ ���� -> Trans Ctrl			_/M
}EXECUTE_RESPONCE_TYPE;

/**
@brief   Main Thread�� Excute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum EXECUTE_HIDPATH_TYPE_
{
	EXECUTE_HIDPATH_NONE		= 0,		///< �޴���
	EXECUTE_HIDPATH_LINE		= 1,		///< ����
	EXECUTE_HIDPATH_CURVE		= 2,		///< �
	EXECUTE_HIDPATH_SLOOP		= 3,		///< ���� Auto �����϶��� ǥ����.
}EXECUTE_HIDPATH_TYPE;

/**
@brief   Main Thread�� Execute Thread ���̿� Return �ϴ� Driving�� ���� for TCU
@author  ehoto
@date    2012.11.12
*/
typedef struct _FLAG_MERGE_POINT_BIT_
{
	char isNear			: 1;
	char isSpeedDown	: 1;
	char isStoped		: 1;
	char researved_4	: 1;
	char researved_5	: 1;
	char researved_6	: 1;
	char researved_7	: 1;
	char researved_8	: 1;
}FLAG_MERGE_POINT_BIT;

typedef union _FLAG_MERGE_POINT_
{
	BYTE ByteValue;
	FLAG_MERGE_POINT_BIT BitValue;
}FLAG_MERGE_POINT;

/**
@brief   CYCLELIST�� �Ľ��ϱ����� �ʿ��� ���� ������ ���� ����ü
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned short			CmdID;				// ���� ID

   	unsigned int 			CmdCycle;           //�۾� ����Ŭ

    MARK_TYPE				MarkType;    // Target ������ Tag Type
	unsigned int			StopNodeID;		// ���������
	unsigned int			StopStationID;	// ������ Station���

	unsigned int 			PhysicalID;

//	int						StationData_StartRunDist;	// Node�κ��� ������ Tag���� �Ÿ� : �����̸� 0���� ǥ��
//	unsigned int			StationDataStart_NextNodeID;		// ���� ���

	unsigned int			StationData_ParentNodeID;		// ���� ���
	unsigned int			StationData_NextNodeID;		// ���� ���
	int						StationData_RunDist;	    // Node�κ��� ������ Tag���� �Ÿ� : �����̸� 0���� ǥ��
    int						TeachingData_STBOffset;  //  �߰�

	unsigned int			NextNodeID;		// ���� ���


	BYTE					PathData_Def_Direction;	// �⺻ �б����
	unsigned int			PathData_StartNodeID;	// ���ù���
	unsigned int			PathData_EndNodeID;	// �������
	unsigned short			PathData_ExtraNum;	//	�⺻�� �б� ������

	unsigned int  			PathData_ExtraID[MAX_PATH_SIZE];

	bool					bIsTargetStationoPassPermitOpen;  //������忡 ���� ������ �±��� ��� �ݳ� ���� 20170730


} PARSING_GO_INFO;


/**
@brief   CYCLELIST�� ������ �ʿ��� ���� ������ ���� ����ü
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned int			OriginStartNodeID;	// �θ�����ġ
	unsigned int			OriginEndNodeID;	// �θ�����ġ

	unsigned int			StartNodeID;	// ��� ��尡 �Ϲ� ������ ��� OriginNID�ϰ� ���� : Station�� Next Node
	unsigned int			EndNodeID;		// ������ ��尡 �Ϲ� ������ ��� OriginNID�ϰ� ���� : Station�� Next Node

	bool   	         		IsStationStart;
	bool            		IsStationEnd;

	int			StationData_StartRunDist;	// �θ� Node�κ��� ������ Tag���� �Ÿ� :
	int			StationData_EndRunDist;	    // �θ� Node�κ��� ������ Tag���� �Ÿ� :

	int			StationData_StartRunDistOfNextNode;		// Next�κ��� ������ Tag���� �Ÿ�
	int			StationData_EndRunDistOfNextNode;	    // Next�κ��� ������ Tag���� �Ÿ�

	BYTE					PathDiv;					//�б����� (0 : �ش����, 1 : ���� 2: ������ 3: ����.

  /*
	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;	// �⺻ �б����
	unsigned int			PathData_StartNodeID;	// ���ù���
	unsigned int			PathData_EndNodeID;	// �������
	unsigned short			PathData_ExtraNum;	//	�⺻�� �б� ������

	unsigned int  			PathData_ExtraID[MAX_PATH_SIZE];
  */

} PROCESSING_GO_INFO;


typedef struct MAP_MAKER_TYPE_
{
	BYTE MapMake_DrivingTag			:1;               ///< ���� MapMaer
	BYTE MapMake_StopTag	  		:1;               ///< ���� MapMaer
	BYTE MapMake_QR_Left_Tag 		:1;               ///< ���� MapMaer
	BYTE MapMake_QR_Right_Tag 		:1;               ///< ���� MapMaer
	BYTE MapMake_Reserved			:4;               ///< ���� MapMaer
} MAP_MAKER_TYPE;

/**
@brief	 Main Thread�� Execute Thread ���̿� ���޵� Driving�� Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_DRIVING_ADD_INFO_
{	 
	// ��� Ÿ��
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)

	// ������ �� �߰��Ǵ� ����
	UINT		StationID;						///< Path ���� Node ID
	UINT 		PhysicalID;
	MARK_TYPE	StationType;						///< ������(NONE:0/TAG:1/QRL:2/QRR:3)
	double		DistToStation;					///< ������ �������� �Ÿ�
	double		STBOffset;					///< ������ �������� �Ÿ�
	unsigned char StationSafetyLevel;		///< SafetyLevel
	bool 		  bIgnolOverRun;			///< OverrunLevel

	double		DistOffsetForStartFromStation;	///< ������ ��忡�� ó�� �ʱ�ȭ �Ǹ� �����κ��� �޴°�
	UINT		FinalNode;						///< ��� ���� ���� ��� : ���� TAG �� �ش��.. (Station�� ������ �� ��� Next Node)

	// Map Make�� ���� �߰��Ǵ� ����
	UINT		SpeedLimitOfMapMake;			///< Map Make Speed Limit
	MAP_MAKER_TYPE	MapMakeType;

	UINT		FinalPortType;

	PROCESSING_GO_INFO 	ProcessingGoInfo; 		///< ���࿡ �ʿ��� ������ Data(�������� ��쿡 ���� �Ÿ����� ��� ����)


} EXECUTE_DRIVING_ADD_INFO;

/**
@brief	 ���� ��� ���� (Path)
@author  puting
@date	 2013.11.09
*/
typedef struct DRIVING_CYCLE_ADD_INFO
{
	EXECUTE_DRIVING_CMD_TYPE	CmdType;	///< Go, MapMake, Clean, Mark(?)

	UINT					StartNID;			///< ���� ���
	UINT        			EndNID;				///< �� ���

	NODE_TYPE				StartNType;			///< Path ���� Node Type: �Ϲ�(0), �б�(1), �շ�(2), �շ��б�(3)	: TCU ������� ��� ����
	NODE_TYPE				EndNType;			///< Path ���� Node Type: �Ϲ�(0), �б�(1), �շ�(2), �շ��б�(3)	: TCU ������� ��� ����

	double      			dSpeed;				///< �ӵ�
	double      			dDistSumToEndNode;	///< EndNode ������ �Ÿ� ��

	PATH_STEERING_INFO		Steering;			///< ���� ����
	PATH_DIRECTION_INFO		Direction;			///< ���� ����
	PATH_DIVERGENCE_INFO  	Division;			///< �б� ���� ����/������(����Ʈ����)

	UINT					TargetStationID;

}DRIVING_CYCLE_ADD_INFO;

/**
@brief	 Main Thread�� Execute Thread ���̿� ���޵� Driving�� Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_DRIVING_INFO_
{	 
	//2	���� �Է� �ÿ� ���Ǵ� ����
	UINT 						PathIndexNum;    ///<Path�� Index Num
	UINT						CmdID;	 	  ///< ���� ID
	unsigned short				SQNum;					// ������ �ѹ�

	//��� ������ �ϱ� ���� ����.
//	BYTE						StartNodeType;    //0: �Ϲ�, 1 ; �б�, ///2: ����Ŭ�� ���� or ��  3: ����Ŭ�� ���� or �� && �б�
//	BYTE						EndNodeType;      //0: �Ϲ�, 1 ; �б�, ////2: ����Ŭ�� ���� or ��  3: ����Ŭ�� ���� or �� && �б�
	double						ChangeEndNodeDistance;     //����������� ������ ��ġ���� �Ÿ���.

	DRIVING_CMD_INFO_OF_PATH	BasicInfo;
	EXECUTE_DRIVING_ADD_INFO	AddInfo; ///< ���� Map Search �̿ܿ� �ʿ��� ����

	// ���� ���� �� ����ġ Ȯ�� �� 
	double				RealPositionOfStop[2];	///< ���� ���� ���� ���� ���ڴ� ��(�������� ���� ��ġ ��)
 //	bool 				bStartNode;						//���� ������� ����߿� �����Ѱ��� Ȯ��.

} EXECUTE_DRIVING_INFO;

/**
@brief	 Main Thread�� Execute Thread ���̿� ���޵� Driving�� Cmd List
@author  puting
@date	 2015.11.09
*/
typedef struct CYCLEDRIVING_PASSPERMIT_INFO_
{
	//2	���� �Է� �ÿ� ���Ǵ� ����
//	bool									CheckCurLinkPPStartPoint;	//����㰡 ���� ��û ����
//	bool									CheckPreLinkPPOpenStartPoint;
//	bool									CheckCurLinkPPEndPoint;		//����㰡 ���� ��û ����
//	bool									CheckCurLinkPPOpenEndPoint;		//����㰡 ���� ��û ����
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///�ٸ����� ����
	bool									IsCurLinkPPPoint;			//���� Link�� ����㰡 ��û����
	bool									IsCurLinkPPOpenPoint;       //���� Link�� ����㰡 ���� ��û ����
//	bool									IsNextLinkPPPoint;			//���� Link�� ����㰡 ���� ��û ����

//	bool									IsPreLinkPPOpenPoint;		//���� Link�� ����㰡 ���� ����( ���۽ÿ� �������� Ȯ����. �������� ��� true)
	bool									IsTargetStationPPOpenPoint;		//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)


}CYCLEDRIVING_PASSPERMIT_INFO;

/**
@brief	 �۾������� ���� �б������� ���� ������
@author  puting
@date	 2015.11.09
*/
typedef struct CYCLEDRIVING_CHANGE_INFO_
{
	UINT									ChangeNodePoint;				// ���� Point Node
	BYTE									ChangeNodeType;					// �б� :1 , ����Ŭ ���۰� �� :2, �б� &&����Ŭ ���۰� �� : 3
	double									ChangeNodeDistance;       		//���� Point���� �Ÿ���
}CYCLEDRIVING_CHANGE_INFO;


/**
@brief	 Main Thread�� Execute Thread ���̿� ���޵� Driving�� Cmd List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_CYCLEDRIVING_INFO_
{
	//2	���� �Է� �ÿ� ���Ǵ� ����
	CYCLESTEP								D_PPStep;		   	 	///< ����㰡 ���õ� Step
	NEXTCYCLESTEP							D_PPOStep;		   	 	///< ����㰡 ���õ� Next Stop Step
	CYCLENODEINFO							CycleNodeInfo; 		///<����~ �������, ����~ ����, ����~ ������, ����~ ������

	CYCLEDRIVING_PASSPERMIT_INFO		 	PassPermitInfo;    	///<���۽�/��������  ����㰡 ���� ��û ���� Ȯ���ϴ� �κ�

	UINT								    StartPoint;   		///< ���۳�� ��ġ
	double								    StartDistance;   	///< ��ߺ��� ����Node���� �Ÿ�
	double								    StopDistance;   	///< ��ߺ��� ����Node���� �Ÿ�
	PASSPERMIT_NODE_TYPE				    StartNodeType;   	///< ���� ���� ����/����/������ ���� Ȯ��

	UINT									EndPoint;     		///< �� ��� ��ġ(���� ���)
	UINT									FinalPoint;     		///< �� ��� ��ġ(���� ���)
	double									EndDistance;      	///< ��ߺ��� �� Node���� �Ÿ�
	PASSPERMIT_NODE_TYPE					EndNodeType;     	///< �� ���� �Ϲ�/����/����/������ ���� Ȯ��

	UINT								    ResetPoint;   		///< Reset��� ��ġ
    double									ResetNodeDistance;  ///< ��ߺ��� �ش� ����㰡������ Reset Node���� �Ÿ�(���� ��� EndDistance�� ������ ��.

	PASSPERMIT_COMMON						CurLinkPassPermitData;	    ///< ����㰡 ����. ���������� ����

	UINT									DivCount; 			 	//�������Ŭ �������� �б� ����
//	CYCLEDRIVING_CHANGE_INFO		  		DivInfo[10];	 	   //�������Ŭ �������� �б� ���� (Max 10�� ����, ���� ��ȹ���� ���� �ʿ�.)


	DRIVING_CYCLE_ADD_INFO		AddInfo; ///< ���� Map Search �̿ܿ� �ʿ��� ����

	// ���� ���� �� ����ġ Ȯ�� ��


} EXECUTE_CYCLEDRIVING_INFO;


 /**
@brief   CYCLELIST�� �Ľ��ϱ����� �ʿ��� ������ ������ ���� ����ü
@author  puting
@date    2015.11.06
*/
typedef struct
{
	unsigned short			CmdID;				// ���� ID

	unsigned int 			CmdCycle;
	unsigned int			TrasType;			//�۾� ����

    unsigned char			AutoRecovery;	    // E84 Time Out �߻� ����.
//	WORKCARRYTYPE			WorkCarryType;		//�ݼ۹� Type
	unsigned char			StationData_PortID[12];    // Target ������ Tag Type

	TRANS_CMD               TransCmd;

} PARSING_TRANS_INFO;

/**
@brief   Main Thread�� Execute Thread ���̿� ���޵� Trans�� Cmd List
@author  zzang9un
@date    2013.08.15
*/
typedef struct EXECUTE_TRANS_INFO_
{
    uTAG_DATA   CmdTag;         ///< Command Tag
    BYTE        TransType;      ///< Trans Type (L, U...)

    TRANS_CMD   TransCmd;       ///< ������ Command ����ü

    /* // [BEGIN] by zzang9un 2013.08.15 : �ߺ��� ������ �ϳ��� ����ü�� ������
    // by zzang9un 2012.11.19 : TransType�� CmdTag���� �� �� �����Ƿ� �Ʒ� ���� �ߺ���
    BYTE        TransType;      ///< Trans Type (L, U...)
    UINT        TransTagID;     ///< Trans Tag ID
    UINT        PortType;       ///< EQ Type(EQ, STB, UTB, FOUP LOADER...)
    UINT        STBDir;         ///< STB Direction(None, Left, Right)
    int         Hoist;          ///< Hoist Teaching ��(mm)
    int         Shift;          ///< Shift Teaching ��(mm)
    int         Rotate;         ///< Rotate Teaching ��(radian)
    BYTE        LookDown;       ///< LookDown Sensor On/Off
    BYTE        TransSound;     ///< ������ ���� On/Off
    BYTE        Oscillation;        ///< Oscillation Sensor On/Off
    UINT        HoistLevel;     ///< Hoist �� �ӵ� ���� ����
    UINT        SlideLevel;     ///< Shift �� �ӵ� ���� ����
    UINT        PIODirection;       ///< ����ϴ� EQ PIO(Left, Right)
    UINT        PIOTimeLevel;   ///< ��Ʈ �� EQ PIO Timeover ����
    BYTE        HandDetectEQ;   ///< �����̾ Stokeró�� ������ �� OHT �ڵ带 �����ϴ� ���� ��Ʈ
    BYTE        PIOAutoRecovery;    ///< PIO ���� ������ �߻����� �� Auto Recovery�� ���� ���� ����
    */ // [END] by zzang9un 2013.08.15 : �ߺ��� ������ �ϳ��� ����ü�� ������
} EXECUTE_TRANS_INFO;

/**
@brief   Main Thread�� Execute Thread ���̿� ���޵� Manual Control�� Cmd List
@author  zzang9un
@date    2013.08.17
*/
typedef struct EXECUTE_MANUALCONTROL_INFO_
{
	// Command Info
	uTAG_DATA uCmdTag; ///< Command Tag ����

	uTP_CMD	uTPCmd;
} EXECUTE_MANUALCONTROL_INFO;

typedef struct NODE_OFFSET_INFO_
{
	// ���� ���� ��� ���� 
	UINT			StartNodeID;		///< Path ���� Node ID
	UINT			EndNodeID;		///< Path ���� Node ID
	UINT			Dist;			///< ���� Node�� ������ ���� Path �Ÿ�(mm)
}NODE_OFFSET_INFO;

typedef struct NODE_DATA_INFO_
{
	UINT NID;               ///< Node ID
	UINT Point_X;           ///< X��ǥ
	UINT Point_Y;           ///< Y��ǥ
} NODE_DATA_INFO;


/**
@brief	 Main Thread�� Execute Thread ���̿� Return �ϴ� Driving�� MapMaking Result List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_RESPONSE_MAPMAKING_INFO_
{
	// Command ID
	UINT				CmdID;				///< ���信 ���̴� ID

	// ������ ����
	UINT				StationID;			///< Station ID
	MARK_TYPE		StationType;			///< Station Type (NONE:0/TAG:1/QRL:2/QRR:3)
	NODE_OFFSET_INFO	NodeOffsetInfo[2];	///< ���� Node �� Offset ����
	NODE_DATA_INFO		NodeDataInfo;		///< Node ��ǥ

	//AOHC-369 STB Ȯ���� ������ ���� Map Make ��� ��
	//Virtual Node ����
	bool			bUseVirtualNode;	//Virtual Node ��� ����
	UINT			nOptionalDistance;	//Optional Distance ������ �ʿ��� ���
} EXECUTE_RESPONSE_MAPMAKING_INFO;


/**
@brief	 Main Thread�� Execute Thread ���̿� Return �ϴ� Driving�� MapMaking Result List
@author  ehoto
@date	 2012.11.12
*/
typedef struct EXECUTE_POSITION_INFO_
{
	// ���� ��ġ ����
	UINT	CurrNode;				///< ���� ��� ID
	UINT	CurrPreCheckNode;	 	///< ���� ��� Precheck Sensor ID
	UINT	NextNode;				///< ���� ��� ID
	int		CurrNodeOffset;			///< �Ҽ��� ����(mm)
	UINT	CurrStation;				///< ������ ��� ��ũ �Ŀ��� ������Ʈ

	// ��� ��ġ ����
	UINT	CurrCommandedNode;		///< ������ ���� ���� ���� ��� ID (���� �ʿ�) - OCS Reroute �� ���� �ڷ� ���� ����
	UINT	CurrCommandedStation;	///< Target ������ Node ID

//	double  dCompensatedPosition;	///< ������ ������ ���� ��ġ( ���ڴ��� - ���� �Ÿ� ������ + ��� Station Offset )

	// ����ġ ����
	MARK_TYPE	StationType;		///< Station Type (NONE:0/TAG:1/QRL:2/QRR:3)
	bool			Marked;			///< ��ŷ �Ϸ� ���� ����

} EXECUTE_POSITION_INFO;        ///���� ���� ������  ------- puting



/**
@brief   PKT_CYCLECMDDATA ������ ���� ����ü
@author  puting
@date    2015.10.31
*/
typedef struct
{


	/*
			���� ����
			 1. Cycle Type  : D/T/M
			 2. ���� ID (Manaul�� ����)
			 3. ������ Number(�޴����ϰ� ������� �����)
			 4. DTM Excuteinfo�� �Ѱ��� ����
			 5. DTM Step check
			   >>������� ���
			   1) ����㰡 ���� ��
			   2) ����㰡 ���� ���� ��(����㰡 �ݳ� ��, 1)�� ����)
			   3) ����㰡 ���� ���� ��(Isexcuteinfo�� ���� Ȯ�εǸ� ���� CMD POP)  -----> �ٸ����ۿ� �ӽ� ����
			   >>������ ���� ���(Isexcuteinfo�� ���� Ȯ�εǸ� ������ CMD POP)
			   //11) ������ �㰡 ��
			   //12) ������ �㰡 ��(Isexcuteinfo�� ���� Ȯ�εǸ� ������ CMD POP)
			   >>�޴��� ���� ���(Isexcuteinfo�� ���� Ȯ�εǸ�  �޴��� CMD POP)
			   //21) Reserved


			 //������ �ʿ��� Data(Excute Info�� �ٷ� �Ѱ��� ����)
			 1. ������� Data
			   - Driving �����Ͽ� Pass Search���Ͽ� �ٷ� ���� ���� �� ���� �� �������� ���� �� ��.
			 2. Trans ���� Data
			   - MTL/�����̵�/ �ø�/ ���� ���ǿ� ���� �з��Ͽ� ���� �� ��.
			 3. Manual ���� Data
			   -Manual �⺻ Data

		*/
	//EXECUTE_MODE							Mode; //


	///> Common <///
	CYCLETYPE								CycleType; 	  ///< D/T/M
	UINT									CmdID;	 	  ///< ���� ID
	bool									IsExcuteInfo; ///< ExcuteInfo�� ���� ���� Ȯ��
	bool									IsNoExcuteCycle; ///< ����Ŭ �������� (��������Ŭ�� ��� ��)
	bool									IsListF2B; 	  ///< List Front ->back ���� Ȯ��
	bool									Isreturn;      ///< �۾����濡 ���� ����㰡 �ݳ� ����



	///> Driving <///
	EXECUTE_CYCLEDRIVING_INFO			   	EXE_CycleDriving_Info;		///< CycleList�� ���� Ȯ���� Execute Thread�� ������ ���޵Ǵ� �κ�

	///> Trans <///
	EXECUTE_TRANS_INFO						EXE_Trans_Info;			///< Execute Thread�� ������ ���޵Ǵ� �κ�

	///> Manaul <///
	EXECUTE_MANUALCONTROL_INFO				EXE_Maual_Info;			///< Execute Thread�� ������ ���޵Ǵ� �κ�

} PKT_CYCLECMDDATA;


typedef struct _DATA_PASSOPEN
{
	UINT													PassOpenIndex;	 	  ///< ���� ID
    PASSPERMIT_COMMON										PassOpneInfo;		  ///<
	BYTE													bPassOpenType;		  /// 0x01 �Ϲ� �ݳ�, 0x02 ������� �ݳ�. 0x03 : �۾����濡 ���� �ݳ�

	PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247	CmdID247Info;   ///��������㰡 �䱸 ����
} DATA_PASSOPEN;

typedef struct _DATA_PASS
{

	PASSPERMIT_COMMON										PassInfo;		  ///<
	BYTE													bCIDType;		  /// 0x00 CId �Ϲ� ����, 0x01 CID ����

} DATA_PASS;




/**
@brief	 Main Thread�� Excute Thread�� ���� �� Cmd Id�� �����ϱ� ���� ����ϴ� ����ü( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )
@author  ���¿�
@date	 2013.04.03
*/
typedef struct STATUS_CMD_ID_
{
	UINT	Status	:8;								///< I, A, G, L, O, U, N ...
	UINT	PreCmd	:8;
	UINT	CurrCmd :8;
	UINT	NextCmd :8;
} STATUS_CMD_ID;

/**
@brief	 Main Thread�� Excute Thread�� ���� �� Cmd Id�� �����ϱ� ���� ����ϴ� Union( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )
@author  ���¿�
@date	 2013.04.03
*/
typedef union STATUS_CMD_ID_UNION_
{
	STATUS_CMD_ID	Info;
	UINT			Data;

} STATUS_CMD_ID_UNION;

/**
@brief	 Main Thread�� Excute Thread�� ����� �ΰ��� �� ����ϴ� ����ü ( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )

	1) FlagMainThreadUsing �� true�� ����
	2) ��� ����Ʈ�� ���� �� �̻� ������ CmdID �� ����
	3) FlagMainThreadUsing �� false�� ����	
@author  ���¿�
@date	 2013.04.03
*/
typedef struct CMD_ON_ADDING_
{
	UINT	FlagMainThreadUsing	:1;	///< MainThread�� Cmd �߰� ���� �ƴ� ��츸  Excute Thread �� Cmd�� ������ ����Ѵ�
	UINT	CmdID				:4;	///< OCS�� ���� �޴� ��� ID
	UINT	Reserved			:27;
} CMD_ON_ADDING;

/**
@brief	 Main Thread�� Excute Thread�� ����� �ΰ��� �� ����ϴ� Union ( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )
@author  ���¿�
@date	 2013.04.03
*/
typedef union CMD_ON_ADDING_UNION_
{
	CMD_ON_ADDING	Info;
	UINT			Data;
} CMD_ON_ADDING_UNION;

/**
@brief	 Excute Thread�� Main Thread�� Map Make ����� Return�� �� ����ϴ� ����ü ( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )

	1) FlagMainThreadUsing �� true�� ����
	2) ��� ����Ʈ�� ���� �� �̻� ������ CmdID �� ����
	3) FlagMainThreadUsing �� false�� ����	
@author  ���¿�
@date	 2013.04.03
*/
typedef struct MAP_INFO_ON_ADDING_
{
	UINT	FlagOnMapMaking			:1;	///< MApMaking ������ �˷��ִ� Flag
	UINT	FlagExecuteThreadUsing	:1;	///< ExecuteThread�� Result �߰� ���� �ƴ� ��츸  Main Thread �� Result�� ������ ����Ѵ�
	UINT	CountOfResult			:30;	///< ��ô�� ���(MapMake, Teaching) : 0~2,147,483,647(2^31) ��
} MAP_INFO_ON_ADDING;

/**
@brief	 Main Thread�� Excute Thread�� ����� �ΰ��� �� ����ϴ� Union ( sink�� ���߱� ���� Union�� Data ������ �����Ͽ� ��� )
@author  ���¿�
@date	 2013.04.03
*/
typedef union MAP_INFO_ON_ADDING_UNION_
{
	MAP_INFO_ON_ADDING	Info;
	UINT				Data;
} MAP_INFO_ON_ADDING_UNION;

/**
@brief	ȣ��� ������ ��ġ�� ���߱� ���� TransUnit�� ȣ���� ���������� ����
@author  ���¿�
@date	 2013.04.03
*/
typedef struct TRANSE_DIFFERENCE_
{
	volatile double dHoist;	///< Rail ���� Jig �ٴڸ������ ����
	volatile double dShift;	///< MTL ���� Shift ���������� ����
	volatile int dRotate; ///< Ratate �������� ���ͱ����� ����
	volatile int dRotate_90; ///< Ratate �������� ���ͱ����� ����
	volatile int dRotate_180; ///< Ratate �������� ���ͱ����� ����
} TRANSE_DIFFERENCE;

/**
@brief	���� �Ÿ� ��
@author  ���¿�
@date	 2014.06.20
*/
typedef struct ACCUMULATION_DATA_
{
	volatile double dDrivingEncoder;	///< ����� ���� �Ÿ���
	volatile double dHoistEncoder;	    ///< ȣ�̽�Ʈ�� ���� �Ÿ���
	volatile double dShiftEncoder;	    ///< ����Ʈ�� ���� �Ÿ���
} ACCUMULATION_DATA;

/**
@brief	�ʱ�ȭ ��ġ ��
@author  ���¿�
@date	 2015.11.16
*/
typedef struct FLAG_INIT_POSITION_
{
	volatile bool   bInitFlag;
	volatile double dDrivingInitPosition;	///< ����� ���� �Ÿ���
} FLAG_INIT_POSITION;

/**
@brief	Overrun ����
@author  ���¿� puting
@date	 2016.05.27
*/
typedef struct FLAG_OVERRUN_INFO_
{
	volatile bool   bCheck; //������ �߻� ����.
	volatile BYTE   OverrunType; //������ ���� : ���� 1, ������ 2
	volatile unsigned int nFinalNode;	///< ������ �߻� ���
	volatile double dDrivingInitPosition;	///< ������ �߻����κ��� ������ �Ÿ�.

} FLAG_OVERRUN_INFO;


/**
@brief	CID ����
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_INFO_
{
	volatile bool   bCIDErrorFlag;//	OCS�� �̻󺸰� �ϱ� ���� Flag
	volatile char   cCIDStateResult;
	volatile char   sCIDStatus[3];

	volatile bool   bMoveRestart;
	volatile char   cMoveResult;
} FLAG_CID_INFO;

/**
@brief	CID ����
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_REPORT_
{
	volatile bool   bCIDInterlock;	//	OCS�� CID ���� ����  �ϱ� ���� Flag
	volatile bool   bCIDStop;		//	OCS�� CID ���� ����  �ϱ� ���� Flag
} FLAG_CID_REPORT;


/**
@brief	CID ����
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_MONITORING_
{
	volatile bool   bCIDFlag;
	volatile int    nStopNodeID;
	volatile int    nResetNodeID;
	volatile char   cResult;
	volatile char   cMonitoring[53];	// CID_Monitoring_PKT, CID_Monitoring_BODY size check
} FLAG_CID_MONITORING;

/**
@brief	CID ����
@author
@date	 2015.11.16
*/
typedef struct FLAG_CID_NOT_USE_
{
	volatile bool   bCIDUse;
	volatile int    nStopNodeID;
	volatile int    nResetNodeID;
} FLAG_CID_NOT_USE;

/**
@brief	�������� ���� ����
@author  ���¿�
@date	 201612.28
*/
typedef struct FLAG_PRETRANS_
{
	volatile bool  bPreShutter; 		///< ����Ŭ ���
	volatile bool  bPrePIO; 		 	///< ����Ŭ ���
	volatile int   nTargetID;      	 	///< ���� ��ġ
	volatile int   nPIOID;      	 	///< PIO ID
	volatile int   nPIOChannel;         ///< PIO ä��
//	volatile char  cPIOType;		    ///< IR, RF, H-PIO
	volatile char  cPortType;		    ///< EQ, EQ_X, STOR_ STB, UTB ��
	volatile int   nPIOCS;
	volatile int   nPIOType;
} FLAG_PRETRANS;




/**
@brief	�ʱ�ȭ ��ġ ��
@author  ���¿�
@date	 2015.11.16
*/
typedef struct FLAG_MTL_INFO_
{
	volatile int   nTakeOut; // ����
	volatile int   nTakeIn;	 ///< ����
	volatile int   nRailNum; //���� ��ȣ
	volatile int   nTakeInOrder; //����
	volatile bool  bTakeoutCycle; //��������Ŭ ����
	volatile bool  bTakeInCycle; //������ ����
	volatile bool  bTakeoutReport; //�������� �÷���
} FLAG_MTL_INFO;

/**
@brief	�ʱ�ȭ ��ġ ��
@author  ���¿�
@date	 2015.11.16
*/
typedef struct FLAG_PATHCHANGE_
{
	volatile bool  bPathChange; //��� ���� ���� Ȯ�� �÷���
	volatile int   nChageNodeID; // ��� ���� ��� ��ġ
	volatile char  cResult;		 ///< ��� ���� ���. //0 �ʱ�ȭ, 1 ����  2 ����


} FLAG_PATHCHANGE;

/**
@brief	���� ������ ��� ���� ���� Flag
@author  puting
@date	 2018.09.06
*/
typedef struct FLAG_CMD_EXCUTE_
{
	volatile bool  bCmdTransCMDExcute; ///<������ ��� ���� ���� Flag
	volatile bool  bCmdDrivingCMDExcute; ///<���� ��� ���� ���� Flag
  //	volatile bool  bCmdManualCMDExcute; ///<�޴��� ��� ���� ���� Flag

} FLAG_CMD_EXCUTE;


/**
@brief	 Main Thread�� Excute Thread�� ����� ���� ����ü
@author  zzang9un
@date	 2012.10.18
*/
typedef struct EXECUTE_INFO_
{
	//1 1. MainControlThread to Execute Thread -------------------------------------
	volatile	EXECUTE_MODE		ExecuteMode;	///< M,A
	volatile	EXECUTE_UNIT		ExecuteUnit;	///< D,T,M,...+ MapMake, Mark

	LList							ListOfCmd;		///< void* type CMD List
	volatile	CMD_ON_ADDING_UNION	OnAddingCmd;	///< [Flag+Info] MainThread�� Cmd �߰� ������ ��Ÿ���� ��ǥ

	volatile	EXECUTE_QUICK_CMD	QuickCmd;		///< ���� ���� ��� ���� ��� ���

    volatile    int QuickCmdEStopCommander;
    volatile    int QuickCmdEStopType;	
	    
	//1 2. ExecuteThread to MainControlThread --------------------------------------
    volatile	STATUS_CMD_ID_UNION 		StatusCmdId;			///< ���� ���� ����(I, A, G, L, O, U, N ...) �� Cmd ID
	volatile	int							ErrCode_ExecuteInfo;	///< ExecuteInfo�� ���� ����
 	
	EXECUTE_POSITION_INFO					ExecutePositionInfo;	///< ���� ��ġ ����
	volatile	bool				   		bPopListOfCmd;          ///< List�� ������ �������� Ȯ���ϴ� ����
	
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	ListOfMapInfo;			///< void* type ���� List : MapMake �� ��� / (���� �ʿ�)
	volatile	MAP_INFO_ON_ADDING_UNION	OnAddingMapInfo;		///< [Flag+Info] Execute Thread�� Return Value �߰� ������ ��Ÿ���� ��ǥ
	volatile	EXECUTE_RESPONCE_TYPE		ResponceType;			///< Execute���� MainThread���� ���� Type (���� �ʿ�)

	volatile	TRANSE_DIFFERENCE			TransDifference;		///< ȣ��� ������ ��ġ�� ���߱� ���� TransUnit�� ȣ���� ���������� ����

	volatile	ACCUMULATION_DATA	   		AccumulationData; 	    ///< ���� �Ÿ���.
	//�̻��	FLAG_MERGE_POINT		 	FlagMergePoint; 	///< �շ� ���� ���� ǥ�� Flag : Bit ���� ó�� (���� �ʿ�)
	volatile	FLAG_OVERRUN_INFO			FlagOverRunInfo;	///< ������ �߻����� Ȯ��
	volatile	FLAG_INIT_POSITION	  		FlagInitPositon;
	volatile    FLAG_CID_INFO				FlagCIDInfo;
	volatile    FLAG_CID_NOT_USE			FlagCIDNotuse;
	volatile    UINT						nNextNodeIDInfo;    ///< CID Link ���� Ȯ���� ���� ����ϴ� �������� 1) �ʱ�ȭ ����, �۾����� �Ǿ��� ���, ���ο� �۾����ø� �޾��� ���
																											   //2) ���� ����  ���ο� ����� �������� �Ҵ�Ǿ��� ���(���������� ������� 0���� ó��), ���� or �ݳ��� ��ġ ����(��λ����Ѹ����� ����� �������), �۾����濡���� ��ɻ����� ù���� ����� ���� �������
	volatile    FLAG_MTL_INFO				FlagMTLInfo;
	volatile    FLAG_PATHCHANGE				FlagPathChange;
	volatile    bool						bChangeResult;
	volatile    FLAG_CID_MONITORING			FlagCIDMonitoring;
	volatile    FLAG_PRETRANS				FlagPreTrans;
	volatile    double  					dCompensatedPosition;	///< ������ ������ ���� ��ġ( ���ڴ��� - ���� �Ÿ� ������ + ��� Station Offset )
	volatile	bool						FlagRunningPositon;
//	volatile	bool						FlagPassPermit;
	volatile	UINT						ExecuteDrivingStep;
//	volatile	bool	  	            	FlagCIDOOccupReturnUBG;
	volatile    PATH_DIVERGENCE_INFO 		nCheckDivInfo;			//�б�����  0, 1
	volatile	bool				   		bSpeedDownFlag;

	volatile    EXECUTE_HIDPATH_TYPE		nHIDPathInfo;			//�б����� , 0 �޴��� 1 ���� 2 �

	volatile    int  						nFinishOfNode;	///< ������ ������ ���� ��ġ( ���ڴ��� - ���� �Ÿ� ������ + ��� Station Offset )
	volatile    int  						nFinishOfStation;	///< ������ ������ ���� ��ġ( ���ڴ��� - ���� �Ÿ� ������ + ��� Station Offset )

    volatile    bool                        bOcsPause;              // OCS Pause ����
    volatile    bool                        bTpPause;               // TP Pause ����
	volatile	bool				   		bJogInterlockIgnore;
	volatile    FLAG_CID_REPORT             FlagCIDReport;
    volatile    FLAG_CMD_EXCUTE				FlagCmdExcute;

	//1 00. �ʱ�ȭ
	EXECUTE_INFO_()
	{
		//2	1. MainControlThread to Execute Thread -------------------------------------
		ExecuteMode			= EXECUTE_MODE_MANUAL;
		ExecuteUnit			= EXECUTE_UNIT_NONE;
		QuickCmd				= EXECUTE_QUICK_CMD_NONE;
		OnAddingCmd.Data		= 0;
		bPopListOfCmd = false;

		//2	2. ExecuteThread to MainControlThread --------------------------------------
		StatusCmdId.Data		= 0;
		StatusCmdId.Info.Status	= STS_INIT;
		ErrCode_ExecuteInfo		= 0x00;

		memset(&ExecutePositionInfo, 0, sizeof(EXECUTE_POSITION_INFO));
		
		OnAddingMapInfo.Data	= 0;
		ResponceType			= EXECUTE_RESPONCE_NONE;

		TransDifference.dHoist	= 320; // Rail 
		TransDifference.dShift	= 0; // Rail

		AccumulationData.dDrivingEncoder = 0.0;
		AccumulationData.dHoistEncoder = 0.0;
		AccumulationData.dShiftEncoder = 0.0;

		FlagInitPositon.bInitFlag    = false;
		FlagInitPositon.dDrivingInitPosition = 0.0;

		FlagOverRunInfo.bCheck = false;
		FlagOverRunInfo.OverrunType = 0x00;
		FlagOverRunInfo.nFinalNode = 0;
		FlagOverRunInfo.dDrivingInitPosition = 0.0;


		FlagCIDInfo.bCIDErrorFlag = false;
		FlagCIDInfo.bMoveRestart = false;

		FlagCIDMonitoring.bCIDFlag =false;
		FlagCIDMonitoring.nStopNodeID =0;
		FlagCIDMonitoring.nResetNodeID =0;
		FlagCIDMonitoring.cResult =0x00;

		FlagPathChange.bPathChange =false; //��� ���� ���� Ȯ�� �÷���
		FlagPathChange.nChageNodeID =0; // ��� ���� ��� ��ġ
		FlagPathChange.cResult= 0x00;		 ///< ��� ���� ���. //0 �ʱ�ȭ, 1 ����, 2 ����
		bChangeResult = false;

		FlagCIDNotuse.bCIDUse =false;
		FlagCIDNotuse.nStopNodeID =0;
		FlagCIDNotuse.nResetNodeID =0;

		FlagPreTrans.bPreShutter = false;
		FlagPreTrans.bPrePIO = false;
		FlagPreTrans.nPIOID = 0;
		FlagPreTrans.nPIOChannel = 0;
		FlagPreTrans.nTargetID = 0;
//		FlagPreTrans.cPIOType = 0x02;   //H-PIO
		FlagPreTrans.cPortType = 0x00;


		FlagMTLInfo.nTakeOut =INIT;
		FlagMTLInfo.nTakeIn =INIT;
		FlagMTLInfo.nRailNum =INIT;
		FlagMTLInfo.nTakeInOrder=INIT;
		FlagMTLInfo.bTakeoutCycle = false;
		FlagMTLInfo.bTakeoutReport = false;
		FlagMTLInfo.bTakeInCycle = false;

		dCompensatedPosition = 0.0;
		nCheckDivInfo = PATH_DIVERGENCE_NONE;

		nFinishOfNode = 0;
        nFinishOfStation = 0;

		nHIDPathInfo = EXECUTE_HIDPATH_NONE;

		FlagRunningPositon = false;
		bSpeedDownFlag = false;

		bJogInterlockIgnore		= true;
        ExecuteDrivingStep = 0; //DRIVING_CTRL_STEP_NONE
//		FlagPassPermit = true;
		nNextNodeIDInfo = 0;

        bOcsPause = false;
		bTpPause = false;

		FlagCIDReport.bCIDInterlock = false;
		FlagCIDReport.bCIDStop = false;

		FlagCmdExcute.bCmdTransCMDExcute = false;
		FlagCmdExcute.bCmdDrivingCMDExcute = false;
//		FlagCIDOOccupReturnUBG = false; //CID �����ݳ� By UBG
		//�̻��		memset(&FlagMeargePoint, 0, sizeof(FLAG_MERGE_POINT));
	}
		
} EXECUTE_INFO;


#endif	// ExecuteInfoH

