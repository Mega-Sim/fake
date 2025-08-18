/**
@file    HWCommon.cpp
@version OHT 7.0
@brief   HWCommon Class CPP File
*/

#include "HWCommon.h"
#include "LogHandler.h"
#include "OHTDetect.h"
#include "DiagManager.h"
#include "OHTMain.h"

#define ERR_NO_REAL_HW	-1
#define ADD_MD_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\MonitoringDetail.log", __VA_ARGS__) //AOHC-253 Monitoring �׸� �߰� 2

/**
 @brief   HWCommon Class ������
 @author  //
 @date    //
 @return  //
 */
HWCommon::HWCommon()
{
	m_nAmcErrCode = 0;
	Open();
	UpdateData();	// 2013.10.16, ���¿�, ���� �� H/W ������ �޾ƿ��µ� ������ ����.
}

/**
 @brief   HWCommon Class �Ҹ���
 @author  //
 @date    //
 @return  //
 */
HWCommon::~HWCommon() 
{
	Close();
}


/**
 @brief   HWCommon Open �ϴ� �Լ�
 @author	���¿�
 @date	2013.02.17
 @return	H/W Mask class�� ������ 
 */
HwMask* HWCommon::GetMask(void)
{
	HwMask *pMask = NULL;
	if(m_pMask!=NULL)
	{
		pMask = m_pMask;
	}
	return pMask;
}

//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
bool HWCommon::IsSerialOpen()
{
 	return m_pMultiSerialLib->g_bSerialDriverOpen;
}

/**
 @brief   HWCommon Open �ϴ� �Լ�
 @author  //
 @date    //
 @return  //
 */
int HWCommon::Open() 
{
	//Memory Leak������ �߰�
    m_pMotionController = NULL;
	if (m_pMotionController != NULL) {
		delete m_pMotionController;
		// m_pMotionController == NULL �ΰ�� InitializeCriticalSection(&m_CS);�� �ȵ� ���¿���
		// CriticalSection�� ����ϸ鼭 ������.
	}
	m_pMotionController = new AMCLibrary();
	m_pMultiSerialLib = new MultiSerialLibrary();
#if((OHT_NAME == OHT_NAME_STD_V80_RETICLE) || (OHT_NAME == OHT_NAME_STD_V85))
	m_pMask = new HwMask(96, 96, 5);
#else
	m_pMask = new HwMask(96, 96, 4);
#endif
	char szCOM[16] = "";

	return NO_ERR;
}

/**
 @brief   HWCommon �ݴ� �Լ�
 @author  //
 @date    //
 @return  ���� //
 */
int HWCommon::Close() 
{

	if (m_pMask != NULL) {
		delete m_pMask;
		m_pMask = NULL;
	}

	if (m_pMultiSerialLib != NULL) {
		delete m_pMultiSerialLib;
		m_pMultiSerialLib = NULL;
	}

	if (m_pMotionController != NULL) {
		m_pMotionController->AMCClose();
		delete m_pMotionController;
		m_pMotionController = NULL;
	}

	return NO_ERR;
}

/**
 @brief   IO�� �ش� Output�� On/Off ��ų �� ����ϴ� �Լ�
 @author  ehoto
 @date    2012.10.29
 @param   index : DEF_HW.h�� ������ ENUM ���� OUTPUT ���(On/Off �ϰ��� �ϴ� ��ȣ ����)
 @param   OnOff : �ش� ����� On �Ϸ��� True, Off �Ϸ��� false.
 @return	 ���ۿ� ���� ������ 0 ����, ���� �߻� �� �ش� ErrorCode ����.
 */
int HWCommon::SetBit(int nIndex, bool bOnOff, bool bOBSControl) {
	int nError = 0;

	if(nIndex > NUMBER_OF_OUTPUT) return nError;  // ignore over NUMBER_OF_OUTPUT Outputs   OHT_NAME_MASK:72

	nError = m_pMotionController->SetBit(nIndex, bOnOff, bOBSControl);

	return nError;
}

/**
 @brief   HWCommon�� MotionController �ʱ�ȭ �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::Init(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->Init(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController �ʱ�ȭ �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::SetSync(int nMasterAxisNum, int nSlaveAxisNum, double dSyncGain) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SetSync(nMasterAxisNum, nSlaveAxisNum,
		dSyncGain);
}


/**
 @brief   HWCommon�� MotionController �ʱ�ȭ �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::SetMaxSWLimitForReset(int nAxisNum, double dPosLimit,
	double dNegLimit) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SetMaxSWLimitForReset(nAxisNum, dPosLimit,
		dNegLimit);
}

/// ���� Ȯ��
/**
 @brief   HWCommon�� MotionController ���� Ȯ�� �ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
bool HWCommon::IsRunning(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return false;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Running.bUse==true)
		{
			return pMaskOfAxis->Running.Value;
		}
	}

	return m_pMotionController->IsRunning(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController ����ġ Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
bool HWCommon::IsInPosition(int nAxisNum) {
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->IsInPosition(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController AMP On/Off ���� Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ(0~)
 @return  Error Code
 */
bool HWCommon::IsAMPEnable(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return false;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->AmpEnable.bUse==true)
		{
			return pMaskOfAxis->AmpEnable.Value;
		}
	}

	return m_pMotionController->IsAMPEnable(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController ���� ��ġ Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
double HWCommon::GetCurrentPosition(int nAxisNum) 
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetCurrentPosition(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController ���� ���� ��ġ Ȯ�� �Լ�
 @author  doori.shin
 @date    2016.04.26
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
double HWCommon::GetPrePosition(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetPrePosition(nAxisNum);
}

/**
@brief   HWCommon�� MotionController ������� ������ �Ÿ��� ����
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ
@return  Error Code
*/
double HWCommon::GetTotalPosition(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);

	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetTotalPosition(nAxisNum);
}

/**
@brief   HWCommon�� MotionController ������� ������ �Ÿ��� �ʱ�ȭ
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ
*/
void HWCommon::ResetTotalPosition(int nAxisNum)
{
	if(m_pMotionController != NULL)
		m_pMotionController->ResetTotalPosition(nAxisNum);
}

/**
@brief   HWCommon�� MotionController ������� ������ ��� Ƚ���� ����
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ
@return  Error Code
*/
int HWCommon::GetTotalMoveCount(int nAxisNum)
{
	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);

	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->Position.bUse==true)
		{
			return pMaskOfAxis->Position.Value;
		}
	}

	return m_pMotionController->GetTotalMoveCount(nAxisNum);
}

/**
@brief   HWCommon�� MotionController ������� ������ ���Ƚ�� �ʱ�ȭ
@author  doori.shin
@date    2016.04.26
@param   nAxisNum : �� ��ȣ
*/
void HWCommon::ResetTotalMoveCount(int nAxisNum)
{
	if(m_pMotionController != NULL)
		m_pMotionController->ResetTotalMoveCount(nAxisNum);
}


/**
 @brief   HWCommon�� MotionController Small Add Move �� ������ ������ ��ġ Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Small Add Move �� ������ ������ ��ġ
 */
double HWCommon::GetSmallAddCheckPosition(int nAxisNum)
{
//	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetSmallAddCheckPosition(nAxisNum);
}


/**
 @brief   HWCommon�� MotionController ���� ��ġ Ȯ�� �Լ�
 @author  ehoto
 @date	  2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Small Add Move �� ������ ������ ������ ǥ����
 */
double HWCommon::GetFinalPosition(int nAxisNum)
{
//	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetFinalPosition(nAxisNum);
}


/**
 @brief   HWCommon�� MotionController ���� �ӵ� Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
double HWCommon::GetCurrentVelocity(int nAxisNum) {
	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetCurrentVelocity(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController Following Error Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
double HWCommon::GetFollowingError(int nAxisNum) {
	if (m_pMotionController == NULL)
		return 0.0;

	return m_pMotionController->GetFollowingError(nAxisNum);
}

/// ���� ����
/**
 @brief   HWCommon�� MotionController Error Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::GetError(int nAxisNum) 
{
	if(m_nAmcErrCode != 0)	// AMC ���� ���� ���� ����
		return m_nAmcErrCode;

	MASK_AXIS	*pMaskOfAxis;

	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	pMaskOfAxis = m_pMask->getMaskOfAxis(nAxisNum);
	if(pMaskOfAxis!=NULL)
	{
		if(pMaskOfAxis->ErrorCode.bUse==true)
		{
			return pMaskOfAxis->ErrorCode.Value;
		}
	}

	return m_pMotionController->GetError(nAxisNum);
}
//AOHC-232 ���� ���� ��, �ڵ� Auto ��ȯ ��� ����
bool HWCommon::IsAMCDllOpen()
{
	return m_pMotionController->IsAMCDllOpen();
}
/**
 @brief   HWCommon�� MotionController reset �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::Reset(int nAxisNum)
{
	m_nAmcErrCode = 0;

	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->Reset(nAxisNum);
}

/// Offset ����
/**
 @brief   HWCommon�� MotionController ���� ��ġ Home ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::SetHome(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->SetHome(nAxisNum);
}

/**
 @brief   HWCommon�� MotionController Offset ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dDistance : Offset �Ÿ�
 @return  Error Code
 */
int HWCommon::SetOffset(int nAxisNum, double dDistance) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetOffset(nAxisNum, dDistance);
}

/**
 @brief   HWCommon�� MotionController Offset �޾ƿ��� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dDistance : Offset �Ÿ�
 @return  Error Code
 */
int HWCommon::GetOffset(int nAxisNum, double *dDistance) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->GetOffset(nAxisNum, dDistance);
}

/// Limit ����
/**
 @brief   HWCommon�� MotionController Postive S/W Limit ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dLimit : Limit �Ÿ�
 @return  Error Code
 */
int HWCommon::SetPositiveSWLimit(int nAxisNum, double dLimit) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetPositiveSWLimit(nAxisNum, dLimit);
}

/**
 @brief   HWCommon�� MotionController Negative S/W Limit ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dLimit : Limit �Ÿ�
 @return  Error Code
 */
int HWCommon::SetNegativeSWLimit(int nAxisNum, double dLimit) {
	if (m_pMotionController == NULL)
		return -1;

	return m_pMotionController->SetNegativeSWLimit(nAxisNum, dLimit);
}

