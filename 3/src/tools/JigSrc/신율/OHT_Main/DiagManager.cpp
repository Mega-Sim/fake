//---------------------------------------------------------------------------


#pragma hdrstop

#include "OHTMain.h"
#include "Utility.h"
#include "DiagManager.h"

#define ADD_LOG_DIAG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\DiagManager.log", __VA_ARGS__)

#define ADD_PREVIOSDIAG_LOG(...)         WRITE_LOG_UNIT(pLogPreviosDiag, __VA_ARGS__)


DiagManager::DiagManager()
{

	m_DiagPatrolInfoList = new LList;
	m_DiagAbnormalDataList = new LList;
	m_nDiagAbnormalDrvDiffCnt = 0;
	m_nVHLWorkingTime = 0;
	m_nDiagCheckCount = 0;
	m_bIsLoadedDiagData = false;
	m_nDiagCheckTimerID = 0;
	m_nTimerRetryCount = 0;
	m_ParamChehckError = NO_ERR;
	memset(&m_DiagServiceData, 0, sizeof(SERVICE_DATA_DIAG));

	memset(&m_DiagStatusData, 0, sizeof(DIAG_OHT_STATUS_DATA));

	memset(&m_DiagParam, 0, sizeof(DIAG_PARAM));
	LoadDiagParamRecorvery("DataFiles\\DiagParam.ini",&m_DiagParam);

	//Make Diag data
	m_DiagDataMap = new map<BYTE, DIAG_OHT_STATISTICS_DATA*>();

	MakeStatDataMap();

	pLogPreviosDiag = getLogUnit("D:\\log\\Monitor.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	InitializeCriticalSection(&m_CSPreDiag);
	ResetPreviosDiagCount();

	//AOHC-196 ���� �α� �߰�
	memset(m_nArrDiagVal, 0, sizeof(m_nArrDiagVal));
	m_dwPrevLogStartTick = 0;
	// SHCHO 2019.03 : �����α� enum�� GetPrevIdxName() �ε����� ���� �ʴ� ��� �̸� �˷��� �ҽֻ縦 ���´�
	for(int i=0; i<COUNTOF_PREVIOUS ; i++)
	{
		char buf[32];
		memset(buf, 0, sizeof(buf));
		if(GetPrevIdxName((PREVIOS_DIAG_PARAM_NUM)i, buf) == false)
		{
			AnsiString msg;
			msg = msg.sprintf("[%s][Line:%d]\n����͸� �α� PREVIOS_DIAG_PARAM_NUM �� ������ GetPrevIdxName(..) �Լ��� �ε����� ���� �ʽ��ϴ�.\n�ڵ� ������ �ؾ��մϴ�.\n���α׷��� ����˴ϴ�.", __FILE__, __LINE__);
			
			wchar_t wmsg[1024];
			msg.WideChar(wmsg, sizeof(wmsg));
			Application->MessageBox(wmsg, L"Monitoring Log Error");
			exit(0);
		}
			
	}


}

DiagManager::~DiagManager()
{
	//Ÿ�̸� �������̾��ٸ� ����
	if(m_nDiagCheckTimerID != NULL)
	{
		timeKillEvent(m_nDiagCheckTimerID);
	}

	//PatrolVHL �ݳ�
	if(m_DiagPatrolInfoList != NULL)
	{
		int nDataCnt = m_DiagPatrolInfoList->getCount();

		for(int i=0; i < nDataCnt; i++)
		{
			SERVICE_DATA_DIAG* pDeleteData = (SERVICE_DATA_DIAG*)m_DiagPatrolInfoList->popFront();

			delete pDeleteData;
		}

		delete m_DiagPatrolInfoList;
	}

	//��� ������ �ݳ�
	if(m_DiagDataMap != NULL)
	{
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin(); iter != m_DiagDataMap->end(); iter++)
		{
			DIAG_OHT_STATISTICS_DATA* pDelData = (DIAG_OHT_STATISTICS_DATA*)iter->second;
			delete pDelData;
		}

		m_DiagDataMap->clear();

		delete m_DiagDataMap;
	}

	//Abnormal Data �ݳ�
	if(m_DiagAbnormalDataList != NULL)
	{
		int nDataCnt = m_DiagAbnormalDataList->getCount();

		for(int i=0; i<nDataCnt; i++)
		{
			DIAG_OHT_ABNORMAL* pDeleteData = (DIAG_OHT_ABNORMAL*)m_DiagAbnormalDataList->popFront();

			delete pDeleteData;
		}

		delete m_DiagAbnormalDataList;
	}

	DeleteCriticalSection(&m_CSPreDiag);
}


