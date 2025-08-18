//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "JigSlideUnit.h"
#include "OHTMain.h"
#include "TaskControl.h"
#include "MyTools.h"
#include "Utility.h"
#include "PUtily.h"
#include "JigThread.h"
#include <math.h>

#define COLOR_AQUA          clAqua
#define COLOR_BLACK         clBlack
#define COLOR_BLUE          clBlue
#define COLOR_CREAM         clCream
#define COLOR_DARKGRAY      clDkGray
#define COLOR_FUCHSIA       clFuchsia
#define COLOR_GRAY          clGray
#define COLOR_GREEN         clGreen
#define COLOR_LIME          clLime
#define COLOR_LIGHTGRAY     clLtGray
#define COLOR_MAROON        clMaroon
#define COLOR_MEDIUMGRAY    clMedGray
#define COLOR_MINTGREEN     clMoneyGreen
#define COLOR_NAVYBLUE      clNavy
#define COLOR_OLIVE         clOlive
#define COLOR_PURPLE        clPurple
#define COLOR_RED           clRed
#define COLOR_SILVER        clSilver
#define COLOR_SKYBLUE       clSkyBlue
#define COLOR_WHITE         clWhite
#define COLOR_YELLOW        clYellow

#define LOG_JIG(...)   WRITE_LOG_UNIT(pLogJig, __VA_ARGS__)

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TSlideUnit *SlideUnit;
JigThread *JigCtrl = NULL;
TaskControl *gSeq = NULL;

