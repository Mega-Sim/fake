//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwPassConfirmStatusReport.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmClwPassConfirmStatusReport *frmClwPassConfirmStatusReport;
//---------------------------------------------------------------------------
__fastcall TfrmClwPassConfirmStatusReport::TfrmClwPassConfirmStatusReport(TComponent* Owner)
	: TForm(Owner)
{
	FeederGrid->Cells[0][0]	= "No";
	FeederGrid->Cells[1][0]	= "������ �ѹ�";
	FeederGrid->Cells[2][0]	= "STOP ����";
	FeederGrid->Cells[3][0]	= "RESET ����(Main)";
	FeederGrid->Cells[4][0]	= "RESET ����(Sub)";

	for (int i = 0; i < 10; i++)
	{
		FeederGrid->Cells[0][i+1] = IntToStr(i + 1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwPassConfirmStatusReport::btnCloseClick(TObject *Sender)
{
		Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwPassConfirmStatusReport::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmClwPassConfirmStatusReport::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246);
	int iReturn, iTemp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	// ---- Header ------------------------------------------------------
	SEND_PACKET.HEADER.MachineType = frmOption->iOpt_MachineType;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(&SEND_PACKET.HEADER.MachineID, frmOption->strMachineID.c_str(), frmOption->strMachineID.Length());

	// ---- Tag ---------------------------------------------------------
	SEND_PACKET.TAG.Value = TagID::RESPONSE_PASSPERMITREQ_OHT2OCS;	// OHT -> OCS ��� �㰡 ���� ���� (ID:246)

	// --- Status -------------------------------------------------------
	// SEND_PACKET.STATUS.WorkingStatus = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	// iTemp = CboIndex_ToHex((unsigned char)cboWorkingStatus->ItemIndex);
	// memcpy(&SEND_PACKET.STATUS.WorkingStatus, &iTemp, 1);

	iTemp = 0;
	if(chkStatus_0->Checked)   iTemp = iTemp + bit_0;
	if(chkStatus_1->Checked)   iTemp = iTemp + bit_1;
	if(chkStatus_2->Checked)   iTemp = iTemp + bit_2;
	if(chkStatus_4->Checked)   iTemp = iTemp + bit_4;
	if(chkStatus_5->Checked)   iTemp = iTemp + bit_5;
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
	SEND_PACKET.STATUS.WaitingCycle = (unsigned char) iTemp;
	SEND_PACKET.STATUS.ErrorCode = StrToInt(edtErrorCode->Text);
	SEND_PACKET.STATUS.APSinalLevel = StrToInt(edtRadioWaveLevel->Text);
	// SEND_PACKET.STATUS.EmStatus = StrToInt(edtEmStatus->Text);

	iTemp = StrToInt(edtDataTransStatus->Text);
	memcpy(&SEND_PACKET.STATUS.DataTransStatus, &iTemp, 1);

	// --- Packet send ------------------------------------------------
	SEND_PACKET.STATUS.ErrorCode = StrToInt(edtErrorCode->Text);
	SEND_PACKET.STATUS.APSinalLevel = StrToInt(edtRadioWaveLevel->Text);
	// SEND_PACKET.STATUS.EmStatus = StrToInt(edtEmStatus->Text);
	iTemp = StrToInt(edtDataTransStatus->Text);
	memcpy(&SEND_PACKET.STATUS.DataTransStatus, &iTemp, 1);

	// --- BODY -------------------------------------------------------
	unsigned char cHigh, cMid, cLow;
	int iEnd = StrToInt(edtConfirmGetCount->Text);

	SEND_PACKET.BODY.Finish_PassPermitNum = iEnd;

	__try
	{
		for(int i = 0; i < iEnd; i++)
		{
			if(FeederGrid->Cells[1][i + 1] == "") {ShowMessage(L"������ �׸��� ����ֽ��ϴ�"); return; }
			SEND_PACKET.BODY.PassPermit_CM[i].SQNum = StrToInt(FeederGrid->Cells[1][i + 1]);	// ������ �ѹ�

			// STOP ����
			if(FeederGrid->Cells[2][i + 1] == "") {ShowMessage(L"STOP ���� �׸��� ����ֽ��ϴ�"); return; }
			iTemp = StrToInt(FeederGrid->Cells[2][i+1]);
			Int2Uchar3(iTemp,cHigh,cMid,cLow);
			SEND_PACKET.BODY.PassPermit_CM[i].StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PassPermit_CM[i].StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PassPermit_CM[i].StopNodeID[LOW_BYTE]  = cLow;

			// RESET ����(Main)
			if(FeederGrid->Cells[3][i + 1] == "") {ShowMessage(L"RESET ����(Main) �׸��� ����ֽ��ϴ�"); return; }
			iTemp = StrToInt(FeederGrid->Cells[3][i+1]);
			Int2Uchar3(iTemp,cHigh,cMid,cLow);
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Main[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Main[MID_BYTE] = cMid;
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Main[LOW_BYTE] = cLow;

			// RESET ����(Sub)
			if(FeederGrid->Cells[4][i + 1] == "") {ShowMessage(L"RESET ����(Sub) �׸��� ����ֽ��ϴ�"); return; }
			iTemp = StrToInt(FeederGrid->Cells[4][i+1]);
			Int2Uchar3(iTemp,cHigh,cMid,cLow);
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Sub[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Sub[MID_BYTE] = cMid;
			SEND_PACKET.BODY.PassPermit_CM[i].ResetNodeID_Sub[LOW_BYTE] = cLow;
		}
	}
	catch(...)
	{
		ShowMessage("����㰡 ���Ǽ��� �����ϰų� �׸��� ����� �Էµ��� �ʾҽ��ϴ�!");
		return;
	}


	// ------ ��Ŷ ����
	Conv_PKT_RESPONSE_PASSPERMITREQ_OHT2OCS_ID_246(SEND_PACKET);
	iReturn = frmUDP->pUdpObject->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
