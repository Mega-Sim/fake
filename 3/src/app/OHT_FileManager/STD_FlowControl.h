//---------------------------------------------------------------------------
/**
@file    STD_FlowControl.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_FlowControlH
#define STD_FlowControlH

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
@brief   FLOWCONTROL.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_FlowControl
{
	UINT StopNodeID;                    ///< Stop Node Id
	UINT ResetNodeMain;                 ///< Reset Node ID(Main) - �շ��� ��� Main���� �Է�"
	BYTE BZStopMain;                    ///< BZ Stop ����
	UINT ResetNodeMainDefaultValue;     ///< Reset Node ID(Main)�� ���� ��� Default ��
	UINT ResetNodeMainDefaultRatio;     ///< Reset Node ID(Main)�� ���� ��� Default �� * (0 ~ 200%)
	UINT ResetNodeSub;                  ///< Reset Node ID(Sub.) - �б��� ��� Main�� Sub.�� ��� �Է�"
	BYTE BZStopSub;                     ///< BZ Stop ����
	UINT ResetNodeSubDefaultValue;      ///< Reset Node ID(Sub.)�� ���� ��� Default ��
	UINT ResetNodeSubDefaultRatio;      ///< Reset Node ID(Sub.)�� ���� ��� Default �� * (0 ~ 200%)
	UINT ResetNodeRedundance1;          ///< Reset Node ID(Redundance 1)
	BYTE BZStopRedundance1;             ///< BZ Stop ����
	UINT ResetNodeRedundance1DefaultValue;        ///< Reset Node ID(Redundance 1)�� ���� ��� Default ��
	UINT ResetNodeRedundance1DefaultRatio;        ///< Reset Node ID(Redundance 1)�� ���� ��� Default �� * (0 ~ 200%)
	UINT ResetNodeRedundance2;          ///< Reset Node ID(Redundance 2)
	BYTE BZStopRedundance2;             ///< BZ Stop ����
	UINT ResetNodeRedundance2DefaultValue;        ///< Reset Node ID(Redundance 2)�� ���� ��� Default ��
	UINT ResetNodeRedundance2DefaultRatio;        ///< Reset Node ID(Redundance 2)�� ���� ��� Default �� * (0 ~ 200%)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2
	UINT Reserved3;                     ///< Reserved_3
	UINT Reserved4;                     ///< Reserved_4
} NODE_INFO_STD_FLOWCONTROL;
//stuct size : 72Byte

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_FlowControl
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_FlowControl
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_FLOWCONTROL *m_NodeArray; ///< Node ���� �迭 ������


	std::map<int, int> m_FlowControlMap;
	void MakeFlowControlMap_STD();

	STD_FlowControl();
	~STD_FlowControl();

	void InitMap(int NodeNum);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
@class   MapDataManager
@author  KJD
@date    2016.03.30
@brief   FlowControl Data�� �����ϴ� Class
*/
class FlowControlManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_FlowControlFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	FlowControlManager_STD();
	~FlowControlManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_FlowControl *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_FlowControl *pMapdata);

};
#endif //STD_FlowControlH