/**
 @brief   HWCommon�� MotionController S/W Limit Ȯ�� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dPosLimit : Positive Limit �Ÿ�
 @param   dNegLimit : Negative Limit �Ÿ�
 @return  Error Code
 */
int HWCommon::GetSWLimit(int nAxisNum, double& dPosLimit, double& dNegLimit) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->GetSWLimit(nAxisNum, dPosLimit, dNegLimit);
}

/**
 @brief   HWCommon�� MotionController S/W Limit ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @return  Error Code
 */
int HWCommon::ResetSWLimit(int nAxisNum) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->ResetSWLimit(nAxisNum);
}

/// ���� ���� - ����: �Ÿ�(mm), �ӵ�(mm/msec), ���ӵ�(mm/msec2), �ð�(msec)
/**
 @brief   HWCommon�� MotionController Amp On/Off ���� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   bValue : On/Off ��
 @return  Error Code
 */
int HWCommon::SetAMPEnable(int nAxisNum, bool bValue) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->SetAMPEnable(nAxisNum, bValue);
}

// ���ӵ� ���
/**
 @brief   HWCommon�� MotionController ���� �������� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dAccel : ���ӵ�
 @param   dVel : �ӵ�
 @return  Error Code
 */
int HWCommon::MovePositive(int nAxisNum, double dAccel, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MovePositive(nAxisNum, dAccel, dVel);
}

/**
 @brief   HWCommon�� MotionController ���� �������� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dAccel : ���ӵ�
 @param   dVel : �ӵ�
 @return  Error Code
 */
int HWCommon::MoveNegative(int nAxisNum, double dAccel, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveNegative(nAxisNum, dAccel, dVel);
}

/**
 @brief   HWCommon�� MotionController �����ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dAccel : ���ӵ�
 @return  Error Code
 */
int HWCommon::MoveStop(int nAxisNum, double dAccel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveStop(nAxisNum, dAccel);
}

/**
 @brief   HWCommon�� MotionController �Ÿ��� ���ӵ�/���ӵ�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dAccel_Up : ���ӵ�
 @param   dAccel_Down : ���ӵ�
 @param   dVel : �ӵ�
 @param   dDistance : �̵� �Ÿ�
 @return  Error Code
 */
int HWCommon::MoveDistance(int nAxisNum, double dAccel_Up, double dAccel_Down,
	double dVel, double dDistance, TRIGGER_CTRL_INFO TriggerCtrlInfo) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveDistance(nAxisNum, dAccel_Up, dAccel_Down,
		dVel, dDistance, TriggerCtrlInfo);
}

// ���� �ð� ���
/**
 @brief   HWCommon�� MotionController ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dwTime : ���� �ð�
 @param   dVel : �ӵ�
 @return  Error Code
 */
int HWCommon::MovePositive(int nAxisNum, DWORD dwTime, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MovePositive(nAxisNum, dwTime, dVel);
}

/**
 @brief   HWCommon�� MotionController ���� �ð� �����Ͽ� ���� �������� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dwTime : ���� �ð�
 @param   dVel : �ӵ�
 @return  Error Code
 */
int HWCommon::MoveNegative(int nAxisNum, DWORD dwTime, double dVel) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveNegative(nAxisNum, dwTime, dVel);
}

/**
 @brief   HWCommon�� MotionController ���� �ð� �����Ͽ� �����ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dwTime : ���� �ð�
 @param   dVel : �ӵ�
 @return  Error Code
 */
int HWCommon::MoveStop(int nAxisNum, DWORD dwTime) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveStop(nAxisNum, dwTime);
}

/**
 @brief   HWCommon�� MotionController �Ÿ��� ���ӽð�/���ӽð�/�ӵ� �������Ϸ� �̵��ϴ� �Լ�
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   dwTime_Up : ���� �ð�
 @param   dwTime_Down : ���� �ð�
 @param   dVel : �ӵ�
 @param   dDistance : �̵� �Ÿ�
 @return  Error Code
 */
int HWCommon::MoveDistance(int nAxisNum, DWORD dwTime_Up, DWORD dwTime_Down,
	double dVel, double dDistance) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;

	return m_pMotionController->MoveDistance(nAxisNum, dwTime_Up, dwTime_Down,
		dVel, dDistance);
}

// ������� ����ÿ��� ���� �� ��� �Լ�
/**
 @brief   HWCommon�� MotionController �� �������ݷ� ��� ����
 @author  ehoto
 @date    2012-11-01
 @param   nAxisNum : �� ��ȣ
 @param   pSendData : ������ Data �� ����ִ� Char �迭
 @param   nSendLen : ������ Data ����
 @param   pRecvData : �޴� Data �� ����ִ� Char �迭
 @param   nRecvLen : �޴� Data ����
 @return  Error Code
 */
int HWCommon::SendData(int nAxisNum, char* pSendData, int nSendLen,
	char* pRecvData, int* nRecvLen) {
	if (m_pMotionController == NULL)
		return ERR_NO_REAL_HW;
	return m_pMotionController->SendData(nAxisNum, pSendData, nSendLen,
		pRecvData, nRecvLen);
}


// ������� ����ÿ��� ���� �� ��� �Լ�
/**
 @brief   HWCommon�� MotionController �� ���� Update
 @author  ehoto
 @date    2012-11-07
 @return  Error Code
 */
void HWCommon::UpdateData(void)
{
	int nInput[4] = {0, }, nOutput[4] = {0, };
	int nInputCount = 0, nOutCount = 0;
	unsigned int nInData[4][32] = {0, };
	unsigned int nOutData[4][32] = {0, };

	MASK_INPUT	*pMaskOfInput = NULL;
    
	if (m_pMotionController == NULL)
		return;

	int nErrCode = 0;
	bool bResult = m_pMotionController->UpdateData(nInput, nInputCount, nOutput, nOutCount, &nErrCode);

	// AMC�� UpdateData �����ϴ� ���� AMC���� Invalidate ��Ŷ�� ���� ����̴�.
	// �̷� ��� ���� ��Ŷ�� ������ �ϱ� ������ IO �������� �ʰ� return �Ѵ�.
	if(bResult == false)
	{
		if(nErrCode != 0)
			m_nAmcErrCode = nErrCode;
		return;
    }



	// Test
//	static nIndex = 0;
//	bool bOnOff = false;
//	bOnOff = (rand() % 2);
//	m_pMotionController->SetBit(nIndex, bOnOff);
//	nIndex++;
//	if (nIndex == 64) {
//		nIndex = 0;
//	}

	for (int i = 0; i < nInputCount; i++)
	{
		for (int j = 0; j < 32; j++)
		{

			nInData[i][j] = (nInput[i] >> j) & 0x01;

			// Value Masking
			pMaskOfInput = m_pMask->getMaskOfInput( (i*32+j) );
			if(pMaskOfInput!=NULL)
			{
				if(pMaskOfInput->bUse==true)
				{
					nInData[i][j] = (pMaskOfInput->Value==true)?ON:OFF;
				}
			}
//			pMaskOfInput = NULL;
		}
	}

#if(OHT_NAME == OHT_NAME_STD_V80)
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];

	m_Input.HOIST_Home = nInData[0][7];

	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.HOIST_Belt_Detect2 = nInData[1][15];
	m_Input.TP_Enable = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.FOUP_Cover_Open_Detect = nInData[1][18];
	m_Input.STOP_Driving_Front = nInData[1][19];
	m_Input.STOP_Driving_PreCheck = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.HOIST_Belt_Detect1 = nInData[1][27];
	m_Input.PIO_Input3 = nInData[1][28];
	m_Input.PIO_Input5 = nInData[1][29];
	m_Input.PIO_Input6 = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
	//TG �߰�
	m_Input.CID_Link_Compt_IN = nInData[2][0];
	m_Input.CID_Interlock_IN = nInData[2][1];
//	m_Input.CID_Pass_OK_IN = nInData[2][2];
	m_Input.CID_Completion_ACK = nInData[2][2];
	m_Input.CID_Status_IN = nInData[2][3];
	m_Input.CID_Reserved_IN1 = nInData[2][4];
	m_Input.CID_Reserved_IN2 = nInData[2][5];
	m_Input.Rotate_0_Sensor = nInData[2][6];
	m_Input.Rotate_90_Sensor = nInData[2][7];
	m_Input.Rotate_180_Sensor = nInData[2][8];
	m_Input.Rotate_270_Sensor = nInData[2][9];

	m_Input.Reverved_74 = nInData[2][10];

	m_Input.Reverved_75 = nInData[2][11];
	m_Input.Reverved_76 = nInData[2][12];
	m_Input.Reverved_77 = nInData[2][13];
	m_Input.Reverved_78 = nInData[2][14];
	m_Input.Reverved_79 = nInData[2][15];

#elif(OHT_NAME == OHT_NAME_STD_V81)
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];

	m_Input.HOIST_Home = nInData[0][7];

	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.HOIST_Belt_Detect2 = nInData[1][15];
	m_Input.TP_Enable = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.FOUP_Cover_Open_Detect = nInData[1][18];
	m_Input.STOP_Driving_Front = nInData[1][19];
	m_Input.STOP_Driving_PreCheck = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.HOIST_Belt_Detect1 = nInData[1][27];
	m_Input.PIO_Input3 = nInData[1][28];
	m_Input.PIO_Input5 = nInData[1][29];
	m_Input.PIO_Input6 = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
	//TG �߰�
	m_Input.CID_Link_Compt_IN = nInData[2][0];
	m_Input.CID_Interlock_IN = nInData[2][1];
//	m_Input.CID_Pass_OK_IN = nInData[2][2];
	m_Input.CID_Completion_ACK = nInData[2][2];
	m_Input.CID_Status_IN = nInData[2][3];
	m_Input.CID_Reserved_IN1 = nInData[2][4];
	m_Input.CID_Reserved_IN2 = nInData[2][5];
	m_Input.Rotate_0_Sensor = nInData[2][6];
	m_Input.Rotate_90_Sensor = nInData[2][7];
	m_Input.Rotate_180_Sensor = nInData[2][8];
	m_Input.Rotate_270_Sensor = nInData[2][9];

	m_Input.Reverved_74 = nInData[2][10];

	m_Input.Reverved_75 = nInData[2][11];
	m_Input.Reverved_76 = nInData[2][12];
	m_Input.Reverved_77 = nInData[2][13];
	m_Input.Reverved_78 = nInData[2][14];
	m_Input.Reverved_79 = nInData[2][15];

