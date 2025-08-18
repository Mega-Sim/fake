//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "OHTMain.h"

using namespace std;
#pragma hdrstop

#include "TeachingData.h"
#include "Def_OHT.h"
//---------------------------------------------------------------------------

/**
@brief   TeachingDataManager Ŭ���� ������
@author  zzang9un
@date    2013.06.23
*/
TeachingDataManager::TeachingDataManager()
{
    Init();
}
//---------------------------------------------------------------------------

/**
@brief   TeachingDataManager Ŭ���� �Ҹ���
@author  zzang9un
@date    2013.06.23
*/
TeachingDataManager::~TeachingDataManager()
{
    // �޸� ����
    for(TEACHING_DATA_MMAP_ITER iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
    {
        TEACHING_DATA *pTD = (TEACHING_DATA *)iter->second;
        delete pTD;
	}

    m_TeachingData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   ������ ������ ���� Teaching Data�� �ʱ�ȭ �Ѵ�.
@author  zzang9un
@date    2013.06.23
*/
void TeachingDataManager::Init()
{
    m_bDataLoaded    = false;
	m_bDataModified  = false;

	memset(&m_FileHeader, 0, sizeof(FILE_HEADER));

    for(TEACHING_DATA_MMAP_ITER iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
    {
        TEACHING_DATA *pTD = (TEACHING_DATA *)iter->second;
        delete pTD;
    }
    m_TeachingData.clear();
}
//---------------------------------------------------------------------------

/**
@brief   Binary ���Ͽ� Teaching Data ������ Save�ϴ� �Լ�
@author  zzang9un
@date    2013.04.25
@param   TeachingDataFileName : ��θ� ������ ���ϸ�
@return  ������ ��� 0�� ����, ���� �߻� �� ���� �ڵ� ����
@note    ���� ������ ��� �������� �÷��׸� false�� �����Ѵ�.
*/
int TeachingDataManager::SaveDataToFile(String FileName, int FileType)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    // ������ data�� �ִ� �� Ȯ��
    if(m_TeachingData.empty())
    {
        // Data�� ���� ���� ����
        Error = ERR_TDATA_NODATA;
		return Error;
    }


    // Step 1. Teaching Data �� ȹ��.
    //fs.open(FileName.w_str(), ios::out | ios::binary);

    int DataNum = GetCount();

    if( DataNum>0 )
    {

		//Step 2. Write File Header Information
        
        // ���� ���� ������ ����.
        // ����� Data ���� �� �ɼǺ�Ʈ ���� 1 bit : �Ķ���� ������bit
		if(FileType == DATATYPE_TEACHING )
		{
			//OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.Version 
			//	= OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.Version;
			OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType |= bit_1; // �Ķ���� ������bit ����
			OHTMainForm->GetTeachingDataManager_STD()->m_TeachingFileHeader.DataCount 
				= DataNum;	
			OHTMainForm->GetTeaching_STD()->m_NodeNum = DataNum;
		}
		else
		{
			//NOP
		}
        
		if(DataNum == 0)
		{
			Error = ERR_TDATA_SAVEFAIL;
		}

        //Step 3. Teaching ���� STD �������� ��ȯ �� ����
        //��� ���濡 ���� �߰� ���θ� �Ǵ������� ����.
        if(Error == NO_ERR)
        {
			TEACHING_DATA *saveData;
            TEACHING_DATA_MMAP_ITER iter;
			int SaveCnt = 0;
            
			if(FileType == DATATYPE_TEACHING )
			{
				//STD Teaching ����ü ���� �� ���Ϸ� ����.
				STD_Teaching *pMapdata;
				pMapdata = OHTMainForm->GetTeaching_STD();
				if(pMapdata->m_NodeArray != NULL)
			    {
					// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
					// �� ������ ���Ž� ���
			        delete [] pMapdata->m_NodeArray;
					pMapdata->m_NodeArray = new NODE_INFO_STD_TEACHING[DataNum];
			    }
				for(iter = m_TeachingData.begin(); iter != m_TeachingData.end(); iter++)
	            {
					saveData = iter->second;

					pMapdata->m_NodeArray[SaveCnt].StationID = saveData->StationID;
					pMapdata->m_NodeArray[SaveCnt].PortType = saveData->PortType;
					pMapdata->m_NodeArray[SaveCnt].Hoist = saveData->Hoist;
					pMapdata->m_NodeArray[SaveCnt].Shift = saveData->Shift;
					pMapdata->m_NodeArray[SaveCnt].Rotate = saveData->Rotate;
					pMapdata->m_NodeArray[SaveCnt].PIOType = saveData->PIOType;
					pMapdata->m_NodeArray[SaveCnt].PIODirection = saveData->PIODirection;
					pMapdata->m_NodeArray[SaveCnt].PIOID = saveData->PIOID;
					pMapdata->m_NodeArray[SaveCnt].PIOChannel = saveData->PIOChannel;
					pMapdata->m_NodeArray[SaveCnt].PIOTimeLevel = saveData->PIOTimeLevel;
					pMapdata->m_NodeArray[SaveCnt].AutoRecovery = saveData->TransOption.Info.AutoRecovery;
					pMapdata->m_NodeArray[SaveCnt].HandDetectEQ = saveData->TransOption.Info.HandDetectEQ;
					pMapdata->m_NodeArray[SaveCnt].STBOffSet = saveData->STBOffSet;
					pMapdata->m_NodeArray[SaveCnt].LookDown = saveData->LookDownLevel;
					pMapdata->m_NodeArray[SaveCnt].SpecialEQLevel =	saveData->SpecialEQLevel;   //AOHC-294
					pMapdata->m_NodeArray[SaveCnt].CarrierStage = saveData->PIOCS;
					pMapdata->m_NodeArray[SaveCnt].MaterialType = saveData->MaterialType;
					pMapdata->m_NodeArray[SaveCnt].Reserved2 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved3 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].Reserved4 = 0x00;
					pMapdata->m_NodeArray[SaveCnt].STBL_Shift = saveData->STBL_Shift;     //soming
					pMapdata->m_NodeArray[SaveCnt].IsDefaultMap.LOW_DefaultMapInfo = saveData->IsDefaultMap.LOW_DefaultMapInfo;

	                SaveCnt++;
	            }
				OHTMainForm->GetTeachingDataManager_STD()->SaveMapFile(FileName.w_str(), OHTMainForm->GetTeaching_STD());
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
@brief   Teaching Data�� ������ ������ �Լ�
@author  zzang9un
@date    2013.06.23
@return  Map�� ����� Data�� ������ ����
*/
int TeachingDataManager::GetCount()
{
    return m_TeachingData.size();
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data ����ü�� Map�� insert�ϴ� �Լ�
@author  zzang9un
@date    2013.06.23
@param   addStationData : �߰��� Station Data ����ü
@return  insert�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int TeachingDataManager::Insert(TEACHING_DATA *addTeachingData)
{
    int Error = NO_ERR;
	int count = 0;//Teaching node Ȯ�ο�

//    TEACHING_DATA_MMAP_RESULT Result;
    TEACHING_DATA_MMAP_ITER Iter;

    TEACHING_DATA *findTD;

    // ������ Station ID�� PortType�� �˻�
    findTD = Find(addTeachingData->StationID, addTeachingData->PortType);

    if(findTD == NULL)
	{
		// ���� ���� �߰�

        Iter = m_TeachingData.insert(TEACHING_DATA_PAIR(addTeachingData->StationID, addTeachingData));
		count = GetCount();//Teaching node Ȯ�ο�
        if(Iter == m_TeachingData.end())
        {
            Error = ERR_TDATA_INSERTFAIL;
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
        Error = ERR_TDATA_DUPLICATION;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data ����ü�� Map�� Update�ϴ� �Լ�
@author  zzang9un
@date    2013.06.23
@param   updateSTD : Update�� Teaching Data ����ü
@return  Update�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int TeachingDataManager::TeachingUpdate(TEACHING_DATA *updateTD)
{
    int Error = NO_ERR;

    TEACHING_DATA *findTD;

    findTD = Find(updateTD->StationID, updateTD->PortType);

    if(findTD == NULL)
	{
        // ���� ���� ���� ó��
        Error = ERR_TDATA_UPDATEFAIL;
    }
    else
    {
        // �ִ� ��� ������Ʈ ����
        memcpy(findTD, updateTD, sizeof(TEACHING_DATA));
        m_bDataModified = true;
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data�� �ڷᱸ�� Map���� delete�ϴ� �Լ�
@date    2013.06.23
@param   StationID : Map �˻��� ���� Key
@param   PortType : Port Type
@return  ������ ��� 0, ������ ��� ���� �ڵ� ����
*/
int TeachingDataManager::Delete(UINT StationID, BYTE PortType)
{
    int Error = NO_ERR;
    int EraseCnt = 0;

	TEACHING_DATA_MMAP_ITER Iter;
    TEACHING_DATA_MMAP_RANGE Range;

    // Station ID�� �� Data ��� �˻�
    Range = m_TeachingData.equal_range(StationID);

	for(Iter = Range.first; Iter != Range.second; Iter++)
    {
        TEACHING_DATA *pTmpData = NULL;

        pTmpData = Iter->second;

        if(PortType == pTmpData->PortType)
        {
            // PortType�� ������ ���
            delete pTmpData;    // �ش� �޸� delete

            TEACHING_DATA_MMAP_ITER ResultIter;
            ResultIter = m_TeachingData.erase(Iter);    // multimap���� ����

            if(ResultIter == m_TeachingData.end())
                Error = ERR_TDATA_DELETEFAIL;
            else
            {
                // ������ ������ ���
                m_bDataModified = true;
            }            
        }
    }

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data�� �˻��ϴ� �Լ�
@author  zzang9un
@date    2013.06.23
@param   StationID : Map �˻��� ���� Key
@param   PortType : Port Type
@return  ã�� ��� Teaching Data�� ������, �� ã�� ��� NULL�� ����
*/
TEACHING_DATA* TeachingDataManager::Find(UINT StationID, BYTE PortType)
{
    TEACHING_DATA *pReturn = NULL;  // ������ Data�� ������

    TEACHING_DATA_MMAP_ITER FindIter;
    TEACHING_DATA_MMAP_RANGE Range;

    // Station ID�� �� Data ��� �˻�
    Range = m_TeachingData.equal_range(StationID);

    bool bFind = false;
    for(FindIter = Range.first; FindIter != Range.second; FindIter++)
    {
        TEACHING_DATA *pTmpData = NULL;

        pTmpData = FindIter->second;

        if(PortType == pTmpData->PortType)
        {
            // PortType�� ������ ���
            pReturn = FindIter->second;
            bFind = true;
            break;
        }
    }

    if(!bFind)
        pReturn = NULL; // ã�� ���� ��� NULL

    return pReturn;
}
//---------------------------------------------------------------------------

/**
@brief   STD Map���κ��� 7.0 Teaching Data�� Load�ϴ� �Լ�
@author  KJD
@date    2016.04.22
@param   TBD
@return  ������ ��� 0, �׷��� ���� ��� ���� �ڵ� ����
*/
int TeachingDataManager::LoadDataFromSTDMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum)
{
	int Error = NO_ERR;
	//int count = 0;//Teaching node Ȯ�ο�
	Init();

	TEACHING_DATA *loadTD;

	OHTMainForm->GetMap()->MakeTeachingMap(m_NodeArray_Teach, DataNum);

	// Step 4. Station Data�� Map�� Insert��
	for(int i = 0; i < DataNum; i++)
	{
		loadTD = new TEACHING_DATA;

		loadTD->StationID = m_NodeArray_Teach[i].StationID;
		loadTD->PortType = m_NodeArray_Teach[i].PortType;
		loadTD->Hoist = m_NodeArray_Teach[i].Hoist;
		loadTD->Shift = m_NodeArray_Teach[i].Shift;
		loadTD->Rotate = m_NodeArray_Teach[i].Rotate;
		loadTD->PIODirection = m_NodeArray_Teach[i].PIODirection;
		loadTD->PIOTimeLevel = m_NodeArray_Teach[i].PIOTimeLevel;
		loadTD->PIOID = m_NodeArray_Teach[i].PIOID;
		loadTD->PIOType = m_NodeArray_Teach[i].PIOType;
		loadTD->PIOChannel = m_NodeArray_Teach[i].PIOChannel;
		loadTD->TransOption.Info.AutoRecovery = m_NodeArray_Teach[i].AutoRecovery & 0x01;
		loadTD->TransOption.Info.HandDetectEQ = m_NodeArray_Teach[i].HandDetectEQ & 0x01;
		loadTD->TransOption.Info.EQPIODisable_TP = 0; //STD MAP�� ����.
		loadTD->TransOption.Info.Oscillation = 1;     //STD MAP�� ����.
		loadTD->TransOption.Info.TransSound = 0;      //STD MAP�� ����.
		loadTD->TransOption.Info.RFReader = 0;        //STD MAP�� ����.
		loadTD->STBOffSet = m_NodeArray_Teach[i].STBOffSet;
		loadTD->LookDownLevel = m_NodeArray_Teach[i].LookDown;
		loadTD->SpecialEQLevel = m_NodeArray_Teach[i].SpecialEQLevel;   //AOHC-294
		loadTD->IsDefaultMap.LOW_DefaultMapInfo = m_NodeArray_Teach[i].IsDefaultMap.LOW_DefaultMapInfo;
		loadTD->STBL_Shift = m_NodeArray_Teach[i].STBL_Shift; //soming
		loadTD->PIOCS = m_NodeArray_Teach[i].CarrierStage;
		loadTD->MaterialType = m_NodeArray_Teach[i].MaterialType;

		Insert(loadTD);
		//count = GetCount();//Teaching node Ȯ�ο�
	}

	if(DataNum != GetCount())
	{
		Error = ERR_TDATA_FORMAT;
	}
	else
	{
		// Load�� ������ ���
		m_bDataLoaded = true;
		m_bDataModified = false;
	}

	return Error;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
