/**
@file    MotionAxis.cpp
@version OHT 7.0
@brief   MotionAxis Class CPP File
*/

#include <math.h>
#include "MotionAxis.h"
#include "LogHandler.h"

#define ERR_NO_HWCOMMON	-1

/**
@brief   MotionAxis Class ����Ʈ ������
@author
@date    2012-11-01
*/
MotionAxis::MotionAxis()
{
	m_nAxisNum = 0;
	m_pHWCommon = NULL;
	m_pUnitConverter = NULL;
	m_bAbsoluteEncoder = false;
	m_dDistOfMoveDistance = 0.0;
}

/**
@brief   MotionAxis Class ������
@author
@date    2012-11-01
@param   pHWCommon : HWCommon Ŭ���� ������
@param   nAxisNum : Motor�� ���� �� ��ȣ
*/
MotionAxis::MotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder)
{
	m_nAxisNum = 0;
	m_pHWCommon = NULL;
	m_pUnitConverter = NULL;
	m_bAbsoluteEncoder = bAbsoluteEncoder;
	m_dDistOfMoveDistance = 0.0;

	if(nAxisNum>=0)
	{
		m_nAxisNum = nAxisNum;
	}
	if(m_pHWCommon==NULL)
	{
		m_pHWCommon = pHWCommon;
	}

}

/**
@brief   MotionAxis Class �Ҹ���
@author
@date    2012-11-01
*/
MotionAxis::~MotionAxis()
{
	m_pUnitConverter = NULL;
	m_pHWCommon = NULL;
	m_nAxisNum = 0;
}

/**
@brief   MotionAxis �ʱ�ȭ �Լ�
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::Init()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->Init(m_nAxisNum);
}

/**
@brief   UnitConverter �̻�� ���� �Լ�
@author  ehoto
@date    2012-11-01
*/
void MotionAxis::DisableUnitConverter(void)
{
	m_pUnitConverter = NULL;
}

/**
@brief   UnitConverter
@author  ehoto
@date    2012-11-01
@param	dUserUnitDistance ����� ���� �Ÿ�
@return	����� ��� �Ÿ�
*/
double MotionAxis::ConvertDistanceUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User --> Cts
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);
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
double MotionAxis::ConvertSpeedUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User/msec --> Cts/msec
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);	// Cts/msec --> User/msec
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
double MotionAxis::ConvertAccelerationUnit(double dInput, bool bUserUnit)
{
	if(m_pUnitConverter!=NULL)
	{
		if(bUserUnit==true)
		{
			return m_pUnitConverter->calcUser2Cts(dInput);	// User/msec2 --> Cts/msec2
		}
		else
		{
			return m_pUnitConverter->calcCts2User(dInput);	// Cts/msec2 --> User/msec2
		}
	}
	else
	{
		return dInput;
	}
}

/// ���� Ȯ��
/**
@brief   MotionAxis ���� Ȯ�� �ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@return  �������̸� true / �����̸� false
*/
bool MotionAxis::IsRunning()
{
	if(m_pHWCommon==NULL) return false;
	return m_pHWCommon->IsRunning(m_nAxisNum);
}

/**
@brief   MotionAxis ����ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  ����ġ�̸� true / ����ġ �ƴϸ� false
*/
bool MotionAxis::IsInPosition()
{
	if(m_pHWCommon==NULL) return false;
	return m_pHWCommon->IsInPosition(m_nAxisNum);
}

/**
@brief   MotionAxis ��� ���� �Ϸ� Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  �Ϸ��̸� true / �Ϸ� �ƴϸ� false
*/
bool MotionAxis::IsAxisDone()
{
	if(m_pHWCommon==NULL) return false;
	if(IsInPosition()==true && IsRunning()==false) return true;	//1	���߿� AMC ���� �� �޾ƿ����� ����
	else return false;
}


/**
@brief   MotionAxis AMP On/Off ���� Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  ���� ��� ���¸� true / ���� ���¸� false ( ex: Servi On / Off )
*/
bool MotionAxis::IsAMPEnable()
{
	if(m_pHWCommon==NULL) return false;

	return m_pHWCommon->IsAMPEnable(m_nAxisNum);
}

/**
@brief   MotionAxis ���� ��ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  ���� ��ġ�� �Ǽ��� ������
*/
double MotionAxis::GetCurrentPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetCurrentPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetCurrentPosition(m_nAxisNum), false);	// cts --> unit
	}
}

