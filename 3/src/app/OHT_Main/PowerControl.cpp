//---------------------------------------------------------------------------
/**
@file    PowerControl.cpp
@version OHT 7.0
@brief   PowerControl Class CPP File
*/

#pragma hdrstop

#include "PowerControl.h"

/**
@brief   PowerControl ������
@author  LSJ
@date    2012.10.13
@param   pHWCommon : HWCommon Class�� ������
@note    Input, Ouput�� ����ϱ� ���� HWCommon�� Pointer�� ������
*/
PowerControl::PowerControl(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   PowerControl �Ҹ���
@author  LSJ
@date    2012.10.13
*/
PowerControl::~PowerControl()
{



}

/**
@brief	 SafetyController�� Reset �� �� �ִ� �Լ�
@author	 LSJ
@date	 2014.09.02
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int PowerControl::ResetController(void)
{
	int nError = 0;
	nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, ON);
	Sleep(1000);
	nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, OFF);

	return nError;
}


/**
@brief	 MC(Magnetic Contactor)�� On, Off �� �� �ִ� �Լ�
@author	 LSJ
@date	 2012.10.27
@param   MC�� On ��Ű���� ON, Off ��Ű���� OFF
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int PowerControl::MCControl(bool bOnOff)
{
	int nError = 0;
	if (bOnOff == ON)
		nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, ON);
	else
		nError = m_pHWCommon->SetBit(OUT_SAFETY_RESET, OFF);

	return nError;
}

/**
@brief   EMO�� ���� �������� Ǯ��(����) �������� Ȯ���ϴ� �Լ�
@author	 LSJ
@date	 2012.10.27
@return	 EMO�� ���� �����̸� true, Ǯ�� �����̸� false ����.
*/
bool PowerControl::IsOnEMO()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.EMO_Press == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief   SMPS Short ��ȣ�� ON���� Ȯ���ϴ� �Լ�
@author	 JSH
@date	 2014.11.13
@return	 EMO�� ���� �����̸� true, Ǯ�� �����̸� false ����.
*/
bool PowerControl::IsOnSMPSFault()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.SMPS_Short == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
