// ---------------------------------------------------------------------------
/**
@file    AMCDllLib.cpp
$Rev:: 2909          $:  Revision of last commit
$Author:: ks4423     $:  Author of last commit
$Date:: 2015-04-30 0#$:  Date of last commit
@version OHT 7.0
@brief   AMCDllLib Class �ҽ� File
*/
// ---------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "AMCDllLib.h"

#include "Def_OHT.h"
#include "OHTMain.h"
#include "NumericSystem.h" // 90���� ��ȯ

#define REG_READ_MODE	0x80	// bit7=1
#define REG_READ_DONE	0xFF

#define EXP_FPGA_VER					0x00 // RO
#define READ_RETRY_CNT					100

#define EXP_BASE_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F1))
#define EXP_LSB_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F2))
#define EXP_MSB_ADDR	((BYTE *)DeviceAMCGetPhysicalAddr(0x03F3))


// ---------------------------------------------------------------------------
#define	NO_ERR						0
#define	ERR_AMC_NOTOPEN				-1

#define MAX_IO_INT_COUNT			3

#define AXIS_DRIVING_F             	0
#define AXIS_DRIVING_R             	1
#define AXIS_HOIST                 	2
#define AXIS_SHIFT                 	3

#define AMC_DPRAM_ADDRESS			0xD0000
#define AMC_INTERRUPT				11 //PCM9377 == 9 , �ű� SOM4463 == 11

#define NAME_AMCPARA				"DefaultParam.ini"	// �̻��
#define PATH_AMCPARA				"DataFiles\\"
#define PATH_NAME_AMCPARA			"DataFiles\\DefaultParam.ini"   // �̻��	AMCPARAM.INI" //
#define MAX_LENGTH_OF_FULL_PATH		1024


#define MAX_BYTE_SIZE_OF_AMC_LOG	1024*100

// ---------------------------------------------------------------------------


/************************************************************
 *  Content : AMC Errorcode ==> axis_source���� �޴°�  *
 ************************************************************/
#define ST_NONE               0x0000 // ������
#define ST_HOME_SWITCH        0x0001 // ����(Home) ���� ����
#define ST_POS_LIMIT          0x0002 // + ���� Limit ���� ����
#define ST_NEG_LIMIT          0x0004 // - ���� Limit ���� ����
#define ST_AMP_FAULT          0x0008 // AMP Fault ���� �߻�
#define ST_A_LIMIT            0x0010 // ��, ���ӽð��� Limit ���� Ŭ�� �߻�
#define ST_V_LIMIT            0x0020 // ����ӵ��� �ӵ� limit���� Ŭ�� �߻�
#define ST_REVESE_MOVE        0x00C0 // �������� �ݴ��� �������� ������
#define ST_X_NEG_LIMIT        0x0040 // - Position limit�� ����� �߻�
#define ST_X_POS_LIMIT        0x0080 // + Position limit�� ����� �߻�
#define ST_ERROR_LIMIT        0x0100 // ��ġ������ Limit ������ Ŭ�� �߻�
#define ST_PC_COMMAND         0x0200 // EVENT�� �߻��� �����϶� �߻�
                                    // set_stop, set_e_stop, set_linear_all_stop
#define ST_OUT_OF_FRAMES      0x0400 // Frame buffer�� Full �����϶� �߻�
#define ST_AMP_POWER_ONOFF    0x0800 // AMP disable �����϶� �߻�
#define ST_ABS_COMM_ERROR     0x1000 // ABS encoder ��� Error�� �߻�
#define ST_INPOSITION_STATUS  0x2000 // In position signal �Է� ����
#define ST_RUN_STOP_COMMAND   0x4000 // Run stop error
#define ST_COLLISION_STATE    0x8000 // �浹���� Error
#define MAX_NODE_COUNT 		10000
#define AMC_ADD_LOG(...)    WRITE_LOG_UNIT(m_pLogAMCSend, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
//#define AMC_ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send.log", __VA_ARGS__)
//#define AMC_PERIODIC_SEND_LOG_OLD(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send_PeriodicOLD.log", __VA_ARGS__)
//#define AMC_PERIODIC_SEND_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send_Periodic.log", __VA_ARGS__)
//#define AMC_PERIODIC_LOG(...)	WRITE_LOG_UNIT(m_pLogUnit, __VA_ARGS__)
//#define AMC_ENC_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Recv.log", __VA_ARGS__)



int ConvertAMCError(int nAmcError);
wchar_t *strOHTMODEL[] = {L"FOUP", L"MAC", L"PROBECARD", L"PROBECARD_JIG", L"FOUP_REV01",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW", L"UNKNOW",
						  L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",
						  L"UNKNOW", L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"UNKNOW",L"OHT_STD_V80_EUV",
						  L"OHT_STD_V80",L"OHT_STD_V81",L"OHT_STD_V81S",L"UNKNOW",L"OHT_STD_V85"};

enum
{
	WDT_MAINLOOP = 1,
	WDT_EXTRA = 2,
	WDT_SUBCONTROL = 3,
	WDT_CONTROL = 4
};


/**
@brief   AMC Library�� Load�Ͽ� �Լ� �ʱ�ȭ
@author  LSJ
@date    2012.10.27
@return  Load�� ���� ������ Instance, ���� �߻��� NULL ����.
*/
HINSTANCE AMCLibrary::RunTimeLink()
{
	EnterCriticalSection(&m_CS);
	HINSTANCE hAMCLib = NULL;
	TCHAR strTmpFullPathFileName[MAX_LENGTH_OF_FULL_PATH]={0,};

#ifdef  UNICODE
	LPCTSTR strTmpFileName = L"DataFiles\\amc.dll";       //TCHAR
	_wfullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#else
	LPCTSTR strTmpFileName = "DataFiles\\amc.dll";       //TCHAR
	_fullpath(strTmpFullPathFileName, strTmpFileName, MAX_LENGTH_OF_FULL_PATH);
#endif

	if(m_bDllLoaded==true)
	{
		LeaveCriticalSection(&m_CS);
		return NULL;
	}

	hAMCLib = LoadLibrary(strTmpFullPathFileName);


	if (hAMCLib == NULL)
	{

		LeaveCriticalSection(&m_CS);
		return NULL;
	}
	else
	{
		// -- �ʱ�ȭ --
		DeviceAMCOpen	= (OPENAMCDEVICE)	GetProcAddress(hAMCLib, "amc_open");
		DeviceAMCClose	= (CLOSEAMCDEVICE)	GetProcAddress(hAMCLib, "amc_close");
		DeviceAMCInit	= (AMCINIT)			GetProcAddress(hAMCLib, "mmc_init");
		DeviceAMCReboot	= (AMC_DSP_REBOOT)	GetProcAddress(hAMCLib,"dsp_reboot");
		// -- ���� Ȯ�� -- ���ϵ� ���� 20100�̶�� 2.01.00
		DeviceAMCVerChk	= (AMC_VERSION_CHK)	GetProcAddress(hAMCLib, "version_chk");

	//==========================================================================
		// -- AMC ���� �ޱ� -- : OHTv7.0 �ϸ鼭 �߰��� �Լ�
		DeviceAMCGetDatas				= (AMC_GETDATAS)				GetProcAddress(hAMCLib, "ReturnAMCData");
		DeviceAMCGatheringTraceData		= (AMC_GATHERING_TRACE_DATA)	GetProcAddress(hAMCLib, "GatheringTraceData");
		DeviceAMCGatheringTraceData_1	= (AMC_GATHERING_TRACE_DATA_1)	GetProcAddress(hAMCLib, "GatheringTraceData_1");

		// AMC Emulator �ø��� ��� ����
		DeviceAMCCreateSerial = (AMC_CREATE_SERIAL) GetProcAddress(hAMCLib, "CreateSerial");
		DeviceAMCReadSerial = (AMC_READ_SERIAL) GetProcAddress(hAMCLib, "ReadSeiral");
		DeviceAMCWriteSerial = (AMC_WRITE_SERIAL) GetProcAddress(hAMCLib, "WriteSeiral");
		DeviceAMCCloseSerial = (AMC_CLOSE_SERIAL) GetProcAddress(hAMCLib, "CloseSeiral");

	//==========================================================================
		// -- Init �� --
		DeviceAMCSetPosition = (AMC_SETPOSITION) GetProcAddress(hAMCLib, "set_position");
		DeviceAMCGetPosition = (AMC_GETPOSITION) GetProcAddress(hAMCLib, "get_position");
		DeviceAMCSetAmpEnable = (AMC_SETAMPENABLE) GetProcAddress(hAMCLib, "set_amp_enable");
		DeviceAMCGetAmpEnable = (AMC_GETAMPENABLE) GetProcAddress(hAMCLib, "get_amp_enable");
		DeviceAMCFaultSet = (AMC_AMPFAULTSET) GetProcAddress(hAMCLib, "amp_fault_set");
		DeviceAMCFaultReset = (AMC_AMPFAULTRESET) GetProcAddress(hAMCLib, "amp_fault_reset");

		// -- Motion --
		DeviceAMCInSequence = (AMC_IN_SEQUENCE) GetProcAddress(hAMCLib, "in_sequence");
		DeviceAMCInMotion = (AMC_IN_MOTION) GetProcAddress(hAMCLib, "in_motion");
		DeviceAMCInPosition = (AMC_IN_POSITION) GetProcAddress(hAMCLib, "in_position");
		DeviceAMCFramesLeft = (AMC_FRAMES_LEFT) GetProcAddress(hAMCLib, "frames_left");
		DeviceAMCMotionDone = (AMC_MOTION_DONE) GetProcAddress(hAMCLib, "motion_done");

		DeviceAMCAxisDone = (AMC_AXIS_DONE) GetProcAddress(hAMCLib, "axis_done");
		DeviceAMCAxisState = (AMC_AXIS_STATE) GetProcAddress(hAMCLib, "axis_state");
		DeviceAMCAxisSource = (AMC_AXIS_SOURCE) GetProcAddress(hAMCLib, "axis_source");
		DeviceAMCClearStatus = (AMC_CLEAR_STATUS) GetProcAddress(hAMCLib, "clear_status");
		DeviceAMCClearStop = (AMC_CLEAR_STOP) GetProcAddress(hAMCLib, "clear_stop");

		DeviceAMCFramesClear = (AMC_FRAMES_CLEAR) GetProcAddress(hAMCLib, "frames_clear");
		DeviceAMCSetClosedLoop = (AMC_SET_CLOSED_LOOP) GetProcAddress(hAMCLib, "set_closed_loop");
		DeviceAMCSetUnipolar = (AMC_SET_UNIPOLAR) GetProcAddress(hAMCLib, "set_unipolar");
		DeviceAMCSetFeedback = (AMC_SET_FEEDBACK) GetProcAddress(hAMCLib, "set_feedback");
		DeviceAMCSetControl = (AMC_SET_CONTROL) GetProcAddress(hAMCLib, "set_control");

		DeviceAMCSetServo = (AMC_SET_SERVO) GetProcAddress(hAMCLib, "set_servo");
		DeviceAMCVMove = (AMC_VMOVE) GetProcAddress(hAMCLib, "v_move");
		DeviceAMCMove = (AMC_MOVE) GetProcAddress(hAMCLib, "move");
		DeviceAMCStartMove = (AMC_STARTMOVE) GetProcAddress(hAMCLib, "start_move");

		DeviceAMCPMove = (AMC_PMOVE) GetProcAddress(hAMCLib, "move_p");
		DeviceAMCNMove = (AMC_NMOVE) GetProcAddress(hAMCLib, "move_n");
		DeviceAMCSMove = (AMC_SMOVE) GetProcAddress(hAMCLib, "move_s");
		DeviceAMCDSMove = (AMC_DSMOVE) GetProcAddress(hAMCLib, "move_ds");

		DeviceAMCPTMove = (AMC_PTMOVE) GetProcAddress(hAMCLib, "move_pt");
		DeviceAMCNTMove = (AMC_NTMOVE) GetProcAddress(hAMCLib, "move_nt");
		DeviceAMCSTMove = (AMC_STMOVE) GetProcAddress(hAMCLib, "move_st");
		DeviceAMCDSTMove = (AMC_DSTMOVE) GetProcAddress(hAMCLib, "move_dst");

		DeviceAMCSetEncoderOffset = (AMC_SET_ENCODEROFFSET) GetProcAddress(hAMCLib, "set_encoder_offset");
		DeviceAMCGetEncoderOffset = (AMC_GET_ENCODEROFFSET) GetProcAddress(hAMCLib, "get_encoder_offset");
		// -- I/O ���� --
		DeviceAMCSetIO_64 = (AMC_SET_IO64) GetProcAddress(hAMCLib, "set_io64");
		DeviceAMCGetIO_64 = (AMC_GET_IO64) GetProcAddress(hAMCLib, "get_io64");
		DeviceAMCGetOutIO = (AMC_GET_OUT64) GetProcAddress(hAMCLib, "get_out64");
		DeviceAMCSetBit = (AMC_SET_BIT) GetProcAddress(hAMCLib, "set_bit");
		DeviceAMCResetBit = (AMC_RESET_BIT) GetProcAddress(hAMCLib, "reset_bit");

		DeviceAMCSetIO_Count    = (AMC_SET_IO_COUNT)        GetProcAddress(hAMCLib, "set_io_count");
		DeviceAMCGetIO_Count    = (AMC_GET_IO_COUNT)        GetProcAddress(hAMCLib, "get_io_count");
		DeviceAMCSetIO_Output    = (AMC_SET_IO_OUTPUT)        GetProcAddress(hAMCLib, "set_io_output");
		DeviceAMCGetIO_Output    = (AMC_GET_IO_OUTPUT)        GetProcAddress(hAMCLib, "get_io_output");
		DeviceAMCGetIO_Input    = (AMC_GET_IO_INPUT)        GetProcAddress(hAMCLib, "get_io_input");

		// -- ����� Command ���� �� File Download --
		DeviceAMCDownloadFile = (AMC_DOWNLOAD_FILE) GetProcAddress(hAMCLib, "DownloadLongBlock");
		DeviceAMCCmdGenerate = (AMC_CMD_GEN) GetProcAddress(hAMCLib, "amc_exchange_string");

		// -- File ���� �Լ� --
		DeviceAMCOutUpdate = (AMC_FS_DOWNLOAD) GetProcAddress(hAMCLib, "fs_download");
		DeviceAMCSetBootFile = (AMC_FS_SETBOOTFILE) GetProcAddress(hAMCLib, "fs_setbootfile");
		DeviceAMCInstall = (AMC_FS_INSTALL) GetProcAddress(hAMCLib, "fs_install");

		DeviceAMCUploadSysParam = (AMC_UPLOAD_SYS_PARAM) GetProcAddress(hAMCLib, "UploadSysParam");
		DeviceAMCDnloadSysParam = (AMC_DNLOAD_SYS_PARAM) GetProcAddress(hAMCLib, "DnloadSysParam");
		DeviceAMCUploadParam = (AMC_UPLOAD_PARAM) GetProcAddress(hAMCLib, "UploadParam");
		DeviceAMCDnloadParam = (AMC_DNLOAD_PARAM) GetProcAddress(hAMCLib, "DnloadParam");

		DeviceAMCParamSaveToDSP = (AMC_SAVE_PARAM) GetProcAddress(hAMCLib, "amc_save_local_sysparam_to_dsp");
		DeviceAMCParamSaveToFile = (AMC_LOAD_PARAM) GetProcAddress(hAMCLib, "amc_load_dsp_sysparam_with_localfile");
		DeviceAMCParamSaveToEEPROM = (AMC_SAVE_PARAM_TO_EEPROM) GetProcAddress(hAMCLib, "amc_flush_sysparam_to_eeprom");

		DeviceAMCGetActVelocity = (AMC_GETACTVELOCITY) GetProcAddress(hAMCLib, "get_act_velocity");
		DeviceAMCGetComVelocity = (AMC_GETCOMVELOCITY) GetProcAddress(hAMCLib, "get_com_velocity");

		DeviceAMCGetActVelocity_RPM = (AMC_GETACTVELOCITY_RPM) GetProcAddress(hAMCLib, "get_act_velocity_rpm");
		DeviceAMCGetComVelocity_RPM = (AMC_GETCOMVELOCITY_RPM) GetProcAddress(hAMCLib, "get_com_velocity_rpm");

		DeviceAMCSetPositiveSWLimit = (AMC_SET_POSITIVE_SW_LIMIT) GetProcAddress(hAMCLib, "set_positive_sw_limit");
		DeviceAMCSetNegativeSWLimit = (AMC_SET_NEGATIVE_SW_LIMIT) GetProcAddress(hAMCLib, "set_negative_sw_limit");

		DeviceAMCGetPositiveSWLimit = (AMC_GET_POSITIVE_SW_LIMIT) GetProcAddress(hAMCLib, "get_positive_sw_limit");
		DeviceAMCGetNegativeSWLimit = (AMC_GET_NEGATIVE_SW_LIMIT) GetProcAddress(hAMCLib, "get_negative_sw_limit");

		DeviceAMCSetErrorLimit = (AMC_SET_ERROR_LIMIT) GetProcAddress(hAMCLib, "set_error_limit");
		DeviceAMCGetErrorLimit = (AMC_GET_ERROR_LIMIT) GetProcAddress(hAMCLib, "get_error_limit");
		DeviceAMCGetError = (AMC_GET_ERROR) GetProcAddress(hAMCLib, "get_error");

		DeviceAMCSetSyncMapAxes = (AMC_SET_SYNC_MAP_AXES) GetProcAddress(hAMCLib, "set_sync_map_axes");
		DeviceAMCSetSyncControl = (AMC_SET_SYNC_CONTROL) GetProcAddress(hAMCLib, "set_sync_control");
		DeviceAMCSetSyncGain = (AMC_SET_SYNC_GAIN) GetProcAddress(hAMCLib, "set_sync_gain");
		DeviceAMCGetSyncControl = (AMC_GET_SYNC_CONTROL) GetProcAddress(hAMCLib, "get_sync_control");
		DeviceAMCGetSyncGain = (AMC_GET_SYNC_GAIN) GetProcAddress(hAMCLib, "get_sync_gain");
		DeviceAMCGetSyncPosition = (AMC_GET_SYNC_POSITION) GetProcAddress(hAMCLib, "get_sync_position");

		DeviceAMCFormat = (AMC_FORMAT) GetProcAddress(hAMCLib, "fs_format");

		// ���� ����..
		DeviceAMCSetPositionLPF = (AMC_SET_POSITION_LPF) GetProcAddress(hAMCLib, "set_position_lpf");
		DeviceAMCSetPositionNotchFilter = (AMC_SET_POSITION_NOTCH_FILTER) GetProcAddress(hAMCLib, "set_position_notch_filter");

		// ** watchdog ����
		DeviceAMCEnableWatchdogReason = (AMC_ENABLE_WDT_REASON) GetProcAddress(hAMCLib, "enable_wdt_reason");
		DeviceAMCDisableWatchdogReason = (AMC_DISABLE_WDT_REASON) GetProcAddress(hAMCLib, "disable_wdt_reason");
		DeviceAMCGetWatchdogStatus = (AMC_GET_WDT_STATUS) GetProcAddress(hAMCLib, "get_wdt_status");
		DeviceAMCSetWatchdogStatus = (AMC_SET_WDT_STATUS) GetProcAddress(hAMCLib, "set_wdt_status");
		DeviceAMCClearWatchdogReason = (AMC_CLR_WDT_REASON) GetProcAddress(hAMCLib, "clr_wdt_reason");
        DeviceAMCGetPhysicalAddr = (AMC_GET_PHYSICAL_ADDR) GetProcAddress(hAMCLib, "GetPhysicalAddr");

		// ** debug status
		DeviceAMCGetDebugStatus = (AMC_GET_DBG_STATUS) GetProcAddress(hAMCLib, "get_dbg_status");


		// ** velocity limit	3.0
		DeviceAMCSetVelCurve = (AMC_SET_VEL_CURVE) GetProcAddress(hAMCLib, "set_vel_curve");
		DeviceAMCSetActVelMargin = (AMC_SET_ACTVEL_MARGIN) GetProcAddress(hAMCLib, "set_actvel_margin");

		DeviceAMCSetOhtModelId = (AMC_SET_OHT_MODEL_ID) GetProcAddress(hAMCLib, "set_oht_model_id");
		DeviceAMCGetOhtModelId = (AMC_GET_OHT_MODEL_ID) GetProcAddress(hAMCLib, "get_oht_model_id");

		if (!DeviceAMCOpen || !DeviceAMCClose || !DeviceAMCInit || !DeviceAMCSetPosition || !DeviceAMCGetPosition ||
			!DeviceAMCSetAmpEnable || !DeviceAMCGetAmpEnable || !DeviceAMCFaultSet || !DeviceAMCFaultReset ||
			!DeviceAMCInSequence || !DeviceAMCInMotion || !DeviceAMCInPosition || !DeviceAMCFramesLeft ||
			!DeviceAMCMotionDone || !DeviceAMCAxisDone || !DeviceAMCAxisState || !DeviceAMCAxisSource || !DeviceAMCClearStatus ||
			!DeviceAMCClearStop || !DeviceAMCFramesClear || !DeviceAMCSetClosedLoop || !DeviceAMCSetUnipolar ||
			!DeviceAMCSetFeedback || !DeviceAMCSetControl || !DeviceAMCSetServo || !DeviceAMCVMove || !DeviceAMCMove ||
			!DeviceAMCStartMove || !DeviceAMCSetIO_64 || !DeviceAMCGetIO_64 || !DeviceAMCGetOutIO || !DeviceAMCSetBit || !DeviceAMCResetBit ||
			!DeviceAMCDownloadFile || !DeviceAMCCmdGenerate || !DeviceAMCUploadSysParam || !DeviceAMCDnloadSysParam ||
			!DeviceAMCUploadParam || !DeviceAMCDnloadParam || !DeviceAMCParamSaveToDSP || !DeviceAMCParamSaveToFile ||
			!DeviceAMCParamSaveToEEPROM || !DeviceAMCGetActVelocity || !DeviceAMCGetComVelocity || !DeviceAMCSetPositiveSWLimit ||
			!DeviceAMCSetNegativeSWLimit || !DeviceAMCGetPositiveSWLimit || !DeviceAMCGetNegativeSWLimit || !DeviceAMCSetErrorLimit ||
			!DeviceAMCVerChk || !DeviceAMCPMove || !DeviceAMCNMove || !DeviceAMCSMove || !DeviceAMCDSMove || !DeviceAMCSetEncoderOffset ||
			!DeviceAMCGetEncoderOffset || !DeviceAMCPTMove || !DeviceAMCNTMove || !DeviceAMCSTMove || !DeviceAMCDSTMove ||
			!DeviceAMCGetActVelocity_RPM || !DeviceAMCGetComVelocity_RPM || !DeviceAMCSetSyncMapAxes || !DeviceAMCSetSyncControl ||
			!DeviceAMCSetSyncGain || !DeviceAMCGetSyncControl || !DeviceAMCGetSyncGain || !DeviceAMCGetSyncPosition ||
			!DeviceAMCOutUpdate || !DeviceAMCReboot || !DeviceAMCSetBootFile || !DeviceAMCSetPositionLPF || !DeviceAMCSetPositionNotchFilter ||
			!DeviceAMCInstall || !DeviceAMCFormat || !DeviceAMCGetErrorLimit || !DeviceAMCGetError || !DeviceAMCEnableWatchdogReason ||
			!DeviceAMCDisableWatchdogReason || !DeviceAMCGetWatchdogStatus || !DeviceAMCSetWatchdogStatus || !DeviceAMCClearWatchdogReason ||
			!DeviceAMCGetDebugStatus || !DeviceAMCSetVelCurve || !DeviceAMCSetActVelMargin ||
	//========================================================================== 7.0 �߰� ����
			!DeviceAMCSetOhtModelId || !DeviceAMCGetOhtModelId ||		
			!DeviceAMCSetIO_Count || !DeviceAMCGetIO_Count || !DeviceAMCSetIO_Output || !DeviceAMCGetIO_Output || !DeviceAMCGetIO_Input ||
			!DeviceAMCGetDatas	|| !DeviceAMCGatheringTraceData|| !DeviceAMCGatheringTraceData_1 || !DeviceAMCGetPhysicalAddr)
		{
			FreeLibrary(hAMCLib);
			LeaveCriticalSection(&m_CS);
			return NULL;
		}
		else
		{
			LeaveCriticalSection(&m_CS);
			return hAMCLib;
		}
	}
}

