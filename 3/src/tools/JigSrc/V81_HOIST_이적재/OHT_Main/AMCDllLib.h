// ---------------------------------------------------------------------------
/**
@file    AMCDllLib.h
@version OHT 7.0
@brief   AMCDllLib Class Header File
*/

#ifndef AMCDllLibH
#define AMCDllLibH

#include <windows.h>
#include "ErrorCode.h"
#include "Def_MotionControler.h"
#include "AMC_Protocol.h"
#include "AMCDllLibQueue.h"
#include "LogHandler.h"	// Log �ۼ� ����
#include "ExecuteInfo.h"
#include "OHTMainStatus.h"

// by zzang9un 2013.07.09 : Input Mask�� ���� �߰�
#include "InputMask.h"

#define	MAX_COUNT_IO_INPUT	4
#define	MAX_COUNT_IO_OUTPUT	4

#define AXIS_NONE	-1
#define AXIS1		0
#define AXIS2		1
#define AXIS3		2
#define AXIS4		3
#define COUNT_OF_AMC_AXIS	4

#define	MOTOR_TYPE_INC	1		///< ����ġ ���ڴ�
#define	MOTOR_TYPE_ABS	2		///< ����ġ ���ڴ�

//	Event defines
//-----------------------------------------------------------
#define NO_EVENT			0 	// ignore a condition
#define STOP_EVENT			1	// generate a stop event
#define E_STOP_EVENT		2 	// generate an e_stop event
#define ABORT_EVENT			3 	// disable PID control, and disable the amplifier

#define AMC_UPDATE_CMD_SET_OUTPUT_ALL		0x01
#define AMC_UPDATE_CMD_CLEAR_NODE_COUNT		0x02

#define Move_None 0
#define Move_Positive 1
#define Move_Negative 2
#define Move_Stop 3
#define Move_Distance 4

//-----------------------------------------------------------

typedef struct _LOG_DATA
{
	volatile int nSignature;				// test characters (0xABCD1234)
	volatile int n200usCnt;
	volatile int nDpramCnt;
	volatile int n1msCnt;
	volatile int cmd_index;                //Retry ���� �߰�
	volatile int nMainLoopCnt;
	volatile int nDprIntClrCnt;
	volatile int status_DprIntClr;
	volatile int nAutoClear;
    // Log �߰� �ʿ�� �߰�

}LOG_DATA;


/**
 @class   AMCDllLib
 @date    2012.10.25
 @author  ehoto
 @brief   AMCDllLib ���� Class
 @note    AMC ���̺귯���� �ε��ϰ� AMC�� �ʱ�ȭ �ϸ�, �����ϴ� �Լ��� ��Ī�ϰ�, �ʿ��� �Լ� ����
 */
class AMCLibrary
{
public:
     AMCLibrary();
	~AMCLibrary();

private:
	LogUnit *m_pLogAmcRcv;
	//LogUnit *m_pLogAmcRcv_Old;	//%! for test
	LogUnit *m_pLogAmcSendPr;
	//LogUnit *m_pLogAmcSendPr_Old;
	LogUnit *m_pLogAMCSend;
	FDCUnit *pFDCUnit;

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

private:
	HINSTANCE m_hAMCLib;

	CRITICAL_SECTION	 m_CS;

	// �Ʒ� 4�� ������ ������, AMCInit, AMCClose ������ ����
	bool	m_bDllLoaded;			///< 1. AMC DLL Load ���� �Ϸ�
	bool	m_bDriverOpen;			///< 2. DLL Load �� DriverOpen ���� 					///< ���� g_bDriverOpen;
	bool	m_bAMCFileDownloaded;	///< 3. AMC Open �� AMCParam.ini �ٿ�ε� ���� �Ϸ�	///<���� Is_AMCDownloadFile;
	bool	m_bAMCReady;			///< 4. AMC ��� ������ ����

	// I/O ���� ����
	int		m_anInput[MAX_COUNT_IO_INPUT];			///< AMC�κ��� �޾ƿ��� ��
	int		m_anOutput[MAX_COUNT_IO_OUTPUT];		///< AMC�κ��� �޾ƿ��� ��
	int		m_anOutput_CMD[MAX_COUNT_IO_OUTPUT];	///< AMC�� ���� ��

	// ����� ����
	float	m_fSyncGain;			///< 1, 2�� Sync Gain ���� 1.0 ���� ���
	DWORD	m_dwSndTime;			///< ���� ��� ���� �ð�

