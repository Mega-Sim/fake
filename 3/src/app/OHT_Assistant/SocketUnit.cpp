//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SocketUnit.h"
#include "MonitoringUtil.h"
#include "FileControl.h"
#include "ProcessTerminator.h"
#include "AssistantMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#define ADD_COMM_LOG(STR)   m_Log->WriteLogFile("Comm", STR)
#define ADD_SOCKET_LOG(STR)	m_Log->WriteLogFile("Socket", STR)
#define STR_SEND            "[Snd]"
#define STR_RECV            "[Rcv]"

#define ADD_TEST_LOG(STR)	m_Log->WriteLogFile("Test", STR)

TSocketForm *SocketForm;
//---------------------------------------------------------------------------
__fastcall TSocketForm::TSocketForm(TComponent* Owner)
	: TForm(Owner)
{
	m_Log = new OHTLogFile;

	// by zzang9un 2013.10.10 : Socket ����
	m_ServerSocketDM->Port = PORT_ASSISTANT_DM;
//	m_ServerSocketDM->Active = true;

	m_ServerSocketTP->Port = PORT_TP_ASSISTANT;
    m_ServerSocketTP->Active = true;

    m_bDMConnected = false;

    // TLV ���� ���� �ʱ�ȭ
	InitBigTLV_DM();
	InitBigTLV_TP();
}
//---------------------------------------------------------------------------

