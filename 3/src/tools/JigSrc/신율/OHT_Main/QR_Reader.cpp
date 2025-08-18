/**
@file	 QR_Reader.cpp
@version OHT 7.0
@brief	 QR_Reader Class CPP File
*/

#include<windows.h>
#include <stdlib.h>
#include <math.h>

#include "QR_Reader.h"
#include "Utility.h"
#include "ErrorCode.h"
#include "Def_OHT.h"
#include "OHTMain.h"

//#define ADD_LOG_QRTEST(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\QRTest.log", __VA_ARGS__)
//����
#define ADD_LOG_QRTEST(...)    WRITE_LOG_UNIT(pLogQRTest, __VA_ARGS__)

#define	CR						0x0D
#define	LF						0x0A
#define KEIENCE_QRR_TRIGGER		"LON"

#define	STX						0x1B
#define	ETX						0x0D
#define	COMMON_QRR_TRIGGER		0x5A
#define	COMMON_QRR_OFF			0x59


#define	QR_RESPONCE_MAX_LENGTH	128

#define QR_BAUDRATE    115200

#define QR_DISTANCE_RATIO	10.0//10.0//10.0//10.6		// 1mm�� �ȼ� ��
#define QR_DISTANCE_CENTER	35.0//���� ��
#define QR_DISTANCE_CENTER_OF_OMRON		 32.7//���� ��
#define QR_DISTANCE_CENTER_OF_DM50	 	 37.0//���� ��
#define QR_DISTANCE_CENTER_OF_DM50_R	 34.0//���� ��
#define QR_DISTANCE_CENTER_OF_MS	 46.5//���� ��
#define QR_DISTANCE_RATIO_OF_KEYENCE	11.0//10.0//10.0//10.6		// 1mm�� �ȼ� ��   0~700
#define QR_DISTANCE_RATIO_OF_OPTICON	8.42857//10.0//10.6		// 1mm�� �ȼ� ��   0~590
#define QR_DISTANCE_RATIO_OF_OMRON	    11.0//6.26667//10.0//10.6		// 1mm�� �ȼ� ��   0~752
#define QR_DISTANCE_RATIO_OF_DM50	    10.16216//6.26667//10.0//10.6		// 1mm�� �ȼ� ��   0~752
#define QR_DISTANCE_RATIO_OF_DM50_R	    11.058824//6.26667//10.0//10.6		// 1mm�� �ȼ� ��   0~752
#define QR_DISTANCE_RATIO_OF_MS	    8.086//6.26667//10.0//10.6		// 1mm�� �ȼ� ��   0~752

#define QR_MAKER_RIGHT 			QR_MAKER_KEYENCE     //QR_MAKER_KEYENCE//QR_MAKER_OMRON
#define QR_MAKER_LEFT 			QR_MAKER_KEYENCE     //QR_MAKER_KEYENCE//QR_MAKER_OMRON

/**
@brief	 QR_Reader Class ������
@author  ���¿�
@date	 2012.12.10.
@param	 pHWCommon : HWCommon Class�� ������
*/


// �α� �ۼ� �Լ�
char	QR_gm_SzLog[1024]	= {0,};
DWORD	QR_gm_time		= timeGetTime();


void QR_writeLog()
{
	QR_gm_time = timeGetTime();
	FILE *fp;
	fp = fopen("D:\\log\\QRR.log", "a");

	if (fp != NULL)
	{
	  try
	  {
		 fprintf(fp, "[%04lu]%s\n", QR_gm_time, QR_gm_SzLog);
		 fflush(fp);
	  }
	  catch (Exception &e)
	  {
		 fclose(fp);
		 fp = NULL;
		 return;
	  }
	  fclose(fp);
	  return;
	}
	fclose(fp);
}

void QR_resetLog()
{
	FILE *fp;
	fp = fopen("D:\\log\\QRR.log", "w");
	fclose(fp);
}

/*
#define QR_ADD_LOG(...)	memset(&QR_gm_SzLog, 0, sizeof(QR_gm_SzLog));\
						sprintf(QR_gm_SzLog, __VA_ARGS__);\
						QR_writeLog();

#define QR_RESET_LOG   QR_resetLog();
*/


