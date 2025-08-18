//---------------------------------------------------------------------------
/**
@file    OHTMain.h 
@version OHT 7.0
@brief   OHT Main Program Main Form Header File

         Sub Cpp File ���
          - Communication.cpp
*/

#ifndef OHTMainH
#define OHTMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <winsock2.h>
//#include <ScktComp.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>
#include <Graphics.hpp>

#pragma comment(lib,"ws2_32.lib")

#include "Def_OHT.h"
#include "Def_Protocol.h"
#include "Def_DefaultIniParam.h"


#include "UdpCom.h"
#include "UdpDiag.h"   //
#include "PassPermit.h"


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
#include "TaskControl.h"
#include "Utility.h"
#include "LogHandler.h"
#include "LogThread.h"
#include "VersionInfo.h"
//#include "MonitorControlThread.h"
//#include "AvsControlThread.h"

//2013.07.09 doori.shin ������� �߰�
#include "TeachingData.h"
#include "StationData.h"

#include "ExtraJobThread.h"
#include "TaskControlThread.h"


#include "MonitoringUnit.h"

#include <ExtCtrls.hpp>

//2016.04.20 KJD ������� �߰�
#include "STD_CID.h"
#include "STD_Collision.h"
#include "STD_FlowControl.h"
#include "STD_MAP.h"
#include "STD_Station.h"
#include "STD_Teaching.h"
#include "STD_PathData.h"

#include "STD_MapMakerFile.h"

//2019-09-03 Puting
#include "MapMakeProcess.h"
//2017.11.29 IJK ������� �߰�
#include "OHTDetect.h"
//#include "AvsControl.h"
//---------------------------------------------------------------------------
#define USE_LOG_MEMO
#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

//ȥ�� PIO
typedef enum EQPIO_TYPE_INFO_
{
    TYPE_PIO_IR   = 0,
    TYPE_PIO_RF   = 1,
    TYPE_PIO_HYBRID = 2
}EQPIO_TYPE_INFO;

