/**
@file    OHTDetect.cpp
@version OHT 7.0
@brief   OHTDetect Class CPP File
*/

#include "OHTDetect.h"
#include "ErrorCode.h"
#include "LogHandler.h"	// Log �ۼ� ����
#include "DiagManager.h"
#include "Utility.h"
#include <windows.h>
#include "OHTMain.h"


#define DETECT_LOG(...)    WRITE_LOG_UNIT(pLogDetect, __VA_ARGS__)
/**
@brief   OHTDetect Class ������
@author  ���¿�
@date    2013.02.22
@param   pHWCommon : HWCommon Class�� ������
*/
OHTDetect::OHTDetect(HWCommon *pHWCommon):VirtualDetect()
{
	m_nComPortNum = UBG_ComPort;
	m_nBaudRate = UBG_Baudrate;
	m_pCommPort = NULL;
	bConnect = false;
	m_pHWCommon	= pHWCommon;
	m_nStartEnableTime = timeGetTime();
	pLogDetect = getLogUnit("D:\\log\\DETECT.log", 1024*10);

	CheckUBGConnect(true);

}

/**
@brief   OHTDetect Class �Ҹ���
@author  ���¿�
@date    2013.02.22
*/
OHTDetect::~OHTDetect()
{
    CheckUBGConnect(false);
}

