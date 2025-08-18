// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
@version OHT 7.0
@brief   AMCDllLibQueue Class �ҽ� File
*/
// ---------------------------------------------------------------------------

#include "AMCDllLibQueue.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	AMC_CMD_BUFFER_IS_NOT_ENOUGH	901
#define	MAX_BUFFER_COUNT	128

#define MAX_BYTE_SIZE_OF_AMC_LOG	1024*100

#define AMC_ADD_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\AMC_Send.log", __VA_ARGS__)

/**
@brief   AMCDllLibQueue �ʱ�ȭ
@author  ���¿�
@date    2013.01.21
*/
AMCDllLibQueue::AMCDllLibQueue()
{
	INITIALIZE_CRITICAL_SECTION;

	init();
}

/**
@brief   AMCDllLibQueue �Ҹ���
@author  ���¿�
@date    2013.01.21
*/
AMCDllLibQueue::~AMCDllLibQueue()
{
	DELETE_CRITICAL_SECTION;
}

bool AMCDllLibQueue::init(void)
{
	bool bReturn = true;	
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;

	ENTER_CRITICAL_SECTION;
	while(m_LList.getCount()>0)
	{
		pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront() ;
		delete pQueuingAmcCmd;
	}
	m_nError = 0;
	m_nReportError= 0;
	LEAVE_CRITICAL_SECTION;

	return bReturn;
}