/**
@brief   TP Sockek�� Read �̺�Ʈ �߻� �� ȣ��Ǵ� �Լ�
@author  zzang9un
@date    2013.04.11
*/
void __fastcall TSocketForm::TPSocketClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	int nError = 0;
	int nRecvBytes;
	int nTotalLenRead = 0;  // Read�� �� Bytes ����
	int nLenBytesToRead;    // �о���� Bytes ����
	int WaitCount;          // ��� Ƚ��

	int BodyLen = 0; // Body ũ��

	char RecvBuf[LEN_TLV_BUFFER]; // Header ������ ����� ����

	String strLog;  // Log�� ���� ����

	// TLV�� ó�� Read�ϴ� ������� ����� TLV������ üũ
	if((bBigTLV_TP == false) && (m_PreCmdHeader_TP.Length == 0))
    {
		// -------------------------------------------
        // [BEGIN] by zzang9un 2013.10.15 : TLV�� ó�� �д� ���

		// Step 1. TLV�� Header�� �о�´�.
		// ������ ���̸� �о�´�.
        WaitCount = 0;
		while(1)
        {
			nRecvBytes = Socket->ReceiveLength();

			if(nRecvBytes >= LEN_HEADER)
                break;

			if(++WaitCount > MAX_WAIT)
				break;

			Sleep(1);
		}

		// ���� ó��
        if(nRecvBytes < LEN_HEADER)
		{
			// ���� �α� ����
            strLog.sprintf(L"[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
			ADD_COMM_LOG(strLog);
			return;
        }

		// ���ۿ��� Header�� Read
        nRecvBytes = Socket->ReceiveBuf(RecvBuf, LEN_HEADER);

		// ���� ó��
        if(nRecvBytes < LEN_HEADER)
        {
			// ���� �α� ����
            strLog.sprintf(L"[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
			ADD_COMM_LOG(strLog);
            return;
		}

		// Header�� ����ü�� ����
        CMD_HEADER RcvHeader;
		RcvHeader = GetHeader(RecvBuf);

		// Step 2. TLV�� Body�� �о�´�.
        // Body�� ���̸� ����
        // Endian ��Ŀ� ���� Body Length ���� ����� �޶���
		BodyLen = (BYTE)RecvBuf[4] | (RecvBuf[5] << 8) ;

        // Step 3. Socket�� ���� ũ��� TLV�� ũ�⸦ ���Ͽ� BigTLV���� �ƴ����� �����Ѵ�.
		// (Body + LEN_HEADER + LEN_TAIL)�� ũ�Ⱑ Socket ������ ũ�⺸�� ū �� ��
        if((BodyLen + LEN_HEADER + LEN_TAIL) > LEN_TLV_BUFFER)
        {
			// -------------------------------------------
            // [BEGIN] by zzang9un 2013.10.15 : Socket ����(8192)���� ū TLV�� ���

			// Socket ���ۺ��� ū Body�� �� ��츦 ����� flag�� ���� Cmd header�� ����
            bBigTLV_TP = true;
            m_PreCmdHeader_TP = RcvHeader;

            // �ʿ��� ũ�⸸ŭ ���� ���۸� ����
            m_BigTLVTotalLen_TP = LEN_HEADER + BodyLen + LEN_TAIL;
			pBigTLV_TP = new char[m_BigTLVTotalLen_TP];

            // �̹� ���� Header�� pBigTLV_TP ���۷� �ű��.
			memcpy(pBigTLV_TP, RecvBuf, LEN_HEADER);
            m_BigTLVReadCnt_TP = LEN_HEADER; // ���� ���� �� �ݿ�

			// Body�� �������� �о�´�.
            WaitCount = 0;
            while(1)
			{
				nLenBytesToRead = Socket->ReceiveLength();

				// ù TLV�� Header�� ������ ũ��� ��
                if(nLenBytesToRead >= (LEN_TLV_BUFFER - LEN_HEADER))
                    break;

                if(++WaitCount > MAX_WAIT)
                    break;

                Sleep(1);
            }

            // ���� ó��
            if(nLenBytesToRead < (LEN_TLV_BUFFER - LEN_HEADER))
			{
                InitBigTLV_TP();

				// ���� �α� ����
                strLog.sprintf(L"[Error][Body:%d] nLenBytesToRead(%d) Less than (LEN_TLV_BUFFER - LEN_HEADER)(%d)", nLenBytesToRead, BodyLen, (LEN_TLV_BUFFER - LEN_HEADER));
                ADD_COMM_LOG(strLog);
				return;
            }

			// ù TLV���� Body�� ���ۿ� Read��
            nRecvBytes = Socket->ReceiveBuf(pBigTLV_TP + LEN_HEADER, nLenBytesToRead);

			// ���� ó��
            if(nRecvBytes < nLenBytesToRead)
            {
				InitBigTLV_TP();

                // ���� �α� ����
				strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
                return;
			}

            m_BigTLVReadCnt_TP += nRecvBytes; // Read�� Byte�� �ݿ�
			// [END] by zzang9un 2013.10.15 : Socket ����(8192)���� ū TLV�� ���
            // -------------------------------------------
        }
		else
		{
            // -------------------------------------------
			// [BEGIN] by zzang9un 2013.10.15 : Socket ����(8192)���� ���� TLV�� ���
			bBigTLV_TP = false;
            memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));

            // TLV Body�� �о�´�
            nLenBytesToRead = BodyLen + LEN_TAIL;

            WaitCount = 0;
            while(1)
			{
                nRecvBytes = Socket->ReceiveLength();

				if(nRecvBytes >= nLenBytesToRead)
                    break;

				if(++WaitCount > MAX_WAIT)
                    break;

				Sleep(1);
            }

			if(nRecvBytes < nLenBytesToRead)
            {
                // ���� �α� ����
				strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
                return;
			}

            nRecvBytes = Socket->ReceiveBuf(RecvBuf + LEN_HEADER, nLenBytesToRead);

            if(nRecvBytes < nLenBytesToRead)
            {
				// ���� �α� ����
                strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
                ADD_COMM_LOG(strLog);
				return;
            }

			// Body+Tail�� ���� ���̿� �̹� ���� header�� ���̸� ����
			nTotalLenRead = LEN_HEADER + nRecvBytes;

			// �Ϸ���� üũ �� TLV ó��
            if((LEN_HEADER + nLenBytesToRead) != nTotalLenRead)
            {
				// ���� �α� ����
				strLog.sprintf(L"[Error] Different (LEN_HEADER + nLenBytesToRead)(%d) and nTotalLenRead(%d)", (LEN_HEADER + nLenBytesToRead), nTotalLenRead);
                ADD_COMM_LOG(strLog);
				return;
            }
            else
			{
                // Tail ����
                int Tail = (BYTE)RecvBuf[nTotalLenRead-4] |
							(RecvBuf[nTotalLenRead-3] << 8) |
                            (RecvBuf[nTotalLenRead-2] << 16) |
                            (RecvBuf[nTotalLenRead-1] << 24);

				// TLV�� Length�� Tail ��
                if(Tail != BodyLen)
				{
                    // ���� �α� ����
                    strLog.sprintf(L"[Error] Different Tail(%d) and BodyLen(%d)", (LEN_HEADER + nLenBytesToRead), nTotalLenRead);
					ADD_COMM_LOG(strLog);
                    return;
                }

                // by zzang9un 2013.10.10 : Data Manager ��� �α�
                AddTLVCommLog(STR_RECV, RecvBuf, nTotalLenRead);
			}

            // Step 4. ������ �Ϸ�� TLV�� ó���ϵ��� �Լ� ȣ��
			nError = HandleTPCmd(RecvBuf, Socket);

            // [END] by zzang9un 2013.10.15 : Socket ����(8192)���� ���� TLV�� ���
			// -------------------------------------------
        }
        // [END] by zzang9un 2013.10.15 : TLV�� ó�� �д� ���
		// -------------------------------------------
    }
    else
	{
		// -------------------------------------------
        // [BEGIN] by zzang9un 2013.10.15 : Big TLV�� ��� Read�ϴ� ���

        // �����ؾ� �� ���� Bytes�� ����Ѵ�.
        if((m_BigTLVTotalLen_TP - m_BigTLVReadCnt_TP) < LEN_TLV_BUFFER)
			nLenBytesToRead = m_BigTLVTotalLen_TP - m_BigTLVReadCnt_TP;
        else
            nLenBytesToRead = LEN_TLV_BUFFER;

		WaitCount = 0;
        while(1)
		{
            nRecvBytes = Socket->ReceiveLength();

			if(nRecvBytes >= nLenBytesToRead)
                break;

			if(++WaitCount > MAX_WAIT)
                break;

			Sleep(1);
		}

		// ���� ó��
		if(nRecvBytes < nLenBytesToRead)
		{
            InitBigTLV_TP();

			// ���� �α� ����
            strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
            ADD_COMM_LOG(strLog);

            return;
        }

        nRecvBytes = Socket->ReceiveBuf(pBigTLV_TP + m_BigTLVReadCnt_TP, nLenBytesToRead);

		// ���� ó��
        if(nRecvBytes < nLenBytesToRead)
        {
			InitBigTLV_TP();

			// ���� �α� ����
			strLog.sprintf(L"[Error] nRecvBytes(%d) Less than nLenBytesToRead(%d)", nRecvBytes, nLenBytesToRead);
            ADD_COMM_LOG(strLog);

			return;
        }

		// Read�� bytes �� �ݿ�
        m_BigTLVReadCnt_TP += nRecvBytes;

		// �Ϸ���� üũ �� TLV ó��
        if(m_BigTLVTotalLen_TP == m_BigTLVReadCnt_TP)
		{
			// Tail ����
            int Tail = (BYTE)pBigTLV_TP[m_BigTLVTotalLen_TP-4] |
                        (pBigTLV_TP[m_BigTLVTotalLen_TP-3] << 8) |
						(pBigTLV_TP[m_BigTLVTotalLen_TP-2] << 16) |
                        (pBigTLV_TP[m_BigTLVTotalLen_TP-1] << 24);

			// TLV�� Length�� Tail�� ���Ѵ�.
            if(Tail != m_PreCmdHeader_TP.Length)
            {
				delete [] pBigTLV_TP;
				memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
				bBigTLV_TP = false;
                return;
            }

            // by zzang9un 2013.10.10 : Data Manager ��� �α�
            AddTLVCommLog(STR_RECV, RecvBuf, nTotalLenRead);

            //ACKData = m_CmdManager->ParsingOCSCmd(RecvBuf, true);

			// Header�� ���� ó��
            nError = HandleTPCmd(pBigTLV_TP, Socket);

			// Cmd ó�� �Ŀ� ���� �Ҵ��� Buffer�� delete
            delete [] pBigTLV_TP;
            memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
			bBigTLV_TP = false;
		// [END] by zzang9un 2013.10.15 : Big TLV�� ��� Read�ϴ� ���
		// -------------------------------------------
		}
	}
}