	bool	m_abIsRunning[COUNT_OF_AMC_AXIS];		///< ���� ������
	bool	m_abInPosition[COUNT_OF_AMC_AXIS];		///< ���� �Ϸ� ��������
	bool	m_abAmpEnabled[COUNT_OF_AMC_AXIS];		///< AMP Enable ��������
	double 	m_adPrePosition[COUNT_OF_AMC_AXIS];		///< ���� �������� ������ : Cts  //����
	double	m_adCurrentPosition[COUNT_OF_AMC_AXIS];	///< ���� ��ġ:	Cts
	double 	m_adTotalPosition[COUNT_OF_AMC_AXIS];	///< ���� �̵��Ÿ� : Cts //����
	double	m_adFinalPosition[COUNT_OF_AMC_AXIS];	///< ���� ��ġ:	Cts
	double	m_adCurrentVelocity[COUNT_OF_AMC_AXIS];	///< ���� �ӵ�: Cts/sec
	double	m_adFollowingError[COUNT_OF_AMC_AXIS];	///< ���� Following Error :  Cts
	double	m_adPositionOfSmallAddCheck[COUNT_OF_AMC_AXIS];	///< Small Add �ǽ� �� Trigger ���� ���� ��ġ:	Cts

	//����
	bool 	m_abIsCounted[COUNT_OF_AMC_AXIS];		///< ��ɼ���Ƚ�� ���谡 �Ǿ����� Ȯ���ϴ� �÷���
	int 	m_anTotalMoveCount[COUNT_OF_AMC_AXIS];	///< �� ��ɼ����� Ƚ��

	int		m_nError;				///< AMC ����
	int		m_anAxisError[COUNT_OF_AMC_AXIS];		///< �ະ ����

	double	m_adPositiveLimit[COUNT_OF_AMC_AXIS];	///< + Limit
	double	m_adNegativeLimit[COUNT_OF_AMC_AXIS];	///< - Limit

	bool	m_bUseWatchDog;


	// AMC �ֱ� ��� ��� Flag
	AMC_CMD m_SndCmd;
	QUEUEING_AMC_CMD m_QueuingAmcCmd;
	AMCDllLibQueue m_AMCDllLibQueue;

//	UINT	m_uNodeCount;
//	double	m_dNodeDetectPosition;

	NODE_CHECK_DATA m_NodeCheckData;

	int nAMCPauseStatus;
	int nAMCTorqueStatus;    //1 : ��ũ���� ���� ����/ 2 : ��ũ���� ���� ����   0: �ʱⰪ
	int nAMCVHLDetectStatus;    //0; ���� ���� ����, 1; ���� ���� ����(���������� ���� ������ ������)

	int nAMCVHLStopStatus;  //0 : Move None, 1 : Move Positive, 2 : Move Negative, 3 : Move Stop, 4 : Move Distance
	int nAMCDetectStopDecelDownStatus;  // ���� ������ ���ӵ� ���濡 �ٸ� ���°�. (0 ����, 1 ����)

	int nAMCDetectForceIOStatus;  // ���� �������� IO ���� ���°�. (0 �̻��, 1 ����)
	int nAMCDecelStatus;  //AMC ����� ���ӵ�(��� ���� ����)

	int nAMCCommError;
	int nAMCBufferError;
    int AbnormalNodeCount;   //NodeCount������ Count
	int m_VolatageMonitor[2];    //KDS ���а� ����͸�
//	int m_nNodeCount; ///< ��� ����





//#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
#if(OHT_NAME == OHT_NAME_STD_V80)
	OHT80_R01_INPUT1_MASK m_Input1Mask_R01;
	OHT80_R01_INPUT2_MASK m_Input2Mask_R01;
	OHT80_R01_INPUT3_MASK m_Input3Mask_R01;
#elif(OHT_NAME == OHT_NAME_STD_V85)
	STANDARD_OHT_V85_INPUT1_MASK m_Input1Mask_R01;
	STANDARD_OHT_V85_INPUT2_MASK m_Input2Mask_R01;
	STANDARD_OHT_V85_INPUT3_MASK m_Input3Mask_R01;
#elif(OHT_NAME == OHT_NAME_STD_V81)
	FOUP_V81_INPUT1_MASK m_Input1Mask_R01;
	FOUP_V81_INPUT2_MASK m_Input2Mask_R01;
	FOUP_V81_INPUT3_MASK m_Input3Mask_R01;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	STANDARD_OHT_V81S_INPUT1_MASK m_Input1Mask_R01;
	STANDARD_OHT_V81S_INPUT2_MASK m_Input2Mask_R01;
	STANDARD_OHT_V81S_INPUT3_MASK m_Input3Mask_R01;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	RETICLE_OHT_V80_INPUT1_MASK m_Input1Mask_R01;
	RETICLE_OHT_V80_INPUT2_MASK m_Input2Mask_R01;
	RETICLE_OHT_V80_INPUT3_MASK m_Input3Mask_R01;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	RETICLE_EUV_OHT_V80_INPUT1_MASK m_Input1Mask_R01;
	RETICLE_EUV_OHT_V80_INPUT2_MASK m_Input2Mask_R01;
	RETICLE_EUV_OHT_V80_INPUT3_MASK m_Input3Mask_R01;
#endif


//#endif

    // by zzang9un 2013.10.07 : AMC ����
    int nAMCVer[1];

