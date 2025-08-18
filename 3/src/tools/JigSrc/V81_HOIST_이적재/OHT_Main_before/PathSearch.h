//---------------------------------------------------------------------------
#ifndef PathSearchH
#define PathSearchH

//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "MAP.h"
#include "List.h"
#include "ExecuteInfo.h"
#include "LogHandler.h"	// Log �ۼ� ����
#include "Def_PathSearchIniParam.h" //�Ķ����
#include "LIniFile.h"
#include "DrivingControl.h"
#include "OHTMainStatus.h"

#define	MARGIN_OF_DIST_TO_STATION	100	// ����: mm
#define	MARGIN_OF_DIST_TO_RESETNODE	250.0	// 100  ����: mm
#define	MARGIN_OF_DIST_TO_STOPNODE	350// 400���� ����: mm
#define	MARGIN_OF_DIST_TO_STOPNODE_ROLLBACK	250// 400���� ����: mm
#define	VHLACCEL				    2000//2000	// ����: mm/s^2
#define	SPEED_LIMIT_OF_MAP_MAKE		0.2	// ����: mm/msec
#define MAX_PATHINDEX 				9999

#define MAX_LIST_COUNT_FOR_PRESTEER				9999
#define FIRST_INDEX				1

// soming UBG �ܰ� Check Default Node ����
#define UBG_CHECK_NODE_1 999999
#define UBG_CHECK_NODE_2 999999
#define UBG_CHECK_NODE_3 999999
#define UBG_CHECK_NODE_4 999999



#define MAXSPEED(a,b) (((a) >(b))?(a) :(b))     //puting

/***********************************************************************************/
// ���� �� map �ӵ� ����ȭ ����
/***********************************************************************************/
#define TIME_OF_CONST_VEL	0.5 // 0.5s
#define TIME_OF_STEERING_CHANGE 0.7	// 0.7s
#define VELOCITY_INCREASE_MARGIN  1.1 // 10%
#define DEFAULT_VEL 3.3 // m/s
#define DEFAULT_ACCEL 2.0 // m/s^2
#define DEFAULT_DECEL 3.0 // m/s^2
#define LAST_INDEX_OF_PATH 0
#define LAST_INDEX_OF_NORMAL_LINK 1
#define MIN_CNT_OF_VEL_CHANGE 2
#define VELOCITY_MARGIN 0.7 //70%

#define HIGH_SPEED_LIMIT_VEL	3.3 // m/s
#define HIGH_SPEED_ACCEL	1.0 // m/s^2, #define HIGH_SPEED_LIMIT_VEL �� ������ HIGH_SPEED_ACCEL ����



#define CAL_SPEED(speed, distance, const_time, accel, decel)	abs((sqrt(((const_time)*(const_time))+	\
	((1.0/(decel)+1.0/(accel))*((speed)*(speed)/(accel)+2.0*(distance))))	\
	-(const_time)) / (1.0/(decel)+1.0/(accel)))

#define GET_ACCEL_DIST(inspeed, outspeed, accel)	abs((((outspeed)*(outspeed))-((inspeed)*(inspeed)))/(2.0*(accel)))
#define GET_DECEL_DIST(inspeed, outspeed, decel)	abs((((inspeed)*(inspeed))-((outspeed)*(outspeed)))/(2.0*(decel)))
#define GET_MOVE_TIME(inspeed, distance, decel)	    abs(((distance)/(inspeed))+((inspeed)/(2*(decel))))

/***********************************************************************************/
// Function Name : PathSearch
// Release Date: 2012.11.12, ���¿�
/***********************************************************************************/

//typedef	struct CHANGE_PATH_NODE_INFO_
//{
//	CYCLEDRIVING_CHANGE_INFO	  			ChangeInfo[MAX_LIST_COUNT_FOR_CHANGE];
//	int	ChangeNodeCount;
//}CHANGE_PATH_NODE_INFO;


/**
@brief   V8.0 OHT Manual Path Search�� ���� ����ü, �̵� Path ������ ��� ����
@author  Labeler
@date    2016.12.22
*/
typedef struct _EXCEPT_PATH_LIST_V80
{
	unsigned int nCnt;
	int ExceptNode[300];
}EXCEPT_PATH_LIST_V80;


