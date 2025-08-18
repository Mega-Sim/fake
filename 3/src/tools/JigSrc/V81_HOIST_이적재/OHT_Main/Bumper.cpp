//---------------------------------------------------------------------------
/**
@file    Bumper.cpp
@version OHT 7.0
@brief   Bumper Class �ҽ� File
*/

#include <windows.h>
#include "Bumper.h"


/**
@brief   Bumper Class ������
@author  zzang9un
@date    2012.10.19
@param   pHWCommon : HWCommon Ŭ���� ������
*/
Bumper::Bumper(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   Bumper Class �Ҹ���
@author  zzang9un
@date    2012.10.19
*/
Bumper::~Bumper()
{

}

/**
@brief	 Bumper Sensor�� On �Ǿ� �ִ��� üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.23
@return	 Bumper�� Press�Ǿ� ������ True, �ƴϸ� False ����.
*/
bool Bumper::IsBumperPress()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.BUMPER_Press == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

/**
@brief	 Bumper Press�� ���� Latch �� ��ȣ�� Ǯ���ֱ� ���� Safety Clear ��ȣ�� ��� ��Ű�� �Լ�
@author	 LSJ
@date	 2012.10.23
@return	 SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int Bumper::ResetBumperPress()
{
   int nError = NO_ERR;

   nError = m_pHWCommon->SetBit(OUT_BUMPER_CLEAR, ON);

   Sleep(200);

   if(nError == NO_ERR)
		nError = m_pHWCommon->SetBit(OUT_BUMPER_CLEAR, OFF);

   return nError;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)

