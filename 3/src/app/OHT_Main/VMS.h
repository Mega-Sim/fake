//---------------------------------------------------------------------------

#ifndef VMSH
#define VMSH

#include "../Common/Protocol_Define_Modify.h"
#include "CommHandler.h"
#include "ErrorCode.h"

#include "Def_DefaultIniParam.h"
#include "OHTMainStatus.h"

#include "LogHandler.h"
#include "LogThread.h"

//---------------------------------------------------------------------------

/* Packet ����
|STX  | Header | Length | Data | CRC16 | ETX |
|1Byte| 1Byte  | 1Byte  | ���� | 2Byte |1Byte|
*/

#define	CR					0x0D
#define	LF					0x0A

// VMS Bluetooth ��ɾ�
#define BT_CMD_IDCHECK          "AT+BTINFO?"
#define BT_CMD_BTMODESET        "AT+BTMODE,0"
#define BT_CMD_MULTIMODESET     "AT+MULTI,2"
#define BT_CMD_CONNECT          "ATD"
#define BT_CMD_ONLINE           "ATO"
#define BT_CMD_DISCONNECT       "ATH"
#define BT_CMD_WAIT 	 	    "+++"
#define BT_CMD_SWRESET 	 	    "ATZ"
#define BT_CMD_HWRESET 	 	    "AT&F"

/**
@class   VMS
@date    2017.12.12
@author  kyh
@brief   VMS ���� Class
*/
class VMS
{
private:
	LogUnit *pLogTest;      ///< �α� ������ ���� ����
	Comm_Port *m_pCommPort;

	OHTMainStatus *m_pOHTMainStatus;
	DEFAULT_PARAM_SET *m_defualtparam;

	bool bConnect;

	// VMS�� �������� (OHT�� �˰� �ִ�)
    bool bVMSConnected;		// OHT�� ���. VMS�� ����Ǿ��ٴ� �÷���

public:
	VMS();
	~VMS();

	bool IsConnect();
	int AnalysisData();

	int OpenCom();
	int CloseCom();
	int ReOpenCom();

	int SetMode();
	int SetMultiMode();
	int Connect(AnsiString strBTID);
	int SetOnline();
	int Disconnect();
	int SetWait();

	int GetBTID();
	int SetSWReset();
	int SetHWReset();

	int GetData();//PKT_OCS2OHT& OcsCmdPkt);
	int SendData2VMS(unsigned char* SendData, int length);

	bool VMSConnected();
	void SetVMSDisconnect();

	void SuperviseSendCmdDataOHT2VMS();
};

#endif