/**
@brief   AMC Library ������

	1. ���� �ʱ�ȭ
	2. DLL Loading
	3. ���� Ȯ��
@author  ehoto
@date    2012.10.27
*/
AMCLibrary::AMCLibrary()
{
	bSetting_Jig = false;
	bAxis_Ignore = false;


	for(int i = 0; i<4; i++)
		bUseAxis_Jig[i] = false;

	// 90���� ��ȯ
	m_pLogAmcRcv = getLogUnit("D:\\log\\AMC_Recv.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	m_pLogAmcSendPr =  getLogUnit("D:\\log\\AMC_Send_Periodic.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	m_pLogAMCSend = getLogUnit("D:\\log\\AMC_Send.log", MAX_BYTE_SIZE_OF_AMC_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", 1024*10);


	InitializeCriticalSection(&m_CS);

	int nError = NO_ERR;
	AbnormalNodeCount = 0;
	nAMCCommError = NO_ERR;
	nAMCBufferError = NO_ERR;

	// 1. ���� �ʱ�ȭ
	m_fSyncGain = 1.0;
	m_hAMCLib = NULL; // handle

	m_bDllLoaded			= false;
	m_bDriverOpen			= false;
	m_bAMCFileDownloaded	= false;
	m_bAMCReady				= false;
	g_uLogDpramBase = 0x0;
	wFPGAVersion= 0x0;
	m_nError = 0;

	nAMCPauseStatus= 0;
	nAMCTorqueStatus = 0;
	nAMCVHLDetectStatus = 0;
	nAMCVHLStopStatus =0;
	nAMCDecelStatus=0;
	nAMCDetectStopDecelDownStatus = 0;
	nAMCDetectForceIOStatus = 0;
	m_VolatageMonitor[0] = 0;
	m_VolatageMonitor[1] = 0;

	m_FollowigAxis1 = 0; //AOHC-196 ���� �α� �߰�
	for(int nAxis=0; nAxis<4; nAxis++)
	{
		m_abIsRunning[nAxis]				=	false;					///< ���� ������
		m_abInPosition[nAxis]				=	false;				///< ���� �Ϸ� ��������
		m_abAmpEnabled[nAxis]				= 	false;					///< AMP Enable ��������    : [�ӽ�]������ ��ȯ�ص�
		m_adCurrentPosition[nAxis]			= 	0.0;				///< ���� ��ġ:	Cts
		m_adFinalPosition[nAxis]			= 	0.0;				///< ���� ��ġ:	Cts
		m_adCurrentVelocity[nAxis]			= 	0.0;			///< ���� �ӵ�: Cts/sec
		m_adFollowingError[nAxis]			=	0.0;			///< ���� Following Error :  Cts
		m_adPositionOfSmallAddCheck[nAxis]	= 	0.0;	///< ���� ��ġ:	Cts

		m_anAxisError[nAxis] 				=	0;	///< �ະ ����

		m_adPrePosition[nAxis] = 0.0;
		m_adTotalPosition[nAxis] = 0.0;
		m_abIsCounted[nAxis] = false;
		m_anTotalMoveCount[nAxis] = 0;

	}

	for(int nIndex=0; nIndex<COUNT_OF_AMC_AXIS; nIndex++)
	{
		m_adPositiveLimit[nIndex]=(0x01<<(sizeof(UINT)*8-1))-1;	// ���ڴ� Max �� ���� : AMC���� ���������� int�� ����
		m_adNegativeLimit[nIndex]=((-1)*pow(0x01,(sizeof(UINT)*8-1)));	// ���ڴ� Min �� ���� : AMC���� ���������� int�� ����
	}

	m_dwSndTime = timeGetTime();

	m_bUseWatchDog = true;		// WatchDog ��� ���� �Ķ����

	// AMC �ֱ� ��� ���
	memset( &m_SndCmd,0,sizeof(AMC_CMD));
	memset( &m_QueuingAmcCmd,0,sizeof(QUEUEING_AMC_CMD));
	memset(&m_NodeCheckData, 0, sizeof(m_NodeCheckData));

	// 2. DLL Loading
	nError = AMCInit();

	// 3. ���� Ȯ��
	if (!nError)
	{
		nError = DeviceAMCVerChk(0, nAMCVer);
		FPGARegRead(EXP_FPGA_VER, READ_RETRY_CNT, wFPGAVersion);

	}
	else
	{
		AMCClose();
	}

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	/// @todo	AMC ���α׷� ������ Ȯ���Ѵ�   // gm_OHTStatus->strFWVer=AMC_Get_FirmwareVision();
}

/**
@brief   AMC Library �Ҹ���
@author  ehoto
@date    2012.10.27
*/
AMCLibrary::~AMCLibrary()
{
	AMCClose();
	DeleteCriticalSection(&m_CS);
}


int AMCLibrary::AMCInit(void)
{
	EnterCriticalSection(&m_CS);

	int	nError = 0;
	int	nRetValue = 0;
	char strTmpFullPathFileName[MAX_LENGTH_OF_FULL_PATH]={0,};
	AMC_DATA RcvData;

	if(m_bAMCReady==true)
	{
		LeaveCriticalSection(&m_CS);
		return NO_ERR;
	}

	// 0. ���� �ʱ�ȭ
	memset( m_anInput, 0, MAX_COUNT_IO_INPUT * sizeof(int) );
	memset( m_anOutput, 0, MAX_COUNT_IO_OUTPUT * sizeof(int) );
	memset( m_anOutput_CMD, 0, MAX_COUNT_IO_OUTPUT * sizeof(int));

	memset(&m_Input1Mask_R01, 0x00, sizeof(m_Input1Mask_R01));
	memset(&m_Input2Mask_R01, 0x00, sizeof(m_Input2Mask_R01));
	memset(&m_Input3Mask_R01, 0x00, sizeof(m_Input3Mask_R01));

	InputMaskInit();

//	Sleep(5000);

	// 1. DLL Load ---------- ---------- ---------- ----------
	LeaveCriticalSection(&m_CS);
	if(!m_hAMCLib) m_hAMCLib = RunTimeLink();
	EnterCriticalSection(&m_CS);
	if(m_hAMCLib!=NULL)
	{
		m_bDllLoaded = true;
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_AMC_INIT;
	}

//    Sleep(5000);

	// 2. AMC Open ---------- ---------- ---------- ----------
	// @todo : ��� ���� ��ġ Ȯ�� �ʿ�
	memset(strTmpFullPathFileName, 0, MAX_LENGTH_OF_FULL_PATH * sizeof(char) );
	_fullpath(strTmpFullPathFileName, PATH_AMCPARA, MAX_LENGTH_OF_FULL_PATH);
	nRetValue = DeviceAMCOpen(AMC_INTERRUPT, AMC_DPRAM_ADDRESS, strTmpFullPathFileName);

	AMC_ADD_LOG("Directory: %s", strTmpFullPathFileName);

	m_bDriverOpen = (nRetValue) ? true : false; // ��� return int�� bool�� ����...

	if(m_bDriverOpen)
	{
		nRetValue = DeviceAMCInit();
		nError = ConvertAMCError(nRetValue);
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ERR_AMC_NOTOPEN;
	}

//	EnterCriticalSection(&m_CS);
//	SetBit(OUT_BRAKE_DRIVING_RELEASE, false, false);
//	DeviceAMCGetDatas(&m_SndCmd, &RcvData);
//	LeaveCriticalSection(&m_CS);

//    Sleep(5000);     //DeviceAMCInit �Լ� ȣ�⶧ �ڷ� �и��� ���� �߻�.

	if(nError!=NO_ERR)
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return nError; //Convert_Error(nError);;
	}

	// 3. AMCParam �ٿ�ε� ---------- ---------- ---------- ----------
	// �̻�: AMC ������ ���� Defaul Parameter�� ���� �� �� ������ �ٽ� AMC�� ������
	// @todo : Return ���� � ������ Ȯ�� �ʿ�
	if(nError==NO_ERR)
	{
		memset( strTmpFullPathFileName, 0, MAX_LENGTH_OF_FULL_PATH * sizeof(char) );
		_fullpath(strTmpFullPathFileName, PATH_NAME_AMCPARA, MAX_LENGTH_OF_FULL_PATH);
		nRetValue = DeviceAMCParamSaveToFile(true, PATH_NAME_AMCPARA);	//"C:\\USER\\OHT\\DataFiles\\DefaultParam.ini"
	}
	if(nError==NO_ERR)
	{
		nRetValue = DeviceAMCParamSaveToDSP();
		ConvertAMCError(nRetValue);
	}
	if(nError==NO_ERR)
	{
		nRetValue = DeviceAMCParamSaveToEEPROM();
		ConvertAMCError(nRetValue);
	}
	if(nError==NO_ERR)
	{
		m_bAMCFileDownloaded = true;
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ConvertAMCError(nRetValue); //Convert_Error(nError);;
    }

	// 4. AMC ��� ������ ���� ---------- ---------- ---------- ----------
	if(m_bAMCFileDownloaded==true && m_bDriverOpen==true && m_bDllLoaded==true)
	{
		m_bAMCReady = true;
		AMC_ADD_LOG("m_bAMCReady = %d [%d:%d:%d]", m_bAMCReady, m_bAMCFileDownloaded, m_bDriverOpen, m_bDllLoaded);

		g_uLogDpramBase = DeviceAMCGetPhysicalAddr(0x1D0);
        AMC_ADD_LOG("Log Dpram Base = %x", g_uLogDpramBase);
	}
	else
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return ERR_AMC_NOTOPEN;
	}

	// 5. �� �ʱ�ȭ ---------- ---------- ---------- ----------
	for(int nIndex=0; nIndex<COUNT_OF_AMC_AXIS; nIndex++)
	{

//		DeviceAMCSetAmpEnable(nIndex, false);
        DeviceAMCSetAmpEnable(nIndex, true);
		AMC_ADD_LOG("DeviceAMCSetAmp Enable ��_�ʱ�ȭ");
	}


#if((OHT_NAME == OHT_NAME_STD_V80) ||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
   if(nError == NO_ERR)
		nError = DeviceAMCSetIO_Count(96, 96);
#endif

	// 7. Watch Dog ���� ---------- ---------- ---------- ----------
   if(m_bUseWatchDog==true)
   {
	   nError = EnableWatchDog();
	   if(nError==NO_ERR) nRetValue = ClearWatchDogStatus();
	   if(nError)
	   {
			LeaveCriticalSection(&m_CS);
			AMCClose();
		  return nError;
	   }
   }

	int OHT_Model = -2;

    // 2016-12-03, Add by Labeler
    // Reticle OHT�� AMC Firmware ���� ��� �����ʿ�
	if(OHT_NAME >= OHT_NAME_STD_V80_RETICLE)
	{
		for(int j=0 ; j<5 ; j++)
		{
			if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))
				DeviceAMCSetOhtModelId(OHT_NAME_STD_V80);
			else
				DeviceAMCSetOhtModelId(OHT_NAME);
			OHTMainForm->memoMain->Lines->Add("OHT Model");
			OHTMainForm->memoMain->Lines->Add(strOHTMODEL[OHT_NAME]);

			DeviceAMCGetOhtModelId(&OHT_Model);
			OHTMainForm->memoMain->Lines->Add("AMC Model");
			OHTMainForm->memoMain->Lines->Add(strOHTMODEL[OHT_Model]);

			if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))
			{
              	if(OHT_NAME_STD_V80 == OHT_Model)
				{
					nError = NO_ERR;
					break;
				}
				else
				{
					nError = ERR_AMC_INIT;
				ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
            }
			else
			{
				if(OHT_NAME == OHT_Model)
				{
					nError = NO_ERR;
					break;
				}
				else
				{
					nError = ERR_AMC_INIT;
			ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_INIT,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
            }

		}

	}



	if(nError != NO_ERR)
	{
		LeaveCriticalSection(&m_CS);
		AMCClose();
		return nError;
	}

	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_DRIVING_F, 56715*5, 1);	// 0.5m/s	// ��ũ
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_DRIVING_R, 56715*5, 1);	// 0.4m/s: 52151
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_HOIST, 150000, 1);	 // 0.5m/s
	if(nError == NO_ERR)
		nError = DeviceAMCSetVelCurve(AXIS_SHIFT, 150000, 1);	 // 0.5m/s


	if(nError == NO_ERR)
		nError = DeviceAMCSetActVelMargin(AXIS_DRIVING_F, 56715*5, ABORT_EVENT, 250);	// margin: 0.3m/s, Time:  250(250*0.2ms=50ms)	// �ӵ�
	if(nError == NO_ERR)
		nError = DeviceAMCSetActVelMargin(AXIS_DRIVING_R, 56715*5, ABORT_EVENT, 250);	// margin: 0.4m/s, Time:  250(250*0.2ms=50ms)



   LeaveCriticalSection(&m_CS);
   return nError;
}

