/**
@file    OHT_TLV.cpp
@version OHT 7.0
@brief   OHT TLV ��Ŷ�� Encode, Decode�ϴ� �Լ��� ������ CPP ����
*/

#pragma hdrstop

#include "OHT_TLV.h"


// -------------------------------------------
// Client Socket I/O
// -------------------------------------------


//-------------------------------------------------------------------------
//  Common Socket I/O
//-------------------------------------------------------------------------

/**
@brief   Socket�� ���۸� Read�ϴ� �Լ�
@author  zzang9un
@date    2012.10.30
@param   pSocket : Read�� Socket
@param   pTLV : ������ ������ ������ ����(tag�� length�� ������ ����)
@param   Bytes : ������ ��ü bytes ��
@param   IsConvertEndian : Length�� ������ �� Endian ��ȯ�� �ʿ��� ���(default:false)
@return  Read ���� �� True ����, ���� �� ���� �ڵ� ����
@note    Length ���� �� Endian ��ȯ ���� ������ ���� �ؾ� ��
@todo    ���� ������ ���� �ڵ带 �߰��ؾ� ��.
*/
int ReadSocket(TCustomWinSocket *pSocket,
			   char *pTLV,
               int *Bytes,
               bool IsConvertEndian /* = flase */)
{
    int WaitCount; // ��� Ƚ��
    int RecvBytes; // ������ Bytes
    int BodyLen = 0; // Body Length

    // Step 1. ���� ���� size Ȯ��
    WaitCount = 0;
    while(1)
    {
        RecvBytes = pSocket->ReceiveLength();

        if(RecvBytes >= LEN_HEADER)
            break;

        // ������ size�� Header���� ���� ��� ���
        if(++WaitCount > MAX_WAIT)
            break;

        Sleep(WAITTIME);
    }

    // ���� ó��
    if(RecvBytes < LEN_HEADER)
    {
        ///@todo ���� ó�� �ʿ�
        return (FALSE);
    }

    // Step 2. Header Read
    RecvBytes = pSocket->ReceiveBuf(pTLV, LEN_HEADER);

    if (RecvBytes < LEN_HEADER)
    {
        ///@todo ���� ó�� �ʿ�
        return (FALSE);
    }

    // Step 3. Body�� Length ����
// -------------------------------------------
// [BEGIN] by zzang9un 2013.10.10 : Length�� Bytes ũ�⿡ ���� �ڵ尡 �����
#if(LEN_LENGTH == 2)
    if(IsConvertEndian)
        BodyLen = ((BYTE)pTLV[4]) * 256 + (BYTE)pTLV[5];
    else
        BodyLen = ((BYTE)pTLV[4]) + (BYTE)pTLV[5] * 256;
#elif(LEN_LENGTH == 4)
    if(IsConvertEndian)
        BodyLen = (pTLV[4] << 24) | (pTLV[5] << 16) | (pTLV[6] << 8) | (BYTE)pTLV[7];
    else
        BodyLen = (BYTE)pTLV[4] | (pTLV[5] << 8) | (pTLV[6] << 16) | (pTLV[7] << 24);
#endif
// [END] by zzang9un 2013.10.10 : Length�� Bytes ũ�⿡ ���� �ڵ尡 �����
// -------------------------------------------

    // by zzang9un 2013.03.15 : Body�� Tail���� �߰��� �о��
    int ReadByteLen = BodyLen + LEN_TAIL;

	// Step 4. Length��ŭ ���� ���ۿ� data�� �ִ� ���� Ȯ��
    WaitCount = 0;
    while(1)
    {
        // by zzang9un 2013.03.15 : Tail���� �Ѳ����� �о��
        RecvBytes = pSocket->ReceiveLength();

        if(RecvBytes >= ReadByteLen)
            break;

        if(++WaitCount > MAX_WAIT)
            break;

        Sleep(WAITTIME);
    }

    // ���� ó��
    if(RecvBytes < ReadByteLen)
    {
        ///@todo ���� ó�� �ʿ�
        return (FALSE);
    }

    // Step 5. Body Read
    // by zzang9un 2013.03.15 : Tail���� �Ѳ����� �о��
    RecvBytes = pSocket->ReceiveBuf(pTLV + LEN_HEADER, ReadByteLen);

    // ���� ó��
    if(RecvBytes < ReadByteLen)
    {
        ///@todo ���� ó�� �ʿ�
        return (FALSE);
    }

    // by zzang9un 2013.03.15 : Tail Lenth �߰�
    *Bytes = BodyLen + LEN_HEADER + LEN_TAIL;

    return (TRUE);
}
//---------------------------------------------------------------------------

