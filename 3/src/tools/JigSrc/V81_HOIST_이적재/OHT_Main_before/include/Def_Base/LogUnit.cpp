// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
$Rev:: 1           $:  Revision of last commit
$Author:: ���¿�   $:  Author of last commit
$Date:: 2013-01-21 0#$:  Date of last commit
@version OHT 7.0
@brief   AMCDllLibQueue Class �ҽ� File
*/
// ---------------------------------------------------------------------------

//#include "stdafx.h"
#include "LogUnit.h"
//#include "LogManager.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	MIN_LOG_BUFFER_SIZE	1024

#define	ERR_LOG_BUFFER_IS_FULL	930	///@todo: ���Ƿ� ���� ���� ��ȣ - ���� ���� �ʿ� - ���� ���� ����, �л� ������ ���� �ʿ� 

#define	SIZE_OF_INT_PLUS_DWORD	8	// ���� ����(int:4) + �ð� ����(DWORD:4)

#define	DEFAULT_LOG_NAME	"LOG.log"


//extern LogManager *gm_LogManager;


/**
@brief   LogUnit �ʱ�ȭ
@author  ���¿�
@date    2013.01.21
*/
LogUnit::LogUnit(char *szLogName, unsigned int nMaxSizeOfLog)
{

	// ũ��Ƽ�� ���� �ʱ�ȭ
	INITIALIZE_CRITICAL_SECTION;

	// TAG �ʱ�ȭ
	m_nTag = 0;

	// ���� �̸� ����

	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szLogName, _MAX_PATH);
	
	
	int nLen = strlen(szFullSavePath);
	if(nLen!=0 && pFullPathResult)
	{
		m_szLogName = new char[nLen+1];
		memset(m_szLogName, 0, nLen+1);
		strcpy(m_szLogName, szFullSavePath);
//		m_szLogName[nLen]=0;	// ������ ���� 0(null)�� ����
	}
	else
	{
		nLen = strlen(DEFAULT_LOG_NAME);
		memset(m_szLogName, 0, nLen+1);
		m_szLogName = new char[nLen+1];
		strcpy(m_szLogName, DEFAULT_LOG_NAME);
//		m_szLogName[nLen]=0;	// ������ ���� 0(null)�� ����
	}

	// Buffer MAX ������ ����
	if(nMaxSizeOfLog>MIN_LOG_BUFFER_SIZE)
	{
		m_nMaxSizeOfLog = nMaxSizeOfLog;
	}
	else
	{
		m_nMaxSizeOfLog = MIN_LOG_BUFFER_SIZE;
	}

	// �սǵ� Log�� ����
	m_nCountOfLostLog = 0;
	m_nSize = 0;

//	gm_LogManager->connectLogUnit(this);

}


/**
@brief   LogUnit �Ҹ���
@author  ���¿�
@date    2013.01.21
*/
LogUnit::~LogUnit()
{

	// File Name �Ҹ�
	delete[]	m_szLogName;
	m_szLogName = NULL;

	// List ����
	LOG_PACKET *pLogPacket = NULL;
	int nTempCount = 0;

	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// ��������� Count �� �޾ƿ´�.
	for(int n=0; n<nTempCount; n++)
	{
		pLogPacket = (LOG_PACKET*)m_LList.popFront();
		delete [] pLogPacket->pcValue;
		delete pLogPacket;
	}
	LEAVE_CRITICAL_SECTION;

	// ũ��Ƽ�� ���� �Ҹ�
	DELETE_CRITICAL_SECTION;
	
}

