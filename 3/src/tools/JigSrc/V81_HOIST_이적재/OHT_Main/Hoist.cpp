/**
@file    Hoist.cpp
@version OHT 7.0
@brief   Hoist Class CPP File
*/

#include "Hoist.h"
#include "HoistDistanceConverter.h"

/**
@brief   Hoist Class ������
@date    
@author  
@param   pHWCommon : HWCommon Ŭ���� ������
@param   nAxisNum : Motor�� ���� �� ��ȣ
*/
Hoist::Hoist(HWCommon *pHWCommon, int nAxisNum):MotionAxis(pHWCommon, nAxisNum, true)
{
	m_pHWCommon = pHWCommon;
	m_nAxisNum = nAxisNum;
	memset(&m_HoistParameter, 0, sizeof(THREE_LINEAR_EQUATION_GEAR_RATIO));

	m_HoistParameter.PositionValue[0].dPulse = -500.0;
	m_HoistParameter.PositionValue[0].dUserUnit = -2.0;
	m_HoistParameter.PositionValue[1].dPulse = 20000.0;
	m_HoistParameter.PositionValue[1].dUserUnit = 100.0;
	m_HoistParameter.PositionValue[2].dPulse = 128000.0;
	m_HoistParameter.PositionValue[2].dUserUnit = 637.0;
	m_HoistParameter.PositionValue[3].dPulse = 300000.0;
	m_HoistParameter.PositionValue[3].dUserUnit = 1460.0;
	m_HoistParameter.PositionValue[4].dPulse = 450000.0;
	m_HoistParameter.PositionValue[4].dUserUnit = 2153.0;
	m_HoistParameter.PositionValue[5].dPulse = 620000.0;
	m_HoistParameter.PositionValue[5].dUserUnit = 2901.0;
	m_HoistParameter.PositionValue[6].dPulse = 780000.0;
	m_HoistParameter.PositionValue[6].dUserUnit = 3579.0;
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	m_HoistParameter.PositionValue[7].dPulse = 940000.0;
	m_HoistParameter.PositionValue[7].dUserUnit = 4225.0;
	m_HoistParameter.PositionValue[8].dPulse = 1110000.0;
	m_HoistParameter.PositionValue[8].dUserUnit = 4881.0;
	m_HoistParameter.PositionValue[9].dPulse = 1290000.0;
	m_HoistParameter.PositionValue[9].dUserUnit = 5556.0;
#endif
	m_pHoistDistanceConverter = new HoistDistanceConverter();
	SetHoistParam(m_HoistParameter);
//    m_pHoistDistanceConverter->calcCoefficient(6830+nOffset, -3.0, 250000+nOffset, 732, 550000+nOffset, 1594, 1240000+nOffset, 3400);    
	EnableUnitConverter();

	SetHoistOriginFromSmalladd(false);
}

/**
@brief   Hoist Class �Ҹ���
@date    
@author
*/
Hoist::~Hoist()
{
	DisableUnitConverter();
	delete m_pHoistDistanceConverter;	// ���� �ڷ� ����
	m_pHoistDistanceConverter = NULL;
}

