/**
@file    Def_Protocol.h
@version OHT 7.0
@brief   Protocol�� ���Ǵ� ���� ���, ���� ���ǵ� Header File
@note    Tag �з� ü��� �������� ������ ����.
*/

#ifndef Def_ProtocolH
#define Def_ProtocolH

#include "Def_OHT.h"

#pragma pack(push, 1) // 1byte�� Pack

// ----------------------------------------------------------------------------------------
// Communication Define
// ----------------------------------------------------------------------------------------
// -------------------------------------------
// TCP/IP Port ���� ����
// -------------------------------------------
#define PORT_OHT_OCS            5001    ///< Port ���� : OHT - OCS
#define PORT_OHT_OCSSUB         5011    ///< Port ���� : OHT - OCS Sub
#define PORT_OHT_TP             5002    ///< Port ���� : OHT - TP
#define PORT_OHT_MONITOR        5004    ///< Port ���� : OHT - monitor(�ڰ�����)
#define PORT_TP_ASSISTANT       5100    ///< Port ���� : TP - Assistant
#define PORT_TP_AUTOTECHING     1234	///< Port ���� : TP - Auto Teaching
#define PORT_ASSISTANT_DM       9000    ///< Port ���� : Assistant - Data Manager

// -------------------------------------------
// Tag ���� Macro �Լ�
// -------------------------------------------
/// Sender�� �����ϴ� ��ũ�� �Լ�
#define GET_SENDER(i)           ((i & MASK_TAG_SENDER) >> 28)
/// Sender�� �����ϴ� ��ũ�� �Լ�
#define GET_RECEIVER(i)         ((i & MASK_TAG_RECEIVER) >> 24)
/// Tag Type�� �����ϴ� ��ũ�� �Լ�
#define GET_TAGTYPE(i)          ((i & MASK_TAG_TYPE) >> 16)
/// Command Type�� �����ϴ� ��ũ�� �Լ�
#define GET_CMDTYPE(i)          ((i & MASK_TAG_CMDTYPE) >> 12)

#define NORMAL_TYPE 	0x01
#define FORCE_TYPE 		0x02
#define JOBCHAGE_RETURN_TYPE 	0x03

// -------------------------------------------
// Tag �˻��� ���� Bit Mask
// -------------------------------------------
#define MASK_TAG_SENDER             0xF0000000 ///< Sender ������ ���� Bit Mask
#define MASK_TAG_RECEIVER           0x0F000000 ///< Receiver ������ ���� Bit Mask
#define MASK_TAG_TYPE               0x00FF0000 ///< Tag Type ������ ���� Bit Mask
#define MASK_TAG_CMDTYPE            0x0000F000 ///< Command Type ������ ���� Bit Mask

// -------------------------------------------
// Tag �з� ����
// -------------------------------------------
// Sender ����
#define TAG_SENDER_OCS              0x3 ///< Tag Sender : OCS
#define TAG_SENDER_ASSISTANT		0x4	///< Tag Sender : Assistant
#define TAG_SENDER_TP               0x5 ///< Tag Sender : TP
#define TAG_SENDER_DM				0x6	///< Tag Sender : Data Manager
#define TAG_SENDER_MONITOR          0x7 ///< Tag Sender : �ڰ�����
#define TAG_SENDER_OHT              0x9 ///< Tag Sender : OHT

// Receiver ����
#define TAG_RECEIVER_OCS            0x3 ///< Tag Receiver : OCS
#define TAG_RECEIVER_ASSISTANT      0x4 ///< Tag Receiver : Assistant
#define TAG_RECEIVER_TP             0x5 ///< Tag Receiver : TP
#define TAG_RECEIVER_DM				0x6	///< Tag Sender : Data Manager
#define TAG_RECEIVER_MONITOR        0x7 ///< Tag Receiver : �ڰ�����
#define TAG_RECEIVER_OHT            0x9 ///< Tag Receiver : OHT

