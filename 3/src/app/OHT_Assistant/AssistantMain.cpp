// ----------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <Filectrl.hpp>
#include <Dbt.h>
#include <tchar.h>
#pragma hdrstop
// ---------------------------------------------------------------------------
#include "AssistantMain.h"
#include "About.h"
#include "Sysinfo.h"
#include "Def_DataFile.h"
#include "FileControl.h"
#include "FBWFManagerUnit.h"
#include "FileThreadUnit.h"
#include "FMcontrolThreadUnit.h"
#include "ProcessTerminator.h"
#include "Utility.h"
// #include "UDP.h"

//#include "Trace.h"

// ---------------------------------------------------------------------------
#pragma resource "*.dfm"
TAssistantMainForm *AssistantMainForm;
//#define ADD_LOG(STR)        m_Log->WriteLogFile("Assistant", STR)
#define ADD_LOG(STR)        m_Log->WriteLogFileAtD("Assistant", STR, "D:")
// ---------------------------------------------------------------------------
__fastcall TAssistantMainForm::TAssistantMainForm(TComponent *Owner) : TForm(Owner)
{
	m_Log = new OHTLogFile;
	memset(&m_AstParam, 0, sizeof(ASSISTANT_PARAM));

	// by zzang9un 2013.04.03 : TrayIcon���� �ٷ� ����
	TrayIconAssistant->Visible = true;

	//2013.12.16 doori.shin FBWF Manager ��ü ����
	m_FBWFStatus = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_CURRENT_STATUS);

	// AssistantParam �б�
	LoadAssistantParam();

	// File Version Save
	// 1) ���Ϲ��� ��������
	AnsiString strAsVer;

	strAsVer.sprintf("%u.%u.%u.%u",
		(BYTE)VerInfo.Major,
		(BYTE)VerInfo.Minor,
		(BYTE)VerInfo.Release,
		(BYTE)VerInfo.Build);

	// 2) Save
	AnsiString strFileName = AS_VER_FILE_PATH;

	FILE *stream;

	try
	{
		stream = fopen(strFileName.c_str(), "w+");
		if(stream == NULL)
			return;

		fprintf(stream, "%s", strAsVer);

		if(stream)
			fclose(stream);
		stream = NULL;
	}
	catch(...)
	{
		if(stream)
			fclose(stream);
		stream = NULL;
	}

	//frmWork->Show();
}


void __fastcall TAssistantMainForm::FormCreate(TObject *Sender)
{
    // HideStart();
	ExceptCheckTimer->Enabled = true;

	//�α����� ���� ������ ����
	ptrFileThread = new FileThreadUnit(true);
	//FM control thread ����
	ptrFMcontrolThread = new FMcontrolThreadUnit(true);
}
// ---------------------------------------------------------------------------

void __fastcall TAssistantMainForm::FormDestroy(TObject *Sender)
{
	ptrFileThread->Terminate();
	delete ptrFileThread;

	ExceptCheckTimer->Enabled = false;

	delete m_Log;
}
// ---------------------------------------------------------------------------

