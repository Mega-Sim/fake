//---------------------------------------------------------------------------

#ifndef DiagManagerH
#define DiagManagerH

#include <vcl.h>
#include <map>
#include <IniFiles.hpp>
//#include "Def_UDPProtocol.h"
#include "Protocol_Define_Modify.h"
#include "List.h"
//#include "OHT_TLV.h"


#define DIAG_PARAM_FILE_PATH		"\\Datafiles\\DiagParam.ini"

//OHT ������� �׸� ������ ����
#define DIAG_DATA_ID_MOVEDISTANCE				0x10
#define DIAG_DATA_ID_MOVESTEERING				0x20
#define DIAG_DATA_ID_MOVESHIFT					0x30
#define DIAG_DATA_ID_REVSHIFT					0x31
#define DIAG_DATA_ID_MOVEHOIST					0x40
#define DIAG_DATA_ID_REVHOIST					0x41
#define DIAG_DATA_ID_MOVEROTATE					0x50
#define DIAG_DATA_ID_MOVEHAND					0x60
#define DIAG_DATA_ID_MOVESHUTTER				0x70
#define DIAG_DATA_ID_COUNTDRVBCR				0x80
#define DIAG_DATA_ID_COUNTSTABCR				0x81
#define DIAG_DATA_ID_TIMELOOKDOWN				0x82
#define DIAG_DATA_ID_TIMEUBG					0x84
#define DIAG_DATA_ID_TIMEPBS					0x85

//OHT ������� ������ �����ϴ� ���ذ�
#define	DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE		100			//km
#define DIAG_DEFAULT_REF_VALUE_MOVESTEERING		5000       	//Count
#define DIAG_DEFAULT_REF_VALUE_MOVESHIFT		2000       	//Count
#define DIAG_DEFAULT_REF_VALUE_REVSHIFT			2 	     	//1��
#define DIAG_DEFAULT_REF_VALUE_MOVEHOIST		2000		//Count
#define DIAG_DEFAULT_REF_VALUE_REVHOIST			10      	//1��
#define DIAG_DEFAULT_REF_VALUE_MOVEROTATE		2000        //Count
#define DIAG_DEFAULT_REF_VALUE_MOVEHAND			2000        //Count
#define DIAG_DEFAULT_REF_VALUE_MOVESHUTTER		2000        //Count
#define DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR		100        //Count
#define DIAG_DEFAULT_REF_VALUE_COUNTSTABCR		100        //Count
#define DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN		24      	//hours
#define DIAG_DEFAULT_REF_VALUE_TIMEUBG			24      	//hours
#define DIAG_DEFAULT_REF_VALUE_TIMEPBS			24      	//hours

//OHT ������� ������ ����
#define DIAG_REF_VALUE_UNIT_MOVEDISTANCE		1000000		// 1,000,000 mm = 1 km
#define DIAG_REF_VALUE_UNIT_MOVESTEERING		1
#define DIAG_REF_VALUE_UNIT_MOVESHIFT			1
#define DIAG_REF_VALUE_UNIT_REVSHIFT			10000		// 10,000 Revolution
#define DIAG_REF_VALUE_UNIT_MOVEHOIST			1
#define DIAG_REF_VALUE_UNIT_REVHOIST			10000		// 10,000 Revolution
#define DIAG_REF_VALUE_UNIT_MOVEROTATE		   	1
#define DIAG_REF_VALUE_UNIT_MOVEHAND			1
#define DIAG_REF_VALUE_UNIT_MOVESHUTTER			1
#define DIAG_REF_VALUE_UNIT_COUNTDRVBCR			10000       // 10,000 Count
#define DIAG_REF_VALUE_UNIT_COUNTSTABCR			10000		// 10,000 Count
#define DIAG_REF_VALUE_UNIT_TIMELOOKDOWN		3600000		//3,600,000 msec = 1 hour
#define DIAG_REF_VALUE_UNIT_TIMEUBG				3600000     //3,600,000 msec = 1 hour
#define DIAG_REF_VALUE_UNIT_TIMEPBS				3600000		//3,600,000 msec = 1 hour

//OHT �̻����� ������ ����
#define DIAG_ABNORMAL_DATA_ID_USING_MEMORY			0x10
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_HAND	0x20
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_SHUTTER	0x21
#define DIAG_ABNORMAL_DATA_ID_TIMEOVER_MOVE_STEER	0x22
#define DIAG_ABNORMAL_DATA_ID_ALARM_RESET_HAND		0x23
#define DIAG_ABNORMAL_DATA_ID_OUT_OF_PHASE_PBS		0x30
#define DIAG_ABNORMAL_DATA_ID_OUT_OF_PHASE_UBG		0x31
#define DIAG_ABNORMAL_DATA_ID_RETRY_BCR				0x32
#define DIAG_ABNORMAL_DATA_ID_DIFFERENCE_ENCODER	0x40