void DiagManager::MakeStatDataMap()
{
	//�� VHL ��赥���� ����
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEDISTANCE,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEDISTANCE, m_DiagParam.nStatUnitMoveDistance, m_DiagParam.nRefStatMoveDistance)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESTEERING,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESTEERING, m_DiagParam.nStatUnitMoveSteering, m_DiagParam.nRefStatMoveSteering)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESHIFT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESHIFT, m_DiagParam.nStatUnitMoveShift, m_DiagParam.nRefStatMoveShift)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_REVSHIFT,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_REVSHIFT, m_DiagParam.nStatUnitRevShift, m_DiagParam.nRefStatRevShift)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEHOIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEHOIST, m_DiagParam.nStatUnitMoveHoist, m_DiagParam.nRefStatMoveHoist)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_REVHOIST,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_REVHOIST, m_DiagParam.nStatUnitRevHoist, m_DiagParam.nRefStatRevHoist)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEROTATE,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEROTATE, m_DiagParam.nStatUnitMoveRotate, m_DiagParam.nRefStatMoveRotate)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVEHAND,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVEHAND, m_DiagParam.nStatUnitMoveHand, m_DiagParam.nRefStatMoveHand)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_MOVESHUTTER,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_MOVESHUTTER, m_DiagParam.nStatUnitMoveShutter, m_DiagParam.nRefStatMoveShutter)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_COUNTDRVBCR,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_COUNTDRVBCR, m_DiagParam.nStatUnitCountDrvBCR, m_DiagParam.nRefStatCountDrvBCR)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_COUNTSTABCR,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_COUNTSTABCR, m_DiagParam.nStatUnitCountStaBCR, m_DiagParam.nRefStatCountStaBCR)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMELOOKDOWN,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMELOOKDOWN, m_DiagParam.nStatUnitTimeLookdown, m_DiagParam.nRefStatTimeLookdown)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMEUBG,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMEUBG, m_DiagParam.nStatUnitTimeUBG, m_DiagParam.nRefStatTimeUBG)));
	m_DiagDataMap->insert(DIAG_STAT_DATA_PAIR(DIAG_DATA_ID_TIMEPBS,
				new DIAG_OHT_STATISTICS_DATA(DIAG_DATA_ID_TIMEPBS, m_DiagParam.nStatUnitTimePBS, m_DiagParam.nRefStatTimePBS)));

}

DiagManager* DiagManager::getInstance()
{
	int nError = NO_ERR;

	if(pInstance == NULL)
	{
		pInstance = new DiagManager;

		if(nError != NO_ERR)
		{
			ADD_LOG_DIAG("[DiagParam] Read File .. ErrorCode : %d", nError);
		}
	}

	return pInstance;
}

// 2018.08.06 SHCHO : To Call Kill Timer
void DiagManager::DestroyInstance()
{
	if(pInstance)
	{
		delete pInstance;   // calling ===> DiagManager::~DiagManager
		pInstance = NULL;
	}

}
/**
@brief	���� ���� Parameter �ҷ���
@data   2015.12.10
@author	doori.shin
*/
bool DiagManager::LoadDiagParamRecorvery(char* szFilePath, DIAG_PARAM* pParamSet)
{
	bool bSuccess = true;
	int nTmpValue = 0;
    int nResult = 0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};
	bool bCheckParam = false;

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
	}

	if(pIniFile != NULL)
	{
		//Read Diag Agent informations
		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		if(StrToIntA(strTempValue) <= StrToIntA(pParamSet->Version))
			bSuccess = false;
		else
		{
			memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
			memset(strTempValue, 0, sizeof(strTempValue));
	    }
		//Read Reference values
		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveDistance", DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE, 10, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveSteering", DIAG_DEFAULT_REF_VALUE_MOVESTEERING, 2000, 10000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveShift", DIAG_DEFAULT_REF_VALUE_MOVESHIFT, 1000, 3000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsRevShift", DIAG_DEFAULT_REF_VALUE_REVSHIFT, 1, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatRevShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

 		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveHoist", DIAG_DEFAULT_REF_VALUE_MOVEHOIST, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsRevHoist", DIAG_DEFAULT_REF_VALUE_REVHOIST, 5, 50, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatRevHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveRotate", DIAG_DEFAULT_REF_VALUE_MOVEROTATE, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveRotate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveHand", DIAG_DEFAULT_REF_VALUE_MOVEHAND, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsMoveShutter", DIAG_DEFAULT_REF_VALUE_MOVESHUTTER, 1000, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsCountDrvBCR", DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR, 50, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatCountDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsCountStaBCR", DIAG_DEFAULT_REF_VALUE_COUNTSTABCR, 50, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatCountStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeLookdown", DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeLookdown = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimeUBG", DIAG_DEFAULT_REF_VALUE_TIMEUBG, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimeUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatisticsTimePBS", DIAG_DEFAULT_REF_VALUE_TIMEPBS, 10, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatTimePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveDistance", DIAG_REF_VALUE_UNIT_MOVEDISTANCE, 100000, 2000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveDistance = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveSteering", DIAG_REF_VALUE_UNIT_MOVESTEERING, 0, 5, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveShift", DIAG_REF_VALUE_UNIT_MOVESHIFT, 0, 5, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitRevShift", DIAG_REF_VALUE_UNIT_REVSHIFT, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitRevShift = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveHoist", DIAG_REF_VALUE_UNIT_MOVEHOIST, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitRevHoist", DIAG_REF_VALUE_UNIT_REVHOIST, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitRevHoist = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveRotate", DIAG_REF_VALUE_UNIT_MOVEROTATE, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveRotate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveHand", DIAG_REF_VALUE_UNIT_MOVEHAND, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitMoveShutter", DIAG_REF_VALUE_UNIT_MOVESHUTTER, 0, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitCountDrvBCR", DIAG_REF_VALUE_UNIT_COUNTDRVBCR, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitCountDrvBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitCountStaBCR", DIAG_REF_VALUE_UNIT_COUNTSTABCR, 1000, 30000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitCountStaBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeLookdown", DIAG_REF_VALUE_UNIT_TIMELOOKDOWN, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeLookdown = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimeUBG", DIAG_REF_VALUE_UNIT_TIMEUBG, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimeUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "StatisticsUnitTimePBS", DIAG_REF_VALUE_UNIT_TIMEPBS, 1000, 5000000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nStatUnitTimePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}


		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT, 1, 50, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormUsingMemoryCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE, 5, 200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormUsingMemoryRate = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND, 500, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER, 500, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveShutter = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING, 500, 20000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormTimeoverMoveSteering = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormAlarmResetHand = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormOutofPhasePBS = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormOutofPhaseUBG = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalRetryBCR", DIAG_ABNORMAL_REF_VALUE_RETRY_BCR, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormRetryBCR = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormDifferenceEncoderCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefAbnormDifferenceEncoderLimit = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatusAvailableSpaceC", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatusAvailableSpaceC = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ReferenceValue", "ReferenceStatusAvailableSpaceD", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE, 0, 100, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nRefStatusAvailableSpaceD = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam =  pIniFile->ReadIntegerMinMaxVerification("ETC", "PBSOutOfRangeTimeout", PBS_OUT_OF_RANGE_TIMEOUT, 100, 5000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPBSOutOfRangeTimeout = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		delete pIniFile;
	}
	else
	{
		//File Open Error
		bSuccess = false;
	}

	if(bSuccess ==false)
	{
		bCheckParam = LoadDiagParam("DataFiles\\DiagParam.bgk", pParamSet);
		if(bCheckParam)
			saveParameter("DataFiles\\DiagParam.ini", pParamSet);
		else
           m_ParamChehckError = ERR_PARADATA_LOAD_FAIL;
	}
	else
	{
		saveParameter("DataFiles\\DiagParam.bgk", pParamSet);
		m_ParamChehckError = NO_ERR;
	}


	return bSuccess;
}


