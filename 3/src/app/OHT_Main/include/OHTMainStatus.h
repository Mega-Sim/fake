/**
@file    OHTMainStatus.h
@version OHT 7.0
@brief   ���� ����ü ���� Header File
*/

#ifndef OHTMainStatusH
#define OHTMainStatusH

#include "../../Common/Def_DataType.h"
#include "../Common/Protocol_Define_Modify.h"

// by zzang9un 2013.04.10 : ���������� ���� Critical Section
extern TCriticalSection *CS_CmdList; ///< CmdList�� ���� Critical Section ����
extern TCriticalSection* CS_TaskList; ///< CmdList�� ���� Critical Section ����
extern TCriticalSection* CS_CycleList; ///< CmdList�� ���� Critical Section ����
extern TCriticalSection* CS_ResponceList; ///< CmdList�� ���� Critical Section ����
extern TCriticalSection *CS_PassOpenList; ///< CmdList�� ���� Critical Section ����
extern TCriticalSection *CS_OHTMainStatus; ///< OHTMainStatus�� ���� Critical Section ����

/**
@brief   Quick Command ���� Enum ����
@author  zzang9un
@date    2013.04.10
*/
typedef enum QUICK_CMD_
{
    QUICK_CMD_NONE      	= 0,    ///< Command None ����
    QUICK_CMD_STOP      	= 1,    ///< E-Stop
    QUICK_CMD_PAUSE_OCS		= 2,
    QUICK_CMD_RESUME_OCS   	= 3,    
    QUICK_CMD_CANCEL    	= 4,    ///< ��� Cancel
    QUICK_CMD_IDRESET   	= 5,    ///< Reset ���
	QUICK_CMD_PAUSE_TP		= 6,
	QUICK_CMD_RESUME_TP    	= 7,
	QUICK_CMD_TURN_OFF		= 8,		///< �⵿ Off
	QUICK_CMD_TURN_ON		= 9
} QUICK_CMD;


/**
@brief   Data Update Check Flag
@author  doori.shin
@date    2013.08.07
*/
typedef struct DATA_USE_FLAG_
{
	bool fStation;
	bool fTeaching;
	bool fMap;
	
	bool fSwapping;
	bool fReadySwapping;
	bool fMapsetLoad;  //1: Mapset ���� �ε� �Ϸ�, 0: �� �ε� �ȵ�
	unsigned char fUBGLoad; //0:false(UBG fail), 1:true(UBG Pass) 0xFF:Init


	bool fSwappingParam;
	bool fReadySwappingParam;

}DATA_USE_FLAG;

/**
@brief   Job Chang Flag
@author  doori.shin
@date    2013.08.07
*/
typedef struct JOB_CHANGE_FLAG_
{
	bool bCycleingChangeFlag;
	int StartStationID;
	double dChangDistance;

}JOB_CHANGE_FLAG;


/**
@brief   Data ������ ���� Flag
@author  zzang9un
@date    2013.07.23
*/
typedef struct MAPMAKE_STATUS_
{
	// MapMake ���� Flag
	bool bReady;        ///< Map Make �غ� Flag
	bool bStart;        ///< Map Make ���� Flag
    bool bComplete;     ///< Map Make ���� Flag

    // Map ���� Flag
    bool bStartBackup_Map;   ///< Map Data Backup�� ���� flag
	bool bMapChanged;        ///< Map Data�� ����� ���

    // Station Data ���� Flag
    bool bStartBackup_Station;   ///< Station Data Backup�� ���� flag

    // Map Make �ΰ� ����
    UINT StartNodeID;           ///< Map Make ���� Node ID
    UINT EndNodeID;             ///< Map Make ���� Node ID
    int Speed_MapMake;          ///< Map Make ���� �ӵ�    
} MAPMAKE_STATUS;

/**
@brief   Map��������� ���� Flag
@author  puting
@date    2013.07.23
*/
typedef struct MAPMAKE_PROCESS_
{
	// MapMake ���� Flag
	bool bReady;        ///< Map Make �غ� Flag
	bool bStart;        ///< Map Make ���� Flag
    bool bComplete;     ///< Map Make ���� Flag

	// Map ���� Flag
	bool bMapChanged;        ///< Map Data�� ����� ���

    // Map Make �ΰ� ����
    UINT StartNodeID;           ///< Map Make ���� Node ID
    UINT EndNodeID;             ///< Map Make ���� Node ID
} MAPMAKE_PROCESS;