/**
@brief   MotionAxis ���� ���� ��ġ Ȯ�� �Լ�
@author  doori.shin
@date    2016.04.26
@return  ���� ���� ��ġ�� �Ǽ��� ������
*/
double MotionAxis::GetPrePosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetPrePosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetPrePosition(m_nAxisNum), false);	// cts --> unit
	}
}

/**
@brief   MotionAxis ���� ���� �Ÿ��� ������
@author  doori.shin
@date    2016.04.26
@param	 bInit : �����Ÿ��� ������ �� �ʱ�ȭ ����
@return  ���� �̵� �����Ÿ��� �Ǽ��� ������
*/
double MotionAxis::GetRunningPosition(bool bInit, bool bConvert)
{
	double dReturnValue = 0.0;

	if(m_pHWCommon == NULL)
	{
		dReturnValue = 0.0;
	}
	else
	{
		//�� �����ϴ��� Ȯ��
		if((bConvert == ON) && (m_pUnitConverter != NULL))
		{
			dReturnValue = ConvertDistanceUnit(m_pHWCommon->GetTotalPosition(m_nAxisNum), false);
		}
		else
		{
			dReturnValue = m_pHWCommon->GetTotalPosition(m_nAxisNum);
		}

		//�����Ÿ� �ʱ�ȭ
		if(bInit == true)
		{
			m_pHWCommon->ResetTotalPosition(m_nAxisNum);
        }
	}

	return dReturnValue;
}

/**
@brief   MotionAxis ���� ���� ������ Ƚ���� ������
@author  doori.shin
@date    2016.04.26
@param	 bInit : ���� ������ �� �ʱ�ȭ ����
@return  ���� ���� ������ Ƚ���� ������
*/
int MotionAxis::GetTotalMoveCount(bool bInit)
{
	int nReturnValue = 0;

	if(m_pHWCommon == NULL)
	{
        nReturnValue = 0;
	}
	else
	{
		nReturnValue = m_pHWCommon->GetTotalMoveCount(m_nAxisNum);

		if(bInit == true)
		{
			m_pHWCommon->ResetTotalMoveCount(m_nAxisNum);
        }
	}

	return nReturnValue;
}


/**
@brief   MotionAxis Small Add Move �� ������ ������ ��ġ Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  Small Add Move �� ������ ������ ��ġ�� ǥ����
*/
double MotionAxis::GetSmallAddCheckPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetSmallAddCheckPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetSmallAddCheckPosition(m_nAxisNum), false);	// cts --> unit
	}
}


/**
@brief	 MotionAxis ���� ��ġ Ȯ�� �Լ�
@author  ehoto
@date	 2012-11-01
@return  ���� ��ġ�� �Ǽ��� ������
*/
double MotionAxis::GetFinalPosition()
{
	if(m_pHWCommon==NULL) return 0.0;
	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetFinalPosition(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetFinalPosition(m_nAxisNum), false); // cts --> unit
	}
}


/**
@brief   MotionAxis ���� �ӵ� Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  ���� �ӵ��� �Ǽ��� ������
*/
double MotionAxis::GetCurrentVelocity()
{
	if(m_pHWCommon==NULL) return 0.0;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetCurrentVelocity(m_nAxisNum);
	}
	else
	{
		return ConvertSpeedUnit(m_pHWCommon->GetCurrentVelocity(m_nAxisNum), false);	// cts/msec --> unit/msec
	}
}

/**
@brief   MotionAxis Following Error Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  Following Error�� �Ǽ��� ������
*/
double MotionAxis::GetFollowingError()
{
	if(m_pHWCommon==NULL) return 0.0;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetFollowingError(m_nAxisNum);
	}
	else
	{
		return ConvertDistanceUnit(m_pHWCommon->GetFollowingError(m_nAxisNum), false);	// cts --> unit
	}
}

double MotionAxis::GetFollowingError_cts()
{
	if(m_pHWCommon==NULL) return 0.0;

	return m_pHWCommon->GetFollowingError(m_nAxisNum);
}


/**
@brief   MoveDistance ����� ���� �Ÿ��� Return �ϴ� �Լ�
@author  ehoto
@date    2013-02-03
@return  MoveDistance ����� ���� �Ÿ��� �Ǽ��� ������
*/
double MotionAxis::GetDistOfRemain()
{
	double dDistOfRemain = GetFinalPosition()-GetCurrentPosition();	//m_dDistOfMoveDistance-
//	double dDistOfRemain = m_dDistOfMoveDistance-GetCurrentPosition();
	return dDistOfRemain;
}

/// ���� ����
/**
@brief   MotionAxis Error Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@return  �ະ Error Code
*/
int MotionAxis::GetError()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->GetError(m_nAxisNum);
}

