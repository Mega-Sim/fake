//---------------------------------------------------------------------------

#ifndef MonitorControlThreadH
#define MonitorControlThreadH
//---------------------------------------------------------------------------

#include "ThreadCtrl.h"

//HID//
#include "HID_VII_ZR.h"
//IPC//
#include "IPCLoadLib.h"
#include "CPUandMemInfo.h"
#include "OHTSysInfo.h"
//Network//
#include "NetworkSignallib.h"
#include "MonitoringUnit.h"
////Servo Pack//
//#include "SigmaMonitor.h"   //���� Test �Ϸ��� ���Ŀ� ���� �� ��.

#include "Def_DataType.h"
#include "OHTMainStatus.h"
#include <time.h>


class MonitorControlThread : public ThreadCtrl
{

private:
	OHTMainStatus 		*m_pOHTMainStatus;   	/// OHT Main Status ����(MainThread�� MonitoringThread ����)
	Virtual_HID 		*m_pHID;
	NetworkSinalLib		*m_pnetWorkDll;
	IPCLibrary		 	*m_pIPCLibrary;
	CPUandMemInfo 		*m_pCPUandMemInfo;
	//SigmaMonitor        *gm_pSigmaMonitor;  //���� �߰� 2013.7.9
	MONITORING          *m_pMonitoring;     //�����߰� 2013.7.18

	void RealThreadProc(void);
	void IsInitDevice();


public:
	MonitorControlThread(OHTMainStatus *pOHTMainStatus, Virtual_HID *pHID, NetworkSinalLib* pnetWorkDll, IPCLibrary *pIPCLibrary, CPUandMemInfo *pCPUandMemInfo, MONITORING *pMonitoring);
	~MonitorControlThread();
//	void SetBreakerFlag(bool flag);


};

#endif
