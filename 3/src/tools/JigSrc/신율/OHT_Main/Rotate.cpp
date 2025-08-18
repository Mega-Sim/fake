/**
@file    Rotate.cpp
@version OHT 8.0
@brief   Rotate Class CPP File
*/

#include "Rotate.h"
#include "OHTMain.h"

//#define ROTATE_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Rotate.log", __VA_ARGS__)
//����
#define ROTATE_LOG(...)    WRITE_LOG_UNIT(pLogRotate, __VA_ARGS__)


/**
@brief   Rotate Class ������
@author  Labeler
@date    2016-11-25
@param   pHWCommon : HWCommon Ŭ���� ������
*/
Rotate::Rotate(HWCommon *pHWCommon)
{
	pLogRotate = getLogUnit("D:\\log\\Rotate.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
    m_pHWCommon = pHWCommon;
    m_pCommPort = NULL;

	m_bCmdExec = false;
	InitializeCriticalSection(&m_CS_CmdExec);
    bInit = false;
	m_nError = Init();
}

/**
@brief   Rotate Class �Ҹ���
@author
@date
*/
Rotate::~Rotate()
{
	Close();
	m_bCmdExec = true;
	DeleteCriticalSection(&m_CS_CmdExec);
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Init()
{
    int nError = NO_ERR;
    m_nPortNum = COM_ROTATE;

	m_nIdxCurCmd = 0;
	m_nRecvFailCnt = 0;
	torque = 0;

    position_cts = 0;
    position_degree = 0;
    Velocity_ctsPsec = 0;

    bOrigin = false;
    //bCmdExec = false;
    bRunning = false;
    bMotorOn = false;

    bReqPause = false;
    bReqPosStatus = false;
    bReqVelStatus = false;
    bReqMotorStatus = false;
    //bReqABSMoveStatus = false;

    dwStartReqPosTime = 0;
    dwStartReqVelTime = 0;
    dwStartReqStatusTime = 0;

    nCntIncorrectResponse = 0;
	nAbnormalCheckMotorStatus = 0;
    nError = Open();

    if(NO_ERR == nError)
    {
        bInit = true;
    }
    else
    {
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Open()
{
    int nError = NO_ERR;

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start ***

    if(NULL == m_pCommPort)
    {
        m_pCommPort = new Comm_Port(m_nPortNum, ROTATE_BAUDRATE);

        if(NULL == m_pCommPort)
        {
            ROTATE_LOG("[Open] Fail");
            nError = ERR_ROTATE_OPEN;
        }
        else if (INVALID_HANDLE_VALUE == m_pCommPort->m_hCOM)
        {
            delete m_pCommPort;
            m_pCommPort = NULL;
            ROTATE_LOG("[Open] Fail - INVALID_HANDLE_VALUE");
            nError = ERR_ROTATE_OPEN;
        }
        else
        {
            ROTATE_LOG("[Open] Success");
        }
    }
	LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ***
	m_bCmdExec = false;

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
void Rotate::Close()
{
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

	if(m_pCommPort!=NULL)
    {
		delete m_pCommPort;
		m_pCommPort = NULL;
        ROTATE_LOG("[Close] Success");
	}

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
}

/**
@brief   Rotate Motor Enable Func
@author  Labeler
@date    2016-11-25
@return  Enable ���� �� Error �߻� ErrorCode Return
         �̹߻� �Ұ�� NO_ERR Return
*/
int Rotate::Enable()
{
	if (NULL == m_pCommPort)
	{
		bOrigin = false;
		m_nError = ERR_ROTATE_NOT_CONNECTED;
		ROTATE_LOG("Enable Failed. No Connection");
		return m_nError;
	}
	if (bMotorOn)
	{
		ROTATE_LOG("Alread Enabled");
		return NO_ERR;
	}

    // Rotate Motor�� Disable ������ ��쿡��, Enable ����
    char ReadBuff[1024] = {0, };
    char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    // Enable �۾� �Ŀ���, �ݵ�� Origin ���� �ʿ�
    bOrigin = false;

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// ** Critical Section Started ***
	ROTATE_LOG("Enable Start");
    Sleep(20);

    sprintf(strMsg, "%s%c%c", ROTATE_ENABLE, CR, LF);
	PurgeComm_s();
	if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
    {

		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] ERR_ROTATE_WRITE");
		m_nError = ERR_ROTATE_WRITE;
		return m_nError;
    }

	ROTATE_LOG("[Enable] Data Sent: %s", strMsg);

    Sleep(COMM_WAIT_TIME);
	if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
    {
		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] Recv Failed");
		bOrigin = false;
		m_nError = ERR_ROTATE_NO_RESPONSE;
		return m_nError;
    }

    strReceiveData = ReadBuff;
    if(0 == strReceiveData.Pos(strMsg))
    {
		LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
		m_bCmdExec = false;
		ROTATE_LOG("[Enable] ERR_ROTATE_NO_RESPONSE2");
		m_nError = ERR_ROTATE_INCORRECT_RESPONSE;
		return m_nError;
    }

	LeaveCriticalSection(&m_CS_CmdExec);	// ** Critical Section Leave **
	m_bCmdExec = false;
	ROTATE_LOG("[Enable] Rotate Motor Enable Success");
    bMotorOn = true;

    Sleep(20);
	return NO_ERR;
}

/**
@brief   Rotate Motor Disable Function
@author  Labeler
@date    2016-11-25
@return  Disable ���� �� Error �߻� ErrorCode Return
         �̹߻� �Ұ�� NO_ERR Return
*/
int Rotate::Disable()
{
    int nError = NO_ERR;

    // Rotate Motor�� Enable ������ ���� ������ ���� ��쿡��, Disable ����
	if((true == bMotorOn) || NO_ERR != GetError());
    {
        char ReadBuff[1024] = {0, };
		char strMsg[1024] = {0,};
        AnsiString strReceiveData = "";

        // Enable �۾� �Ŀ���, �ݵ�� Origin ���� �ʿ�
        bOrigin = false;

        Sleep(20);
        //nError = IsCmdExec();
         m_bCmdExec = true;
		 EnterCriticalSection(&m_CS_CmdExec);

        if(NO_ERR == nError)
        {


            if(NULL == m_pCommPort)
            {
                nError = ERR_ROTATE_NOT_CONNECTED;
                ROTATE_LOG("[Disable] ERR_ROTATE_NOT_CONNECTED");
            }
            else
            {
                sprintf(strMsg, "%s%c%c", ROTATE_DISABLE, CR, LF);
                PurgeComm_s();
                if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
                {
                    nError = ERR_ROTATE_WRITE;
                    ROTATE_LOG("[Disable] ERR_ROTATE_WRITE");
                }
                else
                {
                    ROTATE_LOG("[Disable] Send Data");
                }
            }
        }

        if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
            if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
                nError = ERR_ROTATE_NO_RESPONSE;
                ROTATE_LOG("[Disable] Send ERR_ROTATE_NO_RESPONSE1");
            }
        }

        if(NO_ERR == nError)
        {
            strReceiveData = ReadBuff;
            if(0 == strReceiveData.Pos(strMsg))
            {
                nError = ERR_ROTATE_INCORRECT_RESPONSE;
                ROTATE_LOG("[Disable] Send ERR_ROTATE_INCORRECT_RESPONSE");
            }
            else
            {
                ROTATE_LOG("[Disable] Rotate Motor Disable Success");
            }
        }
        Sleep(20);
		LeaveCriticalSection(&m_CS_CmdExec);
		m_bCmdExec = false;
        //bCmdExec = false;
        bMotorOn = false;
    }

    if(NO_ERR != nError)
    {
        m_nError = nError;
    }
    else
    {
        if(NO_ERR != GetError())
        {
            m_nError = NO_ERR;
        }
    }

    return nError;
}

/**
@brief   Rotate Motor�� ���� ��ġ�� �������� ����//
@author  Labeler//
@date    2016-11-28//
@return  ������ ��� NO_ERR, �������� ��� �����ڵ� ��ȯ//
*/
int Rotate::SetOrigin()
{
    int nError = NO_ERR;
	char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

	Sleep(100);
	ROTATE_LOG("[SetOrigin] Start");
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);
    //nError = IsCmdExec();


    // ���� Position Read
	if(NO_ERR == nError)
    {
		//nError = ReadPosition();     // ������ ��� Read *****
		if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
			sprintf(strMsg, "%s%d%c%c", ROTATE_SETORIGIN, position_cts, CR, LF);
			//nError = IsCmdExec();
        }
	}

    if(NO_ERR == nError)
    {
        //bCmdExec = true;
		if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_NOT_CONNECTED");
        }
    }

    if(NO_ERR == nError)
	{
		ROTATE_LOG("[SetOrigin] send: %s", strMsg);
        PurgeComm_s();
		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
        {
            nError = ERR_ROTATE_WRITE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_WRITE");
        }
        else
        {
            ROTATE_LOG("[SetOrigin] Send Data");
        }
    }

    if(NO_ERR == nError)
    {
        Sleep(COMM_WAIT_TIME);
		if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
		{
            nError = ERR_ROTATE_NO_RESPONSE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_NO_RESPONSE");
        }
    }
	ROTATE_LOG("[SetOrigin] recv: %s", ReadBuff);
    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strMsg))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
            ROTATE_LOG("[SetOrigin] ERR_ROTATE_INCORRECT_RESPONSE");
        }
        else
        {
            ROTATE_LOG("[SetOrigin] Origin Setting Success");
        }
    }

	Sleep(20);

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   Rotate Motor�� �������� �̵�//
@author  Labeler//
@date    2016-11-28//
@return  ������ ��� NO_ERR, �������� ��� �����ڵ� ��ȯ//
*/
int Rotate::Zeroing()
{
    int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == nError)
    {
        //bCmdExec = true;
        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_NOT_CONNECTED");
        }
    }

    if(NO_ERR == nError)
    {
        ROTATE_LOG("[Zeroing] Start");

        sprintf(strMsg, "%s%c%c", ROTATE_ZEROING, CR, LF);
		PurgeComm_s();
        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
            nError = ERR_ROTATE_WRITE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_WRITE");
        }
        else
        {
            ROTATE_LOG("[Zeroing] Send Data");
        }
    }

    if(NO_ERR == nError)
    {
        Sleep(COMM_WAIT_TIME);
        if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
        {
            nError = ERR_ROTATE_NO_RESPONSE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_NO_RESPONSE");
        }
    }

    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strMsg))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
            ROTATE_LOG("[Zeroing] ERR_ROTATE_INCORRECT_RESPONSE");
        }
        else
        {
            ROTATE_LOG("[Zeroing] Zeroing Success");
        }
    }
    //Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;

    if(NO_ERR == nError)
    {
        bOrigin = true;
    }
    else
    {
        bOrigin = false;
        m_nError = nError;
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::GoHome()
{
    int nError = MoveAbsolute(0);

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
//int Rotate::IsCmdExec()
//{
//    int nError = NO_ERR;
//    DWORD dwSTTime;
//    DWORD dwCheckTime;
//
//	if(true == bCmdExec)
//	{
//        bReqPause = true;
//        dwSTTime = timeGetTime();
//        dwCheckTime = (DWORD)OHTMainForm->GetTransControl()->m_pParamSet->TransDefine.nRotate_Exec_Timeout;
//
//		while(true == bCmdExec)
//        {
//			if(OHTUTILITY::PreventOverflowGetTimeMs(dwSTTime) > dwCheckTime)
//			{
//				nError = ERR_ROTATE_EXEC_TIMEOVER;
//                bCmdExec = false;
//                break;
//            }
//            Sleep(10);
//        }
//    }
//
//    bReqPause = false;
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//
//    return nError;
//}

// SHCHO 2019.02
// Rotate ���� ���� ���� ���� �Լ�
void Rotate::ExecuteStep()
{
	static const int nReqCmdCnt = 2; //ArrReqCmd Ŀ�ǵ� ����. �Ʒ� �迭 ������ ���߱�
	static char *ArrReqCmd[nReqCmdCnt] =
	{
		READ_POS_COMMAND,	// ���� ��ġ ��û.  Px.1=50000  ���·� ����. cts ����
		//READ_VEL_COMMAND,   // ���� �ӵ� ��û.  Sn.1=200 	���·� ����.  ����/K37 �Ķ���� ������
		//READ_TORQUE_COMMAND,    // ���� ��ũ ��û.  lx.1=15    ���·� ����.
		READ_STATUS_COMMAND	  // ���� ���� ��û. Ux.1=Int ���·� ����. Int ���� ���� ����
	};

	char szBuf[1024];
	memset(szBuf, 0, sizeof(szBuf));

	// ���� ���� ���� ������ ���� ����
	bool bDoStep = ((m_bCmdExec == false) && (m_nError == NO_ERR) && (m_stepRotate.Step == 0));

	static DTimer idleTimer;
	// 20ms �����ٰ� ��������. �ʹ� ���� ���ŵǼ� �ٸ� �����尡 Critical Section ���� ���ϴ� �� �����뵵
	if (idleTimer.Enable(bDoStep, 20))
	{
		m_bCmdExec = true;
		idleTimer.Init();
		m_stepRotate.Step = 100;
	}

	// SHCHO 2019.02
	// Rotate ���� ���� �񵿱� ���� * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Step 100~102: Rotate ������ ��ġ, ��ũ ���¸� Request �ϰ� ���� �ð� ���� �� ����� Recv�Ͽ� ������� �����Ѵ�
	//  ���� : step �����Ҷ� Ciritcal Section ����������, Step=0 ����� �κп��� �ݵ�� Leave ����� ��.
	// ������, ���� ������ �д°� �� ���̶� Critical section ��
	//
	switch(m_stepRotate.Step)
	{
		case 100:		// Purge ������� ���� Ŭ����
		{
			m_bCmdExec = true;
			EnterCriticalSection(&m_CS_CmdExec);	// Criitical Section Start ***
			PurgeComm_s();     	// purge
			m_stepRotate.Timer.Init();
			m_stepRotate.Step++;
			break;
		}
		case 101:		// Request Ŀ�ǵ� �ϳ��� Rotate�� ����
		{
			if(!m_stepRotate.Timer.Enable(true, 10))	// non-block delay 10ms
				break;

			// ��ġ, ��ũ, ���� �� �ѹ��� ��û
			sprintf(szBuf, "%s%c%c", ArrReqCmd[m_nIdxCurCmd], CR, LF);
			m_nIdxCurCmd++;
			m_nIdxCurCmd %= nReqCmdCnt; // �������� ���� Ŀ�ǵ�.

			// Send Command [PC ---> Rotate]
			int nLen = strlen(szBuf);
			if(m_pCommPort->WriteCommBlock(szBuf, nLen) != nLen)
			{
				// send ���� ���
				if(m_stepRotate.RetryCnt > 10)
				{
					m_stepRotate.RetryCnt = 0;
					m_nError = ERR_ROTATE_WRITE;
					ROTATE_LOG("Request Failed. Retry Count Over");

					m_stepRotate.Step = 0;
					LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
					m_bCmdExec = false;
					break;
				}

				m_stepRotate.RetryCnt++;
				ROTATE_LOG("Request Failed, Retry: %d", m_stepRotate.RetryCnt);

				m_stepRotate.Step = 0;
				LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
				m_bCmdExec = false;
				break;
			}

			m_stepRotate.Timer.Init();
			m_stepRotate.Step++;
			break;
		}
		case 102:		// Rotate���� ����� ����
		{
			if(!m_stepRotate.Timer.Enable(true, 50))	// non-block wait 50ms
				break;

			// Raw Data Recv [Rotate --> PC]
			m_pCommPort->ReadUntilCount(szBuf, '\n', 2);

			// Parse Data ********
			if( ParseCmd(szBuf, strlen(szBuf)) == false)
			{
				// �Ľ� ���� ���
				if(m_nRecvFailCnt > 10)	//10�� ������ ���� �ڵ� ���� ����
				{
					m_nRecvFailCnt = 0;
					m_nError = ERR_ROTATE_INCORRECT_RESPONSE;
					ROTATE_LOG("ERR_ROTATE_INCORRECT_RESPONSE");
					m_stepRotate.Step = 0;
					LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
					m_bCmdExec = false;
					break;

				}
				m_nRecvFailCnt++;
				m_stepRotate.Step = 0;
				LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
				m_bCmdExec = false;
				break;
			}

			m_nRecvFailCnt = 0;
			m_stepRotate.Step++;
			break;
		}
		case 103:		// ������  ���� ������
		{
			m_stepRotate.Timer.Init();
			m_stepRotate.Step = 0;
			LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section End ****
			m_bCmdExec = false;
			break;
		}
	}
}

// SHCHO 2019.02
// Rotate ���� ���� ������ �Ľ�
// ������ ������ �Ľ̺κп��� AnsiString ���̴� �κ� ��� ����
// ��ġ, �ӵ�, ��ũ, ���ͻ��¸� �Ľ��Ҽ�����
bool Rotate::ParseCmd(char *buf, int nSize)
{
	int nVal = 0;

	if(GetThisValue(buf, "Px.1=", &nVal))  // ��ġ
	{
		position_cts = nVal;
		//position_degree = position_cts / 1421.1167;
		position_degree = position_cts / ROTATE_PULSE_PER_DEGREE;

		ROTATE_LOG("[Recv] Position cts=%d, deg=%d", nVal, position_degree);
	}
	else if(GetThisValue(buf, "Ix.1=", &nVal)) // ��ũ
	{
		torque = nVal;
		ROTATE_LOG("[Recv] Torque=%d", nVal);
	}
	else if(GetThisValue(buf, "Ux.1=", &nVal)) // ����
	{
		SetStatusCmd(nVal);	//���´� ������ ��־ ���� ��
		ROTATE_LOG("[Recv] Status=%d", nVal);
	}
	else if(GetThisValue(buf, "Sx.1=", &nVal)) //�ӵ�
	{
		Velocity_ctsPsec = nVal;
		ROTATE_LOG("[Recv] Velocity=%d", nVal);
	}
	else
	{
		// unkown command
		//ROTATE_LOG("Recv Unkown Cmd: %s", buf == NULL ? "[NULL]" : buf);
		//return false;
    }

	return true;
}

// Ux.1 : ���� ���� Recv ���� �����Ϳ� ���� ������� ����
void Rotate::SetStatusCmd(int nRcvData)
{
	switch(nRcvData)
	{
		case 0 : //���� ������(running)
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = true;
		break;

		case 1 : //����ī���� overload
			m_nError = ERR_ROTATE_COUNTOVERLOAD;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 2 : //���ӵ�/ȸ������
			m_nError = ERR_ROTATE_OVERSPEED;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 4 : //������
			m_nError = ERR_ROTATE_OVERLOAD;
			bMotorOn = false;
			bRunning = false;
			bInit = false;
			bOrigin = false;
		break;

		case 8 : //����/��ġ�Ϸ�(inposition)
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = false;
		break;

		case 16 : //���� free
			m_nError = NO_ERR;
			bMotorOn = false;
			bRunning = false;
		break;

		case 32 : //������ running
			m_nError = NO_ERR;
			bMotorOn = true;
			bRunning = true;
		break;

		case 40 : //������ �Ϸ�
			m_nError = NO_ERR;
			bMotorOn = false;
			bRunning = false;
		break;

		default:
		break;
	}
}
/**
@brief      Motor�� ���� ��ġ ��û �Լ�
@author     Labeler
@date       2016-12-03
@return     �ٸ� ��� ���� ��   : NO_ERR
            ���� ��û �Ϸ�      : NO_ERR
            ���� ��û ����      : �ش� ErrorCode ��ȯ
*/
//int Rotate::ReqPosition()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqPosition] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//        sprintf(strMsg, "%s%c%c", READ_POS_COMMAND, CR, LF);
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqPosition] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
////            ROTATE_LOG("[ReqPosition] Send Data");
//			bReqPosStatus = true;
//			dwStartReqPosTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor�� ���� ��ġ ���� ���� �Լ�
//@author     Labeler
//@date       2016-12-03
//@return     �ٸ� ��� ���� ��   : NO_ERR
//            ��ġ ��û ��        : NO_ERR
//            ���� ���� �Ϸ�      : NO_ERR
//            ���� ���� ����      : �ش� ErrorCode ��ȯ
//*/
//int Rotate::ReadPosition()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqPosStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqPosStatus))
//	{
//		bReqPosStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqPosStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqPosStatus) && ((true == bReqMotorStatus) || (true == bReqVelStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqMotorStatus) || (true == bReqVelStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadPosition] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqPosTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqPosStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadPosition] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nPxPos = 0;
//        int nEndPos = 0;
//		int nRcvData = 0;
//       	AnsiString strReceiveData = "";
//
//		strReceiveData = ReadBuff;
//		nPxPos = strReceiveData.Pos("Px.1=");
//
//		if(0 == nPxPos)
//		{
//            if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//            {
//                nCntIncorrectResponse++;
//            }
//            else
//            {
//    			nError = ERR_ROTATE_INCORRECT_RESPONSE;
//    			ROTATE_LOG("[ReadPosition] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//            nCntIncorrectResponse = 0;
//			position_cts = StrToIntDef(strReceiveData.SubString(nPxPos+5, strReceiveData.Length()-nPxPos-6), position_cts);
////            position_degree = position_cts / 1421.1167;
//			position_degree = position_cts / ROTATE_PULSE_PER_DEGREE;
//		}
//		bReqPosStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}
//
///**
//@brief      Motor�� ���� �ӵ� ��û �Լ�
//@author     Labeler
//@date       2016-12-03
//@return     �ٸ� ��� ���� ��   : NO_ERR
//            ���� ��û �Ϸ�      : NO_ERR
//            ���� ��û ����      : �ش� ErrorCode ��ȯ
//*/
//int Rotate::ReqVelocity()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqVelocity] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//        sprintf(strMsg, "%s%c%c", READ_VEL_COMMAND, CR, LF);
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqVelocity] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
////            ROTATE_LOG("[ReqVelocity] Send Data");
//			bReqVelStatus = true;
//			dwStartReqVelTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor�� ���� �ӵ� ���� ���� �Լ�
//@author     Labeler
//@date       2016-12-03
//@return     �ٸ� ��� ���� ��   : NO_ERR
//            �ӵ� ��û ��        : NO_ERR
//            ���� ���� �Ϸ�      : NO_ERR
//            ���� ���� ����      : �ش� ErrorCode ��ȯ
//*/
//int Rotate::ReadVelocity()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqVelStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqVelStatus))
//	{
//		bReqVelStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqVelStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqVelStatus) && ((true == bReqMotorStatus) || (true == bReqPosStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqMotorStatus) || (true == bReqPosStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadVelocity] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqVelTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqVelStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadVelocity] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nSxPos = 0;
//		int nRcvData = 0;
//       	AnsiString strReceiveData = "";
//
//		strReceiveData = ReadBuff;
//		nSxPos = strReceiveData.Pos("Sx.1=");
//		if(0 == nSxPos)
//		{
//            if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//            {
//                nCntIncorrectResponse++;
//            }
//            else
//            {
//			    nError = ERR_ROTATE_INCORRECT_RESPONSE;
//    			ROTATE_LOG("[ReadVelocity] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//			nCntIncorrectResponse = 0;
//			Velocity_ctsPsec = StrToIntDef(strReceiveData.SubString(nSxPos+5, strReceiveData.Length()-nSxPos-6), (int)Velocity_ctsPsec);
//		}
//		bReqVelStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}
//
///**
//@brief      Motor�� ���� ���� ��û �Լ�
//@author     Labeler
//@date       2016-12-03
//@return     �ٸ� ��� ���� ��   : NO_ERR
//            ���� ��û �Ϸ�      : NO_ERR
//            ���� ��û ����      : �ش� ErrorCode ��ȯ
//*/
//int Rotate::ReqMotorStatus()
//{
//	int nError = NO_ERR;
//
//	if((true == bCmdExec) || (true == bReqPause))
//	{
//		return nError;
//	}
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReqMotorStatus] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//	else
//	{
//       	char strMsg[1024] = {0,};
//		bCmdExec = true;
//
//		sprintf(strMsg, "%s%c%c", READ_STATUS_COMMAND, CR, LF); //#define  READ_STATUS_COMMAND  "?99"
//		PurgeComm_s();
//        Sleep(10);
//
//        if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
//		{
//			ROTATE_LOG("[ReqMotorStatus] ERR_ROTATE_WRITE");
//			bCmdExec = false;
//            m_nError = ERR_ROTATE_WRITE;
//			return ERR_ROTATE_WRITE;
//		}
//        else
//        {
//			bReqMotorStatus = true;
//			dwStartReqStatusTime = timeGetTime();
//        }
//	}
//	return nError;
//}
//
///**
//@brief      Motor�� ���� ���� ���� ���� �Լ�
//@author     Labeler
//@date       2016-12-03
//@return     �ٸ� ��� ���� ��   : NO_ERR
//            ���� ��û ��        : NO_ERR
//            ���� ���� �Ϸ�      : NO_ERR
//            ���� ���� ����      : �ش� ErrorCode ��ȯ
//*/
//int Rotate::ReadMotorStatus()
//{
//	int nError = NO_ERR;
//	char ReadBuff[1024] = {0, };
//
//	if((true != bCmdExec) && (true != bReqMotorStatus))
//	{
//		return nError;
//	}
//	else if((true != bCmdExec) && (true == bReqMotorStatus))
//	{
//		bReqVelStatus = false;
//		return nError;
//	}
//    else if((true == bCmdExec) && (true != bReqMotorStatus))
//    {
//        return nError;
//    }
//    else if((true == bReqMotorStatus) && ((true == bReqVelStatus) || (true == bReqPosStatus)))
//    {
//        bReqPosStatus = false;
//        bReqVelStatus = false;
//        bReqMotorStatus = false;
//        bCmdExec = false;
//        return nError;
//    }
//    else if((true == bReqPosStatus) || (true == bReqVelStatus))
//    {
//        return nError;
//    }
//
//	if(NULL == m_pCommPort)
//	{
//		ROTATE_LOG("[ReadMotorStatus1] ERR_ROTATE_NOT_CONNECTED");
//        m_nError = ERR_ROTATE_NOT_CONNECTED;
//		return ERR_ROTATE_NOT_CONNECTED;
//	}
//
//	if(0 == m_pCommPort->ReadUntilCount(ReadBuff, '\n', 2))
//	{
//		if(OHTUTILITY::PreventOverflowGetTimeMs(dwStartReqStatusTime) > COMM_RECIVIE_TIME_LIMIT)
//		{
//			bReqVelStatus = false;
//			bCmdExec = false;
//			nError = ERR_ROTATE_NO_RESPONSE;
//			ROTATE_LOG("[ReadMotorStatus1] ERR_ROTATE_NO_RESPONSE");
//		}
//	}
//	else
//	{
//		int nUxPos = 0;
//		int nRcvData = 0;
//		AnsiString strReceiveData = "";
//		strReceiveData = ReadBuff;
//		ROTATE_LOG("[ReceiveData] strReceiveData : %s, strReceiveData.Length : %d ", strReceiveData, strReceiveData.Length()); //Rotate Motor Status �� �߰� 2018.10.15 JRESPECT.IM
//		nUxPos = strReceiveData.Pos("Ux.1=");
//		ROTATE_LOG("[ReceiveData] nUxPos : %d", nUxPos);
//		//���� Rotate Status Data�� ���̰� �ٸ� ��� ���� ���� ����
////		if(strReceiveData != 10)
////		{
////		  nError = ERR_ROTATE_INCORRECT_RESPONSE;
////		  ROTATE_LOG("[ReadMotorStatus] MotorStatus Length NG");
////		}
//		if(0 >= nUxPos)
//		{
//			if(INCORRECT_RESPONSE_LIMIT_COUNT > nCntIncorrectResponse)
//			{
//				nCntIncorrectResponse++;
//			}
//			else
//			{
//                nCntIncorrectResponse = 0;
//			    nError = ERR_ROTATE_INCORRECT_RESPONSE;
//				ROTATE_LOG("[ReadMotorStatus] ERR_ROTATE_INCORRECT_RESPONSE1");
//            }
//		}
//		else
//		{
//			nRcvData = StrToIntDef(strReceiveData.SubString(nUxPos+5, strReceiveData.Length()-nUxPos-6), -1);
//			if(-1 == nRcvData)
//			{
//				if (nAbnormalCheckMotorStatus >= 2) //Rotate Motor Status ���� �������� ��� Retry 3ȸ �߰� 2018.07.10 JRESPECT.IM
//				{
//					nAbnormalCheckMotorStatus = 0;
//					ROTATE_LOG("[ReadMotorStatus] ERR_ROTATE_INCORRECT_RESPONSE2");
//					nError = ERR_ROTATE_INCORRECT_RESPONSE;
//				}
//				else
//				{
//					nAbnormalCheckMotorStatus++;
//					ROTATE_LOG("[ReadMotorStatus] Abnormal MotorStatus");
//                }
//			}
//			else
//			{
//				ROTATE_LOG("[Read MotorStatus Data] %d", nRcvData);  //Rotate Motor Status �� �߰� 2018.10.15 JRESPECT.IM
//				nAbnormalCheckMotorStatus = 0;
//				switch(nRcvData)
//				{
//					case 0 : //���� ������(running)
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = true;
//					break;
//
//					case 1 : //����ī���� overload
//						nError = ERR_ROTATE_COUNTOVERLOAD;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 2 : //���ӵ�/ȸ������
//						nError = ERR_ROTATE_OVERSPEED;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 4 : //������
//						nError = ERR_ROTATE_OVERLOAD;
//						bMotorOn = false;
//						bRunning = false;
//						bInit = false;
//                        bOrigin = false;
//					break;
//
//					case 8 : //����/��ġ�Ϸ�(inposition)
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = false;
//					break;
//
//					case 16 : //���� free
//						nError = NO_ERR;
//						bMotorOn = false;
//						bRunning = false;
//					break;
//
//					case 32 : //������ running
//						nError = NO_ERR;
//						bMotorOn = true;
//						bRunning = true;
//					break;
//
//					case 40 : //������ �Ϸ�
//						nError = NO_ERR;
//						bMotorOn = false;
//						bRunning = false;
//					break;
//
//					default:
//					break;
//				}
//			}
//		}
//
//		bReqMotorStatus = false;
//		bCmdExec = false;
//	}
//
//    if(NO_ERR != nError)
//    {
//        m_nError = nError;
//    }
//	return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/

