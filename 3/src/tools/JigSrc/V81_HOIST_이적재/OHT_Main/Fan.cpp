/**
@file    Fan.cpp
@version OHT 7.0
@brief   Fan Class CPP File
*/

#include "Fan.h"

/**
@brief	 Fan ������
@author  zzang9un
@date    2012.10.13
@param   pHWCommon : HWCommon Class�� ������
@note    Input, Ouput�� ����ϱ� ���� HWSet�� Pointer�� ������
*/
Fan::Fan(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Fan �Ҹ���
@author  zzang9un
@date    2012.10.13
*/
Fan::~Fan()
{
}

/**
@brief   Fan�� ���� ������ üũ�ϴ� �Լ�
@author  zzang9un
@date    2012.10.13
@return  Fan�� ������ ��� True, ������ ��� False�� Return
*/
bool Fan::IsFanFail()
{
    int nResult = false;

    if (m_pHWCommon->m_Input.FAN_Fail == ON)
        nResult = true;
    else
        nResult = false;

    return nResult;
}
