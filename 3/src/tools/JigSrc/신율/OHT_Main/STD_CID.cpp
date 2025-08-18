/**
@file    STD_CID.cpp
@version OHT 7.5
@brief   MAP�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "STD_CID.h"
//#include "OHTMain.h"
//#include "ErrorCode.h"
#include "PUtily.h"
#include "crc16.h" 


#include <windows.h>
#include <stdio.h>
#include <string.h>


//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\CRC.log", __VA_ARGS__)
//����
//#define ADD_LOG_CID_CRC(...)    WRITE_LOG_UNIT(pLogCRC, __VA_ARGS__)

using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default ������

         ����, ������ ���� �ʱ�ȭ
@author  KDJ
@date    2016.03.30
*/
STD_CID::STD_CID()
{

	m_NodeNum = 0;

    m_NodeArray = NULL;


    // CIDMap �ʱ�ȭ
    m_CIDMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  KJD
@date    2016.03.30
@note
*/
STD_CID::~STD_CID()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;


    m_CIDMap.clear();


}

/**
@brief   MAP Class ������
		 Node ������ �̿��Ͽ� ���� �����Ѵ�.
@author  KJD
@date    2016.03.30
@param   NodeNum : Node ����
*/
void STD_CID::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // ���� �����ϱ� �� Null������ Ȯ�� �� ����
    if(m_NodeArray != NULL)
    {
		// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		// �� ������ ���Ž� ���
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
    }
    else
    {
    	//NodeNum Max������ �׽�Ʈ �ʿ� ??
		m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
    }

    m_CIDMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager Ŭ���� ����Ʈ ������
         ���� �ʱ�ȭ
@author  KJD
@date    2016.03.30

*/
CIDManager_STD::CIDManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_CIDFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� �Ҹ���
         FileStream ���� �޸� ����
@author  KJD
@date    2016.03.30

*/
CIDManager_STD::~CIDManager_STD()
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
int CIDManager_STD::LoadMapFile(TCHAR *FileName, STD_CID *pMapdata)
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
        memset(&m_CIDFileHeader, 0x00, sizeof(m_CIDFileHeader));

        // Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));

		fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));

        // Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
		//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
		memcpy(&m_CIDFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
		ConvertEndian(&m_CIDFileHeader.ModelID);
		ConvertEndian(&m_CIDFileHeader.Version);
		ConvertEndian(&m_CIDFileHeader.DataCount);
		ConvertEndian(&m_CIDFileHeader.DataType);
		
        if( (m_CIDFileHeader.DataCount > 0) && (m_CIDFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data �� size
            // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
			pMapdata->InitMap(m_CIDFileHeader.DataCount);

			// Step 5. Node Size��ŭ Read
			NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);

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
//					ADD_LOG_CID_CRC("[CRC16_ERR] CID File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG_CID_CRC("[EOF_ERR] CID File");
				//�� ���� ���� �� �ּ� �κ� ����
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

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
int CIDManager_STD::SaveMapFile(TCHAR *FileName, STD_CID *pMapdata)
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
		if( (m_CIDFileHeader.DataCount == 0) || (m_CIDFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_CIDFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_CIDFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_CIDFileHeader.DataCount = STD_HEADER.DataCount;    // Node ����

        // step4. Header, Node ���� write
		if(m_CIDFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_CID));
			//fs.write((char *)&m_CIDFileHeader, sizeof(HEADER_INFO_STD_CID));
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);
			// Header ���� wirte
			fs.write((char *)&m_CIDFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_CIDFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

			}
            // Node ���� write
			int NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].ResetNode);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode1);
				ConvertEndian(&pMapdata->m_NodeArray[i].StopNode2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode1_Offset);
				
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_1);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_2);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_3);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_4);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_5);
				ConvertEndian(&pMapdata->m_NodeArray[i].CommStartNode2_Offset);

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
