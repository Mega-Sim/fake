//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ClwStatusReport.h"
#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmClwStatusReport *frmClwStatusReport;



//---------------------------------------------------------------------------
__fastcall TfrmClwStatusReport::TfrmClwStatusReport(TComponent* Owner)
	: TForm(Owner)
{
#ifdef IS_MCP
	btnSend->Visible = false;	// OHT�� MCP�� �����ϴ� ���̹Ƿ�..
#endif

	//int a = sizeof(WORKINGSTATUS);
	//AnsiString strA;

	//strA.sprintf("��Ʈ�ʵ� ����ü ũ��� %d  �̷����� ũ�Ⱚ�� 1�Դϴ�", a);
	//ShowMessage(strA);

	//WORKINGSTATUS wTest = {0,0,0,0,0,0,0,0};;
	//wTest = {0,0,0,0,0,0,0,0};	// ����
	//wTest.NowRunning = 1;

	// cboWorkingStatus->ItemIndex = Get_CboItemIndex(wTest);
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwStatusReport::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);
	int iReturn, iTemp;

	int a = '2';
	int b = '3';

	int c = '0';
	int d = '1';


	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_STATUS_REPORT_ID_03	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	// ---- Header ------------------------------------------------------
	SEND_PACKET.HEADER.MachineType = frmOption->iOpt_MachineType;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(&SEND_PACKET.HEADER.MachineID, frmOption->strMachineID.c_str(), frmOption->strMachineID.Length());

	// ---- Tag ---------------------------------------------------------
	SEND_PACKET.TAG.Value = TagID::CLW_TO_MCP_STATUS;	// �۾�����

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

	// --- BODY -------------------------------------------------------
	// SEND_PACKET.BODY.CmdID = StrToInt(edtOrderID->Text);


	// --- Packet send ------------------------------------------------
	SEND_PACKET.STATUS.ErrorCode = StrToInt(edtErrorCode->Text);
	SEND_PACKET.STATUS.APSinalLevel = StrToInt(edtRadioWaveLevel->Text);
	// SEND_PACKET.STATUS.EmStatus = StrToInt(edtEmStatus->Text);
	iTemp = StrToInt(edtDataTransStatus->Text);
	memcpy(&SEND_PACKET.STATUS.DataTransStatus, &iTemp, 1);

	// ---- Tag ---------------------------------------------------------



	// ------ ��Ŷ ����
	Conv_PKT_STATUS_REPORT_ID_03(SEND_PACKET);
	iReturn = frmUDP->pUdpObject->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwStatusReport::btnCloseClick(TObject *Sender)
{
	// ShowWindow(this->Handle, SW_HIDE);
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmClwStatusReport::FormClose(TObject *Sender, TCloseAction &Action)

{
	Action = caFree;
}
//---------------------------------------------------------------------------

// �� �κ��� OCS, OHT�� Ư���� ������ ���� ������, �����δ� OHT->OCS��
// ��츸 ����ϰ� �� ���ϴ�
void TfrmClwStatusReport::DisplayInform(PKT_STATUS_REPORT_ID_03 Pkt)
{
	AnsiString strDisplay;

	strDisplay .sprintf("MachineType : %d MachineID : %s Tag : %d  WorkingStatus : %d",
					     Pkt.HEADER.MachineType, Pkt.HEADER.MachineID, Pkt.TAG.Value, Pkt.STATUS.WorkingStatus);

	ShowMessage(strDisplay);

}

//---------------------------------------------------------------------------

