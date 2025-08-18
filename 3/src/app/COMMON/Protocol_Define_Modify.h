#ifndef Protocol_Define_ModifyH
#define Protocol_Define_ModifyH

#include <vcl.h>
#include <windows.h>
#include <stdio.h>
//#include <math.h>
//#include "PUtily.h"
#include "Def_DataType.h"

//#include "Def_DataType.h"

// ---------------------------------------------------------------------------
//  �� �κп� �� ���α׷��� ����ϴ� �������� ������ �� ���� ��
//  ������ȣ�� 1���� �����ϴ� ���ڷ� �Ѵ�
#define OHT_REAL_VER		1	// Ver 1.0.0
#define OHT_EMUL_VER		1	// Ver 1.0.0
#define OCS_EMUL_VER		1	// Ver 1.0.0
#define NOTE_TP_VER			1	// Ver 1.0.0
#define ASSISTANCE_VER 		1	// Ver 1.0.0
#define FILEMANAGER_VER 	1	// Ver 1.0.0

// ��Ÿ���� �������� �������� (max) ���.
#define SUPPORT_PROTOCOL_VER 1  // Ver 1.0.0 (2016.12.01)

// �����ϰ� ���� �������� ���� ���
#define APPLY_PROTOCOL_VER 1  // Ver 1.0.0 (2016.12.01)


// �������� �̷��� �Ʒ��� �����ϰ� ���
/*
	2016.12.01 (0) : ���� zero. ��Ÿ�� ����Ȯ�� ��� �׽�Ʈ��
	2016.12.01 (1) : ���� 1.0.0 ����

*/
// ---------------------------------------------------------------------------

// LOG Save Defines
#define RAW_LOG_BYTE_1_LINE_SIZE	100	// RAW �α�1���δ� ������ ����Ʈ ��
#define RAW_LOG_ASCII_LOW_NUM		32	// RAW �α� ����� �ƽ�Ű ���� ���� (�̰ͺ��� Ŀ�� �ƽ�Ű�� ����)
#define RAW_LOG_ASCII_HIGH_NUM		126	// RAW �α� ����� �ƽ�Ű ���� ���� (�̰ͺ��� �۾ƾ� �ƽ�Ű�� ����)

// ��Ʈ��ũ�� �ȵǾ� ���Ƿ� IP�� �����ϴ� ����
// ����ϰ��� �ϸ� �� �κ��� �����ϰ�, ������Ʈ ������ _K_DEMO �� �־��� ��
// [OCS - OHT]
#define K_DEMO_VHL_ADDR 		"127.0.0.1"				// OCS���� �����. OHT�� �ּҸ� �ǹ�
#define K_DEMO_VHL_MACHINE_ID	"OHT001"				// OCS���� �����. OHT�� �̸��� �ǹ�

#define K_DEMO_OCS_ADDR 		"127.0.0.1"				// OHT���� �����. OCS�� �ּҸ� �ǹ�
#define K_DEMO_OCS_MACHINE_ID	"OCS001"				// OHT���� �����. OCS�� �̸��� �ǹ�

#define K_DEMO_OCS_ADDR_2 		"192.168.200.200"		// OHT���� �����. OCS�� �ּҸ� �ǹ�
#define K_DEMO_OCS_MACHINE_ID_2	"OCS002"				// OHT���� �����. OCS�� �̸��� �ǹ�

// [OHT - DIAG]
#define K_DEMO_DIAG_SVR_ADDR 	"192.168.100.100"		// OHT���� �����. ���ܼ����� �ּҸ� �ǹ�
#define K_DEMO_DIAG_SVR_MACHINE_ID	"SDG-01"		    // OHT���� �����. ���ܼ����� �ּҸ� �ǹ�

#define K_DEMO_DIAG_VHL_ADDR 		"192.168.100.100"	// ���ܼ������� �����. OHT�� �ּҸ� �ǹ�
#define K_DEMO_DIAG_VHL_MACHINE_ID	"OHT001"			// ���ܼ������� �����. OHT�� �̸��� �ǹ�

// [OCS - MTL]
#define K_DEMO_MTL_DOWN_ADDR 		"192.168.100.1"		// OCS���� �����. MTL(DOWN)�� �ּҸ� �ǹ�
#define K_DEMO_MTL_DOWN_MACHINE_ID	"MTL-01"			// OCS���� �����. MTL(DOWN)�� �̸��� �ǹ�

#define K_DEMO_MTL_UPPER_ADDR 		"192.168.100.1"		// OCS���� �����. MTL(UPPER)�� �ּҸ� �ǹ�
#define K_DEMO_MTL_UPPER_MACHINE_ID	"MTL-02"			// OCS���� �����. MTL(UPPER)�� �̸��� �ǹ�

#define K_DEMO_MTL_OCS_ADDR 		"192.168.100.100"	// MTL���� �����. OCS�� �ּҸ� �ǹ�
#define K_DEMO_MTL_OCS_MACHINE_ID	"OCS001"			// MTL���� �����. OCS�� �̸��� �ǹ�

// ---------------------------------------------------------------------------
// Default define �� ����
#define MAX_PACKET_SIZE	1024		// ��Ŷ �ִ�ũ��.
#define MAX_BIG_PACKET_SIZE	8192

#define MAX_PATH_SIZE    400        //puting Path Max
#define BROADCAST_IP   "192.167.255.255"//	"255.255.255.255"	// default       20161112 ��Ʈ��ũ ��ε����̽� ����
//#define BROADCAST_IP	"192.168.100.8"	// ocs_test_ver

#define MACHINE_ID_LENGTH	6

// Ocs ���� �� VHL ���� ����
#define MACHINETYPE_VHL_COMMON   12851      // 0x3233, ASCII '23'
//#define MACHINETYPE_VHL_RATICLE  12852		// 0x3234, ASCII '24'
#define MACHINETYPE_VHL_PATROL   12853      // 0x3235, ASCII '25'

// [OCS - OHT]
#define MACHINETYPE_MCP			12336		// 0x3030, ASCII '00'


#define MACHINETYPE_VHL 		MACHINETYPE_VHL_COMMON


#define MACHINETYPE_TP			   02
#define MACHINETYPE_HANDYTP        03
#define MACHINETYPE_MAPVIEW		   04
#define MACHINETYPE_VMS   		   05   // VMS
#define MACHINETYPE_BLACKBOX   	   06

// [OHT - DIAG]
#define MACHINETYPE_DIAG_SVR		0x0000		// AGENT ���ܼ��� �ĺ� ID (��缭 4.2����)
#define MACHINETYPE_DIAG_NORMAL_OHT 0x0101		// SEMES �Ϲ� OHT �ĺ� ID
#define MACHINETYPE_DIAG_CLEAN_OHT  0x0102		// SEMES û�� OHT �ĺ� ID

// [OCS - MTL]
#define MACHINETYPE_MTL				0x3630		// MTL�� ���� ID (ASCII '60')

// [Assistance - FileManager]
#define MACHINETYPE_AS				0x5000
#define MACHINETYPE_FM				0x5001

#define ASCII_SPACE			0x20	// blank (DEC 32)

#define TRANSTYPE_FLOORMOVE		0x06	///< Trans Type : Floor movement
#define TRANSTYPE_MTLMOVE		0x07	///< Trans Type : MTL movement

// ########################## �ּ� ȹ�� ���� ���� ###########################

// [OCS - OHT]
#define ADDRESS_BROADCAST_DEF		48 // 0		// �ּ����� B/C�϶� ��Ŷ�� identify�� �� ��
#define ADDRESS_UNICAST_REPLY		49 // 1
#define ADDRESS_EXPIRE_ORDER		50 // 2
#define ADDRESS_EXPIRE_ALL			51 // 4

// [OHT - DIAG]
#define ADDRESS_DIAG_BROADCAST_DEF		0x0000	// �ּ����� B/C�϶� ��Ŷ�� identify�� �� ��
#define ADDRESS_DIAG_UNICAST_REPLY		0x0001  // 1


// [OCS - OHT]
// 2016-12-14, Add by Labeler
// ROHT�� ���, �ӽ÷� 7.5 Test Bed���� ������ ���� ����
#define ADDRESS_GET_DEFAULT_PORT	9000// 9000      //Port ����
#define DEFAULT_BROADCAST_TIMEOUT   1000	// ms����.

#define CONTROL_DEFAULT_PORT		(ADDRESS_GET_DEFAULT_PORT + 1)
#define DEFAULT_CONTROL_TIMEOUT   	1000	// ms����
#define DEFAULT_STATUS_AUTOSEND_TIME  10	// �ʴ���

#define MAX_OCS_NUMBER				2	// ����ȭ�� ���� OCS�� ����� ���������� ���ΰ��� ����

// [OHT - DIAG]
#define ADDRESS_DIAG_GET_DEFAULT_PORT	9620
#define CONTROL_DIAG_DEFAULT_PORT		9621 //(ADDRESS_DIAG_GET_DEFAULT_PORT + 1)



// ---------- ��ε�ĳ��Ʈ Send��� ----------
#define BROADCAST_SEND_RESULT_SUCCESS	1
#define BROADCAST_SEND_RESULT_FAIL		0

// ---------- ��ε�ĳ��Ʈ Listen ��� ----------
// ���� : ��Ŷ�� �����Լ������� ��Ŷ�� ������ �����ϸ�, ������ ��Ŷ�� ����Ʈ ����
//         �����Ѵ�. ����, ������ ������ �׻� ���̳ʽ��� �����Ͽ��� �Ѵ�
#define BROADCAST_LISTEN_RESULT_SUCCESS					1
#define BROADCAST_LISTEN_RESULT_PACKET_SIZE_INVALID		-1
#define BROADCAST_LISTEN_RESULT_PACKET_CONTENT_INVALID 	-2
#define BROADCAST_LISTEN_RESULT_PACKET_NOT_FOUND		-3
#define BROADCAST_LISTEN_RESULT_OTHER_OHT_S_BROADCAST	-4
#define BROADCAST_LISTEN_RESULT_OTHER_OCS_S_BROADCAST   -5

// ---------- ����ĳ��Ʈ Send��� ----------
#define UNICAST_SEND_RESULT_SUCCESS		1
#define UNICAST_SEND_RESULT_FAIL		0
#define UNICAST_SEND_ADDRESS_NOT_RESISTED	-1	// �ּ�ȹ�� ������ ��ġ�� �ʾ�, ���� �˰��ִ� �ּҰ� ����

// ---------- ����ĳ��Ʈ Listen ��� ----------
// ���� : ��Ŷ�� �����Լ������� ��Ŷ�� ������ �����ϸ�, ������ ��Ŷ�� ����Ʈ ����
//        �����Ѵ�. ����, ������ ������ �׻� ���̳ʽ��� �����Ͽ��� �Ѵ�
// #define UNICAST_LISTEN_RESULT_SUCCESS					1	// �����ɽ�Ʈ Listen��, �����ϸ� ������ Byte���� �����ϹǷ�, �� ���Ǵ� ���ʿ� (����)
#define UNICAST_LISTEN_RESULT_PACKET_SIZE_INVALID		-1
#define UNICAST_LISTEN_RESULT_PACKET_CONTENT_INVALID 	-2
#define UNICAST_LISTEN_RESULT_PACKET_NOT_FOUND		-3
#define UNICAST_LISTEN_RESULT_OTHER_OHT_S_UNICAST	-4
#define UNICAST_LISTEN_RESULT_OTHER_OCS_S_UNICAST   -5
#define TP_ADDRESS_REGISTED							-6
#define TP_ADDRESS_DELETED							-7

// ---------- PC/TP define ------------
#define TP_FIX_PORT_NUM				9031
// #define TP_FIX_PORT_NUM				9031	// PC TP�� ����ϴ� ��Ʈ��ȣ (����), ���ܼ����� ���ļ� ���� 9020���� 9031�� ������
#define OHT_S_TP_LISTEN_PORT_NUM	9030	// OHT�� TP�� ��Ŷ�� �ޱ����� ����ϴ� ��Ʈ��ȣ (����)

// ---------- PC ~ ASSISTANCE define ----------
#define OHT_AS__OHT_PORT_NUM	9035	// OHT�� ��ý��Ͻ� �� ���ӿ� ����ϴ� ��Ʈ (OHT�� ������Ʈ)
#define OHT_AS__AS_PORT_NUM		9036	// OHT�� ��ý��Ͻ� �� ���ӿ� ����ϴ� ��Ʈ (��ý��Ͻ��� ������Ʈ)
#define OHT_AS__LOCAL_IP		"127.0.0.1"

// 22.05.04 JUKim added
// ---------- PC ~ AGENT define ----------
#define OHT_AGENT__OHT_PORT_NUM	        9073	// OHT�� Agent �� ���ӿ� ����ϴ� ��Ʈ (OHT�� ������Ʈ)
#define OHT_AGENT__AGENT_PORT_NUM		9074	// OHT�� Agent �� ���ӿ� ����ϴ� ��Ʈ (Agent�� ������Ʈ)
#define OHT_AGENT__LOCAL_IP		"127.0.0.1"

//AS ���Խ�
#if 1
// ---------- ASSISTANCE ~ FILE Manager define ----------
#define AS_2_FM_PORT			9037	// ��ý��Ͻ����� ���� �޽����� ���ϸŴ����� �����ϴ� ��Ʈ
//#define FM_2_AS_PORT			9038    // ���ϸŴ����� ��ý��Ͻ��� ���� �޽�����  �����ϴ� ��Ʈ
#define FM_2_AS_PORT			9036    // ���ϸŴ����� ��ý��Ͻ��� ���� �޽�����  �����ϴ� ��Ʈ
#else//AS ���ܽ�
// ---------- ASSISTANCE ~ FILE Manager define ----------
#define AS_2_FM_PORT			9036	// OHT���� ���� �޽����� ���ϸŴ����� �����ϴ� ��Ʈ
//#define FM_2_AS_PORT			9038    // ���ϸŴ����� ��ý��Ͻ��� ���� �޽�����  �����ϴ� ��Ʈ
#define FM_2_AS_PORT			9035    // ���ϸŴ����� OHT�� ���� �޽�����  �����ϴ� ��Ʈ

#endif

#define TP_FIX_PORT_NUM_FOR_MAPVIEW	9040
// ########################## �⺻ �������  ###########################
// 2���� ����� define
#define	bit_0 0x01
#define bit_1 0x02
#define bit_2 0x04
#define bit_3 0x08
#define bit_4 0x10
#define bit_5 0x20
#define bit_6 0x40
#define bit_7 0x80

// ------- 3����Ʈ ������ ���� ���� (Ȥ�� H-M-L������ �ڹٲ�ٸ�.. �̰ɷ� �ٷ� ���氡��....
//         ��, 3����Ʈ�� �ִµ� H-M-L ���� �˾Ҵµ� �ݴ��� �̰ɷ� ���� ���氡��
#define HIGH_BYTE 0
#define MID_BYTE  1
#define LOW_BYTE  2

// ------- 4Byte Order
#define FOUR_BYTE_HIGH  0
#define FOUR_BYTE_M1	1
#define FOUR_BYTE_M2	2
#define FOUR_BYTE_LOW	3

#define INIT  		0x00
#define FAIL  		0x01
#define SUCCESS  	0x02
#define REQ_INSERT 	0x02
#define COMPLETE  	0x03
#define REPORT      0x04

// ---------- FTP define ------------
#define FTP_MODE_OFF	(0)
#define FTP_MODE_ON		(1)
#define FTP_WIRELESS_UPDATE	(2)

// ---------- ������ ���� ���� ���� ------------
#define FTP_STATUS_NOP_DONE			(0x00) //����ó�� ���� or ���� �Ϸ�
#define FTP_STATUS_COMMUNICATION	(0x01) //FTP ������ ��� ��
#define FTP_STATUS_FILE_CHECK		(0x02) //������ üũ ó�� ��
#define FTP_STATUS_FILE_BACKUP		(0x03) //���� ���� ó�� ��

#define FTP_STEP_START			    (0x00) //����ó�� ����
#define FTP_STEP_FTP_PATH_CHECK	    (0x01) //FTP ���丮 ��� ���� �˻� �ܰ�
#define FTP_STEP_FTP_FILE_CHECK	    (0x02) //FTP �ٿ�ε�� ���� ���� �˻� �ܰ�
#define FTP_STEP_COMMUNICATION	    (0x03) //FTP ������ ��� ��
#define FTP_STEP_FILE_CHECK		    (0x04) //������ �˻�  ó�� ��
#define FTP_STEP_FILE_BACKUP	    (0x05) //���� ���� ó�� ��
#define FTP_STEP_MAP_DATA_LOAD	    (0x06) //���� ����  ó�� ��
#define FTP_STEP_END			    (0x07) //����ó�� �Ϸ�
#define FTP_STEP_DEFAULT		    (0xFF) //����ó�� �Ϸ� �� ��� ���

#define FILE_TRANS_SUCCESS_ON		(0x01) //������ ������ ���������� �Ϸ����� ��쿡 ON�Ѵ�.
#define FILE_TRANS_SUCCESS_OFF		(0x00) // MCP�������ۿϷ�Ȯ�Ρ��� ON�� ���� Ȯ���ϸ�, OFF�Ѵ�.

#define FILE_TRANS_FAIL_ON			(0x01) //������ ������ ������������ �������� ��쿡 ON�Ѵ�. 
#define FILE_TRANS_FAIL_OFF			(0x00) // MCP�������ۿϷ�Ȯ�Ρ��� ON�� ���� Ȯ���ϸ�, OFF�Ѵ�.

// ------------ ������� �߿��� ������Ʈ -------------
#define FTP_WIRELESS_DEFAULT		(0xFF) // Now work. wait state
#define FTP_WIRELESS_START			(0x00) // Connect
// #define FTP_WIRELESS_MODE_CHANGE	(0x01) // ftp mode change (normal -> bin)
#define FTP_WIRELESS_FILE_CONFIG	(0x02) // files put 
#define FTP_WIRELESS_FILE_MACFIL	(0x03) // files put 
#define FTP_WIRELESS_FILE_FIRMWARE	(0x04) // files put 
#define FTP_WIRELESS_RESET_CMD		(0x05) // Reset command send
#define FTP_WIRELESS_DISCONNECT     (0x06) // Disconnect & Ending. Step go default.

// ---------- ������ �������� ����(�����Ұ� ����) ----------
#define FTP_NOT_ACCEPT_NO_ERR          (0) // �����Ұ� ���� ����
#define FTP_NOT_ACCEPT_NO_SUPPORT_DATA (1) // ������ ������ ����Ʈ�ϰ� ���� �ʴ�.
#define FTP_NOT_ACCEPT_IP_ADDR_ERR     (2) // IP Address�� �ٸ��� �ʴ�.
#define FTP_NOT_ACCEPT_LOGIN_INFO_ERR  (3) // �α��� ������ �ٸ��� �ʴ�.
#define FTP_NOT_ACCEPT_DIRECTORY_ERR   (4) // ����ó�� �ٸ��� �ʴ�.
#define FTP_NOT_ACCEPT_FILE_ERR        (5) // ������ ������ �������� �ʴ´�.
#define FTP_NOT_ACCEPT_ORDER_ERR       (6) // ������ �����Ҽ� �ִ� ���°� �ƴϴ�.
										  // ���� ������ ������ ���� �䱸�� �ٸ���.
										  // --> OHT main���� ��ü ó��, Assistant�� ���ۿ䱸 ������ �Ȱ����� ����
#define FTP_NO_ACCEPT_TRAN_ORDER      (7) // �̹� �ٸ� �������ø� ������

// ---------- ������ �������� �䱸����(FTP->OHT)  ----------
#define FTP_CMDTYPE_MAPSET_TEACHING          (0x01) // �� ���� ��Ʈ ������
#define FTP_CMDTYPE_MAPSET_OPERATING         (0x02) // �� ���� ��Ʈ ����
#define FTP_CMDTYPE_OS                       (0x04) // OS
#define FTP_CMDTYPE_OS_SETTING               (0x08) // OS��������(����)
#define FTP_CMDTYPE_APPLICATION              (0x10) // ���ø����̼� ���α׷�
#define FTP_CMDTYPE_OPERATING_PARA           (0x20) // ��� �Ķ����
#define FTP_CMDTYPE_AXIS_PARA                (0x40) // �� �Ķ����(����)
// ---------- ������ �������� �䱸����(OHT->FTP)  ----------
#define FTP_CMDTYPE_LOGDATA                  (0x01) // �α׵�����
#define FTP_CMDTYPE_MAPSET                   (0x02) // �� ���� ��Ʈ
// ---------- ������ ���۱���  ----------
#define FTP_TESTMODE_FTP2OHT                  (0x00) // FTP -> OHT
#define FTP_TESTMODE_OHT2FTP                  (0x01) // OHT -> FTP

#define FTP_ERROR_ON                          (0x01) // �����Ұ� ��Ʈ : ���� �Ұ��� ��� ON
#define FTP_ERROR_OFF                         (0x00) // �����Ұ� ��Ʈ : ���� ������ ��� OFF


// ---------- ��Ŷ Ÿ�� ���� ----------
// �ּ�ȹ�� ��Ŷ

#pragma pack(push, 1) //�� ����Ʈ�� ��ŷ..

// [OCS - OHT]
typedef struct //AddressBroadcastPacket
{
	unsigned char 		identify;		// 1Byte, ���ڰ� ��
	unsigned short 		MachineType;	// 2Byte
	char 				MachineID[6];	// 6Byte
} AddressBroadcastPacket;

void Conv_AddressBroadcastPacket(AddressBroadcastPacket& Pkt)
{
	ConvertEndian(&Pkt.MachineType);
}

// [OHT - DIAG]
typedef struct 
{
	
	unsigned short 		MachineType;	// 2Byte, MACHINETYPE_DIAG_SVR, MACHINETYPE_DIAG_NORMAL_OHT
	char 				MachineID[6];	// 6Byte, Name 
	unsigned short 		CommandID;		// 2Byte, ID�ּ��� ����(ADDRESS_DIAG_BROADCAST_DEF), ����(ADDRESS_DIAG_UNICAST_REPLY) 
} AddressBroadcast_DIAG_Packet;

void Conv_AddressBroadcast_DIAG_Packet(AddressBroadcast_DIAG_Packet& Pkt)
{
	ConvertEndian(&Pkt.MachineType);
	ConvertEndian(&Pkt.CommandID);
}


// OCS�� �������� ����ü
// [OCS - OHT]
typedef struct
{
	AnsiString strAddress;
	AnsiString strMachineID;
} VHL_INFO;

// [OHT - DIAG]
typedef struct
{
	AnsiString strAddress;
	AnsiString strMachineID;
} DIAG_SVR_INFO;



// ------------------ ����� ��Ŷ�� ��ǰ���� --------------------
typedef struct
{
	// Header
	unsigned short	MachineType;	// 2Byte, ����
	char 			MachineID[6];	// 6Byte , �ӽ�ID
} PROTOCOL_HEADER;



typedef struct
{
	unsigned short	Value;			// 2Byte
} PROTOCOL_TAG;


/**
@brief   TAGCATEGORY ����
@author  puting
@date    2015.11.11
*/
typedef enum STANDARDCATEGORY_
{
	CATEGORY_PRIORITY 	= 1,	//�켱 ó�� ���(Auto, Manual, Reset �� _ Monitoring ���� ����) : �ڵ�/����, �⵿ On/off, ���� ������� ��ɵ�
	CATEGORY_PRIMARY,           //ù° ��� : Error�� ����, �ڵ�/����, �⵿ On/off���� ������� ��ɵ� : �켱ó���� ������ �ƴҶ� ó���� ������ ��ɵ�.
	CATEGORY_SECONDARY,			//�Ѥ� ��� : Error�� ���� ���� ���¿���, �⵿ On/off�� ������� ��ɵ�. : �޴��� ����
	CATEGORY_TERTIARY,          //��° ��� : Error�� ����, �⵿ On, �ڵ� �ϋ��� ó���Ǵ� ��ɵ�.  : OCS ��ɵ�
	CATEGORY_ETC
}STANDARDCATEGORY;


namespace TagID
{
	enum Reserved
	{
	// �۾����� ���� ��ɵ�
		MCP_TO_CLW_STATUS						= 1001,
		CLW_TO_MCP_STATUS						= 1003, // CLW Status ���� (CLW -> MCP)

		CMD_SIMPLE_VERSION_REQUEST				= 1081,	// ������û (OCS -> OHT) 
		RESPONSE_SIMPLE_VERSION_OHT_OCS			= 1082, // �������� (OHT -> OCS)

		CMD_STATION_INFO_REQUST_OHT2OCS		 	= 1123,
		STATION_INFO_RESPONSE_OCS2OHT			= 1124,
	
		JOB_ORDER								= 1121, // �۾����� (MCP -> CLW)
		JOB_ORDER_RESPONSE						= 1012, // �۾��������� (CLW -> MCP)
		JOB_PROGRESS_REPORT						= 1013, // ��ô���� (CLW -> MCP)    //��ô����� ���� 1103 ->1013 //AOHC-297

		JOB_WARNING_REPORT					    = 1029, // ����� ����    //AOHC-296
		
		CMD_PASSPERMIT_OHT2OCS					= 1241, // ��� �㰡 �䱸
		RESPONSE_PASSPERMIT_OCS2OHT				= 1242, // ��� �㰡 �䱸 ����
		CMD_PASSPERMITOPEN_OHT2OCS				= 1243, // ��� �㰡 ���� �䱸
		RESPONSE_PASSPERMITOPEN_OCS2OHT			= 1244, // ��� �㰡 ���� �䱸 ����
		CMD_PASSPERMITREQ_OCS2OHT				= 1245, // ��� �㰡 ����
		
		RESPONSE_PASSPERMITREQ_OHT2OCS  		= 1246, // ��� �㰡 ���� ����
		CMD_PASSPERMITFORCEOPEN_OHT2OCS 		= 1247, // ��� �㰡 ���� ���� �䱸
		RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT	= 1248, // ��� �㰡 ���� ���� ����
		
		REQ_DATATRANSFER_OHT2OCS				= 1221, // ������ ���� �䱸
		CMD_DATATRANSFER_OCS2OHT				= 1223, // ������ ���� ����
		RESPONSE_DATATRANSFER_OHT2OCS			= 1224, // ������ ���� ���� ����
		REPORT_DATATRANSFERPROGRESS_OHT2OCS		= 1225, // ������ ���� ��ô ����

		CID_ERROR_REPORT_OHT2OCS				= 1249, // CID �̻� ����
		MOVE_RESTART_OCS2OHT					= 1250, // ���� ���

		PASSPERMIT_CANCEL_OCS2OHT			= 1270, // ��� �㰡 ���      //soming
		RESPONSE_PASSPERMIT_CANCEL_OHT2OCS	        = 1271, // ��� �㰡 ��� ���� //soming

		CMD_FLOORMOVE_OCS_OHT					= 1115, // �����̵� ����
		RESPONSE_FLOORMOVE_OHT2OCS				= 1116, // �����̵� ���� ����
		
		CMD_INSERTREQ_OHT_OCS					= 1015, // ���� �䱸
		RESPONSE_INSERTREQ_OCS_OHT				= 1016, // ���� �䱸 ����

//		CMD_TAKEOUTREQ_OHT_OCS					= 1017, // ���� ����
		CMD_TAKEOUTREQ_OHT_OCS					= 17, // ���� ����   // OCS�� ���߱� ���� ����

		CMD_DEF_REQ_OHT_OCS						= 1019, // ��ȸ ���� �䱸

		CMD_VERSIONREQ_OCS_OHT					= 1227, // ���� �� ����
		RESPONSE_VERSIONREQ_OHT_OCS				= 1228,	 // ���� �� ����               04228
	
		CMD_AUTOCHANGE_OCS_OHT					= 1251, // manual -> auto ����
		RESPONSE_AUTOCHANGE_OCS_OHT 			= 1252, // manual -> auto ����

		CMD_CARRIERINFO_OHT_OCS                 = 1301, // Carrier ���� ��û
		RESPONSE_CARRIERINFO_OCS_OHT            = 1302, // Carrier ���� ��û ����

		CMD_TP_COMMON							= 44777,
		CMD_SOFTRESET_TP_OHT					= 14213,
		CMD_AUTOMODE_TP_OHT					   	= 14551,
		CMD_MANUALMODE_TP_OHT					= 14552,
		CMD_E_STOP_TP_OHT						= 14555,
		
		CMD_STATUSREQ_TP_OHT					= 14560, // OHT's Status request (from TP)
		CMD_STATUSREQ_TP_RESPONSE				= 14561, // OHT's Retponse (from OHT)

		CMD_INITREQ_TP_OHT						= 14562,
		CMD_INITREQ_TP_RESPONSE					= 14563,

		CMD_VIBDATA_REQ_TP_OHT					= 14564,
		CMD_VIBDATA_REQ_TP_RESPONSE				= 14565,

		CMD_FIRMWARE_VERSION_REQUEST_AGENT2OHT  = 15710, // �߿��� ������û (AGENT -> OHT)
		RESPONSE_FIRMWARE_VERSION_OHT_AGENT     = 15711, // �߿��� �������� (OHT -> AGENT)

		CMD_SIMPLE_VERSION_REQUEST_TP  			= 25081,	// ������û (OCS -> OHT)
		RESPONSE_SIMPLE_VERSION_OHT_TP			= 25082, // �������� (OHT -> OCS)

		JOB_ORDER_TP		   					= 22121, // �۾����� (MCP -> CLW)
		JOB_ORDER_RESPONSE_TP  					= 22012, // �۾��������� (CLW -> MCP)

		CMD_PASSPERMITFORCEOPEN_OHT2TP 			= 23247, // ��� �㰡 ���� ���� �䱸
		RESPONSE_PASSPERMITFORCEOPEN_TP2OHT		= 23248, // ��� �㰡 ���� ���� ����

		REQ_DATATRANSFER_OHT_TP					= 25221, // ������ ���� �䱸
		CMD_DATATRANSFER_TP_OHT					= 25223, // ������ ���� ����
		RESPONSE_DATATRANSFER_OHT_TP			= 25224, // ������ ���� ���� ����
		REPORT_DATATRANSFERPROGRESS_OHT_TP		= 25225, // ������ ���� ��ô ����

		CMD_VERSIONREQ_TP_OHT					= 25227, // ���� �� ����
		RESPONSE_VERSIONREQ_OHT_TP				= 25228, // ���� �� ����               04228

		CMD_IOMAP_REQ_TP2OHT					= 25230, // TP���� OHT�� IO����Ʈ�� ��û�Ѵ�
		CMD_IOMAP_RESPONSE_OHT2TP				= 25231, // OHT���� TP�� IO����Ʈ�� �����Ѵ�

		CMD_IOMAP_USE_ONOFF_TP2OHT				= 25232, // TP���� IO�� ON, OFF�� ���Ѵ�
		CMD_IOMAP_SET_RESET_TP2OHT				= 25233, // TP���� IO�� 1, 0 ������ ���Ѵ�

		CMD_TEACHING_INFO_SAVE					= 25234, // TP���� ƼĪ�� OHT�� ƼĪ���� �����Ѵ�
		CMD_TEACHING_INFO_REQ					= 25235, // TP���� OHT�� ƼĪ �����͸� ��û�Ѵ�
		CMD_TEACHING_INFO_LOAD					= 25236, // OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� �����Ѵ�

		CMD_TRANS_LOAD							= 25237, // ���ϸ� LOAD (���� ��������) �ϵ��� �Ѵ�
		CMD_TRANS_UNLOAD						= 25238, // ���ϸ� UNLOAD (��񿡼� ���� OHT�� �̵�) �ϵ��� �Ѵ�
		CMD_TRANS_MOVE_TEACHING_POINT			= 25239, // TEACHING POINT�� �̵��ϵ��� �Ѵ�

		CMD_STATIONINFOREQ_TP2OHT				= 25240, // TP���� Station ���� �䱸.
		CMD_DRIVINGMAP_INFO_SAVE				= 25253, // TP���� �����ϰ����ϴ� ����Map�� OHT�� �����Ѵ�
		CMD_DRIVINGMAP_INFO_REQ                 = 25254,
		CMD_DRIVINGMAP_INFO_LOAD				= 25255, // OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� �����Ѵ�

		CMD_OACIS_INFO_SAVE						= 25256, // OACIS ����� OHT�� �����Ѵ�

		CMD_WINDOW_OS_SHUTDOWN					= 25257, // Window OS ���� ��ư�� �����Ѵ�.

		CMD_TP_DUAL_STAGE_STB_TRANS				= 25569, // TP���� OHT�� �̵���� (2��STB �۾� ���)

		CMD_TP_MOVE_BY_MAP						= 25570, // TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)
		CMD_TP_MOVE_RESULT						= 25571, // TP���� OHT�� �̵���� Result
        CMD_TP_MOVE_LOAD                        = 25572, // TP���� OHT�� �̵� �� Load ���
		CMD_TP_MOVE_UNLOAD                      = 25574, // TP���� OHT�� �̵� �� Unload ���

		CMD_MAP_MAKE_RESULT						= 25575, // Map Make�� �������� OHT���� TP�� ����

        CMD_TP_MOVE_ALL_HOME                    = 25576,  // TP���� Hoist, Shift, Rotate, Shuttet, Hand Home ���
		CMD_TP_MOVE_HOIST_HOME 					= 25577,
        CMD_TP_AUTOMODE_CHANGE                  = 25578,  // TP���� �ڵ� Auto ��ȯ ���
		CMD_TP_OHT_TAKEOUT                      = 25580,  // TP���� OHT�� MTL�� Take Out ���
		CMD_TP_AUTO_HOISTHOME                   = 25581,  // TP���� Hoist  ������ �ڵ����� �������ִ� ���

		CMD_CID_M_COMMAND_SEND				    = 25600,
		CMD_CID_M_COMMAND_RESULT_READ			= 25601,
		CMD_CID_M_COMMAND_RESULT_RETURN			= 25602,

		CMD_CID_SETTING_SEND					= 25603,
		CMD_CID_SETTING_SEND_RETURN				= 25604,

		CMD_TP_SOUND_ONOFF						= 25610,
		CMD_TP_POWER_ONOFF						= 25611,

		CMD_TP_AMC_UPDATE						= 25612,

		CMD_TP_ERRORIGNOL_ONOFF					= 25613,  //AOHT-0001_FOUPCOVER

		CMD_TP_OCS_PAUSE						= 25615,
		CMD_TP_OCS_RESUME						= 25616,

		CMD_TP_TP_PAUSE 						= 25617,
		CMD_TP_TP_RESUME						= 25618,

		CMD_TP_DEBUG_COMMAND				= 25620,
		CMD_TP_DEBUG_RESULT					= 25621,

		CMD_TP_HID_LOG_SAVE_ORDER				= 25622,
		CMD_TP_HID_LOG_SAVE_RESULT				= 25623,

		CMD_FIRMWARE_UPDATE_RESTART_REQ_OHT2AS	= 25700,
		CMD_FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT  = 25701,
		CMD_FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT= 25702,

		CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS 	= 25703,
		CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT 	= 25704,

		CMD_TEACHING_LOOKDOWN_LEVEL_SET         = 25801, // 2016-12-27, Add by Labeler, LookDown Level ������ Tag �߰�

		CMD_TP_MAPMAKER_FILE                    = 25900,  // TP���� MapMaker Trigger(file ��)
//		CMD_TP_MAPMAKER   			            = 25902,  // TP���� MapMaker Trigger(file ��)
		CMD_TP_AUTOTEACHING_FILE                = 25902,  // TP���� MapMaker Trigger(file ��)
		CMD_TP_AUTOTEACHING                   	= 25903,  // TP���� MapMaker Trigger(file ��)
		CMD_TP_AUTOTEACHING_READ_DRIVING       	= 25904,  // TP���� MapMaker Trigger(file ��)
		CMD_TP_AUTOTEACHING_LOAD_DRIVING	  	= 25905,  // OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� �����Ѵ�
		CMD_TP_AUTOTEACHING_NULL_TYPE          	= 25906,  // ����ƼĪ ��������
		CMD_TP_AUTOTEACHING_VISION              = 25907,  // Vision�� �̿��� Auto Teaching

		CMD_TP_HOIST_TEACH_LOAD_VALUE			= 25910,  // TP -> OHT (hoist pameter)
		CMD_TP_HOIST_TEACH_SAVE_VALUE			= 25911,
		CMD_TP_HOIST_TEACH_MOVE_TO_PULSE		= 25912,
		CMD_TP_HOIST_TEACH_LOAD_RESULT			= 25913,  // OHT -> TP (hoist pameter)
		CMD_TP_CHECK_MATERIAL					= 25914,

		CMD_TP_VIB_DATA_LINK					= 25920,
		CMD_TP_VIB_DATA_REQ_START				= 25921,
		CMD_TP_VIB_DATA_REQ_STOP				= 25922,
		CMD_TP_VIB_DATA_CLOSE					= 25923,
		CMD_TP_VIB_DATA_ALIVE					= 25924,


		CMD_AUTO_DRIVING_TEST_ROUTE_REQ_TP2OHT              = 26931,
		RESPONSE_AUTO_DRIVING_TEST_ROUTE_OHT2TP  			= 26932,
		CMD_AUTO_DRIVING_TEST_MOVE_REQ_TP2OHT				= 26933,
		RESPONSE_AUTO_DRIVING_TEST_MOVE_OHT2TP  			= 26934,

		CMD_TP_TO_MAPVIEW_STATUS_REQ			= 30001,	//TP -> MAP VIEW ���¿�û
		CMD_MAPVIEW_TO_TP_STATUS_RESPONSE		= 30002,    //MAP VIEW -> TP ���¿�û����
		CMD_TP_TO_MAPVIEW_MAP_VERSION_REQ		= 30003,    //TP -> MAP VIEW UI ��û
		CMD_MAPVIEW_TO_TP_MAP_VERSION_RESPONSE	= 30004,    //MAP VIEW -> TP UI ��û����
		CMD_TP_TO_MAPVIEW_UI_UPDATE_REQ			= 30005,    //TP -> MAP VIEW UI ��û
		CMD_MAPVIEW_TO_TP_UI_UPDATE_RESPONSE	= 30006,    //MAP VIEW -> TP UI ��û����

		CMD_CONNECT_VMS_OHT_RESPONSE   			= 40001,    // VMS -> OHT ���� ����
		CMD_STATUS_OHT_VMS				   		= 40002,    // OHT -> VMS ���� ����
		CMD_CONNECT_OHT_VMS   					= 40003,    // OHT -> VMS ����
		CMD_DISCONNECT_OHT_VMS		   			= 40004,    // OHT -> VMS ��������

		CMD_STATUS_OHT_BLACKBOX   		     	= 41001,    // OHT -> BLACKBOX ���� ����
		CMD_TIMESETTING_BLACKBOX   				= 41002,    // OHT -> BLACKBOX �ð� ����
		CMD_TIMESETTING_BLACKBOX_RESPONSE   	= 41003,    // BLACKBOX -> OHT �ð� ���� ����
		CMD_EVENTSET_BLACKBOX   				= 41004,    // OHT -> BLACKBOX �̺�Ʈ �߻� ��û
		CMD_EVENTSET_BLACKBOX_RESPONSE   	    = 41005,    // BLACKBOX -> OHT �̺�Ʈ �߻� ��û ����
		CMD_ERROR_BLACKBOX   	    			= 41006,    // BLACKBOX -> OHT �̺�Ʈ �߻� ��û ����

		CMD_INERNAL_PING_REQ					= 50000, // ���� ���α׷��鳢�� ���� ������ �޾Ƽ� ���Ῡ�θ� Ȯ���Ѵ�
		CMD_INERNAL_PING_RESPONSE				= 50001,
		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};


// �⺻ �б���� ����
namespace DefaultDirection
{
	enum Reserved
	{
	// �۾����� ���� ��ɵ�
		DIR_LEFT						= 0,
		DIR_RIGHT						= 1,
		Reserver_2					= 2,
		Reserver_3					= 3,
		Reserver_4					= 4,
		Reserver_5					= 5,
		Reserver_6					= 6,
		Reserver_7					= 7,
		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};

namespace TAGCATEGORY	// ������ ����Ŭ
{
	enum Reserved
	{
		TAG_CUTLINE_OCSCMD_OUT	  =	0,
		TAG_CATEGORY_OCSWORKORDER   , 		///< Command Type : OCS �۾����� Tag
		TAG_CATEGORY_OCSPASSPERMIT 	, 		///< Command Type : OCS ����㰡 Tag
		TAG_CATEGORY_OCSFLOORMOVE 	, 		///< Command Type : OCS �����̵�/ �����̵� Tag
		TAG_CATEGORY_OCSCOMMON 		,    	///< Command Type : OCS Common Command ���� Tag
		TAG_CUTLINE_OCSCMD_IN			,		//< ���� �ڵ���� ĿƮ����

		TAG_CATEGORY_STATUS 	= 11,		///< Command Type : Status/Report Tag
		TAG_CATEGORY_INIT			,		///< Command Type : �ʱ�ȭ ���� ���� Tag
		TAG_CATEGORY_DATATRANSFER 	,    	///< Command Type : Data Transfer Command ���� Tag(Teaching, Station, IO Map ... Etc)
		TAG_CATEGORY_QUICK ,     			///< Command Type : Quick Command ���� Tag

		TAG_CUTLINE_PCCMD_OUT	  =	21,
		TAG_CATEGORY_TPWORKORDER 	, 		///< Command Type : PC(HandTP) �۾����� Tag
		TAG_CATEGORY_TPPASSPERMIT	 , 		///< Command Type : PC(HandTP) ����㰡 Tag
		TAG_CATEGORY_TPFLOORMOVE 	, 		///< Command Type : PC(HandTP) �����̵�/ �����̵� Tag
		TAG_CATEGORY_TPCOMMON 	    ,		///< Command Type : PC(HandTP) Common Command ���� Tag
		TAG_CUTLINE_PCCMD_IN	   	,		//< ���� �ڵ���� ĿƮ����
		TAG_TP_ADDRESS_DELETE		,
		TAG_TP_ADDRESS_REGISTER		,
		};
};



// ########################## �⺻���� ����ü ����  ###########################
// Header - Tag - Status���� �����Ѵ�. Body�� �����̹Ƿ� ���� �����ؾ� ��
// ǥ��ȭ ��翡 �ǰ�, ������� ���ǵ� ��������� �����Ͽ� ������

// ############## OHT Status �� ���� (��Ʈ�� ������ ���ϰ� ���ش�) #############

/**
@brief   WORKINGSTATUS ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct WORKINGSTATUS_
{
	BYTE NowRunning        		: 1; ///< �⵿��
	BYTE LayoutChangeRun   		: 1; ///< ���̾ƿ� ����� ������
	BYTE AbnormalTermination   	: 1; ///< �̻�
	BYTE Reserved_3  			: 1; ///< Reserved
	BYTE Auto  					: 1; ///< �ڵ�
	BYTE ReturnPossible  		: 1; ///< �ݼ۰���
	BYTE Reserved_6  			: 1; ///< Reserved
	BYTE Reserved_7  			: 1; ///< Reserved
} WORKINGSTATUS;

/**
@brief   RUNNINGSTATUS ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct RUNNINGSTATUS_
{
    BYTE CorrectPositionStop	: 1; ///< ����ġ ����
    BYTE Running		   		: 1; ///< ������
    BYTE BzWait				   	: 1; ///< BZ �����
    BYTE CrashAvoidWait  		: 1; ///< �ߵ����� �����
    BYTE SlowDownStop			: 1; ///< ���� ������
    BYTE Traffic_Sapche  		: 1; ///< ��ü
    BYTE Traffic_Jungche  		: 1; ///< ��ü
    BYTE CorrectPositionOvrRun	: 1; ///< ����ġ ������
} RUNNINGSTATUS;

/**
@brief   Carriage ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct CARRIAGE
{
    BYTE Load1        			: 1; ///< ����1
    BYTE Reserved_2   			: 1; ///< ����2(Reserved)
    BYTE DoubleInput   			: 1; ///< ���߹��� 
    BYTE BlankUnload  			: 1; ///< ������
    BYTE LoadUnloadHP  			: 1; ///< ����� HP
    BYTE Reserved_5  			: 1; ///< ����� HP2(Reserved)
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE E84Timeout  			: 1; ///< E84 Ÿ�Ӿƿ�
} CARRIAGE;

/**
@brief   MAINTANANCESTATE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct MAINTANANCESTATE_
{
	BYTE TakeOutOK        		: 1; ///< ���� OK
	BYTE TakeOutNG   			: 1; ///< ���� NG
	BYTE FloorMoveOk   			: 1; ///< �����̵� OK
	BYTE FloorMoveNg  			: 1; ///< �����̵� NG
	BYTE Rail_1_InputReq  		: 1; ///< Rail1 ����
	BYTE Rail_2_InputReq  		: 1; ///< Rail2 ����
	BYTE HtControlStopped  		: 1; ///< HT Control Stop
    BYTE BackMovingNow  		: 1; ///< ����
} MAINTANANCESTATE;

/**
@brief   CARRYTYPE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct CARRYTYPE_
{
    BYTE LoadTypeCheckingNow   	: 1; ///< �������� üũ��
    BYTE LoadType1   			: 1; ///< ���� ���� 1
    BYTE LoadType2   			: 1; ///< ���� ���� 2 
    BYTE LoadType3  			: 1; ///< ���� ���� 3
    BYTE LoadType4  			: 1; ///< ���� ���� 4
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE Reserved_7  			: 1; ///< Reserved
} CARRYTYPE;

/**
@brief   TUNDIRECTION ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct TUNDIRECTION_
{
    BYTE TurnRight        		: 1; ///< ������ �б�
    BYTE TurnLeft   			: 1; ///< ���� �б�
    BYTE TurnImpossible 	  	: 1; ///< �б� �Ұ� 
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6  			: 1; ///< Reserved
    BYTE PassConfirmGet			: 1; ///< Reserved
} TUNDIRECTION;

/**
@brief   WorkkingStatus ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct DATATRANSER_STATE_
{
    BYTE DataCmdID        		: 3; ///< ������ �������� ID
    BYTE DataType   			: 1; ///< ������ ���� ����
    BYTE DataStatus		 	  	: 2; ///< ������ ���ۻ��� ����  
    BYTE DataSuccess			: 1; ///< Data ���� ����
    BYTE DataFail  				: 1; ///< Data ���� ����
} DATATRANSER_STATE;

/**
@brief   Patrol ������ ���� ����ü
@author  puting
@date    2016.12.05
*/
typedef struct PATROL_STATE_
{
	BYTE LitzCableCheck   		: 1; ///< ������ �������� ID
	BYTE SloopCheck    			: 1; ///< ������ ���� ����
	BYTE oscillateCheck 	  	: 1; ///< ������ ���ۻ��� ����
	BYTE ParticleCheck			: 1; ///< Data ���� ����
	BYTE Mode					: 3; ///< Data ���� ����
	BYTE ErrorBit  				: 1; ///< Data ���� ����
} PATROL_STATE;

/**
@brief   WorkkingStatus ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct PATROL_STATE_2_
{
	BYTE Reserved        		: 1; ///< ������ �������� ID
	BYTE LeftRailMisMatch		: 1; ///< ������ ���� ����
	BYTE RightRailMisMatch	 	: 1; ///< ������ ���ۻ��� ����
	BYTE RegulatorTemper		: 2; ///< Data ���� ����
	BYTE CapBankTemper			: 2; ///< Data ���� ����
	BYTE LitzCableTemper 		: 1;
} PATROL_STATE_2;


namespace TEXTID	// Text ID
{
	enum Reserved
	{
		OCSSTATUS							= 1,
		OHTSTATUS							= 3,
		// TPSTATUS							= 4,
		INSERTREQ_RESPONSE					= 16,
		VERSION_REQ							= 81,
		VERSION_REQ_RESPONSE				= 82,
		STATION_INFO_REQUST					= 123,
		STATION_INFO_RESPONSE				= 124,
		PROGRESS_REPORT						= 13,     //AOHC-297
		WARNING_REPORT						= 29,     //AOHC-296
		JOBORDER		   					= 121,
		JOBORDER_RESPONSE					= 12,
		FLOORORDER			    			= 115,
		FLOORORDER_RESPONSE					= 116,
		REQ_DATATRANSFER					= 221,	// ������ ���� �䱸
		DATATRANSFERORDER					= 223,	// ������ ���� ����
		DATATRANSFERORDER_RESPONSE	   		= 224,	// ������ ���� ���� ����
		DATATRANSFERORDER_PROGRESS          = 225,	// ������ ���� ��ô ����
		VERSION_REQ_DETAIL 					= 227,	// ���� �� ����
		VERSION_REQ_RESPONSE_DETAIL			= 228,	// ���� �� ����
		IOMAP_REQ_TP2OHT					= 230,	// TP���� OHT�� IO����Ʈ�� ��û�Ѵ�
		IOMAP_RESPONSE_OHT2TP				= 231,	// OHT���� TP�� IO����Ʈ�� �����Ѵ�
		IOMAP_USE_ONOFF_TP2OHT				= 232,  // TP���� IO�� ON, OFF�� ���Ѵ�
		IOMAP_SET_RESET_TP2OHT				= 233,  // TP���� IO�� 1, 0 ������ ���Ѵ�	
		TEACHING_INFO_SAVE					= 234,	// TP���� ƼĪ�� OHT�� ƼĪ���� �����Ѵ�
		TEACHING_INFO_REQ					= 235,	// TP���� OHT�� ƼĪ �����͸� ��û�Ѵ�
		TEACHING_INFO_LOAD					= 236,	// OHT�� ����Ǿ� �ִ� ƼĪ���� TP�� �����Ѵ�
		TRANS_LOAD							= 237,  // ���ϸ� LOAD (���� ��������) �ϵ��� �Ѵ�
		TRANS_UNLOAD						= 238,  // ���ϸ� UNLOAD (��񿡼� ���� OHT�� �̵�) �ϵ��� �Ѵ�
		TRANS_MOVE_TEACHING_POINT			= 239,  // TEACHING POINT�� �̵��ϵ��� �Ѵ�
		STATIONINFOREQ_TP2OHT	        	= 240,  // Station ���� �䱸
		PASSPERMIT							= 241,
		PASSPERMIT_RESPONSE					= 242,
		PASSPERMIT_OPEN 					= 243,
		PASSPERMIT_OPEN_RESPONSE			= 244,
		PASSPERMIT_REQ	    				= 245,
		PASSPERMIT_REQ_RESPONSE				= 246,
		FORCE_PASSPERMIT_OPEN				= 247,
		FORCE_PASSPERMIT_OPEN_RESPONSE	    = 248,
		CID_ERROR_REPORT					= 249,  //CID �̻� ����
		MOVE_RESTART						= 250,  //���� ���		
		AUTOCHANGE_REQ						= 251, // manual -> auto ����
		AUTOCHANGE_RESPONSE					= 252, // manual -> auto ����
		DRIVINGMAP_INFO_SAVE				= 253,
		DRIVINGMAP_INFO_REQ					= 254,
		DRIVINGMAP_INFO_LOAD				= 255,

		OACIS_INFO_SAVE                     = 256,
		WINDOW_OS_SHUTDOWN                  = 257,
		PASSPERMIT_CANCEL                   = 270, //soming
		PASSPERMIT_CANCEL_RESPONSE          = 271, //soming
        CARRIERINFO_OHT_OCS                 = 301, //(OHT->OCS)Carrier ���� ��û
		RESPONSE_CARRIERINFO_OCS_OHT        = 302, //(OCS->OHT)Carrier ���� ��û ����

		TP_COMMON							= 777,
		SOFTRESET							= 213,
		AUTOMODE							= 551,
		MANUALMODE							= 552,
		E_STOP_PC							= 555,
		TPSTATUS							= 560,
		TP_INIT								= 562,

		TP_VIB_DATA							= 564,
		TP_VIB_DATA_RESPONSE				= 565,

		TP_DUAL_STAGE_STB_TRANS             = 569,

		TP_MOVE_BY_MAP                      = 570,  // TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)		
        TP_MOVE_LOAD                        = 572,  // TP���� OHT�� �̵� �� Load ���
        TP_MOVE_UNLOAD                      = 574,  // TP���� OHT�� �̵� �� Unload ���

		MAP_MAKE_RESULT						= 575,	// Map Make�� �������� OHT���� TP�� ����
		
		TP_MOVE_ALL_HOME                    = 576,  // TP���� Hoist, Shift, Rotate, Shuttet, Hand Home ���
		TP_MOVE_HOIST_HOME = 577,
		TP_AUTOMODE_CHANGE                  = 578,  // TP���� �ڵ� Auto Mode ��ȯ ���
		TP_OHT_TAKEOUT                      = 580,  // TP���� OHT�� MTL�� Take Out ���
		TP_AUTO_HOISTHOME                   = 581,  // TP���� Hoist  ������ �ڵ����� �������ִ� ���
		
		CID_M_COMMAND_SEND					= 600,	// Monitoring M Command Send
		CID_M_COMMAND_RESULT_READ			= 601,	// Monitoring M Result Read

		CID_SETTING_SEND					= 603,
		CID_SETTING_SEND_RETURN				= 604,

		TP_SOUND_ONOFF						= 610,
		TP_POWER_ONOFF						= 611,

		TP_AMC_UPDATE						= 612,
		TP_ERRORIGNOL_ONOFF             	= 613,   //AOHT-0001_FOUPCOVER

		TP_OCS_PAUSE						= 615,
		TP_OCS_RESUME						= 616,

		TP_TP_PAUSE							= 617,
		TP_TP_RESUME						= 618,

		TP_DEBUG_COMMAND				= 620,
		TP_DEBUG_RESULT 				= 621,

		TP_HID_LOG_SAVE_ORDER				= 622,
		TP_HID_LOG_SAVE_RESULT				= 623,

		FIRMWARE_UPDATE_RESTART_REQ_OHT2AS	= 700,
		FIRMWARE_UPDATE_CLOSE_ORDER_AS2OHT  = 701,
		FIRMWARE_UPDATE_AUTOMODE_ORDER_AS2OHT= 702,

		FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS 	= 703,
		FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT 	= 704,

		FIRMWARE_VERSION_REQUEST_AGENT2OHT = 710,

        TEACHING_LOOKDOWN_LEVEL_SET         = 801,

		TP_MAPMAKER_FILE                    = 900,  // TP���� MapMaker Trigger(file ��)
		TP_AUTOTEACHING_FILE                = 902,  // TP���� MapMaker Trigger(file ��)
		TP_AUTOTEACHING                   	= 903,  // TP���� MapMaker Trigger(file ��)
		TP_AUTOTEACHING_READ_DRIVING        = 904,
		TP_AUTOTEACHING_LOAD_DRIVING        = 905,
		TP_AUTOTEACHING_NULL_TYPE			= 906,
		TP_AUTOTEACHING_VISION				= 907,

		TP_HOIST_TEACH_LOAD_VALUE			= 910,  // OHT -> TP (hoist pameter)
		TP_HOIST_TEACH_SAVE_VALUE			= 911,
		TP_HOIST_TEACH_MOVE_TO_PULSE		= 912,
		TP_CHECK_MATERIAL					= 914,

		TP_VIB_DATA_LINK					= 920,
		TP_VIB_DATA_REQ_START				= 921,
		TP_VIB_DATA_REQ_STOP				= 922,
		TP_VIB_DATA_CLOSE					= 923,
		TP_VIB_DATA_ALIVE					= 924,

		TP_AUTO_DRIVING_ROUTE_REQ			= 931,
		TP_AUTO_DRIVING_ROUTE_RESPONSE		= 932,
		TP_AUTO_DRIVING_MOVE_REQ			= 933,
		TP_AUTO_DRIVING_MOVE_RESPONSE		= 934,

		VMSSTATUS							= 940,

		BLACKBOX_STATUS                     = 950,
	};
};


namespace CYCLE	// ������ ����Ŭ
{
	enum Reserved
	{
		CycleNotExist			= 0x00,	// ����Ŭ ����
		PositionSearching		= 0x01,	// ��ġȮ�� ����Ŭ ��
		MovingToStation			= 0x02, // �̵� ����Ŭ ��(Station)
		CraneUp					= 0x03, // �ø� ����Ŭ ��

		CraneDown				= 0x04, // ���� ����Ŭ ��
		TakeOut			    	= 0x05, // ���� ����Ŭ ��
		MovingToNode			= 0x08, // �̵� ����Ŭ ��(���)
		MovingBetweenFloor		= 0x09,	// ���� �̵� ����Ŭ ��(ID 101�� ��츸)
		MovingAutoSearch        = 0x10,

		Tour 					= 0x1A, // ���� ����Ŭ ��
		Tour_Node 				= 0x1B, // ���� �̵� ����Ŭ ��(Node)
		Tour_Station 			= 0x1C, // ���� �̵� ����Ŭ ��(Station)
		Tour_Acquire			= 0x1D, // ���� �ø� ����Ŭ ��
		Tour_Deposit			= 0x1E, // ���� ���� ����Ŭ ��

		Instrument				= 0x2C, // ���� ����Ŭ(Yellow VHL �����)
		CleanerVhlMove    		= 0x2D, // ���α� �̵� ����Ŭ ��(û�ұ� VHL �����)
		AreaMoving				= 0x2E, // ���� �̵� ����Ŭ(ID 101�� ��츸)
		SideStepMoving			= 0x2F,	// ���� �̵� ����Ŭ(ID 101�� ��츸)

		// ����������, ���� ���ÿ� ��ȿ������
		TP_RoundRunning			= 0x21, // ��ȸ ������ ����Ŭ ��(�̻��)
		TP_ManualOperation 		= 0x22, // ���� ���� ��
		TP_RunningLearning		= 0x23, // ���� �н� ����Ŭ ��
		TP_PortTeaching	    	= 0x24, // ����� �н� ��(ƼĪ��)

		TP_CheckPortTeaching	= 0x25, // �׽�Ʈ ����Ŭ ��(MOVE T)
		TP_CraneHome			= 0x26,	// �׽�Ʈ ����Ŭ ��(����2)(�̻��)
		TP_TestRunningLearning  = 0x27, // ���� �̵� �׽�Ʈ ����Ŭ

		TP_AutoMode				= 0x28,

		TP_AutoHoistHome		 = 0x29,
		TP_PortTeaching_NullType = 0x30,
		TP_SmalladdHoistHome 	 = 0x31,
        TP_MoveAllHome           = 0x32,
		TP_PortTeaching_Vision	 = 0x33, // ����� �н� ��(ƼĪ��)
		TP_CheckMaterial         = 0x34,

	};
};

// ��� ����Ŭ�� define �����۾��� ������ ���� �ӽ÷� ���Ҵ� ���̳� �ݵ�� �ʿ��� ���Ƕ� �ٽ� �츲
namespace WaitingCycle	// ��� ����Ŭ
{
	enum Reserved
	{
		CycleNotExist			= 0x00,	// ����Ŭ ����
		PositionSearching		= 0x01,	// ��ġȮ�� ����Ŭ ��
		MovingToStation			= 0x02, // �̵� ����Ŭ ��(Station)
		CraneUp					= 0x03, // �ø� ����Ŭ ��

		CraneDown				= 0x04, // ���� ����Ŭ ��
		TakeOut			    	= 0x05, // ���� ����Ŭ ��
		MovingToNode			= 0x08, // �̵� ����Ŭ ��(���)
		MovingBetweenFloor		= 0x09,	// ���� �̵� ����Ŭ ��(ID 101�� ��츸)

		Tour 					= 0x1A, // ���� ����Ŭ ��
		Tour_Node 				= 0x1B, // ���� �̵� ����Ŭ ��(Node)
		Tour_Station 			= 0x1C, // ���� �̵� ����Ŭ ��(Station)
		Tour_Acquire			= 0x1D, // ���� �ø� ����Ŭ ��
		Tour_Deposit			= 0x1E, // ���� ���� ����Ŭ ��

		Instrument				= 0x2C, // ���� ����Ŭ(Yellow VHL �����)
		CleanerVhlMove    		= 0x2D, // ���α� �̵� ����Ŭ ��(û�ұ� VHL �����)
		AreaMoving				= 0x2E, // ���� �̵� ����Ŭ(ID 101�� ��츸)
		SideStepMoving			= 0x2F,	// ���� �̵� ����Ŭ(ID 101�� ��츸)

		// ����������, ���� ���ÿ� ��ȿ������
		TP_RoundRunning			= 0x21, // ��ȸ ������ ����Ŭ ��(�̻��)
		TP_ManualOperation 		= 0x22, // ���� ���� ��
		TP_RunningLearning		= 0x23, // ���� �н� ����Ŭ ��
		TP_PortTeaching	    	= 0x24, // ����� �н� ��(ƼĪ��)

		TP_CheckPortTeaching	= 0x25, // �׽�Ʈ ����Ŭ ��(����1)
		TP_CraneHome			= 0x26,	// �׽�Ʈ ����Ŭ ��(����2)(�̻��)
		TP_TestRunningLearning  = 0x27, // �׽�Ʈ ����Ŭ ��(����3)(�̻��)

		TP_AutoMode				= 0x28,

		TP_AutoHoistHome		= 0x29,
		TP_PortTeaching_NullType = 0x30,
		TP_SmalladdHoistHome = 0x31,
		TP_MoveAllHome           = 0x32,
		TP_PortTeaching_Vision	 = 0x33, // ����� �н� ��(ƼĪ��)
		TP_CheckMaterial         = 0x34,
	};
};

namespace PROGRESS	// ��ô����
{
	enum Reserved
	{
		NotProgress				= 0x00,	// �ʱ� ����
		DrivingBegin	 		= 0x01,	// �̵� ����
		Arrived					= 0x02, // ����
		TransBegin				= 0x03, // ���� ����
		TransCompleted			= 0x04, // ���� �Ϸ�
		PositionCheckCompleted 	= 0x05, // ��ġ Ȯ�� �Ϸ�

		InpositionOverRun  		= 0x08, // ����ġ ������
		E84Timeout				= 0x09,	// E84 Timeout
		DoubleInsert			= 0x0A, // ���߹���
		EmptyShipment	     	= 0x0B, // ������.
		E84InterlockCompleted	= 0x0C, // E84 Interlock �Ϸ�    //PIO �Ϸ���������߰�.

		// �������̻�룽����
		HIG_Arrived				= 0x0D, // ���α� �̵��Ϸ�
		YellowVHLBegin	 		= 0x0E, // ���� ����
		YellowVHLCompleted		= 0x0F, // ���� ����
	};
};

namespace JobOderError	// �۾����� �󼼿��� �ڵ�
{
	enum Reserved
	{
		VHL_Turn_Off			= 0x81,	// �⵿ Off
		VHL_Manual		 		= 0x82,	// �޴���
		VHL_Error			   	= 0x83, // ����
		Path_Error				= 0x84, // (�б� ����Ʈ�� ����Ʈ�� �������� �������� ��)���ù��� ���ù��� ��� ������ ������ ���� ���������� ���������� ���� ������ ������ �Ѵ� .
		Taraget_Sam				= 0x85, // �̹� �������� �����ع������Ƿ� �޾� ���ϼ� ����.
		PositionCheckCompleted 	= 0x05, // ��ġ Ȯ�� �Ϸ�

		InpositionOverRun  		= 0x08, // ����ġ ������
		E84Timeout				= 0x09,	// E84 Timeout
		DoubleInsert			= 0x0A, // ���߹���
		EmptyShipment	     	= 0x0B, // ������.
		E84InterlockCompleted	= 0x0C, // E84 Interlock �Ϸ�

		// �������̻�룽����
		HIG_Arrived				= 0x0D, // ���α� �̵��Ϸ�
		YellowVHLBegin	 		= 0x0E, // ���� ����
		YellowVHLCompleted		= 0x0F, // ���� ����
	};
};

// �۾����� Nack�� ���������� ���� �з� �۾� puting
namespace JobChageNackType	// �۾����� Nack ���� ���� �ݿ�
{
	enum Reserved
	{
		CycleNotUse				= 0x00,	// �۾����� �Ұ��� //��Ÿ
		CycleChage_Type1		= 0x01,	// �۾����� ����/�Ұ��� : 3) �Ÿ��� ª�� ��� ����� ��� : O
		CycleChage_Type1_1 		= 0x03, // �۾����� ����/�Ұ��� : 3) ��ΰ� �߰��� �ٲ��� : O
		CycleChage_Type2 		= 0x02, // �۾����� ���� :  1) ������ ��ġ�� ����� ���  2) �ָ��̵��� ���  Nack -> Ack
	};
};


namespace ORDERDIV	// ���� ����
{
	enum Reserved
	{
		NotOrder				= 0,	// ���� ����.
		JobOrder		 		= 1,	// �۾� ����
		JobCancel		  		= 2, 	// �۾� ���
		JobChange				= 3, 	// �۾� ����
		TargetChange			= 4,    // ������ ����
		PathChange 				= 5,    // ��� ����
		JobClear 				= 9,    // �۾� ���
		JobCancel_2 	   		= 13,    // �۾� ���(�ű��߰�)
	};
};
/**
@brief   STATUSORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct STATUSORDER_
{
    BYTE MoveStart   			: 1; ///< �⵿
    BYTE LayoutChangeRunOrder 	: 1; ///< ���̾ƿ� ���� ���� ����
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE CraneDownAdmission 	: 1; ///< ���� ���� �㰡
    BYTE CraneUpAdmission  		: 1; ///< �ø� ���� �㰡
} STATUSORDER;


/**
@brief   STOPORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct STOPORDER_
{
    BYTE DrivingReduce			: 1; ///< ���� ���� ���� 
    BYTE DrivingEmergen			: 1; ///< ���� ��� ���� 
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE TrnasReduce  			: 1; ///< ������ ���� ����
    BYTE TransEmergen  			: 1; ///< ������ ��� ����
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE CycleUnable	  		: 1; ///< ����Ŭ ���� �Ұ�
} STOPORDER;

/**
@brief   MAINTANANCEORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct MAINTANANCEORDER_
{
    BYTE Insert_OK   			: 1; ///< Reserved
    BYTE Insert_NG 				: 1; ///< Reserved
    BYTE FloorTransfer_OK  		: 1; ///< Reserved
    BYTE FloorTransfer_NG		: 1; ///< Reserved
    BYTE Board_OK	  			: 1; ///< Reserved
    BYTE Board_NG	  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} MAINTANANCEORDER;

/**
@brief   MAINTANANCEORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct EMORDER_
{
    BYTE EM_Cut		   			: 1; ///< Reserved
    BYTE Reserved_1 			: 1; ///< Reserved
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} EMORDER;

/**
@brief   MAINTANANCEORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct DATATRANSORDER_
{
    BYTE CheckDataTransfer		: 1; ///< Reserved
    BYTE Reserved_1 			: 1; ///< Reserved
    BYTE Reserved_2   			: 1; ///< Reserved
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} DATATRANSORDER;

/**
@brief   MAINTANANCEORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct RUNNINGSTATUS2_
{
	BYTE CID_Interlock			: 1; ///< Reserved
	BYTE CID_Stop 				: 1; ///< Reserved
	BYTE Reserved_2   			: 1; ///< Reserved
	BYTE Reserved_3  			: 1; ///< Reserved
	BYTE Reserved_4  			: 1; ///< Reserved
	BYTE Reserved_5  			: 1; ///< Reserved
	BYTE OBSDetect	 			: 1; ///< Reserved
	BYTE OHTDetect		  		: 1; ///< Reserved
} RUNNINGSTATUS2;


namespace STOPReason	// Stop ���� ���� �󼼳��� �ۼ� �ʿ�
{
	enum Reserved
	{
		/*
		CycleNotExist			= 0x00,	// ����Ŭ ����
		PositionSearching		= 0x01,	// ��ġȮ�� ����Ŭ ��
		MovingToStation			= 0x02, // �̵� ����Ŭ ��(Station)
		CraneUp					= 0x03, // �ø� ����Ŭ ��

		CraneDown				= 0x04, // ���� ����Ŭ ��
		TakeOut			    	= 0x05, // ���� ����Ŭ ��
		MovingToNode			= 0x08, // �̵� ����Ŭ ��(���)
		MovingBetweenFloor		= 0x09,	// ���� �̵� ����Ŭ ��(ID 101�� ��츸)

		Instrument				= 0x2C, // ���� ����Ŭ(Yellow VHL �����)
		CleanerVhlMove    		= 0x2D, // ���α� �̵� ����Ŭ ��(û�ұ� VHL �����)
		AreaMoving				= 0x2E, // ���� �̵� ����Ŭ(ID 101�� ��츸)
		SideStepMoving			= 0x2F,	// ���� �̵� ����Ŭ(ID 101�� ��츸)

		// ����������, ���� ���ÿ� ��ȿ������
		TP_RoundRunning			= 0x21, // ��ȸ ������ ����Ŭ ��(�̻��)
		TP_ManualOperation 		= 0x22, // ���� ���� ��
		TP_RunningLearning		= 0x23, // ���� �н� ����Ŭ ��
		TP_PortTeaching	    	= 0x24, // ����� �н� ��(ƼĪ��)

		TP_TestPattern1			= 0x25, // �׽�Ʈ ����Ŭ ��(����1)
		TP_TestPattern2			= 0x26,	// �׽�Ʈ ����Ŭ ��(����2)(�̻��)
		TP_TestPattern3			= 0x27, // �׽�Ʈ ����Ŭ ��(����3)(�̻��)
		*/
		};
};

