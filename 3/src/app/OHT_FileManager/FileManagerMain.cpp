//---------------------------------------------------------------------------

#include <vcl.h>
#include <dir.h>
#pragma hdrstop

#include "FileManagerMain.h"
#include "Def_DataFile_STD.h"
#include <mmsystem.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileManagerForm *FileManagerForm;
FtpClient *fFtpClient;
FMThreadUnit *ptrFMThread = NULL;
MCCThreadUnit *ptrMCCThread = NULL;

#define ADD_LOG(STR)        m_Log->WriteLogFileAtD("FM", STR, "D:")
//---------------------------------------------------------------------------

void __fastcall TFileManagerForm::Exit1Click(TObject *Sender)
{
//	AboutBox->Close();

    Close();
}
//---------------------------------------------------------------------------
void __fastcall TFileManagerForm::FormCreate(TObject *Sender)
{
	//Ftp ������ ����
	ptrFMThread = new FMThreadUnit(true);

	ptrMCCThread = new MCCThreadUnit(true);

	// Application->Minimize();
}
//---------------------------------------------------------------------------
__fastcall TFileManagerForm::TFileManagerForm(TComponent* Owner) : TForm(Owner)
{
	fFtpClient = new FtpClient();
	TrayIconFileManager->Visible = true;

	// 1) ���Ϲ��� ��������
	AnsiString strFmVer;

	strFmVer.sprintf("%u.%u.%u.%u",
		(BYTE)VerInfo.Major,
		(BYTE)VerInfo.Minor,
		(BYTE)VerInfo.Release,
		(BYTE)VerInfo.Build);

	// 2) Save
	AnsiString strFileName = FM_VER_FILE_PATH;

	FILE *stream;

	try
	{
		stream = fopen(strFileName.c_str(), "w+");
		if(stream == NULL)
			return;

		fprintf(stream, "%s", strFmVer);

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
int m_Tick =0;
void __fastcall TFileManagerForm::MainTimerTimer(TObject *Sender)
{
	//status count
	if(m_Tick == 8)
		m_Tick = 0;
	else
		m_Tick++;

	return;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

__fastcall FMThreadUnit::FMThreadUnit(bool CreateSuspended) : TThread(CreateSuspended)
{
	//Thread Priority Level : tpIdle, tpLowest, tpLower, tpNormal, tpHigher, tpHighest, tpTimeCritical
	m_Log = new OHTLogFile;
   Priority = tpNormal;
   Resume();
   dwSTTime=timeGetTime();
}
//---------------------------------------------------------------------------
void __fastcall FMThreadUnit::Execute()
{
	DWORD time_array[1000];
	DWORD dwSTTime_previous;
	int i=0;
	AnsiString strLogTmp;

	// AnsiString strRealDownloadFile;

	Sleep(5000);//wait socket bind
	dwSTTime=timeGetTime();
	dwSTTime_previous=dwSTTime;
	//---- Place thread code here ----
	while( !Terminated )
	{
		dwSTTime=timeGetTime();
		dwSTTime_previous=dwSTTime;

		//ȣ�� �ð� Ȯ�ο�
		#if 0
		time_array[i]=dwSTTime-dwSTTime_previous;
		if(i<999)
		{
			i++;
		}
		#endif
		
		//---------------------------------------------------------------------------
		PKT_OCS2OHT SEND_PKT;
		int iReturn;

		iReturn = frmUDP->pUdpFM->AS_2_FM_ThreadCall(SEND_PKT);

		AnsiString strFileName, strPathName, strTmp;
		AnsiString strLog;
		
		bool bIsPathOnly = false;

		DWORD	dwStartTime;
		DWORD	dwEndTime;

		if(iReturn > 0)
		{
			//���� ��Ŷ�� ���� ���
			if(SEND_PKT.TAG.Value == TagID::CMD_DATATRANSFER_OCS2OHT)
			{
				if(uiFTPMode == FTP_MODE_ON)
				{
					// ������ ���� ���� ���� �Ұ� ó��
					// 0x07 : �̹� �ٸ� �������ø� ���� ��(FTP ��� ���� ���� �� ������ �ȵ� ���)

					FileManagerForm->Memo1->Lines->Add("0x07 : �̹� �ٸ� �������ø� ���� ��(FTP ��� ���� ���� �� ������ �ȵ� ���)");
					ADD_LOG(L"0x07 : �̹� �ٸ� �������ø� ���� ��(FTP ��� ���� ���� �� ������ �ȵ� ���)");
					PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
					int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);


					memset(&CmdPkt, 0, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
					memcpy(&CmdPkt, &SEND_PKT, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

					// ��ý��Ͻ��� TagID�� �ٵ� ä�� �ִ´�
					CmdPkt.HEADER.MachineType = MACHINETYPE_FM;
					CmdPkt.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT2OCS;

					CmdPkt.BODY.Ack.Accept_Error = FTP_ERROR_ON;
					CmdPkt.BODY.Ack.Accept_Error_Class = FTP_NO_ACCEPT_TRAN_ORDER;

					frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&CmdPkt, iPacketSize);
				}
				else
				{
					memset(&g_UDP_PKT_Order, 0, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
					memcpy(&g_UDP_PKT_Order, &SEND_PKT, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

					strLog.sprintf("Dir(0:FTP->OHT,1:OHT->FTP):%d CMDType(0x01=map org, 0x02=map op 0x10=Firmware Update) : 0x%02x",
									g_UDP_PKT_Order.BODY.Request.TestMode, g_UDP_PKT_Order.BODY.Request.CMDType);
					FileManagerForm->Memo1->Lines->Add(strLog);

					ADD_LOG(strLog);
					switch(g_UDP_PKT_Order.BODY.Request.CMDType)
					{
						case FTP_CMDTYPE_MAPSET_TEACHING:
						case FTP_CMDTYPE_MAPSET_OPERATING:
						case FTP_CMDTYPE_MAPSET_OPERATING+FTP_CMDTYPE_MAPSET_TEACHING:
						//case FTP_CMDTYPE_LOGDATA:
						//case FTP_CMDTYPE_MAPSET:
							uiFTPMode = FTP_MODE_ON;
							uiFTPStep = FTP_STEP_START;
							uiRetryCount = 0;
							uiTranOrderResponseStatus=0;
							break;
						case FTP_CMDTYPE_APPLICATION:
						case FTP_CMDTYPE_OS:
						case FTP_CMDTYPE_OS_SETTING:
						case FTP_CMDTYPE_OPERATING_PARA:
						case FTP_CMDTYPE_AXIS_PARA:

							FileManagerForm->Memo1->Lines->Add("Firmware Update Run...");
								
							if( g_UDP_PKT_Order.BODY.Request.TestMode == FTP_TESTMODE_FTP2OHT)
							{
								FileManagerForm->Memo1->Lines->Add("FTP_MODE_ON");
									
								uiFTPMode = FTP_MODE_ON;
								uiFTPStep = FTP_STEP_START;
								uiRetryCount = 0;
								uiTranOrderResponseStatus=0;
							}
							else
							{
								// ������ ���� ���� ���� �Ұ� ó��
								// 0x07 : �̹� �ٸ� �������ø� ���� ��(FTP ��� ���� ���� �� ������ �ȵ� ���)
								PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
								int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);

								FileManagerForm->Memo1->Lines->Add("������ ���� ���� ���� �Ұ� ó�� FTP_TESTMODE_OHT2FTP");
									
								memset(&CmdPkt, 0, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
								memcpy(&CmdPkt, &SEND_PKT, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

								// ��ý���Ʈ�� TagID�� �ٵ� ä�� �ִ´�
								CmdPkt.HEADER.MachineType = MACHINETYPE_FM;
								CmdPkt.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT2OCS;

								CmdPkt.BODY.Ack.Accept_Error = FTP_ERROR_ON;
								CmdPkt.BODY.Ack.Accept_Error_Class = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;

								frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&CmdPkt, iPacketSize);
							}
							break;
						default :
						// ������ ���� ���� ���� �Ұ� ó��
						// 0x07 : �̹� �ٸ� �������ø� ���� ��(FTP ��� ���� ���� �� ������ �ȵ� ���)
						FileManagerForm->Memo1->Lines->Add("������ ���� ���� ���� �Ұ� ó�� - default");
						
						PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE CmdPkt;
						int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);


						memset(&CmdPkt, 0, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));
						memcpy(&CmdPkt, &SEND_PKT, sizeof(PKT_DATA_TRANSPER_ORDER_OCS2OHT));

						// ��ý��Ͻ��� TagID�� �ٵ� ä�� �ִ´�
						CmdPkt.HEADER.MachineType = MACHINETYPE_FM;
						CmdPkt.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT2OCS;

						CmdPkt.BODY.Ack.Accept_Error = FTP_ERROR_ON;
						CmdPkt.BODY.Ack.Accept_Error_Class = FTP_NOT_ACCEPT_NO_SUPPORT_DATA;

						frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&CmdPkt, iPacketSize);
					}
				}
			}
			else if(SEND_PKT.TAG.Value == TagID::CLW_TO_MCP_STATUS)
			{
				PKT_STATUS_REPORT_ID_03 CmdPkt;
				int iPacketSize = sizeof(PKT_STATUS_REPORT_ID_03);

				memset(&CmdPkt, 0, sizeof(PKT_STATUS_REPORT_ID_03));
				memcpy(&CmdPkt, &SEND_PKT, sizeof(PKT_STATUS_REPORT_ID_03));
				CmdPkt.TAG.Value = TagID::CLW_TO_MCP_STATUS;
				CmdPkt.HEADER.MachineType = MACHINETYPE_FM;
				// ����� �������ͽ��� OHT�� ä�� �ִ´�. BODY�� ���� ����
				frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&CmdPkt, iPacketSize);
			}
			else if(SEND_PKT.TAG.Value == TagID::CMD_FIRMWARE_UPDATE_ORDER_WIRELESS_OHT2AS)
			{
				// -----------------------------------------
				// 		ftp mode go
				// -----------------------------------------
				memset(&g_UDP_PKT_Wireless_Update, 0, sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS));
				memcpy(&g_UDP_PKT_Wireless_Update, &SEND_PKT, sizeof(PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS));				

				FileManagerForm->Memo1->Lines->Add("FTP_WIRELESS_UPDATE");
					
				uiFTPMode = FTP_WIRELESS_UPDATE;
				uiFTPStep = FTP_WIRELESS_START;
				uiRetryCount = 0;
				// uiTranOrderResponseStatus=0;
			}
			else
			{
				//NOP
			}
		}
		else
		{
			//��Ŷ�� �ȿ°�� FTP ����
			if(uiFTPMode == FTP_MODE_ON)
			{
				int iResult = 0;
				switch(uiFTPStep)
				{
					case FTP_STEP_START:
					{
						AnsiString strConLog;

						//FTP ���� : FTP ����
						strConLog.sprintf("[ftp �������ó���] IP:%d.%d.%d.%d  User : %s  Pass : %s Send To : %s ",
							g_UDP_PKT_Order.BODY.IpAddress[0],  g_UDP_PKT_Order.BODY.IpAddress[1],
							g_UDP_PKT_Order.BODY.IpAddress[2],  g_UDP_PKT_Order.BODY.IpAddress[3],
							g_UDP_PKT_Order.BODY.UserName,  g_UDP_PKT_Order.BODY.PassWord,
							g_UDP_PKT_Order.BODY.DataSendTo);
						FileManagerForm->Memo1->Lines->Add(strConLog);

						iResult = fFtpClient->FTPConnect(g_UDP_PKT_Order);

						FileManagerForm->Memo1->Lines->Add("FTP_STEP_START"+iResult);

						ADD_LOG(L"FTP_STEP_START");
						dwStartTime = timeGetTime();
						
						if(iResult == FTP_NOT_ACCEPT_NO_ERR)
						{
							//���� ����
							uiFTPStep = FTP_STEP_FTP_PATH_CHECK;
							uiFTPTransSatus = FTP_STATUS_NOP_DONE;
						}
						else
						{
							int g_RetryCycleMAX = 3;// ���� �Ķ���� ���Ͽ��� �о�;� ��.
							if(uiRetryCount == 0)
							{
								//���� ����
								uiFTPMode = FTP_MODE_ON;
								uiFTPStep = FTP_STEP_START;
								//������ ���� ���� ����
								DataTransperResponse(g_UDP_PKT_Order, 0); //iResult : expected result 0, 2~5
								uiTranOrderResponseStatus = 1;
								uiRetryCount += 1;
							}
							else if(uiRetryCount < g_RetryCycleMAX)//Retry cycle
							{
								//���� ����
								uiFTPMode = FTP_MODE_ON;
								uiFTPStep = FTP_STEP_START;
								//������ ���� ��ô ����
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0x02);//������ ������
								uiRetryCount += 1;
							}
							else if(uiRetryCount == g_RetryCycleMAX)
							{
								//���� ����
								uiFTPMode = FTP_MODE_OFF;
								uiFTPStep = FTP_STEP_DEFAULT;
								//������ ���� ��ô ����
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0x01);//������ ������
							}
							else
							{
								//���� ����
								uiFTPStep = FTP_STEP_FTP_PATH_CHECK;
								uiFTPTransSatus = FTP_STATUS_NOP_DONE;
							}
						}
					}
					break;
					
					case FTP_STEP_FTP_PATH_CHECK:	
					{
						FileManagerForm->Memo1->Lines->Add("�ٿ�/�� ��� ����");

						strFileName = g_UDP_PKT_Order.BODY.DataSendTo;
						strLogTmp.sprintf("Original Str : %s Length : %d", strFileName, strFileName.Length());
						FileManagerForm->Memo1->Lines->Add(strLogTmp);

						strFileName = strFileName.Trim();
						strLogTmp.sprintf("[Trim��] Str : %s Length : %d", strFileName, strFileName.Length());
						FileManagerForm->Memo1->Lines->Add(strLogTmp);

						ADD_LOG(strLogTmp);
						// ������ 1�� ���ڸ� ���´�
						strTmp = strFileName.SubString(strFileName.Length(), 1);
						FileManagerForm->Memo1->Lines->Add(strTmp);

						if(strTmp == "/")
						{
							bIsPathOnly = true;
							FileManagerForm->Memo1->Lines->Add("bIsPathOnly = true (/�� ������ ���)");
						}
						else
						{
							bIsPathOnly = false;
							FileManagerForm->Memo1->Lines->Add("bIsPathOnly = false (/�� ������ ����)");

							// ������ ��� �ڵ� ����
							strFileName = strFileName + "/";
							memcpy(&g_UDP_PKT_Order.BODY.DataSendTo, strFileName.c_str(), strFileName.Length());

							strLogTmp.sprintf("(��� �� �ڵ�����) ������ : %s", strFileName);
							FileManagerForm->Memo1->Lines->Add(strLogTmp);
						}

						// ----------------------------------------------
						/*
						strLogTmp = "";

						for(int i = 0; i < 255; i++)
						{
							strTmp.sprintf("%c (0x02%x)  ", g_UDP_PKT_Order.BODY.DataSendTo[i], g_UDP_PKT_Order.BODY.DataSendTo[i]);
							strLogTmp = strLogTmp + strTmp;
						}
						FileManagerForm->Memo1->Lines->Add(strLogTmp); 						*/

						// -----------------------------------------------------
						if(g_UDP_PKT_Order.BODY.DataSendTo[0] == '/')
						{
							FileManagerForm->Memo1->Lines->Add("��� /�� ������");
						}
						else
						{
							g_UDP_PKT_Order.BODY.DataSendTo[0] = '/';
							memcpy(&g_UDP_PKT_Order.BODY.DataSendTo[1], strFileName.c_str(), strFileName.Length());
							strLogTmp = g_UDP_PKT_Order.BODY.DataSendTo;
							strLogTmp.sprintf("�ڵ� ������ ��� : %s", strLogTmp);
							FileManagerForm->Memo1->Lines->Add(strLogTmp);
						}

						//FTP ���� : FTP ���丮 ���� Ȯ��
						iResult = fFtpClient->FTPPathCheck(g_UDP_PKT_Order.BODY.DataSendTo);
						if(iResult == FTP_NOT_ACCEPT_NO_ERR)
						{
							if(g_UDP_PKT_Order.BODY.Request.TestMode == FTP_TESTMODE_FTP2OHT)
							{
								//�ٿ�ε� ��� Ȯ�� ����
								//�ٿ�ε� ���� Ȯ��
								FileManagerForm->Memo1->Lines->Add("FTP_STEP_FTP_PATH_CHECK: Download"+iResult);
                                ADD_LOG(L"FTP_STEP_FTP_PATH_CHECK: Download");
								uiFTPStep = FTP_STEP_FTP_FILE_CHECK;
								uiFTPTransSatus = FTP_STATUS_NOP_DONE;

							}
							else
							{
								//���ε� ��� Ȯ�� ����
								//FTP_STEP_COMMUNICATION ���� ��ȯ
								uiFTPStep = FTP_STEP_COMMUNICATION;
								uiFTPTransSatus = FTP_STATUS_NOP_DONE;
								FileManagerForm->Memo1->Lines->Add("FTP_STEP_FTP_PATH_CHECK: Upload"+iResult);
                                ADD_LOG(L"FTP_STEP_FTP_PATH_CHECK: Upload");
								if(uiTranOrderResponseStatus == 0)
								{
									//������ ���� ���� ����
									DataTransperResponse(g_UDP_PKT_Order, iResult); //iResult : expected result 0, 2~5
									uiTranOrderResponseStatus = 1;
								}
							}
						}
						else
						{
							//��� Ȯ�� ����
							uiFTPMode = FTP_MODE_OFF;
							uiFTPStep = FTP_STEP_DEFAULT;
							FileManagerForm->Memo1->Lines->Add("FTP_STEP_FTP_PATH_CHECK : Error"+iResult);
							ADD_LOG(L"FTP_STEP_FTP_PATH_CHECK : Error"+iResult);
							if(uiTranOrderResponseStatus == 0)
							{
								//������ ���� ���� ����
								DataTransperResponse(g_UDP_PKT_Order, iResult); //iResult : expected result 0, 2~5
								uiTranOrderResponseStatus = 1;
							}
							else
							{
								//������ ���� ��ô ����
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0x01);//������ ���� ����
							}
							fFtpClient->FTPDisConnect();
						}
					}
					break;
					case FTP_STEP_FTP_FILE_CHECK:	// shkim. ������� �߿��� ������Ʈ ���� �����ʿ� -> FTPFileCheck() 
					{
						 //FTP ���� : FTP �ٿ�ε� ���� ���� Ȯ��
						iResult = fFtpClient->FTPFileCheck(g_UDP_PKT_Order.BODY.DataSendTo, g_UDP_PKT_Order.BODY.Request.CMDType);
						if(iResult == FTP_NOT_ACCEPT_NO_ERR)
						{
							//�ٿ�ε� ���� ����.
							uiFTPStep = FTP_STEP_COMMUNICATION;
							uiFTPTransSatus = FTP_STATUS_NOP_DONE;
							FileManagerForm->Memo1->Lines->Add("�ٿ�ε� ���� ����");
							ADD_LOG(L"�ٿ�ε� ���� ����");
						}
						else
						{
							//�ٿ�ε� ���� ����.
							uiFTPMode = FTP_MODE_OFF;
							uiFTPStep = FTP_STEP_DEFAULT;
							fFtpClient->FTPDisConnect();
							FileManagerForm->Memo1->Lines->Add("�ٿ�ε� ���� ����");
							ADD_LOG(L"�ٿ�ε� ���� ����");
						}
						FileManagerForm->Memo1->Lines->Add("FTP_STEP_FTP_FILE_CHECK"+iResult);
						//������ ���� ���� ����
						DataTransperResponse(g_UDP_PKT_Order, iResult); //iResult : expected result 0, 2~5
						uiTranOrderResponseStatus = 1;//1?
					}
					break;
					case FTP_STEP_COMMUNICATION:
					{
						//������ ���� ��ô ���� �ܰ�
						uiFTPTransSatus = FTP_STATUS_COMMUNICATION;

						//FTP ���� : file download or Upload
						if(g_UDP_PKT_Order.BODY.Request.TestMode == FTP_TESTMODE_FTP2OHT)
						{
							// ���� �ٿ�ε�
							FileManagerForm->Memo1->Lines->Add("FTPDownload");
							ADD_LOG(L"FTPDownload");
							iResult = fFtpClient->FTPDownload(g_UDP_PKT_Order.BODY.DataSendTo, g_UDP_PKT_Order.BODY.Request.CMDType);
						}
						else
						{
							// ���� ���ε�
							FileManagerForm->Memo1->Lines->Add("fileUpload");
							ADD_LOG(L"fileUpload");
							iResult = fFtpClient->FTPUpload(g_UDP_PKT_Order.BODY.DataSendTo, g_UDP_PKT_Order.BODY.Request.CMDType);
						}

						//������ ���� ��ô ����
						if(iResult == NO_ERR )
						{
							FileManagerForm->Memo1->Lines->Add("FTP_STEP_COMMUNICATION iResult ok 0");
                            ADD_LOG(L"FTP_STEP_COMMUNICATION iResult ok 0");
							uiFTPStep = FTP_STEP_FILE_CHECK;
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);
						}
						else
						{
							FileManagerForm->Memo1->Lines->Add("FTP_STEP_COMMUNICATION iResult error"+iResult);
                            ADD_LOG(L"FTP_STEP_COMMUNICATION iResult error"+iResult);
							uiFTPStep = FTP_STEP_DEFAULT;
							fFtpClient->FTPDisConnect();
							//������ ���� ��ô ����
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
						}

					}
					break;
					case FTP_STEP_FILE_CHECK:	// shkim. �߿��� ������Ʈ ���� ����
					{
						//������ ���� ��ô ���� �ܰ�
						uiFTPTransSatus = FTP_STATUS_FILE_CHECK;

						//FTP ���� : ���� ���� �� file check
						//�ٿ�ε��� ���
						//���ǵ� ���ϸ��� 8���� ������ ���� �ϴ°�
						//���ϵ��� ũ�Ⱑ ����� ��õ� ������ ���� ��ġ �ϴ°�
						if(g_UDP_PKT_Order.BODY.Request.TestMode == FTP_TESTMODE_FTP2OHT)
						{
							FileManagerForm->Memo1->Lines->Add("FTP_STEP_FILE_CHECK 0");
							ADD_LOG(L"FTP_STEP_FILE_CHECK 0");
							iResult = FTP_NOT_ACCEPT_NO_ERR;

							//�ʼ�Ʈ ���� ������ ��� ���� 8�� ���� ���θ� �˻�
							//if((g_UDP_PKT_Order.BODY.Request.CMDType == FTP_CMDTYPE_MAPSET_TEACHING) || (g_UDP_PKT_Order.BODY.Request.CMDType == FTP_CMDTYPE_MAPSET_OPERATING))
							if(((g_UDP_PKT_Order.BODY.Request.CMDType & FTP_CMDTYPE_MAPSET_TEACHING) == FTP_CMDTYPE_MAPSET_TEACHING) || ((g_UDP_PKT_Order.BODY.Request.CMDType & FTP_CMDTYPE_MAPSET_OPERATING) == FTP_CMDTYPE_MAPSET_OPERATING))
							{
								//���� ���� �� ������ 8�� ������ ����
								// 2017.01.05 -> file 2 add. total 10
								fFtpClient->EraseMapsetFiles();
								fFtpClient->Decompression();

								//���� ���� �� ������ 8���� ������ ����, 8���� �ȵǸ� ������
								int iRet = fFtpClient->FindMapsetFiles();
								if(iRet == TOTAL_MAPSET_FILES)
								{
									//������ ����� ũ�� Ȯ��
									int iRet = fFtpClient->CheckFileSizeMapset();
									if( iRet == 0 )
									{
										iResult = FTP_NOT_ACCEPT_NO_ERR;
										FileManagerForm->Memo1->Lines->Add("������ ��� �ֽ��ϴ� üũ OK");
										ADD_LOG(L"������ ��� �ֽ��ϴ� üũ OK");
									}
									else
									{
										iResult = 1; // ���� ũ�Ⱑ �ٸ��� ����.
										FileManagerForm->Memo1->Lines->Add("���� ũ�Ⱑ �ٸ��� ����");
										ADD_LOG(L"���� ũ�Ⱑ �ٸ��� ����");
									}
								}
								else
								{
									iResult = 1;//���� ���� �� ������ 7���� �ȵ�.
									FileManagerForm->Memo1->Lines->Add("���� ���� �� ������ 7���� �ȵ�");
									ADD_LOG(L"���� ���� �� ������ 7���� �ȵ�");
								}

								if(iResult == FTP_NOT_ACCEPT_NO_ERR)
								{
									uiFTPStep = FTP_STEP_FILE_BACKUP;
									DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);
								}
								else
								{
									uiFTPStep = FTP_STEP_DEFAULT;
									fFtpClient->FTPDisConnect();
									//������ ���� ��ô ����
									DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
								}

							}
							// shkim. �߿��� ������Ʈ ���� �߰���
							// else if((g_UDP_PKT_Order.BODY.Request.CMDType&0x40) == FTP_CMDTYPE_AXIS_PARA)
							else if((g_UDP_PKT_Order.BODY.Request.CMDType & FTP_CMDTYPE_APPLICATION) == FTP_CMDTYPE_APPLICATION)
							{

								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);	//������ üũ ó�� ��
							
								fFtpClient->FirmwareUpdateDecompress();

								if(iResult == FTP_NOT_ACCEPT_NO_ERR)
								{
									uiFTPStep = FTP_STEP_END;

									uiFTPTransSatus = FTP_STATUS_FILE_BACKUP;	//���� ���� ó�� ��
									DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);  // �������� only complete // �Ϸ�
									FileManagerForm->Memo1->Lines->Add("Firmware Update File : Received OK");
									// Sleep(500);
									
									// 0x00 complete message
									uiFTPTransSatus = FTP_STATUS_NOP_DONE;
									FileManagerForm->Memo1->Lines->Add("[Firmware Update] Complete Message (0x00) send");
									DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0);
									
								}
								else
								{
									uiFTPStep = FTP_STEP_DEFAULT;
									fFtpClient->FTPDisConnect();
									//������ ���� ��ô ����
									DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
									strLogTmp.sprintf("Firmware Update File : Receive Check - %d", iResult);
									FileManagerForm->Memo1->Lines->Add(strLogTmp);
								}
								

								dwEndTime = timeGetTime();
								strLogTmp.sprintf("���� �ٿ�ε� �ҿ�ð� (ms) : %d", (dwEndTime - dwStartTime));
								FileManagerForm->Memo1->Lines->Add(strLogTmp);
								ADD_LOG(strLogTmp);
								// shkim. ���� ������ ������ �����Ѵٸ� ������ �ϴ���, ����� �������� Ȯ���� �ڵ��ʿ�
							}
					/*
							if(iResult == FTP_NOT_ACCEPT_NO_ERR)
							{
								uiFTPStep = FTP_STEP_FILE_BACKUP;
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);
							}
							else
							{
								uiFTPStep = FTP_STEP_DEFAULT;
								fFtpClient->FTPDisConnect();
								//������ ���� ��ô ����
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
							}
					*/
						}
						else
						{
							// ���ε��� ���
							// ���ε� ���� Ȯ�� ���� �ʿ� ����. ��� �ƴ�.
							FileManagerForm->Memo1->Lines->Add("FTP_STEP_FILE_CHECK 1");
							ADD_LOG(L"FTP_STEP_FILE_CHECK 1");
							iResult = 0;
							if(iResult == 0)
							{
								uiFTPStep = FTP_STEP_END;
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);
							}
							else
							{
								uiFTPStep = FTP_STEP_DEFAULT;
								fFtpClient->FTPDisConnect();
								DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
							}
						}
					}
					break;
					case FTP_STEP_FILE_BACKUP:
					//���� ���� ó�� MAPSET->Backup ���� �� �˻� , Down -> MAPSET ���� �� �˻�
					//case FTP_STEP_MAP_DATA_LOAD:
					{
						//������ ���� ��ô ���� �ܰ�
						uiFTPTransSatus = FTP_STATUS_FILE_BACKUP;
						FileManagerForm->Memo1->Lines->Add("FTP_STEP_FILE_BACKUP 0");
						ADD_LOG(L"FTP_STEP_FILE_BACKUP 0");
						//FTP ���� : file backup
						//iResult = 0; // FTP ���� ���
						iResult = fFtpClient->CopyMapsetFiles();
						if(iResult == 0)
						{
							uiFTPStep = FTP_STEP_END;
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 2);
						}
						else
						{
							uiFTPStep = FTP_STEP_DEFAULT;
							fFtpClient->FTPDisConnect();
							//������ ���� ��ô ����
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
						}
					}
					break;
					case FTP_STEP_END:
					{
						FileManagerForm->Memo1->Lines->Add("FTP_STEP_END");
						ADD_LOG(L"FTP_STEP_END");

						
						//������ ���� ��ô ���� �ܰ�
						uiFTPTransSatus = FTP_STATUS_NOP_DONE;

						//FTP ���� : FTP disconnect
						fFtpClient->FTPDisConnect();
						
						iResult = 0; // FTP ���� ���
						uiFTPStep = FTP_STEP_DEFAULT; //���� ���� ������ ������� �� �Է�
						// uiFTPMode = FTP_MODE_OFF;  // temp. fix shkim
						
						//������ ���� ��ô ����
						//�Ϸ� ����� OHT main���� ���� ó�� �� ����� �����Ѵ�.
						// DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, iResult);
						
						// if((g_UDP_PKT_Order.BODY.Request.CMDType&0x40) == FTP_CMDTYPE_AXIS_PARA)
						{
							Sleep(500);
							FileManagerForm->Memo1->Lines->Add("Communication Complete Message");
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0x99);
						}

						/*
						==> map. 0x00 complete
						else
						{
							FileManagerForm->Memo1->Lines->Add("[Map Update] Complete Message (0x00) send");
							DataTransperResponseProgress(g_UDP_PKT_Order, uiFTPTransSatus, 0);
						}
						*/
					}
					break;
					default:
					{
						// FTP_STEP_DEFAULT
						FileManagerForm->Memo1->Lines->Add("FTP_MODE_OFF");
						ADD_LOG(L"FTP_MODE_OFF");
						uiFTPMode = FTP_MODE_OFF;
					}
					break;
				}
			}
			else if(uiFTPMode == FTP_WIRELESS_UPDATE)
			{
				int iResult = 0;
				int iErrCount = 0;
				UnicodeString strSourceFile, strDestFile;
				
				switch(uiFTPStep)
				{
					case FTP_WIRELESS_DEFAULT:
					{
						;	// do not work
					}
					break;

					case FTP_WIRELESS_START:		// Connect
					{
						AnsiString strIpAdd, strUser, strPass;

						strIpAdd.sprintf("%d.%d.%d.%d",
								g_UDP_PKT_Wireless_Update.BODY.IpAddress[FOUR_BYTE_HIGH],
								g_UDP_PKT_Wireless_Update.BODY.IpAddress[FOUR_BYTE_M1],
								g_UDP_PKT_Wireless_Update.BODY.IpAddress[FOUR_BYTE_M2],
								g_UDP_PKT_Wireless_Update.BODY.IpAddress[FOUR_BYTE_LOW]);
						strUser = g_UDP_PKT_Wireless_Update.BODY.UserName;
						strPass = g_UDP_PKT_Wireless_Update.BODY.PassWord;

						iResult = fFtpClient->FTPConnect(strIpAdd, strUser, strPass);

						FileManagerForm->Memo1->Lines->Add("FTP_WIRELESS_START" + iResult);
						if(iResult == FTP_NOT_ACCEPT_NO_ERR)
						{
							//���� ����
							uiFTPStep = FTP_WIRELESS_FILE_CONFIG;
							FileManagerForm->Memo1->Lines->Add("Connect success");
						}
						else
						{
							int g_RetryCycleMAX = 5;// ���� �Ķ���� ���Ͽ��� �о�;� ��.
							
							if(uiRetryCount < g_RetryCycleMAX)
							{
								//���� ����
								uiFTPMode = FTP_WIRELESS_UPDATE;
								uiFTPStep = FTP_WIRELESS_START;
								uiRetryCount += 1;

								FileManagerForm->Memo1->Lines->Add("Connect fail. retry...");
							}
							else if(uiRetryCount == g_RetryCycleMAX)
							{
								//���� ����
								uiFTPMode = FTP_MODE_OFF;
								uiFTPStep = FTP_STEP_DEFAULT;
								WirelessUpdateResult(WIRELESS_UPDATE_FAIL, "FTP Connection Fail");
							}
							else
							{
								//���� ����
								FileManagerForm->Memo1->Lines->Add("FTP Connection Success");
								uiFTPStep = FTP_WIRELESS_FILE_CONFIG;								
							}
						}

					}
					break;

					case FTP_WIRELESS_FILE_CONFIG:		// files put
					{
						if(g_UDP_PKT_Wireless_Update.BODY.iWirelessConfig == TRUE)
						{
							strSourceFile = WIRELESS_PARAM_FILE_PATH;
							strSourceFile = strSourceFile + WIRELESS_CONFIG_FILE_NAME;
							strDestFile	= WIRELESS_CONFIG_FILE_NAME;
							iResult = fFtpClient->DirectUpload(strSourceFile,strDestFile);

							if(iResult != 0)	// 0 is NO_ERR
								iErrCount++;

							uiFTPStep = FTP_WIRELESS_FILE_MACFIL;
							FileManagerForm->Memo1->Lines->Add("CONFIG File Upload");
						}
						else
						{
							uiFTPStep = FTP_WIRELESS_FILE_MACFIL;
						}
					}
					break;

					case FTP_WIRELESS_FILE_MACFIL:		// files put
					{
						if(g_UDP_PKT_Wireless_Update.BODY.iWirelessMacfil == TRUE)
						{
							strSourceFile = WIRELESS_PARAM_FILE_PATH;
							strSourceFile = strSourceFile + WIRELESS_MACFIL_FILE_NAME;
							strDestFile	= WIRELESS_MACFIL_FILE_NAME;
							iResult = fFtpClient->DirectUpload(strSourceFile,strDestFile);

							if(iResult != 0)	// 0 is NO_ERR
								iErrCount++;

							uiFTPStep = FTP_WIRELESS_FILE_FIRMWARE;
							FileManagerForm->Memo1->Lines->Add("Macfil File Upload");
						}
						else
						{
							uiFTPStep = FTP_WIRELESS_FILE_FIRMWARE;
						}
					}
					break;

					case FTP_WIRELESS_FILE_FIRMWARE:	// files put
					{
						if(g_UDP_PKT_Wireless_Update.BODY.iWirelessFirmware == TRUE)
						{
							strSourceFile = WIRELESS_PARAM_FILE_PATH;
							strSourceFile = strSourceFile + WIRELESS_FIRMWARE_FILE_NAME;
							strDestFile	= WIRELESS_FIRMWARE_FILE_NAME;
							iResult = fFtpClient->DirectUpload(strSourceFile,strDestFile);

							if(iResult != 0)	// 0 is NO_ERR
								iErrCount++;

							uiFTPStep = FTP_WIRELESS_RESET_CMD;

							FileManagerForm->Memo1->Lines->Add("Firmware File Upload");
						}
						else
						{
							uiFTPStep = FTP_WIRELESS_RESET_CMD;
						}
					}
					break;
					


					case FTP_WIRELESS_RESET_CMD:		// Reset command send
					{
						iResult = fFtpClient->DirectCommand("rst");
						uiFTPStep = FTP_WIRELESS_DISCONNECT;

						FileManagerForm->Memo1->Lines->Add("Wireless Module Reset");
					}
					break;

					case FTP_WIRELESS_DISCONNECT:	// Disconnect & Ending. Step go default
					{
						fFtpClient->FTPDisConnect();
						
						uiFTPMode = FTP_MODE_OFF;
						uiFTPStep = FTP_STEP_DEFAULT;
						FileManagerForm->Memo1->Lines->Add("FTP Disconnect. Step Complete");
						WirelessUpdateResult(WIRELESS_UPDATE_SUCCESS, "WIRELESS_UPDATE_SUCCESS");
					}
					break;
				}
			}
			else
			{
				uiFTPMode = FTP_MODE_OFF;
				uiFTPStep = FTP_STEP_DEFAULT;
				uiFTPTransSatus = FTP_STATUS_NOP_DONE;
			}
		}
		//---------------------------------------------------------------------------
		WaitForSingleObject((void *)Handle, 30);
	}
}
//������ ���� ���� ����
void FMThreadUnit::DataTransperResponse(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order, UINT BodyAck)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE);
	int iReturn, iTemp;
	AnsiString strTmp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_CMD_DATA_TRANSPER_ORDER_RESPONSE	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	//Header, Tag, Status�� ����
	memcpy(&SEND_PACKET, &UDP_PKT_Order, sizeof(PKT_STATUS_REPORT_ID_03));
	SEND_PACKET.TAG.Value = TagID::RESPONSE_DATATRANSFER_OHT2OCS;

	// ---- Body
	memset(&SEND_PACKET.BODY, 0, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS));
	memcpy(&SEND_PACKET.BODY.Request, &UDP_PKT_Order.BODY.Request, sizeof(OHTREQUEST));
	SEND_PACKET.BODY.Ack.TransOrderID = UDP_PKT_Order.BODY.OrderID & 0x07;
	//SEND_PACKET.BODY.Ack.TransOrderID = 0;
	switch(BodyAck)
	{
		case 0:
			//������ ���� ���� : ���� ���� ����
			SEND_PACKET.BODY.Ack.Accept_Error = 0x00;
			SEND_PACKET.BODY.Ack.Accept_Error_Class = 0x00;
		break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			//������ ���� ���� : ���� �Ұ� ����
			SEND_PACKET.BODY.Ack.Accept_Error = 0x01;
			SEND_PACKET.BODY.Ack.Accept_Error_Class = BodyAck;
		default:
		break;
	}

	SEND_PACKET.HEADER.MachineType = MACHINETYPE_FM;
	// ������ ����
	frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&SEND_PACKET, iPacketSize);

}