// Stop ����� ���, ���� ��� ��/�� ���� ���� �ٷ� ����
int Rotate::Stop()
{
    int nError = NO_ERR;
    AnsiString strRData;
    int nStopCnt = 0;

    Sleep(20);
    if((true == bReqPosStatus) || (true == bReqVelStatus) || (true == bReqMotorStatus))
    {
        //IsCmdExec();
        ROTATE_LOG("[Stop] IsCmdExec, Error : %d", nError);
        //bCmdExec = true;
	}
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == m_nError)
    {
        ROTATE_LOG("[Stop] Before SendCommand1_1");
        nError = SendCommand1(ROTATE_STOP, strRData);
        nStopCnt++;
        if(NO_ERR == nError)
        {
            ROTATE_LOG("[Stop] Before SendCommand1_2");
            nError = SendCommand1(ROTATE_STOP, strRData);
            nStopCnt++;
        }
    }

    if((NO_ERR != m_nError) && (2 > nStopCnt))
    {
        ROTATE_LOG("[Stop] Before SendCommand1_3");
        nError = SendCommand1(ROTATE_STOP, strRData);
        nStopCnt++;
        if((NO_ERR != m_nError) && (2 > nStopCnt))
        {
            ROTATE_LOG("[Stop] Before SendCommand1_4");
            nError = SendCommand1(ROTATE_STOP, strRData);
        }
    }

	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;
    //if(true == bCmdExec)
    //{
    //    bCmdExec = false;
    //}

    if( (NO_ERR != nError) && (ERR_ROTATE_INCORRECT_RESPONSE != nError) )
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[Stop] Stop Success");
    }
    Sleep(20);
    return nError;
}
//
///**
//@brief   //
//@author  //
//@date    //
//@return  //
//*/
//int Rotate::Move()
//{
//    int nError = NO_ERR;
//    return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Move_Jog_CW()
{
    int nError = NO_ERR;
    AnsiString strRData;

	Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        //bCmdExec = true;

        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
        }

        if(false == bOrigin)
        {
            nError = ERR_ROTATE_NOTORIGIN;
        }
    }

    if(NO_ERR == nError)
    {
        nError = SendCommand1(ROTATE_JOGMOVE_CW, strRData);
    }

    Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;


    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::Move_Jog_CCW()
{
    int nError = NO_ERR;
    AnsiString strRData;

    Sleep(20);
	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);

    if(NO_ERR == nError)
    {
        //bCmdExec = true;

        if(NULL == m_pCommPort)
        {
            nError = ERR_ROTATE_NOT_CONNECTED;
        }

        if(false == bOrigin)
        {
            nError = ERR_ROTATE_NOTORIGIN;
        }
    }

    if(NO_ERR == nError)
    {
        nError = SendCommand1(ROTATE_JOGMOVE_CCW, strRData);
    }

    Sleep(20);
	LeaveCriticalSection(&m_CS_CmdExec);
	m_bCmdExec = false;

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
//int Rotate::MoveIncremental()
//{
//    int nError = NO_ERR;
//    return nError;
//}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveIncremental_CW(int nLoc)
{
    int nError = NO_ERR;
    //AnsiString strRData;
    //AnsiString strSndData;

    //Sleep(20);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        // �̵�
        nError = MoveAbsolute(position_degree+nLoc);
    }

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[MoveIncremental_CW] Move Success");
    }

    return nError;
}

