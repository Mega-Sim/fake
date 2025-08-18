/**
@file    Def_DataType.h
@version OHT 7.0
@brief   OHT�� ���� Data Type(�ڷ���)�� ���� ����
*/

#ifndef Def_DataTypeH
#define Def_DataTypeH

#include "Def_OHT.h"
#include "Def_Protocol.h"
//#include "Protocol_Define_Modify.h"

#include <Classes.hpp>
#include <dos.h>

#pragma pack(push, 1) // 1byte�� Pack
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OCS Command ���� ����ü ����
//---------------------------------------------------------------------------

// -------------------------------------------
// ���� ���� Command
// -------------------------------------------
/**
@brief   Go ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.04.01
@note    �������� ������ ���� Ȯ���� ��.
*/
typedef struct GO_CMD_
{
    // ����
    BYTE CmdID;             ///< Command ID
    UINT StopNodeID;        ///< Target ���� Node ID
    
    // ������� tag�� Go ��� ��
    UINT StopStationID;     ///< Target ������ Tag ID
    int  StopStationOffset; ///< Node�κ��� ������ Tag���� �Ÿ�(mm)
    BYTE StopStationType;   ///< Target ������ Tag Type
    UINT NextNodeID;        ///< Next Target ���� Node ID(Trans Node ���� �� �ʿ��� ����)

    // Speed, Accel Level
    BYTE LineSpeedLevel;    ///< Line Speed Level(50~100%)
    BYTE CurveSpeedLevel;   ///< Curve Speed Level(50~100%)
    BYTE AccelationLevel;   ///< Accelation Level(50~100%)

    // Pre Trans
    BYTE FinalPortType;     ///< �۾� ����(EQ, STB, UTB, Loader ��)
    int  TPosition_Shift;   ///< Shift Teaching��(mm)

	int Tposition_Rotate;

    // by zzang9un 2013.04.01 : �������� ���� ���� �߰�(v1.5)    
    UINT PreSteeringNodeID; ///< �������� ���� Node ID

	BYTE Buffer_Type;
} GO_CMD;

/**
@brief   MTL Insert ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_MTL_INSERT_
{
    BYTE CmdID;             ///< Command ID
    UINT NodeID;            ///< Node ID
    int  Reserved1;         ///< Reserved 1
} CMD_MTL_INSERT;

/**
@brief   Map Make ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_MAP_MAKE_
{
    BYTE CmdID;             ///< Command ID
    UINT EndNodeID;         ///< Map Make �Ϸ� ��ġ
    int  Speed;             ///< Map Make Speed(mm/s)
} CMD_MAP_MAKE;

/**
@brief   Clean ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.03.12
@note    Clean ��� ��ҽ� CmdID�� ��ȿ�ϸ� ������ ���� 0���� �����ؾ� ��.
*/
typedef struct CMD_CLEAN_
{
    BYTE CmdID;             ///< Command ID
    UINT EndNodeID;         ///< Clean �Ϸ� ��ġ
    int  Speed;             ///< Clean Speed(mm/s)
} CMD_CLEAN;

/**
@brief   ��Ÿ Go ���� ����ü
@author  zzang9un
@date    2013.03.18
*/
typedef struct GO_CMD_ETC_
{
    BYTE CmdID;             ///< Command ID
    UINT Data1;             ///< Data1
    int  Data2;             ///< Data2
} GO_ETC_CMD;

/**
@brief   ��Ÿ Go ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.03.18
@note    MTL, Map Make, Clean ��ɿ� ����Ѵ�.
*/
typedef union uGO_ETC_CMD_
{
    CMD_MTL_INSERT  MTLCmd;         ///< MTL Insert
    CMD_MAP_MAKE    MapMakeCmd;     ///< Map Make
    CMD_CLEAN       CleanCmd;       ///< Clean

    GO_ETC_CMD      GoEtcCmd;       ///< Go Etc Cmd  Data
} uGO_ETC_CMD;

/**
@brief   Node Info ���� Command(OCS->OHT) ����ü.
@author  zzang9un
@date    2013.03.12
*/
typedef struct CMD_NODEINFO_
{
    BYTE CmdID;             ///< Command ID
    UINT TargetNodeID;      ///< ���������� Node ID
    int  NodeCount;         ///< Node ����
    UINT *NodeID;           ///< ���� Node ID #N
} CMD_NODEINFO;

// -------------------------------------------
// ������ ���� Command
// -------------------------------------------

/**
@brief   ������ Option ����ü
@author  zzang9un
@date    2013.07.18
*/
typedef struct TRANS_OPTION_
{
    BYTE Reserved2      : 1;    ///<    (�̻��)
	BYTE Reserved1      : 1;    ///<   (�̻��)
    BYTE EQPIODisable_TP: 1;    ///< TP���� EQPIO�� ���� �ɼ�
    BYTE HandDetectEQ   : 1;    ///< Hand Detect EQ ����
    BYTE Oscillation    : 1;    ///< Oscillation On/Off  (�̻��)
	BYTE TransSound     : 1;    ///< ������ �Ҹ� On/Off
	BYTE RFReader       : 1;    ///< RF Read ����      (�̻��)
    BYTE AutoRecovery   : 1;    ///< Auto Recovery ����
} TRANS_OPTION;

