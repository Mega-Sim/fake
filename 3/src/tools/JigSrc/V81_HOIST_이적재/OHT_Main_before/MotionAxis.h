/**
@file    MotionAxis.h
@version OHT 7.0
@brief   MotionAxis Class Header File
*/

#ifndef MotionAxisH
#define MotionAxisH

#include "HWCommon.h"			// HW ���� ���
#include "VirtualUnitConverter.h"	// ���� ǥ��ȭ �߻�ȭ Ŭ����

typedef enum SPEED_TYPE_
{
	SPEED_OF_COMMAND	= 0,		///< ��� �ӵ�
	SPEED_OF_CURRENT	= 1,		///< ���� �ӵ�
	SPEED_OF_MIN		= 2,		///< Min (��� �ӵ�, ���� �ӵ�)
	SPEED_OF_MAX		= 3		///< Max (��� �ӵ�, ���� �ӵ�)
}SPEED_TYPE;

typedef	struct USER_MOVE_DS_ADDITIONAL_INFO_
{
	bool		bStartOnSpeedDown;
	SPEED_TYPE	UseCurrSpeed;

	bool		bUseSmallAdd;
	BYTE		byTriggerIONumber;
	bool		bIsTriggerOnRisingEdge;
	double 		dSmallDistance;
}USER_MOVE_DS_ADDITIONAL_INFO;

/**
@class   MotionAxis
@date    2012.10.17
@author  ehoto
@brief   Axis ���� �������̽� Ŭ����
*/
class MotionAxis
{
private:
	int m_nAxisNum; ///< �� ��ȣ
	HWCommon *m_pHWCommon;
	bool m_bAbsoluteEncoder;

	double m_dDistOfMoveDistance;	///< MoveDS ��� �� ���� �Ÿ� 

protected:
	VirtualUnitConverter *m_pUnitConverter;

	virtual double ConvertDistanceUnit(double dInput, bool bUserUnit);		///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )
	virtual double ConvertSpeedUnit(double dInput, bool bUserUnit);				///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )
	virtual double ConvertAccelerationUnit(double dInput, bool bUserUnit);	///< InputType Flag = bUserInput ( true:UserUnit / false:Cts )

public:
	MotionAxis();
	MotionAxis(HWCommon *pHWCommon, int nAxisNum, bool bAbsoluteEncoder);
	virtual ~MotionAxis();

	/// �ʱ�ȭ
	virtual int Init();
	virtual bool EnableUnitConverter(void) = 0;
	virtual void DisableUnitConverter(void);

	/// ���� Ȯ��
	virtual bool IsRunning();
	virtual bool IsInPosition();
	virtual bool IsAxisDone();
	virtual bool IsAMPEnable();
	virtual double GetCurrentPosition();
	virtual double GetPrePosition();
	virtual double GetRunningPosition(bool bInit, bool bConvert);
	virtual int GetTotalMoveCount(bool bInit);
	virtual double GetFinalPosition();	
	virtual double GetCurrentVelocity();
	virtual double GetFollowingError();
	virtual double GetSmallAddCheckPosition();	
	virtual double GetDistOfRemain();

	/// ���� ����
	virtual int GetError();
	virtual int Reset();

	/// Offset ����
	virtual int SetHome();

	/// Limit ����
	virtual int SetPositiveSWLimit(double dLimit);
	virtual int SetNegativeSWLimit(double dLimit);
	virtual int GetSWLimit(double& dPosLimit, double& dNegLimit);
	virtual int ResetSWLimit();

	/// ���� ���� - ����: �Ÿ�(mm), �ӵ�(mm/msec), ���ӵ�(mm/msec2), �ð�(msec)
	virtual int SetAMPEnable(bool bValue);
	// ���ӵ� ���
	virtual int MovePositive(double dAccel, double dVel);
	virtual int MoveNegative(double dAccel, double dVel);
	virtual int MoveStop(double dAccel);
	virtual int MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance);
	virtual int MoveDistance(double dAccel_Up, double dAccel_Down, double dVel, double dDistance, USER_MOVE_DS_ADDITIONAL_INFO AdditionalInfo);
	// ���� �ð� ���
	virtual int MovePositive(DWORD dwTime, double dVel);
	virtual int MoveNegative(DWORD dwTime, double dVel);
	virtual int MoveStop(DWORD dwDrivingTime);
	virtual int MoveDistance(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);

	// ������� ����ÿ��� ���� �� ��� �Լ�
	virtual int SendData(char* pSendData, int nSendLen, char* pRecvData, int* nRecvLen);	///< Data ����

	// mm������ �̵��ϴ� ���� �ƴ϶� pulse�� �̵��ϵ��� �ϴ� �Լ� (Hoist ƼĪ�� ����)
	virtual int MoveDistance_Pulse(DWORD dwTime_Up, DWORD dwTime_Down, double dVel, double dDistance);
};
 
#endif  // MotionAxisH


