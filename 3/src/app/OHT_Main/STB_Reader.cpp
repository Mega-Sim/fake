/**
@file	 STB_Reader.cpp
@version OHT 7.0
@brief	 STB_Reader Class CPP File
*/

#include<windows.h>
#include <stdlib.h>
#include <math.h>

#include "STB_Reader.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "Def_OHT.h"
#include "OHTMain.h"

#define ADD_LOG_STBTEST(...)    WRITE_LOG_UNIT(pLogSTBTest, __VA_ARGS__)

#define	CR						0x0D
#define	LF						0x0A

#define	STX						0x73  //0x1B   s
#define	ETX						0x65  //0x0D   e
#define	COMMON_TRIGGER			"Trigger"
#define	COMMON_OFF				0x59


#define	STB_QR_RESPONCE_MAX_LENGTH	128

#define STB_QR_BAUDRATE    9600

#define DISTANCE_RATIO	10.0//10.0//10.0//10.6		// 1mm�� �ȼ� ��
#define DISTANCE_CENTER	35.0//���� ��


/**
@brief	 STB_Reader Class ������
@author  ���¿�
@date	 2012.12.10.
@param	 pHWCommon : HWCommon Class�� ������
*/

STB_Reader::STB_Reader(HWCommon *pHWCommon, int nComPortNum, bool nDirectionPlus)
{
	pLogSTBTest = getLogUnit("D:\\log\\STBQRTest.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pHWCommon = pHWCommon;
	m_nPortNum	= nComPortNum;
	m_pCommPort = NULL;
	m_nDirection = (nDirectionPlus==true)?1:(-1);

	m_dLatestOffsetX = 0.0;
	m_dLatestOffsetY = 0.0;
	m_dLatestOffsetZ = 0.0;
	m_pdResultOffsetX = NULL;
	m_pdResultOffsetY = NULL;
	m_pdResultOffsetZ = NULL;

	m_bflag = false;

	m_dstSTBOffsetX = 0.0;
	m_dstSTBOffsetY = 0.0;
	m_dstSTBOffsetZ = 0.0;

	dChecked62 = timeGetTime();
	b_Checked62 = false;
	memset(m_szTempReadBuff, 0, sizeof(m_szTempReadBuff)); // AOHC-206 QR Class Static ���� �ߺ� ����

	Enable();
}

/**
@brief	 QR_Reader Class �Ҹ���
@author  ���¿�
@date	 2012.12.10.
*/
STB_Reader::~STB_Reader()
{
	Disable();
}

/**
@brief	 STB Code�� �д� �Լ�
@author  ���¿�
@date	 2012.12.10.   2012.12.10.
@return
*/
int STB_Reader::GetnOffset(double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ)
{
	DWORD dwLen = 0;
	char szReadBuff[1024] = {0,};
	char szResult[1024] = {0,};
	int nTmpError = NO_ERR;
	double dOffsetX = 0.0;
	double dOffsetY = 0.0;
	double dOffsetZ = 0.0;

	double dX1 = 0.0;
	double dX2 = 0.0;
	double dX3 = 0.0;
	double dX4 = 0.0;
	double dY1 = 0.0;
	double dY2 = 0.0;
	double dY3 = 0.0;
	double dY4 = 0.0;

	m_dLatestOffsetX = 0.0;
	m_dLatestOffsetY = 0.0;
	m_dLatestOffsetZ = 0.0;

	if(m_pdResultOffsetX != NULL)
	{
		*m_pdResultOffsetX = 0;
	}

	if(m_pdResultOffsetY != NULL)
	{
		*m_pdResultOffsetY = 0;
	}

	if(m_pdResultOffsetZ != NULL)
	{
		*m_pdResultOffsetZ = 0;
	}

	*pdOffsetX =0.0;
	*pdOffsetY =0.0;
	*pdOffsetZ =0.0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_QRR_OPEN;
	}

	dwLen = m_pCommPort->ReadUntil(szReadBuff, ETX);

	if(dwLen != 0)
	{
		ADD_LOG_STBTEST("%s", szReadBuff);
	}
	else
	{
		ADD_LOG_STBTEST("[GetnOffset] Length : 0");
		return ERR_QRR_READTIMEOUT;
	}

	if(dwLen != 0)
	{
		//if((szReadBuff[0] == 0x4F) && (szReadBuff[dwLen-1] == LF))  // OK,X,Y,Z<CR><LF>
		if((szReadBuff[0] == STX) && (szReadBuff[dwLen-1] == ETX))  // 1BOK,X,Y,Z<CR><LF>
		{
			//pass(���������� ���� Case)
			//ADD_LOG_STBTEST("STB Test 1");
		}
		else // Data �� �߸� ��� => ETX �ʿ�???
		{
			//if(szReadBuff[0] == 0x4F)
			if(szReadBuff[0] == STX)
			{
				memset(m_szTempReadBuff, 0x00, sizeof(m_szTempReadBuff));
				strcpy(m_szTempReadBuff, &szReadBuff[0]);
				//ADD_LOG_STBTEST("STB Test 2");
			}

			else if(szReadBuff[dwLen-1] == ETX)
			{
				memmove(m_szTempReadBuff + strlen(m_szTempReadBuff), szReadBuff, dwLen);

				memmove(szReadBuff, m_szTempReadBuff, sizeof(m_szTempReadBuff));

				dwLen = strlen(szReadBuff);

				memset(m_szTempReadBuff, 0x00, sizeof(m_szTempReadBuff));
				//ADD_LOG_STBTEST("STB Test 3");
			}
			else
			{
				memmove(m_szTempReadBuff + strlen(m_szTempReadBuff), szReadBuff, dwLen);
				//ADD_LOG_STBTEST("STB Test 4");
			}
		}

		//if((szReadBuff[0] == 0x4F) && (szReadBuff[dwLen-1] == LF))
		if((szReadBuff[0] == STX) && (szReadBuff[dwLen-1] == ETX))
		{
			szReadBuff[dwLen-1]=0;	// ETX ����
			
//			strcpy(szResult, &szReadBuff[0]);   // No Header
			strcpy(szResult, &szReadBuff[1]);   // Header ����
		
//			nTmpError = ParseData(szResult , &dOffsetX, &dOffsetY, &dOffsetZ);
//			ADD_LOG_STBTEST("STB ParseData %d, %s %.2f %.2f %.2f", nTmpError, szResult, dOffsetX, dOffsetY, dOffsetZ);

			nTmpError = ParseData(szResult , &dOffsetX, &dOffsetY, &dOffsetZ, &dX1, &dX2, &dX3, &dX4, &dY1, &dY2, &dY3, &dY4);
			ADD_LOG_STBTEST("STB ParseData %d, %s %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f",
							nTmpError, szResult, dOffsetX, dOffsetY, dOffsetZ,
							dX1, dX2, dX3, dX4, dY1, dY2, dY3, dY4);

			//Parsing �� STB Data�� ERROR�� �߻��ϸ� Erroró��. puting 2015.07.02
			if(nTmpError != NO_ERR)
			{
				nTmpError = ERR_QRR_READ;				  //Read Fail
				//Ű�ؽ� ��ü�� Ȯ�� ��� Error�߻� ����
				//Case 1. AutoID Navigator���� Tigger type�� One-shot�ϰ��, Duration ���� �������Ѱ��
				//        (Timeout �߻����� �ʵ��� mark ��ƾ���� ó��)
				//case 2. AutoID Navigator���� Option���� PreString ������ �Ǿ��ִ� �ش� ���ڰ� ���� �ʴ� ���
				dOffsetX = 0;
				dOffsetY = 0;
				dOffsetZ = 0;
			}
			if(nTmpError == NO_ERR)
			{
				m_bflag = true;
				ADD_LOG_STBTEST("m_bflag");
			}

		}
		else
		{
			// �������ݿ� ���� �� ����
			// szReadBuff[dwLen-2]=0;
			//QR_ADD_LOG("QRR In Format [%s][%x:%x]Len:%d",szReadBuff, szReadBuff[0] ,szReadBuff[dwLen-1], dwLen);
			// QR_ADD_LOG("QRR Format Error Data : [%s]", szReadBuff);

			nTmpError = ERR_QRR_DATAFORMAT;
			dOffsetX = 0.0;
			dOffsetY = 0.0;
			dOffsetZ = 0.0;
		}
	}

//	if(szReadBuff[0] == STX)
//	{
//		m_dstSTBOffsetX = dOffsetX;
//		m_dstSTBOffsetY = dOffsetY;
//		m_dstSTBOffsetZ = dOffsetZ;
//	}
//	else
//	{
//        m_dstSTBOffsetX = 0.0;
//		m_dstSTBOffsetY = 0.0;
//		m_dstSTBOffsetZ = 0.0;
//    }

	m_dLatestOffsetX = dOffsetX;
	m_dLatestOffsetY = dOffsetY;
	m_dLatestOffsetZ = dOffsetZ;

	if(m_pdResultOffsetX != NULL)
	{
		*m_pdResultOffsetX = dOffsetX;
	}
	
	if(m_pdResultOffsetY != NULL)
	{
		*m_pdResultOffsetY = dOffsetY;
	}
	
	if(m_pdResultOffsetZ != NULL)
	{
		*m_pdResultOffsetZ = dOffsetZ;
	}

	*pdOffsetX = dOffsetX;
	*pdOffsetY = dOffsetY;
	*pdOffsetZ = dOffsetZ;

	return	nTmpError;
}

