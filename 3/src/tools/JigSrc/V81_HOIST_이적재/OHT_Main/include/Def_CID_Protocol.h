
#ifndef Def_CID_ProtocolH
#define Def_CID_ProtocolH

// CID ��� ���� (CID-O�� Operatin Mode (O6)
#define	CID_MODE		1	// ���� CID �켱�� ��� ���� ���
#define MTL_LIFTER_MODE	0	// ���������Ϳ� �������� CID-O/R�� ����� ���� ���

#define MTL_AREA_CID_INPUT_NUM	5

// ----- �ø���� �����ϴ� CID�� ���� ��Ŷ ���� (�ʱ�ȭ) -----
#define ELEVETOR_HIGH_BYTE	0
#define ELEVETOR_LOW_BYTE	1

typedef struct
{
	unsigned char			STX;		// �۽� Data�� ����. 0 x02
	unsigned char			CMD;		// CMD(Command)�� "i"(Ascii Code 69 H)
	unsigned char			Reserve[2]; // "00"(Ascii Code 30 H 30 H), ��������Ʈ�� ���� ����
	unsigned char			SUM[2];		// CMD�κ��� SUM�� ���������� ��� ���� ��
	unsigned char			ETX;		// �۽� Data�� ������. 0 x03
} OPC_INIT_PKT;

#define OPC_INIT_PKT_CHECKSUM_LENGTH		4	// üũ�� 4����Ʈ

// ��Ŷ�� �⺻������ �� ���� ������. �����̹Ƿ� ���ǵ� ���� �������
#define OPC_SERIAL_INIT_STX		0x02
#define OPC_SERIAL_INIT_CMD		0x69	// Ascii 'i'
#define OPC_SERIAL_INIT_RESERVE 0x30
#define OPC_SERIAL_INIT_ETX		0x03

#define OPC_SERIAL_INIT_TEST	0xff


// Monitoring OHT -> Agent ID:124
#define CID_MONITOR_INFO_GET_SUCCESS	0x00 // ��� ���� 
#define CID_MONITOR_INFO_GET_STATUS		0x01 //��� ������ ���
#define CID_MONITOR_INFO_GET_MONITORING	0x10 //monitoring ������ ���
#define CID_MONITOR_INFO_GET_FAIL		0x11 // ��� ����

// Monitoring OHT -> Agent ID:249
#define CID_MONITOR_INFO_GET_SUCCESS_249	0x00 // ��� ���� 
#define CID_MONITOR_INFO_GET_FAIL_249		0x01 // ��� ����


#define CID_SERIAL_CMD_DELAY_TURN		10	// 5 - error. 7 - success.  

// ----- �ø���� �����ϴ� CID�� ���� ��Ŷ ���� (�����) -----
// ==> OHT�� CID-O�� CID-R �� �۽��ϴ� ��Ŷ�� �����̴�
typedef struct
{
	unsigned char			STX;		 // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			OHTNAME[6]; // ȣ�� No
	unsigned char			Priority[2]; // �ݼ۹��� �켱��. Ascii Code�� ����
	unsigned char			Turn;		 // ��/�¼���
	unsigned char			LightGuideID; // CID-R ID
	unsigned char			SUM[2];
	unsigned char			ETX;		// �۽� Data�� ������. 0 x03
} OPC_TO_TRANSPER_PKT;

#define OPC_TO_TRANSPER_PKT_CHECKSUM_LENGTH		12	// üũ�� 12����Ʈ

#define OPC_TO_TRANSPER_CMD	0x76		// VHL�� OPC ���� -> ���� ��ġ Command ('v')

#define PRIORITY_LEVEL_1	'1'
#define PRIORITY_LEVEL_2    '2'
#define PRIORITY_LEVEL_3    '3'
#define PRIORITY_LEVEL_4    '4'
#define PRIORITY_LEVEL_5    '5'
#define PRIORITY_LEVEL_6    '6'
#define PRIORITY_LEVEL_7    '7'
#define PRIORITY_LEVEL_8    '8'
#define PRIORITY_LEVEL_9    '9'

