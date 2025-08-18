/**
@file	 Steer.cpp
@version OHT 7.0
@brief	 Steer Class CPP File
*/

#include "Steer.h"
#include "LogHandler.h"
#include "DiagManager.h"
#include "OHTMain.h"

#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)

/**
@brief	 Steer Class ������

 direction���� -> torque ���� (output�� �ݴ���)
 Motor Free : Tr1=1 Tr0=1
 Traction Torque : Tr1=1 Tr0=0
 Stop Torque : Tr1=0 Tr0=1
 Moving Torque : Tr1=0 Tr0=0
@author	 LSJ
@date	 2012.10.29
@param	 pHWCommon : HWCommon Class�� ������
*/
Steer::Steer(HWCommon * pHWCommon)
{
	m_pHWCommon = pHWCommon;
	Init();

	m_nMovingCount = 0;
	InitializeCriticalSection(&m_MovCntCS);

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();

	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
}

/**
@brief	 Steer Class �Ҹ���
@author	 LSJ
@date	 2012.10.29
*/
Steer::~Steer()
{
 	DeleteCriticalSection(&m_MovCntCS);
}

/**
@brief	 Steer class���� ����ϴ� �������� �ʱ�ȭ ��Ŵ.
@author	 LSJ
@date	 2012.10.29
*/
void Steer::Init()
{
	bReadyOperation = false;
	bLongRun = false;

	nSendCommand = 0;
	dwStartTime = 0;
	dwOriginLimitTime = STEER_PWRRST_LIMIT_TIME;
	dwMotionLimitTime = STEER_MOTION_LIMIT_TIME;
}