//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
bool AMCLibrary::IsAMCDllOpen()
{
	return  m_bAMCReady;
}

int AMCLibrary::AMCClose(void)
{
	EnterCriticalSection(&m_CS);

   if(m_bUseWatchDog==true)
   {
	   ClearWatchDogStatus();
	   DisableWatchDog();
   }

   if(m_bDriverOpen==true)
   {
	  DeviceAMCClose();
	  m_bDriverOpen = false;
   }
   if(m_bDllLoaded==true || m_hAMCLib!=NULL)
   {
	  FreeLibrary(m_hAMCLib);
	  m_hAMCLib=NULL;
	  m_bDllLoaded = false;
   }

   m_bAMCFileDownloaded = false;
   m_bAMCReady = false;

   LeaveCriticalSection(&m_CS);
   return NO_ERR;
}

/**
@brief   AMCError�� �Ϲ�ȭ ������ ���� ��ų �� ����ϴ� �Լ�

	sync �ϱ� �� ��ũ �Ϸ��� �� Amp disable ��Ŵ
@author  ehoto
@date    2012.10.29
@param   nMasterAxisNum : Master�� ��ȣ
@param   nSlaveAxisNum : Slave �� ��ȣ
@param   dSyncGain : SyncGain
@return	 ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
*/
int AMCLibrary::SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("DeviceAMCSetAmp Disable_SetSync");

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCSetAmpEnable(nMasterAxisNum, false);
	LeaveCriticalSection(&m_CS);
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetAmpEnable(nSlaveAxisNum, false);
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncMapAxes(nMasterAxisNum, nSlaveAxisNum);
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncControl(true); //sync mode enable
		LeaveCriticalSection(&m_CS);
	}
	if(nTmpError==NO_ERR)
	{
		m_fSyncGain=dSyncGain;
		EnterCriticalSection(&m_CS);
		nTmpError = DeviceAMCSetSyncGain(m_fSyncGain);	///< 1.0
		LeaveCriticalSection(&m_CS);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Max S/W Limit �����ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dPosLimit : Positive Limit �Ÿ�
@param   dNegLimit : Negative Limit �Ÿ�
@return  Error Code
*/
int AMCLibrary::SetMaxSWLimitForReset(int nAxisNum, double dPosLimit, double dNegLimit)
{
	if(m_bAMCReady == false)
	{
		return ERR_AMC_NOTOPEN;
	}

	m_adPositiveLimit[nAxisNum]=dPosLimit;
	m_adNegativeLimit[nAxisNum]=dNegLimit;

	return NO_ERR;
}


/**
@brief   AMCError�� �Ϲ�ȭ ������ ���� ��ų �� ����ϴ� �Լ�
@author  ehoto
@date    2012.10.29
@param   nAmcError : AMC Error
@return	 ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
*/
int ConvertAMCError(int nAmcError)
{
	int nError = 0;

	switch (nAmcError)
    {
    case 0:
		nError = NO_ERR;
        break;
    case 1:
		nError = ERR_AMC_NOT_INITIALIZED;
        break;
    case 2:
		nError = ERR_AMC_TIMEOUT_ERR;
		break;
	case 3:
		nError = ERR_AMC_INVALID_AXIS;
		break;
	case 4:
		nError =  ERR_AMC_ILLEGAL_ANALOG;
        break;
    case 5:
		nError = ERR_AMC_ILLEGAL_IO;
        break;
    case 6:
		nError = ERR_AMC_ILLEGAL_PARAMETER;
		break;
    case 7:
		nError = ERR_AMC_NO_MAP;
        break;
    case 8:
		nError = ERR_AMC_AMP_FAULT;
        break;
    case 9:
		nError = ERR_AMC_ON_MOTION;
        break;
    case 10:
		nError = ERR_AMC_NON_EXIST;
        break;
    case 11:
		nError = ERR_AMC_BOOT_OPEN_ERROR;
        break;
    case 12:
		nError = ERR_AMC_CHKSUM_OPEN_ERROR;
        break;
    case 13:
		nError = ERR_AMC_WINNT_DRIVER_OPEN_ERROR;
        break;
    case 14:
		nError = ERR_AMC_ILLEGAL_PARAM_MOVE_DS;
        break;
    case 15:
		nError = ERR_AMC_BUSY_PROFILE_GEN_MOVEX;
        break;
	case 17:
//		nError = AMC_VERSION_ERROR;
        break;
    case 30:
		nError = ERR_MMC_MOVE_LASTDS;
        break;
    case 31:
		nError = ERR_MMC_MUTEXLOCK_ERROR;
        break;
    case 32:
		nError = ERR_MMC_FILTERING_ERROR;
        break;
    case -1:
		nError = ERR_AMC_FUNC_ERR;
		break;
	// �Ʒ� Ȯ�� �ʿ�
	case 901 :
		nError = ERR_AMC_RCVFORMAT1;
		break;
//	default:
//		nError = ERR_AMC_NOTDEFINED;
//        break;
    }

//	if(nError != NO_ERR)
//	{
//		ADD_FDC_LOG("ERR/%d/1/0",nError)
//	}

	return nError;
}

/**
@brief   IO�� �ش� Output�� On/Off ��ų �� ����ϴ� �Լ�
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h�� ������ ENUM ���� OUTPUT ���(On/Off �ϰ��� �ϴ� ��ȣ ����)
@param   OnOff : �ش� ����� On �Ϸ��� True, Off �Ϸ��� false.
@return	 ���ۿ� ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
*/
int AMCLibrary::SetBit(int nIndex, bool bOnOff, bool bOBSControl)
{
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false)
	{
		return ERR_AMC_NOTOPEN;
	}

	int nTmpError = 0;
	int nArrayNum = 0, nMaskingNum = 0;

	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetOutBit = true;


	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));



	if(bOnOff==true)
	{
		m_anOutput_CMD[nArrayNum] = m_anOutput_CMD[nArrayNum]|nMaskingNum;		//|(0000...010...0000)
	}
	else
	{
		m_anOutput_CMD[nArrayNum] = m_anOutput_CMD[nArrayNum]&(~nMaskingNum);	//&(1111...101...1111)
	}

	if(bOBSControl == true)
	{
		m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1] = m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1]|0x8000;
	}
	
	memcpy(QueuingAmcCmd.DataUnion.Output, m_anOutput_CMD, sizeof(int)*4);

	if(bOBSControl == true)
	{
		m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1] = m_anOutput_CMD[MAX_COUNT_IO_OUTPUT-1]&(~0x8000);
	}

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

//	AMC_ADD_LOG("AMC SetBit Error:%d %d %d", bOnOff,nIndex,bOBSControl);
//	if(nTmpError!=NO_ERR)  nAMCBufferError = ERR_INVALID_RESPONS;

	return ConvertAMCError(nTmpError);
}

/**
@brief   IO�� �ش� Output�� On/Off ��ų �� ����ϴ� �Լ�
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h�� ������ ENUM ���� OUTPUT ���(On/Off �ϰ��� �ϴ� ��ȣ ����)
@param   OnOff : �ش� ����� On �Ϸ��� True, Off �Ϸ��� false.
@return	 ���ۿ� ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
*/
bool AMCLibrary::GetInputBit(int nIndex)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	int nArrayNum = 0, nMaskingNum = 0;
	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));

	return m_anInput[nArrayNum]&nMaskingNum;
}

/**
@brief   IO�� �ش� Output�� On/Off ��ų �� ����ϴ� �Լ�
@author  ehoto
@date    2012.10.29
@param   index : DEF_HW.h�� ������ ENUM ���� OUTPUT ���(On/Off �ϰ��� �ϴ� ��ȣ ����)
@param   OnOff : �ش� ����� On �Ϸ��� True, Off �Ϸ��� false.
@return	 ���ۿ� ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
*/
bool AMCLibrary::GetOutputBit(int nIndex)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	int nArrayNum = 0, nMaskingNum = 0;
	nArrayNum = nIndex/32;
	nMaskingNum = (0x01<<(nIndex%32));

	return m_anOutput[nArrayNum]&nMaskingNum;
}



/**
@brief   AMCLibrary �ʱ�ȭ �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ
@return  Error Code
*/
int AMCLibrary::Init(int nAxisNum)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	return Reset(nAxisNum);
}

/// ���� Ȯ��
/**
@brief   AMCLibrary ���� Ȯ�� �ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ
@return  Error Code
*/
bool AMCLibrary::IsRunning(int nAxisNum)
{
	if(m_bAMCReady == false) return false;
	return m_abIsRunning[nAxisNum];
}

/**
@brief   AMCLibrary ����ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ
@return  Error Code
*/
bool AMCLibrary::IsInPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return false;
	return m_abInPosition[nAxisNum];
}

/**
@brief   AMCLibrary AMP On/Off ���� Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
bool AMCLibrary::IsAMPEnable(int nAxisNum)
{
	if(m_bAMCReady == false) return false;

	return m_abAmpEnabled[nAxisNum];
}

/**
@brief   AMCLibrary ���� ��ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  ���� ��ġ
*/
double AMCLibrary::GetCurrentPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adCurrentPosition[nAxisNum];
}

/**
@brief   AMCLibrary Small Add Move �� ������ ������ ��ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Small Add Move �� ������ ������ ��ġ�� ǥ����
*/
double AMCLibrary::GetSmallAddCheckPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adPositionOfSmallAddCheck[nAxisNum];
}

/**
@brief   AMCLibrary ���� ���� ��ġ Ȯ�� �Լ�
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ (0~3)
@return  ���� ���� ��ġ
*/
double AMCLibrary::GetPrePosition(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0.0;
	else
		return m_adPrePosition[nAxisNum];
}

/**
@brief   Reset�ϱ������� ������ ��
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ (0~3)
@return  ���� position��
*/
double AMCLibrary::GetTotalPosition(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0.0;
	else
		return m_adTotalPosition[nAxisNum];
}

/**
@brief   TotalPosition�� Reset��
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ (0~3)
*/
void AMCLibrary::ResetTotalPosition(int nAxisNum)
{
	if(m_bAMCReady == true)
		m_adTotalPosition[nAxisNum] = 0.0;
}

/**
@brief   TotalMoveCount�� ������
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ (0~3)
*/
int AMCLibrary::GetTotalMoveCount(int nAxisNum)
{
	if(m_bAMCReady == false)
		return 0;
	else
		return m_anTotalMoveCount[nAxisNum];
}

/**
@brief   TotalMoveCount�� Reset��
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ (0~3)
*/
void AMCLibrary::ResetTotalMoveCount(int nAxisNum)
{
	if(m_bAMCReady == true)
		m_anTotalMoveCount[nAxisNum] = 0;
}

/**
@brief	 AMCLibrary ���� ��ġ Ȯ�� �Լ�
@author  ���¿�
@date	 2013-04-03
@param	 nAxisNum : �� ��ȣ (0~3)
@return  ��� ���� ��ġ
*/
double AMCLibrary::GetFinalPosition(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adFinalPosition[nAxisNum];
}

/**
@brief   AMCLibrary ���� �ӵ� Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
double AMCLibrary::GetCurrentVelocity(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;
	return m_adCurrentVelocity[nAxisNum];
}

/**
@brief   AMCLibrary Following Error Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
double AMCLibrary::GetFollowingError(int nAxisNum)
{
	if(m_bAMCReady == false) return 0.0;

	//JIG �� �̿� ��������

	// AXIS ����  (���� ����)
	if(bAxis_Ignore == true)
		return 0;

	//����1. ���׼����� ��
	if(bSetting_Jig == true)
	{
		if(bUseAxis_Jig[nAxisNum] == true)
			return m_adFollowingError[nAxisNum];
		else
			return 0;
	}
	//����2. ���׼����� �ƴ� ��
	else
	{
    	return m_adFollowingError[nAxisNum];
	}

}

/// ���� ����
/**
@brief   AMCLibrary Error Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
int AMCLibrary::GetError(int nAxisNum)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	if(nAMCCommError !=NO_ERR) return nAMCCommError;

	if(nAMCBufferError !=NO_ERR) return nAMCBufferError;

	if(m_AMCDllLibQueue.GetReportError() !=NO_ERR)   return m_AMCDllLibQueue.GetReportError();


	// AXIS ����  (���� ����)
	if(bAxis_Ignore == true)
		return 0;

	//����1. ���׼����� ��
	if(bSetting_Jig == true)
	{
		if(bUseAxis_Jig[nAxisNum] == true)
			return m_anAxisError[nAxisNum];
		else
			return 0;
	}
	//����2. ���׼����� �ƴ� ��
	else
	{
		return m_anAxisError[nAxisNum];
	}
}

/**
@brief   AMCLibrary reset �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
int AMCLibrary::Reset(int nAxisNum)
{
	int nTmpError=NO_ERR;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	/// 1. Clear Event
   nTmpError = DeviceAMCFramesClear(nAxisNum);
   if(nTmpError==NO_ERR) nTmpError = DeviceAMCClearStatus(nAxisNum);

	/// 2. Clear AMP FALUT
	if(nTmpError==NO_ERR)
	{
		nTmpError = DeviceAMCSetAmpEnable(nAxisNum, false);
		if(nTmpError==NO_ERR)
		{
			nTmpError = DeviceAMCFaultReset(nAxisNum);
			Sleep(10);
			nTmpError = DeviceAMCFaultSet(nAxisNum);
		}

		nAMCBufferError =NO_ERR;
	}

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/// Offset ����
/**
@brief   AMCLibrary ���� ��ġ Home ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
int AMCLibrary::SetHome(int nAxisNum)
{
	int nTmpError=NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	switch(nAxisNum)
	{
	case 0:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
		break;
	case 1:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
		break;
	case 2:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
		break;
	case 3:
		QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
		break;
	}
	QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_SET_HOME;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC SetHome 1st:Error:%d,Axis~[%d]", nTmpError,nAxisNum);

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Offset ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dOffset : Offset �Ÿ�
@return  Error Code
*/
int AMCLibrary::SetOffset(int nAxisNum, double dOffset)
{
	int nTmpError = 0;
	int nOffset = (int)dOffset;
	int nSaveOffset=0;

	AMC_ADD_LOG("SetOffset Axis=%d, Offset=%8.1lf", nAxisNum, dOffset);


	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);
    AMC_ADD_LOG("DeviceAMCSetAmp Disable_SetOffset");
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCSetAmpEnable(nAxisNum, false);
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCSetEncoderOffset(nAxisNum, nOffset);
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCParamSaveToEEPROM();

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Offset �޾ƿ��� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dOffset : Offset �Ÿ�
@return  Error Code
*/
int AMCLibrary::GetOffset(int nAxisNum, double* dOffset)
{
	int nTmpError = 0;
	int nOffset = 0;
	int nSaveOffset=0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("GetOffset Axis=%d, Offset=%8.1lf", nAxisNum, dOffset);

	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCGetEncoderOffset(nAxisNum, &nOffset);

	*dOffset = (double)nOffset;

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/// Limit ����
/**
@brief   AMCLibrary Postive S/W Limit ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dLimit : Limit �Ÿ�
@return  Error Code
*/
int AMCLibrary::SetPositiveSWLimit(int nAxisNum, double dLimit)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCSetPositiveSWLimit(nAxisNum,dLimit,STOP_EVENT);
	AMC_ADD_LOG("[Axis%d] SW_PosLimit: %8.1lf",nAxisNum,dLimit);

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary Negative S/W Limit ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dLimit : Limit �Ÿ�
@return  Error Code
*/
int AMCLibrary::SetNegativeSWLimit(int nAxisNum, double dLimit)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	EnterCriticalSection(&m_CS);

	nTmpError = DeviceAMCSetNegativeSWLimit(nAxisNum,dLimit,STOP_EVENT);
	AMC_ADD_LOG("[Axis%d] SW_NegLimit: %8.1lf",nAxisNum,dLimit);

	LeaveCriticalSection(&m_CS);
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary S/W Limit Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dPosLimit : Positive Limit �Ÿ�
@param   dNegLimit : Negative Limit �Ÿ�
@return  Error Code
*/
int AMCLibrary::GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit)
{
	int nTmpError = 0;
	double dTmpPosLimit=0.0, dTmpNegLimit=0.0;
	int nPosAct=0, nNegAct=0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCGetPositiveSWLimit(nAxisNum,&dTmpPosLimit,&nPosAct);
	if(nTmpError==NO_ERR)
	{
		nTmpError = DeviceAMCGetNegativeSWLimit(nAxisNum,&dTmpNegLimit,&nNegAct);
    }
	LeaveCriticalSection(&m_CS);

	dPosLimit = dTmpPosLimit;
	dNegLimit = dTmpNegLimit;

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary S/W Limit ���� �Լ�

	�����Ǿ� �ִ� S/W Limit�� Max �� ����
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@return  Error Code
*/
int AMCLibrary::ResetSWLimit(int nAxisNum)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	AMC_ADD_LOG("DeviceAMCSetAmpDisable_Reset");
	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCSetPositiveSWLimit(nAxisNum,m_adPositiveLimit[nAxisNum],STOP_EVENT);
	if(nTmpError==NO_ERR)
		nTmpError = DeviceAMCSetNegativeSWLimit(nAxisNum,m_adNegativeLimit[nAxisNum],STOP_EVENT);
	LeaveCriticalSection(&m_CS);

	return ConvertAMCError(nTmpError);
}

