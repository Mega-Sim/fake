/**
@file    Driving.h
@version OHT 7.0
@brief   Driving Class Header File
*/

#ifndef DrivingH
#define DrivingH

#include "HWCommon.h"			// HW ���� ���
#include "PropotionalMotionAxis.h"	// ��� ���� �� ��� 
#include "Def_Driving.h"			// ���� ���� ����ü


/**
@class   Driving
@date    2012.10.17
@author  ehoto
@brief   Driving Motor ���� Class
@note    MotionAxis Class ���
*/
class Driving : public PropotionalMotionAxis	//MotionAxis
{
//1	����(Member)
private:
	HWCommon *m_pHWCommon;	///< HW�� ������ ȹ��
	
	int m_nMasterAxisNum, m_nSlaveAxisNum;	///< �� ���� ��ȣ
	bool m_bSync;							///< ���� ���� ����
	/*
	������ ��ġ�� Ȯ���Ͽ� AMC�� ������ ��/�����ϴ� �κ��̸� ���� �������ϰ�츸 ������.
	false�� ����,   true�� ����.
    */
	bool 	m_bTargetPos2AMC;
	bool m_Curve_E_Stop;

public:
	PropotionalMotionAxis *m_pSlaveAxis;	///< ���� 2��° ��

//1	������/�Ҹ��� (Constructor/Destructor)
public:	
	Driving(HWCommon *pHWCommon, int nAxisNum);						///< ������: �������� , ���� ���� 
	Driving(HWCommon *pHWCommon, int nAxisNum, double dGearRatio);		///< ������: �������� , ���� ���� 
	
	Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum);						///< ������: �������� , ���� ���� 
	Driving(HWCommon *pHWCommon, int nMasterAxisNum, int nSlaveAxisNum, double dGearRatio);	///< ������: �������� , ���� ���� 
	
	~Driving();



//1	�Լ�(Method)
public:
	bool	SetGearRatio(PROPOTINAL_GEAR_RATIO GearRatio);	///< Master, Slave ���� ó���� ���� Overriding
	bool	EnableUnitConverter(void);			///< Master, Slave ���� ó���� ���� Overriding


	bool	IsBrakeReleased();				///< IO���� : Brake ���� Ȯ��
	int		SetBrakeRelease(bool bValue);		///< IO���� : Brake ���� ����


	void	ClearNodeCountInfo(void);									///< Motion ����� ���� ������� Node/Station ������ ó���ϱ� ���� �Լ�: ���� �ʱ�ȭ
	int		GetNodeCountInfo(USER_NODE_CHECK_DATA &UserNodeCheckData);	///< Motion ����� ���� ������� Node/Station ������ ó���ϱ� ���� �Լ�: ���� ȹ��
	int		SetDetectInfo(USER_DETECT_INFO UserDetectInfo);		///< ������� ���� ���� ���� �Լ�:  ���� ���� ���� ����

	void	SetTorqueLimit();
	void	SetTorqueLimitRelease();

	void	SetSlopeTime();
	void	SetSlopeTimeRelease();

	void	SetTargetPos();
	void	SetTargetPosRelease();
	bool	IsTargetPos();

	void	SetFirstNodeCountAdd();
	void	SetVHLDetectIgnor();
	void	SetVHLDetectIgnorRelease();

	int		GetFollowing_Axis1();          ///<AOHC-196 ���� �α� �߰�

	void	SetDetectStopDecelDown();
	void	SetDetectStopDecelDownRelease();

	void	SetVHLDetectIOSet(bool bIoSet);

	bool	IsSyncControl(void);			///< ���� ��� ���� �Լ�: ���� �������� Ȯ�� ����
	int		CompensateSlavePos();		///< ���� ��� ���� �Լ�: �ķ� ���ڴ��� ������ ���� - ���� �̻��

	
	void SetCurveEstop(bool value);
	bool GetCurveEstop(void);

#if 1	//1	������ ���� ���� �Լ���
	bool	IsSlaveAMPEnable();
	int		SetSlaveAMPEnable(bool bValue);
	int		SetSlaveHome(void);
	bool	IsSlaveRunning();
	double	GetSlaveCurrentPosition();
	double	GetSlaveCurrentVelocity();
	int		GetSlaveError();
	int		ResetSlave();
#endif

};

#endif  //DrivingH

