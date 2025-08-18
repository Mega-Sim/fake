// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OHTInfoUnit.h"
#include "ConnectUnit.h"
#include "MainUnit.h"
#include "SocketUnit.h"
#include "FileUnit.h"
#include "WarnningUnit.h"
#include "Utility.h"
#include "Define.h"
#include "Def_TP.h"
#include "OHT_TLV.h"
#include "AssistantUnit.h"

#include "UDP.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConnectForm *ConnectForm;

// ---------------------------------------------------------------------------
__fastcall TConnectForm::TConnectForm(TComponent *Owner)
    :
    TForm(Owner)
{

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectNameClick(TObject *Sender)
{
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[ConnectName->ItemIndex];
	gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
}
// ---------------------------------------------------------------------------

/**
@brief   TP���� OHT�� Connect��ư Ŭ���� �߻��ϴ� �Լ�
@author  puting
@date    2012.12.13
@param   SocketForm���κ��� ���� �õ�
*/
void __fastcall TConnectForm::btnConnectClick(TObject *Sender)
{
	// SocketForm->Assistant_DisConnect();

	MainForm->StatusBarVersionInfo->Panels->Items[0]->Text = "";

	TOHTLOGFILE *m_LogFile = new TOHTLOGFILE;
	String strLog;
	strLog.sprintf(L"====> [CONNECT BUTTON] Click...%s [%s]", ConnectName->Text.Trim(), ConnectIP->Text.Trim());
	m_LogFile->WriteLogFile("Communication", strLog);
	delete m_LogFile;

//	�� �κ��� ���ο� UDP�ڵ带 �̿��Ѵ�
	 // ���� : SocketForm->ConnectOHT(ConnectIP->Text.Trim(), ConnectName->Text.Trim());

 ///////////////// Remote.ini �� ���� ȣ�� �ÿ� TP ���� �Ұ�///////////.
	int nItemCnt = ConnectName->Items->Count;     // �޺��ڽ� ������ ����
	bool bSame = false;
	AnsiString    strInputTxt = ConnectName->Text.UpperCase();    // ����ڰ� �Է��� ����. ��ҹ��� �� ���ϵ��� �빮�ڷ� ����

    // �޺��ڽ� ����Ʈ���� ��� �˻��ϸ鼭 ����ڰ� �Է��� ���� ��ġ�ϴ°� �ִ��� ��
    // ����ڰ� �Է��� ���� �޺��ڽ��� ���ٸ�, �ǵ��� ȣ���-IP ��Ī�� �ƴҼ� �ֱ� ������
	for(int i=0; i<nItemCnt ; i++)
    {
        // �޺��ڽ� ����Ʈ �� 1�� �ؽ�Ʈ
        AnsiString strItemTxt = ConnectName->Items->Strings[i].UpperCase(); // ��ҹ��ں�X
        if(strInputTxt == strItemTxt)
		{
           bSame = true;    // ��ġ�ϴ� �׸�  ã��
           break;
		}
    }

    // �Է��� ���� ��ġ�ϴ� �������� ������ Connect ���� �ʰ� �����޽��� �� ����.
    if(bSame == false)
    {
       MainForm->DataDisplay(COLOR_RED, "OHT ȣ��� Ȯ�� �ʿ�");
	   return;
	}

   ///////////ȣ��� 6���� �ƴϸ� �����޼��� ���鼭 TP ���� �Ұ�  ////////////

   int nTextLen = ConnectName->Text.Length();

   if(nTextLen !=6 )      // ȣ��� ���� �� ����  6����
   {
			//SocketForm->DisConnect();	// ���� ���� �ȵȻ��±� ������ Disconnect Ż �ʿ� ����
			Warnning_Display(COLOR_RED, "OHT ȣ����� Ȯ�����ּ���..");
			return ;
   }
   else
   {
		MainForm->m_bIgnoreJungBokConnect = false;
		SocketForm->ConnectOHT(ConnectIP->Text.Trim(), OHT_S_TP_LISTEN_PORT_NUM ,
								TP_FIX_PORT_NUM, ConnectName->Text.Trim());




// ------- ������� �Ʒ����� �߰� ----------------------------------------
 /*
	int iResult = frmUDP->pUdpObject->UnicastBind_TP(ConnectIP->Text.Trim(),
		 StrToInt(ConnectPort->Text.Trim()), TP_FIX_PORT_NUM);	// ���ο� ��Ÿ���� ���۽�Ų��.

		if(iResult == -3)
			ShowMessage("TP�� UDP ������Ʈ�� �̹� ���� �ֽ��ϴ�!");
		else if(iResult == 0)
		{
			char buf[MAX_PACKET_SIZE+1];
			int iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);
			int iReturn, iTemp;
	
			PKT_STATUS_REPORT_ID_03	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����
	
			SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
			SEND_PACKET.TAG.Value = TpCmdID::TP_ADDRESS_REGISTER;
	
			iReturn = frmUDP->pUdpObject->UnicastSend((char*)&SEND_PACKET, iPacketSize);  // TP�ּ� ��Ͽ�û
	
			ShowMessage("TP�� UDP ������Ʈ bind ����");
		}
		else
			ShowMessage("TP�� UDP ������Ʈ bind ����! ��Ʈ�� Ȯ���� �ּ���");
	
		MainForm->tmrUdpListen->Enabled = true;		// UDP �����Ʈ ���Ž���
	*/
	
	// -------------------------------------------------------------------------
	
		if(SocketForm->AssistantSocket->Active == true)
		{
			SocketForm->AssistantSocket->Active = false;
		}
	
		int nError = 0;
	

	
		//SocketForm->AssistantConnect(ConnectIP->Text.Trim(), ConnectName->Text.Trim());
	
		nError = FileForm->FileControlStart(ConnectIP->Text.Trim());
	
	

	}

	//2022.05.11 sseung ó�� ����� Default�� ��ڸ��
	gm_OHTStatus->nProgram_Mode = MODE_WORKER;
	MainForm->panUser->Caption = "���";
}
// ---------------------------------------------------------------------------

/**
@brief   TP���� OHT�� Cancel��ư Ŭ���� �߻��ϴ� �Լ�
@author  puting
@date    2012.12.13
*/
void __fastcall TConnectForm::btnCancelClick(TObject *Sender)
{
    SocketForm->DisConnect();
    
    Close();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuAddYESClick(TObject *Sender)
{
    int i;

    gm_OHTConnectInfo->nCount++;
    gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nCount - 1] = ConnectName->Text;
    // Label1->Caption = ConnectName->Items->Strings[0];//ConnectName->ItemIndex];
    // Label1->Caption = ConnectName->Text;
    gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1] = ConnectIP->Text;
    gm_OHTConnectInfo->RemoteFileSave();
    ConnectName->Clear();
    for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
    {
        ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);
    }
    ConnectName->ItemIndex = gm_OHTConnectInfo->nCount - 1;
    gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
    ConnectIP->Text           = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1];

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuEditYESClick(TObject *Sender)
{
	int i;

	gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nIndex] = ConnectName->Text;
	gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex]   = ConnectIP->Text;
	gm_OHTConnectInfo->RemoteFileSave();
	ConnectName->Clear();


	for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
	 {
	 ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);

	  }
	  ConnectName->ItemIndex = gm_OHTConnectInfo->nIndex;
	  ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex];


}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::pmnuDeleteYESClick(TObject *Sender)
{
	int i;
    for(i = gm_OHTConnectInfo->nIndex; i < gm_OHTConnectInfo->nCount; i++)
    {
		gm_OHTConnectInfo->strOHTName[i] = gm_OHTConnectInfo->strOHTName[i + 1];
        gm_OHTConnectInfo->strOHTIP[i]   = gm_OHTConnectInfo->strOHTIP[i + 1];
    }
    gm_OHTConnectInfo->strOHTName[gm_OHTConnectInfo->nCount - 1] = "";
    gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nCount - 1] = "";
    gm_OHTConnectInfo->nCount--;

    gm_OHTConnectInfo->RemoteFileSave();
	ConnectName->Clear();
    for(i = 0; i < gm_OHTConnectInfo->nCount; i++)
    {
		ConnectName->Items->Add(gm_OHTConnectInfo->strOHTName[i]);
    }
	ConnectName->ItemIndex = gm_OHTConnectInfo->nIndex;
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[gm_OHTConnectInfo->nIndex];

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::FormCreate(TObject *Sender)
{
    gm_OHTConnectInfo->ConnectStart();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::btnConnectEditMouseDown(TObject *Sender,
    TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint P, Q;
    if(Button == mbLeft)
    {
        P.x = X; // P is the TPoint record for the form
        P.y = Y;
        Q   = btnConnectEdit->ClientToScreen(P); // Q is the TPoint for the screen
        pmOHTInfo->Popup(Q.x, Q.y);
    }

}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::FormShow(TObject *Sender)
{
    ConnectForm->Top   = MainForm->Top + MainForm->Height;
    ConnectForm->Left  = MainForm->Left;
    ConnectForm->Width = MainForm->Width;

    // sanghwa
    LinePickupColor->Color = (TColor)gm_OHTConnectInfo->g_iLineColor;
}

// ---------------------------------------------------------------------------
void __fastcall TConnectForm::btnConnectAssistantClick(TObject *Sender)
{
//	int nError = 0;
//
//
//
//	if(SocketForm->AssistantSocket->Socket->Connected == true)
//	{
//		SocketForm->AssistantSocket->Active = false;
//	}
//
//	SocketForm->AssistantConnect(ConnectIP->Text.Trim(), ConnectName->Text.Trim());
//
//	nError = FileForm->FileControlStart(ConnectIP->Text.Trim());

}

// ---------------------------------------------------------------------------
void TConnectForm::Warnning_Display(TColor tWColor, String strData)
{
    MainForm->DataDisplay(tWColor, strData);
    WarnningForm->DisplayMemo->Clear();
    WarnningForm->DisplayMemo->Lines->Add(strData);
    WarnningForm->Show();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::LinePickupColorClick(TObject *Sender)
{
    // Choose Color
    TColorDialog *pDlg = new TColorDialog(this);
    if(!pDlg)
        return;

    if(!pDlg->Execute())
        return;

    MainForm->panLineName->Color    = pDlg->Color;
    gm_OHTConnectInfo->g_iLineColor = pDlg->Color;
    LinePickupColor->Color          = pDlg->Color;
}

// ---------------------------------------------------------------------------
/**
 @brief   Remote.ini�� Edit�ϱ� ���� �Լ�
 @author  puting
 @date    2012.12.14
 @param   Remote.ini(IP, OHT ���� ����) Edit
 */
void __fastcall TConnectForm::btnIPEditClick(TObject *Sender)
{
	String strTmpPath = ExtractFilePath(Application->ExeName) + "Remote.ini";
	ShellExecute(Handle, L"open", strTmpPath.w_str(), NULL, NULL, SW_SHOW);
}

// ---------------------------------------------------------------------------
/**
 @brief   Remote.ini�� Load�ϱ� ���� �Լ�
 @author  puting
 @date    2012.12.14
 @param   Remote.ini(IP, OHT ���� ����) load
 */
void __fastcall TConnectForm::btnIPLoadClick(TObject *Sender)
{
	gm_OHTConnectInfo->ConnectStart();
}
// ---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectNameEnter(TObject *Sender)
{
	ConnectIP->Text = gm_OHTConnectInfo->strOHTIP[ConnectName->ItemIndex];
	gm_OHTConnectInfo->nIndex = ConnectName->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TConnectForm::ConnectIPKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	for(int i =0; i< ConnectName->Items->Count;i++)
	{
		if(StrComp(gm_OHTConnectInfo->strOHTIP[i].c_str(),ConnectIP->Text.c_str())==0)
		{
			ConnectName->ItemIndex = i;
			gm_OHTConnectInfo->nIndex = i;
			break;
		}
	}
}
//---------------------------------------------------------------------------