/**
@brief   MotionAxis reset �Լ�
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::Reset()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->Reset(m_nAxisNum);
}

/// Offset ����
/**
@brief   MotionAxis ���� ��ġ Home ���� �Լ�
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::SetHome()
{
	double dTmpOffset = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;


	if(m_bAbsoluteEncoder==false)
	{
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome m_bAbsoluteEncoder==false");
		m_dDistOfMoveDistance = 0.0;
		return m_pHWCommon->SetHome(m_nAxisNum);
	}
	else
	{
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome m_bAbsoluteEncoder==true");
		m_pHWCommon->GetOffset(m_nAxisNum, &dTmpOffset);
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome_GetOffset Axis=%d, Offset=%7.1lf", m_nAxisNum, dTmpOffset);
		dTmpOffset += m_pHWCommon->GetCurrentPosition(m_nAxisNum); 	// ���� ��ġ�� Home���� ����
		//WRITE_LOG_ON_THIS_FILE("D:\\log\\OriginTest.log", "SetHome_SetOffset Axis=%d, Offset=%7.1lf", m_nAxisNum, dTmpOffset);
		return m_pHWCommon->SetOffset(m_nAxisNum, dTmpOffset);
	}
}

/// Limit ����
/**
@brief   MotionAxis Postive S/W Limit ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   dLimit : Limit �Ÿ�
@return  Error Code
*/
int MotionAxis::SetPositiveSWLimit(double dLimit)
{
	if(m_pHWCommon==NULL) return -1;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, dLimit);
	}
	else
	{
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
		else
		{
			return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
	}
}

/**
@brief   MotionAxis Negative S/W Limit ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   dLimit : Limit �Ÿ�
@return  Error Code
*/
int MotionAxis::SetNegativeSWLimit(double dLimit)
{
	if(m_pHWCommon==NULL) return -1;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, dLimit);
	}
	else
	{
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->SetNegativeSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
		else
		{
			return m_pHWCommon->SetPositiveSWLimit(m_nAxisNum, ConvertDistanceUnit(dLimit, true));
		}
	}
}

/**
@brief   MotionAxis S/W Limit Ȯ�� �Լ�
@author  ehoto
@date    2012-11-01
@param   dPosLimit : Positive Limit �Ÿ�
@param   dNegLimit : Negative Limit �Ÿ�
@return  Error Code
*/
int MotionAxis::GetSWLimit(double& dPosLimit, double& dNegLimit)
{
	double dTmpPosLimit = 0.0, dTmpNegLimit = 0.0;
	int nReturnValue = 0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->GetSWLimit(m_nAxisNum, dPosLimit, dNegLimit);
	}
	else
	{
		nReturnValue = m_pHWCommon->GetSWLimit(m_nAxisNum, dTmpPosLimit, dTmpNegLimit);
		if(m_pUnitConverter->isSameDirection()==true)
		{
			 dPosLimit= ConvertDistanceUnit(dTmpPosLimit, false);
			 dNegLimit= ConvertDistanceUnit(dTmpNegLimit, false);
		}
		else
		{
			dNegLimit= ConvertDistanceUnit(dTmpPosLimit, false);
			dPosLimit= ConvertDistanceUnit(dTmpNegLimit, false);
		}
		return nReturnValue;
	}
}

/**
@brief   MotionAxis S/W Limit ���� �Լ�
@author  ehoto
@date    2012-11-01
@return  Error Code
*/
int MotionAxis::ResetSWLimit()
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->ResetSWLimit(m_nAxisNum);
}

// ���� ���� - ����: �Ÿ�(mm), �ӵ�(mm/msec), ���ӵ�(mm/msec2), �ð�(msec)
/**
@brief   MotionAxis Amp On/Off ���� �Լ�
@author  ehoto
@date    2012-11-01
@param   bValue : On/Off ��
@return  Error Code
*/
int MotionAxis::SetAMPEnable(bool bValue)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->SetAMPEnable(m_nAxisNum, bValue);
}

// ���ӵ� ���
/**
@brief   MotionAxis ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dAccel : ���ӵ�
@param   dVel : �ӵ�
@return  Error Code
*/
int MotionAxis::MovePositive(double dAccel, double dVel)
{
	double dTmpAccel = 0.0, dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MovePositive(m_nAxisNum, dAccel, dVel);
	}
	else
	{
//		 dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		 dTmpAccel = ConvertAccelerationUnit(dAccel, true);	// User/msec2 --> Cts/msec2
		 dTmpAccel = fabs(dTmpAccel);
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dTmpAccel, dTmpVel);
		}
		else
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dTmpAccel, dTmpVel);
		}
	}

}

