// ---------------------------------------------------------------------------

#ifndef OHTInfoUnitH
#define OHTInfoUnitH

#include "include/Def_OHT.h"
#include "include/Def_HW.h"
#include "../Common/Def_DataType.h"

// ---------------------------------------------------------------------------
class TOHTCONNECTINFO
{
private:
    String RemoteFileName;

    int FileExistCheck(String strTmpPath, String &strTmpFileName);
    int RemoteFileCheck(String strTmpFileName);

public:
    TOHTCONNECTINFO();
    ~TOHTCONNECTINFO();

    void ConnectStart();
    bool RemoteFileSave();

	void MakeRemoteIniFile(AnsiString strIniFilePath);

	String strOHTName[2000];
    String strOHTIP[2000];
    int    nCount;
    int    nIndex;

    // added by sanghwa
    String        g_strLineName;
    unsigned long g_iLineColor;
    String        g_strTileName;
};

// ---------------------------------------------------------------------------

/**
@class   TOHT_INPUT
@date    2012.11.26
@author  zzang9un
@brief   OHT Input Data ������ ��� �ִ� Class
*/
class TOHT_INPUT
{
public:

    unsigned int nAMCInput[3];
    unsigned int nInData[4][32];

	uINPUT uInput;
	sINPUT_CommonIO uInputCommon;

    TOHT_INPUT();
    ~TOHT_INPUT();

    void Init();

};

// ---------------------------------------------------------------------------

/**
@class   TOHT_MOTOR
@date    2012.12.12
@author  puting
@brief   OHT Motor Data ������ ��� �ִ� Class
*/
class TOHT_MOTOR
{
public:
    double dPosition_cts;
    double dPosition_mm;
    double dVelocity_ctspermsec; // [cts/msec]
    double dVelocity_mpersec;    // m/sec
    double dVelocity_rpm;        // rpm
    int    nError;
    int    nErrorCode;
    int    nAxis_Running; // 1:running��, 0:����
    bool   bIs_ServoON;
    bool   bIs_Origin;       // rotate��(coolmuscle)�� ���
    int    nAxis_state_code; // 0:NO_EVENT, 1:STOP_EVENT, 2:E_STOP_EVENT, 3:ABORT_EVENT
    int    nAxis_source_code;

    TOHT_MOTOR();
    ~TOHT_MOTOR();

    void Init();
};

// ---------------------------------------------------------------------------

/**
@class   TOHT_OUTPUT
@date    2012.12.12
@author  puting
@brief   OHT Output Data ������ ��� �ִ� Class
*/
class TOHT_OUTPUT
{
public:

    unsigned int nAMCOutput[3];
    unsigned int nOutData[4][32];

    // �߰� puting 2012.11.28
	uOUTPUT uOutput;
    sOUTPUT_CommonIO uOutputCommon;

    TOHT_OUTPUT();
    ~TOHT_OUTPUT();

    void Init();
};

// ---------------------------------------------------------------------------
/**
 @class   TOHT_CHECKON
 @date    2012.12.12
 @author  puting
 @brief   OHT Use Data ������ ��� �ִ� Class
 */
class TOHT_CHECKON
{
public:

    uCMD_USE_DATA Usedata;

    bool bUse_BCR;
    bool bUse_PBSSensor;
    bool bUse_SoundAlarm;
    bool bUse_LampIndi;
    bool bUse_ShutterControl;
    bool bUse_EQPIO;
    bool bUse_STBSensor;
    bool bUse_HandLimitSensorCheck;
    bool bUse_HandControl;
    bool bUse_SteeringControl;
    bool bUse_UTBSensor;
    bool bUse_LookDownSensor;
    bool bUse_HandPushSensorCheck;
    bool bUse_GripperSen;
    bool bUse_HoistAlignSen;
    bool bUse_OscillationSen;
    bool bUse_ErrSound;
    bool bUse_DrivingSound;
    bool bUse_TransSound;
    bool bUse_ObsDetectSound;
	bool bUse_OhtDetectSound;
    bool bUse_ObsIgnoreSound;
    bool bUse_IgnorePIOError;        // �������� pio���� �߻��� �����ϰ� ������ �Ϸ�
    bool bUse_HoistHomePos_PIOError; // pio���� �߻��� 2����� ��� ���� �߻�..
    bool bUse_NextMotion_34Axis;     // Next command ��ɹ޾����� 3,4���� ���� �������� ���� �Ǵ�..
    bool bUse_LocSave_Trans_ing;      // �������� Hand Push�� off�϶� ������ ƼĪ���� �������� ����
    bool bUse_PushAbnormalOnCheck;
    bool bUse_UTBFoupCheck;        // UTB Unload �� Hand�� ƼĪ ��ġ���� �������� Hand Close�� �Ͽ� Foup�� �ִ����� Ȯ������ ����
    bool bUse_InnerFoupDetectSen;
	bool bUse_TransEQSound;
	bool bUse_TransSTKSound;
    bool bUse_TransSTBSound;