// Protocol type ����
#define TAG_PROTOTYPE_REQ           0x1 ///< Protocol Type : ���, ��û Tag
#define TAG_PROTOTYPE_ACK           0xA ///< Protocol Type : ���� Tag

// Cmd Type ����
#define TAG_CMDTYPE_DRVING          0x1 ///< Command Type : ���� ���� Tag
#define TAG_CMDTYPE_DRVING_ETC      0x2 ///< Command Type : ���� ��Ÿ ���� Tag
#define TAG_CMDTYPE_TRANS           0x3 ///< Command Type : ������ ���� Tag
#define TAG_CMDTYPE_OCSCMD          0x4 ///< Command Type : OCS Common Command ���� Tag
#define TAG_CMDTYPE_TPCMD           0x5 ///< Command Type : TP Common Command ���� Tag
#define TAG_CMDTYPE_STATUS          0xA ///< Command Type : Status Tag
#define TAG_CMDTYPE_DATA			0xB	///< Command Type : Transfer Data(Teaching, Station, IO Map ... Etc)
#define TAG_CMDTYPE_INIT            0xC ///< Command Type : �ʱ�ȭ ���� ���� Tag
#define TAG_CMDTYPE_DM_CMD			0xD	///< Command Type : OCS Remote ���� Tag
#define TAG_CMDTYPE_QUICKCMD        0xE ///< Command Type : Quick Command ���� Tag

// -------------------------------------------
// ���� ��� Command Data
// -------------------------------------------
// ���� Command
#define TAG_CMDDATA_GO              0x1000 ///< Command Data : Go ���

// ���� Etc Command
#define TAG_CMDDATA_GO_MTL          0x2000 ///< Command Data : Go ���(MTL)
#define TAG_CMDDATA_GO_MAPMAKE      0x2010 ///< Command Data : Map Maker ���
#define TAG_CMDDATA_GO_CLEAN        0x2020 ///< Command Data : Clean ���(Service OHT)
#define TAG_CMDDATA_GO_CLEANCANCEL  0x2021 ///< Command Data : Clean ��� ���(Service OHT)

// ������ ���� Command
#define TAG_CMDDATA_TR_LOAD         0x3000 ///< Command Data : Load ���
#define TAG_CMDDATA_TR_UNLOAD       0x3010 ///< Command Data : Unload ���

#define TAG_CMDDATA_TR_SCAN         0x3020 ///< Command Data : Scan ��
#define TAG_CMDDATA_TR_HOME_AUTOSET 0x3030 ///< Command Data : Hoist hoem auto set ���

// �ʱ�ȭ �۾� Command
#define TAG_CMDDATA_INIT_INFO_OCS   0xC000 ///< Command Data : OHT �ʱ� ���� ��û(For OCS)
#define TAG_CMDDATA_INIT_INFO_TP    0xC104 ///< Command Data : OHT �ʱ� ���� ��û(For TP)

//2013.08.20 doori.shin IO Map Tag
#define TAG_CMDDATA_DATA_IOMAP_TP   0xB110 ///< Command Data : Send IO Map Field to TP
#define TAG_CMDDATA_DATA_NOT_READY	0xB120 ///< OHT doesn't ready for sending

// Quick Command ����
#define TAG_CMDDATA_CTRL_CANCEL     0x4020 ///< Command Data : Cancel ���
#define TAG_CMDDATA_CTRL_IDRESET    0x4030 ///< Command Data : ID Reset ���
#define TAG_CMDDATA_CTRL_PAUSE      0x4040 ///< Command Data : Pause ���
#define TAG_CMDDATA_CTRL_RESUME     0x4050 ///< Command Data : Resume ���
#define TAG_CMDDATA_CTRL_ESTOP      0x4060 ///< Command Data : E-Stop ���
#define TAG_CMDDATA_CTRL_TURN_OFF   0x4070 ///< Command Data : �⵿ Off ���

// Status ����
#define TAG_CMDDATA_STATUS_OCS      0xA000 ///< Status : OHT->OCS
#define TAG_CMDDATA_STATUS_TP       0xA104 ///< Status : OHT->TP

//2013.08.20 doori.shin IO Map Tag
#define TAG_CMDDATA_DATA_IOMAP_TP   0xB110 ///< Command Data : Send IO Map Field to TP
#define TAG_CMDDATA_DATA_NOT_READY	0xB120 ///< OHT doesn't ready for sending
//2013.12.19 doori.shin Additional Status
#define TAG_CMDDATA_DATA_OHT_STATUS		0xB701
//---------------------------------------------------------------------------

// Data ���� 
#define TAG_CMDDATA_VERSIONINFO             0xD000 ///< Command Data : Version ����
#define TAG_CMDDATA_DATA_DOWNLOAD           0xD100 ///< Command Data : Data Download ���
#define TAG_CMDDATA_DATA_DOWNLOAD_MAP       0xD101 ///< Command Data : Map Data ����
#define TAG_CMDDATA_DATA_DOWNLOAD_STATION   0xD102 ///< Command Data : Station Data ����
#define TAG_CMDDATA_DATA_DOWNLOAD_TEACHING  0xD103 ///< Command Data : Teaching Data ����
#define TAG_CMDDATA_DATA_BAKCUP             0xD200 ///< Command Data : Data Backup ���
#define TAG_CMDDATA_DATA_BAKCUP_MAP         0xD201 ///< Command Data : Map Data Backup
#define TAG_CMDDATA_DATA_BAKCUP_STATION     0xD202 ///< Command Data : Station Backup
#define TAG_CMDDATA_DATA_BAKCUP_TEACHING    0xD203 ///< Command Data : Teaching Backup
#define TAG_CMDDATA_DATA_UPLOAD             0xD300 ///< Command Data : Data Upload ���
#define TAG_CMDDATA_DATA_UPLOAD_MAP         0xD301 ///< Command Data : Map Data ����
#define TAG_CMDDATA_DATA_UPLOAD_STATION     0xD302 ///< Command Data : Station Data ����
#define TAG_CMDDATA_DATA_UPLOAD_TEACHING    0xD303 ///< Command Data : Teaching Data ����

//----------------------------------------------------------------------------
//2013.07.15 doori.shin v7.0 ��� �߰�
#define REQ_DATA_VERSIONINFO_DM_RMT     0xD000
#define ACK_DATA_VERSIONINFO_RMT_DM     0xD000
#define REQ_DATA_DOWNLOADINFO_DM_RMT    0xD100
#define DOWNLOAD_STATIONDATA_RMT_DM     0xD110
//#define RESULT_STATIONDATA_DM_RMT       0xF022
#define DOWNLOAD_TEACHINGDATA_RMT_DM    0xD120
//#define RESULT_REACHINGDATA_DM_RMT      0xF024
//#define REQ_DATA_UPLOADINFO_DM_RMT      0xF030
#define UPLOAD_STATIONDATA_DM_RMT       0xD210
#define RESULT_STATIONDATA_RMT_DM       0xD210
#define UPLOAD_TEACHINGDATA_DM_RMT      0xD220
#define RESULT_TEACHINGDATA_RMT_DM      0xD220

//2014.06.12 doori.shin ���ܸ���͸� ��ſ� �±� �߰�
#define REQ_OHT_OPERATION_STATUS		0xB000
#define SND_OHT_OPERATION_STATUS		0xB000
#define REQ_OHT_MOTION_DATA				0xB010
#define SND_OHT_MOTION_DATA				0xB010
#define CMD_RESET_MOVEMENT_VALUE		0xB020
#define ACK_RESET_MOVEMENT_VALUE		0xB020
#define REQ_OHT_HID_DATA				0xB030
#define SND_OHT_HID_DATA				0xB030