void __fastcall TSocketForm::TPSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
	g_bIsTPServerConnected = true;
}

// ---------------------------------------------------------------------------
void __fastcall TSocketForm::TPSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	g_bIsTPServerConnected = false;
}

// ---------------------------------------------------------------------------
void __fastcall TSocketForm::TPSocketClientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;

}

// ---------------------------------------------------------------------------

/**
 @brief   Ack Data�� ���ۿ� encoding �ϴ� �Լ�
 @author  zzang9un
 @date    2013.04.11
 @param   SndACKData : ������ Ack Data
 @param   pSndBuf : ���� ����
 @return  ������ ������ bytes ��, ���� �� ���� �ڵ� ����
 */
int TSocketForm::MakeACK2TP(ACK_DATA SndACKData, char *pSndBuf)
{
    int Error = NO_ERR;
	int SndTag = SndACKData.AckTagData.Data;
    int SndLen = 0;

	memset(pSndBuf, 0x0, sizeof(LEN_TLV_BUFFER));

	// Header ����
	CMD_HEADER SndHeader;

	// by zzang9un 2013.01.25 : Length 6bytes�� ����
	SndHeader = MakeHeader(SndTag, 6); // Tag�� �������� �ʰ� Body�� ����Ѵ�.

	SndLen = EncodeCmdACK(pSndBuf, &SndHeader, &SndACKData);

	return SndLen;
}
// ---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
    m_bDMConnected = true;

	ADD_COMM_LOG("Connect Data Manager...");
}
//---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	m_bDMConnected = true;

	ADD_COMM_LOG("DisConnect Data Manager...");
}
//---------------------------------------------------------------------------

void __fastcall TSocketForm::DMSocketClientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
	String strErrTxt;

	/*
	eeGeneral   : The socket received an error message that does not fit into any of
				   the following categories.
	eeSend      : An error occurred when trying to write to the socket connection.
	eeReceive   : An error occurred when trying to read from the socket connection.
	eeConnect   : A client connection request that has already been accepted could
				   not be completed.
    eeDisconnect: An error occurred when trying to close a connection.
	eeAccept    : A problem occurred when trying to accept a client connection request.
	*/

	switch(ErrorCode)
	{
	case eeGeneral:
		strErrTxt="The socket received an error message that does not fit into any of the following categories.";
        break;

	case eeSend:
        strErrTxt="An error occurred when trying to write to the socket connection.";
        break;

    case eeReceive:
        strErrTxt="An error occurred when trying to read from the socket connection.";
		break;

    case eeConnect:
		strErrTxt="A client connection request that has already been accepted could not be completed.";
        break;

	case eeDisconnect:
        strErrTxt="An error occurred when trying to close a connection.";
        break;

	case eeAccept:
        strErrTxt="A problem occurred when trying to accept a client connection request.";
		break;

    default:
		break;
	}

	ADD_COMM_LOG(strErrTxt);

	// Reset error code
	ErrorCode = 0;
}
//---------------------------------------------------------------------------

