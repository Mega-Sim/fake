//---------------------------------------------------------------------------
#include <vcl.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#pragma hdrstop

#include "UdpUniCastSender.h"
#include "Protocol_Define_Modify.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

UdpUniCastSender::UdpUniCastSender()
{

}

int UdpUniCastSender::UnicastSendMessage(AnsiString strIpAddr, int iPort, AnsiString strMessage)
{
	char buf[MAX_PACKET_SIZE+1];
	int len;

	strcpy(buf, strMessage.c_str());
	len = strlen(buf);

	if(buf[len-1] == '\n')
		buf[len-1] = '\0';
	if(strlen(buf) == 0)
		return -1;

	return UnicastSendMessage(strIpAddr, iPort, buf, len);
}


int UdpUniCastSender::UnicastSendMessage(AnsiString strIpAddr, int iPort, char* buf, int len)
{
	int retval;
	char cIpAddr[16];

	strcpy(cIpAddr, strIpAddr.c_str());	// String���� ���� �ּҸ� ��ȯ

#if 0
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
#endif

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
		err_display("Broadcast Sender - Invalid_Socket");
		// err_quit("socket()");

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	// remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_addr.s_addr = inet_addr(cIpAddr);
	remoteaddr.sin_port = htons(iPort);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
//	int addrlen;

	retval = sendto(sock, buf, len, 0,
		(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));

	if(retval == SOCKET_ERROR){
//		return -1;
		retval = -1;
		// err_display("Broadcast Sender Error - Send to()");
	}

	// ------------------------------------------
	//   �� �κ��� �۽� ��� �����ϴ� ���, �ʿ��ϸ� �־ ����
	//   ��, �� �κ��� ������ ���ǰ� �ʿ��ϴ�. ������ �������� �ʴ�
	// 	 ����� ó���� �ݵ�� �̷������ �ϸ�,
	//   �̸� ���� ��� ���ϰ� Ÿ�Ӿƿ��� �߰��Ǿ�� �Ѵ�.
	// ------------------------------------------
		// ������ �ޱ�
	/*
		char Receive[MAX_PACKET_SIZE+1];
		int  iRtn = 0;
		AnsiString strRtnMsg;
		addrlen = sizeof(peeraddr);
		iRtn = recvfrom(sock, Receive, MAX_PACKET_SIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if(iRtn == SOCKET_ERROR){
			ShowMessage("[����] recvfrom() Unicast ���� ����");
		}

		// �۽����� IP �ּ� üũ
		if(memcmp(&peeraddr, &remoteaddr, sizeof(peeraddr))){
			ShowMessage("[����] �߸��� ���� �ּҰ� �Խ��ϴ�");
		}

		// ���� ������ ���
		buf[iRtn] = '\0';
		strRtnMsg.sprintf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.", iRtn);
		ShowMessage(strRtnMsg);
	*/
	// ------------------------------------------

	closesocket(sock);

#if 0
	// ���� ����
	WSACleanup();
#endif

	return retval;
}


String UdpUniCastSender::err_display(char *msg)
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