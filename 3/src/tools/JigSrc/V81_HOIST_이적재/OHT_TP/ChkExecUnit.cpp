//---------------------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#pragma hdrstop

#include "ChkExecUnit.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "WarnningUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Def_TP.h"

TCHKOHT TOHTCheck;
//---------------------------------------------------------------------------
TCHKOHT::TCHKOHT()
{
}
//---------------------------------------------------------------------------
TCHKOHT::~TCHKOHT()
{
}
//---------------------------------------------------------------------------

/**
@brief   TP ����� ���� ���������� üũ�ϴ� �Լ�
@date    2013.01.22
@param   nTmpPID : Port ID
@return  
@note    
*/
int TCHKOHT::Is_Executable(void)
{
   int nError = 0;

   if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
   {
      Warnning_Display(COLOR_RED, WARNNING_AUTO);
      nError=1;
   }
   else if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_LOADING)
   {
      Warnning_Display(COLOR_RED, WARNNING_LOADING);
      nError=1;
   }
   else if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_UNLOADING)
   {
      Warnning_Display(COLOR_RED, WARNNING_UNLOADING);
      nError=1;
   }
   /* by zzang9un 2013.02.05 : MoveTPoing�� Status���� ����
   else if(gm_OHTStatus->Status == "V")
   {
      Warnning_Display(COLOR_RED, WARNNING_MOVETPOINT);
      nError=1;
   }
   */
   else if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_GOING)
   {
      Warnning_Display(COLOR_RED, WARNNING_GOING);
      nError=1;
   }
   else if(gm_OHTStatus->Motor_Driving_F->nAxis_Running == ON)
   {
      Warnning_Display(COLOR_RED, WARNNING_1RUNNING);
      nError=1;
   }
   else if(gm_OHTStatus->Motor_Driving_R->nAxis_Running==ON)
   {
      Warnning_Display(COLOR_RED, WARNNING_1RUNNING);
      nError=1;
   }
   else if(gm_OHTStatus->Motor_Hoist->nAxis_Running==ON)
   {
      Warnning_Display(COLOR_RED, WARNNING_2RUNNING);
      nError=1;
   }
   else if(gm_OHTStatus->Motor_Shift->nAxis_Running==ON)
   {
      Warnning_Display(COLOR_RED, WARNNING_3RUNNING);
      nError=1;
   }
   else if(gm_OHTStatus->Motor_Rotate->nAxis_Running==ON)
   {
      Warnning_Display(COLOR_RED, WARNNING_4RUNNING);
      nError=1;
   }
   return nError;
}
//---------------------------------------------------------------------------
int TCHKOHT::Is_Executable(char* ExMessage, ...)
{
   int nError=0;

   char szDebugMsg[256];
   int ncnt, i;
   String strExclude, strWanMsg;

   va_list va;
   va_start(va, ExMessage);
   ncnt=vsprintf(szDebugMsg, ExMessage, (va_list)va);
   va_end(va);


   if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode==MODE_AUTO)
   {
      nError=1;
      strWanMsg=WARNNING_AUTO;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_AUTO)
         {
            nError=0;
            break;
         }
      }
   }

   if((nError==0)&&(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_LOADING))
   {
      nError=1;
      strWanMsg=WARNNING_LOADING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_LOAD)
         {
            nError=0;
            break;
         }
      }
   }

   if((nError==0)&&(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_UNLOADING))
   {
      nError=1;
      strWanMsg=WARNNING_UNLOADING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_UNLOAD)
         {
            nError=0;
            break;
         }
      }
   }

   /* by zzang9un 2013.02.05 : MoveTPoint�� Status���� ����
   if((nError==0)&&(gm_OHTStatus->Status == "V"))
   {
      nError=1;
      strWanMsg=WARNNING_MOVETPOINT;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_MOVETPOINT)
         {
            nError=0;
            break;
         }
      }
   }
   */

   if((nError==0)&&(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_GOING))
   {
      nError=1;
      strWanMsg=WARNNING_GOING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_GOING)
         {
            nError=0;
            break;
         }
      }
   }


   if((nError==0)&&((gm_OHTStatus->Motor_Driving_F->nAxis_Running==ON)||(gm_OHTStatus->Motor_Driving_R->nAxis_Running==ON)))
   {
      nError=1;
      strWanMsg=WARNNING_1RUNNING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_DRIVING_RUNNING)
         {
            nError=0;
            break;
         }
      }
   }

   if((nError==0)&&(gm_OHTStatus->Motor_Hoist->nAxis_Running==ON))
   {
      nError=1;
      strWanMsg=WARNNING_2RUNNING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_HOIST_RUNNING)
         {
            nError=0;
            break;
         }
      }
   }

   if((nError==0)&&(gm_OHTStatus->Motor_Shift->nAxis_Running==ON))
   {
      nError=1;
      strWanMsg=WARNNING_3RUNNING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_SHIFT_RUNNING)
         {
            nError=0;
            break;
         }
      }
   }

   if((nError==0)&&(gm_OHTStatus->Motor_Rotate->nAxis_Running==ON))
   {
      nError=1;
      strWanMsg=WARNNING_4RUNNING;
      for(i=0; i<ncnt; i++)
      {
         strExclude=szDebugMsg[i];

         if(strExclude==EXCLUDE_ROTATE_RUNNING)
         {
            nError=0;
            break;
         }
      }
   }


   if(nError) Warnning_Display(COLOR_RED, strWanMsg);

   return nError;
}
//---------------------------------------------------------------------------
void TCHKOHT::Warnning_Display(TColor tWColor, String strData)
{
   MainForm->DataDisplay(tWColor, strData);
   WarnningForm->DisplayMemo->Clear();
   WarnningForm->DisplayMemo->Lines->Add(strData);
   WarnningForm->Show();
}
//---------------------------------------------------------------------------

