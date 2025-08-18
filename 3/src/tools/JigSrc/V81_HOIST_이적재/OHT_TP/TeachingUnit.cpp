// ---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#pragma hdrstop

#include "TeachingUnit.h"
#include "MainUnit.h"
#include "StatusUnit.h"
#include "OHTInfoUnit.h"
#include "ChkExecUnit.h"
#include "Def_TPCommand.h"
#include "CMDControlUnit.h"
#include "Define.h"
#include "MoveHoistUnit.h"
#include "WarnningUnit.h"
#include "SocketUnit.h"
#include "HoistParamUnit.h"
#include "Def_TP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTeachingForm *TeachingForm;
TCHKOHT TechOHTCheck;
// ---------------------------------------------------------------------------
#define ERROR_MSGBOX(STR)   Application->MessageBox(_T(STR), _T("Error"), MB_OK);

// by zzang9un 2013.07.16 : Jog ������
#define JOG_ABS         0       ///< ���밪 ����
#define JOG_RES         1       ///< ��밪 ����

/**
@brief   TeachingForm ������
*/
__fastcall TTeachingForm::TTeachingForm(TComponent* Owner) : TForm(Owner)
{
    timerUpdateDisplay->Enabled = false;

    dAT2Data = 0.0;
    dAT3Data = 0.0;
    dAT4Data = 0.0;

    // by zzang9un 2013.07.19 : Ư�� UI ReadOnly�� ����
    cbbPre_PortType->Enabled = false;
    edtPre_Hoist->ReadOnly = true;
    edtPre_Shift->ReadOnly = true;
    edtPre_Rotate->ReadOnly = true;
    cbbPre_PIODirection->Enabled = false;
    edtPre_PIOTimeLevel->ReadOnly = true;
    edtPre_LookdownLevel->ReadOnly = true;

    chkPre_RFReader->Checked = false;
    chkPre_TransSound->Checked = false;
    chkPre_Oscillation->Checked = false;
    chkPre_HandDetectEQ->Checked = false;

    chkPre_RFReader->Enabled = false;
    chkPre_TransSound->Enabled = false;
    chkPre_Oscillation->Enabled = false;
    chkPre_HandDetectEQ->Enabled = false;

    edtHoist->ReadOnly = true;
    edtShift->ReadOnly = true;
    edtRotate->ReadOnly = true;

    pgcTeaching->ActivePage = tabJog;
}
// ---------------------------------------------------------------------------

/**
@brief   FormShow �̺�Ʈ�Լ�

         Teaching Form UI �ʱ�ȭ
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::FormShow(TObject *Sender)
{
    // by zzang9un 2013.07.16 : TeachingForm �ʱ� UI ����
    TeachingForm->Top = MainForm->Top + MainForm->Height;
    TeachingForm->Left = MainForm->Left;
    TeachingForm->Width = MainForm->Width + StatusForm->Width;

    if(timerUpdateDisplay->Enabled == false)
        timerUpdateDisplay->Enabled = true;

    // Jog �ʱ� ���� ��
    SetHoistJogUI(JOG_RES);
    SetShiftJogUI(JOG_RES);
    SetRotateJogUI(JOG_RES);

    // Auto Teaching Panel ��Ȱ��ȭ
    panAutoTeaching->Visible = false;

    // Rotate ������ ���� UI ����
    if(gm_OHTStatus->nIsRotateExist == 1)
        grpRotateTeaching->Visible = true;
    else
        grpRotateTeaching->Visible = false;

    // Teaching tab ����

    // Form�� �������� ���� ���� �����´�.
    edtHoist->Text = gm_OHTStatus->Motor_Hoist->dPosition_mm;
    edtShift->Text = gm_OHTStatus->Motor_Shift->dPosition_mm;
    edtRotate->Text = gm_OHTStatus->Motor_Rotate->dPosition_mm/10;

    // ������ UI�� ����Ʈ ������ �����Ѵ�.
    SetDefaultTeachingData();
}
// ---------------------------------------------------------------------------

/**
@brief   udHoistDist OnChanging �̺�Ʈ�Լ�

         Hoist Jog ������ ��밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::udHoistDistChanging(TObject *Sender, bool &AllowChange)
{
    SetHoistJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

/**
@brief   udShiftDist OnChanging �̺�Ʈ�Լ�

         Shift Jog ������ ��밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::udShiftDistChanging(TObject *Sender, bool &AllowChange)
{
    SetShiftJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

/**
@brief   udRotateDist OnChanging �̺�Ʈ�Լ�

         Rotate ������ ��밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::udRotateDistChanging(TObject *Sender, bool &AllowChange)
{
    SetRotateJogUI(JOG_RES);
}
// ---------------------------------------------------------------------------

/**
@brief   edtHoistJogABS OnClick �̺�Ʈ�Լ�

         Hoist Jog ������ ���밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::edtHoistJogABSClick(TObject *Sender)
{
    SetHoistJogUI(JOG_ABS);
}
// ---------------------------------------------------------------------------

/**
@brief   edtHoistJogDist OnClick �̺�Ʈ�Լ�

         Hoist Jog ������ ��밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::edtHoistJogDistClick(TObject *Sender)
{
    SetHoistJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

/**
@brief   edtShiftJogABS OnClick �̺�Ʈ�Լ�

         Shift Jog ������ ���밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::edtShiftJogABSClick(TObject *Sender)
{
    SetShiftJogUI(JOG_ABS);
}
//---------------------------------------------------------------------------

/**
@brief   edtShiftJogDist OnClick �̺�Ʈ�Լ�

         Shift Jog ������ ��밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::edtShiftJogDistClick(TObject *Sender)
{
    SetShiftJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

/**
@brief   edtRotateJogDist OnClick �̺�Ʈ�Լ�

         Rotate Jog ������ ���밪���� �����Ѵ�.
@author  zzang9un
@date    2013.07.16
*/
void __fastcall TTeachingForm::edtRotateJogABSClick(TObject *Sender)
{
    SetRotateJogUI(JOG_ABS);
}
//---------------------------------------------------------------------------

