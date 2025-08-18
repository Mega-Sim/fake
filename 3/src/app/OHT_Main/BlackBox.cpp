
#pragma hdrstop

#include <vcl.h>

#include "BlackBox.h"
#include "OHTMain.h"
#include "mmsystem.h"

#define ADD_BLACKBOX_LOG(...)			WRITE_LOG_UNIT(pLogBlackBoxComm, __VA_ARGS__)

BlackBox::BlackBox()
{
	pLogBlackBoxComm = getLogUnit("D:\\log\\BlackBoxComm.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	
	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	
	if(m_defualtparam->bUse4ChBlackBox == true)
	{
		pBlackBoxClientSocket = new TCPCLIENT;
	}
	
	BlackBoxEventType = 0;
	BlackBoxEventResult = 0;
	BlackBoxError = 0;

	m_bBlackBoxTimeSet = false;
	m_bBlackBoxEventOK = false;
	m_dwRetryCommBlackBox = timeGetTime();

	m_nBlackBoxCommand = BLACKBOX_TIMESET;

	m_nBlackBoxEventType = 0;
}

BlackBox::~BlackBox()
{
	if(m_defualtparam->bUse4ChBlackBox == true)
	{
		delete pBlackBoxClientSocket;
	}
}

void BlackBox::ExcuteTimeCheck()
{
	SYSTEMTIME CurTime;
	GetLocalTime(&CurTime);
	
	if(m_defualtparam->bUse4ChBlackBox == true)
	{
		static bool bTimeSetFlag = false;

		// 0�ÿ� �ð� ����
		if(bTimeSetFlag == false && CurTime.wHour == 0 && CurTime.wMinute == 0)
		{
			bTimeSetFlag = true;
			m_nBlackBoxCommand = BLACKBOX_TIMESET;
		}
		else if(bTimeSetFlag == true && CurTime.wHour == 0 && CurTime.wMinute != 0)
		{
        	bTimeSetFlag = false;
        }
    }
}

void BlackBox::BlackBoxSocketUpdate()
{
	char RecvBuf[LEN_TLV_BUFFER] = {0,}; // ������ ������ ����Ǵ� ����
	AnsiString strIp = "";
	int nError = NO_ERR;
	int nStateRead = 2; // 0: normal, 1: null socket, 2: comm fail
	int nReadDataType = 0;  // 1:TimeSet, 2:Event, 3:Error
	static int nConnectRetry = 0;
	static DWORD dwBlackBoxConnectTime = timeGetTime();

	if(m_defualtparam->bUse4ChBlackBox == true)
	{
		if(pBlackBoxClientSocket != NULL)
		{
			// ���� Check
			if(pBlackBoxClientSocket->IsConnect() == false)
			{
				if(nConnectRetry < 5)
				{
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwBlackBoxConnectTime) > 5000)
					{
						strIp.sprintf("%s", m_defualtparam->BlackBoxIP);
						nError = pBlackBoxClientSocket->Open(strIp, 10007);
						dwBlackBoxConnectTime = timeGetTime();
						ADD_BLACKBOX_LOG("Black Box Connect_1(%d) IP:%s, Error:%d", nConnectRetry, strIp, nError);
						if(nError != -2)
							nConnectRetry++;
						else
							nConnectRetry = 6;
					}
				}
				else if(nConnectRetry < 10)
				{
                	if(OHTUTILITY::PreventOverflowGetTimeMs(dwBlackBoxConnectTime) > 5000)
					{
						nError = pBlackBoxClientSocket->Connect();
						dwBlackBoxConnectTime = timeGetTime();
						ADD_BLACKBOX_LOG("Black Box Connect_3, Error:%d", nError);
						nConnectRetry++;
					}
				}
				else if(nConnectRetry < 20)
				{
					if(OHTUTILITY::PreventOverflowGetTimeMs(dwBlackBoxConnectTime) > 60000)
					{
						strIp.sprintf("%s", m_defualtparam->BlackBoxIP);
						nError = pBlackBoxClientSocket->Open(strIp, 10007);
						dwBlackBoxConnectTime = timeGetTime();
						ADD_BLACKBOX_LOG("Black Box Connect_2 IP:%s, Error:%d", strIp, nError);
						nConnectRetry++;
					}
				}
			}
			else
			{
//				if(OHTMainForm->btnBlackBoxTime->Enabled == false)
//					OHTMainForm->btnBlackBoxTime->Enabled = true;
//
//				if(OHTMainForm->btnEventSave->Enabled == false)
//					OHTMainForm->btnEventSave->Enabled = true;
//
//				if(OHTMainForm->btnEventStreaming->Enabled == false)
//					OHTMainForm->btnEventStreaming->Enabled = true;

				pBlackBoxClientSocket->Update();
				nConnectRetry = 0;
				//Sleep(100);
				nError = BlackBoxSocketParsing(&nReadDataType);

				if(nError == NO_ERR)
				{
					switch(nReadDataType)
					{
						case 1:     // Time Set
							// �ð� ��
							if(DifHour(m_BlackBox_SetTime_Recv, m_BlackBox_SetTime_Send) < 10)
							{
								m_bBlackBoxTimeSet = true;
								m_nBlackBoxCommand = BLACKBOX_STATUS;
								//m_nBlackBoxCommand = BLACKBOX_NONE;
							}
							else
							{
								m_bBlackBoxTimeSet = false;
								m_nBlackBoxCommand = BLACKBOX_STATUS;
								//m_nBlackBoxCommand = BLACKBOX_NONE;
								m_dwRetryCommBlackBox = timeGetTime();
							}
							ADD_BLACKBOX_LOG("Black Box TimeSet Result : %d (OK:1, NG:0)", m_bBlackBoxTimeSet);
							break;
						case 2:     // Event
							if(BlackBoxEventResult == true)
							{
								m_bBlackBoxEventOK = true;
								m_nBlackBoxCommand = BLACKBOX_STATUS;
								//m_nBlackBoxCommand = BLACKBOX_NONE;
							}
							else
							{
								m_bBlackBoxEventOK = false;
								m_nBlackBoxCommand = BLACKBOX_STATUS;
								//m_nBlackBoxCommand = BLACKBOX_NONE;
								m_dwRetryCommBlackBox = timeGetTime();
							}
							ADD_BLACKBOX_LOG("Black Box Event Set Result : %d (OK:1, NG:0)", m_bBlackBoxEventOK);
							break;
						case 3:     // Error
							m_nBlackBoxCommand = BLACKBOX_STATUS;
							//m_nBlackBoxCommand = BLACKBOX_NONE;
							ADD_BLACKBOX_LOG("Black Box Error : %d", BlackBoxError);
							break;
					}
				}
			}
		}
	}
}
//---------------------------------------------------------------------------