//OHT �̻����� ������ ���� ���ذ�
#define DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE			80		//Percent
#define DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT				5		//Count
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND			1500	//msec
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER		2000	//msec
#define DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING		5000	//msec
#define DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND			1		//Count
#define DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS			1		//Count
#define DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG			1       //Count
#define DIAG_ABNORMAL_REF_VALUE_RETRY_BCR					1       //Count
#define DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT		3       //Count
#define DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT	5		//Percent

//OHT Status ���� ��
#define DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE		75      //Percent
#define DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE		75      //Percent

//��Ÿ �Ķ����
#define PBS_OUT_OF_RANGE_TIMEOUT	1500	//PBSŻ���߻��� Abnormal�� ó���ϴ� Timeout �ð�

//OHT Status ����
#define DIAG_OHT_STATUS_NG	0
#define DIAG_OHT_STATUS_OK	1
#define DIAG_SEND_INTERVAL	24

//��Ÿ
#define DIAG_MAX_SIZE_ABNORMAL_DATA_LIST	300
#define DIAG_MAX_SIZE_PATROL_DATA_LIST		300
#define DIAG_CHECK_INIT_STAT_DATA_INTERVAL	10*1000			//10 sec
#define DIAG_CHECK_TIMER_RETRY_COUNT		3600*1000		//1 hour

//Error Code
#define	DIAG_ACK_OK					0x00
#define	DIAG_ACK_ERR_NO_DATA	    0x01
#define DIAG_ACK_DATA_LOGIC			0x02
#define	DIAG_ACK_ERR_UNKNOWN        0x09
#define DIAG_NO_ACK					0xFF

/**
@brief   ���� Agent ���ۿ� Patrol ������
@author  by doori.shin
@date    2015.11.30
*/
typedef struct PATROL_DATA_
{
	WORD wLitzDetect	: 2;
	WORD wLitzTemp		: 2;
	WORD wRailMisMatchR	: 2;
	WORD wRailMisMatchL	: 2;

	WORD wReserved02	: 2;
	WORD wParticleDetect: 1;
	WORD wRailSlope		: 1;
	WORD wReserved01	: 1;
	WORD wSlopeMode		: 1;
	WORD wVibrationMode	: 1;
	WORD wCleanMode		: 1;
}PATROL_DATA;

/**
@brief   PatrolVHL�� ���� Agent�� ������ ���� Protocol
@author  by doori.shin
@date    2015.11.30
*/
typedef struct SERVICE_DATA_DIAG_
{
	UINT nStartNode;
	UINT nEndNode;

	PATROL_DATA PatrolData;

	float fVibrationRMS;

}SERVICE_DATA_DIAG;

/**
@brief   ���� Agent���� ������ ��û�ϴ� ��������
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_REQ_DATA_
{
	BYTE bDataID;
}DIAG_REQ_DATA;

/**
@brief   ���� Agent �ۼ��ſ� ��������
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_STATISTICS_
{
	BYTE 	bInfoID;
	UINT64	nData;
}DIAG_OHT_STATISTICS;

/**
@brief   ���� Agent ���� ���º���� ��������
@author  by doori.shin
@date    2015.11.30
*/
typedef struct RET_DATA_DIAG_
{
	BYTE bInfoID;
	BYTE bResult;
}RET_DATA_DIAG;

/**
@brief   ���� Agent ���� OHT�������� ����ü(1�� 1ȸ)
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_STATUS_DATA_
{
	BYTE bIsLogSave;			//USB�� �α� �����ϴ��� ����
	BYTE bIsEnabledUSB;			//USB�� �������� �νĵǴ��� ����
	BYTE bIsEnabledFAN;			//FAN�� ������ �ϴ��� ����
	BYTE bIsAvailableCDrive;	//C Drive�� ���������� �ִ��� Ȯ��
	BYTE bIsAvailableDDrive;	//D Drive�� ���������� �ִ��� Ȯ��
}DIAG_OHT_STATUS_DATA;

/**
@brief   ���� Agent ���� OHT�̻� �߻��� �����ϴ� ��������
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_OHT_ABNORMAL_
{
	BYTE bDataType;	//�̻��� �߻��� ������ Ÿ��
	UINT nNodeID;	//�̻��� �߻��� ���
}DIAG_OHT_ABNORMAL;

/**
@brief   ���� Agent�κ��� DB�� ����� ��谪 �޴� ����ü
@author  by doori.shin
@date    2015.11.30
*/
typedef struct OHT_STATISTICS_DATA_FROM_DIAG_
{
	UINT64 nMovingDistance;
	UINT64 nMoveCountSteering;
	UINT64 nMoveCountShift;
	UINT64 nMoveRevShift;
	UINT64 nMoveCountHoist;
	UINT64 nMoveRevHoist;
	UINT64 nMoveCountRotate;
	UINT64 nMoveCountHand;
	UINT64 nMoveCountShutter;
	UINT64 nMoveCountDrvBCR;
	UINT64 nMoveCountStaBCR;
	UINT64 nMoveTimeLookdown;
	UINT64 nMoveTimeUBG;
	UINT64 nMoveTimePBS;
}OHT_STATISTICS_DATA_FROM_DIAG;

