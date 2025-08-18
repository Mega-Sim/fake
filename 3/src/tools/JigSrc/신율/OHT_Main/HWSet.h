/**
@file    HWSet.h
@version OHT 7.0
@brief   HWSet Class Header File
*/

#ifndef HWSetH
#define HWSetH


#include "HWCommon.h"
#include "Def_DefaultIniParam.h"

#include "BCR.h"
#include "Bumper.h"
#include "Driving.h"
#include "DrivingTag.h"
#include "EQPIO.h"
#include "Fan.h"
#include "Hand.h"
#include "Hoist.h"
#include "InnerFoup.h"
#include "Lamp.h"
#include "Lookdown.h"
#include "Obstacle.h"
#include "Ocsillation.h"
#include "FoupCoverOpenDetect.h"
#include "OHTDetect.h"
#include "PowerControl.h"
#include "QR_Reader.h"
//#include "RailSupport.h"
#include "Remocon.h"
#include "LCD.h"
#include "Rotate.h"
#include "ServoMonitoring.h"
#include "Shift.h"
#include "Shutter.h"
#include "Steer.h"
#include "TrackBufferDetect.h"
#include "TransStopTag.h"
#include "PassBoxPIO.h"
#include "CID.h"
#include "MTLPIO.h"

/**
@class   HWSet
@date    2012.10.13
@author  zzang9un
@brief   OHT�� ���Ǵ� ��ü HW�� ������ Class
*/
class HWSet
{
private:
	HWCommon *m_pHWCommon;
	DEFAULT_PARAM_SET *m_defualtparam;

    void CreateHWSet();
    void DeleteHWSet();

public:

    BCR					*m_pTransBCR;
    BCR					*m_pDrivingBCR;
    Bumper				*m_pBumper;
	Driving				*m_pDrivingAxis;
	DrivingTag			*m_pDrivingTag;
	EQPIO				*m_pLeftEQPIO;
//	EQPIO				*m_pRightEQPIO;
	Fan					*m_pFan;
	Hand				*m_pHand;
//	HID *m_pHID;
	Hoist				*m_pHoistAxis;
	InnerFoup			*m_pInnerFoup;
	Lamp				*m_pLamp;
	Lookdown			*m_pLookdown;
	Obstacle			*m_pObstacle;
	FoupCoverOpenDetect	*m_pFoupCoverOpenDetect;
	Ocsillation		    *m_pOcsillation;
	OHTDetect			*m_pOHTDetect;
	PowerControl		*m_pPowerControl;
	QR_Reader			*m_pLeftQRR;
	QR_Reader			*m_pRightQRR;
//	RailSupport		    *m_pRailSupport;
	Remocon				*m_pRemocon;
	LCD					*m_pLCD;
	// 2016-12-05, Add by Labeler, ROHT�� ��� Rotate �߰��� ���� �߰�
	Rotate				*m_pRotate;	
	ServoMonitoring	    *m_pServoMonitoring;
    Shift				*m_pShiftAxis;
	Shutter				*m_pShutter;
    Steer				*m_pSteer;
    TrackBufferDetect	*m_pTrackBufferDetect;
    TransStopTag		*m_pTransStopTag;
	PassBoxPIO			*m_pPassBoxPIO;
	CID                 *m_pCID;
	MTLPIO				*m_pMTLPIO;

	int nIs_SounON;

    HWSet();
	~HWSet();
    void Sound_On(int nKindOfSound);
	void Sound_Off();

	int GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff, int& nOutCount);

    // by zzang9un 2013.10.07 : AMC ���� ����
	int GetAMCVersion();
	int GetFPGAVersion();
	int GetAMCPauseStatus();
	int GetAMCTorqueStatus();
	int GetAMCVHLDetectStatus();
	int GetAMCDetectStopDecelDownStatus();

	int GetAMCStopStatus();
	int GetVHLDetectForceIOStatus();
	int GetAMCDecelStatus();


	int GetVolatageMonitor(int Index);
    // ���¿� 2013.10.22 : TraceLog �ۼ�
	void MakeTraceLog();

	//2014.06.09 ji7412.lee BYTE AMCResponse
	BYTE m_AMCResponse;

};

#endif  // HWSetH
