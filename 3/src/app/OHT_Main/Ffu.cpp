/**
@file    FFU.cpp
@version MASK OHT
@brief   FFU Class CPP File
*/

#include "Ffu.h"
#if((OHT_NAME == OHT_NAME_P4_MASK) || (OHT_NAME == OHT_NAME_MASK_JIG))
/**
@brief	 FFU ������
@author
@date    2015.10.27
@param   pHWCommon : HWCommon Class�� ������
@note    Input, Ouput�� ����ϱ� ���� HWSet�� Pointer�� ������
*/
Ffu::Ffu(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Ffu �Ҹ���
@author
@date    2015.10.27
*/
Ffu::~Ffu()
{
}

/**
@brief   P4 Mask OHT�� Hoist FFU�� ������ üũ�ϴ� �Լ�
@author  mgchoi
@date    2023.01.31
@return  FFU Hoist�� ������ ��� True, ������ ��� False�� Return
*/
bool Ffu::IsFfuHoistFail()
{
	int nResult = false;

	if (m_pHWCommon->m_Input.FFU_Fail_Hoist == ON)
		nResult = true;
	else
		nResult = false;

	return nResult;
}

/**
@brief   P4 Mask OHT�� Frame FFU�� ������ üũ�ϴ� �Լ�
@author  mgchoi
@date    2023.01.31
@return  FFU Frame�� ������ ��� True, ������ ��� False�� Return
*/
bool Ffu::IsFfuFrameFail()
{
	int nResult = false;
	if (m_pHWCommon->m_Input.FFU_Fail_Frame == ON)
		nResult = true;
	else
		nResult = false;

	return nResult;
}
#endif
