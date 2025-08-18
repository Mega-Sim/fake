//---------------------------------------------------------------------------
/**
@file    OHTMain.cpp
@version OHT 7.0
@brief   OHT Main Program Main Form CPP File
*/

#include <vcl.h>
#pragma hdrstop

#include "OHTMain.h"
#include "Utility.h"
#include "MyTools.h"
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOHTMainForm *OHTMainForm;


// by zzang9un 2013.04.10 : ���������� ���� Critical Section
TCriticalSection *CS_CmdList; ///< CmdList�� ���� Critical Section ����
TCriticalSection *CS_TaskList; ///< CmdList�� ���� Critical Section ����
TCriticalSection *CS_CycleList; ///< CmdList�� ���� Critical Section ����
TCriticalSection *CS_ResponceList; ///< CmdList�� ���� Critical Section ����
TCriticalSection *CS_PassOpenList;

TCriticalSection *CS_OHTMainStatus; ///< OHTMainStatus�� ���� Critical Section ����

TCriticalSection *CS_ThreadCycleLog;

//---------------------------------------------------------------------------
#define ADD_EXCEPT_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\Exception.log", __VA_ARGS__)
#define THREAD_STATUS_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\ThreadStatus.log", __VA_ARGS__)

#define AXIS_MASK_FUNC_POSITION     0
#define AXIS_MASK_FUNC_AMP_ENABLE   1
#define AXIS_MASK_FUNC_RUNNING      2
#define AXIS_MASK_FUNC_ERRORCODE    3
#define AXIS_MASK_FUNC_ORIGIN       4

//2014.04.16 doori.shin Thread ���ۿ��� Ȯ�� �� ����
#define THREAD_STATUS_UNKNOWN		0
#define THREAD_STATUS_ALIVE			1
#define THREAD_STATUS_STOP			2
#define THREAD_STATUS_DESTROY		3

/**
@brief   TOHTMainForm Ŭ���� ������
@author  zzang9un
@date    2012.11.15
@param   Owner : TComponent
*/
__fastcall TOHTMainForm::TOHTMainForm(TComponent* Owner)
	: TForm(Owner)
{
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	Application->OnRestore = OnRestore;

	g_DataTransProgressStatus = 0;
	g_MAPUpdateStatus=0;

	m_StationData_STD = NULL;
	pLogTaskControl = NULL;
	pLogOCSComm = NULL;
	pLogTPComm = NULL;
	m_PathList = NULL;
	m_pServoControl = NULL;
	m_CIDData_STD = NULL;
	m_CollisionData_STD = NULL;
    m_FlowControlData_STD = NULL;
	m_MapData_STD = NULL;
	m_TeachingData_STD = NULL;
    m_PathData_STD = NULL;
	m_CIDManager_STD = NULL;
    m_CollisionManager_STD = NULL;
    m_FlowControlManager_STD = NULL;
	m_MapManager_STD = NULL;
	m_StationManager_STD = NULL;
	m_TeachingManager_STD = NULL;
	m_PathDataManager_STD = NULL;
	m_MapMakerFileManager_STD = NULL;
	m_pOHTMainStatus = NULL;
	m_pOHTDetect = NULL;
	m_nPreLogThTick = 0;

	memset(m_szUBGInfo_INI, 0x00, sizeof(m_szUBGInfo_INI));
	memset(m_szUBGInfo_Comm, 0x00, sizeof(m_szUBGInfo_Comm));

	// by zzang9un 2013.04.10 : Critical Section ���� �ʱ�ȭ
	CS_CmdList = new TCriticalSection;
	CS_TaskList= new TCriticalSection; ///< CmdList�� ���� Critical Section ����
	CS_CycleList= new TCriticalSection; ///< CmdList�� ���� Critical Section ����
	CS_ResponceList= new TCriticalSection; ///< CmdList�� ���� Critical Section ����
	CS_PassOpenList = new TCriticalSection;
	CS_OHTMainStatus = new TCriticalSection;
	CS_ThreadCycleLog = new TCriticalSection;
//	m_CmdList = NULL;

	m_TaskList= NULL;                 ///< Main Command List
	m_ResponseList= NULL;                  ///< Main Command List
	m_CycleList= NULL;                 ///< Main Command List
	m_PassOpenList = NULL;
	m_PassPermitList = NULL;

    gm_ExecuteInfo = NULL;
    gm_OHTMainStatus = NULL;
	gm_HWSetStatus = NULL;

    m_MapData = NULL;
    m_TmpMapData = NULL;
	m_MapDataManager = NULL;

	m_UDPComm = NULL;
	m_UDPDiag = NULL;


	m_CmdManager = NULL;
	m_PathSearch = NULL;
	m_PassPermit = NULL;


//	m_socketThread = new SocketThread();

    m_HWCommon = NULL;
    m_HWSet = NULL;
    m_HWControlThread = NULL;
	m_MaskOfHW = NULL;

	m_DrivingControl = NULL;
	m_TransControl = NULL;
	m_ManualControl = NULL;
    m_ExecuteThread = NULL;

	m_StatusCheckThread = NULL;
	m_LogThread = NULL;
	m_ControlTask = NULL;
	m_TaskControlThread = NULL;
	m_ExtraJobThread = NULL;
//	m_AvsControlThread = NULL;
	m_MapMake_Status = NULL;
	m_StationDataManager = NULL;
	m_TeachingDataManager = NULL;

	m_MapMakeProcess = NULL;

//	m_MonitorControlThread = NULL;
	m_pParamSet = NULL;
    // by zzang9un 2013.02.19 : Form Title�� ���� ���� ǥ��
	// String strFileDate = "", strTZitle = "", strCaption = ""; //�� ���߻��Ͽ� ����� ���ÿ� ������ �Ҵ�
    // Title
    String strTitle = Application->Title;

    // ���� ���� ���� �ð�
    TSearchRec rc;
    FindFirst(Application->ExeName, faArchive | faDirectory, rc);
	TDateTime dt = FileDateToDateTime(rc.Time);

    String strFileDate = L"[Date:" + FormatDateTime("yyyy.mm.dd", dt) + "]";

    String strCaption = strTitle + VerInfo.VersionString() + strFileDate;
    OHTMainForm->Caption = strCaption;

	checkLogDrive = false;
	checkLogFolder = false;	

	m_MapUpdateResult = NO_ERR;
	m_ParamUpdateResult = NO_ERR;
	
	//2014.04.15 doori.shin Thread Check���� �ʱ�ȭ
	m_nPreExecuteThTick = -1;
	m_nPreExtraJobThTick = -1;
	m_nPreHWCtrlThTick = -1;
	m_nPreTaskCtrlThTick = -1;
	m_nPreStatusChkThTick = -1;
    m_bUBGINILoaded = false;
	m_nEQPIO_Type = TYPE_PIO_HYBRID;
	m_bErrIotGateway = false;
	m_bErrOHTTypeMatching =false;
	InitializeCriticalSection(&m_CS);
}
//---------------------------------------------------------------------------
/**
@brief   Form Create �̺�Ʈ �߻� �� ȣ��Ǵ� �Լ�

         OHT �ʱ�ȭ �۾� ����
@author  zzang9un
@date    2012.11.15
@param   Sender : TObject Ŭ����
*/
void __fastcall TOHTMainForm::FormCreate(TObject *Sender)
{
    // UI Init
	memoMain->Clear();
	tabIOTest->TabVisible = false;
	Tab81S->TabVisible = false;

    tabProgramInfo->Brush->Color = clWhite;

	Application->OnException = AppException;
}

/**
@brief   OnDestroy �̺�Ʈ �߻� �� ȣ��Ǵ� �Լ�

		 ���� �Ҵ�� �޸� ����
         Thread ����
@author  zzang9un
@date    2012.11.15
@param   Sender : TObject Ŭ����
*/
void __fastcall TOHTMainForm::FormDestroy(TObject *Sender)
{
//	tmOCSStatus->Enabled  = false;
//	tmMainTask->Enabled 	= false;
	tmUIupdate->Enabled = false;

	DeleteCriticalSection(&m_CS);

	DestroyClass();
}
//---------------------------------------------------------------------------

void TOHTMainForm::Update_UI()
{
    // by zzang9un 2013.04.10 : ��� ��� �߰�
	if(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO)
        panMode->Caption = "Auto";
    else
		panMode->Caption = "Manaul";

	panStatus->Caption = StatusToString(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Status);

    if((int)m_ControlTask->GetOHTError() != 0)
    {
    	panErrorCode->Caption = IntToStr((int)m_ControlTask->GetOHTError());
        panErrorCode->Color = clRed;
    }
    else
    {
        panErrorCode->Caption = "";
        panErrorCode->Color = clBlack;
    }

	// by zzang9un 2012.12.03 : ���� Node ID ����ϵ��� ����
	panCurNodeID->Caption = (int)gm_ExecuteInfo->ExecutePositionInfo.CurrNode;

	// by zzang9un 2013.03.20 : Station ID
	panCurStationID->Caption = (int)gm_OHTMainStatus->StatusCommon.CurStationID;

	// IoT Gateway ���� �� ��� ���α׷��� �����Ѵ�.
	if(m_bErrIotGateway ||m_bErrOHTTypeMatching)
	{
        this->Close();
    }
}
//---------------------------------------------------------------------------

