//---------------------------------------------------------------------------
/**
@file    PassPermit.cpp
@version OHT 7.0
@brief   PassPermit Class CPP File
*/
#include <windows.h>
#include <stdlib.h>
#include <math.h>


#include "PassPermit.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#include "LogHandler.h"	// Log �ۼ� ����

//#define PASSPERMIT_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\PASSPERMIT.log", __VA_ARGS__)
//����
#define PASSPERMIT_LOG(...)    WRITE_LOG_UNIT(pLogPasspermit, __VA_ARGS__)

//---------------------------------------------------------------------------

PassPermit::PassPermit()
{
	pLogPasspermit = getLogUnit("D:\\log\\PASSPERMIT.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
 	m_pPassOpenList = OHTMainForm->GetPassOpenList();
	m_pPassPermitList = OHTMainForm->GetPassPermitList();

	InitPassPermit();

	InitPassOpenList();
	InitPassPermitList();

}

/**
@brief   CID �Ҹ���
@author	 tg117
@date    2015.12.01
*/
PassPermit::~PassPermit()
{

	InitPassOpenList();
	InitPassPermitList();

}

/**
@brief   CID ����� �� ���� ���� �ʱ�ȭ
@author	 tg117
@date    2015.12.01
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
int PassPermit::InitPassPermit()
{

	m_PPIndex = 0;
	m_PPOIndex = 0;
	m_PPMaxIndex = 0;
	m_SumResetPosition = 0.0;
	m_SumCyclePosition = 0.0;
	m_PrePasOpenSeq = INDEX_NONE;

	m_PPCount = 0;

	m_bCheckRollbackFinish = false;

	m_dwPassPermitTime =  timeGetTime();
	m_dwPassPeriodTime = PASSTIME_NONE;

	m_bISForcePPO = false;


}

DWORD PassPermit::GetPassPermitTime()
{
	return m_dwPassPermitTime;
}
DWORD PassPermit::GetPassPeriodTime()
{
	return m_dwPassPeriodTime;
}

void PassPermit::SetPassPermitTime(DWORD dwTime)
{
   	m_dwPassPermitTime = dwTime;
}
void PassPermit::SetPassPeriodTime(DWORD dwTime)
{
   	m_dwPassPeriodTime =  dwTime;
}

int PassPermit::SetPPIndex(int nIndex)
{
    m_PPIndex = nIndex;
	return m_PPIndex;
}
int PassPermit::SetPPOIndex(int nIndex)
{
	m_PPOIndex = nIndex;
	return m_PPOIndex;
}
//
/**
 @class   GetPPIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::GetPPIndex()
{
	if (m_PPIndex < 0)
		return -1;

	return m_PPIndex;
//
}
/**
 @class   GetPPOIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::GetPPOIndex()
{
	if (m_PPOIndex < 0)
		return -1;

	return m_PPOIndex;
}
/**
 @class   GetPPMaxIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief

int PassPermit::GetPPMaxIndex()
{
	if (m_PPMaxIndex < 0)
		return -1;

	return m_PPMaxIndex;
}
 */
/**
 @class   GetPPMaxIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief
 */
bool PassPermit::GetRollBackFinish()
{
	return m_bCheckRollbackFinish;
}

/**
 @class   SetIncreasePPIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief
 */
int PassPermit::SetIncreasePPIndex(int nMax) //-1 �� ��� �ִ밪 ��ġ��
{
	if(nMax < (m_PPIndex +1))
		return -1;

   m_PPIndex = m_PPIndex +1;

   if(m_PPMaxIndex < m_PPIndex)
		m_PPMaxIndex = m_PPIndex;

   PASSPERMIT_LOG("SetIncreasePPIndex PPIndex:%d, MaxIndex:%d",m_PPIndex, m_PPMaxIndex);
   return m_PPIndex;


}
/**
 @class   SetIncreasePPOIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����
 */
int PassPermit::SetIncreasePPOIndex(int nMax) //-1 �� ��� �ִ밪 ��ġ��
{
	if(nMax < (m_PPOIndex +1))
		return -1;

   m_PPOIndex = m_PPOIndex +1;

//   m_PPMaxIndex = m_PPIndex;
   PASSPERMIT_LOG("SetIncreasePPOIndex %d",m_PPOIndex);
   return m_PPOIndex;


}

/**
 @class   SetDecreasePPOIndex �Լ�.
 @date    2018.03.10
 @author  puting
 @brief   ������ ����
 */
int PassPermit::SetDecreasePPOIndex()
{
	PASSPERMIT_LOG("SetDecreasePPOIndex PP:%d PPO:%d",m_PPIndex, m_PPOIndex);
	if(m_PPOIndex < 1)
		return -1;

   m_PPOIndex = m_PPOIndex -1;

   return m_PPOIndex;

}
/**
 @class   SetDecreasePPIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����
 */
int PassPermit::SetDecreasePPIndex() //-1 �� ��� �ּҰ� ��ġ��.
{
	if(m_PPOIndex > (m_PPIndex -1))
		return -1;

	if(m_PPIndex < 1)
		return -1;

   m_PPIndex = m_PPIndex -1;

   PASSPERMIT_LOG("SetDecreasePPIndex PP:%d PPO:%d",m_PPIndex, m_PPOIndex);
   return m_PPIndex;

}

/**
 @class   SetDecreasePPIndex �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����
 */
void PassPermit::SetRollBackFinish(bool bCheck) //-1 �� ��� �ּҰ� ��ġ��.
{

   if(m_bCheckRollbackFinish != bCheck)
	   PASSPERMIT_LOG("SetRollBackFinish %d",m_bCheckRollbackFinish);

   m_bCheckRollbackFinish = bCheck;

}

/**
 @class   SetSumResetPositon �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����
 */
void PassPermit::SetSumResetPositon(double dResetPostion)
{
	m_SumResetPosition =  (double)m_SumResetPosition  +  (double)dResetPostion;
//	m_SumResetPosition =  (double)dResetPostion;
}
/**
 @class   GetAbsolutePositon �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����
 */
double PassPermit::GetAbsolutePositon()
{
	return (double)m_SumResetPosition;

}
/**
 @class   SetSumCyclePositon �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����

void PassPermit::SetSumCyclePositon(double dCyclePostion)    //Map���� End����.
{
//	m_SumCyclePosition =  (double)m_SumCyclePosition  +  (double)dCyclePostion;
	m_SumCyclePosition =  (double)dCyclePostion;
}
 */
/**
 @class   GetSumCyclePositon �Լ�.
 @date    2015.10.15
 @author  puting
 @brief   ������ ����

double PassPermit::GetSumCyclePositon()
{
	return (double)m_SumCyclePosition;
}
 */

void PassPermit::SetPassOpenSeq(int PasOpenSeq)
{
	m_PrePasOpenSeq = PasOpenSeq;
}

int PassPermit::GetPassOpenSeq()
{
	return (int)m_PrePasOpenSeq;
}

//---------------------------------------------------------------------------
/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  puting
@date    2016.03.15
@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void PassPermit::InitPassOpenList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_pPassOpenList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		CS_PassOpenList->Acquire();
        try
        {
			delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
        }
        __finally
        {
			CS_PassOpenList->Release();
        }
    }

    // ����Ʈ�� Clear�Ѵ�.
	CS_PassOpenList->Acquire();
    try
    {
		m_pPassOpenList->clear();
    }
    __finally
    {
		CS_PassOpenList->Release();
    }

}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����

int PassPermit::DeleteFirstPassOpen()
{
	if(m_pPassOpenList->getCount() == 0)
        return FALSE;

	CS_PassOpenList->Acquire();
    try
    {
		delete (DATA_PASSOPEN *)m_pPassOpenList->popFront();
    }
    __finally
    {
		CS_PassOpenList->Release();
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
int PassPermit::DeletePassOpen(UINT Index)
{
	if(m_pPassOpenList->getCount() == 0)
        return FALSE;

	if(Index > m_pPassOpenList->getCount() - 1)
    {
        // Index�� List�� �������� Ŭ ���
        return FALSE;
    }

	CS_PassOpenList->Acquire();
    try
    {
		delete (DATA_PASSOPEN *)m_pPassOpenList->pop(Index);
    }
    __finally
    {
		CS_PassOpenList->Release();
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
void PassPermit::AddPassOpen(void *pData)
{
	CS_PassOpenList->Acquire();
    try
    {
		m_pPassOpenList->insertBack(pData);
    }
    __finally
    {
		CS_PassOpenList->Release();
    }
}
//---------------------------------------------------------------------------

/**
@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2013.10.28
@return  List�� index Item�� �����͸� �����Ѵ�.
*/
void* PassPermit::ReferPassOpen(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_pPassOpenList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
*/
void* PassPermit::ReferFirstPassOpen()
{
    void *pListItem = NULL;

	pListItem = m_pPassOpenList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------



/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.

void* PassPermit::PopFirstPassOpen()
{
    void *pListItem = NULL;

	CS_PassOpenList->Acquire();
    try
    {
		pListItem = m_pPassOpenList->popFront();
    }
    __finally
    {
        CS_PassOpenList->Release();
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
int PassPermit::GetPassOpenCount()
{
    return m_pPassOpenList->getCount();
}
//---------------------------------------------------------------------------
/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  puting
@date    2016.03.15
@note    List�� RESPONSE_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void PassPermit::InitPassPermitList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_pPassPermitList->getCount();
	for(int i = 0; i < nCount; i++)
    {
		delete (DATA_PASS *)m_pPassPermitList->popFront();

	}

	// ����Ʈ�� Clear�Ѵ�.

   	m_pPassPermitList->clear();

}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Command�� delete�ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Delete�� ������ ��� 1(true), �׷��� ���� ���(count�� 0�� ���) 0(false)�� ����

int PassPermit::DeleteFirstPassPermit()
{
	if(m_pPassPermitList->getCount() == 0)
        return FALSE;

	delete (DATA_PASS *)m_pPassPermitList->popFront();


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
int PassPermit::DeletePassPermit(UINT Index)
{
	if(m_pPassPermitList->getCount() == 0)
        return FALSE;

	if(Index > m_pPassPermitList->getCount() - 1)
    {
        // Index�� List�� �������� Ŭ ���
        return FALSE;
    }

	delete (DATA_PASS *)m_pPassPermitList->pop(Index);


	return TRUE;
}
//---------------------------------------------------------------------------

/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@param   pData : Add�Ϸ��� void ������
*/
void PassPermit::AddPassPermit(void *pData)
{

	m_pPassPermitList->insertBack(pData);

}
//---------------------------------------------------------------------------

/**
@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2013.10.28
@return  List�� index Item�� �����͸� �����Ѵ�.
*/
void* PassPermit::ReferPassPermit(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_pPassPermitList->refer(index);

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
*/
void* PassPermit::ReferFirstPassPermit()
{
    void *pListItem = NULL;

	pListItem = m_pPassPermitList->referFront();

    return pListItem;
}
//---------------------------------------------------------------------------
/**
@brief   List�� ������ Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
*/
void* PassPermit::ReferBackPassPermit()
{
	void *pListItem = NULL;

	pListItem = m_pPassPermitList->referBack();

    return pListItem;
}


/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* PassPermit::PopFirstPassPermit()
{
    void *pListItem = NULL;


	pListItem = m_pPassPermitList->popFront();


    return pListItem;
}
//---------------------------------------------------------------------------
/**
@brief   List�� ������ Item�� Pop�ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* PassPermit::PopBackPassPermit()
{
	void *pListItem = NULL;


	pListItem = m_pPassPermitList->popBack();


	return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� count�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.10
@return  List�� count�� �����Ѵ�.
*/
int PassPermit::GetPassPermitCount()
{
    return m_pPassPermitList->getCount();
}
//---------------------------------------------------------------------------

//
///**
// @class   SetIncreasePPCount �Լ�.
// @date    2015.10.15
// @author  puting
// @brief   ������ ����
// */
//int PassPermit::SetIncreasePPCount() //-1 �� ��� �ִ밪 ��ġ��
//{
//
//   m_PPCount++;
//
//   PASSPERMIT_LOG("SetIncreasePPCount %d",m_PPCount);
//   return m_PPCount;
//
//
//}
///**
// @class   SetDecreasePPCount �Լ�.
// @date    2015.10.15
// @author  puting
// @brief   ������ ����
// */
//int PassPermit::SetDecreasePPCount() //-1 �� ��� �ּҰ� ��ġ��.
//{
//	if((m_PPCount -1) < 0 )
//		return -1;
//
//   m_PPCount = m_PPCount -1;
//
//   PASSPERMIT_LOG("SetDecreasePPCount :%d",m_PPCount);
//   return m_PPCount;
//
//}
///**
// @class   SetIncreasePPCount �Լ�.
// @date    2015.10.15
// @author  puting
// @brief   ������ ����
// */
//int PassPermit::GetPPCount() //-1 �� ��� �ִ밪 ��ġ��
//{
//
//   return m_PPCount;
//
//
//}



