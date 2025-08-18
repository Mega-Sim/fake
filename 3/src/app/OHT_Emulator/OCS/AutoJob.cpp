//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AutoJob.h"
#include "Option.h"
#include "PUtily.h"
#include "UDP.h"


#include "ResponsePasspermitOCS.h"
#include "OcsPassConfirmResponse.h"
#include "RunningStatus.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmAutoJob *frmAutoJob;

TfrmResponsePasspermitOCS 	*frmPasspermitOCS;
TfrmOcsPassConfirmResponse 	*frmOcsPassConfirm;

TfrmRunningStatus			*frmViewLog;

#define GRD_NUM				0	// No
#define GRD_MAIN_CMD		1	// �������
#define GRD_SUB_CMD			2	// �󼼵���
#define GRD_CMD_ID			3	// ����ID
#define GRD_TARGET_NODE		4	// �������

#define GRD_STOP_ORDER		5	// ��������
#define GRD_PATH_DATA		6	// ��ε�����
#define GRD_START_NODE		7	// ���۳��
#define GRD_TARGET_STATION	8	// ������ STA
#define GRD_STOP_TAG		9	// Stop Tag

#define GRD_RUN_DIST		10	// Rundist	
#define GRD_NEXT_NODE_ID	11	// NextNodeID
#define GRD_HOIST			12	// Hoist
#define GRD_SHIFT			13	// Shift
#define GRD_ROTATE			14	// Rotate

#define GRD_DRIVING			15
#define GRD_RESERVED_16		16
#define GRD_RESERVED_17		17

#define GRID_MAX_COL		18	// �׸����� �÷� ����

//---------------------------------------------------------------------------
__fastcall TfrmAutoJob::TfrmAutoJob(TComponent* Owner)
	: TForm(Owner)
{
	// �׸��忡 ������ �׸��� ǥ��
	FeederGrid->Cells[GRD_NUM][0] = "No";
	FeederGrid->Cells[GRD_MAIN_CMD][0] = "�������";
	FeederGrid->Cells[GRD_SUB_CMD][0] = "�󼼵���";
	FeederGrid->Cells[GRD_CMD_ID][0] = "����ID";
	FeederGrid->Cells[GRD_TARGET_NODE][0] = "�������";
	
	FeederGrid->Cells[GRD_STOP_ORDER][0] = "��������";
	FeederGrid->Cells[GRD_PATH_DATA][0] = "��ε�����";
	FeederGrid->Cells[GRD_START_NODE][0] = "���۳��";
	FeederGrid->Cells[GRD_TARGET_STATION][0] = "������ STA";
	FeederGrid->Cells[GRD_STOP_TAG][0] = "Stop Tag";
		
	FeederGrid->Cells[GRD_RUN_DIST][0] = "Rundist";
	FeederGrid->Cells[GRD_NEXT_NODE_ID][0] = "Next(����)";
	FeederGrid->Cells[GRD_HOIST][0] = "Hoist";
	FeederGrid->Cells[GRD_SHIFT][0] = "Shift";
	FeederGrid->Cells[GRD_ROTATE][0] = "Rotate";

	FeederGrid->Cells[GRD_DRIVING][0] = 	"Driving(0,1)";
	FeederGrid->Cells[GRD_RESERVED_16][0] = "����2";
	FeederGrid->Cells[GRD_RESERVED_17][0] = "����3";

	GridClear();

	// ============== ���� ����� �ʱ�ȭ ==============
	memset(&PKT_OCS_STATUS, 0, sizeof(PKT_OCS_STATUS_ID_01));	// OCS���� �ʱ�ȭ

	// ---- Header
	PKT_OCS_STATUS.HEADER.MachineType = MACHINETYPE_VHL;
	memset(&PKT_OCS_STATUS.HEADER.MachineID, ASCII_SPACE, MACHINE_ID_LENGTH);		// �޸� ����
	memcpy(PKT_OCS_STATUS.HEADER.MachineID, frmOption->strOhtMachineID.c_str(), frmOption->strOhtMachineID.Length());

	// ---- Tag
	PKT_OCS_STATUS.TAG.Value = TagID::MCP_TO_CLW_STATUS;

	// --- Status
	int iTemp = 0x00;	// �⵿ OFF
	memcpy(&PKT_OCS_STATUS.STATUS.StatusOrder, &iTemp, 1);

	// ============== ���α׷� �ڵ鸵 =============
	CurrentFileName = "UNTITLED.PRG";

	// �Ϻ� �� ����
	frmPasspermitOCS = new TfrmResponsePasspermitOCS(Application);
	frmPasspermitOCS->SetButtonMode(true);
	frmPasspermitOCS->SetResponseMode(0);	// �׻� Ack�� ����Ʈ
	frmPasspermitOCS->btnHideClick(NULL);

	frmOcsPassConfirm = new TfrmOcsPassConfirmResponse(Application);
	frmOcsPassConfirm->SetButtonMode(true);
	frmOcsPassConfirm->SetResponseMode(0);	// �׻� Ack�� ����Ʈ
	frmOcsPassConfirm->btnHideClick(NULL);

	frmViewLog = new TfrmRunningStatus(Application);
	frmViewLog->btnHideClick(NULL);

	bOHT_NOW_RUNNING = false;
}

//---------------------------------------------------------------------------
void TfrmAutoJob::GridClear()
{
	AnsiString fwidth;
	i_Cmd_Count = 0;
	FeederGrid->RowCount = 1;

	for(int i = 0; i < (i_Cmd_Count + 1); i++)
	{
		FeederGrid->Cells[GRD_NUM][i+1] = IntToStr(i+1);			// No �ʱ�ȭ
	}

	edtCount->Text = IntToStr(i_Cmd_Count);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::FeederGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect)
{
	if(ACol == 5) 	// ��������
	{		
		FWidthComboBox->Visible = false;

		FWidthComboBox->Top = FeederGrid->Top + 2 +
			FeederGrid->DefaultRowHeight*(ARow - FeederGrid->TopRow); // +1);
								  // + 1 * (ARow - FeederGrid->TopRow);

		FWidthComboBox->Text = FeederGrid->Cells[ACol][ARow];
		int index = FWidthComboBox->Items->IndexOf(FeederGrid->Cells[ACol][ARow]);
		if(index == -1) index = 0;

		FWidthComboBox->ItemIndex = index;
		FWidthComboBox->Visible = true;
	}
	else
	{
		FWidthComboBox->Visible = false;
    }

}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::FWidthComboBoxChange(TObject *Sender)
{
	AnsiString fwidth = FWidthComboBox->Items->Strings[FWidthComboBox->ItemIndex].c_str();
	FeederGrid->Cells[GRD_STOP_ORDER][FeederGrid->Row] = fwidth.c_str();
}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::btnNewClick(TObject *Sender)
{
	edtFileName->Text = "UNTITLED.PRG";
	GridClear();
}
//---------------------------------------------------------------------------
void TfrmAutoJob::DisplayBlank(int iGrdIndex)
{
	for(int i = 1; i < GRID_MAX_COL; i++)
		FeederGrid->Cells[i][iGrdIndex] = "-------";
}

