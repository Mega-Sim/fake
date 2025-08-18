#pragma hdrstop

#include "VMS.h"
#include "OHTMain.h"
#include "Utility.h"

#define ADD_LOG_VMS(...)    WRITE_LOG_UNIT(pLogTest, __VA_ARGS__)


#define	CR					0x0D
#define	LF					0x0A

/**
@brief   VMS Class ������
@author  kyh
@date    2017.12.12
@param   pHWCommon : HWCommon Ŭ���� ������
*/

VMS::VMS()
{
	pLogTest = getLogUnit("D:\\log\\VMS.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();

	m_pCommPort = NULL;
	bConnect = false;
	bVMSConnected = false;

	OpenCom();
}


/**
@brief   VMS Class �Ҹ���
@author  kyh
@date    2017.12.12
*/
VMS::~VMS()
{
	CloseCom();
}


/**
@brief   VMS Comm Port�� Open�ϴ� �Լ�
@author  kyh
@date    2017.12.12
@return  Open �ϴµ� ���� ������ 0, ������ �߻��ϸ� ERR_OPEN ����
*/
int VMS::OpenCom()
{
	int nError = 0;

	if(m_pCommPort==NULL)
	{
		m_pCommPort = new Comm_Port(20, 115200, 8 , NOPARITY, 1);
		//m_pCommPort = new Comm_Port(20, 115200);
		if(m_pCommPort==NULL)
		{
			nError = ERR_VMS_OPEN;
		}
		else
		{
			bConnect = true;
		}
	}
	else if(m_pCommPort!=NULL)
	{
		nError = ERR_VMS_OPEN;
		bConnect = false;
	}
	return nError;
}


/**
@brief   VMS ���� ������ üũ�ϴ� �Լ�
@author  kyh
@date    2017.12.15
@return  VMS�� ������ ��� True, �������� ��� False�� Return
*/
bool VMS::IsConnect()
{
	return bConnect;
}


/**
@brief   VMS Comm Port�� Close�ϴ� �Լ�
@author  kyh
@date    2017.12.15
*/
int VMS::CloseCom()
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
@brief   VMS Comm Port�� ReOpen�ϴ� �Լ�
@author  kyh
@date    2017.12.15
*/
int VMS::ReOpenCom()
{
	if(m_pCommPort!=NULL)
	{
		CloseCom();	// ����� ���� �� �����ϰ� �翬��
	}
	return OpenCom();
}

int VMS::SetMode()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_BTMODESET, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			Error = ERR_VMS_WRITE;
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}
			ADD_LOG_VMS("[SetMode]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::SetMultiMode()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_MULTIMODESET, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			Error = ERR_VMS_WRITE;
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}
			ADD_LOG_VMS("[SetMultiMode]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::Connect(AnsiString strBTID)
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%s%c%c", BT_CMD_CONNECT, strBTID, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
//		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("Connect Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(2000);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}
			ADD_LOG_VMS("[Connect:%s]:%s Length:%d Error:%d", strBTID, strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::SetOnline()
{
	char strMsg[1024] = {0,};
	int Error = 0;

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_ONLINE, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("SetOnline Fail[E:%d]", Error);
		}
	}

	return Error;
}

int VMS::Disconnect()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_DISCONNECT, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
		//if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("Disconnect Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}

			ADD_LOG_VMS("[Disconnect]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::SetWait()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_WAIT, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
		//if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("SetWait Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}

			ADD_LOG_VMS("[SetWait]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::GetBTID()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_IDCHECK, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
		//if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("GetBTID Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}

			ADD_LOG_VMS("[GetBTID]:%s Length:%d", strLog, dwLen);
		}
		else
		{
			Error = ERR_VMS_READTIMEOUT;
			ADD_LOG_VMS("GetBTID Fail[E:%d]", Error);
		}
	}
}

