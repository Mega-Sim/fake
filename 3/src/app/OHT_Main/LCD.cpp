#pragma hdrstop

#include "LCD.h"
#include "OHTMain.h"
#include "crc16.h"



/**
@brief   LCD Class ������
@author  kyh
@date    2017.12.12
@param   pHWCommon : HWCommon Ŭ���� ������
*/
LCD::LCD(HWCommon *pHWCommon)
{
	m_nComPortNum = 15;   			//COM 23
	m_nBaudRate = 115200;
	m_pCommPort = NULL;
	bConnect = false;
}


/**
@brief   LCD Class �Ҹ���
@author  kyh
@date    2017.12.12
*/
LCD::~LCD()
{
	CloseCom();
}


/**
@brief   LCD Comm Port�� Open�ϴ� �Լ�
@author  kyh
@date    2017.12.12
@return  Open �ϴµ� ���� ������ 0, ������ �߻��ϸ� ERR_OPEN ����
*/
int LCD::OpenCom()
{
	int nError = 0;

	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(m_nComPortNum, m_nBaudRate);
		if(m_pCommPort==NULL)
		{
			nError=ERR_BCR_OPEN;
		}
		else
		{
			bConnect = true;
		}
	}
	else if(m_pCommPort!=NULL)
	{
		nError = ERR_BCR_OPEN;
		bConnect = false;
	}
	return nError;
}


/**
@brief   LCD ���� ������ üũ�ϴ� �Լ�
@author  kyh
@date    2017.12.15
@return  LCD�� ������ ��� True, �������� ��� False�� Return
*/
bool LCD::IsConnect()
{
	return bConnect;
}


/**
@brief   LCD Comm Port�� Close�ϴ� �Լ�
@author  kyh
@date    2017.12.15
*/
int LCD::CloseCom()
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


/**
@brief   LCD Comm Port�� ReOpen�ϴ� �Լ�
@author  kyh
@date    2017.12.15
*/
int LCD::ReOpenCom()
{
	if(m_pCommPort!=NULL)
	{
		CloseCom();	// ����� ���� �� �����ϰ� �翬��
	}
	return OpenCom();
}



int LCD::SendTriger(unsigned char* ackData, int Length)
{
	unsigned char strBCR[50] = {0,};
	int Error = 0;
	unsigned short cCRC16_header;

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		memcpy(strBCR+1, ackData, Length);

		//CRC ����
		cCRC16_header = crc16_ccitt((const char *)&strBCR+1, Length);

		memcpy(strBCR+Length+1, (const char *)&cCRC16_header, 2);

		strBCR[0]=0x02;//Linker ���� ���� ����Ʈ
		strBCR[Length+3]=0x03;//Linker ���� ���� ����Ʈ
	}

	//strlen���� ���۽� ������ 0�� ���� ����
	//if(strlen(strBCR) != m_pCommPort->WriteCommBlock(strBCR, strlen(strBCR)))
	int LCD_Data_Length = (int)m_pCommPort->WriteCommBlock(strBCR, (Length+4));
	if(LCD_Data_Length != (Length+4))
	{
		Error=ERR_BCR_WRITE;
	}
	return Error;
}


/**
@brief   LCD Read ����� �ְ� �׿� ���� ������ �޴� �Լ�
@author  yunho.kim
@date    2017.12.24
@param   nID : ���� ��� ���� �����ϴ� ����, ���� �� 0 ����
@return  Read�ϴµ� ���� ������ 0
*/
int LCD::GetID(unsigned char* pReadBuff)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	unsigned char szReadBuff[1024] = {0,};

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_BCR_OPEN;
	}
#if((OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
        dwLen = m_pCommPort->ReadLCDUart(szReadBuff);
#else
	dwLen = m_pCommPort->ReadLCDUart(szReadBuff, LCD_ETX);
#endif

	if(dwLen>0)
	{
		memcpy(pReadBuff, szReadBuff, 26);
	}
	else
	{
		Error = ERR_HANDTP_NO_DATA;
	}
	return Error;
}