class PathSearch
{
private:

	bool getNodeType(MAP *pMap, UINT uNID, NODE_TYPE* pNodeType);
	UINT GetIncreaseSeqenceNumber();
	UINT GetSeqenceNumber();
//	double GetSpeedDownDist(double dAccel, double dSpeed);

	LList		*m_PathList;    ///< List
	CRITICAL_SECTION	m_CS;
	LogUnit *pLogPathSearch;      ///< PathSearch �α� ������ ���� ����
	FDCUnit *pFDCUnit;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	UINT m_SquenceIndex;
	DrivingControl	*m_pDrivingControl;

	double calcSpeed(double velIn, double distance, double constTime, double accel, double decel);      // ��� �ð� ����  (constTime)
	double calcSpeed_2(double velIn, double distance, double spdDownTime, double accel, double decel);  // ���� Ÿ�̹� ����. (spdDownTime)

	double GetNewVelocity(double dVelIn, double dVelOut, double dDefaultAccel, double dDefaultDecel, double dDistanceSum, bool bSteeringChange);
	int ChangePathVelocity(void);
//	CHANGE_PATH_NODE_INFO m_CurrentChangePathInfo;  ///< ���� ����Ŭ���� �۾������� ������ ��ġ�� �־��ִ� ��. ���� Path Ž���ҋ� �ʱ�ȭ ���� ��.
//													///<
public:
	PATHSEARCH_PARAM_SET		*m_pParamSet;		///< �Ķ���� ����: ���� ���Ǵ� ��
	PATHSEARCH_PARAM_SET		*m_pTmpParamSet;	///< �Ķ���� ����: ������Ʈ �� �� ���Ǵ� ��
	PATHSEARCH_UBG_CHECK_PARAM_SET  *m_pUBGParamSet; ///<soming UBG Check �Ķ���� ���� :���� ���Ǵ� ��
	PATHSEARCH_NODE_LANCUT_PARAM_SET *m_pNodeLanCutSet; ///<Manual �̵��� �̵��Ұ� ó���� Node ����


public:
	PathSearch();
	~PathSearch();

    void InitPathList();
    void AddPath(void *pData);
    void* ReferFirstPath();
    void* ReferPath(UINT index);
    void* ReferBackPath();
    void* PopFirstPath();
    void* PopBackPath();
    int GetPathCount();

	//�Ķ���� ���� �ڷ�
	bool    loadParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< �Ķ���� �ҷ����� �Լ�
	bool    saveParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< �Ķ���� �����ϴ� �Լ�
	bool    updateParameter();
	bool    loadUBGCheckParam(char* szFilePath, PATHSEARCH_UBG_CHECK_PARAM_SET* pUBGParamSet);  ///< �Ķ���� �ҷ����� �Լ�
	bool    loadNodeLanCutCheckParam(char* szFilePath, PATHSEARCH_NODE_LANCUT_PARAM_SET* pNodeLanCutSet);  ///< �Ķ���� �ҷ����� �Լ�

	bool    loadParameterRecovery(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet);    ///< �Ķ���� �ҷ����� �Լ�
    int  	m_ParamChehckError;
									   ///< �Ķ���� ������Ʈ �ϴ� �Լ�

    //int GetSearchDivergence(MAP *pMap, int StartNode);
    double GetSpeedDownDist(double dAccel, double dSpeed);
    double GetMarginOfSpeedDist(double dSpeed, double Margin);
	int CheckSearchDifferntPoint(MAP *pMap, PARSING_GO_INFO* CmdGoData, UINT &ChangeSqNum, int &ChangeNodeID, double &ChangeNodeDistance, int &nExtraPath, BYTE &chNackToAckType, UINT nPreTargetStation);
    bool CheckSearchChangePoint(int ChangeNodeID, UINT &ChangeSqNum, double &ChangeNodeDistance);

    int GetPassPermitPriod();
    int GetPassPermitRollbackTime();
	int GetPassPermitRollbackFinshTime();
	double GetPassPermitRollbackMargin();

