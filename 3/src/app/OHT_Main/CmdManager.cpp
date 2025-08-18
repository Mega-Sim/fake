/**
@file    CmdManager.cpp
@version OHT 7.0
@brief   CmdManager Class�� ������ CPP File
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "CmdManager.h"
//#include "OHT_TLV.h"
//---------------------------------------------------------------------------
#include "OHTMain.h"
#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

//---------------------------------------------------------------------------
#define MAX_BYTE_SIZE_OF_TASKCONTROL_LOG    1024*10

// �α� �ۼ� �Լ�
#define ADD_LOG(...)	        WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)

#define ERR_LOG(...)            WRITE_LOG_ON_THIS_FILE("D:\\log\\Error.log", __VA_ARGS__);\
								ADD_LOG(__VA_ARGS__)
//---------------------------------------------------------------------------
/**
@brief   CmdManager Ŭ���� ������
@author  zzang9un
@date    2013.04.09
@param   pCmdList : �����ؾ� �ϴ� List Ŭ���� ������
@param   pOHTMainStatus : Quick Command�� ���� ���� ����
*/
CmdManager::CmdManager()
{
    // by zzang9un 2013.08.16 : Log ����
    pLogUnit = getLogUnit("D:\\log\\CmdManager.log", MAX_BYTE_SIZE_OF_TASKCONTROL_LOG);

//	m_CmdList = OHTMainForm->GetCmdList();
	m_TaskList = OHTMainForm->GetTaskList();
    m_CycleList = OHTMainForm->GetCycleList();
	m_ResponseList = OHTMainForm->GetResponseList();
//	m_pPassOpenList = OHTMainForm->GetPassOpenList();
//	m_pPassPermitList = OHTMainForm->GetPassPermitList();        ///puting ����Ʈ������ ���⼭�ϴ� ��� �����غ���.

	m_MainStatus = OHTMainForm->GetOHTStatus();
    m_pExecuteInfo = OHTMainForm->GetExecuteInfo();

//	InitPreOCSCmdBuffer();
//	InitCmdList();
	InitTaskList();
	InitOCSResponseList();
	InitCycleList();
//	InitPassOpenList();
//	InitPassPermitList();
}
//---------------------------------------------------------------------------
/**
@brief   CmdManager Ŭ���� �Ҹ���
@author  zzang9un
@date    2012.10.30
*/
CmdManager::~CmdManager()
{
//	InitPreOCSCmdBuffer();
//	InitCmdList();
	InitTaskList();
	InitOCSResponseList();
	InitCycleList();
//	InitPassOpenList();
//	InitPassPermitList();

}
//---------------------------------------------------------------------------

