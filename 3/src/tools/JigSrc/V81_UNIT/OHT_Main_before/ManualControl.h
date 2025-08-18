/**
@file    ManualControl.h
@version OHT 7.0
@brief   ManualControl Class Header File
*/
//---------------------------------------------------------------------------

#ifndef ManualControlH
#define ManualControlH

#include <math.h>
#include <stdio.h>
#include "VirtualModuleControl.h"
#include "VirtualDrivingControl.h"
#include "VirtualTransControl.h"
#include "HWSet.h"
//#include "HWCommon.h"
#include "ExecuteInfo.h"
#include "Def_TPCommand.h"
#include "LogHandler.h"

//2014.02.10 ji7412.lee : AMC.out Update ���
#define	WM_DSP_MSGID	    (WM_USER+100)
//---------------------------------------------------------------------------

/**
@class   ManualControl
@date    2012.11.14
@author  LSJ
@brief   ManualControl ���� Class
*/
class ManualControl:public VirtualModuleControl
{
private:
    HWSet 					*m_pHWSet;
    HWCommon 				*m_pHWCommon;
    VirtualDrivingControl	*m_pVirtualDrivingControl;
    VirtualTransControl	    *m_pTransControl;

	DEFAULT_PARAM_SET *m_defualtparam;
    LogUnit *pLogUnit;      ///< �α� ������ ���� ����
    
	FDCUnit *pFDCUnit;
    
   	EXECUTE_INFO *m_pExecuteInfo;    
	OHTMainStatus *m_pOHTMainStatus;

    EXECUTE_MANUALCONTROL_INFO m_ManualCmd; ///< �����ؾ� �ϴ� ManualCmd
    
	int		m_nIOControlOutIndex;		///< I/O ��½� I/O ��ȣ : iData1

	double	m_dJogDistance_mm;			///< ���� �̵� �� �Ÿ��� : iData1
	double	m_dAcceleration_mmpermsec2;	///< ���� ���� �� ���ӵ� : iData2
	double	m_dDeceleration_mmpermsec2;	///< ���� ���� �� ���ӵ� : iData3
	double	m_dVel_mmpermsec;			///< ���� ���� �� �ӵ�   : iData4

	bool	m_bNew;						// ����� ó�� �ϴ� �Է� �� �� true�� ���� 

//	double	m_dJogStartPos;				// ��� �޴� ��ġ�� �ƴ� : ���������� ������ ����

    // by zzang9un 2013.08.17 : Use ������ ���� ����
    int m_UseIndex;     ///< Use ������ ���� Index
    bool m_bUseOnOff;    ///< Use ������ ���� OnOff ����

    void	Init();

protected:
	virtual EXECUTE_ACK	Stop(void);
	virtual EXECUTE_ACK	Reset(void);
	virtual	EXECUTE_ACK	TurnOff(void);	///< �⵿ Off
	virtual	EXECUTE_ACK	TurnON(void);	///< �⵿ ON

private:
	int 	ControlDriving();
//	int		controlTrans();		//@todo ���߿� �� �Ұ�	: 
//	int		controlFrame();		//@todo ���߿� �� �� ��	: E_Stop, MC, Bumper, Lamp, etc
//	int		controlHWCommon();	//@todo ���߿� �� �� ��	: I/O, Axis, Serial, etc

public:
    ManualControl();
    ~ManualControl();
    
private:
    int		setOutput();
    int		SetOriginAndLimit();
    int		JogCommand();
	int		ControlHW();
	int		ControlRFPIOSet();

public:
	EXECUTE_ACK	InsertGeneralCmd(void *sManualControlInfo);		///<���� �ʿ�: EXECUTE_MANUALCONTROL_INFO
	UINT 		ExecuteJob(void);
	UINT		CheckAbnormalOnWait(void);						///< ��� �� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
	UINT		CheckAlways(void);		///< �׽� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
	bool		DeleteCmd(void *pCmd);							///< �Ҵ� �� ����� �Ҹ���

	void*		LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory){return NULL;}
	bool		RequestParamUpdate(void* pParameter){return false;}
	bool		IsParamUpdated(){return false;}

	//2014.02.10 ji7412.lee : AMC out���� ������Ʈ �Լ�, OHT->Assistant �ϷẸ�� �Լ�
	int         UpdateAMCDownloadFile();
	void        InformAssistantCommand();

	int 		SetOnOff(int m_UseIndex, int m_bUseOnOff);
	int			SetOutBit(int m_UseIndex, int m_bUseTrueFalse);

	void 		SendCidMonitor(void);
	
	void SoundOn(int iSoundNum);
	void SoundOff();

	void PowerOnOff(int iAxis, int iOnOff);

	void MarkTry(void);
	void MoveByPulse(double dblPulse, double dblMoveSpeed);
};
//---------------------------------------------------------------------------
#endif
