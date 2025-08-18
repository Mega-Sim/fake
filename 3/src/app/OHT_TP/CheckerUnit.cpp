//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckerUnit.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "OHTInfoUnit.h"
#include "ChkExecUnit.h"
#include "SensorSettingUnit.h"
#include "CidCheckerUnit.h"
//#include "LogFileUnit.h"

TCHKOHT TechOHTCheck;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define Log_Steering		0
#define Log_ShiftHome		1
#define Log_HoistHome		2

#define Log_Step_Init			100
#define Log_Step_Check			101
#define Log_Step_Done			102

#define CheckTime_30s		30*1000
#define	CheckTime_1m		1*60*1000
#define CheckTime_5m		5*60*1000

UnicodeString strTitle[5] =
{
	// 1
	L"Steering Front Right",
	L"Steering Rear Right",
	L"Steering Front Left",
	L"Steering Rear Left",
	L"�˻� ����",
};

UnicodeString strTitleShift[3] =
{
	// 1
	L"Shift Right",
	L"Shift Left",
	L"�˻� ����",
};

UnicodeString strTitleHoist[2] =
{
	// 1
	L"Hoist Down",
	L"�˻� ����",
};

UnicodeString strTitleEStop[4] =
{
	// 1
	L"E-STOP Press ON",
	L"E-STOP Error OFF",
	L"Bumper Press ON",
	L"Bumper Error OFF",

};
UnicodeString strTitleOscilation[4] =
{
	// 1
	L"Hoist 300mm�� ������",
	L"Oscil 1�� ���� �� ����",
	L"�� 60�� ���� �� Error",
	L"Oscilation Error OFF",

};
UnicodeString strTitleLookDown[4] =
{
	// 1
	L"LookDown Area ����",
	L"LookDown ����",
	L"�� 10�� ���� �� Error",
	L"LookDown Error OFF",

};
UnicodeString strTitleSTBDetect[1] =
{
	// 1
	L"STB ������ �� Error",
//	L"STB Error Ȯ��",

};
UnicodeString strTitleEQ[2] =
{
	// 1
	L"EQ PORT ������ ���",
	L"EQ PORT Error Ȯ��",

};
UnicodeString strTitleFoupDetect[3] =
{
	// 1
	L"FoupDetect ����",
	L"Unload ��� �ΰ�",
	L"FoupDetect Error OFF",

};

UnicodeString strTitleFoupCover[2] =
{
	// 1
	L"FoupCover ����",
	L"FoupDetect Error OFF",

};

TUnitChecker *UnitChecker;
//---------------------------------------------------------------------------
__fastcall TUnitChecker::TUnitChecker(TComponent* Owner)
	: TForm(Owner)
{
	for(int i = 0; i < 5; i++)
	{
		strngrd1->Cells[0][i + 1] = IntToStr(i + 1);
		strngrd1->Cells[1][i + 1] = strTitle[i];
	}

	strngrd1->Cells[0][0] = L"����";
	strngrd1->Cells[1][0] = L"�˻系��";
	strngrd1->Cells[2][0] = L"�˻���";
	strngrd1->DefaultRowHeight = 16;
	strngrd1->ColWidths[0] = 26;

	for(int i = 0; i < 3; i++)
	{
		strngrdShiftHome->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdShiftHome->Cells[1][i + 1] = strTitleShift[i];
	}

	strngrdShiftHome->Cells[0][0] = L"����";
	strngrdShiftHome->Cells[1][0] = L"�˻系��";
	strngrdShiftHome->Cells[2][0] = L"�˻���";
	strngrdShiftHome->DefaultRowHeight = 16;
	strngrdShiftHome->ColWidths[0] = 26;

	for(int i = 0; i < 2; i++)
	{
		strngrdHoist->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdHoist->Cells[1][i + 1] = strTitleHoist[i];
	}

	strngrdHoist->Cells[0][0] = L"����";
	strngrdHoist->Cells[1][0] = L"�˻系��";
	strngrdHoist->Cells[2][0] = L"�˻���";
	strngrdHoist->DefaultRowHeight = 16;
	strngrdHoist->ColWidths[0] = 26;

	for(int i = 0; i < 4; i++)
	{
		strngrdEStop1->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdEStop1->Cells[1][i + 1] = strTitleEStop[i];
	}
	strngrdEStop1->Cells[0][0] = L"����";
	strngrdEStop1->Cells[1][0] = L"�˻系��";
	strngrdEStop1->Cells[2][0] = L"�˻���";
	strngrdEStop1->DefaultRowHeight = 16;
	strngrdEStop1->ColWidths[0] = 26;
	strngrdEStop1->ColWidths[1] = 120;
	strngrdEStop1->ColWidths[2] = 80;

	for(int i = 0; i < 4; i++)
	{
		strngrdOscilation->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdOscilation->Cells[1][i + 1] = strTitleOscilation[i];
	}
	strngrdOscilation->Cells[0][0] = L"����";
	strngrdOscilation->Cells[1][0] = L"�˻系��";
	strngrdOscilation->Cells[2][0] = L"�˻���";
	strngrdOscilation->DefaultRowHeight = 16;
	strngrdOscilation->ColWidths[0] = 26;
	strngrdOscilation->ColWidths[1] = 120;
	strngrdOscilation->ColWidths[2] = 80;

	for(int i = 0; i < 2; i++)
	{
		strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
	}
	strngrdLookDown->Cells[0][0] = L"����";
	strngrdLookDown->Cells[1][0] = L"�˻系��";
	strngrdLookDown->Cells[2][0] = L"�˻���";
	strngrdLookDown->DefaultRowHeight = 16;
	strngrdLookDown->ColWidths[0] = 26;
	strngrdLookDown->ColWidths[1] = 120;
	strngrdLookDown->ColWidths[2] = 80;


	for(int i = 0; i < 1; i++)
	{
		strngrdSTBDetect->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdSTBDetect->Cells[1][i + 1] = strTitleSTBDetect[i];
	}
	strngrdSTBDetect->Cells[0][0] = L"����";
	strngrdSTBDetect->Cells[1][0] = L"�˻系��";
	strngrdSTBDetect->Cells[2][0] = L"�˻���";
	strngrdSTBDetect->DefaultRowHeight = 16;
	strngrdSTBDetect->ColWidths[0] = 26;
	strngrdSTBDetect->ColWidths[1] = 120;
	strngrdSTBDetect->ColWidths[2] = 80;

	for(int i = 0; i < 2; i++)
	{
		strngrdEQ->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdEQ->Cells[1][i + 1] = strTitleEQ[i];
	}
	strngrdEQ->Cells[0][0] = L"����";
	strngrdEQ->Cells[1][0] = L"�˻系��";
	strngrdEQ->Cells[2][0] = L"�˻���";
	strngrdEQ->DefaultRowHeight = 16;
	strngrdEQ->ColWidths[0] = 26;
	strngrdEQ->ColWidths[1] = 120;
	strngrdEQ->ColWidths[2] = 80;

	for(int i = 0; i < 3; i++)
	{
		strngrdFoupDetect->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdFoupDetect->Cells[1][i + 1] = strTitleFoupDetect[i];
	}
	strngrdFoupDetect->Cells[0][0] = L"����";
	strngrdFoupDetect->Cells[1][0] = L"�˻系��";
	strngrdFoupDetect->Cells[2][0] = L"�˻���";
	strngrdFoupDetect->DefaultRowHeight = 16;
	strngrdFoupDetect->ColWidths[0] = 26;
	strngrdFoupDetect->ColWidths[1] = 120;
	strngrdFoupDetect->ColWidths[2] = 80;


	for(int i = 0; i < 3; i++)
	{
		strngrdFoupCover->Cells[0][i + 1] = IntToStr(i + 1);
		strngrdFoupCover->Cells[1][i + 1] = strTitleFoupCover[i];
	}
	strngrdFoupCover->Cells[0][0] = L"����";
	strngrdFoupCover->Cells[1][0] = L"�˻系��";
	strngrdFoupCover->Cells[2][0] = L"�˻���";
	strngrdFoupCover->DefaultRowHeight = 16;
	strngrdFoupCover->ColWidths[0] = 26;
	strngrdFoupCover->ColWidths[1] = 120;
	strngrdFoupCover->ColWidths[2] = 80;



}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrIOUITimer(TObject *Sender)
{
	FStrLeftLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) ? COLOR_LIME : COLOR_WHITE;
	RStrLeftLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left 	== ON) ? COLOR_LIME : COLOR_WHITE;
	FStrRightLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right 	== ON) ? COLOR_LIME : COLOR_WHITE;
	RStrRightLt->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right 	== ON) ? COLOR_LIME : COLOR_WHITE;

	Shp_ShiftHome->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== ON) ? COLOR_LIME : COLOR_WHITE;

	ShpHoist->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpEStop->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.EMO_Press 	== OFF) ? COLOR_LIME : COLOR_WHITE;
	shpBumperPress->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpOscilation->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.OSCILLATION 	== ON) ? COLOR_LIME : COLOR_WHITE;

	shpLookDown->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpSTBLDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left 	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpSTBRDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpFoupDetect->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect	== ON) ? COLOR_LIME : COLOR_WHITE;
	shpFoupCover->Brush->Color	= (gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect	== ON) ? COLOR_LIME : COLOR_WHITE;


	struct date today;
	struct time currenttime;
	String Time;

	getdate(&today);
	gettime(&currenttime);
	Time.sprintf(L"����ð� : %04d-%02d-%02d : %02d:%02d:%02d", today.da_year, today.da_mon, today.da_day,
					currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec);

	lblCurrentTime->Caption = Time;


	if(chkRailTest->Checked == true)
	{
		for(int i = 0; i < 4; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
			strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
		}
		lblLookDownTime->Visible = true;
	}
	else
	{
		for(int i = 0; i < 2; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = IntToStr(i + 1);
			strngrdLookDown->Cells[1][i + 1] = strTitleLookDown[i];
		}
		for(int i = 2; i < 4; i++)
		{
			strngrdLookDown->Cells[0][i + 1] = "";
			strngrdLookDown->Cells[1][i + 1] = "";
		}
		lblLookDownTime->Visible = false;
	}
	lblVHLName->Caption = "VHL_NAME : "+SocketForm->m_strOHTName;
}