/**
@brief   ���� Data�� Ȯ����
@author ���¿�
@date    2013.07.02
@return Data Ȯ�� �� ���� �ݳ�
*/
int STB_Reader::ReadBuffer()
{
	return GetnOffset(&m_dLatestOffsetX, &m_dLatestOffsetY, &m_dLatestOffsetZ);
}

/**
@brief	 STB QR Trigger On �Լ�
@author  ���¿�
@date
@return
*/
int STB_Reader::SendTriggerOn()
{
	char strMsg[1024] = {0,};

	int nTmpError = 0;

	m_dLatestOffsetX = 0.0;
	m_dLatestOffsetY = 0.0;
	m_dLatestOffsetZ = 0.0;

	if(m_pdResultOffsetX != NULL)
	{
		*m_pdResultOffsetX = 0;
	}
	
	if(m_pdResultOffsetY != NULL)
	{
		*m_pdResultOffsetY = 0;
	}
	
	if(m_pdResultOffsetZ != NULL)
	{
		*m_pdResultOffsetZ = 0;
	}

	if(m_pCommPort == NULL)
	{
		nTmpError = ReStart();
	}

	if(m_pCommPort == NULL)
	{
		return ERR_QRR_OPEN;
	}

	if(nTmpError == NO_ERR && m_pCommPort != NULL)
	{
		sprintf(strMsg, "%s%c%c", COMMON_TRIGGER, CR, LF);

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)))
		{
			nTmpError = ERR_QRR_WRITE;
		}
	}
	return nTmpError;
}

