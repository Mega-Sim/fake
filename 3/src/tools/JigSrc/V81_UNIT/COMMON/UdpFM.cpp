//---------------------------------------------------------------------------

/*
	�� ����� ����ϱ� ���� ������Ʈ ������ �����ؾ� �ϴ� ��
	(������Ʈ ���� -> C++ Compiler -> Directories and Conditionals�� Conditional defines �� ����)

	[��ý��Ͻ��� ���]
	IS_AS			   : ��ý��Ͻ����� ��Ÿ����. �����ؾ� ��
	_IN_UDP_UPPER_FORM : UDP��ų����� ȭ�鿡 ǥ���ϴ� ���(frmUDP) ���� ����. ���⼭�� ������

	[���� �Ŵ����� ���]
	IS_FM 			   : ���ϸŴ��� ���� ��Ÿ����. �����ؾ� ��
	_IN_UDP_UPPER_FORM : UDP��ų����� ȭ�鿡 ǥ���ϴ� ���(frmUDP) ���� ����. ���⼭�� ������


*/

#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpFM.h"
#include "Protocol_Define_Modify.h"

// #include "SocketInitClose.h"
// #include "UdpServer.h"
// #include "UdpBroadCastSender.h"
// #include "UdpUniCastSender.h"

#include "PUtily.h"

#ifdef _IN_UDP_UPPER_FORM
	#include "Udp.h"
#endif
//---------------------------------------------------------------------------

SocketInitClose		*pSocketInitClose;		// ������ �ʱ�ȭ ���

// [AS - FM]
UdpServer 			*pUdpUniCastServer;			// AS - FM ��� ����

// [COMMON]
UdpUniCastSender	*pUdpUniCastSender; 		// �ϴ��� ��� ����ó��

//---------------------------------------------------------------------------

UdpFM::UdpFM()
{
	pSocketInitClose = NULL;
	pUdpUniCastServer = NULL;
	pUdpUniCastSender = NULL;

	if (pSocketInitClose == NULL)
		pSocketInitClose = new SocketInitClose();

	if (pUdpUniCastSender == NULL)
		pUdpUniCastSender = new UdpUniCastSender();

	// ���� �ʱ�ȭ �ϰ������� ����
	pSocketInitClose->SocketInit();

	// BIND �Ͽ� ��Ʈ��ȣ�� �Ҵ�޴´�
	AS_FILE_MANAGER_BIND();

}

//---------------------------------------------------------------------------
void UdpFM::DisplayLog(String strLog)
{
#ifdef _IN_UDP_UPPER_FORM
	frmUDP->DisplayLog(strLog); 	// ��� ����׸� ���� UDP�� �ȿ� �ִ� ���, ȭ�鿡 ǥ��
#endif
}
//---------------------------------------------------------------------------

UdpFM::~UdpFM()
{
	if (pSocketInitClose != NULL)
		delete(pSocketInitClose);

	if (pUdpUniCastSender != NULL)
		delete(pUdpUniCastSender);

	pSocketInitClose->SocketClose();

	AS_FILE_MANAGER_CLOSE();
}
//---------------------------------------------------------------------------
// OHT�� ��ý��Ͻ��� ���ż��� ���ε� ����
// �Լ����� �����ϰ�, ���� ��Ʈ��ȣ�� �ٸ���

int UdpFM::AS_FILE_MANAGER_BIND()
{
	int iRetVal = -1;

	if (pUdpUniCastServer == NULL)
		pUdpUniCastServer = new UdpServer();

#ifdef IS_AS
	iRetVal = pUdpUniCastServer->Bind(FM_2_AS_PORT);
#endif

#ifdef IS_FM
	iRetVal = pUdpUniCastServer->Bind(AS_2_FM_PORT);
#endif

	if(iRetVal == 0)
	{
		DisplayLog(L"[��ý��Ͻ� - ���ϸŴ��� ��Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[��ý��Ͻ� - ���ϸŴ��� ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[��ý��Ͻ� - ���ϸŴ��� ��Ʈ] Bind ���� ����");
	}

	return iRetVal;
}

void UdpFM::AS_FILE_MANAGER_CLOSE()
{
	if (pUdpUniCastServer != NULL)
	{
		pUdpUniCastServer->Close();
		delete(pUdpUniCastServer);
		pUdpUniCastServer = NULL;
	}
}

int UdpFM::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("���ǵ� �ִ� ��Ŷ ������� ū �޽��� �۽Žõ�!!");
		return UNICAST_SEND_RESULT_FAIL;
	}

	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d����Ʈ�� ���½��ϴ�.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);
