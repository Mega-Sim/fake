//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdlib.h>
#pragma hdrstop

#include "UserSelectUnit.h"
#include "MainUnit.h"
#include "OHTInfoUnit.h"
#include "Define.h"

#include "ConnectUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUserSelectForm *UserSelectForm;
//---------------------------------------------------------------------------
__fastcall TUserSelectForm::TUserSelectForm(TComponent* Owner)
   : TForm(Owner)
{
   g_nPWKey=0;
}
//---------------------------------------------------------------------------
/**
@brief   Cancel��ư Ŭ���� �߻��ϴ� �̺�Ʈ �Լ�
@author  zzang9un
@date    2012.12.01
*/
void __fastcall TUserSelectForm::btnCancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TUserSelectForm::FormActivate(TObject *Sender)
{
   UserSelectForm->Left = MainForm->Left+50;
   UserSelectForm->Top = MainForm->Top+100;
}
//---------------------------------------------------------------------------
/**
@brief   PassWord Check �Լ�
@author  puting
@date    2012.12.14
*/
int TUserSelectForm::Is_PwOK(int nUserPw, String strUserPw)
{
   static int nSaveKeyValue[50]=
   {
      8086, 2254, 7542, 4875, 6658, 8663, 5011, 4947, 4325, 9672,
      3636, 3458, 2408, 8446, 1211, 7689, 1585, 9853, 7832, 6452,
      3742, 7645, 1234, 6167, 5457, 2375, 8792, 2928, 7730, 1879,
      2631, 5478, 1956, 4475, 6645, 3458, 5897, 3880, 5580, 8667,
      1782, 5620, 3641, 4279, 1866, 1554, 2223, 8450, 5704, 1642
   };

   if((nUserPw==nSaveKeyValue[g_nPWKey-1]) ||
      (nUserPw==123456) ||
      (nUserPw==1342210) ||
      (nUserPw==1674623) ||
      (nUserPw==1234414) ||
      (nUserPw==1533314) ||
      (nUserPw==1007)    ||
      (nUserPw==1110612) ||
      (nUserPw==1469539) ||
      (nUserPw==1696426) ||
      (nUserPw==1348217) ||
      (nUserPw==1221114) ||
      (nUserPw==1792617) ||
      (nUserPw==1821218) ||
      (nUserPw==2580)    ||
      (nUserPw==0510051))
   {
      gm_OHTStatus->nProgram_Mode=MODE_SETUP;
      #if(LANGUAGE_MODE==MODE_KOREAN)
         MainForm->panUser->Caption = "������";
      #else
         MainForm->panUser->Caption = "Developer";
      #endif
      Close();
   }

   return 0;
}
//---------------------------------------------------------------------------
/**
@brief   OK��ư Ŭ���� �����ϴ� �̺�Ʈ �Լ�
@author  zzang9un
@date    2012.12.01
*/
void __fastcall TUserSelectForm::btnSelectClick(TObject *Sender)
{
   String strUserPw=edtPwd->Text;
   int nUserPw = StrToIntDef(strUserPw,0);

   if(rgUserSelect->ItemIndex==0)
   {
	  gm_OHTStatus->nProgram_Mode = MODE_WORKER;
      #if(LANGUAGE_MODE==MODE_KOREAN)
		 MainForm->panUser->Caption = "���";
      #else
		 MainForm->panUser->Caption = "Worker";
      #endif
      Close();
   }
//   else if(rgUserSelect->ItemIndex==1)
//   {
//		gm_OHTStatus->nProgram_Mode=MODE_OPERATOR;
//	  #if(LANGUAGE_MODE==MODE_KOREAN)
//		 MainForm->panUser->Caption = "�ٹ���";
//	  #else
//		 MainForm->panUser->Caption = "Operator";
//	  #endif
//	  Close();
//   }
   else
   {
      Is_PwOK(nUserPw, strUserPw);
   }

   if(gm_OHTStatus->nProgram_Mode == MODE_OPERATOR)
   {
      ConnectForm->ConnectIP->Enabled = false;
      ConnectForm->btnConnectEdit->Enabled=false;
   }
   else  //���, ������ ����
   {
      ConnectForm->ConnectIP->Enabled = true;
      ConnectForm->btnConnectEdit->Enabled=true;
   }
}
//---------------------------------------------------------------------------
/**
@brief   Editâ�� Enter Key �Է½� ó���Ǵ� �Լ�
@author  zzang9un
@date    2012.12.01
*/
void __fastcall TUserSelectForm::edtPwdKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
   String strUserPw=edtPwd->Text;
   int nUserPw = StrToIntDef(strUserPw,0);

   if(Key==VK_RETURN)
   {
      Is_PwOK(nUserPw, strUserPw);
   }
}
//---------------------------------------------------------------------------
/**
@brief   RadioGroup�� ������/�ٹ��ڿ� ���� Editâ ������� ���� �Լ�
@author  zzang9un
@date    2012.12.01
*/
void __fastcall TUserSelectForm::rgUserSelectClick(TObject *Sender)
{
   if(rgUserSelect->ItemIndex==0)
   {
      edtPwd->Enabled=false;
      edtPwd->Text="";
   }
   else
   {
      edtPwd->Enabled=true;
   }
}
//---------------------------------------------------------------------------

void __fastcall TUserSelectForm::FormShow(TObject *Sender)
{
    // by zzang9un 2013.03.04 : ������ ���� ���� ���� �ӽ÷� �н����� ���� ����
    rgUserSelect->ItemIndex = 0;
    edtPwd->Text = "";           //2022.05.13 sseung ��� ��带 Default�� ����Ͽ� �н����� �⺻�Է°� ����

#if(LANGUAGE_MODE==MODE_KOREAN)
    rgUserSelect->Caption = "�����";
	rgUserSelect->Items->Strings[0] = "���";
    rgUserSelect->Items->Strings[1] = "������";
#else
    rgUserSelect->Caption = "User";
	rgUserSelect->Items->Strings[0] = "Worker";
    rgUserSelect->Items->Strings[1] = "Developer";
#endif
}
//---------------------------------------------------------------------------
int TUserSelectForm::Get_PwKey(void)
{
   int nKeyValue;
   randomize();
   nKeyValue = random(51); //0~50
   if(nKeyValue==0) nKeyValue+=1;
   g_nPWKey=nKeyValue;
   return nKeyValue;
}
//---------------------------------------------------------------------------