/**
@brief   User�� Buffer�� Log�� �Է��� �� ���
@author  ���¿�
@date    2013.01.21
@param   QueuingAmcCmd : �ԷµǴ� ��� ����
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� ����� ������ �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool LogUnit::put(int nLength, char *sLog, int *nError)
{

	bool bReturn = true;	
	int nTempError = 0;
	char *pLog = NULL;
	LOG_PACKET *pLogPacket;

	pLog				= new char[nLength];
	pLogPacket			= new LOG_PACKET();
	pLogPacket->Tag		= LOG_TAG_WRITE;
	memcpy(pLog, sLog, sizeof(char)*nLength);	
	pLogPacket->nLength	= nLength;

    // by zzang9un 2013.03.20 : �ð� �� ���� ����
	//pLogPacket->dwDrivingTime	= timeGetTime();
    GetLocalTime(&pLogPacket->LocalTime);
    
	pLogPacket->pcValue	= pLog;	
	
	ENTER_CRITICAL_SECTION;
	// m_nSize�� Buffer �ִ밪���� Ŭ ���, ���� �����ϰ� ���� �߻�: AMC Buffer �뷮 �ʰ�
	if(m_nSize+nLength+SIZE_OF_INT_PLUS_DWORD>m_nMaxSizeOfLog)
	{
		bReturn = false;
		nTempError = ERR_LOG_BUFFER_IS_FULL;
		m_nCountOfLostLog++;
		delete[] pLog;
		delete pLogPacket;
	}
	else
	{
		m_LList.insertBack((void*)pLogPacket);
		m_nSize+=(nLength+SIZE_OF_INT_PLUS_DWORD);	// ���� ���ڿ� + ���� ����(int:4) + �ð� ����(DWORD:4)
	}
	LEAVE_CRITICAL_SECTION;

	*nError = nTempError;
	return bReturn;
	
}

/**
@brief   User�� Buffer�� Log�� �Է��� �� ���
@author  ���¿�
@date    2013.01.21
@param   QueuingAmcCmd : �ԷµǴ� ��� ����
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� ����� ������ �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool LogUnit::put(int nLength, char *sLog)
{

	int nError = 0;
	return put(nLength, sLog, &nError);

}


/**
@brief   Log�����ϴ� ���� �ʱ�ȭ
@author  ���¿�
@date    2013.02.25
*/
void LogUnit::clear()
{

	char *pLog = NULL;
	LOG_PACKET *pLogPacket;

	pLogPacket = new LOG_PACKET();
	pLogPacket->Tag		= LOG_TAG_FILE_CLEAR;
	pLogPacket->nLength	= 0;

    // by zzang9un 2013.03.20 : �ð� �� ���� ����
	//pLogPacket->dwDrivingTime	= timeGetTime();
    GetLocalTime(&pLogPacket->LocalTime);
    	
	pLogPacket->pcValue	= NULL;	
	
	ENTER_CRITICAL_SECTION;
	m_LList.insertBack((void*)pLogPacket);
	LEAVE_CRITICAL_SECTION;
	
}


/**
@brief   Mgr(Manager)�� Buffer�� Log�� ������ �� ���
@author  ���¿�
@date    2013.01.21
@return	 ���� ó���Ǹ� Tag ��ȯ, ������ �� NULL�ݳ�
*/
bool LogUnit::setTag(int nTag)
{

	bool bReturn = true;	
	m_nTag = nTag;
	return bReturn;
	
}

/**
@brief   Mgr(Manager)�� Buffer�� Log�� ������ �� ���
@author  ���¿�
@date    2013.01.21
@return	 ���� ó���Ǹ� Tag ��ȯ, ������ �� NULL�ݳ�
*/
int LogUnit::getTag()
{

	return m_nTag;
	
}

/**
@brief   Mgr(Manager)�� Buffer�� Log�� ������ �� ���
@author  ���¿�
@date    2013.01.21
@return	 ���� ó���Ǹ� ���� �̸� ��ȯ, ������ �� NULL�ݳ�
*/
char* LogUnit::getLogName()
{

	return m_szLogName;

}

/**
@brief   Mgr(Manager)�� Buffer�� Log�� ������ �� ���
@author  ���¿�
@date    2013.01.21
@return	 ���� ó���Ǹ� ���� �̸� ��ȯ, ������ �� NULL�ݳ�
*/
bool LogUnit::copyLogNameTo(char* szLogName, int nSize)
{
	bool bSuccess = false;
	int nLen = strlen(m_szLogName);

	if((nLen!=0) && ((nLen+1)<nSize))
	{
		memset(szLogName, 0, nLen+1);
		strcpy(szLogName, m_szLogName);
		bSuccess = true;
	}

	return bSuccess;
}

/**
@brief   Mgr(Manager)�� Buffer�� Log�� ������ �� ���
@author  ���¿�
@date    2013.01.21
@param   pAmcCmd : AMC�� ������ ��� ����ü
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� ����� ������ �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool  LogUnit::get(LList *pList, int*pnCountOfLostLog, int *nError)
{

	// ���� �ʱ�ȭ
	bool bReturn = true;	
	int nTempError = 0;
	int nCmd = 0;
	LOG_PACKET *pLogPacket = NULL;
	int nTempCount = 0;

	// ������ �α� ������ �޾ƿ�
	ENTER_CRITICAL_SECTION;
	nTempCount = m_LList.getCount();	// ��������� Count �� �޾ƿ´�.
	*pnCountOfLostLog = m_nCountOfLostLog;
	m_nCountOfLostLog = 0;
	m_nSize = 0;
	LEAVE_CRITICAL_SECTION;

	// ���� ������ �α׸� Manager�� List�� ���� 
	for(int n=0; n<nTempCount; n++)
	{
		ENTER_CRITICAL_SECTION;
		pLogPacket = (LOG_PACKET*)m_LList.popFront();
		LEAVE_CRITICAL_SECTION;
		pList->insertBack((void*)pLogPacket);
	}

	*nError = nTempError;
	return bReturn;
	
}

//---------------------------------------------------------------------------