/**
@brief   ������ Option ���Ͽ�
@author  zzang9un
@date    2013.07.18
*/
typedef union uTRANS_OPTION_
{
    TRANS_OPTION Info;   ///< ��Ÿ ������ �ɼ�
    BYTE         Data;   ///< Data
} uTRANS_OPTION;

/**
@brief   Trans ���� Command(OCS->OHT) ����ü.

         ���� ����� �� ����ü�� �����.
          - Load, Unload ���
          - ��ɿ� ���� ���� ��ȿ�ϰų� ��ȿ���� ���� �� ����.
@author  zzang9un
@date    2013.09.06
*/
typedef struct TRANS_CMD_
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
	uTRANS_OPTION TransOption;  ///< ������ �ɼ� ���ð�
	BYTE BufferType;			///< ������� Buffer ��ġ (�̻��)
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS ��ȣ ����
	BYTE PIOChannel;		  	///< RF PIO PIOChannel (�̻���  0 )
} TRANS_CMD;

// -------------------------------------------
// �Ϲ� Command
// -------------------------------------------
/**
@brief   OCS ���� ���(OCS Common Command)�� Decode�ϱ� ���� ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct OCS_CMD_COMMON_
{
    BYTE cData; ///< cData
    BYTE Data1; ///< Data1
    BYTE Data2; ///< Data2
} OCS_CMD_COMMON;

/**
@brief   Cancel Command(OCS->OHT) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_CANCEL_OCS_OHT_
{
    BYTE CmdID;         ///< Command ID
    BYTE Cancel_CmdID;  ///< Cancel �� Command ID
    BYTE CancelType;    ///< Cancel Type
} CMD_CTRL_CANCEL_OCS_OHT;

/**
@brief   ID Reset Command(OCS->OHT) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_IDRESET_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< �̻��
    BYTE Reserved2; ///< �̻��
} CMD_CTRL_IDRESET_OCS_OHT;

/**
@brief   Pause Command(OCS->OHT) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_PAUSE_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< �̻��
    BYTE Reserved2; ///< �̻��
} CMD_CTRL_PAUSE_OCS_OHT;

/**
@brief   Resume Command(OCS->OHT) ����ü
@author  zzang9un
@date    2012.11.07
*/
typedef struct CMD_CTRL_RESUME_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< �̻��
    BYTE Reserved2; ///< �̻��
} CMD_CTRL_RESUME_OCS_OHT;

/**
@brief   Resume Command(OCS->OHT) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_CTRL_ESTOP_OCS_OHT_
{
    BYTE EStopType; ///< OCS E-Stop ��� Type
    BYTE Reserved1; ///< �̻��
    BYTE Reserved2; ///< �̻��
} CMD_CTRL_ESTOP_OCS_OHT;

/**
@brief   OCS Command(OCS->OHT) ����ü.

         �� ����ü�� �������� ����ϵ��� Union���� ������.
         ���� ����� �� ����ü�� �����.
          - E-Stop, Cancel, ID Reset, Pause, Resume ���
@author  zzang9un
@date    2013.03.13
*/
typedef union uOCS_CMD_
{
    CMD_CTRL_CANCEL_OCS_OHT     Cancel;     ///< Cancel Command
    CMD_CTRL_IDRESET_OCS_OHT    IDReset;    ///< ID Reset Command
    CMD_CTRL_PAUSE_OCS_OHT      Pause;      ///< Pause Command
    CMD_CTRL_RESUME_OCS_OHT     Resume;     ///< Resume Command
    CMD_CTRL_ESTOP_OCS_OHT      EStop;      ///< E-Stop Command

    OCS_CMD_COMMON              OCSCmd;     ///< Decode�� ���� ����� ����ü
} uOCS_CMD;