/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveIncremental_CCW(int nLoc)
{
    int nError = NO_ERR;
    //AnsiString strRData;
    //AnsiString strSndData;

    //Sleep(20);
    //nError = IsCmdExec();

    if(NO_ERR == nError)
    {
        // �̵�
        nError = MoveAbsolute(position_degree-nLoc);
    }

    if(NO_ERR != nError)
    {
        bOrigin = false;
        m_nError = nError;
    }
    else
    {
        ROTATE_LOG("[MoveIncremental_CCW] Move Success");
    }

    return nError;
}


/**
@brief   //
@author  //
@date    //
@return  //
*/
int Rotate::MoveAbsolute(int nLoc)
{
	//nError = IsCmdExec();
     //bCmdExec = true;
    if(NULL == m_pCommPort)
    {
		ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NOT_CONNECTED");
        return  ERR_ROTATE_NOT_CONNECTED;
	}
	if(false == bOrigin)
    {
		ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NOTORIGIN");
        return ERR_ROTATE_NOTORIGIN;
    }

	Sleep(20);
	AnsiString strRData;
	AnsiString strSndData;
	int nBeforeLoc = 0;

    // 2016-12-19, Add by Labeler
    // Rotate ���� �߻� ���� ����, 2�� ������ �����ϵ��� �̵� ��� ����
    if(nLoc > GetCurrentPositionDegree())
    {
        nBeforeLoc = (int)((nLoc - GetCurrentPositionDegree()) * 0.85) + GetCurrentPositionDegree();
    }
    else if(nLoc < GetCurrentPositionDegree())
    {
        nBeforeLoc = (int)((GetCurrentPositionDegree() - nLoc) * 0.15) + nLoc;
    }
    else
    {
        nBeforeLoc = nLoc;
    }

	nLoc = nLoc*ROTATE_PULSE_PER_DEGREE;
    nBeforeLoc = nBeforeLoc*ROTATE_PULSE_PER_DEGREE;
	strSndData.sprintf("%s%d,%s%d", ROTATE_MANUAL_LOC, nBeforeLoc, ROTATE_MANUAL_LOC2, nLoc);

	m_bCmdExec = true;
	EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start *********
	int nError = SendCommand2(strSndData, strRData);

    // 2016-12-27, Add by Labeler
    // ��Ȥ, ������ ���� �ʴ� ���, Motor�� Stop ��� �߻� ��, �� ��� �߻� ó�� ��� �߰�
	if(NO_ERR != nError)
    {
        //bCmdExec = false;
		LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section Leave *********
		m_bCmdExec = false;

		Stop();

		m_bCmdExec = true;
		EnterCriticalSection(&m_CS_CmdExec);	// Critical Section Start *********

        Sleep(100);
        PurgeComm_s();
        //bCmdExec = true;
        // 2016-12-12, Add by Labeler
        // ��Ȥ Error �߻� ��� ���� ���� Sleep �߰�
        Sleep(100);
		nError = SendCommand2(strSndData, strRData);

    }
    Sleep(20);

    if((NO_ERR == nError) || (ERR_ROTATE_NO_RESPONSE == nError))
    {
        nError = SendCommand2(ROTATE_MANUAL_MOVE, strRData);

        if(NO_ERR != nError)
        {
            // 2016-12-12, Add by Labeler
            // ��Ȥ Error �߻� ��� ���� ���� Sleep �߰�
            Sleep(100);
            nError = SendCommand2(ROTATE_MANUAL_MOVE, strRData);
        }
    }
//    Sleep(100);

    PurgeComm_s();
	LeaveCriticalSection(&m_CS_CmdExec);	// Critical Section Leave *********
	m_bCmdExec = false;

    if((NO_ERR != nError) && (ERR_ROTATE_NO_RESPONSE != nError))
    {
        bOrigin = false;
        m_nError = nError;
        ROTATE_LOG("Error[%d] MOVE ABSOLUTE", nError);
    }
    else
    {
        if(ERR_ROTATE_NO_RESPONSE == nError)
        {
            nError = NO_ERR;
            ROTATE_LOG("[MoveAbsolute] ERR_ROTATE_NO_RESPONSE for Skip");
        }
        else
        {
            ROTATE_LOG("[MoveAbsolute] Move Success");
        }
    }

    return nError;
}

