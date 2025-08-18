/**
@file    STD_Collision.cpp
@version OHT 7.5
@brief   STD_Collision�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "STD_Collision.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>



//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CRC.log", __VA_ARGS__)
 //����
//#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogCRC, __VA_ARGS__)

using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default ������

         ����, ������ ���� �ʱ�ȭ
@author  KDJ
@date    2016.03.30
*/
STD_Collision::STD_Collision()
{

    m_NodeNum = 0;

    m_NodeArray = NULL;

    // CollisionMap �ʱ�ȭ
    m_CollisionMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  KJD
@date    2016.03.30
@note
*/
STD_Collision::~STD_Collision()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;

    m_CollisionMap.clear();


}

/**
@brief   MAP Class ������
		 Node ������ �̿��Ͽ� ���� �����Ѵ�.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node ����
*/
void STD_Collision::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // ���� �����ϱ� �� Null������ Ȯ�� �� ����
    if(m_NodeArray != NULL)
    {
		// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		// �� ������ ���Ž� ���
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
    }
    else
    {
    	//NodeNum Max������ �׽�Ʈ �ʿ� ??
		m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
    }

    m_CollisionMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager Ŭ���� ����Ʈ ������
         ���� �ʱ�ȭ
@author  KJD
@date    2016.03.30

*/
CollisionManager_STD::CollisionManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC1.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_CollisionFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� �Ҹ���
         FileStream ���� �޸� ����
@author  KJD
@date    2016.03.30

*/
CollisionManager_STD::~CollisionManager_STD()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data�� File���� �ҷ����� �Լ�
@author  KJD
@date    2016.03.30
@param   FileName : Map Data�� ����� �����̸�(MAPDATA.DAT)
@param   pMapdata : Map Data�� ����� ����ü ������
@return  ������ ��� 0�� ����, ������ ��� ���� �ڵ� ����
@todo    ���� �ڵ� ���� �ʿ�
*/
int CollisionManager_STD::LoadMapFile(TCHAR *FileName, STD_Collision *pMapdata)
//int CollisionManager_STD::LoadMapFile(TCHAR *FileName)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    int Error = NO_ERR;
	unsigned short cCRC16_file;	
	unsigned short cCRC16_header;
	unsigned char cEOF[3];

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::in | ios::binary);

    if(fs.is_open())
    {
        memset(&m_CollisionFileHeader, 0x00, sizeof(m_CollisionFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));
		

        // Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
		//m_CollisionFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_CollisionFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_CollisionFileHeader.ModelID);
		ConvertEndian(&m_CollisionFileHeader.Version);
		ConvertEndian(&m_CollisionFileHeader.DataCount);
		ConvertEndian(&m_CollisionFileHeader.DataType);
		
        if( (m_CollisionFileHeader.DataCount > 0) && (m_CollisionFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data �� size
            // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
			pMapdata->InitMap(m_CollisionFileHeader.DataCount);

			// Step 5. Node Size��ŭ Read
			NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);

			fs.read((char *)pMapdata->m_NodeArray, NodeSize);
			fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
			fs.read((char *)&cEOF, sizeof(cEOF));
			ConvertEndian(&cCRC16_file);
			cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
			if(cEOF[0]=='E' && cEOF[1]=='O' && cEOF[2]=='F') //EOF �� ������ ���� �� ������ ����
			{
				if(cCRC16_header != cCRC16_file)
				{
					fs.close();
//					ADD_LOG("[CRC16_ERR] Collision File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG("[EOF_ERR] Collision File");
				//�� ���� ���� �� �ּ� �κ� ����
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
            for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);
			}
			m_MapFileLoaded = true; // Map File Loaded flag ����
		}
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
	}

    fs.close();
    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data�� File�� �����ϴ� �Լ�
@author  KJD
@date    2016.03.30
@param   FileName : Map Data�� ������ ���� �̸�
@param   pMapdata : ������ Map Data ����ü ������
@return  ������ ��� 0�� ����, ������ ��� ���� �ڵ� ����
@todo    ���� �ڵ� ���� �ʿ�
*/
int CollisionManager_STD::SaveMapFile(TCHAR *FileName, STD_Collision *pMapdata)
{

	//setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

	int Error = NO_ERR;
	unsigned short cCRC16_header;
	unsigned char cEOF[3]={'E','O','F'};

    ofstream fs;

    // Step 1. File Open
	fs.open(FileName, ios::out | ios::binary);

    if(fs.is_open())
    {
        // Step 2. Write File Header Information
		if( (m_CollisionFileHeader.DataCount == 0) || (m_CollisionFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_CollisionFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_CollisionFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_CollisionFileHeader.DataCount = STD_HEADER.DataCount;    // Node ����

        // step4. Header, Node ���� write
		if(m_CollisionFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_Collision));
			//fs.write((char *)&m_CollisionFileHeader, sizeof(HEADER_INFO_STD_Collision));
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);
			// Header ���� wirte
			fs.write((char *)&m_CollisionFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_CollisionFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);

			}
            // Node ���� write
			int NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);

			}
			fs.write((char *)&cCRC16_header, sizeof(cCRC16_header));
			fs.write((char *)&cEOF, sizeof(cEOF));
		}
		fs.close();
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
    }

	return Error;

}
//---------------------------------------------------------------------------
/**
@brief   Node ����ü�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author  KJD
@date    2016.04.29

*/
#if 0
void STD_Collision::MakeCollisionMap_STD()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
		return;

    for(int i = 0; i < m_NodeNum; i++)
    {
        // Key�� Path ID, Value�� �迭 Index
		m_CollisionMap[m_NodeArray[i].NodeID] = i;
	}
}
#endif 
#pragma package(smart_init)