// -------------------------------------------
// OHT �ʱ� ���� ���� 
// -------------------------------------------
/**
@brief   OHT �ʱ� ���� ���� Command(OHT->OCS) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct CMD_INIT_INFO_OCS_OHT_
{
    BYTE CmdID;     ///< Command ID
    BYTE Reserved1; ///< �̻��
    BYTE Reserved2; ///< �̻��    
} CMD_INIT_INFO_OCS_OHT;

// -------------------------------------------
// OHT Data Update ��û ����
// -------------------------------------------
/**
@brief   Map ���� Update Request(OHT->OCS) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct REQ_UPDATE_MAPINFO_OHT_OCS_
{
    BYTE CmdID;         ///< Command ID
    UINT NodeID;        ///< Update�� ���� Node ID
    UINT StationID;     ///< Update�� ������ Station ID
    int  StationOffset; ///< NodeID�κ����� Offset
    BYTE StationType;   ///< Station Type
    UINT NextNodeID;    ///< Station ���� ���� NodeID
} REQ_UPDATE_MAPINFO_OHT_OCS;

/**
@brief   Teaching ���� Update Request(OHT->OCS) ����ü
@author  zzang9un
@date    2013.03.13
*/
typedef struct REQ_UPDATE_TEACHINGINFO_OHT_OCS_
{
    BYTE CmdID;             ///< Command ID
    UINT NodeID;            ///< Update�� ���� Node ID
    UINT CurStationID;      ///< Update�� ������ Station ID
    BYTE PortType;          ///< �۾� ����
    BYTE PIODirection;      ///< PIO ����
    int  TPosition_Hoist;   ///< Hoist Teaching��
    int  TPosition_Shift;   ///< Hoist Teaching��
    int  TPosition_Rotate;  ///< Hoist Teaching��
} REQ_UPDATE_TEACHINGINFO_OHT_OCS;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TP Command ���� ����ü ����
//---------------------------------------------------------------------------
/**
@brief   �Ϲ����� TP ���(Common Command)�� Decode�ϱ� ���� ����ü
@author  zzang9un
@date    2012.11.19
*/
typedef struct TP_CMD_COMMON_
{
    BYTE CmdID;     ///< Command ID
    int  Data1;     ///< Data 1
    int  Data2;     ///< Data 2
    int  Data3;     ///< Data 3 
    int  Data4;     ///< Data 4
} TP_CMD_COMMON;

/**
@brief   TP Jog(Driving) ����� Decode�ϱ� ���� ����ü
@author  zzang9un
@date    2013.02.19
*/
typedef struct CMD_TP_JOG_DIST_
{
    BYTE CmdID;     ///< Command ID
    int  Accel;     ///< ���ӵ�(mm/s^2)
    int  Decel;     ///< ���ӵ�(mm/s^2)
    int  Velocity;  ///< �ӵ�(mm/s)
    int  Distance;  ///< �Ÿ�(mm)
} CMD_TP_JOG_DIST;

/**
@brief   TP Jog(Hoist, Shift) ����� Decode�ϱ� ���� ����ü
@author  zzang9un
@date    2013.02.19
*/
typedef struct CMD_TP_JOG_TR_
{
    BYTE CmdID;     ///< Command ID
    int  AccelTime; ///< ���ӽð�(ms)
    int  DecelTime; ///< ���ӽð�(ms)
    int  Velocity;  ///< �ӵ�(mm/s)
    int  Distance;  ///< �Ÿ�(mm)
} CMD_TP_JOG_TR;

typedef struct CMD_TP_JOG_DRV_
{
    BYTE CmdID;     ///< Command ID
    int  Accel;     ///< ���ӵ�(mm/s^2)
    int  Decel;     ///< ���ӵ�(mm/s^2)
    int  Velocity;  ///< �ӵ�(mm/s)
    int  bJogInterlockIgnore;  ///< ���� ����
} CMD_TP_JOG_DRV;

/**
@brief   OHT Init ������ �ֱ� ���� ����ü
@author  zzang9un
@date    2012.11.27
*/
typedef struct OHT_INFO_
{
    BYTE cData;     ///< Default : 0
    int  OHT_ID;    ///< Data 1
    int  LineInfo;  ///< Data 2
    int  OHT_Ver;   ///< Data 3 
    int  OHT_Type;  ///< Data 4
} OHT_INFO;

/**
@brief   Teaching ���� �� �Ϻθ� Packing�� ����ü
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_ETC_
{
    UINT PIODirection   : 8; ///< PIO ����(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
    UINT LookDownLevel  : 8; ///< LookDownLevel
    UINT Reserved2      : 1; ///< 
    UINT Reserved1      : 1; ///< 
    UINT EQPIODisable_TP: 1; ///< EQPIO Disable
    UINT HandDetectEQ   : 1; ///< Hand Detect EQ ����
    UINT Oscillation    : 1; ///< Oscillation On/Off
    UINT TransSound     : 1; ///< ������ �Ҹ� On/Off
    UINT RFReader       : 1; ///< RF Read ����
    UINT AutoRecovery   : 1; ///< Auto Recovery ����
} TEACHING_INFO_ETC;

/**
@brief   TEACHING_INFO_ETC ����ü�� ���� Dummy ����ü
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_ETC_DUMMY_
{
    UINT PIODirection   : 8; ///< PIO ����(Left, Right)
    UINT PIOTimeLevel   : 8; ///< PIO Time Level
    UINT LookDownLevel  : 8; ///< LookDownLevel
    UINT TransOption    : 8; ///< Trans Option
} TEACHING_INFO_ETC_DUMMY;

/**
@brief   Teaching ���� �� �Ϻθ� Packing�� union ����ü
@author  zzang9un
@date    2013.07.19
*/
typedef union uTEACHING_INFO_ETC_
{
    TEACHING_INFO_ETC Info;
    TEACHING_INFO_ETC_DUMMY Data;
} uTEACHING_INFO_ETC;