void TUnitChecker::MaxDistShiftHoist(int iIndex, int MaxDist, bool IsPass,bool IsHoist)
{
	if(!IsHoist)
	{
		if(IsPass)
		{
		   if(MaxDist == 1)
				strngrdShiftHome->Cells[2][iIndex] = L"Pass";
		   else
				strngrdShiftHome->Cells[2][iIndex] = L"Fail";
		}
		else
			strngrdShiftHome->Cells[2][iIndex] = IntToStr(MaxDist);
	}
	else
	{
        if(IsPass)
		{
		   if(MaxDist == 1)
				strngrdHoist->Cells[2][iIndex] = L"Pass";
		   else
				strngrdHoist->Cells[2][iIndex] = L"Fail";
		}
		else
			strngrdHoist->Cells[2][iIndex] = IntToStr(MaxDist);
    }

}

//---------------------------------------------------------------------------
void TUnitChecker::Pass(int iIndex, bool bPass)
{
	if(bPass)
		strngrd1->Cells[2][iIndex] = L"Pass";
	else
		strngrd1->Cells[2][iIndex] = L"Fail";
}
//---------------------------------------------------------------------------
void TUnitChecker::PassOrNot(TStringGrid* a, int iIndex, bool bPass)
{
	if(bPass)
	{
		struct time currenttime;
		String Time;

		gettime(&currenttime);
		Time.sprintf(L"%02d:%02d:%02d.%03d",currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec, currenttime.ti_hund);
//		lblCurrentTime->Caption = Time;
		a->Cells[2][iIndex] = Time;//L"Pass";
	}
	else
		a->Cells[2][iIndex] = L"Fail";
}
//---------------------------------------------------------------------------
void TUnitChecker::PassOrNot(TStringGrid* a, int iIndex, bool bPass, double dSec)
{
	if(bPass != true)
		return;

	a->Cells[2][iIndex] = String(L"%.2f",dSec);
}
//---------------------------------------------------------------------------
void TUnitChecker::Log(UnicodeString strLog, int Type)
{
	switch(Type)
	{
		case Log_Steering:
		{
		mmoMemo->Lines->Add(strLog);
		}
		break;
		case Log_ShiftHome:
		{
		mmoShiftHome->Lines->Add(strLog);
		}
		break;
		case Log_HoistHome:
		{
        mmoHoist->Lines->Add(strLog);
}
		break;
		case Log_Step_Init:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_WHITE;
		}
		break;
		case Log_Step_Check:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_YELLOW;
		}
		break;
		case Log_Step_Done:
		{
			lblTextPopup->Caption = strLog;
			shpTotalPopup->Brush->Color = COLOR_LIME;
		}
		break;
		default:
		{
			;
		}
		break;

	}
}

void TUnitChecker::LastTime(DWORD dwTmpTime, DWORD dwRefTime, bool bUse)
{
	String strTmpTime;
	if(bUse == true)
	{
		DWORD dwCheckTime = dwRefTime - dwTmpTime;
		strTmpTime.sprintf(L"%02d�� %02d��", (int)(dwCheckTime/1000/60), (int)((dwCheckTime/1000)%60));
		lblLimitTime->Caption = strTmpTime;
	}
	else
	{
		lblLimitTime->Caption = L"00�� 00��";
	}
}
//void TUnitChecker::OACISLocalLog(String strLog)
//{
//	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
//	m_LogFile->WriteLogFile("OACIS",SocketForm->m_strOHTIPAddress+" : "+strLog);
//	delete m_LogFile;
//}

void TUnitChecker::OACISDataSendTP2OHT(short type, short typeNum,int Data1,int Data2,int Data3,int Data4)
{
  	int iPacketSize = sizeof(PKT_OHT2TP_OACIS_INFO);
	int iReturn;

	PKT_OHT2TP_OACIS_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_OACIS_INFO_SAVE;

	SEND_PACKET.BODY.Type = type;
	SEND_PACKET.BODY.TypeNum = typeNum;
	SEND_PACKET.BODY.nData1 = Data1;
	SEND_PACKET.BODY.nData2 = Data2;
	SEND_PACKET.BODY.nData3 = Data3;
	SEND_PACKET.BODY.nData4 = Data4;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}