// ���� Ŀ�ǵ带 ������ �� �Լ��� ȣ���ϸ� �ڵ����� �����ִ� Ŀ�ǵ� Set�� �߰��Ѵ�
void TfrmAutoJob::Minor_Cmd_AutoSet(int iMajorCmd)
{
	int iOrdID; 
	AnsiString fwidth = cboMajorCmd->Items->Strings[iMajorCmd].c_str();	// ������ �ؽ�Ʈ ����

	switch(iMajorCmd)
	{
		// �⵿���
		case Sim_Major_Cmd::MOVE_ON:
			// MCP���º��� (���)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "MCP_TO_CLW_STATUS";			

			// CLW���º��� (����)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "CLW_TO_MCP_STATUS";			
		break;

		// �������
		case Sim_Major_Cmd::MOVE_OFF:
			// MCP���º��� (���)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);

			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] 	= cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "MCP_TO_CLW_STATUS";			
			FeederGrid->Cells[GRD_STOP_ORDER][i_Cmd_Count] 	= "���న��+���簨��";

			// CLW���º��� (����)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);

			FeederGrid->Cells[GRD_NUM][i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD][i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD][i_Cmd_Count] 	= "CLW_TO_MCP_STATUS";							
		break;

		// �۾����� - �̵� ����Ŭ (����)
		case Sim_Major_Cmd::ORD_MOVE_NORMAL:
			// ========== �۾����� (���)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// �������

			// 5����!! ������ ��!!!!														// 5����!! ������ ��!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// ��ε�����
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// ���۳��
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// ������ STA
			FeederGrid->Cells[GRD_STOP_TAG]		[i_Cmd_Count] = "0";						// Stop Tag

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted!  FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate

			// =========== �۾����� ����
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);

			// ========= ��ô���� (����)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;

		// �۾����� - �ø�����Ŭ (����)
		case Sim_Major_Cmd::ORD_UP_NORMAL:
			
			// =========== �۾����� (���)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// �������

			// 5����!! ������ ��!!!!														// 5����!! ������ ��!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// ��ε�����
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// ���۳��
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// ������ STA			

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted! FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate
			FeederGrid->Cells[GRD_DRIVING]		[i_Cmd_Count] = "1";						// Driving (0, 1)

			// ========= �۾����� ����
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			// FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";

			// ========= ��ô���� (����)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[+i_Cmd_Count] 	= "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;

		// �۾����� - ��������Ŭ (����)
		case Sim_Major_Cmd::ORD_DN_NORMAL:			
			
			// ========= �۾����� (���)
			iOrdID = GetOrderID();

			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
			FeederGrid->Cells[GRD_TARGET_NODE]	[i_Cmd_Count] = "2001";						// �������

			// 5����!! ������ ��!!!!														// 5����!! ������ ��!!!!
			FeederGrid->Cells[GRD_PATH_DATA]	[i_Cmd_Count] = ""; 						// ��ε�����
			FeederGrid->Cells[GRD_START_NODE]	[i_Cmd_Count] = "1001";  					// ���۳��
			FeederGrid->Cells[GRD_TARGET_STATION][i_Cmd_Count] = "3001";  					// ������ STA			

			FeederGrid->Cells[GRD_RUN_DIST]		[i_Cmd_Count] = "0";						// Rundist
			// deleted! FeederGrid->Cells[GRD_NEXT_NODE_ID]	[i_Cmd_Count] = "0"; 						// NextNodeID
			FeederGrid->Cells[GRD_HOIST]		[i_Cmd_Count] = "0";						// Hoist
			FeederGrid->Cells[GRD_SHIFT]		[i_Cmd_Count] = "0"; 						// Shift
			FeederGrid->Cells[GRD_ROTATE]		[i_Cmd_Count] = "0";						// Rotate
			FeederGrid->Cells[GRD_DRIVING]		[i_Cmd_Count] = "1";						// Driving (0, 1)
			
			// ========= �۾����� ����
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_ORDER_RESPONSE";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);

			// ========= ��ô���� (����)
			FeederGrid->RowCount = FeederGrid->RowCount + 1;
			i_Cmd_Count++;
			DisplayBlank(i_Cmd_Count);
			
			FeederGrid->Cells[GRD_NUM]			[i_Cmd_Count] = IntToStr(i_Cmd_Count);
			FeederGrid->Cells[GRD_MAIN_CMD]		[i_Cmd_Count] = cboMajorCmd->Items->Strings[iMajorCmd];
			FeederGrid->Cells[GRD_SUB_CMD]		[i_Cmd_Count] = "JOB_PROGRESS_REPORT";
			FeederGrid->Cells[GRD_CMD_ID]		[i_Cmd_Count] = IntToStr(iOrdID);
		break;
	}

	edtCount->Text = IntToStr(i_Cmd_Count);
}
//---------------------------------------------------------------------------
void __fastcall TfrmAutoJob::btnAddClick(TObject *Sender)
{
	Minor_Cmd_AutoSet(cboMajorCmd->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnDeleteClick(TObject *Sender)
{
	if(FeederGrid->RowCount > 1)
	{
		FeederGrid->RowCount = FeederGrid->RowCount - 1;
		i_Cmd_Count--;

		edtCount->Text = IntToStr(i_Cmd_Count);
	}
}
//---------------------------------------------------------------------------

// i_Run_Position �� 0���� ����������, �̴� �������� �ƴ� �����̴�.
// ù ��ɾ�� 1���� �ִٰ� �����Ѵ� (����Ʈ�� ��ġ�ϵ���)
void TfrmAutoJob::StatusMachine(int iCmd, PKT_OHT2OCS Pkt, int iTagID, int iPacketSize)
{
	static int i_Run_Position = 0; // ������ ��� ����
	static int i_Run_Control = Sim_Run_Cmd::STOP;
	int iMajor_Cmd, iMinor_Cmd;
	unsigned char cVar1, cVar2;
	AnsiString strLog;
	//tmrCmdSend->Enabled = false;

	// ========== ������Ʈ �ӽ� �ڵ鸵 �� OHT ���� ó�� ==========
	switch(iCmd)
	{
		case Sim_Run_Cmd::RUN:		// ó������ �����Ѵ�
			i_Run_Position = 0;
			i_Run_Control = Sim_Run_Cmd::CONTINUE;	// ��� ���� �ܰ�� ���� ����			
		break;

		case Sim_Run_Cmd::STOP:
			i_Run_Position = 0;
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::PAUSE:
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::CONTINUE:
			i_Run_Control = iCmd;
		break;

		case Sim_Run_Cmd::RETRY:	// ����� ���´µ� ������ ��� 5�� ����� ��� �������Ѵ�
			i_Run_Position--;
			i_Run_Control = Sim_Run_Cmd::CONTINUE;	
		break;
		// ====================== OHT���� ����ó�� ==========================
		case Sim_Run_Cmd::RESPONSE:
			frmViewLog->Log("OHT�κ��� ��Ŷ ����");

			switch(iTagID)
			{
				case TagID::CLW_TO_MCP_STATUS: 		  	// OHT��������
				{
					frmViewLog->Log("CLW_TO_MCP_STATUS");

					PKT_STATUS_REPORT_ID_03	CmdPkt;
					memcpy(&CmdPkt, &Pkt, iPacketSize);

					// ===== Step 1. Received info process 
					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);
						
						switch(iMajor_Cmd)
						{
							/* �⵿ ON ��������
							1) �⵿�� ON
							2) �ڵ� ON		*/
							case Sim_Major_Cmd::MOVE_ON:
							{
								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 1)	// �⵿�� ON
									&& (CmdPkt.STATUS.WorkingStatus.Auto == 1))		// �ڵ� ON
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT�������� ���� - �⵿���� ����");
								}
								else
								{
									frmViewLog->Log("OHT�������� ���� - �⵿���� �������� ����!!!!");
								}
							}
							break;

							/* �⵿ OFF ��������
							1) �⵿�� OFF
							2) ���� OFF		*/
							case Sim_Major_Cmd::MOVE_OFF:
							{
								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 0)	// �⵿�� OFF
									&& (CmdPkt.STATUS.RunningStatus.Running == 0))	// ���� OFF
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT�������� ���� - �⵿�������� ����");
								}
								else
								{
									frmViewLog->Log("OHT�������� ���� - �⵿�������� �������� ����!!!!");
                                }
							}
							break;

							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// �۾�����(�̵�-����)
							{
							         strLog.sprintf("Status ����, Major Cmd - �۾�����(�̵�) :Wanted Run Pos : %d",(i_Run_Position + 1)); 
									 frmViewLog->Log(strLog);

									frmViewLog->Log("Status ����, Major Cmd - �۾�����(�̵�)");
									strLog.sprintf("���ϴ� ���� ID : %d   ����ġ : %d  ���� : %d  ���� : %d  HP : %d  Working %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP,
													CmdPkt.STATUS.WorkingStatus.NowRunning);
									frmViewLog->Log(strLog);

								if((CmdPkt.STATUS.WorkingStatus.NowRunning == 1)
									&& (CmdPkt.STATUS.RunningStatus.Running == 1)
									&& (bOHT_NOW_RUNNING == false))
									{
										frmViewLog->Log("Ʈ���� ���� �����ϹǷ� ���� ���� ��ٸ�");

										bOHT_NOW_RUNNING = true;
										strLog.sprintf("bOHT_NOW_RUNNING = %d",  bOHT_NOW_RUNNING);
										frmViewLog->Log(strLog);

										break;
									}
								
								if(bOHT_NOW_RUNNING == true)
								{									
									if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// ����ġ����
										&& (CmdPkt.STATUS.RunningStatus.Running == 0))				// ����
										// && (CmdPkt.STATUS.Carriage.Load1 == 0)
										// && (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))

									{
										FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
										tmrRetry->Enabled = false;
										frmViewLog->Log("OHT�������� => �۾����� (�̵�-����) ���� ����");
										
										bOHT_NOW_RUNNING = false;
										frmViewLog->Log("bOHT_NOW_RUNNING = false");
									}
									else
									{
										frmViewLog->Log("OHT�������� => �۾����� (�̵�-����) ���� �������� ����!!!!");
										strLog.sprintf("���ϴ� ���� ID : %d   ����ġ : %d  ���� : %d  ���� : %d  HP : %d",
														StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
														CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
														CmdPkt.STATUS.RunningStatus.Running,
														CmdPkt.STATUS.Carriage.Load1,
														CmdPkt.STATUS.Carriage.LoadUnloadHP);
										frmViewLog->Log(strLog);
	                                }
								}
							}
							break;

							case Sim_Major_Cmd::ORD_UP_NORMAL:		// �۾�����(�ø�-����)
							{
								if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// ����ġ����
									&& (CmdPkt.STATUS.RunningStatus.Running == 0)				// ���� 
									&& (CmdPkt.STATUS.Carriage.Load1 == 1)	// 
									&& (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))	// 
								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT�������� => �۾�����(�ø�-����) ����");
								}
								else
								{
									frmViewLog->Log("OHT�������� => �۾�����(�ø�-����) �������� ����!!!!");
									strLog.sprintf("���ϴ� ���� ID : %d   ����ġ : %d  ���� : %d  ���� : %d  HP : %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP);
									frmViewLog->Log(strLog);
                                }
							}
							break;

							case Sim_Major_Cmd::ORD_DN_NORMAL:		// �۾�����(����-����)
							{
								if((CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)	// ����ġ����
									&& (CmdPkt.STATUS.RunningStatus.Running == 0)				// ���� 
									&& (CmdPkt.STATUS.Carriage.Load1 == 0)	// 
									&& (CmdPkt.STATUS.Carriage.LoadUnloadHP == 1))	// 

								{
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
									tmrRetry->Enabled = false;
									frmViewLog->Log("OHT�������� => �۾�����(����-����) ����");
								}
								else
								{
									frmViewLog->Log("OHT�������� => �۾�����(����-����)�������� ����!!!!");
									strLog.sprintf("���ϴ� ���� ID : %d  ����ġ : %d  ���� : %d  ���� : %d  HP : %d",
													StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1]),
													CmdPkt.STATUS.RunningStatus.CorrectPositionStop,
													CmdPkt.STATUS.RunningStatus.Running,
													CmdPkt.STATUS.Carriage.Load1,
													CmdPkt.STATUS.Carriage.LoadUnloadHP);
									frmViewLog->Log(strLog);
                                }
							}
							break;


							default:
							{
								frmViewLog->Log("CLW_TO_MCP_STATUS - default case!");
							}
							break;
						}
					}

					// ===== Step 2. OHT Status Display
					frmViewLog->DisplayOHT(CmdPkt);
					strLog.sprintf("WorkingStatus.NowRunning : %d  RunningStatus.Running : %d",
									CmdPkt.STATUS.WorkingStatus.NowRunning, CmdPkt.STATUS.RunningStatus.Running);
					frmViewLog->Log(strLog);
				}
				break;

				case TagID::JOB_ORDER_RESPONSE: 		// �۾����� ����
				{
					frmViewLog->Log("JOB_ORDER_RESPONSE");

					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						PKT_JOB_ORDER_RESPONSE_ID_122	CmdPkt;
						memcpy(&CmdPkt, &Pkt, iPacketSize);

						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);

						switch(iMajor_Cmd)
						{
							/* �۾����� ����(�̵�/����) ��������
							1) ���� ID�� ��ġ
							2) ���䱸�� = ��������
							3) �����Ұ����� = ����
							4) ���� ���� = �۾�����
							5) ���û���Ŭ = �̵� ����Ŭ (����Ʈ) */
							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// �۾�����(�̵�-����)
							{
								strLog.sprintf("�۾����� ����(�̵�) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
								frmViewLog->Log(strLog);

							    strLog.sprintf("�۾����� ���� - ���ǺҸ���, ���䱸�� : %d  �����Ұ����� : %d ���� ���� : %d ���û���Ŭ : %d ���� ID : %d",
														CmdPkt.BODY.ResponseAck.PresionCode,
														CmdPkt.BODY.ResponseAck.Accept_Error,
														CmdPkt.BODY.ResponseDiv.CMDType,
														CmdPkt.BODY.CmdCycle,
														CmdPkt.BODY.CmdID);
								frmViewLog->Log(strLog);
									  
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// ���䱸��
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// �����Ұ�����
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)				// ���� ���� = �۾�����
								  && (CmdPkt.BODY.CmdCycle == CYCLE::MovingToStation)) 						// ���û���Ŭ = �̵� ����Ŭ
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("�۾����� ���� - �̵�/���� : ���� ����");
								  }
								  else
								  {
								  	  strLog.sprintf("�۾����� ���� - �̵�/���� :Wanted Run Pos : %d",(i_Run_Position + 1)); 
									  frmViewLog->Log(strLog);
									  
									  strLog.sprintf("�۾����� ���� - ���ǺҸ���, ���䱸�� : %d  �����Ұ����� : %d ���� ���� : %d ���û���Ŭ : %d ���� ID : %d",
														CmdPkt.BODY.ResponseAck.PresionCode,
														CmdPkt.BODY.ResponseAck.Accept_Error,
														CmdPkt.BODY.ResponseDiv.CMDType,
														CmdPkt.BODY.CmdCycle,
														CmdPkt.BODY.CmdID);
									  frmViewLog->Log(strLog);
                                  }
							}
							break;

							/* �۾����� ����(�ø�/����) ��������
							1) ���� ID�� ��ġ
							2) ���䱸�� = ��������
							3) �����Ұ����� = ����
							4) ���� ���� = �۾�����
							5) ���û���Ŭ = �ø� ����Ŭ (����Ʈ) */
							case Sim_Major_Cmd::ORD_UP_NORMAL:		// �۾�����(�ø�-����)
							{
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// ���䱸��
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// �����Ұ�����
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)				// ���� ���� = �۾�����
								  && (CmdPkt.BODY.CmdCycle == CYCLE::CraneUp)) 								// ���û���Ŭ = �ø� ����Ŭ
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("�۾����� ���� -  �ø�/���� : ���� ����");
								  }
								  else
								  {
								  	strLog.sprintf("Wanted Run Pos : %d",(i_Run_Position + 1)); 
									frmViewLog->Log(strLog);
								  	strLog.sprintf("�۾����� ���� -  �ø�/���� �Ҹ��� \r\n ���� ID : %d  ���䱸�� : %d  �����Ұ����� : %d  ���� ���� : %d  ���û���Ŭ : %d",
												CmdPkt.BODY.CmdID, 						CmdPkt.BODY.ResponseAck.PresionCode,
												CmdPkt.BODY.ResponseAck.Accept_Error,	CmdPkt.BODY.ResponseDiv.CMDType,
												CmdPkt.BODY.CmdCycle);

									frmViewLog->Log(strLog);
                                  }
							}
							break;


							/* �۾����� ����(����/����) ��������
							1) ���� ID�� ��ġ
							2) ���䱸�� = ��������
							3) �����Ұ����� = ����
							4) ���� ���� = �۾�����
							5) ���û���Ŭ = ���� ����Ŭ (����Ʈ)	*/
							case Sim_Major_Cmd::ORD_DN_NORMAL:		// �۾�����(����-����)
							{
								if((CmdPkt.BODY.CmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ResponseAck.PresionCode == 0)								// ���䱸��
								  && (CmdPkt.BODY.ResponseAck.Accept_Error == 0)							// �����Ұ�����
								  && (CmdPkt.BODY.ResponseDiv.CMDType == ORDERDIV::JobOrder)   				// ���� ���� = �۾�����
								  && (CmdPkt.BODY.CmdCycle == CYCLE::CraneDown)) 							// ���û���Ŭ = ���� ����Ŭ
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("�۾����� ���� -  ����/���� : ���� ����");
								  }
								  else
								  {
								  	  strLog.sprintf("�۾����� ���� -  ����/���� : Wanted Run Pos : %d",(i_Run_Position + 1)); 
									  frmViewLog->Log(strLog);
								  	// strLog.sprintf("
									// frmViewLog->Log("�۾����� ���� -  ����/���� : ���� �������� ����!!!!");
								  	strLog.sprintf("�۾����� ���� -  ����/���� �Ҹ��� \r\n ���� ID : %d  ���䱸�� : %d  �����Ұ����� : %d  ���� ���� : %d  ���û���Ŭ : %d",
												CmdPkt.BODY.CmdID, 						CmdPkt.BODY.ResponseAck.PresionCode,
												CmdPkt.BODY.ResponseAck.Accept_Error,	CmdPkt.BODY.ResponseDiv.CMDType,
												CmdPkt.BODY.CmdCycle);
									frmViewLog->Log(strLog);
								  }
							}
							break;
							
							default:
							{
								frmViewLog->Log("JOB_ORDER_RESPONSE - default case!");
							}
							break;
						}
					}						
				}
				break;

				case TagID::JOB_PROGRESS_REPORT: 		// ��ô���� ����
				{
					frmViewLog->Log("JOB_PROGRESS_REPORT");

					strLog.sprintf("��ô���� ���� : Wanted Run Pos : %d",(i_Run_Position + 1)); 
					frmViewLog->Log(strLog);


								
					if(IsSendCmd(i_Run_Position + 1) == false)	// if response...
					{
						PKT_JOB_PROGRESS_REPORT_ID_103	CmdPkt;
						memcpy(&CmdPkt, &Pkt, iPacketSize);

						GetRunningCmd((i_Run_Position+1), iMajor_Cmd, iMinor_Cmd);

						strLog.sprintf("�۾����� ���� - ���ǺҸ���, ProgressType : %d  CorrectPositionStop :  %d",
														CmdPkt.BODY.ProgressType,
														CmdPkt.STATUS.RunningStatus.CorrectPositionStop);
						frmViewLog->Log(strLog);

						switch(iMajor_Cmd)
						{
							/* ��ô���� ����(�̵�/����) ��������
							1) ���� ID�� ��ġ
							2) ��ô���� = ����
							3) ����ġ ���� = ON
							4) ������ = OFF  	*/
							case Sim_Major_Cmd::ORD_MOVE_NORMAL:	// ��ô���� (�̵�-����)
							{

								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::Arrived) 					   		// ��ô���� = ����
								  && (CmdPkt.STATUS.RunningStatus.CorrectPositionStop == 1)) 					// ����ġ ���� = ON, ������ = OFF
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("��ô���� ���� -  �̵�/���� : ���� ����");
								  }
								  else
								  {
									  frmViewLog->Log("��ô���� ���� -  �̵�/���� : ���� �������� ����!!!!");
                                  }
							}
							break;

							/* ��ô���� ����(�ø�/����) ��������
							1) ���� ID�� ��ġ
							2) ��ô���� = ����Ϸ�
							3) ��� = ON 		*/
							case Sim_Major_Cmd::ORD_UP_NORMAL:		// ��ô���� (�ø�-����)
							{
								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::TransCompleted) 					// ��ô���� = ����Ϸ�
								  && (CmdPkt.STATUS.Carriage.Load1 == 1))										// ��� = ON
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("��ô���� ���� -  �ø�/���� : ���� ����");
								  }
								  else
								  {
									frmViewLog->Log("��ô���� ���� -  �ø�/���� : ���� �������� ����!!!!");
									strLog.sprintf("���� ID : %d  ��ô���� : %d  ��� : %d",
											CmdPkt.BODY.RcvCmdID, CmdPkt.BODY.ProgressType, CmdPkt.STATUS.Carriage.Load1);
									frmViewLog->Log(strLog);
                                  }
							}
							break;

							/* ��ô���� ����(����/����) ��������
							1) ���� ID�� ��ġ
							2) ��ô���� = ����Ϸ�
							3) ��� = OFF		*/
							case Sim_Major_Cmd::ORD_DN_NORMAL:		// ��ô���� (����-����)
							{
								if((CmdPkt.BODY.RcvCmdID == StrToInt(FeederGrid->Cells[GRD_CMD_ID][i_Run_Position+1])) 	// ���� ID
								  && (CmdPkt.BODY.ProgressType == PROGRESS::TransCompleted) 					// ��ô���� = ����Ϸ�
								  && (CmdPkt.STATUS.Carriage.Load1 == 0))										// ��� = OFF
								  {
									FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";
									tmrRetry->Enabled = false;
									frmViewLog->Log("��ô���� ���� -  ����/���� : ���� ����");
								  }
								  else
								  {
									frmViewLog->Log("��ô���� ���� -  ����/���� : ���� �������� ����!!!!");
									strLog.sprintf("���� ID : %d  ��ô���� : %d  ��� : %d",
											CmdPkt.BODY.RcvCmdID, CmdPkt.BODY.ProgressType, CmdPkt.STATUS.Carriage.Load1);
									frmViewLog->Log(strLog);
                                  }
							}
							break;

							default:
							{
								frmViewLog->Log("JOB_PROGRESS_REPORT - default case!");
							}
							break;

                        }
					}					
				}
				break;

				case TagID::CMD_PASSPERMIT_OHT2OCS:		// ����㰡 �䱸
				{
					frmViewLog->Log("����㰡 �䱸 ���� - �ڵ� �ڵ鸵 ����");
					PKT_PASSPERMIT_OHT2OCS_ID_241			CmdPkt_241;
					memcpy(&CmdPkt_241, &Pkt, iPacketSize);
					frmPasspermitOCS->Trigger(CmdPkt_241);
				}

				break;

				case TagID::CMD_PASSPERMITOPEN_OHT2OCS:	// ����㰡 ����䱸
				{
					frmViewLog->Log("����㰡 ����䱸 ���� - �ڵ� �ڵ鸵 ����");
					PKT_CMD_PASSPERMITOPEN_OHT2OCS_ID_243	CmdPkt_243;
					memcpy(&CmdPkt_243, &Pkt, iPacketSize);
					frmOcsPassConfirm->Trigger(CmdPkt_243);
				}
				break;

				default: 	// �����ϸ� �Ǵ� ���
				{
					frmViewLog->Log("Undefined Scenario case!!! Check please");
					// ShowMessage(L"Undefined Scenario case!");
				}
				break;
			}
		break;
 	}

	// ========== Status Machine���� ���α׷��� ���� �ڵ����� Ŀ�ǵ� ���� ==========
	if(i_Run_Control == Sim_Run_Cmd::CONTINUE)
	{
		// ���� ������ ����� Ŀ�ǵ带 ������� �ϴ� ���
		if(IsSendCmd(i_Run_Position + 1) == true)
		{
			strLog.sprintf("(������۷�ƾ (������ �ϴ� �����) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
			frmViewLog->Log(strLog);
			
			if(Send2OHT(i_Run_Position + 1) > 0) 	// �������� ���� OK
			{
				FeederGrid->Cells[GRD_NUM][++i_Run_Position] = "R";	// �ش��� ���������� �˷��ش�
			}
		}
		else
		{
			strLog.sprintf("(������۷�ƾ (������ �ȵ�) : Wanted Run Pos : %d",(i_Run_Position + 1)); 
			frmViewLog->Log(strLog);		
		}

		// ���� �ݺ�ó�� (Ʈ���� Ÿ�̸� Ȱ��ȭ)
		// if(((i_Run_Position + 1) == FeederGrid->RowCount)
		// 	&& (FeederGrid->Cells[GRD_NUM][i_Run_Position + 1] == "R"))
		strLog.sprintf("Run Pos : %d   RowCount : %d", (i_Run_Position + 1), FeederGrid->RowCount);
		frmViewLog->Log(strLog);
		
		if ((i_Run_Position + 1) == FeederGrid->RowCount)
		{
			
			frmViewLog->Log("(������� �ʱ�ȭ)");
			for(int i = 0; i < i_Cmd_Count; i++)
			{
				FeederGrid->Cells[GRD_NUM][i+1] = IntToStr(i+1);			// No �ʱ�ȭ
			}

			tmrCmdSend->Enabled = true;								// �ٽ� ���۹�ư�� �������� �Ѵ�
		}

	}
	edtRunCount->Text = IntToStr(i_Run_Position + 1);
}

