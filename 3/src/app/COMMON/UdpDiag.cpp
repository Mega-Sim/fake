/*
	�� ����� ����ϱ� ���� ������Ʈ ������ �����ؾ� �ϴ� ��
	(������Ʈ ���� -> C++ Compiler -> Directories and Conditionals�� Conditional defines �� ����)

	[OHT / OHT������ ���]
	_ONE_PC_TEST 		: 1PC���� ���� �׽�Ʈ�� ��쿡�� ���� (OHT, OHT����, OCS����)
	IS_OHT		 		: OHT, OHT ���ķ����ʹ� �ݵ�� �����Ѵ�
	IS_REAL_OHT			: ������ �ƴ� ��¥ OHT������ ����ϵ��� �ϴ� ��� 
	_UDP_MODULE_CHILD 	: OHT ���� / OCS ���ķ����͸� �����Ѵ�
	_IN_UDP_UPPER_FORM 	: OHT ���� / OCS���ķ����ʹ� �ݵ�� ����. ������ �󼼷α׸� �����ϴ� �����ϸ� OHT�� �����Ѵ�

	[OCS ���ķ������� ���]
	_ONE_PC_TEST 		: 1PC���� ���� �׽�Ʈ�� ��쿡�� ���� (OHT, OHT����, OCS����)
	IS_MCP				: OCS ���ķ����ʹ� �ݵ�� �����Ѵ�
	_UDP_MODULE_CHILD 	: OHT ���� / OCS ���ķ����͸� �����Ѵ�
	_IN_UDP_UPPER_FORM 	: OHT ���� / OCS���ķ����ʹ� �ݵ�� ����. ������ �󼼷α׸� �����ϴ� �����ϸ� OHT

	[TP �� ���]
	IS_TP				: TP�� �ݵ�� �����Ѵ�.
	_IN_UDP_UPPER_FORM	: TP�� �ݵ�� �����Ѵ�.
      
*/


#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpDiag.h"
#include "Protocol_Define_Modify.h"
#include "SocketInitClose.h"
#include "UdpServer.h"
#include "UdpBroadCastSender.h"
#include "UdpUniCastSender.h"
#include "PUtily.h"

#ifdef _IN_UDP_UPPER_FORM
	#include "Udp.h"
#endif

#ifdef IS_MCP
	#include "McpMain.h"	// ������ include �ؾ� �ϴϱ�
#endif
#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif
//---------------------------------------------------------------------------

SocketInitClose		*pDIAG_SocketInitClose;		// ������ �ʱ�ȭ ���

// [OHT - DIAG]
UdpServer 			*pDIAG_BroadCastServer;	// �ּ�ȹ�� �������� ó��
UdpServer 			*pDIAG_UniCastServer;		// �ּ�ȹ�� �� ���� �������� ó��

// [OHT - AS]
UdpServer 			*pUdpAsServer;			// OHT - AS ��� ����

// [OHT - AGENT]
UdpServer			*pUdpAgentServer;       // OHT - Agent ��� ����

// [COMMON]
UdpBroadCastSender  *pDIAG_UdpBroadCastSender;	// ��ε��ɽ�Ʈ ����ó��
UdpUniCastSender	*pDIAG_UdpUniCastSender; 	// �ϴ��� ��� ����ó��
UdpUniCastSender	*pAGENT_UdpUniCastSender; 	// AGENT�� �ϴ��� ��� ����ó��

VHL_INFO DIAG_LIST[1];
//---------------------------------------------------------------------------
UdpDiag::UdpDiag()
{
	pDIAG_SocketInitClose = NULL;
	pDIAG_BroadCastServer = NULL;
	pDIAG_UniCastServer = NULL;
	pUdpAsServer = NULL;
	pUdpAgentServer = NULL;

	pDIAG_UdpBroadCastSender = NULL;
	pDIAG_UdpUniCastSender	= NULL;
	pAGENT_UdpUniCastSender = NULL;

	if (pDIAG_SocketInitClose == NULL)
		pDIAG_SocketInitClose = new SocketInitClose();

	if (pDIAG_UdpBroadCastSender == NULL)
		pDIAG_UdpBroadCastSender = new UdpBroadCastSender();

	if (pDIAG_UdpUniCastSender == NULL)
		pDIAG_UdpUniCastSender = new UdpUniCastSender();

	if (pAGENT_UdpUniCastSender == NULL)
		pAGENT_UdpUniCastSender = new UdpUniCastSender();


	// ���� �ʱ�ȭ �ϰ������� ����
	pDIAG_SocketInitClose->SocketInit();

#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPCommunication.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif

#ifdef IS_MCP
	// PORT 9020
	iDIAG_MachineType 	= MACHINETYPE_DIAG_SVR;
	strDIAG_SVR_MachineID 	= "SDG-01";
	iDIAG_BroadCastPort = ADDRESS_DIAG_GET_DEFAULT_PORT;
	iDIAG_UnicastPort 	= CONTROL_DIAG_DEFAULT_PORT;

	// #ifdef	_ONE_PC_TEST
	// 	ShowMessage(L"One PC Test ����Դϴ�!\n\n�����Ͽ� �ֽʽÿ�!");
	// #endif
#endif

#ifdef IS_OHT
	//
	// �� �κ��� CLW�� OCS�� ����ϱ� ���� ����ϴ� �����̴�
	// PORT 9020
	strDIAG_SVR_Address 	= "";		// DIAG_SVR�� �ּ�. ó������ ����ִ�.
	bDIAG_SVR_Connected 	= false;	// OHT�� ���. DIAG_SVR�� ����Ǿ��ٴ� �÷���
	strDIAG_SVR_MachineID 	= "";		// DIAG_SVR�� �ӽ� ID
	iDIAG_MachineType 	= MACHINETYPE_DIAG_NORMAL_OHT;
	strMyMachineId = "OHT001";
	#ifdef	_ONE_PC_TEST
		// PORT 9020
		iDIAG_BroadCastPort = (ADDRESS_DIAG_GET_DEFAULT_PORT + 2);     // OHT�� ��� & 1PC ����� ��쿡 ����, ������ �ڽ��� ��Ʈ��ȣ�� �÷�����
		iDIAG_UnicastPort = (CONTROL_DIAG_DEFAULT_PORT + 2);
		
		// ShowMessage(L"One PC Test ����Դϴ�!\n\n�����Ͽ� �ֽʽÿ�!");
	#else
		// PORT 9020
		iDIAG_BroadCastPort = ADDRESS_DIAG_GET_DEFAULT_PORT;
		iDIAG_UnicastPort = CONTROL_DIAG_DEFAULT_PORT;
	#endif
#endif

#ifdef IS_REAL_OHT
	DIAG_BroadcastBind();
	UnicastBind();
#endif

// shkim. Comm test (temp)
#if 0
	// PORT 9020
	DIAG_LIST[0].strAddress 	= K_DEMO_DIAG_VHL_ADDR;
	DIAG_LIST[0].strMachineID 	= K_DEMO_DIAG_VHL_MACHINE_ID;

	strDIAG_SVR_Address 	= K_DEMO_DIAG_SVR_ADDR;		// OCS�� �ּ�. ó������ ����ִ�.
	bDIAG_SVR_Connected 	= true;	// OHT�� ���. OCS�� ����Ǿ��ٴ� �÷���
	strDIAG_SVR_MachineID 	= K_DEMO_DIAG_SVR_MACHINE_ID;	// OCS�� �ӽ� ID

	AnsiString strInfo2;
	strInfo2.sprintf("Diag OHT �ּ� : %s Diag OHT �̸� : %s  \r\n Diag Svr �ּ� : %s Diag Svr �̸� : %s \r\n ",
			DIAG_LIST[0].strAddress, DIAG_LIST[0].strMachineID, strDIAG_SVR_Address, strDIAG_SVR_MachineID);

    ShowMessage(strInfo2);
#endif
// ----------------------------

//#ifdef IS_OHT
	OHT_AS_Bind();
	OHT_AGENT_Bind();
//#endif

//#ifdef IS_AS
//	OHT_AS_Bind();
//#endif
}

//---------------------------------------------------------------------------
void UdpDiag::DisplayLog(String strLog)
{
#ifdef _IN_UDP_UPPER_FORM
	frmUDP->DisplayLog(strLog); 	// ��� ����׸� ���� UDP�� �ȿ� �ִ� ���, ȭ�鿡 ǥ��
#endif

#ifdef IS_REAL_OHT
	AnsiString strTmp = strLog;
	ADD_LOG("%s", strTmp);
#endif
}
//---------------------------------------------------------------------------

UdpDiag::~UdpDiag()
{
	pDIAG_SocketInitClose->SocketClose();

	if (pDIAG_SocketInitClose != NULL)
		delete(pDIAG_SocketInitClose);

	if (pDIAG_UdpBroadCastSender != NULL)
		delete(pDIAG_UdpBroadCastSender);

	if (pDIAG_UdpUniCastSender != NULL)
		delete(pDIAG_UdpUniCastSender);

	if (pAGENT_UdpUniCastSender != NULL)
		delete(pAGENT_UdpUniCastSender);

	OHT_AS_Close();
	OHT_AGENT_Close();
}
//---------------------------------------------------------------------------
#ifdef IS_OHT
// �μ����� : strSvrAddr�� ���ܼ����� IP�ּ� (127.0.0.1 ��), iListenPortNum�� ������� ��Ʈ��ȣ(OHT�� ���� ���Ϲ�ȣ ��� - ����Ʈ CONTROL_DIAG_DEFAULT_PORT),
//            (OHT�� ���� ���Ϲ�ȣ ���),  strMyName�� OHT�� �̸�(OHT001 ��)
// �Ϲ� OHT : MACHINETYPE_DIAG_NORMAL_OHT �� ����
// û�� OHT : MACHINETYPE_DIAG_CLEAN_OHT �� ����
int UdpDiag::SetDiagInfo(AnsiString strSvrAddr, int iListenPortNum, AnsiString strMyName, int iOhtMachineType)
{
	strDIAG_SVR_Address = strSvrAddr;
	iDIAG_MachineType 	= iOhtMachineType;

#ifdef	_ONE_PC_TEST
	iDIAG_UnicastPort = (iListenPortNum + 2);
#else
	iDIAG_UnicastPort = iListenPortNum;
#endif

	strMyMachineId = strMyName;

	bDIAG_SVR_Connected = true;

	return 0;
}
#endif

//---------------------------------------------------------------------------
#ifdef IS_MCP
// strOhtAddr : ����� ������ OHT�� IP�ּ�, iListenPortNum : ������� ��Ʈ��ȣ (OHT�� ���� ���Ϲ�ȣ ��� - ����Ʈ CONTROL_DIAG_DEFAULT_PORT), 
// strMyName : ���ܼ����� �̸� (SDG-01 ��),  iDiagMachineType : ���ܼ��� �ĺ� ID (MACHINETYPE_DIAG_SVR �� ����Ʈ)
int UdpDiag::SetDiagInfo(AnsiString strOhtAddr, int iListenPortNum, AnsiString strMyName, int iDiagMachineType)
{
	DIAG_LIST[0].strAddress = strOhtAddr;
	iDIAG_UnicastPort = iListenPortNum;
	strDIAG_SVR_MachineID = strMyName;
	iDIAG_MachineType = iDiagMachineType;

	return 0;
}
#endif