#elif ((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))	
	{
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm = nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip = nInData[0][3];
		m_Input.HAND_Close = nInData[0][4];
		m_Input.HAND_Open = nInData[0][5];
		m_Input.HAND_Push = nInData[0][6];
	    m_Input.Hand_Reserved7 = nInData[0][7];
		m_Input.HAND_PIO_Go = nInData[0][8];
		m_Input.EQPIO_Load_REQ = nInData[0][9];
		m_Input.EQPIO_Unload_REQ = nInData[0][10];
		m_Input.EQPIO_Ready = nInData[0][11];
		m_Input.EQPIO_HO_AVBL = nInData[0][12];
		m_Input.EQPIO_ES = nInData[0][13];
		m_Input.EQPIO_Right_Go = nInData[0][14];
		m_Input.EQPIO_Left_Go = nInData[0][15];
		m_Input.STEER_F_Left = nInData[0][16];
		m_Input.STEER_R_Left = nInData[0][17];
		m_Input.STEER_F_Right = nInData[0][18];
		m_Input.STEER_R_Right = nInData[0][19];
		m_Input.SHUTTER_F_Close = nInData[0][20];
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open = nInData[0][22];
		m_Input.SHUTTER_R_Open = nInData[0][23];
		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition = nInData[0][26];
		m_Input.SHUTTER_R_Inposition = nInData[0][27];
		m_Input.OBSTACLE_Long = nInData[0][28];
		m_Input.OBSTACLE_Middle = nInData[0][29];
		m_Input.OBSTACLE_Short = nInData[0][30];
		m_Input.OBSTACLE_Fail = nInData[0][31];

		m_Input.OHTDETECT_Long = nInData[1][0];
		m_Input.OHTDETECT_Middle = nInData[1][1];
		m_Input.OHTDETECT_Short = nInData[1][2];
		m_Input.OHTDETECT_Fail = nInData[1][3];
		m_Input.LOOKDOWN_Detect = nInData[1][4];
		m_Input.LOOKDOWN_Error = nInData[1][5];
		m_Input.SHIFT_Home = nInData[1][6];
		m_Input.Reserved40 = nInData[1][7];
		m_Input.Reserved41 = nInData[1][8];
		m_Input.Reserved42 = nInData[1][9];
		m_Input.FOUP_Detect = nInData[1][10];
		m_Input.OSCILLATION = nInData[1][11];
		m_Input.EMO_Press = nInData[1][12];
		m_Input.BUMPER_Press = nInData[1][13];
		m_Input.FAN_Fail = nInData[1][14];
		m_Input.Reserved48 = nInData[1][15];
		m_Input.Reserved49 = nInData[1][16];
		m_Input.HANDTP_Connect = nInData[1][17];
		m_Input.Reserved51 = nInData[1][18];
		m_Input.STOP_Driving_Front = nInData[1][19];
		m_Input.STOP_Driving_PreCheck = nInData[1][20];
		m_Input.STOP_Trans_Front = nInData[1][21];
		m_Input.STOP_Trans_Rear = nInData[1][22];
		m_Input.BCR_Driving_OK = nInData[1][23];
		m_Input.BCR_Driving_NG = nInData[1][24];
		m_Input.BCR_Trans_OK = nInData[1][25];
		m_Input.BCR_Trans_NG = nInData[1][26];
		m_Input.HOIST_Belt_Detect1 = nInData[1][27];
		m_Input.PIO_Input3 = nInData[1][28];
		m_Input.PIO_Input5 = nInData[1][29];
		m_Input.PIO_Input6 = nInData[1][30];
		m_Input.SMPS_Short = nInData[1][31];
		//TG �߰�
		m_Input.CID_Link_Compt_IN = nInData[2][0];
		m_Input.CID_Interlock_IN = nInData[2][1];
		m_Input.CID_Completion_ACK = nInData[2][2];
		m_Input.CID_Status_IN = nInData[2][3];
		m_Input.CID_Reserved_IN1 = nInData[2][4];
		m_Input.CID_Reserved_IN2 = nInData[2][5];
		m_Input.Rotate_0_Sensor		= nInData[2][6];
		m_Input.Rotate_90_Sensor	= nInData[2][7];
		m_Input.Rotate_180_Sensor 	= nInData[2][8];
		m_Input.Rotate_270_Sensor 	= nInData[2][9];
		m_Input.HOIST_Home = nInData[2][10];
		m_Input.Rotate_Reserved76 = nInData[2][11];
		m_Input.Rotate_Reserved77 = nInData[2][12];
		m_Input.Rotate_Reserved78 = nInData[2][13];
		m_Input.Rotate_Reserved79 = nInData[2][14];
		m_Input.Rotate_Reserved80 = nInData[2][15];
	}

#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];
		
		m_Input.HAND_Close 		= nInData[0][4];		
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
	    m_Input.HOIST_Align 	= nInData[0][7];
		
		m_Input.HAND_PIO_Go 	= nInData[0][8];
		m_Input.EQPIO_Load_REQ 	= nInData[0][9];		
		m_Input.EQPIO_Unload_REQ= nInData[0][10];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][11];
		
		m_Input.EQPIO_HO_AVBL 	= nInData[0][12];
		m_Input.EQPIO_ES 		= nInData[0][13];
		m_Input.EQPIO_Right_Go 	= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];
		
		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];		
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25
		
		m_Input.SHUTTER_F_Alarm = nInData[0][24];		
		m_Input.SHUTTER_R_Alarm = nInData[0][25];		
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];	
		
		m_Input.OBSTACLE_Long 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][29];
		m_Input.OBSTACLE_Short 	= nInData[0][30];
		m_Input.OBSTACLE_Fail 	= nInData[0][31];	

		// 32 - 47	
		m_Input.OHTDETECT_Long 	= nInData[1][0];
		m_Input.OHTDETECT_Middle= nInData[1][1];		
		m_Input.OHTDETECT_Short = nInData[1][2];		
		m_Input.OHTDETECT_Fail 	= nInData[1][3];
		
		m_Input.LOOKDOWN_Detect = nInData[1][4];
		m_Input.LOOKDOWN_Error 	= nInData[1][5];		
		m_Input.SHIFT_Home 		= nInData[1][6];		
		m_Input.STB_Detect_Left 		= nInData[1][7];
		
		m_Input.STB_Detect_Right 		= nInData[1][8];
		m_Input.UTB_Detect 		= nInData[1][9];		
		m_Input.FOUP_Detect 	= nInData[1][10];		
		m_Input.OSCILLATION 	= nInData[1][11];	
		
		m_Input.EMO_Press 		= nInData[1][12];		
		m_Input.BUMPER_Press 	= nInData[1][13];
		m_Input.FAN_Fail 		= nInData[1][14];
		m_Input.HOIST_Belt 		= nInData[1][15];

		// 48 - 63		
		m_Input.TP_Enable 		= nInData[1][16];		// 50
		m_Input.HANDTP_Connect 	= nInData[1][17];		
		m_Input.FOUB_Cover_Detect 		= nInData[1][18];
		m_Input.STOP_Driving_Front 		= nInData[1][19];
		
		m_Input.STOP_Driving_PreCheck 	= nInData[1][20];		
		m_Input.STOP_Trans_Front= nInData[1][21];		
		m_Input.STOP_Trans_Rear = nInData[1][22];
		m_Input.BCR_Driving_OK 	= nInData[1][23];
		
		m_Input.BCR_Driving_NG 	= nInData[1][24];
		m_Input.BCR_Trans_OK 	= nInData[1][25];		
		m_Input.BCR_Trans_NG 		= nInData[1][26];	// 60
		m_Input.HOIST_Belt_Detect1 	= nInData[1][27];
		
		m_Input.PIO_Input3 			= nInData[1][28];
		m_Input.PIO_Input5 			= nInData[1][29];
		m_Input.PIO_Input6 			= nInData[1][30];
		m_Input.SMPS_Short 			= nInData[1][31];

		// 64 - 79	
		m_Input.CID_Link_Compt_IN 	= nInData[2][0];
		m_Input.CID_Interlock_IN 	= nInData[2][1];		
		m_Input.CID_Completion_ACK 	= nInData[2][2];
		m_Input.CID_Status_IN 		= nInData[2][3];
		
		m_Input.CID_Reserved_IN1 	= nInData[2][4];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][5];
		m_Input.Rotate_0_Sensor		= nInData[2][6];
		m_Input.Rotate_90_Sensor	= nInData[2][7];

		m_Input.Rotate_180_Sensor 	= nInData[2][8];
		m_Input.Rotate_270_Sensor 	= nInData[2][9];
		m_Input.HOIST_Home 			= nInData[2][10];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][11];
		
		m_Input.SHUTTER_R_Middle_Close = nInData[2][12];	
		m_Input.POD_EUV_1 			= nInData[2][13];
		m_Input.POD_EUV_2 			= nInData[2][14];
		m_Input.SPARE_5				= nInData[2][15];

		// 80 - 89
		m_Input.SPARE_6 			= nInData[2][16];
		m_Input.SPARE_7 			= nInData[2][17];
		m_Input.SPARE_8				= nInData[2][18];
		m_Input.SPARE_9				= nInData[2][19];
		
		m_Input.SPARE_10			= nInData[2][20];		
		m_Input.SPARE_11			= nInData[2][21];	// 85
		m_Input.SPARE_12			= nInData[2][22];
		m_Input.SPARE_13			= nInData[2][23];
		
		m_Input.SPARE_14			= nInData[2][24];
		m_Input.SPARE_15			= nInData[2][25];		
		m_Input.SPARE_16			= nInData[2][26];	// 90
		m_Input.SPARE_17			= nInData[2][27];
		
		m_Input.SPARE_18			= nInData[2][28];
		m_Input.SPARE_19			= nInData[2][29];
	}