/**
@brief   Motor�� ���� Command�� ������ �Լ�//
@author  Labeler//
@date    2016-11-28//
@return  ������ ��� NO_ERR, ���� �߻��� �����ڵ� ��ȯ//
*/
// �� �Լ��� ȣ���ϴ� �ʿ��� �̹� m_CS_CmdExec ũ��Ƽ�ü������� ��ȣ�� ���¿��� ȣ���Ѵ�
int Rotate::SendCommand1(AnsiString strSendCommand, AnsiString &strRcvData)
{
    int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";

    if(NULL == m_pCommPort)
    {
        nError = ERR_ROTATE_NOT_CONNECTED;
    }

    try
    {
        if(NO_ERR == nError)
        {
            sprintf(strMsg, "%s%c%c", strSendCommand, CR, LF);
            PurgeComm_s();
            if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
            {
                nError = ERR_ROTATE_WRITE;
            }
        }
        if(NO_ERR == nError)
        {
            Sleep(COMM_WAIT_TIME);
            if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
                nError = ERR_ROTATE_NO_RESPONSE;
            }
        }
    }
    __finally
    {

    }

    if(NO_ERR == nError)
    {
        strReceiveData = ReadBuff;
        if(0 == strReceiveData.Pos(strSendCommand))
        {
            nError = ERR_ROTATE_INCORRECT_RESPONSE;
        }
        if(NO_ERR == nError)
        {
            strRcvData = strReceiveData;
        }
    }

    if((strSendCommand == ROTATE_STOP) && (ERR_ROTATE_NO_RESPONSE == nError))
    {
        return NO_ERR;
    }
    else if(NO_ERR != nError)
    {
        ROTATE_LOG("chk1[E%d] Snd=%s, Rcv=%s", nError, strSendCommand, strReceiveData);
    }

    return nError;
}

