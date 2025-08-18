#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpCom.h"
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


//#include "OHTMain.h"

 #ifdef IS_OHT
	#include "OhtMain.h"
 #endif

#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif
#ifdef IS_TP
	#include "MainUnit.h"
	#include "SocketUnit.h"
#endif

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


        ��Ʈ��ũ �������� �����ּҸ� OHT�� OCS���Ŀ� �Ҵ��ϴ¹� : _K_DEMO define�� �����ϰ�, Protocol_define ���Ͽ� ���ϴ� ���� ����
*/

//---------------------------------------------------------------------------

SocketInitClose		*pSocketInitClose;		// ������ �ʱ�ȭ ���

UdpServer 			*pUdpBroadCastServer;	// �ּ�ȹ�� �������� ó��
UdpServer 			*pUdpUniCastServer;		// �ּ�ȹ�� �� ���� �������� ó��
UdpServer 			*pUdpTpServer;			// TP��� ����

UdpBroadCastSender  *pUdpBroadCastSender;	// ��ε��ɽ�Ʈ ����ó��
UdpUniCastSender	*pUdpUniCastSender; 	// �ϴ��� ��� ����ó��

VHL_INFO VHL_LIST[50];
// char PacketBuf[MAX_PACKET_SIZE];	// �۾��� �ӽ� ����. �̰ɷ� �ϰ����ְ� ����

//---------------------------------------------------------------------------
UdpCom::UdpCom(int PortNum)
{
	pSocketInitClose = NULL;

	pUdpBroadCastServer = NULL;
	pUdpUniCastServer = NULL;
	pUdpTpServer = NULL;

	pUdpBroadCastSender = NULL;
	pUdpUniCastSender	= NULL;

	if (pSocketInitClose == NULL)
		pSocketInitClose = new SocketInitClose();

	if (pUdpBroadCastSender == NULL)
		pUdpBroadCastSender = new UdpBroadCastSender();

	if (pUdpUniCastSender == NULL)
		pUdpUniCastSender = new UdpUniCastSender();

	// ���� �ʱ�ȭ �ϰ������� ����
	pSocketInitClose->SocketInit();

#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPCommunication.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif



#ifdef IS_MCP
	for(int i = 0; i < 50; i++)
	{
		VHL_LIST[i].strAddress = "";
		VHL_LIST[i].strMachineID = "";
	}

	iMyMachineType = MACHINETYPE_MCP;
	strMyMachineId = "MCP01";
	iMyBroadCastPort = ADDRESS_GET_DEFAULT_PORT+PortNum;
	iMyUnicastPort = CONTROL_DEFAULT_PORT+PortNum;
	#ifdef	_ONE_PC_TEST
		ShowMessage(L"One PC Test ����Դϴ�!\n\n�����Ͽ� �ֽʽÿ�!");
	#endif
#endif

#ifdef IS_OHT
	
	//
	// �� �κ��� CLW�� OCS�� ����ϱ� ���� ����ϴ� �����̴�
	for(int i = 0; i < MAX_OCS_NUMBER; i++)
	{
		strOcsAddress[i] = "";		// OCS�� �ּ�. ó������ ����ִ�.
		strOcsMachineID[i] = "";	// OCS�� �ӽ� ID
	}
	iOcsCount = 0;

	// bOcsConnected = false;	// OHT�� ���. OCS�� ����Ǿ��ٴ� �÷���

	// ocs_test_ver
	//
	/*
	strOcsAddress = "192.167.0.20";		// OCS�� �ּ�. ó������ ����ִ�.
	bOcsConnected = true;	// OHT�� ���. OCS�� ����Ǿ��ٴ� �÷���
	strOcsMachineID = "MCP01";	// OCS�� �ӽ� ID
	*/

	

	iMyMachineType = MACHINETYPE_VHL;
	strMyMachineId = "OHT001";

	bTpConnected = false; 	// OHT�� ���. TP�� ����Ǿ��ٴ� �÷���
	strTpAddr = "";

	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = (ADDRESS_GET_DEFAULT_PORT +PortNum+ 2);     // OHT�� ��� & 1PC ����� ��쿡 ����, ������ �ڽ��� ��Ʈ��ȣ�� �÷�����
		iMyUnicastPort = (CONTROL_DEFAULT_PORT +PortNum+ 2);
		ShowMessage(L"One PC Test ����Դϴ�!\n\n�����Ͽ� �ֽʽÿ�!");
	#else
		iMyBroadCastPort = ADDRESS_GET_DEFAULT_PORT+PortNum;
		iMyUnicastPort = CONTROL_DEFAULT_PORT+PortNum;
	#endif
#endif

#ifdef IS_TP
	strOhtAddr = "";
	strTpMachineID = "TP01";
	bOhtConnected = false;	// TP�� ����ϴ� ����. OHT�� ����Ǿ��� �÷���

	#ifdef	_ONE_PC_TEST
		iOhtPortNum = (CONTROL_DEFAULT_PORT +PortNum+ 2);
	#else
		iOhtPortNum = CONTROL_DEFAULT_PORT+PortNum;
	#endif
#endif

	iClwCount = 0;


#ifdef IS_REAL_OHT
	BroadcastBind();
	UnicastBind();
	OHT_TP_Bind();
#endif


// shkim. Comm test (temp)
#ifdef _K_DEMO
	VHL_LIST[0].strAddress 		= K_DEMO_VHL_ADDR;
	VHL_LIST[0].strMachineID 	= K_DEMO_VHL_MACHINE_ID;
	iClwCount++;
	
	// bOcsConnected = true;	// OHT�� ���. OCS�� ����Ǿ��ٴ� �÷���

	strOcsAddress[0] = K_DEMO_OCS_ADDR;		// OCS�� �ּ�. ó������ ����ִ�.
	strOcsMachineID[0] = K_DEMO_OCS_MACHINE_ID;	// OCS�� �ӽ� ID

	strOcsAddress[1] = K_DEMO_OCS_ADDR_2;		// OCS�� �ּ�. ó������ ����ִ�.
	strOcsMachineID[1] = K_DEMO_OCS_MACHINE_ID_2;	// OCS�� �ӽ� ID

	iOcsCount = MAX_OCS_NUMBER;

    AnsiString strInfo;
	strInfo.sprintf("IP �������� ����Դϴ�!! \r\n OHT�ּ� : %s OHT �̸� : %s  \r\n  OCS1�ּ� : %s OCS1�̸� : %s  OCS2�ּ� : %s OCS2�̸� : %s \r\n ",
        	VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, strOcsAddress[0], strOcsMachineID[0], strOcsAddress[1], strOcsMachineID[1]);
        ShowMessage(strInfo);
#endif

// --------------------------------------------------------------------------------------------
/*
	�������� ���������� Ȯ���Ѵ�
*/
#ifdef IS_OHT
	#ifdef IS_REAL_OHT 	// OHT Main
		if(SUPPORT_PROTOCOL_VER < OHT_REAL_VER)
			ShowMessage(L"�������� ���� �������� OHT Main�ڵ� ������ �����ϴ�");
	#else 			// OHT emulator
		if(SUPPORT_PROTOCOL_VER < OHT_EMUL_VER)
			ShowMessage(L"�������� ���� �������� OHT Emul�ڵ� ������ �����ϴ�");
	#endif
#endif

#ifdef IS_MCP 		// OCS emulator
		if(SUPPORT_PROTOCOL_VER < OCS_EMUL_VER)
			ShowMessage(L"�������� ���� �������� OCS Emul�ڵ� ������ �����ϴ�");
#endif


#ifdef IS_TP 		// note TP
		if(SUPPORT_PROTOCOL_VER < NOTE_TP_VER)
			ShowMessage(L"�������� ���� �������� TP�ڵ� ������ �����ϴ�");
#endif

#ifdef IS_AS 		// ��ý��Ͻ�
		if(SUPPORT_PROTOCOL_VER < ASSISTANCE_VER)
			ShowMessage(L"�������� ���� �������� ��ý��Ͻ� �ڵ� ������ �����ϴ�");
#endif

#ifdef IS_FM 		// ���ϸŴ���
		if(SUPPORT_PROTOCOL_VER < FILEMANAGER_VER)
			ShowMessage(L"�������� ���� �������� ���ϸŴ��� �ڵ� ������ �����ϴ�");
#endif
// --------------------------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
void UdpCom::DisplayLog(String strLog)
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

UdpCom::~UdpCom()
{
	pSocketInitClose->SocketClose();

	if (pSocketInitClose != NULL)
		delete(pSocketInitClose);

	if (pUdpBroadCastSender != NULL)
		delete(pUdpBroadCastSender);

	if (pUdpUniCastSender != NULL)
		delete(pUdpUniCastSender);


}
//---------------------------------------------------------------------------

int UdpCom::BroadcastSend(int iPort, AnsiString strMessage)
{
	String strMsg;
	int retval = pUdpBroadCastSender->BroadcastSendMessage(iMyBroadCastPort, strMessage);

	strMsg.sprintf(L"[UDP BroadCast] %d����Ʈ�� ���½��ϴ�.",retval);
	DisplayLog(strMsg);

	return retval;
}
//---------------------------------------------------------------------------
int UdpCom::BroadcastSend(int iPort, char* cMessage, int iLen)
{
	AnsiString strMsg, strTmp;
	char oneChar;

	int retval = pUdpBroadCastSender->BroadcastSendMessage(iPort,cMessage,iLen);

	strTmp =  DateTimeToStr(Now());
	strMsg.sprintf("[UDP Broadcast] (%s) PortNo : %d  %d����Ʈ�� ���½��ϴ�.", strTmp, iPort, retval);
	DisplayLog(strMsg);

	strMsg = "";
	for (int i = 0; i < iLen; i++)
	{
		oneChar = cMessage[i];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hexǥ�÷� �ٲ�
		else
			strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);

		strMsg = strMsg + strTmp;
	}
	DisplayLog(strMsg);
	

	return retval;
}

//---------------------------------------------------------------------------
int UdpCom::BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

 	// bind���κ��� �˻�
	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "������ ���ŵ�. ��� IP : " + strClientAddr;
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