QR_Reader::QR_Reader(HWCommon *pHWCommon, int nComPortNum, bool nDirectionPlus)
{
	pLogQRTest = getLogUnit("D:\\log\\QRTest.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_pHWCommon = pHWCommon;
	m_nPortNum	= nComPortNum;
	m_pCommPort = NULL;
	m_nDirection = (nDirectionPlus==true)?1:(-1);

//	if(m_nPortNum==3)
//		m_MakerType = QR_MAKER_RIGHT;
//	else
//		m_MakerType = QR_MAKER_LEFT;
	m_MakerType = QR_MAKER_DM50;

	m_nLatestID = 0;
	m_dLatestOffset = 0.0;
	m_pnResultID=NULL;
	m_pdResultOffset=NULL;

	m_bQRDatasync = false;
	m_bflag = false;
	m_QRDatasyncIndex = 0;
	m_QRDataSynctimeout = timeGetTime();

	m_nStQRID = 0;
	m_dstQROffset = 0.0;
	m_bStaticQRData= false;

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
QR_Reader::~QR_Reader()
{
	Disable();
}


int QR_Reader::SetQRMaker(int nMakerType)
{
	m_MakerType = nMakerType;
    return 0;
}

/**
@brief	 QR Code�� �д� �Լ�
@author  ���¿�
@date	 2012.12.10.   2012.12.10.
@return
*/
int QR_Reader::GetIDnOffset(int *pnID, double *pdOffset)
{
	DWORD dwLen = 0;
	char szReadBuff[1024] = {0,};
	char szResult[1024] = {0,};
	int nTmpError = NO_ERR;
	int nID = 0;
	double dOffset = 0.0;


	m_nLatestID = 0;
	m_dLatestOffset = 0.0;

	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = 0;
	}

	if(m_pdResultOffset!=NULL)
	{
		*m_pdResultOffset = 0;
	}

	*pnID = 0;
	*pdOffset =0.0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_QRR_OPEN;
	}

	// dwLen = m_pCommPort->ReadUntil(szReadBuff, LF);
	dwLen = m_pCommPort->ReadUntil(szReadBuff, ETX);		// KEYENCE
	// dwLen = m_pCommPort->ReadUntil(szReadBuff, 0x03);	// OPTICON
//	ADD_LOG_QRTEST("QR Test %d", dwLen);
	if((dwLen != 0) &&((m_MakerType==QR_MAKER_OMRON) ||(m_MakerType==QR_MAKER_DM50) || (m_MakerType==QR_MAKER_MS)))
	{
		ADD_LOG_QRTEST("%s", szReadBuff);
	}

	if(dwLen != 0)
	{
		if(((szReadBuff[0]==STX) && (szReadBuff[dwLen-1]==ETX)) ||
		  // ((szReadBuff[0]==0x30) && (szReadBuff[dwLen-1]==0x0D)) ||
		   ((szReadBuff[0]==0x02) && (szReadBuff[dwLen-1]==0x03)) ||
		   ((szReadBuff[0]>='A') &&(szReadBuff[0]<='Z')))
		{
			//pass(���������� ���� Case)
			ADD_LOG_QRTEST("QRR Test 1");
		}
		else
		{
			if((szReadBuff[0]==STX) || (szReadBuff[0]==0x02) || ((szReadBuff[0]>='A') &&(szReadBuff[0]<='Z')))
			{
				memset(m_szTempReadBuff, 0x00, sizeof(m_szTempReadBuff));
				strcpy(m_szTempReadBuff, &szReadBuff[0]);
				ADD_LOG_QRTEST("QRR Test 2");
			}
			else if((szReadBuff[dwLen-1]==ETX) || (szReadBuff[dwLen-1]==0x03))
			{
				memmove(m_szTempReadBuff + strlen(m_szTempReadBuff), szReadBuff, dwLen);

				memmove(szReadBuff, m_szTempReadBuff, sizeof(m_szTempReadBuff));

				dwLen = strlen(szReadBuff);

				memset(m_szTempReadBuff, 0x00, sizeof(m_szTempReadBuff));
				ADD_LOG_QRTEST("QRR Test 3");
			}
			else
			{
				memmove(m_szTempReadBuff + strlen(m_szTempReadBuff), szReadBuff, dwLen);
				ADD_LOG_QRTEST("QRR Test 4");
			}
		}

		if(((szReadBuff[0]==STX) && (szReadBuff[dwLen-1]==ETX)) ||
		  // ((szReadBuff[0]==0x30) && (szReadBuff[dwLen-1]==0x0D)) ||
		   ((szReadBuff[0]==0x02) && (szReadBuff[dwLen-1]==0x03)) ||
		   ((szReadBuff[0]>='A') &&(szReadBuff[0]<='Z')))
		{
			szReadBuff[dwLen-1]=0;	// ETX ����

//			if(szReadBuff[0]!=0x02)
//			{
//				szReadBuff[dwLen-2]=0; //OPTICON�� ������ ; ����
//			}
			if(m_bQRDatasync ==true)
			{
				m_QRDatasyncIndex--;
				if(m_QRDatasyncIndex < 0)
					m_QRDatasyncIndex = 0;
			}

			if(m_MakerType==QR_MAKER_OMRON)
			{
				strcpy(szResult, &szReadBuff[0]);   // No header for OMRON
			}
			else
			{
				strcpy(szResult, &szReadBuff[1]);   // Header ����
			}

			nTmpError = ParseData(szResult ,&nID,&dOffset);
			ADD_LOG_QRTEST("QR ParseData %d, %s %d %d", nTmpError, szResult, nID,dOffset);

			//Parsing �� QR Data�� ERROR�� �߻��ϸ� Erroró��. puting 2015.07.02
			if(nTmpError != NO_ERR)
			{
				nTmpError = ERR_QRR_READ;				  //Read Fail
				//Ű�ؽ� ��ü�� Ȯ�� ��� Error�߻� ����
				//Case 1. AutoID Navigator���� Tigger type�� One-shot�ϰ��, Duration ���� �������Ѱ��
				//        (Timeout �߻����� �ʵ��� mark ��ƾ���� ó��)
				//case 2. AutoID Navigator���� Option���� PreString ������ �Ǿ��ִ� �ش� ���ڰ� ���� �ʴ� ���
				nID = 0;
				dOffset = 0;
			}
			if(nTmpError == NO_ERR)
			{
				m_bflag = true;
				ADD_LOG_QRTEST("m_bflag");
			}

		}
		else
		{
			// �������ݿ� ���� �� ����
			// szReadBuff[dwLen-2]=0;
			//QR_ADD_LOG("QRR In Format [%s][%x:%x]Len:%d",szReadBuff, szReadBuff[0] ,szReadBuff[dwLen-1], dwLen);
			// QR_ADD_LOG("QRR Format Error Data : [%s]", szReadBuff);

			nTmpError = ERR_QRR_DATAFORMAT;
			nID = 0;
			dOffset = 0.0;
		}
	}

	if(m_bStaticQRData == true)
	{
		if(nID !=0)
		{
			m_nStQRID =  nID;
			m_dstQROffset = dOffset;

        }
	}else
	{
		m_nStQRID = 0;
        m_dstQROffset = 0.0;
    }

	m_nLatestID = nID;
	m_dLatestOffset = dOffset;

	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = nID;
	}

	if(m_pdResultOffset!=NULL)
	{
		*m_pdResultOffset = dOffset;
	}

	*pnID = nID;
	*pdOffset = dOffset;

	return	nTmpError;
}