/// ���� ���� - ����: �Ÿ�(mm), �ӵ�(mm/msec), ���ӵ�(mm/msec2), �ð�(msec)
/**
@brief   AMCLibrary Amp On/Off ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   bValue : On/Off ��
@return  Error Code
*/
int AMCLibrary::SetAMPEnable(int nAxisNum, bool bValue)
{
	int nTmpError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = ConvertAMCError(DeviceAMCSetAmpEnable(nAxisNum, bValue));
	LeaveCriticalSection(&m_CS);

//	AMC_ADD_LOG("DeviceAMCSetAmpEnable_Check : %d",bValue);   ////timy ���ʿ��� LOG ����

	return nTmpError;
}

// ���ӵ� ���
/**
@brief   AMCLibrary ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dAccel : ���ӵ�
@param   dVel : �ӵ�
@return  Error Code
*/
int AMCLibrary::MovePositive(int nAxisNum, double dAccel, double dVel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel*1000000.0;	///< Cts/sec2
		dVel_CtsPerSec = dVel*1000.0;			///< Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_P;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveP 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dAccel : ���ӵ�
@param   dVel : �ӵ�
@return  Error Code
*/
int AMCLibrary::MoveNegative(int nAxisNum, double dAccel, double dVel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel*1000000.0;	///< Cts/msec2 --> Cts/sec2
		dVel_CtsPerSec =dVel*1000.0;			///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_N;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveN 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2,dVel_CtsPerSec);

	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary �����ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dAccel : ���ӵ�
@return  Error Code
*/
int AMCLibrary::MoveStop(int nAxisNum, double dAccel)
{
	int nTmpError = 0;
	double dAccel_CtsPerSec2 = 0;	///< Cts/sec2, Cts/sec
	const double dDefault_ACCEL=340119.0; //3msec2
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	/// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		if(dAccel!=0.0)
		{
			dAccel_CtsPerSec2 = dAccel*1000000.0;	// UserUnit/msec2 --> Cts/sec2
		}
		else
		{
			///@todo: �ϵ� �ڵ� �� ���� �ʿ�
			dAccel_CtsPerSec2 = 340119.0; 			//3msec2;
		}

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_S;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel= dAccel_CtsPerSec2;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveS:Err:%d[%d:%8.1lf]", nTmpError,nAxisNum,dAccel_CtsPerSec2);

	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary �Ÿ��� ���ӵ�/���ӵ�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dAccel_Up : ���ӵ�
@param   dAccel_Down : ���ӵ�
@param   dVel : �ӵ�
@param   dDistance : �̵� �Ÿ�
@return  Error Code
*/
int AMCLibrary::MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down, double dVel_CtsPerMsec, double dDistance_Cts, TRIGGER_CTRL_INFO TriggerCtrlInfo)
{
	int nTmpError=NO_ERR;
	double dAccel_CtsPerSec2 = 0.0, dDecel_CtsPerSec2 = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	TRIGGER_CTRL_INFO_UNION TrigerCtrlInfoUnion;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	TrigerCtrlInfoUnion.TriggerInfo = TriggerCtrlInfo;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	static bool bCheckPulse = false;

	/// 1. DeviceAMCDSMove
	if(nTmpError==NO_ERR)
	{
		dAccel_CtsPerSec2 = dAccel_Up*1000000.0;		///< Cts/msec2 --> Cts/sec2
		dDecel_CtsPerSec2 = dAccel_Down*1000000.0;		///< Cts/msec2 --> Cts/sec2
		dVel_CtsPerSec = dVel_CtsPerMsec*1000.0;		///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}

		//E2314���� ���� ����
		if((nAxisNum == 0) &&(TrigerCtrlInfoUnion.TriggerInfo.UseSmallAdd ==1))
		{
			if(bCheckPulse == true)
			{
				TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist = TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist+1;

				bCheckPulse = false;
			}
			else
				bCheckPulse = true;
		}


		QueuingAmcCmd.DataUnion.CmdAxis.CmdType			= MMC_CMD_AXIS_MOVE_DS;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel			= dAccel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel			= dDecel_CtsPerSec2;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed			= dVel_CtsPerSec;
		QueuingAmcCmd.DataUnion.CmdAxis.Distance		= dDistance_Cts;
		QueuingAmcCmd.DataUnion.CmdAxis.TriggerCtrlInfo = TrigerCtrlInfoUnion.Cmd;



		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveDS 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf:%8.1lf:%8.1lf] EX[%d:%d/%d:%d:%d:%d]",
            nTmpError,
			nAxisNum,
			dAccel_CtsPerSec2,
			dDecel_CtsPerSec2,
			dVel_CtsPerSec,
            dDistance_Cts,
			TrigerCtrlInfoUnion.TriggerInfo.StartOnSpeedDown,
			TrigerCtrlInfoUnion.TriggerInfo.UseCurrSpeed,
			TrigerCtrlInfoUnion.TriggerInfo.UseSmallAdd, TrigerCtrlInfoUnion.TriggerInfo.IsTriggerOnRisingEdge,
			TrigerCtrlInfoUnion.TriggerInfo.TriggerIONumber,TrigerCtrlInfoUnion.TriggerInfo.AdditionalDist);

	}

	return ConvertAMCError(nTmpError);
}

// ���� �ð� ���
/**
@brief   AMCLibrary ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dwTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int AMCLibrary::MovePositive(int nAxisNum, DWORD dwTime, double dVel)
{
	int nTmpError=NO_ERR;
	double dTime_sec = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dTime_sec = dwTime;	///< msec
		dVel_CtsPerSec = dVel*1000.0;		///< msec  --> sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_P;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dTime_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MovePT 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dTime_sec,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dwTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int AMCLibrary::MoveNegative(int nAxisNum, DWORD dwTime, double dVel)
{
	int nTmpError = 0;
	double dTime_sec = 0.0, dVel_CtsPerSec = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);

	/// 2. DeviceAMCPMove
	if(nTmpError==NO_ERR)
	{
		dTime_sec = dwTime;	///< msec
		dVel_CtsPerSec = dVel*1000.0;			///< Cts/msec  --> Cts/sec

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_N;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dTime_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;


		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveNT 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dTime_sec,dVel_CtsPerSec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary ���� �ð� �����Ͽ� �����ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dwTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int AMCLibrary::MoveStop(int nAxisNum, DWORD dwTime)
{
	int nTmpError = 0;
	double dTime_sec = 0.0;	///< Cts/sec2, Cts/sec
	const double dDefault_ACCEL=340119.0; //3msec2
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCNMove
	if(nTmpError==NO_ERR)
	{
		if(dwTime!=0.0)
		{
			dTime_sec = dwTime;		///< msec
		}
		else
		{
			dTime_sec = 3000.0;	///< UserUnit/msec2 --> Cts/sec2 ( 3m/s )
		}

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_S;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel = dTime_sec;

		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		AMC_ADD_LOG("AMC MoveST:Err:%d[%d:%8.1lf]", nTmpError,nAxisNum,dTime_sec);
	}

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary �Ÿ��� ���ӽð�/���ӽð�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   dwTime_Up : ���� �ð�
@param   dwTime_Down : ���� �ð�
@param   dVel : �ӵ�
@param   dDistance : �̵� �Ÿ�
@return  Error Code
*/
int AMCLibrary::MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{
	int nTmpError = 0;
	double dAccel_Time_sec = 0.0, dDecel_Time_sec = 0.0, dVel_CtsPerSec = 0.0, dDistance_Cts = 0.0;	///< Cts/sec2, Cts/sec
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	/// 1. AMP ON
	nTmpError = SetAMPEnable(nAxisNum, true);


	/// 2. DeviceAMCDSMove
	if(nTmpError==NO_ERR)
	{
		dAccel_Time_sec = (double)dwTime_Up;		///< msec
		dDecel_Time_sec = (double)dwTime_Down;		///< msec
		dVel_CtsPerSec = dVel*1000.0;		///< UserUnit/msec  --> Cts/sec
		dDistance_Cts = dDistance;

		switch(nAxisNum)
		{
		case 0:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis1 = true;
			break;
		case 1:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis2 = true;
			break;
		case 2:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis3 = true;
			break;
		case 3:
			QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.MoveAxis4 = true;
			break;
		}
		QueuingAmcCmd.DataUnion.CmdAxis.CmdType = MMC_CMD_AXIS_MOVE_TIME_DS;
		QueuingAmcCmd.DataUnion.CmdAxis.Accel = dAccel_Time_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Decel = dDecel_Time_sec;
		QueuingAmcCmd.DataUnion.CmdAxis.Speed = dVel_CtsPerSec;
		QueuingAmcCmd.DataUnion.CmdAxis.Distance = dDistance_Cts;


		m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
		 ////timy ���ʿ��� LOG ����
//		AMC_ADD_LOG("AMC MoveDST 1st:Error:%d,Axis~[%d:%8.1lf:%8.1lf:%8.1lf:%8.1lf]", nTmpError,nAxisNum,dAccel_Time_sec,dDecel_Time_sec,dVel_CtsPerSec,dDistance_Cts);

	}

	return ConvertAMCError(nTmpError);
}

// ������� ����ÿ��� ���� �� ��� �Լ�
/**
@brief   AMCLibrary �� �������ݷ� ��� ����
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   pSendData : ������ Data �� ����ִ� Char �迭
@param   nSendLen : ������ Data ����
@param   pRecvData : �޴� Data �� ����ִ� Char �迭
@param   nRecvLen : �޴� Data ����
@return  Error Code
*/
int AMCLibrary::SendData(int nAxisNum, char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen)
{
	int nTmpError = 0;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	EnterCriticalSection(&m_CS);
	nTmpError = DeviceAMCCmdGenerate(pSendData, nSendLen, pRecvData, nRecvLen);
	LeaveCriticalSection(&m_CS);
	m_dwSndTime = timeGetTime();
	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary �� �������ݷ� ��� ����
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   pSendData : ������ Data �� ����ִ� Char �迭
@param   nSendLen : ������ Data ����
@param   pRecvData : �޴� Data �� ����ִ� Char �迭
@param   nRecvLen : �޴� Data ����
@return  Error Code
*/
//AOHC-216 AMC CommFail, CRC ������ ����
bool AMCLibrary::UpdateData(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount, int *_out_pErrCode)
{
    //AOHC-204 Thread �� ���� �ð� ���� ���
	static TimeChk timeChk;
	timeChk.End(0);

	// Thread Cyele Time ���
	static ThreadTimeMonitor tmon;
	tmon.Execute("AmcLibT", timeChk.Get(0));

	timeChk.Start(0);

	int nTmpError = NO_ERR;
	int nTmpFrontError = NO_ERR;
    int nSuccess = 0;
	static AMC_DATA PreRcvData; // ���� ������ AMC Data
	static DWORD dwAMCTrigerTime = timeGetTime();     //�Լ� ���� �ð�.
	static DWORD dwAMCTrigerTimeSend = timeGetTime();     //�Լ� ���� �ð�.
	static int  nAMCIgnorSendCount = 0;     //�Լ� ���� �ð�.
	static int	PreInput[4];
	static int	PreOutput[4];
	static int	PreNodeCount = 0;
	bool bCheckAMCLog = false;

	AMC_DATA RcvData;
	AMC_DATA TempData;
	memset(&TempData, 0x00, sizeof(AMC_DATA));
//  VOLTAGE_MONITOR_INFO_UNION VoltageUnion;

try{
    // by zzang9un 2013.07.09 : Mask���� ����
	//UINT Mask[4] = {0xF0000080, 0x00759FBF,0x00, 0x00};   // UBG B��
    UINT Mask[4] = {0x00, 0x00, 0x00, 0x00};

	// ���� �ʱ�ȭ
    nInputCount = MAX_IO_INT_COUNT;
	nOutCount = MAX_IO_INT_COUNT;

	memcpy(&Mask[0], &m_Input1Mask_R01, sizeof(m_Input1Mask_R01));
	memcpy(&Mask[1], &m_Input2Mask_R01, sizeof(m_Input2Mask_R01));
	memcpy(&Mask[2], &m_Input3Mask_R01, sizeof(m_Input3Mask_R01));

	if(m_bAMCReady == false)
	{
		*_out_pErrCode = ERR_AMC_NOTOPEN;
		return false;
	}

	if(m_AMCDllLibQueue.makeData(&m_SndCmd ,&nTmpError)==0)
	{
		*_out_pErrCode = nTmpError;
		return false;
	}

    if((m_SndCmd.Cmd&(0x3C))!=0)
    {
		// �� ���� ��� �α� �ۼ�
		for(int n=0; n<4;n++)
        {
			if(m_SndCmd.Cmd&(0x01<<(2+n)))
			{
			////timy ���ʿ��� LOG ����
//				AMC_ADD_LOG("AMC m_SndCmd:%o AXIS %d(%d::%d:%d:%d:%d)", m_SndCmd.Cmd, n, m_SndCmd.CmdAxis[n].CmdType,
//				m_SndCmd.CmdAxis[n].Accel, m_SndCmd.CmdAxis[n].Decel, m_SndCmd.CmdAxis[n].Speed, m_SndCmd.CmdAxis[n].Distance );
			}
		}
	}
    // Node Count ��� �α� �ۼ�
	if(m_SndCmd.Cmd&(1<<CMD_TYPE_CLEAR_NODE_COUNT))
	{
        AMC_ADD_LOG("AMC Clear Node Count:%o ", m_SndCmd.Cmd );
	}

	if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE))
	{
		AMC_ADD_LOG("AMC Set_Torque_Limit:%o ", m_SndCmd.Cmd );
	}

	if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE))
	{
		AMC_ADD_LOG("AMC Set_Torque_Limit_REALEASE:%o ", m_SndCmd.Cmd );
	}


    if(m_SndCmd.Cmd&(1<<CMD_TYPE_SET_DETECT_INFO))
	{
#if(USE_CHANGENODE == 1)
		AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d:%d]"
            , m_SndCmd.Cmd
            , (int)m_SndCmd.DetectInfo.UseFollowControlFlag
            , (int)m_SndCmd.DetectInfo.OHTDetectType
			, (int)m_SndCmd.DetectInfo.OBSDetectType
            , m_SndCmd.DetectInfo.NodeCount
			, m_SndCmd.DetectInfo.DistSumOfStraightPath);

		if(m_SndCmd.OptNodeInfo.DistON > 0)
		{
			AMC_ADD_LOG("Optional Node Info: [%d] [%d:%d] [%d:%d]"

				, m_SndCmd.OptNodeInfo.NodeType
				, (int)m_SndCmd.OptNodeInfo.OHTDetectTypeOpt
				, (int)m_SndCmd.OptNodeInfo.OBSDetectTypeOpt
				, m_SndCmd.OptNodeInfo.DistON
				, m_SndCmd.OptNodeInfo.DistAfterON);
		}
#else
		AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d]"
			, m_SndCmd.Cmd
			, (int)m_SndCmd.DetectInfo.UseFollowControlFlag
			, (int)m_SndCmd.DetectInfo.Type
			, m_SndCmd.DetectInfo.NodeCount
			, m_SndCmd.DetectInfo.DistSumOfStraightPath);
