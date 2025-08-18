// ---------------------------------------------------------------------------
/**
@file    RailSupport.cpp
@version OHT 7.0
@brief   RailSupport Class CPP File
*/

#include "RailSupport.h"

#define NOOUTTIRRGER 1

/**
@brief   RailSupport ������

 direction���� -> torque ���� (output�� �ݴ���)
 Motor Free : Tr1=1 Tr0=1
 Traction Torque : Tr1=1 Tr0=0
 Stop Torque : Tr1=0 Tr0=1
 Moving Torque : Tr1=0 Tr0=0
@author	 LSJ
@date    2012.11.01
@param   pHWCommon : HWCommon Class�� ������
*/
RailSupport::RailSupport(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
    Init();
}

/**
@brief   RailSupport �Ҹ���
@author	 LSJ
@date    2012.11.01
*/
RailSupport::~RailSupport()
{

}

/**
@brief   RailSupport�� ����� �� ���� ���� �ʱ�ȭ
@author	 LSJ
@date    2012.11.01
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
void RailSupport::Init()
{
    bReadyOperation = false;
	bLongRun = false;

	nMovingCount = 0;
    dwMaxMovingTime = 0;
    nSendCommand = 0;
	dwStartTime = 0;
	nDelayTime = 0;

	m_nCheckOuttiggerCount = 0;

	dwOriginLimitTime = RAILSUPPORT_PWRRST_LIMIT_TIME;
	dwMotionLimitTime = RAILSUPPORT_MOTION_LIMIT_TIME;

}

/**
@brief   RailSupport Unit�� Work Position���� �̵���Ŵ.
@author	 LSJ
@date    2012.11.01
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
int RailSupport::MoveWorkPos()
{
	int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
	if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
			  //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
			   //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
			  //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(50);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(50);

        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		Sleep(10);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount= 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#elif(OHT_NAME == OHT_NAME_STD)
  if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		Sleep(10);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount= 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
        dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_WORK_COMMAND;
		m_nCheckOuttiggerCount = 0;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
        }
    }
#endif
	return nError;
}

/**
@brief   RailSupport Unit�� Home Position���� �̵���Ŵ.
@author	 LSJ
@date    2012.11.01
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
int RailSupport::MoveHomePos()
{
	int nError = NO_ERR;
#if(  (OHT_NAME == OHT_NAME_STD_V80))
	if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
			//	m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
			//	m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
			 //	m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(50);

		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(50);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);

        Sleep(50);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
	else
	{
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
		nSendCommand = RAILSUPPORT_HOME_COMMAND;

		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
#elif(OHT_NAME == OHT_NAME_STD)
    if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

		if (nError == NO_ERR)
			nMovingCount++;
	}

	if (nError)
		nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
	else
	{
		dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
		m_nCheckOuttiggerCount = 0;
		while ((nError = CheckComplete()) == NEXT_CHECK)
		{
			Sleep(10);
		}
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

		if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
        dwStartTime = timeGetTime();
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
		m_nCheckOuttiggerCount= 0;
        while ((nError = CheckComplete()) == NEXT_CHECK)
        {
            Sleep(10);
        }
    }
#endif
	return nError;
}

/**
@brief   RailSupport Unit�� Work Position���� �̵���Ű�� ��ɸ� ����.
@author	 LSJ
@date    2013.04.23
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
int RailSupport::SendMoveWorkPosCmd()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
    if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }

		m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(10);

		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(10);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);

		Sleep(10);
//		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);


        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
	}
#elif(OHT_NAME == OHT_NAME_STD)
      if (nError == NO_ERR)
	{
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_WORK_COMMAND;
    }
#endif
    return nError;
}

/**
@brief   RailSupport Unit�� Home Position���� �̵���Ű�� ��ɸ� ����.
@author	 LSJ
@date    2013.04.23
@return	 ������ ��� 0, �������� ��� ErrorCode Return
*/
int RailSupport::SendMoveHomePosCmd()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80))
    if (nError == NO_ERR)
    {
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
//				m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);
		   }


        nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
		Sleep(10);

		//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_DIR_OUT, ON);
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
		Sleep(10);

		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, ON);

        Sleep(10);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_HOME_COMMAND;
	}
