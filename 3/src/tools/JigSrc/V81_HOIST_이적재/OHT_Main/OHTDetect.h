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

	int             OpenCom();                 //UBZ PORT�� opne�ϴ� �Լ�
	int             CloseCom();                 //UBZ PORT�� close�ϴ� �Լ�
	bool IsConnect(); //UBG ComOpen ���¿���  ���� ü���� �ʿ��� ��쿡 UBG Comclose ����

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
	int GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail);
	static int GetSensorLevel2(bool bLong, bool bMiddle, bool bShort, bool bFail);
   //	int GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
//	AnsiString GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
	bool IsDetect(int nLevel);
	bool CheckError();

	int GetEnableTime(bool bInit);

	bool CheckSensorOff(); //���� ���� Off ���� Ȯ�� �ϴ� �Լ�

	//UBG ���ռ� Code
	int 			Send_UBGData(unsigned char* ackData, int Length);				   //UBZ DATA�� �ޱ� ���� ��ȣ�� ������ �Լ�
	int				Recieve_UBGData(unsigned char* pReadBuff);			   //UBZ DATA�� �޾ƿ��� �Լ�


	//UBG Logging Code
	void DataReadStart(int *Buff); //��û
	int UpdataData(int *Buff,bool bIsStart = false);  //false RequestData(Loging ��û), true CheckUBG(Logging ����)
	void RequestData(void);
	int CheckUbgSensorData(int *Buff);
	int ReceiveData(char *ReadBuff);
	int GetUbgAreaStatus();  // Check


	int Decode(int start, char *code, int byte);
 	int CheckUBGConnect(bool bDisConnect); //AOHC-256 AutoMode �� UBG Fiemware ���� �Ұ�
};

#endif  // OHTDetectH