/**
@brief	 QR Code�� ��ġ�� ����ϴ� �Լ�
@author  ���¿�
@date	 2012.12.10.
@return
*/
int STB_Reader::ParseData(char* szData, double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ)
{
	// OK,2.12,3.67,4.12<CR><LF>
	// NG,-99.999,-99.999,-99.999<CR><LF>
	int nTmpError = NO_ERR;
	char szTemp[STB_QR_RESPONCE_MAX_LENGTH] = {0,};
	int nLen = strlen(szData);
	int nIndexOfStart = -1, nIndexOfEnd = -1;
	double dTempOffset = 0.0;

	bool		bMeetNoneZero	= false;	// OPTICON
	int			nMeetNoneZero	= 0;		// OPTICON
	int			nDirection		= 1;		// OPTICON
	
	int n = 0;
	int i = 0;

	//------------------------------------------------------------------------------
	//2	1. ����� �޾ƿ´�.  ó�� ���� ,�� ������ ��ġ ��
	for(n=0; n<nLen; n++)
	{
		if((szData[n] >= 'A' && szData[n] <= 'Z') || (szData[n] >= '0' && szData[n] <= '9') || (szData[n] == ',') || (szData[n] == '-') || (szData[n] == '.'))
		{
			szTemp[n] = szData[n];
		}
		else
		{
			ADD_LOG_STBTEST("ParseData 1 Error [%s:%s], [%c]", szData, szTemp, szData[n]);
			break;
		}
	}

	//2.1.1 ���� Data�� "NG"���� Ȯ���Ѵ�.
	if(strstr(szTemp, "N") != NULL)
	{
		// QR_ADD_LOG("ParseData 2 Error [%s:%s]", szData, szTemp);
		return ERR_QRR_READ; //Error ó��
	}
	//------------------------------------------------------------------------------
	for(i=0; i<3; i++)
	{
		//2	2. Offset�� �޾� �´�
		memset(&szTemp, 0, sizeof(szTemp));

		// Offset�� ������ ���� �޾��ش�.
		if(i == 0)
		{
			nIndexOfEnd = nLen;
		}
		else
		{
			for(n=nLen-1; n>=0; n--)
			{
				if(szData[n] == ',')
				{
					nIndexOfEnd = n;
					break;
				}
			}
		}

		//2	Offset�� ���� ���� �޾��ش�.
		for(n=nIndexOfEnd-1; n>=0; n--)
		{
			// if(szData[n]==':')	// only KEYENCE
			if(szData[n] == ',')	//Both KEYENCE and OPTICON
			{
				nIndexOfStart = n;
				break;
			}
		}

		if((nIndexOfStart != -1)&&(nIndexOfEnd !=-1))
		{
			//ADD_LOG_STBTEST("nIndex Check [%d %d]",nIndexOfStart, nIndexOfEnd);
			for(n=nIndexOfStart+1; n<nIndexOfEnd; n++)
			{
				//ADD_LOG_STBTEST("nIndex Check 2 [%c]", szData[n]);
				if((szData[n] >= '0' && szData[n] <= '9') || (szData[n] == '.') || (szData[n] == '-'))
				{
					if(szData[n]!='0')
					{
						bMeetNoneZero = true;
					}
					else if(bMeetNoneZero==false)
					{
						szData[n]=' ';
						nMeetNoneZero++;
					}
					szTemp[n-nIndexOfStart-1-nMeetNoneZero] = szData[n];
				}
			}
		}
		else
		{
			nTmpError = ERR_QRR_DATAFORMAT;
			break;
		}

		if(nTmpError == NO_ERR)
		{
			dTempOffset = atof(szTemp);	// int ó��
		}

		switch(i)
		{
			case 0:
				*pdOffsetZ = dTempOffset;
				break;

			case 1:
				*pdOffsetY = dTempOffset;
				break;

			case 2:
				*pdOffsetX = dTempOffset;
				break;
		}

		nLen = nIndexOfStart + 1;
	}

	return nTmpError;
}

