//---------------------------------------------------------------------------
/**
@file    MainControlTask.h
@version OHT 7.0
@brief   OHT���� MainControl ������ ����ϴ� Header File
*/

#ifndef TaskControlH
#define TaskControlH
//---------------------------------------------------------------------------

#include "../Common/Protocol_Define_Modify.h"
#include "../Common/UdpCom.h"
#include "../Common/UdpDiag.h"
#include "PassPermit.h"

#include "Def_OHT.h"
#include "../Common/Def_Protocol.h"
#include "Def_HW.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"

#include "ServerSocket.h"
#include "ExecuteInfo.h"
#include "ExecuteThread.h"
#include "OHTMainStatus.h"
#include "CmdManager.h"
#include "PathSearch.h"
#include "HWSetStatus.h"
#include "HWCommon.h"
#include "HWSet.h"
#include "HWControlThread.h"
#include "DrivingControl.h"
#include "TransControl.h"
#include "ManualControl.h"
#include "StatusCheckThread.h"
#include "Utility.h"
#include "LogHandler.h"
#include "LogThread.h"
#include "VersionInfo.h"
#include "STD_PathData.h"
#include "StationData.h"
#include "TeachingData.h"
#include "NetworkSignallib.h"

#include "DiagManager.h"

#include "IoTManager.h"
#include "MyTools.h"
//#include "AvsControl.h"
//---------------------------------------------------------------------------
// Task State Definition
//---------------------------------------------------------------------------
#define STEP_NO_DETECT_CHECK            0
#define STEP_LONG_CHECK     			1
#define STEP_LONG_MIDDLE_CHECK          2
#define STEP_MIDDLE_CHECK               3
#define STEP_MIDDLE_SHORT_CHECK         4
#define STEP_LONG_MIDDLE_SHORT_CHECK    5
#define STEP_LONG_SHORT_CHECK           6



#define PARMA_DIAG    		0
#define PARMA_DRIVING    	1
#define PARMA_EQPIO    		2
#define PARMA_PATHSEARCH    3
#define PARMA_TRANS    		4
#define PARMA_UBGCHECKNODES	5
#define PARMA_IOMAP    		6
#define PARMA_OHTDETECTAREA	7
#define PARMA_OBSDETECTAREA 8



/**
@brief   Main Thread ���� ���� ����
@author  zzang9un
@date    2012.11.16
@note    Long Run ������ Test�� ���� �߰�
*/
enum MAIN_TASK_STATE
{
	MAIN_TASK_INITSOCKET = 0,   ///< Task ���� : Node
	MAIN_TASK_DATALOAD,         ///< Task ���� : ��� ���� �Ϸ�
	MAIN_TASK_SOCKETENABLE,     ///< Task ���� : ��� Open
	MAIN_TASK_READY,            ///< Task ���� : OHT Init(Mode, Status)
//	MAIN_TASK_CHECKERROR,       ///< Task ���� : Error�� üũ�Ѵ�.
	MAIN_TASK_CHECKCMD,         ///< Task ���� : Command�� üũ
	MAIN_TASK_SHUTDOWN,         ///< Task ���� : ����
	MAIN_TASK_ERROR             ///< Task ���� : ���� ����
};

// 20161216 sh17.jo : Commfail üũ ��� �߰�
enum NETWORK_RESET_STATE
{
	NETRESET_SOCKET_ERROR = -4,
	NETRESET_IP_ABNORMAL	= -3,
	NETRESET_IP_NULL = -2,
	NETRESET_ABNORMAL_CONNECTION = -1,
	NETRESET_PING_FAIL = 0,
	NETRESET_PING_SUCCESS
};
//---------------------------------------------------------------------------

typedef struct HTAG_DATA_
{
	char CmdData2       : 8;
	char CmdData1       : 8;
	char ProtoCmdType   : 8;
	char SenderReceiver : 8;
}HTAG_DATA;

typedef union _uHTagData
{
	HTAG_DATA Htag_Data;
	int nWholeTagData;

}uHTagData;

// ============================= Firmware Update =======================================
typedef struct 
{
	int iMainUpdate;		// TRUE, FALSE. OHT Main�� ������Ʈ ���� ���θ� �����Ѵ�
	int iAmcUpdate;			// TRUE, FALSE. AMC�� ������Ʈ ���� ���θ� �����Ѵ� 
	int iAsUpdate;			// TRUE, FALSE. ��ý��Ͻ��� ������Ʈ ���� ���θ� �����Ѵ�
	int iFmUpdate;			// TRUE, FALSE. File Manager�� ������Ʈ ���� ���θ� �����Ѵ�
	int iParamUpdate;		// TRUE, FALSE. �Ķ���͸� ������Ʈ ���� ���θ� �����Ѵ�
	
	int iWirelessConfig;	// TRUE, FALSE. ������� Config ������Ʈ ���θ� �����Ѵ�
	int iWirelessMacfil;	// TRUE, FALSE. ������� Macfil ������Ʈ ���θ� �����Ѵ�
	int iWirelessFirmware;	// TRUE, FALSE. ������� Firmware ������Ʈ ���θ� �����Ѵ�

	int iParamFileCount;	// ������Ʈ �� �Ķ���� ������ ����
	AnsiString strParamFileList[100]; // ������Ʈ �� �Ķ���� ���ϸ��� ����ϴ� ���� (�ִ� 99��)

} FIRMWARE_INFO;