#endif
	}

	#ifdef USE_CRC_CHECK      // ����ο��� CRC ���� �ɶ� ifdef ���� ����
	m_SndCmd.MakeCRC16();	// Send Packet CRC �� ����
	#endif


	// AMC CALL ********************************************
	EnterCriticalSection(&m_CS);
	nSuccess = DeviceAMCGetDatas(&m_SndCmd, &RcvData);
	LeaveCriticalSection(&m_CS);

	#ifdef USE_CRC_CHECK      // ����ο��� CRC ���� �ɶ� ifdef ���� ����
	bool bCrcOK = RcvData.CrcCheck();	// Recv Packet CRC üũ
	#endif

	//AOHC-216 AMC CommFail, CRC ������ ����
	if(!nSuccess)
	{
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime)> 3000)
		{
			Get_LogDpram();
			m_bAMCReady = false;
			nAMCCommError = ERR_AMC_COMM;
			*_out_pErrCode = ERR_AMC_COMM;
		}
		AMC_ADD_LOG("[Fail]DeviceAMCGetDatas:%x ,%d", m_SndCmd.Cmd,nSuccess);
		OHTMainForm->GetStatusCheckThread()->AMCCommfail();
		return false;
	}

	// 2019.02.14 Null Data�� ������ CRC Check���� �ɸ��� ������ �ּ�ó����
	//
	//if(memcmp(&RcvData, &TempData, sizeof(AMC_DATA))==0)  // Null Check!
	//{
	//	nAMCCommError = ERR_AMC_RCVFORMAT;
	//	*_out_pErrCode = ERR_AMC_RCVFORMAT;
	//	return false;
	//}

	#ifdef USE_CRC_CHECK      // ����ο��� CRC ���� �ɶ� ifdef ���� ����
	// CRC ���� Case
	static int nCrcRetryCnt = 0;
	if(bCrcOK == false)
	{
		nCrcRetryCnt++;
		AMC_ADD_LOG("[CRC_ERROR] Retry :%d", nCrcRetryCnt);
		if(nCrcRetryCnt > 5)
		{
			nCrcRetryCnt = 0;
			AMC_ADD_LOG("[CRC_ERROR] Retry Count Over!!!");
			nAMCCommError = ERR_AMC_INVALID_RESPONSE;
			*_out_pErrCode = ERR_AMC_INVALID_RESPONSE;
			m_bAMCReady = false;
        }
		return false;
	}
	nCrcRetryCnt = 0;
	#endif

	// ������� ��� 0�ΰ��
	if(((RcvData.Output[0] ==0) &&(RcvData.Output[1] ==0)&& (RcvData.Output[2]==0) && (RcvData.Input[0]==0) &&(RcvData.Input[1]==0)&& (RcvData.Input[2]==0))
	|| ((RcvData.Output[0] ==0xFFFFFFFF) &&(RcvData.Output[1] ==0xFFFFFFFF)&& (RcvData.Output[2]==0xFFFFFFFF) && (RcvData.Input[0]==0xFFFFFFFF) &&(RcvData.Input[1]==0xFFFFFFFF)&& (RcvData.Input[2]==0xFFFFFFFF)))
   {
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime)> 50)
		{
			Get_LogDpram();
			nAMCCommError = ERR_AMC_COMM;
			*_out_pErrCode = ERR_AMC_COMM;
		}
		return false;
   }

	//AMC���� ���� NodeCount�� ������ ���������� ��� 2018.07.23 JRESPECT.IM
	if(RcvData.NodeCheckData.NodeCountOfPreCheck < 0 || RcvData.NodeCheckData.NodeCount < 0 || RcvData.NodeCheckData.NodeCount > MAX_NODE_COUNT || RcvData.NodeCheckData.NodeCountOfPreCheck > MAX_NODE_COUNT)
	{
		if(AbnormalNodeCount > 1)
		{
			AbnormalNodeCount = 0;
			AMC_ADD_LOG("AbnormalNodeNodeCount : %d %d", RcvData.NodeCheckData.NodeCountOfPreCheck , RcvData.NodeCheckData.NodeCount);
			*_out_pErrCode = ERR_AMC_RCVFORMAT1; //ErrorCode��ɷ� �ؾ����� �����
		}
		else
		{
			AbnormalNodeCount++;
		}

		return false;
	}

    // -------------------------------------------
	// [BEGIN] by zzang9un 2014.01.18 : AMC �۽� ��� Logging
	if(m_SndCmd.Cmd)
	{
		// Log ��� (100msec)

		if(OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTimeSend)> 100)
		{
#if(USE_CHANGENODE == 1)
				// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC SEND �α� �ۼ��� ���ǻ���
			// ���� ��궧���� �ݵ�� 10������ int�� ����ؾ���. 16����, �Ҽ���, �ȵ�!! ������ ��.
			char orgLog[2048], encLog[2048];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));
			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
				m_SndCmd.Cmd,
				m_SndCmd.Output[0], m_SndCmd.Output[1], m_SndCmd.Output[2], m_SndCmd.Output[3],
				m_SndCmd.CmdAxis[0].CmdType, m_SndCmd.CmdAxis[0].Accel, m_SndCmd.CmdAxis[0].Decel, m_SndCmd.CmdAxis[0].Speed, m_SndCmd.CmdAxis[0].Distance, m_SndCmd.CmdAxis[0].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[1].CmdType, m_SndCmd.CmdAxis[1].Accel, m_SndCmd.CmdAxis[1].Decel, m_SndCmd.CmdAxis[1].Speed, m_SndCmd.CmdAxis[1].Distance, m_SndCmd.CmdAxis[1].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[2].CmdType, m_SndCmd.CmdAxis[2].Accel, m_SndCmd.CmdAxis[2].Decel, m_SndCmd.CmdAxis[2].Speed, m_SndCmd.CmdAxis[2].Distance, m_SndCmd.CmdAxis[2].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[3].CmdType, m_SndCmd.CmdAxis[3].Accel, m_SndCmd.CmdAxis[3].Decel, m_SndCmd.CmdAxis[3].Speed, m_SndCmd.CmdAxis[3].Distance, m_SndCmd.CmdAxis[3].TriggerCtrlInfo,
				m_SndCmd.DetectInfo.UseFollowControlFlag,
				m_SndCmd.DetectInfo.OHTDetectType,
				m_SndCmd.DetectInfo.OBSDetectType,
				m_SndCmd.DetectInfo.NodeCount,
				m_SndCmd.DetectInfo.DistSumOfStraightPath,
				nAMCIgnorSendCount);
#else
				// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC SEND �α� �ۼ��� ���ǻ���
			// ���� ��궧���� �ݵ�� 10������ int�� ����ؾ���. 16����, �Ҽ���, �ȵ�!! ������ ��.
			char orgLog[2048], encLog[2048];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));
			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
				m_SndCmd.Cmd,
				m_SndCmd.Output[0], m_SndCmd.Output[1], m_SndCmd.Output[2], m_SndCmd.Output[3],
				m_SndCmd.CmdAxis[0].CmdType, m_SndCmd.CmdAxis[0].Accel, m_SndCmd.CmdAxis[0].Decel, m_SndCmd.CmdAxis[0].Speed, m_SndCmd.CmdAxis[0].Distance, m_SndCmd.CmdAxis[0].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[1].CmdType, m_SndCmd.CmdAxis[1].Accel, m_SndCmd.CmdAxis[1].Decel, m_SndCmd.CmdAxis[1].Speed, m_SndCmd.CmdAxis[1].Distance, m_SndCmd.CmdAxis[1].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[2].CmdType, m_SndCmd.CmdAxis[2].Accel, m_SndCmd.CmdAxis[2].Decel, m_SndCmd.CmdAxis[2].Speed, m_SndCmd.CmdAxis[2].Distance, m_SndCmd.CmdAxis[2].TriggerCtrlInfo,
				m_SndCmd.CmdAxis[3].CmdType, m_SndCmd.CmdAxis[3].Accel, m_SndCmd.CmdAxis[3].Decel, m_SndCmd.CmdAxis[3].Speed, m_SndCmd.CmdAxis[3].Distance, m_SndCmd.CmdAxis[3].TriggerCtrlInfo,
				m_SndCmd.DetectInfo.UseFollowControlFlag,
				m_SndCmd.DetectInfo.Type,
				m_SndCmd.DetectInfo.NodeCount,
				m_SndCmd.DetectInfo.DistSumOfStraightPath,
				nAMCIgnorSendCount);
#endif


			NumericSystem::EncodingLog(orgLog, encLog, sizeof(encLog));

            			//Null ���� �κ� ���� ó��. 20191204
			if(gm_LogManager!=NULL && m_pLogAmcSendPr!=NULL && gm_LogManager->Get_LogFolder())
			{
				m_pLogAmcSendPr->put(strlen(encLog), encLog);
			}

			//m_pLogAmcSendPr_Old->put(strlen(orgLog), orgLog);

                nAMCIgnorSendCount = 0;
				dwAMCTrigerTimeSend = timeGetTime();
		}else
			nAMCIgnorSendCount++;
	}
	// [END] by zzang9un 2014.01.18 : AMC �۽� ��� Logging
	// -------------------------------------------

	// ������ ������ ��� ��� �ʱ�ȭ
	memset(&m_SndCmd,0,sizeof(AMC_CMD));

	for(int nAxis=0; nAxis<4; nAxis++)
	{

		if((bUseAxis_Jig[nAxis] == false && bSetting_Jig == true)) // �ش����� �ƴϸ� ������ Aixs ���� (���׼���)
		{
			continue;
		}

		m_abIsRunning[nAxis]                =   (bool)RcvData.Running[nAxis];                   ///< ���� ������
		m_abInPosition[nAxis]               =   (bool)RcvData.InPosition[nAxis];                ///< ���� �Ϸ� ��������
		m_abAmpEnabled[nAxis]               =   (bool)RcvData.AmpEnable[nAxis];                 ///< AMP Enable ��������    : [�ӽ�]������ ��ȯ�ص�
//      m_abAmpEnabled[nAxis]               =   (bool)RcvData.Source[nAxis]&0x0800; 			///< AMP Enable ��������
		m_adPrePosition[nAxis]				= 	m_adCurrentPosition[nAxis];
		m_adCurrentPosition[nAxis]          =   (double)RcvData.Position[nAxis];                ///< ���� ��ġ: Cts
		m_adFinalPosition[nAxis]            =   (double)RcvData.FinalPos[nAxis];                ///< ���� ��ġ: Cts
		m_adCurrentVelocity[nAxis]          =   (double)RcvData.Velocity[nAxis]/1000.0;         ///< ���� �ӵ�: Cts/sec
		m_adFollowingError[nAxis]           =   (double)RcvData.FollowingError[nAxis];          ///< ���� Following Error :  Cts
		m_adPositionOfSmallAddCheck[nAxis]  =   (double)RcvData.PositionOfSmallAddCheck[nAxis]; ///< ���� ��ġ: Cts

		if (nAxis ==0)
		{
			m_FollowigAxis1 = RcvData.FollowingError[nAxis]; //AOHC-196 ���� �α� �߰�
		}

		m_anAxisError[nAxis] = CheckError(RcvData.Source[nAxis], RcvData.State[nAxis]); ///< �ະ ����

		if(m_anAxisError[nAxis] != NO_ERR)
		{
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						m_anAxisError[nAxis],
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}

		//�̵� �������� ����
		if(m_abIsRunning[nAxis] == true)
		{
			double dDelta = m_adCurrentPosition[nAxis] - m_adPrePosition[nAxis];

			//������� ���� ���̰� ������ ��� �������� �ʴ´�
			if(nAxis == AXIS_DRIVING_F || nAxis == AXIS_DRIVING_R)
			{
				if(dDelta > 0)
				{
					m_adTotalPosition[nAxis] += dDelta;
				}
			}
			else
			{
				m_adTotalPosition[nAxis] += fabs(dDelta);
			}
		}

		//��ɼ��� Ƚ�� ���
		if(m_abIsRunning[nAxis] == true)
		{
			//���谡 �ȵǾ��ٸ� ī��Ʈ �� �÷��� ����
			if(m_abIsCounted[nAxis] == false)
			{
				m_abIsCounted[nAxis] = true;
				m_anTotalMoveCount[nAxis] = m_anTotalMoveCount[nAxis]+1;
			}
		}
		else
		{
			//���谡 �Ȼ��°� ������� ������ �Ǹ� �÷��� ����
			if(m_abIsCounted[nAxis] == true)
			{
				m_abIsCounted[nAxis] = false;
			}
		}
		/*
		if(nAxis==1)    // Rear �� ���� ���� -- ������ ����
		{
			if(m_anAxisError[nAxis]==ERR_AXIS1_F_POS_LIMIT || m_anAxisError[nAxis]==ERR_AXIS1_F_NEG_LIMIT)
			{
				m_anAxisError[nAxis]=NO_ERR;
			}
		}
		//ERR_AXIS1_R_AMP_FAULT�߻��� ��� ERR_AXIS1_F_ABORT_EVENT�� Error�ö���� ��� ���� 2018.07.23 JRESPECT.IM
		if(nAxis == 0)  //1�� Error �߻��� ��� nTmpFrontError�� ����
		{
		  if(nTmpError == NO_ERR && m_anAxisError[nAxis] != NO_ERR)
		  {
            m_anAxisError[nAxis] += nAxis;
			nTmpFrontError = m_anAxisError[nAxis];
          }
        }
		*/
		if(nAxis > 0)   //2�� Error���� ������ Error ����
		{
			if(nTmpError == NO_ERR && m_anAxisError[nAxis] != NO_ERR)
			{
				//1�� Error X, ������ Error O
				if(nTmpFrontError == NO_ERR)
				{
					m_anAxisError[nAxis] += nAxis;
					nTmpError = m_anAxisError[nAxis];
				}
				//1�� Error O
				else //if(nTmpFrontError != NO_ERR)
				{
                    //1�� Error O, ������ Error O�� E614�� ���
					if(nTmpFrontError == ERR_AXIS1_F_ABORT_EVENT && m_anAxisError[nAxis] == ERR_AXIS1_F_AMP_FAULT)
					{
						m_anAxisError[nAxis] += nAxis;
						nTmpError = m_anAxisError[nAxis];
					}
					/*//1�� Error O, �������� Error O�� E614�� �ƴ� ���
					else if(m_anAxisError[nAxis] != ERR_AXIS1_F_AMP_FAULT)
					{
						nTmpError = nTmpFrontErrors;
					}
					*/
					//1�� Error O, �������� Error X
					else
					{
						nTmpError = nTmpFrontError;
                    }
                }
			}
		}

		if(nTmpError != NO_ERR)
		{
			ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						nTmpError,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}
	}

	for(int n=0; n<MAX_IO_INT_COUNT; n++)
	{
		m_anInput[n] = (RcvData.Input[n]^Mask[n]);
		m_anOutput[n] = RcvData.Output[n];
		pInputBuff[n] = (RcvData.Input[n]^Mask[n]);
		pOutputBuff[n] = RcvData.Output[n];

		if((PreInput[n] != RcvData.Input[n])
		 ||(PreOutput[n] != RcvData.Output[n]))
		 {
			bCheckAMCLog = true;
		 }

		PreInput[n] = RcvData.Input[n];
		PreOutput[n] = RcvData.Output[n];
	}

	nAMCPauseStatus = RcvData.PauseState;
	nAMCVHLDetectStatus = RcvData.AxisFlag2[2]; //0; ���� ���� ����, 1; ���� ���� ����(���������� ���� ������ ������)
	nAMCTorqueStatus = RcvData.AxisFlag2[3];   //1 : ��ũ���� ���� ����/ 2 : ��ũ���� ���� ����   0: �ʱⰪ

	nAMCVHLStopStatus = RcvData.TargetPos[0];   //
	nAMCDecelStatus = RcvData.TargetPos[1];

	nAMCDetectStopDecelDownStatus = RcvData.AxisFlag1[2];
	nAMCDetectForceIOStatus = RcvData.AxisFlag1[3];

	if(PreNodeCount !=  RcvData.NodeCheckData.NodeCount)
	{
		bCheckAMCLog = true;
		PreNodeCount  = RcvData.NodeCheckData.NodeCount;
	}

	//AMC���� ���� NodeCount������ ������ ��쿡�� ���� Data�� ���

	m_NodeCheckData.NodeCountOfPreCheck= RcvData.NodeCheckData.NodeCountOfPreCheck;
	m_NodeCheckData.NodeCount = RcvData.NodeCheckData.NodeCount;


    m_NodeCheckData.PositionOfDrivingTagPreCheck= (double)RcvData.NodeCheckData.PositionOfDrivingTagPreCheck;
    m_NodeCheckData.PositionOfDrivingTag= (double)RcvData.NodeCheckData.PositionOfDrivingTag;
    m_NodeCheckData.PositionOfTransTagFirst= (double)RcvData.NodeCheckData.PositionOfTransTagFirst;
    m_NodeCheckData.PositionOfTransTagFinal = (double)RcvData.NodeCheckData.PositionOfTransTagFinal;

#if 0
    VoltageUnion.Data = RcvData.VolatageMonitor;
#endif

	m_VolatageMonitor[0] =RcvData.VolatageMonitor[0];
	m_VolatageMonitor[1] =RcvData.VolatageMonitor[1];
