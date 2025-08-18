//---------------------------------------------------------------------------


#pragma hdrstop

#include "HandRMR.h"
#include "DiagManager.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#if((OHT_NAME == OHT_NAME_P4_MASK) || (OHT_NAME == OHT_NAME_MASK_JIG))

/**
@file    HandRMR.cpp
@version OHT 7.0
@brief   HandRMR Class CPP File
*/

#include "HandRMR.h"

#define ORIGIN_POSE  0
#define HOME_POSE  1
#define WORK_POSE  2

#define MANUAL_M 0
#define AUTO_M	1

/**
@brief   HandRMR Class ������
@author  mgchoi
@date    2022.08.22
@param   pHWCommon : HWCommon Class�� ������
*/
HandRMR::HandRMR(HWCommon * pHWCommon)
{
	m_pHWCommon = pHWCommon;
	m_pShutterC	= new Shutter(m_pHWCommon);
	m_pHoistC = new Hoist(m_pHWCommon, 2);
	Init();

	InitializeCriticalSection(&m_MovCntCS);
}

/**
@brief   HandRMR Class �Ҹ���
@author  mgchoi
@date    2022.08.22
*/
HandRMR::~HandRMR()
{;
	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief   HandRMR Class���� ����ϴ� ���� �ʱ�ȭ
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::Init()
{
    bReadyOperation = false;
	nSendCommand = HANDRMR_NO_COMMAND;

	dwOriginLimitTime = HANDRMR_ORIGIN_LIMIT_TIME;
	dwMotionLimitTime = HANDRMR_MOTION_LIMIT_TIME;

    bLongRun = false;
    m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
    bHomingState = false;
	dwStartTime = 0;
}

/**
@brief   HandRMR �� ����� ��� OFF ���·� �ʱ�ȭ��Ű�� �Լ�
@author  mgchoi
@date    2022.08.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::IO_Init()
{
    int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_HAND_WORK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_HOME, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_RESET, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);
	return nError;
}

/**
@brief   HandRMR�� Manual Mode�� Work��ġ�� �̵��ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR�� ���� ���� WORK �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int HandRMR::ManualWork(int withIL)
{
    int nError = NO_ERR;
	int nExec = 0;

    if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR�� �̵��Ұ� ���� üũ
		nError = IsHandRMRMoveAvailableState(MANUAL_M);

	if (nError == NO_ERR)  // RMR ���� ���� ����
		nError = Enable();

	if (nError == NO_ERR)
	{
		nError = IO_Init();
		if (nError == NO_ERR)
			nError = AlarmReset();
	}
	if (nError == NO_ERR)
	{
		if(withIL == 0)  // WORK
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_WORK, ON);
			if (nError)
				nError = ERR_SEND_HANDRMR_WORK_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_WORK_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
		else    // WORK_INTERLOCK
		{
            // 2016-11-14. Add by Labeler
            // Work_IL ��� �߻� ��, Flange Sensor Check ���� ��� �߰�
            if(IsFlangeCheck() == false)
                return ERR_SEND_HANDRMR_WORK_IL_COMMAND;
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, ON);
			if (nError)
				nError = ERR_SEND_HANDRMR_WORK_IL_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_WORK_IL_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
	}
    return nError;
}

/**
@brief   HandRMR�� Auto Mode�� ������ �� Work��ġ�� �̵��ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR�� ���� ���� �̵��Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int HandRMR::AutoWork(int withIL)
{
	int nError = NO_ERR;

    if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR�� �̵��Ұ� ���� üũ
		nError = IsHandRMRMoveAvailableState(AUTO_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

   // alarm check
	if (IsAlarm() == true)
    {
		nError = Origin();
		nCount_HandReset++;
		//Sleep(200);
		Sleep(3500); // E1335 �ٹ߷� ���� �ð� �ø���
	}
   // limit sensor check
	if (nError == NO_ERR)
	{
		if (IsWork() == true)
		{
			if (IsHome() == true)
				nError = ERR_HAND_LIMIT_SEN_ALL_ON;    // ���� ���� �̿�
			else
				return nError;
		}
		else
		{
			if (IsHome() == false)
				nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
			else       // normal case : Home�� ������
			{
				if(withIL == 0)
				{
					nError = m_pHWCommon->SetBit(OUT_HAND_WORK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_WORK_COMMAND;
				}
				else
				{
                    // 2016-11-14. Add by Labeler
                    // Work_IL ��� �߻� ��, Flange Sensor Check ���� ��� �߰�
                    if(IsFlangeCheck() == false)
                        return ERR_SEND_HANDRMR_WORK_IL_COMMAND;

					nError = m_pHWCommon->SetBit(OUT_HAND_WORK_INTERLOCK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_WORK_IL_COMMAND;
				}

				if (nError == NO_ERR)
					m_nMovingCount++;
			}
		}
	}
	if (nError == NO_ERR)
	{
		dwStartTime = timeGetTime();
		if(withIL == 0)
			nSendCommand = HANDRMR_WORK_COMMAND;
		else
			nSendCommand = HANDRMR_WORK_IL_COMMAND;

		bool bIsTimeChecked = false;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			//���ܸ���͸� �׸�(Hand move timeover)
			int nMoveTimeOver = DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveHand;
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= nMoveTimeOver && bIsTimeChecked == false)
			{
				DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_HAND);

				bIsTimeChecked = true;
			}

			Sleep(10);
		}
	}
	return nError;
}

/**
@brief   HandRMR�� Manual Mode�� Home �ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR�� ���� ���� Home �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int HandRMR::ManualHome(int withIL)
{
	int nError = NO_ERR;
	if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR�� �̵��Ұ� ���� üũ
		nError = IsHandRMRMoveAvailableState(MANUAL_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

	if (nError == NO_ERR)
		nError = AlarmReset();

	if (nError == NO_ERR)
	{
		if(withIL==0)
		{
			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_HOME, ON);

			if (nError)
			nError = ERR_SEND_HANDRMR_HOME_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_HOME_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
		else
		{
		   // 2016-11-14. Add by Labeler
		   // Work_IL ��� �߻� ��, Flange Sensor Check ���� ��� �߰�
		  if(IsFlangeCheck() == false)
			  return ERR_SEND_HANDRMR_HOME_IL_COMMAND;

			if (nError == NO_ERR)
				nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, ON);

			if (nError)
				nError = ERR_SEND_HANDRMR_HOME_IL_COMMAND;
			else
			{
				dwStartTime = timeGetTime();
				nSendCommand = HANDRMR_HOME_IL_COMMAND;
				while ((nError = CheckComplete()) == NEXT_CHECK)
				{
					Sleep(10);
				}
			}
		}
	}
    return nError;
}

/**
@brief   HandRMR�� Auto Mode�� ������ �� Home �ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR�� ���� ���� Home �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int HandRMR::AutoHome(int withIL)
{
	int nError = NO_ERR;
	if (bReadyOperation == false)
		nError = ERR_HANDRMR_NOTORIGIN;

	if(nError == NO_ERR)   // RMR�� �̵��Ұ� ���� üũ
		nError = IsHandRMRMoveAvailableState(AUTO_M);

	if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

   // alarm check
    if (IsAlarm() == true)
	{
		nError = Origin();
		nCount_HandReset++;
		//Sleep(200);
		Sleep(3500); // E1335 �ٹ߷� ���� �ð� �ø���
    }

	if (nError == NO_ERR)
	{
		if (IsHome() == true)
		{
			if (IsWork() == true)
				nError = ERR_HAND_LIMIT_SEN_ALL_ON;
			else
				return nError;
		}
		else
		{
			if (IsWork() == false)
				nError = ERR_HAND_LIMIT_SEN_ALL_OFF;
			else     // Normal Case
			{
				if(withIL==0)
				{
					nError = m_pHWCommon->SetBit(OUT_HAND_HOME, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_HOME_COMMAND;
				}
				else
				{
                    // 2016-11-14. Add by Labeler
                    // Work_IL ��� �߻� ��, Flange Sensor Check ���� ��� �߰�
                    if(IsFlangeCheck() == false)
                        return ERR_SEND_HANDRMR_HOME_IL_COMMAND;

					nError = m_pHWCommon->SetBit(OUT_HAND_HOME_INTERLOCK, ON);
					if (nError)
						nError = ERR_SEND_HANDRMR_HOME_IL_COMMAND;
				}

				if (nError == NO_ERR)
					m_nMovingCount++;
			}
		}
	}
	if (nError == NO_ERR)
	{
		dwStartTime = timeGetTime();
		if(withIL==0)
			nSendCommand = HANDRMR_HOME_COMMAND;
		else
			nSendCommand = HANDRMR_HOME_IL_COMMAND;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
            Sleep(10);
        }
    }
    return nError;
}

/**
@brief   HandRMR ���͸� ���� ���ͽ�Ű�� �Լ�
@author  mgchoi
@date    2022.08.22
@return  ���ۿ� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::Origin()
{
	int nError = NO_ERR;

	bReadyOperation = false;
	bHomingState = true;
	if (nError == NO_ERR)
		nError = Enable();

	if(nError == NO_ERR)   // RMR�� �̵��Ұ� ���� üũ
		nError = IsHandRMRMoveAvailableState(MANUAL_M);  // orgin

	if (nError == NO_ERR)
		nError = IO_Init();

	if (nError == NO_ERR)
		nError = AlarmReset();

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, ON);

	Sleep(500);

	if (nError)
		nError = ERR_SEND_HAND_ORIGIN_COMMAND;
	else
    {
		dwStartTime = timeGetTime();
		nSendCommand = HANDRMR_ORIGIN_COMMAND;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
		}
		if (nError == NO_ERR && bReadyOperation == false)
            bReadyOperation = true;
	}
    bHomingState = false;
    return nError;
}

/**
@brief   HandRMR�� �˶� �����̸� �˶��� �����ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::AlarmReset()
{
	int nError = NO_ERR;

	if(IsAlarm() == true)
	{
		//���ܸ���͸� �׸�(Hand Alarm reset)
		DiagManager* pDiagManager = DiagManager::getInstance();

		pDiagManager->IncreaseAbnormalHandResetCount();

		if(pDiagManager->GetAbnormalHandResetCount() > pDiagManager->GetDiagParam().nRefAbnormAlarmResetHand)
		{
			DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_ALARM_RESET_HAND);
			pDiagManager->ResetAbnormalHandResetCount();
		}

		nError = m_pHWCommon->SetBit(OUT_HAND_RESET, ON);
		if (nError == NO_ERR)
		{
			Sleep(500);
			nError = m_pHWCommon->SetBit(OUT_HAND_RESET, OFF);
		}
		Sleep(50);
	}
    return nError;
}

/**
@brief   HandRMR ���͸� Enable ���·� ����� �Լ�
@author  mgchoi
@date    2022.08.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::Enable()
{
	int nError = NO_ERR;
	if (IsFree() == true)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);

	return nError;
}

/**
@brief   Hand RMR ���͸� Free ���·� ����� �Լ�
@author  mgchoi
@date    2022.08.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::Free()
{
    int nError = NO_ERR;
    bReadyOperation = false;

	nError = m_pHWCommon->SetBit(OUT_HAND_FREE, ON);

	return nError;
}

/**
@brief   ��� ��ɿ� �´� ������ ���� ���� �ð� ���� �Ϸ��ߴ��� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  ���� ���� ���� �� 0, ���� �߻��� �ش� ���� �ڵ� ����.
*/
int HandRMR::CheckComplete()
{
	DWORD dwHandrmrMovingTime = 0;

	if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == HANDRMR_ORIGIN_COMMAND) ?
		dwOriginLimitTime : dwMotionLimitTime))
	{
		switch (nSendCommand)
        {
		case HANDRMR_WORK_COMMAND:
			IO_Init();
			return ERR_HANDRMR_WORK_TIMEOVER;

		case HANDRMR_WORK_IL_COMMAND:
			IO_Init();
			return ERR_HANDRMR_WORK_IL_TIMEOVER;

		case HANDRMR_HOME_COMMAND:
			IO_Init();
			return ERR_HANDRMR_HOME_TIMEOVER;

		case HANDRMR_HOME_IL_COMMAND:
			IO_Init();
			return ERR_HANDRMR_HOME_IL_TIMEOVER;

		case HANDRMR_ORIGIN_COMMAND:
            IO_Init();
			return ERR_HAND_ORIGIN_TIMEOVER;           // ���� Hand Error ���
        }
    }
	if (IsAlarm() == true)
	{
        IO_Init();
		return ERR_HAND_ALARM;            // ���� Hand Error ���
	}

	switch (nSendCommand)
	{
		case HANDRMR_WORK_COMMAND:
		case HANDRMR_WORK_IL_COMMAND:
			if ((IsWork() == true) && (IsInposition() == true))
			{
				dwHandrmrMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
				dwStartTime = 0;
				if (dwMaxMovingTime < dwHandrmrMovingTime)
					dwMaxMovingTime = dwHandrmrMovingTime;

				// 2016-11-15
				Sleep(500);
				IO_Init();
				return NO_ERR;
			}
			break;
		case HANDRMR_HOME_COMMAND:
		case HANDRMR_HOME_IL_COMMAND:
			if ((IsHome() == true) && (IsInposition() == true))
			{
				dwHandrmrMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
				dwStartTime = 0;
				if (dwMaxMovingTime < dwHandrmrMovingTime)
					dwMaxMovingTime = dwHandrmrMovingTime;
				// 2016-11-15
				Sleep(500);
				IO_Init();
				return NO_ERR;
			}
			break;
		case HANDRMR_ORIGIN_COMMAND:
			if ((IsHome() == true) && (IsInposition() == true))
			{
				IO_Init();
				return NO_ERR;
			}
			break;
	}
	return NEXT_CHECK;
}