/**
@brief	���� ���� Parameter �ҷ���
@data   2015.12.10
@author	doori.shin
*/
bool DiagManager::LoadDiagParam(char* szFilePath, DIAG_PARAM* pParamSet)
{
	bool bSuccess = true;
	int nTmpValue = 0;
    int nResult = 0;
	bool bTmpValue = false;
	double dResult = 0.0;
	char strTempValue[8]= {0,};

    LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile(szFilePath);

	if(!pIniFile->GetIniFileExist())
    {
		delete pIniFile;
		pIniFile = NULL;
		bSuccess = false;
    }

	if(pIniFile != NULL)
	{
		//Read Diag Agent informations
		pIniFile->ReadString("Version","IniVersion", "00", pParamSet->Version);

		//Read Reference values

		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveDistance", DIAG_DEFAULT_REF_VALUE_MOVEDISTANCE, &pParamSet->nRefStatMoveDistance);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveSteering", DIAG_DEFAULT_REF_VALUE_MOVESTEERING, &pParamSet->nRefStatMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveShift", DIAG_DEFAULT_REF_VALUE_MOVESHIFT, &pParamSet->nRefStatMoveShift);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsRevShift", DIAG_DEFAULT_REF_VALUE_REVSHIFT, &pParamSet->nRefStatRevShift);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveHoist", DIAG_DEFAULT_REF_VALUE_MOVEHOIST, &pParamSet->nRefStatMoveHoist);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsRevHoist", DIAG_DEFAULT_REF_VALUE_REVHOIST, &pParamSet->nRefStatRevHoist);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveRotate", DIAG_DEFAULT_REF_VALUE_MOVEROTATE, &pParamSet->nRefStatMoveRotate);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveHand", DIAG_DEFAULT_REF_VALUE_MOVEHAND, &pParamSet->nRefStatMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsMoveShutter", DIAG_DEFAULT_REF_VALUE_MOVESHUTTER, &pParamSet->nRefStatMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsCountDrvBCR", DIAG_DEFAULT_REF_VALUE_COUNTDRVBCR, &pParamSet->nRefStatCountDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsCountStaBCR", DIAG_DEFAULT_REF_VALUE_COUNTSTABCR, &pParamSet->nRefStatCountStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeLookdown", DIAG_DEFAULT_REF_VALUE_TIMELOOKDOWN, &pParamSet->nRefStatTimeLookdown);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimeUBG", DIAG_DEFAULT_REF_VALUE_TIMEUBG, &pParamSet->nRefStatTimeUBG);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatisticsTimePBS", DIAG_DEFAULT_REF_VALUE_TIMEPBS, &pParamSet->nRefStatTimePBS);

		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveDistance", DIAG_REF_VALUE_UNIT_MOVEDISTANCE, &pParamSet->nStatUnitMoveDistance);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveSteering", DIAG_REF_VALUE_UNIT_MOVESTEERING, &pParamSet->nStatUnitMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveShift", DIAG_REF_VALUE_UNIT_MOVESHIFT, &pParamSet->nStatUnitMoveShift);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitRevShift", DIAG_REF_VALUE_UNIT_REVSHIFT, &pParamSet->nStatUnitRevShift);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveHoist", DIAG_REF_VALUE_UNIT_MOVEHOIST, &pParamSet->nStatUnitMoveHoist);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitRevHoist", DIAG_REF_VALUE_UNIT_REVHOIST, &pParamSet->nStatUnitRevHoist);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveRotate", DIAG_REF_VALUE_UNIT_MOVEROTATE, &pParamSet->nStatUnitMoveRotate);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveHand", DIAG_REF_VALUE_UNIT_MOVEHAND, &pParamSet->nStatUnitMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitMoveShutter", DIAG_REF_VALUE_UNIT_MOVESHUTTER, &pParamSet->nStatUnitMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitCountDrvBCR", DIAG_REF_VALUE_UNIT_COUNTDRVBCR, &pParamSet->nStatUnitCountDrvBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitCountStaBCR", DIAG_REF_VALUE_UNIT_COUNTSTABCR, &pParamSet->nStatUnitCountStaBCR);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeLookdown", DIAG_REF_VALUE_UNIT_TIMELOOKDOWN, &pParamSet->nStatUnitTimeLookdown);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimeUBG", DIAG_REF_VALUE_UNIT_TIMEUBG, &pParamSet->nStatUnitTimeUBG);
		pIniFile->ReadInteger("ReferenceValue", "StatisticsUnitTimePBS", DIAG_REF_VALUE_UNIT_TIMEPBS, &pParamSet->nStatUnitTimePBS);

		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", DIAG_ABNORMAL_REF_VALUE_MEMORY_COUNT, &pParamSet->nRefAbnormUsingMemoryCount);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", DIAG_ABNORMAL_REF_VALUE_MEMORY_USING_RATE, &pParamSet->nRefAbnormUsingMemoryRate);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_HAND, &pParamSet->nRefAbnormTimeoverMoveHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_SHUTTER, &pParamSet->nRefAbnormTimeoverMoveShutter);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", DIAG_ABNORMAL_REF_VALUE_TIMEOVER_MOVE_STEERING, &pParamSet->nRefAbnormTimeoverMoveSteering);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", DIAG_ABNORMAL_REF_VALUE_ALARM_RESET_HAND, &pParamSet->nRefAbnormAlarmResetHand);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_PBS, &pParamSet->nRefAbnormOutofPhasePBS);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", DIAG_ABNORMAL_REF_VALUE_OUT_OF_PHASE_UBG, &pParamSet->nRefAbnormOutofPhaseUBG);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalRetryBCR", DIAG_ABNORMAL_REF_VALUE_RETRY_BCR, &pParamSet->nRefAbnormRetryBCR);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_CNT, &pParamSet->nRefAbnormDifferenceEncoderCount);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", DIAG_ABNORMAL_REF_VALUE_DIFFERENCE_ENCODER_LIMIT, &pParamSet->nRefAbnormDifferenceEncoderLimit);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatusAvailableSpaceC", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_C_DRIVE, &pParamSet->nRefStatusAvailableSpaceC);
		pIniFile->ReadInteger("ReferenceValue", "ReferenceStatusAvailableSpaceD", DIAG_VHLSTATUS_REF_VALUE_AVAILABLE_D_DRIVE, &pParamSet->nRefStatusAvailableSpaceD);
		pIniFile->ReadInteger("ETC", "PBSOutOfRangeTimeout", PBS_OUT_OF_RANGE_TIMEOUT, &pParamSet->nPBSOutOfRangeTimeout);

		bSuccess = true;