//---------------------------------------------------------------------------
//������ ���� ���� ��ô����
void FMThreadUnit::DataTransperResponseProgress(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order, UINT FTPTranStaus, UINT BodyAck)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_DATA_TRANSPER_ORDER_PROGRESS);
	int iReturn, iTemp;
	AnsiString strTmp;

	// ��Ŷ�� ������ �� ���� �� �ִ´�.
	PKT_DATA_TRANSPER_ORDER_PROGRESS	SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����

	//Header, Tag, Status�� ����
	memcpy(&SEND_PACKET, &UDP_PKT_Order, sizeof(PKT_STATUS_REPORT_ID_03));
	SEND_PACKET.TAG.Value = TagID::REPORT_DATATRANSFERPROGRESS_OHT2OCS;

	// ---- Body
	memset(&SEND_PACKET.BODY, 0, sizeof(PROTOCOL_BODY_DATA_TRANSPER_ORDER_RESPONSE_OHT2OCS));
	memcpy(&SEND_PACKET.BODY.Request, &UDP_PKT_Order.BODY.Request, sizeof(OHTREQUEST));
	SEND_PACKET.BODY.Result.TransOrderID = UDP_PKT_Order.BODY.OrderID & 0x07;
	SEND_PACKET.BODY.Result.Not_Accept_Class = FTPTranStaus & 0x03;

	switch(BodyAck)
	{
		case 0:
			//������ ���� �Ϸ� ����
			SEND_PACKET.BODY.Result.Trans_OK = 0x01;
			SEND_PACKET.BODY.Result.Trans_Fail = 0x00;
			SEND_PACKET.BODY.FailClass=0;
			//SEND_PACKET.BODY.FailClass[1]=0;

		break;
			
		case 1:
		case 0xFF:
			//������ ���� ����
			SEND_PACKET.BODY.Result.Trans_OK = 0x00;
			SEND_PACKET.BODY.Result.Trans_Fail = 0x01;

			//������ ����
			SEND_PACKET.BODY.FailClass=0xD8;
			//SEND_PACKET.BODY.FailClass[1]=0xF0;
		break;
			
		case 0x02:
			//������ ���� ��
			SEND_PACKET.BODY.Result.Trans_OK = 0x00;
			SEND_PACKET.BODY.Result.Trans_Fail = 0x00;
			SEND_PACKET.BODY.FailClass=0;
		break;

		case 0x99:
			//reset message
			SEND_PACKET.BODY.Result.Trans_OK = 0x00;
			SEND_PACKET.BODY.Result.Trans_Fail = 0x00;
			SEND_PACKET.BODY.FailClass=999;
		break;

		
		default:
		break;
	}
    SEND_PACKET.HEADER.MachineType = MACHINETYPE_FM;
	// ������ ����
	frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&SEND_PACKET, iPacketSize);

}

