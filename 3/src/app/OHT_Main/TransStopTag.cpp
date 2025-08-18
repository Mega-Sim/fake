//---------------------------------------------------------------------------
/**
@file    TransStopTag.cpp
@version OHT 7.0
@brief   TransStopTag Class CPP File
*/

#pragma hdrstop

#include "TransStopTag.h"

/**
@brief   TransStopTag Class ������
@date
@author
@param   pHWCommon : HWCommon Ŭ���� ������
*/
TransStopTag::TransStopTag(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   TransStopTag Class �Ҹ���
@date
@author
*/
TransStopTag::~TransStopTag()
{
}

/**
@brief	 ������ StopTag�� Front Sensor�� On �Ǿ� �ִ��� üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 ������ Front Stop Sensor�� On�̸� True, Off �� False
*/
bool TransStopTag::IsOnFrontSensor()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Trans_Front == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 ������ StopTag�� Rear Sensor�� On �Ǿ� �ִ��� üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 ������ Rear Stop Sensor�� On�̸� True, Off �� False
*/
bool TransStopTag::IsOnRearSensor()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.STOP_Trans_Rear == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 ������ StopTag�� Front, Rear Sensor�� ��� On �Ǿ� �ִ��� üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 ������ Front, Rear Stop Sensor�� ��� On�̸� True, Off �� False

bool TransStopTag::IsTransStopPosition()
{
	bool bResult = false;

	if(IsOnFrontSensor() && IsOnRearSensor())
		bResult = true;
	else
		bResult = false;

	return bResult;
}
*/
//---------------------------------------------------------------------------

#pragma package(smart_init)
