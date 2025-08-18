/**
@file    OHTDetect.h
@version OHT 7.0
@brief   OHTDetect Class Header File
*/

#ifndef OHTDetectH
#define OHTDetectH

#include "VirtualDetect.h"
#include "HWCommon.h"
#include "CommHandler.h"

#define UBG_ComPort     8 ///< ������������
#define UBG_Baudrate    38400
#define	UBG_Final_Flag (0x0A)
#define	UBG_Target      3

#define UBG_Final_Flag_Fine	(0x5D)
#define	UBG_Target_Fine     1

#define UBG_Command_Packet_Size 4
#define	UBG_Area_Target      4
#define UBG_ReceiveDataSize 1547
#define UBG_Start_Flag_1 (0x24)
#define UBG_Start_Flag_2 (0x47)
#define UBG_Sensor_Data_Num 513
#define UBG_Sensor_Data_StartPacketSize 6
#define UBG_Sensor_Data 3
#define UBG_Sensor_Request_Data_Interval 500
#define UBGLOGTIME 800			//msec
#define UBGCHECKTIME 3000 //msec

#define SOS_UBG_Frame_Data_Size  4036	//test ���� ����
#define SOS_UBG_Area_Data_Size 2200
#define SOS_UBG_Command_Data_Size 15
#define SOS_UBG_Recieve_Data_Size 15  * 2
#define SOS_UBG_CHECK_FRAMEENABLE  12




#define RETRYCOUNT 5
#define SOSMONITORING_FW_VERSION "SOSLAB[Ver 22.12.21]"

/**
@class   OHTDetect
@date    
@author  
@brief   OHTDetect ���� Class
*/


class OHTDetect:VirtualDetect
{
private:
	HWCommon *m_pHWCommon;
    Comm_Port *m_pCommPort;
	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;
	long m_nStartEnableTime;
	LogUnit *pLogDetect;
	LogUnit *pLogUBGAbnormalChk;
	int m_nRcvCount;

	int             OpenCom();                 //UBZ PORT�� opne�ϴ� �Լ�
	int             CloseCom();                 //UBZ PORT�� close�ϴ� �Լ�
	bool IsConnect(); //UBG ComOpen ���¿���  ���� ü���� �ʿ��� ��쿡 UBG Comclose ����
	bool m_bSOSVersionCheck;
	DWORD m_dwSOSLABCheckTime;
	bool m_bSOSlabVer2Monitoring;
	bool m_bStartOHTDetectIOCheck;
	void SetOHTDetectIOCheck(bool start);
	int SOSLABRetryCount;
	bool m_bInputCheckResult;
	bool m_bOutputCheckResult;

	int nCheckSOSInputLevel;
	int nCheckSOSOutputLevel;


	bool m_bSOSLABFrameFlag;
public:
	OHTDetect(HWCommon *pHWCommon);
	~OHTDetect();


	int				Enable();						// Ȱ��ȭ(���� ON)
	int				Disable();						// ��Ȱ��ȭ(���� OFF)

//	int				ReOpenCom();				   //Comport open ���н� ��õ� �ϴ� �Լ�



	DETECT_STATUS	GetStatus();					// ���¸� �޾ƿ��� �Լ�
	void			SetArea(DETECT_AREA_TYPE Type);	// ������ �����ϴ� �Լ�
	double			GetMovableDist();			// �̵� ������ �Ÿ��� �޾ƿ��� �Լ�
	bool 			GetSensorStatus();
	bool 			GetCheckStatus();
	int				GetStatusLevel();				   // ���¸� �޾ƿ��� �Լ�


	int PowerControl(int nMode);
	int ErrorReset();
	int AreaSelect(int nArea);
//	int GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail);          //�̻��
	int GetOutputSensorLevel(bool bArea1, bool bArea2, bool bArea3, bool bArea4);
	static int GetSensorLevel2(bool bLong, bool bMiddle, bool bShort, bool bFail);
	int GetSensorLevel_MCC();
	bool GetOutputSensorStright();
//	AnsiString GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
	bool IsDetect(int nLevel);
	bool IsRegion(int nLevel);
	bool CheckError();

//	int GetEnableTime(bool bInit);             //�̻��

	bool CheckSensorOff(); //���� ���� Off ���� Ȯ�� �ϴ� �Լ�

	//UBG ���ռ� Code
	int 			Send_UBGData(unsigned char* ackData, int Length);				   //UBZ DATA�� �ޱ� ���� ��ȣ�� ������ �Լ�
	int				Recieve_UBGData(unsigned char* pReadBuff);			   //UBZ DATA�� �޾ƿ��� �Լ�
	int				Recieve_UBGData_Fine(unsigned char* pReadBuff);			   //UBZ DATA�� �޾ƿ��� �Լ�

	//UBG Logging Code
	void DataReadStart(int *Buff); //��û
	int OriginDataReadStart(char *Buff);     // ȭ�� ���� ���� ������
	int UpdataData(int *Buff,bool bIsStart = false);  //false RequestData(Loging ��û), true CheckUBG(Logging ����)
	void RequestData(void);
	int CheckUbgSensorData(int *Buff);
	int ReceiveData(char *ReadBuff);
	int GetUbgAreaStatus();  // Check


	int Decode(int start, char *code, int byte);
	int CheckUBGConnect(bool bDisConnect); //AOHC-256 AutoMode �� UBG Fiemware ���� �Ұ�

	unsigned char m_szGetBuff[2200];
	unsigned char m_szLoadBuff[2200];
	int GetAreaDataTest();
	unsigned char SOS_CheckSum(unsigned char *buf, int Length);
	int AreaLevelCaptureCommand();
	bool GetSOSVersion();
	AnsiString SOS_FW_VersionCheck();
   //	int SOS_FW_VersionCheck();
	int SOS_FW_VersionCheckCommand();

	int GetAreaLevelCapture();
	int CheckSOSLevel(int SensorLevel, int InputLevel);
	int CheckSOSArea(int SensorArea, int OutputArea);
	int AreaIndexCommand();
	int AreaIndexRecieve();
	int AreaIndexRecieve(unsigned char *ReadBuff);
	int GetAreaDataCommand();
	int GetAreaData();
	bool LoadSOSData();
	int StrToHex(char* szHex, int nDigit);
	bool CompData();
	bool GetVersion2Monitoring();
	bool CheckYearVersion(int year_version, int month_version, int day_version);
	bool ResultOHTDetectInputCheck();
	bool ResultOHTDetectOutputCheck();
	int RetryCountInfo();
	void SetRetryCountInfo(bool set);
	int GetRetryCountInfo();

	//AMOHTV80F-952 puting
	int SOSLABFrameDataEnableCommand(bool bEnable);
	int SOSLABFrameDataGetCommand();//(unsigned char *ReadBuff);  //����ü ���� �ʿ�...
	bool GetSOSLABFramEnableFlag();
	SOSLAB_FRAMEDATA m_SoslabFramData;

	void CheckUBGSeriInfo(int nStatus); //����� ���� ���� ���� Ȯ��
//	int CheckUBGIOSeriInfo(); //Main ON �Ҷ� ����� ���� ���� ���� Ȯ��    //UBG RESET ��� ������ ����
	char UBGSeriInfoOuput;
	int nStepCheckUBGSeriInfo;
//	int nStepCheckUBGIOSeriInfo;   //UBG RESET ��� ������ ����
//	bool bStepCheckUBGInfo;
//	int nStepCheckUBGInfo;
	bool CheckUBGOuputInfo();
	int GetUBGSelectStatus();  // Check

};

#endif  // OHTDetectH


