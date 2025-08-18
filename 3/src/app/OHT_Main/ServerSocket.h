/**
@file    ServerSocket.h
@version OHT 7.0
@brief   ServerSocket Class Header File
*/
//---------------------------------------------------------------------------
#ifndef ServerSocketH
#define ServerSocketH
//---------------------------------------------------------------------------

#include <windows.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#include "List.h"

#pragma comment(lib,"ws2_32.lib")
#define SOCKETERROR -1
#define INVALID_EVENT 258
#define BUF_SIZE	8192
#include "LogHandler.h"	// Log �ۼ� ����

//extern void CALLBACK CompRoutine(unsigned long, unsigned long, LPWSAOVERLAPPED, unsigned long);


//�ܺο��� �����Ͽ� �ƹ��� �����ֵ��� ����.
typedef struct _CLIENT_DATA_  //���� Ŭ���̾�Ʈ�� ������ ���, �� Ŭ���̾�Ʈ���� ���� ���� �� �����͸� ����Ʈ�� �����ϱ� ����.
{

   SOCKET hRcvSock;   //SocketInfo
   char buf[BUF_SIZE];    //Data ����
   int  Length;		  //Data ����
} CLIENT_DATA, *LPCLIENT_DATA ;

//����Ʈ�� ����� Ŭ���̾�Ʈ ����ü : 2014.11.12 doori.shin
typedef struct _CLIENT_SOCK
{
	SOCKET hClientSock;
	char szIPAddress[20];
}CLIENT_SOCK;

/**
@class   ServerSocket
@date    2013.05.03
@author  puting
@brief   ServerSocket ���� Class
@note    Socket ������� Data�� �о���δ�.
*/


class ServerSocket //: public ThreadCtrl
{

private:
	LogUnit *pLogTCP;      ///< TCP �α� ������ ���� ����
	SOCKET	hServSock; ///<���� ���� ���� ����

	WSAEVENT EventArray[10]; //Max Client ����

	int EventTotal;
	int nPortNum;

	SOCKADDR_IN m_CurClientInfo;

	CLIENT_DATA *m_clientData;                   ///<Client�� ���� Data
	LPCLIENT_DATA m_TempclientData;              ///<�ӽ� Client�� ���� Data

	//char* pBuf;
	LList* m_ClientList;  						///<�ְ� 10���� Client ����
	LList* m_PacketList;                        ///<�ְ� 10���� Receive Data ����

	CRITICAL_SECTION	m_CS;

	int CreateSocketInfo(SOCKET socket);
	int CreateSocketInfo(SOCKET socket, char* ip);
	void freeSocketInfo(int eventIndex);
	int nClientCount;

public:
	ServerSocket();
	~ServerSocket();

	bool SocketOpen(int PortNum);
	bool SocketClose();

	void Update();         // ������ȿ� ������ ��

	bool Read(char* RecvBuf, int* Length, int* Count); //���� �������� ������ ���
	bool Write(char* SendBuf, int Length, UINT Index);

	//int GetIndex(void* pCmd);  //index ������ �ܺο� �ֱ� ���� ���

	bool IsClientConnect();
	bool IsClientDisConnect();
	bool IsClientReadData();

	UINT GetPacketCount();
	UINT GetClientCount();

	void ClientRemove(UINT Index);
	SOCKET  GetClientInfo(UINT Index);

//	char* GetCurIPAddress();                      //�̻��
//	char* GetIPAddress(UINT index);               //�̻��

	//void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

	WSANETWORKEVENTS networkEvents; ///<WaitforEvent�� �ֱ� ���� �̺�Ʈ

	char* BufferPointer;   //�ܺο��� ����ϱ����Ͽ�
	int nRestLenth;        //�ܺο��� ����ϱ����Ͽ�


};
#endif