int VMS::SetSWReset()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_SWRESET, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
		//if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("SetSWReset Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}

			ADD_LOG_VMS("[SetSWReset]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::SetHWReset()
{
	char strMsg[1024] = {0,};
	char szReadBuff[1024] = {0,};
	DWORD dwLen = 0;
	int nLength_Compare = 0;
	int Error = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		sprintf(strMsg, "%s%c%c", BT_CMD_HWRESET, CR, LF);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg));
		//if(strlen(strMsg) != m_pCommPort->WriteCommBlock(strMsg, strlen(strMsg)) )
		if(strlen(strMsg) != nLength_Compare )
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("SetHWReset Fail[E:%d] Length(%d, %d)", Error, strlen(strMsg), nLength_Compare);
		}
	}

	Sleep(500);

	if(Error==NO_ERR)
	{
		dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
		if(dwLen>0)
		{
			for(int i = 0; i < dwLen; i++)
			{
				strTmp.sprintf("%c", szReadBuff[i]);
				strLog = strLog + strTmp;
			}

			ADD_LOG_VMS("[SetHWReset]:%s Length:%d Error:%d", strLog, dwLen, Error);
		}
	}

	return Error;
}

int VMS::SendData2VMS(unsigned char* SendData, int Length)
{
	unsigned char strMsg[1024] = {0,};
	int Error = 0;
	int nLength_Compare = 0;
	AnsiString strLog = "", strTmp = "";

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	if(Error==NO_ERR && m_pCommPort!=NULL)
	{
		memcpy(strMsg, SendData, Length);

		for(int i = 0; i < Length; i++)
		{
			strTmp.sprintf("%c", strMsg[i]);
			strLog = strLog + strTmp;
		}
		ADD_LOG_VMS("[SendData]:%s Length:%d", strLog, Length);

		PurgeComm(m_pCommPort->m_hCOM , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		nLength_Compare = m_pCommPort->WriteCommBlock(strMsg, Length);

		//if(Length != m_pCommPort->WriteCommBlock(strMsg, Length))
		if(Length != nLength_Compare)
		{
			Error = ERR_VMS_WRITE;
			ADD_LOG_VMS("SendData Fail[E:%d] Length(%d, %d)", Error, Length, nLength_Compare);
		}
	}

	return Error;
}


/**
@brief   VMS Read ����� �ְ� �׿� ���� ������ �޴� �Լ�
@author  yunho.kim
@date    2017.12.24
@param   nID : ���� ��� ���� �����ϴ� ����, ���� �� 0 ����
@return  Read�ϴµ� ���� ������ 0
*/
int VMS::GetData()//PKT_OCS2OHT& OcsCmdPkt)
{
	int Error = NO_ERR;
	DWORD dwLen = 0;
	int nReadBuffLength = 0;
	char szReadBuff[1024] = {0,};
	AnsiString strLog = "", strTmp = "";

	int iPacketSize = 0;

	if(m_pCommPort==NULL)
	{
		Error = ReOpenCom();
	}

	if(m_pCommPort==NULL)
	{
		return ERR_VMS_OPEN;
	}

	dwLen = m_pCommPort->ReadUntil(szReadBuff, NULL, false);
	if(dwLen>0)
	{
		for(int i = 0; i < dwLen; i++)
		{
			strTmp.sprintf("%c", szReadBuff[i]);
			strLog = strLog + strTmp;
		}

		ADD_LOG_VMS("[GetData]:%s Length:%d Error:%d", strLog, dwLen, Error);

		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(dwLen < iPacketSize)
		{
//			ADD_LOG_VMS("[GetData] ������Ŷ�� ũ�Ⱑ �ؼ��ϱ⿡ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d ",
//						 iPacketSize, dwLen);

			ADD_LOG_VMS("[GetData] ����size : %d ", dwLen);
			if(dwLen < 9)
			{
                ADD_LOG_VMS("[GetData] ���� �Ϸ� ó��");
				bVMSConnected = true;
			}
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &szReadBuff, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�

		Conv_PKT_CHECK_TAG(Pkt);

		if(Pkt.HEADER.MachineType == MACHINETYPE_VMS)
		{
			switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
			{
				case TagID::CMD_CONNECT_VMS_OHT_RESPONSE:
				{
					iPacketSize = sizeof(PKT_OCS2OHT);
					//memcpy(&OcsCmdPkt, &szReadBuff, iPacketSize);
					bVMSConnected = true;
					ADD_LOG_VMS("[GetData] VMS Tag ID : %d", Pkt.TAG.Value);
				}

				default:
				{
					bVMSConnected = true;
					ADD_LOG_VMS("[GetData] Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�.  Tag ID : %d", Pkt.TAG.Value);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;
			}
		}
	}
	else
	{
		ADD_LOG_VMS("[GetData] Length:%d", dwLen);
		Error = 0;
	}
	return Error;
}

bool VMS::VMSConnected()
{
	return bVMSConnected;
}

void VMS::SetVMSDisconnect()
{
	bVMSConnected = false;
}

void VMS::SuperviseSendCmdDataOHT2VMS()
{
	static DWORD dwCheckVMSSendTime = timeGetTime();
	static DWORD dwCheckVMSTimeout = timeGetTime();
	int nResult = -1;
	char strMsg[1024] = {0,};
	int nLength = 0, nOffset = 0;
	static int nRetryCount = 0;
	static int nErrorCount = 0;
	static bool bAnotherBTConnect = false;

	if(m_defualtparam->bUseVMS == true)
	{
		if(m_pOHTMainStatus->nStatusVMS == BT_CONNECT)
		{
			if(bAnotherBTConnect == false)
				nResult = Connect(m_pOHTMainStatus->strVMSBTId_1);
			else
				nResult = Connect(m_pOHTMainStatus->strVMSBTId_2);

			if(nResult)
			{
             	m_pOHTMainStatus->nStatusVMS = NONE;
			}
			else
			{
				m_pOHTMainStatus->nStatusVMS = CONNECT;
			}
			ADD_LOG_VMS("[VMS] BlueTooth Connect(%d)", nResult);
		}
		else if(m_pOHTMainStatus->nStatusVMS == CONNECT)
		{
			Sleep(1000);
			// ��� ����
			PKT_CHECK_TAG SendData;

			SendData.HEADER.MachineType = MACHINETYPE_VHL;
			memmove(SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));

			memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
			nOffset = nOffset + sizeof(SendData.HEADER);

			SendData.TAG.Value = TagID::CMD_CONNECT_OHT_VMS;

			memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
			nOffset = nOffset + sizeof(SendData.TAG.Value);

			nLength = nOffset;

			SendData2VMS(strMsg, nLength);

			m_pOHTMainStatus->nStatusVMS = CONNECT_CHECK;
			dwCheckVMSTimeout = timeGetTime();
			//m_bVMSConnecting = true;

			ADD_LOG_VMS("[VMS] Connect Try - %d", nRetryCount);
		}
		else if(m_pOHTMainStatus->nStatusVMS == CONNECT_CHECK)
		{
			Sleep(1000);
			nResult = GetData();
			ADD_LOG_VMS("[VMS] Connect Check %d", nResult);
			if(VMSConnected())
			{
				m_pOHTMainStatus->nStatusVMS = CONNECTING;
				bAnotherBTConnect = false;
				nRetryCount=0;
				Sleep(1000);
			}
			else
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckVMSTimeout) > 5000)
				{
					m_pOHTMainStatus->nStatusVMS = CONNECT;

					if(nRetryCount > 5)
					{
						nRetryCount=0;

						if(bAnotherBTConnect == false)
						{
							bAnotherBTConnect = true;
							m_pOHTMainStatus->nStatusVMS = BT_CONNECT;
							ADD_LOG_VMS("[VMS] Another BT Connect Try");
						}
						else
						{
							bAnotherBTConnect = false;
							m_pOHTMainStatus->nStatusVMS = DISCONNECTED;
							ADD_LOG_VMS("[VMS] All BT Connect Fail");
						}
					}
					else
						nRetryCount++;
				}
			}
		}
		else if(m_pOHTMainStatus->nStatusVMS == CONNECTING)
		{
			if(VMSConnected())
			{
				m_pOHTMainStatus->nStatusVMS = CONNECTED;
				//m_bVMSConnecting = false;
				ADD_LOG_VMS("[VMS] Connect OK");
			}
			else if(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckVMSTimeout) > 5000)
			{
				m_pOHTMainStatus->nStatusVMS = BT_CONNECT;
				//m_bVMSConnecting = false;
				ADD_LOG_VMS("[VMS] Connect Timeout");
            }
        }
		else if(m_pOHTMainStatus->nStatusVMS == CONNECTED)
		{
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckVMSSendTime) > 200)
			{
                //Response List�� ����� ����ü (���� �޸� �Ҵ�)
				PKT_OHT2VMS_RESPONSE SendData;

				// Step 1. Header ����  ����
				SendData.HEADER.MachineType = MACHINETYPE_VHL; // OHT�� ����
				memmove(&SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));

				memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
				nOffset = nOffset + sizeof(SendData.HEADER);

				// Step 2. Tag ����  ����
				SendData.TAG.Value = TagID::CMD_STATUS_OHT_VMS;

				memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
				nOffset = nOffset + sizeof(SendData.TAG.Value);

				// Step 3. OHT status Data ����
				memmove(&SendData.OHT_STATUS, &m_pOHTMainStatus->StatusVMS, sizeof(MSG_STATUS_OHT2VMS));

				memmove(strMsg + nOffset, &SendData.OHT_STATUS, sizeof(MSG_STATUS_OHT2VMS));
				nOffset = nOffset + sizeof(SendData.OHT_STATUS);

				nLength = strlen(strMsg);
				nLength = nOffset;

				nResult = SendData2VMS(strMsg, nLength);

				if(nResult != NO_ERR)
					nErrorCount++;
				else
					nErrorCount = 0;

				if(nErrorCount > 5)
				{
					m_pOHTMainStatus->nStatusVMS = BT_CONNECT;
					//m_bVMSConnecting = false;
					nErrorCount = 0;
				}

				dwCheckVMSSendTime = timeGetTime();

				GetData();
			}
		}
		else if(m_pOHTMainStatus->nStatusVMS == DISCONNECT)
		{
			// ��� ���� ����
			// Step 1. Header ����  ����
			PKT_CHECK_TAG SendData;

			memmove(SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));
			SendData.HEADER.MachineType = MACHINETYPE_VHL;

			memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
			nOffset = nOffset + sizeof(SendData.HEADER);

			SendData.TAG.Value = TagID::CMD_DISCONNECT_OHT_VMS;

			memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
			nOffset = nOffset + sizeof(SendData.TAG.Value);

			nLength = nOffset;

			SendData2VMS(strMsg, nLength);

			m_pOHTMainStatus->nStatusVMS = DISCONNECTED;

			SetVMSDisconnect();

			ADD_LOG_VMS("[VMS] DisConnect");
			nErrorCount = 0;
		}
		else if(m_pOHTMainStatus->nStatusVMS == DISCONNECTED)
		{
			nResult = SetWait();
			ADD_LOG_VMS("[VMS] BlueTooth Set Wait(%d)", nResult);

			Sleep(200);

			nResult = Disconnect();
			m_pOHTMainStatus->nStatusVMS = BT_SETRESET;
			ADD_LOG_VMS("[VMS] BlueTooth Disconnect(%d)", nResult);
		}
		else if(m_pOHTMainStatus->nStatusVMS == BT_SETRESET)
		{
			nResult = SetSWReset();
			m_pOHTMainStatus->nStatusVMS = NONE;
			nRetryCount=0;
			ADD_LOG_VMS("[VMS] BlueTooth Set SW Reset (%d)", nResult);
		}
	}
}
