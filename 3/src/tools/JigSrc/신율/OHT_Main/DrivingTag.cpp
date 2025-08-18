/**
@file    DrivingTag.cpp
@version OHT 7.0
@brief   DrivingTag Class CPP File
*/


#include "DrivingTag.h"

/**
@brief   DrivingTag ������
@author  LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class�� ������
@note    Input�� ����ϱ� ���� HWSet�� Pointer�� ������
*/
DrivingTag::DrivingTag(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   DrivingTag �Ҹ���
@author  LSJ
@date    2012.10.22
*/

 DrivingTag::~DrivingTag()
{
}

/**
@brief   DrivingTag Front Sensor On/Off ������ üũ�ϴ� �Լ�
@author  LSJ
@date    2012.10.27
@return  Driving Stop Sensor�� On�� ��� True, Off�� ��� False�� Return
*/

bool DrivingTag::IsOn()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Driving_Front == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

bool DrivingTag::IsPreCheckOn()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Driving_PreCheck== ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}