/**
@brief   OHT Teaching ������ TP�� ������ ���� ����ü
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_INFO_
{
    BYTE PortType;              ///< PortType
    int  TPosition_Hoist;       ///< Teaching �� : Hoist   
    int  TPosition_Shift;       ///< Teaching �� : Shift
    int  TPosition_Rotate;      ///< Teaching �� : Rotate
    uTEACHING_INFO_ETC EtcData;  ///< ��Ÿ Teaching ����
} TEACHING_INFO;

/**
@brief   Teaching ���� ���� ��û�� ���� ����ü
@author  zzang9un
@date    2013.07.19
*/
typedef struct TEACHING_SAVE_
{
    BYTE CmdID;                 ///< Command ID
    int  PortType;              ///< Port Type
    int  Hoist;
	int  Shift;
	int  Rotate;
	BYTE PIOType;
	UINT PIOChannel;
	UINT PIOID;
    uTEACHING_INFO_ETC EtcData;  ///< ��Ÿ Teaching ����
    int  Reserved1;             ///< Reserved1
    int  Reserved2;             ///< Reserved2    
} TEACHING_SAVE;

/**
@brief   Mark ������ ���� ����ü
@author  zzang9un
@date    2013.03.29
*/
typedef struct MAPMAKE_DATAINFO_
{
    BYTE DataType   : 1; ///< Data Type(0:Node, 1:Station)
    BYTE TagType    : 2; ///< Tag Type(0:����, 1:Stop Tag, 2:QR_L, 3:QR_R)
    BYTE Reserved1  : 2; ///< Reserved
    BYTE Reserved2  : 2; ///< Reserved
} MAPMAKE_DATAINFO;

/**
@brief   Map Make ������ ������ ���� ����ü
@author  zzang9un
@date    2013.07.22
@note    Station ID�� StopTagType�� 0�� ��� Offset�� Node�� Node ������ �Ÿ��� �ǹ��Ѵ�.
*/
typedef struct MAPMAKE_INFO_
{
    MAPMAKE_DATAINFO DataInfo;      ///< Map Make ���� ���� flag
    int StartNodeID;    ///< Station Data Load Flag 
    int EndNodeID;      ///< Station Data Load Flag 
    int StationID;      ///< Station ID
    int Offset;         ///< Offset
} MAPMAKE_INFO;

/**
@brief   TP ���� Command(TP->OHT) ����ü.

         �� ����ü�� �������� ����ϵ��� Union���� ������.
         ���� ����� �� ����ü�� �����.
          - TP �Ϲ� Cmd, Jog���� Cmd, OHT Info
@author  zzang9un
@date    2013.07.19
*/
typedef union uTP_CMD_
{
    CMD_TP_JOG_DRV  CmdTP_Jog_DRV;  ///< ���� Jog�� ���� TP Command�� ���
    CMD_TP_JOG_DIST	CmdTP_Jog_DIST; ///< �Ÿ� ���� Jog�� ���� TP Command�� ���
    CMD_TP_JOG_TR   CmdTP_Jog_TR;   ///< ������ Jog�� ���� TP Command�� ���
    OHT_INFO        OHTInfo;        ///< OHT ������ ���� ����ü
    TEACHING_INFO   TeachingInfo;   ///< OHT Teaching �� ������ ���� ����ü
    TEACHING_SAVE   TeachingSave;   ///< Teaching ���� ���� ��û�� ���� ����ü
    MAPMAKE_INFO    MapMakeInfo;    ///< Map Make ���� ������ ���� ����ü
    
    TP_CMD_COMMON   TPCmd;          ///< �Ϲ����� TP Command�� ���
} uTP_CMD;

/**
@brief   OHT Main ���ο��� ����� ó���ϴ� Command Data ����ü.

         OCS, TP�κ��� ������ ��� ����� �� ����ü ���·� ����ȴ�.
@author  zzang9un
@date    2012.11.14
*/
typedef struct CMD_DATA_
{
    uTAG_DATA   uCmdTag;    ///< Command Tag ����

	GO_CMD      GoCmd;      ///< ���� ���� Command
	uGO_ETC_CMD uGoCmdEtc;  ///< ��Ÿ ���� ���� Command
	TRANS_CMD   TransCmd;   ///< ������ ���� Command
	uOCS_CMD    uOCS_CMD;   ///< OCS ���� Command
	uTP_CMD     uTP_CMD;    ///< TP ���� Command 
} CMD_DATA;

//---------------------------------------------------------------------------
// Status ���� ����ü
//---------------------------------------------------------------------------
/**
@brief   2~3��, Hand, Shutter ���� Origin ������ ���� ����ü
@author  zzang9un
@date    2013.03.12
*/
typedef struct ORIGIN_INFO_
{
    WORD Hoist      : 1; ///< Hoist ���� ����
    WORD Shift      : 1; ///< Shift ���� ����
    WORD Rotate     : 1; ///< Rotate ���� ����
    WORD Hand       : 1; ///< Hand ���� ����
    WORD Shutter    : 1; ///< Shutter ���� ����
    WORD Clamp      : 1; ///< Clamp ���� ����
    WORD Lift       : 1; ///< Lift ���� ����
    WORD Sliding    : 1; ///< Sliding ���� ����
    WORD Reserved   : 8; ///< Reserved
} ORIGIN_INFO;