/**
@brief   OHT->OCS Status ������ ���� ����ü
@date    2015.10.27
*/
typedef struct PROTOCOL_STATUS_OHT_
{
	WORKINGSTATUS		WorkingStatus;		// ��������
	RUNNINGSTATUS		RunningStatus;		// �������
	CARRIAGE			Carriage;			// ����� ����
	MAINTANANCESTATE	MaintananceState;	// �������� ����
	unsigned char		CarryType;			// ���� �� ���ϻ���
	unsigned char		CurrentAddr[3];		// �������
	unsigned char		StopNowAddr[3];		// ������ station ����
	unsigned char		DestinationAddr[3]; // ������ station ����
	TUNDIRECTION		TunDirection;		// �б����
	unsigned char		RunningCycle;		// ������ ����Ŭ
	unsigned char		WaitingCycle;		// ��� ����Ŭ
	short				ErrorCode;			// �����ڵ�
//	unsigned char		Error_Reserved[2];  // �����ڵ�
//	unsigned char		EmStatus;			// E/M����
	DATATRANSER_STATE	DataTransStatus;	// ������ ���ۻ���

	unsigned char		APSinalLevel;		// ���İ���
    short				RunningDistance;	// ����Ÿ� (mm)  - Offset��
	unsigned short		RunningSpeed;		// ����ӵ� (mm/s)
	PATROL_STATE		Patrol_State;		//��Ʈ�� ����
	PATROL_STATE_2		Patrol_State_2;		//��Ʈ�� ����
	RUNNINGSTATUS2		RunningVHLStatus;		//CID����
	unsigned char		Dummy[2];			// 30����Ʈ�� ����� �ϹǷ� ���� ������ ����
} PROTOCOL_STATUS_OHT;

typedef struct PROTOCOL_FW_VERSION_OHT_
{
	int RSADrivingRear;
	int RSADRivingFront;
	int RSAHoist;
	int RSASlide;
	int FineSlaveRear;
	int FineSlaveFront;
} PROTOCOL_FW_VERSION_OHT;

/**
@brief   OCS->OHT Status ������ ���� ����ü
@date    2015.10.27
*/
typedef struct PROTOCOL_STATUS_OCS_
{
	STATUSORDER			StatusOrder;		// ��������
	STOPORDER			StopOrder;			// ��������
	unsigned char		StopReason;			// ��������
	MAINTANANCEORDER	MaintananceOrder;	// ������������
	EMORDER				EmOder;				// E/M����
	DATATRANSORDER		DataTransOrder;		// ������ ��������
	unsigned char		Dummy[24];			// 30����Ʈ�� ����� �ϹǷ� ���� ������ ����

} PROTOCOL_STATUS_OCS;


/**
@brief   STATUSORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct INNER_ORDER_STATUS_
{
	BYTE LayoutChangeRunOrder	: 1; ///< ���̾ƿ� ���� ���� ����
	BYTE CycleUnable	   	 	: 1; ///< Cycle ���� �Ұ�
	BYTE DataTransfer	   		: 1; ///< ������ ���� ���� ����
	BYTE CraneDownAdmission  	: 1; ///< ���� ���� �㰡
	BYTE CraneUpAdmission  		: 1; ///< �ø� ���� �㰡
	BYTE Reserved1	  		 	: 1; ///< Reseverd
	BYTE Reserved2		 		: 1; ///< Reseverd
	BYTE Reserved3	  	   		: 1; ///< Reseverd
}INNER_ORDER_STATUS;

/**
@brief   Pause ���ǵ� ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct INNER_PAUSE_STATUS_
{
	BYTE OHT_StartUp		   	: 1; ///< OHT �⵿ On/Off
	BYTE OHT_Detect			 	: 1; ///< OHT ������ ��� ǻ��
	BYTE OBS_Detect	   			: 1; ///< OBS ������ ���
	BYTE CID		 			: 1; ///< CID ���� �� ���
	BYTE LastOHTDetect			: 1; ///< ����㰡 ����� ��ġ.
	BYTE Reseverd5			 	: 1; ///< ���� ���� �㰡
	BYTE Reseverd6		  		: 1; ///< �ø� ���� �㰡
	BYTE Reseverd7 				: 1; ///< �ø� ���� �㰡
} INNER_PAUSE_STATUS;

/**
@brief   OHT Status ������ ���� ����ü
@date    2015.10.27
*/
typedef struct INNER_STATUS_OHT_
{
	PROTOCOL_STATUS_OHT 	StandardOHTStatus;
	INNER_ORDER_STATUS	 	StandardOrderStatus;
	BYTE                    VHL_Insert;
	BYTE 					VHL_FloorTransfer; ///< Reseverd
	BYTE 					VHL_Board; ///< Reseverd
	INNER_PAUSE_STATUS	 	PauseStatus;
	unsigned char			StopReason;			// ��������

} INNER_STATUS_OHT;

/**
@brief   OHT �⵿ Off ������ ���� ����ü
@date    2016.04.22
*/
typedef struct OHT_TURN_OFF_STATUS_
{
	bool                    bMarked; ///<
	UINT					nNodeID; ///< NodeID(����)
	UINT					nStationID; ///< StaionID(������)
	double					dOffset; ///<������ ����Node�κ��� ������ ��.
	BYTE 					VHL_Div; ///< �б�����
} OHT_TURN_OFF_STATUS;


// ########################## ����� �ϼ��� ��Ŷ ������� (ID�� ����д�) (BODY�� ���� ����)  ##########################

/**
@brief   REASPONSEACK ������ ���� ����ü
@author  puting
@date    2015.10.27
//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
*/
typedef struct AUTOCHANGEACK_
{
    BYTE PresionCode	   		: 6; ///< ���ڵ�
    BYTE VHL_Auto 				: 1; ///< ����(Ǯ����1, ���̿���0) : ���������϶�
    BYTE Accept_Error			: 1; ///< �����Ұ�
} AUTOCHANGEACK;


/**
@brief   CMDDIVIDE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct CMDDIVIDE_
{
    BYTE CMDType   			: 7; ///< ���ñ���
    BYTE TestMode 			: 1; ///< Test Mode
} CMDDIVIDE;

/**
@brief   WORKCARRYTYPE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct WORKCARRYTYPE_
{
    BYTE Carriage_1   			: 1; ///< FOUP
    BYTE Carriage_2 			: 1; ///< POD
    BYTE Carriage_3   			: 1; ///< MAC
    BYTE Carriage_4  			: 1; ///< FOSB
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} WORKCARRYTYPE;


typedef struct BUFFERTYPE_
{
	BYTE Buffer_1	 			: 1; ///< ��1���ϴ�	
	BYTE Buffer_2 				: 1; ///< ��2���ϴ�
	BYTE Reserved_2 			: 1; ///< Reserved
	BYTE Reserved_3 			: 1; ///< Reserved
	BYTE Reserved_4 			: 1; ///< Reserved
	BYTE Reserved_5 			: 1; ///< Reserved
	BYTE Reserved_6 			: 1; ///< Reserved
	BYTE Reserved_7 			: 1; ///< Reserved
} BUFFERTYPE;

/**
@brief   RIV ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct RIV_
{
    BYTE Riv_Value 			: 7; ///< �켱��
    BYTE Hotlot 			: 1; ///< Hot lot
} RIV;

/**
@brief   CHANGENODE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct CHANGENODE_
{
    BYTE Map_Num	   			: 4; ///< Map ��ȣ
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} CHANGENODE;

/**
@brief   MAINTANANCEORDER ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct FINAL_PORT_TYPE_
{
    BYTE PortType_1   			: 1; ///< STOKER
    BYTE PortType_2 			: 1; ///< EQ
    BYTE PortType_3   			: 1; ///< STB
    BYTE PortType_4  			: 1; ///< UTB
    BYTE PortType_5  			: 1; ///< LOADER
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} FINAL_PORT_TYPE;

/**
@brief   STATION_MARK_TYPE ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct STATION_MARK_TYPE_
{
    BYTE StopTag   				: 1; ///< STOP TAG
    BYTE QR_Tag_Left 			: 1; ///< QR Tag(Left)
    BYTE QR_Tag_Right  			: 1; ///< QR Tag(Right)
    BYTE Reserved_3  			: 1; ///< Reserved
    BYTE Reserved_4  			: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} STATION_MARK_TYPE;

/**
@brief   TEACHINGDATA_LOOKDOWN ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_LOOKDOWN_
{
    BYTE LKDown_1   			: 1; ///< LKDown_1
    BYTE LKDown_2 				: 1; ///< LKDown_2
    BYTE LKDown_3  				: 1; ///< LKDown_3
    BYTE LKDown_4  				: 1; ///< LKDown_4
    BYTE LKDown_5 	 			: 1; ///< LKDown_5
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_LOOKDOWN;


/**
@brief   TEACHINGDATA_PIOINFO ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_PIOINFO_
{
    BYTE EQPIO_ENABLE  			: 1; ///< EQPIO �������   
    BYTE EQPIO_Left   			: 1; ///< EQPIO_Left
    BYTE EQPIO_Right			: 1; ///< EQPIO_Right
    BYTE EQPIO_IR 				: 1; ///< EQPIO_IR
    BYTE EQPIO_RF 				: 1; ///< EQPIO_RF
    BYTE Reserved_4 	 		: 1; ///< Reserved
    BYTE PIO_CS0 				: 1; ///< PIO_CS0
    BYTE PIO_CS1		  		: 1; ///< PIO_CS1
} TEACHINGDATA_PIOINFO;

/**
@brief   TEACHINGDATA_PIOTIME ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_PIOTIME_
{
    BYTE PIOTime_1   			: 1; ///< PIOTIME_1
    BYTE PIOTime_2				: 1; ///< PIOTIME_2
    BYTE PIOTime_3 				: 1; ///< PIOTIME_3
    BYTE PIOTime_4 				: 1; ///< PIOTIME_4
    BYTE PIOTime_5 	 			: 1; ///< PIOTIME_5
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_PIOTIME;


/**
@brief   TEACHINGDATA_OPTION ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct TEACHINGDATA_OPTION_
{
    BYTE AutoRecovery			: 1; ///< AUTORECOVERY
    BYTE RFReader				: 1; ///< RFREADER
    BYTE TransSound 			: 1; ///< TRANSSOUND
    BYTE Oscilation 			: 1; ///< OSCILATION
    BYTE HnadDecetEQ 	 		: 1; ///< HANDDECETEQ
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} TEACHINGDATA_OPTION;


/**
@brief   PATHDATA_DEF_DIRECTION ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct PATHDATA_DEF_DIRECTION_
{
    BYTE Path_Direction	   		: 1; ///< �б� ����
    BYTE Driving_Flag			: 1; ///< ���� �÷���
    BYTE Reserved_2 			: 1; ///< Reserved
    BYTE Reserved_3 			: 1; ///< Reserved
    BYTE Reserved_4 	 		: 1; ///< Reserved
    BYTE Reserved_5  			: 1; ///< Reserved
    BYTE Reserved_6 			: 1; ///< Reserved
    BYTE Reserved_7		  		: 1; ///< Reserved
} PATHDATA_DEF_DIRECTION;


/**
@brief   REASPONSEACK ������ ���� ����ü
@author  puting
@date    2015.10.27
*/
typedef struct REASPONSEACK_
{
    BYTE PresionCode	   		: 6; ///< ���ڵ�
    BYTE VHL_Error 				: 1; ///< VHL �̻�
    BYTE Accept_Error			: 1; ///< �����Ұ�
} REASPONSEACK;