/**
@brief	���� Read �̺�Ʈ ����
@remark	�ѹ��� ���� �� �ִ� ����Ʈ�� 8192bytes �� �� �̻��� TLV�� ���ŵǸ�
		�����͸� ������ �޾ƾ� ��. �̸� ó���ϱ� ���� �˰������� ����
@author	doori.shin
@date	2015.05.12
*/
void __fastcall TSocketForm::DMSocketClientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	int nError = 0;
	int nRecvLen = 0;							//Socket���� ������ ���� ������ ����
	char szRecvBuf[LEN_TLV_BUFFER] = {0,};      //Socket���� ������ �������� ����
	int nRecvBytes = Socket->ReceiveLength();	//Socket ���ۿ� �ִ� ������ ����
	int WaitCount = 0;
	AnsiString strLog = "";
	int nBreakPoint = 0;                        //while�� Ż���ϱ� ���� ����

	// Step 1. TVL�ޱ� �� ó�� �۾�
	//���� �����Ͱ� ���°�� ���۸� ���
	if(nRecvBytes == 0)
	{
		Socket->ReceiveBuf(szRecvBuf, nRecvBytes);

		if(m_TLV_DM != NULL)
		{
			m_TLV_DM->InitTLV();
			delete m_TLV_DM;
			m_TLV_DM = NULL;
        }

		return;
	}

	//Socket ���۾ȿ� �������� TLV�� ���� �� �����Ƿ� ������ ó��
	//nBreakPoint�� Ȥ�ø� ���ѷ����߻� �������� ����, �ִ� 8192�� �ݺ���
	while(nBreakPoint < LEN_TLV_BUFFER)
	{
		// Step 2. TLV����
		//ó�� �д� �������� ���
		if(m_TLV_DM == NULL)
		{
			//���� ������ ũ�Ⱑ ��� ����� ���̺��� ū�� Ȯ��
			while(1)
			{
				nRecvBytes = Socket->ReceiveLength();

				if(nRecvBytes >= LEN_HEADER)
					break;

				// ������ size�� Header���� ���� ��� ���
				if(++WaitCount > MAX_WAIT)
					break;

				Sleep(WAITTIME);
			}

			//��� ũ�Ⱑ ������ ���� Read�̺�Ʈ�� ó����
			if(nRecvBytes < LEN_HEADER)
			{
				strLog.sprintf("[Error] nRecvByte(%d) Less than LEN_HEADER(%d)", nRecvBytes, LEN_HEADER);
				ADD_COMM_LOG(strLog);

				if(m_TLV_DM != NULL)
				{
					m_TLV_DM->InitTLV();
					delete m_TLV_DM;
					m_TLV_DM = NULL;
				}

				return;
			}

			nRecvLen = Socket->ReceiveBuf(szRecvBuf, LEN_HEADER);

			//�������
			CMD_HEADER header = GetHeader(szRecvBuf, true);
			bool bIsBig = (header.Length > LEN_TLV_BUFFER) ? true : false;

			//TLV����
			m_TLV_DM = new TLV(header.Length + LEN_HEADER + LEN_TAIL, bIsBig);
			memcpy(&m_TLV_DM->m_Header, &header, sizeof(CMD_HEADER));
		}
		//��� �ް��ִ� �������� ���
		else
		{
			//������ ���� ������ �ܿ� �߰��� �����Ͱ� �ִ� ���
			if(nRecvBytes > m_TLV_DM->GetRemainingData())
			{
				//���� ���� ��ŭ�� �޴´�.
				nRecvLen = Socket->ReceiveBuf(szRecvBuf, m_TLV_DM->GetRemainingData());
			}
			//�ƴϸ� �� �޴´�
			else
			{
				nRecvLen = Socket->ReceiveBuf(szRecvBuf, nRecvBytes);
            }
		}

		//�����͸� �����ϰ� ������� ������ �����͸� ����
		int nReadBytes = m_TLV_DM->AppendTLVData(szRecvBuf, nRecvLen);

//		strLog.sprintf("Length : %d", nReadBytes);
//		ADD_COMM_LOG(strLog);

		// Step 3. TLVó��
		//������ ���������� Ȯ��
		if(m_TLV_DM->GetRemainingData() <= 0)
		{
			//TLVL ���� �� ������ Tail�� �޾ƿ�
			int nTail = GetBodyLength(m_TLV_DM->m_TLVBuffer + LEN_HEADER + m_TLV_DM->m_Header.Length, true);

			//Length�� Tail�� ������ Ȯ��
			if(nTail == m_TLV_DM->m_Header.Length)
			{
				//���� ���ŵ� �����ͱ��̿� Length�� ��
				if((nReadBytes - LEN_HEADER - LEN_TAIL) == m_TLV_DM->m_Header.Length)
				{
					//������ ó��
					nError = HandleDMCmd(m_TLV_DM->m_TLVBuffer, Socket);
				}
				else
				{
					strLog = "";
					strLog.sprintf("[Error] Packet length is different to Data Length..[nReadByte:%d, TLV_Length:%d",
									nReadBytes, m_TLV_DM->m_Header.Length);
					ADD_TEST_LOG(strLog);
				}

				//�ʱ�ȭ
				if(m_TLV_DM != NULL)
				{
					m_TLV_DM->InitTLV();
					delete m_TLV_DM;
					m_TLV_DM = NULL;
                }
			}
		}

		//Step 4. ���� ����(Socket ���ۿ� ���̻� �����Ͱ� ���°��),  �����Ͱ� ������ while�� �ݺ�
		if(Socket->ReceiveLength() <= 0)
		{
			if(m_TLV_DM != NULL)
			{
				//�ʱ�ȭ
				m_TLV_DM->InitTLV();
				delete m_TLV_DM;
				m_TLV_DM = NULL;
			}

			break; //End of while()
		}

		nBreakPoint++;

		//nBreakPoint�� 8192�� ������ �ִ� ���̴�. �ڵ� Ȯ�� �ʿ�
		if(nBreakPoint == LEN_TLV_BUFFER)
		{
			ADD_TEST_LOG("[Error] Fall in infinity loop.");
        }
	}
}


//---------------------------------------------------------------------------