/**
@brief	 Steering�� ���� ��°� ����Ʈ ������ Ʋ�� ��� ���� ��ũ�� Steering�� �����̴� �Լ�
@author  LSJ
@date	 2012.11.01
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::Adjust()
{
	int nError = 0;

   // Front
	if (IsLeftPos(FRONT) == true	&&	IsRightPos(FRONT) == false)
	{
		if (m_pHWCommon->m_Output.STEER_F_Dir != OFF)
			nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	}
	else if (IsLeftPos(FRONT) == false	&&	IsRightPos(FRONT) == true)
	{
		if (m_pHWCommon->m_Output.STEER_F_Dir != ON)
			nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	}

   // Rear
	if (IsLeftPos(REAR) == true	&&	IsRightPos(REAR) == false)
	{
		if (m_pHWCommon->m_Output.STEER_R_Dir != OFF)
			nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
	}
	else if (IsLeftPos(REAR) == false	&&	IsRightPos(REAR) == true)
	{
		if (m_pHWCommon->m_Output.STEER_F_Dir != ON)
			nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
	}

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

	return nError;
}

/**
@brief	 �ַ����̵带 Left �������� �̵� ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 bTraction(�Ϲ� ������� ���� ������� ����)
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::Left(bool bTraction)
{
	int nError = NO_ERR;

	if (bTraction == true)
	{
		if (nError == NO_ERR)
			nError = SendTractionLeftCmd();
	}
	else
	{
		if (nError == NO_ERR)
			nError = SendLeftCmd();
	}

	if (nError)
	{
		nError = ERR_SEND_STEER_LEFT_COMMAND;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SEND_STEER_LEFT_COMMAND,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
	}
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_LEFT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}

/**
@brief	 �ַ����̵带 Right �������� �̵� ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 bTraction(�Ϲ� ������� ���� ������� ����)
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::Right(bool bTraction)
{
	int nError = NO_ERR;

	if (bTraction == true)
	{
		if (nError == NO_ERR)
			nError = SendTractionRightCmd();
	}
	else
	{
		if (nError == NO_ERR)
			nError = SendRightCmd();
	}

	if (nError)
	{
		nError = ERR_SEND_STEER_RIGHT_COMMAND;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_SEND_STEER_RIGHT_COMMAND,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
	}
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_RIGHT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}

/**
@brief	 Front������ LEFT �������� �̵� ��Ű�� �Լ�
@author  HJH
@date	 2019.05.09
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::FrontLeft()
{
	int nError = NO_ERR;
    nError = SendFrontCmd(false);

	if (nError)
		nError = ERR_SEND_STEER_LEFT_COMMAND;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_FRONTLEFT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}

/**
@brief	 Front������ Right �������� �̵� ��Ű�� �Լ�
@author  HJH
@date	 2019.05.09
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::FrontRight()
{
	int nError = NO_ERR;
    nError = SendFrontCmd(true);

	if (nError)
		nError = ERR_SEND_STEER_RIGHT_COMMAND;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_FRONTRIGHT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}

/**
@brief	 Rear������ LEFT �������� �̵� ��Ű�� �Լ�
@author  HJH
@date	 2019.05.09
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::RearLeft()
{
	int nError = NO_ERR;
    nError = SendRearCmd(false);

	if (nError)
		nError = ERR_SEND_STEER_LEFT_COMMAND;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_REARLEFT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}

/**
@brief	 Rear������ Right �������� �̵� ��Ű�� �Լ�
@author  HJH
@date	 2019.05.09
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::RearRight()
{
	int nError = NO_ERR;
    nError = SendRearCmd(true);

	if (nError)
		nError = ERR_SEND_STEER_RIGHT_COMMAND;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = STEER_REARRIGHT_COMMAND;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
	return nError;
}
/**
@brief	 ��� ��ɿ� �´� ������ ���� ���� �ð� ���� �Ϸ��ߴ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  ���� ���� ���� �� 0, ���� �߻��� �ش� ���� �ڵ� ����.
*/
int Steer::CheckComplete()
{
	DWORD dwSteerMovingTime = 0;
	static bool bFrontTimeCheck = false;
	static bool bRearTimeCheck = false;
	bool bSteerMoveTimeover[2] = {false,};


   // *** Error Occur : Timeover ****
	if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >= ((nSendCommand == STEER_ORIGIN_COMMAND) ?
		dwOriginLimitTime : dwMotionLimitTime))
	{
		switch (nSendCommand)
		{
		case STEER_LEFT_COMMAND:
        case STEER_FRONTLEFT_COMMAND:   //TP���� Front Left/Rear Left ���� ���� �߰�
        case STEER_REARLEFT_COMMAND:
			Stop();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STEER_LEFT_EXEC_TIMEOVER,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
			return ERR_STEER_LEFT_EXEC_TIMEOVER;

		case STEER_RIGHT_COMMAND:
        case STEER_FRONTRIGHT_COMMAND:    //TP���� Front Right/Rear Right ���� ���� �߰�
        case STEER_REARRIGHT_COMMAND:
			Stop();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STEER_RIGHT_EXEC_TIMEOVER,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
			return ERR_STEER_RIGHT_EXEC_TIMEOVER;

		case STEER_ORIGIN_COMMAND:
			Stop();
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STEER_ORIGIN_EXEC_TIMEOVER,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
			return ERR_STEER_ORIGIN_EXEC_TIMEOVER;
		}
	}

	switch (nSendCommand)
	{

	case STEER_ORIGIN_COMMAND:
		 // left���¿��� Pwr_Rst�� alarm���¿��� �ٷ� return�ǹǷ� alarm check�ؾ� ��...

		if (IsLeftPos(BOTH) == true)
		{
			Stop();
			bReadyOperation = true;
			return NO_ERR;
		}
		break;
	case STEER_LEFT_COMMAND:
		//Max Time Check
		if(IsLeftPos(FRONT) && bFrontTimeCheck ==false)
		{
			dwSteerMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);

			DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_STEERLT_MAX, dwSteerMovingTime);  //AOHC-196 ���� �α� �߰�
			//���ܸ���͸� �׸�(���� Steering timeover) //AOHC-288
			if(dwSteerMovingTime > DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveSteering)
			{
				bSteerMoveTimeover[0] = true;
			}

			bFrontTimeCheck = true;
		}
		else if(!(IsLeftPos(FRONT)))
			bFrontTimeCheck = false;

		if(IsLeftPos(REAR)&& bRearTimeCheck == false)
		{
			dwSteerMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);

			DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_STEERLT_MAX, dwSteerMovingTime);  //AOHC-196 ���� �α� �߰�
			//���ܸ���͸� �׸�(�Ĺ� Steering timeover) //AOHC-288
			if(dwSteerMovingTime > DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveSteering)
			{
				bSteerMoveTimeover[1] = true;
			}

			bRearTimeCheck = true;
		}
		else if(!(IsLeftPos(REAR)))
			bRearTimeCheck = false;


		//postion Check
		if (IsLeftPos(BOTH) == true)
		{
			Stop();

			dwStartTime = 0;

			bReadyOperation = true;
			return NO_ERR;
		}
		break;
	case STEER_RIGHT_COMMAND:
		//Max Time Check
		if(IsRightPos(FRONT) && bFrontTimeCheck ==false)
		{
			dwSteerMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);

			//���ܸ���͸� �׸�(���� Steering timeover) //AOHC-288
			DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_STEERRT_MAX, dwSteerMovingTime);  //AOHC-196 ���� �α� �߰�
			if(dwSteerMovingTime > DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveSteering)
			{
				bSteerMoveTimeover[0] = true;
			}

			bFrontTimeCheck = true;
		}
		else if(!(IsRightPos(FRONT)))
			bFrontTimeCheck = false;

		if(IsRightPos(REAR)&& bRearTimeCheck == false)
		{
			dwSteerMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);

			DiagManager::getInstance()->IncreasePreviosDiagMax(PREVIOS_DIAG_STEERRT_MAX, dwSteerMovingTime);  //AOHC-196 ���� �α� �߰�
			//���ܸ���͸� �׸�(�Ĺ� Steering timeover) //AOHC-288
			if(dwSteerMovingTime > DiagManager::getInstance()->GetDiagParam().nRefAbnormTimeoverMoveSteering)
			{
				bSteerMoveTimeover[1] = true;

			}
			bRearTimeCheck = true;
		}
		else if(!(IsRightPos(REAR)))
			bRearTimeCheck = false;

		//postion Check


		if (IsRightPos(BOTH) == true)
		{
			Stop();

			dwStartTime = 0;

			bReadyOperation = true;
			return NO_ERR;
		}
		break;
        //�Ŵ��� TP ���۽� Steering F/R �������� case , �������� ��üũ
        case STEER_FRONTLEFT_COMMAND:      //Front_Left ��� �� input üũ
		    if (IsLeftPos(FRONT) == true)
		    {
		    	Stop();

		    	dwStartTime = 0;

		    	return NO_ERR;
	    	}
		break;
        case STEER_FRONTRIGHT_COMMAND:     //Front_Right ��� �� input üũ
 		    if (IsRightPos(FRONT) == true)
		    {
		    	Stop();

		    	dwStartTime = 0;

		    	return NO_ERR;
	    	}
		break;
        case STEER_REARLEFT_COMMAND:       //Rear_Left ��� �� input üũ
		    if (IsLeftPos(REAR) == true)
		    {
		    	Stop();

		    	dwStartTime = 0;

		    	return NO_ERR;
	    	}
		break;
        case STEER_REARRIGHT_COMMAND:      //Rear_Right ��� �� input üũ
		    if (IsRightPos(REAR) == true)
		    {
		    	Stop();

		    	dwStartTime = 0;

		    	return NO_ERR;
	    	}
		break;

	}


	if(bReadyOperation == true)
	{
		//���ܸ���͸� �׸�(Timeover Ȯ��)
		if(bSteerMoveTimeover[0] == true || bSteerMoveTimeover[1] == true)
		{
			DiagManager::getInstance()->InsertAbnormalDiagData(DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_STEER);
		}
    }


	return NEXT_CHECK;
}