#if 0
	// ---------- log add -------------
	strMsg = "";	// �ѹ� ��� �� �ݵ�� ����
	for (int i = 0; i < retval; i++)
	{
		oneChar = buf[i];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("%02X(%c) ", oneChar, oneChar);	// Hexǥ�÷� �ٲ�
		else
			strTmp.sprintf("%02X ", oneChar);

		// 10���� �� ������ �����ϰ� Ŭ���� �� �ش�
		if(((i % RAW_LOG_BYTE_1_LINE_SIZE) == 0) && (i != 0))
		{
			DisplayLog(strMsg);
			strMsg = "";
		}

		strMsg = strMsg + strTmp;
	}

	// �����ʹ� �� 10���� �������� ������ �����Ƿ� �������� ����Ʈ �Ѵ�
	if(strMsg != "")
		DisplayLog(strMsg);
	// -------------------------------------------------------------
#endif

	return retval;
}


#ifdef IS_AS
int UdpFM::UnicastSend_AS_2_FM(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , AS_2_FM_PORT, buf, len);
}

int UdpFM::UnicastSend_AS_2_OHT(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , OHT_AS__OHT_PORT_NUM, buf, len);
}

#endif


// ���ϸŴ��� => ��ý��Ͻ��� ��� (OHT -> OCS ǥ�� ��Ŷ ���)
#ifdef IS_AS
int UdpFM::FM_2_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�

		switch(Pkt.TAG.Value)
		{
			case TagID::RESPONSE_DATATRANSFER_OHT2OCS:			// ������ ���� ���� ����     --- UdpCom 1387 line.....?
			{
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("������ ���� ���� ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  �����Ұ� : %d  �����Ұ����� : %d  �������� ID : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:	// ������ ���� ��ô ����
			{
				PKT_DATA_TRANSPER_ORDER_PROGRESS CmdPkt;
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);

				strTmp.sprintf("������ ���� ��ô ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  ���۽��� : %d  ���ۼ��� : %d  ������ ���ۻ��±��� : %d  �������� ID : %d  ���۽��б��� : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
												CmdPkt.BODY.FailClass);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::CLW_TO_MCP_STATUS:						// CLW Status ���� (CLW -> MCP)
			{
				PKT_STATUS_REPORT_ID_03 CmdPkt;
				iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);

				strTmp.sprintf("CLW Status ���� (AS => OHT) Tag : %d  ������ �������� ID : %d  ������ ���� ���� : %d  ������ ���ۻ��� ���� : %d  Data ���� ���� : %d  Data ���� ���� : %d",
								CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
								CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::REQ_DATATRANSFER_OHT2OCS:				// ������ ���� �䱸
			{
				PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS CmdPkt;
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("������ ���� ��ô ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::CMD_INERNAL_PING_RESPONSE:			// ���� ���α׷��鳢�� ���� ������ �޾Ƽ� ���Ῡ�θ� Ȯ���Ѵ�
			{
				PKT_CHECK_TAG CmdPkt;
				iPacketSize = sizeof(PKT_CHECK_TAG);
			
				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("PING ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				memcpy(&CmdPkt, &buf, iPacketSize);
				//strTmp.sprintf("PING ��Ŷ (FM => AS) Tag : %d	���۱��� : %d  �䱸���� : %d",
				//				   CmdPkt.TAG.Value, CmdPkt.  CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				DisplayLog("���ϸŴ����� ��� �ֽ��ϴ�");

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

int UdpFM::AS_ThreadCall(char* AS_Pkt, int* piMachineType)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
		
		if(Pkt.HEADER.MachineType == MACHINETYPE_VHL_COMMON)
		{
			*piMachineType = MACHINETYPE_VHL_COMMON;
			switch(Pkt.TAG.Value)
			{
				case TagID::CMD_DATATRANSFER_OCS2OHT:			// ������ ���� ����
				{
					PKT_DATA_TRANSPER_ORDER_OCS2OHT CmdPkt;
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("������ ���� ���� (OHT => AS) Tag : %d  ���۱��� : %d  �䱸���� : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				break;


				case TagID::CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS:
				{
					PKT_RESTART_REQUEST_OHT2AS CmdPkt;
					iPacketSize = sizeof(PKT_RESTART_REQUEST_OHT2AS);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}				

					// --- Log ---
					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("Packet Received. OHT�� �ڽſ��� �������� �ְ�, ����� ���� �޶�� AS�� ��û�ϴ� ��Ŷ1");
					DisplayLog(strTmp);

					// return ready
					memcpy(AS_Pkt, &buf, iPacketSize);
					return iReturn;
				}
				break;

				case TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS:
				{
					PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS CmdPkt;
					iPacketSize = sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS);
				
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}				
				
					// --- Log ---
					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("Packet Received. OHT�� �ڽſ��� �������� �ְ�, ����� ���� �޶�� AS�� ��û�ϴ� ��Ŷ2");
					DisplayLog(strTmp);
				
					// return ready
					memcpy(AS_Pkt, &buf, iPacketSize);
					return iReturn;
				}
				break;

				
			}
		}
		else if(Pkt.HEADER.MachineType == MACHINETYPE_FM)
		{
			*piMachineType = MACHINETYPE_FM;
			switch(Pkt.TAG.Value)
			{
				case TagID::RESPONSE_DATATRANSFER_OHT2OCS:			// ������ ���� ���� ����     --- UdpCom 1387 line.....?
				{
					PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
					iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);
					strTmp.sprintf("������ ���� ���� ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  �����Ұ� : %d  �����Ұ����� : %d  �������� ID : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
													CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
   				break;



				case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:	// ������ ���� ��ô ����
				{
					PKT_DATA_TRANSPER_ORDER_PROGRESS CmdPkt;
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);

					strTmp.sprintf("������ ���� ��ô ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  ���۽��� : %d  ���ۼ��� : %d  ������ ���ۻ��±��� : %d  �������� ID : %d  ���۽��б��� : %d",
													CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
													CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
													CmdPkt.BODY.FailClass);
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				break;

				case TagID::CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT:	// ������� ������Ʈ �Ǵ� �������б� ���
				{
					PKT_FTP_PUT_GET_RESULT_AS2OHT CmdPkt;
					iPacketSize = sizeof(PKT_FTP_PUT_GET_RESULT_AS2OHT);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);

					strTmp.sprintf("������� ������Ʈ �Ǵ� �������б� ��� (AS => OHT) Tag : %d  Result Code : %d  Result Text : %s",
													CmdPkt.TAG.Value, CmdPkt.BODY.iResult, CmdPkt.BODY.ResultText);
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				break;

				case TagID::CLW_TO_MCP_STATUS:						// CLW Status ���� (CLW -> MCP)
				{
					PKT_STATUS_REPORT_ID_03 CmdPkt;
					iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);

					strTmp.sprintf("CLW Status ���� (AS => OHT) Tag : %d  ������ �������� ID : %d  ������ ���� ���� : %d  ������ ���ۻ��� ���� : %d  Data ���� ���� : %d  Data ���� ���� : %d",
									CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
									CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				break;
				
				case TagID::CMD_INERNAL_PING_RESPONSE:			// ���� ���α׷��鳢�� ���� ������ �޾Ƽ� ���Ῡ�θ� Ȯ���Ѵ�
				{
					PKT_CHECK_TAG CmdPkt;
					iPacketSize = sizeof(PKT_CHECK_TAG);
				
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("PING ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					memcpy(&CmdPkt, &buf, iPacketSize);
					//strTmp.sprintf("PING ��Ŷ (FM => AS) Tag : %d	���۱��� : %d  �䱸���� : %d",
					//				   CmdPkt.TAG.Value, CmdPkt.  CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
					DisplayLog("���ϸŴ����� ��� �ֽ��ϴ�");

					// �����Ͽ� ������ ����
					memcpy(AS_Pkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				break;
			}
		}
		else
		{
			return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}
		
		
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_FM
int UdpFM::UnicastSend_FM_2_AS(char* buf, int len)
{
	return UnicastSend(OHT_AS__LOCAL_IP , FM_2_AS_PORT, buf, len); 
}
#endif


#ifdef IS_FM
// ��ý��Ͻ� => ���ϸŴ��� (OCS -> OHT ǥ�� ��Ŷ ���)
int UdpFM::AS_2_FM_ThreadCall(PKT_OCS2OHT& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	AnsiString strConv, strChk;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	//strTmp.sprintf("iReturn : %d", iReturn);
	//DisplayLog(strTmp);

	if (iReturn > 0)
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�

		//strTmp.sprintf("Pkt.TAG.Value : %d", Pkt.TAG.Value);
		//DisplayLog(strTmp);

		switch(Pkt.TAG.Value)
		{
			case TagID::CMD_DATATRANSFER_OCS2OHT:		// ������ ��������
			{
				PKT_DATA_TRANSPER_ORDER_OCS2OHT CmdPkt;
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("Packet Received. ������ ��������");
				DisplayLog(strTmp);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);
				return iReturn;
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS:		// FTP
			{
				PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS CmdPkt;
				iPacketSize = sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("Packet Received. Wireless FTP");
				DisplayLog(strTmp);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);
				return iReturn;
			}
			break;

			case TagID::CMD_INERNAL_PING_REQ:			// ���� ���α׷��鳢�� ���� ������ �޾Ƽ� ���Ῡ�θ� Ȯ���Ѵ�
			{
				DisplayLog("��ý��Ͻ��� Ȯ�� ��Ŷ�� ���½��ϴ�");

				PKT_CHECK_TAG Response_Pkt;

				// auto response
				iPacketSize = sizeof(PKT_CHECK_TAG);
				memset(&Response_Pkt, 0, iPacketSize);

				Response_Pkt.HEADER.MachineType = MACHINETYPE_FM;
				Response_Pkt.TAG.Value = TagID::CMD_INERNAL_PING_RESPONSE;

				UnicastSend_FM_2_AS((char*)&Response_Pkt, iPacketSize);

				// return ready
				memcpy(&AS_Pkt, &buf, iPacketSize);

				return iReturn;
			}

		}

	}
	return iReturn;
}

#endif



