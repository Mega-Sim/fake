//---------------------------------------------------------------------------
#pragma hdrstop

#include "FoupCoverOpenDetect.h"


/**
@brief   FoupCoverOpenDetect ������
@author  KKS
@date    2015.01.20
@param   pHWCommon : HWCommon Class�� ������
*/
FoupCoverOpenDetect::FoupCoverOpenDetect(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   FoupCoverOpenDetect �Ҹ���
@author  KKS
@date    2015.01.20
*/

 FoupCoverOpenDetect::~FoupCoverOpenDetect()
{
}

/**
@brief   Foup cover open ���������� üũ�ϴ� �Լ�
@author  KKS
@date    2015.01.20
@return  Open ���� true, �׷��� ���� ��� false
*/
bool FoupCoverOpenDetect::IsDetect()
{
	bool bResult = false;

#if (((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)|| (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)) && (SAMSUNG_COMMON_SPEC==ON))
 	if(m_pHWCommon->m_Input.FOUP_Cover_Open_Detect == ON)
	{
		bResult = true;
	}
#endif

	return bResult;
}

#pragma package(smart_init)