int UdpCom::BroadcastBind()
{
	int iRetVal;

	if (pUdpBroadCastServer == NULL)
		pUdpBroadCastServer = new UdpServer();

	iRetVal = pUdpBroadCastServer->Bind(iMyBroadCastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[OCS - �ּ� ��Ʈ�� ��Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OCS - �ּ� ��Ʈ�� ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[OCS - �ּ� ��Ʈ�� ��Ʈ] Bind ���� ����");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpCom::BroadcastBind(int iBroadCastListenPort)
{
	// OHT/OHT���ķ������� ���, ONE PC��忡���� ������ ��Ʈ��ȣ���� +2�� ����
	// ��Ʈ��ȣ�� ����Ѵ�
	// ��, OCS : 9000��, OHT : 9002���� ����Ͽ� �� PC���� ������ ������ �Ѵ�
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = iBroadCastListenPort + 2;
	#else
		iMyBroadCastPort = iBroadCastListenPort;
	#endif
#else  	// OCS ���ķ������� ���, �ڽ��� ��Ʈ��ȣ�� ������ �״�� ����Ѵ�
	iMyBroadCastPort = iBroadCastListenPort;
#endif
	return BroadcastBind();
}
//---------------------------------------------------------------------------

void UdpCom::BroadcastClose()
{
	if (pUdpBroadCastServer != NULL)
	{
		pUdpBroadCastServer->Close();
		delete(pUdpBroadCastServer);
		pUdpBroadCastServer = NULL;
	}

	DisplayLog(L"Broadcast Server Closed");
}

//---------------------------------------------------------------------------
int UdpCom::UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	AnsiString strMsg;
	int retval;

	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr,iPort, strMessage);

	strMsg.sprintf("[UDP Unicast] Addr : %s  PortNo : %d  %d����Ʈ�� ���½��ϴ�.",strIpAddr, iPort, retval);
	DisplayLog(strMsg);

	return retval;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	AnsiString strMsg, strTmp;
	int retval;
	// int iBig, iSmall;
	unsigned char oneChar;


	if(len > MAX_BIG_PACKET_SIZE)
	{
		strMsg.sprintf("Size Over Message Send Try Error (Max Packet Size : %d)", MAX_BIG_PACKET_SIZE);
		DisplayLog(strMsg);
		return UNICAST_SEND_RESULT_FAIL;
	}


	retval = pUdpUniCastSender->UnicastSendMessage(strIpAddr, iPort, buf, len);	
//	strMsg.sprintf("[UDP Unicast] Target Addr : %s  PortNo : %d  %d Bytes Sended",strIpAddr, iPort, retval);
//	DisplayLog(strMsg);

//#ifdef IS_TP
//	return retval;
//#endif

	if ((iPort == TP_FIX_PORT_NUM) || (iPort == OHT_S_TP_LISTEN_PORT_NUM))
		return retval;

	strMsg = "";	// �ѹ� ��� �� �ݵ�� ����
	for (int i = 0; i < retval; i++)
	{
		oneChar = buf[i];
		/*
		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hexǥ�÷� �ٲ�
		else
			strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);
		*/

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("%02X(%c) ", oneChar, oneChar);	// Hexǥ�÷� �ٲ�
		else
			strTmp.sprintf("%02X ", oneChar);
		
		// strMsg = strMsg + strTmp;

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


#if 0
	iBig = len / RAW_LOG_BYTE_1_LINE_SIZE;
	iSmall = len % RAW_LOG_BYTE_1_LINE_SIZE;

	// 10�� ���� �α�����
	for (int i = 0; i < iBig; i++)
	{
		strMsg = "";	// �ѹ� ��� �� �ݵ�� ����
		for(int j = 0; j < RAW_LOG_BYTE_1_LINE_SIZE; j++)
		{
			oneChar = buf[(i * RAW_LOG_BYTE_1_LINE_SIZE) + j];

			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
				strTmp.sprintf("[%d]0\x%02X(%c)  ", ((i * RAW_LOG_BYTE_1_LINE_SIZE) + j), oneChar, oneChar);	// Hexǥ�÷� �ٲ�
			else
				strTmp.sprintf("[%d]0\x%02X  ", (i * RAW_LOG_BYTE_1_LINE_SIZE) + j, oneChar);

			strMsg = strMsg + strTmp;
		}
		DisplayLog(strMsg);
	}

	// 1�� ���� �α�����
	strMsg = ""; 	// �ѹ� ��� �� �ݵ�� ����
	for (int i = 0; i < iSmall; i++)
	{
		oneChar = buf[(iBig * RAW_LOG_BYTE_1_LINE_SIZE) + iSmall];

		if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
			strTmp.sprintf("[%d]0\x%02X(%c)  ", (iBig * RAW_LOG_BYTE_1_LINE_SIZE + i), oneChar, oneChar);	// Hexǥ�÷� �ٲ�
		else
			strTmp.sprintf("[%d]0\x%02X  ", (iBig * RAW_LOG_BYTE_1_LINE_SIZE + i), oneChar);

		strMsg = strMsg + strTmp;
	}
	DisplayLog(strMsg);
#endif

	return retval;

}

//---------------------------------------------------------------------------

// ��Ʈ�� ��Ʈ�� �����ϴ� �����, �� �Լ��� �ܺη� �巯���� ������ ����̴�
// �� PC���� �׽�Ʈ�ϴ� ��� ��Ʈ��ȣ�� Ʋ������ ���� ���ϰ� �Ǵµ�,
// �� �κ��� �ſ� �����ؾ� �Ѵ�.
int UdpCom::UnicastSend(char* buf, int len)
{
// ���� : VHL_LIST[0].strAddress�� OCS�� ����ϴ� ���̰�,
// 		  strOcsAddress�� OHT�� ����ϴ� ���̴�.

#ifdef IS_MCP
	if(VHL_LIST[0].strAddress == "")
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_OHT
	if(iOcsCount == 0)
	{
		DisplayLog(L"Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif

#ifdef IS_TP
	if(strOhtAddr == "")
	{
		DisplayLog(L"OHT Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}
#endif


#ifdef IS_MCP
	#ifdef	_ONE_PC_TEST
		// �� PC�󿡼� �׽�Ʈ ��, OHT�� +2��Ʈ�� ����ϹǷ�..
		UnicastSend(VHL_LIST[0].strAddress, (iMyUnicastPort + 2), buf, len);
	#else
		UnicastSend(VHL_LIST[0].strAddress, iMyUnicastPort, buf, len);
	#endif
#endif

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		// �� PC�׽�Ʈ�϶�, OHT�� ��Ʈ�� ��Ʈ��ȣ�� 9003�� �ȴ�.
		// ���� OCS�� ���� ������ �ٽ� 9001�� �ǵ��� 2�� ������
		for (int i = 0; i < iOcsCount; i++) 
			UnicastSend(strOcsAddress[i], (iMyUnicastPort - 2), buf, len);
	#else
	try{
		for (int i = 0; i < iOcsCount; i++)
			UnicastSend(strOcsAddress[i], iMyUnicastPort, buf, len);
	}catch(...)
	{
    		DisplayLog(L"UnicastSend Exception!!!");
	}
	#endif
#endif

#ifdef IS_TP
	UnicastSend(strOhtAddr, iOhtPortNum, buf, len);
#endif

	return UNICAST_SEND_RESULT_SUCCESS;
}
//---------------------------------------------------------------------------

// OHT -> PC/TP Data send
int UdpCom::UnicastSend_OHT_2_TP(char* buf, int len)
{
	if(strTpAddr == "")
	{
		DisplayLog(L"PC/TP Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}

	UnicastSend(strTpAddr, TP_FIX_PORT_NUM, buf, len);

	return UNICAST_SEND_RESULT_SUCCESS;
}

//---------------------------------------------------------------------------

// PC/TP -> OHT Data send
/*
int UdpCom::UnicastSend_TP_2_OHT(char* buf, int len)
{
	if(strOhtAddr == "")
	{
		DisplayLog(L"PC/TP Address not Registed error!!!");
		return UNICAST_SEND_ADDRESS_NOT_RESISTED;
	}

	return UnicastSend(strOhtAddr, iPort, buf, len);

} */

//---------------------------------------------------------------------------
int UdpCom::OHT_TP_Bind()		// OHT - TP�� ��Ŷ�� �ޱ����� ����ϴ� ��Ʈ��ȣ Open
{
	int iRetVal = -1;

	if (pUdpTpServer == NULL)
		pUdpTpServer = new UdpServer();

#ifdef IS_OHT
	iRetVal = pUdpTpServer->Bind(OHT_S_TP_LISTEN_PORT_NUM);
#endif

#ifdef IS_TP
	iRetVal = pUdpTpServer->Bind(TP_FIX_PORT_NUM);
#endif

	if(iRetVal == 0)
	{
		DisplayLog(L"[OHT - TP Server ��Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OHT - TP Server ��Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[OHT - TP Server ��Ʈ] Bind ���� ����");
	}

	return iRetVal;
}



//---------------------------------------------------------------------------
int UdpCom::UnicastBind()
{
	int iRetVal;

	if (pUdpUniCastServer == NULL)
		pUdpUniCastServer = new UdpServer();

	iRetVal = pUdpUniCastServer->Bind(iMyUnicastPort);

	if(iRetVal == 0)
	{
		DisplayLog(L"[OCS - ���� �����Ʈ] Bind ���� ����");
	}
	else if(iRetVal == -3)
	{
		DisplayLog(L"[OCS - ���� �����Ʈ] �̹� Bind �Ǿ� �ֽ��ϴ�");
	}
	else
	{
		DisplayLog(L"[OCS - ���� �����Ʈ] Bind ���� ����");
	}

	return iRetVal;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastBind(int iUnicastListenPort)
{
#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyUnicastPort = iUnicastListenPort + 2;
	#else
		iMyUnicastPort = iUnicastListenPort;
	#endif
#else
	iMyUnicastPort = iUnicastListenPort;
#endif

	return UnicastBind();
}
//---------------------------------------------------------------------------

// TP�����Լ���. TP�� OHT�� �����Ҷ� �����
// OHT�� �ּҿ� ��Ʈ��ȣ�� ����صΰ�, ������ ���ۺ��� ����ϴ� ������
// �ڽ��� ������Ʈ�� ����δ� ������ �� (TCP���� �Լ� ��ü�� ���� �̷��� ����� ��)
int UdpCom::OHT_TP_Bind(AnsiString sOhtAddr, int iOhtPort)
{
	int iResult = OHT_TP_Bind();

	if(iResult == 0)
	{
		strOhtAddr  = sOhtAddr;
		iOhtPortNum = iOhtPort;
	}

	return iResult;
}

//---------------------------------------------------------------------------
int UdpCom::UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	AnsiString strTmp, strTmp2;
	int iReturn;

	// bind���κ��� �˻�
	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strTmp = "������ ���ŵ�. ��� IP : " + strClientAddr;
		strTmp2.sprintf(" ��� Port : %d ���� : %d ���� : %s",
						 iClientPort, iLen, buf);
		strTmp = strTmp + strTmp2;
		DisplayLog(strTmp);
	}
	else
	{
		strTmp.sprintf("������ ���ŵ��� ����. �����ڵ� : %d", iReturn);
		// DisplayLog(strTmp);
	}

	return iReturn;
}

//---------------------------------------------------------------------------

void UdpCom::OHT_TP_Close()
{
	if (pUdpTpServer != NULL)
	{
		pUdpTpServer->Close();
		delete(pUdpTpServer);
		pUdpTpServer = NULL;
	}
}
//---------------------------------------------------------------------------

void UdpCom::UnicastClose()
{
	if (pUdpUniCastServer != NULL)
	{
		pUdpUniCastServer->Close();
		delete(pUdpUniCastServer);
		pUdpUniCastServer = NULL;
	}
}


//---------------------------------------------------------------------------
// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ� (�μ� �����ϴ� ���)
int UdpCom::Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort)
{
#ifdef IS_OHT
	// OHT�� ����ȭ ����� ����Ǹ鼭 �߰��� (OHT���� ����)
	for(int i = 0; i < MAX_OCS_NUMBER; i++)
	{
		strOcsAddress[i] = "";		// OCS�� �ּ�. ó������ ����ִ�.
		strOcsMachineID[i] = "";	// OCS�� �ӽ� ID
	}
	iOcsCount = 0;

	// bOcsConnected = false;
#endif

	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP�� ��Ŷ�� ä���� BroadCast
	Pkt.identify = ADDRESS_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"�ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"�ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ� (�μ� �����ϴ� ���)
// - �ּ�ȹ��� ���� OCS�ּҸ� ����µ�, ����� ���۸� ���� �ʵ��� �ϴ� �Լ�
int UdpCom::Address_Broadcast_NotClear(int iMachineType, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP�� ��Ŷ�� ä���� BroadCast
	Pkt.identify = ADDRESS_BROADCAST_DEF;
	Pkt.MachineType = iMachineType;
	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());
	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
	{
		DisplayLog(L"�ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_FAIL;
	}
	else
	{
		DisplayLog(L"�ּ�ȹ�� Broadcast�޽��� ���� ����");
		return BROADCAST_SEND_RESULT_SUCCESS;
    }
}

//---------------------------------------------------------------------------
// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ� (���� ����Ʈ�� �����ϴ� ���)
int UdpCom::Address_Broadcast()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
	return Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
}

//---------------------------------------------------------------------------
// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ� (���� ����Ʈ�� �����ϴ� ���)
// - �ּ�ȹ��� ���� OCS�ּҸ� ����µ�, ����� ���۸� ���� �ʵ��� �ϴ� �Լ�
int UdpCom::Address_Broadcast_NotClear()
{
#ifdef	_ONE_PC_TEST
	return Address_Broadcast_NotClear(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
	return Address_Broadcast_NotClear(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
}

//---------------------------------------------------------------------------
// 3.3.3 MCP->CLW, Address drop order, Unicast
int UdpCom::Address_DropOrder_ToClw(AnsiString strIpAddr, AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, sizeof(AddressBroadcastPacket));

	// Step1. MCP�� ��Ŷ�� ä���� BroadCast
	Pkt.identify = ADDRESS_EXPIRE_ORDER;
	Pkt.MachineType = MACHINETYPE_MCP;

	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);

	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = UnicastSend(strIpAddr, iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
		return UNICAST_SEND_RESULT_FAIL;
	else
		return UNICAST_SEND_RESULT_SUCCESS;
}
//---------------------------------------------------------------------------
int UdpCom::Address_DropOrder_ToClw() 	// OHT 1��� ����ϹǷ�, �����ϰ� �̷��� �������̽� ��
{
#ifdef	_ONE_PC_TEST
	return Address_DropOrder_ToClw(VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, (iMyBroadCastPort + 2));
#else
	return Address_DropOrder_ToClw(VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID, iMyBroadCastPort);
#endif
}
//---------------------------------------------------------------------------

// 3.3.4 MCP->All CLW, Address drop order, Broadcast
int UdpCom::Address_DropOrder_AllClw(AnsiString strMachineID, int iPort)
{
	int iPacketLength = sizeof(AddressBroadcastPacket);

	AddressBroadcastPacket Pkt;
	memset(&Pkt, 0, iPacketLength);

	// Step1. MCP�� ��Ŷ�� ä���� BroadCast
	Pkt.identify = ADDRESS_EXPIRE_ALL;
	Pkt.MachineType = MACHINETYPE_MCP;

	memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
	memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

	// strcpy(Pkt.MachineID, strMachineID.c_str());
	// memcpy(&PacketBuf, &Pkt, iPacketLength);
	
	Conv_AddressBroadcastPacket(Pkt);
	int iReturn = BroadcastSend(iPort, (char*)&Pkt, iPacketLength);

	if (iReturn < 0)
		return BROADCAST_SEND_RESULT_FAIL;
	else
		return BROADCAST_SEND_RESULT_SUCCESS;

}

//---------------------------------------------------------------------------
int UdpCom::Address_DropOrder_AllClw()	// OHT 1��� ����ϹǷ�, �����ϰ� �̷��� �������̽� ��
{
#ifdef	_ONE_PC_TEST
	return Address_DropOrder_AllClw(strMyMachineId, (iMyBroadCastPort + 2));
#else
	return Address_DropOrder_AllClw(strMyMachineId, iMyBroadCastPort);
#endif
}
//---------------------------------------------------------------------------

#ifdef IS_MCP
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)	// strMachineID : �ڽ��� MachineID, iPort : ���� ��Ʈ��ȣ (����!!)
{
	AnsiString strMsg, strTmp;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];
	char oneChar1, oneChar2;

	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);

	if (iReturn > 0)
	{
		strMsg =  DateTimeToStr(Now());
		
		// (1) �����Ͱ� ���ǵ� ��Ŷ�԰ݿ� �´� ������ �����Ѵ�.
		// (1-1) ��Ŷ�� �ּ�ȹ��� ��Ŷ ũ������ Ȯ��. �ƴ϶�� �׳� ����
		iPacketSize = sizeof(AddressBroadcastPacket);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(%s) B/C ������Ŷ�� ũ�Ⱑ �����մϴ�. ����size : %d ����size : %d", strMsg, iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcastPacket Pkt;
		memcpy(&Pkt, &buf, iPacketSize);

		Conv_AddressBroadcastPacket(Pkt);	// Endian Convert

		// (1-2) OHT�� OCS���� ���� �������� Ȯ���Ѵ�.
		if((Pkt.identify != ADDRESS_BROADCAST_DEF) && (Pkt.identify != ADDRESS_UNICAST_REPLY))
		{
			strTmp.sprintf("(%s) B/C ������Ŷ�� ������ �����մϴ� (identify : %d)", strMsg, Pkt.identify);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

        // VHL ���� ����ȭ�� ���� Logic ����
//		if(Pkt.MachineType != MACHINETYPE_VHL)		// �� �κ��� OCS�� �ڵ��̸�, ���⸦ �������ٴ� �� VHL�� ���� �Ŷ�� ��
		if((Pkt.MachineType != MACHINETYPE_VHL_COMMON) && (Pkt.MachineType != MACHINETYPE_VHL_RATICLE) && (Pkt.MachineType != MACHINETYPE_VHL_PATROL))
		{   // �� �κп� ������ ���� �ٸ� OCS�� �ּ�ȹ�� B/C�� �� ���� ������ ���
			strTmp.sprintf("(%s) �ٸ� OCS�� VHL�ּҿ�û B/C (MachineType : %d)", strMsg, Pkt.MachineType);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST;
		}

		// (2) ������ ������ ���⼭ �ؼ��Ͽ� ��������
		//  �� �κ��� ��� ���� ������ �ʿ�������, �ϴ� �̷��Ը� �� ���� ����� ��������
		//  ���ķ����ʹ� 1�븸 ������ ���̹Ƿ� �����ϰ� �̷��� �س���
		VHL_LIST[0].strAddress = strClientAddr;
		memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
		cTemp[MACHINE_ID_LENGTH] = '\0';
		VHL_LIST[0].strMachineID = cTemp;
		iClwCount++;

		oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
		oneChar2 = Pkt.MachineType % 256;	// Lower Byte
		
		strTmp.sprintf("(%s) Packet Identify : 0\x%02X(%c) Machine Type : 0\x%02X(%c)(%c) Address : %s Machine ID : %s - Registed",
						strMsg, Pkt.identify, Pkt.identify, Pkt.MachineType, oneChar1, oneChar2, VHL_LIST[0].strAddress, VHL_LIST[0].strMachineID);
		DisplayLog(strTmp);

		// (3) VHL�� OCS�� ã�� Broadcast�� ���� �����ؾ� �Ѵ�.
		if (Pkt.identify == ADDRESS_BROADCAST_DEF)
		{
			memset(&Pkt, 0, iPacketSize);
			Pkt.identify = ADDRESS_UNICAST_REPLY;
			Pkt.MachineType = MACHINETYPE_MCP;
			
			memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));			
			memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());			
			// memcpy(&PacketBuf, &Pkt, iPacketSize);

						
			// (4) OCS�� ���� ��Ŷ�� �����ϰ� ������
			Conv_AddressBroadcastPacket(Pkt);	// Endian Convert
			UnicastSend(VHL_LIST[0].strAddress, iPort, (char*)&Pkt, iPacketSize);
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
int UdpCom::BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort + 2));	// ������ ���� ��ٰ� ���� (OCS : 9000, OHT : 9002)
	#else
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort));
	#endif
}
#endif


