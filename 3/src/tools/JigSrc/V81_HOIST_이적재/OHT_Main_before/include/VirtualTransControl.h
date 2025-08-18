/**
@file    VirtualTransControl.h
@version OHT 7.0
@brief   TransControl�� ����ϴ� Vitual TransControl �Լ�
*/

#ifndef VirtualTransControlH
#define VirtualTransControlH

#include <stdio.h>
#include "VirtualModuleControl.h"

/**
@brief   �Ŵ��󿡼� ����� ����� class
@author  zzang9un
@date    2013.06.28
*/
class VirtualTransControl : public VirtualModuleControl
{
public:
    VirtualTransControl(void) : VirtualModuleControl()    /// ������, ���� �ʱ�ȭ
    {
        
    }
    virtual ~VirtualTransControl(void){;} /// ���� �Ҹ���

public:
    virtual int ChangeUse(int UseIndex, bool YesOrNo) = 0;    ///< Use ���� �����Լ�
    virtual TRANSE_DIFFERENCE getDifference() = 0;	///< Rail ���� Jig �ٴڸ������ Offset
};
//---------------------------------------------------------------------------

#endif