#elif(OHT_NAME == OHT_NAME_STD_V85)
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];

		m_Input.HAND_Close 		= nInData[0][4];
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
		m_Input.HNAD_Reserved_1 	= nInData[0][7];

		m_Input.HNAD_ES 	= nInData[0][8];

		m_Input.HAND_PIO_Go 	= nInData[0][9];
		m_Input.EQPIO_Load_REQ 	= nInData[0][10];
		m_Input.EQPIO_Unload_REQ= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 	= nInData[0][13];
		m_Input.EQPIO_ES 		= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];

		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25

		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Long 	= nInData[0][29];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][30];
		m_Input.OBSTACLE_Short 	= nInData[0][31];
        // 32 - 47
		m_Input.OBSTACLE_Fail 	= nInData[1][0];
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][1];
		m_Input.OHTDETECT_Long 	= nInData[1][2];
		m_Input.OHTDETECT_Middle= nInData[1][3];
		m_Input.OHTDETECT_Short = nInData[1][4];
		m_Input.OHTDETECT_Fail 	= nInData[1][5];

		m_Input.LOOKDOWN_Detect = nInData[1][6];
		m_Input.LOOKDOWN_Error 	= nInData[1][7];
		m_Input.SHIFT_Home 		= nInData[1][8];
		m_Input.STB_Detect_Left 		= nInData[1][9];

		m_Input.STB_Detect_Right 		= nInData[1][10];
		m_Input.UTB_Detect 		= nInData[1][11];
		m_Input.FOUP_Detect 	= nInData[1][12];
		m_Input.OSCILLATION 	= nInData[1][13];

		m_Input.EMO_Press 		= nInData[1][14];
		m_Input.BUMPER_Press 	= nInData[1][15];
		m_Input.FAN_Fail 		= nInData[1][16];
		m_Input.HOIST_Belt_Detect1 		= nInData[1][17];
        m_Input.HOIST_Belt_Detect2 		= nInData[1][18];
		// 48 - 63
		m_Input.TP_Enable 		= nInData[1][19];		// 50
		m_Input.HANDTP_Connect 	= nInData[1][20];
		m_Input.FOUB_Cover_Detect 		= nInData[1][21];
		m_Input.STOP_Driving_Front 		= nInData[1][22];

		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];
		m_Input.STOP_Trans_Front= nInData[1][24];
		m_Input.STOP_Trans_Rear = nInData[1][25];
		m_Input.BCR_Driving_OK 	= nInData[1][26];

		m_Input.BCR_Driving_NG 	= nInData[1][27];
		m_Input.BCR_Trans_OK 	= nInData[1][28];
		m_Input.BCR_Trans_NG 		= nInData[1][29];	// 60

		m_Input.Reserved_1 			= nInData[1][30];
		m_Input.Reserved_2 			= nInData[1][31];

		// 64 - 79
		m_Input.Reserved_3 			= nInData[2][0];
		m_Input.SMPS_Short 			= nInData[2][1];


		m_Input.CID_Link_Compt_IN 	= nInData[2][2];
		m_Input.CID_Interlock_IN 	= nInData[2][3];
		m_Input.CID_Completion_ACK 	= nInData[2][4];
		m_Input.CID_Status_IN 		= nInData[2][5];

		m_Input.CID_Reserved_IN1 	= nInData[2][6];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][7];
		m_Input.Rotate_0_Sensor		= nInData[2][8];
		m_Input.Rotate_90_Sensor	= nInData[2][9];

		m_Input.Rotate_180_Sensor 	= nInData[2][10];
		m_Input.Rotate_270_Sensor 	= nInData[2][11];
		m_Input.HOIST_Home 			= nInData[2][12];	// 75
		m_Input.HOIST_Home_2 			= nInData[2][13];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][14];

		m_Input.SHUTTER_R_Middle_Close = nInData[2][15];
		m_Input.POD_EUV_1 			= nInData[2][16];
		m_Input.POD_EUV_2 			= nInData[2][17];
		m_Input.SPARE_5				= nInData[2][18];

		// 80 - 89
		m_Input.SPARE_6 			= nInData[2][19];
		m_Input.SPARE_7 			= nInData[2][20];
		m_Input.SPARE_8				= nInData[2][21];
		m_Input.SPARE_9				= nInData[2][22];

		m_Input.SPARE_10			= nInData[2][23];
		m_Input.SPARE_11			= nInData[2][24];	// 85
		m_Input.SPARE_12			= nInData[2][25];
		m_Input.SPARE_13			= nInData[2][26];

		m_Input.SPARE_14			= nInData[2][27];
		m_Input.SPARE_15			= nInData[2][28];
		m_Input.SPARE_16			= nInData[2][29];	// 90
		m_Input.SPARE_17			= nInData[2][30];

		m_Input.SPARE_18			= nInData[2][31];

	}
#elif(OHT_NAME == OHT_NAME_STD_V81S)
	{
		// 0 - 15
		m_Input.HAND_Inposition = nInData[0][0];
		m_Input.HAND_Alarm 		= nInData[0][1];
		m_Input.HAND_Right_Grip = nInData[0][2];
		m_Input.HAND_Left_Grip 	= nInData[0][3];

		m_Input.HAND_Close 		= nInData[0][4];
		m_Input.HAND_Open 		= nInData[0][5];		// 5
		m_Input.HAND_Push 		= nInData[0][6];
		m_Input.HOIST_Home 	= nInData[0][7];

		m_Input.HNAD_ES 	= nInData[0][8];

		m_Input.HAND_PIO_Go 	= nInData[0][9];
		m_Input.EQPIO_Load_REQ 	= nInData[0][10];
		m_Input.EQPIO_Unload_REQ= nInData[0][11];		// 10
		m_Input.EQPIO_Ready 	= nInData[0][12];

		m_Input.EQPIO_HO_AVBL 	= nInData[0][13];
		m_Input.EQPIO_ES 		= nInData[0][14];
		m_Input.EQPIO_Left_Go 	= nInData[0][15];		// 15

		// 16 - 31
		m_Input.STEER_F_Left 	= nInData[0][16];
		m_Input.STEER_R_Left 	= nInData[0][17];
		m_Input.STEER_F_Right 	= nInData[0][18];
		m_Input.STEER_R_Right 	= nInData[0][19];

		m_Input.SHUTTER_F_Close = nInData[0][20];		// 20
		m_Input.SHUTTER_R_Close = nInData[0][21];
		m_Input.SHUTTER_F_Open 	= nInData[0][22];
		m_Input.SHUTTER_R_Open 	= nInData[0][23];		// 25

		m_Input.SHUTTER_F_Alarm = nInData[0][24];
		m_Input.SHUTTER_R_Alarm = nInData[0][25];
		m_Input.SHUTTER_F_Inposition 	= nInData[0][26];
		m_Input.SHUTTER_R_Inposition 	= nInData[0][27];


		m_Input.OBSTACLE_Input_Extend 	= nInData[0][28];		// 30
		m_Input.OBSTACLE_Long 	= nInData[0][29];		// 30
		m_Input.OBSTACLE_Middle = nInData[0][30];
		m_Input.OBSTACLE_Short 	= nInData[0][31];
        // 32 - 47
		m_Input.OBSTACLE_Fail 	= nInData[1][0];
		m_Input.OHTDETECT_Input_Extend 	= nInData[1][1];
		m_Input.OHTDETECT_Long 	= nInData[1][2];
		m_Input.OHTDETECT_Middle= nInData[1][3];
		m_Input.OHTDETECT_Short = nInData[1][4];
		m_Input.OHTDETECT_Fail 	= nInData[1][5];

		m_Input.LOOKDOWN_Detect = nInData[1][6];
		m_Input.LOOKDOWN_Error 	= nInData[1][7];
		m_Input.SHIFT_Home 		= nInData[1][8];
		m_Input.STB_Detect_Left 		= nInData[1][9];

		m_Input.STB_Detect_Right 		= nInData[1][10];
		m_Input.UTB_Detect 		= nInData[1][11];
		m_Input.FOUP_Detect 	= nInData[1][12];
		m_Input.OSCILLATION 	= nInData[1][13];

		m_Input.EMO_Press 		= nInData[1][14];
		m_Input.BUMPER_Press 	= nInData[1][15];
		m_Input.FAN_Fail 		= nInData[1][16];
		m_Input.HOIST_Belt_Detect1 		= nInData[1][17];
        m_Input.HOIST_Belt_Detect2 		= nInData[1][18];
		// 48 - 63
		m_Input.TP_Enable 		= nInData[1][19];		// 50
		m_Input.HANDTP_Connect 	= nInData[1][20];
		m_Input.FOUP_Cover_Open_Detect 		= nInData[1][21];
		m_Input.STOP_Driving_Front 		= nInData[1][22];

		m_Input.STOP_Driving_PreCheck 	= nInData[1][23];
		m_Input.STOP_Trans_Front= nInData[1][24];
		m_Input.STOP_Trans_Rear = nInData[1][25];
		m_Input.BCR_Driving_OK 	= nInData[1][26];

		m_Input.BCR_Driving_NG 	= nInData[1][27];
		m_Input.BCR_Trans_OK 	= nInData[1][28];
		m_Input.BCR_Trans_NG 		= nInData[1][29];	// 60

		m_Input.Reserved_1 			= nInData[1][30];
		m_Input.Reserved_2 			= nInData[1][31];

		// 64 - 79
		m_Input.Reserved_3 			= nInData[2][0];
		m_Input.SMPS_Short 			= nInData[2][1];


		m_Input.CID_Link_Compt_IN 	= nInData[2][2];
		m_Input.CID_Interlock_IN 	= nInData[2][3];
		m_Input.CID_Completion_ACK 	= nInData[2][4];
		m_Input.CID_Status_IN 		= nInData[2][5];

		m_Input.CID_Reserved_IN1 	= nInData[2][6];	// 70
		m_Input.CID_Reserved_IN2 	= nInData[2][7];
		m_Input.Rotate_0_Sensor		= nInData[2][8];
		m_Input.Rotate_90_Sensor	= nInData[2][9];

		m_Input.Rotate_180_Sensor 	= nInData[2][10];
		m_Input.Rotate_270_Sensor 	= nInData[2][11];
		m_Input.HNAD_Reserved_1 			= nInData[2][12];	// 75
		m_Input.HOIST_Home_2 			= nInData[2][13];	// 75
		m_Input.SHUTTER_F_Middle_Close = nInData[2][14];

		m_Input.SHUTTER_R_Middle_Close = nInData[2][15];
		m_Input.POD_EUV_1 			= nInData[2][16];
		m_Input.POD_EUV_2 			= nInData[2][17];
		m_Input.SPARE_5				= nInData[2][18];

		// 80 - 89
		m_Input.SPARE_6 			= nInData[2][19];
		m_Input.SPARE_7 			= nInData[2][20];
		m_Input.SPARE_8				= nInData[2][21];
		m_Input.SPARE_9				= nInData[2][22];

		m_Input.SPARE_10			= nInData[2][23];
		m_Input.SPARE_11			= nInData[2][24];	// 85
		m_Input.SPARE_12			= nInData[2][25];
		m_Input.SPARE_13			= nInData[2][26];

		m_Input.SPARE_14			= nInData[2][27];
		m_Input.SPARE_15			= nInData[2][28];
		m_Input.SPARE_16			= nInData[2][29];	// 90
		m_Input.SPARE_17			= nInData[2][30];

		m_Input.SPARE_18			= nInData[2][31];

	}
