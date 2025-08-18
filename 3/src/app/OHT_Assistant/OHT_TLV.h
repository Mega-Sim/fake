/**
@file    OHT_TLV.h
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
#include "ErrorCode.h"
#include "Def_DataFile.h"

// -------------------------------------------
// Common  Definition
// -------------------------------------------
// by zzang9un 2013.10.10 : Ư�� Command ���� ����
#define LEN_ACK_DATA_VERSIONINFO    (sizeof(DATA_VERSION_INFO) * 3)
#define LEN_ACK_DATAFILE_DOWNLOAD   5
#define LEN_ACK_DATAFILE_BACKUP     5
#define LEN_ACK_DATAFILE_UPLOAD     1

#define WAITTIME        10 ///< ���� ��� �ð� ms
#define MAX_WAIT        LEN_HEADER*WAITTIME ///< Socket Read, Write �ִ� ��� Ƚ��



// Command ó�� �Լ�(parsing->Make CmdList)
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian = false);
CMD_HEADER MakeHeader(int Tag, int Length);
int GetBodyLength(char* pRecvBuf, bool IsConvertEndian = false);



/**
@brief	TLV�����Ϳ� ���� ������ �����ϴ� ��ü
@author	doori.shin
@date   2015.05.12
*/
class TLV
{
private:
	int m_nRecvLen;
public:
	bool bIsBigTLV;
	int m_TLVTotalLen;
	CMD_HEADER m_Header;  ///<  CmdHeader
	char *m_TLVBuffer;    ///< TLV ������ ����� ���� ������
	bool bIsMoreRcv;		// ���� Read Event���� �޾ƾ��ϴ� ���

	TLV(int length, bool isBig)
	{
		m_nRecvLen = 0;
		m_TLVTotalLen = length;
		bIsBigTLV = isBig;
		m_TLVBuffer = new char[m_TLVTotalLen];
		bIsMoreRcv = false;

		memset(m_TLVBuffer, 0x00, sizeof(m_TLVTotalLen));
		memset(&m_Header, 0x00, sizeof(CMD_HEADER));
	}

	void InitTLV()
	{
		if(m_TLVBuffer != NULL)
		{
			delete[] m_TLVBuffer;
			m_TLVBuffer = NULL;
		}

		m_TLVTotalLen = 0;
		memset(&m_Header, 0x00, sizeof(CMD_HEADER));
		bIsBigTLV = false;
		m_nRecvLen = 0;
		bIsMoreRcv = false;
	}

	/**
	@brief	���� ���ۿ� data�� ������
	@param 	data : ���ۿ� ������ ������
	@param	length : ������ �������� ����
	@return ���ۿ� ����� �������� �� ����
	*/
	int AppendTLVData(char* data, const int& length)
	{
		memcpy(m_TLVBuffer + m_nRecvLen, data, length);
		m_nRecvLen += length;

		return m_nRecvLen;
	}

	/**
	@brief	�� �޾ƾ� �� Byte ���̸� ����
	*/
	int GetRemainingData()
	{
		return (this->m_TLVTotalLen - this->m_nRecvLen);
    }
};