/**
@brief   Motor�� ABS ���� ���� Command�� ������ �Լ�//
@author  Labeler//
@date    2016-11-28//
@return  ������ ��� NO_ERR, ���� �߻��� �����ڵ� ��ȯ//
*/
// �ּ� 250ms �̻� Deley�� ����� block �Լ�
// �� �Լ��� ȣ���ϴ� �ʿ��� �̹� m_CS_CmdExec ũ��Ƽ�ü������� ��ȣ�� ���¿��� ȣ���Ѵ�
int Rotate::SendCommand2(AnsiString strSendCommand, AnsiString &strRcvData)
{
	int nError = NO_ERR;
    char ReadBuff[1024] = {0, };
   	char strMsg[1024] = {0,};
    AnsiString strReceiveData = "";
    AnsiString strData1, strData2;
    DWORD dwStartTime;

    if(NULL == m_pCommPort)
    {
        nError = ERR_ROTATE_NOT_CONNECTED;
    }

    try
    {
        if(NO_ERR == nError)
        {
			PurgeComm_s();
            Sleep(50);

            sprintf(strMsg, "%s%c%c", strSendCommand, CR, LF);
			if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)))
            {
                nError = ERR_ROTATE_WRITE;
                ROTATE_LOG("[SendCommand2] ERR_ROTATE_WRITE");
            }
        }
        if(NO_ERR == nError)
        {
            Sleep(200/*COMM_WAIT_TIME*/);
			if(0 == m_pCommPort->ReadUntil(ReadBuff, '\n'))
            {
				strReceiveData = ReadBuff;
				nError = ERR_ROTATE_NO_RESPONSE;
                ROTATE_LOG("[SendCommand2] ERR_ROTATE_NO_RESPONSE1");
            }
        }

        if(NO_ERR == nError)
        {
            strReceiveData = ReadBuff;
			if(0 == strReceiveData.Pos(strMsg))
            {
                nError = ERR_ROTATE_INCORRECT_RESPONSE;
				ROTATE_LOG("[SendCommand2] ERR_ROTATE_INCORRECT_RESPONSE2");
            }
        }
    }
    __finally
    {

    }

    if(NO_ERR != nError)
    {
        strData1 = strMsg;
        strData2 = strRcvData;
        if(strData1.Trim() == strData2.Trim())
        {
            nError = NO_ERR;
        }
    }

    if(NO_ERR != nError)
    {
        ROTATE_LOG("chk2[E%d] Snd=%s, Rcv=%s", nError, strSendCommand, strReceiveData);
    }

    return nError;
}

