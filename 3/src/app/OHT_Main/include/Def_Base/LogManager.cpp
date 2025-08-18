// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.cpp
@version OHT 7.0
@brief   AMCDllLibQueue Class �ҽ� File
*/
// ---------------------------------------------------------------------------




#include "LogManager.h"
#include "Def_OHT.h"

#define	INITIALIZE_CRITICAL_SECTION		InitializeCriticalSection(&m_CS)
#define	ENTER_CRITICAL_SECTION			EnterCriticalSection(&m_CS)
#define	LEAVE_CRITICAL_SECTION			LeaveCriticalSection(&m_CS)
#define	DELETE_CRITICAL_SECTION			DeleteCriticalSection(&m_CS)

#define	MIN_TOTAL_LOG_BUFFER_BYTE_SIZE	4096

#define	ERR_LOG_BUFFER_IS_FULL	930	///@todo: ���Ƿ� ���� ���� ��ȣ - ���� ���� �ʿ� - ���� ���� ����, �л� ������ ���� �ʿ� 


/**
@brief   LogManager �ʱ�ȭ
@author  ���¿�
@date    2013.01.21
*/
LogManager::LogManager(unsigned int nMaxByteSizeOfLogSum, bool checkLogFolder)
{
	// ũ��Ƽ�� ���� �ʱ�ȭ
	INITIALIZE_CRITICAL_SECTION;
	m_nByteSizeOfLogSum		= 0;
	m_nMaxByteSizeOfLogSum	= 0;
	m_nLostLogCnt = 0;
	m_nLogCnt = 0;
	m_bLogfileCheck = false;
	m_checkLogFolder = checkLogFolder;
	
	m_LogUnitMap.clear();

	// Buffer MAX ������ ����
	if( nMaxByteSizeOfLogSum > MIN_TOTAL_LOG_BUFFER_BYTE_SIZE )
	{
		m_nMaxByteSizeOfLogSum = nMaxByteSizeOfLogSum;
	}
	else
	{
		m_nMaxByteSizeOfLogSum = MIN_TOTAL_LOG_BUFFER_BYTE_SIZE;
	}
	pClientSocket = new TCPCLIENT;
}

/**
@brief   LogManager �Ҹ���
@author  ���¿�
@date    2013.01.21
*/
LogManager::~LogManager()
{
	// ������ �α� ���� �Ҹ�
	LogUnit* pLogUnit = NULL;
	int nCountOfLogUnit = m_LList.getCount();
	m_LogUnitMap.clear();

	Sleep(3000);

	for(int n=0; n<nCountOfLogUnit; n++)
	{
		pLogUnit = (LogUnit*)m_LList.popFront();
		delete pLogUnit;
		pLogUnit = NULL;
	}
	delete pClientSocket;

	// ũ��Ƽ�� ���� �Ҹ�
	DELETE_CRITICAL_SECTION;
}

/**
@brief   User�� �α� ����ϴ� class�� �����ؼ� �����͸� �������� �Լ�
@author  ���¿�
@date    2013.01.21
@param   pList : Manager �� �α׸� ��ư��� ����ü
@param   nSize : ��ư��� �α��� �� �뷮
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� LogUnit�� ������ ��ȯ, ������ �� NULL �� ��ȯ 
*/
bool	LogManager::connectLogUnit(LogUnit *pLogUnit)
{
	bool	bReturn = false;

	
	ENTER_CRITICAL_SECTION;
	if((m_nByteSizeOfLogSum + pLogUnit->m_nMaxSizeOfLog)>m_nMaxByteSizeOfLogSum)	// �α� �뷮�� ���� �ѿ뷮�� �ʰ��� ��� �̻���
	{
		bReturn = false;
	}
	else
	{
		bReturn = true;
		m_LList.insertBack((void *) pLogUnit);
		m_nByteSizeOfLogSum+=pLogUnit->m_nMaxSizeOfLog;
	}
	LEAVE_CRITICAL_SECTION;
	
	return bReturn;
}