/**
@brief   Hoist Jog ������ ���� UI ������ �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.17
@param   JogSetting : Jog ������(���밪, ��밪)
*/
void TTeachingForm::SetHoistJogUI(BYTE JogSetting)
{
    switch(JogSetting)
    {
    case JOG_ABS: // ���밪 
        // üũ�ڽ� ����
        chkHoistJogABS->Checked = true;
        chkHoistJogDist->Checked = false;
        
        // ��ư Ȱ��ȭ ����
        btnHoistMoveABS->Visible = true;
        btnHoistMoveUpDist->Visible = false;
        btnHoistMoveDownDist->Visible = false;
        break;

    case JOG_RES: // ��밪
        // üũ�ڽ� ����
        chkHoistJogABS->Checked = false;
        chkHoistJogDist->Checked = true;
        
        // ��ư Ȱ��ȭ ����
        btnHoistMoveABS->Visible = false;
        btnHoistMoveUpDist->Visible = true;
        btnHoistMoveDownDist->Visible = true;
        break;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shift Jog ������ ���� UI ������ �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.17
@param   JogSetting : Jog ������(���밪, ��밪)
*/
void TTeachingForm::SetShiftJogUI(BYTE JogSetting)
{
    switch(JogSetting)
    {
    case JOG_ABS: // ���밪 
        // üũ�ڽ� ����
        chkShiftJogABS->Checked = true;
        chkShiftJogDist->Checked = false;
        
        // ��ư Ȱ��ȭ ����
        btnShiftMoveABS->Visible = true;
        btnShiftMoveLeftDist->Visible = false;
        btnShiftMoveRightDist->Visible = false;
        break;

    case JOG_RES: // ��밪
        // üũ�ڽ� ����
        chkShiftJogABS->Checked = false;
        chkShiftJogDist->Checked = true;

        // ��ư Ȱ��ȭ ����
        btnShiftMoveABS->Visible = false;
        btnShiftMoveLeftDist->Visible = true;
        btnShiftMoveRightDist->Visible = true;
        break;
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Rotate Jog ������ ���� UI ������ �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.17
@param   JogSetting : Jog ������(���밪, ��밪)
*/
void TTeachingForm::SetRotateJogUI(BYTE JogSetting)
{
    switch(JogSetting)
    {
    case JOG_ABS: // ���밪 
        // üũ�ڽ� ����
        chkRotateJogABS->Checked = true;
        chkRotateJogDist->Checked = false;
        
        // ��ư Ȱ��ȭ ����
        btnRotateMoveABS->Visible = true;
        btnRotateMoveLeft->Visible = false;
        btnRotateMoveRight->Visible = false;
        break;

    case JOG_RES: // ��밪
        // üũ�ڽ� ����
        chkRotateJogABS->Checked = false;
        chkRotateJogDist->Checked = true;

        // ��ư Ȱ��ȭ ����
        btnRotateMoveABS->Visible = false;
        btnRotateMoveLeft->Visible = true;
        btnRotateMoveRight->Visible = true;
        break;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    timerUpdateDisplay->Enabled = false;
}
// ---------------------------------------------------------------------------

/**
 @brief   TeachingForm�� UI�� �ֱ������� Update�ϴ� timer �Լ�
 @author  zzang9un
 @date    2013.02.19
 */
void __fastcall TTeachingForm::timerUpdateDisplayTimer(TObject *Sender)
{
    int nMotor4Pos;
    String strM4Data;

    // Hoist ���� ���ڴ���, Teaching Data Update
    lbHoistCurCts->Caption = gm_OHTStatus->Motor_Hoist->dPosition_mm;
    lbHoistCurPos->Caption = gm_OHTStatus->Motor_Hoist->dPosition_mm;
    MHoistForm->lblMotor2->Caption = gm_OHTStatus->Motor_Hoist->dPosition_mm;

    // Shift ���� ���ڴ���, Teaching Data Update
    lbShiftCurCts->Caption = gm_OHTStatus->Motor_Shift->dPosition_cts;
    lbShiftCurPos->Caption = gm_OHTStatus->Motor_Shift->dPosition_mm;

    // by zzang9un 2013.02.19 : Rotate�� ���� �ҽ� �״�� ����
    // 2016-12-04, Add by Labeler
    if(gm_OHTStatus->nIsRotateExist == 1)
    {
//        strM4Data.sprintf(L"%n[��]", (int)gm_OHTStatus->Motor_Rotate->dPosition_mm);
//        lblDMotor4->Caption = strM4Data;
        lblDMotor4->Caption = gm_OHTStatus->Motor_Rotate->dPosition_mm/10;
        shp4Run->Brush->Color = (gm_OHTStatus->Motor_Rotate->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
		edtRotate->Text = gm_OHTStatus->Motor_Rotate->dPosition_mm/10;
    }

    shpHoistRunning->Brush->Color = (gm_OHTStatus->Motor_Hoist->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
    shpShiftRunning->Brush->Color = (gm_OHTStatus->Motor_Shift->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;
//    shp4Run->Brush->Color = (gm_OHTStatus->Motor_Rotate->nAxis_Running == ON) ? COLOR_RED : COLOR_WHITE;

    // Teaching Tab�� ������ ������Ʈ�Ѵ�.
    edtHoist->Text = gm_OHTStatus->Motor_Hoist->dPosition_mm;
    edtShift->Text = gm_OHTStatus->Motor_Shift->dPosition_mm;
//    edtRotate->Text = gm_OHTStatus->Motor_Rotate->dPosition_mm;

}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnHoistMoveHomeClick(TObject *Sender)
{
    TechOHTCheck.Axis_Hoist_HOME();
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveHomeClick(TObject *Sender)
{
    TechOHTCheck.Axis_Shift_HOME();
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnHoistMoveUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
   	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bHoistMoveCommand = true;
            nHoistMoveDir = HOIST_JOG_UP;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_NONE;

            bRotateMoveCommand = false;
            nRotateMoveDir = ROTATE_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP");
        }
    }
    else
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bHoistMoveCommand = true;
            nHoistMoveDir = HOIST_JOG_UP;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP");
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnHoistMoveUpMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(TechOHTCheck.Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) == NO_ERR)
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		bHoistMoveCommand = false;
		nHoistMoveDir = HOIST_JOG_NONE;
		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_STOP);
		tmrJogMove->Enabled = false;
		MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_STOP");
		delete m_CMDControl;

    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnHoistMoveDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bHoistMoveCommand = true;
            nHoistMoveDir = HOIST_JOG_DOWN;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_NONE;

            bRotateMoveCommand = false;
            nRotateMoveDir = ROTATE_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DOWN");
        }
    }
    else
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bHoistMoveCommand = true;
            nHoistMoveDir = HOIST_JOG_DOWN;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DOWN");
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveLeftMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bShiftMoveCommand = true;
            nShiftMoveDir = SHIFT_JOG_LEFT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            bRotateMoveCommand = false;
            nRotateMoveDir = ROTATE_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_LEFT");
        }
    }
    else
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bShiftMoveCommand = true;
            nShiftMoveDir = SHIFT_JOG_LEFT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_LEFT");
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveLeftMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(TechOHTCheck.Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) == NO_ERR)
	{
		TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
		bShiftMoveCommand = false;
		nShiftMoveDir = SHIFT_JOG_NONE;
		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_STOP);
		tmrJogMove->Enabled = false;
		MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_STOP");
		delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveRightMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
 	if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type == OHT_TYPE_EUV)
	{
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bShiftMoveCommand = true;
            nShiftMoveDir = SHIFT_JOG_RIGHT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            bRotateMoveCommand = false;
            nRotateMoveDir = ROTATE_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_RIGHT");
        }
    }
    else
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
        {
            bShiftMoveCommand = true;
            nShiftMoveDir = SHIFT_JOG_RIGHT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_RIGHT");
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btn4CWCmdMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    bool bHoistHome = false;

	bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;


	if((bHoistHome == ON) &&
		(fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) < HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            bRotateMoveCommand = true;
            nRotateMoveDir = ROTATE_JOG_CW;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_RIGHT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CW");

/*
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CW);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CW");
            delete m_CMDControl;
*/
        }
    }
    else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btn4CWCmdMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(TechOHTCheck.Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING) == NO_ERR)
	{
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
		bRotateMoveCommand = false;
		nRotateMoveDir = ROTATE_JOG_NONE;
		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_STOP);
		tmrJogMove->Enabled = false;
		MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_STOP");
		delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btn4CCWCmdMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    bool bHoistHome = false;

	bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;


	if((bHoistHome == ON) &&
        (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) < HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            bRotateMoveCommand = true;
            nRotateMoveDir = ROTATE_JOG_CCW;

            bShiftMoveCommand = false;
            nShiftMoveDir = SHIFT_JOG_RIGHT;

            bHoistMoveCommand = false;
            nHoistMoveDir = HOIST_JOG_NONE;

            tmrJogMove->Enabled = true;
            MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CW");

            /*
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CCW);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CCW");
            delete m_CMDControl;
            */
        }
    }
    else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}
