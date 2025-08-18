#ifndef Def_DrivingH
#define Def_DrivingH


#include <stdio.h>


/**
@brief	 Motion ������ ���� �޴� User ������ ��ȯ�� Node ����
@author  ���¿�
@date	 2013.06.30
*/
typedef struct USER_NODE_CHECK_DATA_
{
	int NodeCountOfPreCheck;	///< ��� ����
	int NodeCount;				///< ��� ����
	
	// SetHome, setOffset �� �ϸ� ���� ��ġ�� �� ������ ������
	double	PositionOfDrivingTagPreCheck;	///< ���� ��� ���� ������ ��ġ		
	double	PositionOfDrivingTag;			///< ���� ��� ������ ��ġ			 
	double	PositionOfTransTagFirst;		///< ������ ��� ������ ��ġ(Front)
	double	PositionOfTransTagFinal;		///< ������ ��� ������ ��ġ(Rear)
	
}USER_NODE_CHECK_DATA;

/**
@brief	 Motion ����⿡ ������ �������� ���� data �� ���ѱ� ���� User ������ �Ǿ��ִ� ����
@author  ���¿�
@date	 2013.06.30
*/
typedef struct USER_DETECT_INFO_
{
	char	UseFollowControlFlag;	///< ���� ���� ��� ����: true �� ���, false�� �̻��
#if(USE_CHANGENODE == 1)
	char	OHTDetectType;				///< Link�� UBGPattern
	char	OBSDetectType;				///< Link�� OBSPattern
#else
	char	Type;					///< Link�� Type
#endif
	int		NodeCount;				///< ��� ����
	double	DistSumOfStraightPath;	///< ���� ���� �̾����� ���������� ��ġ (User)
	int		FollowCommandType;		
	int		PBSIgnoreFlag;
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	char	NodeType;
	int		DistON;					// Change node ������ �Ÿ�
	int		DistAfterON;			// Change node ���� ���� �Ÿ�
	char	OHTDetectTypeOpt;		// Optional node 1���� ������ type UBG
	char	OBSDetectTypeOpt;		// Optional node 1���� ������ type OBS
#endif
}USER_DETECT_INFO;


#endif
