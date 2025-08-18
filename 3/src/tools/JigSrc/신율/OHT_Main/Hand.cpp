/**
@file    Hand.cpp
@version OHT 7.0
@brief   Hand Class CPP File
*/

#include "Hand.h"
#include "DiagManager.h"
#include "OHTMain.h"


#define MAX_BYTE_SIZE_OF_TRANS_LOG  1024*10
#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__)  //AOHC-253 Monitoring �׸� �߰� 2
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)



/**
@brief   Hand Class ������
@author  LSJ
@date    2012.10.30
@param   pHWCommon : HWCommon Class�� ������
*/



Hand::Hand(HWCommon * pHWCommon)
{
    m_pHWCommon = pHWCommon;
	Init();
	InitializeCriticalSection(&m_MovCntCS);
	pLogUnit = getLogUnit("D:\\log\\Trans.log", MAX_BYTE_SIZE_OF_TRANS_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	nMovingCount = 0;
	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
}

/**
@brief   Hand Class �Ҹ���
@author  LSJ
@date    2012.10.30
*/
Hand::~Hand()
{
	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief   Hand Class���� ����ϴ� ���� �ʱ�ȭ
@author  LSJ
@date    2012.10.30
*/
void Hand::Init()
{
	bReadyOperation = false;
	nSendCommand = HAND_NO_COMMAND;

    dwOriginLimitTime = HAND_ORIGIN_LIMIT_TIME;
	dwMotionLimitTime = HAND_MOTION_LIMIT_TIME;

    bLongRun = false;
    m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
	bHomingState = false;
	dwStartTime = 0;
}

/**
@brief   Hand �� ����� ��� OFF ���·� �ʱ�ȭ��Ű�� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::IO_Init()
{
	int nError = NO_ERR;
	nError = m_pHWCommon->SetBit(OUT_HAND_CLOSE, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_OPEN, OFF);

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_RESET, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);

	Sleep(500);

	return nError;
}

/**
@brief   Hand�� Manual Mode�� Open �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand�� ���� ���� Open �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int Hand::ManualOpen()
{
	int nError = NO_ERR;


	if (bReadyOperation == false)
	{
		nError = ERR_HAND_NOTORIGIN;
	}
	if (nError == NO_ERR)
	{
		nError = Enable();
	}

	if (nError == NO_ERR)
	{
		nError = IO_Init();
	}

	if (IsAlarm() == true)
	{
		nError = ERR_HAND_ALARM;
		ADD_LOG("ERR_HAND_ALARM");
	}
	if(((IsOpen() == true)&&(IsClose() == true)) || ((IsOpen() == false)&&(IsClose() == false)))
	{
		nError = ERR_SEND_HAND_OPEN_COMMAND;
		ADD_LOG("[HAND OPEN] ERR_HAND_CLOSE SENSOR ON");
	}
	if (nError == NO_ERR)
	{
			nError = m_pHWCommon->SetBit(OUT_HAND_OPEN, ON);
	}
	if (nError)
	{
			nError = ERR_SEND_HAND_OPEN_COMMAND;

	}
//        else
//        {
//            dwStartTime = timeGetTime();
//			nSendCommand = HAND_OPEN_COMMAND;
//			while ((nError = CheckComplete()) == NEXT_CHECK)
//			{
//                Sleep(10);
//            }
//		}


	  //�׸����� On üũ��  100ms ���� 3ȸ���� chattering ���� ���� ó��
//	if (nError == NO_ERR)
//	{
//		if ((IsRightGripOn() == true) || (IsLeftGripOn() == true))
//        {
//			dwStartTime = timeGetTime();
//            while (1)
//            {
//                if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)HAND_GRIPON_WAIT_TIME)
//					break;
//				if ((IsRightGripOn() == false) && (IsLeftGripOn() == false))
//					break;
//				Sleep(10);
//			}
//
//			if ((IsRightGripOn() == true) || (IsLeftGripOn() == true))
//			{
//                ADD_LOG("[AutoClose] ERR_HAND_SENSOR_OFFERR, %d(ms) wait time excess",(int)HAND_GRIPON_WAIT_TIME);
//				nError = ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
//            }
//		}
//
//    }

    return nError;
}

/**
@brief   Hand�� Auto Mode�� ������ �� Open �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand�� ���� ���� Open �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int Hand::AutoOpen()
{
    int nError = NO_ERR;
    int nAutoOpen_Errcnt = 0;

    if (bReadyOperation == false)
	{
		nError = ERR_HAND_NOTORIGIN;
		ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
						ERR_HAND_NOTORIGIN,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(),
						IsClose());
	}
    if (nError == NO_ERR)
		nError = Enable();
	if (nError == NO_ERR)
		nError = IO_Init();

   // alarm check
	if (IsAlarm() == true)
    {
		nError = Origin();
		nCount_HandReset++;
		Sleep(200);

    }

   // limit sensor check
   // Open ���� ON ���� close, grip sensor check ����üũ �� AutoOpen()���
   // chattering 3ȸ �̻�� ���� ����
	if (nError == NO_ERR)
	{

		if (IsOpen() == true)
		{
            dwStartTime = timeGetTime();
			while(IsClose() == true)
            {
                 nAutoOpen_Errcnt++;

                 if(nAutoOpen_Errcnt > 3)
                 {
                    nAutoOpen_Errcnt = 0;
                    ADD_LOG("[AutoOpen] ERR_HAND_LIMIT_SEN_ALL_ON, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
					ADD_FDC_LOG("ERR/%d/4/11/%06d/%06d/%d/%d/%06d/0/0/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										IsOpen(),
										IsClose(),
										IsLeftGripOn(),
										IsRightGripOn());
                    return ERR_HAND_LIMIT_SEN_ALL_ON;
                 }
                 Sleep(10);
                 ADD_LOG("[AutoOpen] Error Count(LIMIT All On), sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));

            }
            nAutoOpen_Errcnt = 0;
            dwStartTime = timeGetTime();
            while((IsRightGripOn() == true) || (IsLeftGripOn() == true))
			{

                nAutoOpen_Errcnt++;

                if(nAutoOpen_Errcnt > 3)
                {
                    nAutoOpen_Errcnt = 0;
                    ADD_LOG("[AutoOpen] ERR_GRIPPER_SEN_ON_IN_HAND_OPEN, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
                    return ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
                }
                Sleep(10);
                ADD_LOG("[AutoOpen] Error Count(GRIP) : %d, sleep time: %d(ms)", nAutoOpen_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));


			}
            nAutoOpen_Errcnt = 0;

			return nError;
		}

        //close, open ��� Off�� �� 3ȸ ä�͸� �� ����ó��

		else if (IsClose() == false)
        {
            dwStartTime = timeGetTime();
            while(IsClose() == false)
            {
                nAutoOpen_Errcnt++;

                if(nAutoOpen_Errcnt > 3)
                {
                    nAutoOpen_Errcnt = 0;
                    ADD_LOG("[AutoOpen] ERR_HAND_LIMIT_SEN_ALL_OFF, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
					ADD_FDC_LOG("ERR/%d/3/11/%06d/%06d/%d/%d/%06d/0/0/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										IsOpen(),
										IsClose(),
										IsLeftGripOn(),
										IsRightGripOn());
                    return ERR_HAND_LIMIT_SEN_ALL_OFF;
                }
                Sleep(10);
                ADD_LOG("[AutoOpen] Error Count(LIMIT All Off) : %d, sleep time: %d(ms)", nAutoOpen_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
            }
            nAutoOpen_Errcnt = 0;
        }



		else
		{
			nError = m_pHWCommon->SetBit(OUT_HAND_OPEN, ON);
			if (nError)
			{
				nError = ERR_SEND_HAND_OPEN_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
						ERR_SEND_HAND_OPEN_COMMAND,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(),
						IsClose());
			}
            if (nError == NO_ERR)
				m_nMovingCount++;
        }
    }

	if (nError == NO_ERR)
    {
        dwStartTime = timeGetTime();
		nSendCommand = HAND_OPEN_COMMAND;

		bool bIsTimeChecked = false;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			//AOHC-196 ���� �α� �߰�
			DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_HANDT_MAX,OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));

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


    //�׸����� On üũ��  100ms ���� 3ȸ���� chattering ���� ���� ó��
    if (nError == NO_ERR)
    {
        dwStartTime = timeGetTime();

		while((IsRightGripOn() == true) || (IsLeftGripOn() == true))
        {
            nAutoOpen_Errcnt++;

            if(nAutoOpen_Errcnt > 3)
            {
                nAutoOpen_Errcnt = 0;
                ADD_LOG("[AutoOpen] ERR_GRIPPER_SEN_ON_IN_HAND_OPEN, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
                return ERR_GRIPPER_SEN_ON_IN_HAND_OPEN;
            }
            Sleep(100);
            ADD_LOG("[AutoOpen] Error Count(GRIP_sleep) : %d, sleep time: %d(ms)", nAutoOpen_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));


		}
        nAutoOpen_Errcnt = 0;
    }

    return nError;
}

/**
@brief   Hand�� Manual Mode�� Close �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand�� ���� ���� Close �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int Hand::ManualClose()
{
    int nError = NO_ERR;
    if (bReadyOperation == false)
		nError = ERR_HAND_NOTORIGIN;

    if (nError == NO_ERR)
		nError = Enable();

	if (nError == NO_ERR)
		nError = IO_Init();

	if (IsAlarm() == true)
	{
		nError = ERR_HAND_ALARM;
		ADD_LOG("ERR_HAND_ALARM");
	}

	if(((IsOpen() == true)&&(IsClose() == true)) || ((IsOpen() == false)&&(IsClose() == false)))
	{
		nError = ERR_SEND_HAND_CLOSE_COMMAND;
		ADD_LOG("[HAND CLOSE] ERR_HAND_OPEN SENSOR ON");
	}

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_CLOSE, ON);
	if (nError)
		nError = ERR_SEND_HAND_CLOSE_COMMAND;
//    else
//    {
//        dwStartTime = timeGetTime();
//		nSendCommand = HAND_CLOSE_COMMAND;
//		while ((nError = CheckComplete()) == NEXT_CHECK)
//        {
//            Sleep(10);
//        }
//	}

//	if (nError == NO_ERR)
//	{
//        if (IsLeftGripOn()!=IsRightGripOn())
//        {
//			dwStartTime = timeGetTime();
//            while (1)
//            {
//                if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)HAND_GRIPON_WAIT_TIME)
//					break;
//				if  (IsLeftGripOn()==IsRightGripOn())
//					break;
//				Sleep(10);
//			}
//
//			if  (IsLeftGripOn()!=IsRightGripOn())
//			{
//                ADD_LOG("[AutoClose] ERR_HAND_SENSOR_OFFERR, %d(ms) wait time excess",(int)HAND_GRIPON_WAIT_TIME);
//				nError = ERR_GRIP_SEN_MISMATCH;
//            }
//		}
//	}


    return nError;
}

/**
@brief   Hand�� Auto Mode�� ������ �� Close �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand�� ���� ���� Close �Ǹ� 0, ���� �߻� �� ���� �ڵ� ����.
*/
int Hand::AutoClose()
{
	int nError = NO_ERR;
    int nAutoClose_Errcnt = 0;
	if (bReadyOperation == false)
	{
		nError = AlarmReset();
		Origin();
//		nError = ERR_HAND_NOTORIGIN;
	}
    if (nError == NO_ERR)
        nError = Enable();

    if (nError == NO_ERR)
        nError = IO_Init();
    if (nError == NO_ERR)
        nError = AlarmReset();

   // alarm check
    if (IsAlarm() == true)
    {
		nError = AlarmReset();
		nCount_HandReset++;
		if (IsAlarm() == true)
		{
            nError = ERR_HAND_RESET_FAIL;
			ADD_FDC_LOG("ERR/%d/1/8/%06d/%06d/%d/%d/%06d/%d/%d/%d",
						ERR_HAND_RESET_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsAlarm(),
						IsOpen(),
						IsClose());
		}
    }

// limit sensor check
    if (nError == NO_ERR)
	{

		if (IsClose() == true)
		{
            dwStartTime = timeGetTime();
			while(IsOpen() == true)
            {
                nAutoClose_Errcnt++;

                if(nAutoClose_Errcnt > 3)
                {
                    nAutoClose_Errcnt = 0;
                    ADD_LOG("[AutoClose] ERR_HAND_LIMIT_SEN_ALL_ON, sleep time: %d(ms)",  OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
					ADD_FDC_LOG("ERR/%d/5/11/%06d/%06d/%d/%d/%06d/0/0/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_ON,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										IsOpen(),
										IsClose(),
										IsLeftGripOn(),
										IsRightGripOn());
                    return ERR_HAND_LIMIT_SEN_ALL_ON;
                }
                Sleep(10);
                ADD_LOG("[AutoClose] Error Count(LIMIT All On) : %d, sleep time: %d(ms)", nAutoClose_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));

            }
            nAutoClose_Errcnt = 0;
            return nError;
		}
		else if (IsOpen() == false)
        {
            dwStartTime = timeGetTime();
            while(IsOpen() == false)
            {
                nAutoClose_Errcnt++;

                if(nAutoClose_Errcnt > 3)
                {
                    nAutoClose_Errcnt = 0;
                    ADD_LOG("[AutoClose] ERR_HAND_LIMIT_SEN_ALL_OFF, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
					ADD_FDC_LOG("ERR/%d/4/11/%06d/%06d/%d/%d/%06d/0/0/%d/%d/%d/%d",
										ERR_HAND_LIMIT_SEN_ALL_OFF,
										m_pOHTMainStatus->StatusCommon.CurNodeID,
										m_pExecuteInfo->ExecutePositionInfo.NextNode,
										m_pOHTMainStatus->StatusCommon.CurNodeOffset,
										m_pOHTMainStatus->StatusCommon.ExistFoup,
										m_pOHTMainStatus->StatusCommon.StopStationID,
										IsOpen(),
										IsClose(),
										IsLeftGripOn(),
										IsRightGripOn());
                    return ERR_HAND_LIMIT_SEN_ALL_OFF;
                }
                Sleep(10);
                ADD_LOG("[AutoClose] Error Count(LIMIT All OFF) : %d, sleep time: %d(ms)", nAutoClose_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));

            }
            nAutoClose_Errcnt = 0;


        }

		else
		{
			nError = m_pHWCommon->SetBit(OUT_HAND_CLOSE, ON);
			if (nError)
			{
				nError = ERR_SEND_HAND_CLOSE_COMMAND;
				ADD_FDC_LOG("ERR/%d/1/7/%06d/%06d/%d/%d/%06d/%d/%d",
						ERR_SEND_HAND_CLOSE_COMMAND,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						IsOpen(),
						IsClose());
			}

            if (nError == NO_ERR)
				m_nMovingCount++;
        }
    }

	if (nError == NO_ERR)
    {
        dwStartTime = timeGetTime();
		nSendCommand = HAND_CLOSE_COMMAND;
        while ((nError = CheckComplete()) == NEXT_CHECK)
		{
            Sleep(10);
        }
    }

	if (nError == NO_ERR)
	{
        if ((IsLeftGripOn() == false) || (IsRightGripOn() == false))
        {
			dwStartTime = timeGetTime();
            while (1)
            {
                if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > (DWORD)HAND_GRIPON_WAIT_TIME)
                    break;
                if ((IsLeftGripOn() == true) && (IsRightGripOn() == true))
					break;
                Sleep(10);
			}

            if ((IsLeftGripOn() == false) || (IsRightGripOn() == false))
			{
                ADD_LOG("[AutoClose] ERR_HAND_SENSOR_OFFERR, %d(ms) wait time excess",(int)HAND_GRIPON_WAIT_TIME);
                nError = ERR_HAND_SENSOR_OFFERR;
				ADD_FDC_LOG("ERR/%d/3/7/%06d/%06d/%d/%d/%06d/%d/%d",
							ERR_HAND_SENSOR_OFFERR,
							m_pOHTMainStatus->StatusCommon.CurNodeID,
							m_pExecuteInfo->ExecutePositionInfo.NextNode,
							m_pOHTMainStatus->StatusCommon.CurNodeOffset,
							m_pOHTMainStatus->StatusCommon.ExistFoup,
							m_pOHTMainStatus->StatusCommon.StopStationID,
							IsRightGripOn(),
							IsLeftGripOn());
            }
		}
	}

	if ((nError != NO_ERR) && (nError != ERR_HAND_SENSOR_OFFERR))
	{
		nError = Origin();
		ADD_MD_LOG("Hand AutoClose Origin nError != ERR_HAND_SENSOR_OFFERR");

      // origin���� gripper offüũ ��ƾ�߰� 2011.03.08
        if (nError == NO_ERR)
        {
            dwStartTime = timeGetTime();
			while ((IsLeftGripOn() == false) || (IsRightGripOn() == false))
            {
                nAutoClose_Errcnt++;

                if(nAutoClose_Errcnt > 3)
                {
                    nAutoClose_Errcnt = 0;
                    ADD_LOG("[AutoClose] ERR_HAND_SENSOR_OFFERR, sleep time: %d(ms)", OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
					ADD_FDC_LOG("ERR/%d/4/7/%06d/%06d/%d/%d/%06d/%d/%d",
								ERR_HAND_SENSOR_OFFERR,
								m_pOHTMainStatus->StatusCommon.CurNodeID,
								m_pExecuteInfo->ExecutePositionInfo.NextNode,
								m_pOHTMainStatus->StatusCommon.CurNodeOffset,
								m_pOHTMainStatus->StatusCommon.ExistFoup,
								m_pOHTMainStatus->StatusCommon.StopStationID,
								IsRightGripOn(),
								IsLeftGripOn());
                    return ERR_HAND_SENSOR_OFFERR;
                }

                ADD_LOG("[AutoClose] Error Count(Grip Sen Off) : %d, sleep time: %d(ms)", nAutoClose_Errcnt, OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
				Sleep(10);
            }
            nAutoClose_Errcnt = 0;
        }
    }

    return nError;
}

/**
@brief   Hand ���͸� ���� ���ͽ�Ű�� �Լ�
@author  LSJ
@date    2012.10.30
@return  ���ۿ� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::Origin()
{
    int nError = NO_ERR;

	bReadyOperation = false;

	if (nError == NO_ERR)
		nError = Enable();
	if (nError == NO_ERR)
		nError = IO_Init();
	if (nError == NO_ERR)
		nError = AlarmReset();
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_HAND_ORIGIN, ON);
//		DiagManager* pDiagManager = DiagManager::getInstance();
//		pDiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HAND_ORIGIN); //AOHC-196 ���� �α� �߰�

	Sleep(500);

	if (nError)
	{
		nError = ERR_SEND_HAND_ORIGIN_COMMAND;

	}

	if(nError==NO_ERR) bReadyOperation = true;
//	else
//    {
//		dwStartTime = timeGetTime();
//		nSendCommand = HAND_ORIGIN_COMMAND;
//        while ((nError = CheckComplete()) == NEXT_CHECK)
//        {
//            Sleep(10);
//		}
//        if (nError == NO_ERR && bReadyOperation == false)
//            bReadyOperation = true;
//	}

    return nError;
}

/**
@brief   Hand�� �˶� �����̸� �˶��� �����ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::AlarmReset()
{
	int nError = NO_ERR;

	if (m_pHWCommon->m_Input.HAND_Alarm == ON)
	{

		//���ܸ���͸� �׸�(Hand Alarm reset)
//		DiagManager* pDiagManager = DiagManager::getInstance();
//
//		pDiagManager->IncreaseAbnormalHandResetCount();
//
//		pDiagManager->IncreasePreviosDiagCount(PREVIOS_DIAG_HAND_ALAM); //AOHC-196 ���� �α� �߰�
//
//		if(pDiagManager->GetAbnormalHandResetCount() > pDiagManager->GetDiagParam().nRefAbnormAlarmResetHand)
//		{
//			DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_ALARM_RESET_HAND);
//			pDiagManager->ResetAbnormalHandResetCount();
//		}

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
@brief   Hand ���͸� Enable ���·� ����� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::Enable()
{
	int nError = NO_ERR;

	if (m_pHWCommon->m_Output.HAND_Free == ON)
		nError = m_pHWCommon->SetBit(OUT_HAND_FREE, OFF);

	return nError;
}

/**
@brief   Hand ���͸� Free ���·� ����� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::Free()
{
    int nError = NO_ERR;
    bReadyOperation = false;

	nError = m_pHWCommon->SetBit(OUT_HAND_FREE, ON);

	return nError;
}

/**
@brief   ��� ��ɿ� �´� ������ ���� ���� �ð� ���� �Ϸ��ߴ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  ���� ���� ���� �� 0, ���� �߻��� �ش� ���� �ڵ� ����.
*/
int Hand::CheckComplete()
{
	DWORD dwHandMovingTime = 0;

	if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == HAND_ORIGIN_COMMAND) ?
		dwOriginLimitTime : dwMotionLimitTime))
	{
		switch (nSendCommand)
        {
		case HAND_OPEN_COMMAND:
			IO_Init();
			ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_HAND_OPEN_TIMEOVER,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
            return ERR_HAND_OPEN_TIMEOVER;

		case HAND_CLOSE_COMMAND:
			IO_Init();
			ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_HAND_CLOSE_TIMEOVER,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
			return ERR_HAND_CLOSE_TIMEOVER;

		case HAND_ORIGIN_COMMAND:
			IO_Init();
			HandPowerControl(PWR_RST);
			ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
						ERR_HAND_ORIGIN_TIMEOVER,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID,
						OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime));
            return ERR_HAND_ORIGIN_TIMEOVER;
        }
    }

    if (IsAlarm() == true)
    {
        IO_Init();
		ADD_FDC_LOG("ERR/%d/1/6/%06d/%06d/%d/%d/%06d/%d",
					ERR_HAND_ALARM,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsAlarm());
		return ERR_HAND_ALARM;
    }

    switch (nSendCommand)
	{
	case HAND_OPEN_COMMAND:

		if ((IsOpen() == true) && (IsInposition() == true))
		{
            dwHandMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
            if (dwMaxMovingTime < dwHandMovingTime)
                dwMaxMovingTime = dwHandMovingTime;

            IO_Init();
            return NO_ERR;
        }
        break;
	case HAND_ORIGIN_COMMAND:

		if ((IsClose() == true) && (IsInposition() == true))
        {
            if (IsInposition() == true)
            {
                IO_Init();
                return NO_ERR;
            }
        }
        break;
	case HAND_CLOSE_COMMAND:

		if ((IsClose() == true) && (IsInposition() == true))
        {
            if (IsInposition() == true)
            {
                dwHandMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
                dwStartTime = 0;
                if (dwMaxMovingTime < dwHandMovingTime)
                    dwMaxMovingTime = dwHandMovingTime;

                IO_Init();
                return NO_ERR;
            }
        }
        break;
    }
    return NEXT_CHECK;
}