/**
@brief   ������ ������ log ��� class�� log�� �����ؼ� ����ϴ� �Լ�
@author  ���¿�
@date    2013.01.21
@param   pList : Manager �� �α׸� ��ư��� ����ü
@param   nSize : ��ư��� �α��� �� �뷮
@param   nError : ���� �� ��ȯ�ϴ� ������
@return	 ���� ó���Ǹ� true ��ȯ(Buffer�� �ƹ� ���� ���� �α׸� ��ư��� �ʾҴٸ� �̰͵� ����ó���̴�)
*/
bool  LogManager::handleLogSum()
{
	int nCountOfLogUnit = 0;
	LogUnit *pLogUnit = NULL;

	// Log Unit�� �� ������ Ȯ��
	ENTER_CRITICAL_SECTION;
	nCountOfLogUnit = m_LList.getCount();
	LEAVE_CRITICAL_SECTION;

	// ������ Log Unit ���� Log�� Ȯ���Ͽ� ����
	for(int nIndex=0; nIndex<nCountOfLogUnit; nIndex++)
	{
		pLogUnit = (LogUnit*)m_LList.refer(nIndex);
		
		// Log ����� ����

		//bool bRet = handleLogUnitTime(pLogUnit); // Line by Line ��� ���
		bool bRet = handleLogUnitTimeEx(pLogUnit); // �޸� ���ۿ� �׾Ƽ� �ѹ��� Writing ���


		//if(bRet)
		//	Sleep(1);	// Context Switching
	}

     return m_bLogfileCheck;
}

LogUnit*	LogManager::getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{

	char szFullSavePath[_MAX_PATH]={0,};
	char* pFullPathResult = NULL;
	pFullPathResult = _fullpath(szFullSavePath, szSavePath, _MAX_PATH);

	LogUnit* pLogUnit = NULL;
	std::string strSavePath = std::string(szFullSavePath);
	std::map<std::string, LogUnit*>::iterator IterPos;

	if(pFullPathResult!=NULL)
	{
		IterPos = m_LogUnitMap.find(strSavePath);

		if(IterPos == m_LogUnitMap.end())	// ���� ������
		{
			pLogUnit = new LogUnit(szFullSavePath, nMaxSizeOfLog);
			connectLogUnit(pLogUnit);
			m_LogUnitMap[strSavePath] = pLogUnit;		//m_LogUnitMap.insert(std::map<std::string, LogUnit*>::value_type(strSavePath, pLogUnit));
		}
		else
		{
			pLogUnit = (*IterPos).second;
		}
	}
	
	return pLogUnit;	
}