//		//������ ��ȿ�� �˻�
//		if(m_DiagParam.strDiagIpAddr == "")
//		{
//			nErrorCode = ERR_DIAGPARAM_INVALID_IP;
//		}
//		else if(m_DiagParam.nDiagPort == 0)
//		{
//			nErrorCode = ERR_DIAGPARAM_INVALID_PORT;
//        }


		delete pIniFile;
	}
	else
	{
		//File Open Error
		bSuccess = false;
	}

	return bSuccess;
}


bool DiagManager::saveParameter(char* szFilePath, DIAG_PARAM* pParamSet)
{
    bool bResult = false;

    LIniFile *m_lWriteIniFile;
    m_lWriteIniFile = NULL;
    m_lWriteIniFile = new LIniFile(szFilePath);

    if(!m_lWriteIniFile->GetIniFileExist())
    {
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
    }

    if(m_lWriteIniFile != NULL)
    {
		m_lWriteIniFile->DeleteIniFile();

		m_lWriteIniFile->WriteString("Version","IniVersion", pParamSet->Version);
		// 1. Hoist Param
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveDistance",  pParamSet->nRefStatMoveDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveSteering",  pParamSet->nRefStatMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveShift",  pParamSet->nRefStatMoveShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsRevShift",  pParamSet->nRefStatRevShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveHoist", pParamSet->nRefStatMoveHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsRevHoist", pParamSet->nRefStatRevHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveRotate", pParamSet->nRefStatMoveRotate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveHand", pParamSet->nRefStatMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsMoveShutter", pParamSet->nRefStatMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsCountDrvBCR", pParamSet->nRefStatCountDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsCountStaBCR", pParamSet->nRefStatCountStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeLookdown", pParamSet->nRefStatTimeLookdown);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimeUBG", pParamSet->nRefStatTimeUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatisticsTimePBS", pParamSet->nRefStatTimePBS);

		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveDistance", pParamSet->nStatUnitMoveDistance);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveSteering", pParamSet->nStatUnitMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveShift", pParamSet->nStatUnitMoveShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitRevShift", pParamSet->nStatUnitRevShift);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveHoist", pParamSet->nStatUnitMoveHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitRevHoist", pParamSet->nStatUnitRevHoist);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveRotate", pParamSet->nStatUnitMoveRotate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveHand", pParamSet->nStatUnitMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitMoveShutter", pParamSet->nStatUnitMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitCountDrvBCR", pParamSet->nStatUnitCountDrvBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitCountStaBCR", pParamSet->nStatUnitCountStaBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeLookdown", pParamSet->nStatUnitTimeLookdown);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimeUBG", pParamSet->nStatUnitTimeUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "StatisticsUnitTimePBS", pParamSet->nStatUnitTimePBS);

		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingCount", pParamSet->nRefAbnormUsingMemoryCount);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalMemoryUsingRate", pParamSet->nRefAbnormUsingMemoryRate);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveHand", pParamSet->nRefAbnormTimeoverMoveHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveShutter", pParamSet->nRefAbnormTimeoverMoveShutter);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalTimeoverMoveSteering", pParamSet->nRefAbnormTimeoverMoveSteering);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalAlarmResetHandCount", pParamSet->nRefAbnormAlarmResetHand);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalOutofPhasePBS", pParamSet->nRefAbnormOutofPhasePBS);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalOutofPhaseUBG", pParamSet->nRefAbnormOutofPhaseUBG);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalRetryBCR", pParamSet->nRefAbnormRetryBCR);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderCount", pParamSet->nRefAbnormDifferenceEncoderCount);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceAbnormalDifferenceEncoderLimit", pParamSet->nRefAbnormDifferenceEncoderLimit);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatusAvailableSpaceC", pParamSet->nRefStatusAvailableSpaceC);
		m_lWriteIniFile->WriteInteger("ReferenceValue", "ReferenceStatusAvailableSpaceD", pParamSet->nRefStatusAvailableSpaceD);
		m_lWriteIniFile->WriteInteger("ETC", "PBSOutOfRangeTimeout", pParamSet->nPBSOutOfRangeTimeout);

		delete m_lWriteIniFile;
		bResult = true;


	}

    return bResult;
}