#define FIRMWARE_UPDATE_PATH		"C:\\User\\OHT\\DataFiles\\Down\\"
#define OHT_PARAM_FILE_PATH			"C:\\User\\OHT\\DataFiles\\"
#define OHT_PROGRAM_PATH			"C:\\User\\OHT\\"
#define AMC_BINARY_PATH				"C:\\User\\OHT\\DataFiles\\"
#define WIRELESS_PARAM_FILE_PATH	"C:\\User\\OHT\\DataFiles\\"


#define OHT_MAIN_FILE_NAME 		L"OHT.exe"
#define ASSISTANCE_FILE_NAME    L"OHTAssistant.exe"
#define FILE_MANAGER_FILE_NAME  L"FM.exe"

#define AMC_BOOT_FILE_NAME		L"coffboot.bin"
#define AMC_MAIN_FILE_NAME		L"amc.out"

#define WIRELESS_CONFIG_FILE_NAME	L"CONFIG"
#define WIRELESS_MACFIL_FILE_NAME	L"MACFIL"
#define WIRELESS_FIRMWARE_FILE_NAME L"FIRMWARE.BIN"


#define BACKUP_FILE_EXTENSION	".bak"


#define AS_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\AsInfo.dat"
#define FM_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\FmInfo.dat"


// Firmware Update Response Direction (OHT -> TP, OHT -> OCS)
enum FWU_RESPONSE
{
	FWU_TO_OCS = 0,   
	FWU_TO_TP,
};

// wireless reset msg 
typedef	enum	WIRELESS_RESET_RESULT_
{
	WIRELESS_RESET_OK			= 0,
	WIRELESS_RESET_WSA_ERR,
	WIRELESS_RESET_INVALID_SOCKET,
	WIRELESS_RESET_CONNECT_ERR,
	WIRELESS_RESET_SOCKET_OPT_ERR,
	WIRELESS_RESET_NO_CONN_MSG,
	WIRELESS_RESET_NO_PASS_MSG,
	WIRELESS_RESET_NO_PASS_OK_MSG,
	WIRELESS_RESET_NO_RESET_OK_MSG,

}WIRELESS_RESET_RESULT;

enum TARGET_DEVICE
{
    OCS_P = 0,
    OCS_S,
    GATEWAY,
	WIRELESS_CLIENT,
	OCS_GATEWAY
};

enum PASS_PERMIT_OPEN_TYPE
{
    FROMTP_TYPE = 0,
    AUTOCHANGE_TYPE
};

typedef struct sIoT_Data
{
	IOT_HEADER 				Info1;
	IOT_DEFECT_SIGNAL_COUNT Info2;

	sIoT_Data()	{	memset(this, 0, sizeof(*this));	}

} sIoT_Data;

#define UPDATE_DRIVING_INI		0
#define UPDATE_TRANS_INI		1
#define UPDATE_EQPIO_INI		2
#define UPDATE_DIAG_PARAM_INI	3
#define UPDATE_PATHSEARCH_INI	4
#define UPDATE_IOMAP_INI		5


#define PREVIOUS_LOG_WRITE_INTERVAL_MIN		   	 60 // ��������  ����� �ð� ����(���� : ��) AOHC-237 UBG ��/�� Ʋ���� ���� ��� ��
// =====================================================================================

/**
@class   TaskControl
@date    2013.05.24
@author  puting
@brief   TaskControl Class
*/
class TaskControl
{
private:
	LogUnit *pLogTaskControl;      ///< TaskControl �α� ������ ���� ����
	LogUnit *m_pLogOCSComm; ///< OCS Comm Log ������ ���� ���� ����
	LogUnit *m_pLogTPComm;  ///< TP Comm Log ������ ���� ���� ����
	LogUnit *pLogMapMake; ///< Map Make ���� �α� ����
	LogUnit *pLogIotComm; ///< IOT ��� �α� ����
	LogUnit *pLogError; ///< Error log���� �α� ����
	LogUnit *pLogPing; ///< PING ��� �α� ����
	LogUnit *pLogHandTP; ///< HandTP ��� �α� ����
	LogUnit *pLogMCC;

	FDCUnit *pFDCUnit;

	//�����ڿ��� �ޱ����� ������
	UdpCom *m_pUDPCommunication;       ///< Communication Class
	UdpDiag *m_pUDPDiag;  ///<Communication Class
	CmdManager *m_pCmdManager;       ///< Command Manager Class
	PassPermit *m_pPassPermit;
	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;
	PathSearch    *m_pPathSearch;
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_TASK_PARAM_SET	m_mccparam;