// ---------------------------------------------------------------------------

/**
@brief   Hoist�� Up �������� ���� �Ÿ� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
@note    ��밪�� ������ ���ο��� �˾Ƽ� ���Ǿ� Move �ȴ�.
*/
void __fastcall TTeachingForm::btnHoistMoveUpDistClick(TObject *Sender)
{
    int MoveDist = 0;
    String strTmpCMD;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDist = (int)(edtHoistJogDist->Text.ToDouble() * 10.0);

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_UP_INC, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP_INC:" + String(MoveDist));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Hoist�� Down �������� ���� �Ÿ� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
@note    ��밪�� ������ ���ο��� �˾Ƽ� ���Ǿ� Move �ȴ�.
*/
void __fastcall TTeachingForm::btnHoistMoveDownDistClick(TObject *Sender)
{
    int MoveDist = 0;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDist = (int)(edtHoistJogDist->Text.ToDouble() * 10.0);

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DOWN_INC, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DOWN_INC:" + String(MoveDist));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Hoist�� Ư�� ��ġ(���밪)�� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TTeachingForm::btnHoistMoveABSClick(TObject *Sender)
{
    int MoveDistABS = 0;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDistABS = (int)(edtHoistJogABS->Text.ToDouble() * 10.0);

        if((MoveDistABS < edtHoistPosLimit->Text.ToDouble()) || (MoveDistABS > edtHoistNegLimit->Text.ToDouble()))
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDistABS;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_ABS:" + String(MoveDistABS));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shift�� Left �������� ���� �Ÿ� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
@note    ��밪�� ������ ���ο��� �˾Ƽ� ���Ǿ� Move �ȴ�.
*/
void __fastcall TTeachingForm::btnShiftMoveLeftDistClick(TObject *Sender)
{
    int MoveDist = 0;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDist = (int)(edtShiftJogDist->Text.ToDouble() * 10.0);

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_LEFT_INC, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_LEFT_INC:" + String(MoveDist));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shift�� Left �������� ���� �Ÿ� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
@note    ��밪�� ������ ���ο��� �˾Ƽ� ���Ǿ� Move �ȴ�.
*/
void __fastcall TTeachingForm::btnShiftMoveRightDistClick(TObject *Sender)
{
    int MoveDist = 0;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDist = (int)(edtShiftJogDist->Text.ToDouble() * 10.0);

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_RIGHT_INC, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_RIGHT_INC:" + String(MoveDist));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

/**
@brief   Shift�� Ư�� ��ġ(���밪)�� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
*/
void __fastcall TTeachingForm::btnShiftMoveABSClick(TObject *Sender)
{
    int MoveDistABS = 0;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
        MoveDistABS = (int)(edtShiftJogABS->Text.ToDouble() * 10.0);

        if((MoveDistABS < edtShiftPosLimit->Text.ToDouble()) || (MoveDistABS > edtShiftNegLimit->Text.ToDouble()))
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDistABS;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_ABS, &TPCmd);
            MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_ABS:" + String(MoveDistABS));
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnRotateMoveLeftClick(TObject *Sender)
{
    int nTmpDist = 0;
	String strTmpCMD;
   bool bHoistHome = false;

	bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;


	// puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
	uTP_CMD TPCmd; // TP Command
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
			nTmpDist = 	(int)(edtRotateJogDist->Text.ToDouble()*10.0);
            TPCmd.CmdTP_Jog_TR.Distance = nTmpDist;

			if(((gm_OHTStatus->Motor_Rotate->dPosition_mm+nTmpDist) >= -50)&& ((gm_OHTStatus->Motor_Rotate->dPosition_mm+nTmpDist) < 2700))
			{
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                strTmpCMD.sprintf(L"%d", nTmpDist);
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CW_INC, &TPCmd);
                // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_ROTATE_CW_INC, strTmpCMD);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CW_INC:" + strTmpCMD);
                delete m_CMDControl;
			}
			else
				  MainForm->DataDisplay(COLOR_BLACK, "Rotate OverPosition..ERROR");
        }
    }
	else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnRotateMoveRightClick(TObject *Sender)
{
    int nTmpDist = 0;
	String strTmpCMD;
    bool bHoistHome = false;


	bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;


    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
		{
			nTmpDist = (int)(edtRotateJogDist->Text.ToDouble()*10.0);

            TPCmd.CmdTP_Jog_TR.Distance = nTmpDist;

			if(((gm_OHTStatus->Motor_Rotate->dPosition_mm-nTmpDist) >= -50)&& ((gm_OHTStatus->Motor_Rotate->dPosition_mm-nTmpDist) < 2700))
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
				strTmpCMD.sprintf(L"%d", nTmpDist);
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CCW_INC, &TPCmd);
                // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_ROTATE_CCW_INC, strTmpCMD);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CCW_INC:" + strTmpCMD);
                delete m_CMDControl;
            }
			else
				  MainForm->DataDisplay(COLOR_BLACK, "Rotate OverPosition..ERROR");
        }
    }
    else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnRotateMoveABSClick(TObject *Sender)
{
   int nTmpDist = 0;
   String strTmpCMD;
   bool bHoistHome = false;

   bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;



    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
		{
			nTmpDist = (int)(edtRotateJogABS->Text.ToDouble()*10.0);
            TPCmd.CmdTP_Jog_TR.Distance = nTmpDist;

// 2016-12-01, Add by Labeler
// 0�϶��� ������ ���� �ʿ�
// 0���� ������ ����

//            if(nTmpDist != 0)
			if((nTmpDist >= -50)&& (nTmpDist < 2700))
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                strTmpCMD.sprintf(L"%d", nTmpDist);
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
                // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_ROTATE_ABS, strTmpCMD);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_ABS =" + strTmpCMD);
                delete m_CMDControl;
            }else
				  MainForm->DataDisplay(COLOR_BLACK, "Rotate OverPosition..ERROR");
        }
    }
    else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnRotateOriginClick(TObject *Sender)
{
// 2016-12-04, Add by Labeler
// ���� ��ġ�� Origin���� �����ϴ� ��� �߰�
    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
    {
        String strTmpCMD;
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_ROTATE_ORIGIN_SETTING);
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Position Origin Set");
        delete m_CMDControl;
    }
}

