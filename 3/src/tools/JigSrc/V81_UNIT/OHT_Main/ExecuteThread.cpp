//---------------------------------------------------------------------------
#include "ExecuteThread.h"
//---------------------------------------------------------------------------

#include "OHTMain.h"
#include "TestLog.h"
#include "LogHandler.h"	// Log �ۼ� ����
#include "MyTools.h"
#define PAUSED_BY_TP       		1
#define PAUSED_BY_OCS      		2

#define DRIVING_CONTROL_MODULE	0
#define TRANS_CONTROL_MODULE	1
#define MANUAL_CONTROL_MODULE	2

#define MAX_BYTE_SIZE_OF_EXECUTE_THREAD_LOG	1024*10

//#define ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\Execute.log", __VA_ARGS__)
//����
#define ADD_LOG(...)   WRITE_LOG_UNIT(pLogExecuteThread, __VA_ARGS__)

/**
@brief	 Motion ������ �����ϴ� �Լ�
@author  ehoto
@date	 2012.11.09
@param	pExecuteInfo: Main Thread�� Execute Thread ���� ���� ����
@param	pDrivingControl: Driving Module
@param	pTransControl: Trans Module
@param	pManualControl: Manual Module
*/
ExecuteThread::ExecuteThread():ThreadCtrl()
{
	pLogExecuteThread = getLogUnit("D:\\log\\Execute.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	m_bReady									= false;
	m_pExecuteInfo 								= OHTMainForm->GetExecuteInfo();
	m_pModuleControl[DRIVING_CONTROL_MODULE]	= OHTMainForm->GetDrivingControl();
	m_pModuleControl[TRANS_CONTROL_MODULE]		= OHTMainForm->GetTransControl();
	m_pModuleControl[MANUAL_CONTROL_MODULE]		= OHTMainForm->GetManualControl();
	m_pHWCommon									= OHTMainForm->GetHWCommon();
	
	if(		m_pExecuteInfo	!= NULL
		&& 	m_pModuleControl[DRIVING_CONTROL_MODULE]	!= NULL
		&& 	m_pModuleControl[TRANS_CONTROL_MODULE]		!= NULL
		&& 	m_pModuleControl[MANUAL_CONTROL_MODULE]		!= NULL		)
	{
		m_bReady=true;
	}
	m_nError = 0;

	for(int n=0; n<3; n++)
	{
		m_Status[n]			= STS_INIT;
		m_ModuleError[n]	= NO_ERR;
	}
}

ExecuteThread::~ExecuteThread(void)
{

}
//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread �Լ�

	1. �� Unit�� Status Ȯ��
	2. Quick CMD ó�� (��� ��� ��)
	3. ��� Ÿ�Կ� ���� �ش�Ǵ� Unit�� �۾� �ǽ� (D/T/M Ȯ��)
		1) Driving ó��
		2) Trans ó��
		3) Manual ó��
	4. ���� �̻� �� ���� (Quick CMD ���� Stop ó��)
	5. �� Unit�� Status �� Error�� ���� ������ ������Ʈ