// AMC_RECV �α� ����� ����10�������� 91������ �ٲ� ����
// �α� ���� ���� �����Ҷ� ��� �ο� �α� ������ ����ؾ� ���߿� Converting �Ҷ� ������ �� �ִ�
// �̿� �α� ������ ���� ������ amc recv �α��������� Ȯ���� �ִ� �Լ�
// �ð� loss�� �ִ� �Լ��� �ƴϴ�
// name ���� "AMC_Recv" Ȥ�� "AMC_Send_Periodic" ���ڿ��� ���⸦ ���
bool LogManager::IsNewLogFile(char *path, char *name)
{
	bool bNewFlag = false;

	try
	{
		bNewFlag = !FileExists(path);

		if(bNewFlag)
		{
			if(name == NULL)
				return false;

			int nNameLen = strlen(name);
			if(nNameLen < 1 || nNameLen > 100)
				return false;

			char szFileName[512];
			memset(szFileName, 0, sizeof(szFileName));

			String str = ExtractFileName(path);
			wcstombs(szFileName, str.c_str() , sizeof(szFileName));

			// ��ġ�ϴ��� Ȯ��
			if(strlen(szFileName) == nNameLen + 14)//  [2019031912.log] <- 14��
			{
				bool bMatch = true;
				for(int i=0; i<nNameLen ; i++)
					bMatch &= (szFileName[i] == name[i]);

				return bMatch;
			}
		}
	}
	catch(...){}

	return false;
}
// SHCHO : �α� ��� �Լ� handleLogUnitTime
// Line by Line ���
// �α� �׸��� �ð� �������� ���� �̸��� �����ϱ� ����
// ��������  ����ð� �������� ������ �����Ͽ� ���� �α� �ð��� ���ϸ��� ��Ģ�� ���� �� �ִ�.
// ex) 1��1�� 00:00:00 �αװ� 12��31�� 23:59:58 �� �����ԵǴ� �� ����.
bool LogManager::handleLogUnitTime(LogUnit* pLogUnit)
{
	int nCountOfLostLog	= 0;
	LList TmpList;
	int nTmpError = 0;	// NO_ERR;

	// �α� ����Ʈ�� ���´�
	if(!pLogUnit->get(&TmpList, &nCountOfLostLog ,&nTmpError))
		return false;

	if(TmpList.getCount() == 0)
		return false;


	while(TmpList.getCount() > 0)
	{
		LOG_PACKET *pLogPacket = (LOG_PACKET*)TmpList.popFront();

		//��¥ �ð��� �ִ´�.
		char szFullPath[256];
		char szTmpPath[256];
		memset(szFullPath, 0, sizeof(szFullPath));
		memset(szTmpPath, 0, sizeof(szTmpPath));

		sprintf(szTmpPath,"%s", pLogUnit->getLogName());
		int len = strlen(szTmpPath);
		if(len < 6)
			continue;  // exception case

		//Ȯ���� ����
		for(int i=0; i<4; i++)
			szTmpPath[len-i-1]='\0';

		sprintf(szFullPath, "%s%04d%02d%02d%02d.log", szTmpPath, pLogPacket->LocalTime.wYear, pLogPacket->LocalTime.wMonth, pLogPacket->LocalTime.wDay, pLogPacket->LocalTime.wHour);

		// SHCHO 2019.03
		// ���� �α������� ���� �����ؾ��ϴ� AMC_RECV or AmcPeriodic �α����� Ȯ��
		// �´ٸ� �α� ��ܿ� �α� ���� ����ؾ���. ���� ��ȯ�ϴ� ������ ����ϱ� ����
		bool bShowLogVer = IsNewLogFile(szFullPath, "AMC_Recv");
			bShowLogVer |= IsNewLogFile(szFullPath, "AMC_Send_Periodic");

		FILE *fp = NULL;
		try
		{	fp = fopen(szFullPath, "a");
			if(fp == NULL)
				continue;

			if(bShowLogVer)
				fprintf(fp, "LOG_VERSION = 2.0\n");	// �α� ���� ���

			// �սǵ� �αװ� �ִٸ� ������ ����Ѵ�
			if(nCountOfLostLog > 0)
			{
				fprintf(fp, ">>> LOG OVERFLOW <<< %u of %d\n", TmpList.getCount(), nCountOfLostLog);
				nCountOfLostLog = 0;
				//Sleep(0);	// Context Switching
			}

			char szContents[1024];
			memset(szContents, 0, sizeof(szContents));
			memcpy(szContents, pLogPacket->pcValue, pLogPacket->nLength);

			fprintf(fp, "[%02d:%02d:%02d.%03d]%s\n", pLogPacket->LocalTime.wHour, pLogPacket->LocalTime.wMinute, pLogPacket->LocalTime.wSecond, pLogPacket->LocalTime.wMilliseconds, szContents);

			//Sleep(0);	// Context Switching

			// ������ �ݴ´�
			fflush(fp);
			//Sleep(0);	// Context Switching
			fclose(fp);
			//Sleep(0);	// Context Switching

		}catch(...)
		{
			if(fp != NULL)
				fclose(fp);
		}

		// ��ü ����
		delete[] pLogPacket->pcValue;
		delete pLogPacket;
	}

	return true;

}