typedef struct OHTREQUEST_
{
    BYTE CMDType   			: 7; ///< �䱸����
    BYTE TestMode 			: 1; ///< ���۱���
    
} OHTREQUEST;


typedef struct TRANSORDERACK_
{
    BYTE TransOrderID	   		: 4; ///< ���� ���� ID    
    BYTE Accept_Error_Class		: 3; ///< ���� �Ұ� ����    
    BYTE Accept_Error			: 1; ///< �����Ұ�
} TRANSORDERACK;


typedef struct TRANS_ORDER_RESULT_
{
    BYTE TransOrderID	   		: 4; ///< ���� ���� ID    
    BYTE Not_Accept_Class		: 2; ///< ���� �Ұ� ����    
    BYTE Trans_OK				: 1; ///< ���� ����
    BYTE Trans_Fail				: 1; ///< ���� ����
} TRANS_ORDER_RESULT;



typedef struct
{
	unsigned short			CmdID;				// ���� ID
	CMDDIVIDE				CmdDiv;				// ���ñ���
	unsigned char			CmdCycle;			// ���� ����Ŭ
	BUFFERTYPE				BufferType;			// ������ Buffer ��ġ   
	unsigned char			StopNodeID[3];		// ���������
	unsigned char			StopStationID[3];	// ������ Station���
		
	RIV						Riv;				// �켱��
	CHANGENODE				ChangeNode;			// ��ȯ���� ����
	unsigned char			AutoRecovery;    		// Auto Recovry
	//���� �� Station ����
	int						StationData_StationID;  //  �߰�
	unsigned char			StationData_MarkType;   // Target ������ Tag Type
	int     				StationData_ParentNodeID;   //�߰�
	int     				StationData_NextNodeID;      //�߰�
	short     				StationData_Offset;      //�߰�
	unsigned char           StationData_Safety_Level;
	int						StationData_PhysicalID;		// 2�� STB, ���� ������ Tag ID
	unsigned char			StationData_Reserved[3];
//	unsigned char			StationData_Reserved[7];    // Target ������ Tag Type

	int						TeachingData_StationID;  //  �߰�
	unsigned char			TeachingData_PortType;  //  �߰�
	int						TeachingData_Hoist;  //  �߰�
	int						TeachingData_Shift;  //  �߰�
	int						TeachingData_Rotate;  //  �߰�
	unsigned char			TeachingData_PIOType;  //  �߰�
	unsigned char			TeachingData_PIODirect;  //  �߰�
	int						TeachingData_PIOID;  //  �߰�
	int						TeachingData_PIOChannel;  //  �߰�
	unsigned char			TeachingData_PIOTimeLevel;  //  �߰�
	unsigned char			TeachingData_AutoRecovery;  //  �߰�
	unsigned char			TeachingData_HandDetectEQ;  //  �߰�
	int						TeachingData_STBOffset;  //  �߰�
	unsigned char			TeachingData_LookDownLevel;  //  �߰�
	unsigned char			TeachingData_SpecialEQLevel;  //  AOHC-294
	unsigned char			TeachingData_PIOCS;
	unsigned char			TeachingData_MaterialType;
	unsigned char			TeachingData_Reserved[5];    // Target ������ Tag Type
//	unsigned char			TeachingData_Reserved[6];    // Target ������ Tag Type
//	unsigned char			TeachingData_Reserved[7];    // Target ������ Tag Type
	
	//���� �� Data
/*
    unsigned char			StationData_MarkType;   // Target ������ Tag Type
	unsigned char			StationData_PortID[12];    // Target ������ Tag Type
	int						StationData_RunDist;			// Target_RunDist
	int						TeachingData_Hoist;	// Node�κ��� ������ Tag���� �Ÿ�
	int						TeachingData_Shift;	// Node�κ��� ������ Tag���� �Ÿ�
	int						TeachingData_Rotate;	// Node�κ��� ������ Tag���� �Ÿ�

	unsigned char 	  		 TeachingData_FinalPortType;
	
	unsigned char			TeachingData_LookDownLevel;	// Node�κ��� ������ Tag���� �Ÿ�
	TEACHINGDATA_PIOINFO	TeachingData_PIOInfo;
	unsigned char			TeachingData_PIOTimeLevel;
	TEACHINGDATA_OPTION		TeachingData_Option;
	unsigned char			TeachingData_Reserved[8];
*/



		
	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;		// �⺻ �б����
	unsigned char			PathData_StartNodeID[3];	// ���ù���
	unsigned char			PathData_EndNodeID[3];		// �������
	unsigned short			PathData_ExtraNum;			//	�⺻�� �б� ������
	
	// �� �׸� �ٷ� �ձ��� ũ�⸦ ����, ���ǵ� ũ�⿡�� ���� �� �ִ� �ִ��� ���� �����Ѵ�
	// �Ʒ��� 40�� �� �׸���� Byte ũ��. ����Ǹ� �ݵ�� ������ �ٰ�
	unsigned char  	PathData_ExtraID[MAX_PACKET_SIZE - 130];
} PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121;

 




typedef struct
{
	unsigned short			CmdID;					// ����ID
	CMDDIVIDE				ResponseDiv;			// ���ñ���
	unsigned char			CmdCycle;				// ���� ����Ŭ
	BUFFERTYPE				BufferType;				// ������ Buffer ��ġ	
	unsigned char			StopNodeID[3];			// ���������
	unsigned char			StopStationID[3];		// ������ Station���
	RIV						Riv;					// �켱��
	REASPONSEACK			ResponseAck;			// ����/���� �Ұ� ����
} PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122;


//==================================Station ���� �䱸/���� �߰�=======================================//

typedef struct
{
//	unsigned short			CmdID;					// ����ID
	unsigned char			RequestDiv;				//  �䱸 ����
//	unsigned char			CmdCycle;				// ���� ����Ŭ
//	BUFFERTYPE				BufferType;				// ������ Buffer ��ġ
//	unsigned char			StopNodeID[3];			// ���������
	unsigned char			StopStationID[3];		// ������ Station���
//	RIV						Riv;					// �켱��
//	REASPONSEACK			ResponseAck;			// ����/���� �Ұ� ����
} PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111;


typedef struct
{
	unsigned char			ResponseType;			// ���䱸�� ---�߰�
	unsigned char		  	PathType;				// ��Ʈ���� ���� ����
	unsigned char			StopStationID[3];	// ������ Station���

	//���� �� Station ����
	int						StationData_StationID;  //  �߰�
	unsigned char			StationData_MarkType;   // Target ������ Tag Type
	int     				StationData_ParentNodeID;   //�߰�
	int     				StationData_NextNodeID;      //�߰�
	short     				StationData_Offset;      //�߰�
	unsigned char           StationData_Safety_Level;
	int						StationData_PhysicalID;		// 2�� STB, ���� ������ Tag ID
	unsigned char			StationData_Reserved[3];
//	unsigned char			StationData_Reserved[7];    // Target ������ Tag Type
//	unsigned char			StationData_Reserved[8];    // Target ������ Tag Type

	int						TeachingData_StationID;  //  �߰�
	unsigned char			TeachingData_PortType;  //  �߰�
	int						TeachingData_Hoist;  //  �߰�
	int						TeachingData_Shift;  //  �߰�
	int						TeachingData_Rotate;  //  �߰�
	unsigned char			TeachingData_PIOType;  //  �߰�
	unsigned char			TeachingData_PIODirect;  //  �߰�
	int						TeachingData_PIOID;  //  �߰�
	int						TeachingData_PIOChannel;  //  �߰�
	unsigned char			TeachingData_PIOTimeLevel;  //  �߰�
	unsigned char			TeachingData_AutoRecovery;  //  �߰�
	unsigned char			TeachingData_HandDetectEQ;  //  �߰�
	int						TeachingData_STBOffset;  //  �߰�
	unsigned char			TeachingData_LookDownLevel;  //  �߰�
	unsigned char			TeachingData_SpecialEQLevel;  //  AOHC-294
	unsigned char			TeachingData_Reserved[7];    // Target ������ Tag Type

	//���� �� Data
/*
    unsigned char			StationData_MarkType;   // Target ������ Tag Type
	unsigned char			StationData_PortID[12];    // Target ������ Tag Type
	int						StationData_RunDist;			// Target_RunDist
	int						TeachingData_Hoist;	// Node�κ��� ������ Tag���� �Ÿ�
	int						TeachingData_Shift;	// Node�κ��� ������ Tag���� �Ÿ�
	int						TeachingData_Rotate;	// Node�κ��� ������ Tag���� �Ÿ�

	unsigned char 	  		 TeachingData_FinalPortType;

	unsigned char			TeachingData_LookDownLevel;	// Node�κ��� ������ Tag���� �Ÿ�
	TEACHINGDATA_PIOINFO	TeachingData_PIOInfo;
	unsigned char			TeachingData_PIOTimeLevel;
	TEACHINGDATA_OPTION		TeachingData_Option;
	unsigned char			TeachingData_Reserved[8];
*/


	PATHDATA_DEF_DIRECTION	PathData_Def_Direction;		// �⺻ �б����
	unsigned char			PathData_StartNodeID[3];	// ���ù���
	unsigned char			PathData_EndNodeID[3];		// �������
	unsigned short			PathData_ExtraNum;			//	�⺻�� �б� ������

	// �� �׸� �ٷ� �ձ��� ũ�⸦ ����, ���ǵ� ũ�⿡�� ���� �� �ִ� �ִ��� ���� �����Ѵ�
	// �Ʒ��� 40�� �� �׸���� Byte ũ��. ����Ǹ� �ݵ�� ������ �ٰ�
	unsigned char  	PathData_ExtraID[MAX_PACKET_SIZE - 121];
} PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112;


//================================================================================================//



namespace AckType
{
	enum Reserved
	{
		Ack				= 0x00,
		Busy			= 0x01,
		ErrorState		= 0x02,
		DataLogicError	= 0x03,
	};
};

typedef struct
{
	unsigned short	RcvCmdID;			// ������ Command ID
	unsigned char	ProgressType;		// ��ô����
	// unsigned int	MoveDist; 			// ����Ÿ�
	// unsigned char	Reserved[32];		//����
	
} PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_13; 	// CLW -> MCP ��ô���� (ID:13) //AOHC-297

typedef struct
{
	unsigned int	WarningMessageID;	 // ���� �޼���
	unsigned char	MessageData[64];	 // MessageData


} PROTOCOL_BODY_JOB_WARNING_REPORT_CLW_TO_MCP_ID_29; 	// OHT -> OCS ����� ���� (ID:29)   //AOHC-296


typedef struct
{
	unsigned short	SQNum;					// ������ �ѹ�
	unsigned char	StopNodeID[3];			// STOP ����
	unsigned char	ResetNodeID_Main[3];	// RESET ����(Main)
	unsigned char	ResetNodeID_Sub[3];		// RESET ����(Sub)

}PASSPERMIT_COMMON;


typedef struct
{
	unsigned char	StopNodeID[3];			// STOP ����
	unsigned char	ResetNodeID_Main[3];	// RESET ����(Main)
	unsigned char	ResetNodeID_Sub[3];		// RESET ����(Sub)
}FORCE_PASSPERMIT_COMMON;
typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;	
	unsigned char		PassDelayTime[3];		// ������ð�

} PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241;		// ��� �㰡 �䱸 (ID:241)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char	AckType;				// ���� ����
	unsigned char	ResultType;				// ��� ����

} PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242;	// ��� �㰡 �䱸 ���� (ID:242)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;

} PROTOCOL_BODY_CMD_PASSPERMITOPEN_OHT2OCS_ID_243;		// ��� �㰡 ���� �䱸 (ID:243)


typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;

} PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244;	// ��� �㰡 ���� �䱸 ���� (ID:244)


typedef struct
{
	// protocol is blank (0 byte!)

} PROTOCOL_BODY_CMD_PASSPERMITREQ_OCS2OHT_ID_245;		// ��� �㰡 ���� (ID:245)


typedef struct
{
	BYTE	Finish_PassPermitNum;	// ����㰡 ����� ���� �Ǽ�
	PASSPERMIT_COMMON	PassPermit_CM[10];
} PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246;	// ��� �㰡 ���� ���� (ID:246)
					// OHT�� ����㰡�� ���� ���¸� OCS�� ������(����㰡���ǿ� ���� ����) 							

typedef struct
{
	FORCE_PASSPERMIT_COMMON ForcePassPermit;

} PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247;	// ��� �㰡 ���� ���� �䱸 (ID:247)
														//OHT�� OCS���� ��������㰡 ��û
														
typedef struct
{
	FORCE_PASSPERMIT_COMMON ForcePassPermit;
	unsigned char   Reserved[6];				//�̻��
	unsigned char	AckType;				// ���� ����
	unsigned char	ResultType;				// ��� ����
} PROTOCOL_BODY_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248;	// ��� �㰡 ���� ���� ���� (ID:248)
											// OHT�� ��������㰡��û�� ���� OCS�� ����								

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char   Reserved[9];
	unsigned char	CIDResultType;				// ��� ����
	unsigned char   CIDStatus[3];
} PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249;	// CID �̻󺸰� (ID:249)

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char   Reserved[9];
	unsigned char	AcOrder;				// ���� ����
	unsigned char   Reserved2[43];
} PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250;	// ���� �簳(ID:250)

//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
typedef struct
{
	AUTOCHANGEACK	AutoChangeACK;
} PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252;	// �ڵ�������ȯ (ID:252)

typedef struct
{
	PASSPERMIT_COMMON	PassPermit_CM;
} PROTOCOL_BODY_PASSPERMIT_CANCEL_OCS2OHT_ID_270;	// ��� �㰡 ���� ���� (ID:246)


typedef struct
{
   //	FORCE_PASSPERMIT_COMMON ForcePassPermit;
	PASSPERMIT_COMMON	PassPermit_CM;
	unsigned char	AckType;				// ���� ����
	unsigned char	ResultType;				// ��� ����
} PROTOCOL_BODY_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271;	// ��� �㰡 ���� ���� ���� (ID:248)

typedef struct
{
	OHTREQUEST				Request;			
} PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS;
// ������ ���� �䱸 (OHT -> OCS)


typedef struct
{
	OHTREQUEST				Request;
	unsigned char			OrderID;
	unsigned char			IpAddress[4];
	char					UserName[17];		// ������
	char					PassWord[17];		// �н�����
	char					Version[33];		// ����
	char					DataSendTo[256];	// ������ ����ó
} PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT;
// ������ ���� �䱸 (OCS -> OHT)


typedef struct
{
	OHTREQUEST				Request;	
	TRANSORDERACK			Ack;
} PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS;
// ������ ���� ���� ����								


typedef struct
{
	OHTREQUEST				Request;	
	TRANS_ORDER_RESULT		Result;
	short int				FailClass;	// ������ ���۽��б��� 						
} PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS;
// ������ ���� ��ô ����								

typedef struct
{
	char					Version[512];	
} PROTOCOL_BODY_OHT_VERSION_OHT2OCS;


typedef struct
{
	char					RailNumber;			
} PROTOCOL_BODY_OHT_INPUT_REQUEST;

typedef struct
{
	char					Ack;
} PROTOCOL_BODY_OHT_INPUT_REQUEST_RESPONSE;


typedef struct
{
	unsigned short			OrderID;			
}
PROTOCOL_BODY_FLOOR_MOVE_ORDER;


typedef struct
{
	unsigned short			OrderID;
	REASPONSEACK			AckClass;		    
}
PROTOCOL_BODY_FLOOR_MOVE_ORDER_RESPONSE;


typedef struct
{
	char AppMonitorVersion[32];
	char ApplicationVersion[32];
	char ParameterVersion[32];
	char MapVersion[32];
}
PROTOCOL_BODY_VERSION_RESPONSE;


typedef struct
{
	unsigned short		CmdID;		    	// ���� ID
	CMDDIVIDE		    CmdDiv;		    	// ���ñ���
	unsigned char		CmdCycle;	    	// ���� ����Ŭ
	unsigned char		StopStationID[3];	// ������ Station

}
PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301;


typedef struct
{
	unsigned short		CmdID;		    	// ���� ID
	CMDDIVIDE		    CmdDiv;		    	// ���ñ���
	unsigned char		CmdCycle;	    	// ���� ����Ŭ
	unsigned char		StopStationID[3];	// ������ Station

    unsigned char       Response;           // Carrier ��û ���� Ȯ��
    unsigned char       CarrierID[64];      // ��û Carrier�� ID

}
PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������� �⺻ ���� PACKET ����  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////




typedef enum TAGTYPE_
{
	TAGTYPE_PASSPERMIT_RESPONSE      =1,
	TAGTYPE_PASSPERMIT_OPEN_RESPONSE,
	TAGTYPE_FORCE_PASSPERMIT_OPEN_RESPONSE,
	TAGTYPE_PASSPERMIT_REQ,
    TAGTYPE_JOBORDER_TARGETINIT,  //�۾����濡 ���� Ÿ�ٿ� ���� ����Ŭ �ʱ�ȭ->�̵�����Ŭ�θ� �����
	TAGTYPE_PASSPERMIT_CANCEL, //soming

	TAGTYPE_TOUR_MODE = 11,  //���� ��� ����
	TAGTYPE_TOUR_TARGETINIT,  //�۾����濡 ���� Ÿ�ٿ� ���� ����Ŭ �ʱ�ȭ->�������Ŭ�θ� �����
    TAGTYPE_TOUR_STATIONMOVEINFO,

	TAGTYPE_AUTOMODE = 21,  //�켱���� ó������ �и�. ����㰡 �ڷ�.
	TAGTYPE_RESET,
	TAGTYPE_DATATRANSFER,
	TAGTYPE_INFOREQ,
	TAGTYPE_JOBORDER_JOBCHAGE,
	TAGTYPE_JOBORDER_TARGETCHAGE,
	TAGTYPE_JOBORDER_PATHCHAGE,
	TAGTYPE_JOBORDER_JOBCLEAR,
	TAGTYPE_JOBORDER_JOBCANCEL,
	TAGTYPE_MANUALMODE,

	TAGTYPE_JOBORDER = 31,
    TAGTYPE_STATIONMOVEINFO,
	TAGTYPE_FLOORMOVEMENT,

	TAGTYPE_MANUAL_COMMON = 41,

	TAGTYPE_AUTOCHANGE = 51 // jhoun.lim

	

}TAGTYPE;



/**
@brief   CYCLETYPE ������ ���� ����ü
@author  puting
@date    2015.10.31
*/

typedef enum CYCLETYPE_
{
	CYCLETYPE_DRIVING 	= 1,		//����㰡 ��û�� �ʿ��� ����
	//CYCLETYPE_DRIVING_FINAL,			//Mark or Target ���ΰ��� ����
	CYCLETYPE_TRANS,
	CYCLETYPE_MANUAL
}CYCLETYPE;