int TSocketForm::HandleDMCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

	// ���� Command���� Header�� ����
	CMD_HEADER RecvHeader;
	memset(&RecvHeader, 0x00, sizeof(CMD_HEADER));

    // Data Manager�� Endian ��ȯ�� �ʿ���
	RecvHeader = GetHeader(pRecvBuf, true);

	// ���� ó��
	if(RecvHeader.uTag.Info.Sender != TAG_SENDER_DM)
	{
		ADD_COMM_LOG("[Error][HandleDMCmd]Sender is not DataManager!");
		return -1;
	}

	switch(RecvHeader.uTag.Info.CmdData)
	{
	case TAG_CMDDATA_VERSIONINFO:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_VERSIONINFO");

		// ���� ���� ����
		Error = SendDataVerionInfo(pRecvBuf, ClientSocket);
        break;

	case TAG_CMDDATA_DATA_DOWNLOAD:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_DOWNLOAD]");

		// Data File ����
		Error = SendDataFile(pRecvBuf, ClientSocket);
		break;

    case TAG_CMDDATA_DATA_BAKCUP:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_BAKCUP]");

		// Data File ����
        Error = SendBackupDataFile(pRecvBuf, ClientSocket);
		break;

	case TAG_CMDDATA_DATA_UPLOAD:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD]");

		// Data ���� ���� ��û
		Error = ReceiveDataFile(pRecvBuf);

        if(Error == NO_ERR)
			Error = SendACKDataUpload(pRecvBuf, ClientSocket);
		break;

	case TAG_CMDDATA_DATA_UPLOAD_MAP:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_MAP]");

		// Map ���� ����
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_MAP);
        break;

    case TAG_CMDDATA_DATA_UPLOAD_STATION:
        ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_STATION]");

        // Station ���� ����
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_STATION);
		break;

    case TAG_CMDDATA_DATA_UPLOAD_TEACHING:
		ADD_SOCKET_LOG("[HandleDMCmd][Rcv]TAG_CMDDATA_DATA_UPLOAD_TEACHING]");

        // Teaching ���� ����
		Error = MakeTLV2File(pRecvBuf, DATA_TYPE_TEACHING);
        break;

	default:
        ADD_SOCKET_LOG("[Error][HandleDMCmd]Unknown TAG_CMDDATA!!");
    }

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendDataVerionInfo(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

	// Step 1. Data Type Decode
    int Offset = LEN_HEADER;

	BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type�� ���� Version�� ������
    DATA_VERSION_INFO DataVerInfo_Map;
	DATA_VERSION_INFO DataVerInfo_Station;
    DATA_VERSION_INFO DataVerInfo_Teaching;

	FILE_HEADER FileHeader;
	MAP_FILE_HEADER MapFileHeader;

	String strFileName;

    switch(DataType)
	{
	case DATA_TYPE_ALL:
		// 1. Map
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
		Error = GetMapFileHeader(strFileName, &MapFileHeader);

		DataVerInfo_Map.DataType = DATA_TYPE_MAP;
		DataVerInfo_Map.ErrCode = Error;
		DataVerInfo_Map.UpdateTime = MapFileHeader.UpdateTime;
		DataVerInfo_Map.CurRevision = MapFileHeader.Revision;

        // 2. Station
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        Error = GetFileHeader(strFileName, &FileHeader);

		DataVerInfo_Station.DataType = DATA_TYPE_STATION;
        DataVerInfo_Station.ErrCode = Error;
        DataVerInfo_Station.UpdateTime = FileHeader.UpdateTime;
		DataVerInfo_Station.CurRevision = FileHeader.Revision;

        // 3. Teaching
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        Error = GetFileHeader(strFileName, &FileHeader);

		DataVerInfo_Teaching.DataType = DATA_TYPE_TEACHING;
        DataVerInfo_Teaching.ErrCode = Error;
        DataVerInfo_Teaching.UpdateTime = FileHeader.UpdateTime;
		DataVerInfo_Teaching.CurRevision = FileHeader.Revision;
        break;
    }

    // ���� ���� �ʱ�ȭ
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_VERSIONINFO;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATA_VERSIONINFO;

    int BodySize = LEN_ACK_DATA_VERSIONINFO;

	ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER; // Offset �ʱ�ȭ

	// Body ���ڵ�
    switch(DataType)
    {
	case DATA_TYPE_ALL:
        // 1. Map
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Map.CurRevision);

        // 2. Station
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Station.CurRevision);

        // 3. Teaching
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.DataType);
		EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.ErrCode);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.UpdateTime);
        EncodeBody(SndBuf, &Offset, &DataVerInfo_Teaching.CurRevision);
		break;
	}

	// Tail
    EncodeBody(SndBuf, &Offset, &BodySize);

	// ����
    ClientSocket->SendBuf(SndBuf, Offset);

	// by zzang9un 2013.10.10 : Data Manager ��� �α�
    AddTLVCommLog(STR_SEND, SndBuf, Offset);

	return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

    // Step 1. Data Type Decode
	int Offset = LEN_HEADER;

    BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

	// Step 2. Data Type�� Size�� ����
    int FileSize = 0;

	String strFileName;

    switch(DataType)
	{
    case DATA_TYPE_MAP:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
		FileSize = (int)GetLocalFileSize(strFileName);
        break;

	case DATA_TYPE_STATION:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        FileSize = (int)GetLocalFileSize(strFileName);
		break;

    case DATA_TYPE_TEACHING:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_ALL:
        break;
	}

    // -------------------------------------------
	// [BEGIN] by zzang9un 2013.10.11 : ���� ���� ����
    // ���� ���� �ʱ�ȭ
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_DOWNLOAD;

    int BodySize = LEN_ACK_DATAFILE_DOWNLOAD;
    int SndFileSize = FileSize;

    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
	memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body ���ڵ�
    EncodeBody(SndBuf, &Offset, &DataType);
	EncodeBody(SndBuf, &Offset, &SndFileSize);

    // Tail ���ڵ�
	EncodeBody(SndBuf, &Offset, &BodySize);

    // ACK_DATAFILE_DOWNLOAD_AST_DM ��� ����
	ClientSocket->SendBuf(SndBuf, Offset);

    // ��� �α� ����
	AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : ���� ���� ����
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : ���� ����

    // Header ����  �ʱ�ȭ
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    // Tag ����
    SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_REQ;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;

    switch(DataType)
    {
	case DATA_TYPE_MAP:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_MAP;
        break;

    case DATA_TYPE_STATION:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_STATION;
		break;

    case DATA_TYPE_TEACHING:
		SndTag.Info.CmdData = TAG_CMDDATA_DATA_DOWNLOAD_TEACHING;
        break;
    }

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = FileSize;    // �����ϰ��� �ϴ� File Size
	BodySize = FileSize;

    // ���� ������ ���� ���� ����
	char *SndFileBuf;
    int SizeOfSndFileBuf = LEN_HEADER + FileSize + LEN_TAIL;
    SndFileBuf = new char[SizeOfSndFileBuf];

    // Header ���ڵ�
    ConvertEndian(&SndHeader.uTag.Data);
	ConvertEndian(&SndHeader.Length);
    memcpy(SndFileBuf, &SndHeader, LEN_HEADER);

	// ���ۿ� ���� ���� ���ڵ�
    Error = MakeFile2TLV(SndFileBuf, strFileName, FileSize);

	// Tail ���ڵ�
    Offset = LEN_HEADER + BodySize;
    EncodeBody(SndFileBuf, &Offset, &BodySize);

    // ���� ����
    ClientSocket->SendBuf(SndFileBuf, SizeOfSndFileBuf);

    // ��� �α� ����
    String strLog;
	strLog.sprintf(L"[SendDataFile] File:%s, Size:%dBytes", strFileName.c_str(), FileSize);
    ADD_SOCKET_LOG(strLog);

	AddTLVCommLog(STR_SEND, SndFileBuf, SizeOfSndFileBuf);

	delete[] SndFileBuf;
	// [END] by zzang9un 2013.10.11 : ���� ����
    // -------------------------------------------
}
//---------------------------------------------------------------------------

