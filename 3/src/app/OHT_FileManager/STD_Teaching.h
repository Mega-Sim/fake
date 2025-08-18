//---------------------------------------------------------------------------
/**
@file    STD_TeachingH.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_TeachingH
#define STD_TeachingH

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
@brief   TEACHING.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Teaching
{
	UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< ������ Port ���� ex) EQ, STB-L, STB-R, UTB"
	int Hoist;                         ///< ������ Hoist �̵��Ÿ�(1/10mm)
	int Shift;                         ///< ������ Shift �̵��Ÿ�(1/10mm)
	int Rotate;                        ///< ������ Rotate �̵��Ÿ�(radian)
	BYTE PIOType;                       ///< 0:IR, 1:RF, 2:Hybrid 5.0GHz
	BYTE PIODirection;                  ///< PIO ����(��, ��)
	UINT PIOID;                         ///< RF-PIO Setting ID ����
	UINT PIOChannel;                    ///< RF-PIO Setting Channel ����
	BYTE PIOTimeLevel;                  ///< PIO Time out ���� �ð� ����
	BYTE AutoRecovery;                  ///< AutoRecovery
	BYTE HandDetectEQ;                  ///< HandDetectEQ
	UINT Reserved1;                     ///< Reserved 1
} NODE_INFO_STD_TEACHING;
//stuct size : 34Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_Station
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_Teaching
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_TEACHING *m_NodeArray; ///< Node ���� �迭 ������

	std::map<int, int> m_TeachingMap;

	STD_Teaching();
	~STD_Teaching();

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
class TeachingManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_TeachingFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	TeachingManager_STD();
	~TeachingManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Teaching *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Teaching *pMapdata);

};
#endif //STD_StationH