	unsigned long g_uLogDpramBase;


private:
public:
	int AMCInit(void);
	int AMCClose(void);
	HINSTANCE RunTimeLink(void);	///< AMC DLL ��ũ �Լ� : AMCInit���� ȣ���

	bool IsAMCDllOpen();  //AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����

private:
	// for dsp
	//int DSP_Reboot(void);		///< ��� ��� ���� ����

	// watchdog����
	int EnableWatchDog(void);
	int DisableWatchDog(void);
	int GetWatchDogStatus(void);
	int ClearWatchDogStatus(void);

// @todo	Sync Gain ������� ���� : ���ǹ� --> �ʿ� �� ���� �ݿ�
/*
	int Clear_AMPFault(int, int);
	int Clear_Event(int);

	int Set_SyncGain(double, double);
	int Check_SyncGain(void);
	int Save_SyncGain(void);
*/
	int CheckError(int Source, int State);	///< source�� state���� ������ �����ϴ� �Լ�

	// by zzang9un 2013.07.09 : Input Mask ���� �ʱ�ȭ �Լ�
	void InputMaskInit(void);
    void Get_LogDpram(void);

public:
	// I/O ���� �Լ� ----------------------------------
	int SetBit(int nIndex, bool bOnOff, bool bOBSControl);
	bool GetInputBit(int nIndex);
	bool GetOutputBit(int nIndex);

	// Motion ���� �Լ� -------------------------------
	// �ʱ�ȭ
	int Init(int nAxisNum);
	int SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain);											//AOHC-191 Driving Servo & Brake Sequence ����
	int SetMaxSWLimitForReset(int nAxisNum, double dPosLimit, double dNegLimit);

	// ���� Ȯ��
	bool	IsRunning(int nAxisNum);
	bool	IsInPosition(int nAxisNum);
	bool	IsAMPEnable(int nAxisNum);
	double	GetCurrentPosition(int nAxisNum);
	//����
	double	GetPrePosition(int nAxisNum);
	double 	GetTotalPosition(int nAxisNum);
	void 	ResetTotalPosition(int nAxisNum);
	double	GetPosition(int nAxisNum);

	double	GetFinalPosition(int nAxisNum);
	double	GetCurrentVelocity(int nAxisNum);
	double	GetFollowingError(int nAxisNum);
	double	GetSmallAddCheckPosition(int nAxisNum);

	//����
	int 	GetTotalMoveCount(int nAxisNum);
	void 	ResetTotalMoveCount(int nAxisNum);

	// ���� ����
	int	GetError(int nAxisNum);
	int	Reset(int nAxisNum);

	// Offset ����
	int	SetHome(int nAxisNum);
	int	SetOffset(int nAxisNum, double dDistance);
	int	GetOffset(int nAxisNum, double *dDistance);

	// Limit ����
	int	SetPositiveSWLimit(int nAxisNum, double dLimit);
	int	SetNegativeSWLimit(int nAxisNum, double dLimit);
	int	GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit);
	int	ResetSWLimit(int nAxisNum);

	// ���� ���� - ����: �Ÿ�(mm), �ӵ�(mm/msec), ���ӵ�(mm/msec2)
	int	SetAMPEnable(int nAxisNum, bool bValue);
	// ���ӵ� ���
	int	MovePositive(int nAxisNum, double dAccel, double dVel);
	int	MoveNegative(int nAxisNum, double dAccel, double dVel);
	int	MoveStop(int nAxisNum, double dAccel);
	int	MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down, double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo);
	// ���� �ð� ���
	int	MovePositive(int nAxisNum, DWORD dwTime, double dVel);
	int	MoveNegative(int nAxisNum, DWORD dwTime, double dVel);
	int	MoveStop(int nAxisNum, DWORD dwTime);
	int	MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);


	// ������� ����ÿ��� ���� �� ��� �Լ�
	int	SendData(int nAxisNum, char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen);	///< Data ����	
	
	// AMC ���ο� �� ���� ������Ʈ �� I/O ���� �޾ƿ��� �Լ�
	bool UpdateData(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount, int *_out_pErrCode); //AOHC-216 AMC CommFail, CRC ������ ����
	int	GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount);

	// OHTv7.0 ������ ���� Ư���� �Լ�
	void	ClearNodeCountInfo(void);
	
	int		GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData);
	int     GetPauseStatusByAMC();
#if(USE_CHANGENODE == 1)
	int		SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo);
#else
	int		SetDetectInfo(DETECT_INFO DetectInfo);
