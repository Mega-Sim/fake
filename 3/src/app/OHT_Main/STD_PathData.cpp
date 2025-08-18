/**
@file    STD_PATHDATA.cpp
@version OHT 7.5
@brief   MAP�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "STD_PathData.h"
//#include "OHTMain.h"
#include "ErrorCode.h"
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
@date    2016.05.08
*/
STD_PathData::STD_PathData()
{
    m_NodeNum = 0;

    m_NodeArray = NULL;

    // PathDataMap �ʱ�ȭ
    m_PathDataMap.clear();

}
//---------------------------------------------------------------------------

/**
@brief   MAP Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  KJD
@date    2016.05.08
@note
*/
STD_PathData::~STD_PathData()
{
    if(m_NodeArray != NULL)
        delete [] m_NodeArray;

    m_PathDataMap.clear();


}

/**
@brief   MAP Class ������
		 Node ������ �̿��Ͽ� ���� �����Ѵ�.
@author  KJD
@date    2016.05.08
@param   NodeNum : Node ����
*/
void STD_PathData::InitMap(int NodeNum)
{
    m_NodeNum = NodeNum;

    // ���� �����ϱ� �� Null������ Ȯ�� �� ����
    if(m_NodeArray != NULL)
    {
		// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		// �� ������ ���Ž� ���
        delete [] m_NodeArray;
		m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
    }
    else
    {
    	//NodeNum Max������ �׽�Ʈ �ʿ� ??
		m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
    }

    m_PathDataMap.clear();

}