#if 1

	// AMC �ֱ� ��� �α�
	if(memcmp(&PreRcvData, &RcvData, sizeof(AMC_DATA))!=0)  // ���� ���� �ٸ� ��츸 �α׸� �����.
	{
		// Log ��� (100msec)
		if((OHTUTILITY::PreventOverflowGetTimeMs(dwAMCTrigerTime) > 100) || (bCheckAMCLog ==true))
		{
			int AmpEnable = (RcvData.AmpEnable[0] << 0) + (RcvData.AmpEnable[1] << 1) + (RcvData.AmpEnable[2] << 2) + (RcvData.AmpEnable[3] << 3);
			int AmpRunning = (RcvData.Running[0] << 0) + (RcvData.Running[1] << 1) + (RcvData.Running[2] << 2) + (RcvData.Running[3] << 3);

			// LogVersion 2.0 SHCHO 2019.03 ************
			// AMC RECV �α� �ۼ��� ���ǻ���
			// ���� ��궧���� �ݵ�� 10������ int�� ����ؾ���. 16����, �Ҽ��� �ȵ�!! ���� ����.
			char orgLog[4096], encLog[4096];
			memset(orgLog, 0, sizeof(orgLog));
			memset(encLog, 0, sizeof(encLog));

			sprintf(orgLog, "/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d"
				, AmpEnable
				, RcvData.FollowingError[0],    RcvData.FollowingError[1],  RcvData.FollowingError[2],  RcvData.FollowingError[3]
				, RcvData.Position[0],          RcvData.Position[1],        RcvData.Position[2],        RcvData.Position[3]
				, AmpRunning
				, RcvData.Velocity[0],          RcvData.Velocity[1],        RcvData.Velocity[2],        RcvData.Velocity[3]
				, RcvData.Output[0],            RcvData.Output[1],          RcvData.Output[2]
				, RcvData.Input[0],				RcvData.Input[1],           RcvData.Input[2]
				, RcvData.NodeCheckData.NodeCount
				, RcvData.NodeCheckData.NodeCountOfPreCheck
				, RcvData.NodeCheckData.PositionOfDrivingTag
				, RcvData.NodeCheckData.PositionOfTransTagFirst
				, RcvData.NodeCheckData.PositionOfTransTagFinal
				, RcvData.NodeCheckData.PositionOfDrivingTagPreCheck
				, RcvData.VolatageMonitor[0]&0xFFFF, (RcvData.VolatageMonitor[0]>>16)&0xFFFF, RcvData.VolatageMonitor[1]&0xFFFF, (RcvData.VolatageMonitor[1]>>16)&0xFFFF
				, RcvData.PauseState
				, RcvData.Source[0], RcvData.Source[1], RcvData.Source[2], RcvData.Source[3], RcvData.State[0],RcvData.State[1]
			);
			// �α� 91�������� ��ȯ
			NumericSystem::EncodingLog(orgLog, encLog, sizeof(encLog));

			if(gm_LogManager!=NULL && m_pLogAmcRcv!=NULL && gm_LogManager->Get_LogFolder())
			{
				m_pLogAmcRcv->put(strlen(encLog), encLog);
			}

			// AMC RECV OLD �α�. �α� ���� ���ռ� Ȯ�εǸ� �����Ұ�
			dwAMCTrigerTime = timeGetTime();
		}
		PreRcvData = RcvData;
	}

#endif

	}catch(...)
	{
		AMC_ADD_LOG("AMC Exception!!!!");
		return false;
	}

	return true;

}

/**
@brief   AMCLibrary �� I/O Set�� �޴� �Լ�
@author  ehoto
@date    2012-11-01
@param   pSendData : ������ Data �� ����ִ� int �迭
@param   nSendLen : ������ Data ����
@param   pRecvData : �޴� Data �� ����ִ� int �迭
@param   nRecvLen : �޴� Data ����
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	for(int n=0; n<MAX_IO_INT_COUNT; n++)
	{
		pInputBuff[n] = m_anInput[n];
		pOutputBuff[n] = m_anOutput[n];
	}

	nInputCount = MAX_IO_INT_COUNT;
	nOutCount = MAX_IO_INT_COUNT;

	return NO_ERR;
}

/**
@brief   AMC�� Node Count�� Reset ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::ClearNodeCountInfo(void)
{
    int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.ClearNodeCount = true;
	AMC_ADD_LOG("ClearNodeCountInfo");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}



/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTorqueLimit(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetToreqLimit = true;
	AMC_ADD_LOG("SetTorqueLimit");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC�� ������ũ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTorqueLimitRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetToreqLimitRelease = true;
	AMC_ADD_LOG("SetTorqueLimitRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetSlopeTime(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetSlopeTime = true;
	AMC_ADD_LOG("SetSlopeTime");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC�� ������ũ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetSlopeTimeRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetSlopeTimeRelease = true;
	AMC_ADD_LOG("SetSlopeTimeRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}


/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTargetPos(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetTargetPos = true;

	AMC_ADD_LOG("SetTargetPos!!!!!!!!!!!!!!!!!!!");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC�� ������ũ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetTargetPosRelease(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetTargetPosRelease = true;

	AMC_ADD_LOG("SetTargetPosRelease!!!!");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetFirstNodeCountAdd(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetFirstNodeCountAdd = true;
	AMC_ADD_LOG("SetFirstNodeCountAdd");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetVHLDetectIgnor(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIgnor = true;
	AMC_ADD_LOG("SetVHLDetectIgnor");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   AMC�� ���� ��ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void AMCLibrary::SetVHLDetectIgnorRelease(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIgnorRelease = true;
	AMC_ADD_LOG("SetVHLDetectIgnorRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
//AOHC-196 ���� �α� �߰�
int	  AMCLibrary::GetFollowing_Axis1()
{
	return m_FollowigAxis1;
}
/**
@brief   ���� ������ ���ӵ� 0.5m/s^2 ����
@author  puting
@date    2019-04-21
*/
void AMCLibrary::SetDetectStopDecelDown(void)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectStopDecelDown = true;
	AMC_ADD_LOG("SetDetectStopDecelDown");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   ���� ������ ���ӵ� 0.5m/s^2 ���� ����
@author  puting
@date    2019-04-21
*/
void AMCLibrary::SetDetectStopDecelDownRelease(void)
{

	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectStopDecelDownRelease = true;
	AMC_ADD_LOG("SetDetectStopDecelDownRelease");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}
/**
@brief   ���������� ���� ���� ����
@author  puting
@date    2019-07-24
*/
void AMCLibrary::SetVHLDetectIOSet(bool bIoSet)
{
	int nTmpError = 0;
	QUEUEING_AMC_CMD QueuingAmcCmd;
	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetVHLDetectIOSet = bIoSet;
	AMC_ADD_LOG("SetVHLDetectIOSet");
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);
}

/**
@brief   Torque data ������ �޴� �Լ�
@author  kks
@date    2014-10-24
@param
@param
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetVHLDetectStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCVHLDetectStatus;
}

/**
@brief   ���� ������ ���� ���ӵ� ���� ���°�
@author  kks
@date    2019-04-21
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetDetectStopDecelDownStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDetectStopDecelDownStatus;
}
/**
@brief   ���� ���� ���� IO ���� ���� ���°�
@author  puting
@date    2019-07-25
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetVHLDetectForceIO()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDetectForceIOStatus;

}

/**
@brief   NodeCount �� ������ �޴� �Լ�
@author  ehoto
@date    2012-11-28
@param   nNodeCount : Node Count�� ���� �޴� ����
@param   dNodeDetectPosition : Node ������ ��ġ�� ���� �޴� ����
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData)
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	NodeCheckData = m_NodeCheckData;

	return NO_ERR;
}

/**
@brief   Pause data ������ �޴� �Լ�
@author  kks
@date    2014-10-24
@param   
@param
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetPauseStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCPauseStatus;
}

/**
@brief   ���а� ��� �Լ�
@author  KDS
@date    2017-11-29
@param
@param
@return  ���а� ����
*/
int AMCLibrary::GetVolatageMonitor(int Index)
{
	  return 	m_VolatageMonitor[Index];
}

/**
@brief   Torque data ������ �޴� �Լ�
@author  kks
@date    2014-10-24
@param
@param
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::GetTorqueStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCTorqueStatus;
}

/**
@brief   Stop State������ �޴� �Լ�
@author  put
@date    2018-09-10
@param
@param
@return  State
*/
int AMCLibrary::GetStopStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCVHLStopStatus;
}

/**
@brief   Decel State������ �޴� �Լ�
@author  put
@date    2019-10-22
@param
@param
@return  State
*/
int AMCLibrary::GetDecelStatusByAMC()
{
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	 return nAMCDecelStatus;
}


#if(USE_CHANGENODE == 1)
/**
@brief   DetectSensor�� ������ �����ϴ� �Լ�
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor �� ���� �����ϱ� ���� ������ ������ �ִ� ����ü
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo=true;
	QueuingAmcCmd.DataUnion.DetectInfo = DetectInfo;

	QueuingAmcCmd.DataUnion.OptNodeInfo = OptNodeInfo; // for change node (jhoun.lim)

//	QueuingAmcCmd.DataUnion.DetectInfo.UseFollowControlFlag = DetectInfo.UseFollowControlFlag;
//	QueuingAmcCmd.DataUnion.DetectInfo.Type = DetectInfo.Type;
//	QueuingAmcCmd.DataUnion.DetectInfo.NodeCount = DetectInfo.NodeCount;
//	QueuingAmcCmd.DataUnion.DetectInfo.DistSumOfStraightPath = DetectInfo.DistSumOfStraightPath;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d:%d]", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.OHTDetectType,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.OBSDetectType,
		QueuingAmcCmd.DataUnion.DetectInfo.NodeCount,
		QueuingAmcCmd.DataUnion.DetectInfo.FollowCommandType,
		QueuingAmcCmd.DataUnion.DetectInfo.PBSIgnoreFlag );

	// for change node (jhoun.lim)
	if(QueuingAmcCmd.DataUnion.OptNodeInfo.DistON > 0)
	{
		AMC_ADD_LOG("AMC Change Node Info: %d [%d:%d] [%d:%d]",
			QueuingAmcCmd.DataUnion.OptNodeInfo.NodeType,
			(int)QueuingAmcCmd.DataUnion.OptNodeInfo.OHTDetectTypeOpt,
			(int)QueuingAmcCmd.DataUnion.OptNodeInfo.OBSDetectTypeOpt,
			QueuingAmcCmd.DataUnion.OptNodeInfo.DistON,
			QueuingAmcCmd.DataUnion.OptNodeInfo.DistAfterON);
	}

	return ConvertAMCError(nTmpError);
}
#else
/**
@brief   DetectSensor�� ������ �����ϴ� �Լ�
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor �� ���� �����ϱ� ���� ������ ������ �ִ� ����ü
@return  Error Code ������ ��� NO_ERR(0)
*/
int AMCLibrary::SetDetectInfo(DETECT_INFO DetectInfo)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo=true;
	QueuingAmcCmd.DataUnion.DetectInfo = DetectInfo;
//	QueuingAmcCmd.DataUnion.DetectInfo.UseFollowControlFlag = DetectInfo.UseFollowControlFlag;
//	QueuingAmcCmd.DataUnion.DetectInfo.Type = DetectInfo.Type;
//	QueuingAmcCmd.DataUnion.DetectInfo.NodeCount = DetectInfo.NodeCount;
//	QueuingAmcCmd.DataUnion.DetectInfo.DistSumOfStraightPath = DetectInfo.DistSumOfStraightPath;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Detect Info:%o [%d:%d:%d:%d]", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.SetDetectInfo,
		(int)QueuingAmcCmd.DataUnion.DetectInfo.Type,
		QueuingAmcCmd.DataUnion.DetectInfo.NodeCount,
		QueuingAmcCmd.DataUnion.DetectInfo.FollowCommandType,
		QueuingAmcCmd.DataUnion.DetectInfo.PBSIgnoreFlag );

	return ConvertAMCError(nTmpError);
}

#endif

/**
@brief   CompensateSlavePos

	�޹����� �չ��� ���ڴ� ������ ������ ���ߴ� �Լ�
	���ķ� �ο� ���� ���� (Ư�� �ҷ� �̵� ��)
@author  ���¿�
@date    2013-03-12
*/
int AMCLibrary::CompensateSlavePos()
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.CompensateRearEncoder=true;

	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("CompensateSlavePos:%X", QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.CompensateRearEncoder);

	return ConvertAMCError(nTmpError);
}

int AMCLibrary::SetPauseControl(int bIsPause, int nCommander)
{
	int nTmpError = NO_ERR;
	QUEUEING_AMC_CMD QueuingAmcCmd;

	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	memset(&QueuingAmcCmd, 0, sizeof(QUEUEING_AMC_CMD));
	QueuingAmcCmd.CmdAnalysisUnion.CmdAnalysis.PauseControl = true;
	QueuingAmcCmd.DataUnion.PauseInfo.Commander = nCommander;
	QueuingAmcCmd.DataUnion.PauseInfo.Pause = bIsPause;
	
	m_AMCDllLibQueue.putData(QueuingAmcCmd , &nTmpError);

	AMC_ADD_LOG("AMC Pause Info:[%d:%d]", QueuingAmcCmd.DataUnion.PauseInfo.Commander,
		QueuingAmcCmd.DataUnion.PauseInfo.Pause	);

	return ConvertAMCError(nTmpError);
}

/**
@brief   AMCLibrary �� �������ݷ� ��� ����
@author  ehoto
@date    2012-11-01
@param   nAxisNum : �� ��ȣ (0~3)
@param   pSendData : ������ Data �� ����ִ� Char �迭
@param   nSendLen : ������ Data ����
@param   pRecvData : �޴� Data �� ����ִ� Char �迭
@param   nRecvLen : �޴� Data ����
@return  Error Code
*/
int AMCLibrary::CheckError(int Source, int State)
{
	int TmpError=0;

	// source Ȯ��
	if(Source & ST_HOME_SWITCH)				TmpError = int(ERR_AXIS1_F_HOME_SWITCH);
	else if(Source & ST_POS_LIMIT)			TmpError = int(ERR_AXIS1_F_POS_LIMIT);
	else if(Source & ST_NEG_LIMIT)			TmpError = int(ERR_AXIS1_F_NEG_LIMIT);
	else if(Source & ST_AMP_FAULT)			TmpError = int(ERR_AXIS1_F_AMP_FAULT);
//	else if(Source & ST_A_LIMIT) TmpError = int(ERR_AXIS1_F_A_LIMIT);
	else if(Source & ST_V_LIMIT)			TmpError = int(ERR_AXIS1_F_V_LIMIT);
//	else if((Source & ST_REVESE_MOVE) == ST_REVESE_MOVE) TmpError = int(ERR_AXIS1_F_REVERSE_MOVE);
	else if(Source & ST_X_NEG_LIMIT)		TmpError = int(ERR_AXIS1_F_X_NEG_LIMIT);
	else if(Source & ST_X_POS_LIMIT)		TmpError = int(ERR_AXIS1_F_X_POS_LIMIT);
	else if(Source & ST_ERROR_LIMIT)		TmpError = int(ERR_AXIS1_F_ERROR_LIMIT);
	else if(Source & ST_PC_COMMAND)			TmpError = int(ERR_AXIS1_F_PC_COMMAND);
	else if(Source & ST_OUT_OF_FRAMES)		TmpError = int(ERR_AXIS1_F_OUT_OF_FRAMES);
	else if(Source & ST_ABS_COMM_ERROR)		TmpError = int(ERR_AXIS1_F_ABS_COMM_ERROR);
	else if(Source & ST_INPOSITION_STATUS)	TmpError = int(ERR_AXIS1_F_INPOSITION_STATUS);
//	else if(Source & ST_RUN_STOP_COMMAND) TmpError = int(ERR_AXIS1_F_RUN_STOP_COMMAND);
	else if(Source & ST_COLLISION_STATE)	TmpError = int(ERR_AXIS1_F_COLLISION_STATE);

	// state Ȯ��
	else if(State==1)	TmpError = int(ERR_AXIS1_F_STOP_EVENT);
	else if(State==2)	TmpError = int(ERR_AXIS1_F_ESTOP_EVENT);
	else if(State==3)	TmpError = int(ERR_AXIS1_F_ABORT_EVENT);

	return TmpError;
}



//watchdog����
//---------------------------------------------------------------------------
int AMCLibrary::EnableWatchDog(void)
{
   int nTmpError=0;
   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

	//DeviceAMCEnableWatchdogReason(WDT_MAINLOOP); //���߿� �����   // main loop
	//DeviceAMCEnableWatchdogReason(WDT_EXTRA);    //���߿� �����   // 5ms loop
	//DeviceAMCEnableWatchdogReason(WDT_SUBCONTROL);                 // 1ms loop
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCEnableWatchdogReason(WDT_CONTROL);   // 200us loop

	LeaveCriticalSection(&m_CS);
   return ConvertAMCError(nTmpError);
}

//---------------------------------------------------------------------------
int AMCLibrary::DisableWatchDog(void)
{
    int nTmpError=0;
    if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
    EnterCriticalSection(&m_CS);
      //DeviceAMCDisableWatchdogReason(WDT_MAINLOOP); //���߿� �����     // main loop
      //DeviceAMCDisableWatchdogReason(WDT_EXTRA);    //���߿� �����     // 5ms loop
      //DeviceAMCDisableWatchdogReason(WDT_SUBCONTROL);                   // 1ms loop
	if(nTmpError==NO_ERR) nTmpError = DeviceAMCDisableWatchdogReason(WDT_CONTROL);     // 200us loop

	LeaveCriticalSection(&m_CS);
   return ConvertAMCError(nTmpError);
}
//---------------------------------------------------------------------------
// ������ (nVal)
//0x80 : ���� ȸ�� ������ INI ���ϰ� ��뺯���� �ٸ� ���
//0x40 : ���� Ÿ��(����ġ, ��ũ����Ż)�� �ٲ� ���
//0x20 : ���� ��ġ�� �۾� ������ ��� ���
//0x08 : control wdt. Event, 200us INT => ��� ����
//0x04 : sub-control wdt. Event, 1ms INT => ��� ����
//0x02 : extra wdt. Event, 10ms INT => DPRAM INT, �� ������ ���� �� ���
//0x01 :  main-loop wdt. Event, Main Loop => �� ������ ���� �� ���

//---------------------------------------------------------------------------
int AMCLibrary::GetWatchDogStatus(void)
{
   int nTmpError=0, nAMCError=0;

   unsigned int nVal=0;

   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

   nAMCError = DeviceAMCGetWatchdogStatus(&nVal);          // nVal=> bit0:main loop, 1:5ms, 2:1ms, 3:200us
   nTmpError = ConvertAMCError(nAMCError);

   if((nTmpError==NO_ERR) && (nVal==8)) //��� AMC���� ����..
   {
      nTmpError=ERR_AMC_WATCHDOG_STOP;
	  ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_AMC_WATCHDOG_STOP,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
   }
   else if(nVal!=0)
   {
      nAMCError = DeviceAMCSetWatchdogStatus(0);
   }

   LeaveCriticalSection(&m_CS);
   return nTmpError;
}

//---------------------------------------------------------------------------
int AMCLibrary::ClearWatchDogStatus(void)
{
   int nTmpError=0, nAMCError=0;
   unsigned int nVal=0;

   if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;
   EnterCriticalSection(&m_CS);

   nAMCError = DeviceAMCGetWatchdogStatus(&nVal);          // nVal=> bit0:main loop, 1:5ms, 2:1ms, 3:200us
   if((nAMCError==NO_ERR) && (nVal!=0))
   {
      nAMCError = DeviceAMCSetWatchdogStatus(0);         // bit0:main loop, 1:5ms, 2:1ms, 3:200us
      nTmpError = ConvertAMCError(nAMCError);
   }

   LeaveCriticalSection(&m_CS);
   return nTmpError;
}
//---------------------------------------------------------------------------
void AMCLibrary::makeTraceLog()
{
	AMC_ADD_LOG("Trace Start");

	DeviceAMCGatheringTraceData();
	DeviceAMCGatheringTraceData_1();

	AMC_ADD_LOG("Trace End");
}
//---------------------------------------------------------------------------

/**
@brief   Input Data�� ������ �Ǵ��ϱ� ���� Mask ������ �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.09
@note    ������ A(Normal Open), B(Normal Close) ������ ���� �����Ѵ�(A���� 0, B������ 1)
*/
void AMCLibrary::InputMaskInit(void)
{
#if ((OHT_NAME == OHT_NAME_STD_V80))
	// Input 1 ����
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;

	m_Input1Mask_R01.HOIST_Home              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 ����
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;


	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.RailSupport_Home        = 0;    //
	m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;
	//tg �߰�
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Outtrigger_F_Alarm_IN   = 0;
	m_Input3Mask_R01.Outtrigger_F_Home_IN    = 0;
	m_Input3Mask_R01.Outtrigger_F_Work_IN    = 0;
	m_Input3Mask_R01.Outtrigger_R_Alarm_IN   = 0;

	m_Input3Mask_R01.Outtrigger_R_Home_IN    = 0;

	m_Input3Mask_R01.Outtrigger_R_Work_IN    = 0;
	m_Input3Mask_R01.Rotate_L_Limit          = 0;
	m_Input3Mask_R01.Rotate_R_Limit          = 0;
	m_Input3Mask_R01.Rotate_Inposition       = 0;
	m_Input3Mask_R01.Rotate_Alarm            = 0;

#elif ((OHT_NAME == OHT_NAME_STD_V81))     // JIG INPUT IO Timy
	// Input 1 ����
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;
	m_Input1Mask_R01.Reserved_7   			 = 0;	//m_Input1Mask_R01.HOIST_Home              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.ROTATE_Inposition		 = 0;  	//m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.ROTATE_Alarm			 = 0;   //m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.ROTATE_90				 = 0;   //m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 ����
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;


	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.ROTATE_0				 = 0; //m_Input2Mask_R01.RailSupport_Home        = 0;
	m_Input2Mask_R01.ROTATE_180				 = 0; //m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.ROTATE_270				 = 0; //m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;

	//tg �߰�
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Reserved_70   		= 0;
	m_Input3Mask_R01.Reserved_71    	= 0;
	m_Input3Mask_R01.Reserved_72   		= 0;
	m_Input3Mask_R01.Reserved_73    	= 0;
	m_Input3Mask_R01.HOIST_Home              = 1; //m_Input3Mask_R01.Reserved_74    	= 0;
	m_Input3Mask_R01.Reserved_75    	= 0;
	m_Input3Mask_R01.Reserved_76          = 0;
	m_Input3Mask_R01.EQPIO_Load_REQ          = 0;        //�ӽ� IO ���� �����Ϲ�����
	m_Input3Mask_R01.EQPIO_Unload_REQ       = 0;
	m_Input3Mask_R01.EQPIO_Ready            = 0;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	// Input 1 ����
	m_Input1Mask_R01.HAND_Inposition         = 0;
	m_Input1Mask_R01.HAND_Alarm              = 0;
	m_Input1Mask_R01.HAND_Right_Grip         = 0;
	m_Input1Mask_R01.HAND_Left_Grip          = 0;
	m_Input1Mask_R01.HAND_Close              = 0;
	m_Input1Mask_R01.HAND_Open               = 0;
	m_Input1Mask_R01.HAND_Push               = 0;
	m_Input1Mask_R01.Reserved_7              = 1;

	m_Input1Mask_R01.HAND_PIO_Go             = 0;
	m_Input1Mask_R01.EQPIO_Load_REQ          = 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ        = 0;
	m_Input1Mask_R01.EQPIO_Ready             = 0;
	m_Input1Mask_R01.EQPIO_HO_AVBL           = 0;
	m_Input1Mask_R01.EQPIO_ES                = 0;
	m_Input1Mask_R01.EQPIO_Left_Go           = 0;
	m_Input1Mask_R01.EQPIO_Right_Go          = 0;

	m_Input1Mask_R01.STEER_F_Left            = 0;
	m_Input1Mask_R01.STEER_R_Left            = 0;
	m_Input1Mask_R01.STEER_F_Right           = 0;
	m_Input1Mask_R01.STEER_R_Right           = 0;
	m_Input1Mask_R01.SHUTTER_F_Close         = 0;
	m_Input1Mask_R01.SHUTTER_R_Close         = 0;
	m_Input1Mask_R01.SHUTTER_F_Open          = 0;
	m_Input1Mask_R01.SHUTTER_R_Open          = 0;

	m_Input1Mask_R01.SHUTTER_F_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm         = 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition    = 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition    = 0;
	m_Input1Mask_R01.OBSTACLE_Long           = 1;
	m_Input1Mask_R01.OBSTACLE_Middle         = 1;
	m_Input1Mask_R01.OBSTACLE_Short          = 1;
	m_Input1Mask_R01.OBSTACLE_Fail           = 1;

    // Input 2 ����
	m_Input2Mask_R01.OHTDETECT_Long          = 1;
	m_Input2Mask_R01.OHTDETECT_Middle        = 1;
	m_Input2Mask_R01.OHTDETECT_Short         = 1;
	m_Input2Mask_R01.OHTDETECT_Fail          = 1;
	m_Input2Mask_R01.LOOKDOWN_Detect         = 1;
	m_Input2Mask_R01.LOOKDOWN_Error          = 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;

	m_Input2Mask_R01.STB_Detect_Left         = 1;

	m_Input2Mask_R01.STB_Detect_Right        = 1;
	m_Input2Mask_R01.UTB_Detect              = 1;
	m_Input2Mask_R01.FOUP_Detect             = 1;
	m_Input2Mask_R01.OSCILLATION             = 0;
	m_Input2Mask_R01.EMO_Press               = 1;
	m_Input2Mask_R01.BUMPER_Press            = 0;
	m_Input2Mask_R01.FAN_Fail                = 1;
	m_Input2Mask_R01.Reserved_48     		 = 1;

	m_Input2Mask_R01.MC_Status    			 = 0;
	m_Input2Mask_R01.HANDTP_Connect          = 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect  = 1;
	m_Input2Mask_R01.STOP_Driving_Front      = 1;
	m_Input2Mask_R01.STOP_Driving_PreCheck   = 1;
	m_Input2Mask_R01.STOP_Trans_Front        = 1;
	m_Input2Mask_R01.STOP_Trans_Rear         = 1;
	m_Input2Mask_R01.BCR_Driving_OK          = 0;

	m_Input2Mask_R01.BCR_Driving_NG          = 0;
	m_Input2Mask_R01.BCR_Trans_OK            = 0;
	m_Input2Mask_R01.BCR_Trans_NG            = 0;
	m_Input2Mask_R01.HOIST_Belt_Detect       = 1;
	m_Input2Mask_R01.RailSupport_Home        = 0;    //
	m_Input2Mask_R01.Railsupport_Work        = 0;
	m_Input2Mask_R01.MAC_Detect              = 1;
	m_Input2Mask_R01.SMPS_Short 			 = 1;
	//tg �߰�
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN 	     = 0;
	m_Input3Mask_R01.CID_Completion_ACK	     = 0;
	m_Input3Mask_R01.CID_Status_IN           = 0;
	m_Input3Mask_R01.CID_Reserved_IN1        = 0;
	m_Input3Mask_R01.CID_Reserved_IN2        = 0;
	m_Input3Mask_R01.Rotate_0_Sensor		    = 0;
	m_Input3Mask_R01.Rotate_90_Sensor	    = 0;
	m_Input3Mask_R01.Rotate_180_Sensor	    = 0;
	m_Input3Mask_R01.Rotate_270_Sensor	    = 0;
	m_Input3Mask_R01.HOIST_Home              = 1;
	m_Input3Mask_R01.Reserved_75             = 0;
	m_Input3Mask_R01.Rotate_L_Limit          = 0;
	m_Input3Mask_R01.Rotate_R_Limit          = 0;
	m_Input3Mask_R01.Rotate_Inposition       = 0;
	m_Input3Mask_R01.Rotate_Alarm            = 0;


#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	// ========== Input 1 ���� ==========

	// 0 - 15
	m_Input1Mask_R01.HAND_Inposition 		= 0;
	m_Input1Mask_R01.HAND_Alarm				= 0;
	m_Input1Mask_R01.HAND_Right_Grip 		= 0;
	m_Input1Mask_R01.HAND_Left_Grip			= 0;
	
	m_Input1Mask_R01.HAND_Close				= 0;
	m_Input1Mask_R01.HAND_Open				= 0;
	m_Input1Mask_R01.HAND_Push				= 0;
	m_Input1Mask_R01.HOIST_Align  			= 1;

	m_Input1Mask_R01.HAND_PIO_Go 			= 0;
	m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
	m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
	m_Input1Mask_R01.EQPIO_Ready 			= 0;
	
	m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
	m_Input1Mask_R01.EQPIO_ES				= 0;
	m_Input1Mask_R01.EQPIO_Right_Go			= 0;
	m_Input1Mask_R01.EQPIO_Left_Go			= 0;

	// 16 - 31
	m_Input1Mask_R01.STEER_F_Left			= 0;
	m_Input1Mask_R01.STEER_R_Left			= 0;
	m_Input1Mask_R01.STEER_F_Right			= 0;
	m_Input1Mask_R01.STEER_R_Right			= 0;
	
	m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
	m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
	m_Input1Mask_R01.SHUTTER_F_Open			= 0;
	m_Input1Mask_R01.SHUTTER_R_Open			= 0;
	
	m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
	m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
	m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
	m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;
	
	m_Input1Mask_R01.OBSTACLE_Long			= 1;
	m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
	m_Input1Mask_R01.OBSTACLE_Short			= 1;
	m_Input1Mask_R01.OBSTACLE_Fail			= 1;

	
	// ========== Input 2 ���� ==========
	// 0 - 15

	m_Input2Mask_R01.OHTDETECT_Long			= 1;
	m_Input2Mask_R01.OHTDETECT_Middle		= 1;
	m_Input2Mask_R01.OHTDETECT_Short 		= 1;
	m_Input2Mask_R01.OHTDETECT_Fail			= 1;
	
	m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
	m_Input2Mask_R01.LOOKDOWN_Error			= 1;

	m_Input2Mask_R01.SHIFT_Home              = 0;

	m_Input2Mask_R01.STB_Detect_Left 		= 1;
	
	m_Input2Mask_R01.STB_Detect_Right		= 1;
	m_Input2Mask_R01.UTB_Detect				= 1;
	m_Input2Mask_R01.FOUP_Detect 			= 1;
	m_Input2Mask_R01.OSCILLATION 			= 0;
	
	m_Input2Mask_R01.EMO_Press				= 1;
	m_Input2Mask_R01.BUMPER_Press			= 0;
	m_Input2Mask_R01.FAN_Fail				= 1;
	m_Input2Mask_R01.HOIST_Belt 			 	= 1;

	// 16 - 31
	m_Input2Mask_R01.MC_Status				= 0;
	m_Input2Mask_R01.HANDTP_Connect			= 0;
	m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
	m_Input2Mask_R01.STOP_Driving_Front		= 1;
	
	m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
	m_Input2Mask_R01.STOP_Trans_Front		= 1;
	m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
	m_Input2Mask_R01.BCR_Driving_OK			= 0;
	
	m_Input2Mask_R01.BCR_Driving_NG			= 0;
	m_Input2Mask_R01.BCR_Trans_OK			= 0;
	m_Input2Mask_R01.BCR_Trans_NG			= 0;
	m_Input2Mask_R01.HOIST_Belt_Detect		= 1;

	m_Input2Mask_R01.PIO_Input3				= 0;
	m_Input2Mask_R01.PIO_Input5				= 0;
	m_Input2Mask_R01.PIO_Input6				= 0;
	m_Input2Mask_R01.SMPS_Short				= 1;

	// ========== tg �߰� ==========
	m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
	m_Input3Mask_R01.CID_Interlock_IN		 = 0;
	m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
	m_Input3Mask_R01.CID_Status_IN			 = 0;
	m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
	m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

	// ========= Input 3 ���� ==========
	// 0 - 15
	m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
	m_Input3Mask_R01.CID_Interlock_IN		= 0;
	m_Input3Mask_R01.CID_Completion_ACK		= 0;
	m_Input3Mask_R01.CID_Status_IN			= 0;
	
	m_Input3Mask_R01.CID_Reserved_IN1		= 0;
	m_Input3Mask_R01.CID_Reserved_IN2		= 0;
	m_Input3Mask_R01.Rotate_0_Sensor			= 0;
	m_Input3Mask_R01.Rotate_90_Sensor		= 0;

	m_Input3Mask_R01.Rotate_180_Sensor		= 0;
	m_Input3Mask_R01.Rotate_270_Sensor		= 0;
	m_Input3Mask_R01.HOIST_Home				= 1;
	m_Input3Mask_R01.SHUTTER_F_Middle_Close 	= 0;
	
	m_Input3Mask_R01.SHUTTER_R_Middle_Close 	= 0;
	m_Input3Mask_R01.POD_EUV_1				= 1;
	m_Input3Mask_R01.POD_EUV_2				= 0;
	m_Input3Mask_R01.SPARE_5					= 0;

	// 16 - 31
	m_Input3Mask_R01.SPARE_6					= 0;
	m_Input3Mask_R01.SPARE_7					= 0;
	m_Input3Mask_R01.SPARE_8					= 0;
	m_Input3Mask_R01.SPARE_9					= 0;
	
	m_Input3Mask_R01.SPARE_10				= 0;
	m_Input3Mask_R01.SPARE_11				= 0;
	m_Input3Mask_R01.SPARE_12				= 0;
	m_Input3Mask_R01.SPARE_13				= 0;
	
	m_Input3Mask_R01.SPARE_14 				= 0;
	m_Input3Mask_R01.SPARE_15 				= 0;
	m_Input3Mask_R01.SPARE_16 				= 0;
	m_Input3Mask_R01.SPARE_17 				= 0;
	m_Input3Mask_R01.SPARE_18 				= 0;
	m_Input3Mask_R01.SPARE_19 				= 0;


#elif(OHT_NAME == OHT_NAME_STD_V85)
// ========== Input 1 ���� ==========

// 0 - 15
m_Input1Mask_R01.HAND_Inposition 		= 0;
m_Input1Mask_R01.HAND_Alarm				= 0;
m_Input1Mask_R01.HAND_Right_Grip 		= 0;
m_Input1Mask_R01.HAND_Left_Grip			= 0;

m_Input1Mask_R01.HAND_Close				= 0;
m_Input1Mask_R01.HAND_Open				= 0;
m_Input1Mask_R01.HAND_Push				= 0;
m_Input1Mask_R01.HNAD_Reserved_1  		= 1;
m_Input1Mask_R01.HNAD_ES  				= 1;

m_Input1Mask_R01.HAND_PIO_Go 			= 0;
m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
m_Input1Mask_R01.EQPIO_Ready 			= 0;

m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
m_Input1Mask_R01.EQPIO_ES				= 0;
m_Input1Mask_R01.EQPIO_Left_Go			= 0;

// 16 - 31
m_Input1Mask_R01.STEER_F_Left			= 0;
m_Input1Mask_R01.STEER_R_Left			= 0;
m_Input1Mask_R01.STEER_F_Right			= 0;
m_Input1Mask_R01.STEER_R_Right			= 0;

m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
m_Input1Mask_R01.SHUTTER_F_Open			= 0;
m_Input1Mask_R01.SHUTTER_R_Open			= 0;

m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;

m_Input1Mask_R01.OBSTACLE_Input_Extend	= 1;
m_Input1Mask_R01.OBSTACLE_Long			= 1;
m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
m_Input1Mask_R01.OBSTACLE_Short			= 1;
m_Input2Mask_R01.OBSTACLE_Fail			= 1;


// ========== Input 2 ���� ==========
// 0 - 15
m_Input2Mask_R01.OHTDETECT_Input_Extend	= 1;
m_Input2Mask_R01.OHTDETECT_Long			= 1;
m_Input2Mask_R01.OHTDETECT_Middle		= 1;
m_Input2Mask_R01.OHTDETECT_Short 		= 1;
m_Input2Mask_R01.OHTDETECT_Fail			= 1;

m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
m_Input2Mask_R01.LOOKDOWN_Error			= 1;
m_Input2Mask_R01.SHIFT_Home             = 0;
m_Input2Mask_R01.STB_Detect_Left 		= 1;
m_Input2Mask_R01.STB_Detect_Right		= 1;
m_Input2Mask_R01.UTB_Detect				= 1;
m_Input2Mask_R01.FOUP_Detect 			= 1;
m_Input2Mask_R01.OSCILLATION 			= 0;

m_Input2Mask_R01.EMO_Press				= 1;
m_Input2Mask_R01.BUMPER_Press			= 0;
m_Input2Mask_R01.FAN_Fail				= 1;
m_Input2Mask_R01.HOIST_Belt_Detect1 	= 1;

// 16 - 31
m_Input2Mask_R01.HOIST_Belt_Detect2 	= 1;
m_Input2Mask_R01.TP_Enable				= 0;
m_Input2Mask_R01.HANDTP_Connect			= 0;
m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
m_Input2Mask_R01.STOP_Driving_Front		= 1;

m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
m_Input2Mask_R01.STOP_Trans_Front		= 1;
m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
m_Input2Mask_R01.BCR_Driving_OK			= 0;

m_Input2Mask_R01.BCR_Driving_NG			= 0;
m_Input2Mask_R01.BCR_Trans_OK			= 0;
m_Input2Mask_R01.BCR_Trans_NG			= 0;
m_Input2Mask_R01.Reserved_1				= 0;

m_Input2Mask_R01.Reserved_2				= 0;

m_Input3Mask_R01.Reserved_3				= 0;
m_Input3Mask_R01.SMPS_Short				= 1;

// ========== tg �߰� ==========
m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
m_Input3Mask_R01.CID_Interlock_IN		 = 0;
m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
m_Input3Mask_R01.CID_Status_IN			 = 0;
m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

// ========= Input 3 ���� ==========
// 0 - 15
m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
m_Input3Mask_R01.CID_Interlock_IN		= 0;
m_Input3Mask_R01.CID_Completion_ACK		= 0;
m_Input3Mask_R01.CID_Status_IN			= 0;

m_Input3Mask_R01.CID_Reserved_IN1		= 0;
m_Input3Mask_R01.CID_Reserved_IN2		= 0;
m_Input3Mask_R01.Rotate_0_Sensor		= 0;
m_Input3Mask_R01.Rotate_90_Sensor		= 0;

m_Input3Mask_R01.Rotate_180_Sensor		= 0;
m_Input3Mask_R01.Rotate_270_Sensor		= 0;
m_Input3Mask_R01.HOIST_Home				= 1;
m_Input3Mask_R01.HOIST_Home_2			= 1;
m_Input3Mask_R01.SHUTTER_F_Middle_Close = 0;

m_Input3Mask_R01.SHUTTER_R_Middle_Close = 0;
m_Input3Mask_R01.POD_EUV_1				= 1;
m_Input3Mask_R01.POD_EUV_2				= 0;
m_Input3Mask_R01.SPARE_5				= 0;

// 16 - 31
m_Input3Mask_R01.SPARE_6					= 0;
m_Input3Mask_R01.SPARE_7					= 0;
m_Input3Mask_R01.SPARE_8					= 0;
m_Input3Mask_R01.SPARE_9					= 0;

m_Input3Mask_R01.SPARE_10				= 0;
m_Input3Mask_R01.SPARE_11				= 0;
m_Input3Mask_R01.SPARE_12				= 0;
m_Input3Mask_R01.SPARE_13				= 0;

m_Input3Mask_R01.SPARE_14 				= 0;
m_Input3Mask_R01.SPARE_15 				= 0;
m_Input3Mask_R01.SPARE_16 				= 0;
m_Input3Mask_R01.SPARE_17 				= 0;
m_Input3Mask_R01.SPARE_18 				= 0;

#elif(OHT_NAME == OHT_NAME_STD_V81S)
// ========== Input 1 ���� ==========

// 0 - 15
m_Input1Mask_R01.HAND_Inposition 		= 0;
m_Input1Mask_R01.HAND_Alarm				= 0;
m_Input1Mask_R01.HAND_Right_Grip 		= 0;
m_Input1Mask_R01.HAND_Left_Grip			= 0;

m_Input1Mask_R01.HAND_Close				= 0;
m_Input1Mask_R01.HAND_Open				= 0;
m_Input1Mask_R01.HAND_Push				= 0;
m_Input1Mask_R01.HOIST_Home	  			= 1;
m_Input1Mask_R01.HNAD_ES  				= 1;

m_Input1Mask_R01.HAND_PIO_Go 			= 0;
m_Input1Mask_R01.EQPIO_Load_REQ			= 0;
m_Input1Mask_R01.EQPIO_Unload_REQ		= 0;
m_Input1Mask_R01.EQPIO_Ready 			= 0;

m_Input1Mask_R01.EQPIO_HO_AVBL			= 0;
m_Input1Mask_R01.EQPIO_ES				= 0;
m_Input1Mask_R01.EQPIO_Left_Go			= 0;

// 16 - 31
m_Input1Mask_R01.STEER_F_Left			= 0;
m_Input1Mask_R01.STEER_R_Left			= 0;
m_Input1Mask_R01.STEER_F_Right			= 0;
m_Input1Mask_R01.STEER_R_Right			= 0;

m_Input1Mask_R01.SHUTTER_F_Close 		= 0;
m_Input1Mask_R01.SHUTTER_R_Close 		= 0;
m_Input1Mask_R01.SHUTTER_F_Open			= 0;
m_Input1Mask_R01.SHUTTER_R_Open			= 0;

m_Input1Mask_R01.SHUTTER_F_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_R_Alarm 		= 0;
m_Input1Mask_R01.SHUTTER_F_Inposition	= 0;
m_Input1Mask_R01.SHUTTER_R_Inposition	= 0;

m_Input1Mask_R01.OBSTACLE_Input_Extend	= 1;
m_Input1Mask_R01.OBSTACLE_Long			= 1;
m_Input1Mask_R01.OBSTACLE_Middle 		= 1;
m_Input1Mask_R01.OBSTACLE_Short			= 1;
m_Input2Mask_R01.OBSTACLE_Fail			= 1;


// ========== Input 2 ���� ==========
// 0 - 15
m_Input2Mask_R01.OHTDETECT_Input_Extend	= 1;
m_Input2Mask_R01.OHTDETECT_Long			= 1;
m_Input2Mask_R01.OHTDETECT_Middle		= 1;
m_Input2Mask_R01.OHTDETECT_Short 		= 1;
m_Input2Mask_R01.OHTDETECT_Fail			= 1;

m_Input2Mask_R01.LOOKDOWN_Detect 		= 1;
m_Input2Mask_R01.LOOKDOWN_Error			= 1;
m_Input2Mask_R01.SHIFT_Home             = 0;
m_Input2Mask_R01.STB_Detect_Left 		= 1;
m_Input2Mask_R01.STB_Detect_Right		= 1;
m_Input2Mask_R01.UTB_Detect				= 1;
m_Input2Mask_R01.FOUP_Detect 			= 1;
m_Input2Mask_R01.OSCILLATION 			= 0;

m_Input2Mask_R01.EMO_Press				= 1;
m_Input2Mask_R01.BUMPER_Press			= 0;
m_Input2Mask_R01.FAN_Fail				= 1;
m_Input2Mask_R01.HOIST_Belt_Detect1 	= 1;

// 16 - 31
m_Input2Mask_R01.HOIST_Belt_Detect2 	= 1;
m_Input2Mask_R01.TP_Enable				= 0;
m_Input2Mask_R01.HANDTP_Connect			= 0;
m_Input2Mask_R01.FOUP_Cover_Open_Detect	= 1;
m_Input2Mask_R01.STOP_Driving_Front		= 1;

m_Input2Mask_R01.STOP_Driving_PreCheck	= 1;
m_Input2Mask_R01.STOP_Trans_Front		= 1;
m_Input2Mask_R01.STOP_Trans_Rear 		= 1;
m_Input2Mask_R01.BCR_Driving_OK			= 0;

m_Input2Mask_R01.BCR_Driving_NG			= 0;
m_Input2Mask_R01.BCR_Trans_OK			= 0;
m_Input2Mask_R01.BCR_Trans_NG			= 0;
m_Input2Mask_R01.Reserved_1				= 0;

m_Input2Mask_R01.Reserved_2				= 0;

m_Input3Mask_R01.Reserved_3				= 0;
m_Input3Mask_R01.SMPS_Short				= 1;

// ========== tg �߰� ==========
m_Input3Mask_R01.CID_Link_Compt_IN		 = 0;
m_Input3Mask_R01.CID_Interlock_IN		 = 0;
m_Input3Mask_R01.CID_Completion_ACK 	 = 0;
m_Input3Mask_R01.CID_Status_IN			 = 0;
m_Input3Mask_R01.CID_Reserved_IN1		 = 0;
m_Input3Mask_R01.CID_Reserved_IN2		 = 0;

// ========= Input 3 ���� ==========
// 0 - 15
m_Input3Mask_R01.CID_Link_Compt_IN		= 0;
m_Input3Mask_R01.CID_Interlock_IN		= 0;
m_Input3Mask_R01.CID_Completion_ACK		= 0;
m_Input3Mask_R01.CID_Status_IN			= 0;

m_Input3Mask_R01.CID_Reserved_IN1		= 0;
m_Input3Mask_R01.CID_Reserved_IN2		= 0;
m_Input3Mask_R01.Rotate_0_Sensor		= 0;
m_Input3Mask_R01.Rotate_90_Sensor		= 0;

m_Input3Mask_R01.Rotate_180_Sensor		= 0;
m_Input3Mask_R01.Rotate_270_Sensor		= 0;
m_Input3Mask_R01.HNAD_Reserved_1	  	= 1;
m_Input3Mask_R01.HOIST_Home_2			= 1;
m_Input3Mask_R01.SHUTTER_F_Middle_Close = 0;

m_Input3Mask_R01.SHUTTER_R_Middle_Close = 0;
m_Input3Mask_R01.POD_EUV_1				= 1;
m_Input3Mask_R01.POD_EUV_2				= 0;
m_Input3Mask_R01.SPARE_5				= 0;

// 16 - 31
m_Input3Mask_R01.SPARE_6					= 0;
m_Input3Mask_R01.SPARE_7					= 0;
m_Input3Mask_R01.SPARE_8					= 0;
m_Input3Mask_R01.SPARE_9					= 0;

m_Input3Mask_R01.SPARE_10				= 0;
m_Input3Mask_R01.SPARE_11				= 0;
m_Input3Mask_R01.SPARE_12				= 0;
m_Input3Mask_R01.SPARE_13				= 0;

m_Input3Mask_R01.SPARE_14 				= 0;
m_Input3Mask_R01.SPARE_15 				= 0;
m_Input3Mask_R01.SPARE_16 				= 0;
m_Input3Mask_R01.SPARE_17 				= 0;
m_Input3Mask_R01.SPARE_18 				= 0;
#endif
}

