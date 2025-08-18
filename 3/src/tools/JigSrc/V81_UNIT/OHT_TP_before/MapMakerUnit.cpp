// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <math.h>
#include "MapMakerUnit.h"
#include "MainUnit.h"
#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "Def_TP.h"
#include "UDP.h"
#include "StatusUnit.h"
#include "ChkExecUnit.h"
#include "WarnningUnit.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMapMakerForm *MapMakerForm;
TCHKOHT CmdOHTCheck;

// ---------------------------------------------------------------------------
__fastcall TMapMakerForm::TMapMakerForm(TComponent* Owner) : TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TMapMakerForm::FormShow(TObject *Sender)
{
    MapMakerForm->Top = MainForm->Top + MainForm->Height;
	MapMakerForm->Left = MainForm->Left;
	MapMakerForm->Width = MainForm->Width + StatusForm->Width;
	MapMakerForm->Height = 560;
}
// ---------------------------------------------------------------------------

void __fastcall TMapMakerForm::btnAutoMapStartClick(TObject *Sender)
{
	int StartNodeID = StrToIntDef(edtStartNodeID->Text,0);
	int EndNodeID = StrToIntDef(edtEndNodeID->Text,0);
	int MapmakerMidfyDist = StrToIntDef(edtMapMakerDist->Text, 0);



    uGO_ETC_CMD SndGoCmdEtc;
	memset(&SndGoCmdEtc, 0x00, sizeof(uGO_ETC_CMD));

    if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        MainForm->Warnning_Display(COLOR_RED, "ERROR : OHT Mode is Auto, Change Manual");
    }
    else if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_MANUAL)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_mm) > HOIST_ORIGIN_LIMIT)
        {
			MainForm->Warnning_Display(COLOR_RED, "ERROR : Hoist[#2] Not Origin");
		}
        else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
        {
			MainForm->Warnning_Display(COLOR_RED, "ERROR : Shift[#3] Not Origin");
		}
		else if((EndNodeID == 0) || (StartNodeID == 0))
		{
            MainForm->Warnning_Display(COLOR_RED, "ERROR : Start End Node Zero!");
        }
        else
        {

			// ���ο� ������ ��Ŷ�� �̿��Ͽ� ����
			int iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);
			int iReturn;

			PKT_OHT2TP_DRIVINGMAP_INFO 	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
			SEND_PACKET.TAG.Value = TagID::CMD_DRIVINGMAP_INFO_SAVE;


			SEND_PACKET.BODY.nStartNodeID = StartNodeID;
			SEND_PACKET.BODY.nEndNodeID = EndNodeID;
			SEND_PACKET.BODY.nMapMakeDist = MapmakerMidfyDist;

			TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			delete m_CMDControl;
        }
    }
}
// ---------------------------------------------------------------------------
void __fastcall TMapMakerForm::mniMemoClearClick(TObject *Sender)
{
    memoMapMaker->Clear();
}
//---------------------------------------------------------------------------

void TMapMakerForm::DisplayMapMakerInfo(uTP_CMD MapInfo)
{
    String strMapInfo;

    String strTagType = TagTypeToStr((BYTE)MapInfo.MapMakeInfo.DataInfo.TagType).w_str();

    strMapInfo.sprintf(L"[LinkModify][%-10s] Start Node:%-6X, Next Node:%-6X, StationID:%-6X, Dist:%-5d",
            strTagType.w_str(),
            MapInfo.MapMakeInfo.StartNodeID,
            MapInfo.MapMakeInfo.EndNodeID,
            MapInfo.MapMakeInfo.StationID,
            MapInfo.MapMakeInfo.Offset);

    memoMapMaker->Lines->Add(strMapInfo);

}

void TMapMakerForm::DisplayDrivingMapInfo(PKT_OHT2TP_DRIVINGMAP_INFO* PACKET)
{
	String strMapInfo;

	strMapInfo.sprintf(L"[LinkModify]Start Node:%6d, Next Node:%d, Dist:%d",
			PACKET->BODY.nStartNodeID,
			PACKET->BODY.nEndNodeID,
			PACKET->BODY.nMapMakeDist);

	memoMapMaker->Lines->Add(strMapInfo);
}

void TMapMakerForm::Display_MapMake_Result(PKT_OHT2TP_LOG_DATA PKT)
{
	AnsiString strResult;
	strResult.sprintf("%s", PKT.BODY.Log);
	memoMapMaker->Lines->Add(strResult);
}

