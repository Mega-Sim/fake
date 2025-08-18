/**
@file    STD_MAP.cpp
@version OHT 7.5
@brief   MAP�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "STD_MAP.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "LogHandler.h" // Log

#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CRC.log", __VA_ARGS__)
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
STD_MAP::STD_MAP()
{
    m_NodeNum = 0;

    m_NodeArray = NULL;

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  KJD
@date    2016.03.30
@note
*/
STD_MAP::~STD_MAP()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;



}

/**
@brief   MAP Class ������
		 Node ������ �̿��Ͽ� ���� �����Ѵ�.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node ����
*/
void STD_MAP::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // ���� �����ϱ� �� Null������ Ȯ�� �� ����
    if(m_NodeArray != NULL)
    {
		// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		// �� ������ ���Ž� ���
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
    }
    else
    {
    	//NodeNum Max������ �׽�Ʈ �ʿ� ??
		m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
    }

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager Ŭ���� ����Ʈ ������
         ���� �ʱ�ȭ
@author  KJD
@date    2016.03.30

*/
MapDataManager_STD::MapDataManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_MapFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� �Ҹ���
         FileStream ���� �޸� ����
@author  KJD
@date    2016.03.30

*/
MapDataManager_STD::~MapDataManager_STD()
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
int MapDataManager_STD::LoadMapFile(TCHAR *FileName, STD_MAP *pMapdata)
//int MapDataManager_STD::LoadMapFile(TCHAR *FileName)
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
        memset(&m_MapFileHeader, 0x00, sizeof(m_MapFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));
		

        // Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
		//m_MapFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_MapFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_MapFileHeader.ModelID);
		ConvertEndian(&m_MapFileHeader.Version);
		ConvertEndian(&m_MapFileHeader.DataCount);
		ConvertEndian(&m_MapFileHeader.DataType);
		
        if( (m_MapFileHeader.DataCount > 0) && (m_MapFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data �� size
            // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
			pMapdata->InitMap(m_MapFileHeader.DataCount);

			// Step 5. Node Size��ŭ Read
			NodeSize = m_MapFileHeader.DataCount * sizeof(NODE_INFO_STD_MAP);
			ADD_LOG("Node sizeof = %d", NodeSize);

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
//					ADD_LOG("[CRC16_ERR] Map Data File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG("[EOF_ERR] Map Data File");
				//�� ���� ���� �� �ּ� �κ� ����
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
            for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeSpeed);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeSpeed);
				ConvertEndian(&pMapdata->m_NodeArray[i].ChargeEnable);   
#if(USE_CHANGENODE == 1)
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeOptRatio);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeOptRatio);
#endif
				if(pMapdata->m_NodeArray[i].ChargeEnable != 0)   //(pMapdata->m_NodeArray[i].NodeID == 120 || pMapdata->m_NodeArray[i].NodeID == 118)
				{
					ADD_LOG("%d Charge? : %d, Left Speed:%d, Right Speed:%d", pMapdata->m_NodeArray[i].NodeID, pMapdata->m_NodeArray[i].ChargeEnable, pMapdata->m_NodeArray[i].LeftNodeSpeed, pMapdata->m_NodeArray[i].RightNodeSpeed);
//					ADD_LOG("%c, Ratio left:%d, right:%d", pMapdata->m_NodeArray[i].RightBranch, pMapdata->m_NodeArray[i].LeftNodeOptRatio, pMapdata->m_NodeArray[i].RightNodeOptRatio);
				}
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
int MapDataManager_STD::SaveMapFile(TCHAR *FileName, STD_MAP *pMapdata)
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
		if( (m_MapFileHeader.DataCount == 0) || (m_MapFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
		//String strTime;
			m_MapFileHeader.DataType |= 0x02;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_MapFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_MapFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_MapFileHeader.DataCount = STD_HEADER.DataCount;    // Node ����

        // step4. Header, Node ���� write
		if(m_MapFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_MAP));
			//fs.write((char *)&m_MapFileHeader, sizeof(HEADER_INFO_STD_MAP));
			ConvertEndian(&m_MapFileHeader.ModelID);
			ConvertEndian(&m_MapFileHeader.Version);
			ConvertEndian(&m_MapFileHeader.DataCount);
			ConvertEndian(&m_MapFileHeader.DataType);
			// Header ���� wirte
			fs.write((char *)&m_MapFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_MapFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_MapFileHeader.ModelID);
			ConvertEndian(&m_MapFileHeader.Version);
			ConvertEndian(&m_MapFileHeader.DataCount);
			ConvertEndian(&m_MapFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeSpeed);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeSpeed);
#if(USE_CHANGENODE == 1)
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeOptRatio);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeOptRatio);
#endif

			}
            // Node ���� write
			int NodeSize = m_MapFileHeader.DataCount * sizeof(NODE_INFO_STD_MAP);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].NodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeSpeed);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeID);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeDistance);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeSpeed);
#if(USE_CHANGENODE == 1)
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeOptRatio);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeOptRatio);
#endif

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


#pragma package(smart_init)
