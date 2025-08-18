//---------------------------------------------------------------------------

#ifndef UdpDiagH
#define UdpDiagH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "Protocol_Define_Modify.h"
#ifdef IS_REAL_OHT
	#include "LogHandler.h"	// Log �ۼ� ����
#endif

//---------------------------------------------------------------------------
class UdpDiag
{
private:	// User declarations

#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
#endif
	void DisplayLog(String strLog);
	int DIAG_BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);
	// -------------- OHT ��źΰ� ����ϴ� ���� -------------------
	// OHT�� ����� ���� 
	// (2) PORT 9020 (OHT-DIAG Server)
	AnsiString strDIAG_SVR_Address;
	AnsiString strDIAG_SVR_MachineID;	// OHT�� ����س��� ���ܼ��� �̸�
										// "��"�� ���ܼ������, �� �̸��� �����ϰ� �ִ�
										// ���ܼ����� ����ϴ� OHT�� ������ DIAG_LIST[] �� ����Ǿ� �ִ�
	bool bDIAG_SVR_Connected; 	// OHT�� ���. DIAG Server�� ����Ǿ��ٴ� �÷���
	// -------------------------------------------------------------
	// --------------- ������ �� ����� ���� -----------------------
	AnsiString strMyMachineId;		// �� �̸� - OHT�� "OHT-01"��, OCS�� "OCS-01"
	// --------------- DIAG ������ �� ����� ���� -----------------------
	int iDIAG_MachineType;
	// AnsiString strDIAG_MachineId;
	int iDIAG_BroadCastPort;
	int iDIAG_UnicastPort;

	// -------------------------------------------------------------
	int UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);
	int BroadcastSend(int iPort, AnsiString strMessage);
	int BroadcastSend(int iPort, char* cMessage, int iLen);
	int UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);


// ====== �ּ�ȹ��  ���� �������̽� (���ܰ��� �Ϸ��� ��������)=======
	int DIAG_BroadcastBind();
	int DIAG_BroadcastBind(int iBroadCastListenPort);
	void DIAG_BroadcastClose();
	int DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort);
	int DIAG_BroadCast_ThreadCall();

	int Address_Broadcast();														// 3.3.1 MCP �Ǵ� CLW�� �ּ�ȹ�� (����Ʈ ������ �����Ѵ�)
	int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ�
// ===================================================================

public:		// User declarations
	UdpDiag();
	~UdpDiag();

// ==================== ����Ʈ ���� �������̽� ====================
	// �ڵ� ������ ����Ʈ���� �ѹ��� ����.
	// (�̰��� ȣ������ ������ �⺻ �������� ����. �ݵ�� ����� �ʿ�� ����)
	//  => �� �Լ��� Bind() ȣ������ ��������� �μ����� Bind����� ���� ����ȴ�.
	// 1PC ���� �ص� ���⼭ �Է��ϸ� �״�� ����ȴ�.
	// void DiagInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort);

// ==================== �ּ�ȹ��  ���� �������̽� ====================
	//int DIAG_BroadcastBind();
	//int DIAG_BroadcastBind(int iBroadCastListenPort);
	//void DIAG_BroadcastClose();
	//int DIAG_BroadCast_ThreadCall(AnsiString strMachineID, int iPort);
	//int DIAG_BroadCast_ThreadCall();

	//int Address_Broadcast();														// 3.3.1 MCP �Ǵ� CLW�� �ּ�ȹ�� (����Ʈ ������ �����Ѵ�)
	//int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ�
// ===================================================================
	int OHT_AS_Bind();
	void OHT_AS_Close();
	
#ifdef IS_OHT
	bool DIAG_Svr_Connected();
	int UnicastSend_OHT_2_AS(char* buf, int len);
	int OHT_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
#endif

#ifdef IS_AS
	int OHT_AS_ThreadCall(PKT_OCS2OHT& AS_Pkt);
	int UnicastSend_AS_2_OHT(char* buf, int len);
#endif
// ==================== Ŀ�ǵ� ��� ���� �������̽� ====================
	int UnicastBind();							// ��Ʈ�� ��Ʈ�� ����Ʈ ������ ���� ������ �� �Լ��� ȣ����
	int UnicastBind(int iUnicastListenPort);	// ��Ʈ�� ��Ʈ�� ��Ʈ��ȣ �����Ͽ� ���� ������ �� �Լ��� ȣ��
	void UnicastClose();
	int UnicastSend(char* buf, int len);

#ifdef IS_OHT
	int SetDiagInfo(AnsiString strSvrAddr, int iListenPortNum, AnsiString strMyName,int iOhtMachineType);
	int DIAG_UniCast_ThreadCall(PKT_OHT2DIAG&	OcsCmdPkt);
#endif

#ifdef IS_MCP
	int SetDiagInfo(AnsiString strOhtAddr, int iListenPortNum, AnsiString strMyName, int iDiagMachineType);
	int DIAG_UniCast_ThreadCall();
#endif

};

#endif