int TSocketForm::MakeFile2TLV(char *pBuf, String strFileName, int FileSize)
{
    setlocale(LC_ALL, "Korean");

	int Error = NO_ERR;

    ifstream fs;

    // File Open
    fs.open(strFileName.w_str(), ios::in | ios::binary);

    if(fs.is_open())
    {
		fs.read((char *)pBuf + LEN_HEADER, FileSize);
    }
	else
    {
        Error = ERR_FILE_OPEN;
	}

    fs.close();

	return Error;
}
//---------------------------------------------------------------------------

void TSocketForm::AddTLVCommLog(AnsiString strTitle, char *pBuf, int size)
{
	AnsiString strTmp;
	AnsiString strLog = strTitle;

	for(int i = 0; i < size; i++)
	{
		strTmp.sprintf("%02X", (BYTE)pBuf[i]);
		strLog += strTmp;
	}
	ADD_COMM_LOG(strLog);
}
//---------------------------------------------------------------------------

int TSocketForm::ReceiveDataFile(char *pRecvBuf)
{
	int Error = NO_ERR;

	// Step 1. Data Type Decode
	int Offset = LEN_HEADER;

	BYTE DataType;

	DecodeBody(pRecvBuf, &Offset, &DataType);
	DecodeBody(pRecvBuf, &Offset, &m_FileSize);

	String strLog;
	strLog.sprintf(L"[ReceiveDataFile] DataType:%d, FileSize:%dBytes", DataType, m_FileSize);
	ADD_SOCKET_LOG(strLog);

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::MakeTLV2File(char *pBuf, BYTE DataType)
{
    setlocale(LC_ALL, "Korean");

	int Error = NO_ERR;

    ofstream fs;

    // ���� �̸� ����
    String strFileName;

	switch(DataType)
    {
	case DATA_TYPE_MAP:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_MAP;
		break;

    case DATA_TYPE_STATION:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_STATION;
		break;

    case DATA_TYPE_TEACHING:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_UPDATE + FILENAME_TEACHING;
        break;
    }

    // File Open
    fs.open(strFileName.w_str(), ios::out | ios::binary);

	if(fs.is_open())
	{
		fs.write((char *)pBuf + LEN_HEADER, m_FileSize);
	}
    else
	{
		Error = ERR_FILE_OPEN;
    }

	fs.close();

    return Error;
}
//---------------------------------------------------------------------------
int TSocketForm::SendACKDataUpload(char * pRecvBuf, TCustomWinSocket *ClientSocket)
{
	int Error = NO_ERR;

	// Step 1. Data Type Decode
	int Offset = LEN_HEADER;

	BYTE DataType;

	DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type�� Size�� ����
//	int FileSize = 0;
//
//	String strFileName;

//    switch(DataType)
//    {
//    case DATA_TYPE_MAP:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_STATION:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_TEACHING:
//        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
//        FileSize = (int)GetLocalFileSize(strFileName);
//        break;
//
//    case DATA_TYPE_ALL:
//        break;
//    }

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : ���� ���� ����
    // ���� ���� �ʱ�ȭ
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag ����
    uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_UPLOAD;

	CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_UPLOAD;

    int BodySize = LEN_ACK_DATAFILE_UPLOAD;
//    int SndFileSize = FileSize;

	ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body ���ڵ�
    EncodeBody(SndBuf, &Offset, &DataType);

    // Tail ���ڵ�
    EncodeBody(SndBuf, &Offset, &BodySize);

	// ACK_DATAFILE_DOWNLOAD_AST_DM ��� ����
    ClientSocket->SendBuf(SndBuf, Offset);

    // ��� �α� ����
    AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : ���� ���� ����
    // -------------------------------------------

    return Error;
}
//---------------------------------------------------------------------------

int TSocketForm::SendBackupDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

    // Step 1. Data Type Decode
    int Offset = LEN_HEADER;

    BYTE DataType;

    DecodeBody(pRecvBuf, &Offset, &DataType);

    // Step 2. Data Type�� Size�� ����
	int FileSize = 0;

    String strFileName;

    switch(DataType)
    {
    case DATA_TYPE_MAP:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_MAP;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_STATION:
		strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_STATION;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_TEACHING:
        strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_TEACHING;
        FileSize = (int)GetLocalFileSize(strFileName);
        break;

    case DATA_TYPE_ALL:
        break;
    }

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : ���� ���� ����
    // ���� ���� �ʱ�ȭ
    char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

    // Tag ����
    uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;
    SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP;

    CMD_HEADER SndHeader;
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = LEN_ACK_DATAFILE_BACKUP;

	int BodySize = LEN_ACK_DATAFILE_BACKUP;
    int SndFileSize = FileSize;

    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndBuf, &SndHeader, LEN_HEADER);

    Offset = LEN_HEADER;

    // Body ���ڵ�
    EncodeBody(SndBuf, &Offset, &DataType);
    EncodeBody(SndBuf, &Offset, &SndFileSize);

    // Tail ���ڵ�
    EncodeBody(SndBuf, &Offset, &BodySize);

    // ACK_DATAFILE_DOWNLOAD_AST_DM ��� ����
    ClientSocket->SendBuf(SndBuf, Offset);

    // ��� �α� ����
    AddTLVCommLog(STR_SEND, SndBuf, Offset);
    // [END] by zzang9un 2013.10.11 : ���� ���� ����
    // -------------------------------------------

    // -------------------------------------------
    // [BEGIN] by zzang9un 2013.10.11 : ���� ����

    // Header ����  �ʱ�ȭ
    memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	// Tag ����
    SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
    SndTag.Info.Receiver = TAG_RECEIVER_DM;
    SndTag.Info.ProtoType = TAG_PROTOTYPE_REQ;
    SndTag.Info.CmdType = TAG_CMDTYPE_DM_CMD;

    switch(DataType)
    {
    case DATA_TYPE_MAP:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_MAP;
        break;

    case DATA_TYPE_STATION:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_STATION;
        break;

    case DATA_TYPE_TEACHING:
        SndTag.Info.CmdData = TAG_CMDDATA_DATA_BAKCUP_TEACHING;
        break;
    }

    SndHeader.uTag.Data = SndTag.Data;
    SndHeader.Length = FileSize;    // �����ϰ��� �ϴ� File Size
    BodySize = FileSize;

    // ���� ������ ���� ���� ����
    char *SndFileBuf;
    int SizeOfSndFileBuf = LEN_HEADER + FileSize + LEN_TAIL;
    SndFileBuf = new char[SizeOfSndFileBuf];

    // Header ���ڵ�
    ConvertEndian(&SndHeader.uTag.Data);
    ConvertEndian(&SndHeader.Length);
    memcpy(SndFileBuf, &SndHeader, LEN_HEADER);

    // ���ۿ� ���� ���� ���ڵ�
    Error = MakeFile2TLV(SndFileBuf, strFileName, FileSize);

    // Tail ���ڵ�
    Offset = LEN_HEADER + BodySize;
    EncodeBody(SndFileBuf, &Offset, &BodySize);

	// ���� ����
    ClientSocket->SendBuf(SndFileBuf, SizeOfSndFileBuf);

    // ��� �α� ����
    String strLog;
    strLog.sprintf(L"[SendDataFile] File:%s, Size:%dBytes", strFileName.c_str(), FileSize);
    ADD_SOCKET_LOG(strLog);

	AddTLVCommLog(STR_SEND, SndFileBuf, SizeOfSndFileBuf);

	delete[] SndFileBuf;
	// [END] by zzang9un 2013.10.11 : ���� ����
    // -------------------------------------------
}
//---------------------------------------------------------------------------