/**
@brief	 Steering ����Ʈ ������ �� ���¿� ���� üũ�ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 nSteerPos(���� ���� �Ǿ�� �� ��ġ)
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::CheckSensor(int nSteerPos)
{
	unsigned int nCheckData = 0;
	int nError = NO_ERR;

	nCheckData = (m_pHWCommon->m_Input.STEER_F_Left << 3) + (m_pHWCommon->m_Input.STEER_R_Left << 2) +
		(m_pHWCommon->m_Input.STEER_F_Right << 1) + (m_pHWCommon->m_Input.STEER_R_Right);

	switch (nCheckData)
	{
	case 0x00: // Front & Rear Steering No Sensing at Both Pos.
		nError = ERR_STR_F_NO_SEN_AND_R_NO_SEN;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_NO_SEN_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x01: // Front  No Sensing at Both Pos. & Rear Right Pos.
		nError = ERR_STR_F_NO_SEN_AND_R_RIGHT_POS;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_NO_SEN_AND_R_RIGHT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x02: // Front Right Pos. & Rear No Sensing at Both Pos.
		nError = ERR_STR_F_RIGHT_POS_AND_R_NO_SEN;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_RIGHT_POS_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x03: // Front Right Pos. & Rear Right Pos.
		if (nSteerPos == STEER_LEFT_POS)
		{
			nError = ERR_STR_F_RIGHT_POS_AND_R_RIGHT_POS;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_RIGHT_POS_AND_R_RIGHT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
        }
		else
			nError = NO_ERR;
		break;
	case 0x04: // Front No Sensing at Both Pos. & Rear Left Pos.
		nError = ERR_STR_F_NO_SEN_AND_R_LEFT_POS;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_NO_SEN_AND_R_LEFT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x06: // Front Right Pos. & Rear Left Pos.
		nError = ERR_F_RIGHT_POS_AND_R_LEFT_POS;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_F_RIGHT_POS_AND_R_LEFT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x08: // Front Left Pos. & Rear No Sensing at Both Pos.
		nError = ERR_F_LEFT_POS_AND_R_NO_SEN;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_F_LEFT_POS_AND_R_NO_SEN,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x09: // Front Left Pos. & Rear Right Pos.
		nError = ERR_F_LEFT_POS_AND_R_RIGHT_POS;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_F_LEFT_POS_AND_R_RIGHT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		break;
	case 0x0C: // Front Left Pos. & Rear Left Pos.
		if (nSteerPos == STEER_RIGHT_POS)
		{
			nError = ERR_STR_F_LEFT_POS_AND_R_LEFT_POS;
			ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_F_LEFT_POS_AND_R_LEFT_POS,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
		}
		else
			nError = NO_ERR;
		break;
	default: // Front or Rear Sensing at Boat Pos.
		nError = ERR_STR_BOTH_POS_SENING;
		ADD_FDC_LOG("ERR/%d/1/9/%06d/%06d/%d/%d/%06d/%d/%d/%d/%d",
					ERR_STR_BOTH_POS_SENING,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID,
					IsFrontLeftOn(),
					IsFrontRightOn(),
					IsRearLeftOn(),
					IsRearRightOn());
	}

	return nError;
}

/**
@brief	 �ַ����̵带 Left �������� �̵� ��Ű�� ��� ���� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendLeftCmd()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);


	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();



	return nError;
}

/**
@brief	 �ַ����̵带 Right �������� �̵� ��Ű�� ��� ���� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendRightCmd()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

	//���ܵ����� ������Ʈ
	if (nError == NO_ERR)
		IncreaseMovingCount();


	return nError;
}

 /**
@brief	 Front ������ Left/Right �������� �̵� ��Ű�� ��� ���� �Լ�
@author  HJH
@date	 2019.05.09
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendFrontCmd(bool bSteer)
{
	int nError = NO_ERR;
    if(bSteer==false) // left�� ��
    {
        nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	    if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
    	if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
    }
    else // right�� ��
    {
        nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	    if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
    	if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
    }

	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();


	return nError;
}

 /**
@brief	 Rear ������ Left/Right �������� �̵� ��Ű�� ��� ���� �Լ�
@author  HJH
@date	 2019.05.09
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendRearCmd(bool bSteer)
{
	int nError = NO_ERR;

    if(bSteer == false) // left
    {
        nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
    	if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
    	if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);
    }
    else                //right
    {
        nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
    	if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
    	if (nError == NO_ERR)
	    	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);
    }



	//���ܵ����� ������Ʈ
	if (nError == NO_ERR)
		IncreaseMovingCount();


	return nError;
}


/**
@brief	 �ַ����̵带 left �������� ���ϰ� �̵� ��Ű�� ��� ���� �Լ�
@author  ���¿�
@date	 2012.12.09
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::SendLeftCmdWeakly()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);


	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();

	return nError;
}
*/

