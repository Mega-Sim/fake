/**
@file    Shift.h
@version OHT 7.0
@brief   Shift Class Header File
*/

#ifndef ShiftH
#define ShiftH

#include "HWCommon.h"			// HW ���� ���
#include "PropotionalMotionAxis.h"	// ��� ���� �� ��� 


/**
@class   Shift
@date    2012.10.17
@author  ���¿�
@brief   Shift Motor ���� Class
*/
class Shift : public PropotionalMotionAxis	//MotionAxis
{
private:
	HWCommon *m_pHWCommon;		///< HW �� ������ �����

public:
	Shift(HWCommon *pHWCommon, int nAxisNum);						///< ������, ���� ���� 
	Shift(HWCommon *pHWCommon, int nAxisNum, double dGearRatio);	///< ������, ���� ���� 
	~Shift();

	bool IsShiftHome();	///< IO ���� : Home ���� ���� Ȯ��

};

#endif  // ShiftH