#elif(OHT_NAME == OHT_NAME_STD)
      if (nError == NO_ERR)
    {
		if ((m_pHWCommon->m_Output.Outtrigger_F_Brake_OUT == ON)||(m_pHWCommon->m_Output.Outtrigger_Reserved_78 == ON)||
		   (m_pHWCommon->m_Output.Outtrigger_F_DIR_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_77 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Speed_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_79 == ON) ||
		   (m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == ON) || (m_pHWCommon->m_Output.Outtrigger_Reserved_76 == ON))
		   {
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_78, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_77, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Speed_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_79, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
				m_pHWCommon->SetBit(OUT_Outtrigger_RESERVED_76, OFF);
		   }
		   Sleep(10);
 // nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
        nSendCommand = RAILSUPPORT_HOME_COMMAND;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
	if (nError == NO_ERR)
    {
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

        if (nError == NO_ERR)
            nMovingCount++;
	}

    if (nError)
        nError = ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD;
    else
    {
		dwStartTime = timeGetTime();
		m_nCheckOuttiggerCount= 0;
		nSendCommand = RAILSUPPORT_HOME_COMMAND;
    }
#endif
    return nError;
}

/**
@brief   ��� ��ɿ� �´� ������ ���� ���� �ð� ���� �Ϸ��ߴ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  ���� ���� ���� �� 0, ���� �߻��� �ش� ���� �ڵ� ����.
*/
int RailSupport::CheckComplete()
{
    DWORD dwRailSupportMovingTime = 0;

   // *** Error Occur : Timeover ****
    if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) >=
		((nSendCommand == RAILSUPPORT_ORIGIN_COMMAND) ? dwOriginLimitTime :
		dwMotionLimitTime))
	{
        switch (nSendCommand)
        {
		case RAILSUPPORT_HOME_COMMAND:
            Stop();
            return ERR_RAILSUPPORT_MOVE_HOME_TIMEOVER;
        case RAILSUPPORT_WORK_COMMAND:
			Stop();
			return ERR_RAILSUPPORT_MOVE_WORK_TIMEOVER;
        }
    }
		