void TUnitChecker::StatusMachineShift(int iCmd)
{
	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 int iRet;
	 static int nCountCheck = 0;

     AnsiString strWanted_Result;

	 // Ŀ�ǵ� ó��
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Shift::StartShiftMove;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulationShift->Enabled = false;
			iStatus = Sim_Step_Shift::None;
			return;
		}
		break;
	 }

	 tmrSimulationShift->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_Shift::None: 	// (0) Not work
		 {
			;
		 }
		 break;


		 // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_Shift::StartShiftMove:
		 {
			 Log("Shift Check Home");
			 iStatus++;
		 }
		 break;

		 // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_Shift::StartShiftRight:
		 {
			 ShiftINCMove(30,0);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftRight_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == OFF)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 7.0)
					{
						Log("Shift Right OK",1);
						MaxDistShiftHoist(1,int(gm_OHTStatus->Motor_Shift->dPosition_mm*10));
						iStatus++;
					}else
					{
						MaxDistShiftHoist(1,2,true);
						Log("Shift Right Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}


				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 10.0)
					{
						MaxDistShiftHoist(1,2,true);
						Log("Shift Right Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}else
					{
						ShiftINCMove(3,0);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(1,2,true);
					Log("Shift Right Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;




		  case Sim_Step_Shift::StartShiftHome1:
		 {
			 TechOHTCheck.Axis_Shift_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftHome1_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 1.0)
					{
						Log("Shift Home1 OK",1);
						iStatus++;
					}else
					{
						Log("Shift Home1 Fail",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}
				}
				else
				{
					Log("Shift Home1 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Shift Home1 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		  // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_Shift::StartShiftLeft:
		 {
			 ShiftINCMove(30,1);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftLeft_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == OFF)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 7.0)
					{
						Log("Shift Left OK",1);
						MaxDistShiftHoist(2,int(gm_OHTStatus->Motor_Shift->dPosition_mm*10));
						iStatus++;
					}else
					{
						MaxDistShiftHoist(2,2,true);
						Log("Shift Left Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}


				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 10.0)
					{
						MaxDistShiftHoist(2,2,true);
						Log("Shift Left Fail!",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}else
					{
						ShiftINCMove(3,1);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(2,2,true);
					Log("Shift Left Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		   case Sim_Step_Shift::StartShiftHome2:
		 {
			 TechOHTCheck.Axis_Shift_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Shift::StartShiftHome2_Confirm:
		 {
			if(gm_OHTStatus->Motor_Shift->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) < 1.0)
					{
						Log("Shift Home2 OK",1);
						iStatus++;
					}else
					{
						Log("Shift Home2 Fail",1);
						iStatus = Sim_Step_Shift::None;
						tmrSimulationShift->Enabled = false;
					}
				}
				else
				{
					Log("Shift Home2 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Shift Home2 Fail!",1);
					iStatus = Sim_Step_Shift::None;
					tmrSimulationShift->Enabled = false;
                }
            }

		 }
		 break;

		 case Sim_Step_Shift::ShiftCheckComplete:
		 {
			Log("(5) Shift Check Complete",1);


			int nOK =0;
			if(fabs((fabs(StrToInt(strngrdShiftHome->Cells[2][1])) -fabs(StrToInt(strngrdShiftHome->Cells[2][2])))) <= 10)
			{
				nOK = 1;
			}
			else
			{
				nOK = 2;
            }

			MaxDistShiftHoist(3,nOK,true);

			OACISDataSendTP2OHT(CATEGORY_SHIFTHOME,0,nOK,StrToInt(strngrdShiftHome->Cells[2][2]),StrToInt(strngrdShiftHome->Cells[2][1]));
			tmrSimulationShift->Enabled = false;
			iStatus = Sim_Step_Shift::None;
		 }
		 break;

	 }

	 if(iStatus != Sim_Step_Shift::None)
		tmrSimulationShift->Enabled = true;

}


void TUnitChecker::StatusMachine(int iCmd)
{
	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 bool bFailFlag = false;

     AnsiString strWanted_Result;

	 // Ŀ�ǵ� ó��
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_ST::StartSteeringMove;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
			delete m_CMDControl;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step_ST::None;
			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_LEFT);
			delete m_CMDControl;
			return;
		}
		break;
	 }

	 tmrSimulation->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_ST::None: 	// (0) Not work
		 {
			;
		 }
		 break;

		 // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_ST::StartSteeringMove:
		 {
			 Log("StartSteeringMove Default Steering");
			 iStatus++;
		 }
		 break;


		 case Sim_Step_ST::StartSteeringFRight:
		 {

			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) &&
				(gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON))
			{
					TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
					m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_FRONT_RIGHT);
					delete m_CMDControl;
					Log("STEER_F_Right Cmd");
					iStatus++;
			}
			else
			{
				Log("Default Steering Fail!");
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;

			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringFRight_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Front�� Right�� �̵��� Ȯ���Ͽ����ϱ�?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_F_Right Pass");
					Pass(1, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_F_Right Fail");
					Pass(1, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }

			}
			else
			{
				Log("STEER_F_Right Fail");
				Pass(1, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;


		 case Sim_Step_ST::StartSteeringRRight:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_REAR_RIGHT);
			delete m_CMDControl;
			Log("STEER_R_Right Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringRRight_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Rear�� Right�� �̵��� Ȯ���Ͽ����ϱ�?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_R_Right Pass");
					Pass(2, true);
					iStatus++;
				}
				else
				{
					Log("STEER_R_Right Fail");
					Pass(2, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }
			}
			else
			{
				Log("STEER_R_Right Fail");
				Pass(2, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringFLeft:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_FRONT_LEFT);
			delete m_CMDControl;
			Log("STEER_F_Left Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringFLeft_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left == ON) &&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Front�� Left�� �̵��� Ȯ���Ͽ����ϱ�?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_F_Left Pass");
					Pass(3, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_F_Left Fail");
					Pass(3, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }
			}
			else
			{
				Log("STEER_F_Left Fail");
				Pass(3, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;

		 case Sim_Step_ST::StartSteeringRLeft:
		 {

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_STEER_REAR_LEFT);
			delete m_CMDControl;
			Log("STEER_R_Left Cmd");
			iStatus++;

		 }
		 break;

		 case Sim_Step_ST::StartSteeringRLeft_Confirm:
		 {
			if((gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left == ON)&&
			   (gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right == OFF))
			{
				String strMsg = NULL;
				strMsg.sprintf(L"Steering Rear�� Left�� �̵��� Ȯ���Ͽ����ϱ�?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
				{
					Log("STEER_R_Left Pass");
					Pass(4, true);
					iStatus++;
				}
				else
				{
                    Log("STEER_R_Left Fail");
					Pass(4, false);
					iStatus = Sim_Step_ST::None;
					tmrSimulation->Enabled = false;
					bFailFlag = true;
                }

			}
			else
			{
				Log("STEER_R_Left Fail");
				Pass(4, false);
				iStatus = Sim_Step_ST::None;
				tmrSimulation->Enabled = false;
				bFailFlag = true;
			}
		 }
		 break;
		 // (28) Cid Check Complete
		 case Sim_Step_ST::SteeringCheckComplete:
		 {
			Log("(5) Steering Check Complete");
			Pass(5, true);
			OACISDataSendTP2OHT(CATEGORY_STEER,1,1);
			tmrSimulation->Enabled = false;
			iStatus = Sim_Step_ST::None;
		 }
		 break;
	 }

	 if(iStatus != Sim_Step_ST::None)
		tmrSimulation->Enabled = true;
}


void __fastcall TUnitChecker::btnCloseClick(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_STOP);
	StatusMachineShift(Sim_Cmd_ST::TEST_STOP);
	StatusMachineHoist(Sim_Cmd_ST::TEST_STOP);
	StatusMachineEStop(Sim_Cmd_ST::TEST_STOP);
	this->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnStartClick(TObject *Sender)
{
		// �˻��� Ŭ����
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
	}
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Steering Checker�� �����Ͻðڽ��ϱ�?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 5; i++)
			{
				strngrd1->Cells[2][i + 1] = "";
			}
			mmoMemo->Lines->Clear();
			StatusMachine(Sim_Cmd_ST::TEST_START);
			tmrSimulation->Enabled = true;
		}
		else
			return;
	}


}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnStopClick(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrSimulationTimer(TObject *Sender)
{
	StatusMachine(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnSMPSCheckClick(TObject *Sender)
{
		if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		return;
	}
	else
	{
    	OACISDataSendTP2OHT(CATEGORY_SMPSVOLTAGE,2,0);
	}

}
//---------------------------------------------------------------------------
void TUnitChecker::ShiftINCMove(int MoveDist,bool bLeft)
{
    uTP_CMD TPCmd; // TP Command
	memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Shift(false) == NO_ERR))
    {
        // by zzang9un 2013.09.09 : �������� v2.9�� ���� ���� ������(1/10000m)

        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			if(bLeft)
			{
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_LEFT_INC, &TPCmd);
            	Log("JOG_SHIFT_LEFT_INC:",1);
			}
			else
			{
				m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_SHIFT_RIGHT_INC, &TPCmd);
				Log("JOG_SHIFT_RIGHT_INC:",1);
            }
            delete m_CMDControl;
		}
	}
}

void TUnitChecker::HoistINCMove(int MoveDist)
{


    uTP_CMD TPCmd; // TP Command
    memset(&TPCmd, 0x00, sizeof(uTP_CMD));

    if((TechOHTCheck.Is_Executable() == NO_ERR) && (TechOHTCheck.Is_MoveAxis_Hoist() == NO_ERR))
    {
        if(MoveDist != 0)
        {
            // by zzang9un 2013.02.19 : Jog(Trans) Command ����
            // ��/���ӽð�, �ӵ��� Main�� �����Ǿ� �����Ƿ� �Ÿ����� ����
            TPCmd.CmdTP_Jog_TR.Distance = MoveDist;

            TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTPCmd2OHT(TPCMD_JOG_HOIST_DOWN_INC, &TPCmd);
			Log("JOG_HOIST_DOWN_INC:",2);
            delete m_CMDControl;
        }
    }
}