/**
@brief	 �ַ����̵带 Right �������� ���ϰ� �̵� ��Ű�� ��� ���� �Լ�
@author  ���¿�
@date	 2012.12.09
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::SendRightCmdWeakly()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();

	return nError;
}
*/


/**
@brief	 �ַ����̵带 ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::Stop()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

	return nError;
}


/**
@brief	 �ַ����̵带 ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::Resume()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, OFF);

	return nError;
}
*/


/**
@brief	 ���ο� ��ũ ������ Left �������� �̵��ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendTractionLeftCmd()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, OFF);

	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();


	return nError;
}

/**
@brief	 ���ο� ��ũ ������ Right �������� �̵��ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::SendTractionRightCmd()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, ON);
	if (nError == NO_ERR)
		// by zzang9un 2013.06.21 : IO ����
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, OFF);

	//�ű� ����Agent, ������ ������Ʈ
	if(nError == NO_ERR)
		IncreaseMovingCount();


	return nError;
}

/**
@brief	 Front Steering �ַ����̵带 Left ���⿡�� ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::FrontLeftStop()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);

   /// @todo �α� ��� ��� Ȯ���Ǹ� ���� �ʿ�

//   AnsiString strdd;
//   strdd.sprintf("-> FrontLeftStop[Drv Pos=%.02f] Node=%d Left[%d][%d], Right[%d][%d]",
//					 gm_OHTStatus->Motor_Driving_F->dPosition_cts,
//					 gm_OHTStatus->nCurNode,
//					 gm_OHTStatus->IOInput->nSteer_Front_Left,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Left,
//					 gm_OHTStatus->IOInput->nSteer_Front_Right,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Right);
//   if(m_LogFile[LOG_DRVEXEC]->g_bIsUseFunction==false) m_LogFile[LOG_DRVEXEC]->WriteLogFile( strdd);


	return nError;
}
*/

/**
@brief	 Rear Steering �ַ����̵带 Left ���⿡�� ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::RearLeftStop()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

   /// @todo �α� ��� ��� Ȯ���Ǹ� ���� �ʿ�

//   AnsiString strdd;
//   strdd.sprintf("-> RearLeftStop[Drv Pos=%.02f] Node=%d Left[%d][%d], Right[%d][%d]",
//					 gm_OHTStatus->Motor_Driving_F->dPosition_cts,
//					 gm_OHTStatus->nCurNode,
//					 gm_OHTStatus->IOInput->nSteer_Front_Left,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Left,
//					 gm_OHTStatus->IOInput->nSteer_Front_Right,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Right);
//   if(m_LogFile[LOG_DRVEXEC]->g_bIsUseFunction==false) m_LogFile[LOG_DRVEXEC]->WriteLogFile( strdd);


	return nError;
}
 */

