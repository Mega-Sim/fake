//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <IniFiles.hpp>
#include <ctime>
#pragma hdrstop
#include "BayTestUnit.h"
#include "Define.h"
#include "OHTInfoUnit.h"
#include "UDP.h"
#include "CMDControlUnit.h"
#include "SocketUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tfrm_Bay_Test *frm_Bay_Test;
//---------------------------------------------------------------------------
__fastcall Tfrm_Bay_Test::Tfrm_Bay_Test(TComponent* Owner)
    : TForm(Owner)
{
	strGridLongrunInfor->ColCount = 6;
	strGridLongrunInfor->RowCount = MAX_DATA;

	strGridLongrunInfor->Cells[0][0] = "No";
	strGridLongrunInfor->Cells[1][0] = "Station ID";
	strGridLongrunInfor->Cells[2][0] = "������ ����";
	strGridLongrunInfor->Cells[3][0] = "����ƼĪ  ����";
	strGridLongrunInfor->Cells[4][0] = "���� ����";
	strGridLongrunInfor->Cells[5][0] = "���� Ƚ��";
	Shape_Status->Brush->Color = COLOR_RED;
	Label_TestStatus->Caption = "*�׽�Ʈ ����*";
    InitData();
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::BayLongRunTimerTimer(TObject *Sender)
{
	int nError = 0;
	static nLongrunCount = 0;

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status == STS_ERROR)
	{
		BayLongRunTimer->Enabled = false;
		nCnt = 0;
		nLongrunCount = 0;
		Step = BAYTEST_STEP::INIT_STEP;
		mmLongRunResult->Lines->Add("[ERR] OHT Error �߻�");
		Label_TestStatus->Caption = "*�׽�Ʈ ����*";
		return;
	}

	StatusDisplay();

	switch(Step)
	{
		case BAYTEST_STEP::INIT_STEP :
			nError = Init_Step();
			if(nError == STATUS_PASS)
			{
				Shape_Status->Brush->Color = COLOR_LIME;
				Label_TestStatus->Caption = "*�׽�Ʈ ��*";
				Step = BAYTEST_STEP::MOVE_STEP;
            }
            break;

		case BAYTEST_STEP::MOVE_STEP :
			nError = Move_Step();
			if(nError == STATUS_PASS)
			{
				Step = BAYTEST_STEP::CHECK_MOVE_STEP;
			}
            break;

		case BAYTEST_STEP::CHECK_MOVE_STEP :
			if(nUseTrans[nCnt] == 2)
				nError = Check_Move_Step(true);
			else
				nError = Check_Move_Step();

			if(nError == STATUS_PASS)
			{
				Label_TestStatus->Caption = "*�̵� �Ϸ�*";
				if(nUseTrans[nCnt] == 1)
				{
					Step = BAYTEST_STEP::TRANS_LOAD_STEP;
				}
				else if(bUseAutoTeaching[nCnt] == true)
				{
					Step = BAYTEST_STEP::AUTO_TEACHING_STEP;
				}
				else
				{
					Step = BAYTEST_STEP::CHECK_COMPLETE_STEP;
				}
			}
			else
			{
				Label_TestStatus->Caption = "*�̵� ��*";
			}
			break;

		case BAYTEST_STEP::TRANS_LOAD_STEP :
			nError = Trans_Load_Step();
			if(nError == STATUS_PASS)
			{
				Step = BAYTEST_STEP::CHECK_TRANS_LOAD_STEP;
			}
			break;

		case BAYTEST_STEP::CHECK_TRANS_LOAD_STEP :
			nError = Check_Trans_Load_Step();
			if(nError == STATUS_PASS)
			{
				Label_TestStatus->Caption = "*������ �Ϸ�*";
				Step = BAYTEST_STEP::TRANS_UNLOAD_STEP;
			}
			else
			{
				Label_TestStatus->Caption = "*������ ��*";
			}
			break;

		case BAYTEST_STEP::TRANS_UNLOAD_STEP :
			nError = Trans_UnLoad_Step();
			if(nError == STATUS_PASS)
			{
				Step = BAYTEST_STEP::CHECK_TRANS_UNLOAD_STEP;
			}
			break;

		case BAYTEST_STEP::CHECK_TRANS_UNLOAD_STEP :
			nError = Check_Trans_UnLoad_Step();
			if(nError == STATUS_PASS)
			{
				Label_TestStatus->Caption = "*������ �Ϸ�*";
				Step = BAYTEST_STEP::CHECK_COMPLETE_STEP;
			}
			else
			{
				Label_TestStatus->Caption = "*������ ��*";
			}
			break;

		case BAYTEST_STEP::AUTO_TEACHING_STEP :
			nError = Auto_Teaching_Step();
			if(nError == STATUS_PASS)
			{
				Step = BAYTEST_STEP::CHECK_AUTO_TEACHING_STEP;
			}
			break;

		case BAYTEST_STEP::CHECK_AUTO_TEACHING_STEP :
			nError = Check_Auto_Teaching_Step();
			 if(nError == STATUS_PASS)
			{
				Label_TestStatus->Caption = "*����ƼĪ �Ϸ�*";
				Step = BAYTEST_STEP::CHECK_COMPLETE_STEP;
			}
			else
			{
				Label_TestStatus->Caption = "*����ƼĪ ��*";
			}
			break;

		case BAYTEST_STEP::CHECK_COMPLETE_STEP :
			nError = Check_Complete_Step();
			if(nError == STATUS_PASS)
			{
				Step = BAYTEST_STEP::MOVE_STEP;
				nLongrunCount++;
				String strLog;
				strLog.sprintf(L"==>LongrunCount[%d]", nLongrunCount);
				SocketForm->BayTestLongrun_Log(strLog);
			}
			break;

	}
	if(nError == STATUS_STOP || nError == STATUS_COMPLETE)
	{
		BayLongRunTimer->Enabled = false;
		//Step = 0;
		nCnt = 0;
		nLongrunCount = 0;
		Shape_Status->Brush->Color = COLOR_RED;
		Label_TestStatus->Caption = "*�׽�Ʈ ����*";
	}
	return;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Init_Step(void)
{
	int nError = STATUS_PASS;
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT && gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED)
	{
		mmLongRunResult->Lines->Add("[ERR] OHT Status Init ���� �ƴ�, OHT Status Clear �� ����� ���ּ���.");
		nError = STATUS_STOP;
	}
	Longrun_per->Position = 0;
	return nError;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Move_Step(void)
{
    int nError = STATUS_PASS;
    int nStartNode = 0;
    int nCurrentNodeID = 0;
	int nCurrentStationID = 0;

    nCurrentNodeID = (int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID;
    nCurrentStationID = (int)gm_OHTStatus->m_OHTStatus.Data.CurStationID;

	if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag == 1)
	{
		nStartNode = nCurrentStationID;
	}
	else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L == 1)
	{
		// EQ ��Ʈ�ε� QR ��ġ�� ��Ʈ�� ���� ó��
		nStartNode = nCurrentStationID;
	}
	else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R == 1)
	{
		nStartNode = nCurrentStationID;
	}
	// Driving Tag Mark
	else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == 1)
	{
		nStartNode = nCurrentNodeID;
	}
	// Mark Fail
	else
	{
		mmLongRunResult->Lines->Add("[ERR] Mark ���з� ���� Test ����");
		nError = STATUS_STOP;
	}

    // �̵� ���
	if(nError == STATUS_PASS)
	{
		if(nStartNode != nStationID[nCnt])
		{
			if(nUseTrans[nCnt] == 2)
			{
				MoveCMD(nStartNode, nStationID[nCnt], true);
			}
			else
			{
				MoveCMD(nStartNode, nStationID[nCnt]);
            }

		}
    }
	return nError;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Check_Move_Step(bool bNode)
{
	int nError = STATUS_PASS;
	int nCurrentStationID = 0;

	if(!bNode)
		nCurrentStationID = (int)gm_OHTStatus->m_OHTStatus.Data.CurStationID;
	else
		nCurrentStationID = (int)gm_OHTStatus->m_OHTStatus.Data.CurNodeID;

	 // ������ ���� ���� Ȯ��
	 if((gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_ARRIVED && gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_INIT) || (nCurrentStationID != nStationID[nCnt]))
     {
		nError = STATUS_WAIT;
	 }
	 else
	 {
		// ���� Mark Type�� ���� ������ ��Ʈ ����
		// �ʿ�� �շ� txt�� �Է� �ʿ�
		if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.StopTag == 1)
		{
			PortType = PORT_TYPE_EQ;
		}
		else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_L == 1)
		{
			// EQ ��Ʈ�ε� QR ��ġ�� ��Ʈ�� ���� ó��
			PortType = PORT_TYPE_STB_L;
		}
		else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.QRLTag_R == 1)
		{
			PortType = PORT_TYPE_STB_R;
		}
		// Driving Tag Mark
		else if(gm_OHTStatus->m_OHTStatus.MarkInfo.Info.DrivingTag == 1)
		{
			PortType = 0x00;
		}
		// Mark Fail
		else
		{
			mmLongRunResult->Lines->Add("[ERR] Mark ���з� ���� Test ����");
			nError = STATUS_STOP;
		}
     }
     return nError;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Trans_Load_Step(void)
{
	int nError = STATUS_PASS;

	if(PortType == PORT_TYPE_STB_L || PortType == PORT_TYPE_STB_R)
	{
		TransCMD(CMD_LOAD);
	}
	else
	{
		mmLongRunResult->Lines->Add( "[ERR] EQ��Ʈ ������ �Ұ�");
		nError = STATUS_STOP;
	}
	return nError;

}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Check_Trans_Load_Step(void)
{
    int nError = STATUS_PASS;
	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_LOADED)
    {
        nError = STATUS_WAIT;
    }
    return nError;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Trans_UnLoad_Step(void)
{
	int nError = STATUS_PASS;

	if(PortType == PORT_TYPE_STB_L || PortType == PORT_TYPE_STB_R)
	{
		TransCMD(CMD_UNLOAD);
	}
	else
	{
		mmLongRunResult->Lines->Add( "[ERR] EQ��Ʈ ������ �Ұ�");
		nError = STATUS_STOP;
	}
	return nError;
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Check_Trans_UnLoad_Step(void)
{
	int nError = STATUS_PASS;

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_UNLOADED)
	{
		nError = STATUS_WAIT;
	}

	return nError;
}

