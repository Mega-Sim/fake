/**
@file    HWControlThread.cpp
@version OHT 7.0
@brief   HWControlThread CPP File
*/

//---------------------------------------------------------------------------
#include <windows.h>

#include "OHTMain.h"
#include "HWControlThread.h"


//---------------------------------------------------------------------------

/**
@brief   HWControlThread ������
@author  LSJ
@date    2012.
@param   pHWCommon : HWCommon Ŭ���� ������
@param   pHWSet : HWSet Ŭ���� ������
*/
HWControlThread::HWControlThread(): ThreadCtrl()
{
	m_pHWCommon = OHTMainForm->GetHWCommon();
	m_pHWSet = OHTMainForm->GetHWSet();
}

/**
@brief   HWControlThread Class �Ҹ���
@author  LSJ
@date    2012.11.04
*/
HWControlThread::~HWControlThread()
{

}

/**
@brief   RealThreadProc Thread �Լ�(IO ������Ʈ, �� ���� ������Ʈ, HID Data ������Ʈ)
@author  LSJ
@date    2012.11.09
*/
void HWControlThread::RealThreadProc()
{
	//---- Place thread code here ----

	SYSTEM_INFO systemInfo;
	bool bCheckCore = false;
	DWORD nProA,nSysA = 0;

 /*
	//Core Affinity Set 1- Core1, 2-Core2, 3-Core1+core2// 2018 01 25
	GetSystemInfo(&systemInfo);
	if(systemInfo.dwNumberOfProcessors > 1)
	{
//		HANDLE hProc = GetCurrentProcess();
//		bCheckCore = SetProcessAffinityMask(hProc,2);
//		GetProcessAffinityMask(hProc,&nProA,&nSysA);
		SetThreadAffinityMask((HANDLE)GetTreadHandle(), 2);
//		ADD_LOG("SetThreadAffinityMask : 2");
	}
    */
	///////////////////////////////////////////////////////

	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	
            break;  // Thread ���Ḧ ���� �ʼ��� �߰�
			
		//2014.04.15 doori.shin Thread Check�� ���� Tick�� ����
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;
    
		if(m_pHWCommon!=NULL)
		{
			m_pHWCommon->UpdateData();
		}

		Sleep(5);
	}
}
//---------------------------------------------------------------------------
