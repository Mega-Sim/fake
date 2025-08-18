/**
@file    Lookdown.cpp
@version OHT 7.0
@brief   Lookdown Class CPP File
*/

#include "Lookdown.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#include "LogHandler.h"	// Log
#define ADD_LOOKDOWN_LOG(...)	WRITE_LOG_UNIT(m_pLogLookdown, __VA_ARGS__)
#define	STX					0x02
#define	ETX					0x03

/**
@brief   Lookdown Class ������
@author
@date
@param   pHWCommon : HWCommon Class�� ������
*/
Lookdown::Lookdown(HWCommon * pHWCommon)
{
	m_pHWCommon = pHWCommon;

	m_nStartEnableTime = timeGetTime();
	memset(DecodedData, 0, sizeof(DecodedData));
	memset(DistanceData, 0, sizeof(DistanceData));

	m_nComPortNum = COM_LOOKDOWN;
	m_nBaudRate = 57600;
	m_pCommPort = NULL;
	bConnect = false;
    m_pLogLookdown = getLogUnit("D:\\log\\Lookdown.log", 1024*10);

	Init();
}
/**
@brief   Lookdown Class �Ҹ���
@author
@date
*/


Lookdown::~Lookdown()
{
	CloseCom();
}



/**
@brief   Lookdown Sensor ������ ��Ʈ�� �ϴ� �Լ�
@author  LSJ
@date    2012.10.30
@Param   nMode(Power On, Power Off, Power Reset)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Lookdown::PowerControl(int nMode)
{
    int nError = 0;

    switch (nMode)
    {
    case PWR_OFF:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, ON);
        break;
    case PWR_ON:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, OFF);
        break;
    case PWR_RST:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, ON);
        if (nError == NO_ERR)
        {
            Sleep(500);
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, OFF);
        }
        break;
    }

    return nError;
}

/**
@brief   Lookdown Sensor ������ �Ѹ鼭 Area1�� �Ѵ� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Lookdown::Enable()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, OFF);

    return nError;
}

/**
@brief   Lookdown Sensor ������ �Ѹ鼭 Area�� ��� ���� �Լ�
@author  LSJ
@date    2012.10.30
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Lookdown::Disable()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_POWER_RESET, OFF);

    return nError;
}

/**
@brief   Lookdown Sensor ������ �Ѹ鼭 Area�� ��� ���� �Լ�
@author  LSJ
@date    2012.10.30
@Param   nArea(0 : Disable, 1 : Area1, 2 : Area2, 3 : Area3)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Lookdown::AreaSelect(int nArea)
{
    int nError = NO_ERR;
	m_nArea = nArea;
    switch (nArea)
    {
    case 0:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 1:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 2:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 3:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 4:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 5:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 6:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 7:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, ON);
        break;
    case 8:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 9:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 10:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 11:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 12:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 13:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 14:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;
    case 15:
        nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA1_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA2_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA3_SEL, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LOOKDOWN_AREA4_SEL, OFF);
        break;		
    }

    return nError;
}

/**
@brief   Lookdown Sensor�� ��ֹ��� �����Ǿ����� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@return  ��ֹ��� ���� ��ٿ� ������ �����Ǹ� true, �������� ������ false ����.
*/
bool Lookdown::IsDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.LOOKDOWN_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Lookdown Sensor�� ���� �������� Ȯ���ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@return  ��ٿ� ������ ������ �־ ���� �Է��� ������ true, ������ false ����.
*/
bool Lookdown::IsError()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.LOOKDOWN_Error == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   Lookdown ���� �����ð��� ����
@author  doori.shin
@date    2016.04.07
@param	 �����ð� ���۰��� �ʱ�ȭ ���� ����
@return  ������ Enable�Ǿ��ִ� �ð��� �������� ����
*/
int Lookdown::GetEnableTime(bool bInit)
{
	int nEnableTime = OHTUTILITY::PreventOverflowGetTimeMs(m_nStartEnableTime);

	if(bInit == true)
	{
        m_nStartEnableTime = 0;
    }

	return nEnableTime;
}

