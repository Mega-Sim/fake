/**
@file	 DrivingControl.h
@version OHT 7.0
@brief	 DrivingControl Class Header File
*/
#ifndef DrivingControlH
#define DrivingControlH

#include <windows.h>
#include <stdio.h>
#include "List.h"
#include "ExecuteInfo.h"
#include "Def_DrivingControl.h"
#include "VirtualDrivingControl.h"
#include "HWSet.h"
#include "MAP.h"
#include "LogHandler.h"	// Log �ۼ� ����
#include "DiagManager.h"
#include "Def_DefaultIniParam.h"
#include "STD_PathData.h"


#if(OHT_NAME == OHT_NAME_STD_V81S)
#define DRIVING_GEAR_RATIO      114.7341619 * 0.8
#define AMCDECEL_VHLDETECT		297318     //3.2m/s^2
#else
#define DRIVING_GEAR_RATIO      114.7341619
#define AMCDECEL_VHLDETECT		397009     //3.5m/s^2
#endif
#include "Def_MCCParam.h"

/**
@class	 DrivingControl
@date	 2012.10.17
@author  ehoto
@brief	 ���� ���� �������̽� Ŭ����

		1. Node Update - Node Count + BCR
		2. ���� Profile ���� (�Ÿ�, ����, �ӵ�)
		3. ���� �ӵ� ���� (��������, ��ֹ� ��..)
		4. ����ġ - ���� Tag / ������ Tag
*/
class DrivingControl:public VirtualDrivingControl//VirtualModuleControl
{
private:
	LogUnit *pLogDrivingError;      ///< DrivingError �α� ������ ���� ����
	LogUnit *pLogBCR;      			///< BCR �α� ������ ���� ����
	LogUnit *pLogDetect;      ///< Detect �α� ������ ���� ����
	LogUnit *pLogCIDMonitor;      			///< CID Monitoring �α� ������ ���� ����
	LogUnit *pLogMCC;
	FDCUnit *pFDCUnit;
	// ����ϴ� HWSet
	BCR				*m_pTransBCR;
	BCR				*m_pDrivingBCR;
	Driving			*m_pDrivingAxis;
	DrivingTag		*m_pDrivingTag;
	Obstacle		*m_pObstacle;
	OHTDetect		*m_pOHTDetect;
	QR_Reader		*m_pLeftQRR;
	QR_Reader		*m_pRightQRR;
    Steer			*m_pSteer;
	TransStopTag	*m_pTransStopTag;
	PassBoxPIO		*m_pPassBoxPIO;
	MTLPIO          *m_pMTLPIO;
	ServoMonitoring	*m_pServoControl;
	OHTMainStatus *m_pOHTMainStatus;

	EXECUTE_INFO *m_pExecuteInfo;
	HWSet *pHWSet;
	DEFAULT_PARAM_SET *m_defualtparam;
	MCC_DRIVING_PARAM_SET	m_mccparam;

	DiagManager*	m_DiagManager;
	PATH_DIRECTION_INFO m_Dectect_Dir;
	PATH_OBS_DIRECTION_INFO m_Dectect_OBS_Dir;

private:	// ���� �� ����ϴ� ���� ���� // �⺻ ���� ����
	USER_DETECT_INFO				m_Detect;	///< ������ ������ Detect ����
	int	m_nPassBoxPIOStep;
	int m_nElevetorRideStep;
	int m_nElevetorDirection;
	int m_nElevetorOffStep;
	bool m_bsecond;  // CID Common Node TG
	bool m_CID_Pause; // CID Pause Check TG  ture : pause, false : resume
	bool m_CID_Status_Pause;
	bool m_CIDComptFlag;
	bool m_CIDMonitoringFlag; // CID Monitoring Flag
	bool m_CIDLinkFlag;  //CID Link ��� Ȯ��
	bool m_CIDStatusFlag; //CID Status Flag
	bool m_CIDNameReadFlag;  // CID��ġ�� �̸��� �����ϰ� (O���) �о�� �Ѵٴ� ���� ���
	
	bool m_Igro_CIDStatusPuaseFlag;
	bool m_CIDComptTryFlag; //CID Compt �õ� Ȯ�� Flag
	bool m_CIDStatusPauseFlag;//CID-R�̻� �Ǵ� Link TimeOut���� OCS�����ϰ� pause�������� ��Ÿ���� ����
	int m_CIDPauseTime;//CID-R�̻� �Ǵ� Link TimeOut���� OCS�����ϰ� pause�������� ��Ÿ���� ����

	bool m_CID_COMMON_bit_off_need;

	// CID ���� ���� �߰�, Kunjin, 20161030   /////////////////////////
	bool m_bCIDNotUse; // CID ���� ���� (Select On ���� ���� �Ǵܿ�)
	double m_dwCIDSelectOnTime;
	int m_CID_Offet_Target;  // ���� Tag���� �� Fiber������ Offset
	bool m_bLinkFail;
	bool m_bLinkDisconnected;
	bool m_bInitialLink;
	CID_DIRECTION m_CIDDirection;
	int m_tmpCurrentResetNode;
	int m_tmpCurrentStopNode;
	
	unsigned char m_Diag_CID_R_ID[9][6];//CID-R ���� ���� �� ó�� ������,  0~7: ���� ������, 8: Not found data 0
	int m_Diag_ResetNode[9];//CID-R ���� ���� �� ó�� ������
	int m_Diag_StopNode[9];//CID-R ���� ���� �� ó�� ������
	unsigned char m_Diag_CID_index;//CID-R ���� ���� �� ó�� ������

	int m_nCIDTerminateCount;//��� �Ϸ� ��ȣ ������ ���� ��� 
	unsigned char m_CIDOperationStarted;
	unsigned char m_job_change_flag_no_cid_area;
	unsigned char m_job_change_flag_exception_CommonTag; //�۾� ���� ��, common tag �Ϸ�ó�� exception flag 
	int m_CurrCIDStartnode;  //���� CID ������ ������ ����� Start node
	unsigned char m_sCIDStatus[3];
	int m_ntempNodeOffset;
		
//	int m_CID_Offet_Target_Left;
//	int m_CID_Offet_Target_Right;
	/////////////////////////////////////////////////////////
	
	CID_CTRL_INFO Info;
	CID_4WAY_INFO CID_4WAY_CONTROL_INFO;
	int m_nMTLTakeOutPIOStep;
	int m_nMTLTakeInPIOStep;
	double m_RunOffsetDistance;//�����Ÿ�
	int m_nPrePathType;

	AnsiString m_strOhtName;	// OHT NAME - ex) "OHT001"
	//int m_nCidControlStep;
	int m_nCidManualStep;
	bool m_bJobChange;
	bool m_job_change_without_link_complete;

	int     bCountOfCID_Delay;			// CID Info - ID 124 ������ ���� delay�� ���� ����
	int     bCountOfCIDLink;        //��ũ ���� ����
	int     bCountOfCIDDualOccup;  	//�������� ����
	int     bCountOfCIDComptACK;  	//�Ϸ�ó�� ���� ����

	/*
	������ ���� �����㰡 ��û����
	���� Lifter In Node ���� �� (Lifter Pre Node ����) VHL_ELEVETOR_PREPASS_STEP_COMPLETE �� �Ǹ�
	Lifter In �������ͽ� �ӽ��� Ÿ�� �ʰ� �ٷ� �����ܰ�� �Ѿ��

	VHL_ELEVETOR_PREPASS_STEP_NONE		 = 0,	// �����㰡 �������� ����
	VHL_ELEVETOR_PREPASS_STEP_TOUCH			,	// �������̾��� (�Ϸ������ ����)
	VHL_ELEVETOR_PREPASS_STEP_COMPLETE		,	// �����㰡 �Ϸ�. RIDE���� ���ʿ�. �ٷ� ������ ��
*/
	int m_iLifterPrePass;
	bool m_IsLifterCheck;

	bool m_IsMTLCheck;
	int nEnforcedSelectOffNodeID; 

	int m_OverRunCount;  //OverRun Ƚ�� ī��Ʈ ����
//	int m_OverRunCount_E2313; // ERR_NOTMATCH_TAGLENGTH_FINALSTATION_TAG_MOVE_BACK_LIMIT OverRun Ƚ�� ī��Ʈ ����
//	int m_OverRunCount_E2314; // ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR   OverRun Ƚ�� ī��Ʈ ����
//	int m_OverRunCount_E250; // ERR_MARK_RSTOPSEN_OFF OverRun Ƚ�� ī��Ʈ ����
	int m_MoniCFNode;
	int m_MoniCFNode2;

	bool m_bFDCMarkFailFlag;
	bool m_bFDCEarlyPathFlag;
	bool m_bFDCLatePathFlag;
protected:
	virtual	EXECUTE_ACK	Stop(void);	///< ��� ����
	virtual	EXECUTE_ACK	Reset(void);	///< ���� ����
	virtual	EXECUTE_ACK	TurnOff(void);	///< �⵿ Off
	virtual	EXECUTE_ACK	TurnON(void);	///< �⵿ ON


public:
		DrivingControl();
		~DrivingControl(void);

private:
	void	init(void);
	void	stop();
	void    turnoff();
	double	getOptimalSpeed(
					double Distance, double SpeedIn, double AccelIn,
					double AccelOut, double SpeedOut, double UniformSpeedRatio
					);	// ���� ��� ��;


//------------------------------------------------------------------------------
//1	Execute���� Ȯ���ϴ� ������ �Լ�
//1	Return �ϱ� ���� �Լ�
//------------------------------------------------------------------------------
public:
	EXECUTE_POSITION_INFO	GetPositionInfo();
	int						GetMapData(List<EXECUTE_RESPONSE_MAPMAKING_INFO*>* pListOfMapData,
												volatile MAP_INFO_ON_ADDING_UNION *pMapInfoOnAdding);		///< MapMaking ������ �޾ƿ��� �Լ�

private:
	void		updateTagInfo();							///< Tag���� ������Ʈ

//------------------------------------------------------------------------------
public:
	UINT		CheckAbnormalOnWait(void);				///< ��� �� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
	UINT		CheckAlways(void);				///< �׽� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
	bool		DeleteCmd(void *pCmd);						///< �Ҵ� �� ����� �Ҹ���
	
//------------------------------------------------------------------------------

//1 --- SEMI WAY -----------------------------------


public:
	int		Manual_Mark(MANUAL_MARK_CMD_TYPE pType);				///< �ܺο��� ȣ�� �Ǵ� �Լ�: Mark ����
	int		Manual_Driving	(MANUAL_DRIVING_CMD_INFO Info);		///< �ܺο��� ȣ�� �Ǵ� �Լ�: Jog �̵� ����
	int		AutoTaeching_Driving	(MANUAL_DRIVING_CMD_INFO Info);
//	void	Manual_DrivingAbnormalCheck();
	int 	Manual_IdReader(DRIVING_MANUAL_CMD_MARK_VALUE MarkType
									, DRIVING_MANUAL_CMD_FRONT_ID_READER_VALUE CmdType
									, int* pnResultID, double* pdResultOffset);	///< ID Reader ���� ���
	int 	Manual_FrontObservation(DRIVING_MANUAL_CMD_FRONT_OBSERVATION_VALUE Level);	///< ���� ���� ���� ���

private:
	int 	manual_Mark(MARK_TYPE MarkType);

//------------------------------------------------------------------------------
//1 --- NEW WAY -----------------------------------

private:
	LogUnit		*pLogUnit;	///< �α� ���� ȿ���� ���̱� ���� ������ �ִ� ������ : ���� �̸����� �����͸� ã�� �۾��� �̽ǽ� 

public:
	DRIVING_PARAM_SET		*m_pParamSet;		///< �Ķ���� ����: ���� ���Ǵ� ��
	CID_Monitoring_PKT 		TEST_PKT_Monitor;

private:
	//2	�Ķ���� ����
	// DRIVING_PARAM_SET		*m_pParamSet;		///< �Ķ���� ����: ���� ���Ǵ� ��
	DRIVING_PARAM_SET		*m_pTmpParamSet;	///< �Ķ���� ����: ������Ʈ �� �� ���Ǵ� ��

	//2	����� �з� �� ���� (�ʱ�ȭ �� ���� ����)
	EXECUTE_DRIVING_INFO m_LastDrivingCmdInfo;
	CLASSIFIED_DRIVING_CMD_SET	m_ClassifiedCmd;	///< �׸� ���� �з��� ��� ( 1. Node  / 2. Steer / 3. ���� / 4. �ӷ� / 5. Path ����  / 6. ������ ���� )
	EXECUTE_QUICK_CMD			m_QuickCmd; 		///< ���� QuickCmd ����: ��� ���� / Pause / Resume / Reset  : ���� ��� �ִ� ������ ���� �ݿ� --> Quick Cmd ���濡 ���� Event ó�� �߰� �ʿ�.

	//2	���� ��� ����
	bool m_bOnMapMake;	///< MapMake ���� Flag
	bool m_bOnBackMove;	///< BackMove ���� Flag
	MAP_MAKER_TYPE m_bMapMakeType;	///< MapMake Type    //0x00 ��ü Map Maker, 0x01 : ���� MapMaker, 0x10: ������ MapMaker
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	m_ListOfTmpStationData;	///< ���� ��带 ������ ������ Station ������ �����ϴ� List
	List<EXECUTE_RESPONSE_MAPMAKING_INFO*>	m_ListOfMapData;		///< MapMake ����� �Է��ϴ� List : Execute Thread ���� �Լ��� ȣ���Ͽ� ������ ��: Manual Reset �� ���� ����

	//2	���� ���� (�ʱ�ȭ �� ���Ǻ� ����)
	DRIVING_INFO_SET		m_InfoSet;				///< ����  ���� ���� ����: �ʱ�ȭ �� ���� ����
	POSITION_STATUS			m_PreviousArrivalInfo;	///< ���� ���� ����: ���� �Ϸ� �� "����  ���� ���� ����"(m_InfoSet)���� Position.Mark ������ �޾ƿ�: Manual Reset �� ���� ����
	EXECUTE_POSITION_INFO	m_PositionInfo;			///< ����� ����: �� ���� ��: PreCheck ���� �� ����, �� ���� Mark �Ϸ� ��, �� Manual Mark �Ϸ� ��, �� Manual ���� ��� ������ �� ó��
	
public:	//2	 �ܺο��� ȣ�� �Ǵ� �Լ�: �Լ����� ������ �빮�ڷ� ǥ��
	EXECUTE_ACK InsertGeneralCmd(void *pDrivingInfo);	//EXECUTE_DRIVING_INFO
	UINT		ExecuteJob();

	int 	ExecuteManual(DRIVING_MANUAL_CMD_INFO Info);		///< �ܺο��� ȣ�� �Ǵ� �Լ�: Manual ��� ����

	//2 Parameter  ����
	void*	LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory);
	bool	RequestParamUpdate(void* pParameter);
	bool	IsParamUpdated();


	//2	�Ķ���� ���� �Լ�
	bool loadParameter(char* szFilePath, DRIVING_PARAM_SET* pParamSet);	///< �Ķ���� �ҷ����� �Լ�
	bool saveParameter(char* szFilePath, DRIVING_PARAM_SET* pParamSet);	///< �Ķ���� �����ϴ� �Լ�
	bool updateParameter();											///< �Ķ���� ������Ʈ �ϴ� �Լ�

	bool loadParameterRecovery(char* szFilePath, DRIVING_PARAM_SET* pParamSet);
	int  m_ParamChehckError;