int BlackBox::BlackBoxSocketParsing(int *nDataType)
{

	int nError = 0;
	int nRecvLen = 0; // ������ bytes
	int SndLen = 0; // �۽��� bytes ��
	char RecvBuf[LEN_TLV_BUFFER] = {0,}; // ������ ������ ����Ǵ� ����
	char SendBuf[LEN_TLV_BUFFER] = {0,}; // �۽��� ������ ����Ǵ� ����


	char *pTmpBuffer;
	int nCount = 0;
	int TotalLen = 0;
	int nsumcheck = 0;

	bool bNotFrist = false;
	bool nFinishFlag = false;
	int iPacketSize;
	int *nBuffOffset = 0;


	if(pBlackBoxClientSocket == NULL)
	{
		ADD_BLACKBOX_LOG("pBlackBoxClientSocket = NULL");
		return 1;
	}

	//Step 0. TLV Socket�� Read�ϱ� ���� ȣ��
	nRecvLen = pBlackBoxClientSocket->Read(RecvBuf);

	if(nRecvLen <=0 || nRecvLen > LEN_TLV_BUFFER)
	{
		//ADD_BLACKBOX_LOG("nRecvLen ������: %d", nRecvLen);
		return 2;// ���빰�� ���� ���  nRecvLen=0;
	}
	//ADD_BLACKBOX_LOG("nRecvLen ���� : Length(%d)", nRecvLen);

	TotalLen = nRecvLen;
	pTmpBuffer = new char[TotalLen];
	memset(pTmpBuffer, 0x00, TotalLen);
	memmove(pTmpBuffer, RecvBuf, TotalLen);

	//Step 1-1. pTmpBuffer�ȿ� �������� TLV�� ���� ��� �ݺ������� �����ϱ� ���� �ݺ�����
	while(!nFinishFlag)
	{
		int UsedLen = 0;
		int chkroof = 0;
		char pTLV[LEN_TLV_BUFFER] = {0,};

		//  Read Header
		// Step 2. 1���� TLV�� �����ϱ� ���� �κ�
		chkroof = GetBlackBoxData(pTmpBuffer, TotalLen, pTLV, &UsedLen, true);

		if(chkroof == TLVSHORTDATA)
        {
//            // TLV�� �������� ����  ���, ������ �����͸� ���� �� ����(���̰� ª�� ����).
//
//			m_BlackBoxServerSock->BufferPointer = new char[TotalLen];
//			memset(m_BlackBoxServerSock->BufferPointer, 0x00, TotalLen);
//			memmove(m_BlackBoxServerSock->BufferPointer, pTmpBuffer, TotalLen);
//			m_BlackBoxServerSock->nRestLenth = TotalLen;

			delete[]pTmpBuffer;
			pTmpBuffer = NULL;

			ADD_BLACKBOX_LOG("Warning : short Data BIS! [TotalLen:%d]",TotalLen);
			nFinishFlag = true;
			break; // wile�� ��������.  return;
		}
		else if(chkroof == TLVWRONGDATA)
		{
			// TLV Data�� �߸� ���۵� ��� ��� ���� �ʱ�ȭ �� ����

			delete[]pTmpBuffer;
			pTmpBuffer = NULL;

			ADD_BLACKBOX_LOG("Warning : Wrong Data BIS! [TotalLen:%d][UsedLen:%d]",TotalLen,UsedLen);
			nFinishFlag = true;
			break; // wile�� ��������.  return;
		}

		// Step 3. ���������� 1���� TLV�� ����� �� Parsing�ϴ� �κ�
		// (1) ��� ũ��� ��Ī�Ǵ��� Ȯ���Ѵ�.
		iPacketSize = sizeof(PKT_CHECK_TAG);	// TAG�� �ǵ��ؾ� �ϹǷ�.

		if(UsedLen > iPacketSize)	// ���� ��Ŷ�� ũ�Ⱑ ����� ���� ������ŭ ������� (ó���ϸ� �ȵ�)
		{
			// (2) ��Ŷ�� ������ �Ǻ��Ѵ�. �׸��� OHT���� �� ���� Ȯ�ε� �ؾ��Ѵ�.
			PKT_CHECK_TAG Pkt;
			//memcpy(&Pkt, &pTmpBuffer, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
			memcpy(&Pkt, &pTLV, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
			//Conv_PKT_CHECK_TAG(Pkt);
			ConvertEndian(&Pkt.HEADER.MachineType);
			ConvertEndian(Pkt.HEADER.MachineID);
			ConvertEndian(&Pkt.TAG.Value);

			ADD_BLACKBOX_LOG("MachineType : %d, Tag ID : %d", Pkt.HEADER.MachineType, Pkt.TAG.Value);

			if(Pkt.HEADER.MachineType == MACHINETYPE_BLACKBOX)
			{
				switch(Pkt.TAG.Value)	// ��ɾ� ������ ���� �б�
				{
					case TagID::CMD_TIMESETTING_BLACKBOX_RESPONSE:  // �ð� ���� ����
						iPacketSize = sizeof(PKT_BLACKBOX_TIME);
						PKT_BLACKBOX_TIME PktBlackBoxTime;
						memcpy(&PktBlackBoxTime, &pTLV, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
						*nDataType = 1;
						m_BlackBox_SetTime_Recv = PktBlackBoxTime.TIME;
						ADD_BLACKBOX_LOG("Tag ID : %d [Time]", Pkt.TAG.Value);
						break;

					case TagID::CMD_EVENTSET_BLACKBOX_RESPONSE:     // �̺�Ʈ ����
						iPacketSize = sizeof(PKT_BLACKBOX2OHT_EVENT);
						PKT_BLACKBOX2OHT_EVENT PktBlackBoxEvent;
						memcpy(&PktBlackBoxEvent, &pTLV, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
						*nDataType = 2;
						BlackBoxEventType = PktBlackBoxEvent.EVENT.EventType;
						BlackBoxEventResult = PktBlackBoxEvent.EVENT.EventResult;
						ADD_BLACKBOX_LOG("Tag ID : %d [Event], Value : Type(%d), Result(%d)", Pkt.TAG.Value, BlackBoxEventType, BlackBoxEventResult);
						break;
					case TagID::CMD_ERROR_BLACKBOX:                 // Error ����
						iPacketSize = sizeof(PKT_BLACKBOX2OHT_EVENT);
						PKT_BLACKBOX2OHT_EVENT PktBlackBoxError;
						memcpy(&PktBlackBoxError, &pTLV, iPacketSize);	// ���۷� �Ѿ�� ������ ���ũ�⸸ŭ�� �ϴ� �����Ѵ�
						*nDataType = 3;
						BlackBoxError = PktBlackBoxError.EVENT.EventType;
						ADD_BLACKBOX_LOG("Tag ID : %d [Error], Value : %d", Pkt.TAG.Value, BlackBoxError);
						break;

					default:
						ADD_BLACKBOX_LOG("Tag ID�ʵ忡 �ؼ� �Ұ����� ���� ���Խ��ϴ�. Tag ID : %d", Pkt.TAG.Value);
						return -1;
				}
			}
		}

		// �ٸ� ����� �����ֳ� Ȯ��
		delete[]pTmpBuffer;
		pTmpBuffer = NULL;

		nFinishFlag = true;
		break;

		Sleep(1);
	}

	return 0;
}
//---------------------------------------------------------------------------

void BlackBox::SuperviseSendDataOHT2BLACKBOX()
{
	static DWORD dwCheckBlackBoxSendTime = timeGetTime();
	char strMsg[1024] = {0,};
	int nLength = 0, nOffset = 0;;

	if(m_defualtparam->bUse4ChBlackBox == true)
	{
		if(pBlackBoxClientSocket->IsConnect() == true)
		{
			// 1. ����ð� ����
			if(m_nBlackBoxCommand == BLACKBOX_TIMESET)
			{
				//Response List�� ����� ����ü (���� �޸� �Ҵ�)
				PKT_BLACKBOX_TIME SendData;

				// Step 1. Header ����  ����
				SendData.HEADER.MachineType = MACHINETYPE_VHL; // OHT�� ����
				ConvertEndian(&SendData.HEADER.MachineType);
				memmove(&SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));
				ConvertEndian(SendData.HEADER.MachineID);

				memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
				nOffset = nOffset + sizeof(SendData.HEADER);

				// Step 2. Tag ����  ����
				SendData.TAG.Value = TagID::CMD_TIMESETTING_BLACKBOX;
				ConvertEndian(&SendData.TAG);

				memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
				nOffset = nOffset + sizeof(SendData.TAG.Value);

				// Step 3. ���� �ð� ���� ����
				GetLocalTime(&m_BlackBox_SetTime_Send);
				SendData.TIME = m_BlackBox_SetTime_Send;
				ConvertEndian(&SendData.TIME.wYear);
				ConvertEndian(&SendData.TIME.wMonth);
				ConvertEndian(&SendData.TIME.wDayOfWeek);
				ConvertEndian(&SendData.TIME.wDay);
				ConvertEndian(&SendData.TIME.wHour);
				ConvertEndian(&SendData.TIME.wMinute);
				ConvertEndian(&SendData.TIME.wSecond);
				ConvertEndian(&SendData.TIME.wMilliseconds);

				memmove(strMsg + nOffset, &SendData.TIME, sizeof(SendData.TIME));
				nOffset = nOffset + sizeof(SendData.TIME);

				pBlackBoxClientSocket->Send(strMsg, sizeof(PKT_BLACKBOX_TIME));
				ADD_BLACKBOX_LOG("Time Setting, Length(%d)", nOffset);

				m_nBlackBoxCommand = BLACKBOX_NONE;

				return;
			}
			else if(m_nBlackBoxCommand == BLACKBOX_EVENT)
			// 2. Event ���
			{
				//Response List�� ����� ����ü (���� �޸� �Ҵ�)
				PKT_OHT2BLACKBOX_EVENT SendData;

				// Step 1. Header ����  ����
				SendData.HEADER.MachineType = MACHINETYPE_VHL; // OHT�� ����
				ConvertEndian(&SendData.HEADER.MachineType);
				memmove(&SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));
				ConvertEndian(SendData.HEADER.MachineID);

				memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
				nOffset = nOffset + sizeof(SendData.HEADER);

				// Step 2. Tag ����  ����
				SendData.TAG.Value = TagID::CMD_EVENTSET_BLACKBOX;
				ConvertEndian(&SendData.TAG);

				memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
				nOffset = nOffset + sizeof(SendData.TAG.Value);

				// Step 3. Event ����
				if(m_nBlackBoxEventType == 1)
					SendData.EventType = BLACKBOX_VIDEOSAVE;
				else if(m_nBlackBoxEventType == 2)
					SendData.EventType = BLACKBOX_STREAMING;
				ConvertEndian(&SendData.EventType);

				memmove(strMsg + nOffset, &SendData.EventType, sizeof(SendData.EventType));
				nOffset = nOffset + sizeof(SendData.EventType);

				pBlackBoxClientSocket->Send(strMsg, sizeof(PKT_OHT2BLACKBOX_EVENT));
				ADD_BLACKBOX_LOG("Event : %d (1:Save, 2:Streaming), Length(%d)", SendData.EventType, nOffset);

				m_nBlackBoxCommand = BLACKBOX_NONE;

				return;
			}
			else if(m_nBlackBoxCommand == BLACKBOX_NONE)
			{
            	return;
			}
			// 3. �������� ����
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwCheckBlackBoxSendTime) > 500)
			{
				//Response List�� ����� ����ü (���� �޸� �Ҵ�)
				PKT_OHT2BLACKBOX SendData;

				// Step 1. Header ����  ����
				SendData.HEADER.MachineType = MACHINETYPE_VHL; // OHT�� ����
				ConvertEndian(&SendData.HEADER.MachineType);
				memmove(&SendData.HEADER.MachineID, &m_defualtparam->VHLName, sizeof(SendData.HEADER.MachineID));
				ConvertEndian(SendData.HEADER.MachineID);

				memmove(strMsg, &SendData.HEADER, sizeof(SendData.HEADER));
				nOffset = nOffset + sizeof(SendData.HEADER);

				// Step 2. Tag ����  ����
				SendData.TAG.Value = TagID::CMD_STATUS_OHT_BLACKBOX;
				ConvertEndian(&SendData.TAG);

				memmove(strMsg + nOffset, &SendData.TAG.Value, sizeof(SendData.TAG.Value));
				nOffset = nOffset + sizeof(SendData.TAG.Value);

				// Step 3. OHT status Data ����
				memmove(&SendData.OHT_STATUS, &m_pOHTMainStatus->StatusBLACKBOX, sizeof(MSG_STATUS_OHT2BLACKBOX));
				ConvertEndian(&SendData.OHT_STATUS.CurMode);
				ConvertEndian(&SendData.OHT_STATUS.CurStatus);
				ConvertEndian(&SendData.OHT_STATUS.CurNodeID);
				ConvertEndian(&SendData.OHT_STATUS.TargetNodeID);
				ConvertEndian(&SendData.OHT_STATUS.CurNodeOffset);
				ConvertEndian(&SendData.OHT_STATUS.DrivingSpeed);
				ConvertEndian(&SendData.OHT_STATUS.Edc);
				ConvertEndian(&SendData.OHT_STATUS.Iout_sign);
				ConvertEndian(&SendData.OHT_STATUS.Iout);
				ConvertEndian(&SendData.OHT_STATUS.IoT_Torque_Driving_F);
				ConvertEndian(&SendData.OHT_STATUS.IoT_Torque_Driving_R);
				ConvertEndian(&SendData.OHT_STATUS.ErrorCode);
				ConvertEndian(&SendData.OHT_STATUS.OHTDetectLevel);
				ConvertEndian(&SendData.OHT_STATUS.OBSDetectLevel);


				memmove(strMsg + nOffset, &SendData.OHT_STATUS, sizeof(SendData.OHT_STATUS));
				nOffset = nOffset + sizeof(SendData.OHT_STATUS);

				pBlackBoxClientSocket->Send(strMsg, sizeof(PKT_OHT2BLACKBOX));

				dwCheckBlackBoxSendTime = timeGetTime();
			}
		}
	}
}
//---------------------------------------------------------------------------