#ifdef IS_OHT

// VHL�� ���, �� �κ��� ����ȴ�
// MCP�� �ּ��ı� ���� ��, ������ �߰��ǹǷ� �� �Լ��ʹ� �и��Ǿ�� �Ѵ�.
//
// iMachineType = MACHINETYPE_MCP, MACHINETYPE_VHL
// strMachineID = ����ڰ� �Է��ص� ��ġ�̸�
// iPort = ȯ�漳���� �Է��ص� broadcast ��Ʈ��ȣ (1PC �ùķ��̼��� �ƴϸ� �����ϰ�, 1PC�� Define�� ���� �ڵ�����)
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	char cTemp[MACHINE_ID_LENGTH + 1];
	char oneChar1, oneChar2;
	int iReplyOcsNo;

try{

	iReturn = pUdpBroadCastServer->Listen(buf, iLen, strClientAddr, iClientPort);
	strAnsiAddr = strClientAddr;
	strMsg = DateTimeToStr(Now());

	if (iReturn > 0)
	{
		// (1) �����Ͱ� ���ǵ� ��Ŷ�԰ݿ� �´� ������ �����Ѵ�.
		// (1-1) ��Ŷ�� �ּ�ȹ��� ��Ŷ ũ������ Ȯ��. �ƴ϶�� �׳� ����
		iPacketSize = sizeof(AddressBroadcastPacket);
		if (iReturn != iPacketSize)
		{
			strTmp.sprintf("(%s) B/C ������Ŷ�� ũ�Ⱑ �����մϴ�. ����size : %d ����size : %d", strMsg, iPacketSize, iReturn);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		AddressBroadcastPacket Pkt;
		memcpy(&Pkt, &buf, iPacketSize);
		Conv_AddressBroadcastPacket(Pkt);	// Endian Convert

		// (1-2) OCS�� OHT���� ���� �������� Ȯ���Ѵ�.
		if((Pkt.identify != ADDRESS_BROADCAST_DEF) && (Pkt.identify != ADDRESS_UNICAST_REPLY)
			&& (Pkt.identify != ADDRESS_EXPIRE_ORDER) && (Pkt.identify != ADDRESS_EXPIRE_ALL))
		{
			strTmp.sprintf("(%s) B/C ������Ŷ�� ������ �����մϴ� (identify : %d)", strMsg, Pkt.identify);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
		}

		oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
		oneChar2 = Pkt.MachineType % 256;	// Lower Byte

		if(Pkt.MachineType != MACHINETYPE_MCP)		// �� �κ��� OHT�� �ڵ��̸�, ���⸦ �������ٴ� �� MCP�� ���� �Ŷ�� ��
		{	// ���� OHT�ε�, �ٸ� OHT�� B/C �۽ų����� �����. (�׷� ��������� �Ѵ�)
			strTmp.sprintf("(%s) �ٸ� ��ġ�� OCS�ּҿ�û B/C�̹Ƿ� ������ (MachineType : 0\x%02X(%c)(%c))", strMsg, Pkt.MachineType, oneChar1, oneChar2);
			DisplayLog(strTmp);
			return BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST;
		}

		// �ּ� �ߺ���� ������ �����ϱ� ���� ���⿡ �α׸� �����Ѵ�


		strTmp.sprintf("[OCS �ּ� �޽��� ����] (%s) Received OCS IP : %s  OCS IP Length : %d  identify : 0\x%02X(%c)",
						strMsg, strAnsiAddr, strAnsiAddr.Length(), Pkt.identify, Pkt.identify);
		DisplayLog(strTmp);

		strTmp.sprintf("[OHT �ּ� ��Ȳ] Ocs Count : %d  OCS1 Addr : %s  OCS1 Length : %d  OCS2 Addr : %s  OCS2 Length : %d",
						iOcsCount, strOcsAddress[0], strOcsAddress[0].Length(), strOcsAddress[1], strOcsAddress[1].Length());
		DisplayLog(strTmp);

		// (2) ������ ������ ���⼭ �ؼ��Ͽ� ��������

		// ���� ���̽��� ���� �б�
		switch(Pkt.identify)
		{
			case ADDRESS_BROADCAST_DEF:	// MCP�� CLW�� Ž���� ��� => �����ϸ� ����� �����Ѵ�
				{
					// (1) �ߺ��� ��� => �ּҸ� �߰��� ����� �ʿ�� ������, ���濡�� ������ �� �� �ʿ䰡 �ִ�.
					//                    ���� ������ ����� �Ŀ�, ������ ������ �ٽ� ���� ���ɼ��� �ֱ� �����̴�.
					bool bOverLap = false;
					
					for(int i = 0; i < iOcsCount; i++)
					{
						if(strOcsAddress[i].Trim() == strAnsiAddr.Trim())
						{
							bOverLap = true;
							iReplyOcsNo = i;
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) Received OCS Addr �ߺ����� �Ǵܵ�. Received Addr : %s", strAnsiAddr);
							DisplayLog(strTmp);
						}
					}

					// (2) ���� ��ϵ� ������ MAX�� ��� (�ߺ� ������ �ʿ��ϴ�)
					//		 => �𸣴� ����� ��� : �ּҸ� ������� �ʾƾ� �ϸ�, ���濡�� ���䵵 ���ʿ��ϴ�
					//		 => �ƴ�  ����� ��� : �ּҸ� ������� �ʾƾ� �ϳ�, ���濡�� ������ �� ��� �Ѵ�
					if(iOcsCount == MAX_OCS_NUMBER)
					{
						strTmp.sprintf("(ADDRESS_BROADCAST_DEF) OCS Limit reached!!, can't Regist!");
						DisplayLog(strTmp);

						if(bOverLap == false)
						{
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) �ߺ��ּҰ� �ƴ� ��3�� OCS ������. MAX�ʰ�. ���� �� �������� �ʰ� ������!!");
							DisplayLog(strTmp);
							return BROADCAST_LISTEN_RESULT_SUCCESS;
						}
						else
						{
							strTmp.sprintf("(ADDRESS_BROADCAST_DEF) �ߺ��ּҸ� ���� OCS �޽�����. ���丸 �ϰ� ���������� ����.");
							DisplayLog(strTmp);
						}
					}

					// (3) �׿��� ��� (Count = 0, 1) => ���� �ּҸ� �߰��Ѵ�.
					if(bOverLap == false)
					{
						iOcsCount++;
						strOcsAddress[iOcsCount - 1] = strAnsiAddr.Trim();
						iReplyOcsNo = iOcsCount - 1;
						strTmp.sprintf("(ADDRESS_BROADCAST_DEF) ���ο� OCS�ּ� �߰���.  Addr : %s  Length : %d  �Ҵ�� OCS NO : %d",
																strAnsiAddr, strAnsiAddr.Length(), iOcsCount);
						DisplayLog(strTmp);
					}

					// (4) �������� �ʰ� ������� ���� ������ �ʿ伺�� �ִ� ����̴�.
					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strOcsMachineID[iReplyOcsNo] = cTemp;

					memset(&Pkt, 0, iPacketSize);
					Pkt.identify = ADDRESS_UNICAST_REPLY;
					Pkt.MachineType = MACHINETYPE_VHL;
					memset(&Pkt.MachineID, ASCII_SPACE, sizeof(Pkt.MachineID));
					memcpy(&Pkt.MachineID, strMachineID.c_str(), strMachineID.Length());

					oneChar1 = Pkt.MachineType / 256;	// Uppder Byte
					oneChar2 = Pkt.MachineType % 256;	// Lower Byte

					strTmp.sprintf("[OCS�� ������Ŷ ����] Packet Identify : 0\x%02X(%c) Machine Type : 0\x%02X(%c)(%c) Address : %s MyMachine ID : %s ������ OCS NO (1,2) : %d ",
									Pkt.identify, Pkt.identify, Pkt.MachineType, oneChar1, oneChar2, strOcsAddress[iReplyOcsNo], strMachineID, (iReplyOcsNo + 1));
					DisplayLog(strTmp);

					// (4) OCS�� ���� ��Ŷ�� �����ϰ� ������
					Conv_AddressBroadcastPacket(Pkt);	// Endian Convert
					UnicastSend(strOcsAddress[iReplyOcsNo], iPort, (char*)&Pkt, iPacketSize);

					return BROADCAST_LISTEN_RESULT_SUCCESS;
				}
			//break;

			case ADDRESS_UNICAST_REPLY:		// CLW�� MCP�� Ž���� ��� => ������ ���常 �Ѵ�
				{
					// �ߺ��ּ����� �Ǵ��ϵ��� ����
					for(int i = 0; i < iOcsCount; i++)
					{
						if(strOcsAddress[i].Trim() == strAnsiAddr.Trim())
						{
							strTmp.sprintf("(ADDRESS_UNICAST_REPLY) Received OCS Addr �ߺ����� �Ǵܵ� . Received Addr : %s", strAnsiAddr);
							DisplayLog(strTmp);

							return BROADCAST_LISTEN_RESULT_SUCCESS;
						}
					}

					if(iOcsCount == MAX_OCS_NUMBER)
					{
						strTmp.sprintf("(ADDRESS_UNICAST_REPLY) OCS Limit reached!!, can't Regist!");
						DisplayLog(strTmp);
						return BROADCAST_LISTEN_RESULT_SUCCESS;
					}
					else
					{
						iOcsCount++;
					}

					strOcsAddress[iOcsCount - 1] = strAnsiAddr.Trim();
					strTmp.sprintf("(ADDRESS_UNICAST_REPLY) ���ο� OCS�ּ� �߰���. Addr : %s Length : %d �Ҵ�� OCS NO %d",
															strAnsiAddr, strAnsiAddr.Length(), iOcsCount);
					DisplayLog(strTmp);
					// bOcsConnected = true;

					memcpy(&cTemp, &Pkt.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strOcsMachineID[iOcsCount - 1] = cTemp;
				}
			break;

			case ADDRESS_EXPIRE_ORDER:		// MCP�� �ּ��ı� ����
				{
					DisplayLog(L"OCS�� �ּ��ı� ��û�� �����Ͽ� �ּҸ� ������");

					for(int i = 0; i < MAX_OCS_NUMBER; i++)
					{
						strOcsAddress[i] = "";
						strOcsMachineID[i] = "";
					}

					iOcsCount = 0;
					// bOcsConnected = false;

					// MCP�� �ּ��Ҵ��� ��û�ϴ� B/C�� ������
					DisplayLog(L"OCS���� �ּ��� �� ������ ��û�ϴ� Broadcast�޽����� ����");
#ifdef	_ONE_PC_TEST
					Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
					Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
				}
			break;

			case ADDRESS_EXPIRE_ALL:
				{
					DisplayLog(L"OCS�� ��� VHL�� �ּ��ı� ��û�� �����Ͽ� �ּ� ����");

					for(int i = 0; i < MAX_OCS_NUMBER; i++)
					{
						strOcsAddress[i] = "";
						strOcsMachineID[i] = "";
					}
					iOcsCount = 0;
					// bOcsConnected = false;

					DisplayLog(L"�� �κп� ���� ������ ��� ����, �����ϴ� ȣ���� ���� �Ѵ�");

					// MCP�� �ּ��Ҵ��� ��û�ϴ� B/C�� ������
#ifdef	_ONE_PC_TEST
					Address_Broadcast(iMyMachineType, strMyMachineId, (iMyBroadCastPort - 2));
#else
					Address_Broadcast(iMyMachineType, strMyMachineId, iMyBroadCastPort);
#endif
				}
			break;
		}
		return BROADCAST_LISTEN_RESULT_SUCCESS;
	}
	else
		return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
	}
	catch(...)
	{
    	DisplayLog(L"��ε��ɽ�Ʈ���� ���� �߻�");
	}

	return BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

