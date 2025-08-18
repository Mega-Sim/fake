//---------------------------------------------------------------------------

#ifndef UdpFMH
#define UdpFMH

#include "Protocol_Define_Modify.h"

#include "SocketInitClose.h"
#include "UdpServer.h"
//#include "UdpBroadCastSender.h"
#include "UdpUniCastSender.h"
//---------------------------------------------------------------------------

class UdpFM
{
private:	// User declarations
	void DisplayLog(String strLog);
	int UnicastSend(AnsiString strIpAddr, int iPort, char* buf, int len);



	// �Ʒ� �� �Լ��� ��ý��Ͻ��� ���ϸŴ��� ���� ����Ѵ�
	int  AS_FILE_MANAGER_BIND();
	void AS_FILE_MANAGER_CLOSE();

public:		// User declarations
	UdpFM();
	~UdpFM();

#ifdef IS_AS  	// ���ϸŴ��� => ��ý��Ͻ��� ��� (OHT -> OCS ǥ�� ��Ŷ ���)
	int UnicastSend_AS_2_FM(char* buf, int len);
	int UnicastSend_AS_2_OHT(char* buf, int len);
	int FM_2_AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
//	int AS_ThreadCall(PKT_OHT2OCS& AS_Pkt);
	int AS_ThreadCall(char* AS_Pkt, int* piMachineType);
#endif

#ifdef IS_FM 	// ��ý��Ͻ� => ���ϸŴ��� (OCS -> OHT ǥ�� ��Ŷ ���)
	int UnicastSend_FM_2_AS(char* buf, int len);
	int AS_2_FM_ThreadCall(PKT_OCS2OHT& AS_Pkt);	
#endif

};

#endif
