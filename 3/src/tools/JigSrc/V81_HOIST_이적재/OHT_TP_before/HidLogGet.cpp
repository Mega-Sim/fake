//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "HidLogGet.h"
#include "OHTInfoUnit.h"
#include "Def_OHT.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmHidLogGet *frmHidLogGet;
//---------------------------------------------------------------------------
__fastcall TfrmHidLogGet::TfrmHidLogGet(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmHidLogGet::btnStartClick(TObject *Sender)
{
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Mode == MODE_AUTO)
	{
		ShowMessage(L"�޴��� ���¿����� ��� �����մϴ�!");
		return;
	}
	else
	{
		MainForm->timerStatusRequest->Enabled = false;

		// ���⼭ ��� ����
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

		SEND_PACKET.TAG.Value = TagID::CMD_TP_HID_LOG_SAVE_ORDER;
		// SEND_PACKET.BODY.iVar1 = 0x01;	// sound on

		m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

		delete m_CMDControl;
	}

    Display_Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmHidLogGet::btnCloseClick(TObject *Sender)
{
	Hide();
}
//---------------------------------------------------------------------------

/*
	��ô��Ȳ : 1���� ����. 1-����. 2-�Ϸ�
	�����ڵ� : 0 : No error.   1 : Serial Open����
*/
#define HID_RESULT_RESPONSE	1	// ������ ����
#define HID_RESULT_COMPLETE 2	// ���� �Ϸ�

#define HID_ERROR_NO_ERR				0				// ������ �������� �Ϸ�
#define HID_ERROR_SERIAL_CAN_T_OPEN 	420	// ERR_HID_OPEN	// �ø�����Ʈ�� ���� ������
#define HID_ERROR_SERIAL_WRITE			421	// ERR_HID_WRITE	// �ø��� ������ ����
#define HID_ERROR_SERIAL_TIMEOUT		333

void TfrmHidLogGet::Display_Result(int iVar1, int iVar2)
{
	// �μ�1�� ���� ���
	switch(iVar1)
	{
		case HID_RESULT_RESPONSE:
				panCmd_Response->Color = clLime;
		break;

		case HID_RESULT_COMPLETE:
				panCmd_Response->Color = clLime;
				panCmd_Complete->Color = clLime;
		break;

		default:
				panCmd_Response->Color = clLime;
				panCmd_Complete->Color = clLime;
		break;
    }

	// �μ�2�� �����ڵ�
	if (iVar1 == HID_RESULT_COMPLETE)
	{
		switch(iVar2)
		{
			case HID_ERROR_NO_ERR:
				edtHelp->Text = "���� ���� �Ϸ���";
			break;

			case HID_ERROR_SERIAL_CAN_T_OPEN:
				edtHelp->Text = "�ø��� ��Ʈ Open Fail";
			break;

			case HID_ERROR_SERIAL_WRITE:
				edtHelp->Text = "�ø��� ��Ʈ Write Fail";
			break;

			case HID_ERROR_SERIAL_TIMEOUT:
				edtHelp->Text = "Time Out";
			break;

			default:
				edtHelp->Text = "�˼� ���� ����";
			break;
		}

		MainForm->timerStatusRequest->Enabled = true;
	}
}



void TfrmHidLogGet::Display_Clear(void)
{
	panCmd_Start->Color = clLime;
	panCmd_Response->Color = clWhite;
	panCmd_Complete->Color = clWhite;

	edtHelp->Text = "";
}
