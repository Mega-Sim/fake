//---------------------------------------------------------------------------
/**
@file    TransStopTag.h
@version OHT 7.0
@brief   TransStopTag Class Header File
*/

#ifndef TransStopTagH
#define TransStopTagH


#include "HWCommon.h"

/**
@class   TransStopTag
@date    2012.10.22
@author  LSJ
@brief   TransStopTag ���� Class
@note    TransStopTag ���õ� ���� ���¸� Ȯ���ϱ� ���� �Լ� ����
*/
class TransStopTag
{
private:
	HWCommon *m_pHWCommon;

public:

	TransStopTag(HWCommon *pHWCommon);
	~TransStopTag();

	bool IsOnFrontSensor();
	bool IsOnRearSensor();
	bool IsTransStopPosition();

};
//---------------------------------------------------------------------------
#endif  //TransStopTagH