void __fastcall TMapMakerForm::btnReqClick(TObject *Sender)
{
//#if 1
//    // Step 1. Station ID üũ
//    if(!gm_OHTStatus->m_OHTStatus.Data.CurStationID)
//    {
//        ERROR_MSGBOX("Station ID�� �����ϴ�.");
//        return;
//    }
//
//    // Step 2. Port Type üũ
//    if(cbbPortType->ItemIndex == -1)
//    {
//        ERROR_MSGBOX("Port Type ���� �����ϴ�.");
//        return;
//	}
//#else
//	ShowMessage("�������̹Ƿ� ������ ���� �ǳʶ�. �ݵ�� Ǯ�� �� ��");
//#endif
	int StartNodeID = StrToIntDef(edtStartNodeID->Text,0);
	int EndNodeID = StrToIntDef(edtEndNodeID->Text,0);
	int MapmakerMidfyDist = StrToIntDef(edtMapMakerDist->Text, 0);
	// Step 3. Teaching Data ��û
	// C������Ʈ ��Ź������ ����
	int iPacketSize = sizeof(PKT_OHT2TP_DRIVINGMAP_INFO);
	int iReturn;

	PKT_OHT2TP_DRIVINGMAP_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);
	SEND_PACKET.TAG.Value = TagID::CMD_DRIVINGMAP_INFO_REQ;

	SEND_PACKET.BODY.nStartNodeID = StartNodeID;
	SEND_PACKET.BODY.nEndNodeID = EndNodeID;
	SEND_PACKET.BODY.nMapMakeDist = MapmakerMidfyDist;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
	delete m_CMDControl;
}
//---------------------------------------------------------------------------

void __fastcall TMapMakerForm::tmrUITimer(TObject *Sender)
{
	bool bMapMakeMoveEnd = true;

	if(edtMoveEnd->Text !="")
	{
		if(gm_OHTStatus->m_OHTStatus.Data.CurNodeID !=0)
		{
			bMapMakeMoveEnd  =(StrToInt(edtMoveEnd->Text) == gm_OHTStatus->m_OHTStatus.Data.CurNodeID) ? true : false;
			ShpIsTargetNode->Brush->Color =  (bMapMakeMoveEnd) ? COLOR_LIME : COLOR_WHITE;

		}
	}

	shpCycle->Brush->Color = (gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling != CYCLE::CycleNotExist)? COLOR_LIME : COLOR_WHITE;
	shpCycle2->Brush->Color = (gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling != CYCLE::CycleNotExist)? COLOR_LIME : COLOR_WHITE;

	ShaAdjustMap->Brush->Color = ((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust ==ON) &&(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON))? COLOR_LIME : COLOR_WHITE;
	ShaAdjustMap2->Brush->Color = ((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust ==ON) &&(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON))? COLOR_LIME : COLOR_WHITE;

	ShaAdjustMapModify->Brush->Color = (gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust_Modify ==ON)? COLOR_LIME : COLOR_WHITE;

	shpDrivingMarked->Brush->Color = (gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == ON)? COLOR_LIME : COLOR_WHITE;
	shpDrivingMarked2->Brush->Color = (gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == ON)? COLOR_LIME : COLOR_WHITE;

	shpCIDPause->Brush->Color = (gm_OHTStatus->m_OHTStatus.IsCIDPause == ON) ? COLOR_LIME : COLOR_WHITE;
	shpCIDPause2->Brush->Color = (gm_OHTStatus->m_OHTStatus.IsCIDPause == ON) ? COLOR_LIME : COLOR_WHITE;

	LabMapMakeCount->Caption = IntToStr(gm_OHTStatus->m_OHTStatus.MapMakedLinkCount);
	LabMapMakeMoveCount->Caption = IntToStr(gm_OHTStatus->m_OHTStatus.MapMakedLinkDrivingCheckCount);

	if(gm_OHTStatus->m_OHTStatus.MapMakedLinkCount !=0)
	{
		int nPercent = (int)(((double)gm_OHTStatus->m_OHTStatus.MapMakedLinkDrivingCheckCount/(double)gm_OHTStatus->m_OHTStatus.MapMakedLinkCount)*100);
		lblpercent->Caption = IntToStr(nPercent) +"%";
		ProgressBar1->Position = nPercent;
	}
	//mapMake �� ���� Trigger ��ư Ȱ��ȭ ���� ó��
	if((gm_OHTStatus->m_OHTStatus.IsCIDPause  ==OFF) &&
	   (gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag ==ON) &&
	   (gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON)&&
	   (gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling == CYCLE::CycleNotExist) &&
	   (bMapMakeMoveEnd ==false))
	   {
			btnMapMake->Enabled = true;
	   }
	   else
	   {
       	 	btnMapMake->Enabled = false;
	   }

	if((gm_OHTStatus->m_OHTStatus.IsCIDPause  ==OFF) &&
	   (gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag ==ON) &&
	   (gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON)&&
	   (gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling == CYCLE::CycleNotExist) &&
	   (gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust_Modify ==ON))
	   {
			BtnMapMakeMoveCheck->Enabled = true;
	   }
	   else
	   {
			BtnMapMakeMoveCheck->Enabled = false;
	   }


}
//---------------------------------------------------------------------------

