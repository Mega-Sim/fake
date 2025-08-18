//---------------------------------------------------------------------------
/**
@file    STD_StationH.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_StationH
#define STD_StationH

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
@brief   STATION.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Station
{
	UINT StationID;                     ///< Station Id
	BYTE StationType;                   ///< Station Type
	UINT ParentNodeID;                  ///< Station�� ���� �ִ� Link�� from Node Id
	UINT NextNodeID;                    ///< Station�� ���� �ִ� Link�� to Node Id
	WORD Offset;                        ///< Parent Node�κ����� �Ÿ�(mm)
	BYTE Safety_Level;                  ///< ������峻 ������ �ݳ� ���� �Ǵ� �÷���
	UINT PhysicalID;					///< 2�� STB, Physical Tag ID
	BYTE Reserved1;         			///< Reserved 3
	BYTE Reserved2;                     ///< Reserved 4
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low����Ұ�.
} NODE_INFO_STD_STATION;
//stuct size : 23Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_Station
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_Station
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_STATION *m_NodeArray; ///< Node ���� �迭 ������

	std::map<int, int> m_StationMap;

	STD_Station();
	~STD_Station();

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
class StationManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC �α� ������ ���� ����
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_StationFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	StationManager_STD();
	~StationManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Station *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Station *pMapdata);
	FILE_HEADER_STD* GetFileHeader();
//	void SetFileHeader(FILE_HEADER_STD *pFileHeader);       //�̻��

};
#endif //STD_StationH