/**
@brief   Bit TLV ���� ������ �ʱ�ȭ ��Ű�� �Լ�
@author  zzang9un
@date    2013.10.15
*/
//����� ����
void TSocketForm::InitBigTLV_DM()
{
	delete [] pBigTLV_DM;
	m_BigTLVTotalLen_DM = 0;
	m_BigTLVReadCnt_DM = 0;
	memset(&m_PreCmdHeader_DM, 0x00, sizeof(CMD_HEADER));
	bBigTLV_DM = false;
}
//---------------------------------------------------------------------------

/**
@brief   Bit TLV ���� ������ �ʱ�ȭ ��Ű�� �Լ�
@author  zzang9un
@date    2013.10.15
*/
//����� ����
void TSocketForm::InitBigTLV_TP()
{
    delete [] pBigTLV_TP;
    m_BigTLVTotalLen_TP = 0;
    m_BigTLVReadCnt_TP = 0;
    memset(&m_PreCmdHeader_TP, 0x00, sizeof(CMD_HEADER));
    bBigTLV_TP = false;
}
//---------------------------------------------------------------------------

/**
@brief   TP���� �� Command�� Handle�ϴ� �Լ�
@author  zzang9un
@date    2013.11.07
*/
int TSocketForm::HandleTPCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket)
{
    int Error = NO_ERR;

    // ���� Command���� Header�� ����
    CMD_HEADER RecvHeader;
    memset(&RecvHeader, 0x00, sizeof(CMD_HEADER));

    // Data Manager�� Endian ��ȯ�� �ʿ���
	RecvHeader = GetHeader(pRecvBuf);

    // ���� ó��
	if(RecvHeader.uTag.Info.Sender != TAG_SENDER_TP)
    {
        ADD_SOCKET_LOG("[Error][HandleTPCmd]Sender is not TP!");
        return -1;
    }

    switch(RecvHeader.uTag.Info.CmdData)
    {
    case TPCMD_AST_OHT_MAINPROG_EXEC:
        ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_MAINPROG_EXEC");

		FileControl::getInstance().ExecOHTProgram();
        break;

    case TPCMD_AST_OHT_MAINPROG_CLOSE:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_MAINPROG_CLOSE");
		SendCloseOHTProgram();
		break;

	case TPCMD_AST_OHT_REQ_FBWF_STATUS:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_STATUS");
		SendFBWFStatus(FBWF_CURRENT_STATUS);
		break;
	case TPCMD_AST_OHT_REQ_FBWF_ENABLE:
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_ENABLE");
		SendFBWFStatus(FBWF_SET_ENABLE);
		break;
	case TPCMD_AST_OHT_REQ_FBWF_DISABLE:
		SendFBWFStatus(FBWF_SET_DISABLE);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_FBWF_DISABLE");
		break;
	case TPCMD_AST_OHT_REQ_LOGDELETER_EXIST:
		SendFileExists(FILENAME_LOGDELETER);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_AST_OHT_REQ_LOGDELETER_EXIST");
		break;
	case TPCMD_OHT_TIMESETTING :
	{
		uTP_CMD data;
		Decode(pRecvBuf, &RecvHeader, &data, false);
		SendCurrentTime(data);
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]TPCMD_OHT_TIMESETTING");
	}
		break;
	case TPCMD_OHT_ADDITIONAL_STATUS :
		SendOHTAdditionalStatus();
		ADD_SOCKET_LOG("[HandleTPCmd][Rcv]REQ_TP_AST_ADDITIONAL_STATUS");
		break;
    default:
		ADD_SOCKET_LOG("[Error][HandleTPCmd]Unknown TAG_CMDDATA!!");
    }

	return Error;
}


