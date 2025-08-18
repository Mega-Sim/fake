//---------------------------------------------------------------------------


#pragma hdrstop

#include "OHTMain.h"
#include "MonitorControlThread.h"

#define LOG_MONITORING(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Monitoring.log", __VA_ARGS__)

//---------------------------------------------------------------------------
/**
@class   MonitorControl ������
@date    2013.06.03
@author  puting
*/
MonitorControlThread::MonitorControlThread(OHTMainStatus *pOHTMainStatus, Virtual_HID *pHID, NetworkSinalLib* pnetWorkDll, IPCLibrary *pIPCLibrary, CPUandMemInfo *pCPUandMemInfo, MONITORING *pMonitoring) : ThreadCtrl()
{
	m_pOHTMainStatus = pOHTMainStatus;
	m_pHID = pHID;         ///�ǹ��� 1
	m_pnetWorkDll = pnetWorkDll;
	m_pIPCLibrary =  pIPCLibrary;
	m_pCPUandMemInfo = pCPUandMemInfo;
	//m_pSigmaMonitor = pSigmaMonitor;
	m_pMonitoring = pMonitoring;

	IsInitDevice();

}
//--------------------------------------------------------------------------
/**
@class   MonitorControl �Ҹ���
@date    2013.06.03
@author  puting
*/
MonitorControlThread::~MonitorControlThread()
{
	m_pOHTMainStatus = NULL;
	m_pHID = NULL;
	m_pnetWorkDll = NULL;
	m_pIPCLibrary = NULL;
	m_pCPUandMemInfo = NULL;
	m_pMonitoring = NULL;

}
//--------------------------------------------------------------------------
/**
@class   MonitorControl Thread �Լ�
@date    2013.06.03
@author  puting
@brief   HID/Network/IPC ���� ���� �� Socket/Monitoring(Motor ����) Updata
*/
void MonitorControlThread::RealThreadProc(void)
{

//	DWORD	dwHIDTrigerTime	= timeGetTime();     //�Լ� ���� �ð�.
//	DWORD	dwServoTrigerTime	= timeGetTime();     //�Լ� ���� �ð�.
//	DWORD   dwOHTTrigerTime    = timeGetTime();
//	int nError = NO_ERR;

	AnsiString strLog;
	//static AnsiString strPreLog = "";

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	break;	// Thread ���Ḧ ���� �ʼ��� �߰�



		//1. HID ���� Updata  �� HID ���� ���
		if(m_pHID != NULL)
		{
			if(m_pHID->UpdateData() == NO_ERR)
			{
				m_pOHTMainStatus->m_cpMonitorData.HIDData = m_pHID->GetHIDData();
			}
		}

		//2. NetWork Data ���
		if(m_pnetWorkDll!=NULL)
		{
			m_pnetWorkDll->Get_WLanInfo(&m_pOHTMainStatus->m_cpMonitorData.APSignalData);

		}
		//3. IPC ���� ���
		if(m_pIPCLibrary !=NULL)
		{
			m_pIPCLibrary->GetIPCData(&m_pOHTMainStatus->m_cpMonitorData.IPCData);
			Get_IPCSysInfo(&m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.IPCSysInfo);

		}
		//4. �޸� �� CPU ������ ���� ���


		//5. ��Ÿ ����͸� ���� ���
		if(m_pMonitoring !=NULL)
		{
				m_pMonitoring->MonitorUpdate(&m_pOHTMainStatus->m_cpMonitorData); //���� ��� ����.

				//�ʱ⿡ �ѹ��� ����Ǵ� �κ�
				if(m_pMonitoring->IsWinSetupLoaded() == false)
				{
					m_pMonitoring->MonitorInitData(&m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus);
                }
		}

		Sleep(500);
	}

}
//----------------------------------------------------------------------------
/**
@class   IsInitDevice �Լ�
@date    2013.06.03
@author  puting
@brief   Device �ʱ�ȭ Check
*/
void MonitorControlThread::IsInitDevice()
{

	if(m_pHID->IsHIDOpen() != true )
	{
		m_pHID = NULL;
	}
	if(m_pnetWorkDll->NETWORKInit()!= 0)
	{
		m_pnetWorkDll = NULL;
	}
	if(m_pIPCLibrary->IsIPCReady() != true)
	{
		m_pIPCLibrary = NULL;
	}
// if(m_MonitorControl->gm_pSigmaMonitor->IsCommOpen() != true)
// {
//	m_MonitorControl->gm_pSigmaMonitor = NULL;
// }

}



#pragma package(smart_init)