/**
@brief   Main Thread�� Status Thread�� ����� ���� ����ü
@author  zzang9un
@date    2012.10.18
*/
typedef struct OHTMainStatus_
{
    // by zzang9un 2013.03.21 : Mode, Status, CmdID ���� ����ü ������� ���� ���� ��� ����
    //BYTE Mode;
    //BYTE Status;              ///< ������ �����ϴ� Status: Execute �� ������ �� ���� ������Ʈ, ���� �߻� �� ������ ���� ����
    
	int ErrCode_StatusChk;  ///< StatusCheckThread�� ���� �ڵ�
	int InnerErrCode;  ///< StatusCheckThread�� ���� �ڵ�

    // ������ ���� ���� �߰�: ���¿� 
    bool IsOnGoCmd;             ///< Go ��� �ΰ� �� True, Go ����� ���� �� Status�� Arived�� �Ǹ� false�� ��ȯ
	UINT FinalNodeOfPathSearch; ///< ���� �� Go ��� �ΰ� �� �������� ���Ǵ� ���� ��. ���� ���� �ƴ� ���� ������ Node ���� �ݿ��Ѵ�.
	UINT FinalEndNodeOfPathSearch; ///< ���� �� Go ��� �ΰ� �� �������� ���Ǵ� ���� ��. ���� ���� �ƴ� ���� ������ Node ���� �ݿ��Ѵ�.
	UINT FinalStationOfPathSearch; ///< ���� �� Go ��� �ΰ� �� �������� ���Ǵ� ������ �����̼� ��ġ ����
	UINT FinalSeqNumOfPP; ///< ������ ������ �ѹ�


	//puting 2015.11.05
	 //----------------------------------------------------------------------------------------
	INNER_STATUS_OHT 	StandardStatusOHT; ///<OHT���� OCS���� ���� Status
	PROTOCOL_STATUS_OHT 	StandardOHTStatusReport;
	bool bOCSReportFlag;

	 //�۾����� �޾Ƽ� �������ִ� ������ �������ֱ�......
	 //Ŭ����� ���� �۾����ð� ���� ��� �۾�����ID �� 0���� cycle�� �������� �ʱ�ȭ.
    double dOHTTotalRunDis;
	BYTE StandardOHTProgress; 			///<���� OHT ��ô ����
	BYTE StandardOHTPreProgress; 			///<���� OHT ��ô ����
	bool bCheckRollbacking; ///< OHT �⵿ On/Off�� ���� Pause ���� ���� �������¿��� �⵿ off ���¿����� Pasuse �ƴ�
	OHT_TURN_OFF_STATUS TurnOffInfo;     ///< �⵿ Off�� ����� ����.

	DWORD CheckCommfailtime;

	unsigned short CurCmdID;
	BYTE CurCycle;
	UINT TargetStationNID;
	bool bRealGoCycleing;
	bool bCycleing;

	JOB_CHANGE_FLAG JobChangeFlag;   ///<�۾����� ������ ������ ����㰡�� �����ϱ� ���� �÷��� puting 20160307

	BYTE WaitCycle;
	UINT WaitTargetStationNID;

	bool bCheckCycleRunning;    //����Ŭ ���¸� Ȯ���ϴ� �κ� 
	bool bIsOHTStoped;                     //���� OHT �����Ȼ�������? ���������� Ȯ���ϴ� �÷���


    STATUS_NODE_CHECK_DATA CurNodeCheckData;  ///���� Node Count �� ��ġ ����.
  //	MARK_TYPE FinalMarkType;         ///< ������ Mark Type ����, ������ Go ����ΰ��� �������� ���Ǵ� ���� Type, ��������
//									///< 0: ����, 1: ���� Node, 2 :������ Node(QR, ������)
 //	UINT FinalStationDist;				///< �������� Station�� ��� Dist �Ÿ���


	//------------------------------------------------------------------------------------------


	// OCS, TP, �ڰ����� ���� ���� Ȯ�ο� ����

//	bool 			CID; ///< CID ���� �� ���
//	bool ConnectedOCS;        ///< OCS�� ���� ���� flag
//	bool ConnectedOCSSub;        ///< OCS�� ���� ���� flag
//	bool ConnectedTP;         ///< TP�� ���� ���� flag
//	bool IsConnectMonitor;    ///< �ڰ����� ���� ���� flag

	// by zzang9un 2013.04.10 : Main ���� Quick Cmd ����
    QUICK_CMD QuickCmd;     ///< ���� Quick Cmd

    int QuickCmdEStopCommander;
    int QuickCmdEStopType;
    
	OHT_COMMON_STATUS StatusCommon; ///< ���� Status

	MSG_STATUS_OHT2OCS StatusOCS; ///< OCS���� ���� OHT Status
	MSG_STATUS_OHT2TP  StatusTP;  ///< TP���� ���� OHT Status

	MSG_STATUS_OHT2VMS  StatusVMS;  ///< VMS���� ���� OHT Status
	MSG_STATUS_OHT2BLACKBOX  StatusBLACKBOX;  ///< BLACK BOX���� ���� OHT Status

	//2013.08.07 doori.shin OHT Data Update�� Ȯ���ϴ� �÷���
	DATA_USE_FLAG DataUseFlag;

	bool fMainReady; ///< Main�� Ready�������� Ȯ��

	MONITOR_DATA_INFO m_cpMonitorData; ///< Monitoring -> OHT Data ���� �迭�� ����Ű�� char�� ������
	//char *m_cpNODE; ///<  OHT -> Monitoring  ���� �迭�� ����Ű�� char�� ������

	bool fMemoryReset; ///< Reset ����
	SIZE_T nfMemorySize; ///< Reset ����
	DWORD dwLogThreadTime;

    // by zzang9un 2014.01.22 : Trace ������ �˸��� Flag
    bool bTraceStart;   ///< StatusCheckThread���� ���� ���� �� true�� �����ϰ� ExtraJobThread���� Trace����
	AnsiString strCIDMachineId;

	bool ConnectedIOTHub;
	bool bTourMode;
	BYTE CheckMaterialType;
    bool bIsHoistHomeCommand;

	int nStatusVMS;
	AnsiString strVMSBTId_1;
	AnsiString strVMSBTId_2;

} OHTMainStatus;


#endif  // OHTMainStatusH