//---------------------------------------------------------------------------

int UdpDiag::BroadcastSend(int iPort, AnsiString strMessage)
{
	String strMsg;
	int retval = pDIAG_UdpBroadCastSender->BroadcastSendMessage(iDIAG_BroadCastPort, strMessage);

	strMsg.sprintf(L"[UDP BroadCast] %d����Ʈ�� ���½��ϴ�.",retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::BroadcastSend(int iPort, char* cMessage, int iLen)
{
	int retval = pDIAG_UdpBroadCastSender->BroadcastSendMessage(iPort,cMessage,iLen);
	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::DIAG_BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

 	// bind���κ��� �˻�
	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "Port 9020 B/C ������ ���ŵ�. ��� IP : " + strClientAddr;
		strTmp2.sprintf(" ��� Port : %d ���� : %d ���� : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("������ ���ŵ��� ����. �����ڵ� : %d", iReturn);
	}

	return iReturn;
}
//---------------------------------------------------------------------------
int UdpDiag::DIAG_BroadcastBind()
{
	int iRetVal;

	if (pDIAG_BroadCastServer == NULL)
		pDIAG_BroadCastServer = new UdpServer();

	iRetVal = pDIAG_BroadCastServer->Bind(iDIAG_BroadCastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[���ܼ��� - �ּ� ��Ʈ�� ��Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[���ܼ��� - �ּ� ��Ʈ�� ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[���ܼ��� - �ּ� ��Ʈ�� ��Ʈ] Bind ���� ����");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------

int UdpDiag::DIAG_BroadcastBind(int iBroadCastListenPort)
{
	iDIAG_BroadCastPort = iBroadCastListenPort;
	return DIAG_BroadcastBind();
}
//---------------------------------------------------------------------------
void UdpDiag::DIAG_BroadcastClose()
{
	if (pDIAG_BroadCastServer != NULL)
	{
		pDIAG_BroadCastServer->Close();
		delete(pDIAG_BroadCastServer);
		pDIAG_BroadCastServer = NULL;
	}

	DisplayLog(L"DIAG(9020) Broadcast Server Closed");
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	AnsiString strMsg;
	int retval;

	retval = pDIAG_UdpUniCastSender->UnicastSendMessage(strIpAddr,iPort, strMessage);

	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d����Ʈ�� ���½��ϴ�.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("���ǵ� �ִ� ��Ŷ ������� ū �޽��� �۽Žõ�!!");
        return UNICAST_SEND_RESULT_FAIL;
    }

	retval = pDIAG_UdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
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

int UdpDiag::UnicastSendAGENT(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	unsigned char oneChar;

	if(len > MAX_PACKET_SIZE)
	{
		ShowMessage("���ǵ� �ִ� ��Ŷ ������� ū �޽��� �۽Žõ�!!");
        return UNICAST_SEND_RESULT_FAIL;
	}

	retval = pAGENT_UdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);
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

//---------------------------------------------------------------------------

// ��Ʈ�� ��Ʈ�� �����ϴ� �����, �� �Լ��� �ܺη� �巯���� ������ ����̴�
// �� PC���� �׽�Ʈ�ϴ� ��� ��Ʈ��ȣ�� Ʋ������ ���� ���ϰ� �Ǵµ�,
// �� �κ��� �ſ� �����ؾ� �Ѵ�.
int UdpDiag::UnicastSend(char* buf, int len)
{
// ���� : DIAG_LIST[0].strAddress�� ���ܼ���(���⼭�� OCS����)�� ����ϴ� ���̰�,
// 		  strDIAG_SVR_Address��  OHT�� ����ϴ� ���̴�.

#ifdef IS_MCP
	if(DIAG_LIST[0].strAddress == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_OHT
	if(strDIAG_SVR_Address == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif


#ifdef IS_MCP
	#ifdef	_ONE_PC_TEST
		// �� PC�󿡼� �׽�Ʈ ��, OHT�� +2��Ʈ�� ����ϹǷ�..
		return UnicastSend(DIAG_LIST[0].strAddress, (iDIAG_UnicastPort + 2), buf, len);
	#else
		return UnicastSend(DIAG_LIST[0].strAddress, iDIAG_UnicastPort, buf, len);
	#endif
#endif

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		// �� PC�׽�Ʈ�϶�, OHT�� ��Ʈ�� ��Ʈ��ȣ�� 9023�� �ȴ�.
		// ���� OCS�� ���� ������ �ٽ� 9021�� �ǵ��� 2�� ������
		return UnicastSend(strDIAG_SVR_Address, (iDIAG_UnicastPort - 2), buf, len);
	#else
		return UnicastSend(strDIAG_SVR_Address, iDIAG_UnicastPort, buf, len);
	#endif
#endif

	return 0;
}
//---------------------------------------------------------------------------
int UdpDiag::UnicastBind()
{
	int iRetVal;

	if (pDIAG_UniCastServer == NULL)
		pDIAG_UniCastServer = new UdpServer();

	iRetVal = pDIAG_UniCastServer->Bind(iDIAG_UnicastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[���ܼ��� - ��Ʈ�� ��Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[���ܼ��� - ��Ʈ�� ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[���ܼ��� - ��Ʈ�� ��Ʈ] Bind ���� ����");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpDiag::UnicastBind(int iUnicastListenPort)
{
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iDIAG_UnicastPort = iUnicastListenPort + 2;
	#else
		iDIAG_UnicastPort = iUnicastListenPort;
	#endif
#else	// OCS Case
		iDIAG_UnicastPort = iUnicastListenPort;
#endif

	return UnicastBind();
}
//---------------------------------------------------------------------------
int UdpDiag::UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

	// bind���κ��� �˻�
	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "(9021) ������ ���ŵ�. ��� IP : " + strClientAddr;
		strTmp2.sprintf(" ��� Port : %d ���� : %d ���� : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("(9021) ������ ���ŵ��� ����. �����ڵ� : %d", iReturn);
		// DisplayLog(strTmp);
	}

	return iReturn;
}

//---------------------------------------------------------------------------
void UdpDiag::UnicastClose()
{
	if (pDIAG_UniCastServer != NULL)
	{
		pDIAG_UniCastServer->Close();
		delete(pDIAG_UniCastServer);
		pDIAG_UniCastServer = NULL;
	}
}

//---------------------------------------------------------------------------
// 3.3.1 ���ܼ����� OHT �ּҸ� ȹ���Ѵ� & 3.3.2 OHT�� ���ܼ��� �ּҸ� ȹ���Ѵ� (�μ� �����ϴ� ���)
int UdpDiag::Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcast_DIAG_Packet);

	AddressBroadcast_DIAG_Packet Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcast_DIAG_Packet));

	// Step1. MCP�� ��Ŷ�� ä���� BroadCast
	Pkt.CommandID = ADDRESS_DIAG_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	Conv_AddressBroadcast_DIAG_Packet(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"(9020) �ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"(9020) �ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ� (���� ����Ʈ�� �����ϴ� ���)
int UdpDiag::Address_Broadcast()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast(iDIAG_MachineType, strMyMachineId, (iDIAG_BroadCastPort - 2));
#else
	return Address_Broadcast(iDIAG_MachineType, strMyMachineId, iDIAG_BroadCastPort);
#endif
}

//---------------------------------------------------------------------------
#ifdef IS_MCP
int UdpDiag::DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];

	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) �����Ͱ� ���ǵ� ��Ŷ�԰ݿ� �´� ������ �����Ѵ�.
		// (1-1) ��Ŷ�� �ּ�ȹ��� ��Ŷ ũ������ Ȯ��. �ƴ϶�� �׳� ����
		iPacketSize = sizeof(AddressBroadcast_DIAG_Packet);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(9020) B/C ������Ŷ�� ũ�Ⱑ �����մϴ�. ����size : %d ����size : %d", iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcast_DIAG_Packet Pkt;
		memcpy(&Pkt, &buf, iPacketSize);

		Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert

		// (1-2) OHT�� OCS���� ���� �������� Ȯ���Ѵ�.
		if((Pkt.CommandID != ADDRESS_DIAG_BROADCAST_DEF) && (Pkt.CommandID != ADDRESS_DIAG_UNICAST_REPLY))
		{
			strTmp.sprintf("(9020) B/C ������Ŷ�� ������ �����մϴ� (identify : %d)", Pkt.CommandID);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		// �� �κ��� OCS�� �ڵ��̸�, ���⸦ �������ٴ� �� VHL�� ���� �Ŷ�� ��
		if((Pkt.MachineType != MACHINETYPE_DIAG_NORMAL_OHT)	&& (Pkt.MachineType != MACHINETYPE_DIAG_CLEAN_OHT))	
		{	// �� �κп� ������ ���� �ٸ� OCS�� �ּ�ȹ�� B/C�� �� ���� ������ ���
			strTmp.sprintf("(9020) �ٸ� OCS�� VHL�ּҿ�û B/C (MachineType : %d)", Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST;
		}

		// (2) ������ ������ ���⼭ �ؼ��Ͽ� ��������
		//	�� �κ��� ��� ���� ������ �ʿ�������, �ϴ� �̷��Ը� �� ���� ����� ��������
		//	���ķ����ʹ� 1�븸 ������ ���̹Ƿ� �����ϰ� �̷��� �س���
		DIAG_LIST[0].strAddress = strClientAddr;
		memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
		cTemp[MACHINE_ID_LENGTH] = '\0';
		DIAG_LIST[0].strMachineID = cTemp;
		
		strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s - Registed",
						Pkt.CommandID, Pkt.MachineType, DIAG_LIST[0].strAddress, DIAG_LIST[0].strMachineID);
		DisplayLog(strTmp);

		// (3) DIAG Server�� OCS�� ã�� Broadcast�� ���� �����ؾ� �Ѵ�.
		if (Pkt.CommandID == ADDRESS_DIAG_BROADCAST_DEF)
		{
			memset(&Pkt, 0, iPacketSize);
			Pkt.CommandID = ADDRESS_DIAG_UNICAST_REPLY;
			Pkt.MachineType = iDIAG_MachineType;
			
			memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID)); 		
			memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());			
	
						
			// (4) DIAG Server�� ���� ��Ŷ�� �����ϰ� ������
			Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert
			UnicastSend(DIAG_LIST[0].strAddress, iPort, (char*)&Pkt, iPacketSize);
		}
		return BROADCAST_LISTEN_RESULT_SUCCESS;
	}
	else
		return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

// �� �Լ��� ��ƴ�. �켱, �� ��� �ʱ⼳��� �ڽ��� ������ �ʱⰪ�� ������.
// ��Ʈ��ȣ�� ���� ���� �ʿ��ϸ�, OCS�� ��Ʈ ���ϱ� ����� ���� ó���� �ʿ�
// OHT�� �ݴ�� �Ѵ�
#ifdef IS_MCP
int UdpDiag::DIAG_BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort + 2));	// ������ ���� ��ٰ� ���� 
	#else																				// (Diag Svr : 9020, OHT : 9022)
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort));
	#endif
}
#endif
#ifdef IS_OHT
int UdpDiag::DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];

	iReturn = pDIAG_BroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		// (1) �����Ͱ� ���ǵ� ��Ŷ�԰ݿ� �´� ������ �����Ѵ�.
		// (1-1) ��Ŷ�� �ּ�ȹ��� ��Ŷ ũ������ Ȯ��. �ƴ϶�� �׳� ����
		iPacketSize = sizeof(AddressBroadcast_DIAG_Packet);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(9020) B/C ������Ŷ�� ũ�Ⱑ �����մϴ�. ����size : %d ����size : %d", iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcast_DIAG_Packet Pkt;
		memcpy(&Pkt, &buf, iPacketSize);
		Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert

		// (1-2) OCS�� OHT���� ���� �������� Ȯ���Ѵ�.
		if((Pkt.CommandID != ADDRESS_DIAG_BROADCAST_DEF) && (Pkt.CommandID != ADDRESS_DIAG_UNICAST_REPLY))
		{
			strTmp.sprintf("(9020) B/C ������Ŷ�� ������ �����մϴ� (CommandID : %d)", Pkt.CommandID);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		if(Pkt.MachineType != MACHINETYPE_DIAG_SVR)		// �� �κ��� OHT�� �ڵ��̸�, ���⸦ �������ٴ� �� MCP�� ���� �Ŷ�� ��
		{	// ���� OHT�ε�, �ٸ� OHT�� B/C �۽ų����� �����. (�׷� ��������� �Ѵ�)
			strTmp.sprintf("(9020) �ٸ� VHL�� Diag SVR �ּҿ�û B/C (MachineType : %d)", Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST;
		}

		// (2) ������ ������ ���⼭ �ؼ��Ͽ� ��������
		// ���� ���̽��� ���� �б�
		switch(Pkt.CommandID)
		{
			case ADDRESS_DIAG_BROADCAST_DEF:	// DIAG SVR�� OHT�� Ž���� ��� => �����ϸ� ����� �����Ѵ�
				{
					strDIAG_SVR_Address = strClientAddr;
					bDIAG_SVR_Connected = true;

 					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strDIAG_SVR_MachineID = cTemp;

					strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s => Registed",
									Pkt.CommandID, Pkt.MachineType, strDIAG_SVR_Address, strDIAG_SVR_MachineID);
					DisplayLog(strTmp);

					memset(&Pkt, 0, iPacketSize);
					Pkt.CommandID = ADDRESS_DIAG_UNICAST_REPLY;
					Pkt.MachineType = iDIAG_MachineType;
					memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
					memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
					
					// (4) OCS�� ���� ��Ŷ�� �����ϰ� ������
					Conv_AddressBroadcast_DIAG_Packet(Pkt);	// Endian Convert
					UnicastSend(strDIAG_SVR_Address, iPort, (char*)&Pkt, iPacketSize);

					return BROADCAST_LISTEN_RESULT_SUCCESS;
				}
			//break;
			
			case ADDRESS_DIAG_UNICAST_REPLY:		// CLW�� MCP�� Ž���� ��� => ������ ���常 �Ѵ�
				{
					strDIAG_SVR_Address = strClientAddr;
					bDIAG_SVR_Connected = true;

					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strDIAG_SVR_MachineID = cTemp;

					strTmp.sprintf("(9020) Packet CommandID : %d Machine Type : %d Address : %s Machine ID : %s => Registed",
									Pkt.CommandID, Pkt.MachineType, strDIAG_SVR_Address, strDIAG_SVR_MachineID);
					DisplayLog(strTmp);
				}
			break;			
			}
		
			return BROADCAST_LISTEN_RESULT_SUCCESS;
		}
		else
			return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