/**
@brief	 Front Steering �ַ����̵带 Right ���⿡�� ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::FrontRightStop()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);

   /// @todo �α� ��� ��� Ȯ���Ǹ� ���� �ʿ�

//   AnsiString strdd;
//   strdd.sprintf("-> FrontRightStop[Drv Pos=%.02f] Node=%d Left[%d][%d], Right[%d][%d]",
//					 gm_OHTStatus->Motor_Driving_F->dPosition_cts,
//					 gm_OHTStatus->nCurNode,
//					 gm_OHTStatus->IOInput->nSteer_Front_Left,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Left,
//					 gm_OHTStatus->IOInput->nSteer_Front_Right,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Right);
//   if(m_LogFile[LOG_DRVEXEC]->g_bIsUseFunction==false) m_LogFile[LOG_DRVEXEC]->WriteLogFile( strdd);


	return nError;
}
*/

/**
@brief	 Rear Steering �ַ����̵带 Right ���⿡�� ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::RearRightStop()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_R_DIR, ON);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

   /// @todo �α� ��� ��� Ȯ���Ǹ� ���� �ʿ�

//   AnsiString strdd;
//   strdd.sprintf("-> RearRightStop[Drv Pos=%.02f] Node=%d Left[%d][%d], Right[%d][%d]",
//					 gm_OHTStatus->Motor_Driving_F->dPosition_cts,
//					 gm_OHTStatus->nCurNode,
//					 gm_OHTStatus->IOInput->nSteer_Front_Left,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Left,
//					 gm_OHTStatus->IOInput->nSteer_Front_Right,
//					 gm_OHTStatus->IOInput->nSteer_Rear_Right);
//   if(m_LogFile[LOG_DRVEXEC]->g_bIsUseFunction==false) m_LogFile[LOG_DRVEXEC]->WriteLogFile( strdd);


	return nError;
}
*/

/**
@brief	 Front Steering �ַ����̵带 Free ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::FrontFree()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, OFF);

	return nError;
}

/**
@brief	 Rear Steering �ַ����̵带 Free ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::RearFree()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, OFF);

	return nError;
}

/**
@brief	 Front Steering �ַ����̵带 ���� ��ũ ���·� Enable ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::FrontEnable()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, ON);

	return nError;
}

/**
@brief	 Rear Steering �ַ����̵带 ���� ��ũ ���·� Enable ��Ű�� �Լ�
@author  LSJ
@date	 2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Steer::RearEnable()
{
	int nError = NO_ERR;

	nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, ON);

	return nError;
}

/**
@brief	 Steering Long Run �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@return  Steering Long Run Flag �� ����
*/
bool Steer::IsLongRun()
{
	return bLongRun;
}

/**
@brief	 Steering Long Run�� �����ϴ� �Լ�
@author  LSJ
@date	 2012.11.01

void Steer::StartLongRun()
{
	if (IsLongRun() == false)
		bLongRun = true;
}
*/

/**
@brief	 Steering Long Run�� ���ߴ� �Լ�
@author  LSJ
@date	 2012.11.01

void Steer::StopLongRun()
{
	if (IsLongRun() == true)
		bLongRun = false;
}
*/


