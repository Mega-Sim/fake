//---------------------------------------------------------------------------

#ifndef LINIFILEH
#define LINIFILEH
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>	// _MAX_PATH, _fullpath
#include <string.h>
#include <vector>
#include "LogHandler.h"

//#include <IniFiles.hpp>
//#include "afxwin.h"

 /**
@brief   LIniFile Ŭ����
@author  puting
@date    2013.04.03
*/
class LIniFile
{

private:
	LogUnit *m_pLogINI;
	char m_szIniFileName[_MAX_PATH];    ///< ini file Path ����
	bool  m_bIniFileExist; 	  ///< ini file ���� ���� check

	char m_szTmpbuffer[256];    ///< �ӽ� ��Ʈ�� ����
	char m_szTmpValue[256];     ///< �ӽ� �� ����


public:
	LIniFile(char* szIniFileName);
	~LIniFile();

	bool GetIniFileExist();   ///< file ���� ���� Check!! �ʱ⿡ �ݵ�� Check

    //
	void DeleteIniFile();
	void CreateFileofWrite();

	//���� ���� �Լ�.
	bool WriteInteger(char* szSection, char* szIdent, int nValue);
	bool WriteDouble(char* szSection, char* szIdent, double dValue);
	bool WriteString(char* szSection, char* szIdent, char* szValue);
	bool WriteBool(char* szSection, char* szIdent, bool bValue);

	bool WriteDiffInteger(char* szSection, char* szIdent, int nValue, int nDefaultValue);
	bool WriteDiffDouble(char* szSection, char* szIdent, double dValue, double dDefaultValue);
	bool WriteDiffString(char* szSection, char* szIdent, char* szValue, char* szDefaultValue);
	bool WriteDiffBool(char* szSection, char* szIdent, bool bValue, bool bDefaultValue);

	//�б� ���� �Լ�
	bool ReadInteger(char* szSection, char* szIdent, int nDefaultValue, int* pnValue);
	bool ReadIntegerMinCheck(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int* pnValue);
	bool ReadIntegerMaxCheck(char* szSection, char* szIdent, int nDefaultValue, int nMaxValue, int* pnValue);
	bool ReadIntegerMinMaxCheck(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int nMaxValue, int* pnValue);
	
	bool ReadDouble(char* szSection, char* szIdent, double dDefaultValue, double* pdValue);
	bool ReadDoubleMinCheck(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double* pdValue);
	bool ReadDoubleMaxCheck(char* szSection, char* szIdent, double dDefaultValue, double dMaxValue, double* pdValue);
	bool ReadDoubleMinMaxCheck(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double dMaxValue, double* pdValue);
	
	bool ReadString(char* szSection, char* szIdent, char* szdefualtValue, char* szValue);
	bool ReadBool(char* szSection, char* szIdent, bool bdefualtValue, bool* bValue);

	//puting �Ķ���� ���ռ� �ڷ� �߰�
	bool ReadDoubleMinMaxVerification(char* szSection, char* szIdent, double dDefaultValue, double dMinValue, double dMaxValue, double* pdValue);
	bool ReadBoolVerification(char* szSection, char* szIdent, bool bdefualtValue, bool* bValue);
   	bool ReadIntegerMinMaxVerification(char* szSection, char* szIdent, int nDefaultValue, int nMinValue, int nMaxValue, int* pnValue);

	std::vector<std::string> GetSectionNames();
	std::vector<std::string> GetKeysInSection(char* szSection);
};
#endif