// -------------------------------------------
// Template �Լ�
// -------------------------------------------
/**
@brief   Command �Ǵ� Status�� Encode�ϴ� Template �Լ� 
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode�� ������ ����� ������ �ּ�
@param   pHeader : ������ Data�� Header ����ü
@tparam  pSndData : Encode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  Encode�� ��ü bytes, ���� �� ���� �ڵ� ����
*/
template<typename Type>
int Encode(char *pBuf, int SndTag, Type *pSndData, bool ConvertEndian = false)
{
	int RtnBytes = 0; // Encode�� �� Bytes

	CMD_HEADER sSndHeader;

	sSndHeader = MakeHeader(SndTag, sizeof(Type));

    switch(sSndHeader.uTag.Info.CmdType)
    {
    case TAG_CMDTYPE_DRVING:
        RtnBytes = EncodeGoCmd(pBuf, &sSndHeader, (GO_CMD *)pSndData, ConvertEndian);
        break;

    case TAG_CMDTYPE_TRANS:
        RtnBytes = EncodeTransCmd(pBuf, &sSndHeader, (TRANS_CMD *)pSndData, ConvertEndian);
        break;

    case TAG_CMDTYPE_OCSCMD:
        RtnBytes = EncodeOCSCmd(pBuf, &sSndHeader, (uOCS_CMD *)pSndData, ConvertEndian);
        break;

	case TAG_CMDTYPE_TPCMD:
		RtnBytes = EncodeTPCmd(pBuf, &sSndHeader, (uTP_CMD *)pSndData, ConvertEndian);
        break;        

    case TAG_CMDTYPE_STATUS:
        if(sSndHeader.uTag.Info.Receiver == TAG_RECEIVER_OCS)
        {
            // OCS���� ������ Status�� ���
            RtnBytes = EncodeStatus_OHT2OCS(pBuf, &sSndHeader, (MSG_STATUS_OHT2OCS *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.Receiver == TAG_RECEIVER_TP)
		{
			// TP���� ������ Status�� ���
			RtnBytes = EncodeStatus_OHT2TP(pBuf, &sSndHeader, (MSG_STATUS_OHT2TP *)pSndData, ConvertEndian);
		}
		break;

	case TAG_CMDTYPE_INIT:
		if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_INIT_INFO_TP)
		{
			// by zzang9un 2013.03.22 : TP�� OHT Info ������ Encode�ϴ� �Լ�
			// OHT_INFO�� uTP_CMD ����ü�� �����
			RtnBytes = EncodeTPCmd(pBuf, &sSndHeader, (uTP_CMD *)pSndData, ConvertEndian);
		}
		break;

	case TAG_CMDTYPE_DATA:
		if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_IOMAP_TP)
		{
			//TP���� ������ IO Map Field
			RtnBytes = EncodeIOMapField_OHT2TP(pBuf, &sSndHeader, (IO_MAP_FIELD *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_NOT_READY)
		{
			//TP���� ������ IO Map Field Not Ready
			RtnBytes = EncodeDataNotReady_OHT2TP(pBuf, &sSndHeader, (BYTE *)pSndData, ConvertEndian);
		}
		else if(sSndHeader.uTag.Info.CmdData == TAG_CMDDATA_DATA_OHT_STATUS)
		{
			RtnBytes = EncodeOHTStatus_AST2TP(pBuf, &sSndHeader, (OHT_ADDITIONAL_INFO *)pSndData, ConvertEndian);
        }
    default:
        ///@todo ���� ó�� �ʿ�
        break;
	}
    
    return RtnBytes;
}

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
        if(pHeader->uTag.Info.Receiver == TAG_RECEIVER_TP)
        {
            // by zzang9un 2013.03.22 : OHT_INFO ������ Decode�Ѵ�.
            RtnBytes = DecodeTPCmd(pBuf, pHeader, (uTP_CMD *)pRecvData, ConvertEndian);
        }
        break;     

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

int EncodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   IO_MAP_FIELD *pSndData,
					   bool IsConvertEndian = false);
int EncodeDataNotReady_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
   					   BYTE *pSndData,
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

//2013.12.19 doori.shin OHT Additional status endcode
int EncodeOHTStatus_AST2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   OHT_ADDITIONAL_INFO *pSndData,
					   bool IsConvertEndian /* = flase */);

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

/**
@brief   Buffer�κ��� 1���� TLV�� ������� �Լ�
@author  puting
@date    2013.07.17
@param   pBuffer : ���� Data�� �ִ� ����
@param   BuffLen : ���� Data�� �ִ� ���� ����
@param   pTLV : 1���� TLV�� ����� ����
@param   Bytes : 1���� TLV Size(bytes)
@note    ���ڿ��� ���� �ݵ�� ���ڿ��� ũ�⸦ �Բ� ȣ���ؾ� �Ѵ�.
*/
int ReadGetData(char *pBuffer,
				int BuffLen,
				char *pTLV,
				int *Bytes,
				bool IsConvertEndian = false);

#endif
