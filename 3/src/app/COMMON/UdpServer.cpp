//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpServer.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef IS_REAL_OHT
	#define  MAX_BYTE_SIZE_OF_UDP_LOG  1024*10
	#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)
#endif


/*
SOCKET sock;
bool bListenMode;
*/

UdpServer::UdpServer()
{
#ifdef IS_REAL_OHT
	pLogUnit = getLogUnit("D:\\log\\UDPServer.log", MAX_BYTE_SIZE_OF_UDP_LOG);
#endif

	bListenMode = false;
}

// ���ϰ��� ���� ������� ���������� ��Ÿ��
// true�� ��� ������
bool UdpServer::ListenMode()
{
	return bListenMode;
}

// bind�� �õ��ϰ�, �����ϸ� true�� �����Ѵ�
int UdpServer::Bind(int iPort)
{
	int retval;

	if (bListenMode == true)
		return -3;	// �̹� ���ӵǾ� �ִ� ���´ϱ� �ٷ� �����Ѵ�.

#if 0
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
#endif

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
		return -1;

	// ����ŷ
	u_long on = 1;
	retval = ioctlsocket(sock, FIONBIO, &on);
	if(retval == SOCKET_ERROR)
		return -2;

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(iPort);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR)
		return -2;

	bListenMode = true;		// ������ ���� ���� ����

	return retval;
}

// ���Ͽ� ���� ���� �ֳ� Ȯ���ϰ�, ������ ���� ����Ʈ���� �����Ѵ�.
// ���� : UDP�̹Ƿ�, �ѹ��� �ϳ��� ��Ŷ�� �����ش�.
//        ���� �ѹ� �о��� �ؼ�, ��� ������ �������� �ƴϴ�. (���۰� �ִ�)
int  UdpServer::Listen(char* buf, int& iLen, String& strClientAddr, int& iClientPort)
{
	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	AnsiString tmpA;
	//char buf[MAX_PACKET_SIZE+1];

	// ������ �ޱ�
	addrlen = sizeof(clientaddr);
	retval = recvfrom(sock, buf, MAX_BIG_PACKET_SIZE, 0,
		(SOCKADDR *)&clientaddr, &addrlen);
	if(retval == SOCKET_ERROR){
		iLen = 0; 	// ������ byte���� 0�Դϴ�.
		return -2;
	}
	else
	{
		iLen = retval;
	}

	// �������� ���� ������ ����Ҷ� ����ϰ� ǥ�õǵ��� ��
   	buf[retval] = '\0';

   //	Sleep(70);
	// clientaddr.sin_addr.S_un.S_un_b.s_b1
	tmpA.sprintf("%s", inet_ntoa(clientaddr.sin_addr));
	strClientAddr = tmpA;

//	ADD_LOG("Listen Succec %s",tmpA);
	iClientPort = ntohs(clientaddr.sin_port);

	// ------------------------------------------
	//   �� �κ��� ���� ��� �۽��ϴ� ���, �ʿ��ϸ� �־ ����
	//   ��, �� �κ��� ������ ���ǰ� �ʿ��ϴ�. ������ �������� �ʴ�
	// 	 ����� ó���� �ݵ�� �̷������ �ϸ�,
	//   �̸� ���� ��� ���ϰ� Ÿ�Ӿƿ��� �߰��Ǿ�� �Ѵ�.
	// ------------------------------------------
	/*
	retval = sendto(sock, buf, retval, 0,
		(SOCKADDR *)&clientaddr, sizeof(clientaddr));

	if(retval == SOCKET_ERROR){
		return -1;
	}
	*/

	return retval;
}

int  UdpServer::Close()
{
	// closesocket()
	closesocket(sock);

#if 0
	// ���� ����
	WSACleanup();
#endif

	bListenMode = false;

	return 0;
}

String UdpServer::err_display(char *msg)
{
	AnsiString strMsg;

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	strMsg.sprintf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);

	return strMsg;
}