#ifdef IS_OHT
bool UdpDiag::DIAG_Svr_Connected()
{
	return bDIAG_SVR_Connected;
}
#endif

#ifdef IS_OHT
int UdpDiag::DIAG_BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort - 2));	// ���� ���� ��ٰ� ���� (OCS : 9000, OHT : 9002), �� 2�� ����� ��
	#else
		return DIAG_BroadCast_ThreadCall(strMyMachineId, (iDIAG_BroadCastPort));
	#endif
}
#endif
#ifdef IS_MCP
int UdpDiag::DIAG_UniCast_ThreadCall()
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	// unsigned char oneChar;
	//unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;
	// int iBig, iSmall;
	
	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{

		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(DIAG_PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("[OHT->���ܼ��� ����] ��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		DIAG_PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
		Conv_DIAG_PKT_CHECK_TAG(Pkt);

		switch(Pkt.HEADER.CommandID)
		{
			case DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE:	// ���� Ȯ�� ���� (EQ -> ���� Agent)
			{
				DIAG_PKT_CONNECT_CHK_RESPONSE	CmdPkt;		// ���� Ȯ�� ���� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ���� Ȯ�� ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ���� Ȯ�� ���� (EQ -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::DIAG_INFO_DATA_RESPONSE:	// ����������� (�Ϲ� OHT -> ���� Agent)
			{
				DIAG_PKT_INFO_DATA_RESPONSE	CmdPkt;		// ����������� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_INFO_DATA_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ����������� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_INFO_DATA_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ����������� (EQ -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::DIAG_DATA_RESET_RESPONSE:	// ������� �ʱ�ȭ ���� (�Ϲ� OHT -> ���� Agent)
			{
				DIAG_PKT_DATA_RESET_RESPONSE	CmdPkt;		// ������� �ʱ�ȭ ���� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_DATA_RESET_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ������� �ʱ�ȭ ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_RESET_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ������� �ʱ�ȭ ���� (EQ -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}


			case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_RESPONSE:	// ������� ���� ���� ���� (�Ϲ� OHT -> ���� Agent)
			{
				DIAG_PKT_LEVEL_CHANGE_RESPONSE	CmdPkt;		// ���� Ȯ�� ���� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_LEVEL_CHANGE_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ������� ���� ���� ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_LEVEL_CHANGE_RESPONSE(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ������� ���� ���� ���� (�Ϲ� OHT -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::CLEAN_CLW_TO_AGENT_LINK_INFO:	// ��ũ�������� (SEMES�� û�� OHT -> ���� Agent)
			{
				DIAG_PKT_CLEAN_LINK_INFO	CmdPkt;		// ��ũ�������� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_CLEAN_LINK_INFO);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ��ũ�������� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CLEAN_LINK_INFO(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ��ũ�������� (�Ϲ� OHT -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}	

			case DiagCmdID::DIAG_INFO_STATISTICS_REQUEST:	// ������� �䱸 (�Ϲ� OHT -> ���� Agent)
			{
				DIAG_PKT_DATA_INFO_REQUEST	CmdPkt;		// ������� ��û (�Ϲ� OHT -> ���� Agent) ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_DATA_INFO_REQUEST);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->���ܼ��� ����] ������� �䱸 (�Ϲ� OHT -> ���� Agent) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_INFO_REQUEST(CmdPkt);
				
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("[OHT->���ܼ��� ����] ������� �䱸 (�Ϲ� OHT -> ���� Agent) (�Ϲ� OHT -> ���� Agent) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2DIAG	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}	


			default:
			{
				strTmp.sprintf("[OHT->���ܼ��� ����] CommandID �ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. CommandID : %d �����ּ� : %s ���� ��Ʈ : %d",
								Pkt.HEADER.CommandID, strClientAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;



}
#endif

#ifdef IS_OHT
int UdpDiag::DIAG_UniCast_ThreadCall(PKT_OHT2DIAG&	OcsCmdPkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv;

	iReturn = pDIAG_UniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(DIAG_PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("[���ܼ���->OHT ����] ��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� DIAG SVR ���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		DIAG_PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
		Conv_DIAG_PKT_CHECK_TAG(Pkt);

		switch(Pkt.HEADER.CommandID)	// ��ɾ� ������ ���� �б�
		{
			case DiagCmdID::AGENT_TO_CLW_CONNECT_CHK_REQUEST:	// ���� Ȯ�� ��û (���� Agent -> EQ)
			{
				DIAG_PKT_CONNECT_CHK_REQUEST	CmdPkt; 	// ���� Ȯ�� ��û ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[���ܼ���->OHT ����] ���� Ȯ�� ��û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_CONNECT_CHK_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[���ܼ���->OHT ����] ���� Ȯ�� ��û (���� Agent -> EQ) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);

				// ���ܼ����� ���� ��Ŷ�� ����
				DIAG_PKT_CONNECT_CHK_RESPONSE Pkt;
				iPacketSize = sizeof(DIAG_PKT_CONNECT_CHK_RESPONSE);	
				memset(&Pkt, 0, iPacketSize);
				Pkt.HEADER.MachineType = MACHINETYPE_DIAG_NORMAL_OHT;
				Pkt.HEADER.CommandID = DiagCmdID::CLW_TO_AGENT_CONNECT_CHK_RESPONSE;	// ����Ȯ�� ����
				
				memset(&Pkt.HEADER.MachineID, ASCII_SPACE, sizeof(Pkt.HEADER.MachineID));			
				memcpy(&Pkt.HEADER.MachineID, strMyMachineId.c_str(), strMyMachineId.Length());
						
				// ���ܼ����� ���� ��Ŷ�� ����
				Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(Pkt);	// Endian Convert
				// UnicastSend(strDIAG_SVR_Address, iDIAG_UnicastPort, (char*)&Pkt, iPacketSize);
				UnicastSend((char*)&Pkt, iPacketSize);

				// �����Ͽ� ������ ����
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			
			case DiagCmdID::DIAG_INFO_DATA_REQUEST:	// ����������� ��û (���� Agent -> �Ϲ� OHT)
			{
				DIAG_PKT_INFO_DATA_REQUEST	CmdPkt; 	// ����������� ��û ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_INFO_DATA_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[���ܼ���->OHT ����] ����������� ��û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_INFO_DATA_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[���ܼ���->OHT ����] ����������� ��û (���� Agent -> �Ϲ� OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// �����Ͽ� ������ ����
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::DIAG_DATA_RESET_REQUEST:	// ������� �ʱ�ȭ ��û (���� Agent -> �Ϲ� OHT)
			{
				DIAG_PKT_DATA_RESET_REQUEST	CmdPkt; 	// ������� �ʱ�ȭ ��û ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_DATA_RESET_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[���ܼ���->OHT ����] ������� �ʱ�ȭ ��û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_RESET_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[���ܼ���->OHT ����] ������� �ʱ�ȭ ��û (���� Agent -> �Ϲ� OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// �����Ͽ� ������ ����
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::DIAG_INFO_LEVEL_CHANGE_REQUEST:	// ������� ���� ���� ��û (���� Agent -> �Ϲ� OHT)
			{
				DIAG_PKT_LEVEL_CHANGE_REQUEST	CmdPkt; 	// ������� ���� ���� ��û ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_LEVEL_CHANGE_REQUEST);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[���ܼ���->OHT ����] ������� ���� ���� ��û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_LEVEL_CHANGE_REQUEST(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[���ܼ���->OHT ����] ������� ���� ���� ��û (���� Agent -> �Ϲ� OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// �����Ͽ� ������ ����
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case DiagCmdID::DIAG_INFO_STATISTICS_RESPONSE:	// ������� ���� (���� Agent -> �Ϲ� OHT)
			{
				DIAG_PKT_DATA_INFO_RESPONSE	CmdPkt; 	// ������� ���� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(DIAG_PKT_DATA_INFO_RESPONSE);		
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[���ܼ���->OHT ����] ������� ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				} 
		
				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_DIAG_PKT_DATA_INFO_RESPONSE(CmdPkt);
		
				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strConv = cTemp;
				strConv.Trim();
		
				strTmp.sprintf("[���ܼ���->OHT ����] ������� ���� (���� Agent -> �Ϲ� OHT) [HEADER] MachineType : %d MachineID : %s CommandID : %d ",
								CmdPkt.HEADER.MachineType, strConv, CmdPkt.HEADER.CommandID);
				DisplayLog(strTmp);
		
				// �����Ͽ� ������ ����
				memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}


			default:
			{
				strTmp.sprintf("[���ܼ���->OHT ����] CommandID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d �����ּ� : %s ���� ��Ʈ : %d",
								Pkt.HEADER.CommandID, strClientAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}
		}	
	}
	
	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

//---------------------------------------------------------------------------

/*
void UdpDiag::DiagInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort)
{
	iDIAG_MachineType = iMachineType;
	strMyMachineId = strMachineId;
	iDIAG_BroadCastPort = iBroadCastPort;
	iDIAG_UnicastPort = iUnicastPort;
}
*/
//---------------------------------------------------------------------------

#ifdef IS_OHT
// OHT -> AS Data send
int UdpDiag::UnicastSend_OHT_2_AS(char* buf, int len)
{
	UnicastSend(OHT_AS__LOCAL_IP, OHT_AS__AS_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif
//---------------------------------------------------------------------------

#ifdef IS_AS
// AS -> OHT Data send
int UdpDiag::UnicastSend_AS_2_OHT(char* buf, int len)
{
	UnicastSend(OHT_AS__LOCAL_IP, OHT_AS__OHT_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif

#ifdef IS_OHT
// OHT -> AGENT Data send
int UdpDiag::UnicastSend_OHT_2_AGENT(char* buf, int len)
{
	UnicastSendAGENT(OHT_AGENT__LOCAL_IP, OHT_AGENT__AGENT_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}
#endif
//---------------------------------------------------------------------------
// OHT�� ��ý��Ͻ��� ���ż��� ���ε� ����
// �Լ����� �����ϰ�, ���� ��Ʈ��ȣ�� �ٸ���
int UdpDiag::OHT_AS_Bind()
{
	int iRetVal = -1;

	if(pUdpAsServer == NULL)
		pUdpAsServer = new UdpServer();

#ifdef IS_OHT
		iRetVal = pUdpAsServer->Bind(OHT_AS__OHT_PORT_NUM);

		if(iRetVal == 0)
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] Bind ���� ����");
		}
		else if(iRetVal == -3)
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
		}
		else
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] Bind ���� ����");
		}
#endif

#ifdef IS_AS
		iRetVal = pUdpAsServer->Bind(OHT_AS__AS_PORT_NUM);

		if(iRetVal == 0)
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] Bind ���� ����");
		}
		else if(iRetVal == -3)
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
		}
		else
		{
			DisplayLog(L"[OHT - AS Server ��Ʈ] Bind ���� ����");
		}
#endif

	return iRetVal;
}

//---------------------------------------------------------------------------
void UdpDiag::OHT_AS_Close()
{
	if (pUdpAsServer != NULL)
	{
		pUdpAsServer->Close();
		delete(pUdpAsServer);
		pUdpAsServer = NULL;
	}


}


//---------------------------------------------------------------------------
// OHT�� AGENT�� ���ż��� ���ε� ����
// �Լ����� �����ϰ�, ���� ��Ʈ��ȣ�� �ٸ���
int UdpDiag::OHT_AGENT_Bind()
{
	int iRetVal = -1;

	if(pUdpAgentServer == NULL)
		pUdpAgentServer = new UdpServer();

#ifdef IS_OHT
		iRetVal = pUdpAgentServer->Bind(OHT_AGENT__OHT_PORT_NUM);

		if(iRetVal == 0)
		{
			//DisplayLog(L"[OHT - AGENT Server ��Ʈ] Bind ���� ����");
			ADD_LOG("[OHT - AGENT Server ��Ʈ] Bind ���� ����");
		}
		else if(iRetVal == -3)
		{
			//DisplayLog(L"[OHT - AGENT Server ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
			ADD_LOG("[OHT - AGENT Server ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
		}
		else
		{
			//DisplayLog(L"[OHT - AGENT Server ��Ʈ] Bind ���� ����");
			ADD_LOG("[OHT - AGENT Server ��Ʈ] Bind ���� ����");
		}
#endif

	return iRetVal;
}

//---------------------------------------------------------------------------
void UdpDiag::OHT_AGENT_Close()
{
	if (pUdpAgentServer != NULL)
	{
		pUdpAgentServer->Close();
		delete(pUdpAgentServer);
		pUdpAgentServer = NULL;
	}


}

//---------------------------------------------------------------------------
// OHT���� AS�� ��Ŷ�� �����ϴ� �Լ���.

//---------------------------------------------------------------------------
// OHT���� AS�� ��Ŷ�� �����ϴ� �Լ���. 

#ifdef IS_OHT
int UdpDiag::OHT_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	// char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv, strChk;

	iReturn = pUdpAsServer->Listen(buf, iLen, strClientAddr, iClientPort);	// �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
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
				// Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				// --- Log ---
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
				// Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				// --- Log ---
				strTmp.sprintf("������ ���� ��ô ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  ���۽��� : %d  ���ۼ��� : %d  �����Ұ����� : %d  �������� ID : %d  ���۽��б��� : %d",
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
				// Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);

				// --- Log ---
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

							

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);	
				 strTmp.sprintf("������ ���� ��ô ���� (AS => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				 DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);
				
				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize); 
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);

				return iReturn;	
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT:
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
			
				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AS => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AS_Pkt, &CmdPkt, iPacketSize);
			
				return iReturn;
			}
			break;

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

//---------------------------------------------------------------------------
// OCS���� ��Ŷ�� OHT�� ���� ������ �κ�. ��Ŷ�� Ÿ���� OCS
#ifdef IS_AS
int UdpDiag::OHT_AS_ThreadCall(PKT_OCS2OHT& AS_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

//	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv, strChk;

	iReturn = pUdpAsServer->Listen(buf, iLen, strClientAddr, iClientPort);	// �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
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


			case TagID::CMD_INERNAL_PING_REQ:			// ���� ���α׷��鳢�� ���� ������ �޾Ƽ� ���Ῡ�θ� Ȯ���Ѵ�
			{

				return iReturn;
			}

		}

	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;

}
#endif

//---------------------------------------------------------------------------
// OHT���� AGENT�� ��Ŷ�� �����ϴ� �Լ���.

#ifdef IS_OHT
int UdpDiag::OHT_AGENT_ThreadCall(PKT_OHT2OCS& AGENT_Pkt)
{
	AnsiString strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	AnsiString strConv, strChk;

	iReturn = pUdpAgentServer->Listen(buf, iLen, strClientAddr, iClientPort);	// �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		strTmp.sprintf("Agent Packet Received. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strClientAddr, iClientPort);
		DisplayLog(strTmp);
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
				// Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				// --- Log ---
				strTmp.sprintf("������ ���� ���� ���� (AGENT => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  �����Ұ� : %d  �����Ұ����� : %d  �������� ID : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Ack.Accept_Error, CmdPkt.BODY.Ack.Accept_Error_Class, CmdPkt.BODY.Ack.TransOrderID);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

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
				// Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				// --- Log ---
				strTmp.sprintf("������ ���� ��ô ���� (AGENT => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d  ���۽��� : %d  ���ۼ��� : %d  �����Ұ����� : %d  �������� ID : %d  ���۽��б��� : %d",
												CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType,
												CmdPkt.BODY.Result.Trans_Fail, CmdPkt.BODY.Result.Trans_OK, CmdPkt.BODY.Result.Not_Accept_Class, CmdPkt.BODY.Result.TransOrderID,
												CmdPkt.BODY.FailClass);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

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
				// Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);

				// --- Log ---
				strTmp.sprintf("CLW Status ���� (AS => OHT) Tag : %d  ������ �������� ID : %d  ������ ���� ���� : %d  ������ ���ۻ��� ���� : %d  Data ���� ���� : %d  Data ���� ���� : %d",
								CmdPkt.TAG.Value,  CmdPkt.STATUS.DataTransStatus.DataCmdID,  CmdPkt.STATUS.DataTransStatus.DataType,
								CmdPkt.STATUS.DataTransStatus.DataStatus,  CmdPkt.STATUS.DataTransStatus.DataSuccess, CmdPkt.STATUS.DataTransStatus.DataFail);

				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

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

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				 strTmp.sprintf("������ ���� ��ô ���� (AGENT => OHT) Tag : %d  ���۱��� : %d  �䱸���� : %d",
								   CmdPkt.TAG.Value, CmdPkt.BODY.Request.TestMode, CmdPkt.BODY.Request.CMDType);
				 DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			break;

			case TagID::CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT:
			{
				PKT_RESTART_KEEPING_INFO_AS2OHT CmdPkt;

				iPacketSize = sizeof(PKT_RESTART_KEEPING_INFO_AS2OHT);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("OHT Close Order (AGENT => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}
			break;

			case TagID::CMD_FIRMWARE_VERSION_REQUEST_AGENT2OHT:
			{
				PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT CmdPkt;

				iPacketSize = sizeof(PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// --- Log ---
				memcpy(&CmdPkt, &buf, iPacketSize);
				strTmp.sprintf("AGENT requested Firmware Version (AGENT => OHT) Tag : %d",
								   CmdPkt.TAG.Value);
				DisplayLog(strTmp);
				memcpy(&AGENT_Pkt, &CmdPkt, iPacketSize);

				return iReturn;
			}

		}

		memcpy(&AGENT_Pkt, &buf, sizeof(PKT_OHT2OCS));
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

