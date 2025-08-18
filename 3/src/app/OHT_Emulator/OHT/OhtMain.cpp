//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OhtMain.h"
#include "About.h"
#include "Option.h"


#include "UDP.h"
#include "ClwStatusReport.h"
#include "ClwJobOrderResponse.h"
#include "ProgressReport.h"
#include "ClwPassConfirm.h"
#include "ClwPassConfirmRequest.h"
#include "ClwVersionResponse.h"
#include "ClwPassConfirmStatusReport.h"
#include "ClwDataTransperResponse.h"

#include "Oht2TP.h"
#include "PUtily.h"
#include "ClwDiagAnswer.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmOHTMain *frmOHTMain;

// ������Ʈ �ɼ� - C++ Compiler - Directorys and Conditionals - Conditional defines
// IS_MCP �� ������ MCP, ������ OHT�� ����
// _ONE_PC_TEST
	// �� PC���� �׽�Ʈ�Ϸ��� ��Ʈ �ϳ��� ���� ���� �����Ƿ�
	// OHT�� Listen ��Ʈ�� +2 �ϵ��� �ϴ� Project define�̴�.

//---------------------------------------------------------------------------
__fastcall TfrmOHTMain::TfrmOHTMain(TComponent* Owner)
	: TForm(Owner)
{
	// ��� ����� �ν��Ͻ� ����
/*
	for(int i = 0; i < MDIChildCount; i++){
		if ( MDIChildren[i]->Name == "frmUDP" ){
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmUDP = new TfrmUDP(Application);   */
}
//---------------------------------------------------------------------------
void __fastcall TfrmOHTMain::mnuAboutClick(TObject *Sender)
{
	AboutBox = new TAboutBox(Application);
    AboutBox->ShowModal();
	delete AboutBox;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuComLogClick(TObject *Sender)
{
	// ��Ÿ���� �����ϰ� �ٷ� �����ϴ� ���̹Ƿ� ���������� �����Ѵ�
	for(int i = 0; i < MDIChildCount; i++){
		if ( MDIChildren[i]->Name == "frmUDP" ){
			MDIChildren[i]->BringToFront();
			ShowWindow(frmUDP->Handle,SW_SHOW);
			return;
		}
	}

	//frmUDP = new TfrmUDP(Application);
	//frmUDP->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuConfigClick(TObject *Sender)
{
	frmOption->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuCascadeClick(TObject *Sender)
{
	Cascade();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuTileHClick(TObject *Sender)
{
	TileMode = tbHorizontal;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuTileVClick(TObject *Sender)
{
	TileMode = tbVertical;
	Tile();
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuServerStartClick(TObject *Sender)
{
	int iBr, iUr, iTP, iDiagBr, iDiagUr;
	AnsiString strBroadCastMask = BROADCAST_IP;

// ONE PC�ɼ��� ���� 2016.08.11 �������� �Ʒ��� ���� ����������,
// ��Ÿ�� ������ ó���ϴ� ���� ���ϴٰ� ���� �Ʒ��� ���� ������
#if 0
	#ifdef	_ONE_PC_TEST
		iBr = frmUDP->pUdpObject->BroadcastBind((frmOption->iBroadCastPort + 2));
		iUr = frmUDP->pUdpObject->UnicastBind((frmOption->iControlPort + 2));

		frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
							 (frmOption->iBroadCastPort + 2),
							 (frmOption->iControlPort + 2));

		frmUDP->pUdpObject->SetBroadCastMask(strBroadCastMask);

		iTP = frmUDP->pUdpObject->OHT_TP_Bind();

		// ���ܼ��� ���� �߰���
		// iDiagBr = frmUDP->pDiagObject->DIAG_BroadcastBind(ADDRESS_DIAG_GET_DEFAULT_PORT + 2);
		iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT + 2);
		//frmUDP->pDiagObject->DiagInfoUpdate(MACHINETYPE_DIAG_NORMAL_OHT, frmOption->strMachineID,
		//						(ADDRESS_DIAG_GET_DEFAULT_PORT + 2), (CONTROL_DIAG_DEFAULT_PORT + 2));

	#else
		iBr = frmUDP->pUdpObject->BroadcastBind(frmOption->iBroadCastPort);
		iUr = frmUDP->pUdpObject->UnicastBind(frmOption->iControlPort);
		iTP = frmUDP->pUdpObject->OHT_TP_Bind();

		frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
							 (frmOption->iBroadCastPort),
							 (frmOption->iControlPort));

		// ���ܼ��� ���� �߰���
		// iDiagBr = frmUDP->pDiagObject->DIAG_BroadcastBind(ADDRESS_DIAG_GET_DEFAULT_PORT);
		iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT);

		//frmUDP->pDiagObject->DiagInfoUpdate(MACHINETYPE_DIAG_NORMAL_OHT, frmOption->strMachineID,
		//						(ADDRESS_DIAG_GET_DEFAULT_PORT), (CONTROL_DIAG_DEFAULT_PORT));
	#endif
#endif

	// 2016.08.11  �Ʒ��� ���� ������
	iBr = frmUDP->pUdpObject->BroadcastBind(frmOption->iBroadCastPort);
	iUr = frmUDP->pUdpObject->UnicastBind(frmOption->iControlPort);
	iTP = frmUDP->pUdpObject->OHT_TP_Bind();

	frmUDP->pUdpObject->MyInfoUpdate(frmOption->iOpt_MachineType, frmOption->strMachineID,
						 (frmOption->iBroadCastPort),
						 (frmOption->iControlPort));

	iDiagUr = frmUDP->pDiagObject->UnicastBind(CONTROL_DIAG_DEFAULT_PORT);

	// 2016.05.19 �������� ��� ������ ��쿡 ���� Ÿ�̸Ӹ� ���� �ʵ��� �� ����
	if((iBr != 0) && (iUr != 0) && (iTP != 0) && /*(iDiagBr != 0) && */ (iDiagUr != 0))
		ShowMessage(L"UDP ���� ������ �����߽��ϴ�!");
	else
		tmrUdpReceiveThread->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindMcp_AddrClick(TObject *Sender)
{
	// OCS�� �ּҸ� ã������ Broad Cast�Ѵ�
	frmUDP->pUdpObject->Address_Broadcast(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  frmOption->iBroadCastPort);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::tmrUdpReceiveThreadTimer(TObject *Sender)
{

// --------------------- �ּ�ȹ�� �������� ó�� ------------------------
	// ������ ��Ʈ��ȣ�� ���� ��Ʈ��ȣ. 1PC ȯ�濡���� OCS�� ��������.
	frmUDP->pUdpObject->BroadCast_ThreadCall(frmOption->strMachineID, frmOption->iBroadCastPort);

	// ���ܼ��� �������� ó��
	// frmUDP->pDiagObject->DIAG_BroadCast_ThreadCall(frmOption->strMachineID, ADDRESS_DIAG_GET_DEFAULT_PORT);

// --------------------- ��Ʈ��  �������� ó�� ------------------------
	// frmUDP->UniCast_ThreadCall();
	PKT_OCS2OHT CmdPkt;
	int iResult;

	iResult = frmUDP->pUdpObject->UniCast_ThreadCall(CmdPkt);
        char cTemp[MACHINE_ID_LENGTH + 1];

	if(iResult > 0)
	{
        	memcpy(&cTemp, &CmdPkt.HEADER.MachineID, MACHINE_ID_LENGTH);
                cTemp[MACHINE_ID_LENGTH] = '\0';

		AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;
		strTmp.sprintf("���� ���� [HEADER] MachineType : %d MachineID : %s Tag : %d ",
					CmdPkt.HEADER.MachineType, cTemp, CmdPkt.TAG.Value);

		switch(CmdPkt.TAG.Value)
		{
			case TagID::CMD_VERSIONREQ_OCS_OHT:
			{
				AnsiString strTmp;

				for(int i = 0; i < MDIChildCount; i++){
				
					strTmp = MDIChildren[i]->Name;
					if ( MDIChildren[i]->Name == "frmClwVersionRequest" ){
						ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
						MDIChildren[i]->BringToFront();
						return;
					}
				}
				
				frmClwVersionRequest = new TfrmClwVersionRequest(Application);

			}
			break;

			case TagID::CMD_DATATRANSFER_OCS2OHT:
			{
				PKT_DATA_TRANSPER_ORDER_OCS2OHT			CmdPkt_Up;
				memcpy(&CmdPkt_Up, &CmdPkt, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

				AnsiString strTmp;
			
				for(int i = 0; i < MDIChildCount; i++){
				
					strTmp = MDIChildren[i]->Name;
					if ( MDIChildren[i]->Name == "frmClwDataTransperResponse" ){
						ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
						MDIChildren[i]->BringToFront();						
						frmClwDataTransperResponse->Trigger(CmdPkt_Up);
						
						return;
					}
				}
				
				frmClwDataTransperResponse = new TfrmClwDataTransperResponse(Application);
				frmClwDataTransperResponse->Trigger(CmdPkt_Up);
				frmClwDataTransperResponse->BringToFront();
			}

			break;

			default:
			{
				// ShowMessage(strTmp);
			}
			break;
		}
	}
// --------------------- TP �������� ó�� ------------------------
	// frmUDP->OHT_TP_ThreadCall();
	iResult = frmUDP->pUdpObject->OHT_TP_ThreadCall(CmdPkt);

	if(iResult > 0)
	{
		AnsiString strTmp;	unsigned char cVar1, cVar2, cVar3;
		strTmp.sprintf("TP ��Ŷ���� [HEADER] MachineType : %d MachineID : %s Tag : %d [OCS STATUS] StatusOrder : %s StopOrder : %s StopReason : %d MaintananceOrder : %s",
					CmdPkt.HEADER.MachineType, CmdPkt.HEADER.MachineID, CmdPkt.TAG.Value,
					Char2BinString(cVar1), Char2BinString(cVar2), CmdPkt.STATUS.StopReason, Char2BinString(cVar3));
		// ShowMessage(strTmp);
	}

// --------------------- ���ܼ��� �������� ó�� ------------------------
	PKT_OHT2DIAG DiagPkt;
	iResult = frmUDP->pDiagObject->DIAG_UniCast_ThreadCall(DiagPkt);

	if(iResult > 0)
	{
		// ShowMessage(L"���� �������� ����");
	}

// --------------------- ��ý��Ͻ� �������� ó�� ----------------------
	PKT_OHT2OCS AS_PKT;
	iResult = frmUDP->pDiagObject->OHT_AS_ThreadCall(AS_PKT);
	// �ʿ��ϸ� ���⼭ �߰� ó��
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuServerStopClick(TObject *Sender)
{
	tmrUdpReceiveThread->Enabled = false;

	frmUDP->pUdpObject->BroadcastClose();
	frmUDP->pUdpObject->UnicastClose();

	frmUDP->pUdpObject->OHT_TP_Close();

	// frmUDP->pDiagObject->DIAG_BroadcastClose();
	frmUDP->pDiagObject->UnicastClose();
}
//---------------------------------------------------------------------------

// OHT�� OCS�� ������ ������ ����.
void __fastcall TfrmOHTMain::mnuSendStatus_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwStatusReport = new TfrmClwStatusReport(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::FormActivate(TObject *Sender)
{
//	static bool bShowOnce = false;
//#ifdef	_ONE_PC_TEST
//	if (bShowOnce == false)
//	{
//		ShowMessage(L"One PC Test ����Դϴ�!\n\n�����Ͽ� �ֽʽÿ�!");
//		bShowOnce = true;
//	}
//#endif
}

//---------------------------------------------------------------------------
/*
void TfrmOHTMain::Protocol_Receiver_JOB_ORDER(PKT_JOB_ORDER_ID_121 Pkt)
{
	AnsiString strTmp, strDisplay;

	strDisplay = strDisplay + strTmp.sprintf("HDR - Type : %d  ID : %s \n",
								Pkt.HEADER.MachineType, Pkt.HEADER.MachineID);

	strDisplay = strDisplay + strTmp.sprintf("TAG : %d \n", Pkt.TAG.Value);



	ShowMessage(strDisplay);
}
*/
//---------------------------------------------------------------------------
/*
void TfrmOHTMain::Protocol_Receiver_OCS_STATUS(PKT_OCS_STATUS_ID_01 Pkt)
{
	AnsiString strTmp, strDisplay;

	strDisplay = strDisplay + strTmp.sprintf("HDR - Type : %d  ID : %s \n",
								Pkt.HEADER.MachineType, Pkt.HEADER.MachineID);

	strDisplay = strDisplay + strTmp.sprintf("TAG : %d \n", Pkt.TAG.Value);



	ShowMessage(strDisplay);
}
*/
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::JobResponseID122Click(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmJobOrderResponse" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmJobOrderResponse = new TfrmJobOrderResponse(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuSendProgress_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmProgressReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmProgressReport = new TfrmProgressReport(Application);
}
//---------------------------------------------------------------------------

void TfrmOHTMain::StandardRead(PKT_OCS2OHT Pkt, int iMachineType)
{
	AnsiString strTmp;
	strTmp.sprintf("OCS ��Ŷ ����, ���Ÿ�� : %d  �ӽ�Ÿ�� : %d" , Pkt.TAG.Value, iMachineType);
    // ShowMessage(strTmp);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuPassRequest_OHT_to_OCSClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirm" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirm = new TfrmClwPassConfirm(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::OHTID1Click(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirmRequest" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirmRequest = new TfrmClwPassConfirmRequest(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuPcTpTestClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmOht2TP" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmOht2TP = new TfrmOht2TP(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuVersionResponseClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwVersionRequest" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwVersionRequest = new TfrmClwVersionRequest(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuOht2OcsPassConfirmStatusReportClick(TObject *Sender)

{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwPassConfirmStatusReport" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwPassConfirmStatusReport = new TfrmClwPassConfirmStatusReport(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindDiag_AddrClick(TObject *Sender)
{
	// OCS�� �ּҸ� ã������ Broad Cast�Ѵ�
	/*
	frmUDP->pDiagObject->Address_Broadcast(MACHINETYPE_DIAG_NORMAL_OHT,
								  frmOption->strMachineID,
								  ADDRESS_DIAG_GET_DEFAULT_PORT);   */
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::test1Click(TObject *Sender)
{
	tmrUdpReceiveThreadTimer(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuDiagAnswerClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "TfrmDiagAnswer" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmDiagAnswer = new TfrmDiagAnswer(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuOcsFileTransperRunClick(TObject *Sender)
{
	AnsiString strTmp;

	for(int i = 0; i < MDIChildCount; i++){

		strTmp = MDIChildren[i]->Name;
		if ( MDIChildren[i]->Name == "frmClwDataTransperResponse" ){
			ShowWindow(MDIChildren[i]->Handle,SW_SHOW);
			MDIChildren[i]->BringToFront();
			return;
		}
	}

	frmClwDataTransperResponse = new TfrmClwDataTransperResponse(Application);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mnuClwFindMcp_Addr_NoClrClick(TObject *Sender)
{
	frmUDP->pUdpObject->Address_Broadcast_NotClear(frmOption->iOpt_MachineType,
								  frmOption->strMachineID,
								  frmOption->iBroadCastPort);

}
//---------------------------------------------------------------------------

void __fastcall TfrmOHTMain::mndDiagInfoSetClick(TObject *Sender)
{
	frmUDP->pDiagObject->SetDiagInfo("127.0.0.1", CONTROL_DIAG_DEFAULT_PORT, frmOption->strMachineID, MACHINETYPE_DIAG_NORMAL_OHT);
	// frmUDP->pDiagObject->SetDiagInfo("192.167.0.20", CONTROL_DIAG_DEFAULT_PORT, frmOption->strMachineID, MACHINETYPE_DIAG_NORMAL_OHT);
}
//---------------------------------------------------------------------------