/**
@brief   CYCLETYPE ������ ���� ����ü
@author  puting
@date    2015.10.31
*/

typedef enum CYCLENODEINFO_
{
	CYCLENODETYPE_START2STOP 	= 1,		//Start Node���� Stop Node 
	CYCLENODETYPE_STOP2STOP,			    //Stop Node���� Next Stop Node	
	CYCLENODETYPE_STOP2TARGET,				//Stop Node���� Target Node	
	CYCLENODETYPE_START2TARGET				//Start Node���� Target Node	
	
}CYCLENODEINFO;


/**
@brief   CYCLESTEP ������ ���� ����ü
@author  puting
@date    2015.10.31
*/

typedef enum CYCLESTEP_
{

	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE = 1,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_MEANINGLESS
/*
	CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE =1,
	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE,
	CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	CYCLESTEP_DRIVING_MEANINGLESS
 */
 //	CYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE,   //����㰡 ������ ���� ��
 //	CYCLESTEP_DRIVING_AFTER_NEXTLINK_PASSPERMIT_RESPONSE     //����㰡 ������� ������ ���� ��

	//CYCLESTEP_TRANS_BEFOER_PERMIT,
	//CYCLESTEP_TRANS_AFTER_PERMIT,
	//CYCLESTEP_MANUAL
}CYCLESTEP;

/**
@brief   NextCYCLESTEP ������ ���� ����ü
@author  puting
@date    2015.10.31
*/

typedef enum NEXTCYCLESTEP_
{
	//NEXTCYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE =0,
	NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE = 1,
	NEXTCYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_OPEN_RESPONSE,
	NEXTCYCLESTEP_DRIVING_MEANINGLESS

//	NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE,   //����㰡 ������ ���� ��
//	NEXTCYCLESTEP_DRIVING_AFTER_NEXTLINK_PASSPERMIT_RESPONSE,     //����㰡 ������� ������ ���� ��
//	NEXTCYCLESTEP_DRIVING_MEANINGLESS

	//CYCLESTEP_TRANS_BEFOER_PERMIT,
	//CYCLESTEP_TRANS_AFTER_PERMIT,
	//CYCLESTEP_MANUAL
}NEXTCYCLESTEP;
// COMMON Protocol Packet -----------------------------------------------


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
} PKT_CHECK_TAG;								// �������� ù �ؼ����� TAG ������ ��ġ�� ã�� ���� ������ ����.

void Conv_PKT_CHECK_TAG(PKT_CHECK_TAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OCS							STATUS;
	unsigned char								Value[MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OCS)];
} PKT_OCS2OHT;

void Conv_PKT_OCS2OHT(PKT_OCS2OHT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT							STATUS;
	unsigned char								Value[MAX_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-sizeof(PROTOCOL_STATUS_OHT)];
} PKT_OHT2OCS;

void Conv_PKT_OHT2OCS(PKT_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
	// OHT Status
	ConvertEndian(&Pkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
}


/**
@brief   Status(OHT->TP) ����ü
@author  puting
@date    2012.11.19
@note    Input, Output�� ��� Light-On, Dark-On�� ����� �ʿ� ����
*/
#if 0
typedef struct DEF_STATUS_OHT2TP_
{
    OHT_COMMON_STATUS Data; ///< OHT �⺻ ����

    // HID ����
    WORD HID_Erec;          ///< HID 2���� �Է�����
    WORD HID_Edc;           ///< HID 2���� �������
    WORD HID_Idc;           ///< HID 2���� ����
    WORD HID_Ecap;          ///< HID ĸ ��ũ ����
    WORD HID_Kw;            ///< HID ��� �Ŀ�
    WORD HID_ErrorCode;     ///< HID Error Code
    WORD HID_PickupTemper;  ///< HID Pick up �µ�
    WORD HID_CPUTemper;     ///< ???

    // IO ����
    UINT Input[3];          ///< OHT Input Data #1~3
    UINT Output[3];         ///< OHT Output Data #1~3  
    
    // �߰� ���� 
    // OHT Use Info
    uCMD_USE_DATA UseInfo;  ///< Use OHT Data(���Ͽ�)

    // Motor ���� ����
    MOTOR_INFO Driving_F;   ///< ������ Front
    MOTOR_INFO Driving_R;   ///< ������ Rear
    MOTOR_INFO Hoist;       ///< Hoist ��
    MOTOR_INFO Shift;       ///< Shift ��
    MOTOR_INFO Rotate;      ///< Rotate ��

    // UBG ����
    BYTE IsUBGSen;          ///< UBG Sensor ����
    BYTE UBGDetectLevel;    ///< UBG Sensor Level

    // PBS ����
    BYTE IsPBSSen;          ///< PBS Sensor ����
    BYTE PBSDetectlevel;    ///< PBS Sensor Level

    BYTE IsFCPause;         ///< FC Pause
    UINT TPCommandType;     ///< TP CommandType 4�ڸ�
    BYTE Cmd_Count;         ///< Cmd ����

    BYTE AMCResponse;       ///< AMC Response
//    UINT MapMakeData;      ///< Map Make Data(���� : mm)
	short MapMakedLinkCount;      			  ///< Map Make ����� ����
    short MapMakedLinkDrivingCheckCount;      ///< Map Make ��������� ����� ����
	BYTE DownPercent;       ///< Down Percent
	double CPUTemperature;  ///< IPC �µ�
    UINT APSignalLevel;     ///< �������� ��ȣ

    // Log ���� ����
    BYTE isLogDrive;        ///< Log Drive ����

    BYTE STBDirection;      ///< STB Direction
    BYTE IsRotateExist;     ///< Rotate Exist
    MAPDATALOADINFO AdjustMapLoadInfo;       ///< ������ Map ����
	MAPMAKE_STATE_INFO MapMakeState;     ///< MapMaker ���� ����
	BYTE IsErrorIgnolState;     ///< FBWF ����  ->ErrorIgnorstate     //AOHT-0001_FOUPCOVER
	FLAG_STATE_INFO IsWarningState;       ///< RF PIO Type       //AOHC-285

    // by zzang9un 2013.03.29 : ���� ���� ���� ����ü
    uVERSION_DATA OHTVer;   ///< OHT Version
	uVERSION_DATA AMCVer;   ///< AMC Version
	uVERSION_MAP  MapVer;

    // by zzang9un 2013.03.29 : Mark ������ ���� ����ü �߰�
    uMARK_INFO MarkInfo;    ///< Mark ������ ���� ����ü

    // by zzang9un 2013.07.22 : Data �ε� ���� �߰�
    uDATALOADED DataLoaded; ///<  Data �ε� ����
} DEF_STATUS_OHT2TP;
#endif

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2TP							OHT_STATUS;	
	uTP_CMD										OHT_BODY;
} PKT_OHT2TP_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2VMS						OHT_STATUS;
} PKT_OHT2VMS_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_STATUS_OHT2BLACKBOX						OHT_STATUS;
} PKT_OHT2BLACKBOX;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	SYSTEMTIME 									TIME;
} PKT_BLACKBOX_TIME;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	BYTE 										EventType;
} PKT_OHT2BLACKBOX_EVENT;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	MSG_EVENT_BLACKBOX2OHT 						EVENT;
} PKT_BLACKBOX2OHT_EVENT;

// -----------------------------------------------------------------------------
//  TP���� OHT�� IO����Ʈ�� �����ϸ� OHT�� �� ��Ŷ�� ����Ʈ�� �����Ͽ� �����Ѵ�
//  2016.06.15 shkim. added
// -----------------------------------------------------------------------------
typedef struct
{
	int	 iOhtInputNum;		// IO����Ʈ ����
	int  iOhtOutputNum;
	char cIONameList[MAX_BIG_PACKET_SIZE-sizeof(PROTOCOL_HEADER)-sizeof(PROTOCOL_TAG)-(sizeof(int) * 2)];	// IO����Ʈ �̸�. ��ǲ �̸��� ���ʴ�� �����ǰ�, �ƿ�ǲ �̸��� �״������� �����ȴ�.
							// �����ڴ� '\0' �̴�.
}
PROTOCOL_BODY_IO_LIST_OHT_2_TP_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_IO_LIST_OHT_2_TP_RESPONSE		BODY;	
} PKT_OHT2TP_IOLIST;


// -------------------------------------------------------------
typedef struct
{
	unsigned char			OHTID[6];
	unsigned char			CIDO_FW_Ver[3];			
	unsigned char			CIDR_FirstLinkLevel[3];	// CID-R ù �� �۽�(Link) ��ȣ ����
	unsigned char			CIDO_FiberCommError[2];	// ��� �� �� ��� ����(%)
	unsigned char			CIDO_RFCommError[2];	// ��� �� ���� ��� ����(%)
	unsigned char			delimiter;				// CID-O�� CID-R ������ ����
	unsigned char			CID_UniqueID[6];		// CID-���� ID
	unsigned char			CIDR_FW_Ver[3];		// CID-R F/W Version
	
	unsigned char			RF_ID[5];				// added (2016.10.30)
	unsigned char			RF_CH[4];				// added (2016.10.30)
	unsigned char			CIDR_MAXCurrent[3];		// ��� �� CID-R �ִ� ����
	unsigned char			CIDR_MAXVoltage[3];		// ��� �� CID-R �ִ� ����
	unsigned char			CIDR_MAXTemp[2];		// ��� �� CID-R �ִ� �µ�
	unsigned char			CIDR_StandbyFiberNoise[3];	// ��� ���� CID-R �� ������ ���Ű�
	unsigned char			CIDO_FirstLinkLevel[3];	// CID-O�� ù �� �۽�(Link) ��ȣ ����
	unsigned char			FiberError_Per[2];		// ��� �� �� ��� ����(%)
	unsigned char			RFError_Per[2];			// ��� �� ���� ��� ����(%)											// �����ڴ� '\0' �̴�.
}
PROTOCOL_BODY_CID_INFO_OHT_2_TP_RESPONSE;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_CID_INFO_OHT_2_TP_RESPONSE	BODY;	
} PKT_OHT2TP_CID_INFO;

// IO�� ���� OnOff�ϰų� 0,1�� �����ϴ� ��Ŷ�̴�
// (�Ʒ� �� ���������� ������ ��Ŷ ���Ǹ� �����)
/* TagID
	CMD_IOMAP_USE_ONOFF_TP2OHT				= 25232, // TP���� IO�� ON, OFF�� ���Ѵ�
	CMD_IOMAP_SET_RESET_TP2OHT				= 25233, // TP���� IO�� 1, 0 ������ ���Ѵ�

   TEXTID
	IOMAP_USE_ONOFF_TP2OHT					= 232,	// TP���� IO�� ON, OFF�� ���Ѵ�
	IOMAP_SET_RESET_TP2OHT	
*/
typedef struct
{
	int	 iIoNum;			// IO����Ʈ no
	int  iOnOff;			// �����ڴ� '\0' �̴�.
}
PROTOCOL_BODY_IO_ONOFF_OHT_2_TP;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_IO_ONOFF_OHT_2_TP				BODY;	
} PKT_OHT2TP_IO_SET;

// -------- ƼĪ�� SAVE ��Ŷ���� (ƼĪ�� LOAD ��Ŷ���ǿ� ����) ---------
// TagID  : CMD_TEACHING_INFO_SAVE = 25234, // TP���� ƼĪ�� OHT�� ƼĪ���� �����Ѵ�
// TextID : TEACHING_INFO_SAVE     = 234,
// -----------------------------------------------------------------------------
typedef struct SUB_BODY_TEACHING_INFO_ETC_
{
    UINT PIODirection   : 4; ///< PIO ����(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
	UINT LookDownLevel  : 8; ///< LookDownLevel
	UINT SpecialEQLevel : 4; ///< SpecialEQLevel
    UINT Reserved2      : 1; ///< 
    UINT Reserved1      : 1; ///< 
    UINT EQPIODisable_TP: 1; ///< EQPIO Disable
    UINT HandDetectEQ   : 1; ///< Hand Detect EQ ����
    UINT Oscillation    : 1; ///< Oscillation On/Off
    UINT TransSound     : 1; ///< ������ �Ҹ� On/Off
    UINT RFReader       : 1; ///< RF Read ����
    UINT AutoRecovery   : 1; ///< Auto Recovery ����
} SUB_BODY_TEACHING_INFO_ETC;


typedef union SUB_BODY_uTEACHING_INFO_ETC_
{
    SUB_BODY_TEACHING_INFO_ETC Info;
    SUB_BODY_TEACHING_INFO_ETC Data;
} SUB_BODY_uTEACHING_INFO_ETC;


typedef struct
{
	BYTE CmdID;                 			///< Command ID
	int  PortType;              			///< Port Type
	int  Hoist;
	int  Shift;
	int  Rotate;
	BYTE PIOType;
	UINT PIOChannel;
	UINT PIOID;
	SUB_BODY_uTEACHING_INFO_ETC EtcData;  	///< ��Ÿ Teaching ����
	//int  Reserved1;             			///< Reserved1
	BYTE  PIOCS;             			     //< PIO Carrier Stage
	BYTE MaterialType;
	BYTE  Reserved[6];        			///< Reserved
} PROTOCOL_BODY_TEACHING_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_TEACHING_SAVE					BODY;
} PKT_OHT2TP_TEACHING_INFO;


// -------- ���ͽý�����  SAVE ��Ŷ���� (��Ŷ���ǿ� ����) ---------
// TagID  : CMD_OACIS_INFO_SAVE = 25256, // TP���� OHT�� ƼĪ���� �����Ѵ�
// TextID : OACIS_INFO_SAVE     = 256,
typedef struct
{
	short  Type;              			///< Type
	short  TypeNum;              		///< Type Num
	int  nData1;                        ///< �����
	int  nData2;                        ///< ���� �Ÿ���
	int  nData3;             			///< Reserved1
	int  nData4;             			///< Reserved2
} PROTOCOL_BODY_OACIS_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_OACIS_SAVE				BODY;
} PKT_OHT2TP_OACIS_INFO;



// -------- Window OS ���� ��ư��Ŷ���� (��Ŷ���ǿ� ����) ---------
// TagID  : CMD_WINDOW_OS_SHUTDOWN		= 25257, // Window OS ���� ��ư�� �����Ѵ�.
// TextID : WINDOW_OS_SHUTDOWN     = 257,
typedef struct
{
	short  Type;              			///< Type
	short  TypeNum;              		///< Type Num
	int  nData1;                        ///< Reserved
	int  nData2;                        ///< Reserved
	int  nData3;             			///< Reserved
	int  nData4;             			///< Reserved
} PROTOCOL_BODY_WINDOW_OS_SHUTDOWN;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_WINDOW_OS_SHUTDOWN		  	BODY;
} PKT_OHT2TP_WINDOW_OS_SHUTDOWN_INFO;




// -------- ���� Map�� SAVE ��Ŷ���� (��Ŷ���ǿ� ����) ---------
// TagID  : CMD_DRIVINGMAP_INFO_SAVE = 25253, // TP���� OHT�� ƼĪ���� �����Ѵ�
// TextID : DRIVINGMAP_INFO_SAVE     = 253,
typedef struct
{
	int  nStartNodeID;              		///< ���۳��
	int  nEndNodeID;                        ///< �����
	int  nMapMakeDist;                      ///< ���� �Ÿ���
	int  Reserved1;             			///< Reserved1
	int  Reserved2;             			///< Reserved2
} PROTOCOL_BODY_DIRIVINGMAP_SAVE;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_DIRIVINGMAP_SAVE				BODY;
} PKT_OHT2TP_DRIVINGMAP_INFO;



// -----------------------------------------------------------------------------
// ���� LOAD (���� ��������), ���� UNLOAD (��񿡼� ���� OHT�� �̵�),
// TEACHING POINT�� �̵��ϵ��� �����ϴ� �����Ŷ
// -----------------------------------------------------------------------------
typedef struct 
{
    BYTE Reserved2      : 1;    ///<
    BYTE Reserved1      : 1;    ///<
    BYTE EQPIODisable_TP: 1;    ///< TP���� EQPIO�� ���� �ɼ�
    BYTE HandDetectEQ   : 1;    ///< Hand Detect EQ ����
    BYTE Oscillation    : 1;    ///< Oscillation On/Off
    BYTE TransSound     : 1;    ///< ������ �Ҹ� On/Off
    BYTE RFReader       : 1;    ///< RF Read ����
    BYTE AutoRecovery   : 1;    ///< Auto Recovery ����
} TRANS_OPT;

typedef union 
{
    TRANS_OPT    Info;   ///< ��Ÿ ������ �ɼ�
    BYTE         Data;   ///< Data
} uTRANS_OPT;

typedef struct 
{
    BYTE CmdID;                 ///< Command ID
    UINT CurNodeID;             ///< ������ Tag�� ���� �ִ� DrivingNodeID ID
    UINT CurStationID;          ///< ������ Node ID
    BYTE PortType;              ///< �۾� ����
    BYTE CarrierType;           ///< �ݼ۹� ����
    BYTE PIODirection;          ///< PIO Direction
    int  Tposition_Hoist;       ///< Hoist Teaching ��
    int  Tposition_Shift;       ///< Shift Teaching ��
    int  Tposition_Rotate;      ///< Rotate Teaching ��
    BYTE HoistSpeedLevel;       ///< Hoist �ӵ� ����(50~100%)
    BYTE ShiftSpeedLevel;       ///< Shift �ӵ� ����(50~100%)
    BYTE PIOTimeLevel;          ///< ��Ʈ �� EQ PIO Timeover ����(1~10)
	BYTE LookDownLevel;         ///< Lookdown Sensor Level(1~16)
	BYTE SpecialEQLevel;         ///< SpecialEQLevel(0~9)
	uTRANS_OPT TransOption;  ///< ������ �ɼ� ���ð�
	BYTE BufferType;			///< ������� Buffer ��ġ
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS ��ȣ ����
	BYTE PIOChannel;		  	///< RF PIO PIOChannel (�̻���  0 )
	BYTE MaterialType;
	BYTE PIOType;               ///< PIO Type ���� ���(�űԷ� ���ǵ� �׸� ���)

} PROTOCOL_BODY_TRANS_INFO;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_TRANS_INFO					BODY;	
} PKT_TP2OHT_TRANS_LOAD_UNLOAD;




typedef struct 
{
	int iVar1;
	int iVar2;
	int iVar3;
} PROTOCOL_BODY_SIMPLE_COMMAND;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_SIMPLE_COMMAND				BODY;	
} PKT_TP2OHT_SIMPLE_COMMAND;

// -------- HOIST ƼĪ���� ����� ���� ��Ŷ���� ----------
typedef struct 
{
	double	dPulse;		///< Pulse �Ÿ� ����
	double	dUserUnit;	///< ����� �Ÿ� ����
} HOIST_TEACH_VALUE;

typedef struct 
{
	HOIST_TEACH_VALUE PositionValue[10];
	
	double dHomeOffsetOfRailToJigOnUserUnit;
	double dHomeSearchOffset;

	double dIdealGearRatio;
	double dIdeal2RealRatio;
	double dHomeOffsetOfMTLToJigOnUserUnit;
} PROTOCOL_BODY_HOIST_TEACH_COMMAND;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_HOIST_TEACH_COMMAND			BODY;	
} PKT_TP2OHT_HOIST_TEACH_COMMAND;

typedef struct
{
	BYTE MaterialType;
} PROTOCOL_BODY_CHECK_MATERIAL;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_CHECK_MATERIAL			    BODY;
} PKT_TP2OHT_CHECK_MATERIAL;

// -------------------------------------------------

typedef struct 
{
	char 	Log[512];
	
	int 	iResult;
	
	int 	iVar1;
	int 	iVar2;
	int 	iVar3;
	int 	iVar4;
} PROTOCOL_BODY_LOG_DATA;


typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_LOG_DATA						BODY;	
} PKT_OHT2TP_LOG_DATA;


#define MAP_MAKE_RESULT_SUCCESS			0
#define MAP_MAKE_RESULT_FAIL   	   	   -1
#define MAP_MAKE_NOT_FOUND_NODE_ID 	   -2

