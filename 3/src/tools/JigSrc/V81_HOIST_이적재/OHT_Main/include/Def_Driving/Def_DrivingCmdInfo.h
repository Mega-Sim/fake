#ifndef Def_DrivingCmdInfoH
#define Def_DrivingCmdInfoH

#include <stdio.h>
#include "Def_Variables.h"	// BYTE, WORD, UINT, UINT64
#include "Def_PathInfo.h"	// NODE_TYPE, PATH_STEERING_INFO, PATH_DIRECTION_INFO, MARK_TYPE

/**
@brief	 ���� ��� ���� (Node)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_NODE_
{
	UINT		ID;					///< NodeID
	NODE_TYPE	Type;				///< Path ���� Node Type: �Ϲ�(0), �б�(1), �շ�(2), �շ��б�(3)
	double		dDistSum;	///< �������� �Ÿ�
	int			nCount;				///< ���ۺ��� ��������� Node ����( 0���� ���� )
//	PATH_DIVERGENCE_INFO    DivInfo;			///< �б� ����
//	SLOPE_NODE_INFO			SlopeInfo;
    DEVICE_NODE_TYPE		NodeType;
}DRIVING_CMD_INFO_OF_NODE;

/**
@brief	 ���� ��� ���� (Steer)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_STEER_
{
	PATH_STEERING_INFO	Type;				///< ���� ����
	PATH_STEERING_INFO  NextType;			///< ������ ����:�� �κп� ��尡 ���� �����Ǹ� ���� ����
	double				dDistSumToEndNode;	///< ���� �������� �̵��ϴ� �Ÿ�
	int					nCount;				///< ���ۺ��� ��������� Node ����
}DRIVING_CMD_INFO_OF_STEER;

/**
@brief	 ���� ��� ���� (Steer)
@author  puting
@date	 2016.11.09
*/
typedef struct DRIVING_CMD_INFO_OF_DIV_
{
	PATH_DIVERGENCE_INFO	Type;				///< ���� ����
	PATH_DIVERGENCE_INFO  NextType;			///< ������ ����:�� �κп� ��尡 ���� �����Ǹ� ���� ����
	double				dDistSumToEndNode;	///< ���� �������� �̵��ϴ� �Ÿ�
	int					nCount;				///< ���ۺ��� ��������� Node ����
}DRIVING_CMD_INFO_OF_DIV;