//////////����ƼĪ ���� �߰�/////////////////////////////////////////////////////////////
int Tfrm_Bay_Test::Auto_Teaching_Step(void)
{
	int nError = STATUS_PASS;

	if(PortType == PORT_TYPE_STB_L || PortType == PORT_TYPE_STB_R)
	{
		Auto_Teaching_CMD();
	}
	else
	{
		mmLongRunResult->Lines->Add( "[ERR] EQ��Ʈ ������ �Ұ�");
		nError = STATUS_STOP;
	}
	return nError;

}
int Tfrm_Bay_Test::Check_Auto_Teaching_Step(void)
{
	int nError = STATUS_PASS;

	if(gm_OHTStatus->m_OHTStatus.Data.MSCmdID.Info.Status != STS_AUTO_TAUGHT)  //STS_UNLOADED�� AutoTeaching ���õȰɷ� �ٲٰ� ������, ���°� �������־ ���� �ٲ������?
	{
		nError = STATUS_WAIT;
	}

	return nError;
}

//---------------------------------------------------------------------------
int Tfrm_Bay_Test::Check_Complete_Step(void)
{
	int nError = STATUS_PASS;
	int nStatus = 0;

	// ������ ����
	bTestStaus[nCnt] = true;
	nTestCnt[nCnt] += 1;
	strGridLongrunInfor->Cells[4][nCnt+1] = "Pass";
	strGridLongrunInfor->Cells[5][nCnt+1] = IntToStr(nTestCnt[nCnt]);

	nCnt++;

	nStatus = (nCnt * 100) / nCount;
	if(nStatus > 100)
		nStatus = 100;

	Longrun_per->Position = nStatus;
	mmLongRunResult->Lines->Add("[INFO] " + IntToStr(nStationID[nCnt-1]) + "��Ʈ �׽�Ʈ �Ϸ�");
	mmLongRunResult->Lines->Add("[INFO] �� " + IntToStr(nCount) + "�� �� " + IntToStr(nCnt) + "�� �Ϸ�, ����� : " + IntToStr(nStatus) + "%");

	// Test Case �Ϸ� ���� Ȯ��
	if(nCnt >= nCount || nCnt >= MAX_DATA)
	{
		// �̵� ��� ���� �Ϸ�
		if(bModeType == BAYTEST_MODE::ONCE_MODE)
		{
			mmLongRunResult->Lines->Add("[INFO] Bay Test ��� �Ϸ�");
			nError = STATUS_COMPLETE;
		}
		else if(bModeType == BAYTEST_MODE::REPEAT_MODE)
		{
			nCnt = 0;
			InitTestResult(false);
			mmLongRunResult->Lines->Add("[INFO] Bay Test �ݺ� ���� ��");
		}
    }
    return nError;
}
//---------------------------------------------------------------------------
void Tfrm_Bay_Test::InitData(void)
{
	bTestFileOK = false;
	nCount = 0;
	nCnt = 0;
	PortType = 0x00;
	Step = BAYTEST_STEP::INIT_STEP;
	Longrun_per->Position = 0;
	for(int i = 0; i<MAX_DATA; i++)
	{
        nStationID[i] = 0;
		nTestCnt[i] = 0;
		nUseTrans[i] = 0;
        bTestStaus[i] = false;
        strGridLongrunInfor->Cells[0][i+1] = "";
		strGridLongrunInfor->Cells[1][i+1] = "";
		strGridLongrunInfor->Cells[2][i+1] = "";
        strGridLongrunInfor->Cells[3][i+1] = "";
		strGridLongrunInfor->Cells[4][i+1] = "";
		strGridLongrunInfor->Cells[5][i+1] = "";
    }
}
//---------------------------------------------------------------------------
void Tfrm_Bay_Test::InitTestResult(bool bAllInit)
{
	nCnt = 0;
	Step = BAYTEST_STEP::INIT_STEP;
	for(int i = 0; i<nCount; i++)
	{
		if(bAllInit == true)
		{
			nTestCnt[i] = 0;
			bTestStaus[i] = false;
			strGridLongrunInfor->Cells[5][i+1] = "";
		}
		strGridLongrunInfor->Cells[4][i+1] = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::TestModeBoxClick(TObject *Sender)
{
	//
	switch(TestModeBox->ItemIndex)
	{
		case 0 :
			bModeType = BAYTEST_MODE::ONCE_MODE;
			break;

		case 1 :
			bModeType = BAYTEST_MODE::REPEAT_MODE;
			break;

		default :
			bModeType = BAYTEST_MODE::NONE_MODE;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::ExcuteModeBoxClick(TObject *Sender)
{
	switch(ExcuteModeBox->ItemIndex)
	{
		case 0 :
			bExecuteType = BAYTEST_MODE::SEQUENTIAL_MODE;
			break;

		case 1 :
			bExecuteType = BAYTEST_MODE::RANDOM_MODE;
			break;

		default :
			bExecuteType = BAYTEST_MODE::NONE_MODE;
	}
}
//---------------------------------------------------------------------------
void Tfrm_Bay_Test::MoveCMD(int Start, int End, bool bNode)
{
	int iPacketSize;
	int iReturn;
	AnsiString strMachineID = "TP001";

	 // ���ο� TP �̵���� ��Ŷ�� �̿��Ѵ�.
	iPacketSize = sizeof(PKT_TP2OHT_MOVE_BY_MAP);
	PKT_TP2OHT_MOVE_BY_MAP	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ---- Tag
	SEND_PACKET.TAG.Value = TagID::CMD_TP_MOVE_BY_MAP; // TP���� OHT�� �̵���� (�������� �����ϰ� MAP���� ���)

	// ---- Body Set
	// ������ ��� Ÿ�� �Է� (Only Station)
	if(!bNode)
		SEND_PACKET.BODY.IsGoStation = TRUE;
	else
       SEND_PACKET.BODY.IsGoStation = false;

	// ������ ��ȣ �Է� (������� ���� ������, ���� ��������)
	SEND_PACKET.BODY.StartNodeID = Start;

	// ������ ��ȣ �Է�
	SEND_PACKET.BODY.StopNodeID = End;

	// ������ ����
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);
}
//---------------------------------------------------------------------------
void Tfrm_Bay_Test::TransCMD(int Type)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize;
	int iReturn, iTemp;
	AnsiString strMachineID = "TP001";
	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;

	// ������ ��� ����ü
	iPacketSize = sizeof(PKT_TP2OHT_TRANS_LOAD_UNLOAD);
	PKT_TP2OHT_TRANS_LOAD_UNLOAD	SEND_PACKET;

	// ����ü �ʱ�ȭ
	memset(&SEND_PACKET, 0, iPacketSize);

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, strMachineID.c_str(), strMachineID.Length());

	// ������ ��� ����
	MakeTransPkt(&SEND_PACKET);

	// ---- Tag
	// Load, Unload ����
	switch(Type)
	{
		case CMD_LOAD:
			// ������ ��� ����
			SEND_PACKET.TAG.Value = TagID::CMD_TRANS_LOAD;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			break;

		case CMD_UNLOAD:
			// ������ ��� ����
			SEND_PACKET.TAG.Value = TagID::CMD_TRANS_UNLOAD;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			break;

		// by zzang9un 2014.04.07 : Move Teaching Point ��� �߰�
		case CMD_MOVETPOINT:
			SEND_PACKET.TAG.Value = TagID::CMD_TRANS_MOVE_TEACHING_POINT;
			m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);
			break;

		default:
			break;
	}
	delete m_CMDControl;
}
//---------------------------------------------------------------------------
void Tfrm_Bay_Test::MakeTransPkt(PKT_TP2OHT_TRANS_LOAD_UNLOAD *pTrans)
{
	pTrans->BODY.CurNodeID = gm_OHTStatus->m_OHTStatus.Data.CurNodeID;
	pTrans->BODY.CurStationID = gm_OHTStatus->m_OHTStatus.Data.CurStationID;
	pTrans->BODY.PortType = PortType;
	pTrans->BODY.PIODirection = 1;
	// by zzang9un 2013.09.02 : Hoist, Shift Level�� ����Ͽ� ����
	pTrans->BODY.HoistSpeedLevel = 100;
	pTrans->BODY.ShiftSpeedLevel = 100;
	// by zzang9un 2013.08.26 : EQPIO Disable ���θ� �߰�
	pTrans->BODY.TransOption.Info.AutoRecovery = false;
	pTrans->BODY.TransOption.Info.HandDetectEQ = false;
	pTrans->BODY.TransOption.Info.EQPIODisable_TP = false;
}
void Tfrm_Bay_Test::Auto_Teaching_CMD(void)
{

	int iPacketSize = sizeof(PKT_OHT2TP_TEACHING_INFO);
	int iReturn;

	PKT_OHT2TP_TEACHING_INFO 	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH);

	SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING;
	SEND_PACKET.BODY.PortType = PortType;

	TCMDCONTROL *m_CMDControl = new TCMDCONTROL;
	m_CMDControl->MakeTP_RAW_SEND_Cmd2OHT((char*)&SEND_PACKET, iPacketSize);

	delete m_CMDControl;

}
void Tfrm_Bay_Test::StatusDisplay(void)
{
	static int cnt = 0;
	cnt++;
	if(cnt == 2)
	{
		Shape_Status->Brush->Color = COLOR_WHITE;
	}
	else if(cnt == 4)
	{
		Shape_Status->Brush->Color = COLOR_LIME;
		cnt = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::Btn_OpenLongrunFileClick(TObject *Sender)
{
	String strFileName;
	String strData;
	int nError = NO_ERR;
	int x, y;

	InitData();
	mmLongRunResult->Lines->Clear();

	if((bExecuteType == BAYTEST_MODE::NONE_MODE) || (bModeType == BAYTEST_MODE::NONE_MODE))
	{
		mmLongRunResult->Lines->Add("[ERR] �׽�Ʈ ���� ���� ��带 �������ּ���");
        return;
    }

	if(OpenTextFileDig_Longrun->Execute())
	{
		strFileName = OpenTextFileDig_Longrun->FileName;

        // File Check
		nError = LongRunFileCheck(strFileName);
		if(nError == NO_ERR)
		{
            if(nCount != 0)
            {
				if(nCount > MAX_DATA)
                {
					mmLongRunResult->Lines->Add("[ERR] Test Case �ִ� ������ �ʰ� �߽��ϴ�. �ִ�" + IntToStr(MAX_DATA) + "ȸ���� ���� �����մϴ�.");
                    InitData();
                    return;
                }
                else
                {
                    // �������� ������ ����
                    if(bExecuteType == BAYTEST_MODE::RANDOM_MODE)
                    {
                        for(int i = 0; i<nCount *2; i++)
                        {
                            x = rand()%nCount;
                            y = rand()%nCount;

                            int nStationID_tmp = nStationID[x];
							int nUseTrans_tmp = nUseTrans[x];
							bool bUseAutoTeaching_tmp = bUseAutoTeaching[x];

							nStationID[x] = nStationID[y];
                            nStationID[y] = nStationID_tmp;

							nUseTrans[x] = nUseTrans[y];
							nUseTrans[y] = nUseTrans_tmp;

							bUseAutoTeaching[x] = bUseAutoTeaching[y];
							bUseAutoTeaching[y] = bUseAutoTeaching_tmp;    //����ƼĪ random ����

						}
                    }
                    //
					strGridLongrunInfor->RowCount = nCount+1;
                    for(int i = 0; i<nCount; i++)
                    {
                        strGridLongrunInfor->Cells[0][i+1] = IntToStr(i+1);
                        strGridLongrunInfor->Cells[1][i+1] = IntToStr(nStationID[i]);

						if(nUseTrans[i] == 1)
						{
							strGridLongrunInfor->Cells[2][i+1] = "True";
						}
						else
						{
							strGridLongrunInfor->Cells[2][i+1] = "False";
						}

						if(bUseAutoTeaching[i] == true)
						{
							strGridLongrunInfor->Cells[3][i+1] = "True";
						}
						else
						{
							strGridLongrunInfor->Cells[3][i+1] = "False";
						}

                    }
					mmLongRunResult->Lines->Add("[INFO] Total Test Case : " + IntToStr(nCount) + "Cycle");
					mmLongRunResult->Lines->Add("[INFO] File Open!");
                    bTestFileOK = true;
                }
            }
            else
            {
				mmLongRunResult->Lines->Add("[ERR] Long Run File is Null");
            }
		}
        else
		{
			mmLongRunResult->Lines->Add("[ERR] File Open Fail!");
		}
    }
}
//---------------------------------------------------------------------------
int Tfrm_Bay_Test::LongRunFileCheck(String strTmpFileName)
{
    FILE* fp;
    TCHAR chBuffer[256];
    String strTmpBuffer;
    int Pos=0;

	if(strTmpFileName.Length() <= 0) return ERR_FILE_NAME;
	fp = _wfopen(strTmpFileName.w_str(), L"r");

	if (fp == NULL)
	{
		fclose(fp);
        return ERR_FILE_OPEN;
    }

    while (feof(fp) == false)
    {
        swprintf(chBuffer, L"");
        try
        {
            fgetws(chBuffer, 255, fp);
        }
        catch (Exception &e)
        {
            fclose(fp);
            return ERR_FILE_OPEN;;
		}
        strTmpBuffer = chBuffer;
        strTmpBuffer.TrimRight();
        while ((Pos = strTmpBuffer.Pos(L"\n")) > 0)
            strTmpBuffer.Delete(Pos, 1);

        if (strTmpBuffer.Length() == 0)  continue;

        Pos=strTmpBuffer.Pos("/");

        if(Pos>0)
        {
            String tmpLeftStr  = strTmpBuffer.SubString(1,Pos-1);
			String tmpRightStr = strTmpBuffer.SubString(Pos+1, strTmpBuffer.Length());

			nCount++;
			if(nCount <= MAX_DATA)
            {
				nStationID[nCount-1] = StrToInt(tmpLeftStr);

				if(StrToInt(tmpRightStr) == 1)
                {
					nUseTrans[nCount-1] = 1;
					bUseAutoTeaching[nCount-1] = false;
				}
				else if(StrToInt(tmpRightStr) == 2)
				{
					 nUseTrans[nCount-1] = 0;
					 bUseAutoTeaching[nCount-1] = true;
				}
				else if(StrToInt(tmpRightStr) == 3)
				{
					nUseTrans[nCount-1] = 2;
					 bUseAutoTeaching[nCount-1] = false;

				}
                else
				{
					nUseTrans[nCount-1] = 0;
					bUseAutoTeaching[nCount-1] = false;
                }
			}
        }
    }
    fclose(fp);
    return 0;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::btn_LongRunStartClick(TObject *Sender)
{
	InitTestResult(true);

	if(bTestFileOK)
	{
		if((bExecuteType != BAYTEST_MODE::NONE_MODE) && (bModeType != BAYTEST_MODE::NONE_MODE))
		{
			mmLongRunResult->Lines->Add("[INFO] �׽�Ʈ ����");
			BayLongRunTimer->Enabled = true;
		}
		else
		{
			mmLongRunResult->Lines->Add("[ERR] �׽�Ʈ ��� ���� �ʿ�");
		}
	}
	return;
}


//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::btn_LongRunStopClick(TObject *Sender)
{
	BayLongRunTimer->Enabled = false;
	mmLongRunResult->Lines->Add("[INFO] �׽�Ʈ ����");
	Shape_Status->Brush->Color = COLOR_RED;
	Label_TestStatus->Caption = "*�׽�Ʈ ����*";
	return;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_Bay_Test::FormClose(TObject *Sender, TCloseAction &Action)
{
	InitData();
	BayLongRunTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall Tfrm_Bay_Test::FormDestroy(TObject *Sender)
{
	InitData();
	BayLongRunTimer->Enabled = false;
}
//---------------------------------------------------------------------------

/*
			 ���� �̿ϼ�
void __fastcall Tfrm_Bay_Test::btn_OCSBase_LongrunStartClick(TObject *Sender)
{
	InitTestResult(true);

	if(bTestFileOK)
	{
		if((bExecuteType != BAYTEST_MODE::NONE_MODE) && (bModeType != BAYTEST_MODE::NONE_MODE))
		{
			mmLongRunResult->Lines->Add("[INFO] OCS MAP ��� �׽�Ʈ ����");
			BayLongRunTimer->Enabled = true;
		}
		else
		{
			mmLongRunResult->Lines->Add("[ERR] �׽�Ʈ ��� ���� �ʿ�");
		}
	}
	return;
}
//---------------------------------------------------------------------------
		*/
