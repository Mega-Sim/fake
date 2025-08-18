//---------------------------------------------------------------------------


#pragma hdrstop

#include "TCPClient.h"
#include "LogHandler.h"	// Log �ۼ� ����

#define TCP_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\TCP.log", __VA_ARGS__)
//---------------------------------------------------------------------------
/**
@brief   TCPCLIENT ������
@author  puting
@date    2013.05.03
*/
TCPCLIENT::TCPCLIENT()
{
	Init();
}
/**
@brief   TCPCLIENT �Ҹ���
@author  puting
@date    2013.05.03
*/
TCPCLIENT::~TCPCLIENT()
{
	Close();

	WSACleanup();  // ��ü ������ ����ǹǷ� Check �ʿ�

	DeleteCriticalSection(&m_CS);

   	delete m_PacketList;
	m_PacketList = NULL;
}
//-----------------------------------------------------------------------------
/**
@brief   �ʱ�ȭ �Լ�
@author  puting
@date    2013.05.03
*/
void TCPCLIENT::Init()
{

	m_bConnected = false;

	////////////////////////////////////////////////
	WSADATA wsaData; ///WSA Data : ���۶� ���
	//1.ũ��Ƽ�� ���� ����
	InitializeCriticalSection(&m_CS);

	hSocket = NULL;

	m_TempclientData = NULL;
	m_clientData = NULL;

	m_PacketList = NULL;
	m_PacketList = new LList();

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		TCP_LOG("[Client]WSAStartup fail!!");
		return;
	}
    //////////////////////////////////////////
}
/**
@brief   Close �Լ�
@date    2013.05.03
@author  puting
*/
void TCPCLIENT::Close()
{

	freeSocketInfo();
    for(int j=0; j<GetPacketCount();j++)
	{
		m_TempclientData = (LPPER_IO_DATA)m_PacketList->popFront();     // ťó�� ���


		delete m_TempclientData;
	}

}
//-----------------------------------------------------------------------------
/**
@brief   Open �Լ�
@date    2013.05.03
@author  puting
@param   nPort : ������ Port Number
@return  ������ ��� 0, ���� �߻��� ��� -1 ����
*/
int TCPCLIENT::Open(AnsiString strIP, int nPort)
{

	if(hSocket==NULL)
	{
//		hSocket = socket(AF_INET,SOCK_STREAM,0);
		hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		memset(&ServAddr,0,sizeof(ServAddr));
		ServAddr.sin_family = AF_INET;
		ServAddr.sin_port = htons(nPort);

		//���� �ܺο��� ������ ���� ��� IP�� ���� �޴� �κ� �߰� �� �ʿ� puting
		ServAddr.sin_addr.s_addr = inet_addr(strIP.c_str());//"10.240.21.24");//


		unsigned long nonblock = 1;
		if(CreateSocketInfo(hSocket)== -1)
		//if(ioctlsocket(hSocket, FIONBIO, &nonblock) == SOCKET_ERROR)
		{
			TCP_LOG("[Client]CreateSocketInfo fail!!");
			return -1;
		}

		if(Connect() == -1)
		{
			TCP_LOG("[Client]Connect fail!!");
			return -1;
		}
		else
		{
			if(CheckConnected() == 0)
			{
				TCP_LOG("[Client]Connect OK!!");
				return 0;
			}
			else
			{
				TCP_LOG("[Client]Connect fail[%d]!!", CheckConnected());
				return -3;
			}
		}
	}
	else
	{
		TCP_LOG("Socket not NULL");
		return -2;
    }
}
//-----------------------------------------------------------------------------
/**
@brief   Connect �Լ�
@date    2013.07.03
@author  puting
@return  ������ ��� 0, ���� �߻��� ��� -1 ����
*/
int TCPCLIENT::Connect()
{
	int iResult = connect(hSocket, (SOCKADDR*)&ServAddr, sizeof(ServAddr));

	if(iResult == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Close();
			TCP_LOG("[Client]Connect_1 %d %d", WSAGetLastError(), iResult);
			return 0;
		}

        // ���� ���� ��(�񵿱� ����)
		TCP_LOG("[Client]Connect_4 %d %d", WSAGetLastError(), iResult);
		return 1;
	}
	else
	{
		m_bConnected = true;

		if(NewEvent != NULL)
		{
			if(WSAEventSelect(hSocket, NewEvent, FD_READ|FD_CONNECT|FD_CLOSE) == SOCKET_ERROR)
			{
				Close();
				TCP_LOG("[Client]Connect_2");
				return -1;
			}
		}

		TCP_LOG("[Client]Connect_3 %d", m_bConnected);
		return 0;
    }
}
/**
@brief   Update �Լ�
@date    2013.05.03
@author  puting
@param   �ش� Updata �Լ��� �����忡 ���� �����Ͽ��� �Ѵ�.(Ŭ���̾�Ʈ ���� �� ������ �ޱ�)
*/
void TCPCLIENT::Update()
{
	int ncheckError = 0;
   //	WSANETWORKEVENTS	NetworkEvents;

	WSAWaitForMultipleEvents(1, &NewEvent, FALSE, 0, FALSE);


	if(WSAEnumNetworkEvents(hSocket,
			   NewEvent, &NetworkEvents) == SOCKET_ERROR)
	{
		ncheckError = WSAGetLastError();
		TCP_LOG("[Client]Update nError:%d", ncheckError);
		return;
	}
	if(NetworkEvents.lNetworkEvents & FD_READ )
	{
		int recvBytes = 0;
		char buf[BUF_SIZE] = {0,};

		TCP_LOG("[Client]Update Read:%d", NetworkEvents.lNetworkEvents);

		recvBytes = recv(hSocket, buf, BUF_SIZE, 0);
		if(recvBytes == 0 || recvBytes == SOCKETERROR)
		{
			int nError = WSAGetLastError();
			TCP_LOG("[Client]recv is  fail! [recvBytes:%d][nError:%d]",recvBytes,nError);
			if(nError == WSAEWOULDBLOCK)
			{
				//non overlapped ���� : �� ���� ���Ͽ� ���� ���� �����Ͱ� ����, overlapped(:12) ���� : �ʹ� ���� ��ø ����� �䱸�� ����
			}
			else if(nError == WSAECONNRESET)
			{
				//������ ���� ȣ��Ʈ�� ���� �缳���Ǿ���.

				Close();
				return;
			}
			else if(nError == WSA_IO_PENDING)
			{
				//Overlapped ������ ���߿� �Ϸ�� ���̴�. ��ø ������ ���� �غ� �Ǿ�����, ��� �Ϸ���� �ʾ��� ���߻�
			}
			else if(nError == WSA_INVALID_HANDLE)
			{
				//������ �̺�Ʈ ��ü �ڵ��� �߸� �Ǿ���.
				Close();
				return;
			}
		}

		EnterCriticalSection(&m_CS);
		int TempCount = GetPacketCount();
		LeaveCriticalSection(&m_CS);

		if(TempCount >= 0 && TempCount < 10)
		{
			if(recvBytes > 0 && recvBytes <= BUF_SIZE) // �����Ͱ� ������� �߰�
			{

				m_clientData = new  PER_IO_DATA;

				//������ ���� Ŭ���̾�Ʈ ������ �Է�
			   //	m_clientData->hRcvSock =(SOCKET)m_ClientList->refer(eventIndex - WSA_WAIT_EVENT_0);
				memcpy(m_clientData->buffer , buf, recvBytes);
//						ClientData.buf = (hRcvbInfo->DataBuf.buf);
				m_clientData->Length = recvBytes;
				m_PacketList->insertBack(m_clientData);       //ťó�� ���
					//ũ��Ƽ�ü��� ����
				// m_clientData =NULL;

			}
			else
			{
				TCP_LOG("[Client]recvBytes is Wrong : %d", recvBytes);
			}
			/////////////////////////////////////////////////////////////////////////////////
		}
		else
		{
			TCP_LOG("[Client]PackCount is Wrong : %d", TempCount);
		}
	}

	if(NetworkEvents.lNetworkEvents & FD_CLOSE)
	{
		Close();
	}
}
int TCPCLIENT::CheckConnected()
{
	int nError = 0;
	int optLen = sizeof(int);

	if( getsockopt(hSocket, SOL_SOCKET, SO_ERROR, (char*)&nError, &optLen) == SOCKET_ERROR)
	{
		// ����� ���� ���µ� ���� �߻�
		// ������ ���� �߻����� errno ������ ��� Ȯ��
		// ���� ����
		TCP_LOG("[Client]CheckConnected %d", WSAGetLastError());
		return 1;
	}

	TCP_LOG("[Client]Check Connected Error %d", nError);

	if(nError != 0)
	{
		if(nError == WSAECONNREFUSED)
		{
			// ���� �źη� ���� ����
			// ���� ó��
			return 2;
		}
		else if(nError == WSAETIMEDOUT)
		{
			// ���� ��� �ð� �ʰ��� ���� ����
	        // ���� ó��
			return 3;
		}

		// ��Ÿ Error
		return 4;
	}

	m_bConnected = true;
	if(NewEvent != NULL)
	{
		if(WSAEventSelect(hSocket, NewEvent, FD_READ|FD_CONNECT|FD_CLOSE) == SOCKET_ERROR)
		{
			Close();
			TCP_LOG("[Client]Check Connected Socket Close");
			return -1;
		}
		return 0;
	}
	return -2;
}
/**
@brief   Read �Լ�
@date    2013.07.03
@author  puting
@param   szRecv : �ش� Ŭ���̾�Ʈ���� ���� Data
@return  Length
*/
int TCPCLIENT::Read(char* szRecv)
{
	int recvBytes = 0;

	EnterCriticalSection(&m_CS);
	int TempCount = GetPacketCount();
	LeaveCriticalSection(&m_CS);

	if(TempCount > 0)
	{

		m_TempclientData = (LPPER_IO_DATA)m_PacketList->popFront();     // ťó�� ���
		if(m_TempclientData->Length > 0 && m_TempclientData->Length < BUF_SIZE)
		{
			memcpy(szRecv, m_TempclientData->buffer, m_TempclientData->Length);
			recvBytes = m_TempclientData->Length;

		}
		else
		{
			TCP_LOG("[Client]ReadFail is Wrong : [Length:%d]", m_TempclientData->Length);
			memset(szRecv,0x00, sizeof(szRecv));
			recvBytes = 0;
        }

		delete m_TempclientData;
		m_TempclientData =NULL;

	}
	else
	{
//		TCP_LOG("[Client]ReadFail is Wrong : [TempCount:%d]", TempCount);
		memset(szRecv,0x00, sizeof(szRecv));
		recvBytes = 0;
	}

	return recvBytes;

}
/**
@brief   Send �Լ�
@date    2013.07.03
@author  puting
@param   szSend : �ش� Ŭ���̾�Ʈ�� �� Data
@param   strLen : �ش� Ŭ���̾�Ʈ�� �� Data�� ����
@return  ������ ��� 0, ���� �߻��� ��� -1 ����
*/
int TCPCLIENT::Send(char* szSend,int strLen)
{

	if(strLen <= 0)
	{
		TCP_LOG("[client]write is Wrong : [strLen:%d]", strLen);
		return -1;
	}
	if(send(hSocket,szSend,strLen,0) == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();


		if(nError == WSAEWOULDBLOCK)
		{
		//non overlapped ���� : �� ���� ���Ͽ� ���� ���� �����Ͱ� ����, overlapped(:12) ���� : �ʹ� ���� ��ø ����� �䱸�� ����
		}
		else if(nError == WSAECONNRESET)
		{
		//������ ���� ȣ��Ʈ�� ���� �缳���Ǿ���.
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}
		else if(nError == WSAECONNABORTED)
		{
		//Ÿ�Ӿƿ� Ȥ�� ������ ���������� ���� ����Ʈ�������� ������ ������ ������.
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}
		else if(nError == WSA_IO_PENDING)
		{
        //Overlapped ������ ���߿� �Ϸ�� ���̴�. ��ø ������ ���� �غ� �Ǿ�����, ��� �Ϸ���� �ʾ��� ���߻�
		}
		else if(nError == WSA_INVALID_HANDLE)
		{
			TCP_LOG("[client]write is Wrong : [strLen:%d][nError:%d]", strLen,nError);
			Close();
			return -1;
		}

	}
	return 0;

}
/**
@brief   IsConnect �Լ�
@date    2013.05.24
@author  puting
@param   Server�� ����Ǿ��� �� true, ���� ������ false
*/
bool TCPCLIENT::IsConnect()
{
	return m_bConnected;
}
//-----------------------------------------------------------------------------

int TCPCLIENT::CreateSocketInfo(SOCKET ClientSock)
{
	if((NewEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		return -1;
	}
	return 1;
}

//-----------------------------------------------------------------------------
void TCPCLIENT::freeSocketInfo()
{
	m_bConnected = false;

	closesocket(hSocket);
	hSocket= NULL;

	if(WSACloseEvent(NewEvent) == TRUE)
    {
		//���� ó��
	   	NewEvent = NULL;
	}
	else
	{
	   //���� ó��
	}
}
/**
@brief   GetPacketCount �Լ�
@date    2013.05.03
@author  puting
@param   ���� �������� ����(�ִ� 10������ ����)
@return  ���� List�� ����� Data ���� return
*/
UINT TCPCLIENT::GetPacketCount()
{
	int Count = m_PacketList->getCount();
	return Count;
}
/**
@brief   ReadData Check �Լ�
@date    2013.05.24
@author  puting
@param   Read Data Check

bool TCPCLIENT::IsReadData()
{
	if(GetPacketCount()<=0)
		return false;
	else
		return true;
}
*/

#pragma package(smart_init) 