private:											

	//2	�ʱ�ȭ �Լ�
	void initStatusSet();					///< ���� ������ �ʱ�ȭ �ϴ� �Լ�
	void initCommandSet();				///< ��� ������ �ʱ�ȭ �ϴ� �Լ�
	void initInfoSet();						///< ���� HW ���� ������ �ʱ�ȭ �ϴ� �Լ�
	void initPreviousArrivalInfo();	///< ���� ���� ������ �ʱ�ȭ �ϴ� �Լ�
	void setPreviousArrivalInfo(POSITION_STATUS PositionStatus);	///< ���� ���� ������ �����ϴ� �Լ�: Station ���� ��� �������� Path ������ �����ϴ� �Լ� 
	void setCompensationInfo(POSITION_STATUS PreviousArrivalInfo);	///< �Ÿ� ������ ����� '��� Station Offset'�� ������ : m_InfoSet.Position.Compensation.dPreArriveOffset

	//2	��� �з� �Լ�
	EXECUTE_ACK setStationOffset(void *pDrivingInfo);		///< ������� ���� Station Offset ������ ������
	EXECUTE_ACK makeCmdInfoLists(void *pDrivingInfo);		///< ������� ���� List ������ ����� ��
	EXECUTE_ACK makeCmdInfoListsBack(void *pDrivingInfo);		///< ������� ���� List ������ ����� ��
	bool isMapMake(void *pDrivingInfo);							///< ����� Map Make ���� Ȯ��
	bool isBackMove(void *pDrivingInfo);							///< ����� Map Make ���� Ȯ��
	bool clearCmdInfoLists();								///< List�� ������ ���� ������
	bool clearCmdInfoListsLessThan(int nNodeCount);	///< nNodeCount ������ List�� ������ ������
	bool clearCmdInfoListsLessAfter(CLASSIFIED_DRIVING_CHANGE TempDrivnigInfo);	///< nNodeCount ������ List�� ������ ������
	bool checkCmdInfoNodeList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, int CheckNodeID, CLASSIFIED_DRIVING_CHANGE* ChagneInfo);  ///< List �����κ��� Node��ġ Ȯ��
	bool clearDrivingCtrlInfoSet();						///< m_InfoSet�� ��� Flag�� �ʱ�ȭ ��
	// makeCmdInfoLists sub Function
	bool makeList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Node Info List ������ ����� ��
	bool makeList(List<DRIVING_CMD_INFO_OF_STEER*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Steer Info List ������ ����� ��
	bool makeList(List<DRIVING_CMD_INFO_OF_DIV*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Steer Info List ������ ����� ��
	bool makeList(List<DRIVING_CMD_INFO_OF_DIRECTION*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);		///< ������� ���� Direction Info List ������ ����� ��
	bool makeList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo, DRIVING_CMD_INFO_OF_TARGET TargetInfo);			///< ������� ���� Speed Info List ������ ����� ��
	bool makeList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Path Info List ������ ����� ��

	bool CancelList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< ������� ���� Node Info List ������ ������.
	bool CancelList(List<DRIVING_CMD_INFO_OF_STEER*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< ������� ���� Steer Info List ������ ������.
  	bool CancelList(List<DRIVING_CMD_INFO_OF_DIV*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< ������� ���� Steer Info List ������ ������.
	bool CancelList(List<DRIVING_CMD_INFO_OF_DIRECTION*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);		///< ������� ���� Direction Info List ������ ������.
	bool CancelList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< ������� ���� Speed Info List ������ ������.
	bool CancelList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, CLASSIFIED_DRIVING_CHANGE DrivingInfo);			///< ������� ���� Path Info List ������ ������.

	bool makeBackList(List<DRIVING_CMD_INFO_OF_NODE*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Node Info List ������ ����� ��
	bool makeBackList(List<DRIVING_CMD_INFO_OF_SPEED*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo, DRIVING_CMD_INFO_OF_TARGET TargetInfo);			///< ������� ���� Speed Info List ������ ����� ��
	bool makeBackList(List<DRIVING_CMD_INFO_OF_PATH*> *pList, DRIVING_CMD_INFO_OF_PATH DrivingInfo);			///< ������� ���� Path Info List ������ ����� ��


	//1	�ڡڡ� ExecuteJob ���� ó�� �Լ� �ڡڡ�

	int	checkError(DRIVING_INFO_SET *pInfoSet);	///<00. ���� Ȯ��

	UINT executeOpenPassBox	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	UINT executePreEvevetor(DEVICE_NODE_TYPE DirType);
	UINT executeRideEvevetor(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	UINT executeOffEvevetor(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);
	
	UINT executeGoInit		(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 1. �ʱ� ����
	UINT executeGoPrepare	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 2. ���� �غ�: ��, ����, ����, ID
	UINT executeGoFirstNode	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 3. ����: ��, ����, ID
	UINT executeGoRun			(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 3. ����: ��, ����, ID
	UINT executeGoMark			(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 4. ��ũ: ��, ����, ID
	UINT executeGoCheckStop	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 5. ���� Ȯ��: ��, ����, ID 
	UINT executeGoComplete	(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet, UINT *puStep);	// 6. �Ϸ�: ��, ����, ID


	UINT checkPosition					(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);		///< ��ġ ���� Ȯ�� �� ������ Classified List ����
	UINT checkSteer						(DRIVING_INFO_SET *pInfoSet);	///< ���� ���� ���� �� ��� ����
	UINT checkFrontObservation		(DRIVING_INFO_SET *pInfoSet);	///< ���� ���� ���� ���� �� ��� ����
	UINT checkAxisFirstNode			(DRIVING_INFO_SET *pInfoSet);	///< �� ��� ����
	UINT checkAxisRun					(DRIVING_INFO_SET *pInfoSet);	///< �� ��� ����
	UINT checkAxisMark					(DRIVING_INFO_SET *pInfoSet);	///< �� ��� ����
	UINT checkOBSAreaforNoFollowControl (DRIVING_INFO_SET *pInfoSet);
	UINT checkServoTorqueFlag			(DRIVING_INFO_SET *pInfoSet);  /// ������ũ Ȯ��

	UINT checMTLTakeOut			(DRIVING_HW_INFO_SET HwInfo);  /// ���� Ȯ��
	UINT checMTLTakeIn			(DRIVING_HW_INFO_SET HwInfo);  /// ���� Ȯ��

	UINT checMTLTakeOut_PIO8			(DRIVING_HW_INFO_SET HwInfo);  /// ���� Ȯ��
	UINT checMTLTakeIn_PIO8			(DRIVING_HW_INFO_SET HwInfo);  /// ���� Ȯ��
	UINT checkJobChange          (DRIVING_INFO_SET *pInfoSet);

	void CID_LR_ON(bool LightGuideDirectionIsLeft);
	int CID_Comm_Time2Dist(CID_CTRL_INFO* Info);
	int controlCID(CID_CTRL_INFO* Info, executeCID_CMD iCmd, DRIVING_INFO_SET *pInfoSet, DRIVING_HW_INFO_SET HwInfo);
	
	int completeCIDComm(CID_CTRL_INFO* Info, DRIVING_INFO_SET *pInfoSet);
	bool checkCommandTarget4CID(CID_CTRL_INFO* Info);

	bool IO_INFO_CID(void);
	void IO_INFO_CID_ALL(void);
	UINT Manual_CID(int iManualCtrl);

	CID_DIRECTION CID_Direction_Chk(CID_CTRL_INFO* Info,  DRIVING_INFO_SET *pInfoSet, int CID_NearestResetNode, int  CID_NearestStopNode);
	
	UINT setCID_VHL_Name(char* OHTNAME);
	UINT requestCID_VHL_Name();
	
	int checkCID(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);
		
	bool NormalCid_Return(CID_CTRL_INFO* Info);
	bool CommonCid_Return(CID_CTRL_INFO* Info, DRIVING_INFO_SET *pInfoSet);

	unsigned char CID_ID_FIND(unsigned char* CID_UniqueID);
	void CID_ID_Buffer_Clear(unsigned char index);
	void CID_Monitor_Status(CID_CTRL_INFO* Info);
	void CID_R_Status_Chk();

	void CID_N_BRANCH_SELECT_OFF();

	bool MarkFail_Recovery_Driving();
	bool m_bIsRecovery;
		
	bool bDirCIDChage;		// ������ȯ �ϷῩ�� (true : ������. false : �Ϸ���)
	int tmpStopID;

	UINT checkBackPosition					(DRIVING_HW_INFO_SET HwInfo, DRIVING_INFO_SET *pInfoSet);		///< ��ġ ���� Ȯ�� �� ������ Classified List ����
	UINT checkBackSteer						(DRIVING_INFO_SET *pInfoSet);	///< ���� ���� ���� �� ��� ����
	UINT checkAxisBackRun					(DRIVING_INFO_SET *pInfoSet);	///< �� ��� ����
	void measureDist	(DRIVING_INFO_SET InfoSet, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfTmpStationData, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfMapData);	///< MapMake �� ��� �Ǵ� �Լ� 

	EXECUTE_RESPONSE_MAPMAKING_INFO* measureNodeDist				(DRIVING_INFO_SET InfoSet, UINT *puCurrNID, double *pdCurrNodeMarkPosition, bool *pbInit);			///< Node MapMake �� ��� �Ǵ� �Լ� 
	EXECUTE_RESPONSE_MAPMAKING_INFO* measureTagStatationDist	(DRIVING_INFO_SET InfoSet, UINT uCurrNID, double dCurrNodeMarkPosition, bool bInit);				///< Tag Station MapMake �� ��� �Ǵ� �Լ� 
	EXECUTE_RESPONSE_MAPMAKING_INFO* measureQRStatationDist	(bool bIsLeft, DRIVING_INFO_SET InfoSet, UINT uCurrNID, double dCurrNodeMarkPosition, bool bInit);	///< QR Station MapMake �� ��� �Ǵ� �Լ� 
	void insertStationInfo(EXECUTE_RESPONSE_MAPMAKING_INFO NodeInfo, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfTmpStationData, List<EXECUTE_RESPONSE_MAPMAKING_INFO*> *pListOfMapData);	///< Node ���� �� ���� ���� Station ������ �����Ͽ� List �� ����

	//2	 H/W ���� ���
	DRIVING_HW_INFO_SET	getHWInfo();
	
	int controlAll(DRIVING_CTRL_INFO_SET CtrlInfo);			///< Axis, Steer, Front
	
	int	controlAxis(CTRL_INFO_OF_SYNC_AXIS_MOVE AxisInfo);		///< �� ���� ���	// 2�� ���� ���� ���� �Լ�: ���� ���� : void	controlAxis(CTRL_INFO_OF_ASYNC_AXIS_MOVE AxisInfo); // 2�� ���� ���� ���� �Լ�loadParameter(char * szFullPath)
	int	controlSteer(CTRL_INFO_OF_STEER SteerInfo);				///< ���� ���� ���
	int	controlFrontObservation(CTRL_INFO_OF_FRONT_DETECT DirectionCmd);			///< ���� ���� ���� ���
	int controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType);		///< ID Reader ���� ���
	
	int	controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType, int* pnResultID);		///< ID Reader ���� ���
	int	controlIdReader(MARK_TYPE MarkType, CTRL_INFO_OF_ID_READER_CMD_TYPE CmdType, int* pnResultID, double* pdResultOffset);		///< ID Reader ���� ���

	//2	Mark ���� ���
	int markOnNode			(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID, int nNodeCount
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);
		
	int markOnTagStation	(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID 
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID,bool bOverrunIngn);

	int markOnTagStationBack(				DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);


	int markOnQRStation	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);

	int markOnQRStationDiffType	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID);

	int markOnQRStationDiffType2	(bool bIsLeft, 	DRIVING_HW_INFO_SET HwInfo, double dRealDistSumToTarget, double dSpeedLimit, int nTargetID
		, DRIVING_MARK_STATE* pMarkState, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo, CTRL_INFO_OF_ID_READER_CMD_TYPE *pIdReaderCmdType, int* pnID, bool bOverrunIngn, double dSTBOffset = 0);

	//2	���� �غ� / ������
	bool checkReadyToRun	(DRIVING_HW_INFO_SET HwInfo, USER_DETECT_INFO DetectCtrl
		, CTRL_INFO_OF_SYNC_AXIS_MOVE* pAxisCmd, CTRL_INFO_OF_STEER* pSteerCmd, CTRL_INFO_OF_FRONT_DETECT* pDetectCtrl, bool bIsManual=false);	// ������ ���� ������ ���� ���� Ȯ�� ( ��: ���� On )

	bool checkCompletion	(DRIVING_HW_INFO_SET HwInfo
		, CTRL_INFO_OF_SYNC_AXIS_MOVE* pAxisCmd, CTRL_INFO_OF_STEER* pSteerCmd, CTRL_INFO_OF_FRONT_DETECT* pDetectCtrl); // ���� �Ϸ� ���� Ȯ�� ( ��: ���� Off )


	//2	Steer Command Maker
	CTRL_INFO_OF_STEER setSteerCmdToCurrPosition(DRIVING_HW_INFO_SET	HwInfo);

	//2	ETC Function
	STEER_HW_INFO_POSITION getSteerPosition(bool bLeft, bool bRight);		///< HW I/O ������ Steer ��ġ ������ ��ȯ
	STEER_HW_INFO_POSITION getSteerTotalPosition(bool bFrontLeftOn, bool bFrontRightOn, bool bRearLeftOn, bool bRearRightOn);
	DRIVING_CMD_INFO_OF_NODE* getNodeInfo(int nNodeCount);	///< Node Count�� �ش��ϴ� Node ���� ������ ��ȯ
	DRIVING_CMD_INFO_OF_PATH* getPathInfo(int nNodeCount);	///< Node Count�� �ش��ϴ� Path ���� ������ ��ȯ

	double compensate(double dRealPos);			///< �Ÿ� ���� ( ���� ���ڴ� ��ġ �� Map ���� ��ġ ������ ��ȯ )
	double getRealPos(double dCompensatedPos);	///< Real ��ġ�� ��ȯ ( Map ���� ��ġ �� ���� ���ڴ� ��ġ�� ��ȯ )

//	double getSpeedDownPos();	///< ���� �����ϴ� ��ġ Return
	double getSpeedDownPos(double dAccel, double dSpeed, double dDist);	///< ���� �����ϴ� ��ġ Return
	bool isMarkArea();													///< Mark �����ϴ� Real ��ġ Return
 	bool getSteerCmd(int nNodeCount, STEER_HW_INFO_CMD *pSteerCmd);	///< �Է��ϴ� Node Count ������ Path�鿡�� ���� ���� ���� ���ϴ� �Լ�: Ư���� ������ false Return
 	bool getSteerCmdOnNBranch(STEER_HW_INFO_CMD *pSteerCmd);		///< N�б⿡�� ���̵忡 ���� ���� ���� �� ���� ���� ���ϴ� �Լ�: Ư���� ������ false Return
	bool getSteerCmdTypeFrom(PATH_STEERING_INFO PathSteerInfo, STEER_HW_INFO_CMD *pSteerCmd);	///< Path �������� Cmd ����� �޾Ƴ� �� ���
	bool getSteerCmdTypeFrom(PATH_STEERING_INFO PathSteerInfo, STEER_HW_INFO_CMD *pSteerCmd, STEER_HW_INFO_CMD *pSteerKey);	///< Path �������� Cmd ����� �޾Ƴ� �� ���
	bool isSteerChangeAreaByGuide();	///< N�б⿡�� ���̵� ���Ͽ� ���� ������ �ٲ�� ���� ���� �ִ����� Ȯ���ϴ� �Լ�
	bool isSteerChangeAreaByGuideNodeCountAdd();	///< N�б⿡�� ���̵� ���Ͽ� ���� ������ �ٲ�� ���� ���� �ִ����� Ȯ���ϴ� �Լ�
	bool getFrontObservation(int nNodeCount, USER_DETECT_INFO* pDetectInfo);	///< �Է��ϴ� Node Count ������ Path�鿡�� ���� ���� ���� ���� ���� ���ϴ� �Լ�: Ư���� ������ false Return
	bool getFrontObservationNodeCount(int nNodeCount);	///< �Է��ϴ� Node Count ������ Path�鿡�� ���� ���� ���� ���� ���� ���ϴ� �Լ�: Ư���� ������ false Return

	bool getDivergenceInfo(int nNodeCount, PATH_DIVERGENCE_INFO* pDivInfo, PATH_DIVERGENCE_INFO* pPreDivInfom, int *NodeCount);	///< �Է��ϴ� Node Count ������ Path�鿡�� ���� ���� ���� ���� ���� ���ϴ� �Լ�: Ư���� ������ false Return


	bool getAxisCmdInfoBySpeed(int nNodeCount, double *pdSpeed, int *pnStopNodeCount);	///< �ӵ��� ���� ���� ���� ȹ��
	bool getAxisCmdInfoBySteer(int nNodeCount, STEER_HW_INFO_POSITION CurTotalSteerPos, int *pnStopNodeCount1, int *pnStopNodeCount2, STEER_HW_INFO_POSITION RealCurTotalSteerPos);		///< ���⿡ ���� ���� ���� ȹ��
	bool getAxisCmdInfo(DRIVING_INFO_SET *pInfoSet, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo);	///< ���� ���� ������ �޴� �Լ�
	bool getAxisCmdInfoBack(DRIVING_INFO_SET *pInfoSet, CTRL_INFO_OF_SYNC_AXIS_MOVE* pCtrlInfo);	///< ���� ���� ������ �޴� �Լ�
	double getDist(int nFromNodeCount, int nToNodeCount);	///< nFromNodeCount ~ nToNodeCount �� �Ÿ��� ���Ѵ�.
	double	getSmoothSpeed(double dDecel, double dDist, DWORD dwTime);///< ���� ������ Normal Path�� �ε巴�� �����ϱ� ���� �ӵ�
	double getMaxSpeed(double dAccel, double dDecel, double dDist, double dCurSpeed = 0.0, double dTargetSpeed = 0.0);	///< ������ �Ÿ� ���ǿ����� �ִ� �ӵ��� ���Ѵ�.
	double	getMarkSpeedLimitOnGoing(int nCurrNodeCount);	///< Going �߿� Path �ӵ��� �ݿ��Ͽ�, Mark �ӵ� Limit�� �����ִ� �Լ�

	double getAccelDivisor(int nStartNodeCount);	///< ���� NodeCounter�� �Է��ϸ� ���� ���� ������ ���´�
	double getDecelDivisor(int nStartNodeCount);	///< ���� NodeCounter�� �Է��ϸ� ���� ���� ������ ���´�

	bool getMarkStatus();
	void setMarkStatusFalse();	///< ��ũ �ƴ� ���·� ���� ��Ű�� �Լ�
	void setMarkStatusFalse_AutoTeahcing();	///< ��ũ �ƴ� ���·� ���� ��Ű�� �Լ�

	void getPostionInfoOnRunning(DRIVING_INFO_SET InfoSet, EXECUTE_POSITION_INFO *pCurrPositionInfo);
	void getPostionInfoOnMarked(DRIVING_INFO_SET InfoSet, EXECUTE_POSITION_INFO *pCurrPositionInfo);
	void getPostionInfoOnManual(DRIVING_HW_INFO_SET HwInfo, EXECUTE_POSITION_INFO *pCurrPositionInfo);

	void checkStartCmd(EXECUTE_DRIVING_INFO *pTmpDrivingInfo);	///< ��ũ ���°� Station �ε�, ���� ����� Next Node ���� ����� �ƴ� ��� ó��
	EXECUTE_ACK checkStartCmdStandard(EXECUTE_DRIVING_INFO *pTmpDrivingInfo);	///< ��ũ ���°� Station �ε�, ���� ����� Next Node ���� ����� �ƴ� ��� ó��
	int Set_Node_XorY(EXECUTE_RESPONSE_MAPMAKING_INFO *pMapMakeNodeInfo, UINT nStNode, UINT nEdNode, UINT nDist, bool bSkip);

	int AutoRecovery_Driving(int Error_AV);

	// VHL ���������� ���� ���� �߰� �Լ� ����
	// bool mbVHL_Lifter_Ride;
	// bool mbVHL_Lifter_Off;
	BYTE cRunNextDivInfo;
	UINT nRunNextNodeID;
	double m_dStationMarkInpos;

	bool   m_bFirstNodeStep;
	int    m_dPreNodecount;
	double m_dPreNodecountPosition;
	double m_dPreNodecountSpeed;
	double m_dPrePrecheckNodecountPosition;
	double m_dCheckPrecheckNodecountPosition;


	//AMC<-> Main ���ӵ� ����ȭ ó��. puting 20191023
	double m_dCheckAMCDecelPosition;   ///<���ӵ� ���濡 ���� �̵��ϴ� ���� ��ġ
	double m_dPreTargetPosition;       ///<���� �� �����ϴ� Ÿ�� ��ġ ����.

	//���� ���� Puting 20160811
	BYTE m_cSlopeNodeInfo;  //0xFF : �ʱ�ȭ , 0x00 �Ϲ� ���� : 0x01 : ����������
	//AnsiString CheckSum_StringToHex(AnsiString sData);
	//AnsiString CheckSum(char* buf, int len);
	AnsiString CheckSum(char* buf, int len, char* check);
	bool Lifter_Prenode_Check(UINT iNodeNum);
	bool bReqLifterCounterReset;

	UINT m_nMarkFailRetryCheckID;

	bool m_bIsFirstNodeCountAdd;
	bool m_bIsAMCVHLDetectIgnore; //���� ���� �÷��� ��� ���� ���� �÷���(���� ���� �� �ʱ�ȭ �κп��� �ʱ�ȭ ó��)

	bool b_ReadBcr;  //BCR READ ���� Check
	bool b_WaitTrg;  //BCR Data�� �����߾����� Check

	//����ȭ ó�� ���� �Լ�
	void DrivingCMDExuteFunc(bool bFlag);

public:
	void getCID_Info(CID_CTRL_INFO* Info, int nCurrentNodeID);

	int VHL_OPC_RESET();
	int SEND_SERIAL(char* cProtocol, int iSize);
	int SEND_SERIAL_NOT_CLEAR(char* cProtocol, int iSize);
	int SendLowSerial(char* cTemp);

	int VHL_OPC_RESET_RESPONSE();
	void VHL_OPC_RESET_RESPONSE_SIMUL(OPC_INIT_PKT RECV_PKT);
	int VHL_OPC_PASS_REQUEST (char* OHTNAME, unsigned char Priority, unsigned char Turn, unsigned char LightGuideID);
	int  GetCID_ELEVETORMODE_VALUE( );

	bool Check_Elevetor_LG1_START_AREA_Tag();
	bool Check_Elevetor_LG1_WAIT_AREA_Tag();
	bool Check_Elevetor_LG2_INSIDE_WAIT_AREA_Tag();
	bool MotionOnOff(bool bOnOff);

	int VHL_OPC_PASS_RESPONSE(unsigned char LightGuideID);
	void CID_IO_SETTING_LIFTER(bool LightGuideDirectionIsLeft);
	int CID_SELECT_ALL_OFF();
	int CID_OPERATION_MODE_LIFTER();
	int CID_SELECT_DIRECTION_LEFT();

	bool CID_Check_Mode_Lifter(bool LightGuideDirectionIsLeft);

	int CID_OPERATION_MODE_CID();
	int CID_SELECT_DIRECTION_RIGHT();
	int CID_Find_Stop_Reset(int callNum, DRIVING_HW_INFO_SET HwInfo, int* StopNode, int* ResetNode);
	int CID_Find_Stop_Reset_Change(int callNum, DRIVING_HW_INFO_SET HwInfo, int* StopNode, int* ResetNode, int Num);

	int ModeChg_CALL();
	int ModeChg_UP();
	int ModeChg_DOWN();

	void ADD_CID_LOG(AnsiString strLog);

//public:
	void SetOhtName(AnsiString strName);

	int m_nCidControlStep;
	int m_Continue_CID_MODE;//0: init, 1:CID ���� ����
	int m_Continue_CID_MODE_StopNode;
	int m_Continue_CID_MODE_ResetNode;
	int m_Continue_CID_MODE_ResetNodeType;

	//�����۾� ���� �Լ�(Shutter, EQ PIO) puting
	bool m_bShutterCloseFlag;     //false Ȯ�� ���ʿ�, true CLose Ȯ�� �ʿ�    //�����ð� ������ �ʱ�ȭ.
	bool m_bShutterOpenFlag;     //false Ȯ�� ���ʿ�, true CLose Ȯ�� �ʿ�    //��߽� �ʱ�ȭ

	bool m_bShutterCloseMovingFlag;     //Inposition Ȯ�� �� Shutter Close ó��. //�����ð� ������ �ʱ�ȭ.

	bool IsInnerFoup();   //���� ���� Ȯ�� �÷���
	int CheckShutterAlarm();

	int CheckShutterClose(double dSpeedOfNow);  //�����ð� �����ҋ����� ���Ͱ� close �ȵǸ� ����.. Shutter Close ����� Trans���� �߻���Ű��, Check�� ���࿡�� Ȯ��.
								//��������Ŭ���� Station�������� Target �Ÿ��� 400mm �̻��� ��츸
								//��� üũ�� �����ϰ� ��üũ ������ ��� üũ �Ұ�.

	int ExeCmdShutterClose(double dSpeedOfNow);    //��߽� ������ ��� �ѹ��� ��.(��߽�, ��������Ŭ(Load)������� ����� ��(������ �����) ����)  Target �Ÿ��� 400mm �̸��� ���� ����)
	int ExeCmdShutterOpen(double dSpeedOfNow);     //Shutter Open ���� ���.(��������Ŭ(Load)  ������� ���� �Ҷ�, Mark �ܰ迡�� ������ ��� ��. Target�Ÿ��� 400mm ���� ��쿡 )
								  //��� üũ�� �����ϰ� ��üũ ������ ��� üũ �Ұ�.

	int ExeCmdEQPIOClose();     //��߽� ������ ��� �ѹ��� ��.(PIO Disable �� )
	int ExeCmdEQPIOOpen(int nPIOType, int nPIOID, int nPIOChannel,int nPIOCS);      //EQPIO Open ���� ����(RF�� ��츸)(���� ����Ŭ, �ö�����Ŭ���� Port Type�� EQ �ΰ�츸 ����������, Mark �ܰ迡��)

	void Reload_QR_Data();
	void Reset_DrivingControl();

	double GetDefaultDecel();
	double GetDefaultAccel();

	void CID_Save_Start_Stop_Reset(CID_CTRL_INFO* Info);
	bool CID_Compare_List(DRIVING_HW_INFO_SET HwInfo);
	int CID_Save_Node[6];

	static double calcMaxSpeedDynamic(double velIn, const double* dynamicVel, const double* dynamicAcc, const int numDynamicArea, const double decel, const double constTime, double dist);
	DWORD CheckReadyToRunTime_ExcuteGo;
	DWORD CheckReadyToRunTime_ManualMark;
	DWORD CheckComplete_ExcuteGo;
	DWORD CheckComplete_ManualMark;
};


#endif	// DrivingControl

