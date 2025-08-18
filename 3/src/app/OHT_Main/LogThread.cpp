/**
@file    LogThread.cpp
@version OHT 7.0
@brief   LogThread CPP File
*/

//---------------------------------------------------------------------------
#include "LogThread.h"
#include "LogHandler.h"
#include "OHTMain.h"

#define ADD_MEMO_T(STR)   OHTMainForm->memoMain->Lines->Add(_T(STR))
#define ADD_MEMO_S(STR)   OHTMainForm->memoMain->Lines->Add(STR)

// �α� �ۼ� �Լ�
//---------------------------------------------------------------------------

/**
@brief   LogThread ������
@author  ���¿�, �ŵ���
@date    2013.01.28 / 2013.07.11
@param   pTeachingDataManager : TeachingData �����ͱ���ü
@param   pStationDataManager : StationData ������ ����ü
@param   pFunctionFlag : �����Ϸ��� �Լ����� �÷��� ������ ����ü
*/
LogThread::LogThread(): ThreadCtrl()
{
	InitializeCriticalSection(&m_CS);//2013.07.11 doori.shin CriticalSection �߰�

}
//---------------------------------------------------------------------------

/**
@brief   LogThread Class �Ҹ���
@author  ���¿�
@date    2013.01.28
*/
LogThread::~LogThread()
{
    DeleteCriticalSection(&m_CS);
}
//---------------------------------------------------------------------------

/**
@brief   RealThreadProc Thread �Լ�(IO ������Ʈ, �� ���� ������Ʈ, HID Data ������Ʈ)
@author  ���¿�
@date    2013.01.28
*/
void LogThread::RealThreadProc()
{
	//---- Place thread code here ----
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker)
			break;  // Thread ���Ḧ ���� �ʼ��� �߰�
		
		//2014.04.15 doori.shin Thread Check�� ���� Tick�� ����
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

		// Manager Log
		manageLog();
		Sleep(1000);
		//AOHC-204 Thread �� ���� �ð� ���� ���
		static TimeChk timeChk;

		timeChk.End(0);
		static ThreadTimeMonitor tmon;
		tmon.Execute("LogT", timeChk.Get(0));

		timeChk.Start(0);
	}

}

