/**
@file    Lookdown.h
@version OHT 7.0
@brief   Lookdown Class Header File
*/

#ifndef LookdownH
#define LookdownH

#include "HWCommon.h"
#include "CommHandler.h"

//PBS�� �Ϲ� ���ǵ�
#define	  NUM_PBS_DATA		        121					   //PBS�κ��� ������� �Ÿ� �������� ����
#define	  PBS_DIFF_ANGLE		    0.0314159265358979	//PBS������ ���� ���� ����.  1.8 deg
#define	  PBS_START_ANGLE		    -0.314159265358979	//������� �������� ���� ����
#define	PBS_BUFF_SIZE		        8192
#define	PBS_DISTOFFSET              100
#define	LINK_WAIT_TIME	            2400			//msec
#define LOOKDOWNLOGTIME  2000      //msec

/**
@class   Lookdown
@date    2012.10.30
@author  LSJ
@brief   Lookdown ���� ���� Class
@note    Lookdown ������ �����̳� Area�� ��Ʈ�� �ϴ� �Լ� ����
*/
class Lookdown
{
private:
	HWCommon *m_pHWCommon;
	//�ű����ܸ���͸� �׸�(Lookdown ���� ���� �����ð�)
	long m_nStartEnableTime;
	Comm_Port *m_pCommPort;
	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;
	int m_nStatus;

	unsigned short	CertifCode;					      //16bit �����ڵ�. PBS���� ���� code�� CRC ���� �ش��Ѵ�.
	char	DecodedData[PBS_BUFF_SIZE];		   //���ڵ��� �����͸� �����ϴ� ����.
	unsigned int				DistanceData[NUM_PBS_DATA];	//���������� ���� �Ÿ� ������
	int            NumDecodedData;

	DWORD			dwStartTime;			// Start Time
	DWORD			dwLastLinkTime;			// Start Time

	int m_nArea;
    LogUnit *m_pLogLookdown;
public:
	Lookdown(HWCommon *pHWCommon);
	~Lookdown();

	int PowerControl(int nMode);
	int Enable();
	int Disable();
	int AreaSelect(int nArea);
	bool IsDetect();
	bool IsError();

	//�ű����ܸ���͸� �׸�(Lookdown ���� ���� �����ð�)
	int GetEnableTime(bool bInit);

	void CheckSensorArea();
	void PBSDetect_ReadStart(void);
	void PBSDetect_ReadStop(void);
	int UpdataData(int *Buff);
private:
	void Init();
	int OpenCom();
	void CloseCom();
	void Obs_OnCommunication();
	void RequestCode(void);
	unsigned short LsbCRC(unsigned short , unsigned char *, unsigned short);
	void Encode(int , unsigned char *, unsigned char *);
	void Decode(int , unsigned char *, unsigned char *);
	int NumEncode(int);
	int NumDecode(int);
	bool OnGetCode(void);
	void RequestLink(void);
	bool OnGetLink(void);
	void RequestData(void);
	bool OnGetData(void);
	int ReadData(char *RtnReadBuff, char cTarget);
};

enum PBS_LOOKDOWN_STATUS
{
	STATUS_PBS_NOT_CONNTECTED = 1,
	STATUS_PBS_WAIT_CODE,
	STATUS_PBS_WAIT_FIRST_LINK_DATA1,
	STATUS_PBS_WAIT_FIRST_LINK_DATA2,
	STATUS_PBS_WAIT_LINK,
	STATUS_PBS_WAIT_DATA,
};


#define PBS_LINK_REQUEST_CYCLE_TIME 2400 // 3000ms ���� link ����
#define PBS_WAIT_CODE_TIME 	200
#define PBS_WAIT_LINK_TIME     300
#define PBS_WAIT_DATA_TIME     300


#endif  // LookdownH