/**
@brief   OHT Detect ������ ������ ON
@author  ���¿�
@date    2013.02.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int OHTDetect::Enable()
{
#if(UBG_SENSOR_ENABLE == ON)
	return m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
#else
	return NO_ERR;
#endif
}

/**
@brief	 OHT Detect ������ ������ OFF
@author  ���¿�
@date	 2013.02.22
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int	OHTDetect::Disable()
{
#if(UBG_SENSOR_ENABLE == ON)
	return m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
#else
	return NO_ERR;
#endif
}
int OHTDetect::OpenCom()
{
	int nError = 0;
	if(m_pCommPort==NULL)
	{
		//m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
		//m_nBaudRate = 38400;
		m_pCommPort = new Comm_Port(m_nComPortNum, 38400, 8 ,NOPARITY, 1);
		DETECT_LOG("OpenCom() %d", m_pCommPort);
		if(m_pCommPort==NULL)
		{
            bConnect = false;
			nError=ERR_UBG_OPEN;
		}
		else
		{
			bConnect = true;
		}
	}
	else
	{
        bConnect = false;
		CloseCom();
	}
	return nError;
}
int OHTDetect::CloseCom()
{
	int nError = 0;
	if(m_pCommPort!=NULL)
	{
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
	bConnect = false;
	return nError;
}

int OHTDetect::Send_UBGData(unsigned char* ackData, int Length)
	{
	unsigned char strUBG[40] = {0,};
	int Error=0;

	if(m_pCommPort==NULL)
	{
		DETECT_LOG("Send_UBGData() ERR_UBG_OPEN");
		return ERR_UBG_OPEN;
	}
	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		DETECT_LOG("Send_UBGData() memcpy");
		memcpy(strUBG, ackData, Length);
	}
	if(Length != m_pCommPort->WriteCommBlock(strUBG, Length))
	{
		DETECT_LOG("Send_UBGData() ERR_UBG_WRITE");
		Error=ERR_UBG_WRITE;
	}
	return Error;
}

int OHTDetect::Recieve_UBGData(unsigned char* pReadBuff)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[1024] = {0,};

	if(m_pCommPort==NULL)
	{
		return ERR_UBG_OPEN;
	}


	dwLen = m_pCommPort->ReadUntilCountOHTDetect(szReadBuff,UBG_Final_Flag,UBG_Target);
	if(dwLen>0)
	{
		memcpy(pReadBuff, szReadBuff, 512);
		return dwLen;
	}
	else
	{
		DETECT_LOG("Recieve_UBGData() ERR_UBG_WRITE");
		Error = ERR_UBG_WRITE;
	}
	return Error;
}

/**
@brief	 OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/

bool	OHTDetect::GetSensorStatus()					// ���¸� �޾ƿ��� �Լ�
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;

    bool breturn = false;
	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return true;
	}

	switch(nValue)
	{
	case 7: //111
	case 5: //101
         breturn = true;
		break;
	}

	return breturn;
#else

	return false;
#endif

}
/**
@brief	 OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/

bool	OHTDetect::GetCheckStatus()					// ���¸� �޾ƿ��� �Լ�
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;

    bool breturn = false;
	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return true;
	}

	switch(nValue)
	{

	case 2:	//010
	case 3: //011
	case 6: //110
	case 4:	//100
	case 7: //111
	case 5: //101
         breturn = true;
		break;
	}

	return breturn;
#else

	return false;
#endif

}
int	OHTDetect::GetStatusLevel()					// ���¸� �޾ƿ��� �Լ�
{

	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;
	if(bFail)
	{
		return 5;
	}

//	switch(nValue)
//	{
//
//	case 2:	//010
//	case 3: //011
//	case 6: //110
//	case 4:	//100
//	case 7: //111
//	case 5: //101
//         breturn = true;
//		break;
//	}

	return nValue;


}
/**
@brief	 OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date	 2013.02.22
@param	 nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/

DETECT_STATUS	OHTDetect::GetStatus()					// ���¸� �޾ƿ��� �Լ�
{
#if(UBG_SENSOR_ENABLE == ON)
	bool	bFail	= 	m_pHWCommon->m_Input.OHTDETECT_Fail;
	int 	nValue	= 	1 * m_pHWCommon->m_Input.OHTDETECT_Long
					+	2 * m_pHWCommon->m_Input.OHTDETECT_Middle
					+	4 * m_pHWCommon->m_Input.OHTDETECT_Short;
	DETECT_STATUS	Status;

	if(bFail)
	{
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		return DETECT_UNIT_FAIL;
	}
#if(OHT_NAME == OHT_NAME_STD_V81S)
	if(OHTMainForm->CheckBox_ignoreOHT->Checked == true)
	{
		return DETECT_NONE;
	}
#endif
	switch(nValue)
	{
	case 0:	//000
		Status = DETECT_NONE;
		break;
	case 1:	//001
	case 3: //011
	case 2:	//010
	case 6: //110
#if(OHT_NAME == OHT_NAME_STD_V81S)
	case 5: //101
#else
	case 4:	//100
#endif

	case 7: //111
		Status = DETECT_AND_MOVE;
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		break;
#if(OHT_NAME == OHT_NAME_STD_V81S)
	case 4:	//100
#else
	case 5:	//100
#endif
		Status = DETECT_AND_STOP;
//		DETECT_LOG("OHTDetect:%d [%d:%d:%d:%d] %d"
//			, nValue
//			, m_pHWCommon->m_Input.OHTDETECT_Fail
//			, m_pHWCommon->m_Input.OHTDETECT_Long
//			, m_pHWCommon->m_Input.OHTDETECT_Middle
//			, m_pHWCommon->m_Input.OHTDETECT_Short
//			, Status);
		break;
	}

	return Status;
#else

	return NO_ERR;
#endif

}

/**
@brief   OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
void	OHTDetect::SetArea(DETECT_AREA_TYPE Type) // ������ �����ϴ� �Լ�
{

}

/**
@brief   OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
double	OHTDetect::GetMovableDist()			// �̵� ������ �Ÿ��� �޾ƿ��� �Լ�
{

}

/**
@brief   OHT Detect ������ ������ ��Ʈ�� �ϴ� �Լ�
@author  ���¿�
@date    2013.02.22
@param   nMode(Power On, Off, Reset, Enable)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int OHTDetect::PowerControl(int nMode)
{
	int nError = NO_ERR;

#if(UBG_SENSOR_ENABLE == ON)
	switch (nMode)
	{
	case PWR_OFF:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
		break;
	case PWR_ON:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
		break;
	case PWR_RST:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
		if (nError == NO_ERR)
		{
			Sleep(500);
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
		}
		break;
	case PWR_ENABLE:
		nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA1_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA2_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA3_SEL, OFF);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA4_SEL, OFF);
		break;
	}
#endif

	return nError;
}

/**
@brief   OHT Detect ������ �˶� �߻����� �� ������ �����ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int OHTDetect::ErrorReset()
{
    int nError = NO_ERR;

#if(UBG_SENSOR_ENABLE == ON)
	if (m_pHWCommon->m_Input.OHTDETECT_Fail == ON)
    {
        nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, ON);
        if (nError == NO_ERR)
        {
            Sleep(500);
            nError = m_pHWCommon->SetBit(OUT_OHTDETECT_POWER_RESET, OFF);
        }
    }
#endif
    return nError;
}

/**
@brief   OHT Detect ������ Area�� ��Ʈ ������ ���� �����ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@param   nArea(��Ʈ ������ ���� �� 16������ Area�� ������ �� ����.)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int OHTDetect::AreaSelect(int nArea)
{
    int nError = NO_ERR;
#if(UBG_SENSOR_ENABLE == ON)
	int i = 0, nDetectOut[4] = {0, };

    for (i = 0; i < 4; i++)
    {
        nDetectOut[i] = ((nArea >> i) & 1) ? 0 : 1;
    }

    /*
    nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA1_SEL, nDetectOut[0]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA2_SEL, nDetectOut[1]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA3_SEL, nDetectOut[2]);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OBSTACLE_AREA4_SEL, nDetectOut[3]);
    */

    nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA1_SEL, nDetectOut[0], true);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA2_SEL, nDetectOut[1], true);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA3_SEL, nDetectOut[2], true);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_OHTDETECT_AREA4_SEL, nDetectOut[3], true);
