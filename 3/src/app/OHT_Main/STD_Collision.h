//---------------------------------------------------------------------------
/**
@file    STD_Collision.h
@version OHT 7.5
@brief   STANDARD MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef STD_CollisionH
#define STD_CollisionH

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
@brief   COLLISION.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_Collision
{
	UINT NodeID;                        ///< ���� Node Id
	UINT LeftNodeID;                    ///< (����) ����� Node Id
	BYTE LeftUBGRegion;                 ///< (����) ����� Point������ ������������ ����
	BYTE LeftObstacleRegion;            ///< (����) ����� Point������ ��ֹ��������� ����
	BYTE LeftObstacleStatus;            ///< (����) ��ֹ��������� ��� ���� (0: �̻��, 1: ���)
   //	UINT Reserved1;
#if(USE_CHANGENODE == 1)
	// for change node (jhoun.lim)
	BYTE LeftOptUBGRegion;
	BYTE LeftOptObstacleRegion;

	BYTE L_Reserved1;
	BYTE L_Reserved2;

	UINT RightNodeID;                   ///< (������) ����� Node Id
	BYTE RightUBGRegion;                ///< (������) ����� Point������ ������������ ����
	BYTE RightObstacleRegion;           ///< (������) ����� Point������ ��ֹ��������� ����
	BYTE RightObstacleStatus;           ///< (������) ��ֹ��������� ��� ���� (0: �̻��, 1: ���)

	// for change node (jhoun.lim)
	BYTE RightOptUBGRegion;
	BYTE RightOptObstacleRegion;

	BYTE R_Reserved1;
	BYTE R_Reserved2;
#else
	BYTE L_Reserved1;
	BYTE L_Reserved2;
	BYTE L_Reserved3;
	BYTE L_Reserved4;
	UINT RightNodeID;                   ///< (������) ����� Node Id
	BYTE RightUBGRegion;                ///< (������) ����� Point������ ������������ ����
	BYTE RightObstacleRegion;           ///< (������) ����� Point������ ��ֹ��������� ����
	BYTE RightObstacleStatus;           ///< (������) ��ֹ��������� ��� ���� (0: �̻��, 1: ���)
//	UINT Reserved2;
	BYTE R_Reserved1;
	BYTE R_Reserved2;
	BYTE R_Reserved3;
	BYTE R_Reserved4;
#endif


} NODE_INFO_STD_COLLISION;

#pragma pack(pop) // Pack ����

// -------------------------------------------

/**
@class   STD_Collision
@author  KJD
@date    2016.03.30
@brief   OHT�� ���Ǵ� Standard Map�� ���õ� Data, �Լ��� ������ class
*/
class STD_Collision
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_COLLISION *m_NodeArray; ///< Node ���� �迭 ������

	std::map<int, int> m_CollisionMap;
	void MakeCollisionMap_STD();

	STD_Collision();
	~STD_Collision();

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
class CollisionManager_STD
{
private:
//	LogUnit *pLogCRC;      ///< CRC �α� ������ ���� ����
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_CollisionFileHeader; ///< Map File Header
	//HEADER_INFO_STD_MAP
	
	CollisionManager_STD();
	~CollisionManager_STD();

	int LoadMapFile(TCHAR *FileName, STD_Collision *pMapdata);
	int SaveMapFile(TCHAR *FileName, STD_Collision *pMapdata);


};
#endif //STD_CollisionH


