//---------------------------------------------------------------------------
/**
@file    TrackBufferDetect.cpp
@version OHT 7.0
@brief   TrackBufferDetect Class �ҽ� File
*/

#pragma hdrstop

#include "TrackBufferDetect.h"


/**
@brief   TrackBufferDetect Class ������
@author	 LSJ
@date	 2012.10.22
@param   pHWCommon : HWCommon Ŭ���� ������
*/
TrackBufferDetect::TrackBufferDetect(HWCommon *pHWCommon)
{
	m_pHWCommon = pHWCommon;
}

/**
@brief   TrackBufferDetect Class �Ҹ���
@author	 LSJ
@date	 2012.10.22
*/
TrackBufferDetect::~TrackBufferDetect()
{

}

/**
@brief	 Left STB�� Foup�� �ִ��� ������ üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 Left STB�� Foup�� ������ true, ������ false.(������ �ݻ����� �����ϴ� ���� ������.)
*/
bool TrackBufferDetect::IsLeftSTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK))
#else
	if(m_pHWCommon->m_Input.STB_Detect_Left == ON)
		bResult = true;
	else
		bResult = false;
#endif
	return bResult;
}

/**
@brief	 Right STB�� Foup�� �ִ��� ������ üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 Right STB�� �ݼ۹��� ������ true, ������ false.(������ �ݻ����� �����ϴ� ���� ������)
*/
bool TrackBufferDetect::IsRightSTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE)  || (OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V86) || (OHT_NAME == OHT_NAME_P4_MASK))
#else
    // by zzang9un 2013.07.09 : ���� ����� ���� �ݼ۹� ���� ���� �Ǵ�
	if(m_pHWCommon->m_Input.STB_Detect_Right == ON)
		bResult = true;
	else
		bResult = false;
#endif

	return bResult;
}

/**
@brief	 UTB�� Foup�� �ִ��� ������ üũ�ϴ� �Լ�
@author	 LSJ
@date	 2012.10.22
@return	 UTB�� �ݼ۹��� ������ true, ������ false.(������ �ݻ����� �����ϴ� ���� ������.)
*/
bool TrackBufferDetect::IsUTBFoupDetect()
{
	bool bResult = false;

// 2016-11-30, Labeler
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE))         //  || (OHT_NAME == OHT_NAME_STD_V85)
#else
    // by zzang9un 2013.07.09 : ���� ����� ���� �ݼ۹� ���� ���� �Ǵ�
	if(m_pHWCommon->m_Input.UTB_Detect == ON)
		bResult = true;
	else
		bResult = false;
#endif
	return bResult;
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