void Lookdown::Init()
{
	m_nStatus = STATUS_PBS_NOT_CONNTECTED;
	CertifCode = 0;
	NumDecodedData = 0;
	dwStartTime = timeGetTime();
	dwLastLinkTime = timeGetTime();
	m_nArea =0;
	//nError = OpenCom();
}

int Lookdown::OpenCom()
{
	int nError = 0;
    m_nStatus = STATUS_PBS_NOT_CONNTECTED;
	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
		if(m_pCommPort==NULL)
		{
			ADD_LOOKDOWN_LOG("ERR COMPORT OPEN FAIL");
            bConnect = false;
			//nError=ERR_LOOKDOWN_OPEN;
		}
		else
		{
			ADD_LOOKDOWN_LOG("COMPORT OPEN SUCCESS");
			bConnect = true;
		}
	}
	else if(m_pCommPort!=NULL)
	{
		//nError = ERR_LOOKDOWN_OPEN;
		bConnect = true;
	}
	return nError;
}

void Lookdown::CloseCom()
{
   if(m_pCommPort)
   {
	  delete (Comm_Port *) m_pCommPort;
	  m_pCommPort = NULL;
    }
   	if(m_pCommPort==NULL)
	{
        ADD_LOOKDOWN_LOG("COMPORT Close SUCCESS");
        bConnect = false;
	}
    else
    {
        ADD_LOOKDOWN_LOG("COMPORT Close Fail");
    }
    m_nStatus = STATUS_PBS_NOT_CONNTECTED;
}

int Lookdown::ReadData(char *RtnReadBuff, char cTarget)
{
	int i, index = 0;
	char chBuff;

	// for log display
	AnsiString strTmp1, strTmp2;
	char oneChar;
	// --------------------------

	for(i=0; i<PBS_BUFF_SIZE; i++)
	{
		if(m_pCommPort == NULL)
		{
			index=0;
			break;
		}
		if(m_pCommPort->ReadCommBlock(&chBuff,1)==0)  // One Byte Reading
		{
			if(m_pCommPort != NULL) {
				PurgeComm(m_pCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
			}
			index=0;
			break;
		}
		*(RtnReadBuff + index ) = chBuff;
		index++;

		if(chBuff == cTarget) {
			break;
		}
	}
	if(index == 0)
	{
		ADD_LOOKDOWN_LOG("Serial Received : NULL");
	}
	else if(index > 0 && index <= PBS_BUFF_SIZE)
	{
		//ADD_LOOKDOWN_LOG("Serial Received :%s", RtnReadBuff);
	}
	else if(index > PBS_BUFF_SIZE)
	{
		ADD_LOOKDOWN_LOG("Serial Received index over:%d", index);
	}

	return index;
}

void Lookdown::CheckSensorArea()
{
	DWORD dwCurTime = timeGetTime();
	if ((m_nStatus == STATUS_PBS_WAIT_CODE && ((dwCurTime - dwStartTime)>=PBS_WAIT_CODE_TIME))
		|| (m_nStatus == STATUS_PBS_WAIT_LINK && ((dwCurTime - dwStartTime)>=PBS_WAIT_LINK_TIME))
		|| (m_nStatus == STATUS_PBS_WAIT_DATA && ((dwCurTime - dwStartTime)>=PBS_WAIT_DATA_TIME)))
	{
		Obs_OnCommunication();
		dwStartTime = timeGetTime();
	}
}

void Lookdown::Obs_OnCommunication()
{
	int		num_data = 0;
	unsigned char data[PBS_BUFF_SIZE];

	num_data = ReadData((char *)&data, (char)ETX);

	if((num_data!=0)&&(num_data!=10)&&( data[num_data-1] == ETX ))
	{
		//���ڵ��� ����
		NumDecodedData = NumDecode(num_data);
		ADD_LOOKDOWN_LOG("NumDecodedData: %d:", NumDecodedData);
		Decode(num_data, data, DecodedData);
		switch(m_nStatus)
		{
			case STATUS_PBS_WAIT_CODE:
				if( OnGetCode() )
				{
					ADD_LOOKDOWN_LOG("Obs_OnCommunication:RequestLink");
					RequestLink();
					m_nStatus = STATUS_PBS_WAIT_LINK;
				}
                		else
				{
					ADD_LOOKDOWN_LOG("Obs_OnCommunication:RequestCode");
					RequestCode();
				}
			break;

			case STATUS_PBS_WAIT_LINK:
				if( OnGetLink() )
				{
					ADD_LOOKDOWN_LOG("Obs_OnCommunication:RequestData1");
					RequestData();
					m_nStatus = STATUS_PBS_WAIT_DATA;
				}

			break;

			case STATUS_PBS_WAIT_DATA:
				OnGetData();
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwLastLinkTime)>=PBS_LINK_REQUEST_CYCLE_TIME)
				{
					ADD_LOOKDOWN_LOG("Obs_OnCommunication:RequestLink2");
					RequestLink();
					m_nStatus = STATUS_PBS_WAIT_LINK;
				}
				else
				{
					ADD_LOOKDOWN_LOG("Obs_OnCommunication:RequestData2");
					RequestData();
					m_nStatus = STATUS_PBS_WAIT_DATA;
				}

			break;

		}
	}
    else if(num_data==10)
    {
        ADD_LOOKDOWN_LOG("Obs_OnCommunication:Waiting Data");
    }
}

