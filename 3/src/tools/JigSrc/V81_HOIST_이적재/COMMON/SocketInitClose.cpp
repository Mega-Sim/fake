#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
//---------------------------------------------------------------------------


#pragma hdrstop

#include "SocketInitClose.h"
#include "Protocol_Define_Modify.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

SocketInitClose::SocketInitClose()
{

}

SocketInitClose::~SocketInitClose()
{

}

// ���� �ʱ�ȭ �Լ�
// ������ 0, ���н� 1�� �����Ѵ�.
int SocketInitClose::SocketInit()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
	else
		return 0;
}

void SocketInitClose::SocketClose()
{
	WSACleanup();
}