//---------------------------------------------------------------------------
// �� �Լ��� ����� ������ ���̴�. ���� Major�� �����ϸ� �ȴ�.
// �׸��嵵 0���� �ε����� �����ϸ�, ����� 1���� �����ϹǷ� �ް����� �ʵ��� �Ѵ�

// ���� ! PKT_OCS_STATUS �� OHT�� ��� ���¸� ������ ����ϴ� �뵵�̹Ƿ�
//        ���� �ʱ�ȭ ���� ����
int TfrmAutoJob::Send2OHT(int iGrdIndex)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	int iMajor_Cmd, iMinor_Cmd;
	// int iStatusSize = sizeof(PROTOCOL_STATUS_OCS);
	unsigned char cHigh, cMid, cLow;

	AnsiString strLog;

	GetRunningCmd(iGrdIndex, iMajor_Cmd, iMinor_Cmd);

	strLog.sprintf("Send2OHT() %d", iGrdIndex);
	frmViewLog->Log(strLog);
	
	switch(iMajor_Cmd)
	{
		// --------------- �⵿
		case Sim_Major_Cmd::MOVE_ON:
		{
			iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			PKT_OCS_STATUS_ID_01	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS���� �ʱ�ȭ - �������� �о�´�
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::MCP_TO_CLW_STATUS;

			// --- Status ����
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;	// �⵿ ON

			// ������ ����
			Conv_PKT_OCS_STATUS_ID_01(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
				return UNICAST_SEND_ADDRESS_NOT_RESISTED;
			}

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// ���°��� �����صд�
			tmrRetry->Enabled = true;
			frmViewLog->Log("�⵿��� �۽�");
		}
		break;

		// --------------- ����
		case Sim_Major_Cmd::MOVE_OFF:
		{
			iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			PKT_OCS_STATUS_ID_01	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS���� �ʱ�ȭ - �������� �о�´�
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::MCP_TO_CLW_STATUS;

			// --- Status ����
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 0;	// �⵿ OFF

			// ��������
			if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 		== "���న��+���簨��")
				iTemp = bit_0 + bit_4;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "���న��+������")
				iTemp = bit_0 + bit_5;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "������+���簨��")
				iTemp = bit_1 + bit_4;
			else if(FeederGrid->Cells[GRD_STOP_ORDER][iGrdIndex] 	== "������+������")
				iTemp = bit_1 + bit_5;
			else
			{
				ShowMessage(L"���� �ɼǿ� ����ġ ���� ���� �����մϴ�!!");
				return -1;
			}

			memcpy(&SEND_PACKET.STATUS.StopOrder, &iTemp, 1);

			// ������ ����
			Conv_PKT_OCS_STATUS_ID_01(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
				return UNICAST_SEND_ADDRESS_NOT_RESISTED;
			}

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// ���°��� �����صд�
			tmrRetry->Enabled = true;
			frmViewLog->Log("������� �۽�");
		}
		break;

		// --------------- �۾����� - �̵�����Ŭ (����)
		case Sim_Major_Cmd::ORD_MOVE_NORMAL:
		{
			bOHT_NOW_RUNNING = false;
				
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);

			// OCS���� �ʱ�ȭ - �������� �о�´�
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// �۾�����

			// --- Status
			// �������� - �⵿ ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;

			// ��������, ��������, ������������, E/M����, ������ ��������  - ���� ����

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// ���� ID

			iTemp = ORDERDIV::JobOrder;						// ���ñ��� - �۾�����
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// ���ñ���

			// ������ Buffer ��ġ
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// ��1 ���ϴ�

			iTemp = CYCLE::MovingToStation;  				// ���û���Ŭ - �̵� ����Ŭ
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);

			// �켱��
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7��Ʈ�ϱ�
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// �¿� �켱����
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = 1;		// Driving flag

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// ������ STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// ��� OFF �̸� ����Tag�� �̵�
			SEND_PACKET.BODY.MarkType = StrToInt(FeederGrid->Cells[GRD_STOP_TAG][iGrdIndex]);	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (item deleted!!)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 1;	// TeachingData_FinalPortType - EQ set  
																// ver 29-5 : bit field, ver 37 : unsigned char (Changed)			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// ver 29-5 : bit field, ver 37 : unsigned char (Changed)
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_Left = 1;
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_RF = 1;
			SEND_PACKET.BODY.TeachingData_PIOInfo.PIO_CS0 = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// ver 29-5 : bit field, ver 37 : unsigned char (Changed)

			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;

			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(",");					// �޸� ��ġŽ��
					strWork = strTmp.SubString(0, (iPos - 1));	// ��ū 1�� ����
					strDim[iCnt++] = strWork.Trim();			// �����Ͽ� ����
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim();					// ������ ���� �Է�
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
             // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("�۾�����22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//				SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			//frmViewLog->Log(strLog);
			// �������� ������ �ִ� ������ = ����� �� ������� �𸣰ڴ�

			// ������ ����
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// ���°��� �����صд�
			tmrRetry->Enabled = true;

			frmViewLog->Log("�۾����� - �̵�����Ŭ (����) ��� �۽�");
		}
		break;

		// --------------- �۾����� - �ø�����Ŭ (����)
		case Sim_Major_Cmd::ORD_UP_NORMAL:
		{
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);
		
			// OCS���� �ʱ�ȭ - �������� �о�´�
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// �۾�����

			// --- Status
			// �������� - �⵿ ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 1;	// �ø� ���� �㰡
			SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 0;

			// ��������, ��������, ������������, E/M����, ������ ��������  - ���� ����

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// ���� ID

			iTemp = ORDERDIV::JobOrder; 					// ���ñ��� - �۾�����
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// ���ñ���

			// ������ Buffer ��ġ
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// ��1 ���ϴ�

			iTemp = CYCLE::CraneUp;  						// ���û���Ŭ - �ø� ����Ŭ
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);
		
			// �켱��
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7��Ʈ�ϱ�
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// �¿� �켱����
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;

			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = StrToInt(FeederGrid->Cells[GRD_DRIVING][iGrdIndex]);		// Driving flag

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// ������ STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// ��� OFF �̸� ����Tag�� �̵�
			SEND_PACKET.BODY.MarkType = 1;	// fixed value // StopTag = 1 Setting


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (Protocol deleted)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 2; // TeachingData_FinalPortType - EQ set
															 // = PortType_2 = 1 Set Value!!
			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// = TeachingData_LookDownLevel.LKDown_1 = 1;
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// PIOTimeLevel.PIOTime_1 = 1;
			
			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;
		
			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(","); 				// �޸� ��ġŽ��
					strWork = strTmp.SubString(0, (iPos - 1));	// ��ū 1�� ����
					strDim[iCnt++] = strWork.Trim();			// �����Ͽ� ����
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim(); 				// ������ ���� �Է�
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
			 // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("�۾�����22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//	SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			//frmViewLog->Log(strLog);
			// �������� ������ �ִ� ������ = ����� �� ������� �𸣰ڴ�

			// ������ ����
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// ���°��� �����صд�

			frmViewLog->Log("�۾����� - �ø�����Ŭ (����) ��� �۽�");
		}		
		break;

		// --------------- �۾����� - ��������Ŭ (����)
		case Sim_Major_Cmd::ORD_DN_NORMAL:
		{
			iPacketSize = sizeof(PKT_JOB_ORDER_ID_121);
			PKT_JOB_ORDER_ID_121	SEND_PACKET;
			memset(&SEND_PACKET, 0, iPacketSize);
		
			// OCS���� �ʱ�ȭ - �������� �о�´�
			memcpy(&SEND_PACKET.HEADER, &PKT_OCS_STATUS.HEADER, sizeof(PROTOCOL_HEADER));
			memcpy(&SEND_PACKET.STATUS, &PKT_OCS_STATUS.STATUS, sizeof(PROTOCOL_STATUS_OCS));

			// ---- Tag
			SEND_PACKET.TAG.Value = TagID::JOB_ORDER;	// �۾�����

			// --- Status
			// �������� - �⵿ ON
			SEND_PACKET.STATUS.StatusOrder.MoveStart = 1;
			SEND_PACKET.STATUS.StatusOrder.CraneUpAdmission = 0;
			SEND_PACKET.STATUS.StatusOrder.CraneDownAdmission = 1;	// ���� ���� �㰡

			// ��������, ��������, ������������, E/M����, ������ ��������  - ���� ����

			// =============================== Body ===============================
			SEND_PACKET.BODY.CmdID = StrToInt(FeederGrid->Cells[GRD_CMD_ID][iGrdIndex]);	// ���� ID

			iTemp = ORDERDIV::JobOrder; 					// ���ñ��� - �۾�����
			memcpy(&SEND_PACKET.BODY.CmdDiv, &iTemp, 1);	// ���ñ���

			// ������ Buffer ��ġ
			SEND_PACKET.BODY.BufferType.Buffer_1 = 1;			// ��1 ���ϴ�	// unsigned char -> bit field changed!

			iTemp = CYCLE::CraneDown; 						// ���û���Ŭ - ���� ����Ŭ
			memcpy(&SEND_PACKET.BODY.CmdCycle, &iTemp, 1);
		
			// �켱��
			RIV tmpRv;
			tmpRv.Riv_Value = 64;							// 7��Ʈ�ϱ�
			tmpRv.Hotlot = 0;								// Hotlot - off
			memcpy(&SEND_PACKET.BODY.Riv, &tmpRv, 1);

			// �¿� �켱����
			if(rdoTrunDefault->ItemIndex == 0)
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_LEFT;
			else
				SEND_PACKET.BODY.PathData_Def_Direction.Path_Direction = DefaultDirection::DIR_RIGHT;
			
			SEND_PACKET.BODY.PathData_Def_Direction.Driving_Flag = StrToInt(FeederGrid->Cells[GRD_DRIVING][iGrdIndex]);		// Driving flag

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_START_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_StartNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_StartNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_StartNodeID[LOW_BYTE]  = cLow;

			// ���� NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.PathData_EndNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.PathData_EndNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.PathData_EndNodeID[LOW_BYTE]  = cLow;

			// STOP NODE
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_NODE][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopNodeID[LOW_BYTE]  = cLow;

			// ������ STATION
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_TARGET_STATION][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.StopStationID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.StopStationID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.StopStationID[LOW_BYTE]  = cLow;

			/* 17/01/03
			// ===== 12/10 add ......
			// ��� OFF �̸� ����Tag�� �̵�
			SEND_PACKET.BODY.MarkType = 1;	// MarkType.StopTag = 1;// fixed value


			// SationData_Target_Rundist
			SEND_PACKET.BODY.StationData_RunDist = StrToInt(FeederGrid->Cells[GRD_RUN_DIST][iGrdIndex]);
			*/
			// NextNodeID (protocol deleted)
			/*
			Int2Uchar3((unsigned int)StrToInt (FeederGrid->Cells[GRD_NEXT_NODE_ID][iGrdIndex]), cHigh, cMid, cLow);
			SEND_PACKET.BODY.NextNodeID[HIGH_BYTE] = cHigh;
			SEND_PACKET.BODY.NextNodeID[MID_BYTE]  = cMid;
			SEND_PACKET.BODY.NextNodeID[LOW_BYTE]  = cLow;
			*/
			/* 17/01/03
			// TeachingData_Hoist - Hoist
			SEND_PACKET.BODY.TeachingData_Hoist = StrToInt(FeederGrid->Cells[GRD_HOIST][iGrdIndex]);

			// TeachingData_Shift - Shift
			SEND_PACKET.BODY.TeachingData_Shift = StrToInt(FeederGrid->Cells[GRD_SHIFT][iGrdIndex]);

			// TeachingData_Rotate - Rotate
			SEND_PACKET.BODY.TeachingData_Rotate = StrToInt(FeederGrid->Cells[GRD_ROTATE][iGrdIndex]);

			// ============ fixed value set ============ 
			SEND_PACKET.BODY.TeachingData_FinalPortType = 2; // FinalPortType.PortType_2 = 1; // TeachingData_FinalPortType - EQ set
			
			SEND_PACKET.BODY.TeachingData_LookDownLevel = 1;	// LookDownLevel.LKDown_1 = 1
			
			SEND_PACKET.BODY.TeachingData_PIOInfo.EQPIO_ENABLE = 1;

			SEND_PACKET.BODY.TeachingData_PIOTimeLevel = 1;		// PIOTimeLevel.PIOTime_1 = 1
			
			SEND_PACKET.BODY.TeachingData_Option.TransSound = 1;
			SEND_PACKET.BODY.TeachingData_Option.Oscilation = 1;
			// =========================================
			*/

			// ================= Path Data Parsing Start ===================
			AnsiString strTmp = FeederGrid->Cells[GRD_PATH_DATA][iGrdIndex];
			AnsiString strDim[1000];
			int iCnt = 0;
		
			strTmp = strTmp.Trim();
			// if((strTmp.Trim().Length() != 0)
			if((strTmp != "") && (strTmp != "(null)"))
			{
				AnsiString strWork;
				int iPos = 0;

				while(strTmp.AnsiPos(",") != 0)
				{
					iPos = strTmp.AnsiPos(","); 				// �޸� ��ġŽ��
					strWork = strTmp.SubString(0, (iPos - 1));	// ��ū 1�� ����
					strDim[iCnt++] = strWork.Trim();			// �����Ͽ� ����
					strTmp = strTmp.Delete(1,iPos);
				}

				strDim[iCnt++] = strTmp.Trim(); 				// ������ ���� �Է�
			}

			// ----------------------------------------------------------------------
			SEND_PACKET.BODY.PathData_ExtraNum = iCnt;

			for(int i = 0; i < iCnt; i++)
			{
				Int2Uchar3((unsigned int)StrToInt (strDim[i]), cHigh, cMid, cLow);
				
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + HIGH_BYTE] = cHigh;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + MID_BYTE]  = cMid;
				SEND_PACKET.BODY.PathData_ExtraID[(i*3) + LOW_BYTE]  = cLow;
			}
			// ================= Path Data Parsing End ===================
			 // shkim
			//strLog.sprintf("MOVE ORDER - Mark Type : %d OrderID : %d  RunDist : %d ", SEND_PACKET.BODY.MarkType, SEND_PACKET.BODY.CmdID, SEND_PACKET.BODY.StationData_RunDist);
			//frmViewLog->Log(strLog);

			//strLog.sprintf("�۾�����22 (ID:121) [HEADER] StopNodeID : %d MarkType : %d ",
			//	SEND_PACKET.BODY.StopNodeID, SEND_PACKET.BODY.MarkType);
			frmViewLog->Log(strLog);
			// �������� ������ �ִ� ������ = ����� �� ������� �𸣰ڴ�

			// ������ ����
			Conv_PKT_JOB_ORDER_ID_121(SEND_PACKET);
			iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
				ShowMessage(L"Address not registed!!, try first");

			memcpy(&PKT_OCS_STATUS.STATUS, &SEND_PACKET.STATUS, sizeof(PROTOCOL_STATUS_OCS));	// ���°��� �����صд�

			frmViewLog->Log("�۾����� - ��������Ŭ (����) ��� �۽�");
		}
		break;
    }

	return iReturn;
}

