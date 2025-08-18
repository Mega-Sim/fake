//---------------------------------------------------------------------------
/**
@file    STD_PATHDATA.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_PATHDATAH
#define STD_PATHDATAH

#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
//#include "Def_PathInfo.h"
//#include "Def_DataFile.h"
#include "Def_DataFile_STD.h"
//#include "LogHandler.h"	// Log �ۼ� ����


//typedef���ϸ� Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack

/**
@brief   PATHDATA.DAT�� data ���� ����ü
@author  KJD
@date    2016.05.08
*/
typedef struct Node_Info_STD_PathData
{
    BYTE LayoutType;                    ///< Layout Type(1: MTL, 2: Lifter)
    UINT LayoutID;                      ///< Layout ID
    UINT LeftNodeIDIn;                  ///< MTL(����), Lifter(�Ʒ�) ���� Node Id
    UINT LeftNodeIDMiddle;              ///< MTL(����), Lifter(�Ʒ�) �߰� Node Id
    UINT LeftNodeIDOut;                 ///< MTL(����) ���� Node Id
    BYTE LeftDriveDirection;            ///< ���� ���� ������ ǥ��(0: ����, 1: ����)
    UINT RightNodeIDIn;                 ///< MTL(������), Lifter(��) ���� Node Id
    UINT RightNodeIDMiddle;             ///< MTL(������), Lifter(��) �߰� Node Id
    UINT RightNodeIDOut;                ///< MTL(������) ���� Node Id
    BYTE RightDriveDirection;           ///< ���� ���� ������ ǥ��(0: ����, 1: ����)
    UINT Reserved1;                     ///< Reserved 1
    UINT Reserved2;                     ///< Reserved 2
} NODE_INFO_STD_PATHDATA;
//stuct size : 39Byte
#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_PATHDATA
@author  KJD
@date    2016.05.08
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_PathData
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_PATHDATA *m_NodeArray; ///< Node ���� �迭 ������


	std::map<int, int> m_PathDataMap;

	STD_PathData();
	~STD_PathData();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.05.08
@brief   Map Data�� �����ϴ� Class
*/
class PathDataManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC �α� ������ ���� ����
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_PathDataFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	PathDataManager_STD();
	~PathDataManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_PathData *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_PathData *pMapdata);

	bool find(STD_PathData *pMapdata, int NodeID, NODE_INFO_STD_PATHDATA* pPathData);

	int GetPathData(STD_PathData *pMapdata, NODE_INFO_STD_PATHDATA *pPathdata,UINT NodeID);


};
#endif //STD_PATHDATAH