// -------------------------------------------
// Command Parsing Function Prototype
// -------------------------------------------
/**
@brief   ������ ������ Header�� ������ �Լ�
@author  zzang9un
@date    2012.11.05
@param   pRecvBuf : ������ ����(tag�� length�� ������ ����)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ Data�� Header ����ü�� ����
*/
CMD_HEADER GetHeader(char *pRecvBuf, bool IsConvertEndian /* = flase */)
{
    CMD_HEADER RtnHeader; // Return�� Header ����ü

    if(IsConvertEndian)
    {
        memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);
        ConvertEndian(&RtnHeader.uTag.Data);
        ConvertEndian(&RtnHeader.Length);
    }
    else
    {
        memcpy(&RtnHeader, pRecvBuf, LEN_HEADER);
    }

    return RtnHeader;
}

/**
@brief   ������ ������ BodyLength�� ������ �Լ�
@author  doori.shin
@date    2015.03.11
@param   pRecvBuf : Length�� ���۵Ǵ� ��ġ�� ����
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  Byte�� �� BodyLength�� ������ ��ȯ�� ��
*/
int GetBodyLength(char* pRecvBuf, bool IsConvertEndian)
{
	int length = 0;

	if(IsConvertEndian == true)
	{
		length = ((BYTE)pRecvBuf[0] << 24)|((BYTE)pRecvBuf[1] << 16)
				|((BYTE)pRecvBuf[2] << 8)|(BYTE)pRecvBuf[3];
	}
	else
	{
		length = ((BYTE)pRecvBuf[0])|((BYTE)pRecvBuf[1] << 8)
				|((BYTE)pRecvBuf[2] << 16)|((BYTE)pRecvBuf[3] << 24);
    }

	return length;
}
//---------------------------------------------------------------------------

/**
@brief   Tag, Length�� �̿��Ͽ� Command�� Header�� Make�ϴ� �Լ�
@author  zzang9un
@date    2012.11.05
@param   Tag : Make �ϴ� Command�� Tag
@param   Length : Make �ϴ� Command�� Length
@return  Make �Ϸ��� Header ����ü�� ����
*/
CMD_HEADER MakeHeader(int Tag, int Length)
{
    CMD_HEADER RtnHeader;

    RtnHeader.uTag.Data = Tag;
    RtnHeader.Length = Length;

    return RtnHeader;
}
//---------------------------------------------------------------------------

