//---------------------------------------------------------------------------
/**
@file    MapMakeProcess.h
@version OHT 8.i
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef MapMakeProcessH
#define MapMakeProcessH


#include <utility>
#include <map>
#include "../Common/Def_DataType.h"
#include "Def_OHT.h"
#include "Def_DataFile_STD.h"

//#include "LogHandler.h" // Log �ۼ� ����


using namespace std;
// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack
/**
@brief   MapMakeProcess Data ���� ����ü
@author  puting
@date    2019.09.02
*/
typedef struct MAMMAKEPROCESS_LINK_INFO
{
	UINT LinkNID;                     ///< �ĺ��� : �б����(���� 0, ������ : 1000000) + ����NodeID
	BYTE IsCheck;                     ///< 0x00 : ������� ������, 0x01 : ������� ���� : 0x01
} MAMMAKEPROCESS_LINK_INFO;
//stuct size : 34Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------
typedef std::map<UINT, MAMMAKEPROCESS_LINK_INFO *> MAMMAKEPROCESS_MAP;
typedef MAMMAKEPROCESS_MAP::iterator MAMMAKEPROCESS_MAP_ITER;
typedef std::pair<UINT, MAMMAKEPROCESS_LINK_INFO *> MAMMAKEPROCESS_MAP_PAIR;
typedef std::pair<MAMMAKEPROCESS_MAP_ITER, bool> MAMMAKEPROCESS_DATA_MAP_RESULT;
/**
@class   STD_MapMakerFile
@author  puting
@date    2016.12.25
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
//---------------------------------------------------------------------------

class MapMakeProcess
{
private:

//	bool m_bDataModified;   ///< Data ���� flag
//	LogUnit *pLogMapMakefile; ///< Map Make STD���� �α� ����
	MAMMAKEPROCESS_MAP m_MapMakeProcessData;    ///< Station Data �ڷᱸ��(Map)
   	void Init();

	MAMMAKEPROCESS_LINK_INFO* Find(UINT LinkNID);
	int m_MapMakeDrivingCount;

public:

	//HEADER_INFO_STD_MAP
	
	MapMakeProcess();
	~MapMakeProcess();


	// Map �ڷᱸ�� ���� �Լ�
	int GetCount();
	int GetCountMapMakeDriving();

	// Map �ڷᱸ�� �߰�, ����/ �߰� ��� ����.
	int Update(MAMMAKEPROCESS_LINK_INFO *updateSTD);
	int Insert(MAMMAKEPROCESS_LINK_INFO *addStationData);

	//�߰�.
//	int GetCountMapMakeDrivingReflash();   //Map Maker file Loading ����                  //�̻��
	void InitMapMakeProcessMemory(); //Loading�� Map maker File �ʱ�ȭ. ���� ���ۿϷ��Ҷ�

};
#endif //STD_StationH
