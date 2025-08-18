/**
@file    HID.h
$Rev:: 679           $:  Revision of last commit
$Author:: ltw070     $:  Author of last commit
$Date:: 2013-03-11 1#$:  Date of last commit
@version OHT 7.0
@brief   HID Class Header File
*/

#ifndef HID_VII_ZRH
#define HID_VII_ZRH

#include <Classes.hpp>
#include <dos.h>

#include "Virtual_HID.h"
#include "LogHandler.h" // Log �ۼ� ����
#include "ExecuteInfo.h"
#include "OHTMainStatus.h"

#include <time.h>

#include <fstream>
#include <iostream>

#include "PUtily.h"

#define SEND_PK_LENGTH70		   	18
#define RCV_PK_LENGTH70			 	44
#define RCV_PK_LENGTH85			 	80
#define RCV_PK_LENGTH_CP            85       //���� CP ver.
#define RCV_PK_LENGTH_ECO           143      //ECO ver.
#define RCV_PK_LENGTH_BMS			179      //�����Ŀ���� ver.


#define RCV_PK_PROTOCOL_LENSTD		50

//#include "atlstr.h"
//#pragma comment(lib, "GPProgrammer.lib")
//#include "GPProgrammer.h"

/**
@class   HID
@date    2012.10.13
@author  zzang9un
@brief   HID ���� Class
@note    Serial ������� HID Data�� �о���δ�.
*/

class HID_VII_ZR : public Virtual_HID
{
private:

	void GetSendData(char *Buff,int nMoveInfo, int nPathInfo);    					   //puting
	void GetSendData(char *SendData,int nMoveInfo, int nPathInfo, bool errorDataFlag);
	BYTE  CheckSUMData(char* SendData, int Len);   //puting
	bool  GetRcvDataCheckSum(char* RcvData, int Len);
	int ErrorConvert(int nErrorcode);
	void SendFirstData(char *SendData);


//	char  CheckSUMData_V85(char* SendData, int Len, int Num);   //puting
	bool  GetRcvDataCheckSum_V85(char* RcvData, int Len);

	char GetSendData_Type(int nType);
	char GetSendData_Path(int nType);

	EXECUTE_INFO *m_pExecuteInfo;
	OHTMainStatus *m_pOHTMainStatus;

	FDCUnit *pFDCUnit;

protected:
	int AnalysisData(char* chRcvData);


public:
	HID_VII_ZR(int ComPortNum);
	~HID_VII_ZR();


	int UpdateData(int nMoveInfo, int nPathInfo);
	HID_DATA GetHIDData();

	int SaveErrorDataLog(void);

	void SetRegulatorCheck(bool bUse);
	bool RegulatorCheck;           //regulator ���� üũ�� �ʿ����� Ȯ���ϴ� ����
	bool BMS_Ver;                  //regulator�� ECO or ���� �Ŀ� ��� �������� Ȯ���ϴ� ����
	bool GetVer;                   //ó�� �� �� ������ �����Դ��� Ȯ���ϴ� ����.
	int Ver_Cnt;                   //���� regulator�� �� ���� üũ Ƚ��
	bool bChargeEnable;                  //��� ����, ���� ����
	int bError;                   //���� �߻� ����
};

#endif  // HID_VII_ZR