/**
@brief   Status(OHT->TP)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    TP�� �����ϴ� ��� Little-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/

int EncodeIOMapField_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   IO_MAP_FIELD *pSndData,
					   bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode�� �� bytes
	WORD BodySize = sizeof(int)*2 + (sizeof(BYTE)+sizeof(String))*pSndData->InputNum*2+sizeof(bool);

	pSndHeader->Length = BodySize;

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy�� ���� Offset

	// -------------------------------------------
	// Step 0. Error ó��
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_IOMAP_TP)
		return ERR_TAG_NAME;

	// Body�� Length�� Ʋ�� ��� ���� ó��
	if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
	// Step 1. Send Buf �ʱ�ȭ
	memset(pBuf, 0x00, LEN_TLV_BUFFER);

	// -------------------------------------------
	// Step 2. Header Encode
	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
		// ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
	}
	else
	{
		memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

	// -------------------------------------------
	// Step 4. Body Encode
	Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
		// ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
	}
	else
	{
		memcpy(pBuf + Offset, &pSndData->fIsLoaded, sizeof(pSndData->fIsLoaded));//bool fIsLoaded, �ʿ���� ����
		Offset += sizeof(pSndData->fIsLoaded);
		memcpy(pBuf + Offset, &pSndData->InputNum, sizeof(pSndData->InputNum));
		Offset += sizeof(pSndData->InputNum);
		memcpy(pBuf + Offset, &pSndData->OutputNum, sizeof(pSndData->OutputNum));
		Offset += sizeof(pSndData->OutputNum);

		BYTE strLen = 0;
		for(int i=0; i<pSndData->InputNum; i++)
		{
			//���ڿ� ���̸� ���� ����
			strLen = strlen(pSndData->InputField[i].c_str())+1;
			memcpy(pBuf + Offset, &strLen, sizeof(BYTE));
			Offset += sizeof(BYTE);

			//�ǵڿ� �ι��� �߰�
			pSndData->InputField[i] += "\0";

			//���ڿ��� ����
			memcpy(pBuf + Offset, pSndData->InputField[i].c_str(), strLen);
			Offset += strLen;
		}

		for(int i=0; i<pSndData->OutputNum; i++)
		{
			//���ڿ� ���̸� ����
			strLen = strlen(pSndData->OutputField[i].c_str())+1;
			memcpy(pBuf + Offset, &strLen, sizeof(BYTE));
			Offset += sizeof(BYTE);

			//�ǵڿ� �ι��� �߰�
			pSndData->OutputField[i] += "\0";

			//���ڿ� ����
			memcpy(pBuf + Offset, pSndData->OutputField[i].c_str(), strLen);
			Offset += strLen;
		}

		// 2013.08.26 doori.shin Body�� ���̰� �������̱⶧���� �������� Length �� tail �ٽ� ���ڵ�
		BodySize = Offset - LEN_HEADER;
		memcpy(pBuf + LEN_TAG, &BodySize, LEN_LENGTH);
		memcpy(pBuf + Offset + BodySize, &BodySize, LEN_TAIL);

		RtnBytes = Offset + LEN_TAIL; // Encode�� �� bytes ����
	}

	return RtnBytes;
}

/**
@brief   Status(OHT->TP)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    TP�� �����ϴ� ��� Little-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeDataNotReady_OHT2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   BYTE *pSndData,
                       bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode�� �� bytes
	WORD BodySize = sizeof(BYTE);

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_NOT_READY)
        return ERR_TAG_NAME;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
	// Step 2. Header Encode
    if(IsConvertEndian)
	{
        // by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
        // ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
		// ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
    }
    else
    {
		memcpy(pBuf + Offset, pSndData, sizeof(BYTE));

		// by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
		memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}

int EncodeOHTStatus_AST2TP(char *pBuf,
					   CMD_HEADER *pSndHeader,
					   OHT_ADDITIONAL_INFO *pSndData,
					   bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Encode�� �� bytes
	WORD BodySize = sizeof(OHT_ADDITIONAL_INFO);

	pSndHeader->Length = BodySize;

	CMD_HEADER SndHeader = *pSndHeader;

	int Offset = 0; // Buffer copy�� ���� Offset

	// -------------------------------------------
	// Step 0. Error ó��
	if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_DATA_OHT_STATUS)
		return ERR_TAG_NAME;

	// Body�� Length�� Ʋ�� ��� ���� ó��
	if(SndHeader.Length != BodySize)
		return ERR_BODY_LENGTH;

	// -------------------------------------------
	// Step 1. Send Buf �ʱ�ȭ
	memset(pBuf, 0x00, LEN_TLV_BUFFER);

	// -------------------------------------------
	// Step 2. Header Encode
	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
		// ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
	}
	else
	{
		memcpy(pBuf, &SndHeader, LEN_HEADER);
	}

	// -------------------------------------------
	// Step 4. Body Encode
	Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

	if(IsConvertEndian)
	{
		// by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
		// ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
	}
	else
	{
		memcpy(pBuf + Offset, pSndData, sizeof(OHT_ADDITIONAL_INFO));
		Offset += sizeof(OHT_ADDITIONAL_INFO);

		//Add tail
//		memcpy(pBuf + Offset, &BodySize, LEN_TAIL);
		memcpy(pBuf + Offset, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + LEN_TAIL; // Encode�� �� bytes ����
	}

	return RtnBytes;
}

/**
@brief   Status(OHT->OCS)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS�� �����ϴ� ��� Big-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2OCS *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2OCS);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_STATUS_OCS)
        return ERR_TAG_NAME;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    MSG_STATUS_OHT2OCS pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
		// ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.

		// by zzang9un 2013.05.14 : OHT �⺻ ����
		// by zzang9un 2013.03.21 : Mode, Status, Command ID�� �ѹ��� Encoding
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.MSCmdID.Data);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.NextCmdID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurNodeID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurNodeOffset);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CurStationID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopNodeID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopStationID);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.StopStationOffset);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.ExistFoup);
        EncodeBody(pBuf, &Offset, &pTmpStatus.Data.CarrierType);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.PauseStatus);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.SteerPosition);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.ErrorCode);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Data.OriginInfo);

		// �߰� ����
		EncodeBodyStr(pBuf, &Offset, pTmpStatus.RFTagData, sizeof(pTmpStatus.RFTagData));
		EncodeBody(pBuf, &Offset, &pTmpStatus.APSignalLevel);
		EncodeBodyStr(pBuf, &Offset, pTmpStatus.APMacAddr, sizeof(pTmpStatus.APMacAddr));
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Drv_F_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Drv_F_Speed);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Hoist_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Shift_Position);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Motor_Rotate);

        // OHT Ÿ��
        EncodeBody(pBuf, &Offset, &pTmpStatus.OHTType);

        // by zzang9un 2013.09.06 : Data ���� ����
        EncodeBody(pBuf, &Offset, &pTmpStatus.MapData_CurRevision);
        EncodeBody(pBuf, &Offset, &pTmpStatus.StationData_CurRevision);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TeachingData_CurRevision);
        
		// by zzang9un 2013.05.14 : HOHT�� �����Ǵ� 7.0 Ư�� Status
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Erec);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Edc);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Idc);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Ecap);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_Kw);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_ErrorCode);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_PickupTemper);
		EncodeBody(pBuf, &Offset, &pTmpStatus.HID_CPUTemper);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[0]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[1]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Input[2]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[0]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[1]);
		EncodeBody(pBuf, &Offset, &pTmpStatus.Output[2]);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Status(OHT->TP)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.14
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    TP�� �����ϴ� ��� Little-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       MSG_STATUS_OHT2TP *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2TP);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    if(SndHeader.uTag.Info.CmdData != TAG_CMDDATA_STATUS_TP)
        return ERR_TAG_NAME;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
        // ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
    }
    else
    {
		memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
        // ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
    }
    else
	{
		memcpy(pBuf + Offset, pSndData, SndHeader.Length);

		// by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
		memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

		RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
	}

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Go Cmd�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(GO_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS Emulator�� ���� ���� �Լ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeGoCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       GO_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(GO_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ���� type�� �ƴ� ��� ���� ó��
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_DRVING)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    GO_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü�� ������ Encode�ϴ� �κ�
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
        EncodeBody(pBuf, &Offset, &pTmpData.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationID);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationOffset);
        EncodeBody(pBuf, &Offset, &pTmpData.StopStationType);
        EncodeBody(pBuf, &Offset, &pTmpData.NextNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.LineSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.CurveSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.AccelationLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.FinalPortType);
        EncodeBody(pBuf, &Offset, &pTmpData.TPosition_Shift);
        EncodeBody(pBuf, &Offset, &pTmpData.PreSteeringNodeID);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Go Cmd(Etc)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(GO_ETC_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS Emulator�� ���� ���� �Լ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeGoEtcCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uGO_ETC_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(uGO_ETC_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ���� type�� �ƴ� ��� ���� ó��
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_DRVING_ETC)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    uGO_ETC_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü�� ������ Encode�ϴ� �κ�
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpData.GoEtcCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Trans Cmd�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2013.03.13
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data(TRANS_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS Emulator�� ���� ���� �Լ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeTransCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       TRANS_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(TRANS_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ���� type�� �ƴ� ��� ���� ó��
    if(SndHeader.uTag.Info.CmdType != TAG_CMDTYPE_TRANS)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    TRANS_CMD pTmpData = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü�� ������ Encode�ϴ� �κ�
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
        EncodeBody(pBuf, &Offset, &pTmpData.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpData.CurNodeID);
        EncodeBody(pBuf, &Offset, &pTmpData.CurStationID);
        EncodeBody(pBuf, &Offset, &pTmpData.PortType);
        EncodeBody(pBuf, &Offset, &pTmpData.CarrierType);
        EncodeBody(pBuf, &Offset, &pTmpData.PIODirection);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Hoist);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Shift);
        EncodeBody(pBuf, &Offset, &pTmpData.Tposition_Rotate);
        EncodeBody(pBuf, &Offset, &pTmpData.HoistSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.ShiftSpeedLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.PIOTimeLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.LookDownLevel);
        EncodeBody(pBuf, &Offset, &pTmpData.TransOption.Data);

        // by zzang9un 2013.03.15 : Tail ���� �߰� Encode

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Common Command(OCS->OHT)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.19
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS�� �����ϴ� ��� Big-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeOCSCmd(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       uOCS_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(uOCS_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 3. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    uOCS_CMD pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
		EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.cData);
        EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpStatus.OCSCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   Common Command(OCS->OHT)�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.19
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS�� �����ϴ� ��� Big-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeTPCmd(char *pBuf,
					   CMD_HEADER *pSndHeader,
                       uTP_CMD *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes
    WORD BodySize = sizeof(uTP_CMD);

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

    // -------------------------------------------
    // Step 3. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    uTP_CMD pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
		EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.CmdID);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data1);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data2);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data3);
        EncodeBody(pBuf, &Offset, &pTmpStatus.TPCmd.Data4);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   ACK_DATA�� �����ϱ� �� Encode�ϴ� �Լ�
@author  zzang9un
@date    2013.01.25
@param   pBuf : Encode�� ������ �ּ�
@param   pSndHeader : ������ Command�� Header
@param   pSndData : Encode�� Data
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Encode�� �� bytes ��, ���� �� �����ڵ� ����
@note    OCS�� �����ϴ� ��� Big-Endian���� Encode�Ѵ�.
@warning Body �� �ڿ� Tail ����(Body Length)�� �ݵ�� �����Ѵ�.
*/
int EncodeCmdACK(char *pBuf,
                       CMD_HEADER *pSndHeader,
                       ACK_DATA *pSndData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Encode�� �� bytes

    // by zzang9un 2013.01.25 : length�� 6����Ʈ
    UINT BodySize = 8; // Tag�� Body�� �������� �����Ƿ� 4�� ����.

    CMD_HEADER SndHeader = *pSndHeader;

    int Offset = 0; // Buffer copy�� ���� Offset

    // -------------------------------------------
    // Step 0. Error ó��
    // Protocol Type�� ���� ACK�� �ƴ� ��� ���� ó��
    if(SndHeader.uTag.Info.ProtoType != TAG_PROTOTYPE_ACK)
        return ERR_TAG_PROTOTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(SndHeader.Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Send Buf �ʱ�ȭ
    memset(pBuf, 0x00, LEN_TLV_BUFFER);

    // -------------------------------------------
    // Step 2. Header Encode
    if(IsConvertEndian)
    {
        ConvertEndian(&SndHeader.uTag.Data);
        ConvertEndian(&SndHeader.Length);
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }
    else
    {
        memcpy(pBuf, &SndHeader, LEN_HEADER);
    }

	// -------------------------------------------
    // Step 4. Body Encode
    Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset
    ACK_DATA pTmpStatus = *pSndData;

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Encode�ؾ� ��.
        // by zzang9un 2013.09.27 : ACKCode�� ACKType�� ������(Protocol�� ������ �κ��� ����)
        EncodeBody(pBuf, &Offset, &pTmpStatus.RecvCmdID);
        EncodeBody(pBuf, &Offset, &pTmpStatus.ACKResult);
        EncodeBody(pBuf, &Offset, &pTmpStatus.ACKErrorCode);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Encode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_ENCODE_LENGTH;
        }

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        EncodeBody(pBuf, &Offset, &BodySize);

        RtnBytes = Offset; // Encode�� �� bytes ����
    }
    else
    {
        memcpy(pBuf + Offset, pSndData, SndHeader.Length);

        // by zzang9un 2013.03.15 : ���������� Tail ���� �߰�
        memcpy(pBuf + Offset + BodySize, &SndHeader.Length, LEN_TAIL);

        RtnBytes = Offset + SndHeader.Length + LEN_TAIL; // Encode�� �� bytes ����
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   ������ Status(OHT->OCS)�� Decode�ϴ� �Լ�

         OCS Emulator�� ���� �Լ�
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeStatus_OHT2OCS(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2OCS *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2OCS);

    // -------------------------------------------
    // Step 0. Error ó��
    if(pHeader->uTag.Info.CmdData != TAG_CMDDATA_STATUS_OCS)
        return ERR_TAG_NAME;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.

        // by zzang9un 2013.05.14 : OHT �⺻ ����
        // by zzang9un 2013.03.21 : Mode, Status, Command ID�� �ѹ��� Decoding
        DecodeBody(pBuf, &Offset, &pRcvData->Data.MSCmdID.Data);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.NextCmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurNodeOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CurStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.StopNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.StopStationID);
		DecodeBody(pBuf, &Offset, &pRcvData->Data.StopStationOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.ExistFoup);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.CarrierType);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.PauseStatus);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.SteerPosition);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.ErrorCode);
        DecodeBody(pBuf, &Offset, &pRcvData->Data.OriginInfo);

        // �߰� ����
        DecodeBodyStr(pBuf, &Offset, pRcvData->RFTagData, sizeof(pRcvData->RFTagData));
        DecodeBody(pBuf, &Offset, &pRcvData->APSignalLevel);
        DecodeBodyStr(pBuf, &Offset, pRcvData->APMacAddr, sizeof(pRcvData->APMacAddr));
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Drv_F_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Drv_F_Speed);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Hoist_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Shift_Position);
        DecodeBody(pBuf, &Offset, &pRcvData->Motor_Rotate);

        // OHT Ÿ��
        DecodeBody(pBuf, &Offset, &pRcvData->OHTType);

        // by zzang9un 2013.09.06 : Data ���� ����
        DecodeBody(pBuf, &Offset, &pRcvData->MapData_CurRevision);
        DecodeBody(pBuf, &Offset, &pRcvData->StationData_CurRevision);
        DecodeBody(pBuf, &Offset, &pRcvData->TeachingData_CurRevision);
        
        // by zzang9un 2013.05.14 : HOHT�� �����Ǵ� 7.0 Ư�� Status
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Erec);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Edc);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Idc);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Ecap);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_Kw);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_ErrorCode);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_PickupTemper);
        DecodeBody(pBuf, &Offset, &pRcvData->HID_CPUTemper);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[0]);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[1]);
        DecodeBody(pBuf, &Offset, &pRcvData->Input[2]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[0]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[1]);
        DecodeBody(pBuf, &Offset, &pRcvData->Output[2]);
        
		if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;

}
//---------------------------------------------------------------------------

