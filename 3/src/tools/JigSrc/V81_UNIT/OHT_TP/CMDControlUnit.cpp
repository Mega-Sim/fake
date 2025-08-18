// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CMDControlUnit.h"
#include "SocketUnit.h"
#include "StatusUnit.h"
#include "OHTInfoUnit.h"

// #include "StatusUnit.cpp"
#include "UDP.h"
// ---------------------------------------------------------------------------
TCMDCONTROL::TCMDCONTROL()
{
    memset(&sSndHeader, 0x00, sizeof(sSndHeader));
}

// ---------------------------------------------------------------------------
TCMDCONTROL::~TCMDCONTROL()
{
}

// ---------------------------------------------------------------------------

/**
@brief   TP Cmd�� Assistant�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.11.07
@param   CmdType : Command Type
@note    Command Type������ Cmd�� �����ؾ� �ϴ� ��쿡�� ����Ѵ�.
*/
void TCMDCONTROL::MakeTPCmd2AST(int CmdType, uTP_CMD *pTPCmd)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen = 0;

    uTAG_DATA SndTAG;
    SndTAG.Data = 0;

    // Tag ����
    SndTAG.Info.Sender = TAG_SENDER_TP;
    SndTAG.Info.Receiver = TAG_RECEIVER_ASSISTANT;
    SndTAG.Info.ProtoType = TAG_PROTOTYPE_REQ;
    SndTAG.Info.CmdType = TAG_CMDTYPE_TPCMD;
    SndTAG.Info.CmdData = CmdType;

	pTPCmd->TPCmd.CmdID = 0;

	// Header ����
	sSndHeader = MakeHeader(SndTAG.Data, sizeof(uTP_CMD));
	nSndLen    = EncodeASTCmd(SndBuf, &sSndHeader, pTPCmd, false);

	SocketForm->SendTPCmd2AST(SndBuf, nSndLen);
}
// ---------------------------------------------------------------------------
void TCMDCONTROL::MakeTP_Quick_Cmd2OHT(int CmdType)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OCS2OHT);
	int iReturn, iTemp;
	
	PKT_OCS2OHT SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH); 

	switch(CmdType)
	{
		case TPCMD_OHT_ESTOP:
			SEND_PACKET.TAG.Value = TagID::CMD_E_STOP_TP_OHT;
		break;

		case TPCMD_OHT_MANUALMODE:
			SEND_PACKET.TAG.Value = TagID::CMD_MANUALMODE_TP_OHT;
		break;

		case TPCMD_OHT_AUTOMODE:
			SEND_PACKET.TAG.Value = TagID::CMD_AUTOMODE_TP_OHT;
		break;

		case TPCMD_OHT_RESET:
			SEND_PACKET.TAG.Value = TagID::CMD_SOFTRESET_TP_OHT;
		break;
		case TPCMD_OHT_STATUS_REQ:
			SEND_PACKET.TAG.Value = TagID::CMD_STATUSREQ_TP_OHT;
		break;

		case TPCMD_OHT_INIT_INFO_REQ:
			SEND_PACKET.TAG.Value = TagID::CMD_INITREQ_TP_OHT;
		break;

		case TPCMD_OHT_DATA_IOMAP:
			SEND_PACKET.TAG.Value = TagID::CMD_IOMAP_REQ_TP2OHT;
		break;

		case TPCMD_CID_MONITORING_CMD:
			SEND_PACKET.TAG.Value = TagID::CMD_CID_M_COMMAND_SEND;
		break;

		case TPCMD_CID_MONITORING_READ:
			SEND_PACKET.TAG.Value = TagID::CMD_CID_M_COMMAND_RESULT_READ;
		break;

		case TPCMD_FWU_VERSION_REQ:
			SEND_PACKET.TAG.Value = TagID::CMD_VERSIONREQ_TP_OHT;
		break;

		case TPCMD_AUTOTEACHING_FILE:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING_FILE;
		break;

		case TPCMD_AUTOTEACHING:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING;
		break;

		case TPCMD_MAPMAKER_FILE:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_MAPMAKER_FILE;
		break;

		case TPCMD_AUTOTEACHING_DRIVING_READ:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_AUTOTEACHING_READ_DRIVING;
		break;

		case TPCMD_OHT_VIB_DATA_LINK:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_LINK;
		break;

		case TPCMD_OHT_VIB_DATA_REQ_START:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_REQ_START;
		break;

		case TPCMD_OHT_VIB_DATA_REQ_STOP:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_REQ_STOP;
		break;

		case TPCMD_OHT_VIB_DATA_CLOSE:
			SEND_PACKET.TAG.Value = TagID::CMD_TP_VIB_DATA_CLOSE;
		break;

	}
	
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);
}

