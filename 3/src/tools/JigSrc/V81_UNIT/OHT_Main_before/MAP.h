//---------------------------------------------------------------------------
/**
@file    MAP.h
@version OHT 7.0
@brief   MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef MAPH
#define MAPH

#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_PathInfo.h"
#include "Def_DataFile.h"
#include "STD_MAP.h"
#include "STD_Collision.h"
#include "STD_FlowControl.h"
#include "STD_Teaching.h"
#include "STD_CID.h"
#include "MapMakeProcess.h"

#define INDEX_NONE  	 0xFFFFFFFF
#define PATHINDEX_OFFSET (1)


#define NAME_NODE_DISABLE 			"NodeDisable.nod"
#define NAME_PASS_BOX_NODE			"PassBoxNode.nod"

using namespace std;

// -------------------------------------------
// [BEGIN] by zzang9un 013.07.30 : 1Byte�� Packing
#pragma pack(push, 1) // 1byte�� Pack

/**
@brief   Path ���� ����ü
@author  zzang9un
@date    2012.09.04
*/
typedef struct Path_Info_
{    
    UINT ID;                ///< Path ID
    UINT StartNID;          ///< Path ���� Node ID
    UINT EndNID;            ///< Path ���� Node ID
    UINT Speed;             ///< Path Max Speed(mm/s)
    BYTE Steering;          ///< Steering ���� (N:0/L:1/R:2) -> 3, 4�� �ʿ��� ���ŵǹǷ� PathBranch�� ó�� �߰� �ʿ�
	UINT Distance;          ///< ���� Path �Ÿ�(mm)
#if(USE_CHANGENODE == 1)
    //BYTE PathDirection;     ///< Path ��� ���� ǥ�� (NoUse:0 / N:1 / L:2 / R:3 / S:4 / B:5 / V:6)   // �ű� �߰� UBGRegion ��
    BYTE UBGRegion;
	BYTE ObstacleRegion;
	BYTE PathBranch;        ///< �Ϲ� �б� �Ǵ� N�б� �Ǵ� (NoUse:0, 1: �Ϲݺб�, 2: N�б�)
	BYTE RailBranch;         ///< ���� ���� N, R, L data
    UINT OutSteeringOffset; ///< ������ ��� ������ Offset
    UINT InSteeringOffset;  ///< ���� ����
    BYTE ObstacleIgnore;     ///< OBS �̻�� ����  (���:true/�̻��:false)  -> Node �������� ��ȯ�ؼ� Path������ ����

	// for change node (jhoun.lim)
	UINT OptDistance;
	UINT OptDistanceAfter;
	BYTE OptUBGRegion;
	BYTE OptObstacleRegion;


	//---------------------------------------------------------------------------
	// for change node (jhoun.lim)
	static UINT RatioEncoding(const double ratio)
	{
		return (((UINT(ratio*0.1) << 4) | (UINT(ratio) % 10)) << 4 | UINT(ratio * 10) % 10) << 4 | UINT(ratio * 100) % 10;
	}
	static double RatioDecoding(const UINT ratio)
	{
		UINT a = ((ratio & 0xF000) >> 12);
		if(a>9) return -1.0;

		UINT b = ((ratio & 0x0F00) >> 8);
		if(b>9) return -1.0;

		UINT c = ((ratio & 0x00F0) >> 4);
		if(c>9) return -1.0;

		UINT d = (ratio & 0x000F);
		if(d>9)    return -1.0;

		UINT partInt = a * 10 + b;
		UINT partFloating = c * 10 + d;
		return partInt + partFloating*0.01;
	}

	static UINT ConvertRatio2Distance(const UINT length, const UINT ratio, const bool isFirst = true)
	{
		return length * RatioDecoding(isFirst ? (ratio & 0xFFFF0000) >> 16 : (ratio & 0x0000FFFF)) * 0.01;
	}
	void calcOptDistance(const UINT ratio) { this->OptDistance = ConvertRatio2Distance(this->Distance, ratio, true); }
	void calcOptDistanceAfter(const UINT ratio) { this->OptDistanceAfter = ConvertRatio2Distance(this->Distance, ratio, false); }
#else
	BYTE PathDirection;     ///< Path ��� ���� ǥ�� (NoUse:0 / N:1 / L:2 / R:3 / S:4 / B:5 / V:6)   // �ű� �߰� UBGRegion ��
	BYTE PathBranch;        ///< �Ϲ� �б� �Ǵ� N�б� �Ǵ� (NoUse:0, 1: �Ϲݺб�, 2: N�б�)
	BYTE RailBranch;         ///< ���� ���� N, R, L data
	UINT OutSteeringOffset; ///< ������ ��� ������ Offset
	UINT InSteeringOffset;  ///< ���� ����
	BYTE ObstacleIgnore;     ///< OBS �̻�� ����  (���:true/�̻��:false)  -> Node �������� ��ȯ�ؼ� Path������ ����
#endif
} PATH_INFO;