#define PRIORITY_LEVEL_NOT_COMMUNICATION	'0'
#define PRIORITY_LEVEL_ERR 					'F'
#define PRIORITY_LEVEL_NETWORK_ERR 			'E'
#define PRIORITY_LEVEL_INCHING              '@'

#define PASS_DIRECTION_REQUEST_RIGHT		'R'
#define PASS_DIRECTION_REQUEST_LEFT			'L'
#define PASS_DIRECTION_REQUEST_ALL			'N'
#define PASS_DIRECTION_REQUEST_INCHING		'@'
#define PASS_DIRECTION_REQUEST_PASS 		'P' // AOHC-217 VHLLifter �ӵ� ���� Lifter ����ϴ� ��� by jhoun.lim 2018.06.04
#define PASS_DIRECTION_REQUEST_BEFORECHECK	'B' // AOHC-217 VHLLifter �ӵ� ���� Lifter ����ϴ� ��� by jhoun.lim 2018.11.14

#define LIGHT_GUIDE_ID_INPUT_UP				'X'
#define LIGHT_GUIDE_ID_INPUT_DOWN			'Y'
#define LIGHT_GUIDE_ID_INPUT_UP_PRE			'x' // AOHC-217 VHLLifter �ӵ� ���� by jhoun.lim 2018.06.04
#define LIGHT_GUIDE_ID_INPUT_DOWN_PRE		'y' // AOHC-217 VHLLifter �ӵ� ���� by jhoun.lim 2018.06.04
#define LIGHT_GUIDE_ID_NONE					'N' // AOHC-217 VHLLifter �ӵ� ���� by jhoun.lim 2018.06.04


#define DELAY_CID_COMMAND					20	// ������ �ּ� 10ms������ ��� ��
#define DELAY_SERIAL_READ	    			70	// ��翡�� �ּ� 50ms�̳�, ������ �̺��� �ణ �� �� �ð��� �ʿ��ϴ�
#define DELAY_SERIAL_RESET	    			90  // ��翡�� �������� ������, ������ ������ �ð��� �ʿ��ϴ� (50ms - ����, 70ms - ��Ȥ �����߻�)
#define DELAY_SERIAL_WAIT_N_REQUEST_RETRY	100

#define LIFTER_PASS_TIME_OUT_MS				240000 // 4min

/*
Priority : �ݼ۹� �켱�� Ascii Code�� ����
            VHL Lifter������ Priority ������ �����ϰ� Lifter Rail�� �����ġ�� ���� �����ų VHL�� �����Ѵ�.
            ��00��             ...VHL�� ����ϰ� ���� �ʴ� ����
            ��01��������99�� ...VHL�� ���� ����. ��01���� ���� ���� Priority. ��99���� ���� ���� Priority
			��FF��             ... VHL�� �̻�
            ��EE��             ... VHL�� �̻�(Network ��ȯ ����)
            ��@@��           ... VHL�� Inching ���� ��

��/�� ���� : VHL�κ��� CID�� ����䱸. Ascii Code�� ����
                ��R��               ...VHL�� ������� ��� �䱸
                ��L��               ...VHL�� �¹����� ��� �䱸
                ��N��               ...VHL�� ���� ���� ���� ��� �䱸. �бⰡ ���� ���(�� : 180�� Curve ��)������ ��� �䱸
                ��@��              ... VHL�� Inching ���� ��


CID-R ID :  ������ VHL�� CID�� �������� ��Ű���, ����ġ�� ��� �Ұ���. Ascii Code�� ����
               CID-R ID�� ���� Ascii Code ���� ��A������ ��Z�� �� ��a������ ��z�������� �������� ���� ���� ����

���VHL�� ��� �Ҵ��� CID-R������, ȣ�� No., Priority, ��/�켱���� ���"0"(Ascii Code 30 H)�� �ȴ�.
*/