#endif
	int		CompensateSlavePos();
	int		SetPauseControl(int bIsPause, int nCommander);
	
	int		SetDriveParam(DRIVING_PARAM DrivingParam);

	int     GetTorqueStatusByAMC();
   	void	SetTorqueLimit();
	void	SetTorqueLimitRelease();

	int     GetStopStatusByAMC();

	int     GetDecelStatusByAMC();

	void	SetSlopeTime();
	void	SetSlopeTimeRelease();

	void	SetTargetPos();
	void	SetTargetPosRelease();

	void	SetFirstNodeCountAdd();
	int     GetVHLDetectStatusByAMC();
	void	SetVHLDetectIgnor();
	void	SetVHLDetectIgnorRelease();

	int		GetFollowing_Axis1();	//AOHC-196 ���� �α� �߰�
	int 	m_FollowigAxis1;	//AOHC-196 ���� �α� �߰�

	void	SetDetectStopDecelDown();
	void	SetDetectStopDecelDownRelease();
	int     GetDetectStopDecelDownStatusByAMC();

	void	SetVHLDetectIOSet(bool bIoSet);
	int 	GetVHLDetectForceIO();

	// AMC ������ Trace �ϱ� ���� �Լ�
	void	makeTraceLog();

    // by zzang9un 2013.10.07 : AMC ���� ����
	int GetAMCVersion();
	int GetFPGAVersion();

	bool FPGARegRead(BYTE byOfsAddr, BYTE byRetryCnt, WORD &wReadData);
	WORD wFPGAVersion;

	int GetVolatageMonitor(int Index);
	// by ji7412.lee 2014.02.10 : AMC Library �߰� �Լ�(AMCupdate)
	int AMCFileSystemForamt(void);
	int AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg);
	int ClearWatchDogStatusPublic(void);
	int DisableWatchDogPublic(void);
	int SetAMCDriverOpenByCommon(bool isOnOff);
	int SetAMCFileDownloaded(bool isOnOff);
	int DSP_Reboot(void);


private:
	// dll Library Function for AMC
	// typedef void     (_stdcall* )();
