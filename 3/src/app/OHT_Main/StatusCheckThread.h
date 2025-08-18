/**
@file    StatusCheckThread.h
@version OHT 7.0
@brief   StatusCheckThread Header File
*/

#ifndef StatusCheckThreadH
#define StatusCheckThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "ThreadCtrl.h"
#include "HWSet.h"
#include "TransControl.h"
#include "OHTMainStatus.h"
#include "ExecuteInfo.h"
#include "CPUandMemInfo.h"
#include "DiagManager.h"

#include "HID_VII_ZR.h"
#include "IPCLoadLib.h"
#include "CPUandMemInfo.h"
#include "NetworkSignallib.h"
//#include "OHTSysInfo.h"
#include "UdpClient.h"
#include "Def_DefaultIniParam.h"
#include "Def_MCCParam.h"
#include "SNMP.h"
#include "LCD.h"
#include "Obstacle.h"
#include "Lookdown.h"
//---------------------------------------------------------------------------

typedef enum    SNMP_PARSING_RESULT_
{
    SNMP_NO_ERR = 0,
    SNMP_SEND_ERR,
    SNMP_RCV_ERR,
    SNMP_RSSI_NULL_ERR,
    SNMP_MAC_NULL_ERR,
    SNMP_SSID_NULL_ERR,
    SNMP_TYPE_ERR
}SNMP_PARSING_RESULT;

#define LCD_NEW_VERSION 0
#define LCD_PRE_VERSION 1

#define WirelessClientStatusPort 161

class TaskControl;

/**
@class   StatusCheckThread
@date    2012.11.15
@brief   StatusCheckThread Ŭ����
*/
class StatusCheckThread : public ThreadCtrl 
{
private:

	LogUnit *pLogExtraJobThread;      ///< ExtraJobThread �α� ������ ���� ����
	LogUnit *pLogUser; ///< User ������ ���� ���� ����
	LogUnit *pLogErrHID;  ///< HID Error ������ ���� ���� ����
	LogUnit *pLogCPUandMemInfo; ///< CPUandMemInfo ���� �α� ����
	LogUnit *pLogWirelessNet; ///< WirelessNet ��� �α� ����
	LogUnit *pLogMCC;

	FDCUnit *pFDCUnit;

	HWSet *m_pHWSet; ///< HWSet Class ����

    // by zzang9un 2013.08.18 : Use ������ ���� �߰�
	TransControl *m_pTransControl; ///< TransConrol Class ����
	TaskControl *m_pTaskControl;

    // by zzang9un 2013.09.02 : Sound ��� ���θ� �Ǵ��ϱ� ���� ����
    bool m_bPlayingSound;
	int m_PlaySoundNum;
	bool m_bRSSICheck;

	UINT nCommfailCount;


	HID_VII_ZR *m_HID;
	IPCLibrary *m_IPCLoadLib;
	NetworkSinalLib *m_NetworkDll;
	CPUandMemInfo *m_CPUandMemInfo;

    PassPermit * m_PassPermit;

	bool m_CheckLampYF;
	bool m_TPESTOP;
	String GetStatusToString(const BYTE status);

    /// OHT Main Status ����(MainThread�� StatusThread ����)
    OHTMainStatus *m_OHTMainStatus;

    /// Execute Info ����(StatusThread�� ExecuteThread ����)
    EXECUTE_INFO *m_ExecuteInfo;

	DiagManager* m_DiagManager;

	//MONITOR_DATA_INFO *m_MonitorDataArray;  ///< HID ���� �迭 ������
	//HID_NODE_INFO *m_NODEArray; ///< NOD ���� �迭 ������

	//void InitSTRCUTInfo();
	//void CloseSTRCUTinfo();
	DEFAULT_PARAM_SET *m_defualtparam;
	DEFAULT_LINE_PARAM_SET *m_LineParamSet;   //AOHC-285
	DEFAULT_AUTOPATCH_PARAM_SET *m_pAutoPatchParamSet;  //AOHC-154
	MCC_STATUS_PARAM_SET	m_mccparam;
    // Wireless Client IP �ּ� (�ܸ��� �Ķ���� �ٸ�)
    char szWirelessClientIP[20];
    UdpClient *m_UdpClient;
	int Get_WLanInfo_FXE_MODEL(char* chAddr, APSIGNAL_DATA* APSignalData);

	//LCD ���� ����

	LCD *m_pLCD;
	bool bOpenLCD;
	int ComOpenForLCD();

	int m_DistanceSensorType;
	int m_OCSAPSignal;     //AOHC-292

	// Cycle Over Check 2018.12.04 SHCHO
	// StatusCheck Thread���� Ȥ�ö�, ��ɸ��� �κ��� �ִ��� üũ�ϱ� ����
	void CycleOverCheck(int nEventNo, DWORD nChkTime);
	void CycleMonitoring(int nEventNo, DWORD nChkTime);
	int m_MoniVANode;
	int m_MoniVDAUNode;
	int m_MoniVBNode;
	int m_MoniVDBUNode;
	int m_MoniVCNode;
	int m_MoniVDCUNode;
	int m_MoniAPNode;

	int m_nResetworkingCount;
	DWORD m_dwResetTime;
    bool ResetWorkingSetSize(SIZE_T dwMemorySize);   //���� Flag ó���ؾߵ� �κ�
	LCD_DATA_FLAG m_lcd_flag;
	int m_nLCDRetry;

	DWORD m_dwSOSLABFrameTime;

	float  	m_fAxis1Torque;
	float  	m_fAxis2Torque;

protected:
	void RealThreadProc(void);	// Execute �� ���� �Լ�

public:
	StatusCheckThread();
	~StatusCheckThread();

	void UpdateStandardStatus(); //puting

	// Status�� Update�ϴ� �Լ���


	void UpdateCommonStatus();
	void UpdateStatusForTP();
	void UpdateStatusForMonitoring();
	void UpdateDiagData();

	void ControlLamp();
	void ControlYellowLamp();
	void ControlSound();
	void ControlSound_ATLAS();

	int GetReadLCDData(bool OHTMainOff = false); //OHTMainOff ��� ó���ϱ� ���� �߰�
	int LCDFirmwareVersionCheck();

	int CheckHID();

	// by zzang9un 2013.09.13 : User�� ���� �α� ���� �Լ�
	void AddUserLog();

	void CheckTraceLog();

	int SaveHID_Log();
	APSIGNAL_DATA HandyTP_AP_Info;

	void SetDistanceSensorType(int cmd);
	int  GetDistanceSensorType();
	void MCCStatusData();

	bool bUseAVS;
	void SaveThreadTime(DWORD max, char* name);
	void AMCCommfail();

	bool SetAMCAutoPatchErrorCheck(bool ErrorCheck);
	bool m_bAMCAutoPatchErrorCheck;

	bool bRegulatorCheck;
	int m_nLCDVersion;

	void UBGDetectTimeCheck(int nOHTDetectArea, int nOHTDetectLevel, int nTmpnOHTDetectLevel);

	int SendLCDDataToNewVersion();
	int SendLCDDataToPreVersion();
};
//---------------------------------------------------------------------------
#endif