// ----- �ø���� �����ϴ� CID�� ���� ��Ŷ ���� (�����) -----
// ==> CID-R�� OHT�� CID-O�� �����ϴ� ��Ŷ�� �����̴�
typedef struct
{
	unsigned char			STX;		 	// �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			OHTNAME[6]; 	// ȣ�� No
	unsigned char			PassInfo;	 	// �������
	unsigned char			LightGuideID; 	//
	unsigned char			TransperStatus;	// ������ġ�� ����
	unsigned char			SUM[2];
	unsigned char			ETX;			// �۽� Data�� ������. 0 x03
} TRANSPER_TO_OPC_PKT;



//CID Monitoring Packet
//[���] [STX]M4D[ETX]
//[����] [STX]M=OHT0011000800001,CID00110065535240212224040080000000156[ETX]

typedef struct
{
	// Head : 3Byte
	unsigned char			STX;               // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			Equal;
} CID_Monitoring_HEADER;


typedef struct
{
	// Here is Body : 38Byte + 15 = Body Size 53Byte
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
	unsigned char			RFError_Per[2];			// ��� �� ���� ��� ����(%)
} CID_Monitoring_BODY;


typedef struct
{
	unsigned char			SUM[2];
	unsigned char			ETX;			// �۽� Data�� ������. 0 x03
} CID_Monitoring_TAIL;


typedef struct	// All Byte : 53Byte
{
	CID_Monitoring_HEADER	HEADER;
	CID_Monitoring_BODY		BODY;
	CID_Monitoring_TAIL		TAIL;
	
} CID_Monitoring_PKT;


//CID Status Packet
typedef struct
{
	unsigned char			Setting[3];
} CID_Status_BODY;

typedef struct
{
	CID_Monitoring_HEADER	HEADER;
	CID_Status_BODY			BODY;
	CID_Monitoring_TAIL		TAIL;
} CID_Status_PKT;


// CIDȣ���ȣ ����
typedef struct
{
	unsigned char			STX;		 // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			SEP;
	unsigned char			OHTNAME[6]; // ȣ�� No
	unsigned char			SUM[2];
	unsigned char			ETX;		// �۽� Data�� ������. 0 x03
} CID_NameSet_PKT;


// CIDȣ���ȣ Ȯ�ο�û
typedef struct
{
	unsigned char			STX;		 // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			SUM[2];
	unsigned char			ETX;		// �۽� Data�� ������. 0 x03
} CID_Name_Request_PKT;


// CIDȣ���ȣ Ȯ������
typedef struct
{
	unsigned char			STX;		 // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			SEP;
	unsigned char			OHTNAME[6]; // ȣ�� No
	unsigned char			SUM[2];
	unsigned char			ETX;		// �۽� Data�� ������. 0 x03
} CID_Name_Request_Response_PKT;


// shkim delete_reserve
#if 0
typedef struct	// All Byte : 53Byte
{
	// Head : 3Byte
	unsigned char			STX;               // �۽� Data�� ����. 0 x02
	unsigned char			CMD;
	unsigned char			Equal;
	// Here is Body : 38Byte + 9 = Body Size 47Byte
	unsigned char			OHTID[6];
	// unsigned char			CIDO_FW_Ver[3];			
	unsigned char			CIDR_FirstLinkLevel[3];	// CID-R ù �� �۽�(Link) ��ȣ ����
	unsigned char			CIDO_FiberCommError[2];	// ��� �� �� ��� ����(%)
	unsigned char			CIDO_RFCommError[2];	// ��� �� ���� ��� ����(%)
	unsigned char			delimiter;				// CID-O�� CID-R ������ ����
	unsigned char			CID_UniqueID[6];		// CID-���� ID
	// unsigned char			CIDR_FW_Ver[3];
	unsigned char			RF_ID[5];				// added (2016.10.30)
	unsigned char			RF_CH[4];				// added (2016.10.30)
	unsigned char			CIDR_MAXCurrent[3];		// ��� �� CID-R �ִ� ����
	unsigned char			CIDR_MAXVoltage[3];		// ��� �� CID-R �ִ� ����
	unsigned char			CIDR_MAXTemp[2];		// ��� �� CID-R �ִ� �µ�
	unsigned char			CIDR_StandbyFiberNoise[3];	// ��� ���� CID-R �� ������ ���Ű�
	unsigned char			CIDO_FirstLinkLevel[3];	// CID-O�� ù �� �۽�(Link) ��ȣ ����
	unsigned char			FiberError_Per[2];		// ��� �� �� ��� ����(%)
	unsigned char			RFError_Per[2];			// ��� �� ���� ��� ����(%)
	// unsigned char			Monitoring6;
	// Tail : 3Byte
	unsigned char			SUM[2];
	unsigned char			ETX;			// �۽� Data�� ������. 0 x03
} CID_Monitoring_PKT;
#endif
// ---------------------------------------------------