int Rotate::GetCurrentPosition()
{
    return position_cts;
}

int Rotate::GetCurrentPositionDegree()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->Position.bUse)
        {
            return pMaskOfAxis->Position.Value;
        }
    }

    return position_degree;
}

double Rotate::GetCurrentVelocity()
{
    return Velocity_ctsPsec;
}

int Rotate::GetError()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

	if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->ErrorCode.bUse)
        {
            return pMaskOfAxis->ErrorCode.Value;
        }
    }

    return m_nError;
}

bool Rotate::IsAMPEnable()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

	if(NULL != pMaskOfAxis)
    {
		if(true == pMaskOfAxis->AmpEnable.bUse)
        {
            return pMaskOfAxis->AmpEnable.Value;
        }
	}

	return bMotorOn;
}
//bool Rotate::IsRotate0Sensor()
//{
//	return m_pHWCommon->m_Input.Rotate_0_Sensor;
//}
//bool Rotate::IsRotate90Sensor()
//{
//	 return m_pHWCommon->m_Input.Rotate_90_Sensor;
//}
//bool Rotate::IsRotate180Sensor()
//{
//	 return m_pHWCommon->m_Input.Rotate_180_Sensor;
//}
//bool Rotate::IsRotate270Sensor()
//{
//	return m_pHWCommon->m_Input.Rotate_270_Sensor;
//}

bool Rotate::IsRunning()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
        if(true == pMaskOfAxis->Running.bUse)
        {
            return pMaskOfAxis->Running.Value;
		}
    }

    return bRunning;
}

bool Rotate::IsOrigin()
{
	MASK_AXIS	*pMaskOfAxis;

	pMaskOfAxis = m_pHWCommon->GetMask()->getMaskOfAxis(ROTATE_AXIS_NUM);

    if(NULL != pMaskOfAxis)
    {
		if(true == pMaskOfAxis->Origin.bUse)
        {
			return pMaskOfAxis->Origin.Value;
        }
	}

	return bOrigin;
}

int Rotate::ResetRotate()
{
// Motor Diable �� Enable �� ���, Error Clear ��
	Disable();
    Sleep(20);
    Enable();
}


// RS232 Serial ��� Thread Safe �Լ� **************
//


void Rotate::PurgeComm_s()
{
	PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
}
