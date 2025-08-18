#ifndef LogUnitH
#define LogUnitH

#include <stdio.h>
#include <windows.h>
#include "List.h"
#include "LogManager.h"

typedef enum LOG_TAG_
{
	LOG_TAG_FILE_CLEAR	= 0,
	LOG_TAG_WRITE		= 1
}LOG_TAG;

typedef struct _LOG_PACKET
{
	LOG_TAG	Tag;
	int		nLength;

    // by zzang9un 2013.03.20 : �ð� ���� ����
	//DWORD	dwDrivingTime;
	SYSTEMTIME LocalTime;
    
	char	*pcValue;
}LOG_PACKET;

class LogUnit
{
private:
	int m_nTag; 					///< class�� Tag
	char *m_szLogName;			///< �α� �̸�

	CRITICAL_SECTION	m_CS;	///< Manager(Log�� �����ϴ� class)�� USer(Log�� ����� class)���� �浹�� �������� Critical Section
	LList m_LList;				///< Log�� �����ϴ� List

	unsigned int m_nMaxSizeOfLog;		///< Log List�� ���尡���� Char(BYTE)�� �ѷ�(Length, Time ���� ����)
	unsigned int m_nSize;				///< Log List�� ����� char(BYTE)�� �ѷ�(Length, Time ���� ����)
	unsigned int m_nCountOfLostLog;	///< Log List�� ����� Char�� �ѷ�(Length ���� ����)

public:
	LogUnit(char *szLogName, unsigned int nMaxSizeOfLog);	///< ������(nTag:�ν���, sLogName:�α� �̸�, nMaxSizeOfLog: Log���� �ִ� Size~Char����)
	virtual ~LogUnit(void);		///< �Ҹ���
	
	bool	put(int nLength, char *sLog, int *nError);	///< Log�� �Է��ϴ� �Լ�(nLength: sLog����, dwDrivingTime:�ۼ��ð�, sLog: �α�, nError: ����) / �����ϸ� true ����
	bool	put(int nLength, char *sLog);				///< Log�� �Է��ϴ� �Լ�(nLength: sLog����, dwDrivingTime:�ۼ��ð�, sLog: �α�, nError: ����) / �����ϸ� true ����
	void	clear();	
	bool	copyLogNameTo(char* szLogName, int nSize);	///< LogUnit�� ���� ���� ������ �޾ƿ��� ���� �Լ�

protected:	
	bool	setTag(int nTag);		///< LogUnit�� Tag ������ �޾ƿ��� ���� �Լ�
	int		getTag(void);			///< LogUnit�� Tag ������ �޾ƿ��� ���� �Լ�
	char*	getLogName(void);	///< LogUnit�� ���� ���� ������ �޾ƿ��� ���� �Լ�
	bool	get(LList *pList, int*pnCountOfLostLog, int *nError);	///< Log�� �������� �Լ�(pList: Log�� �������� List, nError: ����) / �����ϸ� true ����

	friend class LogManager;
};

#endif