int BlackBox::GetBlackBoxData(char *pBuffer, int BuffLen, char *pTLV, int *Bytes, bool IsConvertEndian)
{
	int BodyLen = 0; // Body Length
	int TailLen = 0;
	int ReadByteLen = 0;

	if(BuffLen <= 0)	return TLVWRONGDATA; //Data Wrong

	// ���� ó��
	if(BuffLen < LEN_HEADER_BLACKBOX)	return TLVSHORTDATA;

	// Step 1. Header Read
	memcpy(pTLV, pBuffer, LEN_HEADER_BLACKBOX);

	// Step 2. Body�� Length ����
	if(IsConvertEndian)
	{
		if((((BYTE)pTLV[8]) * 256 + (BYTE)pTLV[9]) == TagID::CMD_TIMESETTING_BLACKBOX_RESPONSE)
			BodyLen = LEN_BLACKBOX_TIMESET;
		else if((((BYTE)pTLV[8]) * 256 + (BYTE)pTLV[9]) == TagID::CMD_EVENTSET_BLACKBOX_RESPONSE)
			BodyLen = LEN_BLACKBOX_EVENT;
		else if((((BYTE)pTLV[8]) * 256 + (BYTE)pTLV[9]) == TagID::CMD_ERROR_BLACKBOX)
			BodyLen = LEN_BLACKBOX_EVENT;
		else
			return TLVWRONGDATA;
	}
	else
	{
		if(((BYTE)pTLV[8] + ((BYTE)pTLV[9]) * 256) == TagID::CMD_TIMESETTING_BLACKBOX_RESPONSE)
			BodyLen = LEN_BLACKBOX_TIMESET;
		else if(((BYTE)pTLV[8] + ((BYTE)pTLV[9]) * 256) == TagID::CMD_EVENTSET_BLACKBOX_RESPONSE)
			BodyLen = LEN_BLACKBOX_EVENT;
		else if(((BYTE)pTLV[8] + ((BYTE)pTLV[9]) * 256) == TagID::CMD_ERROR_BLACKBOX)
			BodyLen = LEN_BLACKBOX_EVENT;
		else
			return TLVWRONGDATA;
	}

	// by zzang9un 2013.03.15 : Body�� Tail���� �߰��� �о��
	if(BodyLen < 0)	return TLVWRONGDATA; //Data Wrong

	ReadByteLen = BodyLen;

	 // ���� ó��
	if(BuffLen < ReadByteLen)	return TLVSHORTDATA;

	// Step 3. Body Read
	// by zzang9un 2013.03.15 : Tail���� �Ѳ����� �о��
	memcpy(pTLV+LEN_HEADER_BLACKBOX, pBuffer+LEN_HEADER_BLACKBOX, ReadByteLen);

	// by zzang9un 2013.03.15 : Tail Lenth �߰�
	ReadByteLen = BodyLen + LEN_HEADER_BLACKBOX;

	*Bytes = ReadByteLen;

	return 0;
}
//---------------------------------------------------------------------------

long BlackBox::DifHour(SYSTEMTIME sTm1, SYSTEMTIME sTm2)

{
	long nRtn;
	FILETIME fTm1, fTm2;
	ULONGLONG *ullVal1, *ullVal2;
	ULONGLONG ullDif;

	SystemTimeToFileTime(&sTm1, &fTm1);
	SystemTimeToFileTime(&sTm2, &fTm2);

	ullVal1 = (ULONGLONG *)&fTm1;
    ullVal2 = (ULONGLONG *)&fTm2;

	if(*ullVal1 > *ullVal2)
	{
		ullDif = *ullVal1 - *ullVal2;
	}
	else
	{
		ullDif = *ullVal2 - *ullVal1;
	}

	nRtn = long(ullDif / 10000000);

    return nRtn;
}

#pragma package(smart_init)