/**
@brief   Hoist�� Home�� �̵���Ű�� �Լ�
@author  zzang9un
@date    2013.02.19
@return  0�� ������
*/
int TCHKOHT::Axis_Hoist_HOME()
{

	// by zzang9un 2013.02.19 : Jog ����ü �̿�
	uTP_CMD TPCmd;
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

	bool bHandPush = false;


	bHandPush = gm_OHTStatus->IOInput->uInputCommon.HAND_Push;


	if(bHandPush)
	{
		// Hand Push ������ On�� ���

		if(Application->MessageBox(L"Push Sensor �� �����Ǿ����ϴ�.\nFOUP�� ���ư� ���� �ʰ� �����Դϱ�?", L"Confirm", MB_OKCANCEL) == IDOK)
		{
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

			// Hand Push ������ On�� ��� �̵� �Ÿ�
			TPCmd.CmdTP_Jog_TR.Distance = HOIST_HOME_CHECKDIST_FOR_HANDPUSHON;

			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_UP_INC, &TPCmd);

			MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP_INC:" + String(HOIST_HOME_CHECKDIST_FOR_HANDPUSHON));
			delete m_CMDControl;
		}
	}
	else
	{
		// by zzang9un 2013.02.19 : Hand Push ���� off�� ��� �߰� Ȯ���ϵ��� ����
		if(Is_Executable()==NO_ERR)
		{
			if(gm_OHTStatus->Motor_Hoist->dPosition_mm > HOIST_HOME_CHECKDIST_FOR_HANDPUSHOFF*2)
			{
				// Hoist�� 30mm �̻� �̵��� ��츸 üũ�Ѵ�.
				String strMsg = NULL;
				strMsg.sprintf(L"Hoist�� Home �������� %dmm �̵��մϴ�.\n�������� ���󿩺θ� Ȯ���ϼ���", HOIST_HOME_CHECKDIST_FOR_HANDPUSHOFF);

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OKCANCEL) == IDOK )
				{
					// Ȯ���� ���� ��� 15mm�� �̵��Ѵ�.
					TCMDCONTROL* m_CMDControl = new TCMDCONTROL;

					// Hand Push ������ On�� ��� �̵� �Ÿ�
					TPCmd.CmdTP_Jog_TR.Distance = HOIST_HOME_CHECKDIST_FOR_HANDPUSHOFF;

					m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_UP_INC, &TPCmd);

					MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_UP_INC:" + String(HOIST_HOME_CHECKDIST_FOR_HANDPUSHOFF));
					delete m_CMDControl;
				}
				else
					return 0;   // OK�� ������ ���� ��� ���ϵ�

				if(Application->MessageBox(L"15mm �̵��߽��ϴ�.\nȮ���� ������ Home���� �̵��˴ϴ�.", L"Confirm", MB_OKCANCEL) == IDOK )
				{
					if(Is_MoveAxis_Hoist()==NO_ERR)
					{

						if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85)
						{
							Axis_Hoist_HOME_With_Smalladd();
						}
						else
						{
							TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
							m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_HOME);
							MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_HOME");
							delete m_CMDControl;
						}

						/*
						TCMDCONTROL* m_CMDControl = new TCMDCONTROL;
						m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_HOME);
						MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_HOME");
						delete m_CMDControl;
						*/
					}
				}
				else
					return 0;   // OK�� ������ ���� ��� ���ϵ�
			}
			else
			{
				// Hoist�� ���� ��ġ�� 30mm ������ ���� Ȯ�� ���� �ٷ� Home ��� ����
				if(Is_MoveAxis_Hoist()==NO_ERR)
				{

					if(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_85)
					{
						Axis_Hoist_HOME_With_Smalladd();
					}
					else
					{
						TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
						m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_HOME);
						MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_HOME");
						delete m_CMDControl;
					}
					/*
					TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
					m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_HOME);
					MainForm->DataDisplay(COLOR_BLACK, "JOG_HOIST_HOME");
					delete m_CMDControl;
					*/
				}
			}
		}
	}

	return 0;
}