//---------------------------------------------------------------------------
// �� �Լ��� ����� ������ �ϴ� ������, �޾ƾ� �ϴ� ������ �Ǵ��Ѵ�
// OCS�� ������ �ϴ� ��� true ����, �ƴϸ� false ����
bool TfrmAutoJob::IsSendCmd(int iGrdIndex)
{
	if((FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "MCP_TO_CLW_STATUS")
	   || (FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER"))
		return true;
	else
		return false;
}
//---------------------------------------------------------------------------

void TfrmAutoJob::GetRunningCmd(int iGrdIndex, int& iMajor, int& iMinor)
{
	// Major Cmd
	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "MOVE_ON")
		iMajor = Sim_Major_Cmd::MOVE_ON;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "MOVE_OFF")
		iMajor = Sim_Major_Cmd::MOVE_OFF;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_MOVE_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_MOVE_NORMAL;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_UP_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_UP_NORMAL;

	if(FeederGrid->Cells[GRD_MAIN_CMD][iGrdIndex] == "ORD_DN_NORMAL")
		iMajor = Sim_Major_Cmd::ORD_DN_NORMAL;

	// Minor Cmd
	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "MCP_TO_CLW_STATUS")
		iMinor = TagID::MCP_TO_CLW_STATUS;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "CLW_TO_MCP_STATUS")
		iMinor = TagID::CLW_TO_MCP_STATUS;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER")
		iMinor = TagID::JOB_ORDER;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_ORDER_RESPONSE")
		iMinor = TagID::JOB_ORDER_RESPONSE;

	if(FeederGrid->Cells[GRD_SUB_CMD][iGrdIndex] == "JOB_PROGRESS_REPORT")
		iMinor = TagID::JOB_PROGRESS_REPORT;
}


