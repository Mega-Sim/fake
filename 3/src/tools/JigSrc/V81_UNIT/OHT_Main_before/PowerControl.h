//---------------------------------------------------------------------------
/**
@file    PowerControl.h
@version OHT 7.0
@brief   PowerControl Class Header File
*/

#ifndef PowerControlH
#define PowerControlH

#include "HWCommon.h"

/**
@class   PowerControl
@date    2012.10.22
@author  LSJ
@brief   MC(Magnetic Contactor) ���� Class
@note    EMO�� �ܺ� ���¿� ���� MC�� Control�ϱ� ���� �Լ� ����
*/
class PowerControl
{
private:
	HWCommon *m_pHWCommon;

public:

	PowerControl(HWCommon *pHWCommon);
	~PowerControl();

	int MCControl(bool);
	int ResetController(void);
	bool IsOnEMO();
	bool IsOnSMPSFault();

};

#endif  //PowerControlH