	// by zzang9un 2013.03.22 : TP���� ������ ���� OHT Info ����(Union ���)
	uTP_CMD m_OHTInfo;      ///< OHT Info


	// OCSStatus Send ���� �� TaskControl ������ �۵����� Check

	bool TaskControlfalg;
	int  nCheckErrorOHTType;

	MAIN_TASK_STATE TaskState_Cur;     ///< Main Thread Task State(����)
	MAIN_TASK_STATE TaskState_Pre;     ///< Main Thread Task State(����)

	// Status�� ���� ����
	MSG_STATUS_OHT2TP m_StatusOHT2TP;

	MAPMAKE_STATUS *m_pMapMake_Status; //2013.07.11 �߰�

	MapMakeProcess *m_pMapMake_Process; //2019.09.15 �߰�
	MAPMAKE_PROCESS m_MapMakeMoveProcess; //2019.09.15 �߰�

	// Firmware Update Response Direction
	FWU_RESPONSE m_iFWU_Direction;

	AnsiString m_strMonitor;

	// doori.shin 2014.07.08 ������TP ���� : ����TP���ӽ� ����TP���� ����
	int m_nTPType;


	// 2016.08.03 ���� IoT Socket �߰�
	SOCKET m_IOTHubSocket;
	ServerSocket *m_IOTHubServerSock;
	bool m_ConnectedIOTHub;


    IotManager *m_IotManager;

	// Data ���� ����
	bool m_bLoadedMap;          ///< Map File Load ���� ����
	bool m_bLoadedStationData;  ///< Station Data File Load ���� ����
	bool m_bLoadedTeachingData; ///< Teaching Data File Load ���� ����

	bool m_bLoadedCID_STD;
	bool m_bLoadedCollision_STD;
	bool m_bLoadedFlowControl_STD;
	bool m_bLoadedMap_STD;
	bool m_bLoadedStation_STD;
	bool m_bLoadedTeaching_STD;
	bool m_bLoadedPathData_STD;
	DWORD m_dwUBGTrigerTime; //AOHC-237 UBG ��/�� Ʋ���� ���� ��� �߰�
	DWORD m_LogTime;  //AOHC-237 UBG ��/�� Ʋ���� ���� ��� �߰�

	int m_MoniUBGPat;
public:
	// 20161216 sh17.jo : Commfail üũ ��� �߰�
	bool m_bCheckRcvData;
	bool m_bSetCheckCommfail;	//OCS���� Commfail üũ �÷���
	int m_nComfailCount[4];
	int m_nComOKCount;
    bool m_bCheckPassOpenListCount;
//    PATHSEARCH_UBG_CHECK_PARAM_SET  *m_pUBGParamSet;     //soming UBG ���� �ܰ� ����
	double m_dMinAccel;


private:

	int m_nOHTDetect_Inpunt_Prev;	// ���� UBG ���� �� 8~2

	// by zzang9un 2013.02.06 : OHT Error Code(���� �߻��� ���� �ڵ带 �ǹ���)
	int OHTError_Current;      ///< OHT�� ���� ������
	int ErrorCode_Main;       ///< Main Thread ���� ����
	//2015.12.10 doori.shin : DiagManager
	DiagManager* m_DiagManager;


	//2013.08.14 doori.shin IO Map Field �����ϴ� ����ü
	IO_MAP_FIELD *m_IOMapField;
	TIniFile *m_IOMapIni;
	String strIOMapPath;
	AnsiString m_strIOVersion;

	//2014.07.15 doori.shin : Network Device
	NetworkSinalLib* m_NetWorkDll;

	// 2016.06.27. shkim. Lifter Name add
	DrivingControl	*m_pDrivingControl;

    // 2017-01-13, Add by Labeler, Auto Change �� ���� �߰�
    bool bAutoChange;
    bool m_bUBGReadCheck;
	// Map Make ó�� �Լ�
//	int SendMapMakeInfoOHT2TP(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int UpdateMapMakeInfo(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int HandleMapMake();

	// Task ���� �Լ�
	void ChangeMainTaskState(MAIN_TASK_STATE MainState);

	// Mode, Status ���� �Լ�
	void ChangeMode(BYTE mode);
	void ChangeStatus(BYTE status);
	void ChangeCmdID(BYTE PreCmdID, BYTE CurCmdID);
	BYTE GetCurrentMode();
	BYTE GetExecuteStatus();
    BYTE GetExecutePreCmdID();
    BYTE GetExecuteCurCmdID();
    bool CheckStatusForDriving();
	bool CheckStatusForTrans();
//    bool CheckStatusForManual();
    bool CheckStatusForCommon();
    bool CheckStatusForReset();
	bool CheckStatusForAuto();
	bool CheckStatusForTask();