//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnRunClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= true;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RUN, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btbPauseClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= false;
	btnResume->Enabled	= true;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::PAUSE, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnResumeClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= false;
	btbPause->Enabled 	= true;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= true;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::CONTINUE, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnStopClick(TObject *Sender)
{
	tmrRetry->Enabled = false;
	btnRun->Enabled		= true;
	btbPause->Enabled 	= false;
	btnResume->Enabled	= false;
	btnStop->Enabled 	= false;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::STOP, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::Button1Click(TObject *Sender)
{
	// AnsiString strTmp = " 12,3,456,2,AC,B";
	AnsiString strTmp = " 12";
	AnsiString strDim[100];
	int iCnt = 0;

	if(strTmp.Trim().Length() != 0)
	{
		AnsiString strWork;
		int iPos = 0;

		while(strTmp.AnsiPos(",") != 0)
		{
			iPos = strTmp.AnsiPos(",");		// �޸� ��ġŽ��
			strWork = strTmp.SubString(0, (iPos - 1));	// ��ū 1�� ����
			strDim[iCnt++] = strWork.Trim();			// �����Ͽ� ����
			strTmp = strTmp.Delete(1,iPos);
		}

		strDim[iCnt++] = strTmp.Trim();					// ������ ���� �Է�
	}
}

//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrCmdSendTimer(TObject *Sender)
{
	tmrCmdSend->Enabled = false;

	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RUN, Pkt, 0, 0);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::chkAutoOcsStatusClick(TObject *Sender)
{
	if(chkAutoOcsStatus->Checked == true)
		tmrOcsStatus->Enabled = true;
	else
		tmrOcsStatus->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrOcsStatusTimer(TObject *Sender)
{
	static int iCount = 0;
	int iReturn;

	if((btnRun->Enabled == false) && (btbPause->Enabled == true))
	{
		iCount++;

		if(iCount == 10)
		{
			iCount = 0;

			// ������ ����
			int iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);	// (1) �ϴ� Endian ��ȯ�� �� ����
			iReturn = frmUDP->UnicastSend((char*)&PKT_OCS_STATUS, iPacketSize);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);	// (2) ����� �ٽ� �ǵ��� ���� �Ѵ�

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
			}
		}

		prgAutoSend->Position = iCount;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnSaveClick(TObject *Sender)
{
	JobSaveAsDialog->FileName = CurrentFileName.c_str();
    JobSaveAsDialog->Options.Clear();
    JobSaveAsDialog->Options << ofOverwritePrompt;
    JobSaveAsDialog->DefaultExt = "prg";
    JobSaveAsDialog->Filter = "Job Program files (*.prg)|*.prg|All files (*.*)|*.*";
    JobSaveAsDialog->FilterIndex = 1; // start the dialog showing all files

	JobSaveAsDialog->InitialDir = CurrentFileName.SubString(1, CurrentFileName.LastDelimiter("\\")-1).c_str();

	if(JobSaveAsDialog->Execute())
	{
		FILE *stream;
		CurrentFileName = JobSaveAsDialog->FileName.c_str();

		// ���� Ȯ���� �߰�
		if(CurrentFileName.Pos(".") == 0) CurrentFileName = CurrentFileName + ".prg";

		// ���� ���ϸ�� ���� ��� �ڵ����
		if(FileExists(CurrentFileName.c_str()))
		{
			UnicodeString BackUpFile = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "bak";
			UnicodeString OrgFile = CurrentFileName;
			CopyFile(OrgFile.c_str(), BackUpFile.c_str(), false);
		}

		// ���� ������ ���� ���� ����
		UnicodeString strPasspermit  = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "pmt";
		frmPasspermitOCS->SaveFile(strPasspermit);

		UnicodeString strPassConfirm = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "con";
		frmOcsPassConfirm->SaveFile(strPassConfirm);

		// ���Ͽ� ����
		try
		{
			stream = fopen(CurrentFileName.c_str(), "w+");
			if(stream == NULL)
			{
				ShowMessage(L"������ ������ Open����!");
				return;
			}

			edtFileName->Text = CurrentFileName;	// ���� ���ϸ� ǥ��

			AnsiString strTmp[GRID_MAX_COL];

			// ��� ��� (�б����, ����㰡, ����䱸, ������ ����)
			fprintf(stream, "%d %d %d %d\r\n", rdoTrunDefault->ItemIndex, rdoGrpPass->ItemIndex, rdoGrpOpenReq->ItemIndex, StrToInt(edtCount->Text));
			// OCS ���º���
			if(chkAutoOcsStatus->Checked)
				fprintf(stream, "%d\r\n", 1);
			else
				fprintf(stream, "%d\r\n", 0);

			// strTmp[0] = 0;
			for(int iRowCnt = 0;  iRowCnt < i_Cmd_Count; iRowCnt++)
			{
				for(int idx1 = 0; idx1 < GRID_MAX_COL; idx1++)
					strTmp[idx1] = FeederGrid->Cells[idx1][iRowCnt + 1];
				// strTmp[0] = strTmp[0] + 1;

				fprintf(stream,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\r\n"
								,strTmp[0],strTmp[1],strTmp[2],strTmp[3],strTmp[4]
								,strTmp[5],strTmp[6],strTmp[7],strTmp[8],strTmp[9]
								,strTmp[10],strTmp[11],strTmp[12],strTmp[13],strTmp[14]
								,strTmp[15],strTmp[16],strTmp[17]);
			}

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
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnLoadClick(TObject *Sender)
{
	FILE *fi;
	int iTmp[4];
	int iOcsSend;

	char strTmp[GRID_MAX_COL][1024];
	AnsiString m_strFileName;

	JobOpenDialog->InitialDir = GetCurrentDir(); // ProgramfilesPath.c_str();
	JobOpenDialog->Title = "Open a Job Program";
	JobOpenDialog->FileName = "";
	JobOpenDialog->Options.Clear();
	JobOpenDialog->Options << ofFileMustExist;
	JobOpenDialog->Filter = "Job program files (*.prg)|*.prg|All files (*.*)|*.*";
	JobOpenDialog->FilterIndex = 1; // start the dialog showing all files

	if (JobOpenDialog->Execute())
	{
		// ������ �ֳ� Ȯ��
		if(!FileExists(JobOpenDialog->FileName.c_str()) || JobOpenDialog->FileName.Length() < 1)
		{
			Application->MessageBox(L"File not exist", L"File Error", MB_OK);
			return;
		}


		try
		{
			// ���� ����
			m_strFileName = JobOpenDialog->FileName;
			if( (fi = fopen( m_strFileName.c_str(), "r" )) == NULL )
			{
				ShowMessage(L"���� ���Ⱑ �����߽��ϴ�");
				return;
			}

			edtFileName->Text = m_strFileName;	// ���� ���ϸ� ǥ��

			GridClear();

			// ��� �б�
			fscanf(fi,"%d %d %d %d \n", &iTmp[0], &iTmp[1], &iTmp[2], &iTmp[3]);

			rdoTrunDefault->ItemIndex 	= iTmp[0];
			rdoGrpPass->ItemIndex 		= iTmp[1];
			rdoGrpOpenReq->ItemIndex 	= iTmp[2];

			// ��ɾ� ����
			edtCount->Text 				= IntToStr(iTmp[3]);
			i_Cmd_Count					= iTmp[3];

			fscanf(fi,"%d \n", &iOcsSend);
			
			if(iOcsSend)
				chkAutoOcsStatus->Checked = true;
			else
				chkAutoOcsStatus->Checked = false;

			for(int iRowCnt = 0;  iRowCnt < iTmp[3]; iRowCnt++)
			{
				// 1 Line Read
				for (int iColCnt = 0; iColCnt < GRID_MAX_COL; iColCnt++)
				{
					fscanf(fi,"%s", &strTmp[iColCnt]); 
				}
				
				// fscanf(fi,"%s %s %s %s %s %s %s %s %s %s", &strTmp[0], &strTmp[1],&strTmp[2],&strTmp[3],&strTmp[4],&strTmp[5],&strTmp[6]);

				FeederGrid->RowCount = FeederGrid->RowCount + 1;

				for(int idx1 = 0; idx1 < GRID_MAX_COL; idx1++)
					FeederGrid->Cells[idx1][iRowCnt + 1] = strTmp[idx1];
			}

			fclose(fi);

			// --------- �ٸ� ���ϵ� ó��
			CurrentFileName = m_strFileName;

			UnicodeString strPasspermit  = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "pmt";
			frmPasspermitOCS->LoadFile(strPasspermit);

			UnicodeString strPassConfirm = CurrentFileName.SubString(1, CurrentFileName.Length()-3) + "con";
			frmOcsPassConfirm->LoadFile(strPassConfirm);
		}
		catch(...)
		{
			if(fi)
				fclose(fi);
			fi = NULL;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::FormClose(TObject *Sender, TCloseAction &Action)
{
	delete frmPasspermitOCS;
	delete frmOcsPassConfirm;

	delete frmViewLog;

	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnPassConfirmSetClick(TObject *Sender)
{
	ShowWindow(frmPasspermitOCS->Handle,SW_SHOW);
	frmPasspermitOCS->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::btnOpenResponseSetClick(TObject *Sender)
{
	ShowWindow(frmOcsPassConfirm->Handle,SW_SHOW);
	frmOcsPassConfirm->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::rdoGrpPassClick(TObject *Sender)
{
	frmPasspermitOCS->SetResponseMode(rdoGrpPass->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::rdoGrpOpenReqClick(TObject *Sender)
{
	frmOcsPassConfirm->SetResponseMode(rdoGrpOpenReq->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall TfrmAutoJob::btnViewStatusClick(TObject *Sender)
{
	ShowWindow(frmViewLog->Handle,SW_SHOW);
	frmViewLog->BringToFront();
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::Button2Click(TObject *Sender)
{
			int iPacketSize = sizeof(PKT_OCS_STATUS_ID_01);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);
			int iReturn = frmUDP->UnicastSend((char*)&PKT_OCS_STATUS, iPacketSize);
			Conv_PKT_OCS_STATUS_ID_01(PKT_OCS_STATUS);

			if(iReturn == UNICAST_SEND_ADDRESS_NOT_RESISTED)
			{
				ShowMessage(L"Address not registed!!, try first");
			}
}
//---------------------------------------------------------------------------

void __fastcall TfrmAutoJob::tmrRetryTimer(TObject *Sender)
{
	PKT_OHT2OCS Pkt;
	StatusMachine(Sim_Run_Cmd::RETRY, Pkt, 0, 0);
}
//---------------------------------------------------------------------------