//---------------------------------------------------------------------------

/**
@brief   AMC�� ������ ������ �Լ�
@author  zzang9un
@date    2013.10.07
@return  AMC�� ���� ����(int)�� ����
*/
int AMCLibrary::GetAMCVersion()
{
	return nAMCVer[0];
}
//---------------------------------------------------------------------------
/**
@brief   FPGA�� ������ ������ �Լ�
@author  zzang9un
@date    2013.10.07
@return  AMC�� ���� ����(int)�� ����
*/
int AMCLibrary::GetFPGAVersion()
{
	return (int)wFPGAVersion;
}
/**
@brief   AMC�� FileSystem ��ü�� �����ϱ� ���� �Լ�
@author  ji7412.lee
@date    2014.02.10
@return
*/
int AMCLibrary::AMCFileSystemForamt()
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	nAMCError = DeviceAMCFormat();
	nTmpError = ConvertAMCError(nAMCError);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC FileSystemFormat Error:%d", nTmpError);

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� Drive������ ������ Set/Reset ��Ű�� �Լ�
@author  ji7412.lee
@date    2014.02.10
@note    v6.5���� AMCUpdate�����Ҷ� DriverOpen Flag�� Reset��Ŵ.
*/
int AMCLibrary::SetAMCDriverOpenByCommon(bool isOnOff)
{
	int nTmpError = 0;

	if(isOnOff)
		m_bDriverOpen = true;
	else
		m_bDriverOpen = false;

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DonwLoadFile Flag�� Set ��Ű�� �Լ�
@author  ji7412.lee
@date    2014.02.10
@note    v6.5���� AMCUpdate����ɶ� FileDownloaded Flag�� Reset��Ŵ.
*/
int AMCLibrary::SetAMCFileDownloaded(bool isOnOff)
{
	int nTmpError = 0;

	if(isOnOff)
		m_bAMCFileDownloaded = true;
	else
		m_bAMCFileDownloaded = false;

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� ClearWatchDogStatus �Լ�
@author  ji7412.lee
@date    2014.02.10
@note    Common���� ��û�Ҽ� �ֵ��� Public �߰�
*/
int AMCLibrary::ClearWatchDogStatusPublic(void)
{
	int nTmpError = 0;
	nTmpError = ClearWatchDogStatus();
	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DisableWatchDog �Լ�
@author  ji7412.lee
@date    2014.02.10
@note    Common���� ��û�Ҽ� �ֵ��� Public �߰�
*/
int AMCLibrary::DisableWatchDogPublic(void)
{
	int nTmpError = 0;
	nTmpError = DisableWatchDog();
	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� File System Install �Լ�
@author  ji7412.lee
@date    2014.02.10
*/
int AMCLibrary::AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg)
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	nAMCError = DeviceAMCInstall(pszBootLoader, pszBootFile, prtn, hWnd, uiMsg);
	nTmpError = ConvertAMCError(nAMCError);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC FileSystemInstall Error:%d", nTmpError);

	return nTmpError;
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DSP_Reboot �Լ�
@author  ji7412.lee
@date    2014.02.10
*/
int AMCLibrary::DSP_Reboot(void)
{
	int nTmpError = 0, nAMCError = 0;
	if(m_bAMCReady == false) return ERR_AMC_NOTOPEN;

	//EnterCriticalSection(&m_CS);

	m_bDriverOpen = false;

	Sleep(500);
	DeviceAMCReboot();
	Sleep(3000);

	//LeaveCriticalSection(&m_CS);
	//AMC_PERIODIC_SEND_LOG("AMC DSP_Reboot Error:%d", nTmpError);
	return nTmpError;
}
//---------------------------------------------------------------------------
/**
@brief   AMC�� DPRAM �α� ����
@author  sh17.jo
@date    2018.05.08
*/
void AMCLibrary ::Get_LogDpram(void)
{
    if(g_uLogDpramBase == 0x0)
    {
        AMC_ADD_LOG("[AMC Comm] DPram Bass Address is Zero");
        return;
    }

	LOG_DATA *pLD = new LOG_DATA;
	memset(pLD, 0, sizeof(LOG_DATA));
	memcpy(pLD, (LOG_DATA *)(g_uLogDpramBase), sizeof(LOG_DATA));
	if(pLD)
    {
		AMC_ADD_LOG("[AMC Comm] DPram Log = %x, DPram Base Address = %x [%d/%d/%d] %d/%d/%d/%d/%d", pLD->nSignature,g_uLogDpramBase, pLD->n1msCnt, pLD->n200usCnt, pLD->nDpramCnt
		,pLD->cmd_index,pLD->nMainLoopCnt,pLD->nDprIntClrCnt,pLD->status_DprIntClr,pLD->nAutoClear);

		delete pLD;
		pLD = NULL;
    }
    else
    {
        AMC_ADD_LOG("[AMC Comm] DPram Bass Address is Zero");
    }
}

//---------------------------------------------------------------------------
/**
@brief   AMC�� DPRAM �α� ����
@author  sh17.jo
@date    2018.05.08
*/
bool AMCLibrary ::FPGARegRead(BYTE byOfsAddr, BYTE byRetryCnt, WORD &wReadData)
{
	BYTE byReady=0x0, byHigh=0x0, byLow=0x0;

	wReadData = 0x0;
	*EXP_BASE_ADDR = REG_READ_MODE | byOfsAddr;
	do
	{
		if(*EXP_BASE_ADDR == REG_READ_DONE)
		{
			byLow = *EXP_LSB_ADDR;
			byHigh = *EXP_MSB_ADDR;
			wReadData = ((byHigh << 8) & 0xff00) | byLow;
			return true;
		}
		Sleep(1);
	}
	while((--byRetryCnt) > 0);
	return false;
}

void AMCLibrary::Set_Jig(int n, bool bUse)
{
	bUseAxis_Jig[n] = bUse;

	int cnt = 0;
	for(int i= 0; i<4; i++)
	{
		if(bUseAxis_Jig[i] == true)
		{
			cnt++;
			bSetting_Jig = true;
		}
	}
	if(cnt == 0)
		bSetting_Jig = false;
}

void AMCLibrary::Axis_Ignore(bool bUse)
{
	if(bUse==true)
		  bAxis_Ignore = true;
}