@author  ehoto
@date	 2012.11.09
*/
void ExecuteThread::RealThreadProc()
{
	int		nTmpError	= NO_ERR;
	int		nIndex		= m_pExecuteInfo->ExecuteUnit;
	int		nCount		= 0;
	TaskControl* m_pControlTask = OHTMainForm->GetTaskControl();
    HWSet * pHWSet = OHTMainForm->GetHWSet();
    bool bNeedResume = false;

	//---- Place thread code here ----
	while(!m_bThreadBreaker)
	{
		if(m_bThreadBreaker==true)	break;	// Thread ���Ḧ ���� �ʼ��� �߰�
		
		//2014.04.15 doori.shin Thread Check�� ���� Tick�� ����
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;


		if(m_bReady==true)
		{
			nTmpError = NO_ERR;

			//1 1. Status Ȯ��
			nTmpError=updateStatus();

            // 1.2. Pause ���� Check �� ���� ó��
            // 2017-01-09, Add by Labeler
            if((m_pExecuteInfo->bTpPause == false)
            && (m_pExecuteInfo->bOcsPause == false)
            && (pHWSet->m_pCID->GetCIDPause() == false))
            {
                if(bNeedResume == true)
                {
                    m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
                    bNeedResume = false;
                }
            }
            else
            {
                bNeedResume = true;
            }

			//1 3. Quick CMD ó��
			if(m_pExecuteInfo->QuickCmd!=EXECUTE_QUICK_CMD_NONE)
			{

				ADD_LOG("QuickCmd:%d Info(Status:%d, Error:%d, Mode:%d, DTM:%d, Count:%d)"
					, m_pExecuteInfo->QuickCmd
					, m_pExecuteInfo->StatusCmdId.Info.Status
					, m_pExecuteInfo->ErrCode_ExecuteInfo
					, m_pExecuteInfo->ExecuteMode
					, m_pExecuteInfo->ExecuteUnit
					, m_pExecuteInfo->ListOfCmd.getCount());
				CS_CmdList->Acquire();
				// E-Stop�� ���� ���� ���� ��쿡��, �׿ܹ� ������ ����(Reset ��)
				if(m_nError==NO_ERR || m_pExecuteInfo->QuickCmd!=EXECUTE_QUICK_CMD_STOP)
				{
					handleQuickCommand(m_pExecuteInfo->QuickCmd);
					
					if(m_pExecuteInfo->QuickCmd==EXECUTE_QUICK_CMD_STOP)
					{
						if(m_pExecuteInfo->QuickCmdEStopCommander == ESTOP_BY_TP)
						{
							m_nError = ERR_TP_ESTOP;
						}
						else
						{
							if(m_pExecuteInfo->QuickCmdEStopType == EXIT_ROUTE_OCS_PATH)		    	m_nError = ERR_OCS_ESTOP1;
							else if(m_pExecuteInfo->QuickCmdEStopType == OHT_EXIST_IN_COLLISON_AREA)	m_nError = ERR_OCS_ESTOP2;
							else if(m_pExecuteInfo->QuickCmdEStopType == CARRIER_EXIST_4_UNLOADING)	    m_nError = ERR_OCS_ESTOP3;
							else if(m_pExecuteInfo->QuickCmdEStopType == NOT_CARRIER_4_LOADING)		    m_nError = ERR_OCS_ESTOP4;
							else if(m_pExecuteInfo->QuickCmdEStopType == TRUNOFF_STOP)		 			m_nError = NO_ERR;
							else if(m_pExecuteInfo->QuickCmdEStopType == TRUNON)						m_nError = NO_ERR;
							else																		m_nError = ERR_OCS_ESTOP5;
						}

                        // STOP ��� �߻� ��, OCS, TP Pause ������ ���, Pasue Flag ����
                        // 2017-01-09, Add by Labeler
                        if((m_pExecuteInfo->bTpPause != false) || (m_pExecuteInfo->bOcsPause != false) || (pHWSet->m_pCID->GetCIDPause() != false))
                        {
                            m_pExecuteInfo->bTpPause = false;
                            m_pExecuteInfo->bOcsPause = false;
                            pHWSet->m_pCID->SetCIDPause(false);
                        }
					}
				}

				m_pExecuteInfo->QuickCmd=EXECUTE_QUICK_CMD_NONE;
				CS_CmdList->Release();
			}

			//1 4. D/T/M Ȯ��
//			ADD_LOG("handleModuleControl m_pExecuteInfo->ExecuteUnit=%d, FlagMainThreadUsing=%d, Count=%d", 
//    			m_pExecuteInfo->ExecuteUnit, m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing, m_pExecuteInfo->ListOfCmd.getCount() );
			if(	(m_pExecuteInfo->ExecuteUnit!=EXECUTE_UNIT_NONE)	&&
				(m_pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing==false) 		&& 
				(m_pExecuteInfo->ListOfCmd.getCount()>0)		)	// ����� �Է� ���� �ƴϰ� ����� ������ SKIP
			{
				switch(m_pExecuteInfo->ExecuteUnit)
				{
				case EXECUTE_UNIT_DRIVING:
					nIndex = DRIVING_CONTROL_MODULE;
					nCount = 10;	// Buffer �� ���� �ִ� 10�� �޾ƿ´� ~ ������ �޾ƿ� ��� ������ ������ �߻� ����
					break;
				case EXECUTE_UNIT_TRANS:
					nIndex = TRANS_CONTROL_MODULE;
					nCount = 1;	// Buffer �� ���� 1���� �޾ƿ´� ~ �����翡�� ���� ��� ���� ����: ���� ó�� ���� �ʿ�
					break;
				case EXECUTE_UNIT_MANUAL:
					nIndex = MANUAL_CONTROL_MODULE;
					nCount = 1;	// Buffer �� ���� 1���� �޾ƿ´� ~ Manual ��ɿ��� ���� ��� ���� ����
					break;
				default:
					break;
				}

				nTmpError = checkModules(nIndex, m_pExecuteInfo);
				
				if(nTmpError==NO_ERR)
				{
					nTmpError=handleModuleControl(nIndex, m_pExecuteInfo, nCount);
				}
			}
			else
			{
				switch(m_pExecuteInfo->ExecuteUnit)
				{
				case EXECUTE_UNIT_DRIVING:
					nIndex = DRIVING_CONTROL_MODULE;
					break;
				case EXECUTE_UNIT_TRANS:
					nIndex = TRANS_CONTROL_MODULE;
					break;
				case EXECUTE_UNIT_MANUAL:
				default:
					nIndex = MANUAL_CONTROL_MODULE;
					break;
				}
				
				nTmpError=checkModules(nIndex, m_pExecuteInfo);
				
				if(nTmpError==NO_ERR && m_pModuleControl[nIndex]->IsOnProcess()==true)
				{
					try{
					nTmpError = m_pModuleControl[nIndex]->ExecuteJob();	// �߰� ��� ���� ���� ��� ����
					}catch(...)
                       	ADD_LOG("Excute 22 Exception!!!");
				}				

			}

			//1 5. ���� �̻� �� ����
			if(nTmpError==NO_ERR)
			{
				nTmpError=updateStatus();
			}
			
			if(		(m_nError==NO_ERR)
				&&	(nTmpError!=NO_ERR)
				&&	(isOnAutoRecovery(DRIVING_CONTROL_MODULE, nTmpError)==false)
				&&	(isOnAutoRecovery(TRANS_CONTROL_MODULE, nTmpError)==false)
				&&	(isOnAutoRecovery(MANUAL_CONTROL_MODULE, nTmpError)==false)
			)
			{
				ADD_LOG("isOnAutoRecovery:TempE:%d",nTmpError);
				m_nError	= nTmpError;

				handleQuickCommand(EXECUTE_QUICK_CMD_STOP);
                
//                if(m_pExecuteInfo->bJogInterlockIgnore == true)
//                {
//	                EnterCriticalSection(&OHTMainForm->m_CS);
//                    m_pExecuteInfo->bJogInterlockIgnore = false;
//                    LeaveCriticalSection(&OHTMainForm->m_CS);
//                }
    		}			

			//1 6. Status �� ���� ������ ������Ʈ
			transferDataToMainThread(m_pExecuteInfo);
		}
       //AOHC-204 Thread �� ���� �ð� ���� ���
		static TimeChk timeChk;
		static ThreadTimeMonitor tmon;
		timeChk.End(0);

		tmon.Execute("ExecT", timeChk.Get(0));

		timeChk.Start(0);

		Sleep(5);

	}
}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread �Լ�
@author  ehoto
@date	 2012.11.09
*/
UINT ExecuteThread::updateStatus(void)
{
	for(int n=0; n<3; n++)
	{
		m_ModuleError[n]=m_pModuleControl[n]->GetError();	
		m_Status[n]=m_pModuleControl[n]->GetStatus();		
	}

	for(int n=0; n<3; n++)
	{
		if(m_ModuleError[n]!=NO_ERR)
		{
			return m_ModuleError[n];
		}
	}

	//@todo:��ġ ID ������Ʈ �ʿ�
	
	
	return NO_ERR;
}