/**
@brief   Hand Open Limit ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Open Limit ������ On�̸� true, Off�� False �� ����
*/
bool Hand::IsOpen(int nLeftRight)
{
	bool bResult = false;

	switch (nLeftRight)
	{

	default:
		if (m_pHWCommon->m_Input.HAND_Open == ON)
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;
}

/**
@brief   Hand Close Limit ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Close Limit ������ On�̸� true, Off�� False �� ����
*/
bool Hand::IsClose(int nLeftRight)
{
	bool bResult = false;

	switch (nLeftRight)
	{

	default:
		if (m_pHWCommon->m_Input.HAND_Close == ON)
			bResult = true;
		else
			bResult = false;
		break;
	}
	return bResult;
}

/**
@brief   Hand Alarm ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Alarm ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool Hand::IsAlarm()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_Alarm == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Free ���� �� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Free ���� ���� On�̸� true, Off�� False �� ����
*/
bool Hand::IsFree()
{
	bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Free == ON)
		bResult = true;
	else
		bResult = false;
	
	return bResult;
}

/**
@brief   Hand�� Origin �Ǿ� ������ �� �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Ready Operation Flag�� ����.
*/
bool Hand::IsReady()
{
    return bReadyOperation;
}

/**
@brief   Hand ���� Inposition ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand ���� Inposition ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool Hand::IsInposition()
{
    bool bResult = false;


	if (m_pHWCommon->m_Input.HAND_Inposition == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand PIO Go ��ȣ ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand PIO Go ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool Hand::IsGoSigOn()
{
	bool bResult = false;

	if (m_pHWCommon->m_Input.HAND_PIO_Go == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Left Gripper ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Left Gripper ������ On�̸� true, Off�� False �� ����
*/
bool Hand::IsLeftGripOn()
{
	bool bResult = false;

    if (m_pHWCommon->m_Input.HAND_Left_Grip == ON)
		bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief   Hand Right Gripper ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Right Gripper ������ On�̸� true, Off�� False �� ����
*/
bool Hand::IsRightGripOn()
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
@author  LSJ
@date    2012.10.30
@return  Hand Push ������ On�̸� true, Off�� False �� ����
*/
bool Hand::IsPushOn(int nLeftRight)
{
	bool bResult = false;
	switch (nLeftRight)
	{

	default:
		if (m_pHWCommon->m_Input.HAND_Push == ON)
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;
}

/**
@brief   Hand Push ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Close ��� ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool Hand::IsCloseOutputOn()
{
    bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Close == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Hand Push ���� ���¸� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Close ��� ��ȣ�� On�̸� true, Off�� False �� ����
*/
bool Hand::IsOpenOutputOn()
{
    bool bResult = false;

	if (m_pHWCommon->m_Output.HAND_Open == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}
/**
@brief   Hand�� Homing ���� �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Homing State Flag �� ����
*/
bool Hand::IsHomingState()
{
	return bHomingState;
}

/**
@brief   Hand Long Run �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Long Run Flag �� ����
*/
bool Hand::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Hand Long Run�� �����ϴ� �Լ�
@author  LSJ
@date    2012.10.30
*/
void Hand::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}

/**
@brief   Hand Long Run�� ���ߴ� �Լ�
@author  LSJ
@date    2012.10.30
*/
void Hand::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}

/**
@brief   Hand Moving Ƚ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@return  Hand Moving Count ����
*/
int Hand::GetMovingCount(bool bInit)
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
@author  LSJ
@date    2012.10.30
@return  Hand Max Moving Time ����
*/
DWORD Hand::GetMaxMovingTime()
{
	return dwMaxMovingTime;
}

/**
@brief   Hand �� ������ ��Ʈ�� �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@param   nCase(On/Off/Reset)�� ������ �� ����.
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Hand::HandPowerControl(int nMode)
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
@author  ���¿�
@date    2013.03.26
@return  Power ��� Return
*/
bool Hand::IsHandPowerOn()
{
	return !(m_pHWCommon->m_Output.HAND_Power_Reset);
}


/**
@brief   Hand �ΰ� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_HAND_NOTORIGIN ����.
*/
int Hand::ReadyToOperation()
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
@author  LSJ
@date    2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_HAND_NOTORIGIN ����.
*/
int Hand::AutoInitialize()
{
    int nError = 0;
    if (IsRightGripOn() == false && IsLeftGripOn() == false)
    {
      // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile("FOUP Check : NO FOUP(Normal case) ===> Hand Origin Start...");
		nError = Origin();
		ADD_MD_LOG("Hand AutoInitialize");
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

	else if (IsRightGripOn() == true && IsLeftGripOn() == true && IsClose() == true && IsOpen() == false)
    {
		bReadyOperation = true;
    }
    nError = ReadyToOperation();
    return nError;
}

unsigned int Hand::GetHandResetCount()
{
	return nCount_HandReset;
}

/**
@brief  ResetMovementData
@author
@date
*/
void Hand::ResetMovementData()
{
    m_nMovingCount = 0;
	dwMaxMovingTime = 0;
	nCount_HandReset = 0;
}
