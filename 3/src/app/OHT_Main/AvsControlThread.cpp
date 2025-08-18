//---------------------------------------------------------------------------


#pragma hdrstop

#include "AvsControlThread.h"
#include "OHTMain.h"


//---------------------------------------------------------------------------
AvsControlThread::AvsControlThread():ThreadCtrl()
{
	m_pAvsControl = OHTMainForm->GetAvsControl();
	bUseAVS = OHTMainForm->GetDefaultParam()->bUseAVS;
}
//---------------------------------------------------------------------------
AvsControlThread::~AvsControlThread()
{

}

//---------------------------------------------------------------------------
void AvsControlThread::RealThreadProc(void)
{
	while(!m_bThreadBreaker)
	{
		try
		{
			if(OHTMainForm->GetAvsControl() == NULL)
				m_bThreadBreaker = true;

			if(m_bThreadBreaker==true)  break;

			if(m_pAvsControl->m_bIsStarted && bUseAVS)
			{
				if(m_pAvsControl->ReceiveVibData() != NO_ERR)
				{
					m_bThreadBreaker = true;
				}

				//TP�� ������� Ȯ�� alive packet�� 5���̻� �̼��� �Ǹ� ����
				m_pAvsControl->CheckTPConnected();

			}

		}
		catch(...)
		{
        	m_bThreadBreaker = true;
		}

		Sleep(25);
	}
}

//---------------------------------------------------------------------------
#pragma package(smart_init)