/**
@brief   MotionAxis ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dAccel : ���ӵ�
@param   dVel : �ӵ�
@return  Error Code
*/
int MotionAxis::MoveNegative(double dAccel, double dVel)
{
	double dTmpAccel = 0.0, dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveNegative(m_nAxisNum, dAccel, dVel);
	}
	else
	{
		 dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dTmpAccel, dTmpVel);
		}
		else
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dTmpAccel, dTmpVel);
		}
	}

}

/**
@brief   MotionAxis �����ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dAccel : ���ӵ�
@return  Error Code
*/
int MotionAxis::MoveStop(double dAccel)
{
	double dTmpAccel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveStop(m_nAxisNum, dAccel);
	}
	else
	{
		dTmpAccel = fabs(ConvertAccelerationUnit(dAccel, true));	// User/msec2 --> Cts/msec2
		return m_pHWCommon->MoveStop(m_nAxisNum, dTmpAccel);
	}

}

/**
@brief   MotionAxis �Ÿ��� ���ӵ�/���ӵ�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dAccel_Up : ���ӵ�
@param   dAccel_Down : ���ӵ�
@param   dVel : �ӵ�
@param   dDistance : �̵� �Ÿ�
@return  Error Code
*/
int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance)
{
	USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo;
	memset(&AdditionalInfo, 0, sizeof(USER_MOVE_DS_ADDITIONAL_INFO));
	return MoveDistance(dAccel_Up, dAccel_Down, dVel, dDistance, AdditionalInfo);
}


/**
@brief   MotionAxis �Ÿ��� ���ӵ�/���ӵ�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dAccel_Up : ���ӵ�
@param   dAccel_Down : ���ӵ�
@param   dVel : �ӵ�
@param   dDistance : �̵� �Ÿ�
@return  Error Code
*/
int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo)
//int MotionAxis::MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo)
{
	double dTmpAccel_Up = 0.0, dTmpAccel_Down = 0.0, dTmpVel = 0.0, dTmpDistance = 0.0;
	int	nTmpSmallDistance = 0;
	TRIGGER_CTRL_INFO TriggerCtrlInfo;
	memset(&TriggerCtrlInfo, 0, sizeof(TRIGGER_CTRL_INFO));

	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// ��� �Ÿ��� �����

	if(m_pUnitConverter==NULL)
	{
		TriggerCtrlInfo.StartOnSpeedDown		= AdditionalInfo.bStartOnSpeedDown;
		TriggerCtrlInfo.UseCurrSpeed			= AdditionalInfo.UseCurrSpeed;	

		TriggerCtrlInfo.UseSmallAdd				= AdditionalInfo.bUseSmallAdd;	
		TriggerCtrlInfo.TriggerIONumber			= AdditionalInfo.byTriggerIONumber;
		TriggerCtrlInfo.IsTriggerOnRisingEdge	= AdditionalInfo.bIsTriggerOnRisingEdge;
		nTmpSmallDistance 						= (int)(AdditionalInfo.dSmallDistance);
		TriggerCtrlInfo.AdditionalDist			= ((nTmpSmallDistance>524287)?524287:nTmpSmallDistance);	// 524287 = 2^19-1

		return m_pHWCommon->MoveDistance(m_nAxisNum, dAccel_Up, dAccel_Down, dVel, dDistance, TriggerCtrlInfo);
	}
	else
	{
		dTmpAccel_Up	= fabs(ConvertAccelerationUnit(dAccel_Up, true));	// User/msec2 --> Cts/msec2
		dTmpAccel_Down	= fabs(ConvertAccelerationUnit(dAccel_Down, true));	// User/msec2 --> Cts/msec2
		dTmpVel			= fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		dTmpDistance	= ConvertDistanceUnit(dDistance, true);	// User --> Cts

		TriggerCtrlInfo.StartOnSpeedDown		= AdditionalInfo.bStartOnSpeedDown;
		TriggerCtrlInfo.UseCurrSpeed			= AdditionalInfo.UseCurrSpeed;	

		TriggerCtrlInfo.UseSmallAdd				= AdditionalInfo.bUseSmallAdd;	
		TriggerCtrlInfo.TriggerIONumber			= AdditionalInfo.byTriggerIONumber;
		TriggerCtrlInfo.IsTriggerOnRisingEdge	= AdditionalInfo.bIsTriggerOnRisingEdge;
		nTmpSmallDistance 						= (int)(ConvertDistanceUnit(AdditionalInfo.dSmallDistance, true));	// User --> Cts
		TriggerCtrlInfo.AdditionalDist			= ((nTmpSmallDistance>524287)?524287:nTmpSmallDistance);	// 524287 = 2^19-1
		
		return m_pHWCommon->MoveDistance(m_nAxisNum, dTmpAccel_Up, dTmpAccel_Down, dTmpVel, dTmpDistance, TriggerCtrlInfo);
	}

}

