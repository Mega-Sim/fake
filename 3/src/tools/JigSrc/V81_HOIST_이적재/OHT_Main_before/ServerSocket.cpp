/**
@file    ServerSocket.cpp
@version OHT 7.0
@brief   ServerSocket Class CPP File
*/
// ---------------------------------------------------------------------------
#include "ServerSocket.h"


// -------------------------------------------
// Debugging�� Define
// -------------------------------------------


//#define TCP_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\TCP.log", __VA_ARGS__)
//����
#define TCP_LOG(...)    WRITE_LOG_UNIT(pLogTCP, __VA_ARGS__)


// ---------------------------------------------------------------------------
/**
@brief   ServerSocket ������
@author  puting
@date    2013.05.03
*/
ServerSocket::ServerSocket()//:ThreadCtrl()
{
	pLogTCP = getLogUnit("D:\\log\\TCP.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
    WSADATA wsaData; ///WSA Data : ���۶� ���
	//1.ũ��Ƽ�� ���� ����
	InitializeCriticalSection(&m_CS);

	EventTotal = 0;
    nPortNum = 0;
	hServSock = NULL;
	m_clientData = NULL;

	m_TempclientData = NULL;

	m_ClientList =NULL;
	m_PacketList = NULL;
	nClientCount =1;   //Server 1��

    m_ClientList = new LList();
	m_PacketList = new LList();
	BufferPointer = NULL;
	nRestLenth = 0;

	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		TCP_LOG("[Server]WSAStartup Fail");
		return;
	}


}
//--------------------------------------------------------------------------
/**
@brief   ServerSocket �Ҹ���
@author  puting
@date    2013.05.03
*/
ServerSocket::~ServerSocket()
{

	SocketClose();

	WSACleanup();  // ��ü ������ ����ǹǷ� Check �ʿ�


	DeleteCriticalSection(&m_CS);

	for(int i=0; i<m_ClientList->getCount(); i++)
	{
		CLIENT_SOCK* deleteSock = (CLIENT_SOCK*)m_ClientList->refer(i);
		if(deleteSock != NULL) delete deleteSock;
	}

	delete m_ClientList;
	delete m_PacketList;

	if(BufferPointer !=NULL)
	{
		delete[] BufferPointer;
		BufferPointer = NULL;
		nRestLenth =0;
	}

	m_ClientList = NULL;
	m_PacketList = NULL;
}
//--------------------------------------------------------------------------
/**
@brief   ServerSocket Open �Լ�
@date    2013.05.03
@author  puting
@param   PortNum�� �Ű������� �޾� ���������� ����(���� ���ῡ ���� �ٽ� �ʱ�ȭ �� ȣ���ϱ� ����)
@return  ������ ��� true, ���� �߻��� ��� false ����
*/
bool ServerSocket::SocketOpen(int PortNum)
{

	unsigned long mode = 1;

	if(hServSock==NULL)
	{
		/////�񵿱� ������ ���////

		SOCKADDR_IN	servAdr;



		/////////////////////////////////////////////////////////////////////////////
//		hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

//		for(int i=0; i<2; i++)
//		{
//		   _beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
//		}
//////////////////////////////////////////////////////////////////////////////

		hServSock = socket(AF_INET, SOCK_STREAM, 0);

	   //	ioctlsocket(hServSock, FIONBIO, &mode); // for non blocking mode socket

		// ���� ũ�� ���� //

	 // int optlen = sizeof(optval);
	 // getsockopt(hServSock,SOL_SOCKET,SO_SNDBUF,(char*)&optval,&optlen);
//		optval = MAXLINE;
//		setsockopt(hServSock, SOL_SOCKET, SO_SNDBUF, (char*)&optval,sizeof(optval));
		/////////

		// ������ ����
		memset(&servAdr, 0, sizeof(servAdr));
		servAdr.sin_family = AF_INET;
		servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
		//servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servAdr.sin_port = htons(PortNum);
		nPortNum = PortNum;

		if(CreateSocketInfo(hServSock)== -1)
		{
			TCP_LOG("[Server]ServerCreateSocket Fail");
			return false;
		}

		 if(WSAEventSelect(hServSock, EventArray[EventTotal - 1], FD_ACCEPT|FD_CLOSE) == SOCKET_ERROR)
		{
			TCP_LOG("[Server]WSAEventSelect Fail");
			return false;
		}


		// Bind Error Check
		if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		{
			TCP_LOG("[Server]bind Fail");
			return false;
		}

		// Listen Error Check
		if(listen(hServSock, 5)==SOCKET_ERROR)   //�ι�° �Ű躯���� �����û ��� ť�� ũ�� ���� ����.
		{
			TCP_LOG("[Server]listen Fail");
			return false;
		}



		//waitforevent�� �ޱ����� �̺�Ʈ ����
	    //newEvent = WSACreateEvent();

	}
	return true;
}
//------------------------------------------------------------------------------
/**
@brief   ServerSocket Close �Լ�
@date    2013.05.03
@author  puting
@param   List �� Data buff ����/���� �� �̺�Ʈ����
@return  ������ ��� true, ���� �߻��� ��� false ����
*/
bool ServerSocket::SocketClose()
{

	int SocketCount = EventTotal;
	if(SocketCount > 0)
	{
		for(int Count = SocketCount; Count--;)
		{
			freeSocketInfo(Count);
		}
	}

	int TempCount = GetPacketCount();

	for(int j=0; j < TempCount;j++)
	{
		m_TempclientData = (LPCLIENT_DATA)m_PacketList->popFront();     // ťó�� ���

		for(int i=0; i<m_ClientList->getCount(); i++)
		{
			CLIENT_SOCK* tmpData = (CLIENT_SOCK*)m_ClientList->refer(i);

			//WSARecv�� ���� Ŭ���̾�Ʈ�� ���� �Ǿ��ִ� ����
			if(tmpData->hClientSock == m_TempclientData->hRcvSock)
			{
				freeSocketInfo(i);
				break;
			}
		}

		delete[] m_TempclientData->buf;
		delete m_TempclientData;
	}


	TCP_LOG("[Server]Socket Close");


}
//--------------------------------------------------------------------------
/**
@brief   Update �Լ�
@date    2013.05.03
@author  puting
@param   �ش� Updata �Լ��� �����忡 ���� �����Ͽ��� �Ѵ�.(Ŭ���̾�Ʈ ���� �� ������ �ޱ�)
*/
void ServerSocket::Update()
{

		SOCKET hClientSock;
		int eventIndex = 0;

		eventIndex = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, 0, FALSE);
		if(eventIndex == WSA_WAIT_FAILED)
		{
		   //	TCP_LOG("[Server]eventIndex is WSA_WAIT_FAILED");
			return;
    	}
		else if(eventIndex==INVALID_EVENT)
		{
		   //	TCP_LOG("[Server]eventIndex is INVALID_EVENT");
			return;
		}
		//�߻��� �̺�Ʈ ���� ����

		if(EventTotal >=eventIndex)
		{
			SOCKET eventSock = ((CLIENT_SOCK*)m_ClientList->refer(eventIndex - WSA_WAIT_EVENT_0))->hClientSock;
			if(WSAEnumNetworkEvents(eventSock, EventArray[eventIndex - WSA_WAIT_EVENT_0], &networkEvents) == SOCKET_ERROR)
			{
			  //	TCP_LOG("[Server]WSAEnumNetworkEvents fail");
				return;
			}

			/////////////////////////////�����/////////////////////////////////////////////
			if(networkEvents.lNetworkEvents & FD_ACCEPT)
			{
				if(networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				{
					TCP_LOG("[Server]networkEvents is ErrorCode : &d", networkEvents.iErrorCode[FD_ACCEPT_BIT]);
					return;
				}

				int nAddrSize = sizeof(m_CurClientInfo);
				if ((hClientSock = accept(eventSock, (SOCKADDR*)&m_CurClientInfo, &nAddrSize)) == INVALID_SOCKET)
				{
					TCP_LOG("[Server]accept is INVALID_SOCKET");
					return;
				}

				if(EventTotal > 10)
				{
					TCP_LOG("[Server]EventTotal is EventTotal Over(Max :10)");
					closesocket(hClientSock);
					return;
				}

				CreateSocketInfo(hClientSock, inet_ntoa(m_CurClientInfo.sin_addr));

				if(WSAEventSelect(hClientSock, EventArray[EventTotal -1], FD_READ|FD_CLOSE) == SOCKET_ERROR)
				{
					TCP_LOG("[Server]WSAEventSelect is SOCKET_ERROR");
					return;
				}
			}
			///////////////////////////////�д� �̺�Ʈ �߻� ��
			if(networkEvents.lNetworkEvents & FD_READ )
			{
				int recvBytes = 0;
				char buf[BUF_SIZE] = {0,};

				recvBytes = recv(eventSock, buf, BUF_SIZE, 0);
				if(recvBytes== 0 || recvBytes == SOCKETERROR)
				{
					int tempErrorCode = WSAGetLastError();
					TCP_LOG("[Server]recv is SOCKETERROR[recvBytes:%d][tempErrorCode:%d]",recvBytes,tempErrorCode);
					switch(tempErrorCode)
					{
						case WSAEWOULDBLOCK:
                         //non overlapped ���� : �� ���� ���Ͽ� ���� ���� �����Ͱ� ����, overlapped(:12) ���� : �ʹ� ���� ��ø ����� �䱸�� ����
						break;

						case WSAENOBUFS:
                        //�����ִ� ���۰����� ��� ������ ����� �� ����
						break;

						case WSAECONNABORTED:
							freeSocketInfo(eventIndex - WSA_WAIT_EVENT_0);
						//Ÿ�Ӿƿ� Ȥ�� ������ ���������� ���� ����Ʈ�������� ������ ������ ������.
							return;
						break;

						default:
						///@todo ���� �ڵ� ���� �ʿ�
						break;


					}

					//Error �߻�
				}

				EnterCriticalSection(&m_CS);
				int TempCount = GetPacketCount();
				LeaveCriticalSection(&m_CS);

				if(TempCount >= 0 && TempCount < 10)
				{
			/////////////////////////////////////////////////////////////////////////////////////
					if(recvBytes > 0 && recvBytes <= BUF_SIZE) // �����Ͱ� ������� �߰�
					{

						m_clientData = new  CLIENT_DATA;

						//������ ���� Ŭ���̾�Ʈ ������ �Է�
						m_clientData->hRcvSock = eventSock;
						memcpy(m_clientData->buf , buf, recvBytes);
//						ClientData.buf = (hRcvbInfo->DataBuf.buf);
						m_clientData->Length = recvBytes;
						m_PacketList->insertBack(m_clientData);       //ťó�� ���
							//ũ��Ƽ�ü��� ����
						// m_clientData =NULL;

					}else
					{
						TCP_LOG("[Server]recvBytes is Wrong : %d", recvBytes);
                    }
				/////////////////////////////////////////////////////////////////////////////////
			   }else
			   {
					TCP_LOG("[Server]PackCount is Wrong : %d", TempCount);
               }

			}

			if(networkEvents.lNetworkEvents & FD_CLOSE)
			{
				freeSocketInfo(eventIndex-WSA_WAIT_EVENT_0);
			}
		}

}
//--------------------------------------------------------------------------
/**
@brief   GetPacketCount �Լ�
@date    2013.05.03
@author  puting
@param   ���� �������� ����(�ִ� 10������ ����)
@return  ���� List�� ����� Data ���� return
*/
UINT ServerSocket::GetPacketCount()
{
	int Count = m_PacketList->getCount();
	return Count;
}
//--------------------------------------------------------------------------
/**
@brief   GetClientCount �Լ�
@date    2013.05.03
@author  puting
@param   Ŭ���̾�Ʈ�� ����(�ִ� 10������ ����)
@return  ���� List�� ����� Ŭ���̾�Ʈ ���� return
*/
UINT ServerSocket::GetClientCount()
{
	int Count = m_ClientList->getCount();
	return Count;  //����� ������ 1�̿�����. <<1��Ʈ�� 1���� ���Ḹ �ϱ⶧����>>
}
//--------------------------------------------------------------------------
/**
@brief   IsClientConnect �Լ�
@date    2013.05.24
@author  puting
@param   ���� ������ �ѹ��� true�� ���ֱ� ����
*/
bool ServerSocket::IsClientConnect()
{
	int nTempClientCount = GetClientCount();
	if(nTempClientCount > 1)
	{
		if(nClientCount < nTempClientCount)
		{
			nClientCount++;
			return true;
		}
	}
		return false;

}
//-----------------------------------------------------------------------------
/**
@brief   IsClientDisConnect �Լ�
@date    2013.05.24
@author  puting
@param   ���� ������ �ѹ��� true�� ���ֱ� ����
*/
bool ServerSocket::IsClientDisConnect()
{
	int nTempClientCount = GetClientCount();
  	if(nTempClientCount > 0)
	{
		if(nClientCount > nTempClientCount)
		{
			nClientCount--;
			return true;
		}

		return false;
	}
}
//---------------------------------------------------------------------------
/**
@brief   ReadData Check �Լ�
@date    2013.05.24
@author  puting
@param   Read Data Check
*/
bool ServerSocket::IsClientReadData()
{
	if(GetPacketCount()<=0)
		return false;
	else
		return true;
}
//---------------------------------------------------------------------------
/**
@brief   Client ������ Remove �Լ�
@date    2013.05.24
@author  puting
@param   Read Data Check
*/
void ServerSocket::ClientRemove(UINT Index)
{
	freeSocketInfo(Index+1);
}
//-----------------------------------------------------------------------
/**
@brief   Client ������ ��� �Լ�
@date    2013.05.24
@author  puting
@param   Get�Լ�
*/
SOCKET ServerSocket::GetClientInfo(UINT Index)
{
	return (SOCKET)((CLIENT_SOCK*)m_ClientList->refer(Index+1))->hClientSock;
}
//---------------------------------------------------------------------------
/**
@brief   Read �Լ�
@date    2013.05.03
@author  puting
@param   List�� ����� ���� �����͸� �д� �Լ�
@return  ������ ��� true, ���� �߻��� ��� false ����
*/
bool ServerSocket::Read(char* RcvBuf, int* Length, int* Count)	// ���� �������� ������ ���
{

	//m_packList�� ���� �����Ͱ� ������� �� �����͸� ���� Ŭ���̾�Ʈ ������ �Բ� �Ѱ��ش�.
	//����ü�� �־��༭ ������� Ŭ���̾�Ʈ ���������� ���۸� �Բ� �Ѱ��ش�.
	bool returnValue = false;

	EnterCriticalSection(&m_CS);
	int TempCount = GetPacketCount();
	LeaveCriticalSection(&m_CS);

	if(TempCount > 0)
	{

		m_TempclientData = (LPCLIENT_DATA)m_PacketList->popFront();     // ťó�� ���

		if(m_TempclientData->Length > 0 && m_TempclientData->Length < BUF_SIZE)
		{
			memcpy(RcvBuf, m_TempclientData->buf, m_TempclientData->Length);
			*Length = m_TempclientData->Length;
			*Count = TempCount;
		}
		else
		{
			TCP_LOG("[Server]ReadFail is Wrong : [Length:%d]", m_TempclientData->Length);
			memset(RcvBuf, 0x00, BUF_SIZE);
			*Length  = 0;
	  		*Count = 0;
        }

		delete m_TempclientData;
		m_TempclientData =NULL;


		returnValue = true;

	}
	else
	{
		TCP_LOG("[Server]ReadFail is Wrong : [TempCount:%d]", TempCount);
		memset(RcvBuf, 0x00, BUF_SIZE);
		*Length  = 0;
		*Count = 0;
		returnValue = false;
	}


	return returnValue;
}
//--------------------------------------------------------------------------
/**
@brief   Write �Լ�
@date    2013.05.03
@author  puting
@param   �ش� Ŭ���̾�Ʈ�� Data�� ����
@return  ������ ��� true, ���� �߻��� ��� false ����
*/
bool ServerSocket::Write(char* SendBuf, int Length, SOCKET ClientSock)
{

	// memset(overlapped,0,sizeof(WSAOVERLAPPED));
	// WSASend((SOCKET)m_ClientList->refer(Index), &(hbInfo->databuf), 1, &recvBytes, 0, overlapped, NULL);
	bool returnValue = false;
	int SendLen = 0;

	if(Length > 0)
	{
		SendLen = send(ClientSock, SendBuf, Length, 0);

		if(SendLen ==SOCKET_ERROR)
		{
			for(int i=0; i<m_ClientList->getCount(); i++)
			{
				SOCKET tmpSock = (SOCKET)((CLIENT_SOCK*)m_ClientList->refer(i))->hClientSock;
				if(tmpSock == ClientSock)
				{
					//WSARecv�� ���� Ŭ���̾�Ʈ�� ���� �Ǿ��ִ� ����
					freeSocketInfo(i);
					TCP_LOG("[Server]write is Wrong : [SendLen:%d][index:%d]", SendLen,i);
					break;
				}
			}

			return false;
		}
		else
			return true;
	}
	else
	{
		 TCP_LOG("[Server]write is Wrong : [Length:%d]", Length);
		 return false;                    //������������ ũ�Ⱑ ������� ����.
	}
}
//-------------------------------------------------------------------------------

