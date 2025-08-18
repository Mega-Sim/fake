//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OcsStatusReport.h"

#include "Protocol_Define_Modify.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOcsStatusReport *frmOcsStatusReport;
//---------------------------------------------------------------------------
__fastcall TfrmOcsStatusReport::TfrmOcsStatusReport(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsStatusReport::FormClose(TObject *Sender, TCloseAction &Action)
{
	// ���� ������ ����
	if(strCurrentFileName == "frmOcsStatusReport.SYS")
		SaveFile("frmOcsStatusReport.SYS");

	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsStatusReport::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsStatusReport::btnSendClick(TObject *Sender)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
	int iReturn, iTemp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_OCS_STATUS_ID_01	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	// ---- Header
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&SEND_PACKET.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(SEND_PACKET.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::MCP_TO_CLW_STATUS;

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

	// ������ ����
        PKT_OCS2OHT	OhtCmdPkt;
        memcpy(&OhtCmdPkt, &SEND_PACKET, iPacketSize);


	Conv_PKT_OCS_STATUS_ID_01(SEND_PACKET);


	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

	if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
		ShowMessage(L"Address not registed!!, try first");
}
//---------------------------------------------------------------------------
void __fastcall TfrmOcsStatusReport::chkStatusAutoClick(TObject *Sender)
{
	if(chkStatusAuto->Checked)
		tmrStatusAuto->Enabled = true;
	else
		tmrStatusAuto->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOcsStatusReport::tmrStatusAutoTimer(TObject *Sender)
{
	static int iCount = 0;

	iCount++;

	if(iCount == 10)
	{
		iCount = 0;
		btnSendClick(NULL);
	}

	prgAutoSend->Position = iCount;

}
//---------------------------------------------------------------------------

void TfrmOcsStatusReport::SaveFile(AnsiString strFileName)
{
	/* AnsiString strMsg;
	strMsg = "�����㰡 �䱸 Save : " + strFileName;
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

		// 3) ��������, �������� ����, EM ����, ������ ��������
		fprintf(stream, "%d %d %d %d \r\n",
				cboStopReason->ItemIndex, cboMaintananceOrder->ItemIndex, cboEmOder->ItemIndex,
				cboDataTransOrder->ItemIndex);

		// 4) ���� �ڵ�����
		fprintf(stream, "%d \r\n", chkStatusAuto->Checked);


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
void TfrmOcsStatusReport::LoadFile(AnsiString strFileName)
{
	/*
	AnsiString strMsg;
	strMsg = "�����㰡 �䱸 Load : " + strFileName;
	ShowMessage(strMsg);
	*/
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


		// 3) ��������, �������� ����, EM ����, ������ ��������
		fscanf(fi,"%d %d %d %d  \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);
		cboStopReason->ItemIndex 		= iTmp[0];
		cboMaintananceOrder->ItemIndex	= iTmp[1];
		cboEmOder->ItemIndex 			= iTmp[2];
		cboDataTransOrder->ItemIndex 	= iTmp[3];

		// 4) 10�ʸ��� ������ ���
		fscanf(fi,"%d \n", &iVar);
		chkStatusAuto->Checked = iVar;

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