void __fastcall TUnitChecker::btnShiftHomeStopClick(TObject *Sender)
{
	StatusMachineShift(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::tmrSimulationShiftTimer(TObject *Sender)
{
	StatusMachineShift(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnShiftHomeStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.",1);
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.",1);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Shift Home Sensor is Not ON. Please Retry After Shift Home");
		Log("Shift Home Sensor is Not ON. Please Retry After Shift Home",1);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Hoist Home Sensor is Not ON. Please Retry After Hoist Home");
		Log("Hoist Home Sensor is Not ON. Please Retry After Hoist Home",1);
		return;
	}
	else if((gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Close == ON))
	{
		MainForm->Warnning_Display(clRed, "Hand Foup Exist. Please Retry After Remove Foup");
		Log("Hand Foup Exist. Please Retry After Remove Foup",1);
		return;
	}
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Shift Home Checker�� �����Ͻðڽ��ϱ�?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 3; i++)
			{
				strngrdShiftHome->Cells[2][i + 1] = "";
			}
			mmoShiftHome->Lines->Clear();
			StatusMachineShift(Sim_Cmd_ST::TEST_START);
			tmrSimulationShift->Enabled = true;
		}
		else
			return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnHoistStopClick(TObject *Sender)
{
	StatusMachineHoist(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSimulationHoistTimer(TObject *Sender)
{
	StatusMachineHoist(Sim_Cmd_ST::TEST_GOING);
}

void TUnitChecker::StatusMachineHoist(int iCmd)
{
  	 static int iStatus = Sim_Cmd_ST::TEST_NONE;
	 int iRet;
	 static int nCountCheck = 0;

     AnsiString strWanted_Result;

	 // Ŀ�ǵ� ó��
	 switch(iCmd)
	 {
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Hoist::StartHoistMove;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmrSimulationHoist->Enabled = false;
			iStatus = Sim_Step_Hoist::None;
			return;
		}
		break;
	 }

	 tmrSimulationHoist->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	 switch(iStatus)
	 {
		// ====== Step 0 ~ 9
		 case Sim_Step_Hoist::None: 	// (0) Not work
		 {
			;
		 }
		 break;


		 // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_Hoist::StartHoistMove:
		 {
			 Log("Shift Check Home");
			 iStatus++;
		 }
		 break;

		 // === (1) VHL �ܺκ��͸��� ����
		 case Sim_Step_Hoist::StartHoistDown:
		 {
			 HoistINCMove(40);
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Hoist::StartHoistDown_Confirm:
		 {
			if(gm_OHTStatus->Motor_Hoist->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == OFF)
				{
					
					Log("Hoist Down & Home Sensor Off OK",2);
					MaxDistShiftHoist(1,int(gm_OHTStatus->Motor_Hoist->dPosition_mm*10),false,true);
					iStatus++;

				}
				else
				{

					if(fabs(gm_OHTStatus->Motor_Shift->dPosition_mm) > 15.0)
					{
						MaxDistShiftHoist(1,2,true,true);
						Log("Hoist Down Fail!",2);
						iStatus = Sim_Step_Hoist::None;
						tmrSimulationHoist->Enabled = false;
					}else
					{
						HoistINCMove(3);
					}


				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					MaxDistShiftHoist(1,2,true,true);
					Log("Hoist Down Fail!",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
				}
            }

		 }
		 break;




		  case Sim_Step_Hoist::StartHoistHome:
		 {
			 TechOHTCheck.Axis_Hoist_HOME();
			 nCountCheck = 0;
			 iStatus++;
		 }
		 break;


		 case Sim_Step_Hoist::StartHoistHome_Confirm:
		 {
			if(gm_OHTStatus->Motor_Hoist->nAxis_Running == OFF)
			{
				 if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home == ON)
				{
					if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) < 1.0)
					{
						Log("Hoist Home OK",2);
						iStatus++;
					}else
					{
						Log("Hoist Home Fail",2);
						iStatus = Sim_Step_Hoist::None;
						tmrSimulationHoist->Enabled = false;
					}
				}
				else
				{
					Log("Hoist Home Fail",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
				}
			}else
			{
				nCountCheck++;
				if(nCountCheck > 5)
				{
					Log("Hoist Home Fail",2);
					iStatus = Sim_Step_Hoist::None;
					tmrSimulationHoist->Enabled = false;
                }
            }

		 }
		 break;

		
		 case Sim_Step_Hoist::HoistCheckComplete:
		 {
			Log("HoistCheck Complete",2);


			int nOK =0;
			if(fabs(StrToInt(strngrdHoist->Cells[2][1]) - StrToInt(edtHoistStandard->Text)) < 10)
			{
				nOK = 1;
			}
			else
			{
				nOK = 2;
            }

			MaxDistShiftHoist(2,nOK,true,true);

			OACISDataSendTP2OHT(CATEGORY_HOISTHOME,0,nOK,StrToInt(strngrdHoist->Cells[2][1]));
			tmrSimulationHoist->Enabled = false;
			iStatus = Sim_Step_Hoist::None;
		 }
		 break;

	 }

	 if(iStatus != Sim_Step_Hoist::None)
		tmrSimulationHoist->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnHoistStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode != MODE_MANUAL)
	{
		MainForm->Warnning_Display(clRed, "OHT Current Mode is Not Automode. Please Retry After Converting to Automode.");
		Log("OHT Current Mode is Not Automode. Please Retry After Converting to Automode.",2);
		return;
	}
	else if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) && (gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_COMPLETE))
	{
		MainForm->Warnning_Display(clRed, "OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.");
		Log("OHT Current Status is Not Init or Complete. Please Retry After Converting to Initialize.",2);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.SHIFT_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Shift Home Sensor is Not ON. Please Retry After Shift Home");
		Log("Shift Home Sensor is Not ON. Please Retry After Shift Home",2);
		return;
	}
	else if(gm_OHTStatus->IOInput->uInputCommon.HOIST_Home 	== OFF)
	{
		MainForm->Warnning_Display(clRed, "Hoist Home Sensor is Not ON. Please Retry After Hoist Home");
		Log("Hoist Home Sensor is Not ON. Please Retry After Hoist Home",2);
		return;
	}
	else if((gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == ON) ||
			(gm_OHTStatus->IOInput->uInputCommon.HAND_Close == ON))
	{
		MainForm->Warnning_Display(clRed, "Hand Foup Exist. Please Retry After Remove Foup");
		Log("Hand Foup Exist. Please Retry After Remove Foup",2);
		return;
	}
	else if(((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  != OHT_TYPE_FOUP) &&(gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Type  != OHT_TYPE_FOUP_S)) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver != OHT_VERSION_81))
	{
		MainForm->Warnning_Display(clRed, "OHT is Not FoupType/V8.1.");
		Log("OHT is Not FoupType/V8.1",2);
		return;
    }
	else
	{
		String strMsg = NULL;
		strMsg.sprintf(L"Hoist Home Checker�� �����Ͻðڽ��ϱ�?\n");

		if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
		{
			for(int i = 0; i < 2; i++)
			{
				strngrdHoist->Cells[2][i + 1] = "";
			}
			mmoHoist->Lines->Clear();
			StatusMachineHoist(Sim_Cmd_ST::TEST_START);
			tmrSimulationHoist->Enabled = true;
		}
		else
			return;
	}
}
//---------------------------------------------------------------------------
/*
�� UNITCHECKER 4) E-STOP & Bumper Press
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEStop(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_SensorCheck::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEStopDone->Caption = "������";
			shpEStopDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblEStopDone->Caption = "����";
			shpEStopDone->Brush->Color	=  COLOR_SILVER;
			tmrEStop->Enabled = false;
			iStatus = Sim_Step_SensorCheck::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrEStop->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_SensorCheck::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_SensorCheck::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS Ȯ��
		case Sim_Step_SensorCheck::CheckPressESTOP:
		{
			Log("E-STOP �� �����ּ���.",Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.EMO_Press == false)// || chkTest->Checked == true)
			{
				PassOrNot(strngrdEStop1, 1, true);
				Log("E-STOP PRESS ON Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 1, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "����";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS ���� Ȯ��
		case Sim_Step_SensorCheck::CheckResetESTOP:
		{
			Log("E-STOP �� �����ϰ� Error �� Reset���ּ���.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.EMO_Press == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEStop1, 2, true);
				Log("E-STOP PRESS OFF �� Error Rest Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 2, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "����";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) BUMPER PERESS Ȯ��
		case Sim_Step_SensorCheck::CheckPressBumper:
		{
			Log("Bumper�� �����ּ���.",Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == true)// || chkTest->Checked == true)
			{
				PassOrNot(strngrdEStop1, 3, true);
				Log("BUMPER PRESS ON Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 3, false);
					String strMsg = "1�� �̻� ����Ǿ����ϴ�. �˻縦 �����մϴ�.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "����";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (5) BUMPER PRESS ���� Ȯ��
		case Sim_Step_SensorCheck::CheckResetBumper:
		{
			Log("Error �� Reset���ּ���.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEStop1, 4, true);
				Log("Error Reset Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEStop1, 4, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_SensorCheck::None;
						tmrEStop->Enabled = false;
						lblEStopDone->Caption = "����";
						shpEStopDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_SensorCheck::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_ESTOPBUMPER,1,1);

			tmrEStop->Enabled = false;
			iStatus = Sim_Step_SensorCheck::None;

			lblEStopDone->Caption = "�Ϸ�";
			shpEStopDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_SensorCheck::None)
		tmrEStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEStopStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if((gm_OHTStatus->IOInput->uInputCommon.EMO_Press == false) ||
			(gm_OHTStatus->IOInput->uInputCommon.BUMPER_Press == true))
	{
		MainForm->Warnning_Display(clRed, "E-STOP �Ǵ� BUMPERPRESS�� �����˴ϴ�. ������ ��õ��ϼ���");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" E-STOP, BUMPER PRESS üũ�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineEStop(Sim_Cmd_ST::TEST_START);
		tmrEStop->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::tmrEStopTimer(TObject *Sender)
{
	StatusMachineEStop(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEStopEndCick(TObject *Sender)
{
	StatusMachineEStop(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------
/*
�� UNITCHECKER 5)STB Detect
*/
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/*
�� UNITCHECKER 6)Oscilation Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineOscilation(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static DWORD dwDetectSensor;
	static double dwDetectTime;
	static int	nErrorCount = 0;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_Oscilation::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblOscilationDone->Caption = "������";
			shpOscilationDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblOscilationDone->Caption = "����";
			shpOscilationDone->Brush->Color	=  COLOR_SILVER;
			tmrOscilation->Enabled = false;
			iStatus = Sim_Step_Oscilation::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrOscilation->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Oscilation::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_Oscilation::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) Hoist �ϰ� Ȯ��
		case Sim_Step_Oscilation::CheckHoistDown:
		{
			Log("Oscil On�� �ǵ��� ������ ����� �ΰ��մϴ�.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == true) && (gm_OHTStatus->Motor_Hoist->dPosition_mm > 300))
			{
				PassOrNot(strngrdOscilation, 1, true);
				Log("Hoist 300mm �̻� �ϰ� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdOscilation, 1, false);
					String strMsg = "Hoist ��ġ �ΰ� �� 5�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "����";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation ���� Ȯ��
		case Sim_Step_Oscilation::CheckOscilation:
		{
			if(nErrorCount > 5)
			{
				PassOrNot(strngrdOscilation, 3, false);
				MainForm->Warnning_Display(clRed, "Oscilation Error �� �߻����� �ʾҽ��ϴ�. Ȯ�� �� ���������ּ���.");
				//OACIS�� ���� ���� ����
				//���溯�� Fail ó��
				iStatus = Sim_Step_Oscilation::None;
				tmrOscilation->Enabled = false;
				lblOscilationDone->Caption = "����";
				shpOscilationDone->Brush->Color	=  COLOR_RED;
				Log("",Log_Step_Init);
				LastTime(0, 0, false);
				tmpTesting = 0;
				break;
			}

			Log("Hand ���� �� Oscilation Sensor�� ���������ּ���(�ּ� 60s�̻�)",Log_Step_Check);

			//21.03.27 JHJ False ���� True ���� Ȯ�� �ʿ�
			if((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == false) && (gm_OHTStatus->Motor_Hoist->dVelocity_mpersec < 0.1))
			{
				dwDetectSensor = timeGetTime();
				PassOrNot(strngrdOscilation, 2, true);
				Log("Oscilation ���� �� ���� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdOscilation, 2, false);
					String strMsg = "���� ���� 5�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "����";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) Oscilation Error Ȯ��
		case Sim_Step_Oscilation::CheckError:
		{
			dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
			String strLog;
			strLog.sprintf(L"���� ���·� �������ּ��� : %d��",(int)(dwTmpTime/1000));
			Log(strLog,Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == false)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 944) //ERR_OSCILLATION_ERROR
				{
					String tmp;
					dwDetectTime = PreventOverflowGetTimeMs(dwDetectSensor)*0.001;
					tmp.sprintf(L"����~Err �߻� �ð� : %.2f��",dwDetectTime);
					lblOscilationTime->Caption = tmp;//String(L"����~Err �߻� �ð� : %.2f��",PreventOverflowGetTimeMs(dwDetectTime));
					PassOrNot(strngrdOscilation, 3, true);
					Log("Oscilation Error Ȯ�� �Ϸ�",Log_Step_Done);

					LastTime(0, 0, false);
					dwSensorCheck = timeGetTime();
					iStatus++;
				}
				else
				{
					LastTime(dwTmpTime, CheckTime_1m, true);
					if(dwTmpTime > CheckTime_1m)
					{
						PassOrNot(strngrdOscilation, 3, false);
						String strMsg = "1�� �̻� Error �� �߻����� �ʾҽ��ϴ�. �˻縦 �����մϴ�.";
						if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
						{
							//OACIS�� ���� ���� ����
							//���溯�� Fail ó��
							iStatus = Sim_Step_Oscilation::None;
							tmrOscilation->Enabled = false;
							lblOscilationDone->Caption = "����";
							shpOscilationDone->Brush->Color	=  COLOR_RED;
							Log("",Log_Step_Init);
							LastTime(0, 0, false);
							tmpTesting = 0;
						}

					}
				}
			}
			else
			{
				nErrorCount++;
				iStatus = Sim_Step_Oscilation::CheckOscilation;
			}
		}
		break;

// === (5) Error ���� Ȯ��
		case Sim_Step_Oscilation::CheckResetError:
		{
			Log("Sensor �������� ġ��� Error �� Reset���ּ���.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.OSCILLATION == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdOscilation, 4, true);
				Log("Error Reset Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdOscilation, 4, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Oscilation::None;
						tmrOscilation->Enabled = false;
						lblOscilationDone->Caption = "����";
						shpOscilationDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_Oscilation::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_OSCILATION,1,1);

			tmrOscilation->Enabled = false;
			iStatus = Sim_Step_Oscilation::None;

			lblOscilationDone->Caption = "�Ϸ�";
			shpOscilationDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Oscilation::None)
		tmrOscilation->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnOscilationStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" Oscilation Check�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineOscilation(Sim_Cmd_ST::TEST_START);
		tmrOscilation->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::btnOsclationEndClick(TObject *Sender)
{
	StatusMachineOscilation(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrOscilationTimer(TObject *Sender)
{
	StatusMachineOscilation(Sim_Cmd_ST::TEST_GOING);
}


//---------------------------------------------------------------------------
/*
�� UNITCHECKER 7)LookDown Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineLookDown(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static DWORD dwDetectSensor;
	static double dwDetectTime;
	static int	nErrorCount = 0;
	static bool	bSendTPCmd = false;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			bSendTPCmd = false;
			iStatus = Sim_Step_LookDown::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblLookDownDone->Caption = "������";
			shpLookDownDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblLookDownDone->Caption = "����";
			shpLookDownDone->Brush->Color	=  COLOR_SILVER;
			tmrLookDown->Enabled = false;
			iStatus = Sim_Step_LookDown::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrLookDown->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_LookDown::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_LookDown::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2)LookDown Area Setting
		case Sim_Step_LookDown::CheckAreaSelect:
		{
			Log("LookDown Area Setting ���� ��",Log_Step_Check);

			if(bSendTPCmd == false)
			{
            	bSendTPCmd = true;
				TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
				m_CMDControl->MakeTPCmd2OHT(TPCMD_LOOKDOWN_AREA1);      //���̰ŵ��۾��Ѵ�
				delete m_CMDControl;
			}
			int nValue	= 	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area1_Sel)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area2_Sel << 1)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area3_Sel << 2)
						+	(gm_OHTStatus->IOOutput->uOutputCommon.LOOKDOWN_Area4_Sel << 3);
			nValue = ~nValue & 0xF;
			if(((nValue == 0x01) && (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error  == false))
			)// || (chkTest->Checked == true))
			{
				PassOrNot(strngrdLookDown, 1, true);
				Log("LookDown Area Setting �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 1, false);
					String strMsg = "LookDown Area Setting ��� �ΰ� �� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "����";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation ���� Ȯ��
		case Sim_Step_LookDown::CheckLookDown:
		{
			if(nErrorCount > 5)
			{
				PassOrNot(strngrdLookDown, 3, false);
				MainForm->Warnning_Display(clRed, "LookDown Error �� �߻����� �ʾҽ��ϴ�. Ȯ�� �� ���������ּ���.");
				//OACIS�� ���� ���� ����
				//���溯�� Fail ó��
				iStatus = Sim_Step_LookDown::None;
				tmrLookDown->Enabled = false;
				lblLookDownDone->Caption = "����";
				shpLookDownDone->Brush->Color	=  COLOR_RED;
				Log("",Log_Step_Init);
				LastTime(0, 0, false);
				tmpTesting = 0;
				break;
			}

			if(chkRailTest->Checked == true)
				Log("�����縦 ������ �� LookDown Sensor�� ���������ּ���(�ּ� 10s�̻�)",Log_Step_Check);
			else
				Log("LookDown Sensor �� ���������ּ���.",Log_Step_Check);

			//21.03.27 JHJ False ���� True ���� Ȯ�� �ʿ�
			if(((gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == true) && (gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error == false))
			)// || (chkTest->Checked == false))
			{
				dwDetectSensor = timeGetTime();
				PassOrNot(strngrdLookDown, 2, true);
				Log("LookDown ���� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;

				if(chkRailTest->Checked == true)
					iStatus++;
				else
				{
					iStatus = Sim_Step_LookDown::SensorCheckComplete;
				}
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 2, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "����";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (4) Oscilation Error Ȯ��
		case Sim_Step_LookDown::CheckError:
		{
			dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
			String strLog;
			strLog.sprintf(L"���� ���·� �������ּ��� : %d��",(int)(dwTmpTime/1000));
			Log(strLog,Log_Step_Check);
			if(gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == true )//|| chkTest->Checked == true)
			{
				if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 941)// || chkTest->Checked == true) //ERR_LOOKDOWN_DETECT
				{
					String tmp;
					dwDetectTime = PreventOverflowGetTimeMs(dwDetectSensor)*0.001;
					tmp.sprintf(L"����~Err �߻� �ð� : %.2f��",dwDetectTime);
					lblLookDownTime->Caption = tmp;

					PassOrNot(strngrdLookDown, 3, true);
					Log("LookDown Error Ȯ�� �Ϸ�",Log_Step_Done);

					LastTime(0, 0, false);
					dwSensorCheck = timeGetTime();
					iStatus++;
				}
				else
				{
					LastTime(dwTmpTime, CheckTime_1m, true);
					if(dwTmpTime > CheckTime_1m)
					{
						PassOrNot(strngrdLookDown, 3, false);
						String strMsg = "1�� �̻� Error �� �߻����� �ʾҽ��ϴ�. �˻縦 �����մϴ�.";
						if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
						{
							//OACIS�� ���� ���� ����
							//���溯�� Fail ó��
							iStatus = Sim_Step_LookDown::None;
							tmrLookDown->Enabled = false;
							lblLookDownDone->Caption = "����";
							shpLookDownDone->Brush->Color	=  COLOR_RED;
							Log("",Log_Step_Init);
							LastTime(0, 0, false);
							tmpTesting = 0;
						}

					}
				}
			}
			else
			{
				nErrorCount++;
				iStatus = Sim_Step_LookDown::CheckLookDown;
			}
		}
		break;

// === (5) Error ���� Ȯ��
		case Sim_Step_LookDown::CheckResetError:
		{
			Log("Sensor �������� ġ��� Error �� Reset���ּ���.",Log_Step_Check);
			if(((gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdLookDown, 4, true);
				Log("Error Reset Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdLookDown, 4, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_LookDown::None;
						tmrLookDown->Enabled = false;
						lblLookDownDone->Caption = "����";
						shpLookDownDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_LookDown::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_LOOKDOWN,1,1);

			tmrLookDown->Enabled = false;
			iStatus = Sim_Step_LookDown::None;

			lblLookDownDone->Caption = "�Ϸ�";
			shpLookDownDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_LookDown::None)
		tmrLookDown->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnLookDownStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.LOOKDOWN_Error  == true)
	{
		MainForm->Warnning_Display(clRed, "LookDown Sensor ���� �� ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}
	if(gm_OHTStatus->CheckOnOff->bUse_LookDownSensor == false)
	{
		MainForm->Warnning_Display(clRed, "LookDown �̻�� �����Դϴ�. Use LookDown�� True�� �������ּ���");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" LookDown Check�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineLookDown(Sim_Cmd_ST::TEST_START);
		tmrLookDown->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnLookDownEndClick(TObject *Sender)
{
	StatusMachineLookDown(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrLookDownTimer(TObject *Sender)
{
	StatusMachineLookDown(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
/*
�� UNITCHECKER 8)STB Detect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineSTBNo(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_STBTrans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblSTBNoDone->Caption = "������";
			shpSTBNoDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 1)
			{
				tmpTesting = 0;

				lblSTBNoDone->Caption = "����";
				shpSTBNoDone->Brush->Color	=  COLOR_SILVER;
				tmrSTBNo->Enabled = false;
				iStatus = Sim_Step_STBTrans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrSTBNo->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_STBTrans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_STBTrans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;
// === (3) E-STOP PRESS ���� Ȯ��
		case Sim_Step_STBTrans::CheckError:
		{
			Log("Unload ����� �ΰ����ּ���.",Log_Step_Check);

			if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == false && gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 851)
			 || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == false && gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 851))//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdSTBDetect, 1, true);
				Log("Error �߻� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdSTBDetect, 1, false);
					String strMsg = "���� �� Error ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_STBTrans::None;
						tmrSTBNo->Enabled = false;
						lblSTBNoDone->Caption = "����";
						shpSTBNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_STBTrans::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_STBDETECT,1,1);

			tmrSTBNo->Enabled = false;
			iStatus = Sim_Step_STBTrans::None;

			lblSTBNoDone->Caption = "�Ϸ�";
			shpSTBNoDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_STBTrans::None)
		tmrSTBNo->Enabled = true;
}
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineSTBDouble(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_STBTrans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblSTBDoubleDone->Caption = "������";
			shpSTBDoubleDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 2)
			{
				tmpTesting = 0;

				lblSTBDoubleDone->Caption = "����";
				shpSTBDoubleDone->Brush->Color	=  COLOR_SILVER;
				tmrSTBDouble->Enabled = false;
				iStatus = Sim_Step_STBTrans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrSTBDouble->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_STBTrans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_STBTrans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS Ȯ��
		case Sim_Step_STBTrans::CheckError:
		{
			Log("Load ����� �ΰ����ּ���.",Log_Step_Check);

			if(((chkSTBDirection->Checked == false) && (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 850))//ERR_STB_FOUP_CHECK_NOEXIST))
			|| ((chkSTBDirection->Checked == true) && (gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 850)))//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdSTBDetect, 1, true);
				Log("Error �߻� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdSTBDetect, 1, false);
					String strMsg = "������ ���� �Ǵ� ������ ���� 5�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_STBTrans::None;
						tmrSTBDouble->Enabled = false;
						lblSTBDoubleDone->Caption = "����";
						shpSTBDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_STBTrans::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_STBDETECT,1,2);

			tmrSTBDouble->Enabled = false;
			iStatus = Sim_Step_STBTrans::None;

			lblSTBDoubleDone->Caption = "�Ϸ�";
			shpSTBDoubleDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_STBTrans::None)
		tmrSTBDouble->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBNoCarrierClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "���� ������ ��ġ�� �ƴմϴ�. ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT�� ���ϰ� �����մϴ�. ���ϸ� ���� ��õ��ϼ���.");
		return;
	}

	if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == true)
	  || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == true))
	{
		MainForm->Warnning_Display(COLOR_RED, "STB�� ���ϰ� �����մϴ�. ���ϸ� ���� ��õ��ϼ���.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" STB ������ TEST�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 1;
		StatusMachineSTBNo(Sim_Cmd_ST::TEST_START);
		tmrSTBNo->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBDoubleCarrierClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "���� ������ ��ġ�� �ƴմϴ�. ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT�� ���ϰ� �������� �ʽ��ϴ�. ���ϸ� ���  ��õ��ϼ���.");
		return;
	}

	if((chkSTBDirection->Checked == false && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Left == false)
	  || (chkSTBDirection->Checked == true && gm_OHTStatus->IOInput->uInputCommon.STB_Detect_Right == false))
	{
		MainForm->Warnning_Display(COLOR_RED, "STB�� ���ϰ� �����ϴ�. ���ϸ� �ְ� ��õ��ϼ���.");
		return;
	}


	String strMsg = NULL;
	strMsg.sprintf(L" STB�������� TEST�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 2;
		StatusMachineSTBDouble(Sim_Cmd_ST::TEST_START);
		tmrSTBDouble->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnSTBDetectEndClick(TObject *Sender)
{
	StatusMachineSTBNo(Sim_Cmd_ST::TEST_STOP);
	StatusMachineSTBDouble(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSTBNoTimer(TObject *Sender)
{
	StatusMachineSTBNo(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrSTBDoubleTimer(TObject *Sender)
{
	StatusMachineSTBDouble(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
�� UNITCHECKER 8)EQ Detect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEQNo(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Trans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEQNoDone->Caption = "������";
			shpEQNoDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 1)
			{
				tmpTesting = 0;

				lblEQNoDone->Caption = "����";
				shpEQNoDone->Brush->Color	=  COLOR_SILVER;
				tmrEQNo->Enabled = false;
				iStatus = Sim_Step_Trans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrEQNo->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Trans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_Trans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS Ȯ��
		case Sim_Step_Trans::CheckLoadUnloadCmd:
		{
			Log("Unload ����� �ΰ����ּ���.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_UNLOADING )//|| chkTest->Checked == true)
			{
				PassOrNot(strngrdEQ, 1, true);
				Log("������ ��� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdEQ, 1, false);
					String strMsg = "������ ���� ���� 5�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Trans::None;
						tmrEQNo->Enabled = false;
						lblEQNoDone->Caption = "����";
						shpEQNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS ���� Ȯ��
		case Sim_Step_Trans::CheckError:
		{
			Log("Error �߻� ���θ� Ȯ���մϴ�.",Log_Step_Check);

			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEQ, 2, true);
				Log("Error �߻� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEQ, 2, false);
					String strMsg = "���� �� Error ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Trans::None;
						tmrEQNo->Enabled = false;
						lblEQNoDone->Caption = "����";
						shpEQNoDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_Trans::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_EQPORT,1,1);

			tmrEQNo->Enabled = false;
			iStatus = Sim_Step_Trans::None;

			lblEQNoDone->Caption = "�Ϸ�";
			shpEQNoDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Trans::None)
		tmrEQNo->Enabled = true;
}
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineEQDouble(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			iStatus = Sim_Step_Trans::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblEQDoubleDone->Caption = "������";
			shpEQDoubleDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			if(tmpTesting == 2)
			{
				tmpTesting = 0;

				lblEQDoubleDone->Caption = "����";
				shpEQDoubleDone->Brush->Color	=  COLOR_SILVER;
				tmrEQDouble->Enabled = false;
				iStatus = Sim_Step_Trans::None;

				Log("",Log_Step_Init);
            }
			return;
		}
		break;
	}

	tmrEQDouble->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_Trans::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_Trans::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

// === (2) E-STOP PRESS Ȯ��
		case Sim_Step_Trans::CheckLoadUnloadCmd:
		{
			Log("Load ����� �ΰ����ּ���.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_LOADING )//|| chkTest->Checked == true)
			{
				PassOrNot(strngrdEQ, 1, true);
				Log("������ ��� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdEQ, 1, false);
					String strMsg = "������ ���� ���� 5�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Trans::None;
						tmrEQDouble->Enabled = false;
						lblEQDoubleDone->Caption = "����";
						shpEQDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) E-STOP PRESS ���� Ȯ��
		case Sim_Step_Trans::CheckError:
		{
			Log("Error �߻� ���θ� Ȯ���մϴ�.",Log_Step_Check);

			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)//|| (chkTest->Checked == false))
			{
				PassOrNot(strngrdEQ, 2, true);
				Log("Error �߻� Ȯ�� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdEQ, 2, false);
					String strMsg = "���� �� Error ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_Trans::None;
						tmrEQDouble->Enabled = false;
						lblEQDoubleDone->Caption = "����";
						shpEQDoubleDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_Trans::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_EQPORT,1,2);

			tmrSTBDouble->Enabled = false;
			iStatus = Sim_Step_Trans::None;

			lblEQDoubleDone->Caption = "�Ϸ�";
			shpEQDoubleDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_Trans::None)
		tmrEQDouble->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnEQNoClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "���� ������ ��ġ�� �ƴմϴ�. ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT�� ���ϰ� �����մϴ�. ���ϸ� ���� ��õ��ϼ���.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" EQ ������ TEST�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 1;
		StatusMachineEQNo(Sim_Cmd_ST::TEST_START);
		tmrEQNo->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------


void __fastcall TUnitChecker::btnEQDoneClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.CurStationID == 0)
	{
		MainForm->Warnning_Display(clRed, "���� ������ ��ġ�� �ƴմϴ�. ��õ��ϼ���.");
		return;
	}
	if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
		gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
	{
		MainForm->Warnning_Display(COLOR_RED, "OHT�� ���ϰ� �������� �ʽ��ϴ�. ���ϸ� ���  ��õ��ϼ���.");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" EQ �������� TEST�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting = 2;
		StatusMachineEQDouble(Sim_Cmd_ST::TEST_START);
		tmrEQDouble->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnEQStopClick(TObject *Sender)
{
	StatusMachineEQNo(Sim_Cmd_ST::TEST_STOP);
	StatusMachineEQDouble(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrEQNoTimer(TObject *Sender)
{
	StatusMachineEQNo(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrEQDoubleTimer(TObject *Sender)
{
	StatusMachineEQDouble(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/*
�� UNITCHECKER 9)FoupDetect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineFoupDetect(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static int	nErrorCount = 0;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_FoupDetect::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblFoupDetectDone->Caption = "������";
			shpFoupDetectDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblFoupDetectDone->Caption = "����";
			shpFoupDetectDone->Brush->Color	=  COLOR_SILVER;
			tmrFoupDetect->Enabled = false;
			iStatus = Sim_Step_FoupDetect::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrFoupDetect->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_FoupDetect::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_FoupDetect::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

		case Sim_Step_FoupDetect::CheckFoupDetect:
		{
			Log("Foup Detect Sensor �� �����ּ���.",Log_Step_Check);
			//21.03.27 JHJ False ���� True ���� Ȯ�� �ʿ�
			if(((gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true))//1230))
			)// || (chkTest->Checked == false))
			{
				PassOrNot(strngrdFoupDetect, 1, true);
				Log("FoupDetect ����  �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				nErrorCount++;
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupDetect, 1, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "����";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (2) Hoist �ϰ� Ȯ��
		case Sim_Step_FoupDetect::CheckUnload:
		{
			Log("������ ���·� Unload ����� �ΰ��մϴ�.",Log_Step_Check);
			if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 858)
			{
				PassOrNot(strngrdFoupDetect, 2, true);
				Log("Error �߻� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdFoupDetect, 2, false);
					String strMsg = "5�� �̻� Error�� �߻����� �ʾҽ��ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "����";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation ���� Ȯ��
// === (5) Error ���� Ȯ��
		case Sim_Step_FoupDetect::CheckResetError:
		{
			Log("Sensor �������� ġ��� Error �� Reset���ּ���.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			{
				PassOrNot(strngrdFoupDetect, 3, true);
				Log("Error Reset Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupDetect, 3, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_FoupDetect::None;
						tmrFoupDetect->Enabled = false;
						lblFoupDetectDone->Caption = "����";
						shpFoupDetectDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_FoupDetect::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_FOUPDETECT,1,1);

			tmrFoupDetect->Enabled = false;
			iStatus = Sim_Step_FoupDetect::None;

			lblFoupDetectDone->Caption = "�Ϸ�";
			shpFoupDetectDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_FoupDetect::None)
		tmrFoupDetect->Enabled = true;
}
void __fastcall TUnitChecker::btnFoupDetectStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}
	//Ǳ�� ��� �ֽ��ϴ�.


	String strMsg = NULL;
	strMsg.sprintf(L" FoupDetect Check�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineFoupDetect(Sim_Cmd_ST::TEST_START);
		tmrFoupDetect->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrFoupDetectTimer(TObject *Sender)
{
	StatusMachineFoupDetect(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnFoupDetectStopClick(TObject *Sender)
{
	StatusMachineFoupDetect(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::tmrFoupCoverTimer(TObject *Sender)
{
	StatusMachineFoupCover(Sim_Cmd_ST::TEST_GOING);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
�� UNITCHECKER 9)FoupDetect Sensor
*/
//---------------------------------------------------------------------------