/**
@brief   Socket�� ������
@remark	 IP������ ������ �ʿ䰡 ���� �� ���
@date    2014.11.12
@author  doori.shin
@param   ClientSock : ������ ����
@return  ������ ��� 1, ���� �߻��� ��� -1
*/
int ServerSocket::CreateSocketInfo(SOCKET ClientSock)
{
	return CreateSocketInfo(ClientSock, NULL);
}

/**
@brief   Socket�� ������
@remark	 IP������ �����ؾ��� �� ���
@date    2014.11.12
@author  doori.shin
@param   ClientSock : ������ ����
@param 	 ip : ������ IP
@return  ������ ��� 1, ���� �߻��� ��� -1
*/
int ServerSocket::CreateSocketInfo(SOCKET ClientSock, char* ip)
{
	if((EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		return -1;
	}

	CLIENT_SOCK* clientSock = new CLIENT_SOCK;
	clientSock->hClientSock = ClientSock;
	memset(clientSock->szIPAddress, 0, 20);
	if(ip != NULL)
		strcpy(clientSock->szIPAddress, ip);

	m_ClientList->insertBack((CLIENT_SOCK*)clientSock);
	EventTotal ++;
	return 1;
}
//-------------------------------------------------------------------------------
void ServerSocket::freeSocketInfo(int eventIndex)
{
//	closesocket((SOCKET)m_ClientList->pop(eventIndex));
	CLIENT_SOCK* clientSock = (CLIENT_SOCK*)m_ClientList->pop(eventIndex);
	closesocket((SOCKET)clientSock->hClientSock);
	delete clientSock;



	for(int i = eventIndex; i < EventTotal; i++)
    {
		EventArray[i] = EventArray[i+1];
	}
    EventTotal--;
}

char* ServerSocket::GetCurIPAddress()
{
	return inet_ntoa(m_CurClientInfo.sin_addr);
}

char* ServerSocket::GetIPAddress(UINT index)
{
	CLIENT_SOCK* clientSock = (CLIENT_SOCK*)m_ClientList->refer(index+1);

	return clientSock->szIPAddress;
}
