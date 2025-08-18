//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ResponsePasspermitOCS.h"

// #include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmResponsePasspermitOCS *frmResponsePasspermitOCS;
//---------------------------------------------------------------------------
__fastcall TfrmResponsePasspermitOCS::TfrmResponsePasspermitOCS(TComponent* Owner)
	: TForm(Owner)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------
void __fastcall TfrmResponsePasspermitOCS::FormClose(TObject *Sender, TCloseAction &Action)

{
	// ���� ������ ����
	if(strCurrentFileName == "frmResponsePasspermitOCS.SYS")
		SaveFile("frmResponsePasspermitOCS.SYS");

	  Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmResponsePasspermitOCS::btnGetOrderIDClick(TObject *Sender)
{
	int iOrderID = GetOrderID();
	edtOrderID->Text = IntToStr(iOrderID);
}
//---------------------------------------------------------------------------

void __fastcall TfrmResponsePasspermitOCS::btnCloseClick(TObject *Sender)
{

	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmResponsePasspermitOCS::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242);
	int iReturn, iTemp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::RESPONSE_PASSPERMIT_OCS2OHT;		// ��� �㰡 �䱸 ����

	// --- Status
	iTemp = 0;
	if(chkStatus_0->Checked)   iTemp = iTemp + bit_0;
	if(chkStatus_1->Checked)   iTemp = iTemp + bit_1;
	if(chkStatus_2->Checked)   iTemp = iTemp + bit_2;
	if(chkStatus_3->Checked)   iTemp = iTemp + bit_3;

	if(chkStatus_4->Checked)   iTemp = iTemp + bit_4;
	if(chkStatus_5->Checked)   iTemp = iTemp + bit_5;
	if(chkStatus_6->Checked)   iTemp = iTemp + bit_6;
	if(chkStatus_7->Checked)   iTemp = iTemp + bit_7;

	memcpy(&SEND_PACKET.STATUS.StatusOrder, &iTemp, 1);
	// ----
	iTemp = 0;
	if(chkStopOrder_0->Checked)   iTemp = iTemp + bit_0;
	if(chkStopOrder_1->Checked)   iTemp = iTemp + bit_1;
	if(chkStopOrder_2->Checked)   iTemp = iTemp + bit_2;
	if(chkStopOrder_3->Checked)   iTemp = iTemp + bit_3;

	if(chkStopOrder_4->Checked)   iTemp = iTemp + bit_4;
	if(chkStopOrder_5->Checked)   iTemp = iTemp + bit_5;
	if(chkStopOrder_6->Checked)   iTemp = iTemp + bit_6;
	if(chkStopOrder_7->Checked)   iTemp = iTemp + bit_7;

	memcpy(&SEND_PACKET.STATUS.StopOrder, &iTemp, 1);
	// ------

	SEND_PACKET.STATUS.StopReason = (unsigned char)cboStopReason->ItemIndex;

	iTemp = CboIndex_ToHex((unsigned char)cboMaintananceOrder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.MaintananceOrder, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboEmOder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.EmOder, &iTemp, 1);

	iTemp = CboIndex_ToHex((unsigned char)cboDataTransOrder->ItemIndex);
	memcpy(&SEND_PACKET.STATUS.DataTransOrder, &iTemp, 1);

	// --- Body
	SEND_PACKET.BODY.PassPermit_CM.SQNum = StrToInt(edtOrderID->Text);	// ������ �ѹ�

	// Stop����
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[HIGH_BYTE] = StrToInt(edtTargetStop_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[MID_BYTE]  = StrToInt(edtTargetStop_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.StopNodeID[LOW_BYTE]  = StrToInt(edtTargetStop_L->Text);

	// Reset���� Main
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[HIGH_BYTE] = StrToInt(edtResetMain_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[MID_BYTE]  = StrToInt(edtResetMain_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Main[LOW_BYTE]  = StrToInt(edtResetMain_L->Text);

	// Reset���� Sub
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[HIGH_BYTE] = StrToInt(edtResetSub_H->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[MID_BYTE]  = StrToInt(edtResetSub_M->Text);
	SEND_PACKET.BODY.PassPermit_CM.ResetNodeID_Sub[LOW_BYTE]  = StrToInt(edtResetSub_L->Text);

	// ����ӵ� ����
	if(rdoPassNo->Checked == true)
		SEND_PACKET.BODY.AckType = 0x00;	// ����Ұ�
	else if(rdoPassNormal->Checked == true)
		SEND_PACKET.BODY.AckType = 0x40;	// ����ӵ�
	else
		SEND_PACKET.BODY.AckType = StrToInt(edtPassSpeed->Text);

	// �������
	if(rdoResultOk->Checked == true)
		SEND_PACKET.BODY.ResultType = 0x00;					// ����
	else if(rdoResultMcpOther->Checked == true)				// MCP ������ ��
		SEND_PACKET.BODY.ResultType = 0x01;
	else if(rdoResultClwConfirmOk->Checked == true)			// �ٸ� CLW�� ��� ����
		SEND_PACKET.BODY.ResultType = 0x02;
	else if(rdoResultMcpInStart_NoTouch->Checked == true)	// MCP�� ���� ���̹Ƿ� ���� ����
		SEND_PACKET.BODY.ResultType = 0x03;	
	else
		SEND_PACKET.BODY.ResultType = 0x04;					// CID ��� �Ұ�

	// ������ ����
        PKT_OCS2OHT	OhtCmdPkt;
        memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);

	Conv_PKT_RESPONSE_PASSPERMIT_OCS2OHT_ID_242(SEND_PACKET);
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------

void TfrmResponsePasspermitOCS::Trigger(PKT_PASSPERMIT_OHT2OCS_ID_241 Pkt)
{
	// Pkt 241 infomation display (body only)
	edtOrderID->Text = IntToStr(Pkt.BODY.PassPermit_CM.SQNum);

	edtTargetStop_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[HIGH_BYTE]);
	edtTargetStop_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[MID_BYTE]);
	edtTargetStop_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.StopNodeID[LOW_BYTE]);

	edtResetMain_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[HIGH_BYTE]);
	edtResetMain_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[MID_BYTE]);
	edtResetMain_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Main[LOW_BYTE]);

	edtResetSub_H->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[HIGH_BYTE]);
	edtResetSub_M->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[MID_BYTE]);
	edtResetSub_L->Text = IntToStr(Pkt.BODY.PassPermit_CM.ResetNodeID_Sub[LOW_BYTE]);

	// Auto send
	if(chkAutoReply->Checked)
	{
		btnSendClick(NULL);
		mBSLog->Lines->Add(L"�ڵ� ���۵Ǿ����ϴ�!");
  	}

}