void Lookdown::PBSDetect_ReadStart(void)
{
	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

    ADD_LOOKDOWN_LOG("PBSDetect_ReadStart, OpenCom");
    OpenCom();
	dwStartTime = timeGetTime();
	ADD_LOOKDOWN_LOG("PBSDetect_ReadStart, RequestCode");
	RequestCode();
	m_nStatus=STATUS_PBS_WAIT_CODE;
}

void Lookdown::PBSDetect_ReadStop(void)
{
	ADD_LOOKDOWN_LOG("PBSDetect_ReadStop");
	m_nStatus=STATUS_PBS_NOT_CONNTECTED;
	if( m_pCommPort != NULL)
		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	CloseCom();
}


void Lookdown::RequestCode(void)
{
	unsigned char data[4];
	unsigned char encode[8];
	unsigned short crc;
	DWORD dwLength;
	DWORD dwdd;

	//��� ������
	data[0] = 0xA0;
	data[1] = 0x69;

	//����� CRC��
	crc = LsbCRC(0,data,2);

	//CRC���� ���޾� ����
	data[2] = (crc>>8) & 0xff;
	data[3] = crc & 0xff;

	//������ ���� ���ڵ�����
	Encode(4,data,encode);

	//������
	dwLength=NumEncode(4);

	if(m_pCommPort->WriteCommBlock(encode,dwLength) != dwLength)
	{
		/******
		TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
		m_LogFile->WriteLogFile("PBSStatus","OBS : RequestCode Write Error[Retry]!");
		DeleteAndNULL(m_LogFile);
		******/
		m_pCommPort->WriteCommBlock(encode,dwLength);
	}

}