#ifdef IS_OHT
int UdpCom::OcsConnected()
{
	// return bOcsConnected;
	return iOcsCount;
}
#endif

#ifdef IS_OHT
int UdpCom::BroadCast_ThreadCall()
{
	#ifdef	_ONE_PC_TEST
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort - 2));	// ���� ���� ��ٰ� ���� (OCS : 9000, OHT : 9002), �� 2�� ����� ��
	#else
		return BroadCast_ThreadCall(strMyMachineId, (iMyBroadCastPort));
	#endif
}
#endif

#ifdef IS_TP
int UdpCom::BroadCast_ThreadCall(AnsiString strMachineID, int iPort)
{
	ShowMessage("TP not using broad cast call!!");
	return 0;
}
#endif

#ifdef IS_TP
int UdpCom::BroadCast_ThreadCall()
{
	ShowMessage("TP not using broad cast call!!");
	return 0;
}
#endif


#ifdef IS_MCP
int UdpCom::UniCast_ThreadCall()
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strChk;
//	unsigned char oneChar;

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);	// �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		// Log Save (Big endian)
		strTmp = DateTimeToStr(Now());		
		strMsg.sprintf("[UDP Unicast RCV] Time = %s Rcv Byte = %d  Addr = %s  Port = %d ",strTmp, iReturn, strAnsiAddr, iClientPort); 
		DisplayLog(strMsg);
 #if 0
		strMsg = "";	// ����� ����
		for (int i = 0; i < iReturn; i++)
		{
			oneChar = buf[i];

			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
				strTmp.sprintf("[%d]0\x%02X(%c)  ", i, oneChar, oneChar);	// Hexǥ�÷� �ٲ�
			else
				strTmp.sprintf("[%d]0\x%02X  ", i, oneChar);

			if(((i % RAW_LOG_BYTE_1_LINE_SIZE) == 0) && (i != 0))
				strMsg = strMsg + '\r' + '\n';

			strMsg = strMsg + strTmp;
		}
		DisplayLog(strMsg);	
 #endif
		
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("[OHT->OCS ����] ������Ŷ�� ũ�Ⱑ �ؼ��ϱ⿡ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
		Conv_PKT_CHECK_TAG(Pkt);

		switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
		{
			case TagID::CLW_TO_MCP_STATUS:	// CLW�� OCS�� ���º��� (ID 03)
			{
				PKT_STATUS_REPORT_ID_03	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] CLW�� OCS�� ���º��� (ID 03) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_STATUS_REPORT_ID_03(CmdPkt);	// Endian��ȯ

				// ��Ʈ ����ü�� char�� ��ȯ
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� ���º��� (ID 03) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// �����Ͽ� ������ ���� (�տ��� Endian��ȯ �����Ƿ� ���⼭�� �ٷ� �������ָ� �ȴ�)
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			// break;

			case TagID::JOB_ORDER_RESPONSE:	// CLW�� OCS�� �۾����� ����  (ID 122)
			{
				PKT_JOB_ORDER_RESPONSE_ID_122	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_JOB_ORDER_RESPONSE_ID_122);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] CLW�� OCS�� �۾����� ����  (ID 122) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��				
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_JOB_ORDER_RESPONSE_ID_122(CmdPkt);

				// ��Ʈ ����ü�� char�� ��ȯ
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� �۾����� ����  (ID 122) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			// break;

			case TagID::JOB_PROGRESS_REPORT:	// CLW�� OCS�� ��ô����   (ID 103)
			{
				PKT_JOB_PROGRESS_REPORT_ID_103	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_JOB_PROGRESS_REPORT_ID_103);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] CLW�� OCS�� ��ô���� (ID 103) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_JOB_PROGRESS_REPORT_ID_103(CmdPkt);

				// ��Ʈ ����ü�� char�� ��ȯ
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� ��ô����   (ID 103) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			// break;

			case TagID::CMD_PASSPERMIT_OHT2OCS:	// CLW�� OCS�� ��� �㰡 �䱸   (ID 241)
			{
				PKT_PASSPERMIT_OHT2OCS_ID_241	CmdPkt;		// ��� �㰡 �䱸 ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_PASSPERMIT_OHT2OCS_ID_241);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] OCS�� ��� �㰡 �䱸 (ID 241) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(CmdPkt);

				// ��Ʈ ����ü�� char�� ��ȯ
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� ��� �㰡 �䱸   (ID 241) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASSPERMIT(CmdPkt);
				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			// break;


			case TagID::CMD_PASSPERMITOPEN_OHT2OCS:	// CLW�� OCS�� ��� �㰡 ���� �䱸   (ID 243)
			{
				PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243	CmdPkt;		// ��� �㰡 ���� �䱸 ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] OCS�� ��� �㰡 ���� �䱸 (ID 243) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243(CmdPkt);

				// ��Ʈ ����ü�� char�� ��ȯ
				memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
				memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
				memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� ��� �㰡 ���� �䱸   (ID 243) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value,
								Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case TagID::RESPONSE_PASSPERMITREQ_OHT2OCS:	// OHT -> OCS ��� �㰡 ���� ���� (ID:246)
			{
				PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246	CmdPkt;		// ��� �㰡 ���� ���� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] ��� �㰡 ���� ���� (ID:246) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("CLW�� OCS�� ��� �㰡 ���� ���� (ID:246) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Finish_PassPermitNum : %d",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, CmdPkt.BODY.Finish_PassPermitNum);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case TagID::RESPONSE_VERSIONREQ_OHT_OCS:	// OHT�� OCS���� �󼼹����� �˸�
			{
				PKT_VERSION_RESPONSE	CmdPkt;		// OHT�� ���� �˷��ֱ� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_VERSION_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] �󼼹����� �˸� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_VERSION_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strMsg = CmdPkt.BODY.Version;
				
				strTmp.sprintf("CLW�� OCS�� Version �󼼺��� (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Version : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, strMsg);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				
				// frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);

				return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case TagID::RESPONSE_SIMPLE_VERSION_OHT_OCS:		// OHT�� OCS���� ������ �˸�
			{
				AnsiString strMonitor, strAppVer, strParamVer, strMapVer;
				
				PKT_SIMPLE_VERSION_RESPONSE	CmdPkt; 	// OHT�� ���� �˷��ֱ� ��Ŷ���� �� �����Ѵ�
				iPacketSize = sizeof(PKT_SIMPLE_VERSION_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] ������ �˸� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_SIMPLE_VERSION_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				/*
				char cVersion[33];					// �ӽ� �޸�
				memset(&cVersion, ASCII_SPACE, 32);
				memcpy(&cVersion, &CmdPkt.BODY.ApplicationVersion, 32);
				cVersion[32] = '\0';
				*/


				strMonitor 	= CmdPkt.BODY.AppMonitorVersion;
				strAppVer 	= CmdPkt.BODY.ApplicationVersion;
				strParamVer = CmdPkt.BODY.ParameterVersion;
				strMapVer 	= CmdPkt.BODY.MapVersion;

				strTmp.sprintf("CLW�� OCS�� Version ���� (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT Body] Monitor : %s App : %s Param : %s  Map : %s",
								CmdPkt.HEADER.MachineType, strChk, CmdPkt.TAG.Value, strMonitor, strAppVer, strParamVer, strMapVer);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				// memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);
				
				// frmMcpMain->Protocol_Receiver_OHT_PASS_OPEN_PERMIT(CmdPkt);

				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			// break;

			case TagID::RESPONSE_DATATRANSFER_OHT2OCS:		// ������ ���� ���� ����
			{
				PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE	CmdPkt; 	
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] ������ ���� ���� ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS:		// ������ ���� ��ô ����
			{
				PKT_DATA_TRANSPER_ORDER_PROGRESS	CmdPkt; 	
				iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS ����] ������ ���� ��ô ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

			case TagID::REQ_DATATRANSFER_OHT2OCS:		// ������ ���� �䱸 (OHT -> OCS)
			{
				PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS	CmdPkt; 	
				iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] ������ ���� �䱸 - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("������ ���� �䱸 - �䱸���� : %d  ���۱��� : %d", 
								CmdPkt.BODY.Request.CMDType, CmdPkt.BODY.Request.TestMode);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)				
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				

				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
			//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
			case TagID::RESPONSE_AUTOCHANGE_OCS_OHT:	// auto Change  (ID:252)
			{
				PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252	CmdPkt;
				iPacketSize = sizeof(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] ������ȯ ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252(CmdPkt);

				memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
				cTemp[MACHINE_ID_LENGTH] = '\0';
				strChk = cTemp;
				strChk.Trim();

				strTmp.sprintf("������ȯ ���� - �䱸���� : %d  ���۱��� : %d",
								CmdPkt.BODY.AutoChangeACK.PresionCode, CmdPkt.BODY.AutoChangeACK.Accept_Error);
				DisplayLog(strTmp);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);

				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}

		/*
			case TagID::RESPONSE_VERSIONREQ_OHT_OCS:
			{
				PKT_VERSION_RESPONSE	CmdPkt; 	
				iPacketSize = sizeof(PKT_VERSION_RESPONSE);

				// ��Ŷ ũ�� ����
				if(iReturn != iPacketSize)
				{
					strTmp.sprintf("[OHT->OCS] ���� �� ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
				}

				// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
				memcpy(&CmdPkt, &buf, iPacketSize);
				Conv_PKT_VERSION_RESPONSE(CmdPkt);

				strTmp.sprintf("���� �� ���� : %s", CmdPkt.BODY.Version);

				// �����Ͽ� ������ ����
				PKT_OHT2OCS OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
				memcpy(&OhtCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�) 			
				frmMcpMain->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType, Pkt.TAG.Value, iPacketSize);				
				
				return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
			}
         */
			default:
			{
				strTmp.sprintf("Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d �����ּ� : %s ���� ��Ʈ : %d",
								Pkt.TAG.Value, strAnsiAddr, iClientPort);
				DisplayLog(strTmp);
				return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
			}
			// break;
		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif



#ifdef IS_OHT
int UdpCom::UniCast_ThreadCall(PKT_OCS2OHT& OcsCmdPkt)
{
	AnsiString strMsg, strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3, cVar4;

	char cTemp[MACHINE_ID_LENGTH + 1];
	AnsiString strConv;
	int iBig, iSmall;
	unsigned char oneChar;

try{

	iReturn = pUdpUniCastServer->Listen(buf, iLen, strClientAddr, iClientPort);	// �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..
	strAnsiAddr = strClientAddr;


	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		// Log Save (Big endian)		
//		strMsg.sprintf("[UDP Unicast RCV] Rcv Byte = %d  Addr = %s  Port = %d ",iReturn, strAnsiAddr, iClientPort);
//		DisplayLog(strMsg);
		
//		iBig 	= iReturn / RAW_LOG_BYTE_1_LINE_SIZE;
//		iSmall 	= iReturn % RAW_LOG_BYTE_1_LINE_SIZE;

		// 10�� ���� �α�����
//		for (int i = 0; i < iBig; i++)
//		{
//			strMsg = "";	// �ѹ� ��� �� �ݵ�� ����
//			for(int j = 0; j < RAW_LOG_BYTE_1_LINE_SIZE; j++)
//			{
//				oneChar = buf[(i * RAW_LOG_BYTE_1_LINE_SIZE) + j];
//
//				if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
//					strTmp.sprintf("%02X(%c)  ", oneChar, oneChar);	// Hexǥ�÷� �ٲ�
//				else
//					strTmp.sprintf("%02X  ", oneChar);
//
//				strMsg = strMsg + strTmp;
//			}
//			DisplayLog(strMsg);
//		}

		
		// 1�� ���� �α�����
//		strMsg = "";	// �ѹ� ��� �� �ݵ�� ����
//		for (int i = 0; i < iSmall; i++)
//		{
//			oneChar = buf[(iBig * RAW_LOG_BYTE_1_LINE_SIZE) + iSmall];
//
//			if((oneChar > RAW_LOG_ASCII_LOW_NUM) && (oneChar < RAW_LOG_ASCII_HIGH_NUM))
//				strTmp.sprintf("%02X(%c)  ", oneChar, oneChar);	// Hexǥ�÷� �ٲ�
//			else
//				strTmp.sprintf("%02X  ", oneChar);
//
//			strMsg = strMsg + strTmp;
//		}
//		DisplayLog(strMsg);
		

		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("[OCS->OHT ����] ������Ŷ�� ũ�Ⱑ �ؼ��ϱ⿡ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
		Conv_PKT_CHECK_TAG(Pkt);

		// if(Pkt.HEADER.MachineType == MACHINETYPE_VHL)
		{
			switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
			{
				case TagID::JOB_ORDER:	// �۾����� (ID:121)
				{
					PKT_JOB_ORDER_ID_121	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);

					// ��Ŷ ũ�� ���� (ID 121 Only Pass! - Variable length)
					if(iReturn > MAX_PACKET_SIZE)
					{
						strTmp.sprintf("[OCS->OHT ����] �۾����� ��Ŷ�� ũ�Ⱑ �ʹ� Ů�ϴ�. �ִ����� size : %d ����size : %d", MAX_PACKET_SIZE, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_JOB_ORDER_ID_121(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("�۾����� (ID:121) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				// break;
				case TagID::STATION_INFO_RESPONSE_OCS2OHT:	// Station �����䱸 ���� (ID:112)
				{
					PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112);

					// ��Ŷ ũ�� ���� (ID 121 Only Pass! - Variable length)
					if(iReturn > MAX_PACKET_SIZE)
					{
						strTmp.sprintf("[OCS->OHT ����] �۾����� ��Ŷ�� ũ�Ⱑ �ʹ� Ů�ϴ�. �ִ����� size : %d ����size : %d", MAX_PACKET_SIZE, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("Station�������� (ID:112) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				case TagID::MCP_TO_CLW_STATUS:	// MCP ���º��� (ID:01)
				{
					PKT_OCS_STATUS_ID_01	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] MCP ���º��� (ID:01) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OCS_STATUS_ID_01(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("MCP ���º��� (ID:01) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				// break;

				case TagID::RESPONSE_PASSPERMIT_OCS2OHT:	// ��� �㰡 �䱸 ���� (ID:242)
				{
					PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ��� �㰡 �䱸 ���� (ID:242) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("��� �㰡 �䱸 ���� (ID:242) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				// break;

				case TagID::CMD_PASSPERMITREQ_OCS2OHT:	// ����㰡���� (ID:245)
				{
					PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ����㰡���� (ID:245) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 1");
					DisplayLog(strTmp);
					// Sleep(200);

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245(CmdPkt);

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 2");
					DisplayLog(strTmp);
					// Sleep(200);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("����㰡���� (ID:245) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::MOVE_RESTART_OCS2OHT:	// ���� �簳  (ID:250)
				{
					PKT_MOVE_RESTART_OCS2OHT_ID_250	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_MOVE_RESTART_OCS2OHT_ID_250);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ���� �簳  (ID:250) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 1");
					DisplayLog(strTmp);
					// Sleep(200);

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_MOVE_RESTART_OCS2OHT_ID_250(CmdPkt);

					strTmp.sprintf("CMD_PASSPERMITREQ_OCS2OHT Step 2");
					DisplayLog(strTmp);
					// Sleep(200);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("�����簳 (ID:250) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				case TagID::RESPONSE_PASSPERMITOPEN_OCS2OHT:	// ��� �㰡 ���� �䱸 (ID:244)
				{
					PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ��� �㰡 ���� �䱸 (ID:244) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("��� �㰡 ���� �䱸 (ID:244) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}
				// break;
                case TagID::RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT:	// ���� ��� �㰡 ���� �䱸 (ID:248)
				{
					PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248	CmdPkt;		// �۾����� ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ���� ��� �㰡 ���� �䱸 ���� (ID:248) - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248(CmdPkt);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);

					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("���� ��� �㰡 ���� �䱸 (ID:248) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// memcpy(&OcsCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// frmOHTMain->StandardRead(OcsCmdPkt, OcsCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::CMD_VERSIONREQ_OCS_OHT:
				{
					PKT_OHT_VERSION_REQUEST	CmdPkt;
					iPacketSize = sizeof(PKT_OHT_VERSION_REQUEST);

					// ��Ŷ ũ�� ����
					/*
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ���� ��û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					*/

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OHT_VERSION_REQUEST(CmdPkt);
					
					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("OCS�� OHT�� �󼼹��� Ȯ�� ��û�� (1228) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::CMD_DATATRANSFER_OCS2OHT:	// ������ �������� (OCS -> OHT)
				{
					PKT_DATA_TRANSPER_ORDER_OCS2OHT	CmdPkt;
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ������ �������� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_DATA_TRANSPER_ORDER_OCS2OHT(CmdPkt);
					
					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("������ �������� (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::RESPONSE_INSERTREQ_OCS_OHT:	// OHT ���Կ䱸 ���� (OCS -> OHT)
				{
					PKT_OHT_INPUT_REQUEST_RESPONSE	CmdPkt;
					iPacketSize = sizeof(PKT_OHT_INPUT_REQUEST_RESPONSE);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] OHT ���Կ䱸 ���� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_OHT_INPUT_REQUEST_RESPONSE(CmdPkt);
					
					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("OHT ���Կ䱸 ���� (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::CMD_FLOORMOVE_OCS_OHT:	// ���� �̵����� (OCS -> OHT)
				{
					PKT_FLOOR_MOVE_ORDER	CmdPkt;
					iPacketSize = sizeof(PKT_FLOOR_MOVE_ORDER);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ���� �̵����� - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_FLOOR_MOVE_ORDER(CmdPkt);
					
					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("���� �̵����� (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::CMD_SIMPLE_VERSION_REQUEST:			// ������û (OCS -> OHT)
				{
					PKT_SIMPLE_VERSION_REQUEST	CmdPkt;
					iPacketSize = sizeof(PKT_SIMPLE_VERSION_REQUEST);

					// ��Ŷ ũ�� ����
					/*
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("[OCS->OHT ����] ������û - ��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					*/

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);
					Conv_PKT_SIMPLE_VERSION_REQUEST(CmdPkt);
					
					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.StatusOrder, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.StopOrder, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.MaintananceOrder, 1);
					memcpy(&cVar4, &CmdPkt.STATUS.DataTransOrder, 1);
					
					memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
					cTemp[MACHINE_ID_LENGTH] = '\0';
					strConv = cTemp;
					strConv.Trim();

					strTmp.sprintf("������û (OCS -> OHT) [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s DataTransOrder : %s",
									CmdPkt.HEADER.MachineType, strConv, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3),Char2BinString(cVar4));
					DisplayLog(strTmp);
					
					// �����Ͽ� ������ ����
					memcpy(&OcsCmdPkt, &CmdPkt, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				default:
				{
					strTmp.sprintf("Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d �����ּ� : %s ���� ��Ʈ : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;
			}
		}
	}
}
catch(...)
{
	strTmp.sprintf("Socket Exception");
	DisplayLog(strTmp);
}
	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}
#endif

// OHT���� TP�� ��Ŷ�� �����ϴ� �Լ���. (Endian ��ȯ ���ʿ�)
#ifdef IS_OHT
int UdpCom::OHT_TP_ThreadCall(PKT_OCS2OHT& OcsCmdPkt, char *szClientIP)
{
	AnsiString strTmp, strAnsiAddr;
	char buf[MAX_PACKET_SIZE+1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
//	unsigned char cVar1, cVar2, cVar3;

	iReturn = pUdpTpServer->Listen(buf, iLen, strClientAddr, iClientPort); // �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		if(szClientIP != NULL)
		{
			wchar_t *p = strClientAddr.c_str();
			wcstombs(szClientIP, p, strClientAddr.Length());
		}

		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�

		if(Pkt.HEADER.MachineType == MACHINETYPE_TP)
		{
			switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
			{
				case TAGCATEGORY::TAG_TP_ADDRESS_REGISTER:	// TP Address Auto Register
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Address auto Resigsted!! Address : %s ",strTpAddr);
					bTpConnected = true;

					DisplayLog(strTmp);
					return TP_ADDRESS_REGISTED;
				}
				// break;

				case TAGCATEGORY::TAG_TP_ADDRESS_DELETE:	// TP Address Delete Register
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Address Deleted!! ������ TP Address : %s ",strTpAddr);

					strTpAddr = "";
					bTpConnected = false;

					DisplayLog(strTmp);
					return TP_ADDRESS_DELETED;
				}
				// break;

				case TagID::CMD_TP_COMMON:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send Common command Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;				

				case TagID::CMD_E_STOP_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_E_STOP_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_MANUALMODE_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_MANUALMODE_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_AUTOMODE_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_AUTOMODE_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;

				case TagID::CMD_SOFTRESET_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_SOFTRESET_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// break;
				
				case TagID::CMD_STATUSREQ_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_STATUSREQ_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_INITREQ_TP_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_INITREQ_TP_OHT Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::JOB_ORDER_TP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send JOB_ORDER Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_MOVE_BY_MAP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send MOVE_BY_MAP Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}
				case TagID::CMD_PASSPERMITFORCEOPEN_OHT2TP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send ��� �㰡 ���� ���� �䱸 Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_IOMAP_REQ_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� OHT�� IO����Ʈ�� ��û Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_IOMAP_USE_ONOFF_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� IO�� ON, OFF�� ���Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_IOMAP_SET_RESET_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� IO�� 1, 0 ������ ���Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_TEACHING_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� ƼĪ�� OHT�� ƼĪ���� �����Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TEACHING_INFO_REQ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� OHT�� ƼĪ �����͸� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TRANS_LOAD:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� ���� LOAD (���� ��������)�� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}

				case TagID::CMD_TRANS_UNLOAD:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� ���� UNLOAD (��񿡼� ���� OHT�� �̵�)�� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }

					
					return iReturn;
				}

				case TagID::CMD_DRIVINGMAP_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� DIRIVINGMAP������ OHT�� �����Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_OACIS_INFO_SAVE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� OACIS������ OHT�� �����Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_DRIVINGMAP_INFO_REQ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� OHT�� DrivingMap �����͸� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TRANS_MOVE_TEACHING_POINT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� TEACHING POINT�� �̵��ϵ��� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}
				case TagID::CMD_STATIONINFOREQ_TP2OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send TP���� Station �䱸 �̵��� ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_CID_M_COMMAND_SEND:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� CID INFO ��û�Ѵ� Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_CID_M_COMMAND_RESULT_READ:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� CID������ �ø���� �о�޶�� ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_SOUND_ONOFF:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� SOUND On/OFF ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_POWER_ONOFF:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Power On/OFF ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				// ------- AutoTeaching ���� () ---------
				case TagID::CMD_TP_MAPMAKER_FILE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� MapMaker_File ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_FILE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� AutoTeaching_File ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� AutoTeaching ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_NULL_TYPE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� AutoTeaching_NullType ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AUTOTEACHING_READ_DRIVING:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� AutoTeaching ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				case TagID::CMD_TP_AMC_UPDATE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� amc update ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_OCS_PAUSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Pause by OCS ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_OCS_RESUME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Resume by OCS ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_TP_PAUSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Pause by TP ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_TP_RESUME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Resume by TP ��û  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_DEBUG_COMMAND:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Auto Teaching Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}


				// ------- �߿��� ������Ʈ ���� (������ ��������) ---------				 
				case TagID::CMD_DATATRANSFER_TP_OHT:	// ������ ���� ����
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("[TP Send] TP���� ������ ���� ���� ��û Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_VERSIONREQ_TP_OHT:		// ���� �� ����
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("[TP Send] TP���� ���� �� ���� ��û Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				// --------------------------------------------------------

				case TagID::REQ_DATATRANSFER_OHT_TP:	// TP���� ������ ���ۿ䱸 (TP -> OHT -> OCS)
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� ������ ���ۿ䱸 (TP -> OHT -> OCS)  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_MOVE_ALL_HOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Move All Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

                case TagID::CMD_TP_MOVE_HOIST_HOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Move All Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_AUTO_HOISTHOME:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� AUTO_HOISTHOME Home Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_AUTOMODE_CHANGE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Auto Mode Change Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//firt Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{ iReturn = 0; }
					
					return iReturn;
				}

				case TagID::CMD_TP_HID_LOG_SAVE_ORDER:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� HID Log Save Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
 				case TagID::CMD_TP_VIB_DATA_LINK:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_LINK Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_REQ_START:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_REQ_START Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_REQ_STOP:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_REQ_STOP Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_CLOSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP Send CMD_TP_VIB_DATA_CLOSE Address : %s ",strTpAddr);
					DisplayLog(strTmp);

					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_LOAD_VALUE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Hoist Param Load Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_SAVE_VALUE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Hoist Param Save Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_MOVE_TO_PULSE:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("TP���� Hoist Pulse Move Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);
					return iReturn;
				}
				
				case TagID::CMD_AUTOCHANGE_OCS_OHT:
				{
					strTpAddr = strClientAddr;
					strTmp.sprintf("Reset + Auto Mode Change Command Send  Address : %s ",strTpAddr);
					DisplayLog(strTmp);
					iPacketSize = sizeof(PKT_OCS2OHT);
					memcpy(&OcsCmdPkt, &buf, iPacketSize);

					//first Map Load Fail..
					if(OHTMainForm->GetOHTStatus()->DataUseFlag.fMapsetLoad == false)
					{
						strTmp.sprintf("Reset + Auto Mode Change Command Send  : Map fail!!");
						DisplayLog(strTmp);
						iReturn = 0;
					}
					
					return iReturn;
				}

				default:
				{
					strTmp.sprintf("TP/PC Packet Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d �����ּ� : %s ���� ��Ʈ : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
//					ShowMessage(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;

			}
		}
		// ---------------------- TP/PC Section end -----------------------------
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_OHT
bool UdpCom::TpConnected()
{
	return bTpConnected;
}
#endif



#ifdef IS_TP
int UdpCom::UniCast_ThreadCall()
{
	ShowMessage("TP Can't use UniCast_ThreadCall()");
	return -1;
}
#endif

#ifdef IS_TP
int UdpCom::OHT_TP_ThreadCall()
{
	AnsiString strTmp, strAnsiAddr;
	// char buf[MAX_PACKET_SIZE+1];
	char buf[MAX_BIG_PACKET_SIZE + 1];
	String strClientAddr;
	int iLen, iClientPort, iReturn, iPacketSize;
	unsigned char cVar1, cVar2, cVar3;

	iReturn = pUdpTpServer->Listen(buf, iLen, strClientAddr, iClientPort); // �� ������ �� �ʿ��� ���� �ƴ�����, �Ѱ��ִϱ�..
	strAnsiAddr = strClientAddr;

	if (iReturn > 0)	// ������ ������ �ִ��� Ȯ������
	{
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(iReturn < iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			strTmp.sprintf("��Ʈ�� ������Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d �����ּ� : %s ���� ��Ʈ : %d",
								 iPacketSize, iReturn, strAnsiAddr, iClientPort);
			DisplayLog(strTmp);
			return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
		}

		// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
		PKT_CHECK_TAG Pkt;
		memcpy(&Pkt, &buf, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�

        // VHL ���� ����ȭ�� ���� Logic ����
//		if(Pkt.HEADER.MachineType == MACHINETYPE_VHL)		// OHT -> TP
		if((Pkt.HEADER.MachineType >= MACHINETYPE_VHL_COMMON) && (Pkt.HEADER.MachineType <= MACHINETYPE_VHL_PATROL))		// OHT -> TP
		{
			switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
			{
				case TagID::CMD_TP_COMMON:
				{
					PKT_OHT2OCS	CmdPkt;		// normal ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_OHT2OCS);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW�� TP�� Command    [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					// MainForm->StandardRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�

					}
					// break;

				case TagID::CMD_STATUSREQ_TP_RESPONSE:
					{
//						PKT_OHT2TP_RESPONSE CmdPkt; 	// normal ��Ŷ���� �� �����Ѵ�
						iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);

						/*
						iPacketSize = sizeof(MSG_STATUS_OHT2TP);
						iPacketSize = sizeof(uTP_CMD);

						iPacketSize = sizeof(CMD_TP_JOG_DRV);
						iPacketSize = sizeof(CMD_TP_JOG_DIST);
						iPacketSize = sizeof(CMD_TP_JOG_TR);
						iPacketSize = sizeof(OHT_INFO);
						iPacketSize = sizeof(TEACHING_INFO);
						iPacketSize = sizeof(TEACHING_SAVE);
						iPacketSize = sizeof(MAPMAKE_INFO);   */

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

#if 0
						// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
						memcpy(&CmdPkt, &buf, iPacketSize);
	
						// ��Ʈ ����ü�� char�� ��ȯ
						memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
						memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
						memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
						strTmp.sprintf("CLW�� TP�� Command	  [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
										CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
										Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
						DisplayLog(strTmp);
#endif

						// �����Ͽ� ������ ����
						PKT_OHT2TP_RESPONSE OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
						
						SocketForm->UdpSocket_Status_Read(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
					
				}
				

				case TagID::CMD_INITREQ_TP_RESPONSE:
					{
//						PKT_OHT2TP_RESPONSE CmdPkt; 	// normal ��Ŷ���� �� �����Ѵ�
						iPacketSize = sizeof(PKT_OHT2TP_RESPONSE);
	
						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						// �����Ͽ� ������ ����
						PKT_OHT2TP_RESPONSE OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
						
						SocketForm->UdpSocket_Status_Read(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
					
				}

				case TagID::JOB_ORDER_RESPONSE_TP:	// �۾����� ����
				{
					PKT_OHT2OCS	CmdPkt;		// normal ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_OHT2OCS);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW�� TP�� �۾����� ����  [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

					// �� ���ο��� ���� ������ ��ġ�� �ڵ带 ȣ�����ش�
					// SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocket_Standard_Read(OhtCmdPkt);

					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�

					}

				case TagID::RESPONSE_PASSPERMITFORCEOPEN_TP2OHT:	// ��� �㰡 ���� ���� ����
				{
					PKT_OHT2OCS	CmdPkt;		// normal ��Ŷ���� �� �����Ѵ�
					iPacketSize = sizeof(PKT_OHT2OCS);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					// ���� ���� ��Ŷ���� Ȯ�εǸ� �α׿� �߿� ������ ǥ��
					memcpy(&CmdPkt, &buf, iPacketSize);

					// ��Ʈ ����ü�� char�� ��ȯ
					memcpy(&cVar1, &CmdPkt.STATUS.WorkingStatus, 1);
					memcpy(&cVar2, &CmdPkt.STATUS.RunningStatus, 1);
					memcpy(&cVar3, &CmdPkt.STATUS.Carriage, 1);
					strTmp.sprintf("CLW�� TP�� ��� �㰡 ���� ���� ���� [HEADER] MachineType : %d MachineID : %s Tag : %d [OHT STATUS] Working : %s Running : %s Carriage : %s",
									CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
									Char2BinString(cVar1), Char2BinString(cVar2), Char2BinString(cVar3));
					DisplayLog(strTmp);

					// �����Ͽ� ������ ����
					PKT_OHT2OCS	OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

					// �� ���ο��� ���� ������ ��ġ�� �ڵ带 ȣ�����ش�
					// SocketForm->UdpSocketRead(OhtCmdPkt, OhtCmdPkt.HEADER.MachineType);
					SocketForm->UdpSocket_Standard_Read(OhtCmdPkt);

					return iReturn;		// ������ ��Ŷ�� ũ�⸦ �����Ѵ�

					}

				case TagID::CMD_IOMAP_RESPONSE_OHT2TP:
					{
						iPacketSize = sizeof(PKT_OHT2TP_IOLIST);
	
						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT���� TP�� IO����Ʈ�� ����");

						// �����Ͽ� ������ ����
						PKT_OHT2TP_IOLIST OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
						
						SocketForm->UdpSocket_IO_LIST_Read(OhtCmdPkt);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�					
				}

				case TagID::CMD_CID_M_COMMAND_RESULT_RETURN:
				{
						iPacketSize = sizeof(PKT_OHT2TP_CID_INFO);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT���� TP�� CID INFO�� ����");

						// �����Ͽ� ������ ����
						PKT_OHT2TP_CID_INFO OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_CID_INFO(OhtCmdPkt);
						return iReturn;
				}
				break;

				case TagID::CMD_TP_MOVE_RESULT:
				{
						iPacketSize = sizeof(PKT_OHT2TP_MOVE_BY_MAP_RESULT);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT���� TP�� Map Move Result�� ����");

						// �����Ͽ� ������ ����
						PKT_OHT2TP_MOVE_BY_MAP_RESULT OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_MapMove_Result(OhtCmdPkt);
						return iReturn;
				}
				break;

				case TagID::CMD_MAP_MAKE_RESULT:
				{
						iPacketSize = sizeof(PKT_OHT2TP_LOG_DATA);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT���� TP�� Map Make Result�� ����");

						// �����Ͽ� ������ ����
						PKT_OHT2TP_LOG_DATA OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_MapMake_Result(OhtCmdPkt);
						return iReturn;
				}
				break;


				case TagID::CMD_TP_DEBUG_RESULT:
				{
						iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}
						
						DisplayLog("OHT���� TP�� Map Move Result�� ����");

						// �����Ͽ� ������ ����
						PKT_TP2OHT_SIMPLE_COMMAND OhtCmdPkt;
						memcpy(&OhtCmdPkt, &buf, iPacketSize);

						SocketForm->UdpSocket_AutoTeach_Result(OhtCmdPkt);
						return iReturn;
				}
				break;


				case TagID::CMD_TEACHING_INFO_LOAD:
					{
						iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	
						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� ����");


						// �����Ͽ� ������ ����
						PKT_OHT2TP_TEACHING_INFO OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

						SocketForm->DisplayTeachingData(OhtCmdPkt);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�					
				}

				case TagID::CMD_DRIVINGMAP_INFO_LOAD:
				{
						iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT�� ����Ǿ� �ִ� Driving Map�� TP�� ����");


						// �����Ͽ� ������ ����
						PKT_OHT2TP_DRIVINGMAP_INFO OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

						SocketForm->DisplayDrivingMapData(OhtCmdPkt);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}

				case TagID::CMD_TP_AUTOTEACHING_LOAD_DRIVING:
					{
						iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);

						// ��Ŷ ũ�� ����
						if(iReturn != iPacketSize)
						{
							strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
							DisplayLog(strTmp);
							return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
						}

						DisplayLog("OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� ����");


						// �����Ͽ� ������ ����
						PKT_OHT2TP_TEACHING_INFO OhtCmdPkt;					// CLW ���º��� ��Ŷ���� �� �����Ѵ�
						memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)

						SocketForm->DisplayAutoTeachingData(OhtCmdPkt);
						return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�
				}


				// ------- �߿��� ������Ʈ ���� (������ ��������) ---------	
				case TagID::RESPONSE_DATATRANSFER_OHT_TP:			// ������ ���� ���� ����
				{
					iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
			
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;

					}
			
					DisplayLog("TP�� ������ ���� ���� ���� ����");

					// �����Ͽ� ������ ����
					PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE OhtCmdPkt; 				// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					
					SocketForm->UdpSocket_DataTransper_Response_Read(OhtCmdPkt);
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�					
				}

				case TagID::REPORT_DATATRANSFERPROGRESS_OHT_TP:	// ������ ���� ��ô ����
				{
					iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);
			
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
			
					DisplayLog("TP�� ������ ���� ��ô ���� ����");
			
					// �����Ͽ� ������ ����
					PKT_DATA_TRANSPER_ORDER_PROGRESS OhtCmdPkt; 				// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					
					SocketForm->UdpSocket_Transper_Progress_Read(OhtCmdPkt);
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�					
				}

				case TagID::RESPONSE_VERSIONREQ_OHT_TP:			// ���� �� ���� 
				{
					iPacketSize = sizeof(PKT_VERSION_RESPONSE);
			
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
			
					DisplayLog("TP�� ���� �� ���� ����");
			
					// �����Ͽ� ������ ����
					PKT_VERSION_RESPONSE OhtCmdPkt; 				// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);	// OHTǥ�� ��Ŷ���� �����ش�. ��Ŷ������� ���� ��Ŷ�� ����� ����Ѵ� (�߿�)
					
					SocketForm->UdpSocket_VersionReq_Response_Read(OhtCmdPkt);
					return iReturn; 	// ������ ��Ŷ�� ũ�⸦ �����Ѵ�					
				}

				case TagID::CMD_TP_HID_LOG_SAVE_RESULT:
				{
					iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					DisplayLog("TP�� HID log save result ����");

					PKT_TP2OHT_SIMPLE_COMMAND OhtCmdPkt; 				// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_HID_LogSave_Result_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_TP_HOIST_TEACH_LOAD_RESULT:
				{
					iPacketSize = sizeof(PKT_TP2OHT_HOIST_TEACH_COMMAND);

					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}

					DisplayLog("TP�� Hoist param load result ����");

					PKT_TP2OHT_HOIST_TEACH_COMMAND OhtCmdPkt; 				// CLW ���º��� ��Ŷ���� �� �����Ѵ�
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_Hoist_Param_Result_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_VIBDATA_REQ_TP_RESPONSE:
				{
					iPacketSize = sizeof(PKT_OHT2TP_VIBRATION_INFO);
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					DisplayLog("TP�� AVS ���� data ����");
					PKT_OHT2TP_VIBRATION_INFO OhtCmdPkt;
					memcpy(&OhtCmdPkt, &buf, iPacketSize);

					SocketForm->UdpSocket_AVS_Vibration_Data_Read(OhtCmdPkt);
					return iReturn;
				}

				case TagID::CMD_TP_VIB_DATA_LINK:	// link success
				case TagID::CMD_TP_VIB_DATA_CLOSE:	// link fail or link disconnection
				{
					iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
					// ��Ŷ ũ�� ����
					if(iReturn != iPacketSize)
					{
						strTmp.sprintf("��Ŷ�� ũ�Ⱑ �ʹ� �۽��ϴ�. �ּ� size : %d ����size : %d", iPacketSize, iReturn);
						DisplayLog(strTmp);
						return UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID;
					}
					DisplayLog("AVS ���� DATA RESPONESE");
					SocketForm->UdpSocket_AVS_Response(Pkt.TAG.Value);
					return iReturn;
				}

				default:
				{
					strTmp.sprintf("Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d �����ּ� : %s ���� ��Ʈ : %d",
									Pkt.TAG.Value, strAnsiAddr, iClientPort);
					DisplayLog(strTmp);
					return UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID;
				}
				// break;
			}
		}
	}

	return UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND;
}

#endif

#ifdef IS_TP
bool UdpCom::OhtConnected()
{
	return bOhtConnected;
}
#endif

//---------------------------------------------------------------------------
void UdpCom::MyInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort)
{
	iMyMachineType = iMachineType;
	strMyMachineId = strMachineId;

#ifdef IS_OHT
	#ifdef	_ONE_PC_TEST
		iMyBroadCastPort = iBroadCastPort + 2;
		iMyUnicastPort = iUnicastPort + 2;
	#else
		iMyBroadCastPort = iBroadCastPort;
		iMyUnicastPort = iUnicastPort;
	#endif
#else  	// OCS ���ķ������� ���, �ڽ��� ��Ʈ��ȣ�� ������ �״�� ����Ѵ�
	iMyBroadCastPort = iBroadCastPort;
	iMyUnicastPort = iUnicastPort;
#endif
}
//---------------------------------------------------------------------------

void UdpCom::SetBroadCastMask(AnsiString strMask)
{
	pUdpBroadCastSender->SetMask(strMask);
}

// 20161216 sh17.jo : Commfail üũ ��� �߰�
void UdpCom::GetOcsAddress(char *OcsIP, int Ocs_Cnt)
{
    // OCS �ּҸ� �μ��� ����
    strcpy(OcsIP, strOcsAddress[Ocs_Cnt-1].c_str());
}
//---------------------------------------------------------------------------
