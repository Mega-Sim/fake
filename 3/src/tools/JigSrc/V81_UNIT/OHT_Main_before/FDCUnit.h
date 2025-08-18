#ifndef FDCUnitH
#define FDCUnitH


#include <stdio.h>
#include <windows.h>
#include "List.h"
#include "../Common/PUtily.h"
//#include "FDCManager.h"
//#include "OHTMainStatus.h"

typedef struct _FDC_PACKET
{

	int		nLength;
	int 	nTransmit_Count;
	int 	nErrorcode;

	SYSTEMTIME LocalTime;
	DWORD 	dwTransmitTime;

	char	*pcValue;
}FDC_PACKET;


class FDCUnit
{
private:
	char 			*m_szFDCName;

	LList 			m_FDC_List;
	LList 			m_WARN_List;

	unsigned int 	m_nMaxSizeOfFDC;		///< Log List�� ���尡���� Char(BYTE)�� �ѷ�(Length, Time ���� ����)
	unsigned int 	m_nSize;				///< Log List�� ����� char(BYTE)�� �ѷ�(Length, Time ���� ����)

	bool 		    m_bIsAutoMode;

	int 			m_nOCSErrorcode;

	unsigned int 	m_nCountOfLostLog;

	bool			m_bUseFDC;

//	OHTMainStatus 	*m_pOHTMainStatus;
public:
	FDCUnit(char *szFDCName, unsigned int nMaxSizeOfFDC);
	virtual ~FDCUnit(void);
	bool 	FDC_put(int nLength, char *sFDC);  ///< FDC Log�� �Է��ϴ� �Լ�(nLength: sLog����, dwDrivingTime:�ۼ��ð�, sLog: �α�, nError: ����) / �����ϸ� true ����
    bool 	FDC_put_WARN(int nLength, char *sFDC);  ///< FDC Log�� �Է��ϴ� �Լ�(nLength: sLog����, dwDrivingTime:�ۼ��ð�, sLog: �α�, nError: ����) / �����ϸ� true ����
	void 	Transmitted(FDC_PACKET *pFDCPacket);
	void	Transmit_Insert(FDC_PACKET *pFDCPacket);
	int 	Transmit_Count(FDC_PACKET *pFDCPacket);
	void    Transmit_pop(FDC_PACKET *pFDCPacket, unsigned int nIndex);

	int 	ExtensionData(int nLength, char *pFDC);

	void	changeAutoMode();
	void	changeManualMode(int error);
	void 	changeUseMode();
	void	changeNoUseMode();

	bool	IsAuto();

	bool  	CheckTransmitted(int *preErrorcode, char *pFDC, SYSTEMTIME *nPreErrorcodeTime, int *Errorcode, bool *bIsWARN);

	int 	getOCSErrorcode();

	void	WARN_Insert();

	bool	Check_WARN(FDC_PACKET *pFDCPacket);

	int 	getFDCSize();
	int 	getFDCListSize();

protected:
	bool	FDC_get(LList *pList, int *nError, int*pnCountOfLostLog);

	friend class FDCManager;
};







#endif