//---------------------------------------------------------------------------
//������ ����� CRC ���� �߰��� �����Ϳ� ���� ������
unsigned short Lookdown::LsbCRC(unsigned short crc_accum, unsigned char *data_blk_ptr,unsigned short data_blk_size)
{
	static unsigned short crc_table[256] =
	{
		0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
		0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
		0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
		0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
		0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
		0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
		0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
		0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
		0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
		0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
		0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
		0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
		0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
		0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
		0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
		0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
		0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
		0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
		0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
		0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
		0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
		0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
		0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
		0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
		0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
		0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
		0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
		0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
		0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
		0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
		0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
		0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
	};


	register unsigned short i, j;
	unsigned short inv, out1, out2;

	for(j=0; j<data_blk_size; j++)
	{

		//������
		inv = 0;
		for(i=0; i<8; i++)
		{
			if(((*data_blk_ptr)>>i)&0x01)
				inv = inv | (0x01<<(7-i));
		}

		//�������
		i = ((unsigned short)(crc_accum >> 8) ^ inv) & 0xff;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
		data_blk_ptr++;

	}

	//����� ������
	out1 = 0;
	for(i=0; i<8; i++)
	{
		if( (crc_accum>>i)&0x01 )
			out1 = out1 | ( 0x01<<(7-i) );
	}
	out2 = 0;
	for(i=0; i<8; i++)
	{
		if( (crc_accum>>(i+8))&0x01 )
			out2 = out2 | ( 0x01<<(7-i) );
	}

	return (out2<<8) + out1;
}

//---------------------------------------------------------------------------
void Lookdown::Encode(int num_in, unsigned char *in, unsigned char *out)
{
	int i,j;

	//	//���� encoding �ϸ� �����Ʈ�� �Ǵ��� ���� �޸� �Ҵ�����. ��ü ����Ʈ�� + STX,ETX
	//	*num_out = (num_in/3+1)*4+2;
	//	out = new unsigned char[*num_out];

	out[0] = 0x02;
	for(i=2,j=1; i<num_in; i+=3,j+=4)
	{
		//3byte�� �ҷ��ͼ� 6��Ʈ�� ���� �� 0x20 ���ϱ�
		out[j]   = (unsigned char)( (in[i-2]>>2)&0x3F )+ 0x20;
		out[j+1] = (unsigned char)( ((in[i-2]&0x03)<<4) ) + (unsigned char)( (in[i-1]>>4)&0x0F ) + 0x20;
		out[j+2] = (unsigned char)( ((in[i-1]&0x0F)<<2) ) + (unsigned char)( (in[i]>>6)&0x03   ) + 0x20;
		out[j+3] = (unsigned char)( in[i]&0x3F ) + 0x20;
	}

	//0�� ä��� �ѹ��� �۾�

	if( i - (num_in-1) == 2 ) //1byte�� ������ ��
	{
		out[j]   = (unsigned char)( (in[i-2]>>2)&0x3F   ) + 0x20;
		out[j+1] = (unsigned char)( ((in[i-2]&0x03)<<4) ) + 0x20;

		//2byte�� 0�̹Ƿ� ������ �ʴ´�.
		out[j+2] = 0x03;

	}
	else if( i - (num_in-1) == 1 ) //2byte ������ ��
	{
		out[j]   = (unsigned char)( (in[i-2]>>2)&0x3F ) + 0x20;
		out[j+1] = (unsigned char)( ((in[i-2]&0x03)<<4) ) + (unsigned char)( (in[i-1]>>4)&0x0F ) + 0x20;
		out[j+2] = (unsigned char)( ((in[i-1]&0x0F)<<2) ) + 0x20;

		//1byte�� 0�̹Ƿ� ������ �ʴ´�.
		out[j+3] = 0x03;
	}
	else //���� byte�� ���� ���
	{
		out[j] = 0x03;
	}
}