// ---------------------------------------------------------------------------
void TCMDCONTROL::MakeTP_IO_SET_Cmd2OHT(int iIoType, int iIndex, int iValue)
{
	char buf[MAX_PACKET_SIZE+1];
	int iPacketSize = sizeof(PKT_OHT2TP_IO_SET);
	int iReturn, iTemp;
	
	PKT_OHT2TP_IO_SET SEND_PACKET;
	memset(&SEND_PACKET, 0, iPacketSize);	// ��Ŷ �޸� Ŭ����
	
	SEND_PACKET.HEADER.MachineType = MACHINETYPE_TP;
	memcpy(SEND_PACKET.HEADER.MachineID, "PC_TP", MACHINE_ID_LENGTH); 

	switch(iIoType)
	{
		case IO_TYPE::ON_OFF_TYPE:
			SEND_PACKET.TAG.Value = TagID::CMD_IOMAP_USE_ONOFF_TP2OHT;
		break;

		case IO_TYPE::ZERO_OR_ONE_TYPE:
			SEND_PACKET.TAG.Value = TagID::CMD_IOMAP_SET_RESET_TP2OHT;
		break;
	}

	SEND_PACKET.BODY.iIoNum = iIndex;
	SEND_PACKET.BODY.iOnOff = iValue;
	
	iReturn = frmUDP->UnicastSend((char*)&SEND_PACKET, iPacketSize);
}

// ---------------------------------------------------------------------------
void TCMDCONTROL::MakeTP_RAW_SEND_Cmd2OHT(char* buf, int len)
{
	int iReturn;
	
	iReturn = frmUDP->UnicastSend(buf, len);
}
// ---------------------------------------------------------------------------

/**
@brief   TP Cmd�� OHT�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.03.18
@param   CmdType : Command Type
@note    Command Type������ Cmd�� �����ؾ� �ϴ� ��쿡�� ����Ѵ�.
*/
void TCMDCONTROL::MakeTPCmd2OHT(int CmdType)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen      = 0;
	int TAG          = TP_TAG;
	int CheckCMDType = CHECK_CMDTYPE;

	uTP_CMD SndTPCmd;

	// int iTmpSize = sizeof(uTP_CMD);
	
	// by zzang9un 2012.11.24 : Data �ʱ�ȭ�� memset���� �����ϹǷ� �ּ�ó��
	memset(&SndTPCmd, 0x00, sizeof(uTP_CMD));

	// by puting 2012.12.16 : Sand CMD ���� Check
	if(CmdType != TPCMD_OHT_STATUS)
		SndTPCmd.TPCmd.CmdID = gm_OHTStatus->nSndCMDID++;

	// by puting 2012.12.16 : Sand CMD ���� �ʱ�ȭ
	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;

	StatusForm->panSndCmdID->Caption = IntToStr(SndTPCmd.TPCmd.CmdID);

	// by puting 2012.12.16 :  Tag ����
	CheckCMDType = (CheckCMDType & CmdType) << 4;
	TAG          = TAG + CheckCMDType + CmdType;

	// CmmandType���� ����ü ���� puting
	sSndHeader = MakeHeader(TAG, sizeof(uTP_CMD));
	nSndLen    = EncodeTPCmd(SndBuf, &sSndHeader, &SndTPCmd, false);

	MainForm->timerStatusRequest->Enabled = false;

	SocketForm->SendTPCmd2OHT(SndBuf, nSndLen);

	MainForm->timerStatusRequest->Enabled = true;
}
// ---------------------------------------------------------------------------

/**
@brief   TP Cmd�� OHT�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.03.18
@param   CmdType : Command Type
@param   pTPCmd : ������ Command�� ����ü ������
@note    Command Type�� ������ ����ü�� �����͸� ����Ѵ�.
*/
void TCMDCONTROL::MakeTPCmd2OHT(int CmdType, uTP_CMD *pTPCmd)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen      = 0;
	int TAG          = TP_TAG;
	int CheckCMDType = CHECK_CMDTYPE;

	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;

	// by zzang9un 2013.09.12 : Send Cmd ID üũ
	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;
    
	pTPCmd->TPCmd.CmdID = gm_OHTStatus->nSndCMDID++;
	StatusForm->panSndCmdID->Caption = IntToStr(pTPCmd->TPCmd.CmdID);

	// Tag ����
	CheckCMDType = (CheckCMDType & CmdType) << 4;
	TAG          = TAG + CheckCMDType + CmdType;

	// Header ����
	sSndHeader = MakeHeader(TAG, sizeof(uTP_CMD));
	nSndLen    = EncodeTPCmd(SndBuf, &sSndHeader, pTPCmd, false);

	MainForm->timerStatusRequest->Enabled = false;

	SocketForm->SendTPCmd2OHT(SndBuf, nSndLen);

	MainForm->timerStatusRequest->Enabled = true;
}
// ---------------------------------------------------------------------------