// ���� �ð� ���
/**
@brief   MotionAxis ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int MotionAxis::MovePositive(DWORD dwTime, double dVel)
{
	double dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dVel);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dTmpVel);	// msec -->msec
		}
		else
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dTmpVel);	// msec -->msec
		}
	}

}

/**
@brief   MotionAxis ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int MotionAxis::MoveNegative(DWORD dwTime, double dVel)
{
	double dTmpVel = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dVel);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		if(m_pUnitConverter->isSameDirection()==true)
		{
			return m_pHWCommon->MoveNegative(m_nAxisNum, dwTime, dTmpVel);	// msec -->sec
		}
		else
		{
			return m_pHWCommon->MovePositive(m_nAxisNum, dwTime, dTmpVel);	// msec -->sec
		}
	}

}

/**
@brief   MotionAxis ���� �ð� �����Ͽ� �����ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dwDrivingTime : ���� �ð�
@param   dVel : �ӵ�
@return  Error Code
*/
int MotionAxis::MoveStop(DWORD dwTime)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	return m_pHWCommon->MoveStop(m_nAxisNum, dwTime);
}

/**
@brief   MotionAxis �Ÿ��� ���ӽð�/���ӽð�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
@author  ehoto
@date    2012-11-01
@param   dwTime_Up : ���� �ð�
@param   dwTime_Down : ���� �ð�
@param   dVel : �ӵ�
@param   dDistance : �̵� �Ÿ�
@return  Error Code
*/
int MotionAxis::MoveDistance(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{
	double	dTmpVel = 0.0, dTmpDistance = 0.0;
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// ��� �Ÿ��� �����

	if(m_pUnitConverter==NULL)
	{
		return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dVel, dDistance);
	}
	else
	{
		 dTmpVel = fabs(ConvertSpeedUnit(dVel, true));	// User/msec --> Cts/msec
		 dTmpDistance = ConvertDistanceUnit(dDistance, true);	// User --> Cts
		 return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dTmpVel, dTmpDistance);	// User --> Cts, msec -->sec
	}

}

// ������� ����ÿ��� ���� �� ��� �Լ�
/**
@brief   MotionAxis �� �������ݷ� ��� ����
@author  ehoto
@date    2012-11-01
@param   pSendData : ������ Data �� ����ִ� Char �迭
@param   nSendLen : ������ Data ����
@param   pRecvData : �޴� Data �� ����ִ� Char �迭
@param   nRecvLen : �޴� Data ����
@return  Error Code
*/
int MotionAxis::SendData(char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen)
{
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;
	return m_pHWCommon->SendData(m_nAxisNum, pSendData, nSendLen, pRecvData, nRecvLen);
}



// mm������ �̵��ϴ� ���� �ƴ϶� pulse�� �̵��ϵ��� �ϴ� �Լ� (Hoist ƼĪ�� ����)
int MotionAxis::MoveDistance_Pulse(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance)
{	
	if(m_pHWCommon==NULL) return ERR_NO_HWCOMMON;

	m_dDistOfMoveDistance = dDistance;	// ��� �Ÿ��� �����
	return m_pHWCommon->MoveDistance(m_nAxisNum, dwTime_Up, dwTime_Down, dVel, dDistance);
}

SPEED_MODE MotionAxis::GetSpeedMode()
{
	SPEED_MODE returnMode = SPEED_MODE_NORMAL;
	static double dStartTime = clock();
	static double dPreSpeed = GetCurrentVelocity() * 1000;
	double dDelta = (fabs(GetCurrentVelocity() * 1000) - dPreSpeed)/100;
	double dCheckTime = clock() - dStartTime;
	if(dCheckTime > 50)
	{
		if(fabs(dDelta) < 0.5)
		{
			if(fabs(GetCurrentVelocity()) < 0.05)
				returnMode = SPEED_MODE_NORMAL;
			else
				returnMode = SPEED_MODE_CRUISE;
		}else if(dDelta >= 0.5)
		{
			returnMode = SPEED_MODE_ACCEL;
		}else
		{
		   returnMode = SPEED_MODE_DECEL;
		}

		dPreSpeed  =fabs(GetCurrentVelocity() * 1000);

		dStartTime = clock();
	}

	return returnMode;
}
