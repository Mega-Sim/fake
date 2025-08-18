//---------------------------------------------------------------------------


#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "OHTMain.h"

#include "StationData.h"

using namespace std;
#pragma hdrstop
//#define STATION_LOAD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\StationLoad.log", __VA_ARGS__);
//����
//#define STATION_LOAD_LOG(...)    WRITE_LOG_UNIT(pLogStationLoad, __VA_ARGS__)
//---------------------------------------------------------------------------

/**
@brief   StationDataManager Ŭ���� ������
@author  zzang9un
@date    2013.06.08
*/
StationDataManager::StationDataManager()
{
	Init();
//	pLogStationLoad = getLogUnit("D:\\log\\StationLoad.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
}

/**
@brief   StationDataManager Ŭ���� �Ҹ���
@author  zzang9un
@date    2013.06.08
*/
StationDataManager::~StationDataManager()
{
    // �޸� ����
    for(STATION_DATA_MAP_ITER iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
    {
        STATION_DATA *pSTD = (STATION_DATA *)iter->second;
        delete pSTD;
	}

    m_StationData.clear();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   Data�� File�� save�ϴ� �Լ�
@author  zzang9un
@date    2013.06.09
@param   FileName : File Name(��θ� ��ü ����)
@return  ������ ��� 0, �׷��� ���� ��� ���� �ڵ� ����
*/
int StationDataManager::SaveDataToFile(String FileName, int FileType)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    // ������ data�� �ִ� �� Ȯ��
    if(m_StationData.empty())
    {
        // Data�� ���� ���� ����
        Error = ERR_STDATA_NODATA;
		return Error;
    }


    // Step 1. File open
    //fs.open(FileName.w_str(), ios::out | ios::binary);

   
    int DataNum = GetCount();

    if( DataNum>0 )
    {

		//Step 2. Write File Header Information
        
        // ���� ���� ������ ����.
        // ����� Data ���� �� �ɼǺ�Ʈ ���� 1 bit : �Ķ���� ������bit
		if(FileType == DATATYPE_STATION )
		{
			OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataType |= bit_1; // �Ķ���� ������bit ����
			OHTMainForm->GetStationDataManager_STD()->m_StationFileHeader.DataCount = DataNum;
			OHTMainForm->GetStation_STD()->m_NodeNum = DataNum;
		}
		else
		{
			//NOP
		}
        
		
        //Step 3. Teaching ���� STD �������� ��ȯ �� ����
        //��� ���濡 ���� �߰� ���θ� �Ǵ������� ����.
        if(Error == NO_ERR)
        {
            STATION_DATA *saveData;
            STATION_DATA_MAP_ITER iter;
            int SaveCnt = 0;
            
			if(FileType == DATATYPE_STATION )
			{
				//STD Station ����ü ���� �� ���Ϸ� ����.
				STD_Station *pMapdata;
				pMapdata = OHTMainForm->GetStation_STD();

				for(iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
	            {
					saveData = iter->second;

					pMapdata->m_NodeArray[SaveCnt].StationID = saveData->StationID;
					pMapdata->m_NodeArray[SaveCnt].NextNodeID = saveData->NextNodeID;
					pMapdata->m_NodeArray[SaveCnt].Offset = saveData->StationOffset;
					pMapdata->m_NodeArray[SaveCnt].ParentNodeID = saveData->ParentNodeID;
					pMapdata->m_NodeArray[SaveCnt].StationType = saveData->StationType;
					pMapdata->m_NodeArray[SaveCnt].Safety_Level = saveData->Safety_Level;
					pMapdata->m_NodeArray[SaveCnt].PhysicalID = saveData->PhysicalID;
					pMapdata->m_NodeArray[SaveCnt].Reserved1 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved2 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved3 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved4 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved5 = 0x00;
//					pMapdata->m_NodeArray[SaveCnt].Reserved6 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].IsDefaultMap.LOW_DefaultMapInfo = saveData->IsDefaultMap.LOW_DefaultMapInfo;  //���� �ʿ�.
					
					SaveCnt++;
				}
				OHTMainForm->GetStationDataManager_STD()->SaveMapFile(FileName.w_str(), OHTMainForm->GetStation_STD());
			}
			else
			{
			}

            if(DataNum != SaveCnt)
            {
                Error = ERR_TDATA_SAVEFAIL;
            }
            else
            {
                // ������ ������ ���
                m_bDataModified = false;
            }
        }
    }
    else
    {
        Error = ERR_TDATA_NODATA;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   StationDataManager �ʱ�ȭ
@author  zzang9un
@date    2013.06.08
*/
void StationDataManager::Init()
{
    m_bDataLoaded    = false;
    m_bDataModified  = false;

	memset(&m_FileHeader, 0, sizeof(FILE_HEADER));

    for(STATION_DATA_MAP_ITER iter = m_StationData.begin(); iter != m_StationData.end(); iter++)
    {
        STATION_DATA *pSTD = (STATION_DATA *)iter->second;
        delete pSTD;
	}

	m_StationData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data�� ������ ������ �Լ�
@author  zzang9un
@date    2013.06.13
@return  Map�� ����� Data�� ������ ����
*/
int StationDataManager::GetCount()
{
    return m_StationData.size();
}
//---------------------------------------------------------------------------

/**
@brief   Station Data ����ü�� Map�� insert�ϴ� �Լ�
@author  zzang9un
@date    2013.06.13
@param   addStationData : �߰��� Station Data ����ü
@return  insert�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int StationDataManager::Insert(STATION_DATA *addStationData)
{
    int Error = NO_ERR;

    STATION_DATA_MAP_RESULT Result;

    STATION_DATA *findSTD;

    findSTD = Find(addStationData->StationID);

    if(findSTD == NULL)
    {
        // ���� ���� �߰�
        Result = m_StationData.insert(STATION_DATA_PAIR(addStationData->StationID, addStationData));

        if(Result.second == false)
        {
            Error = ERR_STDATA_INSERTFAIL;            
//			STATION_LOAD_LOG("STATION ERR_STDATA_INSERTFAIL : %d", addStationData->StationID);
        }
        else
        {
            // �߰� ������ ���
            m_bDataModified = true;
        }
    }
    else
    {
        // �ִ� ��� �ߺ� ���� ó��
        Error = ERR_STDATA_DUPLICATION;
//		STATION_LOAD_LOG("STATION ERR_STDATA_DUPLICATION : %d", addStationData->StationID);
    }

    return Error;           
}
//---------------------------------------------------------------------------

/**
@brief   Station Data ����ü�� Map�� Update�ϴ� �Լ�
@author  zzang9un
@date    2013.06.13
@param   updateSTD : Update�� Station Data ����ü
@return  Update�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int StationDataManager::Update(STATION_DATA *updateSTD)
{
    int Error = NO_ERR;

    STATION_DATA *findSTD;

    findSTD = Find(updateSTD->StationID);

    if(findSTD == NULL)
    {
        // ���� ���� ���� ó��
        Error = ERR_STDATA_UPDATEFAIL;
    }
    else
    {
        // �ִ� ��� ������Ʈ ����
        memcpy(findSTD, updateSTD, sizeof(STATION_DATA));
        m_bDataModified = true;
    }

    return Error;           
}
//---------------------------------------------------------------------------

/**
@brief   Station Data�� �ڷᱸ�� Map���� delete�ϴ� �Լ�
@date    2013.06.13
@param   StationID : Map �˻��� ���� Key
@return  ������ ��� 0, ������ ��� ���� �ڵ� ����
*/
int StationDataManager::Delete(UINT StationID)
{
    int Error = NO_ERR;
    
    int DelCnt = 0;
    
	STATION_DATA_MAP_ITER FindIter;

	FindIter = m_StationData.find(StationID);

	//Error üũ, ���ٸ� ����
	if(FindIter == m_StationData.end())
		Error = ERR_STDATA_DELETEFAIL;
    else
    {
        // �ִ� ��� �޸� ������ ���� Find
        STATION_DATA *tmpSTD;

        tmpSTD = Find(StationID);

        delete tmpSTD;  // �޸� ����

        DelCnt = m_StationData.erase(StationID);

        if(DelCnt == 1)
            m_bDataModified = true;
        else
            Error = ERR_STDATA_DELETEFAIL;   
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Station Data�� �˻��ϴ� �Լ�
@author  zzang9un
@date    2013.06.09
@param   StationID : Map �˻��� ���� Key
@return  ã�� ��� Station Data�� ������, �� ã�� ��� NULL�� ����
*/
STATION_DATA* StationDataManager::Find(UINT StationID)
{
    STATION_DATA_MAP_ITER FindIter;

    FindIter = m_StationData.find(StationID);

    // Error üũ
    if(FindIter == m_StationData.end())
        return NULL;    // ã�� ���� ��� Null�� ����

    return FindIter->second;
}
//---------------------------------------------------------------------------

/**
@brief   STD Map���κ��� 7.0 Station Data�� Load�ϴ� �Լ�
@author  KJD
@date    2016.04.22
@param   TBD
@return  ������ ��� 0, �׷��� ���� ��� ���� �ڵ� ����
*/
int StationDataManager::LoadDataFromSTDMap(MAP *pMapdata, NODE_INFO_STD_STATION *m_NodeArray, UINT DataNum)
{
	int Error = NO_ERR;

	Init();

	STATION_DATA *loadSTD;

	// Step 1. Station Data�� Map�� Insert��
	for(int i = 0; i < DataNum; i++)
	{
		loadSTD = new STATION_DATA;

		loadSTD->StationID = m_NodeArray[i].StationID;
		loadSTD->StationType = m_NodeArray[i].StationType;
		loadSTD->ParentNodeID = m_NodeArray[i].ParentNodeID;
		loadSTD->NextNodeID = m_NodeArray[i].NextNodeID;
		loadSTD->StationOffset = m_NodeArray[i].Offset;
		loadSTD->Safety_Level = m_NodeArray[i].Safety_Level;
		loadSTD->PhysicalID = m_NodeArray[i].PhysicalID;
		loadSTD->IsDefaultMap.LOW_DefaultMapInfo = m_NodeArray[i].IsDefaultMap.LOW_DefaultMapInfo;
		if(m_NodeArray[i].Offset > 5000)
		{
			Error = ERR_STDATA_OFFSET;
//			STATION_LOAD_LOG("STATION ERR_STDATA_OFFSET : %d", loadSTD->StationID);
		}
		int tmp = pMapdata->FindNodeIndex(loadSTD->ParentNodeID);
		if(tmp != INDEX_NONE)
		{
			if( !((pMapdata->m_NodeArray[tmp].NID == loadSTD->ParentNodeID)))
	//			  (pMapdata->m_PathArray[pMapdata->m_NodeArray[tmp].NextPathIndex[0]].EndNID == loadSTD->NextNodeID ||
	//			   pMapdata->m_PathArray[pMapdata->m_NodeArray[tmp].NextPathIndex[1]].EndNID == loadSTD->NextNodeID)))
			{
				Error = ERR_STDATA_LINK;
	//			STATION_LOAD_LOG("STATION ERR_STDATA_LINK : %d", loadSTD->StationID);
			}
			Error = Insert(loadSTD);
		}
//������ �ǵ� Ȯ�� �ʿ�
//		else
//		{
//			Error = Error;
//		}
	}

	if(DataNum != GetCount())
	{
		Error = ERR_STDATA_FORMAT;
	}
	else
	{
		m_bDataLoaded = true;
		m_bDataModified = false;
	}

	return Error;
}

int StationDataManager::MemoryInsert(STATION_DATA *addStationData)
{
	int nError = NO_ERR;

	STATION_DATA *loadSTD;
	loadSTD = new STATION_DATA;

	loadSTD->StationID = addStationData->StationID;
	loadSTD->StationType = addStationData->StationType;
	loadSTD->ParentNodeID = addStationData->ParentNodeID;
	loadSTD->NextNodeID = addStationData->NextNodeID;
	loadSTD->StationOffset = addStationData->StationOffset;
	loadSTD->Safety_Level = addStationData->Safety_Level;

	Insert(loadSTD);

	return nError;
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