// -------------------------------------------------

// TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)
typedef struct 
{
	bool IsGoStation;		    // Target => Node : 0, Station : 1
	int  StartNodeID;			// ������ �׽�Ʈ �� ���� ���ϰ� �Է��ϴ� �뵵��
	int  StopNodeID;			// Target Node No
    int  PortType;              // Port Type ;
                                // 0x01 : EQ, 0x02 : EQ_X, 0x03 : STK, 0x04 : Loader,
								// 0x05 : STB_L, 0x06 : STB_R, 0x07 : UTB

	BYTE IsMapMaker;            //Map Maker Flag , 0x00 �Ϲ� ����, 0x01: MapMaker, 0x02: MapMaker ���� ����
	BYTE IsSteerCheck;          //Map Maker ��, Steer Check ��� ��� ����
	BYTE nCycle; // Move 2, Load 3, Unload 4
} PROTOCOL_BODY_MOVE_BY_MAP;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_MOVE_BY_MAP					BODY;	
} PKT_TP2OHT_MOVE_BY_MAP;

// -----------------------------------------------------------------------------

// TP���� OHT�� 2�� STB �۾� ���
typedef struct
{
	BYTE RequestType;		    // Move: 0, Load: 1, Unload: 2
	int  StopStationID;			// Logical Tag ID
} PROTOCOL_BODY_DUAL_STAGE_STB_TRANS;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_DUAL_STAGE_STB_TRANS					BODY;
} PKT_TP2OHT_DUAL_STAGE_STB_TRANS;

// -----------------------------------------------------------------------------

typedef struct 
{
	int	 iResult;		    

} PROTOCOL_BODY_MOVE_BY_MAP_RESULT;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_MOVE_BY_MAP_RESULT			BODY;	
} PKT_OHT2TP_MOVE_BY_MAP_RESULT;

#define MAP_MOVE_NO_ERR					0
#define MAP_MOVE_ERR_NOT_MARKED			-1
#define MAP_MOVE_ERR_MAP_SEARCH_FAIL	-2
#define MAP_MOVE_ERR_NOT_STATION_INFO   -3

// ---------------------------------------
//	TP Command Debug Support
// ---------------------------------------
// Command (TP -> OHT)
#define TP_DEBUG_START			0
#define TP_DEBUG_STOP				1

// Result (OHT -> TP)
#define TP_DEBUG_PROGRESS			0
#define TP_DEBUG_ERR				1
#define TP_DEBUG_ENDED			2



typedef struct
{
	PKT_OHT2OCS                                 AckPkt;
	UINT 										Length;
} PKT_ACK_OHT2OCS;								// �������� ù �ؼ����� TAG ������ ��ġ�� ã�� ���� ������ ����.

void Conv_PKT_ACK_OHT2OCS(PKT_ACK_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.AckPkt.HEADER.MachineType);
	ConvertEndian(&Pkt.AckPkt.TAG.Value);
	// OHT Status
	ConvertEndian(&Pkt.AckPkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.AckPkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.AckPkt.STATUS.RunningSpeed);

	// Length
	ConvertEndian(&Pkt.Length);
}



/*
@brief   PKT_TASKCMDDATA ������ ���� ����ü
@author  puting
@date    2015.10.31
*/

/*
	  - �۾����� List�� �Ϲ� ����Ŭ�� ��ɰ� ���/������ ���� ����Ŭ�� �����Ͽ� ����
	  - �켱 ������ �ϱ�� ����.
		 �켱ó�� >> �۾�����(�۾����, ������, ��� ����) >= ����㰡 ��� > �����䱸 or ������ ���� >> �۾�����(�۾�����) = ���� �̵� ���� �� NextCMD > Manual ����

		�켱ó�� ��� (1~10)
			//-------------��Ⱑ �ʿ��ϰų�, ��ó�� ������ �ʿ��� �켱ó�� ���---------//
			1 : ���� ��ȯ( �޴���� ����)
			2 : �ڵ� ��ȯ(�޴��󿡼���, �޴���� ����)
			//3 : Soft Reset(�޴����ۿ����� ���۰���) ������� QUICK���� ó��

			3 : �۾����� �� �۾�����
			4 : �۾����� �� ������ ����
			5 : �۾����� �� ��� ����
			6 : �۾����� �� �۾� ���/Ŭ���� ����
			7 : ���� ����㰡 ���� ����(�޴��� ���ۿ����� ���� ����)
			8 : ����㰡 ���� ����
			9 : ����㰡 ����

		�����䱸 �� Data ����(11~40)
			11 : ����㰡 ����
			12 : Data ���� ����
			13 : ���� �䱸

		///--------------------CMD ��Ⱑ �ʿ��� ��ɵ�----------------------//
		Task(CMD_NextCmd ����)(31~40)
			31 : �۾� ����
			32 : �����̵� ����

		��Ÿ �޴���_����(41~)
			41 : �޴��� �۾�����
			42 : �޴��� ����( ����, IO ����)
*/
typedef struct
{
	TAGTYPE									DetailTagSort; //���� ������ : �۾������� ������ ���浵 ���ԵǾ....
	STANDARDCATEGORY 						CategoryDiv;
	PKT_OCS2OHT								PKTDATA;
} PKT_TASKCMDDATA;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������� ����ü ���� ���� ID ���� ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OCS Using Protocol Packet ---------------------------------------------
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;	
} PKT_OCS_STATUS_ID_01; 						// MCP -> CLW ���º��� (ID:01)

void Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS_ID_01& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


// OHT Station ���� �䱸 (OCS -> OHT)  20161022
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111		BODY;
} PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111;

void Conv_PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111(PKT_STATION_INFO_REQUEST_CLW_TO_MCP_ID_111& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121					BODY;
} PKT_JOB_ORDER_ID_121;							// MCP -> CLW �۾����� (ID:121)

void Conv_PKT_JOB_ORDER_ID_121(PKT_JOB_ORDER_ID_121& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.CmdID);
	// ConvertEndian(&Pkt.BODY.StationData_Start_RunDist);
	ConvertEndian(&Pkt.BODY.StationData_StationID);
	ConvertEndian(&Pkt.BODY.StationData_ParentNodeID);
	ConvertEndian(&Pkt.BODY.StationData_NextNodeID);
	ConvertEndian(&Pkt.BODY.StationData_Offset);
	ConvertEndian(&Pkt.BODY.StationData_PhysicalID);

	ConvertEndian(&Pkt.BODY.TeachingData_StationID);
	ConvertEndian(&Pkt.BODY.TeachingData_Hoist);
	ConvertEndian(&Pkt.BODY.TeachingData_Shift);
	ConvertEndian(&Pkt.BODY.TeachingData_Rotate);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOID);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOChannel);
	ConvertEndian(&Pkt.BODY.TeachingData_STBOffset);

	ConvertEndian(&Pkt.BODY.PathData_ExtraNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMIT_OCS2OHT_ID_242			BODY;
} PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242;		// ��� �㰡 �䱸 ���� (ID:242)

void Conv_PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242(PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244		BODY;
} PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244;	// ��� �㰡 ���� �䱸 ���� (ID:244)

void Conv_PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244(PKT_RESPONSE_PASSPERMITOPEN_OCS2OHT_ID_244& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITREQ_OCS2OHT_ID_245				BODY;
} PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245;			// ��� �㰡 ���� (ID:245)

void Conv_PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245(PKT_CMD_PASSPERMITREQ_OCS2OHT_ID_245& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248	BODY;
} PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248;	// ��� �㰡 ���� ���� ���� (ID:248)

void Conv_PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248(PKT_RESPONSE_PASSPERMITFORCEOPEN_OCS2OHT_ID_248& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

//	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_MOVE_RESTART_OCS2OHT_ID_250					BODY;
} PKT_MOVE_RESTART_OCS2OHT_ID_250;	// ���� �簳  (ID:250)

void Conv_PKT_MOVE_RESTART_OCS2OHT_ID_250(PKT_MOVE_RESTART_OCS2OHT_ID_250& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_ORDER_OCS2OHT					BODY;
} PKT_DATA_TRANSPER_ORDER_OCS2OHT;
												// ������ ���� ����	
void Conv_PKT_DATA_TRANSPER_ORDER_OCS2OHT(PKT_DATA_TRANSPER_ORDER_OCS2OHT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

  //	ConvertEndian(&Pkt.BODY.IpAddress);
}
												

typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
} PKT_OHT_VERSION_REQUEST;

void Conv_PKT_OHT_VERSION_REQUEST(PKT_OHT_VERSION_REQUEST& Pkt)	// OCS�� OHT�� ���� ����
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

//�������� OCS -> OHT ��û
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
} PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251;

void Conv_PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251(PKT_REQUEST_AUTOCHANGE_OCS_OHT_ID_251& Pkt)	// OCS�� OHT�� ���� ����
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

// OHT ���Կ䱸 ���� (OCS -> OHT)																				
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_OHT_INPUT_REQUEST								BODY;
} PKT_OHT_INPUT_REQUEST_RESPONSE;

void Conv_PKT_OHT_INPUT_REQUEST_RESPONSE(PKT_OHT_INPUT_REQUEST_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
}

// ���� �̵����� (OCS -> OHT)
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;
	PROTOCOL_BODY_FLOOR_MOVE_ORDER								BODY;
} PKT_FLOOR_MOVE_ORDER;

void Conv_PKT_FLOOR_MOVE_ORDER(PKT_FLOOR_MOVE_ORDER& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.BODY.OrderID);
}

// �������� (OCS -> OHT) 
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS 										STATUS;	
} PKT_SIMPLE_VERSION_REQUEST;


void Conv_PKT_SIMPLE_VERSION_REQUEST(PKT_SIMPLE_VERSION_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	
}

// OHT Using Protocol Packet ---------------------------------------------
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_STATUS_REPORT_ID_03;						// CLW Status ���� (CLW��MCP) (ID:03)

void Conv_PKT_STATUS_REPORT_ID_03(PKT_STATUS_REPORT_ID_03& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_TAKEOUT_REPORT_ID_17;						// CLW ���� ���� (CLW��MCP) (ID:17)

void Conv_PKT_TAKEOUT_REPORT_ID_17(PKT_TAKEOUT_REPORT_ID_17& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
} PKT_DEF_REQ_ID_19;						//  ��ȸ���� ��û (CLW��MCP) (ID:19)

void Conv_PKT_DEF_REQ_ID_19(PKT_DEF_REQ_ID_19& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

//============================Station ���� ����  =========================//
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112		BODY;
} PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112;							// MCP -> OHT(ID :112

void Conv_PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112(PKT_STATION_INFO_RESPONSE_MCP_TO_CLW_ID_112& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.StationData_StationID);
	ConvertEndian(&Pkt.BODY.StationData_ParentNodeID);
	ConvertEndian(&Pkt.BODY.StationData_NextNodeID);
	ConvertEndian(&Pkt.BODY.StationData_Offset);
	ConvertEndian(&Pkt.BODY.StationData_PhysicalID);

	ConvertEndian(&Pkt.BODY.TeachingData_StationID);
	ConvertEndian(&Pkt.BODY.TeachingData_Hoist);
	ConvertEndian(&Pkt.BODY.TeachingData_Shift);
	ConvertEndian(&Pkt.BODY.TeachingData_Rotate);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOID);
	ConvertEndian(&Pkt.BODY.TeachingData_PIOChannel);
	ConvertEndian(&Pkt.BODY.TeachingData_STBOffset);

	ConvertEndian(&Pkt.BODY.PathData_ExtraNum);
}
//============================Station ���� ����  MCP -> OHT(ID :112)=========================//
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_RESPONSE_CLW_TO_MCP_ID_122				BODY;

} PKT_JOB_ORDER_RESPONSE_ID_122;				// CLW -> MCP �۾����� ���� (ID:122)

void Conv_PKT_JOB_ORDER_RESPONSE_ID_122(PKT_JOB_ORDER_RESPONSE_ID_122& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.CmdID);
}



typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_PROGRESS_REPORT_CLW_TO_MCP_ID_13			BODY;
} PKT_JOB_PROGRESS_REPORT_ID_13;				// CLW -> MCP ��ô���� (ID:103)  AOHC-297


void Conv_PKT_JOB_PROGRESS_REPORT_ID_13(PKT_JOB_PROGRESS_REPORT_ID_13& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.RcvCmdID);
}

//AOHC-296
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_JOB_WARNING_REPORT_CLW_TO_MCP_ID_29			BODY;
} PKT_JOB_WARNING_REPORT_ID_29;				// CLW -> MCP ����� (ID:29)

void Conv_PKT_JOB_WARNING_REPORT_ID_29(PKT_JOB_WARNING_REPORT_ID_29& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.WarningMessageID);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_PASSPERMIT_OHT2OCS_ID_241						BODY;
} PKT_PASSPERMIT_OHT2OCS_ID_241;				// ��� �㰡 �䱸 (ID:241)

void Conv_PKT_PASSPERMIT_OHT2OCS_ID_241(PKT_PASSPERMIT_OHT2OCS_ID_241& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITOPEN_OHT2OCS_ID_243				BODY;
} PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243;		// ��� �㰡 ���� �䱸 (ID:243)

void Conv_PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243(PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246			BODY;
} PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246;	// ��� �㰡 ���� ���� (ID:246)

void Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

//	ConvertEndian(&Pkt.BODY.Finish_PassPermitNum);

	//OHT Main ����Ǵ� �κ� 20160421 -------�ذ�. puting
	for(int i=0;  i<Pkt.BODY.Finish_PassPermitNum; i++)
	{
		ConvertEndian(&Pkt.BODY.PassPermit_CM[i].SQNum);
	}

}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247		BODY;
} PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247;	// ��� �㰡 ���� ���� �䱸 (ID:247)

void Conv_PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247(PKT_CMD_PASSPERMITFORCEOPEN_OHT2OCS_ID_247& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

//	ConvertEndian(&Pkt.BODY.Finish_PassPermitNum);

}
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_CID_ERROR_REPORT_OHT2OCS_ID_249				BODY;
} PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249;		// CID �̻� ���� (ID:249)

void Conv_PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249(PKT_CMD_CID_ERROR_REPORT_OHT2OCS_ID_249& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

//soming
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_PASSPERMIT_CANCEL_OCS2OHT_ID_270		        BODY;
} PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270;	// ��� �㰡 ���(ID:270)

void Conv_PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270(PKT_PASSPERMIT_CANCEL_OCS2OHT_ID_270& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271	    BODY;
} PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271;	// ��� �㰡 ��� ����(ID:271)

void Conv_PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271(PKT_RESPONSE_PASSPERMIT_CANCEL_OHT2OCS_ID_271& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.PassPermit_CM.SQNum);
}

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_REQUEST_OHT2OCS					BODY;
} PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS;
												// ������ ���� �䱸
												
void Conv_PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}

												
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS			BODY;
} PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE;

void Conv_PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}												// ������ ���� ���� ����								


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_DATA_TRANSPER_PROGRESS_OHT2OCS				BODY;
} PKT_DATA_TRANSPER_ORDER_PROGRESS;
												// ������ ���� ��ô ����

void Conv_PKT_DATA_TRANSPER_ORDER_PROGRESS(PKT_DATA_TRANSPER_ORDER_PROGRESS& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.FailClass);
}
		
//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252		 	BODY;
} PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252;	// ������ȯ   (ID:252)

void Conv_PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252(PKT_RESPONSE_AUTOCHANGE_OCS_OHT_ID_252& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

}

 typedef struct
{
	PROTOCOL_HEADER					            	HEADER;
	PROTOCOL_TAG						            TAG;
	PROTOCOL_STATUS_OHT					            STATUS;
	PROTOCOL_BODY_CMD_CARRIERINFO_OHT_OCS_1301		BODY;

}PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301;

void Conv_PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301(PKT_CMD_CARRIERINFO_OHT_OCS_ID_1301& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
    ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
	ConvertEndian(&Pkt.BODY.CmdID);

}

typedef struct
{
	PROTOCOL_HEADER					                	HEADER;
	PROTOCOL_TAG					                	TAG;
	PROTOCOL_STATUS_OCS					                STATUS;
	PROTOCOL_BODY_RESPONSE_CARRIERINFO_OCS_OHT_1302		BODY;

}PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302;

void Conv_PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302(PKT_CMD_RESPONSE_CARRIERINFO_OCS_OHT_ID_1302& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);
    ConvertEndian(&Pkt.BODY.CmdID);

}

// ------------ OHT Main����� �Ƿ� (OHT -> AS�� ��û) --------------
typedef struct
{
	int	iOhtMainCopyNeed;	// 0 : No,  1 : Need Copy
	int	iOhtParamCopyNeed;	// 0 : No,  1 : Need Copy
	DWORD OHTPID;
	int ShutDownDelay;
} PROTOCOL_BODY_RESTART_INFO_OHT2AS;

// OHT�� �ڽſ��� �������� �ְ�, ����� ���� �޶�� AS�� ��û�ϴ� ��Ŷ
typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESTART_INFO_OHT2AS							BODY;
} PKT_RESTART_REQUEST_OHT2AS;

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_RESTART_INFO_OHT2AS							BODY;
} PKT_RESTART_KEEPING_INFO_AS2OHT;

// -------------------------------------------------------------------

// AGENT�κ��� �߿��� ���� ��û�޴� ��Ŷ
typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT                         STATUS;
}PKT_FIRMWARE_VERSION_REQUEST_AGENT2OHT;


// AGENT ���� �߿��� ������ �����ϴ� ��Ŷ
typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_STATUS_OHT                         STATUS;
	PROTOCOL_FW_VERSION_OHT						BODY;
}PKT_FIRMWARE_VERSION_RESPONSE_OHT2AGENT;

// ------------ ������� ������Ʈ �Ǵ� �������б� �Ƿ� (OHT -> AS�� ��û) --------------
#define WIRELESS_PUT	0
#define WIRELESS_GET	1
typedef struct
{
	int						iCmd;				// 0 = put, 1 = get
	
	unsigned char			IpAddress[4];
	char					UserName[17];		// ������
	char					PassWord[17];		// �н�����
	char					FilePath[512];		// Local Path
	char					DataSendTo[256];	// ������ ����ó (Remote)

	int iWirelessConfig;	// TRUE, FALSE. ������� Config ������Ʈ ���θ� �����Ѵ�
	int iWirelessMacfil;	// TRUE, FALSE. ������� Macfil ������Ʈ ���θ� �����Ѵ�
	int iWirelessFirmware;	// TRUE, FALSE. ������� Firmware ������Ʈ ���θ� �����Ѵ�

} PROTOCOL_BODY_FTP_PUT_GET_INFO_OHT2AS;


typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OCS											STATUS;
	PROTOCOL_BODY_FTP_PUT_GET_INFO_OHT2AS						BODY;
} PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS;

// -------------------------------------------------------------------


// ------------ ������� ������Ʈ �Ǵ� �������б� ��� (AS -> OHT) --------------
typedef struct
{
	int						iResult;
	char					ResultText[512];
	
} PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT;

typedef struct
{
	PROTOCOL_HEADER												HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT											STATUS;
	PROTOCOL_BODY_FTP_PUT_GET_RESULT_AS2OHT						BODY;
} PKT_FTP_PUT_GET_RESULT_AS2OHT;

#define WIRELESS_UPDATE_SUCCESS	0x00
#define WIRELESS_UPDATE_FAIL	0x01

// -------------------------------------------------------------------

typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_OHT_VERSION_OHT2OCS							BODY;
} PKT_VERSION_RESPONSE;

												// OHT�� ������ ����	
void Conv_PKT_VERSION_RESPONSE(PKT_VERSION_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_OHT_INPUT_REQUEST								BODY;
} PKT_OHT_INPUT_REQUEST;
												// OHT�� ������ OCS�� �䱸								
void Conv_PKT_OHT_INPUT_REQUEST(PKT_OHT_INPUT_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);	

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_FLOOR_MOVE_ORDER_RESPONSE						BODY;
} PKT_FLOOR_MOVE_ORDER_RESPONSE;
												 // �����̵���������								