int STB_Reader::ParseData(char* szData, double *pdOffsetX, double *pdOffsetY, double *pdOffsetZ, double *dX1, double *dX2, double *dX3, double *dX4, double *dY1, double *dY2, double *dY3, double *dY4)
{
	// OK,2.12,3.67,4.12<CR><LF>
	// NG,-99.999,-99.999,-99.999<CR><LF>
	int nTmpError = NO_ERR;
	char szTemp[STB_QR_RESPONCE_MAX_LENGTH] = {0,};
	int nLen = strlen(szData);
	int nIndexOfStart = -1, nIndexOfEnd = -1;
	double dTempOffset = 0.0;

	bool		bMeetNoneZero	= false;	// OPTICON
	int			nMeetNoneZero	= 0;		// OPTICON
	int			nDirection		= 1;		// OPTICON

	int n = 0;
	int i = 0;

	//------------------------------------------------------------------------------
	//2	1. ����� �޾ƿ´�.  ó�� ���� ,�� ������ ��ġ ��
	for(n=0; n<nLen; n++)
	{
		if((szData[n] >= 'A' && szData[n] <= 'Z') || (szData[n] >= '0' && szData[n] <= '9') || (szData[n] == ',') || (szData[n] == '-') || (szData[n] == '.'))
		{
			szTemp[n] = szData[n];
		}
		else
		{
			ADD_LOG_STBTEST("ParseData 1 Error [%s:%s], [%c]", szData, szTemp, szData[n]);
			break;
		}
	}

	//2.1.1 ���� Data�� "NG"���� Ȯ���Ѵ�.
	if(strstr(szTemp, "N") != NULL)
	{
		// QR_ADD_LOG("ParseData 2 Error [%s:%s]", szData, szTemp);
		return ERR_QRR_READ; //Error ó��
	}
	//------------------------------------------------------------------------------
	for(i=0; i<11; i++)
	{
		//2	2. Offset�� �޾� �´�
		memset(&szTemp, 0, sizeof(szTemp));

		// Offset�� ������ ���� �޾��ش�.
		if(i == 0)
		{
			nIndexOfEnd = nLen;
		}
		else
		{
			for(n=nLen-1; n>=0; n--)
			{
				if(szData[n] == ',')
				{
					nIndexOfEnd = n;
					break;
				}
			}
		}

		//2	Offset�� ���� ���� �޾��ش�.
		for(n=nIndexOfEnd-1; n>=0; n--)
		{
			// if(szData[n]==':')	// only KEYENCE
			if(szData[n] == ',')	//Both KEYENCE and OPTICON
			{
				nIndexOfStart = n;
				break;
			}
		}

		if((nIndexOfStart != -1)&&(nIndexOfEnd !=-1))
		{
			//ADD_LOG_STBTEST("nIndex Check [%d %d]",nIndexOfStart, nIndexOfEnd);
			for(n=nIndexOfStart+1; n<nIndexOfEnd; n++)
			{
				//ADD_LOG_STBTEST("nIndex Check 2 [%c]", szData[n]);
				if((szData[n] >= '0' && szData[n] <= '9') || (szData[n] == '.') || (szData[n] == '-'))
				{
					if(szData[n]!='0')
					{
						bMeetNoneZero = true;
					}
					else if(bMeetNoneZero==false)
					{
						szData[n]=' ';
						nMeetNoneZero++;
					}
					szTemp[n-nIndexOfStart-1-nMeetNoneZero] = szData[n];
				}
			}
		}
		else
		{
			nTmpError = ERR_QRR_DATAFORMAT;
			break;
		}

		if(nTmpError == NO_ERR)
		{
			dTempOffset = atof(szTemp);	// int ó��
		}

		switch(i)
		{
			case 0:
				*dX1 = dTempOffset;
				break;

			case 1:
				*dX2 = dTempOffset;
				break;

			case 2:
				*dX3 = dTempOffset;
				break;

			case 3:
				*dX4 = dTempOffset;
				break;

			case 4:
				*dY1 = dTempOffset;
				break;

			case 5:
				*dY2 = dTempOffset;
				break;

			case 6:
				*dY3 = dTempOffset;
				break;

			case 7:
				*dY4 = dTempOffset;
				break;

			case 8:
				*pdOffsetZ = dTempOffset;
				break;

			case 9:
				*pdOffsetY = dTempOffset;
				break;

			case 10:
				*pdOffsetX = dTempOffset;
				break;
		}

		nLen = nIndexOfStart + 1;
	}

	return nTmpError;
}


