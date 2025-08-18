//---------------------------------------------------------------------------
/**
@file    ExecuteThread.h
@version OHT 7.0
@brief   ExecuteThread Class Header File
*///---------------------------------------------------------------------------

#ifndef ExecuteThreadH
#define ExecuteThreadH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ThreadCtrl.h"
#include "ExecuteInfo.h"
#include "VirtualModuleControl.h"
#include "DrivingControl.h"
#include "TransControl.h"
#include "ManualControl.h"
#include "HWCommon.h"
#include "LogHandler.h"	// Log �ۼ� ����
//---------------------------------------------------------------------------

/**
@class   ExecuteThread
@date    2012.10.22
@author  ehoto
@brief   ExecuteThread ���� ���� �����ϴ� Class
@note    ExecuteThread ���� ���� ������ ���� �Լ� ����
*/
class ExecuteThread:public ThreadCtrl
{
private:
	LogUnit *pLogExecuteThread;   ///< ExecuteThread �α� ������ ���� ����

protected:
	void RealThreadProc(void);	///< Execute �� ���� �Լ�
public:
	ExecuteThread();        ///< ������ : Tool Class �� Data ���� ����ü
	~ExecuteThread(void);   ///< �Ҹ��� :

private:
	EXECUTE_INFO *m_pExecuteInfo;            ///< MainThread�� ������ �����ϴ� ����ü
	VirtualModuleControl *m_pModuleControl[3];    ///< Driving:1 / Trans:2 / Manual:3

    BYTE m_Status[3];
	UINT m_ModuleError[3];

	HWCommon	*m_pHWCommon;
    
private:
	UINT m_nError;
	bool m_bReady;
    
    private:
	UINT updateStatus(void);

	UINT handleQuickCommand(EXECUTE_QUICK_CMD CmdType);
	UINT stopCmd(void);

    UINT handleModuleControl(int nIndex, EXECUTE_INFO *pExecuteInfo, UINT nMaxCountGetCmd);
    UINT checkModules(int nIndex, EXECUTE_INFO * pExecuteInfo);
    UINT deleteCommand(int nIndex, EXECUTE_INFO *pExecuteInfo);

    UINT transferDataToMainThread(EXECUTE_INFO *pExecuteInfo);

    bool isOnAutoRecovery(int nIndex, int nError);

};
//---------------------------------------------------------------------------
#endif