//---------------------------------------------------------------------------
__fastcall TSlideUnit::TSlideUnit(TComponent* Owner)
	: TForm(Owner)
{
	pLogJig = getLogUnit("D:\\log\\jig.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	gSeq = NULL;
	JigCtrl = NULL;
	Init();

	//Slide�� ����
	gSeq->m_pHWCommon->Set_Jig(3, true); //Slide ��

	JigCtrl->m_StepJig_Slide.bSignal = true;                    //�ñ׳�

	//Task Control, Trans Control �Ⱦ��� �������� ���� timy
//	gSeq->bSlideJig = true;     //���� Setting
//	OHTMainForm->GetTransControl()->bSlideJig = true;           //Trans control  Hoist ���� On
//	OHTMainForm->GetTransControl()->nSlideTeaching = 0;         //Teaching
	m_pCmdManager = OHTMainForm->GetCmdManager();               //Main Reset�� ���� ���

		//IO Setting
	//���� ����ó�� I/O Mask : 44, 46, 47, 48, 64
	MASK_INPUT  *pMaskOfInput   = NULL;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(44-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(46-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(47-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(48-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(64-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	//���� input ����ó�� : 21~28


	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(21-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(22-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(23-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(24-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(25-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(26-1);
	pMaskOfInput->Value = false;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(27-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

	pMaskOfInput = OHTMainForm->m_MaskOfHW->getMaskOfInput(28-1);
	pMaskOfInput->Value = true;
	pMaskOfInput->bUse  = true;

//	gSeq->m_pHWSet->m_pShutter->bSlideJig = true;    // Shutter���� Shutter Ready ���·� �������
	edt_RelMoveVal->Text  = "0";

}

void TSlideUnit::Init()
{
	gSeq = OHTMainForm->GetTaskControl();
	JigCtrl = OHTMainForm->GetJigCtrl();
}


void __fastcall TSlideUnit::FormClose(TObject *Sender, TCloseAction &Action)
{
	//Param ����
	SaveParam();
}

void TSlideUnit::UpdateParam(bool bUI_toParam) //���� ����
{
	// UI --> �޸� Param���� ������Ʈ
	if(bUI_toParam)
	{
		JigCtrl->m_JigParam.dSlide_SlideTeachingPos = StrToFloat(edtSlide_moveDist->Text.c_str());
		JigCtrl->m_JigParam.nSlide_Interval_ms = StrToInt(edtSlide_Interval->Text.c_str());
		JigCtrl->m_JigParam.nSlide_CycleCnt = StrToInt(edtSlide_Count->Text.c_str());
		JigCtrl->m_JigParam.nSlide_CurrentCnt = StrToInt(edt_Slide_RealCount->Text.c_str());
		SaveParam();
	}
	// �޸� Param -> UI�� ������Ʈ
	else
	{
		edtSlide_moveDist->Text = edtSlide_moveDist->Text.sprintf(L"%g", JigCtrl->m_JigParam.dSlide_SlideTeachingPos);
		edtSlide_Interval->Text = edtSlide_Interval->Text.sprintf(L"%d", JigCtrl->m_JigParam.nSlide_Interval_ms);
		edtSlide_Count->Text = edtSlide_Count->Text.sprintf(L"%d", JigCtrl->m_JigParam.nSlide_CycleCnt);
		edt_Slide_RealCount->Text = edt_Slide_RealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nSlide_CurrentCnt);
	}

}

void TSlideUnit::SaveParam()
{
	LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->WriteDouble("Setting", "Slide_SlideTeachingPos", JigCtrl->m_JigParam.dSlide_SlideTeachingPos);
	pIniFile->WriteInteger("Setting", "Slide_Interval_ms", JigCtrl->m_JigParam.nSlide_Interval_ms);
	pIniFile->WriteInteger("Setting", "Slide_CycleCnt", JigCtrl->m_JigParam.nSlide_CycleCnt);
	pIniFile->WriteInteger("Setting", "Slide_CurrentCnt", JigCtrl->m_JigParam.nSlide_CurrentCnt);

	delete pIniFile;
}

void TSlideUnit::LoadParam()
{
	LIniFile *pIniFile = new LIniFile("DataFiles\\JigTestParam.ini");

	pIniFile->ReadDouble("Setting", "Slide_SlideTeachingPos", 0, &JigCtrl->m_JigParam.dSlide_SlideTeachingPos);
	pIniFile->ReadInteger("Setting", "Slide_Interval_ms", 0, &JigCtrl->m_JigParam.nSlide_Interval_ms);
	pIniFile->ReadInteger("Setting", "Slide_CycleCnt", 0, &JigCtrl->m_JigParam.nSlide_CycleCnt);
	pIniFile->ReadInteger("Setting", "Slide_CurrentCnt", 0,	&JigCtrl->m_JigParam.nSlide_CurrentCnt);

	delete pIniFile;

}

// Slide �׽�Ʈ ���� ��ư
void __fastcall TSlideUnit::btnSlideAxis_RunClick(TObject *Sender)
{

	LoadParam();

	if(JigCtrl->m_StepJig_Slide.Step != 0)
	{
		Application->MessageBox(L"Slide Step isn't Initialized...", L"Error", MB_OK + MB_ICONSTOP);
		return;
	}

	int value = (int)gSeq->m_pHWSet->m_pShiftAxis->GetCurrentPosition();     //���� ��ġ��


	if(value < 1 && value > -1)
	{
		JigCtrl->m_StepJig_Slide.Step = 100;
		GetLocalTime(&CurTime);
		edtStartDate->Text = edtStartDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
	}
	else
	{
		edt_SlideErrMsg->Text = edt_SlideErrMsg->Text.sprintf(L"Slide isn't in home");
	}

}



// Ÿ�̸�. �׻� ��������
void __fastcall TSlideUnit::tmr1Timer(TObject *Sender)
{
	static CStepTimer tmrEnable;

	// ó������ �� �ʵ����� Ÿ�̸� �����Ű�� ����
	if(tmrEnable.OnDelay(true, 1000) == false)
		return;

	if(gSeq == NULL)
		return;

//
//	// ���α׷��� �� ����
//   progress_Slide->Position = RANGE_VAL(0, 100, ((double)JigCtrl->m_JigParam.nSlide_CurrentCnt / JigCtrl->m_JigParam.nSlide_CycleCnt) * 100);

   edt_SlideStepMsg->Text = edt_SlideStepMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Slide.Step, JigCtrl->m_StepJig_Slide.StepComment);
   pnl_SlideServoStat->Color = gSeq->m_pHWSet->m_pShiftAxis->IsAMPEnable() ? COLOR_LIME : COLOR_FUCHSIA;

   pnl_SlideCurPos->Caption = (int)(((int)(gSeq->m_pHWSet->m_pShiftAxis->GetCurrentPosition()*1000))/1000.0);
   pnl_SlideVel->Caption = ((int)(gSeq->m_pHWSet->m_pShiftAxis->GetCurrentVelocity()*1000))/1000.0;

   pnl_SlideBusy->Color = gSeq->m_pHWSet->m_pShiftAxis->IsRunning() ? COLOR_FUCHSIA : COLOR_LIME;
   pnl_SlideReady->Color = JigCtrl->m_StepJig_Slide.bSignal ? COLOR_LIME : COLOR_FUCHSIA;

   //���� �޽��� �����ֱ�
   static AnsiString prevErrMsg_Slide = "";

   if(prevErrMsg_Slide != JigCtrl->m_StepJig_Slide.ErrMsg)
   {
		edt_SlideErrMsg->Text = edt_SlideErrMsg->Text.sprintf(L"%d: %s", JigCtrl->m_StepJig_Slide.nErrorCode, JigCtrl->m_StepJig_Slide.ErrMsg);
		prevErrMsg_Slide = JigCtrl->m_StepJig_Slide.ErrMsg;
   }

	edt_Slide_RealCount->Text = edt_Slide_RealCount->Text.sprintf(L"%d", JigCtrl->m_JigParam.nSlide_CurrentCnt);

}


void __fastcall TSlideUnit::btn_SlideResetClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.bSignal == false)
	{
		gSeq->m_pHWSet->m_pShiftAxis->Reset();
		JigCtrl->m_StepJig_Slide.bSignal = true;
		JigCtrl->m_StepJig_Slide.Step = 0;
		JigCtrl->m_StepJig_Slide.StepComment = "Thread Reset";
		GetLocalTime(&CurTime);
		edtStartDate->Text ="0";
		edtEndDate->Text ="0";
	}

}


void __fastcall TSlideUnit::Slide_LoadParamClick(TObject *Sender)
{
	LoadParam();
	UpdateParam(false);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TSlideUnit::Slide_EStopClick(TObject *Sender)
{
//	SaveParam();

	gSeq->m_pHWSet->m_pShiftAxis->SetAMPEnable(false);

	if(JigCtrl->m_StepJig_Slide.Step>=100 &&
	JigCtrl->m_StepJig_Slide.Step<200)
	{
		GetLocalTime(&CurTime);
		edtEndDate->Text = edtEndDate->Text.sprintf(L"%d/%d/%d, %d:%d:%d",CurTime.wYear,CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);

	}

	JigCtrl->m_StepJig_Slide.Step = 200;
	JigCtrl->m_StepJig_Slide.StepComment = "E-Stop";
	JigCtrl->m_StepJig_Slide.bSignal = false;

}
//---------------------------------------------------------------------------


void __fastcall TSlideUnit::btn_Servo_OnClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.bSignal == true)
		JigCtrl->m_StepJig_Slide.Step = 1;
}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::btn_Servo_OffClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.bSignal == true)
		JigCtrl->m_StepJig_Slide.Step = 2;
}
//---------------------------------------------------------------------------



void __fastcall TSlideUnit::btn_SlideHomeClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.Step == 0 && JigCtrl->m_StepJig_Slide.bSignal == true)
		JigCtrl->m_StepJig_Slide.Step = 3;
}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::btn_SlideSetHomeClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.Step == 0 && JigCtrl->m_StepJig_Slide.bSignal == true)
		JigCtrl->m_StepJig_Slide.Step = 4;
}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::btn_SlideJog_PClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.Step == 0 && JigCtrl->m_StepJig_Slide.bSignal == true)
	{
		int nVal = edt_RelMoveVal->Text.ToIntDef(1);
		JigCtrl->nJogVal = nVal;
		JigCtrl->m_StepJig_Slide.Step = 5;
	}

}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::btn_SlideJogMClick(TObject *Sender)
{
	if(JigCtrl->m_StepJig_Slide.Step == 0 && JigCtrl->m_StepJig_Slide.bSignal == true)
	{
		int nVal = edt_RelMoveVal->Text.ToIntDef(1);
		JigCtrl->nJogVal = nVal;
		JigCtrl->m_StepJig_Slide.Step = 6;
	}

}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::btn_LongrunClick(TObject *Sender)
{
	if(chkSlidelongrun->Checked == true)
		JigCtrl->m_StepJig_Slide.bLongrun = true;
	else
		JigCtrl->m_StepJig_Slide.bLongrun = false;
}
//---------------------------------------------------------------------------


void __fastcall TSlideUnit::btnSlide_Main_ResetClick(TObject *Sender)
{
		m_pCmdManager->SendQuickCmd2Main(QUICK_CMD_IDRESET);
}
//---------------------------------------------------------------------------

void __fastcall TSlideUnit::Slide_SaveParamClick(TObject *Sender)
{
      SaveParam();
}
//---------------------------------------------------------------------------

