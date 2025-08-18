/**
@file    PingUnit.h
$Author:: soo583
$Date:: 2013/07/17
@version OHT 6.5
@brief   Ping Unit Header File
*/
//---------------------------------------------------------------------------

#ifndef PingUnitH
#define PingUnitH
#include <winsock2.h>

#define TIME_OUT	1		//sec
//---------------------------------------------------------------------------
/**
@class   TOHT_PING
@date    2013/07/17
@author  soo583
@brief   Ping ���� Class
*/
class TOHT_PING
{
 private :
   int SendEchoRequest(SOCKET,LPSOCKADDR_IN);
   int WaitForEchoReply(SOCKET);
   DWORD RecvEchoReply(SOCKET, LPSOCKADDR_IN, u_char *);
   u_short in_cksum(u_short *, int);

   void Init();
   	int status;
	String strResult;

 public :
   TOHT_PING();
   ~TOHT_PING();
   int Socket_Create();
   int Socket_Destroy();
   int Ping(AnsiString);

//   String GetResultString();            //�̻��
   int GetStatus();
//   int ExecuteDosCmd(String strDocCmd);      //�̻��

 public :
	SOCKET   rawSocket;
};
//---------------------------------------------------------------------------
extern TOHT_PING *gm_Ping;

#endif
