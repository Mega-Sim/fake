/**
@file    PropotionalMotionAxis.cpp
@version OHT 7.0
@brief   PropotionalMotionAxis Class �ҽ� File
*/
#include "PropotionalMotionAxis.h"
//#include "LogHandler.h"


/**
@brief   PropotionalMotionAxis Class ������
@date
@author
@param   pHWCommon : HWCommon Ŭ���� ������
@param   nAxisNum : Motor�� ���� �� ��ȣ
*/
PropotionalMotionAxis::PropotionalMotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder, double dGearRatio):MotionAxis(pHWCommon, nAxisNum, bAbsoluteEncoder)
{
	m_GearRatio.dIdealGearRatio = dGearRatio;
	m_GearRatio.dIdeal2RealRatio = 1.0;
	m_pPropotionalConverter = new PropotionalConverter(dGearRatio);
	EnableUnitConverter();
}
/**
@brief   PropotionalMotionAxis Class �Ҹ���
@date
@author
*/
PropotionalMotionAxis::~PropotionalMotionAxis()
{
	DisableUnitConverter();
	delete m_pPropotionalConverter;
	m_pPropotionalConverter = NULL;
}

/**
@brief   PropotionalMotionAxis ���� ��ȯ ��� ��� ���� �Լ�

	���� ������ m_pPropotionalConverter�� �̿��Ѵ�.
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  Unit ��� ���� �ϸ� true Return
*/
bool PropotionalMotionAxis::EnableUnitConverter(void)
{
	if(m_pPropotionalConverter==NULL)
	{
		return false;
	}
	else
	{
		m_pUnitConverter = m_pPropotionalConverter;
		return true;
	}
}

/**
@brief   ���� ���� �Լ�
@author  ���¿�
@date	 2013.06.23
@return  ���� ����
*/
bool PropotionalMotionAxis::SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio)
{
	double dGearRatio = GearRatio.dIdealGearRatio * GearRatio.dIdeal2RealRatio;
	if((m_pPropotionalConverter!=NULL)&&(dGearRatio>0))
	{
		m_GearRatio = GearRatio;
		return m_pPropotionalConverter->setCoefficient(dGearRatio);
    }
    else
    {
    	return false;
    }
}

/**
@brief   ���� ȹ�� �Լ�
@author  ���¿�
@date	 2013.06.23
@return  ���� ����

PROPOTINAL_GEAR_RATIO	PropotionalMotionAxis::GetGearRatio()
{
	return m_GearRatio;
}
*/
