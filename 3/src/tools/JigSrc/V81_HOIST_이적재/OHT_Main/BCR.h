/**
@file    BCR.h
@version OHT 7.0
@brief   BCR Class Header File
*/

#ifndef BCRH
#define BCRH

#include "VirtualIdReader.h"
#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

#include "LogHandler.h" // Log �ۼ� ����

/**
@class   BCR
@date    
@author  
@brief   BCR ���� Class
*/
class BCR: public VirtualIdReader
{
private:
	LogUnit *pLogBCRRaw;      ///< BCR Raw �α� ������ ���� ����

    Comm_Port *m_pCommPort;
    HWCommon *m_pHWCommon;

    int m_nPortNum;
    int m_nIndex;
	
	int m_nLatestID;
	int* m_pnResultID;

	//����
	int m_nRetryCount;
	int m_nMovingCount;
	CRITICAL_SECTION m_MovCntCS;

	int nPreID;
	bool ReadCheck;


public:
    BCR(HWCommon *pHWCommon, int nLeftRight);
    ~BCR();
    
	bool IsBCROK();
    bool IsBCRNG();
    
	int GetID(int *nID);
	int GetID(int *pnID, bool *bCheck);  //AOHC-249 BCR Read Sequence ����

	int SetResultPointer(int* pnResultID, ...);
	
	int SendTriggerOn();
	int SendTriggerOff();
	int ReadBuffer();
	int	GetLatestID();
	
	
	int Enable();
	void Disable();
	int ReStart();
    bool isEnable();		//AOHC-249 BCR Read Sequence ����
    //����
	int GetMovingCount(bool bInit);
	int GetRetryCount();
	void ResetRetryCount();
	void IncreaseRetryCount();

};

#endif  //BCRH
