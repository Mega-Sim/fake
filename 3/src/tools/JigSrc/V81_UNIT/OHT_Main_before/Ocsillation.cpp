/**
@file    Ocsillation.cpp
@version OHT 7.0
@brief   Ocsillation Class CPP File
*/


#include "Ocsillation.h"

/**
@brief   Ocsillation ������
@author	 LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class�� ������
*/
Ocsillation::Ocsillation(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Ocsillation �Ҹ���
@author  LSJ
@date    2012.10.27
*/

 Ocsillation::~Ocsillation()
{
}

/**
@brief   Ocsillation�� ���� ������ üũ�ϴ� �Լ�
@author  LSJ
@date    2012.10.27
@return  Hand�� ������ ������ ��� True, �׷��� ���� ��� false
*/

bool Ocsillation::IsDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.OSCILLATION == ON)
		bResult = false;
	else
		bResult = true;

	return bResult;
}