/**
@brief      ���� ������ FBWF���¸� TP�� ����
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendFBWFStatus(String cmd)
{
	int error = NO_ERR;
	int status;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// ���� ���� �ʱ�ȭ
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_ACK_FBWF_STATUS;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	sndData->TPCmd.CmdID = AssistantMainForm->GetFBWFStatus();

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}

//---------------------------------------------------------------------------
/**
@brief		������ �����ϴ��� ���θ� TP�� ����(LogDeleter.bat)
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendFileExists(String strFileName)
{
	int error = NO_ERR;
	int status;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// ���� ���� �ʱ�ȭ
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_ACK_LOGDELETER;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	if(MonitoringUtil::getInstance().isFileExist(strFileName))
		sndData->TPCmd.CmdID = 0x01;
	else
		sndData->TPCmd.CmdID = 0x00;

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}

/**
@brief      TP���� OHT �ý����� �ΰ����� ������ ����
@author  	doori.shin
@date    	2013.12.31
@remark		�ΰ����� : ��ũ ����, ���α׷� �� ���� ���翩��, ���α׷� ���� ��
*/
void TSocketForm::SendOHTAdditionalStatus()
{
	int error = NO_ERR;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// ���� ���� �ʱ�ȭ
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_DATA;
	SndTag.Info.CmdData = TAG_CMDDATA_DATA_OHT_STATUS;

	//Step 2. Make Data
	OHT_ADDITIONAL_INFO* additionalInfo = new OHT_ADDITIONAL_INFO;
	memset(additionalInfo, 0x00, sizeof(OHT_ADDITIONAL_INFO));

	//1) FBWF Status
	additionalInfo->FBWFStatus = AssistantMainForm->GetFBWFStatus();

	//2) LogDeleter.bat ���� ���翩��
	additionalInfo->LogDeleterStatus = MonitoringUtil::getInstance().isFileExist(FILENAME_LOGDELETER) ? 0x01 : 0x00;

//	//3) Log Drive(D:\) ���翩��
//	additionalInfo->LogDriveStatus = monitoringUtil->GetDiskInfo(4)->TotalSpace == -1 ? 0x00 : 0x01;

	//4) C:\ ����
	DISK_INFO* diskCInfo = MonitoringUtil::getInstance().GetDiskInfo(3);
	memcpy(&additionalInfo->SystemDiskInfo, diskCInfo, sizeof(DISK_INFO));
	delete diskCInfo;

	//5) D:\ ����
	DISK_INFO* diskDInfo = MonitoringUtil::getInstance().GetDiskInfo(4);
	memcpy(&additionalInfo->LogDiskInfo, diskDInfo, sizeof(DISK_INFO));
	delete diskDInfo;

	//5) OHT ���� �� �������翩��
	OHT_VERSION* version = MonitoringUtil::getInstance().GetOHTMainProgramVersion();
	memcpy(&additionalInfo->OHTVersion, version, sizeof(OHT_VERSION));
	delete version;

	//Step 3. Encoding
	Offset = Encode(SndBuf, SndTag.Data, additionalInfo, false);
	delete additionalInfo;

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);
}
//---------------------------------------------------------------------------
/**
@brief      OHT �������α׷��� �����Ų ����� ����
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendCloseOHTProgram()
{
	int error = NO_ERR;

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// ���� ���� �ʱ�ȭ
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_AST_OHT_MAINPROG_CLOSE;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	uTP_CMD *sndData = new uTP_CMD;
	memset(sndData, 0, sizeof(uTP_CMD));

	DWORD pid = ProcessTerminator::getInstance().FindPID(PROC_NAME);
	sndData->TPCmd.CmdID = ProcessTerminator::getInstance().TerminateApp(pid, 1000);

	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, sndData, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);

	delete sndData;
}
//---------------------------------------------------------------------------

/**
@brief		���� ������ OHT�ý����� �ð��� TP�� ����
@author  	doori.shin
@date    	2013.12.31
*/
void TSocketForm::SendCurrentTime(uTP_CMD& data)
{
	//Set the time received TP
	if(data.TPCmd.Data1 != 0 && data.TPCmd.Data2 != 0)
	{
		String strDate;
		strDate.sprintf(L"%08d%06d", data.TPCmd.Data1, data.TPCmd.Data2);
		MonitoringUtil::getInstance().SetCurrentTime(strDate);
	}

	// Step 1. Make header
	int Offset = LEN_HEADER;

	// ���� ���� �ʱ�ȭ
	char SndBuf[LEN_TLV_BUFFER];
	memset(SndBuf, 0x00, LEN_TLV_BUFFER);

	// Tag ����
	uTAG_DATA SndTag;
	SndTag.Info.Sender = TAG_SENDER_ASSISTANT;
	SndTag.Info.Receiver = TAG_RECEIVER_TP;
	SndTag.Info.ProtoType = TAG_PROTOTYPE_ACK;
	SndTag.Info.CmdType = TAG_CMDTYPE_TPCMD;
	SndTag.Info.CmdData = TPRET_OHT_TIMESETTING;

	CMD_HEADER SndHeader;
	memset(&SndHeader, 0x00, sizeof(CMD_HEADER));

	SndHeader.uTag.Data = SndTag.Data;
	SndHeader.Length = sizeof(uTP_CMD);

	// Step 2. Make value
	struct time curTime;
	gettime(&curTime);
	struct date curDate;
	getdate(&curDate);

	data.TPCmd.Data1 = curDate.da_year*10000 + curDate.da_mon*100 + curDate.da_day;
	data.TPCmd.Data2 = curTime.ti_hour*10000 + curTime.ti_min*100 + curTime.ti_sec;


	// Step 3. Encoding
	Offset = EncodeTPCmd(SndBuf, &SndHeader, &data, false);

	// Step 4. Transferring
	if(m_ServerSocketTP->Socket->ActiveConnections != 0)
		m_ServerSocketTP->Socket->Connections[0]->SendBuf(SndBuf, Offset);
}
void __fastcall TSocketForm::FormDestroy(TObject *Sender)
{
	delete m_Log;
}