/**
@brief   Mode, Status, CmdID�� �����ϴ� ����ü
@author  zzang9un
@date    2013.03.21
*/
typedef struct MSCMDID_
{
    UINT CurCmdID   : 8; ///< Current Command ID
    UINT PreCmdID   : 8; ///< Previous Command ID
    UINT Status     : 8; ///< OHT ����
    UINT Mode       : 8; ///< OHT ���
} MSCMDID;

/**
@brief   Mode, Status, CmdID�� �����ϴ� ����ü(Union)
@author  zzang9un
@date    2013.03.21
*/
typedef union uMSCMDID_
{
    MSCMDID Info; ///< Status�� Cmd ID�� �����ϴ� ����ü
    UINT    Data; ///< Data
} uMSCMDID;


/**
@brief   OHT �� ��Һ� ��� ���� ������ ������ �ִ� ����ü
@author  puting
@date    2012.11.23
*/
typedef struct USE_INFO_
{
    UINT BCR                    : 1; ///< BCR ��� ����
    UINT PBSSensor              : 1; ///< PBS ��� ����
    UINT SoundAlarm             : 1; ///< Sound ��� ����
    UINT Lamp                   : 1; ///< Lamp ��� ����
    UINT ShutterControl         : 1; ///< Shutter ���� ��� ����
    UINT DirectLogSave          : 1; ///< �α� �ٷ� ���� ����
    UINT EQ_PIO                 : 1; ///< EQ PIO ��� ����
    UINT STBSensor              : 1; ///< STB ���� ��� ����

    UINT HandLimitSensorCheck   : 1; ///< Hand Limit ���� üũ ����
    UINT HandControl            : 1; ///< Hand ���� ��� ����
    UINT SteeringControl        : 1; ///< ���� ���� ��� ����
    UINT UTBSensor              : 1; ///< UTB ���� ��� ����
    UINT LookDownSensor         : 1; ///< Lookdown ���� ��� ����
    UINT HandPushSensorCheck    : 1; ///< Hand Push ���� üũ ����
    UINT GripperSen             : 1; ///< Gripper ���� ��� ����
    UINT HoistAlignSensor       : 1; ///< Hoist ���� ���� ��� ����
    
    UINT OscillationSensor      : 1; ///< Oscillation ���� ��뿩��
    UINT ErrSound               : 1; ///< ���� ����� ��� ����
    UINT DrivingSound           : 1; ///< ���� Sound ��� ����
    UINT TransSound             : 1; ///< ������ Sound ��� ����
    UINT ObsDetectSound         : 1; ///< OBS ���� ���� Sound ��� ����
    UINT IgnorePIOError         : 1; ///< PIO ���� ���� ����
    UINT HoistHomePos_PIOError  : 1; ///< Hoist ���� ��ġ���� PIO ���� �߻� ����
	UINT ObsIgnoreSound         : 1; ///< OBS ���� Sound ��� ����

    UINT LocSave_Trans_ing      : 1; ///< �������� Log ���� ����
    UINT PushAbnormalOnCheck    : 1; ///< PushAbnormalOnCheck
    UINT CrossRail_Interlock    : 1; ///< Crossrail ���Ͷ�
    UINT UTBFoupCheck           : 1; ///< UTB Foup üũ ����
    UINT InnerFoupDetectSen     : 1; ///< Inner Foup ���� ���� ��� ����
    UINT Reserved1              : 1; ///<�̻��
    UINT Reserved2              : 1; ///<�̻��
    UINT Reserved3              : 1; ///<�̻��
} USE_INFO;

/**
@brief   USE_INFO�� ���� Union
@author  puting
@date    2012.11.19
*/
typedef union uCMD_USE_DATA_
{
    // Decode�� ���� ����� ����ü
    USE_INFO Info;
    UINT Data;
    
} uCMD_USE_DATA;


/**
@brief   Motor ���� ����ü
@author  zzang9un
@date    2013.09.10
*/
typedef struct MOTOR_INFO_
{
    // Motor ����
    int  Position;  ///< �̵� �Ÿ�(mm), �� Hoist, Shift�� ��� (1/10000m)
    int  Speed;     ///< �̵� �ӵ�(mm/s)
    UINT ErrorCode; ///< ���� �ڵ�
    BYTE AMPEnable; ///< AMP Enable
    BYTE IsRunning; ///< Running ������ �Ǵ��ϴ� ����

    // 2016-12-03, Add by Labeler
    BYTE IsOrigin; ///< Rotate Motor�� Origin ��/�� üũ ���� �߰�
} MOTOR_INFO;

/**
@brief   Version ���� ������ ���� ����ü
@author  zzang9un
@date    2013.03.26
*/
typedef struct VERSION_DATA_
{
    UINT Major      : 8; ///< Major
    UINT Minor      : 8; ///< Minor
    UINT Release    : 8; ///< Release
    UINT Build      : 8; ///< Build
} VERSION_DATA;

