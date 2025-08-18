//---------------------------------------------------------------------------
/**
@file    STD_MapMakerFileH.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_MapMakerFileH
#define STD_MapMakerFileH


#include <utility>
#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_DataFile_STD.h"

#include "StationData.h"
//#include "LogHandler.h" // Log �ۼ� ����


//typedef���ϸ� Def_Variables.h
using namespace std;

// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack


/**
@brief   TEACHING.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_MapMakerFile
{
	BYTE MapMakerType;                  ///< MapMaker ���� ex) Stationg, Teaching"
	UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< ������ Port ���� ex) EQ, STB-L, STB-R, UTB"
	BYTE IsCheck;                      ///<  0x00 : MapMaker&ƼĪX, 0x01 : MapMaker&ƼĪ ������ 0x02 : MapMaker&ƼĪ �Ϸ�  //0x03 : MapMaker&ƼĪ fail ó��(station ����, �θ��� �ٸ�, ���)
} NODE_INFO_STD_MAPMAEKRFILE;
//stuct size : 34Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------
typedef std::map<UINT, NODE_INFO_STD_MAPMAEKRFILE *> MAPMAKERFILEDATA_MAP;
typedef MAPMAKERFILEDATA_MAP::iterator MAPMAKERFILEDATA_MAP_ITER;
typedef std::pair<UINT, NODE_INFO_STD_MAPMAEKRFILE *> MAPMAKERFILEDATA_MAP_PAIR;
typedef std::pair<MAPMAKERFILEDATA_MAP_ITER, bool> MAPMAKERFIL_DATA_MAP_RESULT;
/**
@class   STD_MapMakerFile
@author  puting
@date    2016.12.25
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
//---------------------------------------------------------------------------

class MapMakerFileManager_STD
{
private:
	bool m_bDataLoaded; ///< Map File Load Flag
	bool m_bDataModified;   ///< Data ���� flag
	MAPMAKERFILE_HEADER_STD m_MapMakerFileHeader; ///< Map File Header
//	LogUnit *pLogMapMakefile; ///< Map Make STD���� �α� ����

public:
	MAPMAKERFILEDATA_MAP m_MapMakerFileData;    ///< Station Data �ڷᱸ��(Map)
	//HEADER_INFO_STD_MAP
	
	MapMakerFileManager_STD();
	~MapMakerFileManager_STD();

	int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName);

	MAPMAKERFILE_HEADER_STD* GetFileHeader();
//	void SetFileHeader(MAPMAKERFILE_HEADER_STD *pFileHeader);         //�̻��


	// Map �ڷᱸ�� ���� �Լ�
	void Init();
	int GetCount();

	// Map �ڷᱸ�� �߰�, ������Ʈ, ����, �˻�  �Լ�
	int Insert(NODE_INFO_STD_MAPMAEKRFILE *addStationData);
//	int Update(NODE_INFO_STD_MAPMAEKRFILE *updateSTD);
//	int Delete(BYTE MapMakerType, UINT StationID);
//	NODE_INFO_STD_MAPMAEKRFILE* Find(UINT nTypeAndStation);
//	NODE_INFO_STD_MAPMAEKRFILE* Find(BYTE MapMakerType, UINT StationID);

	//Map Maker�� ƼĪ �ʿ��� �κ� Ȯ��.
//	NODE_INFO_STD_MAPMAEKRFILE* FindUpdate(BYTE MapMakerType);               //�̻��
	bool ProcessingFindMapMakerFileUpdate(UINT CurNode, StationDataManager *TempStationManager, UINT &EndNodeID, bool &bMapMaker);
	bool ProcessingFindTeachingFileUpdate(UINT CurStation, UINT &TargetStationID, bool &bIsMove,BYTE &PortType);

	bool SucessesFindUpdate(BYTE MapMakerType, UINT StationID);

//	bool MapMakerFileFindUpdate(BYTE MapMakerType, UINT StationID);           //�̻��


	void InitFindUpdate();    //���� �߻� �� �������� �κ� �ʱ�ȭ(0x01 -> 0x00)

	//�߰�.
	bool IsMapMakeFileLoading();   //Map Maker file Loading ����
//	void InitMapMakeFileMemory(); //Loading�� Map maker File �ʱ�ȭ. ���� ���ۿϷ��Ҷ�         //�̻��

};
#endif //STD_StationH