/**
@brief   ���ܺ���� ������� ����ü
@author  by doori.shin
@date    2015.11.30
*/
class DIAG_OHT_STATISTICS_DATA
{
private:
    CRITICAL_SECTION	m_DataCS;

public:
	BYTE bID;						//��� �׸� ID
	UINT nReferenceValue;			//����Ǵ� ���ذ�
	UINT nUnit;						//����Ǵ� ����
	double dTotalData;				//���� ������
	double dPreSendData;			//���������� ����ñ⸦ �˱� ���� ���� ����������

public:
	DIAG_OHT_STATISTICS_DATA(BYTE bID, UINT nUnit, UINT nRefValue)
	{
		InitializeCriticalSection(&m_DataCS);
		this->bID = bID;
		this->nUnit = nUnit;
		this->nReferenceValue = nRefValue;
		this->dTotalData = 0.0f;
		this->dPreSendData = 0.0f;
	}

	~DIAG_OHT_STATISTICS_DATA()
	{
		DeleteCriticalSection(&m_DataCS);
	}

	//TotalData �ʱ�ȭ
	void ClearData()
	{
		EnterCriticalSection(&m_DataCS);
		this->dTotalData = 0.0f;
		this->dPreSendData = 0.0f;
		LeaveCriticalSection(&m_DataCS);
	}

	//���ܼ����� ���� ������ ����
	DIAG_OHT_STATISTICS* MakeSendData()
	{
		DIAG_OHT_STATISTICS* pStatData = new DIAG_OHT_STATISTICS;
		double dInteger, dFraction = 0.f;

		memset(pStatData, 0, sizeof(DIAG_OHT_STATISTICS));

		pStatData->bInfoID = this->bID;	//ID�Է�
		dFraction = modf(this->dTotalData, &dInteger); //������, �Ҽ��� ����

		pStatData->nData = dInteger;

		return pStatData;
	}

	//TotalData�� ���� ������
	void UpdateData(double nValue)
	{
		if(this->nUnit != 0 && nValue > 0)
		{
			EnterCriticalSection(&m_DataCS);
			this->dTotalData += (double)nValue / (double)(this->nUnit);
			LeaveCriticalSection(&m_DataCS);
		}
	}

	//�������� ��������� �ʰ��ߴ��� Ȯ��
	bool IsReadyToSend()
	{
		bool bResult = false;

		if(this->nReferenceValue != 0)
		{
			bResult = (this->dTotalData >= ((double)this->dPreSendData + (double)this->nReferenceValue));
		}

		return bResult;
	}
};

//������� ����� Map �ڷᱸ�� type definition
typedef std::map<BYTE, DIAG_OHT_STATISTICS_DATA*> 	DIAG_STAT_DATA_MAP;
typedef DIAG_STAT_DATA_MAP::iterator		DIAG_STAT_DATA_MAP_ITER;
typedef std::pair<BYTE, DIAG_OHT_STATISTICS_DATA*> DIAG_STAT_DATA_PAIR;