	// Map ���� �Լ�
	int LoadMapData();
	int UpdateMapData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);

//	int GetPathData(NODE_INFO_STD_PATHDATA *pPathdata, UINT NodeID);

	// Station ���� �Լ�
//	int LoadStationData();
	int UpdateStationData(EXECUTE_RESPONSE_MAPMAKING_INFO *MapInfo);
	int GetStationData(STATION_DATA *pStationData, UINT StationID);        	// by zzang9un 2013.08.19 : Ư�� Station ID�� Station Data�� �������� �Լ� �߰�

    // Teaching Data ���� �Լ�
//    int LoadTeachingData();
	//int UpdateTeachingData(uTP_CMD *uTPCmd);
	int UpdateTeachingData(PKT_OHT2TP_TEACHING_INFO *pTPPacket, bool bOnlyLookDownChange);
//	int SendTeachingData(uTP_CMD* uSndTPCmd);
	int GetManaulTeachingData(UINT CurStationID, BYTE PortType, TRANS_CMD* pTransData);   //���°� Ȯ�� �� ��Ÿ ȣ�� �Լ� ---puting

	// STD Map Load ���� �Լ�
	int LoadCIDData_STD();
	int LoadCollisionData_STD();
	int LoadFlowControlData_STD();
	int LoadMapData_STD();
	int LoadStationData_STD();
	int LoadTeachingData_STD();
	int LoadPathData_STD();

	// 7.0������  LoadMapData, LoadTeachingData, LoadStationData���� ����ϴ� �ڷᱸ�� ���� �Լ�
	int MakeMapData();
	int MakeStationData();
	int MakeTeachingData();
	
	// Command ���� �Լ�

	int MakeSetStationOffset(STATION_DATA *pStationData);

	//2013.09.10 ji7412.lee HandTP ����߰�
	Remocon *m_pRemocon;
	bool bOpenRemocon;
	UINT m_nInputData[4][32];
	uINPUT_BITFIELD uInput;

	int ComOpenForRemocon();
//	int GetReadRemoconData();
	int MakeAckDataRemocon(char* szReadBuff, int Error);
	void Analysis_IOData();

    // 2016-12-25, Add by Labeler
    // Handy TP �� Longrun ��� �߻� �Լ� �߰�
    int MakeLongrunCmdForHandyTP(char* szReadBuff, uINPUT_BITFIELD uInput);


	///----------Auto to Manual Check ---//
	bool m_bCheckAutoToManual;
    NODE_INFO_STD_STATION m_OCSStaionInfo;


   ////-----------------------------ǥ��ȭ OHT ���� �Լ��� �߰� ����.-------------------------------------------//puting 2015.11.02
	PROTOCOL_STATUS_OHT m_PreStatusOHT2OCS;    ///< ���� ���°� Ȯ���� ���� ����
//	DWORD m_dwJobChangeCheckTime;   //�۾����� ������ ������ Ȯ�� ó���ϱ� ���� ����
//	bool m_bJobChangeCheckFlag;   //�۾����� ������ ������ Ȯ�� ó���ϱ� ���� ����

	//by puting 2015 10.15 ���°� ����
	void ChangeCmdIDAndCycle(BYTE CurCycle, unsigned short CurCmdID, UINT StationNID =0,bool bWaitCycleClear =false);  //���� ����Ŭ ��� ����


	bool CheckStandardOHTStatus();					///< Status ���� ��ȭ Ȯ��
	bool ChangePreStandardOHTStatus(PROTOCOL_STATUS_OHT* pPreStatusOHT2OCS);		///< �������°� ����

	bool CheckStatusForCycleClear();       ///<�۾� ���� ���� �� �����߻���, �۾� Ŭ��� ���� �κ� puting 20160307
	bool CheckTaskSatus();
	bool CheckStatusForCycle();
	bool CheckManualForCycle();

	void CheckSatusCriticalSection();


	// by puting 2015.10.16 : Command�� Parsing�ϴ� �Լ��� ���� �и���
	int StandardParsingOCSCmd(PKT_OCS2OHT* RcvPkt);
	int StandardParsingPCCmd(PKT_OCS2OHT* RcvPkt);
	int StandardParsingASTCmd(PKT_OHT2OCS* RcvPkt);
	int StandardParsingMonitoringCmd(PKT_OHT2DIAG* RcvPkt);
	
	int CheckASTCMDData(PKT_OHT2OCS* pRcvData, PKT_ACK_OHT2OCS* AckPkt);
  	int CheckDIAGCMDData(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* AckPkt);

	//puting 2015. 10. 15 : Cyclist�� ���� ���� �� �����ϴ� �κ�
	int MakeCycleList(PKT_TASKCMDDATA *pCmdData);
	int CheckCycleList(PKT_TASKCMDDATA *pCmdData);
	int ChangeCycleList(PKT_TASKCMDDATA *pCmdData);
	void InitClearCycle(bool bcheckManual = false);
	void InitExcuteOverRun();

	//puting : TaskList ->Cyclist�� �� ����.
	int MakeGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0, int noffset =0);   ///<���� �۾����õ� ���⼭ ����.
	int ChangeGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0);   ///<���� �۾����õ� ���⼭ ����.
	int BackGoCycleList(PARSING_GO_INFO *pCmdData,int StartNode=0, int StationNode =0, int nDiv =0);   ///<���� ������ ���� �κ�.
