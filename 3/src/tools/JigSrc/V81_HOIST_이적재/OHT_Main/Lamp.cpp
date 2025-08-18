/**
@file    Lamp.cpp
@version OHT 7.0
@brief   Lamp Class CPP File
*/

#include "Lamp.h"

/**
@brief   Lamp Class ������
@author
@date
@param   pHWCommon : HWCommon Class�� ������
*/
Lamp::Lamp(HWCommon *pHWCommon)
{
    m_pHWCommon = pHWCommon;
}

/**
@brief   Lamp Class �Ҹ���
@author
@date
*/
Lamp::~Lamp()
{
}

/**
@brief   ��� ���� Lamp�� On�ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@param   nLampStatus(Color �� �Ѱ� �������� ������ ���� ����)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
@Modify  P-PJT P1_2L OHT I/O�߰��� ���� Lamp���� ���� #JRespect 2018.01.03
*/
int Lamp::LampOn(int nLampStatus)
{
    int nError = NO_ERR;

    switch (nLampStatus)
	{
    case SETLAMP_R_ON:
    case SETLAMP_R_FLICKER:
        nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
        break;
    case SETLAMP_Y_ON:
    case SETLAMP_Y_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, ON);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        break;
    case SETLAMP_G_ON:
    case SETLAMP_G_FLICKER:
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
        break;
    case SETLAMP_ALL_ON:
        nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
        if (nError == NO_ERR)
			//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        if (nError == NO_ERR)
            nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

    // Red ����, Green ����
	case SETLAMP_R_ON_G_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
			 nError = m_pHWCommon->SetBit(OUT_LAMP_RED, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

	// Yellow ����, Yellow ����
	case SETLAMP_G_FLICKER_Y_FLICKER:
			nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
			nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

	// Green ����, Yellow_LEFT ����
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
	   //nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
        if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;
	// Green ����, Yellow_RIGHT ����
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		if (nError == NO_ERR)
			nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, ON);
		break;

    }
    return nError;
}
int Lamp::LampYellowSteerOn(int nLampStatus)
{
    int nError = NO_ERR;

    switch (nLampStatus)
	{
	// Green ����, Yellow_LEFT ����
	case SETLAMP_G_FLICKER_Y_L_FLICKER:
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, ON);
		break;
	// Green ����, Yellow_RIGHT ����
	case SETLAMP_G_FLICKER_Y_R_FLICKER:
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, ON);
		break;

    }
    return nError;
}
/**
@brief   ��� ���� Lamp�� Off�ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
@Modify  P-PJT P1_2L OHT I/O�߰��� ���� Lamp���� ���� #JRespect 2018.01.03
*/
int Lamp::LampOff()
{
    int nError = 0;

    nError = m_pHWCommon->SetBit(OUT_LAMP_RED, OFF);

    if (nError == NO_ERR)
		//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
		nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);

    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, OFF);

    return nError;
}

/**
@brief   Red �� ������ Lamp�� Off�ϴ� �Լ�
@date    2016.03.10
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
@Modify  P-PJT P1_2L OHT I/O�߰��� ���� Lamp���� ���� #JRespect 2018.01.03 ���������
**/
int Lamp::LampOff_Except_Red()
{
    int nError = 0;

	//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);

    if (nError == NO_ERR)
        nError = m_pHWCommon->SetBit(OUT_LAMP_GREEN, OFF);

    return nError;
}
/**
@brief   Yellow Lamp�� Off�ϴ� �Լ�
@author  JRespect
@date    2018.01.09
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
**/
int Lamp::LampOff_Yellow()
{
    int nError = 0;

	//nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_RIGHT, OFF);
	nError = m_pHWCommon->SetBit(OUT_LAMP_YELLOW_LEFT, OFF);
    return nError;
}
/**
@brief   Lamp ���� ����� On/Off ���� ȭ�� �Լ�
@author  LSJ
@date    2012.10.31
@param   nColor(Red : 0, Yellow : 1, Green : 2)
@return  ����� ���� ������ true, ���� ������ false ����.
@Modify  P-PJT P1_2L OHT I/O�߰��� ���� Lamp���� ���� #JRespect 2018.01.03
*/
bool Lamp::IsLampOn(int nColor)
{
    bool bResult = false;

    switch (nColor)
    {
    case DEF_RED:
        if (m_pHWCommon->m_Output.LAMP_Red == ON)
            bResult = true;
        else
            bResult = false;
        break;
	case DEF_YELLOW_RIGHT:
		//if (m_pHWCommon->m_Output.LAMP_Yellow == ON)
		if (m_pHWCommon->m_Output.LAMP_Yellow_Right == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_YELLOW_LEFT:
		//if (m_pHWCommon->m_Output.LAMP_Yellow == ON)
		if (m_pHWCommon->m_Output.LAMP_Yellow_Left == ON)
			bResult = true;
		else
			bResult = false;
		break;
	case DEF_GREEN:
        if (m_pHWCommon->m_Output.LAMP_Green == ON)
            bResult = true;
        else
            bResult = false;
        break;
    }

    return bResult;
}

/**
@brief   OHT�� ���¿� ���� Lamp�� Control�ϴ� �Լ�
@author  LSJ
@date    2012.10.31
@param   nLampStatus(OHT�� Status�� ���� ���� ����)
@return  SetBit�� ���� ������ 0, ���� �߻� �� ���� �ڵ� Return
@Modify  P-PJT P1_2L OHT I/O�߰��� ���� Lamp���� ���� #JRespect 2018.01.03 ������ ����
*/
int Lamp::LampControl(int nLampStatus)
{
    int nError = NO_ERR;
	int nRedOut = 0, nGreenOut = 0;
	//int nYellowOut = 0;
	int nYellowRightOut = 0,  nYellowLeftOut = 0;
    static int nPreLampStatus = -1;

	nRedOut = IsLampOn(DEF_RED);
	//nYellowOut = IsLampOn(DEF_YELLOW);
	nGreenOut = IsLampOn(DEF_GREEN);
	nYellowLeftOut = IsLampOn(DEF_YELLOW_LEFT);
	nYellowRightOut = IsLampOn(DEF_YELLOW_RIGHT);

    switch (nLampStatus)
    {
	case SETLAMP_ALL_OFF:
		//if ((nRedOut == 1) || (nYellowOut == 1) || (nGreenOut == 1))
		if ((nRedOut == 1) || (nYellowLeftOut == 1) || (nGreenOut == 1) || (nYellowRightOut == 1))
			nError = LampOff();
		break;

    case SETLAMP_Y_ON:
		//if ((nYellowOut == 0) || (nPreLampStatus != nLampStatus))
		if ((nYellowRightOut == 0) || (nYellowLeftOut == 0) || (nPreLampStatus != nLampStatus))
            nError = LampOff();
			nError = LampOn(nLampStatus);
        break;
    case SETLAMP_G_ON:
        if ((nGreenOut == 0) || (nPreLampStatus != nLampStatus))
        {
            nError = LampOff();
            nError = LampOn(nLampStatus);
        }
        break;
    case SETLAMP_Y_FLICKER:
		//if ((nYellowOut == 0) && (nPreLampStatus == nLampStatus))
		if ((nYellowRightOut == 0) && (nYellowLeftOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;
    case SETLAMP_G_FLICKER:
        if ((nGreenOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;

    case SETLAMP_R_FLICKER:
        if ((nRedOut == 0) && (nPreLampStatus == nLampStatus))
            nError = LampOn(nLampStatus);
        else
            nError = LampOff();
        break;
    }

    nPreLampStatus = nLampStatus;

    return nError;
}
