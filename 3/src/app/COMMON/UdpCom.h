//---------------------------------------------------------------------------

#ifndef UdpComH
#define UdpComH
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
class UdpCom
{
private:	// User declarations

#ifdef IS_REAL_OHT
	LogUnit *pLogUnit;
#endif
	void DisplayLog(String strLog);
	int BroadcastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);

	// -------------- OHT�� �������� (TP�� �˰� �ִ�) --------------
	AnsiString strOhtAddr;
	bool bOhtConnected;
	int iOhtPortNum;
	// -------------------------------------------------------------

	// -------------- OHT ��źΰ� ����ϴ� ���� -------------------
	// TP�� �������� (OHT�� �˰� �ִ�)
	AnsiString strTpAddr;
	AnsiString strTpMachineID;
	bool bTpConnected;		// OHT�� ���. TP�� ����Ǿ��ٴ� �÷���

	// OHT�� ����� ����
	AnsiString strOcsAddress[MAX_OCS_NUMBER];
	AnsiString strOcsMachineID[MAX_OCS_NUMBER];
	int iOcsCount;	
	// bool bOcsConnected; 	// OHT�� ���. OCS�� ����Ǿ��ٴ� �÷���
	// -------------------------------------------------------------
    // bool bOcsConnected;
	// --------------- MCP�� ����� ���� ---------------------------
	//AnsiString strClwList[50];
	int iClwCount;
	// -------------------------------------------------------------

	// --------------- ������ �� ����� ���� -----------------------
	int iMyMachineType;
	AnsiString strMyMachineId;
	int iMyBroadCastPort;
	int iMyUnicastPort;
	// -------------------------------------------------------------

	int UnicastSend(AnsiString strIpAddr, int iPort, AnsiString strMessage);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);

	int Address_DropOrder_AllClw(AnsiString strMachineID, int iPort);
	int BroadcastSend(int iPort, AnsiString strMessage);
	int BroadcastSend(int iPort, char* cMessage, int iLen);

	int UnicastListen(char* buf, int& iLen, String& strClientAddr, int& iClientPort);
	int Address_DropOrder_ToClw(AnsiString strIpAddr, AnsiString strMachineID, int iPort);

public:		// User declarations
	UdpCom(int PortNum);
	~UdpCom();

// ==================== ����Ʈ ���� �������̽� ====================
	// �ڵ� ������ ����Ʈ���� �ѹ��� ����.
	// (�̰��� ȣ������ ������ �⺻ �������� ����. �ݵ�� ����� �ʿ�� ����)
	//  => �� �Լ��� Bind() ȣ������ ��������� �μ����� Bind����� ���� ����ȴ�.
	// 1PC ���� �ص� ���⼭ �Է��ϸ� �״�� ����ȴ�.
	void MyInfoUpdate(int iMachineType, AnsiString strMachineId, int iBroadCastPort, int iUnicastPort);

// ==================== �ּ�ȹ��  ���� �������̽� ====================
	int BroadcastBind();							// �ּ�ȹ�� ��Ʈ�� ����Ʈ ������ ���� ������ �� �Լ��� ȣ����
	int BroadcastBind(int iBroadCastListenPort);	// �ּ�ȹ�� ��Ʈ�� ��Ʈ��ȣ�� �����Ͽ� ȣ����
	void BroadcastClose();
	int BroadCast_ThreadCall(AnsiString strMachineID, int iPort);	// strMachineID : �ڽ��� MachineID, iPort : ���� ��Ʈ��ȣ (����!!)
	int BroadCast_ThreadCall();

	int Address_Broadcast();														// 3.3.1 MCP �Ǵ� CLW�� �ּ�ȹ�� (����Ʈ ������ �����Ѵ�)
	int Address_Broadcast(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ�

	// - �ּ�ȹ��� ���� OCS�ּҸ� ����µ�, ����� ���۸� ���� �ʵ��� �ϴ� �Լ� (OHT)
	int Address_Broadcast_NotClear();														// 3.3.1 MCP �Ǵ� CLW�� �ּ�ȹ�� (����Ʈ ������ �����Ѵ�)
	int Address_Broadcast_NotClear(int iMachineType, AnsiString strMachineID, int iPort);	// 3.3.1 MCP�� CLW �ּҸ� ȹ���Ѵ� & 3.3.2 CLW�� MCP�ּҸ� ȹ���Ѵ�

	int Address_DropOrder_ToClw();													// OCS�� CLW���� �ּҸ� �����ϵ��� ������
	int Address_DropOrder_AllClw(); 												// OCS�� ��� CLW���� �ּҸ� �����ϵ��� ������

#ifdef IS_OHT
	int OcsConnected();															// OHT�� ���. OCS�� ���� Count 
#endif

    // 20161216 sh17.jo : Commfail üũ ��� �߰�
    void GetOcsAddress(char *OcsIP, int Ocs_Cnt);

// ==================== Ŀ�ǵ� ��� ���� �������̽� ====================
	int UnicastBind();							// ��Ʈ�� ��Ʈ�� ����Ʈ ������ ���� ������ �� �Լ��� ȣ����
	int UnicastBind(int iUnicastListenPort);	// ��Ʈ�� ��Ʈ�� ��Ʈ��ȣ �����Ͽ� ���� ������ �� �Լ��� ȣ��
	void UnicastClose();
	int UnicastSend(char* buf, int len);
	int UnicastSend_toMapView(AnsiString strMessage);
	void SetBroadCastMask(AnsiString strMask);

#ifdef IS_OHT
	int UniCast_ThreadCall(PKT_OCS2OHT&	OcsCmdPkt);
#endif

#ifdef IS_MCP
	int UniCast_ThreadCall();
#endif

#ifdef IS_TP
	int UniCast_ThreadCall();
#endif


// ==================== TP ���� �������̽� ====================
#ifdef IS_TP
	int TP_MAPVIEW_Bind();
#endif
	int OHT_TP_Bind(AnsiString sOhtAddr, int iOhtPort);
	int OHT_TP_Bind();									// OHT - TP�� ��Ŷ�� �ޱ����� ����ϴ� ��Ʈ��ȣ Open (����Ʈ��)
	void OHT_TP_Close();
	int UnicastSend_OHT_2_TP(char* buf, int len);		// OHT -> TP Send func. (OHT Only Use)

#ifdef IS_OHT
	int OHT_TP_ThreadCall(PKT_OCS2OHT& OcsCmdPkt, char *szClientIP = NULL);		// OHT - TP�� ��� ������ ȣ�� (OHT ����)
	bool TpConnected();									// OHT�� ���. TP�� ����Ǿ��ٴ� �÷���
#endif

#ifdef IS_TP
	int MAPVIEW_TP_ThreadCall();
	int OHT_TP_ThreadCall();							// OHT - TP�� ��� ������ ȣ�� (TP ����)
	bool OhtConnected();								// TP�� ����ϴ� ����. OHT�� ����Ǿ���. (���� �̱�����!)
#endif
};
//---------------------------------------------------------------------------

#endif