// -------------------------------------------
// Tag ���� ����ü
// -------------------------------------------
/**
@brief   Tag ���� �и��� ���� ����ü.

         ��Ʈ �ʵ带 �̿��Ͽ� ���� �и���.
@author  zzang9un
@date    2012.11.14
@warning Little Endian�̹Ƿ� ���� ������ ������Ʈ(0)���� ������.
*/
typedef struct TAG_INFO_
{
    UINT CmdData    : 16;   ///< Command Data
    UINT CmdType    : 4;    ///< Command Type
    UINT ProtoType  : 4;    ///< Protocol Type
    UINT Receiver   : 4;    ///< Receiver
    UINT Sender     : 4;    ///< Sender
} TAG_INFO;

/**
@brief   Tag ���� �и��� ���� ����ü.

         Union���·� Tag ������ �ٷ� ������ �� �ֵ��� ������.
@author  zzang9un
@date    2012.11.14
*/
typedef union uTAG_DATA_
{
    TAG_INFO    Info; ///< Tag Info ����ü
    UINT        Data; ///< Tag�� ������ ����
} uTAG_DATA;
//---------------------------------------------------------------------------

// -------------------------------------------
// Ack Error ����
// -------------------------------------------
#define ACK_ERR_BUSY            0x01 ///< Busy : �ٸ� �۾� ������
#define ACK_ERR_STATE           0x02 ///< Error State : ���� ����
#define ACK_ERR_PROTOCOL        0x03 ///< Protocol Error : ������ �̻�
#define ACK_ERR_DATALOGIC       0x04 ///< Data Logic Error : �Ұ��� �۾�, �ǹ̾��� �۾�

/**
@brief   Parsing �� ACK�� TYPE�� ���� enum ����
@author  zzang9un
@date    2012.11.21
*/
typedef enum ACK_TYPE_
{
    ACK_TYPE_CMD_ERR_OHT2OCS    = -2,   ///< Command�� ������ �߻��� ���
    ACK_TYPE_CMD_ERR_OHT2TP     = -1,   ///< Command�� ������ �߻��� ���
    ACK_TYPE_NONE               = 0,    ///< None
    ACK_TYPE_STATUS_OHT2OCS     = 1,    ///< OCS���� Status�� ������ �ϴ� ���
    ACK_TYPE_STATUS_OHT2TP,             ///< TP���� Status�� ������ �ϴ� ���
    ACK_TYPE_CMD_OHT2OCS,               ///< OCS���� Command�� ���� ACK�� ������ �ϴ� ���
    ACK_TYPE_CMD_OHT2TP,                ///< TP���� Command�� ���� ACK�� ������ �ϴ� ���
	ACK_TYPE_OHTINIT_OHT2TP,            ///< TP���� OHT Info�� ������ �ϴ� ���
	//2013.07.16 doori.shin �߰�
	ACK_TYPE_UPLOAD_DM2RMT,             ///< OCS Remote���� �����Ϳ�û �ϴ� ���
	ACK_TYPE_DOWNLOAD_RMT2DM,           ///< DataManager���� ACK�� ������ �ϴ� ���

	//2013.08.22 doori.shin �߰�
	ACK_TYPE_DATA_OHT2TP
} ACK_TYPE;


/**
@brief   �۽��ڿ��� �����ϱ� ���� Ack ������ ���� ����ü
@author  zzang9un
@date    2013.01.24
*/
typedef struct ACK_DATA_
{
    uTAG_DATA AckTagData;   ///< ���� Tag
    BYTE RecvCmdID;         ///< ������ Command ID
    ACK_TYPE ACKType;       ///< ACK �� Data�� Type
    BYTE ACKResult;         ///< ACK ���
    UINT ACKErrorCode;      ///< ACK Error Code
} ACK_DATA;
//---------------------------------------------------------------------------

#pragma pack(pop) // Pack ����


#endif // Def_ProtocolH