/**
@brief   Version ���� ������ ���� Union
@author  zzang9un
@date    2013.03.26
*/
typedef union uVERSION_DATA_
{
    VERSION_DATA Info;
    UINT         Data;
} uVERSION_DATA;

/**
@brief   Mark ������ ���� ����ü
@author  zzang9un
@date    2013.03.29
*/
typedef struct MARK_INFO_
{
    BYTE DrivingTag : 2; ///< ���� Tag Marked
    BYTE StopTag    : 2; ///< Stop Tag Marked
    BYTE QRLTag_L   : 2; ///< QR Left Tag Marked
    BYTE QRLTag_R   : 2; ///< QR Right Tag Marked
} MARK_INFO;

/**
@brief   Mark ������ ���� Union
@author  zzang9un
@date    2013.03.29
*/
typedef union uMARK_INFO_
{
    MARK_INFO Info;
    BYTE Data;
} uMARK_INFO;

/**
@brief   Data Load ������ ���� ����ü
@author  zzang9un
@date    2013.07.22
*/
typedef struct DATALOADED_
{
    BYTE Map        : 1; ///< Map Data Load Flag
    BYTE Station    : 1; ///< Station Data Load Flag 
    BYTE Teaching   : 1; ///< Teaching Data Load Flag 
    BYTE Reserved1  : 1; ///< Reserved
    BYTE Reserved2  : 1; ///< Reserved
    BYTE Reserved3  : 1; ///< Reserved
    BYTE Reserved4  : 1; ///< Reserved
    BYTE Reserved5  : 1; ///< Reserved
} DATALOADED;

/**
@brief   Data Load ������ ���� union
@author  zzang9un
@date    2013.07.22
*/
typedef union uDATALOADED_
{
    DATALOADED Info;
    BYTE Data;
} uDATALOADED;

/**
@brief   OHT Status �⺻ Data
@author  zzang9un
@date    2013.03.20
@note    OCS�� TP, �ڰ����� �� OHT Status�� ���� �⺻ Data�� ��� ������ ����ü
*/
typedef struct OHT_COMMON_STATUS_
{
    // �⺻ ����
    // by zzang9un 2013.03.21 : �Ʒ� 4���� ������ ����ü �ϳ��� ������
    // BYTE Mode;              ///< OHT ���
    // BYTE Status;            ///< OHT ����
    // BYTE PreCmdID;          ///< Previous Command ID
    // BYTE CurCmdID;          ///< Current Command ID
    uMSCMDID MSCmdID;       ///< Mode, Status, CmdID ����ü

    BYTE NextCmdID;         ///< Next Command ID

    UINT CurNodeID;         ///< ���� ���� Node ID
    int  CurNodeOffset;     ///< ���� ���� Node�κ��� �̵��� �Ÿ�(mm)
    UINT CurStationID;      ///< ���� ������ Node ID
    UINT StopNodeID;        ///< Target ���� Node ID
    UINT StopStationID;     ///< Target ������ Node ID
    int  StopStationOffset; ///< ���� Node�κ��� Target Trans Node�� �Ÿ�(mm)
    BYTE ExistFoup;         ///< ���� ���� ����
    BYTE CarrierType;       ///< �ݼ۹� ����
    BYTE PauseStatus;       ///< OHT Pause ����(��ֹ�, ���� ���� ����)
    BYTE SteerPosition;     ///< ���� ����
    UINT ErrorCode;         ///< OHT ���� �ڵ�

	ORIGIN_INFO OriginInfo; ///< 2~4��, Hand, Shutter ���� ����


} OHT_COMMON_STATUS;
/**
@brief   Status(OHT->OCS) ����ü
@author  zzang9un
@date    2012.11.04
*/
typedef struct MSG_STATUS_OHT2OCS_
{
    OHT_COMMON_STATUS Data; ///< OHT �⺻ ����

    // RF Tag Data
    char RFTagData[10];         ///< RF Tag Data

    // AP ����
    UINT APSignalLevel;         ///< OHT ���� ��ȣ ����
    BYTE APMacAddr[6];          ///< AP Mac Address

    // Motor ����
    int  Motor_Drv_F_Position;  ///< OHT ���� �� Encoder ��(mm)
    int  Motor_Drv_F_Speed;     ///< OHT ���� �ӵ�(mm/s)
    int  Motor_Hoist_Position;  ///< Hoist�� �̵� �Ÿ�(mm)
    int  Motor_Shift_Position;  ///< Shift�� �̵� �Ÿ�(mm)
    int  Motor_Rotate;          ///< Rotate�� �̵� �Ÿ�(radian)

    BYTE OHTType;               ///< OHT Type

    // by zzang9un 2013.09.06 : Data ���� ����
    int MapData_CurRevision;
    int StationData_CurRevision;
    int TeachingData_CurRevision;

    // by zzang9un 2013.05.14 : 7.0 Ư�� Status(HOHT�� �����Ǵ� �κ�)
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
} MSG_STATUS_OHT2OCS;

