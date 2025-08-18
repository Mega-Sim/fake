/**
@file    QR_Reader.h
@version OHT 7.0
@brief   QR_Reader Class Header File
*/

#ifndef QR_ReaderH
#define QR_ReaderH

//#include "VirtualIdReader.h"
#include "VirtualIdAndOffsetReader.h"
#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

typedef enum QR_MAKER_
{
	QR_MAKER_KEYENCE   			= 0,	///< Ű�ؽ��� ����
	QR_MAKER_OMRON 				= 1,	///< �ȷл� ����
	QR_MAKER_DM50 				= 2,	///< �ڱ״н��� ����
	QR_MAKER_MS 				= 3,	///< ����ũ�� ��ĵ ����
	QR_MAKER_DM50_DECODETIME    = 4		///< �ڱ״н��� ���� (Decode Time ȹ��)
}QR_MAKER;

/**
@class   QR_Reader
@date
@author
@brief   QR_Reader ���� Class
*/
class QR_Reader: public VirtualIdAndOffsetReader
{
private:

	LogUnit *pLogQRTest;      ///< QRTest �α� ������ ���� ����
    Comm_Port *m_pCommPort;
    HWCommon *m_pHWCommon;
    int m_nPortNum;
	//int m_nIndex;
    int m_nDirection;

	int m_nLatestID;
	double m_dLatestOffset;

	int* m_pnResultID;
	double* m_pdResultOffset;
	int m_MakerType;

	DWORD dChecked62;
	bool b_Checked62;
	char m_szTempReadBuff[1024]; // AOHC-206 QR Class Static ���� �ߺ� ����
     int nSetupVersion;
	 int nMinDecodeTime;
	 int nMaxDecodeTime;
	 int nSumDecodeTime;
	 double dAvgDecodeTime;
	 int nCntDecodeTime;

	 int m_nTriggerCount;

	 CRITICAL_SECTION m_MovCntCS;

public:
    QR_Reader(HWCommon *pHWCommon, int nComPortNum, bool nDirectionPlus);
    ~QR_Reader();

	int SetQRMaker(int nMakerType);
	int GetIDnOffset(int *nID, double *nOffset);
	int ParseData(char* szData, int *nID, double *nOffset, int *nDecodeTime, int *nSetupVersion);

	int SetResultPointer(int* pnResultID, ...);

	int SendTriggerOn();
	int SendTriggerOff();
	int ReadBuffer();
	int	GetLatestID();
	double	GetLatestOffset();


	//QRData Sync�� ���� �κ�(Map maker ��)
	void SetQRDataSync(bool bsync);
	bool IsQRDataSync();
	int  GetQRDataSync();

	bool m_bQRDatasync;
	int m_QRDatasyncIndex;
	DWORD m_QRDataSynctimeout;
	/////////

	///static QRData Ȯ���� ���� �κ�(AUto Teaching ��)
	void SetStaticQRDataTrigger(bool bsync);
	int GetStaticQRID();
	double GetStaticQROffset();
	bool m_bStaticQRData;
	int m_nStQRID;
	double m_dstQROffset;


	int Enable();
	void Disable();
	int ReStart();
	bool Connect();
	bool m_bflag;
    int CheckDecodeTime(int nDecodeTime);
	int GetMaxDecodeTime();
	int GetMinDecodeTime();
	double GetAvgDecodeTime();
	int GetSetupVersion();
	int ResetDecodeTime();

    int GetTriggerCount(bool bInit);

};

#endif  // QR_ReaderH