// SHCHO 2019.03 : �α� ��� �Լ� handleLogUnitTimeEx(..)
// ���� �α� ��� ����� Line by Line ���
// �� Line ���� ���� Open -> fprintf -> close �ݺ��Ͽ� �ð� ������尡 �ſ� ŭ
// SDī�忡 ���� �����Ͽ� ���� ���� ���� ����.
// �α� ��Ͻð��� �� �α� ������ ��쵵 ��Ȥ�߻���
//
// ������ ���: �޸� ���ۿ� �α׸� ��Ƽ� �ѹ��� ���
// ������ �� ����Ŭ ������
// �α� �ð��밡 �ٲ�ų�, ����ũ��(8Kb)�� ������ File Writing
// ���۰� ������ ���� ��ٸ��°� �ƴϰ�, �� ����Ŭ�� ���۰� �� ������ ������ File Writing
bool LogManager::handleLogUnitTimeEx(LogUnit* pLogUnit)
{
	LList TmpList;
	int nTmpError = 0;	// NO_ERR;
	m_nLostLogCnt = 0;
	m_nLogCnt = 0;

	// �α� ����Ʈ�� ���´�
	if(!pLogUnit->get(&TmpList, &m_nLostLogCnt ,&nTmpError))
		return false;


	if(TmpList.getCount() <= 0 && m_nLostLogCnt == 0)
		return false;

	char dumpBuf[1024*8];    // 8Kb
	memset(dumpBuf, 0, sizeof(dumpBuf));
	int nLogCnt = TmpList.getCount();
	m_nLogCnt = nLogCnt;

	LOG_PACKET *pObj = NULL;  // ���طα�
	int nBufLen = 0;
    // Lost �α� ���� ���

	try
	{
		// �α� ����Ʈ�� ���鼭, �����ϴ� Ÿ���� �α״� �޸𸮿� �״ٰ�
		// �ٸ� Ÿ���� �αװ� ������ �޸𸮿� ���� �α׸� ���Ͽ� Wrting �Ѵ�
		for(int h=0 ; h<nLogCnt ; h++)
		{
			LOG_PACKET *pCur = (LOG_PACKET*)TmpList.refer(h);

			if(pObj == NULL)
				pObj = pCur;

			if(pCur->nLength <= 0)
				continue;

			bool bSizeChk = sizeof(dumpBuf) > (strlen(dumpBuf) + pCur->nLength + 256); // �ð�ǥ�ñ��� ���ļ� �˳��ϰ� �뷮���

			// ���� �α׿� �ð��� üũ.
			//�׸��� ���� �뷮�� ��ġ�� ������ �޸𸮿� �׾Ƴ��´�.
			//�ѹ��� ���� ���� ����
			if(IsSameLogHour(pObj, pCur) && bSizeChk)
			{
				nBufLen = LogMemCopy(pCur, dumpBuf, nBufLen);
				continue;   // ���� �α׷�...
			}

			// ���� �Դٴ°� �ð��밡 �ٲ���ų�, ���۰� �� á�� ���. ���� ����� �Ѵ�
			// ���۸� ���Ͽ� WRITE �Ѵ�.
			LogFileWrite(pLogUnit, pObj, dumpBuf, nBufLen);

			memset(dumpBuf, 0, sizeof(dumpBuf));
			nBufLen = 0;

			// ���طα� ����
			nBufLen = LogMemCopy(pCur, dumpBuf, nBufLen);
			pObj = pCur;
		}

		// ������ dumpBuf Flush
		if(pObj != NULL && nBufLen > 0)
			LogFileWrite(pLogUnit, pObj, dumpBuf, nBufLen);

	}catch(...) { }

	// ��ü ����
	while(TmpList.getCount() > 0)
	{
		LOG_PACKET *pLogPacket = (LOG_PACKET*)TmpList.popFront();
		delete[] pLogPacket->pcValue;
		delete pLogPacket;
	}

	return true;
}

bool LogManager::Get_LogFolder()
{
	return this->m_checkLogFolder;
}

bool LogManager::IsSameLogHour(void *pvA, void *pvB)
{
	LOG_PACKET *pA = (LOG_PACKET*)pvA;
	LOG_PACKET *pB = (LOG_PACKET*)pvB;

	if(pA == NULL || pB == NULL)
		return false;

	if(pA == pB)	// ���� ��ü
		return true;

	bool bSame = true;
	bSame &= pA->LocalTime.wYear 	== pB->LocalTime.wYear;
	bSame &= pA->LocalTime.wMonth  	== pB->LocalTime.wMonth;
	bSame &= pA->LocalTime.wDay 	== pB->LocalTime.wDay;
	bSame &= pA->LocalTime.wHour 	== pB->LocalTime.wHour;

	return bSame;
}

int LogManager::LogMemCopy(void* pvLog, char *logBuf, int nBufSize)
{
	LOG_PACKET* pLog = (LOG_PACKET*)pvLog;

	if(pLog == NULL)
		return 0;

	// �α� ���� �ð� �ٹ̱�
	char szTime[64];
	memset(szTime, 0, sizeof(szTime));
	sprintf(szTime, "[%02d:%02d:%02d.%03d]", pLog->LocalTime.wHour, pLog->LocalTime.wMinute, pLog->LocalTime.wSecond, pLog->LocalTime.wMilliseconds);
	int nTimeLen = strlen(szTime);

	char lineFeed = '\n';

	// strcpy�� strcat, strlen �� �Լ��� ������ ���� �Լ��̹Ƿ� ����� �����Ѵ�
	// ��Ȯ�� memcpy�� ����ϸ� �ν� ������

	// �ð� ����
	memcpy(logBuf + nBufSize, szTime, nTimeLen);
	nBufSize += nTimeLen;

	// �α� ���� ����
	memcpy(logBuf + nBufSize, pLog->pcValue, pLog->nLength);
	nBufSize += pLog->nLength;

	// �ٹٲ� char ����
	memcpy(logBuf + nBufSize, &lineFeed, 1);
	nBufSize += 1;

	return nBufSize;
}

