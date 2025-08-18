/**
@file    Hoist.h
@version OHT 7.0
@brief   Hoist Class Header File
*/

#ifndef HoistH
#define HoistH

#include "HWCommon.h"				// HW ���� ���
#include "MotionAxis.h"				// Motion�� ��� 
#include "HoistDistanceConverter.h"	// ���� ǥ��ȭ: 3�� �溡�� �ٻ� 
#include "AxisParam.h"				// 3�� ������ ���� �� �Ķ���� ��� ���� 

//AOHC-286
#if(OHT_NAME == OHT_NAME_STD_V85)      //�ӽ�
#define HOIST_SMALL_ADD_HOISTHOME    	76
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#elif((OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))  //V86 ���Ⱑ �´��� Ȯ�� ��
#define HOIST_SMALL_ADD_HOISTHOME    	73
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#else
#define HOIST_SMALL_ADD_HOISTHOME		74
#define HOIST_SMALL_ADD_HOISTHOME_DISTANCE	6.0
#endif

/**
@class   Hoist
@date    2012.10.17
@author  ���¿�
@brief   Hoist Motor ���� Class
*/
class Hoist : public MotionAxis
{
//1	����(Member)
private:
	HWCommon *m_pHWCommon;									///< HW������ ���
	HoistDistanceConverter *m_pHoistDistanceConverter;	///< ���� ǥ��ȭ : 3�� ���������� �ٻ�ȭ
	int m_nAxisNum;											///< �� ����
	//double m_dHomeOffsetOfRailToCarrierBottomOnUserUnit;	///< Hoist Home ��ġ ���, Rail �Ʒ����� �ݼ�ü �ٴڱ����� �Ÿ�
	THREE_LINEAR_EQUATION_GEAR_RATIO m_HoistParameter;

	bool bHoistOrigin;


//1	������/�Ҹ��� (Constructor/Destructor)	
public:
    Hoist(HWCommon *pHWCommon, int nAxisNum);	///< ������
    ~Hoist();									///< �Ҹ��� 


//1	�Լ�(Method)
protected:
	double ConvertSpeedUnit(double dInput, bool bUserUnit);				///< ���� ǥ��ȭ : �ӵ� ���� ��ȯ
	double ConvertAccelerationUnit(double dUser, bool bUserUnit);	///< ���� ǥ��ȭ : ���ӵ� ���� ��ȯ


public:
	bool IsHoistHome();												///< IO ���� : Ȩ ��ġ Ȯ��
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S)  ||(OHT_NAME == OHT_NAME_STD_V85)  || (OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86))
	bool IsHoistBeltDetect();
//	int HoistExternalBrakeReleaseOnOFF(bool);                //�̻��
#endif
	bool EnableUnitConverter(void);							///< ���� ��ȭ Ȱ��ȭ
    bool SetHoistParam(THREE_LINEAR_EQUATION_GEAR_RATIO HoistParam);	///< ���� ��ȯ�� ���� �Ķ���� �Է�

	double GetRelativeDistanceFromSmalladd();
	bool   IsHoistOriginFromSmalladd();
	void   SetHoistOriginFromSmalladd(bool bOrigin);

	double GetHoistMaxUserPosition();
	double GetHoistMinUserPosition();

	double GetRelativeDistanceFromSmalladdCurrentPosition();
	double GetHoistPulseValue();
	int SetNegativeLimit4PulseMoving(double dMinCtsValue);

#if(OHT_NAME == OHT_NAME_P4_MASK)
	/// mgchoi 2022-08-19. P4 MaskOHT�� �߰��Լ� ///
	bool IsHoistLowerHome();
	bool IsHoistEUVHome();
	bool IsHoistRSP200Home();
	bool IsHoistRSP150Home();
	////////////////////////////////////////////////
#endif


};

#endif  // HoistH