/**
@brief   DiagManager�� Parameter ���� ����ü
@author  by doori.shin
@date    2015.11.30
*/
typedef struct DIAG_PARAM_
{
	//OHT Name
	char Version[8];

	//VHL ��� ���ذ�
	int nRefStatMoveDistance;
	int nRefStatMoveSteering;
	int nRefStatMoveShift;
	int nRefStatRevShift;
	int nRefStatMoveHoist;
	int nRefStatRevHoist;
	int nRefStatMoveRotate;
	int nRefStatMoveHand;
	int nRefStatMoveShutter;
	int nRefStatCountDrvBCR;
	int nRefStatCountStaBCR;
	int nRefStatTimeLookdown;
	int nRefStatTimeUBG;
	int nRefStatTimePBS;

	//VHL ��� ���ذ� ����
	int nStatUnitMoveDistance;
	int nStatUnitMoveSteering;
	int nStatUnitMoveShift;
	int nStatUnitRevShift;
	int nStatUnitMoveHoist;
	int nStatUnitRevHoist;
	int nStatUnitMoveRotate;
	int nStatUnitMoveHand;
	int nStatUnitMoveShutter;
	int nStatUnitCountDrvBCR;
	int nStatUnitCountStaBCR;
	int nStatUnitTimeLookdown;
	int nStatUnitTimeUBG;
	int nStatUnitTimePBS;


	//VHL Abnormal ���ذ�
	int nRefAbnormUsingMemoryRate;
	int nRefAbnormUsingMemoryCount;
	int nRefAbnormTimeoverMoveHand;
	int nRefAbnormTimeoverMoveShutter;
	int nRefAbnormTimeoverMoveSteering;
	int nRefAbnormAlarmResetHand;
	int nRefAbnormOutofPhasePBS;
	int nRefAbnormOutofPhaseUBG;
	int nRefAbnormRetryBCR;
	int nRefAbnormDifferenceEncoderCount;
	int nRefAbnormDifferenceEncoderLimit;

	//VHL Status ���ذ�
	int nRefStatusAvailableSpaceC;
	int nRefStatusAvailableSpaceD;

	//PBSŻ�� chattering
	int nPBSOutOfRangeTimeout;

}DIAG_PARAM;


