/**
@file    PropotionalMotionAxis.h
@version OHT 7.0
@brief   PropotionalMotionAxis Class Header File
*/

#ifndef PropotionalMotionAxisH
#define PropotionalMotionAxisH

#include "PropotionalConverter.h"

#include "HWCommon.h"	// HW ���� ���
#include "MotionAxis.h"	// Motion�� ��� 
#include "Def_Driving.h"	// ���� ���� ����ü
#include "AxisParam.h"	// ���� �� �Ķ���� ��� ���� 

/**
@class   PropotionalMotionAxis
@date    2012.10.17
@author  ���¿�
@brief   PropotionalMotionAxis Motor ���� Class
@note    MotionAxis Class ���
*/
class PropotionalMotionAxis : public MotionAxis
{
//1	����(Member)
protected:
	PropotionalConverter *m_pPropotionalConverter;		///< ������ ���� ǥ��ȭ : ��� ���� ���
	PROPOTINAL_GEAR_RATIO m_GearRatio;

//1	������/�Ҹ��� (Constructor/Destructor)	
public:
	PropotionalMotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder, double dGearRatio);	///< ������
	virtual ~PropotionalMotionAxis();	///< �Ҹ��� 
	

//1	�Լ�(Method)
public:
	bool	SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio);	///< ���� ����
	PROPOTINAL_GEAR_RATIO	GetGearRatio();									///< ���� Ȯ��

	bool	EnableUnitConverter(void);			///< ���� ����ϵ���

private:

};

#endif  //PropotionalMotionAxisH