/**
@brief   ������ Status(OHT->TP)�� Decode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.05
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(Template)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeStatus_OHT2TP(char *pBuf,
                       CMD_HEADER *pHeader,
                       MSG_STATUS_OHT2TP *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(MSG_STATUS_OHT2TP);

    // -------------------------------------------
    // Step 0. Error ó��
    if(pHeader->uTag.Info.CmdData != TAG_CMDDATA_STATUS_TP)
        return ERR_TAG_NAME;

	// Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // by zzang9un 2013.01.31 : OHT�� TP�� Endian�� Covert�� �ʿ䰡 ����
        // ���� �ʿ��� ��츦 ���� �ּ� ó���� ��
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;

}
//---------------------------------------------------------------------------

/**
@brief   ���� ���� Commmand�� Decode�ϴ� �Լ�

         ���� Command�� �� �Լ��� �̿��Ͽ� Decode�Ѵ�.
          - Go, Go More ���
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(GO_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeGoCmd(char *pBuf,
                       CMD_HEADER *pHeader,
                       GO_CMD *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
	int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(GO_CMD);

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ���� Type�� �ƴ� ��� ���� ó��
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_DRVING)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.
        DecodeBody(pBuf, &Offset, &pRcvData->CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationOffset);
        DecodeBody(pBuf, &Offset, &pRcvData->StopStationType);
        DecodeBody(pBuf, &Offset, &pRcvData->NextNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->LineSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->CurveSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->AccelationLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->FinalPortType);
        DecodeBody(pBuf, &Offset, &pRcvData->TPosition_Shift);
        DecodeBody(pBuf, &Offset, &pRcvData->PreSteeringNodeID);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
	{
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   ���� ��Ÿ ���� Commmand�� Decode�ϴ� �Լ�

         ���� Command�� �� �Լ��� �̿��Ͽ� Decode�Ѵ�.
          - Go MTL, Map Make, Clean ���
@author  zzang9un
@date    2013.04.12
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(uGO_ETC_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeGoEtcCmd(char * pBuf,
                       CMD_HEADER *pHeader,
                       uGO_ETC_CMD *pRcvData,
                       bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(uGO_ETC_CMD);

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ���� Type�� �ƴ� ��� ���� ó��
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_DRVING_ETC)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
	int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->GoEtcCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   ������ ���� Command�� Decode�ϴ� �Լ�

         ���� Command�� �� �Լ��� �̿��Ͽ� Decode�Ѵ�.
          - Load, Unload ���
@author  zzang9un
@date    2012.11.12
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(TRANS_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
*/
int DecodeTransCmd(char *pBuf,
                        CMD_HEADER *pHeader,
						TRANS_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(TRANS_CMD);

    // -------------------------------------------
    // Step 0. Error ó��
    // Command Type�� ������ Type�� �ƴ� ��� ���� ó��
    if(pHeader->uTag.Info.CmdType != TAG_CMDTYPE_TRANS)
        return ERR_TAG_CMDTYPE;

    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.
        DecodeBody(pBuf, &Offset, &pRcvData->CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->CurNodeID);
        DecodeBody(pBuf, &Offset, &pRcvData->CurStationID);
        DecodeBody(pBuf, &Offset, &pRcvData->PortType);
        DecodeBody(pBuf, &Offset, &pRcvData->CarrierType);
        DecodeBody(pBuf, &Offset, &pRcvData->PIODirection);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Hoist);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Shift);
        DecodeBody(pBuf, &Offset, &pRcvData->Tposition_Rotate);
        DecodeBody(pBuf, &Offset, &pRcvData->HoistSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->ShiftSpeedLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->PIOTimeLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->LookDownLevel);
        DecodeBody(pBuf, &Offset, &pRcvData->TransOption.Data);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
		}

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   OCS ���� Command�� Decode�ϴ� �Լ�

         ���� Command�� �� �Լ��� �̿��Ͽ� Decode�Ѵ�.
         E-Stop, Cancel, ID Reset, Pause, Resume ���
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(uTP_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
@note    Decode�� �� union ����ü�� CMD_COMMON�� ����Ѵ�.
*/
int DecodeOCSCmd(char *pBuf,
                        CMD_HEADER *pHeader,
                        uOCS_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(uOCS_CMD);

    // -------------------------------------------
    // Step 0. Error ó��
    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

	// -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.cData);
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->OCSCmd.Data2);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}
//---------------------------------------------------------------------------

