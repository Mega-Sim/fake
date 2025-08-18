//---------------------------------------------------------------------------
/**
@file    StationData.h
$Rev:: 477           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-07 1#$:  Date of last commit
@version OHT 7.0
@brief   Station Data ������ ���� Header ����
*/

#ifndef StationDataH
#define StationDataH

#include <map>
#include <utility>

#include "Def_OHT.h"
#include "Def_DataFile.h"
#include "STD_Station.h"
#include "MAP.h"

using namespace std;
//---------------------------------------------------------------------------

// by zzang9un 2013.07.19 : ��� �������� ������ ���� 1����Ʈ�� Pack
#pragma pack(push, 1) // 1byte�� Pack

// Error Code
#define ERR_STDATA_MAKEMAP      -1
#define ERR_STDATA_FILEOPEN     -1001
#define ERR_STDATA_FORMAT       -2000   ///< Data Format Error
#define ERR_STDATA_DUPLICATION  -2001   ///< Data �ߺ�
#define ERR_STDATA_INSERTFAIL   -2002   ///< Map Insert Fail
#define ERR_STDATA_DELETEFAIL   -2003   ///< Map Delete Fail
#define ERR_STDATA_UPDATEFAIL   -2004   ///< Map Update Fail
#define ERR_STDATA_NODATA       -2005   ///< Map Data ����
#define ERR_STDATA_SAVEFAIL     -2006   ///< Save ����
#define ERR_STDATA_OFFSET	  	-2007   ///< Save ����
#define ERR_STDATA_LINK		  	-2008   ///< Save ����

#define NUM_STATION_DATA_ELEMENT    5   ///< Station Data ����ü ���� ����
/**
@brief   Station Data ����ü
@author  zzang9un
@date    2013.06.08
*/
typedef struct STATION_DATA_
{
    UINT    StationID;      ///< Station ID
    BYTE    StationType;    ///< Station Type
    UINT    ParentNodeID;   ///< Parent Node ID
    UINT    NextNodeID;     ///< Next Node ID
	int     StationOffset;  ///< Node�κ����� �Ÿ�(mm)
	BYTE    Safety_Level;  ///< Node�κ����� �Ÿ�(mm)
	UINT	PhysicalID;		//< 2�� STB, Physical Tag ID
	DEFAULT_MAPINFO    IsDefaultMap;  ///< Node�κ����� �Ÿ�(mm)
} STATION_DATA;



#pragma pack(pop) // Pack ����

// Station Data�� ���� map ����
typedef std::map<UINT, STATION_DATA *> STATION_DATA_MAP;
typedef STATION_DATA_MAP::iterator STATION_DATA_MAP_ITER;
typedef std::pair<UINT, STATION_DATA *> STATION_DATA_PAIR;
typedef std::pair<STATION_DATA_MAP_ITER, bool> STATION_DATA_MAP_RESULT;

/**
@class   StationDataManager
@date    2013.06.08
@author  zzang9un
@brief   Station Data�� �����ϴ� Class

         1. Station Data�� map(�ڷᱸ��)�� �����Ǿ� ����
         2. binary ������ ���ϸ� �о�� �� ����
*/
class StationDataManager
{
private:
//	LogUnit *pLogStationLoad;      ///< StationLoad �α� ������ ���� ����

public:
    bool m_bDataLoaded;     ///< Data �ε� flag
	bool m_bDataModified;   ///< Data ���� flag
	FILE_HEADER m_FileHeader;
    
public:
    STATION_DATA_MAP m_StationData;    ///< Station Data �ڷᱸ��(Map)
    
    StationDataManager();
    ~StationDataManager();

    // ���� ó�� �Լ�
//	int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName, int FileType);


	// Map �ڷᱸ�� ���� �Լ�
	void Init();
	int GetCount();

	// Map �ڷᱸ�� �߰�, ������Ʈ, ����, �˻�  �Լ�
	int Insert(STATION_DATA *addStationData);
	int Update(STATION_DATA *updateSTD);
	int Delete(UINT StationID);
	STATION_DATA* Find(UINT StationID);

	int MemoryInsert(STATION_DATA *addStationData);
	int LoadDataFromSTDMap(MAP *pMapdata, NODE_INFO_STD_STATION *m_NodeArray, UINT DataNum);

};

//---------------------------------------------------------------------------

#endif