//	int MakeTransCycleList(PARSING_TRANS_INFO *pCmdData);
	int MakeManualCycleList(EXECUTE_MANUALCONTROL_INFO *pCmdData);      ///< ����, IO���� �� ���� ������ɸ�.

	//puting 2015.11.05  : Cyclelist -> Excute List�� �� ����.
	int MakeGoExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);
	int MakeTransExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);
	int MakeManualExcuteInfo(PKT_CYCLECMDDATA *pCmdData, LList *pExeCmd);


	void MakeResponseStandardOHT2OCS(PKT_ACK_OHT2OCS* AckData);
	void MakeResponseStandardOHT2TP(PKT_OHT2TP_RESPONSE* AckDataPC);
	void MakeResponseStandardOHT2DIAG(PKT_ACK_OHT2DIAG* AckDataMonitoring);

	//--------------�۾����� ���� �Լ�---------------------- puting
	bool CheckJobChange(PROTOCOL_BODY_JOB_ORDER_MCP_TO_CLW_ID_121 *Cmd121,BYTE &chNackToAckType);
	bool PathChangeReq2Driving(int ChangeTagID,int SQNum, bool bJobChage =false);   ///< �۾����� ��û�ϴ� �κ�(������)
	int  CheckPathChange2Driving();  ///< �۾����� �Ϸ����� Ȯ���ϴ� �κ�
	bool CheckChangeCycle(int ChangeTagID);     //<  �ӵ��� ���� �۾����� �Ÿ� Ȯ�� �� ���� ���� Check
	bool CancelPathInfo(int ChangeTagID, int SQNum);

	void JobOrderDataCancel();     //�۾����ÿ� ���� ���� or �ø�����Ŭ�� ��� �̵�����Ŭ�� �����ϴ� ó�� �κ�
	void WaitCycleDataCancel();    //������Ŭ ������ �ִ� ���, ���� ó��(������ ���� ���� ������Ŭ ������ ������) (�̵�����Ŭ�� ����ǰ�, ������ ��ġ�� �������� ��� ������Ŭ ó�� ����.)



	//-----Auto Teaching ���� �Լ� --------------- puting 20161220

//	bool CheckCmdMapMakerResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< file�̵����õ� Map Map Maker TP ��ɿ� ���� ����
//	bool CheckCmdAutoTeachingResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< file Teaching TP ��ɿ� ���� ����


	int MakerMapMakerInfo(int nEndNode, int nStationNode, int nPortType, int iCycle, bool bMove=true);
	int MakeAutoTeachingInfo(int TargetStationID, bool bMove, BYTE PortType,bool bTeachingNullType = false);     // Ȯ���� ����.
	int MakeMapMakerInfo(int EndNode, bool bMapMaker);

	bool m_FileMapMakeflag;   //Flag On ������ TP���� ��� �޾�����, Off������ File�� ���̻� ���� �����Ͱ� ���ų�, �����߻����� ���
	bool m_FileTeachingflag;  //Flag On ������ TP���� ��� �޾�����, Off������ File�� ���̻� ���� �����Ͱ� ���ų�, �����߻����� ���(���� �̻�� Auto Teaching Mudule ������)

	bool  CheckMapMakerFile(int MapCheckType); //file ���� �� ���� Ȯ�� : ���� : 0, ���� X : 1, file Load X : 2
//	int  UpdateMapMakerFile(); //������ ���ŵǾ����� ���� Update MapMaker�� ��츸 Ȯ���Ұ�.
//	bool CheckMapMakerData(); //�÷��� ������ map maker���� �ִ��� Ȯ���ϴ� �κ�
//	bool CheckTeachingData(); //�÷��� ������ teaching Data �մ��� Ȯ���ϴ� �κ�
//	int DeleteMapMakerFile(); //�Ϸ�Ǿ����� ���� ���� �� �ʱ�ȭ
	int Load_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType);

	void Send_MapMakeResult(AnsiString strLog);


	//----------------------------����㰡���� ����-----------------------------------------//
//	unsigned int dwPassPermitTime;                ///����㰡 ��û�� ���� �ð� ����

//	DWORD dwPassPermitTime;                ///����㰡 ��û�� ���� �ð� ����
	bool IsBeforeAutoCheckPassPermit(bool bStationTag, int CurrNode =0,int NextNode =0);       //Auto ��ȯ �� ����㰡 Ȯ��.
	bool IsOHTDetectCheckPassPermit(double dRestDistance);//,int nDectetType);       //���������� ���� ����㰡 ��û ���� Ȯ��  puting 2017.05.11

	void CheckAbsolutePosition();
