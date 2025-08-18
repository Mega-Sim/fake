//---------------------------------------------------------------------------

#ifndef LogTLVH
#define LogTLVH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
//#include <ScktComp.hpp>
#include <iostream>
#include <algorithm>

#include "../Common/Def_DataType.h"

#define LEN_LOGTLV_BUFFER  8192 ///< TLV �ִ� ����

#define MAX_WAIT        60 ///< Socket Read, Write �ִ� ��� Ƚ��


using namespace std;

#pragma pack(push, 1)
typedef enum LOG_DATA_TAG_
{
	LOG_CLEAR	= 0,
	LOG_WRITE	= 1
}LOG_DATA_TAG;
/**
@brief   Log Header
@author  ���¿�
@date
*/
typedef struct LOG_HEADER_
{
	LOG_DATA_TAG	Tag;    ///<Tag : Clear/Write
	UINT		nLength;    ///<���̰� 1000������ �Ѿ�Ƿ� UINT(4����Ʈ�� ���)
} LOG_HEADER;
/**
@brief   Log Packet
@author  ���¿�
@date
*/
typedef struct LOG_DATA_PACKET_
{
	char	pPath[256];    ///<Path ����
	char	pValue[1024];  ///<Log Data ����
}LOG_DATA_PACKET;
#pragma pack(pop) // Pack ����

/**
@brief   GetLogHeader �Լ�
@author  ���¿�
@date    2013.07.30
@note    ������ ���� ���� �����͸� ���� Header�� �����ϱ� ���� �Լ�.
*/
LOG_HEADER GetLogHeader(char *pRecvBuf)
{
	LOG_HEADER RtnHeader; // Return�� Header ����ü

	memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);

	return RtnHeader;
}

/**
@brief   MakeLogHeader �Լ�
@author  ���¿�
@date    2013.07.30
@note   Tag�� Length�� ���� Header�� ����� ���� �Լ�.
*/
LOG_HEADER MakeLogHeader(int Tag, int Length)
{
	LOG_HEADER RtnHeader;

	RtnHeader.Tag = Tag;
	RtnHeader.nLength = Length;

	return RtnHeader;
}

//-------------------------------------------------------------------------
//  Common Socket I/O
//-------------------------------------------------------------------------
/**
@brief   Socket�� ���۸� Read�ϴ� �Լ�
@author  puting
@date    2013.07.30
@param   pSocket : Read�� Socket
@param   pTLV : ������ ������ ������ ����(tag�� length�� ������ ����)
@param   Bytes : ������ ��ü bytes ��
@return  Read ���� �� True ����, ���� �� ���� �ڵ� ����
@note    Length ���� �� Endian ��ȯ ���� ������ ���� �ؾ� ��
*/
//int ReadLogSocket(TCustomWinSocket *pSocket,
//			   char *pTLV,
//			   int *Bytes)
//			  // bool IsConvertEndian /* = flase */)
//{
//	int WaitCount; // ��� Ƚ��
//    int RecvBytes; // ������ Bytes
//    int BodyLen; // Body Length
//
//    // Step 1. ���� ���� size Ȯ��
//    WaitCount = 0;
//    while(1)
//    {
//		RecvBytes = pSocket->ReceiveLength();
//
//        if(RecvBytes >= LEN_HEADER)
//            break;
//
//        // ������ size�� Header���� ���� ��� ���
//        if(++WaitCount > MAX_WAIT)
//            break;
//
//        Sleep(WAITTIME);
//    }
//
//    // ���� ó��
//    if(RecvBytes < LEN_HEADER)
//    {
//        ///@todo ���� ó�� �ʿ�
//        return (FALSE);
//    }
//
//    // Step 2. Header Read
//    RecvBytes = pSocket->ReceiveBuf(pTLV, LEN_HEADER);
//
//    if (RecvBytes < LEN_HEADER)
//    {
//        ///@todo ���� ó�� �ʿ�
//		return (FALSE);
//	}
//
//    // Step 3. Body�� Length ����
//	BodyLen = ((BYTE)pTLV[4]) + (BYTE)pTLV[5] * 256 + ((BYTE)pTLV[6]) * 256 * 256 + + ((BYTE)pTLV[7]) * 256 * 256 * 256;
//
//	if(BodyLen <0)
//	{
//	     return (FALSE);
//    }
//	// by zzang9un 2013.03.15 : Body�� Tail���� �߰��� �о��
//	int ReadByteLen = BodyLen + LEN_TAIL;
//
//	// Step 4. Length��ŭ ���� ���ۿ� data�� �ִ� ���� Ȯ��
//	WaitCount = 0;
//	while(1)
//	{
//		// by zzang9un 2013.03.15 : Tail���� �Ѳ����� �о��
//		RecvBytes = pSocket->ReceiveLength();
//
//        if(RecvBytes >= ReadByteLen)
//            break;
//
//        if(++WaitCount > MAX_WAIT)
//            break;
//
//		Sleep(WAITTIME);
//    }
//
//    // ���� ó��
//    if(RecvBytes < ReadByteLen)
//    {
//        ///@todo ���� ó�� �ʿ�
//        return (FALSE);
//    }
//
//    // Step 5. Body Read
//    // by zzang9un 2013.03.15 : Tail���� �Ѳ����� �о��
//	RecvBytes = pSocket->ReceiveBuf(pTLV + LEN_HEADER, ReadByteLen);
//
//    // ���� ó��
//    if(RecvBytes < ReadByteLen)
//    {
//        ///@todo ���� ó�� �ʿ�
//        return (FALSE);
//    }
//
//    // by zzang9un 2013.03.15 : Tail Lenth �߰�
//	*Bytes = BodyLen + LEN_HEADER + LEN_TAIL;
//
//    return (TRUE);
//}
/**
@brief   OHT���� Monitoring���� ������ ����ϴ� �Լ�
@author  puting
@date    2013.07.30
@param   pBuf : Send�� buffer
@param   SndTag : Send�� Tag��
@param   LOG_HEADER : ���� Header��
@param   LOG_PACKET : ���� ����ü ����
@return  ���� �� ���� ����, ���� �� -1 ����
*/
int EncodeLogCmd(char *pBuf,
				 LOG_HEADER *pSndHeader,
				 LOG_DATA_PACKET *pSndData)
{
	int RtnBytes = 0; // Encode�� �� bytes

	LOG_HEADER SndHeader = *pSndHeader;
	LOG_DATA_PACKET SndPacket = *pSndData;

	UINT BodySize = sizeof(LOG_DATA_PACKET);

	// Step 0. Error ó��
	// Body�� Length�� Ʋ�� ��� ���� ó��
	if(SndHeader.nLength != BodySize)
		return -1;

	// -------------------------------------------
	// Step 1. Send Buf �ʱ�ȭ
	memset(pBuf, 0x00, LEN_LOGTLV_BUFFER);  //8192

	// -------------------------------------------
	// Step 2. Header Encode
	memcpy(pBuf, &SndHeader, LEN_HEADER);

	// -------------------------------------------
	// Step 3. Body Encode
	memcpy(pBuf + LEN_HEADER, &SndPacket, SndHeader.nLength);  //Path :1024, SYSTETIME : 16, HEADER :6

	// by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
	memcpy(pBuf + LEN_HEADER + BodySize, &SndHeader.nLength, LEN_TAIL);

	RtnBytes = LEN_HEADER + SndHeader.nLength + LEN_TAIL; // Encode�� �� bytes ����


	return RtnBytes;
}