/**
@brief	���ܿ��� ����ϴ� �Ķ���͸� ����
@data   2016.04.06
@author	doori.shin
*/
DIAG_PARAM DiagManager::GetDiagParam()
{
	return this->m_DiagParam;
}

/**
@brief	���� �����͸� ������Ʈ �Ѵ�
@data   2016.04.06
@author	doori.shin
@param	bID : ������Ʈ �� ������ ID
@param	nValue : �߰��� ���� ��
@return ������Ʈ ������ true, ���н� false(Map �ڷᱸ���� ID�� �ش��ϴ� ������ ����)
*/
int DiagManager::UpdateStatDiagData(BYTE bID, double nValue)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	//0���� ū �������� ��츸 ������Ʈ
	if(nValue > 0.0f)
	{
		if(iter == m_DiagDataMap->end())
		{
			nResult = DIAG_ACK_ERR_NO_DATA;
		}
		else
		{
			iter->second->UpdateData(nValue);
			nResult = DIAG_ACK_OK;
		}
	}

	return nResult;
}

/**
@brief	���� �����͸� �����ϴ� ���� �����͸� �����Ѵ�
@data   2016.04.06
@author	doori.shin
@param	bID : ������Ʈ �� ������ ID
@param	nValue : ���ŵǴ� ��
@return ������Ʈ ������ true, ���н� false(Map �ڷᱸ���� ID�� �ش��ϴ� ������ ����)
*/
int DiagManager::UpdateStatReferenceValue(BYTE bID, int nValue)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	if(iter == m_DiagDataMap->end())
	{
		nResult = DIAG_ACK_ERR_NO_DATA;
	}
	else
	{
		iter->second->nReferenceValue = nValue;
		nResult = DIAG_ACK_OK;
	}

	return nResult;
}

/**
@brief	������ ���ܵ����͸� Ŭ���� �Ѵ�
@data   2016.04.06
@author	doori.shin
@param	bID : Ŭ���� �� ������ ID
@return Clear���� �� true, ���н� false(Map �ڷᱸ���� ID�� �ش��ϴ� ������ ����)
*/
int DiagManager::ClearStatDiagData(BYTE bID)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	int nResult = DIAG_ACK_ERR_UNKNOWN;

	if(iter == m_DiagDataMap->end())
	{
		nResult = DIAG_ACK_ERR_NO_DATA;
	}
	else
	{
		iter->second->ClearData();
		nResult = DIAG_ACK_OK;
	}

	return nResult;
}

/**
@brief	������ ���ܵ����͸� �����ص� �Ǵ��� Ȯ��
@data   2016.04.06
@author	doori.shin
@param	bID : Ȯ���� ������ ID
@return ���۰���(���ذ��̻�)�̸� true, �� �ܿ��� false(Map �ڷᱸ���� ID�� �ش��ϴ� ������ ����)
*/
bool DiagManager::CheckStatDataToSend(BYTE bID)
{
	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	bool bResult = false;

	if(iter == m_DiagDataMap->end())
	{
		bResult = false;
	}
	else
	{
		bResult = iter->second->IsReadyToSend();
	}

	return bResult;
}
/**
@brief	���ܵ����� ���� �� ����
@data   2016.04.06
@author
@param	bID : Ȯ���� ������ ID
@return
*/
double DiagManager::GetStatDiagData(BYTE bID)
{
 	DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->find(bID);
	double bResult = 0.0f;

	if(iter == m_DiagDataMap->end())
	{

	}
	else
	{
		bResult = iter->second->dTotalData;
	}

	return bResult;
}
/**
@brief	���� �����͸� ������Ʈ �Ѵ�
@data   2016.04.06
@author	doori.shin
@param	bID : ������Ʈ �� ������ ID
@param	nValue : �߰��� ���� ��
@return ������Ʈ ������ true, ���н� false(Map �ڷᱸ���� ID�� �ش��ϴ� ������ ����)
*/
bool DiagManager::UpdateStatDiagAllData(DIAG_PKT_BODY_DATA_INFO_RESPONSE nStatData)
{
//	    UINT64 nStatData = 0;
		int nLoadedCount = 0;
//		int nOffset = UDP_HEADER_LEN;

		//Parsing ��� ������
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin();
			iter != m_DiagDataMap->end(); iter++)
		{
			DIAG_OHT_STATISTICS_DATA* pData = (DIAG_OHT_STATISTICS_DATA*)iter->second;

			try
			{

				double dUpdateData = nStatData.InfoData[nLoadedCount] * pData->nUnit;
				pData->UpdateData(dUpdateData);
				pData->dPreSendData = nStatData.InfoData[nLoadedCount];	//���ܼ����� ����Ǿ��ִ� ������
				ADD_LOG_DIAG("Update Data. [ID:%02X, Value:%d]", pData->bID, nStatData.InfoData[nLoadedCount]);

				nLoadedCount++;
			}
			catch(Exception& e)
			{
				ADD_LOG_DIAG("[Exception] %s, ", e.StackTrace);
				break;
			}
		}

		//�����Ͱ� ���� �ε��Ǿ����� Ÿ�̸� ����
		if(nLoadedCount == 14)
		{
			m_bIsLoadedDiagData = true;
			timeKillEvent(m_nDiagCheckTimerID);
			ADD_LOG_DIAG("Update Complete. Count : %d", nLoadedCount);
		}
		else
		{
			m_bIsLoadedDiagData = false;
		}

}