#if(OHT_NAME == OHT_NAME_STD)
	m_nCheckOuttiggerCount++;

	switch (nSendCommand)
	{
	case RAILSUPPORT_ORIGIN_COMMAND:
		 // left���¿��� Pwr_Rst�� alarm���¿��� �ٷ� return�ǹǷ� alarm check�ؾ� ��...
        if (IsHomePos() == true)
        {
            Stop();
            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_HOME_COMMAND:
        if (IsHomePos() == true)
        {
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
            if (dwMaxMovingTime < dwRailSupportMovingTime)
				dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_WORK_COMMAND:
		if ((IsWorkPos() == true) && (m_nCheckOuttiggerCount >=40))
		{
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
			if (dwMaxMovingTime < dwRailSupportMovingTime)
                dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
	}
#else
	switch (nSendCommand)
    {
	case RAILSUPPORT_ORIGIN_COMMAND:
		 // left���¿��� Pwr_Rst�� alarm���¿��� �ٷ� return�ǹǷ� alarm check�ؾ� ��...
        if (IsHomePos() == true)
        {
            Stop();
            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_HOME_COMMAND:
		if ((IsHomePos() == true) || (m_nCheckOuttiggerCount > 3))
		{
			m_nCheckOuttiggerCount++;
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
            if (dwMaxMovingTime < dwRailSupportMovingTime)
				dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
    case RAILSUPPORT_WORK_COMMAND:
		if ((IsWorkPos(1) == true) || (m_nCheckOuttiggerCount > 3))
		{
			m_nCheckOuttiggerCount++;
            Stop();

            dwRailSupportMovingTime = OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime);
            dwStartTime = 0;
			if (dwMaxMovingTime < dwRailSupportMovingTime)
                dwMaxMovingTime = dwRailSupportMovingTime;

            bReadyOperation = true;
            return NO_ERR;
        }
        break;
	}
#endif
    return NEXT_CHECK;
}

/**
@brief   RailSupport ��ũ ���� ���� ��ũ�� �����ϰ� �ִ��� Ȯ���Ͽ� ����� �����ϴ� �Լ�
@author  LSJ
@date    2014.02.10
@return  ���� ��ũ �����̸� true, �̿��̸� false�� Return
*/
bool RailSupport::IsMovingTorque()
{
	bool bStatus = false;
#if((OHT_NAME == OHT_NAME_STD_V80))
	if ((m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == 1)) //&& (m_pHWCommon->m_Output.Outtrigger_R_Start_OUT == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
	}
#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Output.Outtrigger_F_Start_OUT == 1) && (m_pHWCommon->m_Output.Outtrigger_R_Start_OUT == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
	}
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
#else
	if ((m_pHWCommon->m_Output.RailSupport_TRQ_0 == 1) && (m_pHWCommon->m_Output.RailSupport_TRQ_1 == 1))
	{
		bStatus = true;
    }
    else
	{
		bStatus = false;
    }
#endif

	return bStatus;
}

/**
@brief   RailSupport �ַ����̵带 Free ��Ű�� �Լ�
@author  LSJ
@date    2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int RailSupport::Free()
{
    int nError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, OFF);
#endif
    return nError;
}

/**
/**
@brief   RailSupport �ַ����̵带 ���� ��ũ ���·� Enable ��Ű�� �Լ�
@author  LSJ
@date    2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int RailSupport::Enable()
{
    int nError = NO_ERR;
#if((OHT_NAME == OHT_NAME_STD)|| (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);
#endif
    return nError;
}

/**
@brief   �ַ����̵带 ���� ��ũ�� �ٲٴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int RailSupport::Stop()
{
    int nError = NO_ERR;

#if((OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V85) || (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, ON);
	Sleep(10);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_DIR_OUT, OFF);
#elif(OHT_NAME == OHT_NAME_STD)
   	nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Brake_OUT, ON);
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Brake_OUT, ON);
	Sleep(50);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_Outtrigger_F_Start_OUT, OFF);
	if (nError == NO_ERR)
	//nError = m_pHWCommon->SetBit(OUT_Outtrigger_R_Start_OUT, OFF);

#else
    nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);

#endif
	return nError;
}

/**
@brief   Rail Support ��ġ�� Work ��ġ ���� Ȯ���ϴ� �Լ�
@author	 LSJ
@date    2012.11.01
@return  Work ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.
*/
bool RailSupport::IsWorkPos(int nCheck)
{
	bool bResult = false;
#if((OHT_NAME == OHT_NAME_STD_V80))

	if(nCheck == 0)
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Alarm_IN == ON) || (m_pHWCommon->m_Input.Outtrigger_F_Work_IN == ON)) // || (m_pHWCommon->m_Input.Outtrigger_F_Home_IN == OFF))
			bResult = true;
		else
			bResult = false;
	}else if(nCheck == 1)
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Alarm_IN == ON))
			bResult = true;
		else
			bResult = false;
	}else
	{
		if ((m_pHWCommon->m_Input.Outtrigger_F_Work_IN == ON))
			bResult = true;
		else
			bResult = false;
    }

#if NOOUTTIRRGER
		bResult = false;
#endif

#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == ON) //&& (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == ON))
		bResult = true;
	else
		bResult = false;
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (m_pHWCommon->m_Input.Railsupport_Work == ON)
        bResult = true;
    else
        bResult = false;

#endif
	return bResult;
}