/**
@brief   Status(OHT->TP) ����ü
@author  puting
@date    2012.11.19
@note    Input, Output�� ��� Light-On, Dark-On�� ����� �ʿ� ����
*/
typedef struct MSG_STATUS_OHT2TP_
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
    UINT MapMakeData;      ///< Map Make Data(���� : mm)
    BYTE DownPercent;       ///< Down Percent
    double CPUTemperature;  ///< IPC �µ�
    UINT APSignalLevel;     ///< �������� ��ȣ

    // Log ���� ����
    BYTE isLogDrive;       ///< Log Save Mode

    BYTE STBDirection;      ///< STB Direction
    BYTE IsRotateExist;     ///< Rotate Exist
    BYTE IsRFPIORS23;       ///< RF PIO RS23 ����
    BYTE IsRFPIO6Digit;     ///< FR PIO 6Digit ����
    BYTE IsFBWF;            ///< FBWF ����
    BYTE IsRFPIOType;       ///< RF PIO Type  

    // by zzang9un 2013.03.29 : ���� ���� ���� ����ü
    uVERSION_DATA OHTVer;   ///< OHT Version
    uVERSION_DATA AMCVer;   ///< AMC Version

    // by zzang9un 2013.03.29 : Mark ������ ���� ����ü �߰�
    uMARK_INFO MarkInfo;    ///< Mark ������ ���� ����ü

    // by zzang9un 2013.07.22 : Data �ε� ���� �߰�
    uDATALOADED DataLoaded; ///<  Data �ε� ����
} MSG_STATUS_OHT2TP;

///**
//@brief   HID ������ ���� ����ü
//@author  zzang9un
//@date    2012.11.07
//*/
typedef struct HID_DATA_
{
	int Edc;                ///<���ַ����� �������: 300 -> 300V
	int Ecap;               ///< ĸ ��ũ ���� : 3�ڸ�
	int Eout;               ///< ��� ����
	char Iout_sign;         ///< �����������(+,-) : -�����̸� ȸ�����
	int Iout;               ///< ��ŧ������ �Է�����
	int Iac;                ///< Pickup �Է� ����

	int ErrorCode;          ///< �����ڵ� : 000~999 ����
//	int WarningCode;        ///< warning �ڵ�

	int PickupTemperature;  ///< 132 -> 132��

	int IGBTTemperature;
	int ECap1Temperature;
	int ECap2Temperature;
	int ECapRoundTemperature;
	int Reserve1;
	int Reserve2;
	int Reserve3;
} HID_DATA;

/**
@brief   APSIGNAL ������ ���� ����ü
@author  puting
@date    2013.03.26
*/
typedef struct APSIGNAL_DATA_
{
	int 	nAPSignal;
	int 	nAPSignalLevel;
	unsigned int     nChannel;
	unsigned char 	szAPMacAddress[6];
	unsigned char 	szSSID[32];
} APSIGNAL_DATA;

/**
@brief   SIGMA ������ ���� ����ü
@author  puting
@date    2013.07.10
*/
typedef struct SIGMA_DATA_
{
	DWORD dwTempErrcode;
	long lTempReadData;
} SIGMA_DATA;

// ---------------------------------------------------------------------------
/**
@brief   IPC_DATA_INFO�� ���� ����ü
@author  puting
@date    2013.03.28
*/
typedef struct _IPC_DATA_INFO_ {
	double dCPUTemperature; ;
	///< IPC CPU �µ�
	double dVoltage_Input;
	///< IPC �Է� ����
} IPC_DATA_INFO;


/**
@brief   IPC_SYS_INFO�� ���� ����ü
@author  puting
@date    2013.03.28
*/
typedef struct _IPC_SYS_INFO_
{
	char szOHTSysTime[16];

	BYTE DrvType;
	char szFileSysName[16];
	UINT64 nfreeSpace;
	UINT64 nTotalSpace;
} IPC_SYS_INFO;


/**
@brief   CPU_INFO�� ���� ����ü
@author  puting
@date    2013.03.28
*/
typedef struct _CPU_INFO_ {
	int nCPU;
	long lMem;
} CPU_INFO;

/**
@brief   OHT_MONITORING_SYS_INFO�� ���� ����ü
@author  puting
@date    2013.07.16
*/
typedef struct _OHT_DATA_INFO_
{
	 //Data info
	double dCurr_DrivingVel;
	double dCurr_HoistVel;
	double dCurr_ShiftVel;
	double dCurr_RotateVel;

	int IOInput[4];
	int IOOutput[4];
	char nFan_Alive1; //IOInput
	int nAMCVer;

} OHT_DATA_INFO;