//---------------------------------------------------------------------------
//STX(0x02)�� ETX(0x03)�� ������ ������ ������ decoding�� �Ѵ�.
void Lookdown::Decode(int num_in, unsigned char *in, unsigned char *out)
{
	int i,j;


	for(i=4,j=0; i<num_in-1; i+=4,j+=3)
	{

		out[j]   = (unsigned char)( ((in[i-3]-0x20)<<2)&0xFC ) + (unsigned char)( ((in[i-2]-0x20)>>4)&0x03 );
		out[j+1] = (unsigned char)( (in[i-2]-0x20)<<4 )        + (unsigned char)( ((in[i-1]-0x20)>>2)&0x0F );
		out[j+2] = (unsigned char)( (in[i-1]-0x20)<<6 )        + (unsigned char)( (in[i]-0x20)&0x3F );
	}

	//0�� ä��� �ѹ��� �۾�
	if( i - (num_in-2) == 3 )  //1byte�� ������ ��
	{

		out[j]   = (unsigned char)( ((in[i-3]-0x20)<<2)&0xFC );
	}
	else if( i - (num_in-2) == 2 )  //2byte�� ������ ��
	{

		out[j]   = (unsigned char)( ((in[i-3]-0x20)<<2)&0xFC ) + (unsigned char)( ((in[i-2]-0x20)>>4)&0x03 );
		out[j+1] = (unsigned char)( (in[i-2]-0x20)<<4 );
	}
	else if( i - (num_in-2) == 1 )  //3byte�� ������ ��
	{

		out[j]   = (unsigned char)( ((in[i-3]-0x20)<<2)&0xFC ) + (unsigned char)( ((in[i-2]-0x20)>>4)&0x03 );
		out[j+1] = (unsigned char)( (in[i-2]-0x20)<<4 )        + (unsigned char)( ((in[i-1]-0x20)>>2)&0x0F );
		out[j+2] = (unsigned char)( (in[i-1]-0x20)<<6 );
	}

}
//---------------------------------------------------------------------------
int Lookdown::NumEncode(int num_in)
{
	//���� encoding �ϸ� �����Ʈ�� �Ǵ��� ���� �޸� �Ҵ�����. ��ü ����Ʈ�� + STX,ETX
	if( num_in%3 == 1)
		return (num_in/3+1)*4;
	else if(num_in%3 == 2)
		return (num_in/3+1)*4+1;
	else
		return (num_in/3)*4+2;
}
//---------------------------------------------------------------------------
int Lookdown::NumDecode(int num_in)
{
	//���� decoding �ϸ� �����Ʈ�� �Ǵ��� ���� �޸� �Ҵ�����.
	//STX,ETX ���� 6bit ¥���� ������ �̾� 8bit�� �ڸ� �����̴�.
	return	((num_in-2)*6+1)/8;
}
//---------------------------------------------------------------------------
//���� �ڵ� �޾��� �� �Ҹ��� �Լ�
bool Lookdown::OnGetCode(void)
{
	// DecodedData���� 2byte���� 9byte���� �� 8byte�� ����� CRC���� ���Ѵ�.
	// �̰��� ��ũ ������ Ű ������ �̿�ȴ�.
	int i;
	unsigned char code[8];

	if( DecodedData[1] != 0x69 )
		return false;

	for(i=0; i<8; i++)
		code[i] = DecodedData[i+2];

	CertifCode = LsbCRC(0, code, 8);

	return true;
}

//---------------------------------------------------------------------------
//��ũ ���� �䱸
void Lookdown::RequestLink(void)
{
	unsigned char data[7];
	unsigned char encode[12];
	unsigned short crc;
	DWORD dwLength;

	//��� ������
	data[0] = 0xA0;
	data[1] = 0x5A;
	data[2] = 0x01;
	data[3] = (CertifCode>>8) & 0xff;
	data[4] = CertifCode & 0xff;

	//����� CRC��
	crc = LsbCRC(0,data,5);

	//CRC���� ���޾� ����
	data[5] = (crc>>8) & 0xff;
	data[6] = crc & 0xff;

	//������ ���� ���ڵ�
	Encode(7,data,encode);

	//������
	dwLength=NumEncode(7);
	ADD_LOOKDOWN_LOG("RequestLink writeblock1, dwLength:%d", dwLength);
	if(m_pCommPort->WriteCommBlock(encode,dwLength) != dwLength)
	{
		/******
		TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
		m_LogFile->WriteLogFile("PBSStatus","OBS : RequestLink Write Error[Retry]!");
		DeleteAndNULL(m_LogFile);
		******/
		ADD_LOOKDOWN_LOG("RequestLink writeblock2, dwLength:%d", dwLength);
		m_pCommPort->WriteCommBlock(encode,dwLength);
	}

	ADD_LOOKDOWN_LOG("RequestLink writeblock3, dwLength:%d", dwLength);
	//AnsiString strdd.sprintf("Snd : %X %X  %X %X",data[0], data[1], data[2], data[3]);

	dwLastLinkTime = timeGetTime();
}

