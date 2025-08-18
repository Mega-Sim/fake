//---------------------------------------------------------------------------


#ifndef FDCManagerH
#define FDCManagerH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <map>
#include <string>
#include <vcl.h>


#include "../Common/UdpClient.h"
#include "LIniFile.h"
#include "List.h"
#include "FDCUnit.h"

/**
@brief	 �̻��� ����
@author  SEMES MoonHS
@date
*/
typedef struct FDC_PARAM_
{
	char    sIPAddress[16];  	    ///< �̻��� ������ �����ϴ� IP Address
	char    sOHTIPAddress[16];  	///< OHT�� IP Address
	int  	nPortNo;				///< �̻��� ������ �����ϴ� Port No

	char    sFDC_OHT_Type[16];       //< OHT�� ���� ex)FOUP, MGZ
	char    sFDC_OHT_No[16];     	//< OHT�� ȣ�� ��ȣ
	char	sFDC_HW_Version[16];     //< OHT�� HW ���� ex)OHT70, OHT 65

	int 	nFDC_Retry_Count_Max;		//< FDC�� �ִ� ������ Ƚ��
	int 	nFDC_Retry_Time;			//< FDC�� ������ �ֱ�

	int 	nFDC_WARN_Send_Delay;			//< FDC�� ������ �ֱ�
	bool	bUseFDC;

}FDC_PARAM;

/**
@brief	 �̻��� Parameter
@author  SEMES MoonHS
@date
*/
struct FDC_PARAM_SET
{

	FDC_PARAM     FDC_Param;
};


class FDCUnit;

class FDCManager
{
private:
	CRITICAL_SECTION	m_CS;						///< LogManager�� LogUnit�� ó���� ���� LogUnit class�� �����ϴ� �κа��� �浹�� �������� Critical Section
	LList				m_FDC_List;
	unsigned int		m_nMaxByteSizeOfFDCSum;
	unsigned int		m_nByteSizeOfFDCSum;
	bool 				m_checkFDCFolder;
	bool				m_bFDCUse;

	std::map<std::string, FDCUnit*> m_FDCUnitMap;

	UdpClient 			*m_UdpClient;

public:
	FDCManager(unsigned int nMaxByteSizeOfFDCSum, bool checkFDCFolder);	///< ������(nTag:�ν���, sLogName:�α� �̸�, nMaxByteSizeOfLogSum: Log���� �ִ� Size~Byte)
	virtual ~FDCManager(void);

	void ChangeMode();

	bool	connectFDCUnit(FDCUnit *pFDCUnit);

    FDCUnit*  getFDCUnit(char *szSavePath, unsigned int nMaxSizeOfFDC);

	bool handleFDCSum();

	bool Get_FDCFolder();

	bool handleFDC_Transmit_Fun();
	bool FDC_Socket_Connection();
	bool FDC_Check_Transmit(char *Msg,char *szTemp, int *msgLength);
	bool AddDrivingInfo(char *Msg,int nLength,char *rtnMSG);

	bool LoadParameter(char* szFilePath, FDC_PARAM_SET* pParamSet);	///< �Ķ���� �ҷ����� �Լ�
	bool SaveParameter(char* szFilePath, FDC_PARAM_SET* pParamSet);	///< �Ķ���� �����ϴ� �Լ�

	bool FDC_Transmit_Snd(FDCUnit *pFDCUnit);
	bool FDC_Transmit_Rcv(FDCUnit *pFDCUnit, char *MSG);

	FDC_PARAM_SET *m_pParamSet;
	FDC_PARAM_SET *m_pTmpParamSet;

private:
	bool    updateParameter();                                      ///< �Ķ���� ������Ʈ �ϴ� �Լ�

protected:
	friend class FDCUnit;
};


#endif
