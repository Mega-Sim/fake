#ifndef LogManagerH
#define LogManagerH

#include <stdio.h>
#include <windows.h>
#include <map>
#include <string>

#include "List.h"			// List�� ����ϱ� ���� ����
#include "LogUnit.h"		// User���� Log class�� I/F�� �����ϱ� ���� ����
#include "TCPClient.h"
#include "LogTLV.h"

#define MAX_BYTE_SIZE_OF_LOG    1024*1024
#define MAX_BYTE_SIZE_OF_MAIN_LOG   1024*10     //10->20 ���� AOHC-287 Log ������ ���� -> ����

class LogUnit;

class LogManager
{
private:
	CRITICAL_SECTION	m_CS;						///< LogManager�� LogUnit�� ó���� ���� LogUnit class�� �����ϴ� �κа��� �浹�� �������� Critical Section
	LList 				m_LList;						///< Log�� �����ϴ� List
	unsigned int		m_nMaxByteSizeOfLogSum;		///< LogFile�� �ִ� �뷮
	unsigned int		m_nByteSizeOfLogSum;			///< LogFile�� ���� �뷮
	bool 				m_checkLogFolder;

	std::map<std::string, LogUnit*> m_LogUnitMap;	///< �����η� LogUnit�� ã������ map

public:
	TCPCLIENT*	pClientSocket;


public:
	LogManager(unsigned int nMaxByteSizeOfLogSum, bool checkLogFolder);	///< ������(nTag:�ν���, sLogName:�α� �̸�, nMaxByteSizeOfLogSum: Log���� �ִ� Size~Byte)
	virtual ~LogManager(void);

	// 1. �α� ����ϴ� class�� �����ؼ� �����͸� �����ϴ� �Լ�
	bool	connectLogUnit(LogUnit *pLogUnit);	///< 1. �α� ����ϴ� class�� �����ؼ� �����͸� �����ϴ� �Լ�

	// 2. ������ log ��� class�� log�� �����ؼ� ����ϴ� �Լ�
	bool 	handleLogSum(void);	///< 2. ������ log ��� class�� log�� �����ؼ� ����ϴ� �Լ�

	// 3. Log Unit�� �����ϰų�, �̹� �����ϴ� ��ü�� �����͸� �޾ƿ��� �Լ�
	LogUnit*	getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog);

	bool Get_LogFolder();	
	bool IsNewLogFile(char *path, char *name);
	bool m_bLogfileCheck;
private:
		int LogMemCopy(void* pLog, char *logBuf, int nBufSize);
		bool LogFileWrite(LogUnit *pLogUnit, void* pLog, char *buffer, int nBufSize);
		bool IsSameLogHour(void *pA, void *pB);

		int m_nLostLogCnt;
		int m_nLogCnt;

protected:



	virtual bool handleLogUnitTimeEx(LogUnit* pLogUnit);
	virtual bool handleLogUnitTime(LogUnit* pLogUnit);		///< �α׸� �ð��뺰�� ����


	friend class LogUnit;

};



#endif
