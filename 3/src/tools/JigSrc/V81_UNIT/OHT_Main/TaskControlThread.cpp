/**
@file    TaskControlThread.cpp 
@version OHT 7.0
@brief   TaskControl ���� Thread
*/

#pragma hdrstop

#include "TaskControlThread.h"
#include "OHTMain.h"
#include "LogHandler.h"	// Log �ۼ� ����

// ---------------------------------------------------------------------------

TaskControlThread::TaskControlThread() : ThreadCtrl()
{
    m_pControlTask = NULL;
	m_pControlTask = OHTMainForm->GetTaskControl();
}
//--------------------------------------------------------------------------

TaskControlThread::~TaskControlThread()
{
	m_pControlTask =NULL;
}
//--------------------------------------------------------------------------

void TaskControlThread::RealThreadProc(void)
{
	static DWORD dwOCSTrigerTime = timeGetTime();     //�Լ� ���� �ð�.

    // 20161216 sh17.jo : Commfail üũ ��� �߰�
    static DWORD dwCommfailTrigerTime = timeGetTime();

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)  break;  // Thread ���Ḧ ���� �ʼ��� �߰�

		//2014.04.15 doori.shin Thread Check�� ���� Tick�� ����
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

		if(m_pControlTask !=NULL)
		{
			// 1. OCS Data Update

			m_pControlTask->StandardOCSReadDataUpdate();

            // 2. Handy TP Update
            // 2016-12-19, Add by Labeler
            // ���� Handy TP�� ���, TaskControl Class�� ExcuteStandardTaskUpdate ���� ó���ϳ�, �ش� ��ġ���� ó����� ���� �ʿ�
            m_pControlTask->GetReadRemoconData();

			// 3. TP Update
			m_pControlTask->StandardPCReadDataUpdate();

			// 4.1 AST Update
			m_pControlTask->StandardASTReadDataUpdate();

			// 5. Monitoring Update
			m_pControlTask->StandardMonitoringReadDataUpdate();

			//6.  Map Maker File Update
			m_pControlTask->StandardMapMakerFileUpdate();

			// 6. Task ������Ʈ
			m_pControlTask->ExcuteStandardTaskUpdate();

			// 6.1 ������ ���� ����
			m_pControlTask->ExecuteSteps();

//			m_pControlTask->ExecuteResetSteps();

			// 7. OCS�� Data ������ ���� �κ� (100msec)
			m_pControlTask->SuperviseSendCmdDataOHT2OCS();

			// 8. OCS������ ���� üũ
			if(OHTUTILITY::PreventOverflowGetTimeMs(dwOCSTrigerTime) >Time_10sec)
			{
				m_pControlTask->StandardAddress_Broadcast();
				dwOCSTrigerTime = timeGetTime();
            }
			
            // 20161216 sh17.jo : Commfail üũ ��� �߰�
			//9. ���� ���� ���� üũ
			if(m_pControlTask->m_bSetCheckCommfail == true)
			{
				if(OHTUTILITY::PreventOverflowGetTimeMs(dwCommfailTrigerTime) > Time_1sec)
				{
					m_pControlTask->CheckCommfail();
					dwCommfailTrigerTime = timeGetTime();
				}
			}
			else
			{
				m_pControlTask->m_bflagARPClear = false; //AOHC-234 C3850 Switch Fail-Over �� ��� ���� ����
			}

			//10. IoT ��� update �κ�
			m_pControlTask->IOTHubSocketClientUpdate();
			//AOHC-204 Thread �� ���� �ð� ���� ���
			static TimeChk timeChk;
			static ThreadTimeMonitor tmon;
			timeChk.End(0);
			tmon.Execute("TaskCtrl", timeChk.Get(0));
			timeChk.Start(0);
		}

		Sleep(10);
	}

}
//----------------------------------------------------------------------------




#pragma package(smart_init)



