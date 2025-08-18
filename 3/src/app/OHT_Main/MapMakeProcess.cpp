/**
@file    MapMakeProcess.cpp
@version OHT 8.i
@brief   MAPMake�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "MapMakeProcess.h"
#include "PUtily.h"

//#define ADD_LOG(...)        WRITE_LOG_UNIT(pLogMapMakefile, __VA_ARGS__)

using namespace std;
#pragma hdrstop


//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Class default ������

         ����, ������ ���� �ʱ�ȭ
@author  puting
@date    2019.09.13
*/
MapMakeProcess::MapMakeProcess()
{
	Init();

//	pLogMapMakefile = getLogUnit("D:\\log\\MapMakefile.log", 1024*2);

}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  puting
@date    2019.09.13
@note
*/
MapMakeProcess::~MapMakeProcess()
{
    // �޸� ����
	for(MAMMAKEPROCESS_MAP_ITER iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); iter++)
    {
		MAMMAKEPROCESS_LINK_INFO *pSTD = (MAMMAKEPROCESS_LINK_INFO *)iter->second;
        delete pSTD;
	}

	m_MapMakeProcessData.clear();


}
/**
@brief   MapMakeProcess �ʱ�ȭ
@author  puting
@date    2019.09.13
*/
void MapMakeProcess::Init()
{


	for(MAMMAKEPROCESS_MAP_ITER iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); iter++)
    {
		MAMMAKEPROCESS_LINK_INFO *pSTD = (MAMMAKEPROCESS_LINK_INFO *)iter->second;
        delete pSTD;
	}

	m_MapMakeProcessData.clear();

	m_MapMakeDrivingCount = 0;
}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data�� ������ ������ �Լ�
@author  puting
@date    2019.09.13
@return  Map�� ����� Data�� ������ ����
*/
int MapMakeProcess::GetCount()
{
	return m_MapMakeProcessData.size();
}
//---------------------------------------------------------------------------
 /**
@brief   MapMakeProcess Data�� ������ ������ �Լ�
@author  puting
@date    2019.09.13
@return  Map�� ����� Data�� ������ ����
*/
int MapMakeProcess::GetCountMapMakeDriving()
{
	return m_MapMakeDrivingCount;
}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data ����ü�� Map�� insert�ϴ� �Լ�(MapLoading�ø� ����Ұ�)
@author  puting
@date    2019.09.13
@param   addStationData : �߰��� Station Data ����ü
@return  insert�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int MapMakeProcess::Insert(MAMMAKEPROCESS_LINK_INFO *addStationData)
{
    int Error = NO_ERR;

	MAMMAKEPROCESS_DATA_MAP_RESULT Result;

	MAMMAKEPROCESS_LINK_INFO *findSTD;


	findSTD = Find(addStationData->LinkNID);

    if(findSTD == NULL)
    {
        // ���� ���� �߰�
		Result = m_MapMakeProcessData.insert(MAMMAKEPROCESS_MAP_PAIR(addStationData->LinkNID, addStationData));

        if(Result.second == false)
        {
            Error = -1;
//			STATION_LOAD_LOG("STATION ERR_STDATA_INSERTFAIL : %d", addStationData->StationID);
        }
        else
        {
            // �߰� ������ ���
//            m_bDataModified = true;
        }
	}
    else
    {
        // �ִ� ��� �ߺ� ���� ó��
        Error = -2;
//		STATION_LOAD_LOG("STATION ERR_STDATA_DUPLICATION : %d", addStationData->StationID);
    }

	return Error;

}
//---------------------------------------------------------------------------

/**
@brief   MapMakeProcess Data ����ü�� Map�� Update�ϴ� �Լ�
@author  puting
@date    2019.09.13
@param   updateSTD : Update�� Station Data ����ü
//@return  Update�� ������ ��� 0, �׷��� ���� ��� Error Code ����
*/
int MapMakeProcess::Update(MAMMAKEPROCESS_LINK_INFO *updateSTD)
{
    int Error = NO_ERR;

	MAMMAKEPROCESS_LINK_INFO *findSTD;

	findSTD = Find(updateSTD->LinkNID);

    if(findSTD == NULL)
	{
		MAMMAKEPROCESS_LINK_INFO *newData;
		newData = new MAMMAKEPROCESS_LINK_INFO;
		memmove(newData,updateSTD, sizeof(MAMMAKEPROCESS_LINK_INFO));
		Insert(newData);
        // ���� ���� ���� ó��
    }
    else
    {
        // �ִ� ��� ������Ʈ ����
		memcpy(findSTD, updateSTD, sizeof(MAMMAKEPROCESS_LINK_INFO));
    }

    return Error;
}

//---------------------------------------------------------------------------


/**
@brief   MapMakeProcess Data�� �˻��ϴ� �Լ�
@author  puting
@date    2019.09.13
@param   StationID : Map �˻��� ���� Key
@return  ã�� ��� Station Data�� ������, �� ã�� ��� NULL�� ����
*/
MAMMAKEPROCESS_LINK_INFO* MapMakeProcess::Find(UINT LinkNID)
{
	MAMMAKEPROCESS_MAP_ITER FindIter;

	FindIter = m_MapMakeProcessData.find(LinkNID);

    // Error üũ
	if(FindIter == m_MapMakeProcessData.end())
        return NULL;    // ã�� ���� ��� Null�� ����

    return FindIter->second;
}

/*
int MapMakeProcess::GetCountMapMakeDrivingReflash()
{

	MAMMAKEPROCESS_LINK_INFO *saveData;
	MAMMAKEPROCESS_MAP_ITER iter;
	m_MapMakeDrivingCount = 0;


	for(iter = m_MapMakeProcessData.begin(); iter != m_MapMakeProcessData.end(); ++iter)
	{
		saveData = iter->second;
		if(saveData->IsCheck == 0x01)
		   m_MapMakeDrivingCount++;
	}

	return m_MapMakeDrivingCount;
}
*/


void MapMakeProcess::InitMapMakeProcessMemory()
{
 	Init();
}
#pragma package(smart_init)