/**
@brief   Go Command�� OHT���� �����ϴ� �Լ�
@author  zzang9un
@date    2012.12.02
@param   nCommandType : Command type
@param   pGoCmd : Go Command ����ü
*/
void TCMDCONTROL::SendGoCmd2OHT(int nCommandType, GO_CMD *pGoCmd)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen      = 0;
	int TAG          = TP_TAG;
	int CheckCMDType = CHECK_CMDTYPE;

	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;

	// by zzang9un 2013.09.12 : Send Cmd ID üũ
	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;
    
	pGoCmd->CmdID = gm_OHTStatus->nSndCMDID++;
	StatusForm->panSndCmdID->Caption = IntToStr(pGoCmd->CmdID);

	// Tag ����
	CheckCMDType = (CheckCMDType & nCommandType) << 4;
	TAG          = TAG + CheckCMDType + nCommandType;

	// Header ����
	sSndHeader = MakeHeader(TAG, sizeof(GO_CMD));
	nSndLen    = EncodeGoCmd(SndBuf, &sSndHeader, pGoCmd, false);

	MainForm->timerStatusRequest->Enabled = false;

	SocketForm->SendTPCmd2OHT(SndBuf, nSndLen);

	MainForm->timerStatusRequest->Enabled = true;
}
// ---------------------------------------------------------------------------

/**
 @brief   Go Command�� OHT���� �����ϴ� �Լ�
 @author  zzang9un
 @date    2012.12.02
 @param   nCommandType : Command type
 @param   pGoCmd : Go Command ����ü
 */
void TCMDCONTROL::SendGoCmdEtc2OHT(int CmdType, uGO_ETC_CMD *pGoCmdEtc)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen      = 0;
	int TAG          = TP_TAG;
	int CheckCMDType = CHECK_CMDTYPE;

	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;

	// Command ID �߰�
	pGoCmdEtc->GoEtcCmd.CmdID        = gm_OHTStatus->nSndCMDID++;
	StatusForm->panSndCmdID->Caption = IntToStr(pGoCmdEtc->GoEtcCmd.CmdID);

	// Tag ����
	CheckCMDType = (CheckCMDType & CmdType) << 4;
	TAG          = TAG + CheckCMDType + CmdType;

	// Header ����
	sSndHeader = MakeHeader(TAG, sizeof(uGO_ETC_CMD));
	nSndLen    = EncodeGoEtcCmd(SndBuf, &sSndHeader, pGoCmdEtc, false);

	MainForm->timerStatusRequest->Enabled = false;

	SocketForm->SendTPCmd2OHT(SndBuf, nSndLen);

	MainForm->timerStatusRequest->Enabled = true;
}
// ---------------------------------------------------------------------------

/**
@brief   ������ Command�� OHT���� �����ϴ� �Լ�
@author  zzang9un
@date    2012.12.02
@param   nCommandType : Command type
@param   pTransCmd : Trans Command ����ü
*/
void TCMDCONTROL::SendTransCmd2OHT(int nCommandType, TRANS_CMD *pTransCmd)
{
	char SndBuf[LEN_TLV_BUFFER];
	int nSndLen      = 0;
	int TAG          = TP_TAG;
	int CheckCMDType = CHECK_CMDTYPE;

	if(gm_OHTStatus->nSndCMDID == 10)
		gm_OHTStatus->nSndCMDID = 1;

	// Command ID �߰�
	pTransCmd->CmdID                 = gm_OHTStatus->nSndCMDID++;
	StatusForm->panSndCmdID->Caption = IntToStr(pTransCmd->CmdID);

	// Tag ����
	CheckCMDType = (CheckCMDType & nCommandType) << 4;
	TAG          = TAG + CheckCMDType + nCommandType;

	// Header ����
	sSndHeader = MakeHeader(TAG, sizeof(TRANS_CMD));
	nSndLen    = EncodeTransCmd(SndBuf, &sSndHeader, pTransCmd, false);

	MainForm->timerStatusRequest->Enabled = false;

	SocketForm->SendTPCmd2OHT(SndBuf, nSndLen);

	MainForm->timerStatusRequest->Enabled = true;
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