/**
@brief   TP ���� Command�� Decode�ϴ� �Լ�
@author  zzang9un
@date    2012.11.14
@param   pBuf : Decode�� ������ �ּ�
@param   pHeader : ������ Command�� Header
@param   pRcvData : Decode�� Data(uTP_CMD)
@param   IsConvertEndian : Endian ��ȯ�� �ʿ��� ���(default:false)
@return  ������ ��� Decode�� �� bytes ��, ���� �� �����ڵ� ����
@note    Decode�� �� union ����ü�� CMD_COMMON�� ����Ѵ�.
*/
int DecodeTPCmd(char *pBuf,
                        CMD_HEADER *pHeader,
						uTP_CMD *pRcvData,
                        bool IsConvertEndian /* = flase */)
{
    int RtnBytes = 0; // Decode�� �� bytes
    WORD BodySize = sizeof(uTP_CMD);

    // -------------------------------------------
    // Step 0. Error ó��
    // Body�� Length�� Ʋ�� ��� ���� ó��
    if(pHeader->Length != BodySize)
        return ERR_BODY_LENGTH;

    // -------------------------------------------
    // Step 1. Body Decode
    int Offset = LEN_HEADER; // memcpy�ϱ� ���� buf�� Offset

    if(IsConvertEndian)
    {
        // ����ü ���ǵ� ������ �����Ͽ� Decode�ؾ� ��.
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.CmdID);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data1);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data2);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data3);
        DecodeBody(pBuf, &Offset, &pRcvData->TPCmd.Data4);

        if(BodySize != (Offset - LEN_HEADER))
        {
            // Decode�� bytes�� body�� �ٸ� ��� ���� ó��
            return ERR_DECODE_LENGTH;
        }

        RtnBytes = Offset;
    }
    else
    {
        memcpy(pRcvData, pBuf + Offset, pHeader->Length);
        RtnBytes = Offset + pHeader->Length;
    }

    return RtnBytes;
}


//---------------------------------------------------------------------------


#pragma package(smart_init)
