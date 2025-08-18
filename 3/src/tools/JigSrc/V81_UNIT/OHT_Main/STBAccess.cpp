/**
@file    STBAccess.cpp
@version OHT 7.0
@brief   STBAccess Class CPP File
*/

#include "STBAccess.h"

/**
@brief   STBAccess Class ������
@author	 LSJ
@date	 2012.10.27
@param   pHWCommon : HWCommon Class�� ������
*/
STBAccess::STBAccess(HWCommon * pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   STBAccess Class �Ҹ���
@author	 LSJ
@date	 2012.10.27
*/
STBAccess::~STBAccess()
{

}

/**
@brief   Left STB �۾� �� Left STB OHT Detect Sensor�� On/Off ���� Ȯ���ϴ� �Լ�.
@author	 LSJ
@date	 2012.10.27
@return  Left STB OHT Detect Sensor�� On�� ��� True, Off�� ��� False�� Return
*/
bool STBAccess::IsSTBLOHTDetect()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.STB_OHT_Detect_Left == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief   Right STB �۾� �� Left STB OHT Detect Sensor�� On/Off ���� Ȯ���ϴ� �Լ�.
@author	 LSJ
@date	 2012.10.27
@return  Right STB OHT Detect Sensor�� On�� ��� True, Off�� ��� False�� Return
*/
bool STBAccess::IsSTBROHTDetect()
{
    bool bResult = false;

    if (m_pHWCommon->m_Input.STB_OHT_Detect_Right == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief   Left STB�� �۾� �� �� �� ���� �߱��� ������ On �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author	 LSJ
@date	 2012.10.27
@return	 Left STB Work ��� ���� On�� ��� True, Off�� ��� False�� Return
*/
bool STBAccess::IsSTBLWorkOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Output.STB_Left_Work == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 Right STB�� �۾� �� �� �� ���� �߱��� ������ On �Ǿ� �ִ��� Ȯ���ϴ� �Լ�
@author	 LSJ
@date	 2012.10.27
@return  Right STB Work ��� ���� On�� ��� True, Off�� ��� False�� Return
*/
bool STBAccess::IsSTBRWorkOn()
{
    bool bResult = false;

    if (m_pHWCommon->m_Output.STB_Right_Work == ON)
        bResult = true;
    else
        bResult = false;

    return bResult;
}

/**
@brief	 Left STB ������ �۾� �� �߱��� ������ On ��Ű�� �Լ�
@author	 LSJ
@date	 2012.10.29
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int STBAccess::STBLWorkOn()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_STB_L_WORK, ON);

    return nError;
}

/**
@brief	 Right STB ������ �۾� �� �߱��� ������ On ��Ű�� �Լ�
@author	 LSJ
@date	 2012.10.29
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int STBAccess::STBRWorkOn()
{
    int nError = NO_ERR;

    nError = m_pHWCommon->SetBit(OUT_STB_R_WORK, ON);

    return nError;
}

/**
@brief	 Left STB ������ �۾��� ���� �� �߱��� ������ Off ��Ű�� �Լ�
@author	 LSJ
@date	 2012.10.29
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int STBAccess::STBLWorkOff()
{
    int nError = NO_ERR;

    if (IsSTBLWorkOn() == true)
        nError = m_pHWCommon->SetBit(OUT_STB_L_WORK, OFF);

    return nError;
}

/**
@brief	 Left STB ������ �۾��� ���� �� �߱��� ������ Off ��Ű�� �Լ�
@author	 LSJ
@date	 2012.10.29
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
*/
int STBAccess::STBRWorkOff()
{
    int nError = NO_ERR;

    if (IsSTBRWorkOn() == true)
        nError = m_pHWCommon->SetBit(OUT_STB_R_WORK, OFF);

    return nError;
}