//---------------------------------------------------------------------------

void __fastcall TFileManagerForm::btnClearClick(TObject *Sender)
{
	Memo1->Lines->Clear();
}
//---------------------------------------------------------------------------


// -----------------------------------------
//		make response packet
// -----------------------------------------
/*
	iResult : WIRELESS_UPDATE_SUCCESS, WIRELESS_UPDATE_FAIL 
*/
void FMThreadUnit::WirelessUpdateResult(int iResult, AnsiString strResult)
{
	// AnsiString strResult;

	PKT_FTP_PUT_GET_RESULT_AS2OHT CmdPkt;
	int iPacketSize = sizeof(PKT_FTP_PUT_GET_RESULT_AS2OHT);

	memset(&CmdPkt, 0, sizeof(PKT_FTP_PUT_GET_RESULT_AS2OHT));
	// memcpy(&CmdPkt, &SEND_PKT, sizeof(PKT_FTP_PUT_GET_RESULT_AS2OHT));
	CmdPkt.TAG.Value = TagID::CMD_FIRMWARE_UPDATE_RESULT_WIRELESS_AS2OHT;
	CmdPkt.HEADER.MachineType = MACHINETYPE_FM;

	// Result Code
	CmdPkt.BODY.iResult = WIRELESS_UPDATE_SUCCESS;

	// strResult = "Test Update OK";
	memset(&CmdPkt.BODY.ResultText, '\0', (strResult.Length() + 1));
	memcpy(&CmdPkt.BODY.ResultText, strResult.c_str(), strResult.Length());

	// ����� �������ͽ��� OHT�� ä�� �ִ´�. BODY�� ���� ����
	frmUDP->pUdpFM->UnicastSend_FM_2_AS((char*)&CmdPkt, iPacketSize);

	return;
}

