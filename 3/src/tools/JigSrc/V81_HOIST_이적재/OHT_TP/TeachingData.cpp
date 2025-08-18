//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
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
@brief   FileHeader�� ����
@author  doori.shin
@date    2013.07.26
*/
FILE_HEADER* TeachingDataManager::GetFileHeader()
{
	return &m_FileHeader;
}
//---------------------------------------------------------------------------

/**
@brief   FileHeader ����
@author  doori.shin
@date    2013.07.26
*/
void TeachingDataManager::SetFileHeader(FILE_HEADER* pHeader)
{
	memcpy(&m_FileHeader, pHeader, sizeof(FILE_HEADER));
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
@brief   Binary ���Ͽ��� Teaching Data ������ load�ϴ� �Լ�
@author  zzang9un
@date    2013.06.23
@param   TechingDataFileName : ��θ� ������ ���ϸ�
@return  ������ ��� 0�� ����, ���� �߻� �� ���� �ڵ� ����
*/
int TeachingDataManager::LoadDataFromFile(String FileName)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    int Error = NO_ERR;

	ifstream fs;

	// Step 1. File Open
	fs.open(FileName.w_str(), ios::in | ios::binary);

	if(fs.is_open())
	{
        int DataNum = 0; // Data ����
 
        Init();

        // Step 2. Read File Header ����
        FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
		fs.read((char *)&FileHeader, sizeof(FileHeader));

        // Step 3. �Ľ� Header
        m_FileHeader = ParsingFileHeader(&FileHeader);

        if(m_FileHeader.NumOfData > 0)
        {
            DataNum = m_FileHeader.NumOfData;

            TEACHING_DATA *loadTD;

            // Step 3. Station Data�� Map�� Insert��
            for(int i = 0; i < DataNum; i++)
            {
                loadTD = new TEACHING_DATA;

                fs.read((char *)loadTD, sizeof(TEACHING_DATA));

                Insert(loadTD);
            }

            if(DataNum != GetCount())
                Error = ERR_TDATA_FORMAT;
            else
            {
                // Load�� ������ ���
                m_bDataLoaded = true;
                m_bDataModified = false;
            }
        }
	}
    else
	{
		Error = ERR_TDATA_FILEOPEN;
	}

    fs.close();

    return Error;
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
int TeachingDataManager::SaveDataToFile(String FileName)
{
    int Error = NO_ERR;

    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    // ������ data�� �ִ� �� Ȯ��
    if(m_TeachingData.empty())
    {
        // Data�� ���� ���� ����
        Error = ERR_TDATA_NODATA;
    }

    ofstream fs;

    // Step 1. File open
    fs.open(FileName.w_str(), ios::out | ios::binary);

    if(fs.is_open())
    {
        int DataNum = GetCount();

		//Step 2. Write File Header Information

        // Data ����
        m_FileHeader.NumOfData = GetCount();
        
		if(m_FileHeader.NumOfData == 0)
			Error = ERR_TDATA_SAVEFAIL;
        else
            DataNum = m_FileHeader.NumOfData;

        //Step 3. Check Size and Make Header
        FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
		if(Error == NO_ERR)
            FileHeader = MakeFileHeader(&m_FileHeader);

        if(Error == NO_ERR)
        {
    		fs.write((char *)&FileHeader, sizeof(FILE_HEADER));

            // Step 3. Station Data ���� write
            TEACHING_DATA *saveData;
            TEACHING_DATA_MMAP_ITER iter;
            int SaveCnt = 0;

            for(iter = m_TeachingData.begin(); iter != m_TeachingData.end(); ++iter)
            {
                saveData = iter->second;
                fs.write((char *)saveData, sizeof(TEACHING_DATA));
                SaveCnt++;
            }

            if(DataNum != SaveCnt)
                Error = ERR_TDATA_SAVEFAIL;
            else
            {
                // ������ ������ ���
                m_bDataModified = false;
            }
        }
    }
    else
    {
        Error = ERR_TDATA_FILEOPEN;
    }

    fs.close();

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

    TEACHING_DATA_MMAP_RESULT Result;
    TEACHING_DATA_MMAP_ITER Iter;

    TEACHING_DATA *findTD;

    // ������ Station ID�� PortType�� �˻�
    findTD = Find(addTeachingData->StationID, addTeachingData->PortType);

    if(findTD == NULL)
    {
        // ���� ���� �߰�
        Iter = m_TeachingData.insert(TEACHING_DATA_PAIR(addTeachingData->StationID, addTeachingData));

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
int TeachingDataManager::Update(TEACHING_DATA *updateTD)
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
#pragma package(smart_init)
