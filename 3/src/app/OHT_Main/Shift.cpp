/**
@file    Shift.cpp
@version OHT 7.0
@brief   Shift Class CPP File
*/

#include "Shift.h"


/**
@brief   Shift Class ������
@author  //
@date    //
*/
Shift::Shift(HWCommon *pHWCommon, int nAxisNum):PropotionalMotionAxis(pHWCommon, nAxisNum, true, 1.0)	//MotionAxis
{
	m_pHWCommon = pHWCommon;
}													

/**
@brief   Shift Class ������
@author  //
@date    //
*/
Shift::Shift(HWCommon *pHWCommon, int nAxisNum, double dGearRatio):PropotionalMotionAxis(pHWCommon, nAxisNum, true, dGearRatio)	//MotionAxis
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Shift Class �Ҹ���
@author  //
@date    //
*/
Shift::~Shift()
{

}

/**
@brief   // �Է�
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  // �Է�(���� ��� ����)
*/
bool Shift::IsShiftHome()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.SHIFT_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