    TOHT_CHECKON();
    ~TOHT_CHECKON();

    void Init();
};

// ---------------------------------------------------------------------------
class TOHT_CMDCONTROL
{
public:
    // bool bSendCMD;
    // int  nSendCMDID;

        public :
    TOHT_CMDCONTROL();
    ~TOHT_CMDCONTROL();

    void Init();
};

// ---------------------------------------------------------------------------
/**
 @class   TOHT_HIDDATA
 @date    2012.12.12
 @author  puting
 @brief   OHT HID Data ������ ��� �ִ� Class
 */
class TOHT_HIDDATA
{
public:
    int nErec; // ��������
    int nEdc;  // �������
    int nIdc;  // �������
    int nKw;   // ����Ŀ�

    int      nErrBit;
    String   strErrString;
    unsigned nFuseOpen;
    unsigned nOverTemperature;
    unsigned nEdcOverVoltage;
    unsigned nEdcUnderVoltage;
    unsigned nIdcOverCurrent;
    unsigned nErecOverVoltage;
    unsigned nErecUnderVoltage;
    unsigned nErrStorage;
    unsigned nOverOutput_KW;

    String strTemperature[2];

    TOHT_HIDDATA();
    ~TOHT_HIDDATA();

    void Init();
};

class TOHT_VIBDATA
{
public:
	float x;
	float y;
	float z;

	TOHT_VIBDATA();
    ~TOHT_VIBDATA();

	void Init();
};

// ---------------------------------------------------------------------------
/**
 @class   TOHT_STATUS
 @date    2012.12.12
 @author  puting
 @brief   OHT ��� Data ������ ��� �ִ� Class
 */
class TOHTSTATUS
{
public:
    TOHT_INPUT      *IOInput;
    TOHT_INPUT      *IONumInput;
    TOHT_OUTPUT     *IOOutput;
    TOHT_CHECKON    *CheckOnOff;
    TOHT_CMDCONTROL *CmdControl;
	TOHT_HIDDATA    *HIDData;
	TOHT_VIBDATA    *VIBData;
    TOHT_MOTOR      *Motor_Driving_F;
    TOHT_MOTOR      *Motor_Driving_R;
    TOHT_MOTOR      *Motor_Hoist;
    TOHT_MOTOR      *Motor_Shift;
    TOHT_MOTOR      *Motor_Rotate;

    // by zzang9un 2012.12.06 : 7.0�� ����� ������ ��ü
    MSG_STATUS_OHT2TP   m_OHTStatus;  ///< TP���� ������� OHT Status ����ü
    uTP_CMD             m_OHTInfo;    ///< OHT Info
    
    bool bIs_Hand_Origin;
    bool bIs_Shutter_Origin;
    bool bIs_Steering_Origin;
    bool bIs_Rotate_Origin;

    int nSndCMDID;
    int nTPCommandType;
    int nCmdCount;
    int nTransCount;
    int nDetectLevel;

    BYTE PreStatus;    ///< ���� Status

    // by zzang9un 2013.02.05 : ������ �շ��� ���� ���� ������ Setting�� ����� ����
    BYTE PreTransCmd;    ///< ���� ������ ���
    BYTE PrePortType;    ///< ���� ������ Port Type

    String strErrorString;
    String strPMACRespose;
    String strMapDist;
    String strPMACResponse;
    String strDownPercent;
	
    String strNetworkDrive;
	String strLogworkDrive;
	
    bool   bReceiveStatus;

    int    nTransLongMode;
    int    nProgram_Mode;
    String strObsDist;
    String strDetectDist;
    bool   bSendMapData;
    int    g_nIsOHTBoot;

	String strOHTVer; // "4.5" "5.0_U" "5.0_S"
	String strOHTType;
    String strLineInfo;
    String strOHTNum;

    String strLogSaveMode;
    String strLogSavePath;
    String strAPSignal;
    int    nIs_DirectLogSave;

    ///////v7.0 puting 2012.11.28
    int nSTBDire;
    int nIsRotateExist;
    MAPDATALOADINFO AdjustMapLoadInfo;
    MAPMAKE_STATE_INFO MapMakeState;
    int nIsRFPIOType;

    int nIsUBGSenUse;
    int nUBGDetectLevel;
    int nIsPauseSet;

    TOHTSTATUS();
    ~TOHTSTATUS();

    void Init();

};

// ---------------------------------------------------------------------------
extern TOHTCONNECTINFO *gm_OHTConnectInfo;
extern TOHTSTATUS *gm_OHTStatus;

#endif