//	typedef int(*AMC_GETDATAS)(void *Data);      ///< ����ȭ DLL������ ��� ����
	typedef int(*AMC_GETDATAS)(AMC_CMD *cmd, AMC_DATA *Data);	///< ����ȭ DLL������ ��� ����
	typedef int(*AMC_GATHERING_TRACE_DATA)(void);			///< ����ȭ DLL������ ��� ����
	typedef int(*AMC_GATHERING_TRACE_DATA_1)(void);			///< ����ȭ DLL������ ��� ����

	// AMC Emulator �ø��� ��� ����
	typedef int (*AMC_CREATE_SERIAL)(char *FileName, char* BaudRate);		///< ����ȭ DLL������ ��� ����
	typedef int (*AMC_READ_SERIAL)(int index, char* Buffer, int Size);		///< ����ȭ DLL������ ��� ����
	typedef int (*AMC_WRITE_SERIAL)(int index, char* Buffer, int Size);		///< ����ȭ DLL������ ��� ����
	typedef void (*AMC_CLOSE_SERIAL)(int index, char* Buffer, int Size);		///< ����ȭ DLL������ ��� ����


	typedef bool(*OPENAMCDEVICE)(int intr, unsigned long dpram, char *workdir);
    typedef void(*CLOSEAMCDEVICE)();
    typedef int(*AMCINIT)(void);
    typedef void(*AMC_DSP_REBOOT)(void);
    typedef int(*AMC_VERSION_CHK)(int, int[]);
    typedef int(*AMC_SETPOSITION)(int axis, double dpos);
    typedef int(*AMC_GETPOSITION)(int axis, double *dpos);

    typedef int(*AMC_SETAMPENABLE)(int axis, int tf);
    typedef int(*AMC_GETAMPENABLE)(int axis, int *tf);
    typedef int(*AMC_AMPFAULTSET)(int axis);
    typedef int(*AMC_AMPFAULTRESET)(int axis);

    typedef int(*AMC_IN_SEQUENCE)(int axis, int *chk_err);
    typedef int(*AMC_IN_MOTION)(int axis, int *chk_err);
    typedef int(*AMC_FRAMES_LEFT)(int axis, int *chk_err);
    typedef int(*AMC_AXIS_STATE)(int axis, int *chk_err);
    typedef int(*AMC_AXIS_SOURCE)(int axis, int *chk_err);
    typedef int(*AMC_IN_POSITION)(int axis);
    typedef int(*AMC_MOTION_DONE)(int axis);
    typedef int(*AMC_AXIS_DONE)(int axis);
    typedef int(*AMC_CLEAR_STATUS)(int axis);
    typedef int(*AMC_CLEAR_STOP)(int axis);
    typedef int(*AMC_FRAMES_CLEAR)(int axis);

    typedef int(*AMC_SET_CLOSED_LOOP)(int axis, int tf);
    typedef int(*AMC_SET_UNIPOLAR)(int axis, int tf);
    typedef int(*AMC_SET_CONTROL)(int axis, int tf);
    typedef int(*AMC_SET_FEEDBACK)(int axis, int tf);
    typedef int(*AMC_SET_SERVO)(int axis);
    typedef int(*AMC_VMOVE)(int axis, double dvel, int nacc);
    typedef int(*AMC_MOVE)(int axis, double dpos, double dvel, int nacc);
    typedef int(*AMC_STARTMOVE)(int axis, double dpos, double dvel, int nacc);

	// ** acceleration base(dacc[cts/sec2], dvel[cts/sec])
    typedef int(*AMC_PMOVE)(int axis, double dacc, double dvel);
    typedef int(*AMC_NMOVE)(int axis, double dacc, double dvel);
    typedef int(*AMC_SMOVE)(int axis, double dacc);
    typedef int(*AMC_DSMOVE)(int axis, double dacc1, double dacc2, double dvel,
        double dpos);

	// time base(dacctime[sec], dvel[cts/sec])
    typedef int(*AMC_PTMOVE)(int axis, double dacctime, double dvel);
    typedef int(*AMC_NTMOVE)(int axis, double dacctime, double dvel);
    typedef int(*AMC_STMOVE)(int axis, double dacctime);
    typedef int(*AMC_DSTMOVE)(int axis, double dacctime1, double dacctime2,
        double dvel, double dpos);

    typedef int(*AMC_SET_ENCODEROFFSET)(int axis, int nOfs);
    typedef int(*AMC_GET_ENCODEROFFSET)(int axis, int *nOfs);

    typedef int(*AMC_SET_IO64)(int port, int[]);
    typedef int(*AMC_GET_IO64)(int port, int[]);
    typedef int(*AMC_GET_OUT64)(int port, int[]);
	typedef int(*AMC_SET_BIT)(int bitpos);
    typedef int(*AMC_RESET_BIT)(int bitpos);

    typedef int(*AMC_DOWNLOAD_FILE)(void *pMap, int uiE, int nBytes);
    typedef int(*AMC_CMD_GEN)(char *Command, int Buf, char *rtnCmd,
        int *tnCmdlen);

    typedef int(*AMC_FS_DOWNLOAD)(char *pszname, int *prtn, HWND hWnd,
        unsigned int uiMsg);
    typedef int(*AMC_FS_SETBOOTFILE)(char *pszname);
    typedef int(*AMC_FS_INSTALL)(char *pszBootLoader, char *pszBootFile,
        int *prtn, HWND hWnd, UINT uiMsg);

    typedef int(*AMC_UPLOAD_SYS_PARAM)(char *pDstn, int *pnUploadBytes);
    typedef int(*AMC_DNLOAD_SYS_PARAM)(char *pSrc, int nDnloadBytes);
    typedef int(*AMC_UPLOAD_PARAM)(char *pDstn, int nStartAddr, int nLoadBytes);
    typedef int(*AMC_DNLOAD_PARAM)(char *pSrc, int nSaveAddr, int nSaveBytes);

    typedef int(*AMC_SAVE_PARAM)(void);
    typedef int(*AMC_LOAD_PARAM)(bool bCopy, char *DstnFileName);
    typedef int(*AMC_SAVE_PARAM_TO_EEPROM)(void);

	// RPS
    typedef int(*AMC_GETACTVELOCITY)(int axis, int *chk_err);
    typedef int(*AMC_GETCOMVELOCITY)(int axis, int *chk_err);

	// RPM
    typedef int(*AMC_GETACTVELOCITY_RPM)(int axis, int *chk_err);
    typedef int(*AMC_GETCOMVELOCITY_RPM)(int axis, int *chk_err);

	typedef int(*AMC_SET_POSITIVE_SW_LIMIT)(int nAxis, double dPos, int nAction);
	typedef int(*AMC_SET_NEGATIVE_SW_LIMIT)(int nAxis, double dPos, int nAction);

	typedef int(*AMC_GET_POSITIVE_SW_LIMIT)(int nAxis, double *dPos, int *nAction);
	typedef int(*AMC_GET_NEGATIVE_SW_LIMIT)(int nAxis, double *dPos, int *nAction);

	typedef int(*AMC_SET_ERROR_LIMIT)(int nAxis, double dPos, int nAction);
	typedef int(*AMC_GET_ERROR_LIMIT)(int nAxis, double *dlimit, int *nAction);
	typedef int(*AMC_GET_ERROR)(int nAxis, double *dlimit);

	typedef int(*AMC_SET_SYNC_MAP_AXES)(int nMaster, int nSlave);
	typedef int(*AMC_SET_SYNC_CONTROL)(int nState);
	typedef int(*AMC_SET_SYNC_GAIN)(float fGain);
	typedef int(*AMC_GET_SYNC_CONTROL)(int *pState);
	typedef int(*AMC_GET_SYNC_GAIN)(float *pGain);
	typedef int(*AMC_GET_SYNC_POSITION)(double *pMaster, double *pSlave);

	// ** ���� ����
	typedef int(*AMC_SET_POSITION_LPF)(int nAxis, int nFreq);
	typedef int(*AMC_SET_POSITION_NOTCH_FILTER)(int nAxis, int nFreq);

	typedef int(*AMC_FORMAT)(void);

	// ** watchdog ����
	typedef int(*AMC_ENABLE_WDT_REASON)(int nReason); // enable watchdog
	typedef int(*AMC_DISABLE_WDT_REASON)(int nReason); // disable watchdog
	typedef int(*AMC_GET_WDT_STATUS)(unsigned int *nWDTStatus);
	// watchdog ���� �ҷ�����(0�� ����)
	typedef int(*AMC_SET_WDT_STATUS)(unsigned int nWDTStatus);
	// get���� �ö�� �� ���� -> �ش� watchdog��������
	typedef int(*AMC_CLR_WDT_REASON)(int nReason); // Test�� , �̻��
    typedef DWORD(*AMC_GET_PHYSICAL_ADDR) (unsigned long addr);

	// ** debug status
	typedef int(*AMC_GET_DBG_STATUS)(char *chrStatus);

	typedef int(*AMC_SET_VEL_CURVE)(int nAxis, int dvel, int nAction);
	typedef int(*AMC_SET_ACTVEL_MARGIN)(int nAxis, int limit, int action, int time);

	typedef int      (* AMC_SET_IO_COUNT)(int nInputCount, int nOutputCount);
	typedef int      (* AMC_GET_IO_COUNT)(int *nInputCount, int *nOutputCount);
	typedef int      (* AMC_SET_IO_OUTPUT)(int port, int []);
	typedef int      (* AMC_GET_IO_OUTPUT)(int port, int []);
	typedef int      (* AMC_GET_IO_INPUT)(int port, int []);
	
	typedef int(*AMC_SET_OHT_MODEL_ID)(int);
	typedef int(*AMC_GET_OHT_MODEL_ID)(int*);	

    OPENAMCDEVICE DeviceAMCOpen;
    CLOSEAMCDEVICE DeviceAMCClose;
    AMCINIT DeviceAMCInit;
    AMC_DSP_REBOOT DeviceAMCReboot;

    AMC_VERSION_CHK DeviceAMCVerChk;

	//==========================================================================
	// -- AMC ���� �ޱ� -- : OHTv7.0 �ϸ鼭 �߰��� �Լ�
	AMC_GETDATAS 				DeviceAMCGetDatas;			///< ����ȭ DLL������ ��� ����
	AMC_GATHERING_TRACE_DATA	DeviceAMCGatheringTraceData;	///< ����ȭ DLL������ ��� ����
	AMC_GATHERING_TRACE_DATA_1	DeviceAMCGatheringTraceData_1;	///< ����ȭ DLL������ ��� ����

public:
	// AMC Emulator �ø��� ��� ����
	AMC_CREATE_SERIAL DeviceAMCCreateSerial;	///< ����ȭ DLL������ ��� ����
	AMC_READ_SERIAL DeviceAMCReadSerial;		///< ����ȭ DLL������ ��� ����
	AMC_WRITE_SERIAL DeviceAMCWriteSerial;		///< ����ȭ DLL������ ��� ����
	AMC_CLOSE_SERIAL DeviceAMCCloseSerial;		///< ����ȭ DLL������ ��� ����
	//==========================================================================

private:
    AMC_SETPOSITION DeviceAMCSetPosition; // ������ ��� : retry
    AMC_GETPOSITION DeviceAMCGetPosition;
	// ������ ��� : log, ignore, //Error Generate
    AMC_SETAMPENABLE DeviceAMCSetAmpEnable; // ������ ��� : retry
    AMC_GETAMPENABLE DeviceAMCGetAmpEnable;
	// ������ ��� : log, ignore, //Error Generate
    AMC_AMPFAULTSET DeviceAMCFaultSet; // ������ ��� : retry
    AMC_AMPFAULTRESET DeviceAMCFaultReset; // ������ ��� : retry

    AMC_IN_SEQUENCE DeviceAMCInSequence; // ������ ��� : ignore, retry
    AMC_IN_MOTION DeviceAMCInMotion; // ������ ��� : ignore, retry
    AMC_IN_POSITION DeviceAMCInPosition; // ������ ��� : ignore, retry
    AMC_FRAMES_LEFT DeviceAMCFramesLeft; // -
    AMC_MOTION_DONE DeviceAMCMotionDone; // -

    AMC_AXIS_DONE DeviceAMCAxisDone; // ������ ��� : ignore, retry
    AMC_AXIS_STATE DeviceAMCAxisState; // ������ ��� : ignore, retry
    AMC_AXIS_SOURCE DeviceAMCAxisSource; // ������ ��� : ignore, retry
    AMC_CLEAR_STATUS DeviceAMCClearStatus; // ������ ��� : retry
    AMC_CLEAR_STOP DeviceAMCClearStop; // -

    AMC_FRAMES_CLEAR DeviceAMCFramesClear; // ������ ��� : retry
    AMC_SET_CLOSED_LOOP DeviceAMCSetClosedLoop; // -
    AMC_SET_UNIPOLAR DeviceAMCSetUnipolar; // -
    AMC_SET_FEEDBACK DeviceAMCSetFeedback; // -
    AMC_SET_CONTROL DeviceAMCSetControl; // -
    AMC_SET_SERVO DeviceAMCSetServo; // -
    AMC_VMOVE DeviceAMCVMove; // -
    AMC_MOVE DeviceAMCMove; // -
    AMC_STARTMOVE DeviceAMCStartMove; // -

    AMC_PMOVE DeviceAMCPMove; // ������ ��� : retry
    AMC_NMOVE DeviceAMCNMove; // ������ ��� : retry
    AMC_SMOVE DeviceAMCSMove; // ������ ��� : retry
    AMC_DSMOVE DeviceAMCDSMove; // ������ ��� : retry

    AMC_PTMOVE DeviceAMCPTMove; // ������ ��� : retry
    AMC_NTMOVE DeviceAMCNTMove; // ������ ��� : retry
    AMC_STMOVE DeviceAMCSTMove; // ������ ��� : retry
    AMC_DSTMOVE DeviceAMCDSTMove; // ������ ��� : retry

    AMC_SET_ENCODEROFFSET DeviceAMCSetEncoderOffset; // ������ ��� : retry
    AMC_GET_ENCODEROFFSET DeviceAMCGetEncoderOffset; // ������ ��� : ignore, retry

    AMC_SET_IO64 DeviceAMCSetIO_64; // ������ ��� : retry
    AMC_GET_IO64 DeviceAMCGetIO_64; // ������ ��� : ignore, retry
    AMC_GET_OUT64 DeviceAMCGetOutIO; // ������ ��� : ignore, retry
    AMC_SET_BIT DeviceAMCSetBit; // ������ ��� : retry
    AMC_RESET_BIT DeviceAMCResetBit; // ������ ��� : retry

    AMC_DOWNLOAD_FILE DeviceAMCDownloadFile; // -
    AMC_CMD_GEN DeviceAMCCmdGenerate; // -
    AMC_FS_DOWNLOAD DeviceAMCOutUpdate; // -
    AMC_FS_SETBOOTFILE DeviceAMCSetBootFile; // -
    AMC_FS_INSTALL DeviceAMCInstall; // Error

    AMC_UPLOAD_SYS_PARAM DeviceAMCUploadSysParam; // -
    AMC_DNLOAD_SYS_PARAM DeviceAMCDnloadSysParam; // -
    AMC_UPLOAD_PARAM DeviceAMCUploadParam; // -
    AMC_DNLOAD_PARAM DeviceAMCDnloadParam; // -

    AMC_SAVE_PARAM DeviceAMCParamSaveToDSP; // Error
    AMC_LOAD_PARAM DeviceAMCParamSaveToFile; // Error
    AMC_SAVE_PARAM_TO_EEPROM DeviceAMCParamSaveToEEPROM; // Err0r

    AMC_GETACTVELOCITY DeviceAMCGetActVelocity; // ������ ��� : ignore, retry
    AMC_GETCOMVELOCITY DeviceAMCGetComVelocity; // -

    AMC_GETACTVELOCITY_RPM DeviceAMCGetActVelocity_RPM; // -
    AMC_GETCOMVELOCITY_RPM DeviceAMCGetComVelocity_RPM; // -

    AMC_SET_POSITIVE_SW_LIMIT DeviceAMCSetPositiveSWLimit; // Retry
    AMC_SET_NEGATIVE_SW_LIMIT DeviceAMCSetNegativeSWLimit; // Retry

    AMC_GET_POSITIVE_SW_LIMIT DeviceAMCGetPositiveSWLimit; // Retry
    AMC_GET_NEGATIVE_SW_LIMIT DeviceAMCGetNegativeSWLimit; // Retry

    AMC_SET_ERROR_LIMIT DeviceAMCSetErrorLimit; // -
    AMC_GET_ERROR_LIMIT DeviceAMCGetErrorLimit; // -
    AMC_GET_ERROR DeviceAMCGetError; // -

    AMC_SET_SYNC_MAP_AXES DeviceAMCSetSyncMapAxes; // error
    AMC_SET_SYNC_CONTROL DeviceAMCSetSyncControl; // error
    AMC_SET_SYNC_GAIN DeviceAMCSetSyncGain; // error
    AMC_GET_SYNC_CONTROL DeviceAMCGetSyncControl; // error
    AMC_GET_SYNC_GAIN DeviceAMCGetSyncGain; // ignore, retry
    AMC_GET_SYNC_POSITION DeviceAMCGetSyncPosition; // -

    AMC_SET_POSITION_LPF DeviceAMCSetPositionLPF; // -
    AMC_SET_POSITION_NOTCH_FILTER DeviceAMCSetPositionNotchFilter; // -

    AMC_FORMAT DeviceAMCFormat; // -

	// watchdog ����
    AMC_ENABLE_WDT_REASON DeviceAMCEnableWatchdogReason; // retry, ignore
    AMC_DISABLE_WDT_REASON DeviceAMCDisableWatchdogReason; // retry, ignore
    AMC_GET_WDT_STATUS DeviceAMCGetWatchdogStatus; // ignore
    AMC_SET_WDT_STATUS DeviceAMCSetWatchdogStatus; // retry, ignore
    AMC_CLR_WDT_REASON DeviceAMCClearWatchdogReason; // -
    AMC_GET_PHYSICAL_ADDR DeviceAMCGetPhysicalAddr;

	// debug status
	AMC_GET_DBG_STATUS DeviceAMCGetDebugStatus;

	AMC_SET_VEL_CURVE DeviceAMCSetVelCurve; // ������ �ӵ�

	AMC_SET_ACTVEL_MARGIN DeviceAMCSetActVelMargin;

	AMC_SET_IO_COUNT        DeviceAMCSetIO_Count;
	AMC_GET_IO_COUNT        DeviceAMCGetIO_Count;
	AMC_SET_IO_OUTPUT       DeviceAMCSetIO_Output;
	AMC_GET_IO_OUTPUT       DeviceAMCGetIO_Output;
	AMC_GET_IO_INPUT        DeviceAMCGetIO_Input;
	
    AMC_SET_OHT_MODEL_ID DeviceAMCSetOhtModelId;
	AMC_GET_OHT_MODEL_ID DeviceAMCGetOhtModelId;

