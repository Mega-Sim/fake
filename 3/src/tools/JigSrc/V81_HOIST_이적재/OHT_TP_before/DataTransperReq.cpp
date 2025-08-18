//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DataTransperReq.h"

#include "CMDControlUnit.h"
#include "Def_TPCommand.h"
#include "SocketUnit.h"
#include "StatusUnit.h"
#include "OHTInfoUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmDataTransperReq *frmDataTransperReq;
//---------------------------------------------------------------------------
__fastcall TfrmDataTransperReq::TfrmDataTransperReq(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmDataTransperReq::btnSendClick(TObject *Sender)
{

	// �α� �����ֱ� �߰���. �����Ȳ�� ǥ��������
	Reset_Display();				// ������� ���븦 Ŭ����
	panCmd_Start->Color = clLime;   // ���������� ǥ������

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";

	// ��������  ��� ����ü
	iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS);
	PKT_CMD_DATA_TRANSPER_REQUEST_OHT2OCS	SEND_PACKET;

	// ����ü �ʱ�ȭ
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	SEND_PACKET.TAG.Value = TagID::REQ_DATATRANSFER_OHT_TP;		// ������ ���� �䱸


	// OHTREQUEST
	if(rdoSendParam->ItemIndex == 0)	// Ftp -> OHT �� ����
	{
		iTemp = 0;

	   if(((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust==ON)) &&
			((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust_Modify == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust_Modify ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust_Modify ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust_Modify==ON)))
			{

				String strMsg = NULL;
				strMsg.sprintf(L"������ Map/���� ���¿��� MapUpload�� �����Ͻðڽ��ϱ�?\n");

				if(Application->MessageBox(strMsg.w_str(), L"Confirm", MB_OKCANCEL) == IDOK )
				{
				  //EOF
				}else
				{
					return;
                }
            }

/*
		2017.03.13. ������ CheckBox ��� �ڵ���
		if(chkFtpOht_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkFtpOht_Req_1->Checked)   iTemp = iTemp + bit_1;
		if(chkFtpOht_Req_2->Checked)   iTemp = iTemp + bit_2;
		if(chkFtpOht_Req_3->Checked)   iTemp = iTemp + bit_3;

		if(chkFtpOht_Req_4->Checked)   iTemp = iTemp + bit_4;
		if(chkFtpOht_Req_5->Checked)   iTemp = iTemp + bit_5;
		if(chkFtpOht_Req_6->Checked)   iTemp = iTemp + bit_6;
*/

		// 2017.03.13. ���ο� ������ư ��� �ڵ�� �ٲ�
		if(rdoFtpOht_Req_0->Checked)   iTemp = iTemp + bit_0;	// ������ (�ٿ�ε�)
		if(rdoFtpOht_Req_1->Checked)   iTemp = iTemp + bit_1; 	// ���� (�ٿ�ε�)
		if(rdoFtpOht_Req_4->Checked)   iTemp = iTemp + bit_4; 	// �߿��� (�ٿ�ε�)
		// ---------------------------------------------------


		memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

		//memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
		// ------

		// �������� ���� ��Ʈ�� �� �ִ´�. (���� ���� ����� �� ���̹Ƿ�)
		//SEND_PACKET.BODY.Request.TestMode = 0x00;
	}
	else
	{
		iTemp = 0;

		if((gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Map_Adjust == ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Station_Adjust ==ON)||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.Teaching_Adjust ==ON) ||
			(gm_OHTStatus->m_OHTStatus.AdjustMapLoadInfo.CID_Adjust==ON))
			{
			// 2017.03.13. ���ο� ������ư ��� �ڵ�� �ٲ�
				if(rdoOhtFtp_Req_1->Checked)   iTemp = iTemp + bit_1;
				// ---------------------------------------------------

				iTemp = iTemp + bit_7;
				memcpy(&SEND_PACKET.BODY.Request, &iTemp, 1);

				// memcpy(&SEND_PACKET.BODY.Request.CMDType, &iTemp, 1);
				// ------

				// �������� ���� ��Ʈ�� �� �ִ´�. (���� ���� ����� �� ���̹Ƿ�)
				//SEND_PACKET.BODY.Request.TestMode = 0x01;
				DataTranaper_Display("��� ���� ����!");
			}
			else
			{
				DataTranaper_Display("������Map�� �ƴϹǷ� Map ���� UpLoad Fail");
				return;
            }
/*		2017.03.13. ������ CheckBox ��� �ڵ���
		if(chkOhtFtp_Req_0->Checked)   iTemp = iTemp + bit_0;
		if(chkOhtFtp_Req_1->Checked)   iTemp = iTemp + bit_1;
*/

	}

	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

}
//---------------------------------------------------------------------------

void TfrmDataTransperReq::Reset_Display(void)
{
	panCmd_Start->Color = clWhite;

	panCmd_0->Color = clWhite;
	panCmd_1->Color = clWhite;
	panCmd_2->Color = clWhite;
	panCmd_3->Color = clWhite;
}

void TfrmDataTransperReq::DataTranaper_Display(AnsiString strMsg)
{
	mmLog->Lines->Add(strMsg);
	Parse_Result(strMsg);
}


void __fastcall TfrmDataTransperReq::btnLogClearClick(TObject *Sender)
{
	mmLog->Lines->Clear();

	Reset_Display();
}
//---------------------------------------------------------------------------

void TfrmDataTransperReq::Parse_Result(AnsiString strMsg)
{

	AnsiString strCmd = "";		// �Ľ��Ͽ� ���Ÿ�ɾ� �κи� �ִ´�
	AnsiString strParam = "";	// �Ľ��Ͽ� ���� �Ķ���� �κи� �ִ´�

	int iParam_Position = 0;	// �Ķ����
	int iTrans_Response = 0;	// ���� ���� ����
	int iTrans_Progress = 0;    // ���� ������ ����

	// Step 1 : �Ķ���� ����
	strMsg = strMsg.Trim();
	iParam_Position = strMsg.Pos("0x");				// 0xAB�� ���� ������ ã�´�

	if(iParam_Position != 0)	   // �Ķ���Ͱ� ������
		strParam = strMsg.SubString(iParam_Position, 4); 	// �Ķ���� ȹ��

	// Step 2 : ��� ���� ����
	iTrans_Response = strMsg.Pos("���� ���� ����");
	iTrans_Progress = strMsg.Pos("���� ��ô ����");

	// Step 3 : ȭ�� ǥ��
	if(iTrans_Response != 0)
	{
		panCmd_0->Color = clLime;
	}

	if(iTrans_Progress != 0)
	{
		if(strParam == "0x01")
			panCmd_1->Color = clLime;
		else if(strParam == "0x02")
			panCmd_2->Color = clLime;
		else
			panCmd_3->Color = clLime;;
	}
}