/**
@brief	 ���� ��� ���� (Direction)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_DIRECTION_
{
#if(USE_CHANGENODE == 1)
	PATH_DIRECTION_INFO		UBGRegion; 		///< ���� ���� ���� ����
	PATH_OBS_DIRECTION_INFO ObstacleRegion;
#else
	PATH_DIRECTION_INFO	Type;				///< ���� ���� ���� ����
#endif
	int					nPBSIgnoreFlag;
	int					nFollowsCommandType;
	double				dDistSumToEndNode;	///< ���� �������� �̵��ϴ� �Ÿ�
	int					nCount;				///< ���ۺ��� ��������� Node ����
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	DEVICE_NODE_TYPE		NodeType;			// node type 	(ex : OPT_TAG, OPT_DISTANCE..)
	int						OptDistance;		// Change node 1������ �Ÿ�
	int						OptDistanceAfter;		//
	PATH_DIRECTION_INFO		OptUBGRegion;		// Optional node 1���� ������ UBG type
	PATH_OBS_DIRECTION_INFO OptObstacleRegion;	// Optional node 1���� ������ OBS type
#endif
}DRIVING_CMD_INFO_OF_DIRECTION;

/**
@brief	 ���� ��� ���� (Speed)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_SPEED_
{
	double		dSpeed;					///< �ӵ�
	double		dDistSumToFinalNode;	///< ���� �ӵ� ���� Node���� �Ÿ� ( Node���� ����: Target Node / Station������ ����: Next Node )
	int			nCount;					///< ���ۺ��� ���� Node������ ����
	
	MARK_TYPE	MarkType;				///< ���� �������� MarkType ( 0:Node / 1:TagStation / 2:QRLeft / 3:QRRight )
	double		dDistSumToTarget;		///< ���� ������������ �Ÿ�
	double		dDecelDist;				///< ���� �Ÿ�
}DRIVING_CMD_INFO_OF_SPEED;

/**
@brief	 ���� ��� ���� (Path)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_PATH_
{
	UINT					StartNID;			///< ���� ���
	UINT        			EndNID;				///< �� ���
	NODE_TYPE				StartNType;			///< Path ���� Node Type: �Ϲ�(0), �б�(1), �շ�(2), �շ��б�(3)	: TCU ������� ��� ����
	NODE_TYPE				EndNType;			///< Path ���� Node Type: �Ϲ�(0), �б�(1), �շ�(2), �շ��б�(3)	: TCU ������� ��� ����
	PASSPERMIT_NODE_TYPE	StartType;			///< ���� ����㰡�� ���� Type   ����, ����, ����, �Ϲ�
//	DEVICE_NODE_TYPE        DeviceType;         ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4
	DEVICE_NODE_TYPE        NodeType;           ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4-> (�Ϲ� : 0, ������ : 1  (�ӽ÷� �������� 6)
	DEVICE_NODE_TYPE        NextNodeType;           ///< Lifter In : 1, Lifter Out :2, MTL In : 3, MTL Out :4-> (�Ϲ� : 0, ������ : 1  (�ӽ÷� �������� 6)
	double      			dSpeed;				///< �ӵ�
	double      			dLength;			///< Path�� ����
	double      			dDistSumToEndNode;	///< EndNode ������ �Ÿ� ��
	int						nPBSIgnoreFlag;
	int         			nCount;				///< ���ۺ��� ����(End Node)������ Node ����
	PATH_STEERING_INFO		Steering;			///< ���� ����
#if(USE_CHANGENODE == 1)
	PATH_DIRECTION_INFO		UBGRegion;			///< ���� ����
	PATH_OBS_DIRECTION_INFO ObstacleRegion;
#else
	PATH_DIRECTION_INFO		Direction;			///< ���� ����
#endif
	PATH_STEERING_INFO		PreSteering;		///< ������ ����
	PATH_DIVERGENCE_INFO    DivInfo;			///< �б� ����
	PATH_DIVERGENCE_INFO	PreDivInfo;         ///< ���б� ����
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	int						OptDistance;		// Change node 1������ �Ÿ�
	int						OptDistanceAfter;	//
	PATH_DIRECTION_INFO		OptUBGRegion;		// Optional node 1���� ������ UBG type
	PATH_OBS_DIRECTION_INFO OptObstacleRegion;	// Optional node 1���� ������ OBS type
#endif
}DRIVING_CMD_INFO_OF_PATH;

/**
@brief	 ���� ��� ���� (Brief)
@author  ehoto
@date	 2013.06.20
*/
typedef struct DRIVING_CMD_INFO_OF_TARGET_
{
	MARK_TYPE				Type;			///< ���� �������� Type
	
	UINT					Node;			///< Node ID ( Node���� ����: Target Node, Station������ ����: Parent Node)
	double					dDistSumToNode;	///< ���� ������������ �Ÿ� ( Node���� ����: Target Node ������ �Ÿ� / Station������ ����: Parent Node ������ �Ÿ� )
	int						nCount;			///< ���� �ӵ� ���� Node���� nCount ( Node���� ����: Target Node, Station������ ����: Next Node)

	UINT					Station;		///< Staion ID
	double					dOffset;		///< Node���� Station������ ����
	double					dSTBOffset;		///< STB�� Station������ ����

	UINT					NextNode;		///< Next Node ID ( Node���� ����: N/A, Station������ ����: Next Node )
	double					dLength;		///< Node���� NextNode������ ����
	bool					PassBoxIn;
	PATH_STEERING_INFO		Steering;		///< ���� ����: Station �� �� ���
	unsigned short			SQNum;			///< ������ �ѹ�
	unsigned char StationSafetyLevel;		///< SafetyLevel
	bool 		  bIgnolOverRun;			///< OverrunLevel

}DRIVING_CMD_INFO_OF_TARGET;

#endif
