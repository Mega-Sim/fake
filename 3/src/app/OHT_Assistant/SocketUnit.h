//---------------------------------------------------------------------------

#ifndef SocketUnitH
#define SocketUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>

#include "OHT_TLV.h"
#include "../Common/Def_Protocol.h"
#include "Utility.h"
#include "../Common/Def_TPCommand.h"


//---------------------------------------------------------------------------
class TSocketForm : public TForm
{
__published:	// IDE-managed Components
	TServerSocket *m_ServerSocketDM;
	TServerSocket *m_ServerSocketTP;

	void __fastcall TPSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall TPSocketClientError(TObject *Sender, TCustomWinSocket *Socket,
		TErrorEvent ErrorEvent, int &ErrorCode);

	void __fastcall DMSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall DMSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall DMSocketClientError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall DMSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall FormDestroy(TObject *Sender);

private:	// User declarations
	// by zzang9un 2013.10.10
	OHTLogFile *m_Log;     ///< Log ������ ���� Ŭ����

    // by zzang9un 2013.10.12 : ������ ���� ������
	int m_FileSize;

	bool g_bIsTPServerConnected;
	bool m_bDMConnected;    // Data Manager ���� Ȯ�� ����
	// by zzang9un 2013.04.11 : TP Command ����
	bool m_TPConnected;    ///< TP�� ���� ���θ� �Ǵ��ϴ� ����

	// by zzang9un 2013.10.14 : ���� ���ۺ��� ū TLV�� ���� Flag
	bool bBigTLV_DM;               ///< ���� ���ۺ��� ū TLV�� ���� Flag
	CMD_HEADER m_PreCmdHeader_DM;  ///< ���� CmdHeader
	char *pBigTLV_DM;              ///< TLV ������ ����� ���� ������
	int m_BigTLVTotalLen_DM;       ///< TLV ��Ŷ�� �� SIZE
	int m_BigTLVReadCnt_DM;        ///< Big TLV�� Read�� Byte ��
	void InitBigTLV_DM();      ///< Big TLV ���� ���� �ʱ�ȭ �Լ�

	// by zzang9un 2013.11.07 : TP�� ����� ���� �߰���
	///@todo Socket���� �Ʒ� �������� �־�� ��(����ȭ �ʿ�)
	bool bBigTLV_TP;               ///< ���� ���ۺ��� ū TLV�� ���� Flag
	CMD_HEADER m_PreCmdHeader_TP;  ///< ���� CmdHeader
	char *pBigTLV_TP;              ///< TLV ������ ����� ���� ������
	int m_BigTLVTotalLen_TP;       ///< TLV ��Ŷ�� �� SIZE
	int m_BigTLVReadCnt_TP;        ///< Big TLV�� Read�� Byte ��
	void InitBigTLV_TP();      ///< Big TLV ���� ���� �ʱ�ȭ �Լ�

	// TP Command ���� �Լ�
	int MakeACK2TP(ACK_DATA SndACKData, char *pSndBuf);

	// by zzang9un 2013.10.10 : Command ���� �Լ�
	int HandleDMCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int HandleTPCmd(char *pRecvBuf, TCustomWinSocket *ClientSocket);

	// by zzang9un 2013.10.11 : ��� Log ���� �Լ�
	void AddTLVCommLog(AnsiString strTitle, char *pBuf, int size);

	// by zzang9un 2013.10.12 : Data Manager�� ���� Read �Լ�
	//int ReadDMSocket(TCustomWinSocket *pSocket, int *Bytes, bool IsConvertEndian /* = flase */)

	// by zzang9un 2013.10.10 : Data ���� �Լ�
	int SendDataVerionInfo(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int SendDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int SendBackupDataFile(char *pRecvBuf, TCustomWinSocket *ClientSocket);
	int MakeFile2TLV(char *pBuf, String strFileName, int FileSize);
	int SendACKDataUpload(char *pRecvBuf, TCustomWinSocket *ClientSocket);

	int ReceiveDataFile(char *pRecvBuf);
	int MakeTLV2File(char *pBuf, BYTE DataType);

	void SendFBWFStatus(String cmd);
	void SendSetDisableFBWFStatus();
	void SendSetEnableFBWFStatus();
	void SendFileExists(String strFileName);
	void SendOHTAdditionalStatus();
	void SendCurrentTime(uTP_CMD& data);
	void SendCloseOHTProgram();

	//2015.05.12 doori.shin : TLV Class
    TLV* m_TLV_DM;

public:		// User declarations
	__fastcall TSocketForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSocketForm *SocketForm;
//---------------------------------------------------------------------------
#endif