/**
@brief	Abnormal�� ������ ���ܵ����͸� ����
@data   2016.04.19
@author	doori.shin
@param	pInsertData : ����Ʈ�� ������ ������
*/
void DiagManager::InsertAbnormalDiagData(BYTE bDataType)
{
	if(m_DiagAbnormalDataList != NULL)
	{
		DIAG_OHT_ABNORMAL* pInsertData = new DIAG_OHT_ABNORMAL;
		pInsertData->bDataType = bDataType;
		pInsertData->nNodeID = OHTMainForm->GetOHTStatus()->StatusCommon.CurNodeID;

		if(m_DiagAbnormalDataList->getCount() > DIAG_MAX_SIZE_ABNORMAL_DATA_LIST)
		{
			DIAG_OHT_ABNORMAL* pDeleteData = (DIAG_OHT_ABNORMAL*)m_DiagAbnormalDataList->popFront();

			delete pDeleteData;
		}

		m_DiagAbnormalDataList->insertBack(pInsertData);

		ADD_LOG_DIAG("[DiagManager] Inserted Abnormal data : [ID:%02X], [NodeID:%05d]",
					pInsertData->bDataType,
					pInsertData->nNodeID);
	}
}

/**
@brief	Abnormal ������ ���� ������ ����Ʈ ������
@data   2016.04.19
@author	doori.shin
*/
LList* DiagManager::GetAbnormalDiagDataList()
{
	return this->m_DiagAbnormalDataList;
}

/**
@brief	Disk�� ��뷮 ������ Ȯ��
@data   2016.04.19
@author	doori.shin
@param	pInfo : Disk������ ������ ����ü
@return DIAG_OHT_STATUS_OK : Parameter�� ���� ��뷮 ��ġ���� ����
		DIAG_OHT_STATUS_NG : Parameter�� ���� ��뷮 ���� �ʰ����
*/
bool DiagManager::CheckDiskStatus(DISK_INFO* pInfo)
{
	bool bResult = DIAG_OHT_STATUS_OK;
	int nUpperbound = 0;
	int nUsingRate = 0;

	if(pInfo == NULL)
	{
		ADD_LOG_DIAG("[Error] Parameter(pInfo) is NULL pointer.");
	}
	else if(OHTUTILITY::GetDiskInfo(pInfo, pInfo->Drive) == NO_ERR)
	{
		//Parameter�� ������ �ִ� Drive ���Ѽ����� ������
		switch(pInfo->Drive)
		{
			case 'C':
			nUpperbound = m_DiagParam.nRefStatusAvailableSpaceC;
			break;
			case 'D':
			nUpperbound = m_DiagParam.nRefStatusAvailableSpaceD;
			break;
		}

		//������ ���
		try
		{
			nUsingRate = ((double)pInfo->UsedSpace / (double)pInfo->TotalSpace) * 100;
		}
		catch(Exception& e)
		{
			ADD_LOG_DIAG("[Error] Divided by 0 in CheckDiskStatus");
		}

		//��뷮�� ���Ѽ����� ŭ
		if(nUpperbound < nUsingRate)
		{
			bResult = DIAG_OHT_STATUS_NG;
		}
	}

	return bResult;
}

/**
@brief	OHT Status �����׸��� Ȯ��
@data   2016.04.19
@author	doori.shin
*/
void DiagManager::CheckOHTStatus()
{
	int nError = NO_ERR;
	DISK_INFO* pDiskInfo = NULL;	//��ũ ��뷮 Ȯ���� �� ���

	//1. C Drive ��뷮 Ȯ��
	pDiskInfo = new DISK_INFO;
	pDiskInfo->Drive = 'C';
	m_DiagStatusData.bIsAvailableCDrive = CheckDiskStatus(pDiskInfo);
	delete pDiskInfo;

	//2. D Drive ��뷮 Ȯ��
	pDiskInfo = new DISK_INFO;
	pDiskInfo->Drive = 'D';
	m_DiagStatusData.bIsAvailableDDrive = CheckDiskStatus(pDiskInfo);
	delete pDiskInfo;

	//3. USB�޸𸮿� ������ ��Ͽ��� Ȯ��
    m_DiagStatusData.bIsLogSave = true;

	//4. USB�޸� �ν�Ȯ��
	m_DiagStatusData.bIsEnabledUSB = OHTUTILITY::Check_DDrive();

	//5. FAN���ۿ��� Ȯ��(Ȯ���ʿ�)
	m_DiagStatusData.bIsEnabledFAN = OHTMainForm->GetHWSet()->m_pFan->IsFanFail() ? false : true;

	ADD_LOG_DIAG("Check OHT Status : LogSave(%d), EnabledUSB(%d), FanFail(%d), Disk C(%d), Disk D(%d)",
			m_DiagStatusData.bIsLogSave, m_DiagStatusData.bIsEnabledUSB,
			m_DiagStatusData.bIsEnabledFAN, m_DiagStatusData.bIsAvailableCDrive,
			m_DiagStatusData.bIsAvailableDDrive);

}