int TCHKOHT::Axis_Hoist_HOME_With_Smalladd()
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

	SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_HOIST_HOME;

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

	return 0;
}
//---------------------------------------------------------------------------
int TCHKOHT::Axis_Shift_HOME(void)
{
   //if(Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING)==NO_ERR)
   if(Is_Executable()==NO_ERR)
   {
      if(Is_MoveAxis_Shift(true)==NO_ERR)
      {
		 TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
         m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_HOME);
         MainForm->DataDisplay(COLOR_BLACK,"JOG_SHIFT_HOME");
         delete m_CMDControl;
      }
   }
   return 0;
}
//---------------------------------------------------------------------------
int TCHKOHT::Axis_Rotate_HOME(void)
{

   if(Is_Executable(EXCLUDE_HOIST_RUNNING EXCLUDE_SHIFT_RUNNING EXCLUDE_ROTATE_RUNNING)==NO_ERR)
   {
      if(Is_MoveAxis_Rotate()==NO_ERR)
      {
         TCMDCONTROL* m_CMDControl=new TCMDCONTROL;
         m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_ROTATE_HOME);
         MainForm->DataDisplay(COLOR_BLACK,"JOG_ROTATE_HOME");
         delete m_CMDControl;
      }
   }
   return 0;
}
//---------------------------------------------------------------------------
int TCHKOHT::Check_HandLimitSensor(void)
{
	int nError=0;

	bool bHandClose = false;
	bool bHandOpen = false;


	bHandOpen = gm_OHTStatus->IOInput->uInputCommon.HAND_Open;
	bHandClose = gm_OHTStatus->IOInput->uInputCommon.HAND_Close;



   if(gm_OHTStatus->nProgram_Mode==MODE_OPERATOR)//�ٹ���
   {
	  if((bHandOpen==ON)||(bHandClose==ON))
		 nError=0;
      else
	  {
         nError=1;
         Warnning_Display(COLOR_RED, "[ERROR] Hand Limit Sensor NOT ON..");
      }
   }

   return nError;
}
//---------------------------------------------------------------------------
int TCHKOHT::Is_MoveAxis_Hoist(void)
{
   int nError=0;
   bool bShutterFOpen = false;
   bool bShutterROpen = false;
   bool bOscillation = false;

   nError = Check_HandLimitSensor();
   if(nError) return nError;


		bShutterFOpen = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open;
		bShutterROpen = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open;
		bOscillation = gm_OHTStatus->IOInput->uInputCommon.OSCILLATION;


	   if(gm_OHTStatus->CheckOnOff->bUse_ShutterControl==ON)
	   {
		  nError=0;
//		  if((bShutterFOpen==ON)&&(bShutterROpen==ON))
//		  {
//			 nError=0;
//		  }
//		  else
//		  {
//			 nError=1;
//			 Warnning_Display(COLOR_RED, "Shutter -> Close...Hoist can't Up...");
//		  }
	   }
	   else if(gm_OHTStatus->CheckOnOff->bUse_OscillationSen==ON)
	   {
	   		nError=0;
//		  if(bOscillation==ON)
//		  {
//			 nError=0;
//		  }
//		  else
//		  {
//			 nError=1;
//			 Warnning_Display(COLOR_RED, "Oscillation Sensor is Off...Hoist can't Up...");
//		  }
	   }
	   else
	   {
		  nError=0;
	   }

   return nError;
}
//---------------------------------------------------------------------------
int TCHKOHT::Is_MoveAxis_Shift(bool bOrigin)
{
   int nError=0;
   bool bShutterFClose = false;
   bool bShutterRClose = false;
   bool bHoistHome = false;

   nError = Check_HandLimitSensor();
   if(nError) return nError;


   bShutterFClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close;
   bShutterRClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close;
   bHoistHome = gm_OHTStatus->IOInput->uInputCommon.HOIST_Home;


   if(bOrigin == true)
   {
	  if((bHoistHome==ON)&&(fabs(StrToFloat(gm_OHTStatus->Motor_Hoist->dPosition_mm))<HOIST_ORIGIN_LIMIT))
      {
		 if((bShutterFClose==ON)||(bShutterRClose==ON))
		 {
            nError=1;
            Warnning_Display(COLOR_RED, "Foup[Exist], Shutter[Close] -> Shift can't Move...");
         }
         else
            nError=0;
      }
      else
      {
         Warnning_Display(COLOR_RED, "Hoist Not Origin....");
         nError=1;
      }
   }
   else
   {
	  if((bShutterFClose==ON)||(bShutterRClose==ON))
      {
         nError=1;
         Warnning_Display(COLOR_RED, "Foup[Exist], Shutter[Close] -> Shift can't Move...");
      }
      else
         nError=0;
   }

   return nError;
}
//---------------------------------------------------------------------------
int TCHKOHT::Is_MoveAxis_Rotate(void)
{
   int nError=0;
   bool bHandRGrip = false;
   bool bHandLGrip = false;
   bool bShutterFClose = false;
   bool bShutterRClose = false;

   nError = Check_HandLimitSensor();

   if(nError) return nError;



   bHandRGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip;
   bHandLGrip = gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip;
   bShutterFClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close;
   bShutterRClose = gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close;



   // 2016-12-01, Add by Labeler
   // Rotate Origin �ϴ� �Ⱦ�, �ӽ� ��ġ
   /*
   if(gm_OHTStatus->Motor_Rotate->bIs_Origin==true)
   {
	  if((bHandRGrip==ON)||(bHandLGrip==ON))
      {
         if(gm_OHTStatus->CheckOnOff->bUse_ShutterControl==ON)
         {
			if((bShutterFClose==ON)||(bShutterRClose==ON))
			{
			   nError=1;
               Warnning_Display(COLOR_RED, "Foup[Exist], Shutter[Close] -> Rotate can't Move...");
            }
			else
               nError=0;
         }
         else
            nError=0;
      }
      else
         nError=0;
   }
   else
   {
      nError=1;
      Warnning_Display(COLOR_RED, "Rotate Not Origin....");
   }
   */

  if((bHandRGrip==ON)||(bHandLGrip==ON))
  {
     if(gm_OHTStatus->CheckOnOff->bUse_ShutterControl==ON)
     {
        if((bShutterFClose==ON)||(bShutterRClose==ON))
        {
           nError=1;
           Warnning_Display(COLOR_RED, "Foup[Exist], Shutter[Close] -> Rotate can't Move...");
        }
        else
           nError=0;
     }
     else
        nError=0;
  }


   return nError;
}



#pragma package(smart_init)