//AOHC-196 ���� �α� �߰�
// 2019.03 SHCHO * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// �����α�, �߰��Ҷ� �����ؾ��� �κ�
// �Ʒ� enum PREVIOS_DIAG_PARAM_NUM_ ���� �ε��� �߰��ϰ�
// �� �Ʒ� GetPrevIdxName(..) �Լ����� �ش� �ε��� �̸� �����ָ� �˴ϴ�.
// �ٸ� �κ��� ������ �ʿ� �����ϴ�. ������ ������ Increase �Լ��� ȣ���ϸ� �˴ϴ�.
#define PREVIOUS_LOG_WRITE_INTERVAL_MIN		   	60        // �����α� ����� �ð� ����(���� : ��)
typedef enum PREVIOS_DIAG_PARAM_NUM_
{
	PREVIOS_DIAG_HO					= 0,		//AG�ϋ�, Off -> On�ɶ�, Count++
	PREVIOS_DIAG_IF					   ,		//AG�ϋ�  Foup�� ������, Off -> On�ɶ�, Count++      AG�ϋ�  Foup�� ������, On -> Off�ɶ�, Count++
	PREVIOS_DIAG_IF_B				   ,        //AG�ϋ�  Foup�� ������, Off -> On�ɶ�, Count++      AG�ϋ�  Foup�� ������, On -> Off�ɶ�, Count++
	PREVIOS_DIAG_FC					   ,		//AG�ϋ�  Foup�� ������, On -> Off�ɶ�, Count++
	PREVIOS_DIAG_SH					   ,		//AG�ϋ�, ON-> Off�ɶ�, Count++
	PREVIOS_DIAG_OC					   ,		//������ �� Ocillation Pause �ɸ� Ƚ�� + ä�͸� ���� �̳��� �ɸ� �� Ƚ��(Pause �ɸ�����)
	PREVIOS_DIAG_HS					   ,		 //1) Hand Close/Open ���ÿ�  On   //3) Hand Open  ��Ȳ����  Gripper On�϶�    4) Hand Go �ñ׳� off�϶�
	PREVIOS_DIAG_HS_B				   ,	       //1) Hand Close/Open ���ÿ�  On   //3) Hand Open  ��Ȳ����  Gripper On�϶�    4) Hand Go �ñ׳� off�϶�
	PREVIOS_DIAG_HS_C				   ,           //1) Hand Close/Open ���ÿ�  On   //3) Hand Open  ��Ȳ����  Gripper On�϶�    4) Hand Go �ñ׳� off�϶�
	PREVIOS_DIAG_TR					   ,		//���� Front �������� �̵� �� front ������ Off -> On�̵Ǵ� ��쿡 Count++
	PREVIOS_DIAG_VD					   ,		//5V : 4.9�̻󿡼� 4.9 ���Ϸ� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)       12V : 11.64���� 300V : 240V ����
	PREVIOS_DIAG_VD_B				   ,          //5V : 4.9�̻󿡼� 4.9 ���Ϸ� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)
	PREVIOS_DIAG_VD_C				   ,          //5V : 4.9�̻󿡼� 4.9 ���Ϸ� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)
	PREVIOS_DIAG_PS					   ,		//���� ���� ���� �� ���� ������ ���� ���� �ʾ����� ��ֹ� ������ �����Ǵ� ���
	PREVIOS_DIAG_CF     			   ,		 //CID ���� offset �Ÿ� + �� �ӵ� ��� 200ms ���� �Ÿ� + Tag margin 100(common normal ���ϰ� ����)
	PREVIOS_DIAG_COMM   			   ,					//OCS ��� ���� ����, ��� �����̰� �߻��� �ִ� �ð� ���
	PREVIOS_DIAG_OT     			   ,                    //OCS ��� ���� ����, 10�� ������ OCS ��� ������ ���� ��쿡 ī��Ʈ ����
	PREVIOS_DIAG_DBT     			   ,                    //DrivingBCR Time out(������) �߻� Ƚ���� ���� Ƚ�� �߰�
	PREVIOS_DIAG_TBT                   ,                     //TransBCR Time out(������) �߻� Ƚ���� ���� Ƚ�� �߰�
	PREVIOS_DIAG_UC     			   ,    //soming         //UBG ���� ��ɿ� ���� ������ ���� �������� �ʴ� ��쿡 ī��Ʈ ���� soming 2���� �̻� ƥ��
	PREVIOS_DIAG_UD     			   ,    //soming        //UBG Direction UBG�� Ʋ������ �������� ���� ������. ������������ OBS���� ���� UBG�� 3~6���� ��
	PREVIOS_DIAG_UF     			   ,    //soming        //UBG Fail. OBS�� ������ UBG �ȵ���. ������������

	//P2 �׸�  2019 02 18 puting
	PREVIOS_DIAG_CPU 	   				,					//���� CPU ������ �̻�(40%) �ö� ��� ī��Ʈ ����
	PREVIOS_DIAG_MEM 	   				,               	//���� �޸� ������ �̻�(50%) �ö� ��� ī��Ʈ ����
	PREVIOS_DIAG_HANDT_MAX 	   			,               	 //Hand Close/Open�� �ִ� ���� �ð�
	PREVIOS_DIAG_SHUT_MAX 	  			,               	  //Shutter Close/open�� �ִ� ���� �ð�
	PREVIOS_DIAG_STEERLT_MAX 			  	,           	 //���� Front Left/Right�� �ִ� ���� �ð�
	PREVIOS_DIAG_STEERRT_MAX 			   	,           	 //���� Rear Left/Right�� �ִ� ���� �ð�
	PREVIOS_DIAG_MO_FRGAP 	   			,               	 //������ �ķ��� ������ �ִ� ���ڴ���
	PREVIOS_DIAG_FOLOWF_MAX 			,               	 //���� �ִ� Following Error �ִ� ���̰�
	PREVIOS_DIAG_MAP_DGAP 	   			,                 // ���� Link�� ������ Map���� ���̰� 100mm(??) �̻��� ��� ī��Ʈ ����
	PREVIOS_DIAG_E84TM 	   				,               //  E84Timeout �߻��� ī��Ʈ ����
	PREVIOS_DIAG_EMPTY	   				,                // ������ �߻��� ī��Ʈ ����
	PREVIOS_DIAG_DOUBLE 	   			,               // ���߹��� �߻��� ī��Ʈ ����
	PREVIOS_DIAG_HAND_ALAM 	   			,               //Hand Alaram �� Reset�� ī��Ʈ ����
	PREVIOS_DIAG_HAND_ORIGIN			,                //Hand Origin ���� �� ī��Ʈ ����
	PREVIOS_DIAG_PPO_RETYE 	   			,  // ȣ��Ǵ� �� ����   	//Shutter Alaram�� ��Ʈ���̽� ī��Ʈ ����
	PREVIOS_DIAG_SHU_ALAM 	   			,               	//������� Retry ��û�� ���� Ƚ���� �Ѿ ��û�� ��� ī��Ʈ ����
	PREVIOS_DIAG_CID_RETYE 	   			,               	//CID ���� �簳��Ʈ���̽� ī��Ʈ ����
	PREVIOS_DIAG_CID_ERROR_ST 			,               	//CID �̻� ����� ī��Ʈ ����
	PREVIOS_DIAG_JOBO_NAK 	   			,               	//  �۾� ���ý� Nack�� ��� ī��Ʈ ����
	PREVIOS_DIAG_JOBC_NAK 	   			,               //  �۾� ����� Nack�� ��� ī��Ʈ ����
	PREVIOS_DIAG_JOBD_NAK       		,               	//   �۾� ��ҽ� Nack�� ��� ī��Ʈ ����
	PREVIOS_DIAG_VD_U   				,                //5V : 5.2�̻����� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)       12V : 12.36���� 300V : 355V ����
	PREVIOS_DIAG_VD_B_U   				,                //5V : 4.9�̻󿡼� 4.9 ���Ϸ� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)
	PREVIOS_DIAG_VD_C_U         		,                //5V : 4.9�̻󿡼� 4.9 ���Ϸ� �������� ������  Count++ (5.0�� 4.8 O, 4.8�� 4.8 X)
	PREVIOS_DIAG_UF_2					,				//UBG Fail. UBG�� ��翡 ���� IO�� ��µɰ�� ó��.On On off
	PREVIOS_DIAG_OF_2					,				//OBG Fail. OBG�� ��翡 ���� IO�� ��µɰ�� ó��.On On off
	PREVIOS_DIAG_UBG_PAT                ,               //������ Node���� ���� ������ ��� UBG ���� ���� �α� Ȯ�� ���� ī��Ʈ
	PREVIOS_DIAG_AP                     ,               //AP ����<4 ����͸� �α�



	//HJH : Teaching Auto Adjustment
	PREVIOS_DIAG_LOAD_TEACHING_ABNORMAL_CNT		,
	PREVIOS_DIAG_UNLOAD_TEACHING_ABNORMAL_CNT 	,

	PREVIOS_DIAG_HP      ,                                   // Hoist Pause Count
	PREVIOS_DIAG_LP     ,                                    // LookDown Pause Count
	PREVIOS_DIAG_HANDPIOP,                                   //Hand Pio Pause Count
	// �ݵ�� ������ �ε���
	COUNTOF_PREVIOUS


}PREVIOS_DIAG_PARAM_NUM;