#else
	m_Input.HAND_Inposition = nInData[0][0];
	m_Input.HAND_Alarm = nInData[0][1];
	m_Input.HAND_Right_Grip = nInData[0][2];
	m_Input.HAND_Left_Grip = nInData[0][3];
	m_Input.HAND_Close = nInData[0][4];
	m_Input.HAND_Open = nInData[0][5];
	m_Input.HAND_Push = nInData[0][6];
	m_Input.HOIST_Home = nInData[0][7];
	m_Input.HAND_PIO_Go = nInData[0][8];
	m_Input.EQPIO_Load_REQ = nInData[0][9];
	m_Input.EQPIO_Unload_REQ = nInData[0][10];
	m_Input.EQPIO_Ready = nInData[0][11];
	m_Input.EQPIO_HO_AVBL = nInData[0][12];
	m_Input.EQPIO_ES = nInData[0][13];
	m_Input.EQPIO_Right_Go = nInData[0][14];
	m_Input.EQPIO_Left_Go = nInData[0][15];
	m_Input.STEER_F_Left = nInData[0][16];
	m_Input.STEER_R_Left = nInData[0][17];
	m_Input.STEER_F_Right = nInData[0][18];
	m_Input.STEER_R_Right = nInData[0][19];
	m_Input.SHUTTER_F_Close = nInData[0][20];
	m_Input.SHUTTER_R_Close = nInData[0][21];
	m_Input.SHUTTER_F_Open = nInData[0][22];
	m_Input.SHUTTER_R_Open = nInData[0][23];
	m_Input.SHUTTER_F_Alarm = nInData[0][24];
	m_Input.SHUTTER_R_Alarm = nInData[0][25];
	m_Input.SHUTTER_F_Inposition = nInData[0][26];
	m_Input.SHUTTER_R_Inposition = nInData[0][27];
	m_Input.OBSTACLE_Long = nInData[0][28];
	m_Input.OBSTACLE_Middle = nInData[0][29];
	m_Input.OBSTACLE_Short = nInData[0][30];
	m_Input.OBSTACLE_Fail = nInData[0][31];

	m_Input.OHTDETECT_Long = nInData[1][0];
	m_Input.OHTDETECT_Middle = nInData[1][1];
	m_Input.OHTDETECT_Short = nInData[1][2];
	m_Input.OHTDETECT_Fail = nInData[1][3];
	m_Input.LOOKDOWN_Detect = nInData[1][4];
	m_Input.LOOKDOWN_Error = nInData[1][5];
	m_Input.SHIFT_Home = nInData[1][6];
	m_Input.STB_Detect_Left = nInData[1][7];
	m_Input.STB_Detect_Right = nInData[1][8];
	m_Input.UTB_Detect = nInData[1][9];
	m_Input.FOUP_Detect = nInData[1][10];
	m_Input.OSCILLATION = nInData[1][11];
	m_Input.EMO_Press = nInData[1][12];
	m_Input.BUMPER_Press = nInData[1][13];
	m_Input.FAN_Fail = nInData[1][14];
	m_Input.STB_OHT_Detect_Left = nInData[1][15];
	m_Input.STB_OHT_Detect_Right = nInData[1][16];
	m_Input.HANDTP_Connect = nInData[1][17];
	m_Input.STOP_Driving_PreCheck = nInData[1][18];
	m_Input.MC_Status = nInData[1][19];
	m_Input.STOP_Driving_Front = nInData[1][20];
	m_Input.STOP_Trans_Front = nInData[1][21];
	m_Input.STOP_Trans_Rear = nInData[1][22];
	m_Input.BCR_Driving_OK = nInData[1][23];
	m_Input.BCR_Driving_NG = nInData[1][24];
	m_Input.BCR_Trans_OK = nInData[1][25];
	m_Input.BCR_Trans_NG = nInData[1][26];
	m_Input.OHT_COMM_CD = nInData[1][27];
	m_Input.RailSupport_Home = nInData[1][28];
	m_Input.Railsupport_Work = nInData[1][29];
	m_Input.MAC_Detect = nInData[1][30];
	m_Input.SMPS_Short = nInData[1][31];
#endif

	for (int i = 0; i < nOutCount; i++) {
		for (int j = 0; j < 32; j++) {
			nOutData[i][j] = (nOutput[i] >> j) & 0x01;
		}
	}

#if(OHT_NAME == OHT_NAME_STD_V80)
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.EQPIO_Left_Select = nOutData[0][17];
	m_Output.EQPIO_Right_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];	

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];

	m_Output.LAMP_Yellow_Right = nOutData[1][19];
	m_Output.LAMP_Green = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.SAFETY_Reset = nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_POWER = nOutData[1][26];
	m_Output.LAMP_Yellow_Left = nOutData[1][27];
	m_Output.Reserved_60 = nOutData[1][28];
	m_Output.Reserved_61 = nOutData[1][29];
	m_Output.Reserved_62 = nOutData[1][30];
	m_Output.Reserved_63 = nOutData[1][31];

	//CID output �߰�
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_Reserved_OUT2 = nOutData[2][7];
	m_Output.Outtrigger_F_Start_OUT = nOutData[2][8];
	m_Output.Outtrigger_F_DIR_OUT = nOutData[2][9];
	m_Output.Outtrigger_F_Brake_OUT = nOutData[2][10];
	m_Output.Outtrigger_F_Speed_OUT = nOutData[2][11];
//	m_Output.Outtrigger_R_Start_OUT = nOutData[2][12];
	m_Output.Outtrigger_Reserved_76 = nOutData[2][12];
//	m_Output.Outtrigger_R_DIR_OUT = nOutData[2][13];
	m_Output.Outtrigger_Reserved_77 = nOutData[2][13];
//	m_Output.Outtrigger_R_Brake_OUT = nOutData[2][14];
	m_Output.Outtrigger_Reserved_78 = nOutData[2][14];
//	m_Output.Outtrigger_R_Speed_OUT = nOutData[2][15];
	m_Output.Outtrigger_Reserved_79 = nOutData[2][15];

#elif(OHT_NAME == OHT_NAME_STD_V81)
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.EQPIO_Left_Select = nOutData[0][17];
	m_Output.EQPIO_Right_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];

	m_Output.LAMP_Yellow_Right = nOutData[1][19];
	m_Output.LAMP_Green = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.SAFETY_Reset = nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_POWER = nOutData[1][26];
	m_Output.LAMP_Yellow_Left = nOutData[1][27];
	m_Output.Reserved_60 = nOutData[1][28];
	m_Output.Reserved_61 = nOutData[1][29];
	m_Output.Reserved_62 = nOutData[1][30];
	m_Output.Reserved_63 = nOutData[1][31];

	//CID output �߰�
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_Reserved_OUT2 = nOutData[2][7];
	m_Output.Reserved_72 = nOutData[2][8];
	m_Output.Reserved_73 = nOutData[2][9];
	m_Output.Reserved_74 = nOutData[2][10];
	m_Output.Reserved_75 = nOutData[2][11];
//	m_Output.Outtrigger_R_Start_OUT = nOutData[2][12];
	m_Output.Reserved_76 = nOutData[2][12];
//	m_Output.Outtrigger_R_DIR_OUT = nOutData[2][13];
	m_Output.Reserved_77 = nOutData[2][13];
//	m_Output.Outtrigger_R_Brake_OUT = nOutData[2][14];
	m_Output.Reserved_78 = nOutData[2][14];
//	m_Output.Outtrigger_R_Speed_OUT = nOutData[2][15];
	m_Output.Reserved_79 = nOutData[2][15];
#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_NOMAL))
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_CS_2 = nOutData[0][12];
	m_Output.EQPIO_CS_3 = nOutData[0][13];
	m_Output.EQPIO_TR_Req = nOutData[0][14];
	m_Output.EQPIO_Busy = nOutData[0][15];
	m_Output.EQPIO_Complete = nOutData[0][16];
	m_Output.EQPIO_Left_Select = nOutData[0][17];
	m_Output.EQPIO_Right_Select = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 = nOutData[0][19];
	m_Output.STEER_F_TRQ_1 = nOutData[0][20];
	m_Output.STEER_R_TRQ_0 = nOutData[0][21];
	m_Output.STEER_R_TRQ_1 = nOutData[0][22];
	m_Output.STEER_F_Dir = nOutData[0][23];
	m_Output.STEER_R_Dir = nOutData[0][24];
	m_Output.SHUTTER_F_Reset = nOutData[0][25];
	m_Output.SHUTTER_R_Reset = nOutData[0][26];
	m_Output.SHUTTER_F_Close = nOutData[0][27];
	m_Output.SHUTTER_R_Close = nOutData[0][28];
	m_Output.SHUTTER_F_Open = nOutData[0][29];
	m_Output.SHUTTER_R_Open = nOutData[0][30];
	m_Output.SHUTTER_F_Origin = nOutData[0][31];	

	m_Output.SHUTTER_R_Origin = nOutData[1][0];
	m_Output.SHUTTER_F_Free = nOutData[1][1];
	m_Output.SHUTTER_R_Free = nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];
	m_Output.LAMP_Red = nOutData[1][18];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][19];
	m_Output.LAMP_Green 		= nOutData[1][20];
	m_Output.HAND_Power_Reset 	= nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear 		= nOutData[1][23];
	m_Output.SAFETY_Reset 		= nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];
	m_Output.QRR_Power 		= nOutData[1][26];
	m_Output.LAMP_Yellow_Left	= nOutData[1][27];
	m_Output.Reserved60 		= nOutData[1][28];
	m_Output.Reserved61 		= nOutData[1][29];
	m_Output.Reserved62 		= nOutData[1][30];
	m_Output.Reserved63 		= nOutData[1][31];

	//CID output �߰�
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];
	m_Output.CID_Occupancy = nOutData[2][2];