/**
@brief   Hand ��ġ�� RMR�� ������ �� �ִ��� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand�� Antidrop���� ���� �ְų�, POD ���������� Placement ������ �����ǰų�, POD�� ����µ� unload��ġ, Antidrop�� Close�̸� �̵� �Ұ�.
*/
int HandRMR::IsHandRMRMoveAvailableState(int mode)              // selpose: 0 Origine, 1: Home, 2 Work
{
	int nError = NO_ERR;

	///AntiTemp..Pod�ӽ� �ּ�ó��. mgchoi
	if((m_pShutterC->IsPodPlacement3On() == true) || (m_pShutterC->IsPodPlacement4On() == true))  //Placement���� �������� ���
			nError = ERR_HANDRMR_NOTMOVE_PLACEMENT_SEN_ON;
	else if(((m_pShutterC->IsClose(FRONT) == true) || (m_pShutterC->IsOpen(FRONT) == false))
		&& (m_pHoistC->IsHoistHome() == false) && (mode == MANUAL_M))         // Manual�϶��� Home������ Shutter�� Close���־ Hand������
			nError = ERR_HANDRMR_NOTMOVE_ANTIDROPNOTOPEN;
	else if(((m_pShutterC->IsClose(FRONT) == true) || (m_pShutterC->IsOpen(FRONT) == false))
		&& (mode == AUTO_M))         // Antidrop Close ��ġ�� ���
			nError = ERR_HANDRMR_NOTMOVE_ANTIDROPNOTOPEN;
	else if(IsLeftGripOn() != IsRightGripOn())
		nError = ERR_GRIP_SEN_MISMATCH;
	else if((IsUnloadPosition() == true) && ((IsLeftGripOn()== true) || (IsRightGripOn()== true)))  // POD�� ����µ� Unload��ġ
		nError = ERR_POD_NOTMOVE_IN_UNLOADPOS;
	else
		;

	return nError;
}