void __fastcall TAssistantMainForm::mniAboutClick(TObject *Sender)
{
    // by zzang9un 2013.04.03 : AboutBox Form Show
    AboutBox->ShowModal();
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantMainForm::mniExitClick(TObject *Sender)
{

    AboutBox->Close();
//    SubForm->Close();
    Close();
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantMainForm::DeviceChangeProc(Messages::TMessage &Message)
{
    // Device Insert/Eject Event
    PDEV_BROADCAST_HDR hdr;
    // if(gm_OHTTotalData->gm_ParameterData==NULL) TForm::WndProc(Message);

    switch(Message.Msg)
    {
    case WM_DEVICECHANGE:
        switch(Message.WParam)
        {
        case DBT_DEVICEREMOVECOMPLETE: // remove
            hdr = (PDEV_BROADCAST_HDR)Message.LParam;
            if(hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) // USB Type
            {
                if(g_bIsEnableLogSave == true)
                    g_bIsEnableLogSave = false;
            }
            break;
        case DBT_DEVICEARRIVAL: // insert
            hdr = (PDEV_BROADCAST_HDR)Message.LParam;
            // if(hdr->dbch_devicetype==DBT_DEVTYP_VOLUME) //USB Type
            // {
            // }
            break;
        }
        Message.Result = true;
        break;
    default:
        TForm::WndProc(Message);
    }
}



void __fastcall TAssistantMainForm::mniOHTExecClick(TObject *Sender)
{
	FileControl::getInstance().ExecOHTProgram();
}
// ---------------------------------------------------------------------------

void __fastcall TAssistantMainForm::mniOHTCloseClick(TObject *Sender)
{
	FileControl::getInstance().CloseOHTProgram();
	FileControl::getInstance().CloseExceptProgram();
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantMainForm::ExceptCheckTimerTimer(TObject *Sender)
{
	FileControl::getInstance().CloseExceptProgram();
}

// ---------------------------------------------------------------------------
void __fastcall TAssistantMainForm::LogDriveTimerTimer(TObject *Sender)
{
	static int nCount = 0;
	int nIsAlive = 0;

	if(AssistantMainForm->g_bIsEnableLogSave == false)
	{
		LogDriveTimer->Enabled = false;
		return;
	}

	nIsAlive = Get_IsLogDriveAlive(g_strLogPath.t_str());

	if(nIsAlive == 0)
    {
        if(g_bIsEnableLogSave == true)
            g_bIsEnableLogSave = false;
        nCount = 0;
    }
    else
    {
        if(g_bIsEnableLogSave == false)
        {
            nCount++;
            if(nCount >= 1000) // 10��
            {
                g_bIsEnableLogSave = true;
                nCount = 0;
            }
        }
        else
            nCount = 0;
    }
}
/**
@brief   	OHT Main, Fm���α׷��� ���࿩�� Monitoring
@author  	Jrespect
@date    	2018.03.10
@return	 	���࿩�ο� ����  LOG �ۼ�
@purpose    OHT Main ����� ���� ��źҰ� or CP Down�� ���� ��źҰ� �Ǵ�
*/
void __fastcall TAssistantMainForm::MonitoringOHT(TObject *Sender)
{
  String strLog;  // Log�� ���� ����
  DWORD pOHT = ProcessTerminator::getInstance().FindPID(OHT_MAIN_FILE_NAME);

  if (pOHT != NULL){
		strLog.sprintf(L"OHT Running");
		ADD_LOG(strLog);
  }
  else{
		strLog.sprintf(L"OHT Close, Warning Communication fail");
		ADD_LOG(strLog);
	  }

  DWORD pFM = ProcessTerminator::getInstance().FindPID(FM_FILE_NAME);
  if (pFM != NULL) {
		strLog.sprintf(L"FM Running");
		ADD_LOG(strLog);
  }
  else{
		strLog.sprintf(L"FM Close, Warning Communication fail");
		ADD_LOG(strLog);
	  }
}

//---------------------------------------------------------------------------
void __fastcall TAssistantMainForm::mniFBWFClick(TObject *Sender)
{
	FBWFManagerForm->Show();
}
//---------------------------------------------------------------------------


int TAssistantMainForm::GetFBWFStatus()
{
	return m_FBWFStatus;
}

int TAssistantMainForm::SetFBWFStatus(int status)
{
	switch(status)
	{
		case TPCMD_AST_OHT_REQ_FBWF_ENABLE:
			m_FBWFStatus = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_SET_ENABLE);
		break;
		case TPCMD_AST_OHT_REQ_FBWF_DISABLE:
			m_FBWFStatus = MonitoringUtil::getInstance().ExecuteDosCmd(FBWF_SET_DISABLE);
		break;
	}

	if(m_FBWFStatus != 0)
	{
		//error
	}

	return m_FBWFStatus;
}

/*

	#define FILE_PATH_AST_PARAM		"./DataFiles\\AssistantParam.ini"  example

	[FileControl]
	MaxFileMonitoringInterval	= 1000*60*60	; Deleter ���� �ֱ�(ms)
	MaxWaitTimeToDelete		= 7		; �ִ� ���� �� ��(days)

	[Log]
	LogSaveMode	= 2	; 0:�������, 1:�������� ����Log����, 2: LogPath�� ����
	LogPath		= "D:\Log\"
	
*/
void TAssistantMainForm::LoadAssistantParam()
{
	m_IniFile = new TIniFile(FILE_PATH_AST_PARAM);

	if(m_IniFile != NULL)
	{
		//FileControl
		m_AstParam.nFileMonitoringInterval = m_IniFile->ReadInteger("FileControl", "MaxFileMonitoringInterval", 1000*60*60);
		m_AstParam.nMaxWaitTimeToDelete = m_IniFile->ReadInteger("FileControl", "MaxWaitTimeToDelete", 5);

		//Log
		m_AstParam.nLogSaveMode = m_IniFile->ReadInteger("Log", "LogSaveMode", 2);
		m_AstParam.strLogPath = m_IniFile->ReadString("Log", "LogPath", "D:\\Log\\");
	}

	delete m_IniFile;
}

void __fastcall TAssistantMainForm::mnuComLogViewClick(TObject *Sender)
{
	ptrFMcontrolThread->UdpShow();
}
//---------------------------------------------------------------------------

void __fastcall TAssistantMainForm::mnuComLogHideClick(TObject *Sender)
{
	ptrFMcontrolThread->UdpHide();
}
//---------------------------------------------------------------------------
