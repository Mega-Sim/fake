//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ShutterOpenUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TShutterOpenForm *ShutterOpenForm;
//---------------------------------------------------------------------------
__fastcall TShutterOpenForm::TShutterOpenForm(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TShutterOpenForm::FormShow(TObject *Sender)
{
   #if(LANGUAGE_MODE==MODE_KOREAN)
      ShutterOpenForm->Caption = "Shutter Open Ȯ��";
      GroupBox1->Caption = " üũ �׸� ";
      CheckBox1->Caption = "�ݼ�ü ����, �̻� ���� ���� Ȯ��";
      CheckBox2->Caption = "Hand Close ���� Ȯ��(����==ON)";
      CheckBox3->Caption = "Hand Gripper ���� ���� ����(2EA=ON)";
      CheckBox4->Caption = "Hand Alarm==OFF �� InPosition==ON ���� ";

   #else
      ShutterOpenForm->Caption = "Checklist for Open Shutter";
      GroupBox1->Caption = " CheckList ";
      CheckBox1->Caption = "FOSB Present, Please do a visual check a Problem.";
      CheckBox2->Caption = "Check that Hand Unit is Close State.";
      CheckBox3->Caption = "Check that the Hand Gripper Sensor is On.(2EA=ON).";
      CheckBox4->Caption = "Check that the Hand Alarm Input is Off and the Hand Inposition Input is On.";
   #endif

   CheckBox1->Checked=false;
   CheckBox2->Checked=false;
   CheckBox3->Checked=false;
   CheckBox4->Checked=false;
   btnOK->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TShutterOpenForm::btnOKClick(TObject *Sender)
{
    if(gm_OHTStatus->IOInput->uInput.Info.HAND_Close==ON)
    {
        TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
        m_CMDControl->MakeTPCmd2OHT(TPCMD_SHUTTER_OPEN);
        MainForm->DataDisplay(COLOR_BLACK,"Shutter Open");
        delete m_CMDControl;

        Close();
    }
}
//---------------------------------------------------------------------------
int TShutterOpenForm::Check_OKBtn(void)
{
   int nError=0;

   if((CheckBox1->Checked==true) &&
      (CheckBox2->Checked==true) &&
      (CheckBox3->Checked==true) &&
      (CheckBox4->Checked==true))
   {
      btnOK->Enabled=true;
   }
   else
   {
      btnOK->Enabled=false;
   }

   return nError;
}
//---------------------------------------------------------------------------
void __fastcall TShutterOpenForm::btnCancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TShutterOpenForm::CheckBox1Click(TObject *Sender)
{
   Check_OKBtn();
}
//---------------------------------------------------------------------------