// ---------------------------------------------------------------------------
void __fastcall TTeachingForm::btnSetHoistNegLimitClick(TObject *Sender)
{
    int nLimit;
    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    nLimit = StrToIntDef(edtHoistNegLimit->Text, 0);
    TPCmd.TPCmd.Data1 = StrToIntDef(edtHoistNegLimit->Text, 0);

    if(TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR)
    {
        if(nLimit >= 0)
        {
            MainForm->DataDisplay(COLOR_RED, "Limit Error : nLimit>=0");
        }
        else
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SETLIMIT_NEG, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_HOIST_SETLIMIT_NEG,edtHoistNegLimit->Text);
            MainForm->DataDisplay(COLOR_BLACK, "HOIST_SETLIMIT_NEG :" + edtHoistNegLimit->Text);
            delete m_CMDControl;
        }
    }
}

// ---------------------------------------------------------------------------
void __fastcall TTeachingForm::btnSetHoistPosLimitClick(TObject *Sender)
{
    int nLimit;
    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    nLimit = StrToIntDef(edtHoistPosLimit->Text, 0);
    TPCmd.TPCmd.Data1 = StrToIntDef(edtHoistPosLimit->Text, 0);

    TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SETLIMIT_POSI, &TPCmd);
    // m_CMDControl->MakeCommand2OHT(TPCMD_HOIST_SETLIMIT_POSI,edtHoistPosLimit->Text);
    MainForm->DataDisplay(COLOR_BLACK, "HOIST_SETLIMIT_POSI :" + edtHoistPosLimit->Text);
    delete m_CMDControl;

    if(TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR)
    {
        if(nLimit <= 0)
        {
            MainForm->DataDisplay(COLOR_RED, "Limit Error : nLimit<=0");
        }
        else
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_HOIST_SETLIMIT_POSI, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_HOIST_SETLIMIT_POSI,edtHoistPosLimit->Text);
            MainForm->DataDisplay(COLOR_BLACK, "HOIST_SETLIMIT_POSI :" + edtHoistPosLimit->Text);
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnSetShiftNegLimitClick(TObject *Sender)
{
    int nLimit;
    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    nLimit = StrToIntDef(edtShiftNegLimit->Text, 0);
    TPCmd.TPCmd.Data1 = StrToIntDef(edtShiftNegLimit->Text, 0);

    if(TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR)
    {
        if(nLimit >= 0)
        {
            MainForm->DataDisplay(COLOR_RED, "Limit Error : nLimit>=0");
        }
        else
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SETLIMIT_NEG, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_SHIFT_SETLIMIT_NEG,edtShiftNegLimit->Text);
            MainForm->DataDisplay(COLOR_BLACK, "SHIFT_SETLIMIT_NEG :" + edtShiftNegLimit->Text);
            delete m_CMDControl;
        }
    }

}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnSetShiftPosLimitClick(TObject *Sender)
{
    int nLimit;
    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    nLimit = StrToIntDef(edtShiftPosLimit->Text, 0);
    TPCmd.TPCmd.Data1 = StrToIntDef(edtShiftPosLimit->Text, 0);

    if(TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR)
    {
        if(nLimit <= 0)
        {
            MainForm->DataDisplay(COLOR_RED, "Limit Error : nLimit<=0");
        }
        else
        {
            TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
            m_CMDControl->MakeTPCmd2OHT(TPCMD_SHIFT_SETLIMIT_POSI, &TPCmd);
            // m_CMDControl->MakeCommand2OHT(TPCMD_SHIFT_SETLIMIT_POSI,edtShiftPosLimit->Text);
            MainForm->DataDisplay(COLOR_BLACK, "SHIFT_SETLIMIT_POSI :" + edtShiftPosLimit->Text);
            delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::ATTimerTimer(TObject *Sender)
{
    btnATMoveClick(Sender);
    
    if(TeachingForm->edtHoistJogDist->Text.w_str()[0] == '-')
        btnHoistMoveUpDistClick(Sender);
    else
        btnHoistMoveDownDistClick(Sender);
    
    if(TeachingForm->edtShiftJogDist->Text.w_str()[0] == '-')
        btnShiftMoveLeftDistClick(Sender);
    else                                                
        btnShiftMoveRightDistClick(Sender);
    
    if(TeachingForm->edtRotateJogDist->Text.w_str()[0] == '-')
        btnRotateMoveRightClick(Sender);
    else 
        btnRotateMoveLeftClick(Sender);

    ATTimer->Enabled = false;
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnATConnectClick(TObject *Sender)
{
    SocketForm->AutoTeaching_Connect();
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnATDisconClick(TObject *Sender)
{
    SocketForm->AutoTeaching_DisConnect();
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btn12mmUpClick(TObject *Sender)
{
    int nTmpDist = 0;
    String strTmpCMD;

    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));



    // by zzang9un 2013.07.23 : 12mm Up
    // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
    TPCmd.CmdTP_Jog_TR.Distance = (int)(12.0 * 10.0);

    if(nTmpDist != 0)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        strTmpCMD.sprintf(L"%d", nTmpDist);
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_UP_INC, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_HOIST_UP_INC, strTmpCMD);
        MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP_INC:" + strTmpCMD);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btn12mmDownClick(TObject *Sender)
{
    int nTmpDist = 0;
    String strTmpCMD;

    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    nTmpDist = nTmpDist - (int)(gm_OHTStatus->Motor_Hoist->dPosition_mm);

    // by zzang9un 2013.07.23 : 12mm Down
    // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)
    TPCmd.CmdTP_Jog_TR.Distance = (int)(12.0 * 10.0);
    
    if(nTmpDist != 0)
    {
        TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
        strTmpCMD.sprintf(L"%d", nTmpDist);
        m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DOWN_INC, &TPCmd);
        // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_HOIST_DOWN_INC, strTmpCMD);
        MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DOWN_INC:" + strTmpCMD);
        delete m_CMDControl;
    }
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnATReadClick(TObject *Sender)
{
    SocketForm->AutoTeaching_Command2AT();

    // ��밪���� ����
    SetHoistJogUI(JOG_RES);
    SetShiftJogUI(JOG_RES);
    SetRotateJogUI(JOG_RES);
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnATMoveClick(TObject *Sender)
{
    // by zzang9un 2013.07.24 : ��ȣ�� �����Ƿ� �ѹ������θ� ����ֵ��� ����
    btnHoistMoveDownDistClick(Sender);

    btnShiftMoveLeftDistClick(Sender);

    // by zzang9un 2013.07.24 : Rotate�� �����Ƿ� �ּ�ó��
    /*
    if(gm_OHTStatus->nIsRotateExist == 1)
    {
        if(TeachingForm->edtRotateJogDist->Text.w_str()[0]=='-')
            btnRotateMoveLeftClick(Sender);
        else
            btnRotateMoveRightClick(Sender);
    }
    */
}
// ---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnATReadMoveClick(TObject *Sender)
{
    // SocketForm->AutoTeaching_Command2AT();
    // ��밪���� ����
    SetHoistJogUI(JOG_RES);
    SetShiftJogUI(JOG_RES);
    SetRotateJogUI(JOG_RES);

    int iCount = 0;

    while(SocketForm->bATReceive == false)
    {
        Sleep(10);
        iCount++;

        if( iCount > 100 )
        {
            iCount = 0;
#if(LANGUAGE_MODE==MODE_KOREAN)
            Application->MessageBox(L"AutoTeaching Unit���� ������ �����ϴ�.",L"Error",MB_OK);
#else
            Application->MessageBox(L"No Response from AutoTeaching Unit.",L"Error",MB_OK);
#endif
            break;
        }
    }

    if(SocketForm->bATReceive == true)
    {
        ATTimer->Enabled = true;
        SocketForm->bATReceive = false;
    }
}

// ---------------------------------------------------------------------------
int TTeachingForm::Delete_Comma(String strInData)
{
    int nTmpPos;
    nTmpPos = strInData.Pos(",");
    strInData.Delete(nTmpPos, 1);
    return StrToIntDef(strInData, 0xFFFFFFFF);
}

// ---------------------------------------------------------------------------
double TTeachingForm::Delete_dComma(String strdbData)
{
    double dTmpPos;
    double dResp;
    dTmpPos = strdbData.Pos(",");
    strdbData.Delete(dTmpPos, 1);
    dResp = strdbData.ToDouble() / 1.0;
    return dResp;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkHoistJogABSMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkHoistJogABS->Checked)
        SetHoistJogUI(JOG_ABS);
    else
        SetHoistJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkHoistJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkHoistJogDist->Checked)
        SetHoistJogUI(JOG_RES);
    else
        SetHoistJogUI(JOG_ABS);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkShiftJogABSMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkShiftJogABS->Checked)
        SetShiftJogUI(JOG_ABS);
    else
        SetShiftJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkShiftJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkShiftJogDist->Checked)
        SetShiftJogUI(JOG_RES);
    else
        SetShiftJogUI(JOG_ABS);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkRotateJogABSMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkRotateJogABS->Checked)
        SetRotateJogUI(JOG_ABS);
    else
        SetRotateJogUI(JOG_RES);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::chkRotateJogDistMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(chkRotateJogDist->Checked)
        SetRotateJogUI(JOG_RES);
    else
        SetRotateJogUI(JOG_ABS);
}
//---------------------------------------------------------------------------

