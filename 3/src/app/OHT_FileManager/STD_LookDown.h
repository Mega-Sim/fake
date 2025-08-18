//---------------------------------------------------------------------------
/**
@file    STD_LookDown.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_LookDownH
#define STD_LookDownH

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
@brief   LOOKDOWN.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_LookDown
{
    UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< ������ Port ���� ex) EQ, STB-L, STB-R, UTB"
	BYTE LookDown;                      ///< Station ������ �� Lookdown Area ����
	UINT Reserved1;                     ///< Reserved_1
} NODE_INFO_STD_LOOKDOWN;
//stuct size : 10Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_LookDown
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_LookDown
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_LOOKDOWN *m_NodeArray; ///< Node ���� �迭 ������

	std::map<int, int> m_LookDownMap;

	STD_LookDown();
	~STD_LookDown();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   Map Data�� �����ϴ� Class
*/
class LookDownManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_LookDownFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	LookDownManager_STD();
	~LookDownManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_LookDown *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_LookDown *pMapdata);

};
#endif //STD_LookDownH