void __fastcall TFileManagerForm::Button1Click(TObject *Sender)
{
	Application->Minimize();

#if 0
	int iReturn;

	// IdFTP2->Host = "192.168.110.18";
	IdFTP2->Host = "127.0.0.1";
	IdFTP2->Port = 21;
	// IdFTP2->Username = "admin";
	IdFTP2->Username = "anonymous";
	IdFTP2->Password = "pass";
	IdFTP2->AutoLogin = false;
	IdFTP2->Passive = true;
	IdFTP2->Connect();
	IdFTP2->Login();

	int iResult;

	if(IdFTP2->Connected())
	{
		iResult = IdFTP2->SendCmd("ls");
		iResult = IdFTP2->SendCmd("ls");
		iResult = IdFTP2->SendCmd("quote rst");
	}
#endif
	// iResult = IdFTP2->Disconnect();

		//IdTCPClient1->

	// iReturn = IdTCPClient1->
}
//---------------------------------------------------------------------------

void __fastcall TFileManagerForm::btnShowClick(TObject *Sender)
{
	frmUDP->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFileManagerForm::btnHideClick(TObject *Sender)
{
	frmUDP->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TFileManagerForm::trmMinimizeTimer(TObject *Sender)
{
	trmMinimize->Enabled = false;
	Application->Minimize();
}
//---------------------------------------------------------------------------


typedef std::basic_string<TCHAR> tstring;
TCHAR* StringToTCHAR(AnsiString& s)
{
	 tstring tstr;
	 const char* all = s.c_str();
	 int len = 1 + strlen(all);
	 wchar_t* t = new wchar_t[len];
	 if (NULL == t) throw std::bad_alloc();
	 mbstowcs(t, all, len);
	 return (TCHAR*)t;
}


__fastcall MCCThreadUnit::MCCThreadUnit(bool CreateSuspended) : TThread(CreateSuspended)
{
	//Thread Priority Level : tpIdle, tpLowest, tpLower, tpNormal, tpHigher, tpHighest, tpTimeCritical
   Priority = tpIdle;
   m_Log = new OHTLogFile;
   Resume();
   zipflag = true;
   firstDelete = true;
   GetLocalTime(&LastTime);
}

//---------------------------------------------------------------------------
void __fastcall MCCThreadUnit::Execute()
{
	SYSTEMTIME CurTime;
	DWORD dwCurT, dwBufT;
	AnsiString strLog;

	LoadAssistantParam();

	while( !Terminated )
	{
		GetLocalTime(&CurTime);

		if(CurTime.wHour != LastTime.wHour)
		{
			if(CurTime.wMinute > 5)
			{
				zipflag = true;
				LastTime = CurTime;
			}
		}

		if(zipflag)
		{
			dwCurT = GetTickCount();

			zipflag = false;

			CompressMCCLog();

			dwBufT = GetTickCount() - dwCurT;
			strLog.sprintf("MCC Zip Time : %d",dwBufT);
			ADD_LOG(strLog);

			CompressAutoRecoveryLog();

			dwBufT = GetTickCount() - dwCurT;
			strLog.sprintf("AutoRecovery Zip Time : %d",dwBufT);
			ADD_LOG(strLog);

			CompressOtherLog();

			dwBufT = GetTickCount() - dwCurT;
			strLog.sprintf("Other Zip Time : %d",dwBufT);
			ADD_LOG(strLog);

			if(firstDelete)
			{
				DeleteOldZip();
				firstDelete = false;
			}
			else
			{
//				DeleteZip();
				DeleteZip_OneDay();
			}

            dwBufT = GetTickCount() - dwCurT;
			strLog.sprintf("Delete Zip Time : %d",dwBufT);
			ADD_LOG(strLog);
		}

		Sleep(1000);
	}
}

void MCCThreadUnit::CompressMCCLog()
{
	AnsiString strFName;
	AnsiString strLogPath;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, CompressDate, OneHour;
	AnsiString strLog;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);

	HZIP hz;

	TCHAR  zipName[100];
	TCHAR  logName[100];
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		strFName.sprintf("D:\\Log\\MCC*.log");
		strLogPath.sprintf("D:\\Log\\");

		done = findfirst(strFName.c_str(),&ffblk,0);

	   while (!done)
	   {
	      sprintf(FileName,"%s",ffblk.ff_name);

		  for(filesize = 0; filesize < sizeof(FileName); filesize++)
		  {
			  if(FileName[filesize] == NULL)
				break;
		  }

		  if(filesize < 14)
		  {
				done = findnext(&ffblk);
				Sleep(10);
				continue;
		  }
		  memcpy(Ziptime,&FileName[filesize-14],10);

		  memcpy(szTemp,Ziptime,4);
		  Year = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[4],2);
		  Month = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[6],2);
		  Day = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[8],2);
		  Hour = atoi(szTemp);

		  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
		  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

		  CompressDate = CurrentDateHour - OneHour;

		  AnsiString strtmpCompressDate = CompressDate.DateTimeString();
		  AnsiString strtmpLogDate = LogDate.DateTimeString();

		  if(strtmpLogDate == strtmpCompressDate)
		  {
			  strFName.sprintf("%s%s.zip", strLogPath,ffblk.ff_name);
			  wsprintf(zipName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
			  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s", ffblk.ff_name);
			  wsprintf(logName, TEXT("%s"),StringToTCHAR(strFName));

			 try
			 {
				hz = CreateZip(zipName, 0);
				ZipAdd(hz,logName,targetlogName);
				DeleteFile(targetlogName);
				CloseZip(hz);
			 }
			 catch (Exception &e)
			 {
			 }
		  }

		  done = findnext(&ffblk);
		  Sleep(10);
	   }
	   findclose(&ffblk);
	}
	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::CompressAutoRecoveryLog()
{
	AnsiString strFName;
	AnsiString strLogPath;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, CompressDate, OneHour;
	AnsiString strLog;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);

	HZIP hz;

	TCHAR  zipName[100];
	TCHAR  logName[100];
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		strFName.sprintf("D:\\Log\\AutoRecovery*.log");
		strLogPath.sprintf("D:\\Log\\");

		done = findfirst(strFName.c_str(),&ffblk,0);

	   while (!done)
	   {
	      sprintf(FileName,"%s",ffblk.ff_name);

		  for(filesize = 0; filesize < sizeof(FileName); filesize++)
		  {
			  if(FileName[filesize] == NULL)
				break;
		  }

		  if(filesize < 14)
		  {
				done = findnext(&ffblk);
				Sleep(10);
				continue;
		  }
		  memcpy(Ziptime,&FileName[filesize-14],10);

		  memcpy(szTemp,Ziptime,4);
		  Year = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[4],2);
		  Month = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[6],2);
		  Day = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[8],2);
		  Hour = atoi(szTemp);

		  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
		  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

		  CompressDate = CurrentDateHour - OneHour;

		  AnsiString strtmpCompressDate = CompressDate.DateTimeString();
		  AnsiString strtmpLogDate = LogDate.DateTimeString();

		  strLog.sprintf("Recovery strtmpCompressDate : %s, strtmpLogDate : %s",strtmpCompressDate, strtmpLogDate);
		  ADD_LOG(strLog);

		  if(strtmpLogDate == strtmpCompressDate)
		  {
			  strFName.sprintf("%s%s.zip", strLogPath,ffblk.ff_name);
			  wsprintf(zipName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
			  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s", ffblk.ff_name);
			  wsprintf(logName, TEXT("%s"),StringToTCHAR(strFName));

			 try
			 {
				hz = CreateZip(zipName, 0);
				ZipAdd(hz,logName,targetlogName);
				DeleteFile(targetlogName);
				CloseZip(hz);
			 }
			 catch (Exception &e)
			 {
			 }
		  }

		  done = findnext(&ffblk);
		  Sleep(10);
	   }
	   findclose(&ffblk);
	}
	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::CompressOtherLog()
{
	AnsiString strFName;
	AnsiString strLogPath;
	AnsiString strLogFront;
	AnsiString strLog;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, CompressDate, OneDay, OneHour;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	char LogNameFront[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);
	memset(LogNameFront,0,sizeof(char)*10);

	HZIP hz;

	TCHAR  zipName[100];
	TCHAR  logName[100];
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneDay = TDateTime::TDateTime(23, 0, 0, 0);
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		CompressDate = CurrentDateHour;

		  for(int i = 0; i < nCompressDelay; i++)
		  {
				CompressDate -= OneDay;
				CompressDate -= OneHour;
		  }

	  AnsiString strtmpCompressDate = CompressDate.FormatString("yyyymmddHH");

	  strLog.sprintf("strtmpCompressDate : %s",strtmpCompressDate);
	  ADD_LOG(strLog);

		strFName.sprintf("D:\\Log\\*%s*.log",strtmpCompressDate);
		strLogPath.sprintf("D:\\Log\\");

		done = findfirst(strFName.c_str(),&ffblk,0);

	   while (!done)
	   {
//			strLog.sprintf("Compress File Name : %s",ffblk.ff_name);
//			ADD_LOG(strLog);

			sprintf(FileName,"%s",ffblk.ff_name);

			  for(filesize = 0; filesize < sizeof(FileName); filesize++)
			  {
				  if(FileName[filesize] == NULL)
					break;
			  }

			  if(filesize < 14)
			  {
					done = findnext(&ffblk);
					Sleep(10);
					continue;
			  }

			  memcpy(LogNameFront,FileName,8);
			  strLogFront.sprintf("%s",LogNameFront);

			  if(strLogFront != "AMC_AXIS")
			  {
				  memcpy(Ziptime,&FileName[filesize-14],10);
			  }
			  else
			  {
					memcpy(Ziptime,&FileName[filesize-18],10);
			  }

			  memcpy(szTemp,Ziptime,4);
			  Year = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[4],2);
			  Month = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[6],2);
			  Day = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[8],2);
			  Hour = atoi(szTemp);

			  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
			  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

			  strFName.sprintf("%s%s.zip", strLogPath,ffblk.ff_name);
			  wsprintf(zipName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
			  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

			  strFName.sprintf("%s", ffblk.ff_name);
			  wsprintf(logName, TEXT("%s"),StringToTCHAR(strFName));

			 try
			 {
				hz = CreateZip(zipName, 0);
				ZipAdd(hz,logName,targetlogName);
				DeleteFile(targetlogName);
				CloseZip(hz);
			 }
			 catch (Exception &e)
			 {
			 }

			  done = findnext(&ffblk);
			  Sleep(10);
	   }
	   findclose(&ffblk);
	}
	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::DeleteOldZip()
{
	AnsiString strFName;
	AnsiString strLogPath;
	AnsiString strLogFront;
	AnsiString strLog;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, DeleteDate, OneDay, OneHour;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	char LogNameFront[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);
	memset(LogNameFront,0,sizeof(char)*10);

	HZIP hz;
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneDay = TDateTime::TDateTime(23, 0, 0, 0);
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		DeleteDate = CurrentDateHour;

		for(int i = 0; i < nMaxWaitTimeToDelete; i++)
		{
			DeleteDate -= OneDay;
			DeleteDate -= OneHour;
		}

		strFName.sprintf("D:\\Log\\*.zip");
		strLogPath.sprintf("D:\\Log\\");

		done = findfirst(strFName.c_str(),&ffblk,0);

	   while (!done)
	   {
//			strLog.sprintf("Delete File Name : %s",ffblk.ff_name);
//			ADD_LOG(strLog);

			sprintf(FileName,"%s",ffblk.ff_name);

		  for(filesize = 0; filesize < sizeof(FileName); filesize++)
		  {
			  if(FileName[filesize] == NULL)
				break;
		  }

		  if(filesize < 18)
		  {
				done = findnext(&ffblk);
				Sleep(10);
				continue;
		  }

		  memcpy(LogNameFront,FileName,8);
		  strLogFront.sprintf("%s",LogNameFront);

		  if(strLogFront != "AMC_AXIS")
		  {
			  memcpy(Ziptime,&FileName[filesize-18],10);
		  }
		  else
		  {
			  memcpy(Ziptime,&FileName[filesize-22],10);
		  }

		  memcpy(szTemp,Ziptime,4);
		  Year = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[4],2);
		  Month = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[6],2);
		  Day = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[8],2);
		  Hour = atoi(szTemp);

		  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
		  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

		  if(LogDate < DeleteDate)
		  {
			  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
			  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

			 try
			 {
				DeleteFile(targetlogName);
			 }
			 catch (Exception &e)
			 {
			 }
		  }

		  done = findnext(&ffblk);
		  Sleep(10);
	   }
	   findclose(&ffblk);
	}
	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::DeleteZip()
{
	AnsiString strFName;
	AnsiString strLogPath;
	AnsiString strLogFront;
	AnsiString strLog;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, DeleteDate, OneDay, OneHour;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	char LogNameFront[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);
	memset(LogNameFront,0,sizeof(char)*10);

	HZIP hz;
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneDay = TDateTime::TDateTime(23, 0, 0, 0);
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		DeleteDate = CurrentDateHour;

		for(int i = 0; i < nMaxWaitTimeToDelete; i++)
		{
			DeleteDate -= OneDay;
			DeleteDate -= OneHour;
		}

		AnsiString strtmpCompressDate = DeleteDate.FormatString("yyyymmdd");

		strLog.sprintf("strtmpCompressDate : %s",strtmpCompressDate);
	    ADD_LOG(strLog);

		strFName.sprintf("D:\\Log\\*%s*.log.zip",strtmpCompressDate);
		strLogPath.sprintf("D:\\Log\\");

		done = findfirst(strFName.c_str(),&ffblk,0);

	   while (!done)
	   {
//			strLog.sprintf("Delete File Name : %s",ffblk.ff_name);
//			ADD_LOG(strLog);

			sprintf(FileName,"%s",ffblk.ff_name);

		  for(filesize = 0; filesize < sizeof(FileName); filesize++)
		  {
			  if(FileName[filesize] == NULL)
				break;
		  }

		  if(filesize < 18)
		  {
				done = findnext(&ffblk);
				Sleep(10);
				continue;
		  }

		  memcpy(LogNameFront,FileName,8);
		  strLogFront.sprintf("%s",LogNameFront);

		  if(strLogFront != "AMC_AXIS")
		  {
			  memcpy(Ziptime,&FileName[filesize-18],10);
		  }
		  else
		  {
			  memcpy(Ziptime,&FileName[filesize-22],10);
		  }

		  memcpy(szTemp,Ziptime,4);
		  Year = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[4],2);
		  Month = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[6],2);
		  Day = atoi(szTemp);

		  memset(szTemp,0,sizeof(char)*10);
		  memcpy(szTemp,&Ziptime[8],2);
		  Hour = atoi(szTemp);

		  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
		  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

		  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
		  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

			 try
			 {
				DeleteFile(targetlogName);
			 }
			 catch (Exception &e)
			 {
			 }
		  done = findnext(&ffblk);
		  Sleep(10);
	   }
	   findclose(&ffblk);
	}

	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::DeleteZip_OneDay()
{
	AnsiString strFName;
	AnsiString strLogPath;
	AnsiString strLogFront;
	AnsiString strLog;
	TDateTime LogDate, LogTime, CurrentDate, CurrentDateHour, DeleteDate, OneDay, OneHour;

	char FileName[1024];
	char szTemp[10];
	char Ziptime[10];
	char LogNameFront[10];
	int filesize = 0;

	memset(FileName,0,sizeof(char)*1024);
	memset(szTemp,0,sizeof(char)*10);
	memset(Ziptime,0,sizeof(char)*10);
	memset(LogNameFront,0,sizeof(char)*10);

	HZIP hz;
	TCHAR  targetlogName[100];

	struct ffblk ffblk;
	int done;
	int  Year, Month, Hour, Day;
	WORD  CurYear, CurMonth, CurDay, CurHour, CurMin, CurSecond, CurMsec;

	try
	{
		OneDay = TDateTime::TDateTime(23, 0, 0, 0);
		OneHour = TDateTime::TDateTime(1, 0, 0, 0);
		CurrentDate = Now();
		DecodeDate(CurrentDate, CurYear, CurMonth, CurDay);
		DecodeTime(CurrentDate, CurHour, CurMin, CurSecond, CurMsec);

		LogTime = TDateTime::TDateTime(CurHour, 0, 0, 0);
		CurrentDateHour = TDateTime::TDateTime(CurYear, CurMonth, CurDay)+(LogTime-(int)LogTime);

		DeleteDate = CurrentDateHour;

		for(int i = 0; i < nMaxWaitTimeToDelete; i++)
		{
			DeleteDate -= OneDay;
			DeleteDate -= OneHour;
		}

		DeleteDate += OneHour;

		for(int beforehour = 0;beforehour < 24 ; beforehour++)
		{
			DeleteDate -= OneHour;
			AnsiString strtmpDeleteDate = DeleteDate.FormatString("yyyymmddHH");

			strLog.sprintf("%d - strtmpDeleteDate : %s",beforehour,strtmpDeleteDate);
			ADD_LOG(strLog);

			strFName.sprintf("D:\\Log\\*%s.log.zip",strtmpDeleteDate);
			strLogPath.sprintf("D:\\Log\\");

			done = findfirst(strFName.c_str(),&ffblk,0);

		   while (!done)
		   {
//				strLog.sprintf("Delete File Name : %s",ffblk.ff_name);
//				ADD_LOG(strLog);

				sprintf(FileName,"%s",ffblk.ff_name);

			  for(filesize = 0; filesize < sizeof(FileName); filesize++)
			  {
				  if(FileName[filesize] == NULL)
					break;
			  }

			  if(filesize < 18)
			  {
					done = findnext(&ffblk);
					Sleep(10);
					continue;
			  }

			  memcpy(LogNameFront,FileName,8);
			  strLogFront.sprintf("%s",LogNameFront);

			  if(strLogFront != "AMC_AXIS")
			  {
				  memcpy(Ziptime,&FileName[filesize-18],10);
			  }
			  else
			  {
				  memcpy(Ziptime,&FileName[filesize-22],10);
			  }

			  memcpy(szTemp,Ziptime,4);
			  Year = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[4],2);
			  Month = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[6],2);
			  Day = atoi(szTemp);

			  memset(szTemp,0,sizeof(char)*10);
			  memcpy(szTemp,&Ziptime[8],2);
			  Hour = atoi(szTemp);

			  LogTime = TDateTime::TDateTime(Hour, 0, 0, 0);
			  LogDate = TDateTime::TDateTime(Year, Month, Day)+(LogTime-(int)LogTime);

			  strFName.sprintf("%s%s", strLogPath,ffblk.ff_name);
			  wsprintf(targetlogName, TEXT("%s"),StringToTCHAR(strFName));

				 try
				 {
					DeleteFile(targetlogName);
				 }
				 catch (Exception &e)
				 {
				 }
			  done = findnext(&ffblk);
			  Sleep(10);
		   }
		   findclose(&ffblk);
		   Sleep(10);
		}
	}

	catch(...)
	{
		strLog.sprintf("Status Exception!!!!");
		ADD_LOG(strLog);
		findclose(&ffblk);
	}
}

void MCCThreadUnit::LoadAssistantParam()
{
	m_IniFile = new TIniFile(FILE_PATH_AST_PARAM);

	if(m_IniFile != NULL)
	{
		//FileControl
		//AOHC-418 ���� ���� ���� �Ⱓ �Ķ���� ����
		nMaxWaitTimeToDelete = m_IniFile->ReadInteger("FileControl", "MaxWaitTimeToDeleteZip", 12);
		nCompressDelay  = m_IniFile->ReadInteger("FileControl", "CompressDelay", 1);

	}

	delete m_IniFile;
}

//---------------------------------------------------------------------------


