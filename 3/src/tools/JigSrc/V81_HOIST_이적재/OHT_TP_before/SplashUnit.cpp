/**
@file    SplashUnit.CPP
$Rev:: 735           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-22 1#$:  Date of last commit
@version OHT 7.0
@brief   OHT TP ���α׷� �ʱ�ȭ���� ����ϴ� SplashForm CPP ����
*/

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <mmsystem.h>

#include "SplashUnit.h"
#include "Def_TP.h"
#include "MainUnit.h"
#include "SocketUnit.h"

#include "StationManagerUnit.h" // by zzang9un 2013.04.22 : For Test
#include "OHTInfoUnit.h"        // by zzang9un 2013.04.23 : For Test
#include "Define.h"             // by zzang9un 2013.04.23 : For Test
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplashForm *SplashForm;
//---------------------------------------------------------------------------
#define TIMER_INTERVAL      500     ///< Timer Interval
#define WAITTIME_CONNECT    5000    ///< ���� ��� �ð� 10��
#define MAXLENGTH_ID        20      ///< ID ���ڿ� �ִ� ����
#define MAXLENGTH_PW        20      ///< Password ���ڿ� �ִ� ����
//---------------------------------------------------------------------------

/**
@brief   SplashForm ������
@author  zzang9un
@date    2013.04.18
*/
__fastcall TSplashForm::TSplashForm(TComponent* Owner)
    : TForm(Owner)
{
    // Active Paqge�� Login page�� ����
    pgcSplash->ActivePageIndex = 0;

    // timer �ʱ�ȭ
    timerSplash->Enabled = false;
    timerSplash->Interval = TIMER_INTERVAL;

    // �޸� �ʱ�ȭ
    memoSplash->Clear();

    // progressbar �ʱ�ȭ
    pbSplash->Max = STATE_END;

    m_strID = NULL;
    m_strPassword = NULL;

    // SplashForm Show
    //Show();
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm Timer

         OHT TP ���α׷� �ʱ�ȭ ��ƾ�� �� �Լ����� �����Ѵ�.
@author  zzang9un
@date    2013.04.19
*/
void __fastcall TSplashForm::timerSplashTimer(TObject *Sender)
{
    static INIT_STATE Step = STATE_NONE;
    static DWORD BeginTime = 0;
    String strLog = NULL;

    pbSplash->Position = Step;

    if(Step == STATE_MAX)
    {
        // ������ Step�� ���� Splash ����
        timerSplash->Enabled = false;
        Close();
        MainForm->Show();
    }

    switch(Step)
    {
    case STATE_NONE:
        BeginTime = timeGetTime(); // ���� ���� ���
        strLog.sprintf(L"Data Server(%s) ���� �õ� ��...", m_strDataServerIPAddr);
        PrintMemo(COLOR_BLACK, strLog);
        Step = STATE_CONNECTCHECK;
        break;

    case STATE_CONNECTCHECK:    // Data Server Connect üũ
        if(SocketForm->DataServerSocket->Socket->Connected)
        {
            strLog.sprintf(L"Data Server(%s) ���� ����...", m_strDataServerIPAddr);
            PrintMemo(COLOR_BLACK, strLog);
            Step = STATE_DATAVERREQUEST;
        }

        if((Step == STATE_CONNECTCHECK) && (timeGetTime() - BeginTime > WAITTIME_CONNECT))
        {
            Step = STATE_CONNECTFAIL;
        }

        break;

    case STATE_DATAVERREQUEST:  // Data ���� ��û
        // Data Sever�� ���� ��û ���� ����

        PrintMemo(COLOR_BLACK, "Data ���� ���� ��û...");
        Step = STATE_STATIONSYNC;
        break;

    case STATE_STATIONSYNC:     // Station Map Data �ٿ�ε�
        PrintMemo(COLOR_BLACK, "Station Map Data �ٿ�ε�...");
        Step = STATE_STATIONSYNCDONE;
        break;

    case STATE_STATIONSYNCDONE: // Station Map Data �ٿ�ε� �Ϸ�
        PrintMemo(COLOR_BLACK, "Station Map Data �ٿ�ε� �Ϸ�...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_STATIONSYNCFAIL: // Station Map Data �ٿ�ε� Fail
        PrintMemo(COLOR_BLACK, "Station Map Data �ٿ�ε� ����...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_TEACHINGSYNC:     // Teaching Data �ٿ�ε�
        PrintMemo(COLOR_BLACK, "Teaching Data �ٿ�ε�...");
        Step = STATE_TEACHINGSYNCDONE;
        break;

    case STATE_TEACHINGSYNCDONE: // Teaching Data �ٿ�ε� �Ϸ�
        PrintMemo(COLOR_BLACK, "Teaching Data �ٿ�ε� �Ϸ�...");
        Step = STATE_DATASYNCCOMPLETE;
        break;

    case STATE_TEACHINGSYNCFAIL: // Teaching Data �ٿ�ε� Fail
        PrintMemo(COLOR_BLACK, "Teaching Data �ٿ�ε� ����...");
        Step = STATE_TEACHINGSYNC;
        break;

    case STATE_DATASYNCCOMPLETE:
        PrintMemo(COLOR_BLACK, "Data ����ȭ �Ϸ�...");
        Step = STATE_END;
        break;

    case STATE_DATASYNCFAIL:
        PrintMemo(COLOR_RED, "Data ����ȭ ����...");
        Step = STATE_END;
        break;

    case STATE_CONNECTFAIL:
        PrintMemo(COLOR_RED, "Data Server ���� ����...");
        Step = STATE_END;
        break;

    case STATE_END:
        PrintMemo(COLOR_BLUE, "�ʱ�ȭ �Ϸ�...");
        Step = STATE_MAX;
        break;
    }
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm �޸𿡼� ���� TTextAttributes�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.04.19
@return  TTextAttributes    
*/
TTextAttributes *__fastcall TSplashForm::CurrText(void)
{
	return memoSplash->SelAttributes;
}
//---------------------------------------------------------------------------

/**
@brief   SplashForm �޸� ����ϴ� �Լ�
@author  zzang9un
@date    2013.04.18
@param   TmpFontColor : �ؽ�Ʈ ����
@param   strDisplayData : �޸� ����� ���ڿ�
*/
void TSplashForm::PrintMemo(TColor TmpFontColor, String strDisplayData)
{
	if(memoSplash->Lines->Count>500)
		memoSplash->Clear();

	CurrText()->Color = TmpFontColor;
	memoSplash->Lines->Add(strDisplayData);
}
//---------------------------------------------------------------------------

/**
@brief   Login ��ư Ŭ�� �� ȣ��Ǵ� �̺�Ʈ �Լ�
@author  zzang9un
@date    2013.04.19
*/
void __fastcall TSplashForm::btnLoginClick(TObject *Sender)
{
    m_strID = edtID->Text;
    m_strPassword = edtPassword->Text;
    m_strDataServerIPAddr = edtDataServerIPAddr->Text;

    // ���� ó��
    if(m_strID.Length() > MAXLENGTH_ID)
    {
        Application->MessageBoxW(L"ID�� 20�� �̳��� �Է��ϼ���.", L"Error", MB_OKCANCEL);
        return;
    }

    if(m_strPassword.Length() > MAXLENGTH_PW)
    {
        Application->MessageBoxW(L"Password�� 20�� �̳��� �Է��ϼ���.", L"Error", MB_OKCANCEL);
        return;
    }

    if(IsAbailableIP(m_strDataServerIPAddr) == false)
    {
        Application->MessageBoxW(L"IP �ּ� ������ Ʋ���ϴ�.", L"Error", MB_OKCANCEL);
        return;
    }

    // Data Server�� �α��� ó��
    SocketForm->ConnectDataServer(m_strDataServerIPAddr);
    timerSplash->Enabled = true;

    // by zzang9un 2013.04.23 : ������ �����ڷ� �α��εǵ��� �ӽ� ����
    gm_OHTStatus->nProgram_Mode = MODE_SETUP;
    MainForm->panUser->Caption = "������";

    // Active Paqge�� memo�� �ִ� �������� ����
    pgcSplash->ActivePageIndex = 1;
}
//---------------------------------------------------------------------------

/**
@brief   ���ڿ��� IP �ּҷ� ���� �� �ִ� ���� üũ�ϴ� �Լ�
@author  zzang9un
@date    2013.04.19
@param   strIPAddr : üũ�� ���ڿ�
@return  IP �ּҷ� ����� �� �ִ� ���ڿ��̸� true, �׷��� ���� ��� false�� ����
*/
bool TSplashForm::IsAbailableIP(String strIPAddr)
{
    if(strIPAddr == NULL )
        return FALSE;
        
    int len = strIPAddr.Length();
 
    // 7��( 1.1.1.1 ) �̻�&& 15��( 123.123.123.123 ) ����
    if(len > 15 || len < 7)
        return FALSE;
 
    int nNumCount = 0;
    int nDotCount = 0;

    // ��ȿ���˻�
    for(int i = 1; i < len + 1; i++)
    {
        TCHAR ch = strIPAddr[i];

        if(ch < '0' || ch > '9')
        {
            if('.' == ch)
            {
                ++nDotCount;
                nNumCount = 0;    
            }
            else
                return FALSE;
        }
        else
        {
            if(++nNumCount > 3)
                return FALSE;
        }
    }
 
    if(nDotCount != 3)
        return FALSE;
 
    return TRUE;
}
//---------------------------------------------------------------------------
void __fastcall TSplashForm::imgOHTClick(TObject *Sender)
{
    // by zzang9un 2013.04.22 : For Test
    StationManagerForm->Show();
}
//---------------------------------------------------------------------------