/**
@brief   HandRMR HAND_UnloadPosition ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR HAND_UnloadPosition On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsLoadPosition()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_LoadPosition == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR HAND_UnloadPosition ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR HAND_UnloadPosition On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsUnloadPosition()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_UnloadPosition == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Work Limit ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Work Limit ������ On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsWork()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Work == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Home Limit ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Home Limit ������ On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsHome()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Alarm ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Alarm ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsAlarm()
{
    bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Alarm == ON)
		bResult = true;
	else
		bResult = false;

    return bResult;
}

/**
@brief   HandRMR Free ���� �� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Free ���� ���� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsFree()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Free == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR�� Origin �Ǿ� ������ �� �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Ready Operation Flag�� ����.
*/
bool HandRMR::IsReady()
{
    return bReadyOperation;
}

/**
@brief   HandRMR ���� Inposition ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR ���� Inposition ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsInposition()
{
    bool bResult = false;

	if(m_pHWCommon->m_Input.HAND_Alarm == ON)
		bResult = false;
	else if (m_pHWCommon->m_Input.HAND_Work == ON || m_pHWCommon->m_Input.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Flange Sensor ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Flange ���� ���°� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsFlangeCheck()
{
    bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Flange_Check == ON)
		bResult = true;
	else
		bResult = false;
	return bResult;
}

/**
@brief   Hand PIO Go ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand PIO Go ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsGoSigOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_PIO_Go == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   HandRMR Left Gripper ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Left Gripper ������ On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsLeftGripOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Left_Grip == ON)
		bResult = true;
	else
		bResult = false;

    return bResult;
}

/**
@brief   HandRMR Right Gripper ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  HandRMR Right Gripper ������ On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsRightGripOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Right_Grip == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Push ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Close ��� ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsWorkOutputOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Work == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Push ���� ���¸� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Close ��� ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool HandRMR::IsHomeOutputOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand�� Homing ���� �������� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Homing State Flag �� ����
*/
bool HandRMR::IsHomingState()
{
    return bHomingState;
}