#define TRANSPER_TO_OPC_PKT_CHECKSUM_LENGTH		11	// üũ�� 11����Ʈ

#define TRANSPER_TO_OPC_CMD 					0x74			// ���� ��ġ -> VHL�� OPC ���� Command ('t')

// ZCU���� Light Guide Controller�� ���� OFF�� ���, ȣ�� No., ��� ����, Light Guide ID �� ���"0"(Ascii Code 30 H)
// �׷� ���¿����� ������ġ�� Status�� ��O��(����)
#define TRANSPER_TO_OPC_POWER_OFF_STATUS		0x30

/*
	[TransperStatus]
	ASCII Code �� ���� �� �׿� ���� �ǹ̴� �ϱ�� ����.

	����O����Ascii code 4FH�� ������
	-> ZCU�� ���� �� ��� DATA�� ���������� ���ŵ� ��� ��O��

	����P����Ascii code 50H�� ��Parity Error
	-> ZCU�� ���� �� ��� DATA �� Parity���� Ʋ���� ���ŵ� ��� ��P"
	?
	����S����Ascii code 53H�� ��SUM Check Error
	-> ZCU�� ���� �� ��� DATA �� SUM Chech���� Ʋ���� ���ŵ� ��� ��S��

	����E����Ascii code 45H�� ���� �� Error
	-> ZCU�� �� ��� �� �� ���� ������ �߻��� ��� ��E��


*/

#define TRANSPER_STATUS_DATA_RECEIVED_OK		0x4f	// "O", ����
#define TRANSPER_STATUS_PARITY_ERR				0x50	// "P", �и�Ƽ����
#define TRANSPER_STATUS_SUM_ERR					0x53   // "S", SUMüũ ����
#define TRANSPER_STATUS_OTHER_ERR				0x45	// "E", �׿� ����

#define TRANSPER_STATUS_DATA_NOT_RECEIVED		0x10
#define TRANSPER_STATUS_DATA_LENGTH_ERR			0x20

// ----- VHL ������ => CID-R �� ������ ��Ŷ �� ���� -------
typedef struct
{
	unsigned char			STX;		 	// �۽� Data�� ����. 0 x02
	unsigned char			CMD;

	unsigned char           OHTNAME_1[6]; 	// ȣ�� No 1
	unsigned char           PASSINFO_1;		// Fiber1 �������
	unsigned char			CID_R_ID_1;		// Fiber1 CID-R ID

	unsigned char           OHTNAME_2[6]; 	// ȣ�� No 2
	unsigned char           PASSINFO_2;		// Fiber2 �������
	unsigned char			CID_R_ID_2;		// Fiber2 CID-R ID

	unsigned char           OHTNAME_3[6]; 	// ȣ�� No 3
	unsigned char           PASSINFO_3;		// Fiber3 �������
	unsigned char			CID_R_ID_3;		// Fiber3 CID-R ID

	unsigned char           OHTNAME_4[6]; 	// ȣ�� No 4
	unsigned char           PASSINFO_4;		// Fiber4 �������
	unsigned char			CID_R_ID_4;		// Fiber4 CID-R ID

	unsigned char			SUM[2];
	unsigned char			ETX;			// �۽� Data�� ������. 0 x03
} LIFTER_TO_OPC_PKT;

#define LIFTER_TO_OPC_PKT_CHECKSUM_LENGTH		34	// üũ�� 34����Ʈ

#define VHL_LIFTER_TO_CID_R_CMD	0x74;	// "t" �� �ش�. ��ɾ� �����

#define VHL_LIFTER_TO_CID_R_RIGHT_PASS_OK		0x52	// "R" ����� ��� OK
#define VHL_LIFTER_TO_CID_R_LEFT_PASS_OK        0x4c	// "L" �¹��� ��� OK
#define VHL_LIFTER_TO_CID_R_RIGHT_LOWSPEED_OK	0x4d	// "M" ����� ���� ��� OK
#define VHL_LIFTER_TO_CID_R_LEFT_LOWSPEED_OK 	0x48	// "H" �¹��� ���� ��� OK

#define VHL_LIFTER_TO_CID_R_RIGHT_ONLY_OK 		0x72	// "r" ����� ��� ����
#define VHL_LIFTER_TO_CID_R_LEFT_ONLY_OK 		0x6c	// "i" �¹��� ��� ����
#define VHL_LIFTER_TO_CID_R_ALL_PASS_OK   		0x41	// "A" �������� ���� ����
#define VHL_LIFTER_TO_CID_R_ALL_LOWSPEED_OK 	0x44	// "D" ������� ������� ����
#define VHL_LIFTER_TO_CID_R_ALL_PASS_OK_P  		0x50	// "P" Pass through lifter //AOHC-217 VHLLifter �ӵ� ���� add by jhoun.lim

#define VHL_LIFTER_TO_CID_R_PASS_NG 			0x53	// "S" ����Ұ���
#define VHL_LIFTER_TO_CID_R_PASS_OK 			0x4e	// "N" ������ ȣ���� ������ ������ ��ȿ�� ������
#define VHL_LIFTER_TO_CID_R_LIFTER_OFF_STATE 	0x40	// "@" ����Ʈ �⵿ Off���� (�ڵ����� �ƴ�)

/*  (���� : ������ ��缭 p17)
CMD : CMD(Command)�� ��t��(Ascii Code 74 H)�� �ȴ�
.
ȣ�� No. : VHL ȣ��� Ascii Code�� ����
              ȣ��� ǥ�� ��� : ��000000��(Ascii Code 30H 30H 30H 30H 30H 30H)
              ȣ����� CID-R�κ��� ���� ȣ����� �Է��Ͽ� �۽��Ѵ�.

������� : CID�� ���� VHL ������� ���� Ascii Code�� ����
			   ��R����Ascii Code 52H�� ... ����� ��� OK
			   ��L����Ascii Code 4CH�� ... �¹��� ��� OK
			   ��M����Ascii Code 4DH�� ... ����� ���� ��� OK. VHL�� �������� �����Ű���� �ϴ� ��쿡 �۽��Ѵ�.
			   ��H����Ascii Code 48H�� ... �¹��� ���� ��� OK. VHL�� �������� �����Ű���� �ϴ� ��쿡 �۽��Ѵ�.
			   ��r����Ascii Code 72H��  ... ����� ��� ����(�¹����� ��� NG�̳�, ������̶�� ��� ����)
													 VHL�� ��r���� �����ϸ� VHL Lifter�� ����� ��� �䱸�Ѵ�.(��R���� VHL Lifter�� �۽��Ѵ�.)
			   ��l����Ascii Code 6CH��  ... �¹��� ��� ����(������� ��� NG�̳�, �¹����̶�� ��� ����)
													 VHL�� ��l���� �����ϸ� VHL Lifter�� �¹��� ��� �䱸�Ѵ�.(��L���� VHL Lifter�� �۽��Ѵ�.)
			   ��A����Ascii Code 41H�� ... ���� ���� ���� ��� OK(���� ���� ���� ��� OK)
			   ��D����Ascii Code 44H�� ... ���� ���� ���� ���� ��� OK(���� ���� ���� ���� ��� OK) VHL�� �������� �����
												 Ű���� �ϴ� ��쿡 �۽��Ѵ�.
			   ��S����Ascii Code 53H�� ... ��� NG(VHL�� ������� �۽�)
			   ��N����Ascii Code 4EH�� ... ��ȿ�� Data. VHL�� ����ϰ� ���� �ʴ� ��쿡 ��N���� �۽�
               ��@�� ��Ascii Code 40H�� ... VHL Lifter�� �⵿ OFF ��(�ڵ� ���� ���°� �ƴ�). VHL�� ��� �Ҵ�.
               �� Up&Down Type�� ��� VHL �� VHL Lifter���� ��� �� ��/�� ���ÿ� ���� ���� ������ Rail�ε� �̵��� �����ϴ�.
                   �� ��� Rail���� VHL Lifter�� ���� ��� Rail�� �̵� or �Ϻ� Rail���� VHL Lifter�� ���� �Ϻ� Rail�� �̵�

CID-R ID : ������ VHL�� CID�� �������� ��Ű���, ����ġ�� ��� �Ұ��� Ascii Code�� ����
              CID-R ID�� ���� Ascii Code ���� ��A������ ��Z�� �� ��a������ ��z�������� �������� ���� ���� ����

���VHL�� ��� �Ҵ��� CID-R������, ȣ�� No., Priority, ��/�켱���� ���"0"(Ascii Code 30 H)�� �ȴ�.
*/
// ----------------------------------------------------------------


