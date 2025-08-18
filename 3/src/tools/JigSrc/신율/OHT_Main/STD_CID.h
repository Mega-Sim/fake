//---------------------------------------------------------------------------
/**
@file    STD_CID.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_CIDH
#define STD_CIDH

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
@brief   CID.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_CID
{
    BYTE LayoutType;                    ///< �շ�����, ����, MTL, Lift, �߰�(Interface), Pass Door, Fire Door
    //UINT LayoutID;                    // UI ǥ�ÿ� ID
    BYTE LayoutID[6];                   ///< UI ǥ�ÿ� ID : CID001, CID002...
    UINT ResetNode;                     ///< Layout �� Reset Node
    UINT StopNode1;                     ///< Layout �� Stop Node 1
    UINT StopNode2;                     ///< Layout �� Stop Node 2
    BYTE ResetNodeType;                 ///< Layout �� Reset Node Type
    BYTE CIDDirection1;                 ///< Stop Node 1 �� Reset Node������ CID-O ����
    UINT CommStartNode1_1;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 1
    UINT CommStartNode1_2;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 2
    UINT CommStartNode1_3;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 3
    UINT CommStartNode1_4;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 4
    UINT CommStartNode1_5;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 5
    INT  CommStartNode1_Offset;         ///< CommStartNode1_1 ���� CID���� �������� ������ �Ÿ�(mm, �⺻��0, ���ۼ��� ���谪 ����)
    BYTE CIDDirection2;                 ///< Stop Node 2 �� Reset Node������ CID-O ����
    UINT CommStartNode2_1;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 1
    UINT CommStartNode2_2;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 2
    UINT CommStartNode2_3;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 3
    UINT CommStartNode2_4;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 4
    UINT CommStartNode2_5;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 5
    INT  CommStartNode2_Offset;         ///< CommStartNode2_1 ���� CID���� �������� ������ �Ÿ�(mm, �⺻��0, ���ۼ��� ���谪 ����)
    UINT Reserved1;                     ///< Reserved_1
    UINT Reserved2;                     ///< Reserved_2
} NODE_INFO_STD_CID;
//stuct size : 79Byte
#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_CID
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_CID
{

public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_CID *m_NodeArray; ///< Node ���� �迭 ������

	std::map<int, int> m_CIDMap;

	STD_CID();
	~STD_CID();

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
class CIDManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC �α� ������ ���� ����
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_CIDFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	CIDManager_STD();
	~CIDManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_CID *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_CID *pMapdata);

};
#endif //STD_CIDH