/**
@brief   // �Է�
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  // �Է�(���� ��� ����)
*/
bool Hoist::IsHoistHome()
{

	bool bResult = false;

	if(m_pHWCommon->m_Input.HOIST_Home == ON)
		bResult = true;
	else
		bResult = false;

	return bResult;
}

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S)|| (OHT_NAME == OHT_NAME_STD_V85)|| (OHT_NAME == OHT_NAME_STD_V80_RETICLE))
/**
@brief   // �Է�
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  // �Է�(���� ��� ����)
*/
bool Hoist::IsHoistBeltDetect()
{
	bool bResult = false;
#if ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	if(m_pHWCommon->m_Input.HOIST_Belt_Detect1 == ON)
	{
		HoistExternalBrakeReleaseOnOFF(false);
		bResult = true;
	}
	else
		bResult = false;
#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1)) 
	if((m_pHWCommon->m_Input.HOIST_Belt_Detect1 == ON) || (m_pHWCommon->m_Input.HOIST_Belt == ON))
	{
		HoistExternalBrakeReleaseOnOFF(false);
		bResult = true;
	}
	else
		bResult = false;
#elif((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V81S))
	if((m_pHWCommon->m_Input.HOIST_Belt_Detect1 == ON) || (m_pHWCommon->m_Input.HOIST_Belt_Detect2 == ON))
	{
//		HoistExternalBrakeReleaseOnOFF(false);
		bResult = true;
	}
	else
		bResult = false;

#elif(OHT_NAME == OHT_NAME_STD_V85)
	if((m_pHWCommon->m_Input.HOIST_Belt_Detect1 == ON) || (m_pHWCommon->m_Input.HOIST_Belt_Detect2 == ON))
	{
		HoistExternalBrakeReleaseOnOFF(false);
		bResult = true;
	}
	else
		bResult = false;
#else
	if(m_pHWCommon->m_Input.HOIST_Belt_Detect == ON)
	{
		HoistExternalBrakeReleaseOnOFF(false);
		bResult = true;
	}
	else
		bResult = false;

#endif


	return bResult;
}

/**
@brief   // �Է�
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  // �Է�(���� ��� ����)
*/
int Hoist::HoistExternalBrakeReleaseOnOFF(bool OnOff)
{

	int nError = NO_ERR;

	if(OnOff == true)
	{
		if( m_pHWCommon->m_Output.HOIST_Ext_Brake_Release == OFF )
			nError = m_pHWCommon->SetBit(OUT_HOIST_EXT_BRAKE_RELEASE, ON);
	}
	else
		nError = m_pHWCommon->SetBit(OUT_HOIST_EXT_BRAKE_RELEASE, OFF);

	return nError;
}
#endif

/**
@brief   Hoist�� ���� ��ȯ ��� ��� ���� �Լ�

	���� ������ m_pHoistDistanceConverter�� �̿��Ѵ�.
@date    // �Է�
@author  // �Է�
@param   // �Է�(���� ��� ����)
@return  Unit ��� ���� �ϸ� true Return
*/
bool Hoist::EnableUnitConverter(void)
{
	if(m_pHoistDistanceConverter==NULL)
	{
		return false;
	}
	else
	{
		m_pUnitConverter = m_pHoistDistanceConverter;
		return true;
	}
}


/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitSpeed ����� ���� �ӵ�
@return	����� ��� �ӵ�
*/
double Hoist::ConvertSpeedUnit(double dInput, bool bUserUnit)
{
	double dTmpSpeed = 0;
	double dTmpPosition = 0.0;
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			dTmpPosition = GetCurrentPosition();
			dTmpSpeed = m_pHoistDistanceConverter->calcSpeedUser2Cts(dInput, dTmpPosition);
			return dTmpSpeed;
		}
		else
		{
			dTmpPosition = m_pHWCommon->GetCurrentPosition(m_nAxisNum);
			dTmpSpeed = m_pHoistDistanceConverter->calcSpeedCts2User(dInput, dTmpPosition);
			return dTmpSpeed;

		}
	}
	else
	{
		return dInput;
	}
}

/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitSpeed ����� ���� �ӵ�
@return	����� ��� �ӵ�
*/
double Hoist::ConvertAccelerationUnit(double dInput, bool bUserUnit)
{
	double dTmpAccel = 0;
	double dTmpPosition = 0.0;
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			dTmpPosition = GetCurrentPosition();
			dTmpAccel = m_pHoistDistanceConverter->calcAccelUser2Cts(dInput, dTmpPosition);
			return dTmpAccel;
		}
		else
		{
			dTmpPosition = m_pHWCommon->GetCurrentPosition(m_nAxisNum);
			dTmpAccel = m_pHoistDistanceConverter->calcAccelCts2User(dInput, dTmpPosition);
			return dTmpAccel;
		}
	}
	else
	{
		return dInput;
	}
}