//---------------------------------------------------------------------------
//��ũ ���� �޾��� �� �Ҹ��� �Լ�
bool Lookdown::OnGetLink(void)
{
	if( DecodedData[1] != 0x5A )
		return false;

	return true;
}
//---------------------------------------------------------------------------
//������ �䱸
void Lookdown::RequestData(void)
{
	unsigned char data[4];
	unsigned char encode[8];
	unsigned short crc;
	DWORD dwLength;

	//��� ������
	data[0] = 0xA2;
	data[1] = 0x69;

	//����� CRC��
	crc = LsbCRC(0,data,2);

	//CRC���� ���޾� ����
	data[2] = (crc>>8) & 0xff;
	data[3] = crc & 0xff;

	//������ ���� ���ڵ�����
	Encode(4,data,encode);

	//������
	dwLength=NumEncode(4);
	if(m_pCommPort->WriteCommBlock(encode,dwLength) != dwLength)
	{
		/******
		TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
		m_LogFile->WriteLogFile("PBSStatus","OBS : RequestData Write Error[Retry]!");
		DeleteAndNULL(m_LogFile);
		*******/
		m_pCommPort->WriteCommBlock(encode,dwLength);
	}

	AnsiString strdd;
	strdd.sprintf("Snd : %X %X  %X %X",data[0], data[1], data[2], data[3]);
}
//---------------------------------------------------------------------------
//������ �޾��� �� �Ҹ��� �Լ�
bool Lookdown::OnGetData(void)
{
	int i;
	static int nError = 0;
	AnsiString strLog, strTmp;


	//�߸��� �����Ͱ� ���� ��� ī��Ʈ
	if(NumDecodedData !=246 )
	{
		nError++;

		strLog.sprintf("OBS : Error[Data[%d] != 246][Count=%d]",NumDecodedData, nError);
		ADD_LOOKDOWN_LOG("Error Cnt: %d",nError);
		return false;
	}


	//�����͸� DistanceData�� DataSet�� �ִ´�.
	/*
	for(i=0; i<NUM_PBS_DATA; i++)
	{
		if(i<35 || i>85)
		{
			DistanceData[i] = -1;
		}
		else
		{
			DistanceData[i] = (int)(DecodedData[2+i*2+1]*0x100 + DecodedData[2+i*2]);
			if( DistanceData[i]> 0xf000 ) DistanceData[i] = -1;
			//DistanceData[i] = DistanceData[i]+PBS_DISTOFFSET;
		}
	}
	*/

	strLog =m_nArea;

	for (int i = 0; i < NUM_PBS_DATA; i++)
	{
		DistanceData[i] = (int)(DecodedData[2+i*2+1]*0x100 + DecodedData[2+i*2]);
		if( DistanceData[i]> 0xf000 ) DistanceData[i] = -1;
		strTmp.sprintf("/%03d", DistanceData[i]);
		strLog = strLog + strTmp;
	}
	ADD_LOOKDOWN_LOG("DistanceData: %s", strLog);

	return true;
}

//---------------------------------------------------------------------------

int Lookdown::UpdataData(int *Buff)
{
	int nError = NO_ERR;

	CheckSensorArea();
	for(int i = 0; i <NUM_PBS_DATA; i++) Buff[i] = DistanceData[i];

	return nError;
}
//---------------------------------------------------------------------------




