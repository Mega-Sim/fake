/**
@file    InnerFoup.cpp
@version OHT 7.0
@brief   InnerFoup Class CPP File
*/

#include "InnerFoup.h"

/**
@brief   InnerFoup Class ������
@author	 LSJ
@date    2012.10.27
@param   pHWCommon : HWCommon Class�� ������
*/
InnerFoup::InnerFoup(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   InnerFoup Class �Ҹ���
@author	 LSJ
@date    2012.10.27
*/
InnerFoup::~InnerFoup()
{
}

/**
@brief   Foup ���� ���θ� üũ�ϴ� �Լ�
@author  zzang9un
@date    2013.08.05
@return  Foup�� ������ true, ������ false�� ����
*/
bool InnerFoup::IsFoupDetect()
{
	bool bResult = false;

	if(m_pHWCommon->m_Input.FOUP_Detect == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}


bool InnerFoup::IsEuvDetect()
{
	bool bResult = false;

// EUV_1, EUV_2 ��� �̸��� �ٸ� OHT define���δ� �����Ƿ� �̷��� �ص־� ���� �ȳ���
// EUV�� �̿��� ������ �׻� false�� ����
#if (((OHT_NAME == OHT_NAME_STD_V80_RETICLE)|| (OHT_NAME == OHT_NAME_STD_V85))&& (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON) && (m_pHWCommon->m_Input.POD_EUV_2 == ON))
#else
	if((m_pHWCommon->m_Input.POD_EUV_1 == ON))
#endif
		bResult = true;
	else
		bResult = false;
#endif

	return bResult;
}