/**
@brief	���ܿ� ���� Status ������ �ʱ�ȭ(������ ���� �ʱ�ȭ)
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ClearDiagStatusData()
{
	memset(&m_DiagStatusData, 0x00, sizeof(DIAG_OHT_STATUS_DATA));
}

/**
@brief	��� �׸� ���� �����Ͱ� �ִ� Map �ڷᱸ���� ������
@data   2016.04.27
@author	doori.shin
*/
DIAG_STAT_DATA_MAP* DiagManager::GetDiagStatMap()
{
	return this->m_DiagDataMap;
}

/**
@brief	��/�Ĺ� ���ڴ��� �̻��� Ƚ���� ������
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetAbnormalDrvDiffCount()
{
	return m_nDiagAbnormalDrvDiffCnt;
}

DIAG_OHT_STATUS_DATA* DiagManager::GetDiagOHTstatus()
{
    return &(this->m_DiagStatusData);
}

/**
@brief	��/�Ĺ� ���ڴ��� �̻��� Ƚ���� ����
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseAbnormalDrvDiffCount()
{
	m_nDiagAbnormalDrvDiffCnt++;
}

/**
@brief	��/�Ĺ� ���ڴ��� �̻��� Ƚ���� �ʱ�ȭ
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetAbnormalDrvDiffCount()
{
	m_nDiagAbnormalDrvDiffCnt = 0;
}


/**
@brief	Hand Reset �̻��� Ƚ���� ������
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetAbnormalHandResetCount()
{
	return m_nDiagAbnormalHandResetCount;
}

/**
@brief	Hand Reset �̻��� Ƚ���� ����
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseAbnormalHandResetCount()
{
	m_nDiagAbnormalHandResetCount++;
}

/**
@brief	Hand Reset �̻��� Ƚ���� �ʱ�ȭ
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetAbnormalHandResetCount()
{
	m_nDiagAbnormalHandResetCount = 0;
}

/**
@brief	24�ð� üũ�ϴ� ������ ������
@data   2016.04.27
@author	doori.shin
*/
int DiagManager::GetDiagCheckCount()
{
	return m_nDiagCheckCount;
}

/**
@brief	24�ð� üũ�ϴ� ������ ����
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::IncreaseDiagCheckCount()
{
	m_nDiagCheckCount++;
}

/**
@brief	��/24�ð� üũ�ϴ� ������ �ʱ�ȭ
@data   2016.04.27
@author	doori.shin
*/
void DiagManager::ResetDiagCheckCount()
{
	m_nDiagCheckCount = 0;
}

/**
@brief	OHT�� ����� ���ܼ����� ���� ��� �����͸� �޾Ҵ��� Ȯ��
@data   2016.04.27
@author	doori.shin
@return OK : ���ܼ������� ��� �����͸� ����
		NG : ���ܼ������� ������ ������
*/
bool DiagManager::IsDiagStatLoaded()
{
	return this->m_bIsLoadedDiagData;
}

/**
@brief	���ܼ����� ��������� ��û�ϴ� �ݹ��Լ�
@data   2016.05.16
@author	doori.shin
*/
void CALLBACK CheckInitStatDataProc(UINT _timerId, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	if(DiagManager::getInstance()->IsDiagStatLoaded() == false)
	{
		OHTMainForm->GetTaskControl()->SendReqeustDiagStatData();
		DiagManager::getInstance()->IncreaseRetryCount();

		if(DiagManager::getInstance()->GetRetryCount() > DIAG_CHECK_TIMER_RETRY_COUNT)
		{
			//Timer ����
			timeKillEvent(DiagManager::getInstance()->GetCheckTimerID());
			ADD_LOG_DIAG("[Inform] DiagStatCheckTimer is terminated..[RetryCount Over : %d]", DIAG_CHECK_TIMER_RETRY_COUNT);
		}
	}
}

/**
@brief	OHT�� ����� ���ܼ����� ������� ������ ��û Ÿ�̸�
@data   2016.05.16
@author	doori.shin
*/
void DiagManager::SetDiagStatCheckTimer()
{
	TIMECAPS timeCaps;

	timeGetDevCaps(&timeCaps, sizeof(TIMECAPS));

	if(m_nDiagCheckTimerID != NULL)
		timeKillEvent(m_nDiagCheckTimerID);

	m_nDiagCheckTimerID = timeSetEvent(DIAG_CHECK_INIT_STAT_DATA_INTERVAL,
										timeCaps.wPeriodMax,
										CheckInitStatDataProc,
										(DWORD)0,
										TIME_PERIODIC);

	if(m_nDiagCheckTimerID == NULL)
	{
		ADD_LOG_DIAG("[Error] Fail to Create DiagStatCheckTimer...");
	}
	else
	{
		ADD_LOG_DIAG("Success to Create DiagStatCheckTimer. [ID:%d]", m_nDiagCheckTimerID);
	}
}