//	m_Output.CID_PASS_Compt_OUT = nOutData[2][3];
	m_Output.CID_Pass_Completion = nOutData[2][3];
	m_Output.CID_Abnormal = nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];
	m_Output.CID_Common_Tag = nOutData[2][6];
	m_Output.CID_Reserved_OUT2 = nOutData[2][7];
	m_Output.Outtrigger_Reserved73 = nOutData[2][8];
	m_Output.Outtrigger_Reserved74 = nOutData[2][9];
	m_Output.Outtrigger_Reserved75 = nOutData[2][10];
	m_Output.Outtrigger_Reserved76 = nOutData[2][11];
	m_Output.Outtrigger_Reserved77 = nOutData[2][12];
	m_Output.Outtrigger_Reserved78 = nOutData[2][13];
	m_Output.Outtrigger_Reserved79 = nOutData[2][14];
	m_Output.Outtrigger_Reserved80 = nOutData[2][15];

#elif((OHT_NAME == OHT_NAME_STD_V80_RETICLE) && (OHT_RETICLE_EXTEND_MODEL == RETICLE_EUV_1))
    // 0 - 15
	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];
	
	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	
	m_Output.HAND_PIO_Select 	= nOutData[0][8];
	m_Output.EQPIO_Valid 		= nOutData[0][9];
	m_Output.EQPIO_CS_0 		= nOutData[0][10];
	m_Output.EQPIO_CS_1 		= nOutData[0][11];
	
	m_Output.EQPIO_CS_2 		= nOutData[0][12];
	m_Output.EQPIO_CS_3 		= nOutData[0][13];
	m_Output.EQPIO_TR_Req 		= nOutData[0][14];
	m_Output.EQPIO_Busy 		= nOutData[0][15];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][16];
	m_Output.EQPIO_Right_Select = nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];
	
	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];
	
	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];
	
	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];	
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];
	
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];	
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][7];
	
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][9];	
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][11];
	
	m_Output.OHTDETECT_Power_Reset = nOutData[1][12];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][15];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][17];	
	m_Output.LAMP_Red 			= nOutData[1][18];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][19];
	
	m_Output.LAMP_Green 		= nOutData[1][20];
	m_Output.HAND_Power_Reset 	= nOutData[1][21];	
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear 		= nOutData[1][23];
	
	m_Output.SAFETY_Reset 		= nOutData[1][24];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][25];		
	m_Output.QRR_Power 		= nOutData[1][26];
	m_Output.LAMP_Yellow_Left	= nOutData[1][27];
	
	m_Output.Reserved60 		= nOutData[1][28];
	m_Output.Reserved61 		= nOutData[1][29];
	m_Output.Reserved62 		= nOutData[1][30];
	m_Output.Reserved63 		= nOutData[1][31];

	// 64 - 79
	m_Output.CID_Left_Select_ON = nOutData[2][0];
	m_Output.CID_Right_Select_ON = nOutData[2][1];	
	m_Output.CID_Occupancy 		= nOutData[2][2];
	m_Output.CID_Pass_Completion = nOutData[2][3];	
	
	m_Output.CID_Abnormal 		= nOutData[2][4];
	m_Output.CID_Operation_Mode = nOutData[2][5];	
	m_Output.CID_Common_Tag 	= nOutData[2][6];
	m_Output.CID_Reserved_OUT2 	= nOutData[2][7];
	
	m_Output.SPARE_6 = nOutData[2][8];
	m_Output.SPARE_7 = nOutData[2][9];	
	m_Output.SPARE_8 = nOutData[2][10];
	m_Output.SHUTTER_F_Middle_Close = nOutData[0][11];
	
	m_Output.SHUTTER_R_Middle_Close = nOutData[0][12];
	m_Output.SPARE_9 				= nOutData[2][13];
	m_Output.SPARE_10 				= nOutData[2][14];
	m_Output.SPARE_11 				= nOutData[2][15];

	// 80 - 93
	m_Output.SPARE_12 				= nOutData[2][16];
	m_Output.SPARE_13 				= nOutData[2][17];
	m_Output.SPARE_14 				= nOutData[2][18];
	m_Output.SPARE_15 				= nOutData[2][19];
	
	m_Output.SPARE_16 				= nOutData[2][20];
	m_Output.SPARE_17 				= nOutData[2][21];
	m_Output.SPARE_18 				= nOutData[2][22];
	m_Output.SPARE_19 				= nOutData[2][23];
	
	m_Output.SPARE_20 				= nOutData[2][24];
	m_Output.SPARE_21 				= nOutData[2][25];
	m_Output.SPARE_22 				= nOutData[2][26];
	m_Output.SPARE_23 				= nOutData[2][27];

	m_Output.SPARE_24				= nOutData[2][28];
	m_Output.SPARE_25				= nOutData[2][29];
	m_Output.SPARE_26				= nOutData[2][30];
	m_Output.SPARE_27				= nOutData[2][31];

#elif(OHT_NAME == OHT_NAME_STD_V85)
 	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];

	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	m_Output.HAND_HandTrigger 	= nOutData[0][7];
	m_Output.HAND_PIO_Select 	= nOutData[0][9];
	m_Output.EQPIO_Valid 		= nOutData[0][10];
	m_Output.EQPIO_CS_0 		= nOutData[0][11];
	m_Output.EQPIO_CS_1 		= nOutData[0][12];

	m_Output.EQPIO_CS_2 		= nOutData[0][13];
	m_Output.EQPIO_CS_3 		= nOutData[0][14];
	m_Output.EQPIO_TR_Req 		= nOutData[0][15];
	m_Output.EQPIO_Busy 		= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];

	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];

	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];

	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];

	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Area5_Sel = nOutData[1][7];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][8];

	m_Output.OHTDETECT_Area1_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][12];
	m_Output.OHTDETECT_Area5_Sel = nOutData[1][13];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][14];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][17];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][18];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][19];
	m_Output.LAMP_Red 			= nOutData[1][20];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][21];
    m_Output.LAMP_Yellow_Left	= nOutData[1][22];
	m_Output.LAMP_Green 		= nOutData[1][23];
	m_Output.HAND_Power_Reset 	= nOutData[1][24];
	m_Output.BRAKE_Driving_Release = nOutData[1][25];
	m_Output.Bumper_Clear 		= nOutData[1][26];

	m_Output.SAFETY_Reset 		= nOutData[1][27];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][28];
	m_Output.QRR_Power 		= nOutData[1][29];
	m_Output.Reserved60 		= nOutData[1][30];
	m_Output.Reserved61 		= nOutData[1][31];

    	// 64 - 79
	m_Output.Reserved62 		= nOutData[2][0];
	m_Output.Reserved63 		= nOutData[2][1];

	m_Output.CID_Left_Select_ON = nOutData[2][2];
	m_Output.CID_Right_Select_ON = nOutData[2][3];
	m_Output.CID_Occupancy 		= nOutData[2][4];
	m_Output.CID_Pass_Completion = nOutData[2][5];

	m_Output.CID_Abnormal 		= nOutData[2][6];
	m_Output.CID_Operation_Mode = nOutData[2][7];
	m_Output.CID_Common_Tag 	= nOutData[2][8];
	m_Output.CID_Reserved_OUT2 	= nOutData[2][9];

	m_Output.SPARE_6			= nOutData[2][10];
	m_Output.SPARE_7 			= nOutData[2][11];
	m_Output.SHUTTER_F_Middle_Close = nOutData[2][12];

	m_Output.SHUTTER_R_Middle_Close = nOutData[2][13];
	m_Output.SPARE_10 				= nOutData[2][14];
	m_Output.SPARE_11 				= nOutData[2][15];

	// 80 - 93
	m_Output.SPARE_12 				= nOutData[2][16];
	m_Output.SPARE_13 				= nOutData[2][17];
	m_Output.SPARE_14 				= nOutData[2][18];
	m_Output.SPARE_15 				= nOutData[2][19];

	m_Output.SPARE_16 				= nOutData[2][20];
	m_Output.SPARE_17 				= nOutData[2][21];
	m_Output.SPARE_18 				= nOutData[2][22];
	m_Output.SPARE_19 				= nOutData[2][23];

	m_Output.SPARE_20 				= nOutData[2][24];
	m_Output.SPARE_21 				= nOutData[2][25];
	m_Output.SPARE_22 				= nOutData[2][26];
	m_Output.SPARE_23 				= nOutData[2][27];

	m_Output.SPARE_24				= nOutData[2][28];
	m_Output.SPARE_25				= nOutData[2][29];
	m_Output.SPARE_26				= nOutData[2][30];
	m_Output.SPARE_27				= nOutData[2][31];