void Conv_PKT_FLOOR_MOVE_ORDER_RESPONSE(PKT_FLOOR_MOVE_ORDER_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);  

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);

	ConvertEndian(&Pkt.BODY.OrderID);
}


// �������� (OHT -> OCS) 
typedef struct
{
	PROTOCOL_HEADER 											HEADER;
	PROTOCOL_TAG												TAG;
	PROTOCOL_STATUS_OHT 										STATUS;
	PROTOCOL_BODY_VERSION_RESPONSE								BODY;
} PKT_SIMPLE_VERSION_RESPONSE;

void Conv_PKT_SIMPLE_VERSION_RESPONSE(PKT_SIMPLE_VERSION_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.TAG.Value);  

	ConvertEndian(&Pkt.STATUS.RunningDistance);
	ConvertEndian(&Pkt.STATUS.RunningSpeed);
	ConvertEndian(&Pkt.STATUS.ErrorCode);
}


typedef struct _STANDARD_NODE_INFO
{
	unsigned int Num1			: 4;		///<��� �ΰ� ����( 0:�ٷ� ����, 1:���� ���������� ���� ������ ��� �ΰ�
	unsigned int Num2			: 4;		///<����� �ӵ�( 0: ��� ���� �ӵ�, 1: ���� �ӵ�, 2:Min(��ɼӵ�, ����ӵ�), 4:MAX(��ɼӵ�, ����ӵ�) )
	unsigned int Num3			: 4;		///<Small Add ( 0: NoUse, 1: Use )
}STANDARD_NODE_INFO;


// AMC_CMD.Cmd �� ���� ����� �м��ϱ� ���� union
typedef union _STANDARD_NODE_INFO_UNION
{
	STANDARD_NODE_INFO NodeIDInfo;	///< AMC_CMD.Cmd�� bitfield
	unsigned int NodeID;
}STANDARD_NODE_INFO_UNION;



// ============================= DIAG Server - OHT Define ==============================
namespace DiagCmdID
{
	enum Reserved
	{
	// �۾����� ���� ��ɵ�
		AGENT_TO_CLW_CONNECT_CHK_REQUEST		= 0x0001,	// ���� Ȯ�� ��û (���� Agent -> EQ)
		CLW_TO_AGENT_CONNECT_CHK_RESPONSE		= 0x0002,   // ���� Ȯ�� ���� (EQ -> ���� Agent)

		DIAG_INFO_DATA_REQUEST					= 0x0111,	// ����������� ��û (���� Agent -> �Ϲ� OHT) 
		DIAG_INFO_DATA_RESPONSE					= 0x0112, 	// ����������� (�Ϲ� OHT -> ���� Agent) 
	
		DIAG_DATA_RESET_REQUEST					= 0x0113, 	// ������� �ʱ�ȭ ��û (���� Agent -> �Ϲ� OHT)
		DIAG_DATA_RESET_RESPONSE				= 0x0114, 	// ������� �ʱ�ȭ ���� (�Ϲ� OHT -> ���� Agent)

		DIAG_INFO_LEVEL_CHANGE_REQUEST			= 0x0115, 	// DELETED - ������� ���� ���� ��û (���� Agent -> �Ϲ� OHT)
		DIAG_INFO_LEVEL_CHANGE_RESPONSE			= 0x0116, 	// DELETED - ������� ���� ���� ���� (�Ϲ� OHT -> ���� Agent)

		//���� �߰��� �κ� //
		DIAG_INFO_STATUS_REPORT					= 0x0118, 	// �������� ����  (�Ϲ� OHT -> ���� Agent)
		DIAG_INFO_ERROR_REPORT					= 0x0120, 	// �̻��̷� ���� (�Ϲ� OHT -> ���� Agent)
		DIAG_INFO_STATISTICS_REQUEST			= 0x0122, 	// ������� �䱸 (�Ϲ� OHT -> ���� Agent)
		DIAG_INFO_STATISTICS_RESPONSE			= 0x0123, 	// ������� ���� (���� Agent -> �Ϲ� OHT)
		DIAG_INFO_CIDMONITORING_REPORT			= 0x0124, 	// CID ���� Monitoring ���� ���� (OHT -> ���� Agent)
		//���� �߰��� �κ� //

		CLEAN_CLW_TO_AGENT_LINK_INFO			= 0x0212, 	// ��ũ�������� (SEMES�� û�� OHT -> ���� Agent)

		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};


// �����׸� ID
namespace DiagParamID
{
	enum Reserved
	{
	// �۾����� ���� ��ɵ�
		RUNNING_DISTANCE				= 0x10,			// ���� �������� ���� ����Ÿ�
		STEERING_COUNT					= 0x20,   		// Steering ���� ���� Ƚ��		

		LOAD_UNLOAD_COUNT				= 0x30,			// Shift ���� ���� Ƚ��
		LOAD_UNLOAD_ROT_COUNT			= 0x31, 		// Shift ���� ���� ȸ����
	
		HOIST_COUNT						= 0x40, 		// Hoist ���� ���� Ƚ��
		HOIST_ROT_COUNT					= 0x41, 		// Hoist ���� ���� ȸ����
		
		ROTATE_COUNT					= 0x50, 		// Rotate ���� ���� Ƚ��
		HAND_COUNT						= 0x60,			// Hand ���� ���� Ƚ��		
		SHUTTER_COUNT					= 0x70, 		// Shutter ���� ���� Ƚ��
		
		RUN_NODE_BARCODE_READ_COUNT		= 0x80, 		// ������ ���ڵ帮�� ���� ���� Ƚ��
		STATION_NODE_BARCODE_READ_COUNT	= 0x81, 		// �����̼ǳ�� ���ڵ帮�� ���� ���� Ƚ��
		
		LOOK_DOWN_SENSOR_TIMES			= 0x82, 		// Look down ���� ���� ���� �ð�		
		UBG_SENSOR_TIMES				= 0x84, 		// UBG ���� ���� ���� �ð�
		PBS_SENSOR_TIMES				= 0x85, 		// PBS ���� ���� ���� �ð� (��ֹ� ����)		

		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};

// �̻��׸� ID
namespace ErrorParamID
{
	enum Reserved
	{
	// �۾����� ���� ��ɵ�
		MEMORY_ERROR					= 0x10,			// Memory ��뷮 �̻�
		HAND_MOVING_ERROR				= 0x20,   		// Hand ���� �ð� �̻�
		SHUTTER_MOVING_ERROR			= 0x21,			// Shutter ���� �ð� �̻�

		STEERING_MOVIING_ERROR			= 0x22,			// Steering ���� �ð� �̻�
		HAND_ALRAM_RESET_COUNT_ERROR	= 0x23, 		// Hand Alarm Reset Ƚ�� �̻� (�̻�Ƚ�� �̻� �߻��ø��� ����)

		PBS_ERROR						= 0x30, 		// PSB Ż�� �߻�(��ġ���� ����)
		UBG_ERROR						= 0x31, 		// UBG Ż�� �߻�(��ġ���� ����)

		BCR_RETRY_COUNT_ERROR		 	= 0x32, 		// BCR Retry Ƚ�� �̻�
		FORNT_REAR_ENCODER_GAP_ERROR	= 0x40, 		// ���� ��/�ķ� ���ڴ� �� ���� �̻�(���ڴ� ���� �̻� Ƚ���� �ϴ����� Reset�ϸ� �̻�Ƚ�� �̻� �߻��ø��� ������. )


		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};


typedef struct DIAG_HEADER_ 
{
	
	unsigned short 		MachineType;	// 2Byte, MACHINETYPE_DIAG_SVR, MACHINETYPE_DIAG_NORMAL_OHT
	char 				MachineID[6];	// 6Byte, Name 
	unsigned short 		CommandID;		// 2Byte, ID�ּ��� ����(ADDRESS_DIAG_BROADCAST_DEF), ����(ADDRESS_DIAG_UNICAST_REPLY) 
} DIAG_HEADER;


typedef struct DIAG_PKT_CHECK_TAG_
{
	DIAG_HEADER								HEADER;
} DIAG_PKT_CHECK_TAG;								// �������� ù �ؼ����� TAG ������ ��ġ�� ã�� ���� ������ ����.

void Conv_DIAG_PKT_CHECK_TAG(DIAG_PKT_CHECK_TAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// OHT -> DIAG SVR standart packet
typedef struct PKT_OHT2DIAG_
{
	DIAG_HEADER		HEADER;
	unsigned char	Value[MAX_PACKET_SIZE-sizeof(DIAG_HEADER)];
} PKT_OHT2DIAG;

void Conv_PKT_OHT2DIAG(PKT_OHT2DIAG& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}


typedef struct
{
	PKT_OHT2DIAG                                AckPkt;
	UINT 										Length;
} PKT_ACK_OHT2DIAG;								// �������� ù �ؼ����� TAG ������ ��ġ�� ã�� ���� ������ ����.


// ���� Ȯ�� ��û (���� Agent -> EQ)
typedef struct DIAG_PKT_CONNECT_CHK_REQUEST_
{
	DIAG_HEADER				HEADER;
} DIAG_PKT_CONNECT_CHK_REQUEST;

void Conv_DIAG_PKT_CONNECT_CHK_REQUEST(DIAG_PKT_CONNECT_CHK_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}
// ���� Ȯ�� ���� (EQ -> ���� Agent)
typedef struct DIAG_PKT_BODY_CONNECT_CHK_RESPONSE_
{
	char					result;
} DIAG_PKT_BODY_CONNECT_CHK_RESPONSE;

typedef struct DIAG_PKT_CONNECT_CHK_RESPONSE_
{
	DIAG_HEADER							HEADER;
    DIAG_PKT_BODY_CONNECT_CHK_RESPONSE BODY;
} DIAG_PKT_CONNECT_CHK_RESPONSE;

void Conv_DIAG_PKT_CONNECT_CHK_RESPONSE(DIAG_PKT_CONNECT_CHK_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// ����������� ��û (���� Agent -> �Ϲ� OHT)
typedef struct DIAG_PKT_BODY_INFO_DATA_REQUEST_
{
	char Parameter;		// �����׸� ID
} DIAG_PKT_BODY_INFO_DATA_REQUEST;

typedef struct DIAG_PKT_INFO_DATA_REQUEST_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_DATA_REQUEST		BODY;
} DIAG_PKT_INFO_DATA_REQUEST;

void Conv_DIAG_PKT_INFO_DATA_REQUEST(DIAG_PKT_INFO_DATA_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}


// ����������� (�Ϲ� OHT -> ���� Agent) 
typedef struct DIAG_PKT_BODY_INFO_DATA_RESPONSE_
{
	char Parameter;		// �����׸� ID
	UINT64 Value;		// ���谪 1~8
} DIAG_PKT_BODY_INFO_DATA_RESPONSE;

typedef struct DIAG_PKT_INFO_DATA_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_DATA_RESPONSE	BODY;
} DIAG_PKT_INFO_DATA_RESPONSE;

void Conv_DIAG_PKT_INFO_DATA_RESPONSE(DIAG_PKT_INFO_DATA_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.Value);
}


// ������� �ʱ�ȭ ��û (���� Agent -> �Ϲ� OHT)
typedef struct DIAG_PKT_BODY_DATA_RESET_
{
	char Parameter;		// �����׸� ID
} DIAG_PKT_BODY_DATA_RESET;

typedef struct DIAG_PKT_DATA_RESET_REQUEST_
{
	DIAG_HEADER					HEADER;
	DIAG_PKT_BODY_DATA_RESET	BODY;
} DIAG_PKT_DATA_RESET_REQUEST;

void Conv_DIAG_PKT_DATA_RESET_REQUEST(DIAG_PKT_DATA_RESET_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// ������� �ʱ�ȭ ���� (�Ϲ� OHT -> ���� Agent)
typedef struct DIAG_PKT_BODY_DATA_RESET_RESPONSE_
{
	char Parameter;		// �����׸� ID
	char SuccessCode;	// 0x00 (����) / �����ڵ�
} DIAG_PKT_BODY_DATA_RESET_RESPONSE;

typedef struct DIAG_PKT_DATA_RESET_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_DATA_RESET_RESPONSE	BODY;
} DIAG_PKT_DATA_RESET_RESPONSE;

void Conv_DIAG_PKT_DATA_RESET_RESPONSE(DIAG_PKT_DATA_RESET_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}



// ---------------- DELETED SEPC ------------------------
// ������� ���� ���� ��û (���� Agent -> �Ϲ� OHT)
typedef struct DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST_
{
	char Parameter;		// �����׸� ID
	UINT64 Value;		// ���谪 1~8
} DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST;

typedef struct DIAG_PKT_LEVEL_CHANGE_REQUEST_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_LEVEL_CHANGE_REQUEST	BODY;
} DIAG_PKT_LEVEL_CHANGE_REQUEST;

void Conv_DIAG_PKT_LEVEL_CHANGE_REQUEST(DIAG_PKT_LEVEL_CHANGE_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.Value);
}


// ������� ���� ���� ���� (�Ϲ� OHT -> ���� Agent)
typedef struct DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE_
{
	char Parameter;		// �����׸� ID
	char SuccessCode;	// 0x00 (����) / �����ڵ�
} DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE;

typedef struct DIAG_PKT_LEVEL_CHANGE_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_LEVEL_CHANGE_RESPONSE	BODY;
} DIAG_PKT_LEVEL_CHANGE_RESPONSE;

void Conv_DIAG_PKT_LEVEL_CHANGE_RESPONSE(DIAG_PKT_LEVEL_CHANGE_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}
// --------------------------------------------------------------



// ��� �������� ���� (�Ϲ� OHT -> ���� Agent)
typedef struct DIAG_PKT_BODY_INFO_STATUS_REPORT_
{
	char IsUSBWrite;		// USB ��� ����
	char IsUSBCheck;		// USB �ν� ����
	char IsFANCheck;		// Fan ���� ����
	char IsC_Check;			// C ����̹� �뷮 ���� ����
	char IsD_Check;			// D ����̹� �뷮 ���� ����
} DIAG_PKT_BODY_INFO_STATUS_REPORT;

typedef struct DIAG_PKT_INFO_STATUS_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_INFO_STATUS_REPORT	BODY;
} DIAG_PKT_INFO_STATUS_REPORT;

void Conv_DIAG_PKT_INFO_STATUS_REPORT(DIAG_PKT_INFO_STATUS_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// �̻��̷� ����  (�Ϲ� OHT -> ���� Agent)
typedef struct DIAG_PKT_BODY_ERROR_REPORT_
{
	char ErrorID;		// �̻��׸� ID
	UINT  NodeID;		//  NODE

} DIAG_PKT_BODY_ERROR_REPORT;

typedef struct DIAG_PKT_ERROR_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_ERROR_REPORT			BODY;
} DIAG_PKT_ERROR_REPORT;

void Conv_DIAG_PKT_ERROR_REPORT(DIAG_PKT_ERROR_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	 ConvertEndian(&Pkt.BODY.NodeID);
}

// ������� ��û (�Ϲ� OHT -> ���� Agent)
typedef struct DIAG_PKT_DATA_INFO_REQUEST_
{
	DIAG_HEADER				HEADER;
} DIAG_PKT_DATA_INFO_REQUEST;

void Conv_DIAG_PKT_DATA_INFO_REQUEST(DIAG_PKT_DATA_INFO_REQUEST& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);
}

// ������� ���� (���� Agent -> OHT)
typedef struct DIAG_PKT_BODY_DATA_INFO_RESPONSE_
{
	UINT64 InfoData[14];		// ����������

} DIAG_PKT_BODY_DATA_INFO_RESPONSE;

typedef struct DIAG_PKT_DATA_INFO_RESPONSE_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_DATA_INFO_RESPONSE	BODY;
} DIAG_PKT_DATA_INFO_RESPONSE;

void Conv_DIAG_PKT_DATA_INFO_RESPONSE(DIAG_PKT_DATA_INFO_RESPONSE& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	for(int i=0; i<14; i++)
		ConvertEndian(&Pkt.BODY.InfoData[i]);
}

// CID ���� Monitoring ���� ����  (�Ϲ� OHT -> ���� Agent)
// 2015-06-01 Spec changed
typedef struct DIAG_PKT_BODY_CID_REPORT_
{
	int	 StopNodeID;		// STOP NODE
	int  ResetNodeID;		//  RESET NODE
	char Reseult;            //��� ��� (���� : 0x00, ���� : 0x01)
	// char CIDStatus[25];       //CID ���� Monitoring ����(25Byte ASCII)
	//char CIDStatus[47];       //CID ���� Monitoring ����(38Byte ASCII) => 47Byte Changed
	char CIDStatus[53]; 	  //CID ���� Monitoring ����(47Byte ASCII) => 53Byte Changed
	char CIDErrorInfo[3];		//��� �߻� ���� 

} DIAG_PKT_BODY_CID_REPORT;

typedef struct DIAG_PKT_CID_REPORT_
{
	DIAG_HEADER							HEADER;
	DIAG_PKT_BODY_CID_REPORT			BODY;
} DIAG_PKT_CID_REPORT;

void Conv_DIAG_PKT_CID_REPORT(DIAG_PKT_CID_REPORT& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.StopNodeID);
	ConvertEndian(&Pkt.BODY.ResetNodeID);
}


// ��ũ�������� (SEMES�� û�� OHT -> ���� Agent)
typedef struct DIAG_PKT_BODY_CLEAN_LINK_INFO_
{
	int FromNode;		// ��ũ ���� node
	int ToNode;			// ��ũ �� node
} DIAG_PKT_BODY_CLEAN_LINK_INFO;

typedef struct DIAG_PKT_CLEAN_LINK_INFO_
{
	DIAG_HEADER						HEADER;
	DIAG_PKT_BODY_CLEAN_LINK_INFO	BODY;
} DIAG_PKT_CLEAN_LINK_INFO;

void Conv_DIAG_PKT_CLEAN_LINK_INFO(DIAG_PKT_CLEAN_LINK_INFO& Pkt)
{
	ConvertEndian(&Pkt.HEADER.MachineType);
	ConvertEndian(&Pkt.HEADER.CommandID);

	ConvertEndian(&Pkt.BODY.FromNode);
	ConvertEndian(&Pkt.BODY.ToNode);
}
//���� ������ ����
typedef struct
{
	PROTOCOL_HEADER							HEADER;
	PROTOCOL_TAG							TAG;
	AVS_VIBRATION_DATA_INFO					BODY;
} PKT_OHT2TP_VIBRATION_INFO;


typedef struct
{
	BYTE Mode;            		//0x00 ���, 0x01: �����̼�, 0x02: ��ũ
	int  StartID;	    		//��������
	int  LinkCount;         	//��ũ ī��Ʈ ����
	int  LaneCutList[20][2];    //������ ����
}PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ	BODY;
}PKT_TP2OHT_AUTO_DRIVING_TEST_ROUTE_REQ;


typedef struct
{
	bool  Success;			//��������
}PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_RESPONSE;

typedef struct
{
	PROTOCOL_HEADER									HEADER;
	PROTOCOL_TAG									TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_RESPONSE	BODY;
}PKT_OHT2TP_AUTO_DRIVING_TEST_ROUTE_RESPONSE;


typedef struct
{
	BYTE Mode;            		//0x00 ���, 0x01: �����̼�, 0x02: ��ũ
	int RouteList[50];    	//����������
	int RouteCount;				//�����ΰ���
	int StartID;              //������
	int EndID;                //����
}PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ;

typedef struct
{
	PROTOCOL_HEADER								HEADER;
	PROTOCOL_TAG								TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ	BODY;
}PKT_TP2OHT_AUTO_DRIVING_TEST_MOVE_REQ;



typedef struct
{
	bool Success;	    		//��������
}PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE;

typedef struct
{
	PROTOCOL_HEADER									HEADER;
	PROTOCOL_TAG									TAG;
	PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE			BODY;
}PKT_OHT2TP_AUTO_DRIVING_TEST_MOVE_RESPONSE;



#pragma pack(pop) //�� ����Ʈ�� ��ŷ..

#endif