/**
@brief	Retry Count ����
@data   2016.05.16
@author	doori.shin
*/
void DiagManager::IncreaseRetryCount()
{
	this->m_nTimerRetryCount++;
}

/**
@brief	Retry Count ������
@data   2016.05.16
@author	doori.shin
*/
int DiagManager::GetRetryCount()
{
	return this->m_nTimerRetryCount;
}

/**
@brief	Timer ID�� ������
@data   2016.05.16
@author	doori.shin
*/
int DiagManager::GetCheckTimerID()
{
	return this->m_nDiagCheckTimerID;
}

void DiagManager::Reset_DiagManager()
{
	ADD_LOG_DIAG("[PARAM APPLY] ��� ������ �ݳ�.....");
	
	//��� ������ �ݳ� (from ~DiagManager())
	if(m_DiagDataMap != NULL)
	{
		for(DIAG_STAT_DATA_MAP_ITER iter = m_DiagDataMap->begin(); iter != m_DiagDataMap->end(); iter++)
		{
			ADD_LOG_DIAG("[PARAM APPLY] data set return..");
				
			DIAG_OHT_STATISTICS_DATA* pDelData = (DIAG_OHT_STATISTICS_DATA*)iter->second;
			delete pDelData;
		}

		m_DiagDataMap->clear();

		ADD_LOG_DIAG("[PARAM APPLY] Data Cleared");
		// delete m_DiagDataMap;

	}
	else
	{
		ADD_LOG_DIAG("[PARAM APPLY] data re alloc");
		m_DiagDataMap = new map<BYTE, DIAG_OHT_STATISTICS_DATA*>();
    }

	// data remake
	ADD_LOG_DIAG("[PARAM APPLY] data remake....");
	MakeStatDataMap();
}
//AOHC-196 ���� �α� �߰�
// ����͸� �α� ��� �׸� �α��ϱ� (����)
void DiagManager::LogginPreviosDiagCount()
{
	char buf[2048];
	memset(buf, 0, sizeof(buf));
	EnterCriticalSection(&m_CSPreDiag);
	
	for(int i=0; i<COUNTOF_PREVIOUS ; i++)
	{
		char name[32];	// ����͸� �α� �׸� �̸� ���
		memset(name, 0, sizeof(name));
		GetPrevIdxName((PREVIOS_DIAG_PARAM_NUM)i, name);

		char item[64];	// �̸�/�� ���·� �����
		memset(item, 0, sizeof(item));
		sprintf(item, "%s/%d/", name, m_nArrDiagVal[i]);

		strcat(buf, item);	// ���ۿ� �ױ�
	}
	
	LeaveCriticalSection(&m_CSPreDiag);
	// �α�
	ADD_PREVIOSDIAG_LOG(buf);

	m_dwPrevLogStartTick = timeGetTime();	// ������ �α� �ð� ���

}

// �ش� �׸� ī��Ʈ ����
void DiagManager::IncreasePreviosDiagCount(PREVIOS_DIAG_PARAM_NUM nType)
{
	if(nType >= COUNTOF_PREVIOUS || nType < 0)
		return;
	
	EnterCriticalSection(&m_CSPreDiag);
	if(m_nArrDiagVal[nType] < INT_MAX)
		m_nArrDiagVal[nType]++;
	LeaveCriticalSection(&m_CSPreDiag);
}

// �ش� �׸� �ִ밪 ����
void DiagManager::IncreasePreviosDiagMax(PREVIOS_DIAG_PARAM_NUM nType, int dwTime)
{
	if(nType >= COUNTOF_PREVIOUS || nType < 0)
		return;
	
	EnterCriticalSection(&m_CSPreDiag);
	
	// �ð� �ִ밪�� �����ϵ��� ����
	if(dwTime > m_nArrDiagVal[nType])
		m_nArrDiagVal[nType] = dwTime;
	
	LeaveCriticalSection(&m_CSPreDiag);
}

// ����͸� �α� �ʱ�ȭ
void DiagManager::ResetPreviosDiagCount()
{
	EnterCriticalSection(&m_CSPreDiag);
	memset(&m_nArrDiagVal, 0, sizeof(m_nArrDiagVal));
	LeaveCriticalSection(&m_CSPreDiag);
}

// ����͸� �α� �� ���
int DiagManager::GetPreviosValue(PREVIOS_DIAG_PARAM_NUM type)
{
	if(type >= COUNTOF_PREVIOUS || type < 0)
		return 0;
	
	int nRet = 0;
	EnterCriticalSection(&m_CSPreDiag);
	
	nRet = m_nArrDiagVal[type];
	
	LeaveCriticalSection(&m_CSPreDiag);
	
	return nRet;
}

// ����͸� �α� ��� �ð��� ���缭 ����ϵ��� üũ�ϴ� �Լ�
bool DiagManager::PrevLogTimeChk()
{
	if(m_dwPrevLogStartTick == 0)
		m_dwPrevLogStartTick = timeGetTime();  // ó���� ȣ������� �ʱ�ȭ

	DWORD dwElapse = OHTUTILITY::PreventOverflowGetTimeMs(m_dwPrevLogStartTick);
	const static int nInterval_ms = PREVIOUS_LOG_WRITE_INTERVAL_MIN*60*1000;
	if(dwElapse > nInterval_ms)
		return true;

	return false;
}

#pragma package(smart_init)