void __fastcall TMapMakerForm::btnMapMakeClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn;

	AnsiString strMachineID = "TP001";


	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
        return;
    }

    if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
    }

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
    {
        Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
        else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
        }

    }


	if((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == OFF) ||
		(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==OFF))
	{
		Warnning_Display(COLOR_RED, "������ Map�� �ƴմϴ�.");
		return ;
	}
	else if(StrToInt(edtMoveEnd->Text) == gm_OHTStatus->m_OHTStatus.Data.CurNodeID)
	{
		Warnning_Display(COLOR_RED, "In Use On the Same Node(CurrentNode=TargetNode)");
		return ;
	}
	else if(gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling != CYCLE::CycleNotExist)
	{
		Warnning_Display(COLOR_RED, "Cycle ������");
		return ;

	}


	// ���ο� TP �̵���� ��Ŷ�� �̿��Ѵ�.
	iPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
	PKT_TP2OHT_MOVE_BY_MAP	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_BY_MAP; // TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)

	// ---- Body Set
	// ������ ��� Ÿ�� �Է�
	SEND_PACKET.BODY.IsGoStation = FALSE;

	// ������ ��ȣ �Է�
	SEND_PACKET.BODY.StopNodeID = StrToInt (edtMoveEnd->Text);

	//Map Maker ��� ����
	SEND_PACKET.BODY.PortType = 0x01;
	SEND_PACKET.BODY.IsMapMaker = 0x01;

//	String strMsg = NULL;
//	strMsg.sprintf(L"Steering Check ����� ����ϰڽ��ϱ�?\n");
//
//	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
//	{
	SEND_PACKET.BODY.IsSteerCheck = 0x01;
//	}else
//	{
//		SEND_PACKET.BODY.IsSteerCheck = 0x00;
//	}

	// ������ ����
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void TMapMakerForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}