public:
	void CheckUBGSequence(int nDistanceSensorType); //soming UBG üũ

private:

	void JudgePassPermitReq();     //����㰡 �� ���� ��û �Ǵ��ϴ� �κ�

	void JudgeProgressReq();       //��ô���� ��û
	void SendProgressReq(BYTE SendProgress);

	void JudgeOHTInOutReq();       //���� ���� ���� ��û
	void SendOHTInReq(BYTE SendData);  //���� �䱸
	void SendOHTOutReq();              //���� �䱸
	void SendOHTDefReq();              //��ȸ���� �䱸

	void JudgeCIDReportAndIgnor();       //CID ���� ���� �� CID ����ó�� Ȯ��
	void SendCIDError(BYTE cCIDStateResult, char* sCIDStatus);              //��ȸ���� �䱸
	void CIDInfoOfPassPermit();              //����㰡����Ʈ�� �����ſ� ���� CID ����

	void SendQuickCmd2Execute(EXECUTE_QUICK_CMD QuickCmd);

	FLAG_CID_NOT_USE m_pCurrCIDUSE;
	FLAG_CID_NOT_USE m_pListCIDUSE;

	void JudgeMAPUpdate();
	void SendDataProgressReport(BYTE SendProgressSuccess, BYTE SendProgressFail);

	void MakeOHTInCmd(BYTE Type);              //����/���Կ� ���� ���� ��� ����.

	//���������� �����ߵ�.  Cmd�� ���� ���� Ȯ��

	bool CheckCmdJobOrder(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, TAGTYPE* TagType,  int QuickCmd, STANDARDCATEGORY Catagory);       ///< �۾����� ��ɿ� ���� ���� üũ
	bool CheckCmdPPResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< ����㰡�� ���� ��ɿ� ���� ���� üũ
	bool CheckCmdPPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< ����㰡���濡 ���� ��ɿ� ���� ���� üũ
	bool CheckCmdForcePPOResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< ����㰡���濡 ���� ��ɿ� ���� ���� üũ