/**
@brief   ���� Data�� Ȯ����
@author ���¿�
@date    2013.07.02
@return Data Ȯ�� �� ���� �ݳ�
*/
int QR_Reader::ReadBuffer()
{
	return GetIDnOffset(&m_nLatestID, &m_dLatestOffset);
}

/**
@brief   ���ŵ� ID�� ����
@author ���¿�
@date    2013.07.02
@return ���ŵ� ID�� �ݳ�
*/
int QR_Reader::GetLatestID()
{
	return m_nLatestID;
}

/**@brief   ��� ���� ���� ID�� ����@author ���¿�@date    2013.07.02@return ���� �߻� �� ���� �߻� */
int QR_Reader::SetResultPointer(int* pnResultID, ...)
{
	double* pdResultOffset;
	va_list ap;
	va_start(ap, pnResultID);

	pdResultOffset = va_arg (ap, double*);

	m_pnResultID = pnResultID;
	m_pdResultOffset = pdResultOffset;

	va_end (ap);

	return 0;
}

/**
@brief   ���ŵ� Offset�� ����
@author ���¿�
@date    2013.07.02
@return ���ŵ� Offset�� �ݳ�
*/
double QR_Reader::GetLatestOffset()
{
	return m_dLatestOffset;
}

/**
@brief	 QR Trigger On �Լ�
@author  ���¿�
@date
@return
*/
int QR_Reader::SendTriggerOn()
{
	char strMsg[1024] = {0,};

	int nTmpError=0;

	m_nLatestID = 0;
	m_dLatestOffset = 0.0;
	if(m_pnResultID!=NULL)
	{
		*m_pnResultID = 0;
	}
	if(m_pdResultOffset!=NULL)
	{
		*m_pdResultOffset = 0;
	}

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_QRR_OPEN;
	}

	if(nTmpError==NO_ERR && m_pCommPort!=NULL)
	{
//		sprintf(strMsg, "%s%c%c",KEIENCE_QRR_TRIGGER, CR, LF);
		sprintf(strMsg, "%c%c%c",STX, COMMON_QRR_TRIGGER, ETX);

		if(m_bQRDatasync ==true)
		{
			m_QRDatasyncIndex++;
			m_QRDataSynctimeout = timeGetTime();
		}

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			nTmpError=ERR_QRR_WRITE;
		}
	}
	return nTmpError;
}