// ----- CID-R => VHL �����ͷ� ������ ��Ŷ �� ���� -------
//	������ ��缭 p.18, 19
typedef struct
{
	unsigned char           OHTNAME_2[6]; 	// ȣ�� No
	unsigned char			Priority[2]; 	// �ݼ۹��� �켱��. Ascii Code�� ����
	unsigned char			Turn;		 	// ��/�¼���
	unsigned char			LightGuideID; 	// CID-R ID
}VHL_INFOMATION;

typedef struct
{
	unsigned char			STX;		 	// �۽� Data�� ����. 0 x02
	unsigned char			CMD;

	VHL_INFOMATION				FIBER_INFO_1;
	VHL_INFOMATION				FIBER_INFO_2;
	VHL_INFOMATION				FIBER_INFO_3;
	VHL_INFOMATION				FIBER_INFO_4;

	unsigned char			CID_R_STATUS;	// CID-R ����
	unsigned char			SUM[2];
	unsigned char			ETX;			// �۽� Data�� ������. 0 x03
} OPC_TO_LIFTER_PKT;

#define OPC_TO_LIFTER_PKT_CHECKSUM_LENGTH		43	// üũ�� 43����Ʈ

/*
#define CID_R_STATUS_OK				0x4f	// 'O', ����
#define CID_R_STATUS_PARITY_ERR 	0x50	// 'P', �и�Ƽ ����
#define CID_R_STATUS_SUM_ERR		0x53	// 'S'  Sum����
#define CID_R_STATUS_OTHER_ERR		0x45	// 'E' �׿��� ����
	CID-R Status : CID-R �̻����� ����(����/�̻�) Ascii Code�� ����
                   ?ASCII Code �� ���� �� �׿� ���� �ǹ̴� �ϱ�� ����.
                   ����O����Ascii code 4FH�� ������
                       ? ZCU�� ���� �� ��� DATA�� ���������� ���ŵ� ��� ��O��
                   ����P����Ascii code 50H�� ��Parity Error
                       ? ZCU�� ���� �� ��� DATA �� Parity���� Ʋ���� ���ŵ� ��� ��P��
                   ����S����Ascii code 53H�� ��SUM Check Error
                       ? ZCU�� ���� �� ��� DATA �� SUM Chech���� Ʋ���� ���ŵ� ��� ��S��
                   ����E����Ascii code 45H�� ���� �� Error
					  ? ZCU�� �� ��� �� �� ���� ������ �߻��� ��� ��E��
*/
// ----------------------------------------------------------------

// �ø���� �������� ����� ���³��� ���� ���� (CID�� ���� ���� ����)
#define OPC_SERIAL_SEND_OK		0
#define OPC_SERIAL_SEND_ERR		-1

#define OPC_SERIAL_RCV_OK		0
#define OPC_SERIAL_RCV_NO_DATA	-1
#define OPC_SERIAL_RCV_ERROR	-2

#endif