void __fastcall TMapMakerForm::BtnMapMakeMoveCheckClick(TObject *Sender)
{
/*
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn;

	AnsiString strMachineID = "TP001";


	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
    {
        Warnning_Display(COLOR_RED, "OHT Mode is not Manual Mode.........");
        return;
    }

    if((gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_ERROR_LIMIT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_F_AMP_FAULT)
        || (gm_OHTStatus->m_OHTStatus.Data.ErrorCode == ERR_AXIS1_R_AMP_FAULT)	)
    {
        Warnning_Display(COLOR_RED, "Driving Axis1 Error.........");
        return;
    }

    if( (gm_OHTStatus->m_OHTStatus.Data.ErrorCode) &&
		((gm_OHTStatus->IOInput->uInputCommon.STEER_F_Left != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Left) ||
		(gm_OHTStatus->IOInput->uInputCommon.STEER_F_Right != gm_OHTStatus->IOInput->uInputCommon.STEER_R_Right)))
    {
        Warnning_Display(COLOR_RED, "Front/Rear Steering is Not Same direction.........");
        return;
    }

    if(CmdOHTCheck.Is_Executable(EXCLUDE_DRIVING_RUNNING) == NO_ERR)
    {
        if(fabs(gm_OHTStatus->Motor_Hoist->dPosition_cts) > HOIST_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Hoist Not Origin...#1 can't Move...");
            return;
        }
        else if(fabs(gm_OHTStatus->Motor_Shift->dPosition_cts) > SHIFT_ORIGIN_LIMIT)
        {
            Warnning_Display(COLOR_RED, "Shift Not Origin...#1 can't Move...");
            return;
        }
    }

    if((gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_70) || (gm_OHTStatus->m_OHTInfo.OHTInfo.OHT_Ver == OHT_VERSION_71))
    {
		if(gm_OHTStatus->IOInput->uInputCommon.FOUP_Detect == true)
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_EXIST...");
                return;
            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Close == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Close == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_CLOSE...");
                return;
            }
        }
        else
        {
			if (gm_OHTStatus->IOInput->uInputCommon.HAND_Open == false ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Close == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Right_Grip == true ||
				gm_OHTStatus->IOInput->uInputCommon.HAND_Left_Grip == true)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_HAND_CAST_NOTEXIST...");
                return;

            }

			if (gm_OHTStatus->IOInput->uInputCommon.SHUTTER_F_Open == false ||
                gm_OHTStatus->IOInput->uInputCommon.SHUTTER_R_Open == false)
            {
                Warnning_Display(COLOR_RED, "ERROR : ERR_SHUTTER_NOT_OPEN...");
                return;
            }
        }

    }


	if((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == OFF) ||
		(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==OFF))
	{
		Warnning_Display(COLOR_RED, "������ Map�� �ƴմϴ�.");
		return ;
	}
	else if(StrToInt(edtMoveEnd->Text) == gm_OHTStatus->m_OHTStatus.Data.CurNodeID)
	{
		Warnning_Display(COLOR_RED, "In Use On the Same Node(CurrentNode=TargetNode)");
		return ;
	}
	else if(gm_OHTStatus->m_OHTStatus.MapMakeState.IsCycling != CYCLE::CycleNotExist)
	{
		Warnning_Display(COLOR_RED, "Cycle ������");
		return ;

	}


	// ���ο� TP �̵���� ��Ŷ�� �̿��Ѵ�.
	iPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
	PKT_TP2OHT_MOVE_BY_MAP	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_BY_MAP; // TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)

	// ---- Body Set
	// ������ ��� Ÿ�� �Է�
	SEND_PACKET.BODY.IsGoStation = FALSE;

	// ������ ��ȣ �Է�
	SEND_PACKET.BODY.StopNodeID = StrToInt (edtMoveEnd->Text);

	//Map Maker ��� ����
	SEND_PACKET.BODY.PortType = 0x01;
	SEND_PACKET.BODY.IsMapMaker = 0x02;

//	String strMsg = NULL;
//	strMsg.sprintf(L"Steering Check ����� ����ϰڽ��ϱ�?\n");
//
//	if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_YESNO) == IDYES )
//	{
//	SEND_PACKET.BODY.IsSteerCheck = 0x01;
//	}else
//	{
//		SEND_PACKET.BODY.IsSteerCheck = 0x00;
//	}

	// ������ ����
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");

		*/
}
//---------------------------------------------------------------------------

void __fastcall TMapMakerForm::btnSolutionClick(TObject *Sender)
{
	String strMapInfo ="";
	strMapInfo.sprintf(L"[���⿡�� ��ġ ���] ");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"	1)�бⰡ �ƴ� ���, ���� ��ġ�� ������ ��ġ���� MapMake������ 	2) �б��� ���, �ش� ��ġ ���� Node ���� Ȯ�� �� ����Node �̵�");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"[CID-P�� ��� ��ġ ���]");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"	1)CID ���� Ȯ�� 	2) Bypass-CID���� 	3)�����簳ó��");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"[E305 ��� ��ġ ���]");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"	1)E305Tap�̵�	 2) �Ÿ����� Ȯ�� �� �ӽ� �Է�	 3)MapMake ������");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"[E405,ERR_BCR_READ ��� ��ġ ���] ");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"	1)Driving Tag ��ü(BCR ���ν� �� �νĺҷ�)   2) MapMake ������");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"[E407,ERR_BCR_READ_TRANS��� ��ġ ���]");
	memoMapMaker->Lines->Add(strMapInfo);

	strMapInfo = "";
	strMapInfo.sprintf(L"	1)Station Tag ��ü(BCR/�ݻ��� ���ν� �� �νĺҷ�)   2) MapMake ������");
	memoMapMaker->Lines->Add(strMapInfo);


}
//---------------------------------------------------------------------------