//---------------------------------------------------------------------------
/**
@brief	 ExecuteThread �켱���� ���� ��� ���� �Լ�

	�Ʒ��� ����� �� ���� ������
	EXECUTE_QUICK_CMD_NONE = 0, 		///< �ʱ�ȭ ����		A/M
	EXECUTE_QUICK_CMD_STOP = 1, 		///< ��� ����			A/M 	~ OCS_Estop / Status Check ���� Internal Cmd �� E-stop / 
	EXECUTE_QUICK_CMD_PAUSE = 2,		  ///< �Ͻ� ����			A/M 	~ OCS_Pause  
	EXECUTE_QUICK_CMD_RESUME = 3,		  ///< �� ����			A/M 	~ OCS_Resume 
	EXECUTE_QUICK_CMD_CANCEL = 4,		 ///< ���� ��� Clear		 A/M	 ~ OCS �� ��� ������ ���
	EXECUTE_QUICK_CMD_RESET = 5   

@author  ehoto
@date	 2012.11.09
@param	CmdType : ��� Ÿ�� 
@return	���� ���� ( 0:���� �Ϸ� / �׿�:�����ڵ� )

*/
UINT ExecuteThread::handleQuickCommand(EXECUTE_QUICK_CMD CmdType)
{
	if(CmdType==EXECUTE_QUICK_CMD_RESET)
	{
		m_nError = NO_ERR;
	}

	switch(CmdType)
	{
		case EXECUTE_QUICK_CMD_PAUSE_OCS :
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_OCS);
            m_pExecuteInfo->bOcsPause = true;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_RESUME_OCS :
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause�� ��� ���� �Ǿ������ Resume ��� �߻���Ű�� ���� �ּ� ó��
//			m_pHWCommon->SetPauseControl(false, PAUSED_BY_OCS);
            m_pExecuteInfo->bOcsPause = false;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_PAUSE_TP :
			m_pHWCommon->SetPauseControl(true, PAUSED_BY_TP);
            m_pExecuteInfo->bTpPause = true;
			return NO_ERR;
		case EXECUTE_QUICK_CMD_RESUME_TP :
// 2016-12-29, Add by Labeler
// OCS, TP, CID Pause�� ��� ���� �Ǿ������ Resume ��� �߻���Ű�� ���� �ּ� ó��
//			m_pHWCommon->SetPauseControl(false, PAUSED_BY_TP);
            m_pExecuteInfo->bTpPause = false;
			return NO_ERR;
		default :
			break;
	}
	
	for(int n=0; n<3; n++)
	{
		m_ModuleError[n]=m_pModuleControl[n]->SetQuickCmd(CmdType);
	}

	for(int n=0; n<3; n++)
	{
		if(m_ModuleError[n]!=EXECUTE_ACK_NO_ERR)
		{
			return m_ModuleError[n];
		}
	}
	
	return NO_ERR;	
}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread �Լ�

	1. ���� �� ���� Ȯ��
	2. ExecuteInfo�� ��� ����Ʈ Ȯ���Ͽ� ������� �� ModuleControl�� ��� ����
	3. ExecuteJob �Լ� ȣ��
