/**
@file    SerialSelect.cpp
@version OHT 7.0
@brief   SerialSelect Class CPP File
*/

#include "SerialSelect.h"

#define DEF_SHUTTER     0
#define DEF_OBS         1
#define DEF_OHTDETECT   2
#define DEF_LOOKDOWN    3

/**
@brief   SerialSelect Class ������
@author
@date
@param   pHWCommon : HWCommon Class�� ������
*/
SerialSelect::SerialSelect(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
    Init();
}

/**
@brief   SerialSelect Class �Ҹ���
@author
@date
*/
SerialSelect::~SerialSelect()
{
}

/**
@brief   SerialSelect �ʱ�ȭ �Լ�
@author
@date
@return
*/
int SerialSelect::Init()
{
    int nError = 0;

    nError = SetSerial(0);

    return nError;
}

/**
@brief   SerialSelect Set �Լ�
@author
@date
@return
*/
int SerialSelect::SetSerial(int nDevice)
{
    int nError = 0;

    switch (nDevice)
    {
    case DEF_SHUTTER:
        nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_1, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_2, OFF);
        break;
    case DEF_OBS:
        nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_1, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_2, OFF);
        break;
    case DEF_OHTDETECT:
        nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_1, OFF);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_2, ON);
        break;
    case DEF_LOOKDOWN:
        nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_1, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_SERIAL_SELECT_2, ON);
        break;
    }

    return nError;
}