#endif
    return nError;
}

/**
@brief   OHT Detect ������ Level�� ������ �Լ�
@author  LSJ
@date    2012.10.31
@param   bLong : Long �Է� On/Off ����, bMiddle : Middle �Է� On/Off ����, bShort : Short �Է� On/Off ����, bFail : Fail �Է� On/Off ����
@return  OHT Detect ���� Level�� ����
*/
int OHTDetect::GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail)
{
    int nLeveld = 0;

#if(UBG_SENSOR_ENABLE == ON)
	if (bFail == ON)
        nLeveld = -1; // fail
    else if ((bLong == OFF) && (bMiddle == ON) && (bShort == ON))
        nLeveld = 7; // UBG Output1(���� �հŸ� ����)
    else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == ON))
        nLeveld = 6; // UBG Output2(���� �հŸ� ����)
    else if ((bLong == ON) && (bMiddle == OFF) && (bShort == ON))
        nLeveld = 5; // UBG Output3(���� �հŸ� ����)
    else if ((bLong == ON) && (bMiddle == OFF) && (bShort == OFF))
        nLeveld = 4; // UBG Output4(���� �հŸ� ����)
    else if ((bLong == OFF) && (bMiddle == OFF) && (bShort == OFF))
        nLeveld = 3; // UBG Output5(���� �հŸ� ����)
    else if ((bLong == OFF) && (bMiddle == ON) && (bShort == OFF))
        nLeveld = 2; // UBG Output6(���� �հŸ� ����)
    else if ((bLong == ON) && (bMiddle == ON) && (bShort == ON))
        nLeveld = 0; // �̰���
    else
        nLeveld = -1; // Fail ==>��Ȯ���ʿ�(ON ON OFF)
#endif
    return nLeveld;
}

// �� �Լ� ����
int OHTDetect::GetSensorLevel2(bool bLong, bool bMiddle, bool bShort, bool bFail)
{
	// Long Mid Shor ������
	// 000 -> 100 -> 110 -> 010 -> 011 -> 111 -> 101
	//	8  -> 7   -> 6   -> 5    -> 4   -> 3  ->  2  (�� �ϱ�� ����)

	if(bFail)
		return -1;

	if(!bLong && !bMiddle && !bShort)  //000
		return 8;

	if(bLong && !bMiddle && !bShort)  //100
		return 7;

	if(bLong && bMiddle && !bShort)   //110
		return 6;

	if(!bLong && bMiddle && !bShort)    //010
		return 5;

	if(!bLong && bMiddle && bShort)     //011
		return 4;

	if(bLong && bMiddle && bShort)       //111
		return 3;

	if(bLong && !bMiddle && bShort)     //101
		return 2;

	return -1;
}



