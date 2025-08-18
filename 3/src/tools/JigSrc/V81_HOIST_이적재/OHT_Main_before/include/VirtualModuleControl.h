/**
@file	 VirtualModuleControl.h
@version OHT 7.0
@brief	 ExecuteThread�� ������ Module�� �߻� �Լ�
*/

#ifndef VirtualModuleControlH
#define VirtualModuleControlH

#include <stdio.h>
#include "VirtualModuleMonitor.h"
#include "ExecuteInfo.h"

//---------------------------------------------------------------------------
typedef unsigned int		UINT;	///< 4byte �ڷ���(unsigned int)
//---------------------------------------------------------------------------
class VirtualModuleControl: public VirtualModuleMonitor
{
   public:
		VirtualModuleControl(void)	///< ������, ���� �ʱ�ȭ
		{
			m_QuickCmd			= EXECUTE_QUICK_CMD_NONE;
		}
		virtual ~VirtualModuleControl(void){;} ///< ���� �Ҹ���

   protected:
		EXECUTE_QUICK_CMD	m_QuickCmd;		///< Quick Command Status

		virtual EXECUTE_ACK	Stop(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ��� 
		virtual EXECUTE_ACK	Pause(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ��� 
		virtual EXECUTE_ACK	Resume(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ��� 
		virtual EXECUTE_ACK	Cancel(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ��� 
		virtual EXECUTE_ACK	Reset(void)		{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ���
		virtual EXECUTE_ACK	TurnOff(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ���
        virtual EXECUTE_ACK	TurnON(void)	{return EXECUTE_ACK_NOT_AVAILABLE_CMD;}	///< �ʿ信 ���� ���� ���̵� �Ͽ� ���

   public:
		virtual EXECUTE_ACK	InsertGeneralCmd(void *pCmdInfo)			= 0;	///< ����� �����ϴ� �Լ� (���� ����� �ݵ�� delete �ؾ� �Ѵ� / ���� ���� ���� �� ���·� ���� �ʼ�)
		virtual EXECUTE_ACK	SetQuickCmd(EXECUTE_QUICK_CMD QuickCmd);		///< ��� ����� �����ϴ� �Լ�: ����/�Ͻ�����/�����/�ʱ�ȭ/��� Ŭ���� ��
		virtual UINT		ExecuteJob(void)							= 0;	///< �Ҵ�� �۾��� ���� ��Ű�� �Լ� (Step ���·� ���� - Thread�� ����)

		virtual UINT		CheckAbnormalOnWait(void)					= 0;	///< ��� �� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
		virtual UINT		CheckAlways(void)					= 0;	///< �׽� �̻� ���� Ȯ�� �Լ� (Step ���·� ���� - Thread�� ����)
 		virtual bool		DeleteCmd(void *pCmd)						= 0;	///< �Ҵ� �� ����� �Ҹ���
};
//---------------------------------------------------------------------------

EXECUTE_ACK VirtualModuleControl::SetQuickCmd(EXECUTE_QUICK_CMD QuickCmd)	  ///< ��� ����� �����ϴ� �Լ�: ����/�Ͻ�����/�����/�ʱ�ȭ/��� Ŭ���� ��
{
	m_QuickCmd=QuickCmd;
	EXECUTE_ACK Ack = EXECUTE_ACK_NOT_AVAILABLE_CMD;
	switch(QuickCmd)
	{
	case EXECUTE_QUICK_CMD_NONE:		///< �ʱ�ȭ ����		A/M
		break;
	case EXECUTE_QUICK_CMD_STOP:		///< ��� ����		A/M	  ~ OCS_Estop / Status Check ���� Internal Cmd �� E-stop /
		Ack = Stop();
		break;
	case EXECUTE_QUICK_CMD_CANCEL:		///< ���� ��� Clear	A/M	   ~ OCS �� ��� ������ ���
		Ack = Cancel();
		break;
	case EXECUTE_QUICK_CMD_RESET:		///< ���� ��� Clear	A/M	  ~ OCS �� ��� ������ ���
		m_StatusSet.uStep	= 0;
		Ack = Reset();
		break;
	case EXECUTE_QUICK_CMD_TURN_OFF:
        Ack = TurnOff();
		break;
	case EXECUTE_QUICK_CMD_TURN_ON:
        Ack = TurnON();
		break;
	}
	return Ack;
}



#endif