/**
@brief   Status ������ String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2012.11.26
@param   status : Port ID
@return  Status�� ��ȯ�� ���ڿ�
*/
String TOHTMainForm::StatusToString(BYTE status)
{
    String strRtnStatus = "";

    switch(status)
	{
    case STS_INIT:
        strRtnStatus = "Initialized";
        break;
    case STS_GOING:
        strRtnStatus = "Going";
		break;
    case STS_ARRIVED:
        strRtnStatus = "Arrived";
		break;
    case STS_UNLOADING:
        strRtnStatus = "Unloading";
		break;
    case STS_UNLOADED:
        strRtnStatus = "Unloaded";
		break;
    case STS_LOADING:
        strRtnStatus = "Loading";
		break;
    case STS_LOADED:
        strRtnStatus = "Loaded";
		break;
    case STS_AUTOPOSITION:
        strRtnStatus = "Auto Position";
		break;
    case STS_RUNNING:
        strRtnStatus = "Running";
		break;
    case STS_COMPLETE:
        strRtnStatus = "Complete";
		break;
    case STS_ERROR:
        strRtnStatus = "Error";
		break;
	case STS_AUTORECOVERY:
		strRtnStatus = "Auto Recovery";
		break;
	case STS_AUTORECOVERY_BLANKTRANS:
		strRtnStatus = "Auto Recovery_B";
		break;
	case STS_AUTORECOVERY_DOUBLETRANS:
		strRtnStatus = "Auto Recovery_D";
		break;
	case STS_AUTORECOVERY_OVERRUN:
		strRtnStatus = "OverRun";
		break;
	case STS_TURN_OFF:
		strRtnStatus = "TurnOff";
		break;
	case STS_AUTO_TEACHING:
	case STS_AUTO_TEACHING_NULLTYPE:
		strRtnStatus = "Teaching";
		break;
	case STS_AUTO_TAUGHT:
		strRtnStatus = "Taught";
		break;
	case STS_AUTO_TRANS_HOMING:
		strRtnStatus = "Trans Homing";
		break;
	default:
		strRtnStatus = "Unknown";
        break;
    }

    return strRtnStatus;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::SetInputMask(TCheckBox *CB_InputMask, TEdit *Edt_Mask_Num, TEdit *Edt_Mask_Value)
{
    int     nNum        = Edt_Mask_Num->Text.ToIntDef(1)-1;
    bool    bValue      = (bool)Edt_Mask_Value->Text.ToIntDef(0);
    bool    bChecked    = CB_InputMask->Checked;

    MASK_INPUT  *pMaskOfInput   = NULL;

    // �Է� �� feedback
    Edt_Mask_Num->Text     = nNum+1;
    Edt_Mask_Value->Text   = (int)bValue;


	// HW Mask�� ���� �� ó��
    if(m_MaskOfHW==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // Input Mask�� ���� �� ó��
    pMaskOfInput = m_MaskOfHW->getMaskOfInput(nNum);
    if(pMaskOfInput==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // �Է� ���� ������� �ʰ� ����
    Edt_Mask_Num->Enabled       = !bChecked;
    Edt_Mask_Value->Enabled     = !bChecked;
    Edt_Mask_Num->ReadOnly      = bChecked;
    Edt_Mask_Value->ReadOnly    = bChecked;

    // �� Masking
    if(bChecked==true)
    {
        pMaskOfInput->Value = bValue;
		pMaskOfInput->bUse  = true;
    }
    else
    {
        pMaskOfInput->bUse  = false;
    }

}

//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::tmInintTimer(TObject *Sender)
{
	tmInint->Enabled = false;
	OHTInit();

	Sleep(1000);
	tmUIupdate->Enabled = true;

}
//---------------------------------------------------------------------------

/**
@brief   OHT �ʱ�ȭ�� �����ϴ� �Լ�
@author  zzang9un
@date    2012.11.15
@return  ������ ��� 0, ���� �߻� �� ���� �ڵ� ����
*/
void TOHTMainForm::OHTInit()
{
	uTP_CMD OHTInfo;      ///< OHT Info

	// Check D-Drive and Log folder by ks4423
	if(checkLogDrive = OHTUTILITY::Check_DDrive())
	{
		checkLogFolder = OHTUTILITY::Check_LogFolder();
	}
	else
	{
		checkLogFolder = false;
	}

	// Step 1. OHT�� ����� Class ����
	CreateClass();

    // Step 2. flag ���� �ʱ�ȭ
 //	ErrorCode_Main = NO_ERR;

	m_PathSearch->InitPathList();
//	m_CmdManager->InitCmdList();
	m_PassPermit->InitPassPermit();

    // Step 3. OHT ���� ���� �ʱ�ȭ : �ӽ�
    ///@todo by zzang9un 2012.11.26 : ���� �����ؾ� ��
    gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode = MODE_MANUAL;

	// by zzang9un 2012.11.28 : TP�� �ʱ�ȭ ���� �׽�Ʈ
	// OHT Init�� ���� OHT Info ���� �߰�
	OHTInfo.OHTInfo.cData = 0;
	OHTInfo.OHTInfo.OHT_ID = 70;
	OHTInfo.OHTInfo.LineInfo = 100;
#if(OHT_NAME == OHT_NAME_STD_V80)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
// 2016-12-03, Add by Labeler
#elif(OHT_NAME == OHT_NAME_STD_V81)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_81;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_RETICLE;
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_80;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_EUV;
#elif(OHT_NAME == OHT_NAME_STD_V85)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_85;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_EUV;
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_81;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP_S;
#else
	OHTInfo.OHTInfo.OHT_Ver = OHT_VERSION_70;
	OHTInfo.OHTInfo.OHT_Type = OHT_TYPE_FOUP;
#endif

	m_ControlTask->SetOHTInfo(OHTInfo);

	// by zzang9un 2012.12.03 : Main Task Timer�� Ȱ��ȭ
  //	tmMainTask->Enabled = true;
	tmUIupdate->Enabled = true;

    // by zzang9un 2013.12.27 : OHT, AMC Version�� ǥ��
	String strOHTVer = "", strAMCVer = "", strFPGAVer="";
	int nAMCVer = 0;
	int nFPGAVer = 0;

	if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_FOUP)
	{
	    strOHTVer.sprintf(L"OHT_FOUP[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_RETICLE)
	{
	    strOHTVer.sprintf(L"OHT_Reticle[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else if(OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
	    strOHTVer.sprintf(L"OHT_EUV[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	else
	{
	    strOHTVer.sprintf(L"OHT[Ver %u.%u.%u.%u]_%d",
	        (BYTE)OHTMainForm->VerInfo.Major,
	        (BYTE)OHTMainForm->VerInfo.Minor,
	        (BYTE)OHTMainForm->VerInfo.Release,
	        (BYTE)OHTMainForm->VerInfo.Build,
	        INTERNAL_VER);
	}
	

    nAMCVer = m_HWSet->GetAMCVersion();

	int nAMCVerMajor = nAMCVer / 10000;
    int nAMCVerMinor = (nAMCVer - (nAMCVerMajor * 10000)) / 100;
    int nAMCVerRelease = (nAMCVer - (nAMCVerMajor * 10000)) % 100;

    strAMCVer.sprintf(L"AMC[Ver %d.%d.%02d]", nAMCVerMajor, nAMCVerMinor, nAMCVerRelease);


	nFPGAVer= m_HWSet->GetFPGAVersion();
	int nFPGAVerMajor = nFPGAVer / 10000;
	int nFPGAVerMinor = (nFPGAVer - (nFPGAVerMajor * 10000)) / 100;
	int nFPGAVerRelease = (nFPGAVer - (nFPGAVerMajor * 10000)) % 100;

	strFPGAVer.sprintf(L"FPGA[Ver %d.%d.%02d]", nFPGAVerMajor, nFPGAVerMinor, nFPGAVerRelease);


	lbOHTVer->Caption = strOHTVer;
	lbAMCVer->Caption = strAMCVer;
    lblFPGA->Caption =strFPGAVer;


}
//---------------------------------------------------------------------------

/**
@brief   OHT�� ����� Class�� �����ϴ� �Լ�
@author  zzang9un
@date    2012.10.22
@warning �ݵ�� ���� ������ ������ �� Parameter�� Ȯ���ؾ� ��.
*/
void TOHTMainForm::CreateClass()
{
    // -------------------------------------------
    //1 Step 0. ��Ÿ �۾��� �ϴ� Thread
	startLogManager(MAX_BYTE_SIZE_OF_LOG, this->checkLogFolder);

    // by zzang9un 2013.03.20 : Log Class ���� �߰�
	pLogTaskControl = getLogUnit("D:\\log\\MainControl.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogOCSComm = getLogUnit("D:\\log\\OCSComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
    pLogTPComm = getLogUnit("D:\\log\\TPComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	//by puting 2016 1126 : default param load
	m_pParamSet =  new DEFAULT_PARAM_SET();
	memset(m_pParamSet, 0x00, sizeof(DEFAULT_PARAM_SET));
	LoadParameter("DataFiles\\OHTPARAM.ini",m_pParamSet);
	LoadPatternData("DataFiles\\OHTDetectAreaInfo.ini");
	LoadHighSpeedParameter("DataFiles\\HighSpeed.ini");
	WriteLog_NetworkInfo();	// �̴��� ī�� ���� �α� [IPCONFIG]

	m_bErrIotGateway = IoTGatewayCheck() == false;	// IoT ����Ʈ���� üũ

	// SHCHO 2018.10.17
	// �� ���� OHT�� IOT ��ī�忡 Gateway�� ������� ���, ���α׷��� �����Ѵ�
	// OCS Retry ������ ������ ��ġ�� ������
	// �߸��� ���, ���� ���ð������� �߸� �� ���̱� ������
	// �����ڵ� �߻����� ���α׷� ���� �� �缳���� �����Ѵ�

	try
	{
#if(OHT_NAME == OHT_NAME_STD_V80)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP80) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_EUV80) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V81S)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP81S) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V81)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_FOUP81) != 0);
#elif(OHT_NAME == OHT_NAME_STD_V85)
		m_bErrOHTTypeMatching = (strcmp(m_pParamSet->OHTType, OHTTYPE_EUV85) != 0);
#endif

	}catch(...)
	{
    	m_bErrOHTTypeMatching = true;
    }

	if(m_bErrOHTTypeMatching == true)
	{
		AnsiString msg = "## OHT Type�� ���� �ʴ� ������ �����Ͽ����ϴ�. ##\n\n";
		msg += "OHT.exe ���������� OHTType�� �´��� Ȯ���Ͻñ� �ٶ��ϴ�.\n";
		msg += "���� OHT.exe ���������� �´ٸ�, OHTPARAM.ini�� [OHTInfo]�� OHTType Ȯ���Ͻñ� �ٶ��ϴ�.";
		msg += "(FOUP_V81E, EUV_V85,FOUP_V81S,FOUP_V80,EUV_V80)\n";
		msg += "\nȮ�� �� Ȯ�ι�ư�� ������ ���α׷��� ����˴ϴ�.\n";

		wchar_t wmsg[1024];
		msg.WideChar(wmsg, sizeof(wmsg));
		Application->MessageBox(wmsg, L"OHT Type Error");
    }


	if(m_bErrIotGateway == true)
	{
		AnsiString msg = "## ������ ��Ʈ��ũ ����� ���� ���� ##\n\n";
		msg += "IoT�� ����Ǵ� �̴��� ������� Gateway�� �����Ȱ� �����ϴ�.\n";
		msg += "�̴��� ����� ������ �ٽ� Ȯ���� ������.\n\n";
		msg += "���� IoT Gateway Error�� �˻����� �ʱ⸦ ���Ѵٸ�\n";
		msg += "OHTParam.INI�� [WIRELESS] IoT_Lan_Check_Ignore = 1\n���� ���� �����Ͻñ� �ٶ��ϴ�.\n";
		msg += "IoT �̴��� ī�� �̸��� [WIRELESS] IOT_EternetName = ���� ������ �� �ֽ��ϴ�.\n";
		msg += "\nȮ���� ������ ���α׷��� ����˴ϴ�.\n";

		wchar_t wmsg[1024];
		msg.WideChar(wmsg, sizeof(wmsg));
		Application->MessageBox(wmsg, L"IoT Gateway Error");
	}


    // -------------------------------------------
	//1 Step 1. ���� ���� �� ���� ����
//	m_CmdList = new LList;
	m_TaskList = new LList;                 ///< Main Command List
	m_ResponseList= new LList;                 ///< Main Command List
	m_CycleList = new LList;             ///< Main Command List
	m_PathList = new LList;             ///< Main Command List
	m_PassOpenList = new LList;        ///puting ��������� ���� List
	m_PassPermitList = new LList;

	m_UDPComm = new UdpCom(m_pParamSet->nPortNumAdd);
	m_UDPDiag = new UdpDiag;


//	m_UDPComm->MyInfoUpdate()
//	m_UDPComm->Address_Broadcast();   //�ּ�ȹ�� ���� ���н� ��õ� �ؾߵ�.

	gm_ExecuteInfo = new EXECUTE_INFO;
	gm_OHTMainStatus = new OHTMainStatus;
	gm_HWSetStatus = new HWSetStatus;

    //2013.07.11 doori.shin �÷��� ����ü �޸��Ҵ�
	m_MapMake_Status = new MAPMAKE_STATUS;
	memset(m_MapMake_Status, 0x00, sizeof(MAPMAKE_STATUS));
	m_MapMakeProcess = new MapMakeProcess();

//	TEST_ADD_LOG("CreateClass 00 TOHTMainForm = %d", this);
//	TEST_ADD_LOG("CreateClass 00 gm_OHTMainStatus = %d", &gm_OHTMainStatus);

	// by zzang9un 2013.03.19 : Status ���� ����ü �ʱ�ȭ
	memset(gm_OHTMainStatus, 0x00, sizeof(OHTMainStatus));

	m_MapData = new MAP;
	m_TmpMapData = new MAP;
	m_MapDataManager = new MapDataManager;


	//2013.07.09 doori.shin Ŭ���� �ʱ�ȭ

	m_StationDataManager = new StationDataManager();
	m_TeachingDataManager = new TeachingDataManager();

	// -------------------------------------------
	//2016.04.21 KJD ǥ��ȭ �� ���� Ŭ���� �ʱ�ȭ
	m_CIDData_STD = new STD_CID;
	m_CollisionData_STD = new STD_Collision;
	m_FlowControlData_STD = new STD_FlowControl;
	m_MapData_STD = new STD_MAP;
	m_StationData_STD = new STD_Station;
	m_TeachingData_STD = new STD_Teaching;
	m_PathData_STD = new STD_PathData;

	m_CIDManager_STD = new CIDManager_STD;
	m_CollisionManager_STD = new CollisionManager_STD;
	m_FlowControlManager_STD = new FlowControlManager_STD;
	m_MapManager_STD = new MapDataManager_STD;
	m_StationManager_STD = new StationManager_STD;
	m_TeachingManager_STD = new TeachingManager_STD;
	m_PathDataManager_STD = new PathDataManager_STD;

	//File Map Teahcing puting
//	m_MapMakerFile_STD = new STD_MapMakerFile;
	m_MapMakerFileManager_STD = new MapMakerFileManager_STD;


	// -------------------------------------------
	//1 2. Command Manager �� Pathsearch
	m_PathSearch    = new PathSearch();
	m_CmdManager    = new CmdManager();
	m_PassPermit    = new PassPermit();

    // -------------------------------------------
    // 3. HW Set
    m_HWCommon = new HWCommon;
	m_HWSet = new HWSet();

	if(m_HWSet!=NULL)
    {
        m_pOHTDetect = m_HWSet->m_pOHTDetect;
    }
//	m_AvsControl = new AvsControl();

	m_HWControlThread = new HWControlThread();

	m_MaskOfHW = m_HWCommon->GetMask();

    // -------------------------------------------
    //1 4. D, T, M, Execute& LogThread
    m_DrivingControl = new DrivingControl();
	m_TransControl = new TransControl();
	m_ManualControl = new ManualControl();
	m_ExecuteThread = new ExecuteThread();
	m_LogThread = new LogThread();//2013.07.11 �߰�
    // -------------------------------------------
    //1 5. Status Thread
    m_StatusCheckThread = new StatusCheckThread();
	m_ControlTask = new TaskControl();
	m_TaskControlThread = new TaskControlThread();

	m_ExtraJobThread = new ExtraJobThread();
//	m_AvsControlThread = new AvsControlThread();
  //	m_TaskControlThread->setMainPointer((void*)this);

 	//1 6. MonitorControl Thread
//	m_HID = new HID_VII_ZR(COM_HID);     //��ġ �ű��
//	m_IPCLoadLib = new IPCLibrary();
//	m_NetworkDll = new NetworkSinalLib();
//	m_CPUandMemInfo = new CPUandMemInfo();
//	m_EtcMonitoring = new MONITORING;
	//m_SigmaMonitor = NULL;	//new SigmaMonitor();
//	m_MonitorControlThread = new MonitorControlThread(gm_OHTMainStatus, m_HID, m_NetworkDll, m_IPCLoadLib, m_CPUandMemInfo, m_EtcMonitoring);

	// -------------------------------------------
	//1 6-0. Thread �켱 ���� ����
	m_LogThread->setPriority(THREAD_PRIORITY_LOWEST);    // �켱 ������ ������: THREAD_PRIORITY_BELOW_NORMAL
	m_HWControlThread->setPriority(THREAD_PRIORITY_HIGHEST);
	m_ExecuteThread->setPriority(THREAD_PRIORITY_HIGHEST);
	m_ExtraJobThread->setPriority(THREAD_PRIORITY_NORMAL);
//	m_AvsControlThread->setPriority(THREAD_PRIORITY_NORMAL);
//	m_MonitorControlThread->setPriority(THREAD_PRIORITY_LOWEST);

	// -------------------------------------------
	//1 6. Thread ����
	m_LogThread->beginThread();    // ���� ���� ������ �� : ��� ���� ó��
	m_ExtraJobThread->beginThread();
	m_HWControlThread->beginThread();
	m_ExecuteThread->beginThread();
	m_StatusCheckThread->beginThread();
	m_TaskControlThread->beginThread();
//	m_MonitorControlThread->beginThread();
  //	TEST_ADD_LOG("CreateClass 01 m_socketThread = %d", m_socketThread);

	//2014.04.15 doori.shin Thread Start
	if(m_HWSet != NULL) m_pServoControl = m_HWSet->m_pServoMonitoring;

	tmChkThreadStatus->Enabled = true;

//	m_ControlTask->m_pUBGParamSet = OHTMainForm->m_PathSearch->m_pUBGParamSet;	// TaskControl�� UBG �������� ������ �� ����
//	m_StatusCheckThread->m_pHWCommon = GetHWCommon();
}
//---------------------------------------------------------------------------

/**
@brief   OHT�� ����� Class�� �Ҹ��ϴ� �Լ�
@author  zzang9un
@date    2012.10.22
@warning �ݵ�� ���� ������ �������� delete ó��
*/
void TOHTMainForm::DestroyClass()
{
//	Test_RESET_LOG;

	//DWORD dwTime = timeGetTime();
    // -------------------------------------------
    // step 1. Thread ����
    // by zzang9un 2012.11.15 (���¿� ���� Ȯ��): Thread ���� ���� delete�� �ص� ��
//	TEST_ADD_LOG("Destroy 00");
//	ADD_EXCEPT_LOG("Destroy 00");
	//delete m_socketThread;
	m_HWSet->m_pDrivingAxis->MoveStop(0.0035);
	m_HWCommon->SetBit(OUT_BRAKE_DRIVING_RELEASE, false);
	if (m_pParamSet->IsLCDAndLED)
	{
		m_StatusCheckThread->GetReadLCDData(true); // LCD�� ���� OHTOFF ó��
    }
//	//��� ȣ�� or Main Close ���ȱ��� by Jrespect 2018.03.10
//	m_HWSet->m_pLamp->LampOff();//Main ����� LED OFF
	Sleep(100);

	m_StatusCheckThread->qiutRealThreadProc();
	m_ExecuteThread->qiutRealThreadProc();
	m_HWControlThread->qiutRealThreadProc();
	m_LogThread->qiutRealThreadProc();
	m_TaskControlThread->qiutRealThreadProc();
	m_ExtraJobThread->qiutRealThreadProc();
//	m_AvsControlThread->qiutRealThreadProc();
//	m_MonitorControlThread->qiutRealThreadProc();

//	ADD_EXCEPT_LOG("Destroy 01");

	//dwTime = timeGetTime();

//	dwTime = timeGetTime();
//	while(timeGetTime()-dwTime<5000)	// �ӽ� Thread Timeout
	while(1)	// �ӽ� Thread Timeout
	{
		if(	m_StatusCheckThread->isOnThreading()==false &&
			m_ExecuteThread->isOnThreading()==false &&
			m_HWControlThread->isOnThreading()==false &&
			m_LogThread->isOnThreading()==false &&
			m_TaskControlThread->isOnThreading()==false &&
			m_ExtraJobThread->isOnThreading()==false)// &&
		   //	m_AvsControlThread->isOnThreading()==false)
		{
			break;
		}
		Sleep(1);
	}
//    ADD_EXCEPT_LOG("Destroy 02");
	//SHCHO 2018.08.06 : oht.exe ���� ���� �ؼҵ�. DiagManager �Ҹ��� ȣ��Ǿ� timer ��������
	DiagManager::DestroyInstance();

//    TEST_ADD_LOG("Destroy 1 ~ Interval:%d", (timeGetTime()-dwTime));

	delete m_StatusCheckThread;
    delete m_ExecuteThread;
	delete m_HWControlThread;
	delete m_LogThread;
	delete m_TaskControlThread;
//	delete m_MonitorControlThread;

//	ADD_EXCEPT_LOG("Destroy 03");

	// -------------------------------------------

	delete m_ControlTask;
	/*
	// Socket Class ����
	delete m_OCSSocket;
	delete m_TPSocket;
	*/

//	ADD_EXCEPT_LOG("Destroy 04");

    // -------------------------------------------
    // Step 2. Execute ���ο��� ����Ǵ� Class ����
    delete m_DrivingControl;
    delete m_TransControl;
    delete m_ManualControl;

//	ADD_EXCEPT_LOG("Destroy 05");


    // -------------------------------------------
    // Step 3. HW Set
    delete m_HWSet;
    delete m_HWCommon;

//	delete m_AvsControl;
//	ADD_EXCEPT_LOG("Destroy 06");

    // -------------------------------------------
    // Step 4. ���� ����, ���, ���� List ����
//  delete m_OCSSocket;
//    delete m_TPSocket;
	delete m_PassPermit;
	delete m_CmdManager;
	delete m_PathSearch;

//	ADD_EXCEPT_LOG("Destroy 07");
    delete m_MapMakerFileManager_STD;

	delete m_PathDataManager_STD;
	delete m_TeachingManager_STD;
	delete m_StationManager_STD;

	delete m_MapManager_STD;
	delete m_FlowControlManager_STD;
    delete m_CollisionManager_STD;
	delete m_CIDManager_STD;

	delete m_PathData_STD;
	delete m_TeachingData_STD;
    delete m_StationData_STD;
	delete m_MapData_STD;
	delete m_FlowControlData_STD;
	delete m_CollisionData_STD;
	delete m_CIDData_STD;

	delete m_TeachingDataManager;
	delete m_StationDataManager;

	delete m_MapDataManager;
	delete m_TmpMapData;

	if(m_MapData != NULL)
		delete m_MapData;	//<-- ���� �߻�

	delete m_MapMakeProcess;
	delete m_MapMake_Status;



	delete gm_HWSetStatus;
	delete gm_OHTMainStatus;
	delete gm_ExecuteInfo;

	delete m_UDPDiag;
	delete m_UDPComm;

//	ADD_EXCEPT_LOG("Destroy 11");

//	delete m_CmdList;
	delete m_PassPermitList;
	delete m_PassOpenList;        ///puting ��������� ���� List
	delete m_PathList;             ///< Main Command List
	delete m_CycleList;             ///< Main Command List
	delete m_ResponseList;                 ///< Main Command List
	delete m_TaskList;                ///< Main Command List

    delete m_pParamSet;

//	ADD_EXCEPT_LOG("Destroy 12");

    endLogManager();

//    TEST_ADD_LOG("Destroy 9");

	tmChkThreadStatus->Enabled = false;

//	Sleep(1000);

//	TEST_ADD_LOG("Destroy 10");

}

void __fastcall TOHTMainForm::tmUIupdateTimer(TObject *Sender)
{
	Update_UI();
}
//---------------------------------------------------------------------------

/**
@brief   Remocon ��ü�� ��ȯ�ϱ� ���� �Լ�
@author  ji7412.lee
@date    2013.05.27
*/
Remocon* TOHTMainForm::GetHWSetRemocon()
{
	if(m_HWSet->m_pRemocon != NULL)
	{
		return m_HWSet->m_pRemocon;
	}
	return NULL;
}


//---------------------------------------------------------------------------

/**
@brief   LCD ��ü�� ��ȯ�ϱ� ���� �Լ�
@author  kyh
@date    2017.12.12
*/
LCD* TOHTMainForm::GetHWSetLCD()
{
	if(m_HWSet->m_pLCD != NULL)
	{
		return m_HWSet->m_pLCD;
	}
	return NULL;
}


/**
@brief   Hand�� Shutter Origin ���¸� ���� ���� �Լ�
         ������ ���� Ȯ�� �߰� : ������ ���ΰ�� AUTO�� ��ȯ �ȵ�.
@author  elibeli2
@date    2014.01.23
*/

// Manual_CID()

int TOHTMainForm::GetReadyToOperation()
{
	int nError = NO_ERR;
	int tmpCurrenrtNodeID = 0;

	// ������ Ȯ���ϱ� ���Ͽ� �� �����Ͽ� ���
	unsigned int CID_DataType = GetCIDDataManager_STD()->m_CIDFileHeader.DataType;
    unsigned int Collision_DataType = GetCollisionDataManager_STD()->m_CollisionFileHeader.DataType;
    unsigned int FlowControl_DataType = GetFlowControlDataManager_STD()->m_FlowControlFileHeader.DataType;
    unsigned int Map_DataType = GetMapDataManager_STD()->m_MapFileHeader.DataType;
    unsigned int Station_DataType = GetStationDataManager_STD()->m_StationFileHeader.DataType;
    unsigned int Teaching_DataType = GetTeachingDataManager_STD()->m_TeachingFileHeader.DataType;
    unsigned int Path_DataType = GetPathDataManager_STD()->m_PathDataFileHeader.DataType;
	// By elibeli2 2014.01.23 Origin Check
	nError = m_HWSet->m_pHand->ReadyToOperation();
	if( nError == NO_ERR)
	{
		 if(m_HWSet->m_pHand->IsInposition() ==false)
		 {
           	nError = ERR_HAND_NOTORIGIN;
         }
    }
	if( nError == NO_ERR)
	{
		nError = m_HWSet->m_pShutter->ReadyToOperation();
		if(nError == NO_ERR)
		{
			if(m_HWSet->m_pShutter->IsInposition(BOTH) ==false)
				nError = ERR_SHUTTER_NOTORIGIN;
		}
	}


	if( nError == NO_ERR)
	{
		if(((m_HWSet->m_pInnerFoup->IsFoupDetect()== false)  &&(m_HWSet->m_pHand->IsClose() ==true)) ||
			((m_HWSet->m_pInnerFoup->IsFoupDetect()== true)  &&(m_HWSet->m_pHand->IsOpen() ==true)))
				nError = ERR_HAND_NOTORIGIN;
	}

    if( nError == NO_ERR)
	{
		if(m_HWSet->m_pHand->IsPushOn()==true)
			nError = ERR_HAND_PUSHSEN_ABNORMAL_ON;
	}


	//������OK�÷��� ó�� Shift/Hoist Home ��ġ Ȯ��_ 2017 06 04
	if( nError == NO_ERR)
	{
		if((fabs(m_HWSet->m_pShiftAxis->GetCurrentPosition()) >0.5)
			|| (m_HWSet->m_pShiftAxis->IsShiftHome() ==false))

		{
			nError = ERR_SHIFT_NOTORIGIN;
		}

	}

	if( nError == NO_ERR)
	{
#if(OHT_NAME != OHT_NAME_STD_V85)
		if((m_HWSet->m_pHoistAxis->IsHoistHome()==false)

//#ifdef OHT_HIGH_RAIL_CHECK_HOME      //puting ���� Ȯ�� 20190402
//		|| ((m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladd() < (HOIST_SMALL_ADD_HOISTHOME_DISTANCE + 0.5))
//		&& (m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladd() > (HOIST_SMALL_ADD_HOISTHOME_DISTANCE - 0.5))))
//#else
		|| (fabs(m_HWSet->m_pHoistAxis->GetCurrentPosition()) > HOIST_HOME_INPOS_LIMIT))
//#endif
		{
			nError = ERR_HOIST_NOTORIGIN;
		}
#else
		if((m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd()==false)
		|| (m_HWSet->m_pHoistAxis->IsHoistHome()==false)
		|| (fabs(m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition()) > HOIST_HOME_INPOS_LIMIT))
		{
			if(m_HWSet->m_pHoistAxis->IsHoistHome()==false)
				ADD_MEMO_T("IsHoistHome");
			else if(m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd()==false)
				ADD_MEMO_T("IsHoistOriginFromSmalladd");

			//ADD_EXCEPT_LOG("Hoist Origin Error, %d %f",m_HWSet->m_pHoistAxis->IsHoistOriginFromSmalladd(),m_HWSet->m_pHoistAxis->GetRelativeDistanceFromSmalladdCurrentPosition());
			nError = ERR_HOIST_NOTORIGIN;
		}
#endif



	}

// ������ �� ����� �������� �ʴ� ���� �ӽ���ġ
// ���߿� �� Ǯ����� �Ѵ�
    if( nError == NO_ERR)
	{
		if( ((CID_DataType & bit_0) == bit_0)
	  ||((Collision_DataType & bit_0) == bit_0)
	  ||((FlowControl_DataType & bit_0) == bit_0)
	  ||((Map_DataType & bit_0) == bit_0)
	  ||((Station_DataType & bit_0) == bit_0)
	  ||((Teaching_DataType & bit_0) == bit_0)
	  ||((Path_DataType & bit_0) == bit_0) )
		{
			// �� ������ �� ������ ���� ���Ե�.
			// ���� ���� ����
			nError = ERR_AUTO_DATAUPLOAD;
		}
		else
		{


			nError = NO_ERR;
		}
	}


	
	// ���±� 2016 03 23
	if(nError == NO_ERR)
	{
		tmpCurrenrtNodeID = (int)gm_ExecuteInfo->ExecutePositionInfo.CurrNode;
		nError = m_HWSet->m_pCID->CID_Manual_Mode_Off();
		if (nError == NO_ERR)
		{
			if((gm_ExecuteInfo->ExecutePositionInfo.CurrStation == 0) &&(tmpCurrenrtNodeID !=0))
			{
				nError = m_HWSet->m_pCID->ReadyToOperation(tmpCurrenrtNodeID, false);

			}else if((gm_ExecuteInfo->ExecutePositionInfo.CurrStation != 0) &&(tmpCurrenrtNodeID !=0))
			{
				STATION_DATA *pSData;

				pSData = GetStationDataManager()->Find(gm_ExecuteInfo->ExecutePositionInfo.CurrStation);

				if (pSData == NULL)
					nError = ERR_DATA_STATION_NOEXIST;
				else
                	nError = m_HWSet->m_pCID->ReadyToOperation(pSData->NextNodeID, true);

			}
		}

	}
	//AOHC-125 Auto ��ȯ ��, �ڰ� ���� ��� �߰� 
	// KYH 2019.01.21. FoupCoverDetect ������ ������ ��� Error
	if(nError == NO_ERR)
	{
		if(m_HWSet->m_pFoupCoverOpenDetect->IsDetect() == true)
			nError = ERR_FOUP_COVER_OPEN_DETECT;
	}

	// KYH 2019.01.21. Regulator ������ ���� ��� Error
	if(nError == NO_ERR)
	{
		if(GetOHTStatus()->m_cpMonitorData.HIDData.Ecap <= 240)
			nError = ERR_ECAP_OVERVOLTAGE_RUN; //������� �ʴ� ������ ���..
	}

	// KYH 2019.01.21. Steering ���� ���� Ȯ�� (�б� Node Station ����)
	if(nError == NO_ERR)
	{
		if(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_NODE)
		{
			if(m_HWSet->m_pSteer->IsLeftPos(BOTH) == true)
			{
				nError = m_HWSet->m_pSteer->Right(false);
			}
			else if(m_HWSet->m_pSteer->IsRightPos(BOTH) == true)
			{
				nError = m_HWSet->m_pSteer->Left(false);
			}
			else
			{
				nError = ERR_STEER_NOTSAMESTATUS;
			}
		}
		else if((gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_TAG_STATION) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_QR_STATION_LEFT) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_QR_STATION_RIGHT) ||
		(gm_ExecuteInfo->ExecutePositionInfo.StationType == MARK_TYPE_TAG_STATION_BACK))
		{
			int nID = gm_ExecuteInfo->ExecutePositionInfo.CurrNode;
			NODE_INFO_STD_MAP *pNode = GetNodeObj(nID);

			if(pNode != NULL)
			{
				if(pNode->LeftSteering == 0) //0 = N, 1 = L, 2 = R ���ʸ� ������ ������ �����Ƿ�
				{
					if(m_HWSet->m_pSteer->IsLeftPos(BOTH) == true)
					{
						nError = m_HWSet->m_pSteer->Right(false);
					}
					else if(m_HWSet->m_pSteer->IsRightPos(BOTH) == true)
					{
						nError = m_HWSet->m_pSteer->Left(false);	
					}
					else
					{
						nError = ERR_STEER_NOTSAMESTATUS;
					}
				}
			}
		}
		else
		{
		//ADD_LOG_TASKCONTROL("��ũ �ȵǾ� Steering NG");
			//Mark �ȵ�.
		}
	}

	if(nError == NO_ERR)
	{
		//AOHC-256  M -> A UBG Open Com
		//UBG ���� 0 �� üũ ��, Open com ���� �������� �ȵ� ��� ��Ȯ���ϱ� ���Ͽ� �߰�
		nError =  m_pOHTDetect->CheckUBGConnect(true);

	}


	return nError;
}

/**
@brief   Application�� Exception ������ �߻��ϸ� �α׸� ��� ȭ���� Capture�ϱ� ���� �Լ�
@author  LSJ
@date    2013.10.17
*/
void __fastcall TOHTMainForm::AppException(TObject *Sender, Exception *E)
{
	ADD_EXCEPT_LOG("%s", E->Message);

	if(gm_LogManager->Get_LogFolder())
	{
		CaptureScreen();
	}
}

/**
@brief   ������ Windows ȭ���� Capture�ϱ� ���� �Լ�
@author  LSJ
@date    2013.10.17
*/
void TOHTMainForm::CaptureScreen()
{
   struct date today;
   struct time currenttime;
   AnsiString strFullName = "";
   AnsiString strOHTPath=".";
   AnsiString strLogPath=".";

   getdate(&today);
   gettime(&currenttime);

   HWND hwnd = GetDesktopWindow ();
   if(hwnd)
   {
      HDC dc = ::GetWindowDC(hwnd);
      Graphics::TCanvas *pCanvas = new Graphics::TCanvas;
      pCanvas->Handle = dc;

      TRect rect = pCanvas->ClipRect;
      TImage *pImg = new TImage(OHTMainForm);
      pImg->Picture->Bitmap->Width  = rect.Width();
      pImg->Picture->Bitmap->Height = rect.Height();
      pImg->Picture->Bitmap->Canvas->CopyRect(rect, pCanvas, rect);

      strOHTPath=ExtractFileDir(Application->ExeName);

	  strFullName.sprintf("D:\\log\\MainForm%04d%02d%02d%02d%02d.bmp",
						  today.da_year, today.da_mon, today.da_day,currenttime.ti_hour,currenttime.ti_sec);

	  pImg->Picture->SaveToFile(strFullName);
      delete pImg;
	  delete pCanvas;
      ::ReleaseDC(hwnd, dc);
   }
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkAxisClick(TObject *Sender)
{
    TCheckBox   *CB_InputMask   = chkAxis;

    TEdit       *Edt_Mask_Value = edtAxisMask_Value;

    int     nAxisNum    = cbbAxisNum->ItemIndex;
    int     nFuncNum    = cbbFnType->ItemIndex;
    int     nValue      = Edt_Mask_Value->Text.ToIntDef(0);
    bool    bChecked    = CB_InputMask->Checked;

    MASK_AXIS   *pMaskOfAxis    = NULL;

    // �Է� �� feedback
    if(nAxisNum==-1)
    {
        cbbAxisNum->ItemIndex = 0;
    }
    if(nFuncNum==-1)
    {
		cbbFnType->ItemIndex = 0;
    }
    if(nAxisNum==-1 || nFuncNum==-1)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }
    Edt_Mask_Value->Text   = (int)nValue;

    // HW Mask�� ���� �� ó��
    if(m_MaskOfHW==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
    }

    // Axis Mask�� ���� �� ó��
    pMaskOfAxis = m_MaskOfHW->getMaskOfAxis(nAxisNum);
    if(pMaskOfAxis==NULL)
    {
        if(bChecked==true)
        {
            CB_InputMask->Checked = false;
        }
        return;
	}

    // �Է� ���� ������� �ʰ� ����
    cbbAxisNum->Enabled   = !bChecked;
    cbbFnType->Enabled    = !bChecked;
    Edt_Mask_Value->Enabled     = !bChecked;
    Edt_Mask_Value->ReadOnly    = bChecked;

    // �� Masking
    switch(nFuncNum)
    {
    case AXIS_MASK_FUNC_POSITION:
        if(bChecked==true)
        {
            pMaskOfAxis->Position.Value = nValue;
            pMaskOfAxis->Position.bUse  = true;
        }
        else
        {
            pMaskOfAxis->Position.bUse = false;
        }
        break;

    case AXIS_MASK_FUNC_AMP_ENABLE:
        if(bChecked==true)
        {
            pMaskOfAxis->AmpEnable.Value    = nValue;
            pMaskOfAxis->AmpEnable.bUse     = true;
        }
        else
        {
			pMaskOfAxis->AmpEnable.bUse     = false;
        }
        break;

    case AXIS_MASK_FUNC_RUNNING:
        if(bChecked==true)
        {
            pMaskOfAxis->Running.Value    = nValue;
            pMaskOfAxis->Running.bUse     = true;
        }
        else
        {
            pMaskOfAxis->Running.bUse     = false;
        }
        break;

    case AXIS_MASK_FUNC_ERRORCODE:
        if(bChecked==true)
        {
            pMaskOfAxis->ErrorCode.Value    = nValue;
            pMaskOfAxis->ErrorCode.bUse     = true;
        }
        else
        {
            pMaskOfAxis->ErrorCode.bUse     = false;
        }
        break;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
    case AXIS_MASK_FUNC_ORIGIN:
		if(bChecked==true)
        {
            pMaskOfAxis->Origin.Value    = nValue;
            pMaskOfAxis->Origin.bUse     = true;
		}
        else
        {
            pMaskOfAxis->Origin.bUse     = false;
        }
        break;
#endif
    default:
        break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask1Click(TObject *Sender)
{
    SetInputMask(chkInputMask1, edtInputMask_Num1, edtInputMask_Value1);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask2Click(TObject *Sender)
{
    SetInputMask(chkInputMask2, edtInputMask_Num2, edtInputMask_Value2);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask3Click(TObject *Sender)
{
    SetInputMask(chkInputMask3, edtInputMask_Num3, edtInputMask_Value3);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask4Click(TObject *Sender)
{
     SetInputMask(chkInputMask4, edtInputMask_Num4, edtInputMask_Value4);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask5Click(TObject *Sender)
{
     SetInputMask(chkInputMask5, edtInputMask_Num5, edtInputMask_Value5);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask6Click(TObject *Sender)
{
    SetInputMask(chkInputMask6, edtInputMask_Num6, edtInputMask_Value6);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask7Click(TObject *Sender)
{
    SetInputMask(chkInputMask7, edtInputMask_Num7, edtInputMask_Value7);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkInputMask8Click(TObject *Sender)
{
    SetInputMask(chkInputMask8, edtInputMask_Num8, edtInputMask_Value8);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::imgIOTestDblClick(TObject *Sender)
{
	if(tabIOTest->TabVisible)
        tabIOTest->TabVisible = false;
    else
		tabIOTest->TabVisible = true;

	if(Tab81S->TabVisible)
		Tab81S->TabVisible = false;
	else
		Tab81S->TabVisible = true;


}
//---------------------------------------------------------------------------


/**
@brief   OHTMain���� ������ UDPCOm �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  TaskControl ������, ������ ���� ���� ��� Null�� ����
*/
UdpCom* TOHTMainForm::GetUDPCom()
{
	if(this->m_UDPComm == NULL)
		return NULL;
	else
		return this->m_UDPComm;
}


/**
@brief   OHTMain���� ������ UDPDiag �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  TaskControl ������, ������ ���� ���� ��� Null�� ����
*/
UdpDiag* TOHTMainForm::GetUDPDiag()
{
	if(this->m_UDPDiag == NULL)
		return NULL;
	else
		return this->m_UDPDiag;
}


/**
@brief   OHTMain���� ������ TaskControl�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  TaskControl ������, ������ ���� ���� ��� Null�� ����
*/
TaskControl* TOHTMainForm::GetTaskControl()
{
    if(this->m_ControlTask == NULL)
        return NULL;
    else
        return this->m_ControlTask;
}
//---------------------------------------------------------------------------

///**
//@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
//@author  zzang9un
//@date    2014.02.06
//@return  LList ������, ������ ���� ���� ��� Null�� ����
//*/
//LList* TOHTMainForm::GetCmdList()
//{
//    if(this->m_CmdList == NULL)
//        return NULL;
//    else
//        return this->m_CmdList;
//}

//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetTaskList()
{
    if(this->m_TaskList == NULL)
        return NULL;
    else
		return this->m_TaskList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetCycleList()
{
    if(this->m_CycleList == NULL)
        return NULL;
    else
        return this->m_CycleList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetResponseList()
{
    if(this->m_ResponseList == NULL)
        return NULL;
    else
        return this->m_ResponseList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetPathList()
{
    if(this->m_PathList == NULL)
        return NULL;
    else
		return this->m_PathList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetPassOpenList()
{
	if(this->m_PassOpenList == NULL)
		return NULL;
	else
		return this->m_PassOpenList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ LList�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  LList ������, ������ ���� ���� ��� Null�� ����
*/
LList* TOHTMainForm::GetPassPermitList()
{
	if(this->m_PassPermitList == NULL)
		return NULL;
	else
		return this->m_PassPermitList;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ EXECUTE_INFO�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  EXECUTE_INFO ������, ������ ���� ���� ��� Null�� ����
*/
EXECUTE_INFO* TOHTMainForm::GetExecuteInfo()
{
    if(this->gm_ExecuteInfo == NULL)
        return NULL;
    else
        return this->gm_ExecuteInfo;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ OHT_STATUS�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  OHT_STATUS ������, ������ ���� ���� ��� Null�� ����
*/
OHTMainStatus* TOHTMainForm::GetOHTStatus()
{
    if(this->gm_OHTMainStatus == NULL)
        return NULL;
    else
        return this->gm_OHTMainStatus;    
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ HWSET_STATUS�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  HWSET_STATUS ������, ������ ���� ���� ��� Null�� ����
*/
HWSetStatus* TOHTMainForm::GetHWSetStatus()
{
	if(this->gm_HWSetStatus == NULL)
		return NULL;
	else
		return this->gm_HWSetStatus;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ MAPMAKE_STATUS�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  MAPMAKE_STATUS ������, ������ ���� ���� ��� Null�� ����
*/
MAPMAKE_STATUS* TOHTMainForm::GetMapMakeStatus()
{
	if(this->m_MapMake_Status == NULL)
        return NULL;
    else
        return this->m_MapMake_Status;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ MAP�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
MAP* TOHTMainForm::GetMap()
{
    if(this->m_MapData== NULL)
        return NULL;
    else
        return this->m_MapData;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ STD_CID MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_CID* TOHTMainForm::GetCID_STD()
{
    if(this->m_CIDData_STD == NULL)
        return NULL;
    else
        return this->m_CIDData_STD;
}
//---------------------------------------------------------------------------


/**
@brief   OHTMain���� ������ STD_Collision MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_Collision* TOHTMainForm::GetCollision_STD()
{
    if(this->m_CollisionData_STD == NULL)
        return NULL;
    else
        return this->m_CollisionData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ STD_FlowControl MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_FlowControl* TOHTMainForm::GetFlowControl_STD()
{
    if(this->m_FlowControlData_STD == NULL)
        return NULL;
    else
        return this->m_FlowControlData_STD;
}

//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ STD_MAP MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_MAP* TOHTMainForm::GetMap_STD()
{
    if(this->m_MapData_STD == NULL)
        return NULL;
    else
        return this->m_MapData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ STD_Station MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_Station* TOHTMainForm::GetStation_STD()
{
    if(this->m_StationData_STD == NULL)
        return NULL;
    else
        return this->m_StationData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ STD_Teaching MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_Teaching* TOHTMainForm::GetTeaching_STD()
{
    if(this->m_TeachingData_STD == NULL)
        return NULL;
    else
        return this->m_TeachingData_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ STD_PathData MAP�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  MAP ������, ������ ���� ���� ��� Null�� ����
*/
STD_PathData* TOHTMainForm::GetPathData_STD()
{
	if(this->m_PathData_STD == NULL)
		return NULL;
	else
        return this->m_PathData_STD;
}
//---------------------------------------------------------------------------


/**
@brief   OHTMain���� ������ MapDataManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  MapDataManager ������, ������ ���� ���� ��� Null�� ����
*/
MapDataManager* TOHTMainForm::GetMapDataManager()
{
    if(this->m_MapDataManager == NULL)
        return NULL;
    else
        return this->m_MapDataManager;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ StationDataManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  StationDataManager ������, ������ ���� ���� ��� Null�� ����
*/
StationDataManager* TOHTMainForm::GetStationDataManager()
{
	if(this->m_StationDataManager == NULL)
		return NULL;
	else
		return this->m_StationDataManager;
}
//---------------------------------------------------------------------------
 /**
@brief   OHTMain���� ������ StationDataManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  StationDataManager ������, ������ ���� ���� ��� Null�� ����
*/
MapMakeProcess* TOHTMainForm::GetMapMakeProcess()
{
	if(this->m_MapMakeProcess == NULL)
		return NULL;
	else
		return this->m_MapMakeProcess;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ TeachingDataManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  TeachingDataManager ������, ������ ���� ���� ��� Null�� ����
*/
TeachingDataManager* TOHTMainForm::GetTeachingDataManager()
{
    if(this->m_TeachingDataManager == NULL)
        return NULL;
    else
        return this->m_TeachingDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ CIDManager_STD DataManager�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_CIDDataManager ������, ������ ���� ���� ��� Null�� ����
*/
CIDManager_STD* TOHTMainForm::GetCIDDataManager_STD()
{
    if(this->m_CIDManager_STD == NULL)
        return NULL;
    else
        return this->m_CIDManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ CollisionManager_STD DataManager�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_CollisionManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
CollisionManager_STD* TOHTMainForm::GetCollisionDataManager_STD()
{
    if(this->m_CollisionManager_STD == NULL)
        return NULL;
    else
        return this->m_CollisionManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ FlowControlManager_STD DataManager�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_FlowControlManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
FlowControlManager_STD* TOHTMainForm::GetFlowControlDataManager_STD()
{
    if(this->m_FlowControlManager_STD == NULL)
        return NULL;
    else
        return this->m_FlowControlManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ m_MapManager_STD�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_MapManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
MapDataManager_STD* TOHTMainForm::GetMapDataManager_STD()
{
    if(this->m_MapManager_STD == NULL)
        return NULL;
    else
        return this->m_MapManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ m_StationManager_STD�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_StationManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
StationManager_STD* TOHTMainForm::GetStationDataManager_STD()
{
    if(this->m_StationManager_STD == NULL)
        return NULL;
    else
        return this->m_StationManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ m_TeachingManager_STD�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_TeachingManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
TeachingManager_STD* TOHTMainForm::GetTeachingDataManager_STD()
{
    if(this->m_TeachingManager_STD == NULL)
        return NULL;
    else
        return this->m_TeachingManager_STD;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ m_PathDataManager_STD�� �����ϴ� �Լ�
@author  KJD
@date    2016.04.20
@return  m_PathDataManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
PathDataManager_STD* TOHTMainForm::GetPathDataManager_STD()
{
	if(this->m_PathDataManager_STD == NULL)
		return NULL;
	else
		return this->m_PathDataManager_STD;
}


/**
@brief   OHTMain���� ������ m_PathDataManager_STD�� �����ϴ� �Լ�
@author  puting
@date    2016.12.20
@return  m_PathDataManager_STD ������, ������ ���� ���� ��� Null�� ����
*/
MapMakerFileManager_STD* TOHTMainForm::GetMapMakerFileManager_STD()
{
	if(this->m_MapMakerFileManager_STD == NULL)
		return NULL;
	else
		return this->m_MapMakerFileManager_STD;
}
//---------------------------------------------------------------------------
DEFAULT_PARAM_SET* TOHTMainForm::GetDefaultParam()
{
	if(this->m_pParamSet == NULL)
		return NULL;
	else
		return this->m_pParamSet;
}
/**
@brief   OHTMain�� Map�� �����ϴ� �Լ�
@author  doori.shin
@date    2015.03.08
@param  Map : ������ Map ������
*/

void TOHTMainForm::SetMap(MAP* mapData)
{
	this->m_MapData = mapData;
}

//---------------------------------------------------------------------------
void TOHTMainForm::SetCID_STD(STD_CID* CIDData)
{
	this->m_CIDData_STD = CIDData;
}
void TOHTMainForm::SetCollision_STD(STD_Collision* CollisionData)
{
	this->m_CollisionData_STD = CollisionData;
}
void TOHTMainForm::SetFlowControl_STD(STD_FlowControl* FlowControlData)
{
	this->m_FlowControlData_STD = FlowControlData;
}
void TOHTMainForm::SetMap_STD(STD_MAP* MapData)
{
	this->m_MapData_STD = MapData;
}
void TOHTMainForm::SetStation_STD(STD_Station* StationData)
{
 	this->m_StationData_STD = StationData;
}
void TOHTMainForm::SetTeaching_STD(STD_Teaching* TeachingData)
{
	this->m_TeachingData_STD = TeachingData;
}
void TOHTMainForm::SetPathData_STD(STD_PathData* PathData)
{
	this->m_PathData_STD = PathData;
}


void TOHTMainForm::SetCIDDataManager_STD(CIDManager_STD* CIDDataManager)
{
	this->m_CIDManager_STD = CIDDataManager;
}
void TOHTMainForm::SetCollisionDataManager_STD(CollisionManager_STD* CollisionDataManager)
{
	this->m_CollisionManager_STD = CollisionDataManager;
}
void TOHTMainForm::SetFlowControlDataManager_STD(FlowControlManager_STD* FlowControlDataManager)
{
	this->m_FlowControlManager_STD = FlowControlDataManager;
}
void TOHTMainForm::SetapDataManager_STD(MapDataManager_STD* MapDataManager)
{
	this->m_MapManager_STD = MapDataManager;
}
void TOHTMainForm::SetStationDataManager_STD(StationManager_STD* StationDataManager)
{
	this->m_StationManager_STD = StationDataManager;
}
void TOHTMainForm::SetTeachingDataManager_STD(TeachingManager_STD* TeachingDataManager)
{
	this->m_TeachingManager_STD = TeachingDataManager;
}
void TOHTMainForm::SetPathDataManager_STD(PathDataManager_STD* PathDataManager)
{
	this->m_PathDataManager_STD = PathDataManager;
}
void TOHTMainForm::SetMapMakerFileManager_STD(MapMakerFileManager_STD* MapMakerFileData)
{
	this->m_MapMakerFileManager_STD = MapMakerFileData;
}

/**
@brief   OHTMain�� MapDataManager�� �����ϴ� �Լ�
@author  doori.shin
@date    2015.03.08
@param  MapDataManager : ������ ������
*/

void TOHTMainForm::SetMapDataManager(MapDataManager* mapDataManager)
{
	this->m_MapDataManager = mapDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain�� StationDataManager�� �����ϴ� �Լ�
@author  doori.shin
@date    2015.03.08
@param  StationDataManager : ������ ������
*/

void TOHTMainForm::SetStationDataManager(StationDataManager* stationDataManager)
{
	this->m_StationDataManager = stationDataManager;
}

//---------------------------------------------------------------------------

/**
@brief   OHTMain�� TeachingDataManager�� �����ϴ� �Լ�
@author  doori.shin
@date    2015.03.08
@param  TeachingDataManager : ������ ������
*/

void TOHTMainForm::SetTeachingDataManager(TeachingDataManager* teachingDataManager)
{
	this->m_TeachingDataManager = teachingDataManager;
}


//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ CmdManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  CmdManager ������, ������ ���� ���� ��� Null�� ����
*/
CmdManager* TOHTMainForm::GetCmdManager()
{
	if(this->m_CmdManager == NULL)
		return NULL;
	else
		return this->m_CmdManager;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ PassPermit�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  CmdManager ������, ������ ���� ���� ��� Null�� ����
*/
PassPermit* TOHTMainForm::GetPassPermit()
{
	if(this->m_PassPermit == NULL)
		return NULL;
	else
		return this->m_PassPermit;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ CmdManager�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  CmdManager ������, ������ ���� ���� ��� Null�� ����
*/
PathSearch* TOHTMainForm::GetPathSearch()
{
	if(this->m_PathSearch == NULL)
		return NULL;
	else
		return this->m_PathSearch;
}
//---------------------------------------------------------------------------
/**
@brief   OHTMain���� ������ HWCommon�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  HWCommon ������, ������ ���� ���� ��� Null�� ����
*/
HWCommon* TOHTMainForm::GetHWCommon()
{
    if(this->m_HWCommon == NULL)
        return NULL;
    else
        return this->m_HWCommon;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ HWSet�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  HWSet ������, ������ ���� ���� ��� Null�� ����
*/
HWSet* TOHTMainForm::GetHWSet()
{
    if(this->m_HWSet == NULL)
        return NULL;
    else
        return this->m_HWSet;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ DrivingControl�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  DrivingControl ������, ������ ���� ���� ��� Null�� ����
*/
DrivingControl* TOHTMainForm::GetDrivingControl()
{
    if(this->m_DrivingControl == NULL)
        return NULL;
    else
        return this->m_DrivingControl;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ TransControl�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  TransControl ������, ������ ���� ���� ��� Null�� ����
*/
TransControl* TOHTMainForm::GetTransControl()
{
    if(this->m_TransControl == NULL)
        return NULL;
    else
        return this->m_TransControl;
}
//---------------------------------------------------------------------------

/**
@brief   OHTMain���� ������ ManualControl�� �����ϴ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  ManualControl ������, ������ ���� ���� ��� Null�� ����
*/
ManualControl* TOHTMainForm::GetManualControl()
{
    if(this->m_ManualControl == NULL)
        return NULL;
    else
        return this->m_ManualControl;
}
//---------------------------------------------------------------------------

/**
@brief   Main ���α׷��� ���� ������ �������ִ� �Լ�
@author  zzang9un
@date    2014.02.06
@return  uVERSION_DATA ����ü
*/
uVERSION_DATA TOHTMainForm::GetOHTProgramVersion()
{
	uVERSION_DATA rtnVersion;

    rtnVersion.Info.Major = this->VerInfo.Major;
	rtnVersion.Info.Minor = this->VerInfo.Minor;
    rtnVersion.Info.Release = this->VerInfo.Release;
    rtnVersion.Info.Build = this->VerInfo.Build;

    return rtnVersion;
}
//---------------------------------------------------------------------------
void __fastcall TOHTMainForm::tmMinimizerTimer(TObject *Sender)
{
	Application->Minimize();
	tmMinimizer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TOHTMainForm::OnRestore(TObject *Sender)
{
	tmMinimizer->Enabled = true;
}


void TOHTMainForm::Set_LogFolder(bool bflag)
{
	if(gm_OHTMainStatus!=NULL)
	{
		gm_OHTMainStatus->StatusTP.isLogDrive = bflag;
	}			
}
//---------------------------------------------------------------------------
/**
@brief	Thread ���¸� Ȯ���ϴ� Ÿ�̸�
@author	doori.shin
@date	2014.04.15
*/
void __fastcall TOHTMainForm::tmChkThreadStatusTimer(TObject *Sender)
{
	AnsiString strLog = "";
	BYTE retValue = 0x00;

	//�켱���� ���� Thread���� Check
	//Alive(A), Dead(D), Loop(L)
	//1. HWControlThread(H)
	retValue = CheckThreadStatus(m_HWControlThread, m_nPreHWCtrlThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog = "H:" + GetThreadStatusToString(retValue) + " ";
	}
	//2. ExecuteThread(E)
	retValue = CheckThreadStatus(m_ExecuteThread, m_nPreExecuteThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "E:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",E:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_ExecuteThread, m_nPreExecuteThTick));
	//3. TaskControlThread(T)
	retValue = CheckThreadStatus(m_TaskControlThread, m_nPreTaskCtrlThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "T:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",T:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_TaskControlThread, m_nPreTaskCtrlThTick));
	//4. ExtraJobThread(J)
	retValue = CheckThreadStatus(m_ExtraJobThread, m_nPreExtraJobThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "J:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",J:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_ExtraJobThread, m_nPreExtraJobThTick));
	//5. StatusCheckThread(S)
	retValue = CheckThreadStatus(m_StatusCheckThread, m_nPreStatusChkThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "S:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",S:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_StatusCheckThread, m_nPreStatusChkThTick));
	//6. LogThread(L)
	retValue = CheckThreadStatus(m_LogThread, m_nPreLogThTick);
	if(retValue != THREAD_STATUS_ALIVE)
	{
		strLog += "L:" + GetThreadStatusToString(retValue) + " ";
	}
//	strLog += ",L:";
//	strLog += GetThreadStatusToString(CheckThreadStatus(m_LogThread, m_nPreLogThTick));

	//�α� ���
	if(strLog.Length() > 0)
		THREAD_STATUS_LOG(strLog.c_str());
}
//---------------------------------------------------------------------------

/**
@brief	Thread ���࿩�� Ȯ��
@author	doori.shin
@date	2014.04.15
@param	thread : Ȯ���Ϸ��� Thread
@param	tick : Ȯ���Ϸ��� Thread�� ���� tick��
@return	Thread ���°�
		- THREAD_ALIVE : ������
		- THREAD_STOP :Ŭ������ ��������� Thread�� �����(�ٽ� ���డ��)
		- THREAD_DESTROY : Ŭ������ �����(NULL�� �Ǵ� ���ܻ�Ȳ, �ٽ� ����Ұ���)
		- THREAD_UNKNOWN : �ʱ���°�
*/
inline BYTE TOHTMainForm::CheckThreadStatus(ThreadCtrl* thread, int& tick)
{
	BYTE result = THREAD_STATUS_UNKNOWN;

	try
	{
		if(tick == thread->getTick())//thread is stopped
		{
			result = THREAD_STATUS_STOP;
			//Thread�� �ٽ� ������ �� �ִ�.
			//thread->beginThread();
		}
		else //thread is running
			result = THREAD_STATUS_ALIVE;

		tick = thread->getTick();
	}
	catch(Exception& e)
	{
		//thread is dead
		result = THREAD_STATUS_DESTROY;
	}

	return result;
}
//---------------------------------------------------------------------------
/**
@brief	Thread ���°��� String���� ����
@author	doori.shin
@date	2014.04.15
@param	status : Thread���°�
@return	String���� ��ȯ�� ���°�
*/
inline String TOHTMainForm::GetThreadStatusToString(BYTE status)
{
	String result = L"";

	switch(status)
	{
	case THREAD_STATUS_ALIVE:
		result = L"A";
		break;
	case THREAD_STATUS_STOP:
		result = L"S";
		break;
	case THREAD_STATUS_DESTROY:
		result = L"D";
		break;
	case THREAD_STATUS_UNKNOWN:
		result = L"U";
		break;
	}

	return result;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	static bool bCritical = true;

	if(bCritical == false)
		return;

	bCritical = false; 	// �Ӱ迵�� ����
	/// ����!! bCiritical �� false�� �ٲ��� ������ �Լ� �߰����� return �ϴ� ���� �������ؾ���

	if(m_bErrIotGateway)
	{
		CanClose = true;
		RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  IOT Gateway Error =======");
        bCritical = true;
		return;
	}else if(m_bErrOHTTypeMatching)
	{
		CanClose = true;
		RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  OHT Type Error =======");
        bCritical = true;
		return;
    }

	RapidLog(MAIN_EXIT_LOG, "FormCloseQuery Enter");

	if(gm_OHTMainStatus->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL)
	{
		String strTemp = "";
		strTemp.sprintf(L"���� %c%c%c%c%c%c ���� �Ͻðڽ��ϱ�?", m_pParamSet->VHLName[0],m_pParamSet->VHLName[1],m_pParamSet->VHLName[2],m_pParamSet->VHLName[3],m_pParamSet->VHLName[4],m_pParamSet->VHLName[5]);

		RapidLog(MAIN_EXIT_LOG, "Try Exit - manual mode");

		//if(ID_YES != MessageBox(this->Handle, strTemp.w_str(), L"���",  MB_YESNO))
		if(MessageDlg(strTemp.c_str(), mtConfirmation, TMsgDlgButtons() << mbOK << mbCancel, 0) != mrOk)
		{
			RapidLog(MAIN_EXIT_LOG, "Close Cancled");
			CanClose = false;
		}
		else
		{
            RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  MANUAL MODE =======");
			CanClose = true;


		}
	}
	else
	{
		//MessageBox(NULL,L"Manual Mode�� ��ȯ �� �������ּ���.",L"���",NULL);
		RapidLog(MAIN_EXIT_LOG, "FormCloseQuery_Interlock");
		ADD_MEMO_T("Manual Mode�� ��ȯ �� �������ּ���.");
		CanClose = false;
	}

	bCritical = true;   // �Ӱ迵�� ����
}
//---------------------------------------------------------------------------
//ȥ�� PIO//
void TOHTMainForm::Set_EQPIO_Type(int nEqPioType)
{
	m_nEQPIO_Type = nEqPioType;
	return;
}

int TOHTMainForm::Get_EQPIO_Type()
{
	return m_nEQPIO_Type;
}


//---------------------------------------------------------------------------
bool TOHTMainForm::LoadParameter(char* szFilePath, DEFAULT_PARAM_SET* pParamSet)
{
	bool bSuccess = false;
	int nTmpValue = 0;
    double dTmpValue = 0.0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[30]= {0,};

    LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	//���� �ʱ�ȭ

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
        pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadString("OHTInfo","MachineName", "V00001", strTempValue);
		memmove(pParamSet->VHLName, strTempValue, sizeof(pParamSet->VHLName)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadString("OHTInfo","BroadCastMask", "192.167.255.255", strTempValue);
		memmove(pParamSet->BroadCasMask, strTempValue, sizeof(pParamSet->BroadCasMask)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadInteger("OHTInfo", "PortNumAdd", 0, &nTmpValue);
		pParamSet->nPortNumAdd = nTmpValue;

		pIniFile->ReadString("OHTInfo","OHTType", "00000", strTempValue);
		memmove(pParamSet->OHTType, strTempValue, sizeof(pParamSet->OHTType)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadString("OHTInfo","LineType", "00000", strTempValue);
		memmove(pParamSet->LineType, strTempValue, sizeof(pParamSet->LineType)-1); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		pIniFile->ReadInteger("OHTInfo", "IsLCDAndLED", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L ���� �Ķ���� #JRespect 2018.01.09
		pParamSet->IsLCDAndLED = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("OHTInfo", "HoistHomeSensorOriginCountAdd", 0, 0, 4, &nTmpValue); //add P1L VS P1_2L & P1_3L ���� �Ķ���� #JRespect 2018.01.09
		pParamSet->nHoistHomeSensorOriginConutAdd = nTmpValue;

		pIniFile->ReadInteger("OHTInfo", "bHoistHomeSensorOriginRetry", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L ���� �Ķ���� #JRespect 2018.01.09
		pParamSet->bHoistHomeSensorOriginRetry = nTmpValue;


		pIniFile->ReadInteger("OHTInfo", "IsIOTHub", 0, &nTmpValue); //add P1L VS P1_2L & P1_3L ���� �Ķ���� #JRespect 2018.01.09
		pParamSet->IsIOTHub = nTmpValue;

		pIniFile->ReadInteger("OHTInfo", "VHLLifterPassEnable", 0, &nTmpValue); //AOHC-217 VHLLifter �ӵ� ����
		pParamSet->VHLLifterPassEnable = nTmpValue;

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
		pIniFile->ReadInteger("OHTInfo", "IsRotateSensor", 0, &nTmpValue); //0�� ���� ���Ͷ� �̻��, 1�� �������Ͷ� ���
		pParamSet->IsRotateSensor = nTmpValue;
#endif
		//2017.01.04 puting ����ƼĪ ���� IP ȹ��
		pIniFile->ReadString("AUTOTEACHING","IpAddress", "192.168.200.165", strTempValue);
		memmove(&(pParamSet->AutoTeachingIP), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)||(OHT_NAME == OHT_NAME_STD_V85))
		pIniFile->ReadInteger("PRETRANS", "PreEQPIO", 0, &nTmpValue);
		pParamSet->PreEQPIO = nTmpValue;
        pParamSet->PreShutter =0;
#else
		pIniFile->ReadInteger("PRETRANS", "PreEQPIO", 0, &nTmpValue);
		pParamSet->PreEQPIO = nTmpValue;

		pIniFile->ReadInteger("PRETRANS", "PreShutter", 0, &nTmpValue);
		pParamSet->PreShutter = nTmpValue;
#endif
        //2016.12.20 sh17.jo ���� ��Ʈ��ũ ���� ���� ���� �Ķ���� �߰�
 		pIniFile->ReadString("WIRELESS","GatewayIP", "192.168.0.1", strTempValue);
		memmove(&(pParamSet->GatewayIP), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		//2018.08.21 shcho : OCS�� gateway �ּ�. OCS ���� �ҷ��� Ping �õ��ϱ� ����
		memset(pParamSet->GatewayIP_OCS, 0, sizeof(pParamSet->GatewayIP_OCS));
		pIniFile->ReadString("WIRELESS","GatewayIP_OCS", "192.167.0.1", strTempValue);
		memmove(&(pParamSet->GatewayIP_OCS), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));
		if(strlen(pParamSet->GatewayIP_OCS) < 7)	// ����� �� IP ���� �ȵ�� ���� ��� default ����
		{
			sprintf(pParamSet->GatewayIP_OCS, "192.167.0.1");
		}

        pIniFile->ReadString("WIRELESS","IpAddress", "192.168.110.1", strTempValue);
		memmove(&(pParamSet->WirelessClientIP), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadString("WIRELESS","UserID", "admin", strTempValue);
		memmove(&(pParamSet->UserID), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadString("WIRELESS","Password", "pass", strTempValue);
		memmove(&(pParamSet->Password), strTempValue, strlen(strTempValue)); //Return Value
        memset(strTempValue, 0, sizeof(strTempValue));

        pIniFile->ReadInteger("WIRELESS", "MaxOCSRcvTime", 25000, &nTmpValue);
        pParamSet->nMaxOCSRcvTime = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step1", 40000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step1= nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step2", 100000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step2 = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_Step3", 160000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_Step3 = nTmpValue;

        pIniFile->ReadInteger("WIRELESS", "MaxPingCheckTime_ETC", 20000, &nTmpValue);
        pParamSet->nMaxPingCheckTime_ETC = nTmpValue;
		pParamSet->bIgnoreIoTLanCheck = false;
		pIniFile->ReadInteger("WIRELESS", "IOT_Lan_Check_Ignore", 0, &nTmpValue);
		pParamSet->bIgnoreIoTLanCheck = nTmpValue;

		pIniFile->ReadString("WIRELESS","IOT_EternetName", "", strTempValue);
		memmove(&(pParamSet->IotEternetName), strTempValue, strlen(strTempValue));
		memset(strTempValue, 0, sizeof(strTempValue));


		pIniFile->ReadString("DiagAgentInfo","IPAddress", "", strTempValue);
		pParamSet->strDiagIpAddr.sprintf("%s", strTempValue);


		pIniFile->ReadInteger("DiagAgentInfo", "Port", 0.0, &nTmpValue);
		pParamSet->nDiagPort = nTmpValue;



		// 1. driving PARAM
		//pIniFile->ReadDoubleMinCheck("DRIVING","IdealGearRatio", DRIVING_GEAR_RATIO, 0.1, &dTmpValue);
		//pParamSet->DrivingDefault.dIdealGearRatio = dTmpValue; //Return Value
		////���� 5.5:1�� ����.(���� 5.555:1) = 114.7341619

		//pIniFile->ReadDoubleMinCheck("DRIVING","Ideal2RealRatio", 1.0, 0.8, &dTmpValue);
		//pParamSet->DrivingDefault.dIdeal2RealRatio = dTmpValue;

		pParamSet->DrivingDefault.dIdealGearRatio = DRIVING_GEAR_RATIO;
		pParamSet->DrivingDefault.dIdeal2RealRatio = 1.0;

		 // 1. HOIST PARAM
		pIniFile->ReadDouble("HOIST", "dPosition0_pulse", -500.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[0].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition0_User", -1.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[0].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition1_pulse", 20000.0, &dTmpValue);   	//STB
		pParamSet->HoistDefault.PositionValue[1].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition1_User", 106.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[1].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition2_pulse", 130000.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[2].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition2_User", 672.0, &dTmpValue);      	//STK
		pParamSet->HoistDefault.PositionValue[2].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition3_pulse", 350000.0, &dTmpValue);    //Centor
		pParamSet->HoistDefault.PositionValue[3].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition3_User", 1757.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[3].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition4_pulse", 450000.0, &dTmpValue);    //Centor ~ EQ Start ���̰�
		pParamSet->HoistDefault.PositionValue[4].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition4_User", 2185.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[4].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition5_pulse", 620000.0, &dTmpValue);  //EQ Start
		pParamSet->HoistDefault.PositionValue[5].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition5_User", 3012.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[5].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition6_pulse", 800000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[6].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition6_User", 3797.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[6].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition7_pulse", 950000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[7].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition7_User", 4424.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[7].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition8_pulse", 1100000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[8].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition8_User", 5021.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[8].dUserUnit = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition9_pulse", 1230000.0, &dTmpValue);  //EQ End
		pParamSet->HoistDefault.PositionValue[9].dPulse = dTmpValue;

		pIniFile->ReadDouble("HOIST", "dPosition9_User", 5517.0, &dTmpValue);
		pParamSet->HoistDefault.PositionValue[9].dUserUnit = dTmpValue;



		pIniFile->ReadDouble("HOIST", "dHomeOffsetOfRailToJigOnUserUnit", 0.0, &dTmpValue);
		pParamSet->HoistDefault.dDeviceDifference = dTmpValue;

		pIniFile->ReadInteger("HOIST", "dHomeSearchOffset", 0.0, &nTmpValue);
		pParamSet->HoistDefault.nHomeSearchCtsOffset = nTmpValue;



		// 2. SHIFT PARAM
		//pIniFile->ReadDoubleMinCheck("SHIFT", "dIdealGearRatio", SHIFT_IDEALGEARRATIO, 0.1, &dTmpValue);
		//pParamSet->ShiftDefault.dIdealGearRatio = dTmpValue;

		//pIniFile->ReadDoubleMinCheck("SHIFT", "dIdeal2RealRatio", 1.0, 0.8, &dTmpValue);
		//pParamSet->ShiftDefault.dIdeal2RealRatio = dTmpValue;

		//pIniFile->ReadDouble("SHIFT", "dHomeOffsetOfMTLToJigOnUserUnit", 0.0, &dTmpValue);
		//pParamSet->ShiftDefault.dDeviceDifference = dTmpValue;

		//pIniFile->ReadInteger("SHIFT", "dHomeSearchOffset", 0.0, &nTmpValue);
		//pParamSet->ShiftDefault.nHomeSearchCtsOffset = nTmpValue;

		pParamSet->ShiftDefault.dIdealGearRatio = SHIFT_IDEALGEARRATIO;
		pParamSet->ShiftDefault.dIdeal2RealRatio = 1.0;
		pParamSet->ShiftDefault.dDeviceDifference = 0.000000;
		pParamSet->ShiftDefault.nHomeSearchCtsOffset = 0;


		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference = nTmpValue;
		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit_90", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference_90 = nTmpValue;
		pIniFile->ReadIntegerMinMaxCheck("ROTATE","nHomeOffsetOfMTLToJigOnUserUnit_180", 0, -20, 20, &nTmpValue);
		pParamSet->RotateDefault.nDeviceDifference_180 = nTmpValue;

		delete pIniFile;
		bSuccess = true;

	}
	else
	{
        //2016.12.20 sh17.jo ���� ��Ʈ��ũ ���� ���� ���� �Ķ���� �߰�
        strcpy(pParamSet->VHLName, "V00001");
        strcpy(pParamSet->BroadCasMask, "192.167.255.255");
        strcpy(pParamSet->GatewayIP, "192.168.0.1");
		strcpy(pParamSet->WirelessClientIP, "192.168.110.1");
		strcpy(pParamSet->UserID, "admin");
		strcpy(pParamSet->Password, "pass");

		strcpy(pParamSet->OHTType, "00000");
		strcpy(pParamSet->LineType, "00000");
        pParamSet->nMaxOCSRcvTime = 25000;
		pParamSet->nMaxPingCheckTime_Step1 =  40000;
        pParamSet->nMaxPingCheckTime_Step2 = 100000;
        pParamSet->nMaxPingCheckTime_Step3 = 160000;
        pParamSet->nMaxPingCheckTime_ETC   =  20000;

		strcpy(pParamSet->AutoTeachingIP, "192.168.200.165");

		pParamSet->PreEQPIO = 0;
		pParamSet->PreShutter = 0;
		pParamSet->nPortNumAdd = 0;
		pParamSet->IsLCDAndLED = 0;
		pParamSet->nHoistHomeSensorOriginConutAdd = 0;
		pParamSet->bHoistHomeSensorOriginRetry = 0;
		pParamSet->IsIOTHub = 0;
		pParamSet->VHLLifterPassEnable=0;
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))
		pParamSet->IsRotateSensor = 0;
#endif

		pParamSet->DrivingDefault.dIdealGearRatio = DRIVING_GEAR_RATIO; //Return Value
		pParamSet->DrivingDefault.dIdeal2RealRatio = 1.0;


		pParamSet->HoistDefault.PositionValue[0].dPulse = -500.0;
		pParamSet->HoistDefault.PositionValue[0].dUserUnit = -2.0;
		pParamSet->HoistDefault.PositionValue[1].dPulse = 20000.0;
		pParamSet->HoistDefault.PositionValue[1].dUserUnit = 100.0;
		pParamSet->HoistDefault.PositionValue[2].dPulse = 128000.0;
		pParamSet->HoistDefault.PositionValue[2].dUserUnit = 637.0;
		pParamSet->HoistDefault.PositionValue[3].dPulse = 300000.0;
		pParamSet->HoistDefault.PositionValue[3].dUserUnit = 1460.0;
		pParamSet->HoistDefault.PositionValue[4].dPulse = 450000.0;
		pParamSet->HoistDefault.PositionValue[4].dUserUnit = 2153.0;
		pParamSet->HoistDefault.PositionValue[5].dPulse = 620000.0;
		pParamSet->HoistDefault.PositionValue[5].dUserUnit = 2901.0;
		pParamSet->HoistDefault.PositionValue[6].dPulse = 780000.0;
		pParamSet->HoistDefault.PositionValue[6].dUserUnit = 3579.0;
		pParamSet->HoistDefault.PositionValue[7].dPulse = 940000.0;
		pParamSet->HoistDefault.PositionValue[7].dUserUnit = 4225.0;
		pParamSet->HoistDefault.PositionValue[8].dPulse = 1110000.0;
		pParamSet->HoistDefault.PositionValue[8].dUserUnit = 4881.0;
		pParamSet->HoistDefault.PositionValue[9].dPulse = 1290000.0;
		pParamSet->HoistDefault.PositionValue[9].dUserUnit = 5556.0;

		pParamSet->HoistDefault.dDeviceDifference = 0.0;
		pParamSet->HoistDefault.nHomeSearchCtsOffset = 0.0;

		pParamSet->ShiftDefault.dIdealGearRatio = SHIFT_IDEALGEARRATIO;
		pParamSet->ShiftDefault.dIdeal2RealRatio = 1.0;
		pParamSet->ShiftDefault.dDeviceDifference = 0.0;
		pParamSet->ShiftDefault.nHomeSearchCtsOffset = 0.0;

		pParamSet->RotateDefault.nDeviceDifference = 0;
		pParamSet->RotateDefault.nDeviceDifference_90 = 0;
		pParamSet->RotateDefault.nDeviceDifference_180 = 0;



	}

	return bSuccess;
}
void TOHTMainForm::HoistParam_Read(int iIndex, double &dbl_Pluse, double &dbl_User)
{
	dbl_Pluse = m_pParamSet->HoistDefault.PositionValue[iIndex].dPulse;
	dbl_User = m_pParamSet->HoistDefault.PositionValue[iIndex].dUserUnit;

	return;
}
void TOHTMainForm::DeviceDifference_Read(double &dbl_Pluse)
{
	dbl_Pluse = m_pParamSet->HoistDefault.dDeviceDifference;

	return;
}

bool TOHTMainForm::HoistParamSaveParameter(PKT_TP2OHT_HOIST_TEACH_COMMAND PKT)
{
	bool bSuccess = false;
	
	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\OHTPARAM.ini");	

	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
	}

	if(pIniFile != NULL)		
	{
		m_pParamSet->HoistDefault.dDeviceDifference = PKT.BODY.dHomeOffsetOfRailToJigOnUserUnit;
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
		for(int i = 0; i < 10; i++)
		{
			m_pParamSet->HoistDefault.PositionValue[i].dPulse = PKT.BODY.PositionValue[i].dPulse;
			m_pParamSet->HoistDefault.PositionValue[i].dUserUnit = PKT.BODY.PositionValue[i].dUserUnit;
		}
		 // 1. HOIST PARAM
		pIniFile->WriteDouble("HOIST", "dPosition0_pulse", m_pParamSet->HoistDefault.PositionValue[0].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition0_User", m_pParamSet->HoistDefault.PositionValue[0].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition1_pulse", m_pParamSet->HoistDefault.PositionValue[1].dPulse); 	//STB
		pIniFile->WriteDouble("HOIST", "dPosition1_User",  m_pParamSet->HoistDefault.PositionValue[1].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition2_pulse", m_pParamSet->HoistDefault.PositionValue[2].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition2_User", m_pParamSet->HoistDefault.PositionValue[2].dUserUnit);		//STK		

		pIniFile->WriteDouble("HOIST", "dPosition3_pulse",m_pParamSet->HoistDefault.PositionValue[3].dPulse);	//Centor
		pIniFile->WriteDouble("HOIST", "dPosition3_User", m_pParamSet->HoistDefault.PositionValue[3].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition4_pulse", m_pParamSet->HoistDefault.PositionValue[4].dPulse);	//Centor ~ EQ Start ���̰�
		pIniFile->WriteDouble("HOIST", "dPosition4_User", m_pParamSet->HoistDefault.PositionValue[4].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition5_pulse", m_pParamSet->HoistDefault.PositionValue[5].dPulse);  //EQ Start
		pIniFile->WriteDouble("HOIST", "dPosition5_User", m_pParamSet->HoistDefault.PositionValue[5].dUserUnit);		

		pIniFile->WriteDouble("HOIST", "dPosition6_pulse", m_pParamSet->HoistDefault.PositionValue[6].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition6_User",  m_pParamSet->HoistDefault.PositionValue[6].dUserUnit);	 

		pIniFile->WriteDouble("HOIST", "dPosition7_pulse", m_pParamSet->HoistDefault.PositionValue[7].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition7_User",  m_pParamSet->HoistDefault.PositionValue[7].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition8_pulse", m_pParamSet->HoistDefault.PositionValue[8].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition8_User",  m_pParamSet->HoistDefault.PositionValue[8].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition9_pulse", m_pParamSet->HoistDefault.PositionValue[9].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition9_User",  m_pParamSet->HoistDefault.PositionValue[9].dUserUnit);

#else
		for(int i = 0; i < 7; i++)
		{
			m_pParamSet->HoistDefault.PositionValue[i].dPulse = PKT.BODY.PositionValue[i].dPulse;
			m_pParamSet->HoistDefault.PositionValue[i].dUserUnit = PKT.BODY.PositionValue[i].dUserUnit;
		}
		 // 1. HOIST PARAM
		pIniFile->WriteDouble("HOIST", "dPosition0_pulse", m_pParamSet->HoistDefault.PositionValue[0].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition0_User", m_pParamSet->HoistDefault.PositionValue[0].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition1_pulse", m_pParamSet->HoistDefault.PositionValue[1].dPulse); 	//STB
		pIniFile->WriteDouble("HOIST", "dPosition1_User",  m_pParamSet->HoistDefault.PositionValue[1].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition2_pulse", m_pParamSet->HoistDefault.PositionValue[2].dPulse);
		pIniFile->WriteDouble("HOIST", "dPosition2_User", m_pParamSet->HoistDefault.PositionValue[2].dUserUnit);		//STK

		pIniFile->WriteDouble("HOIST", "dPosition3_pulse",m_pParamSet->HoistDefault.PositionValue[3].dPulse);	//Centor
		pIniFile->WriteDouble("HOIST", "dPosition3_User", m_pParamSet->HoistDefault.PositionValue[3].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition4_pulse", m_pParamSet->HoistDefault.PositionValue[4].dPulse);	//Centor ~ EQ Start ���̰�
		pIniFile->WriteDouble("HOIST", "dPosition4_User", m_pParamSet->HoistDefault.PositionValue[4].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition5_pulse", m_pParamSet->HoistDefault.PositionValue[5].dPulse);  //EQ Start
		pIniFile->WriteDouble("HOIST", "dPosition5_User", m_pParamSet->HoistDefault.PositionValue[5].dUserUnit);

		pIniFile->WriteDouble("HOIST", "dPosition6_pulse", m_pParamSet->HoistDefault.PositionValue[6].dPulse);  //EQ End
		pIniFile->WriteDouble("HOIST", "dPosition6_User",  m_pParamSet->HoistDefault.PositionValue[6].dUserUnit);
#endif

		pIniFile->WriteDouble("HOIST", "dHomeOffsetOfRailToJigOnUserUnit",m_pParamSet->HoistDefault.dDeviceDifference);

		delete pIniFile;

		m_HWSet->m_pHoistAxis->SetHoistParam(m_pParamSet->HoistDefault);

		bSuccess = true;
	}
	
	return bSuccess;
}

void TOHTMainForm::ForceClose(void)
{
	RapidLog(MAIN_EXIT_LOG, "======== OHT MAIN PROGRAM TERMINATING  -  FORCE CLOSE =======");

	tmUIupdate->Enabled = false;

	DeleteCriticalSection(&m_CS);

	DestroyClass();

	Sleep(100);

	exit(0);
}




StatusCheckThread* TOHTMainForm::GetStatusCheckThread()
{
	if(this->m_StatusCheckThread == NULL)
        return NULL;
    else
		return this->m_StatusCheckThread;
}

// "DataFiles\\OHTDetectAreaInfo.ini" ���� UBG �������� ���� ��  ���ذ��� �� �� �ε�
bool TOHTMainForm::LoadPatternData(char* szFilePath)
{
	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	m_bUBGINILoaded = false;
	ZeroMemory(m_szUBGInfo_INI, sizeof(m_szUBGInfo_INI));

    char szLog[100];	ZeroMemory(szLog, sizeof(szLog));
	char szSection[20];
	ZeroMemory(szSection, sizeof(szSection));

	char szKey[20];
	ZeroMemory(szKey, sizeof(szKey));

	if(!pIniFile->GetIniFileExist())
	{
		RapidLog(RLOG_UBG_CHK, "INI File is not exist.");
		return false;
    }

	if(pIniFile == NULL)
	{
		RapidLog(RLOG_UBG_CHK, "INI File Load Fail");
		return false;
	}

	char szPattern[256];
	ZeroMemory(szPattern, sizeof(szPattern));

	pIniFile->ReadString("Setting", "CheckPattern", "", szPattern);
	std::string strPattern = szPattern;
	StrReplace(&strPattern, " ", ""); 	// ���� ����

	// �� �������� ������ �ʿ䰡 ���� 1,2,3,12,13 �� ������
	m_nArrCheckPattern = ExtractNumber_inString(strPattern, ',');  // int �迭�� ���

	bool bOldVersion = false;
	int nSize = m_nArrCheckPattern.size();
	if(nSize == 0)
	{
		bOldVersion = true;
		for(int i=0; i<7 ;i++) 	// ���������� 0~6�� �⺻���� ��
            m_nArrCheckPattern.push_back(i+1);
		nSize = m_nArrCheckPattern.size();
		RapidLog(RLOG_UBG_CHK, "Old INI Version. Default 0~6 Pattern setting.");
		//delete pIniFile;
		//return false;

    }
	for(int i=0 ; i<nSize ; i++)
	{
		int nPtnIdx = m_nArrCheckPattern[i];
		if(nPtnIdx > COUNTOF_PATTERN+1 || nPtnIdx < 1)
		{
			sprintf(szLog, "INI Load Failed. Wrong Pattern No : %d", nPtnIdx);
			RapidLog(RLOG_UBG_CHK, szLog);
			delete pIniFile;
			return false;

		}
		m_nArrCheckPattern[i] -= 1;	// INI���� +1 �� ���̱� ������ �迭 �ε����� �ٲٱ� ���ؼ� -1 �Ѵ�
    }

	// INI���� �о m_szUBGInfo_INI[][] ������ ����
	for(int a=0; a<nSize ; a++)
	{
		int nPtnIdx = m_nArrCheckPattern[a];

		sprintf(szSection, "Pattern%d", bOldVersion ? nPtnIdx : nPtnIdx+1);
		for(int b=0; b<COUNTOF_PATTERN_ASCII ; b++)
		{
			sprintf(szKey, "VAL%d", b);
			int nTmpVal = INT_MAX;
			pIniFile->ReadInteger(szSection, szKey, nTmpVal, &nTmpVal);

			// INI �ε� ���� ���. �Ѱ��� ���� ���ϸ� ����
			if(nTmpVal == INT_MAX)
			{
				sprintf(szLog, "INI Load Failed in [%s], [%s]", szSection, szKey);
				RapidLog(RLOG_UBG_CHK, szLog);
				delete pIniFile;
				return false;

            }
			m_szUBGInfo_INI[nPtnIdx][b] = (char)nTmpVal;
		}
	}

	m_bUBGINILoaded = true;
	delete pIniFile;

	return true;
}

//////////////////////////////////////////
// AddUBGPattern(...) �Լ�
// UBG�κ��� ���� ������ �ɹ������� �����Ѵ�
// �� �ѹ��� 4���� ���Ͽ� ������ ������ ������ �����͸� �߶� m_szUBGInfo_Comm[][] �迭�� �����Ѵ�
//
void TOHTMainForm::AddUBGPattern(int nCmdFRIdx1234, char *szRecv)
{
	// szRecv �� ����� 201 �̶�� ���� ����Ǿ��ٴ� �����Ͽ� �Լ� ����
	// szRecv ������ ����ó���� ȣ���ϴ� �κп��� ��

	if(nCmdFRIdx1234 < 1 || nCmdFRIdx1234 > 4)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Add Pattern Index Range Over.");
		return;
    }

	char tmpBuf[512];
	int nSize = COUNTOF_PATTERN_ASCII*4;

	memcpy(tmpBuf, szRecv+8, nSize);  // +8 ������ ���� start index �ǳʶ�
	tmpBuf[nSize] = '\0';		//�ι��� �߰������ �α� ���ڰ� ���Ƽ�

	// ���� ������ �α�
	//
	char szLogMsg[1024];
	ZeroMemory(szLogMsg, sizeof(szLogMsg));

	sprintf(szLogMsg,"UBG RecvData [FR1%d]: %s", nCmdFRIdx1234, tmpBuf);
	RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging


	// data �߶� m_szUBGInfo_Comm[][] ���� ����
	//
	int nStartIdx = (nCmdFRIdx1234-1)*4;	// ���� 1���� �����ϱ� -1 ����

	int nCnt = 0;
	// �Ѱ� ��ɴ� 4���� ������ ����ֱⶫ�� 4�� ���� �ݺ���
	for(int i=nStartIdx; i<nStartIdx+4 ; i++)
	{
		memcpy(m_szUBGInfo_Comm[i], tmpBuf + nCnt*COUNTOF_PATTERN_ASCII, COUNTOF_PATTERN_ASCII);

		if(false)	// Only debugging
		{
			ZeroMemory(szLogMsg, sizeof(szLogMsg));

			sprintf(szLogMsg,"UBG memcpy m_szUBGInfo_Comm[%d]: %s", i, m_szUBGInfo_Comm[i]);
			RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

		}
		nCnt++;
	}

}

bool TOHTMainForm::DoCompUBGPattern(int nPatternNo0to15)
{
	if(nPatternNo0to15 < 0 || nPatternNo0to15 > COUNTOF_PATTERN)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Comp Pattern Index Range Over.");
		return false;
	}
	if(m_bUBGINILoaded == false)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Comp Failed. INI is not loaded.");
		return false;
    }

	char strHex_Comm[1024], strHex_INI[1024];
	char szComm[1024], szINI[1024];

	ZeroMemory(strHex_Comm, sizeof(strHex_Comm));
	ZeroMemory(strHex_INI, sizeof(strHex_INI));
	ZeroMemory(szComm, sizeof(szComm));
	ZeroMemory(szINI, sizeof(szINI));



	int nIdx = nPatternNo0to15;

	// �ݵ�� �ӽ� ������ �����ϰ� ���ؾ���
	memcpy(szComm, m_szUBGInfo_Comm[nIdx], COUNTOF_PATTERN_ASCII);
	memcpy(szINI, m_szUBGInfo_INI[nIdx], COUNTOF_PATTERN_ASCII);

	StringToHexStream(szComm, strHex_Comm, sizeof(strHex_Comm), ',', false); 	//
	StringToHexStream(szINI, strHex_INI, sizeof(strHex_INI), ',', false);


	if(false)	// only debugging
	{
		char szLogMsg[2048];
		ZeroMemory(szLogMsg, sizeof(szLogMsg));

		sprintf(szLogMsg,"### CompPattern [%d] szComm: %s", nIdx,szComm);

		RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

		sprintf(szLogMsg,"### CompPattern [%d] szINI: %s", nIdx,szINI);
		RapidLog(RLOG_UBG_CHK, szLogMsg);   // logging

	}


	bool bSame = (strcmp(strHex_Comm, strHex_INI) == 0);

	if(bSame == false)
	{
		char szTmpLog[4096];
		sprintf(szTmpLog, "[CompFail_%d]\nRCV: %s\nINI: %s", nPatternNo0to15+1, strHex_Comm, strHex_INI);
		RapidLog(RLOG_UBG_CHK, szTmpLog);
	}

	return bSame;
}

// OHTDetectAreaInfo_UBG_RECV.ini ���Ͽ�
// UBG Comm ���� ���� �����͸� ������ �����Ѵ�
bool TOHTMainForm::WriteUBGRecv_INI()
{
	char filePath[200];	ZeroMemory(filePath, sizeof(filePath));
	sprintf(filePath, "C:\\User\\OHT\\DataFiles\\OHTDetectAreaInfo_UBG_RECV.ini");

	DeleteFileA(filePath);

	FILE *fp = fopen(filePath, "a");    // write+append

	if(fp == NULL)
	{
		RapidLog(RLOG_UBG_CHK, "[ERROR] Can not save [OHTDetectAreaInfo_UBG_RECV.ini]");
		return false;
	}

	fprintf(fp, "[Setting]");
	fprintf(fp, "\n");
	fprintf(fp, "CheckPattern=");


	// INI �� �ε� �Ǿ��ٸ� ���ϰ��� �ϴ� ���Ϲ�ȣ�� ������ �ִ�
	if(m_bUBGINILoaded)
	{
		//[Setting]  CheckPattern=1,2,3,4,5,6,12,13  <= �̷� ���� ���
		int nSize = m_nArrCheckPattern.size();
		for(int r=0 ; r<nSize ; r++)
		{	fprintf(fp, "%d", m_nArrCheckPattern[r]+1);
			if(r != nSize-1)
				fprintf(fp, ",");
		}

		fprintf(fp, "\n");

		for(int i=0; i<nSize ; i++)
		{
			int nPtnIdx = m_nArrCheckPattern[i];
			fprintf(fp, "[Pattern%d]", nPtnIdx+1);
			fprintf(fp, "\n");

			for(int j=0; j<COUNTOF_PATTERN_ASCII ; j++)
			{
				fprintf(fp, "VAL%d=%d", j, m_szUBGInfo_Comm[nPtnIdx][j]);
				fprintf(fp, "\n");
			}
		}
	}
	else	// INI �ε� �ȵǾ��ٸ� ��� ���� INI save
	{
		for(int r=0 ; r<COUNTOF_PATTERN ; r++)
		{	fprintf(fp, "%d", r+1);        	//[Setting]  CheckPattern=1,2,3,4,5,6,12,13  <= �̷� ���� ���
			if(r != COUNTOF_PATTERN-1)
				fprintf(fp, ",");
		}

		fprintf(fp, "\n");

	   for(int i=0; i<COUNTOF_PATTERN ; i++)
	   {
		   fprintf(fp, "[Pattern%d]", i+1);
		   fprintf(fp, "\n");

		   for(int j=0; j<COUNTOF_PATTERN_ASCII ; j++)
		   {
				fprintf(fp, "VAL%d=%d", j, m_szUBGInfo_Comm[i][j]);
				fprintf(fp, "\n");
		   }
	   }
	}

	fclose(fp);
	RapidLog(RLOG_UBG_CHK, "[SUCCESS] Writting OK. OHTDetectAreaInfo_UBG_RECV.ini");

	return true;
}


// IP ������ �α׷� �����. ���α׷� ����� �ѹ� ����
void TOHTMainForm::WriteLog_NetworkInfo()
{


	FILE *fp = NULL;
	try
	{
		SYSTEMTIME CurTime;
		GetLocalTime(&CurTime);

		char szFilePath[512];
		ZeroMemory(szFilePath, sizeof(szFilePath));

		sprintf(szFilePath, "D:\\Log\\NetworkInfo%d%02d%02d%02d.log", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);

		fp = fopen(szFilePath, "a");    // write+append

		if(fp == NULL)
			return;


		fprintf(fp, "\n\n[%02d:%02d:%02d.%03d] IPCONFIG >>> ", CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds);
		fclose(fp);
		fp = NULL;

		char cmd1[256];
		ZeroMemory(cmd1, sizeof(cmd1));

		sprintf(cmd1, "ipconfig >> %s", szFilePath);
		system(cmd1); // ���� Ŀ�ǵ� IPCONFIG ����

	}
	catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}
}

void __fastcall TOHTMainForm::btnReadMonitorRegClick(TObject *Sender)
{
	short axis;
	short reg_add;
	long result;
	int cbbIndex = cbbMonitorRegister->ItemIndex;
	AnsiString strTemp;
	axis = cbbAxisNum4Monitor->ItemIndex;

	if(axis < 0 || axis > 3) return;
	else axis+=1;

	if(cbbIndex >= 0 && cbbIndex < 10) reg_add = ERROR_TRACE_DATE0 + cbbIndex;
	else if(cbbIndex == 10) reg_add = CURRUNT_ALARM_STATE;
	else if(cbbIndex == 11) reg_add = SERVO_STATUS_MONITOR;
	else return;



	m_pServoControl->ReadMonitorData(axis, reg_add, &result);
	m_pServoControl->DisplayServoMonitorResult(reg_add, result);
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::chkAxisMonitorClick(TObject *Sender)
{
	if(chkAxisMonitor->Checked) btnReadMonitorReg->Enabled = true;
	else btnReadMonitorReg->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::servoClick(TObject *Sender)
{
	if((gm_ExecuteInfo->ExecuteMode != EXECUTE_MODE_AUTO)
	&& (gm_ExecuteInfo->ExecutePositionInfo.Marked ==true ))
	{
		//�������� Ȯ�� GetNodeTypeSlop

		if(!m_ControlTask->GetNodeTypeSlop())
		{
			m_HWSet->m_pServoMonitoring->SetFn009(1);
			m_HWSet->m_pServoMonitoring->SetFn009(2);
		}
		else
		{
			memoMain->Lines->Add("Slope�� �ƴ� ������ Mark���ּ���");
		}
	}
	else
	{
		memoMain->Lines->Add("Slope�� �ƴ� ������ Mark���ּ���");
	}
}

// SHCHO 2018.10.17
// IoT Ethernet ī�� ��Ʈ���� ���� �ƴ��� Ȯ��
bool TOHTMainForm::IoTGatewayCheck()
{
	if(m_pParamSet->bIgnoreIoTLanCheck)
		return true;

	sEthernetInfo info[5];

	// �̴��� ���� ������
	int nCnt = GetEternetInfo(info, 5);
	if(nCnt != 2)
		return true;   // �̴��� ī�� �ΰ��� �ƴϸ� �Ϲ� 8.0 VHL�� �ƴ϶�� ����

	// OHTParam.ini�� IOT ��ī�� �̸��� ������� ��, �ش� ī�� �̸��� �˻��Ѵ�.
	// ������� �ʾҴٸ� ù��° ��ī�尡 IOT��� �����Ѵ�
	if(strlen(m_pParamSet->IotEternetName) > 0)
	{
		for(int i=0; i < nCnt ; i++)
		{
			bool bSame = strcmp(m_pParamSet->IotEternetName, info[i].Name) == 0;
			if(bSame)
			{
				// gateway�� ���� ������ return false
				if(strlen(info[i].Gateway) > 0)
					return false;
				else
					return true;

			}
		}

		// ��ī�� �̸��� OHTParam.ini�� ��� �������� ipconfig���� ã�� �� ���ٸ�
		// return false �Ѵ�.
		return false;
	}


	// ��õ� ��ī�� �̸��� ���� �� ù��°�� IOT ��ī���� �����Ѵ�.
	// gateway�� ���� ������ return false
	if(strlen(info[0].Gateway) > 0)
		return false;

	return true;
}

//---------------------------------------------------------------------------
// ��� ��ȣ�� �˶� ��� ��ü �����͸� ��� ������ �������
NODE_INFO_STD_MAP* TOHTMainForm::GetNodeObj(int nNodeID)
{
	STD_MAP *pData = GetMap_STD();

	if(pData == NULL)
		return NULL;

	try
	{
		for(int i=0; i<pData->m_NodeNum ; i++)
		{
			if(pData->m_NodeArray[i].NodeID == nNodeID)
			{
				NODE_INFO_STD_MAP *pNode = &pData->m_NodeArray[i];
				return pNode;
			}
		}
	}

	catch(...){return NULL;}

	return NULL;
}
//AOHC-204 Thread �� ���� �ð� ���� ���
void TOHTMainForm::ThreadCycleLog(char *msg)
{
	CS_ThreadCycleLog->Acquire();

	WRITE_LOG_ON_THIS_FILE("D:\\log\\ThreadCycle.log", msg);

	CS_ThreadCycleLog->Release();
}

//---------------------------------------------------------------------------
//AOHC-204 Thread �� ���� �ð� ���� ���
ThreadTimeMonitor::ThreadTimeMonitor()
{
	memset(this, 0, sizeof(*this));
}


void ThreadTimeMonitor::Execute(char *name, DWORD time_ms)
{
	DWORD dwCur = timeGetTime();
	static bool	bTimeMonitor_MCC = false;
	if(nIdx == 0)
	{
		dwStart = dwCur;
		min = max = time_ms;
	}

	sum += time_ms;

	if(min > time_ms)
		min = time_ms;
	if(max < time_ms)
		max = time_ms;

	nIdx++;

	DWORD dwSub = dwCur - dwStart;

	// �����ð��� �ѹ��� �α��Ѵ�
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwStart) > 3000)
	{
		char szLog[128];
		memset(szLog, 0, sizeof(szLog));
		int avr = -1;
		if(nIdx > 0)
			avr = (int)((double)sum/(double)nIdx);

		// min, max, avr, cnt, cycle elapse
		sprintf(szLog, "/%s/%lu/%lu/%d", name, min, max, avr);
		OHTMainForm->ThreadCycleLog(szLog);
		nIdx = 0;
		sum = 0;

		if(bTimeMonitor_MCC == false)
			bTimeMonitor_MCC = true;
	}
	if(bTimeMonitor_MCC)
	{
		OHTMainForm->GetStatusCheckThread()->SaveThreadTime(max, name);
	}
}



void __fastcall TOHTMainForm::btn1Click(TObject *Sender)
{
	if(gm_ExecuteInfo->ExecuteMode != EXECUTE_MODE_AUTO)
	{
		gm_OHTMainStatus->bTraceStart = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit_frontClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_Front();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limitClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_800();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit_rearClick(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_Rear();
}
//---------------------------------------------------------------------------

void __fastcall TOHTMainForm::btn_limit1Click(TObject *Sender)
{
	m_pServoControl->SetInnerLimitTorque_90();
}
//---------------------------------------------------------------------------

bool TOHTMainForm::LoadHighSpeedParameter(char* szFilePath)
{
    LIniFile iniFile(szFilePath);

    if(!iniFile.GetIniFileExist())
    {
    	return false;
    }

    int nTmpValue = 0;
	iniFile.ReadInteger("TorqueLimit", "Front", 800, &nTmpValue);
	OHTMainForm->Edit_front_torque->Text = nTmpValue;
	iniFile.ReadInteger("TorqueLimit", "Rear", 800, &nTmpValue);
	OHTMainForm->Edit_rear_torque->Text = nTmpValue;

	iniFile.ReadInteger("TorqueLimit", "FrontName", 1, &nTmpValue);
	OHTMainForm->Edit_frontName->Text = nTmpValue;
	iniFile.ReadInteger("TorqueLimit", "RearName", 2, &nTmpValue);
	OHTMainForm->Edit_rearName->Text = nTmpValue;

	return true;
}


void __fastcall TOHTMainForm::tmAvsControlTimer(TObject *Sender)
{
//	m_AvsControl->ReceiveVibDataTimer();
}
//---------------------------------------------------------------------------
//AvsControl* TOHTMainForm::GetAvsControl()
//{
//    if(this->m_AvsControl == NULL)
//        return NULL;
//    else
//        return this->m_AvsControl;
//}

