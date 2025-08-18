//---------------------------------------------------------------------------

#ifndef FileManagerMainH
#define FileManagerMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdFTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include "UDP.h"
#include "FtpClient.h"
#include "FtpClass.h"
#include <Menus.hpp>
#include <IniFiles.hpp>

#include "VersionInfo.h"
#include "Utility.h"

#include "zip.h"
#include "unzip.h"


// ----------------------------------------------------------
#define WIRELESS_PARAM_FILE_PATH	"C:\\User\\OHT\\DataFiles\\"

// for test file names
/*
#define WIRELESS_CONFIG_FILE_NAME	L"CONFIG.TST"
#define WIRELESS_MACFIL_FILE_NAME	L"MACFIL.TST"
#define WIRELESS_FIRMWARE_FILE_NAME L"FIRMWARE.BIN.TST"
*/

#define WIRELESS_CONFIG_FILE_NAME	L"CONFIG"
#define WIRELESS_MACFIL_FILE_NAME	L"MACFIL"
#define WIRELESS_FIRMWARE_FILE_NAME L"FIRMWARE.BIN"

#define FM_VER_FILE_PATH		"C:\\User\\OHT\\DataFiles\\FmInfo.dat"
#define FILE_PATH_AST_PARAM		"./DataFiles\\AssistantParam.ini"
// -------------------------------------------------------------





#pragma package(smart_init)
//---------------------------------------------------------------------------
class TFileManagerForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *MainTimer;
	TMemo *Memo1;
	TIdFTP *IdFTP1;
	TTrayIcon *TrayIconFileManager;
	TPopupMenu *popupTrayIcon;
	TMenuItem *Exit1;
	TButton *btnClear;
	TIdTCPClient *IdTCPClient1;
	TButton *Button1;
	TIdFTP *IdFTP2;
	TButton *btnShow;
	TButton *btnHide;
	TTimer *trmMinimize;
	void __fastcall MainTimerTimer(TObject *Sender);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnClearClick(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall btnShowClick(TObject *Sender);
	void __fastcall btnHideClick(TObject *Sender);
	void __fastcall trmMinimizeTimer(TObject *Sender);

private:	// User declarations
	// shkim. 2016.12.12 �����б� ����� ���� �߰�
	TVersionInfo VerInfo;
public:		// User declarations
	__fastcall TFileManagerForm(TComponent* Owner);

};
//---------------------------------------------------------------------------

class FMThreadUnit : public TThread
{
private:
  	OHTLogFile *m_Log;     ///< Log ������ ���� Ŭ����
protected:
	void __fastcall Execute();
public:
	__fastcall FMThreadUnit(bool CreateSuspended);
	DWORD dwSTTime;
	UINT uiFTPMode;
	UINT uiFTPTransSatus;
	UINT uiFTPStep;
	UINT uiRetryCount;
	UINT uiTranOrderResponseStatus;//������������ �޽��� �۽� ���� Ȯ�ο�
	PKT_STATUS_REPORT_ID_03 g_UDP_PKT_Common;
	PKT_DATA_TRANSPER_ORDER_OCS2OHT g_UDP_PKT_Order;
	void DataTransperResponse(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order, UINT BodyAck);
	void DataTransperResponseProgress(PKT_DATA_TRANSPER_ORDER_OCS2OHT UDP_PKT_Order, UINT FTPTranStaus, UINT BodyAck);

	PKT_RESTART_FTP_PUT_GET_INFO_OHT2AS g_UDP_PKT_Wireless_Update;
	void WirelessUpdateResult(int iResult, AnsiString strResult);
};
//---------------------------------------------------------------------------

class MCCThreadUnit : public TThread
{
private:
	bool	zipflag;
	bool 	firstDelete;
	OHTLogFile *m_Log;
	SYSTEMTIME LastTime;

	TIniFile *m_IniFile;

	int nMaxWaitTimeToDelete;
	int nCompressDelay;
protected:
	void __fastcall Execute();
public:
	__fastcall MCCThreadUnit(bool CreateSuspended);
	void CompressMCCLog();
	void CompressOtherLog();
	void DeleteOldZip();
	void DeleteZip();
	void DeleteZip_OneDay();
	void LoadAssistantParam();
	void CompressAutoRecoveryLog();
};
//---------------------------------------------------------------------------
extern PACKAGE TFileManagerForm *FileManagerForm;
extern FMThreadUnit *ptrFMThread;

//---------------------------------------------------------------------------
#endif
