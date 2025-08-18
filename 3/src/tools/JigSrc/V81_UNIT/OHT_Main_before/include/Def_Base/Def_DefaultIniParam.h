#ifndef Def_DefaultIniParamH
#define Def_DefaultIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM

/**
@brief	 ����Ʈ �Ķ���� 
@author  puting
@date	 2016.11.26
*/
struct DEFAULT_PARAM_SET
{
	unsigned char VHLName[7];
	unsigned char BroadCasMask[20];


	unsigned char OHTType[20];  //0 : None (error ó��), 1: Foup 8.0, 2: Reticle 8.0(EUV) , 3: Foup 8.1, 4: Reticle 8.5(EUV) 5: Foup 8.1(���ȭ)
	unsigned char LineType[20];

	int nPortNumAdd;

	//Diag Agent ����
	AnsiString strDiagIpAddr;
	int nDiagPort;

	bool IsLCDAndLED;    //add P1L VS P1_2L & P1_3L ���� �Ķ���� #JRespect 2018.01.09
	bool IsRotateSensor;
	int  nHoistHomeSensorOriginConutAdd;
	bool  bHoistHomeSensorOriginRetry;
	bool  IsIOTHub;
	bool VHLLifterPassEnable;
	//Auto Teaching
	unsigned char AutoTeachingIP[20];

	//��������
	bool PreEQPIO;
	bool PreShutter;

    //2016.12.20 sh17.jo ���� ��Ʈ��ũ ���� ���� ���� �Ķ���� �߰�
	unsigned char GatewayIP[20];
	unsigned char GatewayIP_OCS[20];	//2018.08.21 shcho : OCS�� gateway �ּ�. OCS ���� �ҷ��� Ping �õ��ϱ� ����
    unsigned char WirelessClientIP[20];
    unsigned char UserID[10];
    unsigned char Password[10];
    unsigned int  nMaxOCSRcvTime;
    unsigned int  nMaxPingCheckTime_Step1;
    unsigned int  nMaxPingCheckTime_Step2;
    unsigned int  nMaxPingCheckTime_Step3;
    unsigned int  nMaxPingCheckTime_ETC;

	// IOT �̴����� ������ ��Ʈ��ũ �̸�.
	// ���� �����Ǿ� ���� �ʴٸ� �⺻�� ù��° �̴��� ī�尡 IOT ��ī���� �����Ѵ�
	char IotEternetName[50];
	bool bIgnoreIoTLanCheck;    // IOT ����Ʈ���� �˻縦 �������� ����

	PROPOTINAL_GEAR_RATIO DrivingDefault;
	PROPOTINAL_GEAR_RATIO ShiftDefault;
	THREE_LINEAR_EQUATION_GEAR_RATIO HoistDefault;

	PROPOTINAL_GEAR_ROTATE RotateDefault;
	
};

#endif