//---------------------------------------------------------------------------

void __fastcall TfrmResponsePasspermitOCS::btnDelClick(TObject *Sender)
{
	mBSLog->Lines->Clear();
}
//---------------------------------------------------------------------------
// true = auto mode, false = normal mode
void TfrmResponsePasspermitOCS::SetButtonMode(bool bAutoModeChild)
{
	if(bAutoModeChild == true)
	{
		btnClose->Visible = false;
		btnHide->Visible  = true;
	}
	else
	{
		btnClose->Visible = true;
		btnHide->Visible  = false;
	}
}
//---------------------------------------------------------------------------

void TfrmResponsePasspermitOCS::SetResponseMode(int iResponseType)
{
	if(iResponseType == 0)		// �׻� Ack
	{
		rdoPassNormal->Checked = true;
		chkAutoReply->Checked = true;
	}
	else if(iResponseType == 1)	// �׻� Nak
	{
		rdoPassNo->Checked = true;
		chkAutoReply->Checked = true;
	}
	else	// ������
	{
		chkAutoReply->Checked = false;
    }
}

//---------------------------------------------------------------------------
void TfrmResponsePasspermitOCS::SaveFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "����㰡 �䱸 Save : " + strFileName;
	ShowMessage(strMsg);  */

	FILE *stream;

	try
	{
		// --------------- ���� ���� -----------------
		stream = fopen(strFileName.c_str(), "w+");
		if(stream == NULL)
		{
			ShowMessage(L"������ ������ Open����! - frmResponsePasspermitOCS");
			return;
		}

		// --------------- ���� ���� -----------------
		// 1) ��������
		fprintf(stream, "%d %d %d %d %d %d %d %d \r\n",
				chkStatus_0->Checked, chkStatus_1->Checked, chkStatus_2->Checked, chkStatus_3->Checked,
				chkStatus_4->Checked, chkStatus_5->Checked, chkStatus_6->Checked, chkStatus_7->Checked);
		// 2) ��������
		fprintf(stream, "%d %d %d %d %d %d %d %d \r\n",
				chkStopOrder_0->Checked, chkStopOrder_1->Checked, chkStopOrder_2->Checked, chkStopOrder_3->Checked,
				chkStopOrder_4->Checked, chkStopOrder_5->Checked, chkStopOrder_6->Checked, chkStopOrder_7->Checked);

		// 3) ��������, �������� ����, EM ����, ������ ��������, ����㰡 �ڵ�����
		fprintf(stream, "%d %d %d %d %d \r\n",
				cboStopReason->ItemIndex, cboMaintananceOrder->ItemIndex, cboEmOder->ItemIndex,
				cboDataTransOrder->ItemIndex, chkAutoReply->Checked);

		// Body����
		// 4) ������ �ѹ�
		fprintf(stream, "%d \r\n", StrToInt(edtOrderID->Text));

		// 5) Stop ����
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtTargetStop_H->Text), StrToInt(edtTargetStop_M->Text), StrToInt(edtTargetStop_L->Text));

		// 6) Reset���� (Main)
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtResetMain_H->Text), StrToInt(edtResetMain_M->Text), StrToInt(edtResetMain_L->Text));

		// 7) Reset ���� (Sub)
		fprintf(stream, "%d %d %d \r\n", StrToInt(edtResetSub_H->Text), StrToInt(edtResetSub_M->Text), StrToInt(edtResetSub_L->Text));

		// 8) ���䱸��
	   fprintf(stream, "%d %d %d %d \r\n", rdoPassNo->Checked, rdoPassNormal->Checked, rdoPassUser->Checked, StrToInt(edtPassSpeed->Text));

		// 9) �������
		fprintf(stream, "%d %d %d %d %d \r\n", rdoResultOk->Checked, rdoResultMcpOther->Checked, rdoResultClwConfirmOk->Checked, 
											   rdoResultMcpInStart_NoTouch->Checked, rdoResultMcpInStart_CID_CantUse->Checked);


		// �ݱ�
		if(stream)
			fclose(stream);
		stream = NULL;
	}
	catch(...)
	{
		if(stream)
			fclose(stream);
		stream = NULL;
	}
}