/**
@brief   Teaching Data ���� Default ���������� �ǵ�����.
@author  zzang9un
@date    2013.07.18
*/
void TTeachingForm::SetDefaultTeachingData()
{
    // by zzang9un 2013.07.19 : Previous ���� ����д�.
    cbbPre_PortType->ItemIndex = -1;
    edtPre_Hoist->Text = 0;
    edtPre_Shift->Text = 0;
    edtPre_Rotate->Text = 0;
    cbbPre_PIODirection->ItemIndex = -1;
    edtPre_PIOTimeLevel->Text = 0;
    edtPre_LookdownLevel->Text = 0;

    chkPre_RFReader->Checked = false;
    chkPre_TransSound->Checked = false;
    chkPre_Oscillation->Checked = false;
    chkPre_HandDetectEQ->Checked = false;

    // ����Ʈ ������ Teaching Data�� ä���.
    edtPIOTimeLevel->Text = MIN_PIOTIMELEVEL;
    edtLookDownLevel->Text = MIN_LOOKDOWNLEVEL;
    chkRFReader->Checked = false;
    chkTransSound->Checked = true;
    chkOscillation->Checked = true;
    chkHandDetectEQ->Checked = false;

    cbbPIODirection->ItemIndex = 0;
}
//---------------------------------------------------------------------------

