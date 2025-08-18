/**
@file	 VirtualModuleMonitor.h
@version OHT 7.0
@brief	 ExecuteThread�� ������ Module�� �߻� �Լ�
*/

#ifndef VirtualModuleMonitorH
#define VirtualModuleMonitorH

#include <stdio.h>
#include "Def_Variables.h"	//UINT
//---------------------------------------------------------------------------
typedef struct STATUS_SET_
{
	BYTE	byStatus;		///< ���� ����
	UINT	uError;			///< �����ڵ�
	bool	bIsOnProcess; 	///< ������ ���� ���� ����
	UINT	uStep;			///< ������ ��� ó���� ���� Step
}STATUS_SET;
//---------------------------------------------------------------------------
class VirtualModuleMonitor
{
	public:
		VirtualModuleMonitor(void)	///< ������, ���� �ʱ�ȭ
		{
			memset(&m_StatusSet, 0, sizeof(STATUS_SET));
		}
		virtual ~VirtualModuleMonitor(void){;} ///< ���� �Ҹ���

	protected:
   		STATUS_SET	m_StatusSet;		///< ���� ���� ����

	public:
		virtual UINT		GetError(void)			{return m_StatusSet.uError;}				///< ���� ���¸� �޾� ���� �Լ�
		virtual BYTE		GetStatus(void)			{return m_StatusSet.byStatus;}			///< ���¸� �޾ƿ��� �Լ�
		virtual bool		IsOnProcess(void)		{return m_StatusSet.bIsOnProcess;}		///< ���ۿ��θ� Ȯ���ϴ� �Լ� (������:true / �����:false)
		virtual UINT		GetStep(void)			{return m_StatusSet.uStep;}
		virtual void*		LoadParam(char* NewFileNameAndDirectory, char* BackUpFileNameAndDirectory) = 0;	///< �Ķ���͸� ����ϰ�, ���ο� ������ Pointer�� �޾ƿ��� �Լ�
		virtual bool		RequestParamUpdate(void* pParameter) = 0;									///< �޾ƿ� Parameter ������ ������ ��û�ϴ� �Լ�
		virtual bool		IsParamUpdated() = 0;															///< �Ķ���� ���� ���θ� Ȯ���ϴ� �Լ�
};
//---------------------------------------------------------------------------



#endif