/**
@class   TOHTMainForm
@date    2012.11.15
@author  zzang9un
@brief   OHTMainForm Class
*/
class TOHTMainForm : public TForm
{
__published:	// IDE-managed Components
  //  TTimer *tmMainTask;
	//TTimer *tmOCSStatus;
    TTimer *tmInint;
	TMemo *memoMain;
	TTimer *tmUIupdate;
    TPanel *panTop;
    TLabel *lbMode;
    TPanel *panMode;
    TLabel *lbStatus;
    TPanel *panStatus;
    TPanel *panCurNodeID;
	TLabel *lbCurrentNodeID;
    TPanel *panCurStationID;
    TLabel *lbStopNodeID;
    TPanel *panErrorCode;
    TLabel *lbErrorCode;
    TPageControl *pgcMain;
    TTabSheet *tabProgramInfo;
    TTabSheet *tabIOTest;
    TComboBox *cbbAxisNum;
    TComboBox *cbbFnType;
    TCheckBox *chkAxis;
    TCheckBox *chkInputMask1;
    TCheckBox *chkInputMask2;
    TCheckBox *chkInputMask3;
    TCheckBox *chkInputMask4;
    TCheckBox *chkInputMask5;
    TCheckBox *chkInputMask6;
    TCheckBox *chkInputMask7;
    TCheckBox *chkInputMask8;
    TEdit *edtAxisMask_Value;
    TEdit *edtInputMask_Num1;
    TEdit *edtInputMask_Num2;
    TEdit *edtInputMask_Num3;
    TEdit *edtInputMask_Num4;
    TEdit *edtInputMask_Num5;
    TEdit *edtInputMask_Num6;
    TEdit *edtInputMask_Num7;
    TEdit *edtInputMask_Num8;
    TEdit *edtInputMask_Value1;
    TEdit *edtInputMask_Value2;
    TEdit *edtInputMask_Value3;
    TEdit *edtInputMask_Value4;
    TEdit *edtInputMask_Value5;
    TEdit *edtInputMask_Value6;
    TEdit *edtInputMask_Value7;
    TEdit *edtInputMask_Value8;
    TImage *imgProgramIcon;
    TImage *imgIOTest;
    TLabel *lbOHTVer;
    TLabel *lbAMCVer;
	TTimer *tmMinimizer;
	TTimer *tmChkThreadStatus;
	TCheckBox *chkAxisMonitor;
	TComboBox *cbbAxisNum4Monitor;
	TComboBox *cbbMonitorRegister;
	TButton *btnReadMonitorReg;
	TButton *servo;
	TPanel *pnIotHubState;
	TPanel *pnSlope;
	TButton *btn1;
	TTabSheet *Tab81S;
	TLabel *lbl2;
	TEdit *Edit_front_torque;
	TButton *btn_limit_front;
	TButton *btn_limit;
	TEdit *Edit_frontName;
	TLabel *lbl1;
	TEdit *Edit_rear_torque;
	TButton *btn_limit_rear;
	TButton *btn_limit1;
	TEdit *Edit_rearName;
	TCheckBox *CheckBox_ignoreOHT;
	TCheckBox *CheckBox_ignoreOBS;
	TLabel *lblFPGA;
	TTimer *tmAvsControl;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);

    // Timer �Լ�
    void __fastcall tmInintTimer(TObject *Sender);
	void __fastcall tmUIupdateTimer(TObject *Sender);
	void __fastcall AppException(TObject *Sender, Exception *E);
	void __fastcall chkAxisClick(TObject *Sender);
    void __fastcall chkInputMask1Click(TObject *Sender);
    void __fastcall chkInputMask2Click(TObject *Sender);
    void __fastcall chkInputMask3Click(TObject *Sender);
	void __fastcall chkInputMask4Click(TObject *Sender);
    void __fastcall chkInputMask5Click(TObject *Sender);
    void __fastcall chkInputMask6Click(TObject *Sender);
    void __fastcall chkInputMask7Click(TObject *Sender);
	void __fastcall chkInputMask8Click(TObject *Sender);
	void __fastcall imgIOTestDblClick(TObject *Sender);
	void __fastcall tmMinimizerTimer(TObject *Sender);
	void __fastcall OnRestore(TObject *Sender);
	void __fastcall tmChkThreadStatusTimer(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

	void __fastcall btnReadMonitorRegClick(TObject *Sender);
	void __fastcall chkAxisMonitorClick(TObject *Sender);
	void __fastcall servoClick(TObject *Sender);
	void __fastcall btn1Click(TObject *Sender);
	void __fastcall btn_limit_frontClick(TObject *Sender);
	void __fastcall btn_limitClick(TObject *Sender);
	void __fastcall btn_limit_rearClick(TObject *Sender);
	void __fastcall btn_limit1Click(TObject *Sender);
	void __fastcall tmAvsControlTimer(TObject *Sender);



private:	// User declarations
	// -------------------------------------------
	LogUnit *pLogTaskControl;   ///< Main Log ������ ���� ���� ����
	LogUnit *pLogOCSComm;       ///< OCS Comm Log ������ ���� ���� ����
	LogUnit *pLogTPComm;       ///< TP Comm Log ������ ���� ���� ����
    
	// 1. ���� ����, Data ���� Class
	LogThread *m_LogThread;

	//Server ���� ���۹� MainControlTask�� ���� Thread
	TaskControl	*m_ControlTask;
	TaskControlThread   *m_TaskControlThread;

	// Main Command List
//	LList *m_CmdList;                  ///< Main Command List
	LList *m_TaskList;                  ///< Main Command List
	LList *m_ResponseList;                  ///< Main Command List
	LList *m_CycleList;                  ///< Main Command List
	LList *m_PathList;                  ///< Main Command List
	LList *m_PassOpenList;                  ///< Pass Permit Open Command List
	LList *m_PassPermitList;                  ///< Pass Permit Open Command List

	/// Execute Info(MainThread�� ExecuteThread ����)
    EXECUTE_INFO *gm_ExecuteInfo;

	/// OHT Main Status(MainThread�� StatusThread, LogThread ����)
	OHTMainStatus *gm_OHTMainStatus;

    /// HWSet Status(StatusThread�� HWSetThread ����)
    HWSetStatus *gm_HWSetStatus;

    //2013.07.11 doori.shin
    //Function flag(Task Thread�� ExtraJobThread ����)
	MAPMAKE_STATUS *m_MapMake_Status;

	MAP *m_MapData;                     ///< Map Data
	MAP *m_TmpMapData;                  ///< Temp Map Data(for Map Make)
	MapDataManager *m_MapDataManager;   ///< MapDataManager

	//2013.07.09 doori.shin Ŭ���� ����
	StationDataManager *m_StationDataManager;   ///< Station Data Manager
	TeachingDataManager *m_TeachingDataManager; ///< Teaching Data Manager

	MapMakeProcess *m_MapMakeProcess;

	ServoMonitoring	*m_pServoControl;
	
	// Data
	STD_CID *m_CIDData_STD;
	STD_Collision *m_CollisionData_STD;
	STD_FlowControl *m_FlowControlData_STD;
	STD_MAP *m_MapData_STD;
	STD_Station *m_StationData_STD;
	STD_Teaching *m_TeachingData_STD;
	STD_PathData *m_PathData_STD;

	// Data Manager
	CIDManager_STD *m_CIDManager_STD;
	CollisionManager_STD *m_CollisionManager_STD;
	FlowControlManager_STD *m_FlowControlManager_STD;
	MapDataManager_STD *m_MapManager_STD;
	StationManager_STD *m_StationManager_STD;
	TeachingManager_STD *m_TeachingManager_STD;
	PathDataManager_STD *m_PathDataManager_STD;

	

	//Auto Map Teaching
//	STD_MapMakerFile *m_MapMakerFile_STD;
	MapMakerFileManager_STD *m_MapMakerFileManager_STD;

	//2013.08.07 doori.shin
	ExtraJobThread *m_ExtraJobThread;

	// -------------------------------------------
	// 2. Command, Communication Class
	// by zzang9un 2013.01.23 : Socket�� ��� ServerSocket���� ����
	CmdManager *m_CmdManager;       ///< Command Manager Class
	PathSearch *m_PathSearch;
    PassPermit *m_PassPermit;

	bool checkLogDrive;
	bool checkLogFolder;

//	SOCKET ClientInfo;
	//ȥ�� PIO
	EQPIO_TYPE_INFO m_nEQPIO_Type;

	UdpCom  *m_UDPComm;
	UdpDiag *m_UDPDiag;

	// -------------------------------------------
	// 3. HW Set
	HWCommon *m_HWCommon;               ///< HWCommon Class
	HWSet *m_HWSet;                     ///< HWSet Class
	HWControlThread *m_HWControlThread; ///< HWControlThread Class
	HwMask	*m_MaskOfHW;					///< HW_Common Mask

    // -------------------------------------------
	// 4. D,T, M & Execute Thread
	DrivingControl *m_DrivingControl; ///< DrivingControl Class
    TransControl *m_TransControl;     ///< TransControl Class
    ManualControl *m_ManualControl;   ///< ManualControl Class
	ExecuteThread *m_ExecuteThread;   ///< ExecuteThread Class

    // -------------------------------------------
	// 5. Status Thread
	StatusCheckThread *m_StatusCheckThread; ///< StatusCheckThread Class


	DEFAULT_PARAM_SET		*m_pParamSet;		///< �Ķ���� ����: ���� ���Ǵ� ��
	OHTMainStatus *m_pOHTMainStatus;
	OHTDetect		*m_pOHTDetect;
//	AvsControl *m_AvsControl;
	// 6. Monitoring Thread
//	HID_VII_ZR *m_HID;
//	IPCLibrary *m_IPCLoadLib;
//	NetworkSinalLib *m_NetworkDll;
//	CPUandMemInfo *m_CPUandMemInfo;
//	MONITORING *m_EtcMonitoring;
	//m_SigmaMonitor = NULL;	//new SigmaMonitor();
//	MonitorControlThread *m_MonitorControlThread;	///< MonitorControlThread Class

    // �ʱ�ȭ ���� �Լ�
    void OHTInit();
	void CreateClass();
	void DestroyClass();
	void CaptureScreen();
	
	//2014.04.15 doori.shin Thread check�� ���� ����, �� �����庰 ������ ����
	int m_nPreExecuteThTick;	///< ExecuteThread
	int m_nPreExtraJobThTick;	///< ExtraJobThread
	int m_nPreHWCtrlThTick;		///< HWControlThread
	int m_nPreTaskCtrlThTick;	///< TaskControlThread
	int m_nPreStatusChkThTick;	///< StatusCheckThread
	int m_nPreLogThTick;	///< StatusCheckThread
	inline BYTE CheckThreadStatus(ThreadCtrl* thread, int& tick);
	inline String GetThreadStatusToString(BYTE status);

	bool    LoadParameter(char* szFilePath, DEFAULT_PARAM_SET* pParamSet);    ///< �Ķ���� �ҷ����� �Լ�
	bool    LoadHighSpeedParameter(char* szFilePath);    ///< parameter for 8.1s jhoun.lim

public:		// User declarations
    __fastcall TOHTMainForm(TComponent* Owner);

    CRITICAL_SECTION	 m_CS;

    // by zzang9un 2014.02.06 : Getter �Լ�
    TaskControl* GetTaskControl();
//	LList* GetCmdList();
	LList* GetTaskList();
	LList* GetCycleList();
	LList* GetResponseList();
	LList* GetPathList();
	LList* GetPassOpenList();
	LList* GetPassPermitList();

	EXECUTE_INFO* GetExecuteInfo();
    OHTMainStatus* GetOHTStatus();
    HWSetStatus* GetHWSetStatus();
    MAPMAKE_STATUS* GetMapMakeStatus();
    MAP* GetMap();
    MapDataManager* GetMapDataManager();
    StationDataManager* GetStationDataManager();
	TeachingDataManager* GetTeachingDataManager();

	//STD MAP ���� 
	STD_CID* GetCID_STD();
	STD_Collision* GetCollision_STD();
	STD_FlowControl* GetFlowControl_STD();
	STD_MAP* GetMap_STD();
	STD_Station* GetStation_STD();
	STD_Teaching* GetTeaching_STD();
	STD_PathData* GetPathData_STD();

	NODE_INFO_STD_MAP* GetNodeObj(int nNodeNum);	// //AOHC-125 Auto ��ȯ ��, �ڰ� ���� ��� �߰�
													//��� ��ȣ�� �˶� ��� ��ü �����͸� ��� ������ �������

	CIDManager_STD* GetCIDDataManager_STD();
	CollisionManager_STD* GetCollisionDataManager_STD();
	FlowControlManager_STD* GetFlowControlDataManager_STD();
	MapDataManager_STD* GetMapDataManager_STD();
	StationManager_STD* GetStationDataManager_STD();
	TeachingManager_STD* GetTeachingDataManager_STD();
	PathDataManager_STD* GetPathDataManager_STD();

    //Auto Map Maker Teaaching
//	STD_MapMakerFile* GetMapMakerFile_STD();
	MapMakerFileManager_STD* GetMapMakerFileManager_STD();

	MapMakeProcess* GetMapMakeProcess();

	DEFAULT_PARAM_SET* GetDefaultParam();
	
	CmdManager* GetCmdManager();
	PathSearch* GetPathSearch();
	PassPermit* GetPassPermit();
    HWCommon* GetHWCommon();
    HWSet* GetHWSet();
    DrivingControl* GetDrivingControl();
    TransControl* GetTransControl();
	ManualControl* GetManualControl();
	StatusCheckThread* GetStatusCheckThread();

	UdpCom* GetUDPCom();
	UdpDiag* GetUDPDiag();

    
	uVERSION_DATA GetOHTProgramVersion();

//	AvsControl* GetAvsControl();
//	AvsControlThread *m_AvsControlThread;

	//2015.03.08 doori.shin : Data ������Ʈ �������� �߰�
	void SetMap(MAP* mapData);
	void SetMapDataManager(MapDataManager* mapDataManager);
	void SetStationDataManager(StationDataManager* stationDataManager);
	void SetTeachingDataManager(TeachingDataManager* teachingDataManager);


	void SetCID_STD(STD_CID* CIDData);
	void SetCollision_STD(STD_Collision* CollisionData);
	void SetFlowControl_STD(STD_FlowControl* FlowControlData);
	void SetMap_STD(STD_MAP* MapData);
	void SetStation_STD(STD_Station* StationData);
	void SetTeaching_STD(STD_Teaching* TeachingData);
	void SetPathData_STD(STD_PathData* PathData);



	void SetCIDDataManager_STD(CIDManager_STD* CIDDataManager);
	void SetCollisionDataManager_STD(CollisionManager_STD* CollisionDataManager);
	void SetFlowControlDataManager_STD(FlowControlManager_STD* FlowControlDataManager);
	void SetapDataManager_STD(MapDataManager_STD* MapDataManager);
	void SetStationDataManager_STD(StationManager_STD* StationDataManager);
	void SetTeachingDataManager_STD(TeachingManager_STD* TeachingDataManager);
	void SetPathDataManager_STD(PathDataManager_STD* PathDataManager);

	void SetMapMakerFileManager_STD(MapMakerFileManager_STD* MapMakerFileData);

    void Update_UI();
	String StatusToString(BYTE status);
	void __fastcall SetInputMask(TCheckBox *CB_InputMask, TEdit *Edt_Mask_Num, TEdit *Edt_Mask_Value);

    // ������ �շ� �׽�Ʈ�� ���� �߰�
	//EXECUTE_TRANS_INFO testTrans;
	//EXECUTE_MANUALCONTROL_INFO testJog;

	//2013.05.27 ji7412.lee : Remocon��ü�� ��ȯ�ϱ� ���� �Լ�
	Remocon* GetHWSetRemocon();

	LCD* GetHWSetLCD();

	// By elibeli2 2014.01.23 Origin Check
	int GetReadyToOperation();

	void Set_LogFolder(bool bflag);

	 // by zzang9un 2013.02.19 : ���� ���� �߰�
	TVersionInfo VerInfo;

	void Set_EQPIO_Type(int);
	int  Get_EQPIO_Type();
	
	int g_DataTransProgressStatus;//������ ���� ��ô ���� ���� OHT ���� ���� �����
	int g_MAPUpdateStatus;// �� ������Ʈ ���� ���� �����

	// Map UpdateResult
	unsigned int m_MapUpdateResult;
	unsigned int m_ParamUpdateResult;

	void ForceClose(void);
	
	void HoistParam_Read(int iIndex, double &dbl_Pluse, double &dbl_User);
	void DeviceDifference_Read(double &dbl_Pluse);
	bool HoistParamSaveParameter(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT);
	
	
	//  UBG ���� ���� �Լ�
	//	UBG ���� �Ѱ��� 48�� ascii ���ϵ�.
	static const int COUNTOF_PATTERN_ASCII = 48;
	static const int COUNTOF_PATTERN = 16;
	bool m_bUBGINILoaded;
	char m_szUBGInfo_INI[COUNTOF_PATTERN][COUNTOF_PATTERN_ASCII];  // INI���� ���� ��
	char m_szUBGInfo_Comm[COUNTOF_PATTERN][COUNTOF_PATTERN_ASCII]; // �������� Comm���� ���� ��
	bool LoadPatternData(char *path);
	std::vector<int> m_nArrCheckPattern;	// ����ڰ� üũ�ϰ��� �ϴ� ���� �迭

	void AddUBGPattern(int nPatternNo1234, char *szRecv);
	bool DoCompUBGPattern(int nPatternNo0to15);	//���� ��
	bool WriteUBGRecv_INI();

	// IP ������ �α׷� �����. ���α׷� ����� �ѹ� ����
	void WriteLog_NetworkInfo();
	bool IoTGatewayCheck();
	bool m_bErrIotGateway; // IoT ����Ʈ���� ���ÿ� �����ִٴ� �÷���
	bool m_bErrOHTTypeMatching; // OHTType Matching�� ���� �ִٴ� �÷���
	void ThreadCycleLog(char *msg);

};
//---------------------------------------------------------------------------
extern PACKAGE TOHTMainForm *OHTMainForm;
//---------------------------------------------------------------------------
//AOHC-204 Thread �� ���� �ð� ���� ���
// ������ �ֱ� ����. �����ð�(1~3��) ���� ��� Cycle �ð��� ���ؼ� min, max, avr ����
// ���ֱ� ���Ҵ����� ���
class ThreadTimeMonitor
{

	DWORD min, max, sum;
	int nIdx;
	DWORD dwStart;

public:
	ThreadTimeMonitor();
	void Execute(char *name, DWORD time_ms);
};
#endif