/**
@brief   OHT Detect ������ � �ܰ谡 ���� �Ǿ����� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@param   nLevel(Long, Middle, Short.)
@return  ������ �Ǿ� ������ true, �������� ������ false Return.
*/
bool OHTDetect::IsDetect(int nLevel)
{
    bool bResult = false;

#if(UBG_SENSOR_ENABLE == ON)
	switch (nLevel)
    {
    case DEF_LONG:
        if (m_pHWCommon->m_Input.OHTDETECT_Long == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case DEF_MIDDLE:
        if (m_pHWCommon->m_Input.OHTDETECT_Middle == ON)
            bResult = true;
        else
            bResult = false;
        break;
    case DEF_SHORT:
        if (m_pHWCommon->m_Input.OHTDETECT_Short == ON)
            bResult = true;
        else
            bResult = false;
        break;
    }
#endif
    return bResult;
}

/**
@brief   OHT Detect ������ �˶� �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@return  Fail ��ȣ�� Off�̸� 0, ON�̸� ���� �ڵ� Return.
*/
bool OHTDetect::CheckError()
{
    int nError = NO_ERR;

#if(OHT_NAME == OHT_NAME_STD_V81S)
	if(OHTMainForm->CheckBox_ignoreOHT->Checked == true)
	{
		return nError;
	}
#endif


#if(UBG_SENSOR_ENABLE == ON)
	if (m_pHWCommon->m_Input.OHTDETECT_Fail == ON)
		nError = ERR_DETECT_SENSOR_FAIL;
#endif
	return nError;
}

int OHTDetect::GetEnableTime(bool bInit)
{
	int nEnableTime = OHTUTILITY::PreventOverflowGetTimeMs(m_nStartEnableTime);

	if(bInit == true)
	{
		m_nStartEnableTime = 0;
	}

	return nEnableTime;
}

/**
@brief   OHT Detect ������ off �������� Ȯ���ϴ� �Լ�
@author  puting
@date    2017.08.27
@return  true�� Output ��� 1(Off), �׿� false
*/
bool OHTDetect::CheckSensorOff()
{
	bool breturn = false;

	if((m_pHWCommon->m_Output.OHTDETECT_Area1_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area2_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area3_Sel ==ON) &&
		(m_pHWCommon->m_Output.OHTDETECT_Area4_Sel ==ON))
	{
		breturn = true;
	}
	else
	{
		breturn = false;
	}
	return breturn;
}

void OHTDetect::RequestData(void)
{
	unsigned char Request_Data[UBG_Command_Packet_Size] = {0x24, 0x47, 0x0D, 0x0A};

    if(bConnect == false)
    {
		DETECT_LOG("RD1: %d", bConnect);
		OpenCom();
		if(bConnect == false)
        {
			DETECT_LOG("RD2 : %d", bConnect);
			return;
        }
    }

    if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    if(m_pCommPort->WriteCommBlock(Request_Data, UBG_Command_Packet_Size) != UBG_Command_Packet_Size)
    {
        DETECT_LOG("UBG RequestData Send Fail");
    }
}

int OHTDetect::ReceiveData(char *ReadBuff)
{
    int nError = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[2048] = {0,};

	if((m_pCommPort==NULL) || (bConnect==false))
	{
		DETECT_LOG("CommClose");
	}
    else
    {
        // 0x0A 4�� ���ŵ� �� ����
	    dwLen = m_pCommPort->ReadUntilCountOHTDetect2(szReadBuff,UBG_Final_Flag,UBG_Area_Target);

	    if(dwLen>0)
	    {
			memcpy(ReadBuff, szReadBuff, UBG_ReceiveDataSize);
			DETECT_LOG("UBG Receive : %d", dwLen);
		    nError = dwLen;
		}
	    else
	    {
		    nError = 0;
            DETECT_LOG("UBG Data Receive Fail");
	    }

        if( m_pCommPort != NULL)
		    PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    }
    return nError;
}

//Data ������ ���� Ȯ��.
int OHTDetect::CheckUbgSensorData(int *Buff)
{
    int nError = NO_ERR;
    int nReceiveDataSize = 0;
    int nSensorData = 0;
    int nStart_Pos = 0;
    unsigned char Receive_Data[2048] = {0, };
    AnsiString strLog, strTmp;

    nReceiveDataSize = ReceiveData(Receive_Data);
	DETECT_LOG("CheckUbgSensorData");

	if(nReceiveDataSize == UBG_ReceiveDataSize)
	{
		if((Receive_Data[0] == UBG_Start_Flag_1) && (Receive_Data[1] == UBG_Start_Flag_2))
		{
			strLog.sprintf("%02d", GetUbgAreaStatus());
			DETECT_LOG("UBG Sensor Area : %s", strLog);
			strLog = "";
			RapidLog(UBG_ABNORMAL_CHECK_LOG ,Receive_Data);

            for(int i = 0; i < UBG_Sensor_Data_Num; i++)
            {
                nStart_Pos = 3 * i + UBG_Sensor_Data_StartPacketSize;
                //Buff[UBG_Sensor_Data_Num-i-1] = Decode(nStart_Pos, Receive_Data, UBG_Sensor_Data);
                Buff[i] = Decode(nStart_Pos, Receive_Data, UBG_Sensor_Data);

                strTmp.sprintf("/%05d", Buff[i]);
                strLog = strLog + strTmp;

                if(i == 100 || i == 200 || i == 300 || i == 400 || i == 512)
                {
                    DETECT_LOG("UBG Data[%d] : %s", i, strLog);
                    strLog = "";
                }
            }
        }
    }
    else
    {
        nError = -1;
        DETECT_LOG(" UBG Receive Data Size is not Correct");

	}

    return nError;
}

int OHTDetect::Decode(int start, char *code, int byte)
{
	int value = 0;
	int i;
	for (i = start; i < start + byte; ++i)
    {
		value <<= 6;
		value &= ~0x3f;
		value |= code[i] - 0x30;
	}
	return value;
}

//UBG ���� Ȯ��.
int OHTDetect::UpdataData(int *Buff, bool bIsStart)
{
    static DWORD dwUBGTrigerTime = timeGetTime();
    DWORD dwStartTime = timeGetTime();

    int nError = NO_ERR;
    static int Pre_Buff[UBG_Sensor_Data_Num] = {0,};

    if(bIsStart)
    {
		RequestData();
        dwUBGTrigerTime = timeGetTime();
        DETECT_LOG("[Update]bIsStart");
    }
    else
    {
        // ���� �ּ� ���� �ð� : 240ms
        //if(dwStartTime - dwUBGTrigerTime > UBG_Sensor_Request_Data_Interval)
        if(OHTUTILITY::PreventOverflowGetTimeMs(dwUBGTrigerTime) > UBG_Sensor_Request_Data_Interval)
        {
            // Receive
            DETECT_LOG("[Update]Recieved");
            nError = CheckUbgSensorData(Buff);

            // Send
            if(nError == NO_ERR)
            {

                for(int i=0; i<UBG_Sensor_Data_Num; i++)
                {
                    Pre_Buff[i] = Buff[i];
                }
                RequestData();
            }
            else
            {
                DETECT_LOG("error");
            }

            dwUBGTrigerTime = timeGetTime();
        }
        else
        {
            for(int i=0; i<UBG_Sensor_Data_Num; i++)
            {
                Buff[i] = Pre_Buff[i];
            }
        }
    }
    return nError;
}

void OHTDetect::DataReadStart(int *Buff)
{
   //	OpenCom();  //AOHC-256
	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	DETECT_LOG("DataReadStart");
	UpdataData(Buff, true);
}


int OHTDetect::GetUbgAreaStatus()
{
	int nValue = 0;

	nValue	= 	1 * m_pHWCommon->m_Output.OHTDETECT_Area1_Sel
			+	2 * m_pHWCommon->m_Output.OHTDETECT_Area2_Sel
			+	4 * m_pHWCommon->m_Output.OHTDETECT_Area3_Sel
			+	8 * m_pHWCommon->m_Output.OHTDETECT_Area4_Sel;

	return nValue;
}

bool OHTDetect::IsConnect()
{
	if(m_pCommPort!=NULL)
	return true;
	else
	return false;
}
//AOHC-256
int OHTDetect::CheckUBGConnect(bool bConnect)
{
	int nError = NO_ERR;

	if(bConnect)
	{
		if(IsConnect() == false)
		   {
				OpenCom();
				DETECT_LOG("CheckUBGConnect -> OpenCom");
				if(IsConnect() == false)
				{
					DETECT_LOG("CheckUBGConnect -> OpenCom NG");
				   nError = ERR_UBG_WRITE;
				   return nError;
				}
				else
				{
				   return nError;       //open com �Ϸ�� return
				}

		   }
		   else
		   {
			   return nError;
		   }
	}
	else
	{    if(IsConnect() == true)
		   {
			   CloseCom();
			  DETECT_LOG("CheckUBGConnect -> Closecom");
				if(IsConnect() == true)
				{  	DETECT_LOG("CheckUBGConnect -> Closecom NG");
					nError = ERR_UBG_WRITE;
					return  nError;
				}
				else
				return nError;  //close com �Ϸ�� return

		   }
		   else
		   {
				return nError;
			}
	}

}
// ---------------------------------------------------------------------------