/**
@brief	 Steering ��ġ�� Left ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 nFrontRear(Front Steering�� ����, Rear Steering�� ����, �� �� ���� ����)
@return  Left ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.
*/
bool Steer::IsLeftPos(int nFrontRear)
{
	bool bResult = false;

	switch (nFrontRear)
	{
	case FRONT:
		if (m_pHWCommon->m_Input.STEER_F_Left == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case REAR:
		if (m_pHWCommon->m_Input.STEER_R_Left == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case BOTH:
		if ((m_pHWCommon->m_Input.STEER_F_Left == ON) && (m_pHWCommon->m_Input.STEER_R_Left == ON))
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;
}

/**
@brief	 Steering ��ġ�� Right ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 nFrontRear(Front Steering�� ����, Rear Steering�� ����, �� �� ���� ����)
@return  Right ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.
*/
bool Steer::IsRightPos(int nFrontRear)
{
	bool bResult = false;

	switch (nFrontRear)
	{
	case FRONT:
		if (m_pHWCommon->m_Input.STEER_F_Right ==ON)
			bResult = true;
		else
			bResult = false;
		break;
	case REAR:
		if (m_pHWCommon->m_Input.STEER_R_Right == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case BOTH:
		if ((m_pHWCommon->m_Input.STEER_F_Right == ON) && (m_pHWCommon->m_Input.STEER_R_Right == ON))
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;
}

/**
@brief	 Steering ��ġ�� Left ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 nFrontRear(Front Steering�� ����, Rear Steering�� ����, �� �� ���� ����)
@return  Left ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.

bool Steer::IsLeftCmd(int nFrontRear)
{
	bool bResult = false;

	switch (nFrontRear)
	{
	case FRONT:
		if (m_pHWCommon->m_Output.STEER_F_Dir==OFF)
			bResult = true;
		else
			bResult = false;
		break;
	case REAR:
		if (m_pHWCommon->m_Output.STEER_R_Dir== OFF)
			bResult = true;
		else
			bResult = false;
		break;
	case BOTH:
		if ((m_pHWCommon->m_Output.STEER_F_Dir == OFF) && (m_pHWCommon->m_Output.STEER_R_Dir == OFF))
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;
}
*/

/**
@brief	 Steering ��ġ�� Right ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.01
@param	 nFrontRear(Front Steering�� ����, Rear Steering�� ����, �� �� ���� ����)
@return  Right ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.

bool Steer::IsRightCmd(int nFrontRear)
{
	bool bResult = false;

	switch (nFrontRear)
	{
	case FRONT:
		if (m_pHWCommon->m_Output.STEER_F_Dir==ON)
			bResult = true;
		else
			bResult = false;
		break;
	case REAR:
		if (m_pHWCommon->m_Output.STEER_R_Dir== ON)
			bResult = true;
		else
			bResult = false;
		break;
	case BOTH:
		if ((m_pHWCommon->m_Output.STEER_F_Dir == ON) && (m_pHWCommon->m_Output.STEER_R_Dir == ON))
			bResult = true;
		else
			bResult = false;
		break;
	}

	return bResult;

}
*/

/**
@brief	 Steering ��ũ ���� ���� ��ũ�� �����ϰ� �ִ��� Ȯ���ϰ� ���� ��ũ�� ���� �ð� ���� �Ǹ� ���� ��ũ�� �ٲٴ� �Լ�

 Moving Torque�� ���ʵ��� ���ӵ� ��� Solenoid �ļ����� ���� ���������� Stop Torque�κ���
@author  LSJ
@date	 2012.11.01
@param	 bResetCount : Auto Going ���¸� true, �ƴϸ� False
@return  Auto ���� �ų� Going �����̸� false ����, ���� ��ũ�� �ٲ� �� SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return

int Steer::Check_Steering_Torque(bool bResetCount)
{
	int nError = NO_ERR;
	static int nFCount = 0;
	static int nRCount = 0;

   if(bResetCount == true)
   {
	  nFCount=0;
	  nRCount=0;
	  return nError;
   }

	if (((m_pHWCommon->m_Output.STEER_F_TRQ_0 == 1) && (m_pHWCommon->m_Output.STEER_F_TRQ_1 == 1)) || ((m_pHWCommon->m_Output.STEER_F_TRQ_0 == 1) && (m_pHWCommon->m_Output.STEER_F_TRQ_0 == 0)))
	{
		nFCount++;
		if (nFCount > 1000)
		{
			nFCount = 0;
			Stop();
		}
	}
	else
	{
		nFCount = 0;
	}

	if (((m_pHWCommon->m_Output.STEER_R_TRQ_0 == 1) && (m_pHWCommon->m_Output.STEER_R_TRQ_1 == 1)) || ((m_pHWCommon->m_Output.STEER_R_TRQ_0 == 1) && (m_pHWCommon->m_Output.STEER_R_TRQ_1 == 0)))
	{
		nRCount++;
		if (nRCount > 1000)
		{
			nRCount = 0;
			Stop();
		}
	}
	else
	{
		nRCount = 0;
	}
	return nError;
}
*/

/**
@brief	 Steering �ΰ� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_STEER_NOT_READY ����.
*/
int Steer::ReadyToOperation(void)
{
	int nError = NO_ERR;
	nError = CheckSensor(STEER_NO_POS);
	if (nError != NO_ERR)
		nError = ERR_STEER_NOT_READY;

	return nError;
}

/**
@brief	 Steering ���� ���¿� ���� Origin ����� �ְ� �� �� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date	 2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_STEER_NOT_READY ����.
*/
int Steer::AutoInitialize()
{
	int nError = 0;
   // AnsiString strLogData;
   // strLogData.sprintf("Steering Auto Origin Start[E%d]", nRcvError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);

	nError = Left(false);
	if (nError == NO_ERR)
	{
		nError = CheckSensor(STEER_LEFT_POS);
	}
	if (nError == NO_ERR)
		bReadyOperation = true;
	else
		bReadyOperation = false;
   // strLogData.sprintf("Steering Auto Origin End..[E%d]", nError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);
   nError = ReadyToOperation();

	return nError;
}




//1	New Way


/**
@brief	 Front Steer Left On ���� Ȯ��
@author  ehoto
@date	 2013.07.01
*/
bool Steer::IsFrontLeftOn()
{
	return ((m_pHWCommon->m_Input.STEER_F_Left==ON)?true:false);
}
/**
@brief	 Rear Steer Left On ���� Ȯ��
@author  ehoto
@date	 2013.07.01
*/
bool Steer::IsFrontRightOn()
{
	return ((m_pHWCommon->m_Input.STEER_F_Right==ON)?true:false);
}
/**
@brief	 Front Steer Right On ���� Ȯ��
@author  ehoto
@date	 2013.07.01
*/
bool Steer::IsRearLeftOn()
{
	return ((m_pHWCommon->m_Input.STEER_R_Left==ON)?true:false);
}
/**
@brief	 Rear Steer Right On ���� Ȯ��
@author  ehoto
@date	 2013.07.01
*/
bool Steer::IsRearRightOn()
{
	return ((m_pHWCommon->m_Input.STEER_R_Right==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}

/**
@brief	 Steer Output On ���� Ȯ��
@author  Moon
@date	 2019.12.26
*/
bool Steer::IsFTorque0On()
{
	return ((m_pHWCommon->m_Output.STEER_F_TRQ_0==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}
/*
bool Steer::IsFTorque1On()
{
	return ((m_pHWCommon->m_Output.STEER_F_TRQ_1==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}
*/
bool Steer::IsRTorque0On()
{
	return ((m_pHWCommon->m_Output.STEER_R_TRQ_0==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}
/*
bool Steer::IsRTorque1On()
{
	return ((m_pHWCommon->m_Output.STEER_R_TRQ_1==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}
*/
bool Steer::IsFDirOn()
{
	return ((m_pHWCommon->m_Output.STEER_F_Dir==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}

bool Steer::IsRDirOn()
{
	return ((m_pHWCommon->m_Output.STEER_R_Dir==ON)?true:false);
	// m_pHWCommon->m_Output.CID_Common_Tag
}

/**
@brief	 Front Steer ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_DIRECTION Steer::GetFrontPos()
{
	HW_STEER_DIRECTION ResultDirection = HW_STEER_DIRECTION_NONE;

	if(m_pHWCommon->m_Input.STEER_F_Left==ON && m_pHWCommon->m_Input.STEER_F_Right==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_ERROR;
	}
	else if(m_pHWCommon->m_Input.STEER_F_Left==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_LEFT;
	}
	else if(m_pHWCommon->m_Input.STEER_F_Right==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_RIGHT;
	}

	return ResultDirection;
}

/**
@brief	 Rear Steer ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_DIRECTION Steer::GetRearPos()
{
	HW_STEER_DIRECTION ResultDirection = HW_STEER_DIRECTION_NONE;

	if(m_pHWCommon->m_Input.STEER_R_Left==ON && m_pHWCommon->m_Input.STEER_R_Right==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_ERROR;
	}
	else if(m_pHWCommon->m_Input.STEER_R_Left==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_LEFT;
	}
	else if(m_pHWCommon->m_Input.STEER_R_Right==ON)
	{
		ResultDirection = HW_STEER_DIRECTION_RIGHT;
	}

	return ResultDirection;

}

/**
@brief	 Front Steer ��� ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_DIRECTION Steer::GetFrontCmd()
{
	HW_STEER_DIRECTION ResultDirection = HW_STEER_DIRECTION_NONE;

	if(m_pHWCommon->m_Output.STEER_F_Dir==OFF)
	{
		ResultDirection = HW_STEER_DIRECTION_LEFT;
	}
	else
	{
		ResultDirection = HW_STEER_DIRECTION_RIGHT;
	}

	return ResultDirection;

}

/**
@brief	 Rear Steer ��� ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_DIRECTION Steer::GetRearCmd()
{
	HW_STEER_DIRECTION ResultDirection = HW_STEER_DIRECTION_NONE;

	if(m_pHWCommon->m_Output.STEER_R_Dir==OFF)
	{
		ResultDirection = HW_STEER_DIRECTION_LEFT;
	}
	else
	{
		ResultDirection = HW_STEER_DIRECTION_RIGHT;
	}

	return ResultDirection;

}

/**
@brief	 Front Torque ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_TORQUE Steer::GetFrontTorque()
{
	HW_STEER_TORQUE ResultTorque = HW_STEER_TORQUE_FREE;

	if(m_pHWCommon->m_Output.STEER_F_TRQ_0==OFF)
	{
		if(m_pHWCommon->m_Output.STEER_F_TRQ_1==OFF)
		{
			ResultTorque = HW_STEER_TORQUE_FREE;
		}
		else
		{
			ResultTorque = HW_STEER_TORQUE_STOP;
		}
	}
	else
	{
		if(m_pHWCommon->m_Output.STEER_F_TRQ_1==OFF)
		{
			ResultTorque = HW_STEER_TORQUE_TRACTION;
		}
		else
		{
			ResultTorque = HW_STEER_TORQUE_MOVING;
		}
	}

	return ResultTorque;
}

/**
@brief	 Rear Torque ���� ����
@author  ehoto
@date	 2013.07.01
*/
HW_STEER_TORQUE Steer::GetRearTorque()
{
	HW_STEER_TORQUE ResultTorque = HW_STEER_TORQUE_FREE;

	if(m_pHWCommon->m_Output.STEER_R_TRQ_0==OFF)
	{
		if(m_pHWCommon->m_Output.STEER_R_TRQ_1==OFF)
		{
			ResultTorque = HW_STEER_TORQUE_FREE;
		}
		else
		{
			ResultTorque = HW_STEER_TORQUE_STOP;
		}
	}
	else
	{
		if(m_pHWCommon->m_Output.STEER_R_TRQ_1==OFF)
		{
			ResultTorque = HW_STEER_TORQUE_TRACTION;
		}
		else
		{
			ResultTorque = HW_STEER_TORQUE_MOVING;
		}
	}

	return ResultTorque;

}

/**
@brief	 Front Direction ���
@author  ehoto
@date	 2013.07.02
*/
int Steer::SetFrontDirection(HW_STEER_DIRECTION_CMD Direction)
{
	bool bDirection = OFF;

	switch(Direction)
	{
	case HW_STEER_DIRECTION_CMD_LEFT:
		bDirection = OFF;
		break;
	case HW_STEER_DIRECTION_CMD_RIGHT:
		bDirection = ON;
		break;
	}

	dwStartTime = timeGetTime();

	return m_pHWCommon->SetBit(OUT_STEER_F_DIR, bDirection);
}
/**
@brief	 Rear Direction ���
@author  ehoto
@date	 2013.07.02
*/
int Steer::SetRearDirection(HW_STEER_DIRECTION_CMD Direction)
{
	bool bDirection = OFF;

	switch(Direction)
	{
	case HW_STEER_DIRECTION_CMD_LEFT:
		bDirection = OFF;
		break;
	case HW_STEER_DIRECTION_CMD_RIGHT:
		bDirection = ON;
		break;
	}

    dwStartTime = timeGetTime();

	return m_pHWCommon->SetBit(OUT_STEER_R_DIR, bDirection);
}
/**
@brief	 Front Torque ���
@author  ehoto
@date	 2013.07.02
*/

int Steer::SetFrontTorque(HW_STEER_TORQUE Torque)
{
	int nError = NO_ERR;
	bool bTorque[2] = {OFF,};

	switch(Torque)
	{
	case HW_STEER_TORQUE_FREE:
		bTorque[0] = OFF;
		bTorque[1] = OFF;
		break;
	case HW_STEER_TORQUE_STOP:
		bTorque[0] = OFF;
		bTorque[1] = ON;
		break;
	case HW_STEER_TORQUE_MOVING:
		bTorque[0] = ON;
		bTorque[1] = ON;
		break;
	case HW_STEER_TORQUE_TRACTION:
		bTorque[0] = ON;
		bTorque[1] = OFF;
		break;
	}

	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_0, bTorque[0]);
	}
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_STEER_F_TRQ_1, bTorque[1]);
	}
	return nError;

}
/**
@brief	 Rear Torque ���
@author  ehoto
@date	 2013.07.02
*/

int Steer::SetRearTorque(HW_STEER_TORQUE Torque)
{
	int nError = NO_ERR;
	bool bTorque[2] = {OFF,};

	switch(Torque)
	{
	case HW_STEER_TORQUE_FREE:
		bTorque[0] = OFF;
		bTorque[1] = OFF;
		break;
	case HW_STEER_TORQUE_STOP:
		bTorque[0] = OFF;
		bTorque[1] = ON;
		break;
	case HW_STEER_TORQUE_MOVING:
		bTorque[0] = ON;
		bTorque[1] = ON;
		break;
	case HW_STEER_TORQUE_TRACTION:
		bTorque[0] = ON;
		bTorque[1] = OFF;
		break;
	}

	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_0, bTorque[0]);
	}
	if (nError == NO_ERR)
	{
		nError = m_pHWCommon->SetBit(OUT_STEER_R_TRQ_1, bTorque[1]);
	}

	return nError;

}

void Steer::IncreaseMovingCount()
{
	EnterCriticalSection(&m_MovCntCS);
	m_nMovingCount++;
	LeaveCriticalSection(&m_MovCntCS);
}

int Steer::GetMovingCount(bool bInit)
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