bool Hoist::SetHoistParam(THREE_LINEAR_EQUATION_GEAR_RATIO HoistParam)
{
	m_HoistParameter = HoistParam;

	//3���� ù��° �׷���
	m_pHoistDistanceConverter->calcCoefficient(	
		HoistParam.PositionValue[0].dPulse, HoistParam.PositionValue[0].dUserUnit,
		HoistParam.PositionValue[1].dPulse, HoistParam.PositionValue[1].dUserUnit,
		HoistParam.PositionValue[2].dPulse, HoistParam.PositionValue[2].dUserUnit,
		HoistParam.PositionValue[3].dPulse, HoistParam.PositionValue[3].dUserUnit		);

	//3���� �ι�° �׷���
	m_pHoistDistanceConverter->calcCoefficient2(
		HoistParam.PositionValue[3].dPulse, HoistParam.PositionValue[3].dUserUnit,
		HoistParam.PositionValue[4].dPulse, HoistParam.PositionValue[4].dUserUnit,
		HoistParam.PositionValue[5].dPulse, HoistParam.PositionValue[5].dUserUnit,
		HoistParam.PositionValue[6].dPulse, HoistParam.PositionValue[6].dUserUnit		);
#ifdef OHT_HIGH_RAIL_HOIST_PRAM
	//3���� ����° �׷���
	m_pHoistDistanceConverter->calcCoefficient3(
		HoistParam.PositionValue[6].dPulse, HoistParam.PositionValue[6].dUserUnit,
		HoistParam.PositionValue[7].dPulse, HoistParam.PositionValue[7].dUserUnit,
		HoistParam.PositionValue[8].dPulse, HoistParam.PositionValue[8].dUserUnit,
		HoistParam.PositionValue[9].dPulse, HoistParam.PositionValue[9].dUserUnit		);
#endif
}



// hoist home�� small add�� ���� 6mm������ �����ϴµ� �ʿ�
double Hoist::GetRelativeDistanceFromSmalladd()
{
	double dRelativeDistance;
	double dSmallAddPosition;
	double dCurrentPosition;

	dCurrentPosition = GetCurrentPosition();
	dSmallAddPosition = GetSmallAddCheckPosition();

	dRelativeDistance = dSmallAddPosition - dCurrentPosition;

	// 6mm�� return �ؾ� ����
	return dRelativeDistance;
}

//SmallAdd�� ���� ����Ǵ� Inpos�� ??? ��� �ʿ�.
double Hoist::GetRelativeDistanceFromSmalladdCurrentPosition()
{
	double dRelativeDistance;
	double dSmallAddPosition;

	dSmallAddPosition = GetRelativeDistanceFromSmalladd();
#if(OHT_NAME == OHT_NAME_STD_V85)
	if((GetCurrentPosition() - GetSmallAddCheckPosition()) < 0.0)
		dRelativeDistance = GetCurrentPosition() - (GetSmallAddCheckPosition() - HOIST_SMALL_ADD_HOISTHOME_DISTANCE);
	else
		dRelativeDistance = GetCurrentPosition();
	//
#else
    dRelativeDistance = GetCurrentPosition();
#endif
	return dRelativeDistance;
}

bool   Hoist::IsHoistOriginFromSmalladd()
{
	return bHoistOrigin;
}

void Hoist::SetHoistOriginFromSmalladd(bool bOrigin)
{
	 bHoistOrigin =  bOrigin;
}

double Hoist::GetHoistMaxUserPosition()
{
  	return m_pHoistDistanceConverter->GetHoistMaxUserPosition();
}

//Min�� ������� ����... ������ Min���� �����ϱ⶧���� �ǹ̰� ���� ��...
double Hoist::GetHoistMinUserPosition()
{
    return m_pHoistDistanceConverter->GetHoistMinUserPosition();
}