/**
@brief   Node ���� ����ü
@author  zzang9un
@date    2012.09.04
*/
typedef struct Node_Info_
{
    UINT NID;               ///< Node ID
//    UINT Point_X;           ///< X��ǥ
//	UINT Point_Y;           ///< Y��ǥ
	UINT TagType;           ///< Node Type N S R C    puting
	// TagType ���� ��� Ȯ�� �ʿ�
	
//	UINT CID;               ///< Start/ ����/ End / ���� --> CID.DAT ����ü ��� 
//	UINT FrontOBS2;         ///< ���� ��ֹ� ���� Node 2    -> ��ȯ�ؼ� Path������ ����
//	UINT LowAccellation;    ///< ���ӵ� ����
//	UINT CollisionInfo;     ///< ���� ����� �ʿ��� ���� (���� ����)
	//CollisionInfo�� ����ü�� ���� �䱸 by puting
	// CollisionInfo ����ü�� ���� �����Ͽ� ȣ���ϴ� ����
	// NODE_INFO�� ���� ȣ���ϴ� �ͺ��� ������...��.
	// CollisionInfo NODE_INFO�� ��ü ������ �����Ƿ� NODE_INFO�� CollisionInfo�� ������ �ִ°� ���� ����.
	// ���� �ʵ����� ����� ������ġ�� �߰����θ� 
	UINT InOutInfo;         ///< ����㰡 Stop/Reset/����                     		puting
							// 1: Lifter In
							// 2: Lifter Out
							// 3: MTL In
							// 4: MTL Middle
							// 5: MTL Out
	UINT PrePathIndex[2];   ///< �ش� Node�� EndNID�� Path ����ü �迭�� Index : �Ÿ� ����ġ ���� ���� PrePathIndex[0]  - PathSearch �� ����
	UINT NextPathIndex[2];  ///< �ش� Node�� StartNID�� Path ����ü �迭�� Index : �Ÿ� ����ġ ���� ���� NextPathIndex[0]   - PathSearch �� ����
													            //--------> �б� ���� [0] ���� �б� Node , [1] ������ �б� Node        puting
} NODE_INFO;

#pragma pack(pop) // Pack ����
// [END] by zzang9un 013.07.30 : 1Byte�� Packing
// -------------------------------------------

/**
@class   MAP
@date    2012.11.09
@author  zzang9un
@brief   OHT�� ���Ǵ� Map�� ���õ� Data, �Լ��� ������ class
*/
class MAP
{
private:
    void    MakeNodeMap();
	void	MakeRightNodeMapFromPathArray();
	int 	MakePathIndex(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map);
    void    ComparePathDistance(UINT *PathIndex);

public:
    int m_PathNum; ///< Path ����
	int m_NodeNum; ///< Node ����
	unsigned char m_FlowcontrolCheckResult;

    PATH_INFO *m_PathArray; ///< Path ���� �迭 ������
    NODE_INFO *m_NodeArray; ///< Path ���� �迭 ������

	std::map<int, int> m_MakeLeftNodeMap;
    std::map<int, int> m_MakeRightNodeMap;
    std::map<int, int> m_NodeMap;
	std::map<int, int> m_TeachingMap;
	std::map<int, int> m_LookDownMap;
	std::map<int, int> m_CIDResetMap;
	std::map<int, int> m_CIDStopMap_1;
	std::map<int, int> m_CIDStopMap_2;
	std::map<int, int> m_CIDStartMap_1;
	std::map<int, int> m_CIDStartMap_2;

	std::map<int, int> m_FlowControl;
	std::map<int, int> m_NextNode;
	std::map<unsigned long long, int> m_FlowControl_StopReset;	
	std::map<unsigned long long, int> m_RailMerge_StopReset;

    MAP();
    ~MAP();