/**
@brief	 QR Trigger On �Լ�
@author  ���¿�
@date
@return
*/
int QR_Reader::SendTriggerOff()
{
	char strMsg[1024] = {0,};

	int nTmpError=0;

	if(m_pCommPort==NULL)
	{
		nTmpError = ReStart();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_QRR_OPEN;
	}

	if(nTmpError==NO_ERR && m_pCommPort!=NULL)
	{
//		sprintf(strMsg, "%s%c%c",KEIENCE_QRR_TRIGGER, CR, LF);
		sprintf(strMsg, "%c%c%c",STX, COMMON_QRR_OFF, ETX);
		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			nTmpError=ERR_QRR_WRITE; //AOHC-261 QR Trigger Fail�� QRR WRITE�� ���� #Jrespect.im 2019.08.05
			ADD_LOG_QRTEST("[SendTriggerOff] UnValid Writemsg");
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
int QR_Reader::ParseData(char* szData, int *nID, double *dOffset)
{
	//12345B;;+0182,+0098;

	//777777:125ms:171/303
	int nTmpError=NO_ERR;
	char szTemp[QR_RESPONCE_MAX_LENGTH]={0,};
	int nLen = strlen(szData);
	int nIndexOfStart=-1, nIndexOfEnd=-1;
	int nOffset = 0;
	double dTempOffset = 0.0;

	// QR_MAKER	MakerType	= QR_MAKER_KEYENCE;
	double		dDistRatio	= QR_DISTANCE_RATIO_OF_KEYENCE;
	bool		bMeetNoneZero	= false;	// OPTICON
	int			nMeetNoneZero	= 0;		// OPTICON
	int			nDirection		= 1;		// OPTICON

	//------------------------------------------------------------------------------
	//2	1. ID�� �޾ƿ´�.  ó�� ���� :�� ������ ��ġ ��
	for(int n=0; n<nLen; n++)
	{
		if(	(szData[n]>='0' && szData[n]<='9') ||
			(szData[n]>='A' && szData[n]<='F') ||
			(szData[n]>='a' && szData[n]<='f') )
		{
			szTemp[n] = szData[n];
		}
		else
		{
			// if(szData[n]!=':')	// only KEYENCE
			// if(szData[n]!=':' && szData[n]!=';')	//Both KEYENCE and OPTICON
			// {
			// 		nTmpError = ERR_QRR_DATAFORMAT;
			// 		QR_ADD_LOG("QRR[%s:%s]- ParseData 1 [%c]",szData, szTemp, szData[n]);
			// }
			ADD_LOG_QRTEST("ParseData 1 Error [%s:%s], [%c]",szData, szTemp, szData[n]);
			switch(szData[n])
			{
			case ':':
				if(m_MakerType==QR_MAKER_KEYENCE)
				{
					nDirection	= 1;
					dDistRatio	= QR_DISTANCE_RATIO_OF_KEYENCE;
				}
				else if (m_MakerType==QR_MAKER_OMRON)
				{
					nDirection	= -1;   //�ӽ÷� ����  -1�� ���� �ʿ� puting 20160314

					dDistRatio	= QR_DISTANCE_RATIO_OF_OMRON;
				}
				else if(m_MakerType==QR_MAKER_MS)
				{
					nDirection	= 1;
					dDistRatio	= QR_DISTANCE_RATIO_OF_MS;
				}
				else if(m_MakerType==QR_MAKER_DM50)
				{
					nDirection	= 1;

					if(m_nPortNum==3)
						dDistRatio	= QR_DISTANCE_RATIO_OF_DM50_R;
					else
						dDistRatio	= QR_DISTANCE_RATIO_OF_DM50;
				}
				break;

			case ',':  //m_MakerType==QR_MAKER_DM50)
					nDirection	= -1;
					dDistRatio	= QR_DISTANCE_RATIO_OF_DM50;
				break;
			case ';':
				dDistRatio	= QR_DISTANCE_RATIO_OF_OPTICON;
				nDirection	= -1;
				break;

			default:
				nTmpError	= ERR_QRR_DATAFORMAT;
				// QR_ADD_LOG("ParseData 1 Error [%s:%s], [%c]",szData, szTemp, szData[n]);
				break;
			}
			break;
		}
	}


	if(nTmpError==NO_ERR)
	{
		*nID = 	atoi(szTemp);//(int)strtol( szTemp, NULL, 16); // hex ó��

	}

	//2.1.1 ���� Data�� "ERROR"���� Ȯ���Ѵ�.
	 if(strstr(szTemp, "E") != NULL)
	 {
		// QR_ADD_LOG("ParseData 2 Error [%s:%s]", szData, szTemp);
		return ERR_QRR_READ; //Error ó��
	 }
	//------------------------------------------------------------------------------
	//2	2. Offset�� �޾� �´�
	memset(&szTemp, 0, sizeof(szTemp));

	// Offset�� ������ ���� �޾��ش�.
	for(int n=nLen-1; n>=0; n--)
	{
		// if(szData[n]=='/')	// only KEYENCE
		if(szData[n]=='/' || szData[n]==',')	//Both KEYENCE and OPTICON
		{
			nIndexOfEnd = n;
			break;
		}
	}

	//2	Offset�� ���� ���� �޾��ش�.
	for(int n=nIndexOfEnd-1; n>=0; n--)
	{
		// if(szData[n]==':')	// only KEYENCE
		if(szData[n]==':' || szData[n]==';' || szData[n]==',')	//Both KEYENCE and OPTICON
		{
			nIndexOfStart = n;

			break;
		}
	}

	if((nIndexOfStart != -1)&&(nIndexOfEnd !=-1))
	{
        ADD_LOG_QRTEST("nIndex Check [%d %d]",nIndexOfStart, nIndexOfEnd);
		for(int n=nIndexOfStart+1; n<nIndexOfEnd; n++)
		{
			ADD_LOG_QRTEST("nIndex Check 2 [%c]", szData[n]);
			if((szData[n]>='0' && szData[n]<='9') || (szData[n] =='.'))
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
			else
			{
				if(szData[n]=='+' && (n==(nIndexOfStart+1)))//for OPTICON's '+'
				{
					szData[n]=' ';
				}
				else
				{
					nTmpError = ERR_QRR_DATAFORMAT;
					// QR_ADD_LOG("ParseData 3 Error [%s:%s], [%c]",szData, szTemp, szData[n]);
					break;
				}
			}
		}
	}
	else
	{
		nTmpError =ERR_QRR_DATAFORMAT;
	}



	if(nTmpError==NO_ERR)
	{
		if(m_MakerType!=QR_MAKER_MS)
		{
			nOffset = atoi(szTemp);	// int ó��
		}else
          	dTempOffset = atof(szTemp);	// int ó��
		// QR_ADD_LOG("QRR[%s/%d], index[%d/%d]",szTemp, nOffset, nIndexOfStart, nIndexOfEnd);
	}

	if(nTmpError!=NO_ERR)
	{
		*nID = 0;
		nOffset = 0;
	}



	 if(((nOffset < 999) && ( nOffset > 0)) || ((dTempOffset < 999) && ( dTempOffset > 0.0)))
	 {
		if(m_MakerType==QR_MAKER_OMRON)
		{
			*dOffset = (double)(((nOffset/dDistRatio) - QR_DISTANCE_CENTER_OF_OMRON)*m_nDirection*nDirection);
		}
		else if(m_MakerType==QR_MAKER_DM50)
		{
			if(m_nPortNum==3)
				*dOffset = (double)(((nOffset/dDistRatio) - QR_DISTANCE_CENTER_OF_DM50_R)*m_nDirection*nDirection);
			else
				*dOffset = (double)(((nOffset/dDistRatio) - QR_DISTANCE_CENTER_OF_DM50)*m_nDirection*nDirection);
		}
		else if(m_MakerType==QR_MAKER_MS)
		{
			*dOffset = (double)(((dTempOffset/dDistRatio) - QR_DISTANCE_CENTER_OF_MS)*m_nDirection*nDirection);
        }
		else
		{
			*dOffset = (double)(((nOffset/dDistRatio) - QR_DISTANCE_CENTER)*m_nDirection*nDirection);
			// QR_ADD_LOG("QRR[%X/%d:%8.1lf]",*nID, nOffset, *dOffset);
		}
	 }
	 else
	 {
		 nTmpError = ERR_QRR_DATAFORMAT;
	 }

	return nTmpError;
}


/**
@brief	 QR_Reader �ø��� ����� open�ϴ� �Լ�
@author  ���¿�
@date	 2012.12.10.
@return
*/
int QR_Reader::Enable()
{
	int nError=NO_ERR;

	if(m_pCommPort==NULL)
	{
//		m_pCommPort = new Comm_Port(m_nPortNum, QR_BAUDRATE, 8 ,ODDPARITY, ONESTOPBIT );
		m_pCommPort = new Comm_Port(m_nPortNum, QR_BAUDRATE, 8 ,NOPARITY, ONESTOPBIT );	// Opticon
		 // NOPARITY, ODDPARITY, EVENPARITY  // ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
		if(m_pCommPort==NULL)
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
void QR_Reader::Disable()
{
	if(m_pCommPort!=NULL)
	{
		delete m_pCommPort;
		m_pCommPort = NULL;
	}
//	m_pCommPort->Close();
}

int QR_Reader::ReStart()
{
	if(m_pCommPort!=NULL)
	{
		Disable();	// ����� ���� �� �����ϰ� �翬��
	}

	return Enable();
}
//QR Data ����ȭ�� ���� �κ�
void QR_Reader::SetQRDataSync(bool bsync)
{
	if(m_bQRDatasync != bsync)
		m_QRDatasyncIndex = 0;

	m_bQRDatasync = bsync;
	m_QRDataSynctimeout = timeGetTime();

}
bool QR_Reader::IsQRDataSync()
{
	return m_bQRDatasync;
}
int  QR_Reader::GetQRDataSync()
{
	//sync ����� ���, return�� 0�� ��츸 trigger ��� �ټ��ְ� ó���Ұ�.
//    int nIndex = -1; // -1�� ���� ó��.
	// m_QRDatasyncIndex�� 0�� ��츸 TriggerON ��� ó�� ����.
	if(m_bQRDatasync == true)
	{
		if(m_QRDatasyncIndex !=0)
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(m_QRDataSynctimeout) > 300)
			{
				if(m_QRDatasyncIndex == 1)
				{
					SendTriggerOn();

				}
				else if(m_QRDatasyncIndex > 1)
				{
					m_QRDatasyncIndex++; //3�� ��� ���� ó��.
				}
			}
		}else
			m_QRDataSynctimeout = timeGetTime();

		return m_QRDatasyncIndex;

	}else
		return 0;
}


///static QRData Ȯ���� ���� �κ�(AUto Teaching ��, �޴��� ���¿��� ���)
void QR_Reader::SetStaticQRDataTrigger(bool bsync)
{
	m_nStQRID = 0;
	m_dstQROffset = 0.0;

	m_bStaticQRData = bsync;

	if(m_bStaticQRData ==true)
	{
     	SendTriggerOn();
    }
}
int QR_Reader::GetStaticQRID()
{
	return m_nStQRID;
}
double QR_Reader::GetStaticQROffset()
{
	return m_dstQROffset;
}

bool QR_Reader::Connect()
{
	if(m_bflag == true)	return true;
	else	return false;

}