/**
@brief   User�� Buffer�� ������ �Է��� �� ���
@author  ���¿�
@date    2013.01.21
@param   QueuingAmcCmd : �ԷµǴ� ��� ����
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� ����� ������ �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool AMCDllLibQueue::putData(QUEUEING_AMC_CMD QueuingAmcCmd, int *nError)
{
	bool bReturn = true;
	int nTempError = 0;
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;
	int nTmpListCount;

	ENTER_CRITICAL_SECTION;
	nTmpListCount = m_LList.getCount();
	LEAVE_CRITICAL_SECTION;

	if(m_nError!=0)
	{
		for(int nloop = 0 ; nloop < nTmpListCount ; nloop++)
		{
			ENTER_CRITICAL_SECTION;
			pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront() ;
			LEAVE_CRITICAL_SECTION;

			delete pQueuingAmcCmd;
		}

        AMC_ADD_LOG("[putData]Error:%d", m_nError);
		return m_nError;	// ���� �߻� �� ��� �߰� ���� ����
	}
	else if(nTmpListCount >= MAX_BUFFER_COUNT)
	{
		if((nTmpListCount % MAX_BUFFER_COUNT) ==0)
			AMC_ADD_LOG("[putData]AMC_CMD_BUFFER_IS_NOT_ENOUGH, Buffer:%d", nTmpListCount);
	}
	else if(nTmpListCount >= (MAX_BUFFER_COUNT*2))
	{
		for(int nloop = 0 ; nloop < nTmpListCount ; nloop++)
		{
			ENTER_CRITICAL_SECTION;
			pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront();
			LEAVE_CRITICAL_SECTION;

			delete pQueuingAmcCmd;
		}

//		m_nError = AMC_CMD_BUFFER_IS_NOT_ENOUGH;
		m_nReportError = ERR_WINDOWEXCEPTION;
		nTempError = AMC_CMD_BUFFER_IS_NOT_ENOUGH;
        AMC_ADD_LOG("[putData]AMC_CMD_BUFFER_IS_NOT_ENOUGH:%d, Buffer full:%d", m_nError, nTmpListCount);
	}

	pQueuingAmcCmd = new QUEUEING_AMC_CMD();
	memcpy(pQueuingAmcCmd, &QueuingAmcCmd, sizeof(QUEUEING_AMC_CMD));

	///@todo: m_nCount�� Ư�� ������ Ŭ ��� ���� �����ϰ� ���� �߻�: AMC Buffer �뷮 �ʰ�
	ENTER_CRITICAL_SECTION;
	m_LList.insertBack((void*)pQueuingAmcCmd);
	LEAVE_CRITICAL_SECTION;


	*nError = nTempError;
	return bReturn;
}

int AMCDllLibQueue::GetReportError()
{
	return m_nReportError;
}

/**
@brief   AMC�� �ֱ������� ���� ��� ����ü�� ���� �� ���
@author  ���¿�
@date    2013.01.21
@param   pAmcCmd : AMC�� ������ ��� ����ü
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� ����� ������ �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool  AMCDllLibQueue::makeData(AMC_CMD* pAmcCmd, int *nError)
{
	bool bReturn = true;	
	int nTempError = 0;
	int nCmd = 0;
	int nTempCount = 0;
	QUEUEING_AMC_CMD *pQueuingAmcCmd = NULL;

	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// ��������� Count �� �޾ƿ´�.
	LEAVE_CRITICAL_SECTION;

	for(int n=0; n<nTempCount; n++)
	{
		ENTER_CRITICAL_SECTION;
		pQueuingAmcCmd = (QUEUEING_AMC_CMD*)m_LList.popFront();
		LEAVE_CRITICAL_SECTION;

		nCmd = pQueuingAmcCmd->CmdAnalysisUnion.Cmd;
		pAmcCmd->Cmd = pAmcCmd->Cmd|nCmd;
		switch(nCmd)
		{
		case 0x01<<CMD_TYPE_SET_OUTBIT:
            for(int i=0; i<4; i++)
            {
                pAmcCmd->Output[i] = pQueuingAmcCmd->DataUnion.Output[i];
            }
//				memcpy(pAmcCmd->Output, pQueuingAmcCmd->DataUnion.Output, sizeof(int)*4);
			break;
		case 0x01<<CMD_TYPE_CLEAR_NODE_COUNT:
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS1:
			memcpy(&(pAmcCmd->CmdAxis[0]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS2:
			memcpy(&(pAmcCmd->CmdAxis[1]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS3:
			memcpy(&(pAmcCmd->CmdAxis[2]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_MOVE_AXIS4:
			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;
		case 0x01<<CMD_TYPE_SET_DETECT_INFO:
			memcpy(&(pAmcCmd->DetectInfo), &(pQueuingAmcCmd->DataUnion.DetectInfo), sizeof(DETECT_INFO));
#if(USE_CHANGENODE == 1)
			memcpy(&(pAmcCmd->OptNodeInfo), &(pQueuingAmcCmd->DataUnion.OptNodeInfo), sizeof(OPTIONAL_NODE_INFO)); // for change node (jhoun.lim)
#endif
			break;
//			case 0x01<<CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING:
//				pAmcCmd->GearRatioOfDriving = pQueuingAmcCmd->DataUnion.GearRatioOfDriving;
//				break;
//			case 0x01<<AMC_CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
//				break;

		case 0x01<<CMD_TYPE_SET_TORQUE_LIMIT_RESPONSE:
//			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;

		case 0x01<<CMD_TYPE_SET_TORQUE_LIMIT_REALEASE_RESPONSE:
//			memcpy(&(pAmcCmd->CmdAxis[3]), &(pQueuingAmcCmd->DataUnion.CmdAxis), sizeof(CMD_AXIS));
			break;

		case 0x01<<CMD_TYPE_SET_SLOPE_TIME:
//				pAmcCmd->GearRatioOfDriving = pQueuingAmcCmd->DataUnion.GearRatioOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_SLOPE_TIME_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;

		case 0x01<<CMD_TYPE_SET_TARGET_POS:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_TARGET_POS_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_FIRSTNODE_ADD:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_VHLDETECTIGNOR:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_SET_VHLDETECTIGNOR_RELEASE:
//				pAmcCmd->DecelLimitOfDriving= pQueuingAmcCmd->DataUnion.DecelLimitOfDriving;
				break;
		case 0x01<<CMD_TYPE_PAUSE_CONTROL:
			memcpy(&(pAmcCmd->PauseInfo), &(pQueuingAmcCmd->DataUnion.PauseInfo), sizeof(PAUSE_INFO));
			break;
		}
		delete pQueuingAmcCmd;
	}

	*nError = nTempError;
	return bReturn;
}

//---------------------------------------------------------------------------