    void InitMap(int PathNum, int NodeNum);
	int MakeMap(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map);
	void MakeTeachingMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum);
	void MakeCIDMap(NODE_INFO_STD_CID * m_NodeArray_CID, UINT DataNum);
	void MakeFlowControlMap(NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum);

    int FindNodeIndex(int NodeID);
	int FindNextIndex(int NodeID);
	int FindRightNodeIndex(int NodeID);
	int FindStationIndexFromTeachingMap(int NodeID);
	int FindStationIndexFromLookDownMap(int NodeID);
	int FindNodeIndexFromCIDReset(int NodeID);
	int FindNodeIndexFromCIDStop_1(int NodeID);
	int FindNodeIndexFromCIDStop_2(int NodeID);
	int FindNodeIndexFromCIDStart_1(int NodeID);
	int FindNodeIndexFromCIDStart_2(int NodeID);

	int FindNodeIndexFromFlowControl(int NodeID,int NextNodeID,STD_FlowControl *pFlowMap);
	int FindIndex_FlowControl_StopReset(unsigned long long NodeID);
	int FindIndex_RailMerge_StopReset(unsigned long long NodeID);


    // by zzang9un 2013.07.23 : PID�� ã�� �Լ� �߰�(for Map Make)
    int GetPathIndex(int StartNID, int EndNID);

	int ReadNodeDisableFile();
	int ReadPassBoxNodeFile();

	int CheckFlowControlData(unsigned long long StopResetNode);

	TStringList* tNodeDisalbeList;
	TStringList* tPassBoxNodeList;
};
//---------------------------------------------------------------------------

// -------------------------------------------
// Map ���� ���� �ڵ�
// -------------------------------------------
//#define ERR_MAP_FILESAVEFAIL        -2000

/**
@class   MapDataManager
@date    2013.07.30
@author  zzang9un
@brief   Map Data�� �����ϴ� Class
*/
class MapDataManager
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag
    
    MAP_FILE_HEADER m_MapFileHeader; ///< Map File Header

public:
    MapDataManager();
    ~MapDataManager();

	int LoadMapFile(TCHAR *FileName, MAP *pMapdata);
	int SaveMapFile(TCHAR *FileName, MAP *pMapdata);
	int LoadDataFromSTDMap(MAP *pMapdata, 
                           NODE_INFO_STD_MAP * m_NodeArray_Map, 
                           NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
                           NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   NODE_INFO_STD_CID * m_NodeArray_CID,
						   MapMakeProcess* pMapMakeProcess,
                           UINT NodeNum_Map, UINT DataNum_Flow, UINT DataNum_CID, bool bAdjust = false);

	bool GetMapLoaded();
	unsigned int GetMapVersion();

};
//---------------------------------------------------------------------------

// by zzang9un 2012.11.30 : Map ������ String�� Int�� �����ϴ� �Լ� ����
// by zzang9un 2012.11.30 : ���¿� ���� ��û�� ���� ������

/**
@brief   Steering ���� ���ڸ� ���� enum ������ convert�ϴ� �Լ�
@author  zzang9un
@date    2012.11.30
@param   strSteering : Port ID
@return  BYTE(PATH_STEERING_INFO) ������ ����
*/
BYTE SteeringToInt(TCHAR cSteering)
{
    BYTE ReturnVal = PATH_STEERING_NONE;
    
    switch(cSteering)
    {
    case 'N':
        ReturnVal = PATH_STEERING_NONE;
        break;

    case 'L':
        ReturnVal = PATH_STEERING_LEFT;
        break;

    case 'R':
        ReturnVal = PATH_STEERING_RIGHT;
        break;

    case 'l':    
        ReturnVal = PATH_STEERING_N_BRANCH_L2R;
        break;

    case 'r':    
        ReturnVal = PATH_STEERING_N_BRANCH_R2L;
        break;    

    default:
        break;        
    }

    return ReturnVal;
}
//---------------------------------------------------------------------------

/**
@brief   Steering ���� ���ڸ� ���� enum ������ convert�ϴ� �Լ�
@author  zzang9un
@date    2012.11.30
@param   strSteering : Port ID
@return  PATH_STEERING_INFO ������ ����
*/
BYTE PathDirToInt(TCHAR cPathDir)
{
    BYTE ReturnVal = PATH_DIRECTION_NO_USE;
    
    switch(cPathDir)
    {
    case 'N':
        ReturnVal = PATH_DIRECTION_NORMAL;
        break;

    case 'L':
        ReturnVal = PATH_DIRECTION_LEFT;
        break;

    case 'R':
        ReturnVal = PATH_DIRECTION_RIGHT;
        break;

    case 'S':
        ReturnVal = PATH_DIRECTION_SHORT;
        break;

    case 'B':
        ReturnVal = PATH_DIRECTION_BOTH;
        break;    

    case 'V':
        ReturnVal = PATH_DIRECTION_NARROW;
		break;

	case 'H':
		ReturnVal = PATH_DIRECTION_HILL;
        break;
        
    default:
        break;        
    }

    return ReturnVal;
}
//---------------------------------------------------------------------------
#endif //MAPH