//	bool CheckCmdForcePPO(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< ����㰡���濡 ���� ��ɿ� ���� ���� üũ
//    ���� ����㰡 ���� ���� ��û -> �Ϲ� �������� ����� �κ�
//      PASS_PERMIT_OPEN_TYPE
//      FROMTP_TYPE = 0
//      AUTOCHANGE_TYPE =1
    bool CheckCmdPassOpen_fromTP_and_AutoChange(PASS_PERMIT_OPEN_TYPE Type, PKT_OCS2OHT* pRcvData, int QuickCmd);
    bool CheckCmdPPREQResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);    ///< ������ǿ� ���� ��ɿ� ���� ���� üũ
	bool CheckCmdDataTransfer(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);   ///< ������ ���ÿ� ���� ���� üũ
    bool CheckCmdVersion(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);   ///< ���� ���� ��û�� ���� ���� üũ
	bool CheckCmdInsertResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);     ///< ���Կ䱸�� ���� ��ɿ� ���� ���� üũ
	bool CheckCmdMoveRestart(PKT_OCS2OHT* pRcvData, int QuickCmd);    ///< ������� ��ɿ� ����  üũ
	bool CheckStaionInfoReqResponse(PKT_OCS2OHT* pRcvData, int QuickCmd);

	bool CheckCmdAutoChangeResponse(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* pAckPkt, int QuickCmd);    //AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ���� 
																									 ///< ������ȯ  ���� ��ɿ� ���� ���� üũ

	//���ܰ��� �κ�
	bool CheckCmdDiagConnect(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   ///< ������� ���� ��û
	bool CheckCmdDiagInfoReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   ///< ������� ���� ��û
	bool CheckCmdDiagInitReq(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);   	 ///< ������� �ʱ�ȭ ��û
	bool CheckCmdDiagInfoResponse(PKT_OHT2DIAG* pRcvData, PKT_ACK_OHT2DIAG* pAckPkt);  	 ///< ������� ����

	bool ConvertBigLitte(PKT_ACK_OHT2OCS *Temp);
	bool ConvertBigLitteDIAG(PKT_ACK_OHT2DIAG *Temp);

	int m_nCountCheckPathSearch;   			//Path Search NACK�� ����Ƚ�������� ����(���������)
//	char strOHTID[7];                       /// OHT ID �����ϴ� �κ�
//	int LoadOHTData();
	AnsiString strBCmask;					// BoradCast Send Mask
	void SendIoStatus_2_OHT();
	//void SendTeachingInfo_2_OHT();
	void Send_CID_INFO_2_OHT();
	void ReadMCommandResult();
	void SendTeachingInfo_2_OHT(BYTE portType);
	void SendDrivingMapInfo_2_OHT(int StartNID, int EndNID);
	
	int LoadUpdateIniFile(AnsiString strIniFilePath, FIRMWARE_INFO* Info);
	int UpdateAMCDownloadFile(AnsiString strAmcFilePath);

	int FirmwareUpdate_FileExistCheck(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_FileBackup(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_AS_FM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_PARAM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_WIRELESS(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_AMC(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int FirmwareUpdate_RESTART(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
	int ExecuteProgram(String strFileName);
	void getInfoDetailVersion(PKT_VERSION_RESPONSE& SEND_PACKET, FWU_RESPONSE iDirection);
	AnsiString ReqIniVersion(int nIniType);
	void chkFirmwareUpdateDirectory(void);
	void LoadWirelessModuleInfo(AnsiString& IpAddress, AnsiString& UserName, AnsiString& Password);
	void FirmwareUpdateMessageToTp(AnsiString strMsg);
	bool FirmwareUpdate_ModeChange();
	int Change2AutoMode(void);

	bool bMainRestart;
	bool bMainCopy;
	FIRMWARE_INFO m_FIRMWARE_UPDATE_INFO;
	// ------ Parameter apply ------
	int ParamApply(int iUpdateFile);

//	DRIVING_PARAM_SET *m_pTempDrivingParam;
//	TRANS_PARAM_SET *m_pTempTransParam;

	TransControl	    *m_pTransControl;
	// -----------------------------
	
	void IpStrToByteAddr(AnsiString strAddr, int& iHi, int& iM1, int& iM2, int& iLow);
	
	int ManualMoving(int iStartNode, int iStopNode, bool iIsStation, int iPortType, int iCycle);
	bool ManualMovePathSearch(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList);


	int ManualMovingMapMake(int iStartNode, int iStopNode, BYTE MapMakeType);
	int ManualMovePathSearchMapMake(int iStartNode, int iStopNode, int& iDirection, int& iCount, unsigned int* iNodeNumList, int &nTargetNode, bool bMapMakeTestMove= false);

    // 20161216 sh17.jo : Commfail üũ ��� �߰�
    NETWORK_RESET_STATE CheckPingBeforeReset(unsigned char *ip);
    char* ConvertNetResetToString(NETWORK_RESET_STATE state);

    // 2016-12-25, Add by Labeler
    // Handy TP Longrun ��� �߻� ���� �߰�
    int m_nLongrunTypeHandyTP;
    bool m_bRotateJogCmdExist;
    DWORD m_dwRotateJogMoveStartTime;
    int m_nPortType_HandTPLongrun;

	DWORD dwTurnOffJobOrderTime;
	/*
	������ ���۸���� OCS�κ��� �޾� ���������� Ȯ���ϴ� �÷���.
	true�� ����������, OCS���� �� �䱸�ؼ��� �ȵȴ�.
	false�� TP�� �� �䱸�� OCS�� �����ص� �ȴ�
	*/
	bool m_bDataTransper_InProgress;
	void GetServoAlarmCode(int errorCode);
	ServoMonitoring *m_pServoControl;


    void ServerSocketOpen();
	void ServerSocketClose();

	// 2016.08.03  ���� IoT Hub ���� �Լ�(����)
	void IOTHubSocketClientConnect();
	void IOTHubSocketClientDisconnect();
	int IOTHubSocketClientRead(short *StateIot);


	bool bCheck;
	int nCheck;
	DWORD dwUBGCheckTime;
	bool bprebumpper;
	int nUBGSensorDetecting;
	bool bUBGOneHourMonitoring;
	int nCycleCnt;

	bool bIncreased_UD;
	bool bIncreased_UF;
	bool bIncreased_UC;

	 // ä�͸� Ÿ�̸�
	CStepTimer chtTm_UD;
	CStepTimer chtTm_UF;


public:
	TaskControl();
	~TaskControl();

	//////////////////////////////////////////////
	void SetOHTInfo(uTP_CMD m_pOHTInfo);
	int GetOHTError();

	//////�����忡 ���ߵ� Update �Լ�///////
	void ExcuteTaskUpdate();

	//putig 20151117 ǥ��ȭ OHT �����忡 ���ߵ� �Լ���
	void ExcuteStandardTaskUpdate();
	void ExecuteSteps();
	void ExecuteResetSteps();
	void StandardOCSReadDataUpdate();
	void StandardPCReadDataUpdate();
	void StandardASTReadDataUpdate();
	void StandardMonitoringReadDataUpdate();
	void SuperviseSendCmdDataOHT2OCS();
	int SuperviseTaskList2CycleList();
	int SuperviseCycleList2ExcuteInfo();

	CStepClass m_StepDetectAreaCheck;     // Detect Area Check �񵿱� ����	
	CStepClass m_StepAutoChangeReset;     // jhoun.lim
	
	//puting 20170108 MapMakerFile Check�ϴ� �κ�.
	void StandardMapMakerFileUpdate();

	void StandardAddress_Broadcast();

    // 2016-12-19, Add by Labeler, Handy TP ����  Update �Լ�
    int GetReadRemoconData();



	//IOMap Loading �Լ�
	int LoadIOMapField(String, String);

	// Virus check �Լ�
	void CheckVirus();

	int LoadParamIni(int nIniType);


	CMD_HEADER* MakeMonitoringHeader(const UINT& data, int size);


	//doori.shin 2015.12.10 : ����Agent �� �Լ�
	void UpdateStatisticsDiagData();
	void UpdateAbnormalDiagData();
	void UpdateStatusDiagData();
	void UpdateCIDStatusDiagData();

	void IOTHubSocketClientUpdate();

	void SendAbnormalDiagData(DIAG_OHT_ABNORMAL* pSendData);
	void SendStatDiagData(DIAG_OHT_STATISTICS* pSendData);
	void SendStatusDiagData(DIAG_OHT_STATUS_DATA* pSendData);
	void SendReqeustDiagStatData();
	void SendCIDStatusDiagData(DIAG_PKT_BODY_CID_REPORT* pSendData);

	int CheckCMDData2TaskList(PKT_OCS2OHT* pRcvData, PKT_ACK_OHT2OCS* AckPkt, PKT_OHT2TP_RESPONSE* AckPktPC, int QuickCmd);
	int CheckError();

    // Handy TP ���� Public���� ��ȯ
    bool CheckStatusForManual();
	int MakeTransCycleList(PARSING_TRANS_INFO *pCmdData);
	int CheckStatusForAutoChange();  //AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ���� -> Auto ��ȯ �ڵ�ȭ�� ���� ���� �Լ�
	bool CheckAutoChangeFullSemi();  //AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ���� -> Auto ��ȯ �� Ǯ�����ֳ� �Ϻθ� ���ֳ�

    // 20161216 sh17.jo : Commfail üũ ��� �߰�
    void CheckCommfail(void);
    //bool GetCheckCommfail(void);
    int SendSnmpWirelessClient(void);

	// 2016.12.27. comm fail reset func add
	WIRELESS_RESET_RESULT Wireless_Module_Reset(char* chAddr);

	// auto teaching result send (OHT -> TP)
	void Send_AutoTeachResult(int iResult);

    // 2016-12-25, Handy TP�� Longrun Type ��ȯ �Լ�
    int GetHandyTPLongRun();
    void SetHandyTPLongRun(int nType);

    bool GetRotateJogCmd();
    void SetRotateJogCmd(bool bCmd);
    DWORD GetRotateJogMoveStartTime();

	//2013.05.27 ji7412.lee : HandTP�� ���� MakeCmdList(HandTPTest �ӽ��Լ�)
	int MakeCmdList_HandTP(unsigned char* pRecvBuf, bool IsConvertEndian, uINPUT_BITFIELD uInput);

    void MakeVHLStatusForHandyTP(unsigned char * cBuf, int iIndex);
    void MakeVHLIOInfoForHandyTP(unsigned char * cBuf, int iIndex);
	void MakeVHLStatusBGForHandyTP(unsigned char * cBuf, int iIndex);
	void MakeAckDataForHandTp(unsigned char * pSendBuf, int nCmdType, int nError);    

    int CheckInterlock4Cycle(unsigned char* pRecvBuf);
	int CheckInterlock4ManualKey(unsigned char cKeyCmd);

	bool GetNodeTypeSlop();
	bool FirmwareUpdateFlag;
	bool bSlope;

	MyTPAddrMan m_TPAddrMan;        // TP �������� ó���ϱ� ���ؼ� TP �ּ� ���� ��ü
	bool m_bMultiTPConnected;      // TP�� 2�� �̻� ���ӵǾ���
	bool m_bAutoChaning;			// ���� Auto ��ȯ��

	void GetIotData(sIoT_Data *pBuf);
	void GetMCCCount(int &JobO_NAK,int &JobC_NAK,int &JobD_NAK,int &OCSComm);

	bool m_bflagARPClear;	//AOHC-234 C3850 Switch Fail-Over �� ��� ���� ����
private:
	sIoT_Data m_IoTData;  // critical section ������ GetIotData() �Լ� ���ؼ� ����
	IOT_HEADER IotHeader;
	IOT_DEFECT_SIGNAL_COUNT *DefSignalCount;

	//AOHC-234 C3850 Switch Fail-Over �� ��� ���� ����
//	DWORD m_dwARPLastClearTime;
	void CheckARPTableClear(unsigned char *szAddr);
	SWTimer m_SWArpInterval;

public:
	bool bHoistJig;
	bool HoistLongRun(int nAction, int nHoistTeachingPos);
	HWSet 		*m_pHWSet;
	HWCommon 	*m_pHWCommon;
//
//	bool bSlideJig;
//	bool SlideLongRun(int nAction, int nHoistTeachingPos);

};

#endif