/**
@brief   btnTeachingSave OnClick �̺�Ʈ �Լ�

         ���� ������ Teaching Data�� ���� ��û�Ѵ�.
@author  zzang9un
@date    2013.07.18
*/
void __fastcall TTeachingForm::btnTeachingSaveClick(TObject *Sender)
{
    // Teaching Data�� ��� ���� ��ȿ���� Ȯ���Ѵ�.
    if(Application->MessageBox(L"Teaching Data�� �����Ͻðڽ��ϱ�?\n�ٽ� �ѹ� Ȯ���غ��ñ� �ٶ��ϴ�.", L"Confirm", MB_OKCANCEL) != IDOK)
        return;        
	 //gm_OHTStatus->m_OHTStatus.Data.CurStationID=10007;

	 if(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust ==OFF)
	 {
	 	ERROR_MSGBOX("������ Map�� �ƴմϴ�..");
        return;
	 }

    // Step 1. Station ID üũ
    if(!gm_OHTStatus->m_OHTStatus.Data.CurStationID)
    {
        ERROR_MSGBOX("Station ID�� �����ϴ�.");
        return;
    }

	if(gm_OHTStatus->Motor_Hoist->dPosition_mm < 50.0)
	{
		ERROR_MSGBOX("hoist ���̰� �����ϴ�.");
		return;

    }
    // Step 2. Port Type üũ
    if(cbbPortType->ItemIndex == -1)
    {
        ERROR_MSGBOX("Port Type ���� �����ϴ�.");
        return;
    }

    // Step 3. PIO Direction üũ
    if(cbbPIODirection->ItemIndex == -1)
    {
        ERROR_MSGBOX("PIO Direction ���̾����ϴ�.");
        return;
    }

	// Step 4. PIO Time Level üũ
	if(edtPIOTimeLevel->Text == NULL)
	{
		ERROR_MSGBOX("PIO Time Level ���� �����ϴ�.");
		return;
	}
	if((edtPIOTimeLevel->Text.ToInt() <= 0) || (edtPIOTimeLevel->Text.ToInt() > 10))
	{
		ERROR_MSGBOX("PIO Time Level Range[1~10] Error");
		return;
	}

	// Step 5. Lookdown Level üũ
	if(edtLookDownLevel->Text == NULL)
	{
		ERROR_MSGBOX("Lookdown Level ���� �����ϴ�.");
		return;
	}
	if((edtLookDownLevel->Text.ToInt() <= 0) || (edtLookDownLevel->Text.ToInt() > 16))
	{
		ERROR_MSGBOX("Lookdown Level Ragne[1~16] Error");
		return;
	}



	// ���ο� ������ ��Ŷ�� �̿��Ͽ� ����
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_TEACHING_INFO_SAVE;

	/*	PortType (SEND_PACKET.BODY.PortType)
	EQ			= 1
	EQ_X		= 2
	Stocker		= 3
	Loader		= 4
	STB_Left	= 5
	STB_Right	= 6
	UTB
	*/
	SEND_PACKET.BODY.PortType = cbbPortType->ItemIndex + 1;
	
	/*	PIODirection (SEND_PACKET.BODY.EtcData.Info.PIODirection)
	Left	= 1
	Right	= 2
	*/
	//Hoist Shift Rotate�� ���� ����.
	//Hoist Shift Rotate�� OHT Main�� ������ �ִ� ���� ����

	SEND_PACKET.BODY.EtcData.Info.PIODirection = cbbPIODirection->ItemIndex + 1;

	SEND_PACKET.BODY.PIOType = cbbPIOType->ItemIndex + 1;
	SEND_PACKET.BODY.PIOChannel = edtPIOChannel->Text.ToInt();
	SEND_PACKET.BODY.PIOID = edtPIOID->Text.ToInt();

	if(StrToInt(edtPIOTimeLevel->Text) < MIN_PIOTIMELEVEL)
		SEND_PACKET.BODY.EtcData.Info.PIOTimeLevel = MIN_PIOTIMELEVEL;
	else if(StrToInt(edtPIOTimeLevel->Text) > MAX_PIOTIMELEVEL)
		SEND_PACKET.BODY.EtcData.Info.PIOTimeLevel = MAX_PIOTIMELEVEL;
	else
		SEND_PACKET.BODY.EtcData.Info.PIOTimeLevel = edtPIOTimeLevel->Text.ToInt();

	if(StrToInt(edtLookDownLevel->Text) < MIN_LOOKDOWNLEVEL)
		SEND_PACKET.BODY.EtcData.Info.LookDownLevel = MIN_LOOKDOWNLEVEL;
	else if(StrToInt(edtLookDownLevel->Text) > MAX_LOOKDOWNLEVEL)
		SEND_PACKET.BODY.EtcData.Info.LookDownLevel = MAX_LOOKDOWNLEVEL;
	else
		SEND_PACKET.BODY.EtcData.Info.LookDownLevel = edtLookDownLevel->Text.ToInt();

	// by zzang9un 2013.09.02 : �Ʒ� option�� ������ false ó��
	SEND_PACKET.BODY.EtcData.Info.EQPIODisable_TP = false;
	SEND_PACKET.BODY.EtcData.Info.Reserved1 = false;
	SEND_PACKET.BODY.EtcData.Info.Reserved2 = false;

	SEND_PACKET.BODY.EtcData.Info.AutoRecovery = false;// Check �߰� �ʿ�!!
	SEND_PACKET.BODY.EtcData.Info.RFReader = chkRFReader->Checked;
	SEND_PACKET.BODY.EtcData.Info.TransSound = chkTransSound->Checked;
	SEND_PACKET.BODY.EtcData.Info.Oscillation = chkOscillation->Checked;
	SEND_PACKET.BODY.EtcData.Info.HandDetectEQ = chkHandDetectEQ->Checked;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

/**
@brief   btnTeachingDataReq OnClick �̺�Ʈ �Լ�

         ���� Station ID�� Port Type�� Teaching Data�� ��û�Ѵ�.
@author  zzang9un
@date    2013.07.18
*/
void __fastcall TTeachingForm::btnTeachingDataReqClick(TObject *Sender)
{
    //gm_OHTStatus->m_OHTStatus.Data.CurStationID=10007;
#if 1
    // Step 1. Station ID üũ
    if(!gm_OHTStatus->m_OHTStatus.Data.CurStationID)
    {
        ERROR_MSGBOX("Station ID�� �����ϴ�.");
        return;
    }

    // Step 2. Port Type üũ
    if(cbbPortType->ItemIndex == -1)
    {
        ERROR_MSGBOX("Port Type ���� �����ϴ�.");
        return;
	}
#else
	ShowMessage("�������̹Ƿ� ������ ���� �ǳʶ�. �ݵ�� Ǯ�� �� ��");
#endif

	// Step 3. Teaching Data ��û

#if 0 	// �� �κ��� ���� �����ϴ� �ڵ���
	// by zzang9un 2013.07.19 : Teaching Data ��û
    uTP_CMD SndTPCmd;     // TP Command
    memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

	// by zzang9un 2013.07.19 : Port Type�� �����Ѵ�.
    // TEACHING_SAVE ����ü�� �״�� ����Ѵ�.
    SndTPCmd.TeachingSave.PortType = cbbPortType->ItemIndex + 1;

    TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
    m_CMDControl->MakeTPCmd2OHT(TPCMD_TEACHING_REQ, &SndTPCmd);
    MainForm->DataDisplay(COLOR_BLACK, "Request Teaching Data");
	delete m_CMDControl;
#endif

	// C������Ʈ ��Ź������ ����
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_TEACHING_INFO_REQ;

	SEND_PACKET.BODY.PortType = cbbPortType->ItemIndex + 1;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

/**
@brief   btnDefault OnClick �̺�Ʈ �Լ�

         Teaching Data ���� Default ���������� �ǵ�����.
@author  zzang9un
@date    2013.07.18
*/
void __fastcall TTeachingForm::btnDefaultClick(TObject *Sender)
{
    SetDefaultTeachingData();
}
//---------------------------------------------------------------------------

/**
@brief   btnDefault OnClick �̺�Ʈ �Լ�

         Teaching Data ���� Default ���������� �ǵ�����.
@author  zzang9un
@date    2013.07.18
*/
//void TTeachingForm::DisplayTeachingData(uTP_CMD *pTData)
void TTeachingForm::DisplayTeachingData(PKT_OHT2TP_TEACHING_INFO *pTData)
{

		SetDefaultTeachingData();

}
//---------------------------------------------------------------------------
/**
@brief   btnDefault OnClick �̺�Ʈ �Լ�

         Teaching Data ���� Default ���������� �ǵ�����.
@author  zzang9un
@date    2013.07.18
*/
//void TTeachingForm::DisplayTeachingData(uTP_CMD *pTData)
void TTeachingForm::DisplayAutoTeachingData(PKT_OHT2TP_TEACHING_INFO *pTData)
{
	String S ="";
	if(pTData->BODY.PortType == 0)
	{
		// Teaching Data�� ���� ���
		SetDefaultTeachingData();
	}
	else
	{

		S.sprintf(L"ID :%d, Port :%d, DrivingOffset : %d", pTData->BODY.Reserved1, pTData->BODY.PortType, pTData->BODY.Reserved2);
		memoAT->Lines->Add(S);

    }
}


void __fastcall TTeachingForm::tmrJogMoveTimer(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	tmrJogMove->Enabled = false;

	//���� �̵�
	if(bShiftMoveCommand)
	{
		switch(nShiftMoveDir)
		{
			case SHIFT_JOG_LEFT :
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_LEFT);
				MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_LEFT");
				break;

			case SHIFT_JOG_RIGHT :
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_RIGHT);
				MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_RIGHT");
				break;

			default :
				break;
		}
	}
	else if(bHoistMoveCommand)
	{
		switch(nHoistMoveDir)
		{
			case HOIST_JOG_DOWN :
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DOWN);
				MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_DOWN");
				break;

			case HOIST_JOG_UP :
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_UP);
				MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP");
				break;

			default :
				break;
		}
	}
    else if(bRotateMoveCommand)
    {
        switch(nRotateMoveDir)
        {
            case ROTATE_JOG_CW:
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CW);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CW");
                break;

            case ROTATE_JOG_CCW:
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_CCW);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_CCW");
                break;

            default :
                break;
        }
    }

	//�˻�
	//���콺�� �ȴ����� ���� ���
	if((GetKeyState(VK_LBUTTON) & 0x80) == 0)
	{
		// Jog ��� Timer���� Jog ����� �۽��ϰ� ������
		if(bHoistMoveCommand==true)
		{
			bHoistMoveCommand = false;
			nHoistMoveDir = HOIST_JOG_NONE;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_STOP);
			MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_STOP");
		}

		if(bShiftMoveCommand==true)
		{
			bShiftMoveCommand = false;
			nShiftMoveDir = SHIFT_JOG_NONE;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_STOP);
			MainForm->DataDisplay(COLOR_BLACK, "JOG_SHIFT_STOP");
		}

        if(bRotateMoveCommand==true)
        {
            bRotateMoveCommand = false;
            nRotateMoveDir = ROTATE_JOG_NONE;
    		m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_STOP);
    		MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_STOP");
        }

		tmrJogMove->Enabled = false;
	}
	else
	{
		tmrJogMove->Enabled = true;
	}

	delete m_CMDControl;
}
//---------------------------------------------------------------------------


