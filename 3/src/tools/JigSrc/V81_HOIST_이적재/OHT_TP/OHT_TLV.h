/**
@file    OHT_TLV.h
$Rev:: 2173          $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2014-03-12 1#$:  Date of last commit
@version OHT 7.0
@brief   OHT TLV ��Ŷ�� Encode, Decode�ϴ� �Լ��� ������ Header ����
*/

#ifndef OHT_TLVH
#define OHT_TLVH

#include <vcl.h>
#include <stdio.h>
#include <ScktComp.hpp>
#include <iostream>
#include <algorithm>
using namespace std;

#include "../Common/Def_Protocol.h"
#include "../Common/Def_DataType.h"

#include "../Common/PUtily.h"

// -------------------------------------------
// Common  Definition
// -------------------------------------------
#define CPLUS_BUILDER_XE ///< IDE Tool ����
#define LITTLE_ENDIAN    ///< Little-Endian System�� ��� 

#define LEN_TLV_BUFFER  8192 ///< TLV �ִ� ����


// -------------------------------------------
// Common Error Code
// -------------------------------------------
#define ERR_TAG_NAME        -1000
#define ERR_TAG_CMDTYPE     -1001
#define ERR_TAG_PROTOTYPE   -1002
#define ERR_BODY_LENGTH  	-1010
#define ERR_ENCODE_LENGTH  	-1020
#define ERR_DECODE_LENGTH  	-1021
#define ERR_UNKNOWNDATALEN  -1030

#define ERR_PROTOCOL_DECODEERR -1030

// Command ó�� �Լ�(parsing->Make CmdList)
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian = false);
CMD_HEADER MakeHeader(int Tag, int Length);


/**
@brief   Big-Endian�� Little-Endian���� ��ȯ���ִ� �Լ�
@author  zzang9un
@date    2012.11.01
@param   objp : Convert�� Data�� �ּ�
@tparam  Type : template parameter
*/
/*
template <class Type>
void ConvertEndian(Type *objp)
{
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(Type));
}
*/


/**
@brief   Command�Ǵ� Status�� Big-Endian���� Decode�ϴ� �Լ� 
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : Decode�� Data�� Header
@tparam  pRecvData : Decode�� ����� ����Ǵ� ����ü �ּ�
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  Decode�� ��ü bytes, ���� �� ���� �ڵ� ����
*/
template<typename Type>
int Decode(char *pBuf, CMD_HEADER *pHeader, Type *pRecvData, bool ConvertEndian = false)
{
    int RtnBytes = 0; // Decode�� �� Bytes

	switch(pHeader->uTag.Info.CmdType)
    {
    case TAG_CMDTYPE_DRVING: // ���� ���
        RtnBytes = DecodeGoCmd(pBuf, pHeader, (GO_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_DRVING_ETC: // ���� ��Ÿ ���
        RtnBytes = DecodeGoEtcCmd(pBuf, pHeader, (uGO_ETC_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TRANS: // ������ ���
        RtnBytes = DecodeTransCmd(pBuf, pHeader, (TRANS_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_OCSCMD: // OCS �Ϲ� ���
		RtnBytes = DecodeOCSCmd(pBuf, pHeader, (uOCS_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TPCMD: // TP ���
        RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
        break;

    case TAG_CMDTYPE_STATUS: // Status ��û ���
        if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_OCS)
        {
            RtnBytes = DecodeStatus_OHT2OCS(pBuf, pHeader, (MSG_STATUS_OHT2OCS *)pRecvData, ConvertEndian);
        }
		else if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_TP)
        {
            RtnBytes = DecodeStatus_OHT2TP(pBuf, pHeader, (MSG_STATUS_OHT2TP *)pRecvData, ConvertEndian);
		}
        break; 

    case TAG_CMDTYPE_INIT: // �ʱ�ȭ ���� ���
		if(pHeader->uTag.Info.CmdData == TAG_CMDDATA_INIT_INFO_TP)
		{
			// by zzang9un 2013.03.22 : OHT_INFO ������ Decode�Ѵ�.
			RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
		}
        break;     
	case TAG_CMDTYPE_DATA:
		switch(pHeader->uTag.Info.CmdData)
		{
			case TAG_CMDDATA_DATA_IOMAP_TP :
				RtnBytes = DecodeIOMapField_OHT2TP(pBuf, pHeader, (IO_MAP_FIELD *)pRecvData, ConvertEndian);
				break;
			case TAG_CMDDATA_DATA_NOT_READY :
				break;
        }
    default:
        break;
    }    
    
    return RtnBytes;
}
//---------------------------------------------------------------------------

// -------------------------------------------
// Status ���� Encode �Լ�
// -------------------------------------------
int EncodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2OCS *pSndData,
                       bool IsConvertEndian = false);
int EncodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2TP *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// ���� �� ������ Command Encode �Լ�(For OCSEmulator, TP)
// -------------------------------------------
int EncodeGoCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       GO_CMD *pSndData,
                       bool IsConvertEndian = false);
int EncodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uGO_ETC_CMD *pSndData,
                       bool IsConvertEndian = false);
int EncodeTransCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       TRANS_CMD *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// OCS Command Encode �Լ�(For OCSEmulator)
// -------------------------------------------
int EncodeOCSCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uOCS_CMD *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// TP Command Encode �Լ�(For TP)
// -------------------------------------------
int EncodeTPCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian = false);

// by zzang9un 2013.01.25 : ACK_DATA Encode �Լ�
int EncodeCmdACK(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       ACK_DATA *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// Assistant Command Encode �Լ�(For Assistant)
// -------------------------------------------
int EncodeASTCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian = false);

// -------------------------------------------
// Command Decode �Լ� ����
// -------------------------------------------  
int DecodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2OCS *pRcvData,
                       bool IsConvertEndian = false);
int DecodeStatus_OHT2TP(char *pBuf,
					   CMD_HEADER *pHeader,
					   MSG_STATUS_OHT2TP *pRcvData,
					   bool IsConvertEndian = false);
int DecodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pHeader,
					   IO_MAP_FIELD *pRcvData,
					   bool IsConvertEndian = false);
int DecodeGoCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       GO_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uGO_ETC_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeTransCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       TRANS_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeOCSCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uOCS_CMD *pRcvData,
                       bool IsConvertEndian = false);
int DecodeTPCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       uTP_CMD *pRcvData,
					   bool IsConvertEndian = false);
int DecodeOHTStatus_AST2TP(char *pBuf,
						CMD_HEADER *pHeader,
						OHT_ADDITIONAL_INFO *pRcvData,
						bool IsConvertEndian /* = flase */);
                       
/**
@brief   �⺻ �ڷ����� Data�� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.04
@param   pBuf : Encode�� ������ �ּ�
@param   pOffset : Encode�� ������ offset
@tparam  pData : Encode�� Data(template)
@return  ������ ��� Encode�� Bytes�� ����, �������� ��� ���� �ڵ� ����
*/
template <typename Type>
int EncodeBody(char *pBuf, int *pOffset, Type *pData)
{
    int RtnEncodeSize = sizeof(Type);

    if(RtnEncodeSize == 1)
    {
        // 1byte�� ���(char) Convert �ʿ� ����
        memcpy(pBuf + *pOffset, pData, RtnEncodeSize);
        *pOffset += RtnEncodeSize;
        return RtnEncodeSize;
    }
    else if(RtnEncodeSize >= 2)
    {
        // 2byte���� ū ���� Covert �� �� Encode
        ConvertEndian(pData);
        memcpy(pBuf + *pOffset, pData, RtnEncodeSize);
        *pOffset += RtnEncodeSize;
        return RtnEncodeSize;
    }
    else
    {
        // ���� ó��
        return ERR_UNKNOWNDATALEN;
    }    
}

/**
@brief   ���ڿ��� Encode�� ��� ����ϴ� �Լ�
@author  zzang9un
@date    2012.11.04
@param   pBuf : Encode�� ������ �ּ�
@param   pOffset : Encode�� ������ offset
@param   pData : Encode�� Data(���ڿ�)
@param   nSize : ���ڿ��� Size(bytes)
@note    ���ڿ��� ���� �ݵ�� ���ڿ��� ũ�⸦ �Բ� ȣ���ؾ� �Ѵ�.
*/
void EncodeBodyStr(char *pBuf, int *pOffset, char *pData, int Size)
{
    memcpy(pBuf + *pOffset, pData, Size);
    *pOffset += Size;
}

/**
@brief   �⺻ �ڷ����� Data�� Decode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode�� ������ �ּ�
@param   pOffset : Decode�� ������ offset
@tparam  pData : Decode�� Data(template)
@return  ������ ��� Decode�� Bytes�� ����, �������� ��� ���� �ڵ� ����
*/
template <typename Type>
int DecodeBody(char *pBuf, int *pOffset, Type *pData)
{
    int RtnDecodeSize = sizeof(Type);

    if(RtnDecodeSize == 1)
    {
        // 1byte�� ���(char) Convert �ʿ� ����
        memcpy(pData, pBuf + *pOffset, RtnDecodeSize);
        *pOffset += RtnDecodeSize;
        return RtnDecodeSize;
    }
    else if(RtnDecodeSize >= 2)
    {
        // 2byte���� ū ���� Covert �� �� Encode
        memcpy(pData, pBuf + *pOffset, RtnDecodeSize);
        ConvertEndian(pData);
        *pOffset += RtnDecodeSize;
        return RtnDecodeSize;
    }
    else
    {
        // ���� ó��
        return ERR_UNKNOWNDATALEN;
    }    
}

/**
@brief   ���ڿ��� Decode�� ��� ����ϴ� �Լ�
@author  zzang9un
@date    2012.11.04
@param   pBuf : Decode�� ������ �ּ�
@param   pOffset : Decode�� ������ offset
@param   pData : Decode�� ������ ����Ǵ� ���ڿ� �ּ�
@param   nSize : ���ڿ��� Size(bytes)
@note    ���ڿ��� ���� �ݵ�� ���ڿ��� ũ�⸦ �Բ� ȣ���ؾ� �Ѵ�.
*/
void DecodeBodyStr(char *pBuf, int *pOffset, char *pData, int Size)
{
    memcpy(pData, pBuf + *pOffset, Size);
    *pOffset += Size;
}


// Socket I/O �Լ�
int ReadSocket(TCustomWinSocket *pSocket,
               char *pTLV,
               int *Bytes,
               bool IsConvertEndian = false);

#endif