//---------------------------------------------------------------------------
void TfrmResponsePasspermitOCS::LoadFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "����㰡 �䱸 Load : " + strFileName;
	ShowMessage(strMsg);  */

	int iTmp[8];
	int iVar;
	char strTmp[7][1024];

	FILE *fi;

	strCurrentFileName = strFileName;

	try
	{
		// --------------- ���� ���� -----------------
		if( (fi = fopen( strFileName.c_str(), "r" )) == NULL )
		{
			// ShowMessage(L"���� ���Ⱑ �����߽��ϴ�");
			return;
		}

		// --------------- ���� �б� -----------------
		// 1) ��������
		// 1) ��������
		fscanf(fi,"%d %d %d %d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4], &iTmp[5], &iTmp[6], &iTmp[7]);
		chkStatus_0->Checked = iTmp[0];
		chkStatus_1->Checked = iTmp[1];
		chkStatus_2->Checked = iTmp[2];
		chkStatus_3->Checked = iTmp[3];
		chkStatus_4->Checked = iTmp[4];
		chkStatus_5->Checked = iTmp[5];
		chkStatus_6->Checked = iTmp[6];
		chkStatus_7->Checked = iTmp[7];

		// 2) ��������
		fscanf(fi,"%d %d %d %d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4], &iTmp[5], &iTmp[6], &iTmp[7]);
		chkStopOrder_0->Checked = iTmp[0];
		chkStopOrder_1->Checked = iTmp[1];
		chkStopOrder_2->Checked = iTmp[2];
		chkStopOrder_3->Checked = iTmp[3];
		chkStopOrder_4->Checked = iTmp[4];
		chkStopOrder_5->Checked = iTmp[5];
		chkStopOrder_6->Checked = iTmp[6];
		chkStopOrder_7->Checked = iTmp[7];


		// 3) ��������, �������� ����, EM ����, ������ ��������, ����㰡 �ڵ�����
		fscanf(fi,"%d %d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4]);
		cboStopReason->ItemIndex 		= iTmp[0];
		cboMaintananceOrder->ItemIndex	= iTmp[1];
		cboEmOder->ItemIndex 			= iTmp[2];
		cboDataTransOrder->ItemIndex 	= iTmp[3];
		chkAutoReply->Checked 			= iTmp[4];

		// Body����
		// 4) ������ �ѹ�
		fscanf(fi,"%d \n", &iVar);
		edtOrderID->Text = IntToStr(iVar);

		// 5) Stop ����
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtTargetStop_H->Text = IntToStr(iTmp[0]);
		edtTargetStop_M->Text = IntToStr(iTmp[1]);
		edtTargetStop_L->Text = IntToStr(iTmp[2]);

		// 6) Reset���� (Main)
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtResetMain_H->Text = IntToStr(iTmp[0]);
		edtResetMain_M->Text = IntToStr(iTmp[1]);
		edtResetMain_L->Text = IntToStr(iTmp[2]);

		// 7) Reset ���� (Sub)
		fscanf(fi,"%d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2]);
		edtResetSub_H->Text = IntToStr(iTmp[0]);
		edtResetSub_M->Text = IntToStr(iTmp[1]);
		edtResetSub_L->Text = IntToStr(iTmp[2]);

		// 8) ���䱸��
		fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);
		rdoPassNo->Checked 		= iTmp[0];
		rdoPassNormal->Checked	= iTmp[1];
		rdoPassUser->Checked	= iTmp[2];
		edtPassSpeed->Text 		= IntToStr(iTmp[3]);

		// 9) �������
		fscanf(fi,"%d %d %d %d %d\n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3], &iTmp[4]);
		rdoResultOk->Checked						= iTmp[0];
		rdoResultMcpOther->Checked 					= iTmp[1];
		rdoResultClwConfirmOk->Checked 				= iTmp[2];
		rdoResultMcpInStart_NoTouch->Checked 		= iTmp[3];
		rdoResultMcpInStart_CID_CantUse->Checked 	= iTmp[4];


		// ���� �ݱ�
		if(fi)
			fclose(fi);
		fi = NULL;

	}
	catch(...)
	{
		if(fi)
			fclose(fi);
		fi = NULL;
	}
}

//---------------------------------------------------------------------------
void __fastcall TfrmResponsePasspermitOCS::btnHideClick(TObject *Sender)
{
	ShowWindow(this->Handle, SW_HIDE);
}
//---------------------------------------------------------------------------