//---------------------------------------------------------------------------
/**
@brief   MapDatamanager Ŭ���� ����Ʈ ������
         ���� �ʱ�ȭ
@author  KJD
@date    2016.05.08

*/
PathDataManager_STD::PathDataManager_STD()
{
//	pLogCRC = getLogUnit("D:\\log\\CRC.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_MapFileLoaded = false;

	memset(&m_PathDataFileHeader, 0x00, sizeof(FILE_HEADER_STD));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� �Ҹ���
         FileStream ���� �޸� ����
@author  KJD
@date    2016.05.08

*/
PathDataManager_STD::~PathDataManager_STD()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data�� File���� �ҷ����� �Լ�
@author  KJD
@date    2016.05.08
@param   FileName : Map Data�� ����� �����̸�(PATHDATA.DAT)
@param   pMapdata : Map Data�� ����� ����ü ������
@return  ������ ��� 0�� ����, ������ ��� ���� �ڵ� ����
@todo    ���� �ڵ� ���� �ʿ�
*/
int PathDataManager_STD::LoadMapFile(TCHAR *FileName, STD_PathData *pMapdata)
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
        memset(&m_PathDataFileHeader, 0x00, sizeof(m_PathDataFileHeader));

		// Step 2. Read File Header Information
		FILE_HEADER_STD FileHeader;
		memset(&FileHeader, 0x00, sizeof(FileHeader));

        fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));
		

        // Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
        //m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
        memcpy(&m_PathDataFileHeader,&FileHeader,sizeof(FILE_HEADER_STD));
        ConvertEndian(&m_PathDataFileHeader.ModelID);
        ConvertEndian(&m_PathDataFileHeader.Version);
        ConvertEndian(&m_PathDataFileHeader.DataCount);
        ConvertEndian(&m_PathDataFileHeader.DataType);
		
        if( (m_PathDataFileHeader.DataCount > 0) && (m_PathDataFileHeader.DataCount < NODE_COUNT_MAX) )
        {
        	int NodeSize = 0; // Node Data �� size
            // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
			pMapdata->InitMap(m_PathDataFileHeader.DataCount);

			// Step 5. Node Size��ŭ Read
			NodeSize = m_PathDataFileHeader.DataCount * sizeof(NODE_INFO_STD_PATHDATA);

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
//					ADD_LOG("[CRC16_ERR] Path Data File: cCRC16_header: 0x%x, cCRC16_file: 0x%x",cCRC16_header, cCRC16_file);
					Error = ERR_MAP_FILEOPENFAIL;
					return Error;
				}
			}
			else
			{
//				ADD_LOG("[EOF_ERR] Path Data File");
				//�� ���� ���� �� �ּ� �κ� ����
				//Error = ERR_MAP_FILEOPENFAIL;
				//fs.close();
				//return Error;
			}
			
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDOut);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDOut);

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
int PathDataManager_STD::SaveMapFile(TCHAR *FileName, STD_PathData *pMapdata)
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
		if( (m_PathDataFileHeader.DataCount == 0) || (m_PathDataFileHeader.DataCount > NODE_COUNT_MAX))
		{
			Error = ERR_MAP_FILESAVEFAIL;
		}
		// Step 3. Make Header
        //String strTime;
        //strTime = Now().FormatString("yyyymmddhhnnss");
		//m_PathDataFileHeader.UpdateTime = StrToInt64("0x" + strTime);
		//m_PathDataFileHeader.VersionString = StrToInt64("0x" + strTime);

		//m_PathDataFileHeader.DataCount = STD_HEADER.DataCount;    // Node ����

        // step4. Header, Node ���� write
		if(m_PathDataFileHeader.DataCount > 0)
        {
			//fs.write((char *)&FileHeader, sizeof(HEADER_INFO_STD_CID));
			//fs.write((char *)&m_CIDFileHeader, sizeof(HEADER_INFO_STD_CID));
			ConvertEndian(&m_PathDataFileHeader.ModelID);
			ConvertEndian(&m_PathDataFileHeader.Version);
			ConvertEndian(&m_PathDataFileHeader.DataCount);
			ConvertEndian(&m_PathDataFileHeader.DataType);
			// Header ���� wirte
			fs.write((char *)&m_PathDataFileHeader, sizeof(FILE_HEADER_STD));
			cCRC16_header = crc16_ccitt((const char *)&m_PathDataFileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&cCRC16_header);
			ConvertEndian(&m_PathDataFileHeader.ModelID);
			ConvertEndian(&m_PathDataFileHeader.Version);
			ConvertEndian(&m_PathDataFileHeader.DataCount);
			ConvertEndian(&m_PathDataFileHeader.DataType);

			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDOut);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDOut);

			}
            // Node ���� write
			int NodeSize = m_PathDataFileHeader.DataCount * sizeof(NODE_INFO_STD_PATHDATA);
			fs.write((char *) pMapdata->m_NodeArray, NodeSize);
			for(int i = 0; i < pMapdata->m_NodeNum; i++)
			{
				ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDOut);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDIn);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDMiddle);
				ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDOut);

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
bool PathDataManager_STD::find(STD_PathData *pMapdata,int NodeID, NODE_INFO_STD_PATHDATA* pPathData)
{
	bool bResult =false;

	for(int Index =0; Index <pMapdata->m_NodeNum; Index++)
	{
		if(pMapdata->m_NodeArray[Index].LeftNodeIDIn == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
		}
		else if(pMapdata->m_NodeArray[Index].LeftNodeIDMiddle == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
		}
		else if(pMapdata->m_NodeArray[Index].RightNodeIDIn == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
		}
		else if(pMapdata->m_NodeArray[Index].RightNodeIDMiddle == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
        }
        else if(pMapdata->m_NodeArray[Index].LeftNodeIDOut == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
		}
		else if(pMapdata->m_NodeArray[Index].RightNodeIDOut == NodeID)
		{
			memmove(pPathData, &pMapdata->m_NodeArray[Index], sizeof(NODE_INFO_STD_PATHDATA));
			bResult = true;
			break;
		}
	}

	return bResult;

}

int PathDataManager_STD::GetPathData(STD_PathData *pMapdata, NODE_INFO_STD_PATHDATA *pPathdata,UINT NodeID)
{
	int Error = NO_ERR;

	bool bCheck =false;
//	NODE_INFO_STD_PATHDATA *pPData;

	bCheck = find(pMapdata, NodeID, pPathdata);
//

  	if (bCheck == false)
	{
//		Error = ERR_DATA_STATION_NOEXIST;

		pPathdata->LayoutType  			= 0x01;
		pPathdata->LayoutID  			= 0x01;
		pPathdata->LeftNodeIDIn  		= 20001;
		pPathdata->LeftNodeIDMiddle  	= 303;
		pPathdata->LeftNodeIDOut  		= 302;
		pPathdata->LeftDriveDirection  	= 0;
	}

	return Error;
}

#pragma package(smart_init)
