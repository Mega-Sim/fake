//---------------------------------------------------------------------------
/**
@file    Obstacle.h
@version OHT 7.0
@brief   Obstacle Class Header File
*/

#ifndef ObstacleH
#define ObstacleH

#include "HWCommon.h"
#include "VirtualDetect.h"
#include "CommHandler.h"

#define OBS_ComPort     9 ///< ������������
#define OBS_Baudrate    38400
#define OBS_Command_Packet_Size 4
#define	OBS_Final_Flag (0x0A)
#define	OBS_Target      4
#define OBS_ReceiveDataSize 1547
#define OBS_Start_Flag_1 (0x24)
#define OBS_Start_Flag_2 (0x47)
#define OBS_Sensor_Data_Num 513
#define OBS_Sensor_Data_StartPacketSize 6
#define OBS_Sensor_Data 3
#define OBS_Sensor_Request_Data_Interval 800
#define ASCII_ZERO 0x30

#define OBSLOGTIME 1000 //OBS LOG ����ϴ� �� ����

/**
@class   Obstacle
@date    2012.10.22
@author  LSJ
@brief   Obstacle Sensor ���� Class
@note    Obstacle Sensor�� Area�� �����ϰ� �����ϱ� ���� �Լ� ����
*/
class Obstacle:VirtualDetect
{
private:
	HWCommon *m_pHWCommon;

	//�ű����ܸ���͸� �׸�(���� ���� �����ð�)
	long m_nStartEnableTime;
    Comm_Port *m_pCommPort;
    bool bConnect;
    int m_nComPortNum;
	int m_nBaudRate;
    LogUnit *m_pLogObstacle;

public:

	Obstacle(HWCommon *pHWCommon);
	~Obstacle();

	int				Enable();						// Ȱ��ȭ(���� ON)
	int				Disable();						// ��Ȱ��ȭ(���� OFF)

	DETECT_STATUS	GetStatus();					// ���¸� �޾ƿ��� �Լ�
	void			SetArea(DETECT_AREA_TYPE Type);	// ������ �����ϴ� �Լ�
	double			GetMovableDist();			// �̵� ������ �Ÿ��� �޾ƿ��� �Լ�
	int				GetObsAreaStatus();				// ���¸� �޾ƿ��� �Լ�	

	int	PowerControl(int nMode);
    int ErrorReset();
	int AreaSelect(int nArea);
    bool IsDetect(int nLevel);
	bool CheckError();
    int OpenCom(void);
    int Decode(int start, char *code, int byte);
    int CheckObsSensorData(void);
    int CheckObsSensorData(int *Buff);
    int ReceiveData(char *ReadBuff);
    void RequestData(void);
    int CloseCom(void);
    int UpdataData( int *Buff, bool bIsStart = false);
    void DataReadStart(int *Buff);

	//�ű� ���ܸ���͸� �Լ�(���� ���� �����ð� ����)
	int GetEnableTime(bool bInit);
	int GetSensorLevel(bool bLong, bool bMiddle, bool bShort, bool bFail); //soming OBS ���� USER_LOG �߰�
   //	int GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);
	AnsiString GetOutputSensorLevel(bool bSel1, bool bSel2, bool bSel3, bool bSel4, bool bPower);

};

#endif  //ObstacleH