void __fastcall TTeachingForm::btnHoistMoveUpMouseLeave(TObject *Sender)
{
	bHoistMoveCommand = false;
	nHoistMoveDir = HOIST_JOG_NONE;
}
//---------------------------------------------------------------------------



void __fastcall TTeachingForm::btnHoistMoveDownMouseLeave(TObject *Sender)
{
	bHoistMoveCommand = false;
	nHoistMoveDir = HOIST_JOG_NONE;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveRightMouseLeave(TObject *Sender)
{
    bShiftMoveCommand = false;
	nShiftMoveDir = SHIFT_JOG_NONE;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnShiftMoveLeftMouseLeave(TObject *Sender)
{
	bShiftMoveCommand = false;
	nShiftMoveDir = SHIFT_JOG_NONE;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnMotor4HomeClick(TObject *Sender)
{
   int nTmpDist = 0;
   String strTmpCMD;
   bool bHoistHome = false;


   bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;



    // puting 2012.12.03 ����cmd ����ü �ʱ�ȭ
    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	if((bHoistHome == ON) && (fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm)) <
        HOIST_ORIGIN_LIMIT))
    {
        if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Rotate() == NO_ERR))
        {
            TPCmd.CmdTP_Jog_TR.Distance = 0;

            if(nTmpDist >= 0)
            {
                TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
                strTmpCMD.sprintf(L"%d", nTmpDist);
                m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_ABS, &TPCmd);
                // m_CMDControl->MakeCommand2OHT(TPCMD_JOG_ROTATE_ABS, strTmpCMD);
                MainForm->DataDisplay(COLOR_BLACK, "JOG_ROTATE_ABS =" + strTmpCMD);
                delete m_CMDControl;
            }
        }
    }
    else
        MainForm->DataDisplay(COLOR_BLACK, "Rotate Zeroing..ERROR[check Hoist Origin]");

}
//---------------------------------------------------------------------------