#elif(OHT_NAME == OHT_NAME_STD_V81S)
 	m_Output.HAND_Close 		= nOutData[0][0];
	m_Output.HAND_Open 			= nOutData[0][1];
	m_Output.HAND_Origin 		= nOutData[0][2];
	m_Output.HAND_Reset 		= nOutData[0][3];

	m_Output.HAND_Free 			= nOutData[0][4];
	m_Output.HAND_Reserved6 	= nOutData[0][5];
	m_Output.HAND_Reserved7 	= nOutData[0][6];
	m_Output.HAND_Reserved8 	= nOutData[0][7];
	m_Output.HAND_HandTrigger 	= nOutData[0][7];
	m_Output.HAND_PIO_Select 	= nOutData[0][9];
	m_Output.EQPIO_Valid 		= nOutData[0][10];
	m_Output.EQPIO_CS_0 		= nOutData[0][11];
	m_Output.EQPIO_CS_1 		= nOutData[0][12];

	m_Output.EQPIO_CS_2 		= nOutData[0][13];
	m_Output.EQPIO_CS_3 		= nOutData[0][14];
	m_Output.EQPIO_TR_Req 		= nOutData[0][15];
	m_Output.EQPIO_Busy 		= nOutData[0][16];

	// 16 - 31
	m_Output.EQPIO_Complete 	= nOutData[0][17];
	m_Output.EQPIO_Left_Select  = nOutData[0][18];
	m_Output.STEER_F_TRQ_0 		= nOutData[0][19];

	m_Output.STEER_F_TRQ_1 		= nOutData[0][20];
	m_Output.STEER_R_TRQ_0 		= nOutData[0][21];
	m_Output.STEER_R_TRQ_1 		= nOutData[0][22];
	m_Output.STEER_F_Dir 		= nOutData[0][23];

	m_Output.STEER_R_Dir 		= nOutData[0][24];
	m_Output.SHUTTER_F_Reset 	= nOutData[0][25];
	m_Output.SHUTTER_R_Reset 	= nOutData[0][26];
	m_Output.SHUTTER_F_Close 	= nOutData[0][27];

	m_Output.SHUTTER_R_Close 	= nOutData[0][28];
	m_Output.SHUTTER_F_Open 	= nOutData[0][29];
	m_Output.SHUTTER_R_Open 	= nOutData[0][30];
	m_Output.SHUTTER_F_Origin 	= nOutData[0][31];

	// 32 - 47
	m_Output.SHUTTER_R_Origin 	= nOutData[1][0];
	m_Output.SHUTTER_F_Free 	= nOutData[1][1];
	m_Output.SHUTTER_R_Free 	= nOutData[1][2];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][3];

	m_Output.OBSTACLE_Area2_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][5];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][6];
	m_Output.OBSTACLE_Area5_Sel = nOutData[1][7];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][8];

	m_Output.OHTDETECT_Area1_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][10];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][11];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][12];
	m_Output.OHTDETECT_Area5_Sel = nOutData[1][13];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][14];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][15];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][16];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][17];

	// 48 - 63
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][18];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][19];
	m_Output.LAMP_Red 			= nOutData[1][20];
	m_Output.LAMP_Yellow_Right 		= nOutData[1][21];
    m_Output.LAMP_Yellow_Left	= nOutData[1][22];
	m_Output.LAMP_Green 		= nOutData[1][23];
	m_Output.HAND_Power_Reset 	= nOutData[1][24];
	m_Output.BRAKE_Driving_Release = nOutData[1][25];
	m_Output.Bumper_Clear 		= nOutData[1][26];

	m_Output.SAFETY_Reset 		= nOutData[1][27];
	m_Output.HOIST_Ext_Brake_Release = nOutData[1][28];
	m_Output.QRR_Power 		= nOutData[1][29];
	m_Output.Reserved60 		= nOutData[1][30];
	m_Output.Reserved61 		= nOutData[1][31];

    	// 64 - 79
	m_Output.Reserved62 		= nOutData[2][0];
	m_Output.Reserved63 		= nOutData[2][1];

	m_Output.CID_Left_Select_ON = nOutData[2][2];
	m_Output.CID_Right_Select_ON = nOutData[2][3];
	m_Output.CID_Occupancy 		= nOutData[2][4];
	m_Output.CID_Pass_Completion = nOutData[2][5];

	m_Output.CID_Abnormal 		= nOutData[2][6];
	m_Output.CID_Operation_Mode = nOutData[2][7];
	m_Output.CID_Common_Tag 	= nOutData[2][8];
	m_Output.CID_Reserved_OUT2 	= nOutData[2][9];

	m_Output.SPARE_6			= nOutData[2][10];
	m_Output.SPARE_7 			= nOutData[2][11];
	m_Output.SHUTTER_F_Middle_Close = nOutData[2][12];

	m_Output.SHUTTER_R_Middle_Close = nOutData[2][13];
	m_Output.SPARE_10 				= nOutData[2][14];
	m_Output.SPARE_11 				= nOutData[2][15];

	// 80 - 93
	m_Output.SPARE_12 				= nOutData[2][16];
	m_Output.SPARE_13 				= nOutData[2][17];
	m_Output.SPARE_14 				= nOutData[2][18];
	m_Output.SPARE_15 				= nOutData[2][19];

	m_Output.SPARE_16 				= nOutData[2][20];
	m_Output.SPARE_17 				= nOutData[2][21];
	m_Output.SPARE_18 				= nOutData[2][22];
	m_Output.SPARE_19 				= nOutData[2][23];

	m_Output.SPARE_20 				= nOutData[2][24];
	m_Output.SPARE_21 				= nOutData[2][25];
	m_Output.SPARE_22 				= nOutData[2][26];
	m_Output.SPARE_23 				= nOutData[2][27];

	m_Output.SPARE_24				= nOutData[2][28];
	m_Output.SPARE_25				= nOutData[2][29];
	m_Output.SPARE_26				= nOutData[2][30];
	m_Output.SPARE_27				= nOutData[2][31];

#else
	m_Output.HAND_Close = nOutData[0][0];
	m_Output.HAND_Open = nOutData[0][1];
	m_Output.HAND_Origin = nOutData[0][2];
	m_Output.HAND_Reset = nOutData[0][3];
	m_Output.HAND_Free = nOutData[0][4];
	m_Output.HAND_Reserved6 = nOutData[0][5];
	m_Output.HAND_Reserved7 = nOutData[0][6];
	m_Output.HAND_Reserved8 = nOutData[0][7];
	m_Output.HAND_PIO_Select = nOutData[0][8];
	m_Output.EQPIO_Valid = nOutData[0][9];
	m_Output.EQPIO_CS_0 = nOutData[0][10];
	m_Output.EQPIO_CS_1 = nOutData[0][11];
	m_Output.EQPIO_TR_Req = nOutData[0][12];
	m_Output.EQPIO_Busy = nOutData[0][13];
	m_Output.EQPIO_Complete = nOutData[0][14];
	m_Output.EQPIO_Left_Select = nOutData[0][15];
	m_Output.EQPIO_Right_Select = nOutData[0][16];
	m_Output.STEER_F_TRQ_0 = nOutData[0][17];
	m_Output.STEER_F_TRQ_1 = nOutData[0][18];
	m_Output.STEER_R_TRQ_0 = nOutData[0][19];
	m_Output.STEER_R_TRQ_1 = nOutData[0][20];
	m_Output.STEER_F_Dir = nOutData[0][21];
	m_Output.STEER_R_Dir = nOutData[0][22];
	m_Output.SHUTTER_F_Reset = nOutData[0][23];
	m_Output.SHUTTER_R_Reset = nOutData[0][24];
	m_Output.SHUTTER_F_Close = nOutData[0][25];
	m_Output.SHUTTER_R_Close = nOutData[0][26];
	m_Output.SHUTTER_F_Open = nOutData[0][27];
	m_Output.SHUTTER_R_Open = nOutData[0][28];
	m_Output.SHUTTER_F_Origin = nOutData[0][29];
	m_Output.SHUTTER_R_Origin = nOutData[0][30];
	m_Output.SHUTTER_F_Free = nOutData[0][31];

	m_Output.SHUTTER_R_Free = nOutData[1][0];
	m_Output.OBSTACLE_Area1_Sel = nOutData[1][1];
	m_Output.OBSTACLE_Area2_Sel = nOutData[1][2];
	m_Output.OBSTACLE_Area3_Sel = nOutData[1][3];
	m_Output.OBSTACLE_Area4_Sel = nOutData[1][4];
	m_Output.OBSTACLE_Power_Reset = nOutData[1][5];
	m_Output.OHTDETECT_Area1_Sel = nOutData[1][6];
	m_Output.OHTDETECT_Area2_Sel = nOutData[1][7];
	m_Output.OHTDETECT_Area3_Sel = nOutData[1][8];
	m_Output.OHTDETECT_Area4_Sel = nOutData[1][9];
	m_Output.OHTDETECT_Power_Reset = nOutData[1][10];
	m_Output.LOOKDOWN_Area1_Sel = nOutData[1][11];
	m_Output.LOOKDOWN_Area2_Sel = nOutData[1][12];
	m_Output.LOOKDOWN_Area3_Sel = nOutData[1][13];
	m_Output.LOOKDOWN_Area4_Sel = nOutData[1][14];
	m_Output.LOOKDOWN_Power_Reset = nOutData[1][15];
	m_Output.LAMP_Red = nOutData[1][16];
	m_Output.LAMP_Yellow = nOutData[1][17];
	m_Output.LAMP_Green = nOutData[1][18];
	m_Output.STB_Right_Work = nOutData[1][19];
	m_Output.STB_Left_Work = nOutData[1][20];
	m_Output.HAND_Power_Reset = nOutData[1][21];
	m_Output.BRAKE_Driving_Release = nOutData[1][22];
	m_Output.Bumper_Clear = nOutData[1][23];
	m_Output.MC_Control = nOutData[1][24];
	m_Output.LCD_Display_Reset = nOutData[1][25];
	m_Output.SERIAL_Select_1 = nOutData[1][26];
	m_Output.SERIAL_Select_2 = nOutData[1][27];
	m_Output.RailSupport_TRQ_0 = nOutData[1][28];
	m_Output.RailSupport_TRQ_1 = nOutData[1][29];
	m_Output.RailSupport_Dir = nOutData[1][30];
	m_Output.Reserved_1 = nOutData[1][31];