/**
@brief   Rail Support ��ġ�� Home ��ġ ���� Ȯ���ϴ� �Լ�
@author	 LSJ
@date    2012.11.01
@return  Home ����Ʈ ������ On �Ǿ� ������ true, Off�Ǿ� ������ false ����.
*/
bool RailSupport::IsHomePos()
{
	bool bResult = false;
#if((OHT_NAME == OHT_NAME_STD_V80))
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == ON))// && (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == OFF))
		bResult = true;
    else
		bResult = false;
#if NOOUTTIRRGER
		bResult = true;
#endif

#elif(OHT_NAME == OHT_NAME_STD)
	if ((m_pHWCommon->m_Input.Outtrigger_F_Home_IN == OFF) && (m_pHWCommon->m_Input.Outtrigger_R_Home_IN == OFF))
		bResult = true;
    else
		bResult = false;
// 2016-11-30, Labeler
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
#else
    if (m_pHWCommon->m_Input.RailSupport_Home == ON)
        bResult = true;
    else
        bResult = false;
#endif
    return bResult;
}

/**
@brief   Rail Support Long Run �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  Rail Support Long Run Flag �� ����
*/
bool RailSupport::IsLongRun()
{
    return bLongRun;
}

/**
@brief   Rail Support Long Run�� �����ϴ� �Լ�
@author  LSJ
@date    2012.11.01
*/
void RailSupport::StartLongRun()
{
    if (IsLongRun() == false)
        bLongRun = true;
}

/**
@brief   Rail Support Long Run�� ���ߴ� �Լ�
@author  LSJ
@date    2012.11.01
*/
void RailSupport::StopLongRun()
{
    if (IsLongRun() == true)
        bLongRun = false;
}

/**
@brief   Rail Support Moving Ƚ���� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  Rail Support Moving Count ����
*/
int RailSupport::GetMovingCount()
{
    return nMovingCount;
}

/**
@brief   Rail Support Max Moving Time�� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.01
@return  Rail Support Max Moving Time ����
*/
DWORD RailSupport::GetMaxMovingTime()
{
    return dwMaxMovingTime;
}

/**
@brief   RailSupport �ΰ� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_STEER_NOT_READY ����.
*/
int RailSupport::ReadyToOperation(void)
{
    int nError = NO_ERR;
    if (IsHomePos() != true)
        nError = ERR_RAILSUPPORT_NOT_READY;

    return nError;
}

/**
@brief   RailSupport ���� ���¿� ���� Origin ����� �ְ� �� �� ������ �غ� �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.11.15
@return  �غ� �Ǿ� ������ 0, �Ǿ� ���� ������ ERR_STEER_NOT_READY ����.
*/
int RailSupport::AutoInitialize()
{
    int nError = 0;
   // AnsiString strLogData;
   // strLogData.sprintf("Steering Auto Origin Start[E%d]", nRcvError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);

    //nError = MoveHomePos();

    if (IsHomePos() == true)
        bReadyOperation = true;
    else
        bReadyOperation = false;
   // strLogData.sprintf("Steering Auto Origin End..[E%d]", nError);
   // if(m_LogFile[LOG_RUNNING]->g_bIsUseFunction==false) m_LogFile[LOG_RUNNING]->WriteLogFile(strLogData);
   nError = ReadyToOperation();

    return nError;
}

/**
@brief	 RailSupport�� ���� ��°� ����Ʈ ������ Ʋ�� ��� ���� ��ũ�� RailSupport�� �����̴� �Լ�
@author  LSJ
@date	 2014.02.06
@return  ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int RailSupport::Adjust()
{
	int nError = 0;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD) ||  (OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80) || (OHT_NAME == OHT_NAME_STD_V80_2))
#else
	if (IsHomePos() == true)
	{
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, ON);
	}
	else if (IsWorkPos() == true)
	{
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_DIR, OFF);
	}

	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_0, OFF);
	if (nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_RAILSUPPORT_TRQ_1, ON);
#endif
	return nError;
}

// ---------------------------------------------------------------------------