void TTeachingForm::Display_Teaching_Result(PKT_TP2OHT_SIMPLE_COMMAND PKT)
{
	switch(PKT.BODY.iVar1)
	{
		case TP_DEBUG_PROGRESS:
			edtTeaching->Text = "����ƼĪ ������";
		break;

		case TP_DEBUG_ERR:
			edtTeaching->Text = "����ƼĪ�� ���� �߻�";
		break;

		case TP_DEBUG_ENDED:
			edtTeaching->Text = "����ƼĪ �Ϸ�";
		break;
    }
}

void __fastcall TTeachingForm::btnTeachStartClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// ������ ��� ����ü
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// ����ü �ʱ�ȭ
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	SEND_PACKET.TAG.Value = TagID::CMD_TP_DEBUG_COMMAND;
	SEND_PACKET.BODY.iVar1 = TP_DEBUG_START;	// Start Command
	SEND_PACKET.BODY.iVar2 = cboParam->ItemIndex;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnTeachStopClick(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// ������ ��� ����ü
	iPacketSize = sizeof(PKT_TP2OHT_SIMPLE_COMMAND);
	PKT_TP2OHT_SIMPLE_COMMAND	SEND_PACKET;

	// ����ü �ʱ�ȭ
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	SEND_PACKET.TAG.Value = TagID::CMD_TP_DEBUG_COMMAND;
	SEND_PACKET.BODY.iVar1 = TP_DEBUG_STOP;	// Start Command


	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::Button1Click(TObject *Sender)
{
	btnTeachStartClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::Button3Click(TObject *Sender)
{
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_AUTOTEACHING_FILE);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::Button4Click(TObject *Sender)
{
 	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_MAPMAKER_FILE);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::Button5Click(TObject *Sender)
{
//	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
//	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_AUTOTEACHING);
//	delete m_CMDControl;


	 // Step 2. Port Type üũ
	if(ComboBox2->ItemIndex < 0)
	{
		ERROR_MSGBOX("Port���� �����ϴ�.");
		return;
	}

			// C������Ʈ ��Ź������ ����
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING;

	SEND_PACKET.BODY.PortType = ComboBox2->ItemIndex + 1;
//	SEND_PACKET.BODY.Reserved1 = StrToInt(edtStationIDAT->Text);

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;

}
//---------------------------------------------------------------------------


void __fastcall TTeachingForm::Button2Click(TObject *Sender)
{

	 // Step 2. Port Type üũ
	if((ComboBox1->ItemIndex < 0) || (StrToInt(edtStationIDAT->Text) ==0))
	{
		ERROR_MSGBOX("Port or Staation ���� �����ϴ�.");
		return;
	}

		// C������Ʈ ��Ź������ ����
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING_READ_DRIVING;

	SEND_PACKET.BODY.PortType = ComboBox1->ItemIndex + 1;
	SEND_PACKET.BODY.Reserved1 = StrToInt(edtStationIDAT->Text);

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;


}
//---------------------------------------------------------------------------

void __fastcall TTeachingForm::btnNullTypeClick(TObject *Sender)
{
//	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
//	m_CMDControl->MakeTP_Quick_Cmd2OHT(TPCMD_AUTOTEACHING);
//	delete m_CMDControl;


	 // Step 2. Port Type üũ
	if(((ComboBox2->ItemIndex+1) != PORT_TYPE_STB_L) && ((ComboBox2->ItemIndex+1) != PORT_TYPE_STB_R))
	{
		ERROR_MSGBOX("STB Port�� �ƴմϴ�.");
		return;
	}

			// C������Ʈ ��Ź������ ����
	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING_NULL_TYPE;

	SEND_PACKET.BODY.PortType = ComboBox2->ItemIndex + 1;
//	SEND_PACKET.BODY.Reserved1 = StrToInt(edtStationIDAT->Text);

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------