// * * * * ���� enum �� ���� �ݵ�� �¾ƾ� �մϴ�.
bool GetPrevIdxName(PREVIOS_DIAG_PARAM_NUM idx, char *buf)
{
	if(buf == NULL)		return false;
	
	switch(idx)
	{
		case PREVIOS_DIAG_HO			:		strcpy(buf, "HO");	break;
		case PREVIOS_DIAG_IF			:		strcpy(buf, "IA");	break;
		case PREVIOS_DIAG_IF_B			:		strcpy(buf, "IB");	break;
		case PREVIOS_DIAG_FC			:		strcpy(buf, "FC");	break;
		case PREVIOS_DIAG_SH			:		strcpy(buf, "SH");	break;
		case PREVIOS_DIAG_OC			:		strcpy(buf, "OC");	break;
		case PREVIOS_DIAG_HS			:		strcpy(buf, "HA");	break;
		case PREVIOS_DIAG_HS_B			:		strcpy(buf, "HB");	break;
		case PREVIOS_DIAG_HS_C			:		strcpy(buf, "HC");	break;
		case PREVIOS_DIAG_TR			:		strcpy(buf, "TR");	break;
		case PREVIOS_DIAG_VD			:		strcpy(buf, "VA");	break;
		case PREVIOS_DIAG_VD_B			:		strcpy(buf, "VB");	break;
		case PREVIOS_DIAG_VD_C			:		strcpy(buf, "VC");	break;
		case PREVIOS_DIAG_PS			:		strcpy(buf, "PS");	break;
		case PREVIOS_DIAG_CF     		:		strcpy(buf, "CF");	break;
		case PREVIOS_DIAG_COMM   		:		strcpy(buf, "CM");	break;
		case PREVIOS_DIAG_OT     		:		strcpy(buf, "OT");	break;
		case PREVIOS_DIAG_DBT     		:		strcpy(buf, "DBT");	break;
		case PREVIOS_DIAG_TBT           :       strcpy(buf, "TBT");	break;
		case PREVIOS_DIAG_UC     		:		strcpy(buf, "UC");	break;
		case PREVIOS_DIAG_UD     		:		strcpy(buf, "UD");	break;
		case PREVIOS_DIAG_UF     		:		strcpy(buf, "UF");	break;
		case PREVIOS_DIAG_CPU 	   		:		strcpy(buf, "CPU");	break;
		case PREVIOS_DIAG_MEM 	   		:		strcpy(buf, "MEM");	break;
		case PREVIOS_DIAG_HANDT_MAX 	:		strcpy(buf, "HAN_MAX");	break;
		case PREVIOS_DIAG_SHUT_MAX 	  	:		strcpy(buf, "SHU_MAX");	break;
		case PREVIOS_DIAG_STEERLT_MAX 	:		strcpy(buf, "STL_MAX");	break;
		case PREVIOS_DIAG_STEERRT_MAX 	:		strcpy(buf, "STR_MAX");	break;
		case PREVIOS_DIAG_MO_FRGAP 	   	:		strcpy(buf, "MO_FRGAP");	break;
		case PREVIOS_DIAG_FOLOWF_MAX 	:		strcpy(buf, "FOLOW_MAX");	break;
		case PREVIOS_DIAG_MAP_DGAP 	   	:		strcpy(buf, "MAP_DGAP");	break;
		case PREVIOS_DIAG_E84TM 	   	:		strcpy(buf, "E84TM");	break;
		case PREVIOS_DIAG_EMPTY	   		:		strcpy(buf, "EMPTY");	break;
		case PREVIOS_DIAG_DOUBLE 	   	:		strcpy(buf, "DOUBLE");	break;
		case PREVIOS_DIAG_HAND_ALAM 	:		strcpy(buf, "HAN_ALAM");	break;
		case PREVIOS_DIAG_HAND_ORIGIN   :		strcpy(buf, "HAN_ORI");	break;
		case PREVIOS_DIAG_PPO_RETYE 	:		strcpy(buf, "PPO_RET");	break;		// ������ �����־���
		case PREVIOS_DIAG_SHU_ALAM 	   	:		strcpy(buf, "SHU_ALAM");	break;
		case PREVIOS_DIAG_CID_RETYE 	:		strcpy(buf, "CID_RETYE");	break;
		case PREVIOS_DIAG_CID_ERROR_ST 	:		strcpy(buf, "CID_ERROR_ST");	break;
		case PREVIOS_DIAG_JOBO_NAK 	   	:		strcpy(buf, "JOBO_NAK");	break;
		case PREVIOS_DIAG_JOBC_NAK 	   	:		strcpy(buf, "JOBC_NAK");	break;
		case PREVIOS_DIAG_JOBD_NAK      :		strcpy(buf, "JOBD_NAK");	break;
		case PREVIOS_DIAG_VD_U   		:		strcpy(buf, "VD_A_U");	break;
		case PREVIOS_DIAG_VD_B_U   		:		strcpy(buf, "VD_B_U");	break;
		case PREVIOS_DIAG_VD_C_U        :		strcpy(buf, "VD_C_U");	break;
		case PREVIOS_DIAG_UF_2			:		strcpy(buf, "UF_2");	break;
		case PREVIOS_DIAG_OF_2			:		strcpy(buf, "OF_2");	break;
		case PREVIOS_DIAG_UBG_PAT       :       strcpy(buf, "UBG_PAT");	break;
		case PREVIOS_DIAG_AP            :       strcpy(buf, "AP");	break;
		case PREVIOS_DIAG_LOAD_TEACHING_ABNORMAL_CNT		: 	strcpy(buf, "LN");		break;
		case PREVIOS_DIAG_UNLOAD_TEACHING_ABNORMAL_CNT		:   strcpy(buf, "UN");      break;
		case PREVIOS_DIAG_HP            :       strcpy(buf, "HP");	break;
		case PREVIOS_DIAG_LP            :       strcpy(buf, "LP");	break;
		case PREVIOS_DIAG_HANDPIOP      :       strcpy(buf, "HPIO");	break;


		default: return false;
	}
	return true;
	
}