void TUnitChecker::StatusMachineFoupCover(int iCmd)
{
	static int iStatus = Sim_Cmd_ST::TEST_NONE;
	static DWORD dwSensorCheck;
	static DWORD dwTmpTime;
	static int	nErrorCount = 0;

	 // Ŀ�ǵ� ó��
	switch(iCmd)
	{
		case Sim_Cmd_ST::TEST_START:
		{
			nErrorCount = 0;
			iStatus = Sim_Step_FoupCover::SensorCheckStart;
			return;
		}
		break;

		case Sim_Cmd_ST::TEST_GOING:
		{
			lblFoupCoverDone->Caption = "������";
			shpFoupCoverDone->Brush->Color	=  COLOR_YELLOW;	// �ƹ��͵� ���ϰ� �״�� ����
		}
		break;

		case Sim_Cmd_ST::TEST_STOP:
		{
			tmpTesting = 0;

			lblFoupCoverDone->Caption = "����";
			shpFoupCoverDone->Brush->Color	=  COLOR_SILVER;
			tmrFoupCover->Enabled = false;
			iStatus = Sim_Step_FoupCover::None;

			Log("",Log_Step_Init);
			return;
		}
		break;
	}

	tmrFoupCover->Enabled = false;	// �ߺ� �������

	 // Status Machine ����
	switch(iStatus)
	{
		// ====== Step 0 ~ 9
		case Sim_Step_FoupCover::None: 	// (0) Not work
		{
			;
		}
		break;

// === (1) Check ����
		case Sim_Step_FoupCover::SensorCheckStart:
		{
			dwSensorCheck = timeGetTime();
			iStatus++;
		}
		break;

		case Sim_Step_FoupCover::CheckFoupCover:
		{
			Log("Foup Cover Detect Sensor �� �����ּ���.",Log_Step_Check);
			//21.03.27 JHJ False ���� True ���� Ȯ�� �ʿ�
			if((gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect == true) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 1230))
			{
				PassOrNot(strngrdFoupCover, 1, true);
				Log("Error �߻� �Ϸ�",Log_Step_Done);
				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_5m, true);
				if(dwTmpTime > CheckTime_5m)
				{
					PassOrNot(strngrdFoupCover, 1, false);
					String strMsg = "5�� �̻� Error�� �߻����� �ʾҽ��ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_FoupCover::None;
						tmrFoupCover->Enabled = false;
						lblFoupCoverDone->Caption = "����";
						shpFoupCoverDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;
// === (3) Oscilation ���� Ȯ��
// === (5) Error ���� Ȯ��
		case Sim_Step_FoupCover::CheckResetError:
		{
			Log("Sensor �������� ġ��� Error �� Reset���ּ���.",Log_Step_Check);
			if((gm_OHTStatus->IOInput->uInputCommon.FOUB_Cover_Detect == false) && (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == 0))
			{
				PassOrNot(strngrdFoupCover, 2, true);
				Log("Error Reset Ȯ�� �Ϸ�",Log_Step_Done);

				LastTime(0, 0, false);
				dwSensorCheck = timeGetTime();
				iStatus++;
			}
			else
			{
				dwTmpTime = PreventOverflowGetTimeMs(dwSensorCheck);
				LastTime(dwTmpTime, CheckTime_1m, true);
				if(dwTmpTime > CheckTime_1m)
				{
					PassOrNot(strngrdFoupCover, 2, false);
					String strMsg = "���� ���� 1�� �̻� ����Ǿ����ϴ�. Ȯ�� �� ��õ����ּ���.";
					if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OK) == IDOK)
					{
						//OACIS�� ���� ���� ����
						//���溯�� Fail ó��
						iStatus = Sim_Step_FoupCover::None;
						tmrFoupCover->Enabled = false;
						lblFoupCoverDone->Caption = "����";
						shpFoupCoverDone->Brush->Color	=  COLOR_RED;
						Log("",Log_Step_Init);
						LastTime(0, 0, false);
						tmpTesting = 0;
					}

				}
			}
		}
		break;

// === (6) SENSOR CHECK �Ϸ� Ȯ��
		case Sim_Step_FoupCover::SensorCheckComplete:
		{
			//OACIS�� ���� ���� ����
			OACISDataSendTP2OHT(CATEGORY_FOUPCOVER,1,1);

			tmrFoupCover->Enabled = false;
			iStatus = Sim_Step_FoupCover::None;

			lblFoupCoverDone->Caption = "�Ϸ�";
			shpFoupCoverDone->Brush->Color	=  COLOR_LIME;
			tmpTesting = 0;
			Log("",Log_Step_Init);
			LastTime(0, 0, false);

		}
		break;
	}
	if(iStatus != Sim_Step_FoupCover::None)
		tmrFoupCover->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TUnitChecker::btnFoupCoverStartClick(TObject *Sender)
{
	if(tmpTesting !=0)
	{
		MainForm->Warnning_Display(clRed, "���� �ٸ� TEST�� �������Դϴ�. �Ϸ� �� ��õ��ϼ���. ");
		return;
	}
	if(gm_OHTStatus->m_OHTStatus.Data.ErrorCode != 0)
	{
		MainForm->Warnning_Display(clRed, "Error �� �߻��߽��ϴ�. ������ ��õ��ϼ���");
		return;
	}

	String strMsg = NULL;
	strMsg.sprintf(L" FOUP Cover Detect Check�� �����Ͻðڽ��ϱ�?\n");
	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
	{
		tmpTesting++;
		StatusMachineFoupCover(Sim_Cmd_ST::TEST_START);
		tmrFoupCover->Enabled = true;
	}
	else
		return;         //�������� ��� ���� ���� Ȯ�� �ʿ�
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnFoupCoverStopClick(TObject *Sender)
{
	StatusMachineFoupCover(Sim_Cmd_ST::TEST_STOP);
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnUBGOBSClick(TObject *Sender)
{
	OBS_UBG_Form->Show();
	OBS_UBG_Form->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TUnitChecker::btnCIDCheckClick(TObject *Sender)
{
	frmCidChecker->Show();
	frmCidChecker->BringToFront();
}
//---------------------------------------------------------------------------


