//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwJobOrderResponse.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmJobOrderResponse *frmJobOrderResponse;
//---------------------------------------------------------------------------
__fastcall TfrmJobOrderResponse::TfrmJobOrderResponse(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJobOrderResponse::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmJobOrderResponse::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmJobOrderResponse::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmJobOrderResponse::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_JOB_ORDER_RESPONSE_ID_122);
	int iReturn, iTemp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_JOB_ORDER_RESPONSE_ID_122	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	// ---- Header ----------------------------------------
	SEND_PACKET.HEADER.MachineType = frmOption->iOpt_MachineType;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strMachineID.c_str(), frmOption->strMachineID.Length());

	// ---- Tag -------------------------------------------
	SEND_PACKET.TAG.Value = TagID::JOB_ORDER_RESPONSE;	// �۾�����

	// --- Status -----------------------------------------
	// SEND_PACKET.STATUS.WorkingStatus = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	iTemp = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.WorkingStatus, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboRunningStatus->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.RunningStatus, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboCarriage->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.Carriage, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboMaintananceState->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.MaintananceState, &iTemp, 1);
	iTemp = CboIndex_ToHex(cboCarryType->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.CarryType, &iTemp, 1);

	// ---
	SEND_PACKET.STATUS.CurrentAddr[HIGH_BYTE] = StrToInt(edtCurAddr_H->Text);
	SEND_PACKET.STATUS.CurrentAddr[MID_BYTE] = StrToInt(edtCurAddr_M->Text);
	SEND_PACKET.STATUS.CurrentAddr[LOW_BYTE] = StrToInt(edtCurAddr_L->Text);

	SEND_PACKET.STATUS.StopNowAddr[HIGH_BYTE] = StrToInt(edtStopAddr_H->Text);
	SEND_PACKET.STATUS.StopNowAddr[MID_BYTE] = StrToInt(edtStopAddr_M->Text);
	SEND_PACKET.STATUS.StopNowAddr[LOW_BYTE] = StrToInt(edtStopAddr_L->Text);

	SEND_PACKET.STATUS.DestinationAddr[HIGH_BYTE] = StrToInt(edtDestAddr_H->Text);
	SEND_PACKET.STATUS.DestinationAddr[MID_BYTE] = StrToInt(edtDestAddr_M->Text);
	SEND_PACKET.STATUS.DestinationAddr[LOW_BYTE] = StrToInt(edtDestAddr_L->Text);

	iTemp = CboIndex_ToHex(cboTunDirection->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.TunDirection, &iTemp, 1);
	SEND_PACKET.STATUS.RunningDistance = StrToInt(edtRunningDistance->Text);

	// --- ������ ����Ŭ ó��
	iTemp = cboRunningCycle->ItemIndex;

	// (1) �޺��ε��� 0 ~ 9���� ó��
	if (iTemp < 10)
		;	// do nothing
	// (2) �޺� �ε��� 10 ~ 13����. �ε��� 10�̸� 0x2C(44) ������� ���;� ��. ���� 34 ���Ѵ�
	else if((iTemp > 9) && (iTemp < 14))
		iTemp = iTemp + 34;		// 0x2C ~ 0x2F���� ó���� ����
	// (3) �޺� �ε��� 14 ~ ������. �ε��� 14�� 0x21 ���;� ��. ���� 19 ���Ѵ�
	else if (iTemp > 13)
		iTemp = iTemp + 19;		// 0x2C ~ 0x2F���� ó���� ����

	// memcpy(&SEND_PACKET.STATUS.RunningCycle, &iTemp, 1);
	SEND_PACKET.STATUS.RunningCycle = (unsigned char) iTemp;

	// --- ��� ����Ŭ ó��
	iTemp = cboWaitingCycle->ItemIndex;

	// (1) �޺��ε��� 0 ~ 9���� ó��
	if (iTemp < 10)
		;	// do nothing
	// (2) �޺� �ε��� 10 ~ 13����. �ε��� 10�̸� 0x2C(44) ������� ���;� ��. ���� 34 ���Ѵ�
	else if((iTemp > 9) && (iTemp < 14))
		iTemp = iTemp + 34;		// 0x2C ~ 0x2F���� ó���� ����
	// (3) �޺� �ε��� 14 ~ ������. �ε��� 14�� 0x21 ���;� ��. ���� 19 ���Ѵ�
	else if (iTemp > 13)
		iTemp = iTemp + 19;		// 0x2C ~ 0x2F���� ó���� ����

	// memcpy(&SEND_PACKET.STATUS.WaitingCycle, &iTemp, 1);
	// ---
	SEND_PACKET.STATUS.WaitingCycle = (unsigned char) iTemp;
	SEND_PACKET.STATUS.ErrorCode = StrToInt(edtErrorCode->Text);
	SEND_PACKET.STATUS.APSinalLevel = StrToInt(edtRadioWaveLevel->Text);
	//SEND_PACKET.STATUS.EmStatus = StrToInt(edtEmStatus->Text);
	iTemp = StrToInt(edtDataTransStatus->Text);
	memcpy(&SEND_PACKET.STATUS.DataTransStatus, &iTemp, 1);

	// --- Body -----------------------------------------
	SEND_PACKET.BODY.CmdID = StrToInt(edtOrderID->Text);

	// ���䱸�� �ڵ� ����
	if(chkNotAccept->Checked == true)
	{
		if(rdoVhlError->Checked == true)
		{
			iTemp = bit_7 + (choVhlError->ItemIndex + 1);
		}
		else
		{
			iTemp = bit_7 + bit_6 + (choNotVhlError->ItemIndex + 1);
		}
	}
	else
	{
		iTemp = 0;
	}

	memcpy(&SEND_PACKET.BODY.ResponseAck, &iTemp, 1);

	// ���ñ��� �ڵ� ����
	if(chkTestMode->Checked == true)
	{
		iTemp = bit_7 + (cboOrderSep->ItemIndex + 1);
	}
	else
	{
		iTemp = cboOrderSep->ItemIndex + 1;
	}

	memcpy(&SEND_PACKET.BODY.ResponseDiv, &iTemp, 1);

	// ���� ����Ŭ ����
	/*
	if(rdoOrderPort1->Checked == true)
	{
		iTemp = bit_6; 	// ��1 ���ϴ�
	}
	else
	{
		iTemp = bit_7; 	// ��2 ���ϴ�
	}

	if(cboOrderCycle->ItemIndex == 9)
	{
		iTemp = iTemp + 0x2C;	// ���� ����Ŭ
	}
	else if(cboOrderCycle->ItemIndex == 10)
	{
		iTemp = iTemp + 0x2E;	// ���� �̵� ����Ŭ
	}
	else
	{
		iTemp = iTemp + (cboOrderCycle->ItemIndex + 1);
	}
	*/
	iTemp = cboOrderCycle->ItemIndex;
	memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

	// ������ Node
	SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = StrToInt(edtTargetNode_H->Text);
	SEND_PACKET.BODY.StopNodeID[MID_BYTE] = StrToInt(edtTargetNode_M->Text);
	SEND_PACKET.BODY.StopNodeID[LOW_BYTE] = StrToInt(edtTargetNode_L->Text);

	// ������ Station Node
	SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = StrToInt(edtTargetStationNode_H->Text);
	SEND_PACKET.BODY.StopStationID[MID_BYTE] = StrToInt(edtTargetStationNode_M->Text);
	SEND_PACKET.BODY.StopStationID[LOW_BYTE] = StrToInt(edtTargetStationNode_L->Text);

	// �켱�� ����
	iTemp = StrToInt(edtPriority->Text);
	if (iTemp > 127)
		iTemp = 127;

	if (iTemp < 0)
		iTemp = 0;

	if (chkHotLot->Checked)
		iTemp = iTemp + bit_7;

	memcpy(&SEND_PACKET.BODY.Riv, &iTemp, 1);

	// ------ ��Ŷ ���� ---------------------------------
	Conv_PKT_JOB_ORDER_RESPONSE_ID_122(SEND_PACKET);
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void __fastcall TfrmJobOrderResponse::chkNotAcceptClick(TObject *Sender)
{
	if(chkNotAccept->Checked == true)
		grpNotAcceptReason->Enabled = true;
	else
		grpNotAcceptReason->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderResponse::rdoVhlErrorClick(TObject *Sender)
{
	if(rdoVhlError->Checked == true)
	{
		choVhlError->Enabled 	= true;
		choNotVhlError->Enabled = false;
	}
	else
	{
		choVhlError->Enabled 	= false;
		choNotVhlError->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobOrderResponse::rdoNotVhlErrorClick(TObject *Sender)
{
	if(rdoVhlError->Checked == true)
	{
		choVhlError->Enabled 	= true;
		choNotVhlError->Enabled = false;
	}
	else
	{
		choVhlError->Enabled 	= false;
		choNotVhlError->Enabled = true;
	}
}
//---------------------------------------------------------------------------