bool LogManager::LogFileWrite(LogUnit *pLogUnit, void* pvLog, char *buffer, int nBufSize)
{
	LOG_PACKET *pLog = (LOG_PACKET*)pvLog;

	if(pLogUnit == NULL || pLog == NULL || buffer == NULL)
		return false;

	if(nBufSize == 0)
		return true;


	//��¥ �ð��� �ִ´�.
	char szFullPath[256];
	char szTmpPath[256];
	memset(szFullPath, 0, sizeof(szFullPath));
	memset(szTmpPath, 0, sizeof(szTmpPath));

	sprintf(szTmpPath,"%s", pLogUnit->getLogName());

	int len = strlen(szTmpPath);
	if(len < 6)
		return false;  // exception case

	//Ȯ���� ����
	for(int i=0; i<4; i++)
		szTmpPath[len-i-1]='\0';

	sprintf(szFullPath, "%s%04d%02d%02d%02d.log", szTmpPath, pLog->LocalTime.wYear, pLog->LocalTime.wMonth, pLog->LocalTime.wDay, pLog->LocalTime.wHour);

	// SHCHO 2019.03
	// ���� �α������� ���� �����ؾ��ϴ� AMC_RECV or AmcPeriodic �α����� Ȯ��
	// �´ٸ� �α� ��ܿ� �α� ���� ����ؾ���. ���� ��ȯ�ϴ� ������ ����ϱ� ����
	bool bShowLogVer = IsNewLogFile(szFullPath, "AMC_Recv");
		 bShowLogVer |= IsNewLogFile(szFullPath, "AMC_Send_Periodic");

	bool bShowARLogVer = IsNewLogFile(szFullPath, "AutoRecovery");

	char	cOHTType[10];
	memset(cOHTType, 0, 10);
#if(OHT_NAME == OHT_NAME_STD_V80_RETICLE)
	sprintf(cOHTType,"V80_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V80)
	sprintf(cOHTType,"V80_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V81)
	sprintf(cOHTType,"V81_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	sprintf(cOHTType,"V81S_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V85)
	sprintf(cOHTType,"V85_ROHT");
#elif(OHT_NAME == OHT_NAME_STD_V82)
	sprintf(cOHTType,"V82_FOHT");
#elif(OHT_NAME == OHT_NAME_STD_V86)
	sprintf(cOHTType,"V86_ROHT");
#else
	sprintf(cOHTType,"Unknown");
#endif

	FILE *fp = NULL;
	int wbyte = 0;
	try
	{	fp = fopen(szFullPath, "a");
		if(fp == NULL)
		{
			m_bLogfileCheck = false;
			return false;
		}

		if(bShowLogVer)
			fprintf(fp, "LOG_VERSION = 2.0\n");	// LOG ���� ���
		if(bShowARLogVer)
			fprintf(fp, "Log Ver,%.3f,VHL Ver,%s\n",1.5, cOHTType);	// 1.1 ���� 1�� AutoRecovery ���� �� ���� �� �ø�, �Ҽ����� AutoRecovery Lof ���� �� �ø�

		// ���࿡ �α� ������ ������ ����
		if(m_nLostLogCnt > 0)
		{
			fprintf(fp, ">>> LOG OVERFLOW <<< %d of %d \n", m_nLostLogCnt, m_nLogCnt);
			m_nLostLogCnt = 0;
		}
		// �������� �ѹ濡 ��! Writing
		wbyte = fwrite(buffer, 1, nBufSize, fp);

		if(wbyte != nBufSize)
			m_bLogfileCheck = false;
		else
			m_bLogfileCheck = true;

		fclose(fp);

	}catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}

	return (wbyte == nBufSize);

}

//---------------------------------------------------------------------------