public:
	int nIgnore_JIG;
	void Set_Jig(int n);

/*
private:

//	int Set_Bit_ON(int); 
//	int Set_Bit_OFF(int);
//	int Set_Bit_SELECT(int, int);
	int Clear_AMPFault(int, int);
	int Clear_Event(int);

//	int Set_EncoderOffset(int);
//	int Set_ServoONOFF(int, bool);
	int Set_SyncGain(double, double);
	int Check_SyncGain(void);
	int Save_SyncGain(void);
*/
//	bool Get_AMPEnable(int);
//	int Set_AMPEnable(int);

	// acceleration[CTS/SEC2] ���
//	int Move_Positive(int, double, double);
//	int Move_Negative(int, double, double);
//	int Move_Stop(int, double);
//	int Move_Distance(int, double, double, double, double);

	// time[SEC] ���
//	int Move_Time_Positive(int, double, double);
//	int Move_Time_Negative(int, double, double);
//	int Move_Time_Stop(int, double);
//	int Move_Time_Distance(int, double, double, double, double);

/*
	// Driving Motor Stop
//	int DrivingMotor_EncoderClear(void);
//	int DrivingMotor_Stop_Cmd(String, String);
//	int DrivingMotor_Stop_Cmd();
//	int DrivingMotor_Stop_Cmd(double);
//	int DrivingMotor_JogMove_Forward(double, double);
//	int DrivingMotor_JogMove_Backward(double, double);
//	int DrivingMotor_JogMoveT_Forward(double, double);
//	int DrivingMotor_JogMoveT_Backward(double, double);

//	int DrivingMotor_JogMove_Dist(double, double, double, double);
//	int DrivingMotor_JogMoveT_Dist(double, double, double);
*/

