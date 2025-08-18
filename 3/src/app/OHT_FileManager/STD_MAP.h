//---------------------------------------------------------------------------
/**
@file    STD_MAP.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_MAPH
#define STD_MAPH

#include <map>
#include "Def_DataType.h"
#include "Def_OHT.h"
//#include "Def_PathInfo.h"
//#include "Def_DataFile.h"
#include "Def_DataFile_STD.h"




//typedef���ϸ� Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack



/**
@brief   MAPDATA.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Map
{
	UINT NodeID;                        ///< ���� Node Id
	BYTE NodeType;                      ///< Node�� ����
										//   0x1: Lift In, 0x2: Lift out 0x3: MTL In, 0x4 : MTL Middle, 0x5:MTL out
	UINT LeftNodeID;                    ///< (����) ����� Node Id
	UINT LeftNodeDistance;              ///< (����) ����� Node������ �Ÿ�(mm)
	WORD LeftNodeSpeed;                 ///< (����) ����� Node������ �̵� �ӵ�(mm/s)
	BYTE LeftSteering;                  ///< (����) ����� Point������ Steer ����
	BYTE LeftBranch;                    ///< (����) ����� Point������ �б�����(�Ϲݺб�,N�б�)
	UINT RightNodeID;                   ///< (������) ����� Node Id
	UINT RightNodeDistance;             ///< (������) ����� Node������ �Ÿ�(mm)
	WORD RightNodeSpeed;                ///< (������) ����� Node������ �̵� �ӵ�(mm/s)
	BYTE RightSteering;                 ///< (������) ����� Point������ Steer ����
	BYTE RightBranch;                   ///< (������) ����� Point������ �б�����(�Ϲݺб�,N�б�)
	UINT ResetNodeMain;                 ///< ���_�㰡_Reset Node ID(Main)
	UINT ResetNodeSub;                  ///< ���_�㰡_Reset Node ID(Sub.)
	UINT ResetNodeRedundance1;          ///< ���_�㰡_Reset Node ID(Redundance 1)
	UINT ResetNodeRedundance2;          ///< ���_�㰡_Reset Node ID(Redundance 2)
	UINT Reserved_1;                    ///< Reserved_1
	UINT Reserved_2;                    ///< Reserved_2
	UINT Reserved_3;                    ///< Reserved_3
} NODE_INFO_STD_MAP;
//stuct size : 57Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_MAP
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/

class STD_MAP
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_MAP *m_NodeArray; ///< Node ���� �迭 ������

	STD_MAP();
	~STD_MAP();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   Map Data�� �����ϴ� Class
*/
class MapDataManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_MapFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	MapDataManager_STD();
	~MapDataManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_MAP *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_MAP *pMapdata);

};
//---------------------------------------------------------------------------

#endif //STD_MAPH