/**
@brief   Monitring�� ���� �����͸� �����Ҷ� ���.
@author  puting
@date    2013.07.30
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(Template)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeLogCmd(char *pBuf,
				 LOG_HEADER *pHeader,
				 LOG_DATA_PACKET *pRcvData)
{
	int RtnBytes = 0; // Decode�� �� bytes
	WORD BodySize = sizeof(LOG_DATA_PACKET);

	// -------------------------------------------
	// Step 0. Error ó��
	// Body�� Length�� Ʋ�� ��� ���� ó��
	if(pHeader->nLength != BodySize)
		return -1;

	// -------------------------------------------
	// Step 1. Body Decode
	int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

	memcpy(pRcvData, pBuf + Offset, pHeader->nLength);
	// Step 2. Length ���� �Է�
	RtnBytes = LEN_HEADER + pHeader->nLength;

	return RtnBytes;

}
/**
@brief   Monitoring ���α׷����� �Ľ��Ҷ� ���� �Լ�.
@author  puting
@date    2013.07.30
@param   pRecvBuf : ���� �����͸� �̿��Ͽ� Decoding
@return  LOG_DATA_PACKET return
@note    OHT Assistant���� ���Ǵ� �Լ�.
*/
int ParsingLogCmd(char *pRecvBuf, LOG_DATA_PACKET* LogData)
{
	int Error = 0;

	int DecodeSize = 0; // Decode�� ��ü ������
	LOG_HEADER Header;  // Header�� parsing�� ����ü

	memset(LogData, 0x00, sizeof(LOG_DATA_PACKET));

	// step1. Header parsing
	Header = GetLogHeader(pRecvBuf);

	// step2. Tag�� Type�� ���� Decode �Լ� ȣ��

	DecodeSize = DecodeLogCmd(pRecvBuf, &Header, LogData);

	if(DecodeSize ==-1)
	{

		return -1;
	}
	////////////////////////
//	���� ������ �ڵ� �ۼ�.
   ///����.LogData�� ���� �������.
///
	return Error;

}
//---------------------------------------------------------------------------
/**
@brief   OHT���� Monitornig���� ���� �����͸� ���� �Լ�.
@author  puting
@date    2013.07.30
@param   SndBuf : ���� Data�� ���ۿ� �ֱ� ���� ����
@param   pLogHeader : LogHeader ����
@param   pLogPacket : LogPacket ����
@return  ���� �� ���� ����, ���� �� -1 ����
@note    OHT���� ���Ǵ� �Լ�.
*/
int MakeLogOHT2Monitor(char *SndBuf, LOG_HEADER *pLogHeader, LOG_DATA_PACKET *pLogPacket)
{
	int SndLen = 0;

	memset(SndBuf, 0x0,LEN_LOGTLV_BUFFER);

	SndLen = EncodeLogCmd(SndBuf, pLogHeader, pLogPacket);

	// ���� ó��
	if(SndLen < 0)
	{
      return -1;
	}

	return SndLen;
}

#endif