@author  ehoto
@date	 2012.11.09
@param	nIndex: Index of Module
@param	pExecuteInfo: ������ ���� ������ ������
@param	nMaxCountGetCmd: ������������ ����� ������ �ִ� ����
@return	���� ���� ( 0:���� �Ϸ� / �׿�:�����ڵ� )

*/
UINT ExecuteThread::handleModuleControl(int nIndex, EXECUTE_INFO *pExecuteInfo, UINT nMaxCountGetCmd)
{
	UINT nTmpError=NO_ERR;
	BYTE Status = 0;
	UINT nCount = 0;
	UINT nTmpCountGetCmd=nMaxCountGetCmd;
	void* 		pCmd = NULL;
	EXECUTE_ACK TmpExecuteAck = 0;
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	// 1. ���� �� ���� Ȯ��
	Status		= pModuleControl->GetStatus();
	nTmpError	= pModuleControl->GetError();

	if((m_pExecuteInfo->ExecuteMode != EXECUTE_MODE_MANUAL) || (!m_pExecuteInfo->bJogInterlockIgnore))	
	{
		if(nTmpError!=NO_ERR || Status == STS_ERROR) //|| Status == STS_TURN_OFF)
		{
			// �ܿ� ��� ����
			deleteCommand(nIndex, pExecuteInfo);
			return nTmpError;
		}
	}	

	// 2. ExecuteInfo�� ��� ����Ʈ Ȯ���Ͽ� ������� �� ModuleControl�� ��� ����
	nCount = pExecuteInfo->ListOfCmd.getCount();

	if((pExecuteInfo->OnAddingCmd.Info.FlagMainThreadUsing==false) && (nCount>0))
	{
		CS_CmdList->Acquire();

		if(nCount>nTmpCountGetCmd)
		{
			nCount = nTmpCountGetCmd;	// 10�� �̻� ��带 �������� �ʴ´� -> �ٸ� ���ۿ� ���� �ʷ� ����
		}

		ADD_LOG("Excute Count %d",nCount);

		for(int n=0; n<nCount; n++)
		{
			pCmd = pExecuteInfo->ListOfCmd.referFront();
//			ADD_LOG("Excute address %X",pCmd);
			if(pCmd ==NULL)
			{
 				pCmd = pExecuteInfo->ListOfCmd.referFront();     //���� ������ ����.
			}

			TmpExecuteAck = pModuleControl->InsertGeneralCmd(pCmd);
			switch(TmpExecuteAck)
			{
			case EXECUTE_ACK_NO_ERR:	// ���� ó�� �Ϸ�
				pExecuteInfo->ListOfCmd.popFront();
				break;
			case EXECUTE_ACK_BUSY:		// ������ busy �� ��� ���� ���� ��� �ϴ�
				n=nCount;				// for �� ���
				break;
			case EXECUTE_ACK_NOT_AVAILABLE_CMD:	// �Ұ��� ��� �� ��� ��� ����
			case EXECUTE_ACK_ON_ERR:
				deleteCommand(nIndex, pExecuteInfo);
				n=nCount;				// for �� ���
				break;
			}
		}

		CS_CmdList->Release();
	}

	updateStatus();
	transferDataToMainThread(pExecuteInfo); 	// �ӽ�: ���� �� ���� ���� ���� ���� �ϱ� ���� 

	// 3. ExecuteJob �Լ� ȣ��
	if(pModuleControl->IsOnProcess()==true)
	{
		nTmpError = pModuleControl->ExecuteJob();
	}

	return nTmpError;
		
}