/**
@brief	 QR_Reader �ø��� ����� open�ϴ� �Լ�
@author  ���¿�
@date	 2012.12.10.
@return
*/
int STB_Reader::Enable()
{
	int nError = NO_ERR;

	if(m_pCommPort == NULL)
	{
		m_pCommPort = new Comm_Port(m_nPortNum, STB_QR_BAUDRATE, 8 , NOPARITY, ONESTOPBIT);
		 // NOPARITY, ODDPARITY, EVENPARITY  // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
		if(m_pCommPort == NULL)
		{
			nError=ERR_QRR_OPEN;
		}
	}
	return nError;
}

/**
@brief	 QR_Reader �ø��� ����� close�ϴ� �Լ�
@author  ���¿�
@date	 2012.12.10.
@return
*/
void STB_Reader::Disable()
{
	if(m_pCommPort != NULL)
	{
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
//	m_pCommPort->Close();
}

int STB_Reader::ReStart()
{
	if(m_pCommPort != NULL)
	{
		Disable();	// ����� ���� �� �����ϰ� �翬��
	}

	return Enable();
}

bool STB_Reader::Connect()
{
	if(m_bflag == true)	return true;
	else	return false;

}

int STB_Reader::ReadData()
{
	int nError = NO_ERR;
	double dX = 0.0, dY = 0.0, dZ = 0.0;

	if(m_pCommPort != NULL)
	{
		nError = SendTriggerOn();

		Sleep(3200);

		if(nError == NO_ERR)
		{
			nError = GetnOffset(&dX, &dY, &dZ);
		}

		if(nError == NO_ERR)
			ADD_LOG_STBTEST("GetnOffset [%.2f, %.2f, %.2f]", dX, dY, dZ);
	}

	return nError;
}