/**
@brief   Hand Long Run �������� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Long Run Flag �� ����
*/
bool HandRMR::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Hand Long Run�� �����ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}

/**
@brief   Hand Long Run�� ���ߴ� �Լ�
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}

/**
@brief   Hand Moving Ƚ���� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Moving Count ����
*/
int HandRMR::GetMovingCount(bool bInit)
{
	int nMovingCount = m_nMovingCount;

	if(bInit == true)
	{
		EnterCriticalSection(&m_MovCntCS);
		m_nMovingCount = 0;
		LeaveCriticalSection(&m_MovCntCS);
	}

	return nMovingCount;
}

/**
@brief   Hand Max Moving Time�� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Hand Max Moving Time ����
*/
DWORD HandRMR::GetMaxMovingTime()
{
    return dwMaxMovingTime;
}

/**
@brief   Hand �� ������ ��Ʈ�� �ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@param   nCase(On/Off/Reset)�� ������ �� ����.
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int HandRMR::HandPowerControl(int nMode)
{
    int nError = NO_ERR;

    switch (nMode)
    {
    case PWR_OFF:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, ON);
        break;
    case PWR_ON:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, OFF);
        break;
    case PWR_RST:
        nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, ON);
		if (nError == NO_ERR)
        {
            Sleep(300);
			nError = m_pHWCommon->SetBit(OUT_HAND_POWER_RESET, OFF);
        }
        break;
    }

    return nError;
}

/**
@brief   Hand �� ������ ��Ʈ�� �ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  Power ��� Return
*/
bool HandRMR::IsHandPowerOn()
{
	return !(m_pHWCommon->m_Output.HAND_Power_Reset);
}