//---------------------------------------------------------------------------
/**
@brief	 CheckAbnormal �Լ�

	1. D/T/M ��ɿ� ���� �ٸ� ��⿡���� ������ ���� ���� Ȯ��
@author  ���¿�
@date	 2013.02.08
@param	nIndex: Index of Module
@return ���� ���� ( 0:���� / �׿�:�����ڵ� )
*/										
UINT ExecuteThread::checkModules(int nIndex, EXECUTE_INFO * pExecuteInfo)
{

	UINT nTMPError	=NO_ERR;
	int nMouduleIndex[3]	= {0,};
	int nCountOfModule	= 0;
	EXECUTE_MODE Mode = pExecuteInfo->ExecuteMode;

	if(	Mode==EXECUTE_MODE_AUTO &&
		m_pModuleControl[TRANS_CONTROL_MODULE]->IsOnProcess()==false &&
		m_pModuleControl[DRIVING_CONTROL_MODULE]->IsOnProcess()==false
		)
	{
		nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;
		nMouduleIndex[1]	= TRANS_CONTROL_MODULE;
		nMouduleIndex[2]	= DRIVING_CONTROL_MODULE;
		nCountOfModule	= 3;
	}
	else
	{
		switch(nIndex)
		{
		case DRIVING_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;    //������ �޴��� �ϴ°� ����
			nMouduleIndex[1]	= TRANS_CONTROL_MODULE;
			nCountOfModule	= 2;
			break;
		case TRANS_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;    //������ �޴��� �ϴ°� ����
			nMouduleIndex[1]	= DRIVING_CONTROL_MODULE;
			nCountOfModule	= 2;
			break;
		case MANUAL_CONTROL_MODULE:
			nMouduleIndex[0]	= MANUAL_CONTROL_MODULE;   
			nCountOfModule	= 1;
			break;
		}
	}

	//2	��� Ȯ�� ����
	try{
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[MANUAL_CONTROL_MODULE]->CheckAlways();
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[DRIVING_CONTROL_MODULE]->CheckAlways();
	if(nTMPError==NO_ERR)	nTMPError = m_pModuleControl[TRANS_CONTROL_MODULE]->CheckAlways();
	}catch(...)
    	ADD_LOG("Excute Exception!!!");
	//2	��� �� �ٸ� Unit �� ���� ���� �� Ȯ�� �ϴ� ����
	if(nTMPError==NO_ERR)
	{
		for(int nTmpIndex=0; nTmpIndex<nCountOfModule; nTmpIndex++)
		{
			nTMPError = m_pModuleControl[nMouduleIndex[nTmpIndex]]->CheckAbnormalOnWait();
			if(nTMPError!=NO_ERR)	break;
		}
	}
	
	if((nTMPError == NO_ERR) && (m_pExecuteInfo->ExecuteMode == EXECUTE_MODE_MANUAL) && (m_pExecuteInfo->bJogInterlockIgnore))
	{
		nTMPError = NO_ERR;
	}
	else
	{
//		if(m_pExecuteInfo->bJogInterlockIgnore == true)
//		{
//			EnterCriticalSection(&OHTMainForm->m_CS);
//			m_pExecuteInfo->bJogInterlockIgnore = false;
//			LeaveCriticalSection(&OHTMainForm->m_CS);
//		}
		
		//2	���� �߻� �� ��� ����
		if(nTMPError!=NO_ERR)
		{
			deleteCommand(nIndex, pExecuteInfo);
		}
	}
	
	return nTMPError;
		
}