/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@note    List�� CMD_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void CmdManager::InitTaskList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_TaskList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_TaskList->Acquire();
        try
        {
			delete (PKT_TASKCMDDATA *)m_TaskList->popFront();
        }
        __finally
        {
			CS_TaskList->Release();
        }
    }

    // ����Ʈ�� Clear�Ѵ�.
	CS_TaskList->Acquire();
    try
    {
		m_TaskList->clear();
    }
    __finally
    {
		CS_TaskList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����

int CmdManager::DeleteFirstTask()
{
	if(m_TaskList->getCount() == 0)
        return FALSE;

	CS_TaskList->Acquire();
    try
    {
		delete (PKT_TASKCMDDATA *)m_TaskList->popFront();
    }
	__finally
    {
		CS_TaskList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� Index�� �̿��Ͽ� Ư�� Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   Index : delete�Ϸ��� List Index
@return  Delete�� ������ ��� 1(true), �׷��� ���� ��� 0(false)�� ����
*/
int CmdManager::DeleteTask(UINT Index)
{
	if(m_TaskList->getCount() == 0)
        return FALSE;

	if(Index > m_TaskList->getCount() - 1)
    {
        // Index�� List�� �������� Ŭ ���
        return FALSE;
    }

	CS_TaskList->Acquire();
    try
    {
		delete (PKT_TASKCMDDATA *)m_TaskList->pop(Index);
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return TRUE;
}
//---------------------------------------------------------------------------

/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   pData : Add�Ϸ��� void ������
*/
void CmdManager::AddTask(void *pData)
{
	CS_TaskList->Acquire();
    try
    {
		m_TaskList->insertBack(pData);
    }
    __finally
    {
		CS_TaskList->Release();
    }
}
//---------------------------------------------------------------------------
/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   pData : Add�Ϸ��� void ������
*/
void CmdManager::AddTask(void *pData, int Index)
{
	CS_TaskList->Acquire();
    try
	{
		m_TaskList->insert(pData,Index);
    }
    __finally
    {
		CS_TaskList->Release();
    }
}
//---------------------------------------------------------------------------


/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
*/
void* CmdManager::ReferFirstTask()
{
    void *pListItem = NULL;

	pListItem = m_TaskList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2013.10.28
@return  List�� index Item�� �����͸� �����Ѵ�.
*/
void* CmdManager::ReferTask(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_TaskList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* CmdManager::PopFirstTask()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_TaskList->popFront();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� count�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� count�� �����Ѵ�.
*/
int CmdManager::GetTaskCount()
{
	return m_TaskList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  puting
@date    2012.11.16
@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void CmdManager::InitOCSResponseList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_ResponseList->getCount();
    for(int i = 0; i < nCount; i++)
    {
		CS_ResponceList->Acquire();
        try
        {
			delete (PKT_ACK_OHT2OCS *)m_ResponseList->popFront();
        }
        __finally
        {
			CS_ResponceList->Release();
		}
    }

    // ����Ʈ�� Clear�Ѵ�.
	CS_ResponceList->Acquire();
    try
    {
        m_ResponseList->clear();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����

int CmdManager::DeleteFirstOCSResponse()
{
    if(m_ResponseList->getCount() == 0)
        return FALSE;

	CS_ResponceList->Acquire();
    try
    {
		delete (PKT_ACK_OHT2OCS *)m_ResponseList->popFront();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� Index�� �̿��Ͽ� Ư�� Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   Index : delete�Ϸ��� List Index
@return  Delete�� ������ ��� 1(true), �׷��� ���� ��� 0(false)�� ����

int CmdManager::DeleteOCSResponse(UINT Index)
{
    if(m_ResponseList->getCount() == 0)
        return FALSE;

    if(Index > m_ResponseList->getCount() - 1)
    {
        // Index�� List�� �������� Ŭ ���
        return FALSE;
    }

	CS_ResponceList->Acquire();
    try
    {
		delete (PKT_ACK_OHT2OCS *)m_ResponseList->pop(Index);
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   pData : Add�Ϸ��� void ������
*/
void CmdManager::AddOCSResponse(void *pData)
{
	CS_ResponceList->Acquire();
    try
    {
        m_ResponseList->insertBack(pData);
    }
    __finally
    {
		CS_ResponceList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.

void* CmdManager::ReferFirstOCSResponse()
{
    void *pListItem = NULL;

    pListItem = m_ResponseList->referFront();

    return pListItem;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* CmdManager::PopFirstOCSResponse()
{
    void *pListItem = NULL;

	CS_ResponceList->Acquire();
    try
    {
        pListItem = m_ResponseList->popFront();
    }
    __finally
    {
		CS_ResponceList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� count�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� count�� �����Ѵ�.
*/
int CmdManager::GetOCSResponseCount()
{
    return m_ResponseList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  puting
@date    2012.11.16
@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void CmdManager::InitCycleList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_CycleList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_CycleList->Acquire();
        try
        {
            delete (PKT_CYCLECMDDATA *)m_CycleList->popFront();
        }
        __finally
        {
			CS_CycleList->Release();
        }
    }

    // ����Ʈ�� Clear�Ѵ�.
	CS_CycleList->Acquire();
    try
    {
        m_CycleList->clear();
    }
    __finally
    {
		CS_CycleList->Release();
    }

}
//---------------------------------------------------------------------------


  /**
@brief   List�� ������  Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����
*/
int CmdManager::DeleteBackCycle()
{
	if(m_CycleList->getCount() == 0)
		return FALSE;

	CS_CycleList->Acquire();
	try
	{
		delete (PKT_CYCLECMDDATA *)m_CycleList->popBack();
	}
	__finally
	{
		CS_CycleList->Release();
	}

	return TRUE;
}
//-----------

/**
@brief   List�� ù��° Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����

int CmdManager::DeleteFirstCycle()
{
	if(m_CycleList->getCount() == 0)
		return FALSE;

	CS_CycleList->Acquire();
	try
	{
		delete (PKT_CYCLECMDDATA *)m_CycleList->popFront();
	}
	__finally
	{
		CS_CycleList->Release();
	}

	return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� Index�� �̿��Ͽ� Ư�� Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   Index : delete�Ϸ��� List Index
@return  Delete�� ������ ��� 1(true), �׷��� ���� ��� 0(false)�� ����

int CmdManager::DeleteCycle(UINT Index)
{
    if(m_CycleList->getCount() == 0)
        return FALSE;

    if(Index > m_CycleList->getCount() - 1)
    {
        // Index�� List�� �������� Ŭ ���
        return FALSE;
    }

	CS_CycleList->Acquire();
    try
    {
		delete (PKT_CYCLECMDDATA *)m_CycleList->pop(Index);
    }
    __finally
    {
		CS_CycleList->Release();
    }

    return TRUE;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   pData : Add�Ϸ��� void ������
*/
void CmdManager::AddCycle(void *pData)
{
	CS_CycleList->Acquire();
    try
    {
        m_CycleList->insertBack(pData);
    }
    __finally
    {
		CS_CycleList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2013.10.28
@return  List�� index Item�� �����͸� �����Ѵ�.
*/
void* CmdManager::ReferCycle(UINT index)
{
    void *pListItem = NULL;

    pListItem = m_CycleList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.

void* CmdManager::ReferFirstCycle()
{
    void *pListItem = NULL;

    pListItem = m_CycleList->referFront();

    return pListItem;
}
*/
//---------------------------------------------------------------------------
/**
@brief   List�� ������ Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* CmdManager::PopBackCycle()
{
	void *pListItem = NULL;


	pListItem = m_CycleList->popBack();


	return pListItem;
}


/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.

void* CmdManager::PopFirstCycle()
{
    void *pListItem = NULL;

	CS_CycleList->Acquire();
    try
    {
		pListItem = m_CycleList->popFront();
    }
    __finally
    {
        CS_CycleList->Release();
    }

    return pListItem;
}
*/
//---------------------------------------------------------------------------

/**
@brief   List�� count�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� count�� �����Ѵ�.
*/
int CmdManager::GetCycleCount()
{
    return m_CycleList->getCount();
}
////---------------------------------------------------------------------------
///**
//@brief   List�� �ʱ�ȭ�ϴ� �Լ�
//@author  puting
//@date    2016.03.15
//@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
//*/
//void CmdManager::InitPassOpenList()
//{
//    // List�� ���� �ش� ������ delete ��
//	for(int i = 0; i < m_pPassOpenList->getCount(); i++)
//    {
//		CS_PassOpenList->Acquire();
//        try
//        {
//			delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
//        }
//        __finally
//        {
//			CS_PassOpenList->Release();
//        }
//    }
//
//    // ����Ʈ�� Clear�Ѵ�.
//	CS_PassOpenList->Acquire();
//    try
//    {
//		m_pPassOpenList->clear();
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� ù��° Command�� delete�ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����
//*/
//int CmdManager::DeleteFirstPassOpen()
//{
//	if(m_pPassOpenList->getCount() == 0)
//        return FALSE;
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Index�� �̿��Ͽ� Ư�� Command�� delete�ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@param   Index : delete�Ϸ��� List Index
//@return  Delete�� ������ ��� 1(true), �׷��� ���� ��� 0(false)�� ����
//*/
//int CmdManager::DeletePassOpen(UINT Index)
//{
//	if(m_pPassOpenList->getCount() == 0)
//        return FALSE;
//
//	if(Index > m_pPassOpenList->getCount() - 1)
//    {
//        // Index�� List�� �������� Ŭ ���
//        return FALSE;
//    }
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		delete (DATA_PASSOPEN *)m_pPassOpenList->pop(Index);
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Command�� �߰��ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@param   pData : Add�Ϸ��� void ������
//*/
//void CmdManager::AddPassOpen(void *pData)
//{
//	CS_PassOpenList->Acquire();
//    try
//    {
//		m_pPassOpenList->insertBack(pData);
//    }
//    __finally
//    {
//		CS_PassOpenList->Release();
//    }
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
//@author  puting
//@date    2013.10.28
//@return  List�� index Item�� �����͸� �����Ѵ�.
//*/
//void* CmdManager::ReferPassOpen(UINT index)
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassOpenList->refer(index);
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//*/
//void* CmdManager::ReferFirstPassOpen()
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassOpenList->referFront();
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
//
//
///**
//@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
//*/
//void* CmdManager::PopFirstPassOpen()
//{
//    void *pListItem = NULL;
//
//	CS_PassOpenList->Acquire();
//    try
//    {
//		pListItem = m_pPassOpenList->popFront();
//    }
//    __finally
//    {
//        CS_PassOpenList->Release();
//    }
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� count�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� count�� �����Ѵ�.
//*/
//int CmdManager::GetPassOpenCount()
//{
//    return m_pPassOpenList->getCount();
//}
////---------------------------------------------------------------------------
///**
//@brief   List�� �ʱ�ȭ�ϴ� �Լ�
//@author  puting
//@date    2016.03.15
//@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
//*/
//void CmdManager::InitPassPermitList()
//{
//    // List�� ���� �ش� ������ delete ��
//	for(int i = 0; i < m_pPassPermitList->getCount(); i++)
//    {
//		delete (PASSPERMIT_COMMON *)m_pPassPermitList->popFront();
//
//	}
//
//	// ����Ʈ�� Clear�Ѵ�.
//
//   	m_pPassPermitList->clear();
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� ù��° Command�� delete�ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����
//*/
//int CmdManager::DeleteFirstPassPermit()
//{
//	if(m_pPassPermitList->getCount() == 0)
//        return FALSE;
//
//	delete (PASSPERMIT_COMMON *)m_pPassPermitList->popFront();
//
//
//    return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Index�� �̿��Ͽ� Ư�� Command�� delete�ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@param   Index : delete�Ϸ��� List Index
//@return  Delete�� ������ ��� 1(true), �׷��� ���� ��� 0(false)�� ����
//*/
//int CmdManager::DeletePassPermit(UINT Index)
//{
//	if(m_pPassPermitList->getCount() == 0)
//        return FALSE;
//
//	if(Index > m_pPassPermitList->getCount() - 1)
//    {
//        // Index�� List�� �������� Ŭ ���
//        return FALSE;
//    }
//
//	delete (PASSPERMIT_COMMON *)m_pPassPermitList->pop(Index);
//
//
//	return TRUE;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Command�� �߰��ϴ� �Լ�
//@author  zzang9un
//@date    2012.11.16
//@param   pData : Add�Ϸ��� void ������
//*/
//void CmdManager::AddPassPermit(void *pData)
//{
//
//	m_pPassPermitList->insertBack(pData);
//
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
//@author  puting
//@date    2013.10.28
//@return  List�� index Item�� �����͸� �����Ѵ�.
//*/
//void* CmdManager::ReferPassPermit(UINT index)
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->refer(index);
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//*/
//void* CmdManager::ReferFirstPassPermit()
//{
//    void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->referFront();
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
///**
//@brief   List�� ������ Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//*/
//void* CmdManager::ReferBackPassPermit()
//{
//	void *pListItem = NULL;
//
//	pListItem = m_pPassPermitList->referBack();
//
//    return pListItem;
//}
//
//
///**
//@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
//*/
//void* CmdManager::PopFirstPassPermit()
//{
//    void *pListItem = NULL;
//
//
//	pListItem = m_pPassPermitList->popFront();
//
//
//    return pListItem;
//}
////---------------------------------------------------------------------------
///**
//@brief   List�� ������ Item�� Pop�ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� ù��° Item�� �����͸� �����Ѵ�.
//@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
//*/
//void* CmdManager::PopBackPassPermit()
//{
//	void *pListItem = NULL;
//
//
//	pListItem = m_pPassPermitList->popBack();
//
//
//	return pListItem;
//}
////---------------------------------------------------------------------------
//
///**
//@brief   List�� count�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2013.04.10
//@return  List�� count�� �����Ѵ�.
//*/
//int CmdManager::GetPassPermitCount()
//{
//    return m_pPassPermitList->getCount();
//}
////---------------------------------------------------------------------------
/**
@brief   �۾������ ���� ������Ŭ �۾����ÿ� ���� �������ִ� �Լ�
@author  puting
@date    2018.03.13
*/
void CmdManager::MakeTaskList_Standard_Del_JobOrder()
{
	bool bAddTask =false;
	int nTaskCount = GetTaskCount();
	PKT_TASKCMDDATA* TempCmdTaskData;        ///< ���ϱ����� ���Ǵ� �ӽ� ����ü

	if(nTaskCount !=0)
	{
		//puting Ž�� ���� Ȯ�� �ʿ� 2018 03 13
		for(int Index =0; Index < nTaskCount; Index++)
		{
			TempCmdTaskData = (PKT_TASKCMDDATA*) ReferTask(Index);
			if(TempCmdTaskData->DetailTagSort == TAGTYPE_JOBORDER)
				{
					DeleteTask(Index);
            		ADD_LOG("Insert_Index:%d", Index);
					break;
				}
		}
	}


}
/**
@brief   ���ռ� Ȯ���� �Ϸ�� CMD�� ���ؼ� Task(Cmd) List�� Add�ϴ� �Լ�
@author  puting
@date    2015.10.30
@param   pCmdHeader : Command�� Header
@param   pCmdGoEtc : Command ����ü
*/
void CmdManager::MakeTaskList_Standard(PKT_TASKCMDDATA *pCmdPktData)
{
	bool bAddTask =false;
	int nTaskCount = GetTaskCount();
	PKT_TASKCMDDATA* CmdTaskData = new PKT_TASKCMDDATA;
	PKT_TASKCMDDATA* TempCmdTaskData;        ///< ���ϱ����� ���Ǵ� �ӽ� ����ü

	memmove(CmdTaskData, pCmdPktData, sizeof(PKT_TASKCMDDATA));

//    // �ʱ�ȭ
//    memset(pCmdTaskData, 0x00, sizeof(PKT_TASKCMDDATA));

	if(nTaskCount !=0)
		{
			//puting Ž�� ���� Ȯ�� �ʿ� 2015 10 31
			for(int Index =0; Index < nTaskCount; Index++)
			{
				TempCmdTaskData = (PKT_TASKCMDDATA*) ReferTask(Index);
				if(TempCmdTaskData->DetailTagSort > CmdTaskData->DetailTagSort)
					{
						AddTask(CmdTaskData, Index);
						bAddTask = true;
						ADD_LOG("Insert_Index:%d", Index);
						break;
					}
			}
		}

	if(!bAddTask)	AddTask(CmdTaskData);

    if(nTaskCount >1)
		ADD_LOG("MakeTaskList_Standard  Count %d CmdDiv:%d, Data1:%04X" , (nTaskCount+1),CmdTaskData->CategoryDiv, CmdTaskData->DetailTagSort);

}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   Main Thread�� Quick Command�� ������ �Լ�
@author  zzang9un
@date    2013.04.09
@param   QuickCmd : ������ Quick Command
*/
void CmdManager::SendQuickCmd2Main(QUICK_CMD QuickCmd)
{
    CS_OHTMainStatus->Acquire();
    try
    {
		m_MainStatus->QuickCmd = QuickCmd;
    }
    __finally
    {
        CS_OHTMainStatus->Release();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/**
@brief   HandTP ����� ���� ���������� üũ�ϴ� �Լ�
@date    2013.01.22
@param   nTmpPID : Port ID
@return
@note    CmdManager Ư���� ���� �ʴ� �Լ�������, ����� ���� �ӽ÷� �ۼ�
*/
int CmdManager::Is_Executable(void)
{
    int nError = 0;

    if(m_MainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO) 
        nError = 1;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING) 
        nError=2;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_UNLOADING) 
		nError=3;
	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING)
		nError=3;
	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING_NULLTYPE)
		nError=3;
   	else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TRANS_HOMING)
		nError=3;
    else if(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING) 
        nError=4;
    else if(m_MainStatus->StatusTP.Driving_F.IsRunning == ON) 
        nError=5;
    else if(m_MainStatus->StatusTP.Driving_R.IsRunning == ON) 
        nError=6;
    else if(m_MainStatus->StatusTP.Hoist.IsRunning == ON)
        nError=7;
    else if(m_MainStatus->StatusTP.Shift.IsRunning == ON)
        nError=8;
    // 2016-12-25, Add by Labeler, Rotate ���� �� ���� Check �߰�
    else if(m_MainStatus->StatusTP.Rotate.IsRunning == ON)
        nError=9;

    return nError;
}
//---------------------------------------------------------------------------

/**
@brief   HandTP ����� ���� ���������� üũ�ϴ� �Լ�
@date    2013.01.22
@param   nTmpPID : Port ID
@return
@note    CmdManager Ư���� ���� �ʴ� �Լ�������, ����� ���� �ӽ÷� �ۼ�
*/
int CmdManager::Is_Executable(char* ExMessage, ...)
{
   int nError = NO_ERR;

   char szDebugMsg[256] = "";
   int ncnt = 0, i = 0;
   String strExclude = "";

   va_list va;
   va_start(va, ExMessage);
   ncnt=vsprintf(szDebugMsg, ExMessage, (va_list)va);
   va_end(va);

   if(m_MainStatus->StatusCommon.MSCmdID.Info.Mode==MODE_AUTO)
   {
      nError = 1;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_AUTO)
         {
            nError =NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_LOADING))
   {
      nError = 2;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_LOAD)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_UNLOADING))
   {
      nError = 3;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_UNLOAD)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING))
   {
	  nError = 3;

	  for(i=0; i<ncnt; i++)
	  {
		 strExclude=szDebugMsg[i];

		 if(strExclude==STS_AUTO_TAUGHT)
		 {
			nError = NO_ERR;
			break;
		 }
	  }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_AUTO_TEACHING_NULLTYPE))
   {
	  nError = 3;

	  for(i=0; i<ncnt; i++)
	  {
		 strExclude=szDebugMsg[i];

		 if(strExclude==STS_AUTO_TAUGHT)
		 {
			nError = NO_ERR;
			break;
		 }
	  }
   }

   if((nError==0)&&(m_MainStatus->StatusCommon.MSCmdID.Info.Status == STS_GOING))
   {
      nError = 4;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_GOING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&((m_MainStatus->StatusTP.Driving_F.IsRunning == ON)||(m_MainStatus->StatusTP.Driving_R.IsRunning == ON)))
   {
      nError = 5;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_DRIVING_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Hoist.IsRunning == ON))
   {
      nError = 7;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_HOIST_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Shift.IsRunning == ON))
   {
      nError = 8;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_SHIFT_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   if((nError==0)&&(m_MainStatus->StatusTP.Rotate.IsRunning == ON))
   {
      nError = 99;

      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_ROTATE_RUNNING)
         {
            nError = NO_ERR;
            break;
         }
      }
   }

   return nError;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
