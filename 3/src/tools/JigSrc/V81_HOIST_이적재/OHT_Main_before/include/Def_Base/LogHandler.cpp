// ---------------------------------------------------------------------------
/**
@file    LogHandler.cpp
@version OHT 7.0
@brief   LogHandler Class �ҽ� File
*/
// ---------------------------------------------------------------------------

//#include "stdafx.h"
//#include <stdlib.h>
//#include <exception>
//#include <windows.h>

#include <iostream>
#include <map>
#include <string>
#include "LogHandler.h"

using namespace std;

//---------------------------------------------------------------------------
LogManager *gm_LogManager = NULL;	///< �α� �Ŵ���
FDCManager *gm_FDCManager = NULL;   ///< �̻��� �Ŵ���
//---------------------------------------------------------------------------

/**
@brief   Log Manager ����
@author  ���¿�
@date    2013.04.18
*/
bool startLogManager(unsigned int unSize, bool checkLogFolder)
{
	if(gm_LogManager==NULL)
	{
		gm_LogManager = new LogManager(unSize, checkLogFolder);
		gm_FDCManager = new FDCManager(unSize, checkLogFolder);
		return true;
	}
	else
	{
		return false;
	}
};
//---------------------------------------------------------------------------

/**
@brief   Log Manager ��
@author  ���¿�
@date    2013.04.18
*/
bool endLogManager()
{
	if(gm_LogManager!=NULL)
	{
		delete gm_LogManager;
		gm_LogManager = NULL;

		delete gm_FDCManager;

		return true;
	}
	else
	{
		return false;
	}
};

//---------------------------------------------------------------------------

/**
@brief   Log ó��
@author  ���¿�
@date    2013.04.18
*/
bool manageLog()
{
	if(gm_LogManager!=NULL)
	{
		gm_LogManager->handleLogSum();
		gm_FDCManager->handleFDCSum();
		return true;
	}
	else
	{
		return false;
	}
};

//---------------------------------------------------------------------------

/**
@brief   Log ó��
@author  ���¿�
@date    2013.04.18
*/
LogUnit*	getLogUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{
	if(gm_LogManager!=NULL)
	{
		return gm_LogManager->getLogUnit(szSavePath, nMaxSizeOfLog);
	}
	else
	{
		return NULL;
	}
}

//---------------------------------------------------------------------------

/**
@brief   Log �ۼ�
@author  ���¿�
@date    2013.04.18
*/
bool	writeLogUnit(LogUnit* pLogUnit, char *pStr, ...)
{
	if(gm_LogManager==NULL)
	{
		return false;
	}

    va_list     argp;
    char        szBuf[1024];
	
	memset(&szBuf, 0, sizeof(szBuf));
 
    va_start(argp, pStr);
    sprintf(szBuf, pStr, argp);
    va_end(argp);
	
	return pLogUnit->put(strlen(szBuf), szBuf);

}

//---------------------------------------------------------------------------


/**
@brief   �̻��� ó��
@author  ������
@date    2018.06.25
*/

FDCUnit*	getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfLog)
{
	if(gm_FDCManager!=NULL)
	{
		return gm_FDCManager->getFDCUnit(szSavePath, nMaxSizeOfLog);
	}
	else
	{
		return NULL;
	}
}