/**
@brief   Hand �ΰ� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_HAND_NOTORIGIN ����.
*/
int HandRMR::ReadyToOperation()
{
    int nError = NO_ERR;

    if (bReadyOperation == true)
        nError = NO_ERR;
    else
        nError = ERR_HAND_NOTORIGIN;
    return nError;
}

/**
@brief   Hand ���� ���¿� ���� Origin ����� �ְ� �� �� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  mgchoi
@date    2022.08.22
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_HAND_NOTORIGIN ����.
*/
int HandRMR::AutoInitialize()
{
	int nError = 0;
	nError = IsHandRMRMoveAvailableState(AUTO_M);
	if (nError == NO_ERR)
    {
      // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile("FOUP Check : NO FOUP(Normal case) ===> Hand Origin Start...");
        nError = Origin();
        if (nError == NO_ERR)
        {
		 // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile("   ===> SUCCESS Hand Origin");
			bReadyOperation = true;
        }
        else
        {
			bReadyOperation = false;
		}
	}
	nError = ReadyToOperation();
	return nError;
}

unsigned int HandRMR::GetHandResetCount()
{
	return nCount_HandReset;
}

/**
@brief  ResetMovementData
@author  mgchoi
@date    2022.08.22
*/
void HandRMR::ResetMovementData()
{
	m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
}

bool HandRMR::IsOpen(int nLeftRight)
{
	return false;
}
bool HandRMR::IsClose(int nLeftRight)
{
	return false;
}
bool HandRMR::IsCloseOutputOn()
{
	return false;
}

bool HandRMR::IsOpenOutputOn()
{
	return false;
}
bool HandRMR::IsPushOn()
{
	return false;
}
#endif