#endif

	//AOHC-196 ���� �α� �߰�
	// SHCHO 2019.03 : UBG, OBS ��缭�� ���� ���� ���� ��� �����α� ����
	int nUBGLevel = OHTDetect::GetSensorLevel2(m_Input.OHTDETECT_Long,m_Input.OHTDETECT_Middle, m_Input.OHTDETECT_Short , m_Input.OHTDETECT_Fail);
	int nOBSLevel = OHTDetect::GetSensorLevel2(m_Input.OBSTACLE_Long, m_Input.OBSTACLE_Middle, m_Input.OBSTACLE_Short , m_Input.OBSTACLE_Fail);

	static OHTMainStatus *pMainStat = NULL;
	if(pMainStat == NULL)
		pMainStat = OHTMainForm->GetOHTStatus();

	static DiagManager *pDiag = NULL;
	if(pDiag == NULL)
		pDiag = DiagManager::getInstance();
	if(pDiag != NULL && pMainStat != NULL)
	{
		bool bAutoMode = pMainStat->StatusCommon.MSCmdID.Info.Mode == MODE_AUTO;

		if(bAutoMode)
		{	if(nUBGLevel < 0)
			{
				pDiag->IncreasePreviosDiagCount(PREVIOS_DIAG_UF_2);
				ADD_MD_LOG("UF_2 UBG Lv : %d%d%d%d",m_Input.OHTDETECT_Long,m_Input.OHTDETECT_Middle,m_Input.OHTDETECT_Short,m_Input.OHTDETECT_Fail);

			}

			if(nOBSLevel < 0)
			{
				pDiag->IncreasePreviosDiagCount(PREVIOS_DIAG_OF_2);
				ADD_MD_LOG("OF_2 OBS Lv : %d%d%d%d", m_Input.OBSTACLE_Long,m_Input.OBSTACLE_Middle,	m_Input.OBSTACLE_Short,m_Input.OBSTACLE_Fail);
			}
		}
	}


}

/**
 @brief   HWCommon�� I/O�� ���� ȹ��
 @author  ehoto
 @date    2012-11-19
 @param	pInputBuff: Input �޴� ����
 @param	nInputCount: Input �޴� ���� ����(int ����)
 @param	pOutputBuff: Output �޴� ����
 @param	nOutCount: Output �޴� ���� ����(int ����)
 @return  Error Code ������ ��� NO_ERR(0)
 */
int HWCommon::GetIOSet(int* pInputBuff, int& nInputCount, int* pOutputBuff,
	int& nOutCount)
{
	return m_pMotionController->GetIOSet(pInputBuff, nInputCount, pOutputBuff,
		nOutCount);
}

/**
@brief   ����� ������ũ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTorqueLimit(void)
{
	m_pMotionController->SetTorqueLimit();
}
/**
@brief   ����� ������ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTorqueLimitRelease(void)
{
	m_pMotionController->SetTorqueLimitRelease();
}

/**
@brief   ����� ������ũ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetSlopeTime(void)
{
	m_pMotionController->SetSlopeTime();
}
/**
@brief   ����� ������ũ ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetSlopeTimeRelease(void)
{
	m_pMotionController->SetSlopeTimeRelease();
}

/**
@brief   ����� ������ ��ġ ���� ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTargetPos(void)
{
	m_pMotionController->SetTargetPos();
}
/**
@brief   ����� ������ ��ġ  ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetTargetPosRelease(void)
{
	m_pMotionController->SetTargetPosRelease();
}

/**
@brief   ����� ������ ��ġ  ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetFirstNodeCountAdd(void)
{
	m_pMotionController->SetFirstNodeCountAdd();
}
/**
@brief   ����� ������ ��ġ  ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetVHLDetectIgnor(void)
{
	m_pMotionController->SetVHLDetectIgnor();
}
/**
@brief   ����� ������ ��ġ  ����  ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetVHLDetectIgnorRelease(void)
{
	m_pMotionController->SetVHLDetectIgnorRelease();
}

//AOHC-196 ���� �α� �߰�
int HWCommon::GetFollowing_Axis1(void)
{
	return m_pMotionController->GetFollowing_Axis1();
}
/**
@brief   ���������� ���ӵ� ����
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetDetectStopDecelDown(void)
{
	m_pMotionController->SetDetectStopDecelDown();
}
/**
@brief   ���������� ���ӵ� ���� ����
@author  ehoto
@date    2012-11-28
*/
void HWCommon::SetDetectStopDecelDownRelease(void)
{
	m_pMotionController->SetDetectStopDecelDownRelease();
}
/**
@brief   ���������� ���� ���� ����
@author  puting
@date    2019-07-24
*/
void HWCommon::SetVHLDetectIOSet(bool bIoSet)
{
   m_pMotionController->SetVHLDetectIOSet(bIoSet);
}

/**
@brief   ����� Node Count�� Reset ��û�ϴ� �Լ�
@author  ehoto
@date    2012-11-28
*/
void HWCommon::ClearNodeCountInfo(void)
{
	m_pMotionController->ClearNodeCountInfo();
}

/**
@brief   ����� NodeCount �� ������ �޴� �Լ�
@author  ehoto
@date    2012-11-28
@param   nNodeCount : Node Count�� ���� �޴� ����
@param   dNodeDetectPosition : Node ������ ��ġ�� ���� �޴� ����
@return  Error Code ������ ��� NO_ERR(0)
*/
int HWCommon::GetNodeCountInfo(NODE_CHECK_DATA &NodeCheckData)
{
	return m_pMotionController->GetNodeCountInfo(NodeCheckData);
}

/**
@brief   DetectSensor�� ������ �����ϴ� �Լ�
@author  ehoto
@date    2013-01-22
@param   DetectInfo : DetectSensor �� ���� �����ϱ� ���� ������ ������ �ִ� ����ü
@return  Error Code ������ ��� NO_ERR(0)
*/

#if(USE_CHANGENODE == 1)
int HWCommon::SetDetectInfo(DETECT_INFO DetectInfo, OPTIONAL_NODE_INFO OptNodeInfo)
{
	return m_pMotionController->SetDetectInfo(DetectInfo, OptNodeInfo);
}
#else
int HWCommon::SetDetectInfo(DETECT_INFO DetectInfo)
{
	return m_pMotionController->SetDetectInfo(DetectInfo);
}
#endif
/**
@brief   CompensateSlavePos 

	�޹����� �չ��� ���ڴ� ������ ������ ���ߴ� �Լ�
	���ķ� �ο� ���� ���� (Ư�� �ҷ� �̵� ��)
@author  ���¿�
@date    2013-03-12
*/
int HWCommon::CompensateSlavePos()
{
	return m_pMotionController->CompensateSlavePos();
}

int HWCommon::SetPauseControl(int bIsPause, int nCommander)
{
	return m_pMotionController->SetPauseControl(bIsPause, nCommander);
}

void	HWCommon::makeTraceLog()
{
	m_pMotionController->makeTraceLog();
}
//---------------------------------------------------------------------------

/**
@brief   ���а� ���  �Լ�
@author  KDS
@date    2017.11.29
@return  ���а� ����
*/
int HWCommon::GetVolatageMonitor(int Index)
{
	return	m_pMotionController->GetVolatageMonitor(Index);
}

/**
@brief   AMC�� ������ ������ �Լ�
@author  zzang9un
@date    2013.10.07
@return  AMC�� ���� ����(int)�� ����
*/
int HWCommon::GetAMCVersion()
{
    return m_pMotionController->GetAMCVersion();
}
//---------------------------------------------------------------------------

int HWCommon::GetFPGAVersion()
{
    return m_pMotionController->GetFPGAVersion();
}

int HWCommon::GetAMCTorqueStatus()
{
    return m_pMotionController->GetTorqueStatusByAMC();
}

int HWCommon::GetAMCPauseStatus()
{
    return m_pMotionController->GetPauseStatusByAMC();
}

int HWCommon::GetAMCVHLDetectStatus()
{
    return m_pMotionController->GetVHLDetectStatusByAMC();
}

int HWCommon::GetAMCStopStatus()
{
    return m_pMotionController->GetStopStatusByAMC();
}
int HWCommon::GetAMCDecelStatus()
{
    return m_pMotionController->GetDecelStatusByAMC();
}
int HWCommon::GetAMCDetectStopDecelDownStatus()
{
	return m_pMotionController->GetDetectStopDecelDownStatusByAMC();
}
int HWCommon::GetVHLDetectForceIOStatus()
{
    return m_pMotionController->GetVHLDetectForceIO();
}

/**
@brief   AMC�� FileSystem ��ü�� �����ϱ� ���� �Լ�
@author  ji7412.lee
@date    2014-02-10
@param   nAxisNum : �� ��ȣ
@return  Error Code
 */
int HWCommon::AMCFileSystemForamt()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCFileSystemForamt();
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� Drive������ ������ Set/Reset ��Ű�� �Լ�
@author  ji7412.lee
@date    2014-02-10
 */
int HWCommon::SetAMCDriverOpenByCommon(bool isOnOff)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->SetAMCDriverOpenByCommon(isOnOff);
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DonwLoadFile Flag�� Set ��Ű�� �Լ�
@author  ji7412.lee
@date    2014-02-10
 */
int HWCommon::SetAMCFileDownloaded(bool isOnOff)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->SetAMCFileDownloaded(isOnOff);
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� ClearWatchDogStatus �Լ�
@author  ji7412.lee
@date    2014-02-10
@note    Common���� ��û�Ҽ� �ֵ��� Public �߰�
 */
int HWCommon::ClearWatchDogStatus()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->ClearWatchDogStatusPublic();
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DisableWatchDog �Լ�
@author  ji7412.lee
@date    2014.02.10
@note    Common���� ��û�Ҽ� �ֵ��� Public �߰�
 */
int HWCommon::DisableWatchDog()
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->DisableWatchDogPublic();
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� File System Install �Լ�
@author  ji7412.lee
@date    2014.02.10
 */
int HWCommon::AMCFileSystemInstall(char *pszBootLoader, char *pszBootFile, int *prtn, HWND hWnd, UINT uiMsg)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->AMCFileSystemInstall(pszBootLoader, pszBootFile, prtn, hWnd, uiMsg);
}
//---------------------------------------------------------------------------

/**
@brief   AMC�� DSP_Reboot �Լ�
@author  ji7412.lee
@date    2014.02.10
 */
int HWCommon::DSP_Reboot(void)
{
	if (m_pMotionController == NULL)
		return false;
	return m_pMotionController->DSP_Reboot();
}
//---------------------------------------------------------------------------