    int	GetNextNode(MAP *pMap, UINT nFromNode, UINT nPathDiv);  //puting
	bool GetIsNBrench(MAP *pMap, UINT nFromNode, UINT nNextNode);  //puting
	bool GetIsNBrenchMapMake(MAP *pMap, UINT nNextNode);  //puting
    int	GetPreSteering(MAP *pMap, UINT StopNode, UINT PreSteeringNode, PATH_STEERING_INFO *pPreSteeringInfo);
    int CheckPathOfStation(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo);	///< Station �� �ִ� Path�� ���� �˻�
    int SearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch ��� Return
	int StandardSearchPassPermit(MAP *pMap,STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, UINT &uCheckExtraNum, bool bIsFoup,double PathExtra =0.0, bool bIgnorOverrun = false);	///< PathSearch ��� Return
    int StandardSearchOnlyOnePassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, bool bcheckTarget, PKT_CYCLECMDDATA *FinalCycleData, UINT &uCheckExtraNum,double &dTempDis, UINT FinalSeqNum, PASSPERMIT_COMMON TempPassPermitData,bool bIsFoup, bool bIgnorOverrun = false);	///<//AOHC-195 ������ ���� �۾�(����2) ( PathSearch ��� Return)
    int CheckPassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool bIsTargetStationoPassPermitOpen=false);	///< PathSearch ��� Return
    int CheckOnlyOnePassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool IsStartStation,bool IsEndStation,bool bStartStationPassPermit,bool bIsTargetStationoPassPermitOpen);	///< PathSearch ��� Return

    int StandardSearchPassBack(MAP *pMap,PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch ��� Return

	PATH_DIVERGENCE_INFO GetStationDivInfo(MAP *pMap, UINT nFromNode, UINT nNextNode);  //puting

	bool CheckTurnOffOfPathInfo(MAP *pMap,UINT nCurNode);  //puting
	int CheckDeviceOfPathInfo(MAP *pMap,UINT nCurNode);  //puting

 	PATH_STEERING_INFO GetSteeringInfo(MAP *pMap, UINT nFromNode, UINT nNextNode); //puting

    // 2016-12-22, Add by Labeler
    int FrontSearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch ��� Return
    int FrontSearchDrivingPathMapMake(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, int &nTargetNode);	///< PathSearch ��� Return
    int FrontSearchDrivingPathAutoMoveTest(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List);	///< PathSearch ��� Return
	int SearchExceptPathForV80(MAP * pMap, UINT FromNode, UINT ToNode, EXCEPT_PATH_LIST_V80 *pPath,int &TargetNode, BYTE MapMakeBrench=0x00);

	int GetIncreaseSQNumber();

	bool CheckNodeAndLinkIndex(MAP * pMap, UINT FromNode, UINT ToNode);
	bool CheckEndStationOffset(MAP * pMap, UINT ParentNode, UINT NextNode, int EndStationOffset);
	bool CheckSteering4Deccel(PARSING_GO_INFO *GetGOInfo, int ChangeNodeID);

};

//----------------�ӵ� ���̺� ���� ����㰡 �Ÿ���------------------------------/
//puting ����㰡�� ���� �Ÿ���� �κ� �߰�
inline double PathSearch::GetSpeedDownDist(double dAccel, double dSpeed)
{
	return (fabs((dSpeed*dSpeed)/(2*dAccel)));   // S=(V^2)/(2*a)
}


inline double PathSearch::GetMarginOfSpeedDist(double dSpeed, double Margin)
{
   //	return (fabs(sqrt(sqrt(sqrt(dSpeed))))*Margin);
   return (fabs(dSpeed*(Margin/1000))); // ����㰡 ��û�ֱ� * (�ӵ� * Ƚ��)
}

inline UINT PathSearch::GetIncreaseSeqenceNumber()
{
	if(m_SquenceIndex ==65535) m_SquenceIndex = 0;
	return ++m_SquenceIndex;
}
inline UINT PathSearch::GetSeqenceNumber()
{
	return m_SquenceIndex;
}
//---------------------------------------------------------------------------
#endif