/**
@brief   Operation Status �������� ���� ����͸� ����ü
@author  doori.shin
@date    2014.06.11
*/
typedef struct _MONITOR_OPERATION_STATUS
{
	BYTE bIsFBWF;
	BYTE LogSaveMode;
	BYTE bIs_LogSave;
	BYTE bIs_USBAlive;
	UINT nLogCount;

	BYTE bIs_OHTInk;
	BYTE bIs_AssistantInk;
	BYTE bIs_VNCInk;

	IPC_SYS_INFO IPCSysInfo;  //�ý�������(��ũ, �ð�)

	char szDBVersion[16];   //V3 DB Version
	char szVirusVersion[16];  //V3 Version
	UINT nOHTVer;
	UINT nAMCVer;
	UINT nAssistantVer;
	UINT nMapVer;         //Map Revision
	UINT nStationVer;     //Station Revision
	UINT nTeachingVer;    //Teaching Revision
	UINT nCIDVer;         //CID Revision
	UINT nCollisionVer;   //Collision Revision
	UINT nLooKDownVer;    //Lookdown Revision
	UINT nFlowControlVer; //Flowcontrol Revision

	CPU_INFO CPUandMemInfo;   //CPU and Memory ������
	BYTE Is_VirusCheckProgram;  //�������� ���α׷�
	BYTE bIsVirusCheckProgramExec; //���హ��

	int Reserve1;
	int Reserve2;
	int Reserve3;
	int Reserve4;
	int Reserve5;
}MONITOR_OPERATION_STATUS;



/**
@brief   Movement �������� ���� ����͸� ����ü
@author  doori.shin
@date    2014.06.11
*/
typedef struct _MONITOR_MOVEMENT_DATA
{
	double dSum_DrivingPosition;
	double dSum_HoistPosition;
	double dSum_ShiftPosition;
	double dSum_RotatePosition;
	UINT nSum_SteerMoveCount;
	UINT nSum_HandMoveCount;
	UINT nSum_ShutterMoveCount;

	UINT nMaxHandMovingTime;
	UINT nMaxShutterMovingTime;
	UINT nMaxFrontSteeringMovingTime;
	UINT nMaxRearSteeringMovingTime;

	double fMaxVelgap;
	UINT nVelgapEventCount;
	UINT nCountHandReset;
	UINT nFollowingLimitOverCountDrivingAxis;
	UINT nFollowingLimitOverCountHoistAxis;
	UINT nFollowingLimitOverCountShiftAxis;

	int Reserve1;
	int Reserve2;
	int Reserve3;
	int Reserve4;
	int Reserve5;
	int Reserve6;
}MONITOR_MOVEMENT_DATA;


/**
@brief   OHT Disk ���� ����ü
@author  doori.shin
@date    2013.12.18
*/
typedef struct DISK_INFO_
{
	char  Drive;
	INT64 TotalSpace;
	INT64 FreeSpace;
	INT64 UsedSpace;
}DISK_INFO;

/**
@brief    Monitor ����ü
@author  bDAyTA  Jeong
@date    2013.06.03
*/
typedef struct _MONITOR_DATA_INFO_ {
	//IPC//
	IPC_DATA_INFO	IPCData;

	//HID//
	HID_DATA HIDData;

	//Network//
	APSIGNAL_DATA APSignalData;

	//CPU ���� //
	CPU_INFO pValue;

	//Servo Pack//
	SIGMA_DATA SigmaData;

	 //Monitoring Info//
	 MONITOR_OPERATION_STATUS    MonitorOperStatus;         //<--�̺κ� �߰�
	 MONITOR_MOVEMENT_DATA       MonitorMovementData;    //<--�̺κ� �߰�

} MONITOR_DATA_INFO;

/**
@brief   OHT �� Monitor Info ���� ����ü
@author  by Jeong
@date    2013.06.03
*/
typedef struct _TOTAL_DATA_INFO_
{
	MONITOR_DATA_INFO MonitorInfo;
	OHT_DATA_INFO OHTInfo;
}TOTAL_DATA_INFO;


/**
@brief   IO Map Field ����ü
@author  by doori.shin
@date    2013.08.14
*/
typedef struct IO_MAP_FIELD_
{
	bool fIsLoaded;

    int InputNum;
    int OutputNum;

	AnsiString* InputField;
    AnsiString* OutputField;
}IO_MAP_FIELD;
//---------------------------------------------------------------------------

/*
@brief	 Motion ������ ���� �޴� User ������ ��ȯ�� Node ����
@author  ���¿�
@date	 2013.06.30
*/
typedef struct STATUS_NODE_CHECK_DATA_
{
	int NodeCountOfPreCheck;	///< ��� ����
	int NodeCount;				///< ��� ����

	// SetHome, setOffset �� �ϸ� ���� ��ġ�� �� ������ ������
	double	PositionOfDrivingTagPreCheck;	///< ���� ��� ���� ������ ��ġ
	double	PositionOfDrivingTag;			///< ���� ��� ������ ��ġ
	double	PositionOfTransTagFirst;		///< ������ ��� ������ ��ġ(Front)
	double	PositionOfTransTagFinal;		///< ������ ��� ������ ��ġ(Rear)

}STATUS_NODE_CHECK_DATA;

#pragma pack(pop) // Pack ����


#endif // Def_DataTypeH