/**
@brief   ���ܸ���͸� �׸� ���� �� ������ ����ϴ� Ŭ����
@author  by doori.shin
@date    2015.11.30
*/
class DiagManager
{
private:
	static DiagManager* pInstance;
	DiagManager();
	~DiagManager();

	LogUnit *pLogPreviosDiag; ///< IOT ��� �α� ����
	CRITICAL_SECTION	m_CSPreDiag;		///< Buffer�� Count�� �а� �� ��

	DIAG_STAT_DATA_MAP* m_DiagDataMap;		//VHL ���� ���ܵ����͸� ����ִ� �ڷᱸ��
	LList* m_DiagAbnormalDataList;			//�����׸� �� Abnormal ������ �����͵� ����Ǵ� �ڷᱸ��
	LList* m_DiagPatrolInfoList; 			//Patrol���� ������ �����ϴ� ����Ʈ
	SERVICE_DATA_DIAG m_DiagServiceData;	//Patrol�̺�Ʈ ����ϴ� ����
	DIAG_OHT_STATUS_DATA m_DiagStatusData;	//OHT Status ���� ����ϴ� ����

	int m_nDiagAbnormalDrvDiffCnt;			//Abnormal �׸��� Driving Encoder ���̹߻� Ƚ�� �����ϴ� ����
	int m_nDiagAbnormalHandResetCount;		//
	int m_nDiagCheckCount;					//24�ð� ���� üũ ����

	DIAG_PARAM m_DiagParam;

	// ����ü���� �迭�� ����. �����α� ���� �þ�� ����
	int m_nArrDiagVal[COUNTOF_PREVIOUS];	//PREVIOS_DIAG_PARAM m_PreviosDiagParam; //���� ���� �α� puting 20180510
	DWORD m_dwPrevLogStartTick;

	int m_nVHLWorkingTime;	//VHL �����ð�, ���� : Hour (UBG, PBS, Lookdown ���������)

	bool m_bIsLoadedDiagData;	//���� ����� ���ܼ����� ���� ��� �����͸� �޾Ҵ��� ����
	UINT m_nDiagCheckTimerID;	//Timer ID
	UINT m_nTimerRetryCount;	//Timer Retry Ƚ��

	//Status ���� �Լ���
	bool CheckDiskStatus(DISK_INFO* pInfo);
	void ClearDiagStatusData();

	void MakeStatDataMap();
public:
	static DiagManager* getInstance();
	static void DestroyInstance();	//shcho 2018.08.06
	bool IsDiagStatLoaded();



	//Socket ����
//	AnsiString GetDiagIpAddr();

	//��Ŷ����

	//Param����
	bool LoadDiagParam(char* szFilePath, DIAG_PARAM* pParamSet);  //���� �Ķ���� �б�
	DIAG_PARAM GetDiagParam();

	bool LoadDiagParamRecorvery(char* szFilePath, DIAG_PARAM* pParamSet);  //���� �Ķ���� �б�
	int m_ParamChehckError;
    bool saveParameter(char* szFilePath, DIAG_PARAM* pParamSet);

	//������� ���� Map ���� �Լ���
	int UpdateStatDiagData(BYTE bID, double nValue);
	int UpdateStatReferenceValue(BYTE bID, int nRefValue);
	int ClearStatDiagData(BYTE bID);
	bool CheckStatDataToSend(BYTE bID);
	double GetStatDiagData(BYTE bID);
	DIAG_STAT_DATA_MAP* GetDiagStatMap();

	bool UpdateStatDiagAllData(DIAG_PKT_BODY_DATA_INFO_RESPONSE RcvData);

	//Abnormal ���� ���� Map ���� �Լ���
	void InsertAbnormalDiagData(BYTE bDataType);
	LList* GetAbnormalDiagDataList();

	//Abnormal ������ Cnt���� ���� �׸��
	int GetAbnormalDrvDiffCount();
	void IncreaseAbnormalDrvDiffCount();
	void ResetAbnormalDrvDiffCount();

	int GetAbnormalHandResetCount();
	void IncreaseAbnormalHandResetCount();
	void ResetAbnormalHandResetCount();

	int GetDiagCheckCount();
	void IncreaseDiagCheckCount();
	void ResetDiagCheckCount();

	//OHTStatus ���� �Լ���
	void CheckOHTStatus();
    DIAG_OHT_STATUS_DATA* GetDiagOHTstatus();

	//OHT �����ð� Ȯ��
	void IncreaseWorkingTime();
	int GetWorkingTime(bool bInit);

	//���ܼ��ϰ��� �Լ���
//	UDP_ACK_DATA ParsingDiagCmd(char* pRecvBuf);


	//���� ���� �α� ���� �Լ�
	void LogginPreviosDiagCount();
	void IncreasePreviosDiagCount(PREVIOS_DIAG_PARAM_NUM nType);	// �ش��׸� ���� 1���� / AOHC-196 ���� �α� �߰�
	void IncreasePreviosDiagMax(PREVIOS_DIAG_PARAM_NUM nType, int dwTime);	// �ش��׸� �ִ밪 ���� / AOHC-196 ���� �α� �߰�
	void ResetPreviosDiagCount();  //Arrived �϶� ȣ���Ұ�.

	// �����α�  �� ���� / AOHC-196 ���� �α� �߰�
	int 	GetPreviosValue(PREVIOS_DIAG_PARAM_NUM type);	// �ش��׸� �� ����

	//Timer ���� �Լ���
	void SetDiagStatCheckTimer();
	void IncreaseRetryCount();
	int GetRetryCount();
	int GetCheckTimerID();
	void Reset_DiagManager();
	bool PrevLogTimeChk();			// �α� ��� �ð� üũ
};

DiagManager* DiagManager::pInstance = NULL;

//---------------------------------------------------------------------------
#endif