/*
	// Hoist Motor
//	int HoistMotor_MoveHomePos(void);

//	int Motor234_Stop_Cmd(int);
//	int Motor234_TimeStop_Cmd(int);
//	int Check_StopCmd_Exec(int);
*/

/*
	// Limit����
//	int Set_SWLimit(int, double, double);
//	int Get_SWLimit(int, String &);
//	int Set_PositiveSWLimit(int, double);
//	int Set_NegativeSWLimit(int, double);
//	int Set_SWLimitFree(int);

//	double Get_FollowingErrorLimit(int);
//	double Get_FollowingError(int);

//	int Convert_Error(int);
//	int Convert_Error(int, String);
*/
	// AMC���� ��ſ� ���� ��� (���� protocol)
////	int CheckRcvData_FromAMC(String, String);
//	int AMC_EStop(void);
//	int AMC_Status_Init(void); // reset
//	int AMC_Go_Start(void);
//	int AMC_GO_Stop(void);
////	int AMC_Status(String, String &);
//	int AMC_VFDDisplay(String);
//	int AMC_DRV_Command(int, double, double, double, double, String &);
//	int AMC_DRV_Command(int, double, double, double, double);
//	int AMC_DRV_Command(int, String);
////	String AMC_Get_FirmwareVision(void);
//	int AMC_Mark_Start(void);
//	int AMC_FinalNodeControl_SET(int, double, double, double, double);
//	int AMC_FinalNodeControl_RESET(void);
//	int AMC_FC_PAUSEControl(bool, int);
////	String AMC_GetDebugData(void);
//	int AMC_Positioning(void);
/*
	// watchdog����
	int Enable_WatchDog(void);
	int Disable_WatchDog(void);
	int Get_WatchDog_Status(void);
	int Clear_WatchDog_Status(void);
*/
	// Debug Status����
////	String Get_AMC_Debug_Status(void);
////	String GetCommandString(char);


};
// ---------------------------------------------------------------------------
#endif
