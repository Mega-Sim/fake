//---------------------------------------------------------------------------

#ifndef FMcontrolThreadUnitH
#define FMcontrolThreadUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>


typedef enum OHT_MAIN_RESTART_STEP_
{
	OHT_MAIN_STEP_NONE		= 0,	// no job
	
	OHT_MAIN_STEP_ORDER_RECEIVED,   // 1.wait
	OHT_MAIN_STEP_DELAY_10	   ,
	OHT_MAIN_STEP_DELAY_11	   ,
	OHT_MAIN_STEP_DELAY_12	   ,
	
	OHT_MAIN_STEP_CLOSE_ORDER  ,    // 2.AS -> OHT Close order
	OHT_MAIN_STEP_DELAY_20	   ,
	OHT_MAIN_STEP_DELAY_21	   ,
	OHT_MAIN_STEP_DELAY_22	   ,
	OHT_MAIN_STEP_DELAY_23	   ,
	
	OHT_MAIN_STEP_COPY		   ,	// 3.copy
	OHT_MAIN_STEP_DELAY_30	   ,
	OHT_MAIN_STEP_DELAY_31	   ,
	
	OHT_MAIN_STEP_MAIN_RUN	   ,	// 4.run & wait

	// 3sec delay
	OHT_MAIN_STEP_DELAY_41	   ,
	OHT_MAIN_STEP_DELAY_42	   ,
	OHT_MAIN_STEP_DELAY_43	   ,
	OHT_MAIN_STEP_DELAY_44	   ,
	OHT_MAIN_STEP_DELAY_45	   ,
	
	OHT_MAIN_STEP_DELAY_46	   ,	
	OHT_MAIN_STEP_DELAY_47	   ,
	OHT_MAIN_STEP_DELAY_48	   ,
	OHT_MAIN_STEP_DELAY_49	   ,
	OHT_MAIN_STEP_DELAY_50	   ,

	// 3sec delay
	OHT_MAIN_STEP_DELAY_51	   ,
	OHT_MAIN_STEP_DELAY_52	   ,
	OHT_MAIN_STEP_DELAY_53	   ,
	OHT_MAIN_STEP_DELAY_54	   ,
	OHT_MAIN_STEP_DELAY_55	   ,
	
	OHT_MAIN_STEP_DELAY_56	   ,
	OHT_MAIN_STEP_DELAY_57	   ,
	OHT_MAIN_STEP_DELAY_58	   ,
	OHT_MAIN_STEP_DELAY_59	   ,
	OHT_MAIN_STEP_DELAY_60	   ,

	// 1sec delay
	OHT_MAIN_STEP_DELAY_61	   ,	
	OHT_MAIN_STEP_DELAY_62	   ,
	OHT_MAIN_STEP_DELAY_63	   ,

	OHT_MAIN_STEP_MODE_AUTO	   ,	// 5.auto mode change
	OHT_MAIN_STEP_END		   ,	
	
}OHT_MAIN_RESTART_STEP;

// ============================= Firmware Update =======================================
typedef struct 
{
	int iMainUpdate;		// TRUE, FALSE. OHT Main�� ������Ʈ ���� ���θ� �����Ѵ�
	int iAmcUpdate;			// TRUE, FALSE. AMC�� ������Ʈ ���� ���θ� �����Ѵ�
	int iAsUpdate;			// TRUE, FALSE. ��ý��Ͻ��� ������Ʈ ���� ���θ� �����Ѵ�
	int iFmUpdate;			// TRUE, FALSE. File Manager�� ������Ʈ ���� ���θ� �����Ѵ�
	int iParamUpdate;		// TRUE, FALSE. �Ķ���͸� ������Ʈ ���� ���θ� �����Ѵ�
	
	int iWirelessConfig;	// TRUE, FALSE. ������� Config ������Ʈ ���θ� �����Ѵ�
	int iWirelessMacfil;	// TRUE, FALSE. ������� Macfil ������Ʈ ���θ� �����Ѵ�
	int iWirelessFirmware;	// TRUE, FALSE. ������� Firmware ������Ʈ ���θ� �����Ѵ�

	int iParamFileCount;	// ������Ʈ �� �Ķ���� ������ ����
	AnsiString strParamFileList[100]; // ������Ʈ �� �Ķ���� ���ϸ��� ����ϴ� ���� (�ִ� 99��)

} FIRMWARE_INFO;

struct UPDATEINI_CHECK
{
	bool bAsUpdate;
	bool bFmUpdate ;
	bool bPARAMUPATE ;
	bool bMainUpdate;
	bool bAmcUpdate ;
	bool bAmcDLLUpdate ;
	bool bWirelessConfig;
	bool bWirelessMacfil ;
	bool bWirelessFirmware ;
};

#define FIRMWARE_UPDATE_PATH	"C:\\User\\OHT\\DataFiles\\Down\\"
#define OHT_PARAM_FILE_PATH		"C:\\User\\OHT\\DataFiles\\"
#define OHT_PROGRAM_PATH		"C:\\User\\OHT\\"
#define OHT_MAIN_FILE_NAME 		L"OHT.exe"
#define FM_FILE_NAME L"FM.exe"
//AOHC-154
#define AMC_DLL_FILE_NAME       L"amc.dll"
#define AMC_LIB_FILE_NAME       L"amc.lib"


//---------------------------------------------------------------------------
class FMcontrolThreadUnit : public TThread
{
private:
	int ExecuteProgram(String strFileName);
	
protected:
	void __fastcall Execute();

public:
	__fastcall FMcontrolThreadUnit(bool CreateSuspended);


   UPDATEINI_CHECK m_UpdateIniChk;
   DWORD dwSTTime;
   int FirmwareUpdate_PARAM(FIRMWARE_INFO& FIRMWARE_UPDATE_INFO);
   int LoadUpdateIniPARAMFile(TIniFile *FwIniFile, FIRMWARE_INFO* Info);
   int LoadUpdateIniFile(AnsiString strINIFilePath);
   void UdpShow();
   void UdpHide();


};
//---------------------------------------------------------------------------
extern FMcontrolThreadUnit *ptrFMcontrolThread;
//---------------------------------------------------------------------------
#endif