/**
@brief	 CheckAbnormal �Լ�

	1. D/T/M ��ɿ� ���� �ٸ� ��⿡���� ������ ���� ���� Ȯ��
@author  ���¿�
@date	 2013.02.08
@param	nIndex: Index of Module
@return ���� ���� ( 0:���� / �׿�:�����ڵ� )
*/										
UINT ExecuteThread::deleteCommand(int nIndex, EXECUTE_INFO *pExecuteInfo)
{
	int		nTMPError 	= NO_ERR;
	UINT	nCount 		= 0;
	void	*pCmd 		= NULL;
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	nCount = pExecuteInfo->ListOfCmd.getCount();
	CS_CmdList->Acquire();


	for(int n=0; n<nCount; n++)
	{
		pCmd = pExecuteInfo->ListOfCmd.popFront();
		if(pCmd!=NULL)
		{
			if(pModuleControl->DeleteCmd(pCmd)==false) break;	//@todo: �߸��� �޸� ó���� ���� ó�� �ʿ� ���ɼ� ����
		}
	}
	CS_CmdList->Release();
	return nTMPError;

}

//---------------------------------------------------------------------------
/**
@brief	 RealThreadProc Thread �Լ�

	1. Status						// I, A, G, L, O, U, N ...
	2. nErrorCode
	3. CurrentDrivingNode			// ���� ��� ID
	4. CurNodeOffset		// ���� ��忡���� Offset �Ÿ� : �Ҽ��� ����(mm)
	5. CurrentTransNode			//< ������ ��� : ��ũ �Ŀ��� ������Ʈ
@author  ehoto
@date	 2012.11.09
@param	pExecuteInfo: ������ ���� ������ ������
@return	���� ���� ( 0:���� �Ϸ� / �׿�:�����ڵ� )
*/
UINT ExecuteThread::transferDataToMainThread(EXECUTE_INFO *pExecuteInfo)
{
	UINT nTMPError=NO_ERR;
	BYTE TmpStatus=STS_INIT;
	EXECUTE_INFO *pTmpExecuteInfo = pExecuteInfo;
	STATUS_CMD_ID_UNION	CurrStatusCmdID;
	STATUS_CMD_ID_UNION	TmpStatusCmdID;
	CMD_ON_ADDING_UNION CurrOnAddingCmd;
	EXECUTE_UNIT		CurrExecuteUnit = pTmpExecuteInfo->ExecuteUnit;
	int nCmdCount = pTmpExecuteInfo->ListOfCmd.getCount();
	VirtualDrivingControl* pDrivingControl = ((VirtualDrivingControl*)m_pModuleControl[DRIVING_CONTROL_MODULE]);
	VirtualTransControl* pTransControl = ((VirtualTransControl*)m_pModuleControl[TRANS_CONTROL_MODULE]);

	//1	0. �⺻ ���� ������Ʈ
	pExecuteInfo->ExecutePositionInfo = pDrivingControl->GetPositionInfo();
	pDrivingControl->GetMapData(&(pExecuteInfo->ListOfMapInfo), &(pExecuteInfo->OnAddingMapInfo));
	pExecuteInfo->TransDifference = pTransControl->getDifference();

	//1	1. Status
	//1	2. nErrorCode
	CurrStatusCmdID.Data = pTmpExecuteInfo->StatusCmdId.Data;
	TmpStatusCmdID.Data	= CurrStatusCmdID.Data;
	CurrOnAddingCmd.Data = pTmpExecuteInfo->OnAddingCmd.Data;
	if(m_nError==NO_ERR)
	{
		for(int n=0; n<3; n++)
		{
//			if(m_ModuleError[n]!=NO_ERR &&m_nError==NO_ERR )
//			{
//				m_nError = m_ModuleError[n];
//			}
			if(m_Status[n]==STS_ERROR && TmpStatus!=STS_ERROR)
			{
				if(m_ModuleError[n]!=NO_ERR &&m_nError==NO_ERR )
				{
					m_nError = m_ModuleError[n];
				}
				TmpStatus = m_Status[n];
			}
		}
	}

	if(m_nError!=NO_ERR||TmpStatus==STS_ERROR)
	{
		TmpStatusCmdID.Info.Status 			= STS_ERROR;
		pTmpExecuteInfo->StatusCmdId.Data	= TmpStatusCmdID.Data;	// int ���� ���� //Info.Status = STS_ERROR;
		pTmpExecuteInfo->ErrCode_ExecuteInfo= m_nError;
	}
	else
	{
		TmpStatus = STS_INIT;
		nTMPError = NO_ERR;

		switch(pTmpExecuteInfo->ExecuteUnit)
		{
		case EXECUTE_UNIT_DRIVING:
			{
				TmpStatus=m_Status[DRIVING_CONTROL_MODULE];
				nTMPError=m_ModuleError[DRIVING_CONTROL_MODULE];
			}
			break;
		case EXECUTE_UNIT_TRANS:
			{
				TmpStatus=m_Status[TRANS_CONTROL_MODULE];
				nTMPError=m_ModuleError[TRANS_CONTROL_MODULE];
			}
			break;
		case EXECUTE_UNIT_MANUAL:
			{
				TmpStatus=m_Status[MANUAL_CONTROL_MODULE];
				nTMPError=m_ModuleError[MANUAL_CONTROL_MODULE];
			}
			break;
		default:
			break;
		}

		//2	Status ���� �����(Cmd ID ����)
		if(	(TmpStatusCmdID.Info.Status == STS_ARRIVED		&& CurrExecuteUnit!=EXECUTE_UNIT_DRIVING)	||
			(TmpStatusCmdID.Info.Status == STS_UNLOADED		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)		||
			(TmpStatusCmdID.Info.Status == STS_LOADED		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)	  ||
			(TmpStatusCmdID.Info.Status == STS_AUTO_TAUGHT		&& CurrExecuteUnit!=EXECUTE_UNIT_TRANS)	)
		{
			// �Ϸ� ���´� ���� ���� ����: D/T/M ���濡 ���� ���� ����
			if(	TmpStatus!=STS_ARRIVED	&& TmpStatus!=STS_UNLOADED	&& TmpStatus!=STS_LOADED  &&  TmpStatus!=STS_AUTO_TAUGHT &&
				TmpStatus!=STS_INIT		&& TmpStatus!=STS_COMPLETE	)
			{
				TmpStatusCmdID.Info.Status = TmpStatus;
			}
			else if(TmpStatus==STS_INIT	
				&&	m_Status[DRIVING_CONTROL_MODULE]==STS_INIT
				&&	m_Status[TRANS_CONTROL_MODULE]==STS_INIT
				&&	m_Status[MANUAL_CONTROL_MODULE]==STS_INIT
				)
			{
				TmpStatusCmdID.Info.Status = TmpStatus;
			}
		}
		else
		{
			TmpStatusCmdID.Info.Status = TmpStatus;
		}
		
		switch(TmpStatus)	//2	 ���� ��Ȳ�� ���� Cmd ID ���� 
		{
		case STS_INIT:
			TmpStatusCmdID.Info.PreCmd 	= 0;
			TmpStatusCmdID.Info.CurrCmd	= 0;
			TmpStatusCmdID.Info.NextCmd	= 0;
			break;

		case STS_ARRIVED:
		case STS_UNLOADED:
		case STS_LOADED:
		case STS_COMPLETE:
		case STS_AUTO_TAUGHT:
			if(	(CurrOnAddingCmd.Info.FlagMainThreadUsing==false)	// �Է����̸� �Ѿ
			&&	(CurrStatusCmdID.Info.CurrCmd	!= 0)
			&&	(nCmdCount==0)
			)
			{
				TmpStatusCmdID.Info.PreCmd 	= CurrStatusCmdID.Info.CurrCmd;
				TmpStatusCmdID.Info.CurrCmd	= 0;
				TmpStatusCmdID.Info.NextCmd	= 0;
			}
			break;

		case STS_GOING:
		case STS_UNLOADING:
		case STS_LOADING:
		case STS_AUTOPOSITION:
		case STS_TURN_OFF: // �⵵�����
		case STS_RUNNING:
		case STS_AUTO_TEACHING:
		case STS_AUTO_TEACHING_NULLTYPE:
		case STS_AUTO_TRANS_HOMING:
			if( (CurrOnAddingCmd.Info.FlagMainThreadUsing==false)	// �Է��� �Ϸ�� ��ɿ��� ����
			&&	(CurrOnAddingCmd.Info.CmdID!=CurrStatusCmdID.Info.CurrCmd)
            &&	(CurrOnAddingCmd.Info.CmdID!=0))
			{
                if(CurrStatusCmdID.Info.CurrCmd!=0)
                {
				    TmpStatusCmdID.Info.PreCmd = CurrStatusCmdID.Info.CurrCmd;
                }
				TmpStatusCmdID.Info.CurrCmd	= CurrOnAddingCmd.Info.CmdID;
				TmpStatusCmdID.Info.NextCmd	= 0;

				ADD_LOG("Status:%d CmdID(%d:%d:%d) AddingCmd(%d,%d) --> CmdID(%d:%d:%d)"
					, CurrStatusCmdID.Info.Status
					, CurrStatusCmdID.Info.PreCmd
					, CurrStatusCmdID.Info.CurrCmd
					, CurrStatusCmdID.Info.NextCmd
					, CurrOnAddingCmd.Info.FlagMainThreadUsing
					, CurrOnAddingCmd.Info.CmdID
					, TmpStatusCmdID.Info.PreCmd
					, TmpStatusCmdID.Info.CurrCmd
					, TmpStatusCmdID.Info.NextCmd
					);
			}

			break;
			
		case STS_ERROR:
		default:
			break;
		}
		
		pTmpExecuteInfo->StatusCmdId.Data		= TmpStatusCmdID.Data;
		pTmpExecuteInfo->ErrCode_ExecuteInfo	= nTMPError;
	}

	nTMPError = pTmpExecuteInfo->ErrCode_ExecuteInfo;
	return nTMPError;

	//@todo:��ġ ID ������Ʈ �ʿ�
	// 3. CurrentDrivingNode			// ���� ��� ID
	// 4. CurNodeOffset		// ���� ��忡���� Offset �Ÿ� : �Ҽ��� ����(mm)
	// 5. CurrentTransNode			//< ������ ��� : ��ũ �Ŀ��� ������Ʈ


}
//---------------------------------------------------------------------------


bool ExecuteThread::isOnAutoRecovery(int nIndex, int nError)
{
	bool bOnRetry = false;
	
	VirtualModuleControl *pModuleControl = m_pModuleControl[nIndex];

	if(((pModuleControl->GetStatus()==STS_AUTORECOVERY && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_OVERRUN && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_BLANKTRANS && pModuleControl->GetError()==nError)) ||
		((pModuleControl->GetStatus()==STS_AUTORECOVERY_DOUBLETRANS && pModuleControl->GetError()==nError)))
	{
		bOnRetry = true;
	}
	
	return bOnRetry;

}



