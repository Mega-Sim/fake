/**
@file    ExtraJobThread.cpp
@version OHT 7.0
@brief   Data�� Update�ϴ� Thread�� ������ cpp file
*/

#pragma hdrstop
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include <windows.h>
#include <mmsystem.h>
#include <dir.h>
#include "ExtraJobThread.h"
#include "OHTMain.h"
#include "Def_DataFile_STD.h"
#include "MyTools.h"

//20.06.29 JHJ
#include <psapi.h>
#pragma link "Psapi.lib"
#include <dir.h>


// �α� �ۼ� �Լ�
#define ADD_LOG_EXTRAJOB(...)   WRITE_LOG_UNIT(pLogUnit, __VA_ARGS__)

//---------------------------------------------------------------------------
ExtraJobThread::ExtraJobThread() : ThreadCtrl()
{
	m_pMapMake_Status = OHTMainForm->GetMapMakeStatus();
	m_pTaskControl = OHTMainForm->GetTaskControl();
	//2013.08.06 doori.shin OHTMainStatus�߰�, Auto Update ��
    m_pOHTMainStatus = OHTMainForm->GetOHTStatus();

    // by zzang9un 2014.01.22 : Trace�� ���� HwSet�� ������
	m_pHwSet = OHTMainForm->GetHWSet();

	m_TempFlowControlManager_STD = NULL;
	m_TempMapManager_STD= NULL;
	m_TempStationManager_STD = NULL;
	m_TempTeachingManager_STD = NULL;
	m_TempPathDataManager_STD = NULL;
	m_TempMapMakerFileManager_STD = NULL;
	m_pTempMap = NULL;
	m_pTempMapData = NULL;
	m_pTempTeachingData = NULL;
	m_pTempStationData = NULL;
	m_TempCIDData_STD = NULL;
    m_TempCollisionData_STD = NULL;
	m_TempFlowControlData_STD = NULL;
	m_TempMapData_STD = NULL;
	m_TempStationData_STD = NULL;
	m_TempTeachingData_STD = NULL;
	m_TempPathData_STD = NULL;
    m_TempCIDManager_STD = NULL;
	m_TempCollisionManager_STD = NULL;

    gUpdateErrCnt = 0;

    pLogUnit = getLogUnit("D:\\log\\ExtraJobThread.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

    // by doori.shin 2013.08.06 : ó���� ��θ� ������...���־��ϱ�...
    strOHTPath = ExtractFileDir(Application->ExeName);

	UpdateDataStep = UPDATE_STATE_CHECK_FILE;
	UpdateDataStepParam = UPDATEPARAM_STATE_CHECK_FILE;

    // 2013.08.07 doori.shin
    m_pDataUseFlag = new DATA_USE_FLAG;
	memset(m_pDataUseFlag, false, sizeof(DATA_USE_FLAG));

	m_strFileParam[PARMA_DIAG] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_DIAG;
	m_strFileParam[PARMA_DRIVING] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_DRIVING;
	m_strFileParam[PARMA_EQPIO] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_EQPIO;
	m_strFileParam[PARMA_PATHSEARCH] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHSEARCH;
	m_strFileParam[PARMA_TRANS] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TRANS;
	m_strFileParam[PARMA_UBGCHECKNODES] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_UBGCHECKNODES;

	m_strFileParamDST[PARMA_DIAG] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_DIAG;
	m_strFileParamDST[PARMA_DRIVING] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_DRIVING;
	m_strFileParamDST[PARMA_EQPIO] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_EQPIO;
	m_strFileParamDST[PARMA_PATHSEARCH] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_PATHSEARCH;
	m_strFileParamDST[PARMA_TRANS] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_TRANS;
	m_strFileParamDST[PARMA_UBGCHECKNODES] = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_UBGCHECKNODES;

	m_nParamNum = -1;




}
//---------------------------------------------------------------------------

ExtraJobThread::~ExtraJobThread()
{
 	delete m_pDataUseFlag;
}
//---------------------------------------------------------------------------

void ExtraJobThread::RealThreadProc(void)
{
    int Error=0;
	String strUpdatePath = "";
	static int MapfileCheckCount = 0;
    //USB ���� ���� ��ġ ����
	static DWORD dwUpdateInterval = timeGetTime();
//	static CStepTimer timerIPCStat;

    while(!m_bThreadBreaker)
    {
        if(m_bThreadBreaker)
            break;  // Thread ���Ḧ ���� �ʼ��� �߰�

		//2014.04.15 doori.shin Thread Check�� ���� Tick�� ����
		if(m_Tick == MAX_TICK)
			m_Tick = 0;
		else
			m_Tick++;

        if(m_pOHTMainStatus->fMainReady)
        {
            // -------------------------------------------
            // Step 1. Data Backup : Map Make�� ���۵Ǹ� ����Ѵ�.

            // Map Data Backup
			// Station Data Backup
            if(m_pMapMake_Status->bStartBackup_Station || m_pMapMake_Status->bStartBackup_Map)
			{

				BackupMapsetFiles();
				m_pMapMake_Status->bStartBackup_Map = false;
				m_pMapMake_Status->bStartBackup_Station = false;
			}

			// Step 2. Map Make �Ϸ� �� ���� ���� ---> �����н� �Ϸ� �� ??
			// Map File ����
            if(m_pMapMake_Status->bMapChanged && m_pMapMake_Status->bComplete)
            {
                SaveMapData();
				ADD_LOG_EXTRAJOB("MapMaker Map");
//				SaveMapsetFiles();
				m_pMapMake_Status->bMapChanged = false;
            }

            // Station Data File ����
            if(IsStationDataModified() && m_pMapMake_Status->bComplete)
			{
				ADD_LOG_EXTRAJOB("MapMaker Station");
                SaveStationData();
            }
            // -------------------------------------------

            // Teaching Data üũ �� ����
            if(IsTeachingDataModified())
            {
				//���� ����
				ADD_LOG_EXTRAJOB("MapMaker Teahcing");
                SaveTeachingData();
            }

            //Map ���� Update
            //2013.08.06 doori.shin Update������ ���� ���� Ȯ���� ������Ʈ ����
			if(UpdateDataStep == UPDATE_STATE_CHECK_FILE) //�۾��������̾�����..
            {
				//Error = FindFirst(strUpdatePathA, faAnyFile-faDirectory, sr);
				int iRet = FindMapsetFiles();

				if(iRet == TOTAL_MAPSET_FILES)
				{
					MapfileCheckCount = 0;
					//if(CheckFileCopyCompleted(sr.Name)==true)
					if(CheckFileCopyCompleted()==true)
					{
						AutoUpdate(&sr);
					}
				}
				else if(iRet > 0 && iRet < TOTAL_MAPSET_FILES)
				{
					MapfileCheckCount++;
					//ADD_LOG_EXTRAJOB("[MAP]DeleteMapsetFile : %d",MapfileCheckCount);
					if(MapfileCheckCount >= 2000) // 50ms X 2000 �� 100��
					{
						ADD_LOG_EXTRAJOB("[MAP]DeleteMapsetFile : %d",iRet);
						DeleteMapsetFile();
					}
				}
				else
				{
					MapfileCheckCount = 0;
				}

            }
            else
            {
				AutoUpdate(&sr);
				MapfileCheckCount = 0;
            }


			//by puting 2019 05 13 : param update
			if(UpdateDataStepParam == UPDATEPARAM_STATE_CHECK_FILE) //�۾��������̾�����..
            {
				int iRet = FindParamsetFiles();
				if(iRet != -1)
				{
                    //if(CheckFileCopyCompleted(sr.Name)==true)
					if(CheckFileCopyCompletedParam()==true)
					{
						if(m_nParamNum != -1)
							AutoUpdateParam();
					}
                }

            }
            else
			{
				if(m_nParamNum != -1)
					AutoUpdateParam();
			}


            // by zzang9un 2014.01.22 : Trace �α� ���� flag ����
            if(m_pOHTMainStatus->bTraceStart)
            {
                LARGE_INTEGER liCounter1, liCounter2, liFrequency;
                QueryPerformanceFrequency(&liFrequency); // retrieves the frequency of the high-resolution performance counter
                QueryPerformanceCounter(&liCounter1); // Start
                
                m_pHwSet->MakeTraceLog();
                m_pOHTMainStatus->bTraceStart = false;

                QueryPerformanceCounter(&liCounter2); // End

                // by zzang9un 2014.01.22 : �ӽ� ����� �α�
                ADD_LOG_EXTRAJOB("Trace Logging End, Time:%f", (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);        	
            }
		}


		//USB ���� ���� ��ġ ����
		if(OHTUTILITY::PreventOverflowGetTimeMs(dwUpdateInterval) > 60*60*1000)
		{
			if(gm_LogManager!=NULL)
			{
				if((OHTUTILITY::PreventOverflowGetTimeMs(m_pOHTMainStatus->dwLogThreadTime) < 30000) && (gm_LogManager->m_bLogfileCheck == true)) //LogTHread�� 30�� �̸��϶��� ����
				{
					DiagManager::getInstance()->CheckOHTStatus();
					m_pOHTMainStatus->nfMemorySize = CheckMemoryInfo(); //�޸𸮸���
				}else
				{
					DiagManager::getInstance()->CheckOHTStatusFail();
                }

			}
			dwUpdateInterval = timeGetTime();
		}

//		if(timerIPCStat.OnDelay(true, 1000))
//		{
//			timerIPCStat.OnDelay(false);
//
//			Get_IPCSysInfo(&m_pOHTMainStatus->m_cpMonitorData.MonitorOperStatus.IPCSysInfo);
//        }

        //AOHC-204 Thread �� ���� �ð� ���� ���
		static TimeChk timeChk;
		static ThreadTimeMonitor tmon;
		timeChk.End(0);

		tmon.Execute("ExtJobT", timeChk.Get(0));

		timeChk.Start(0);

        Sleep(50);
    }
}
//---------------------------------------------------------------------------
/*
int ExtraJobThread::BackUpMapData()
{
    TDateTime tdt;
    tdt = tdt.CurrentDateTime();

    // ��� ������ �ð� ������ ���ϸ� �����Ѵ�.
	String strFileName = "";
    strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "map_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";

	OHTMainForm->GetMapDataManager()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetMap());

	ADD_LOG_EXTRAJOB("[MapMake] Map File Backup Complete...");

    return 0;
}
*/
//---------------------------------------------------------------------------
/*
int ExtraJobThread::BackUpStationData()
{
    int Error = NO_ERR;

    TDateTime tdt;
    tdt = tdt.CurrentDateTime();

    // ��� ������ �ð� ������ ���ϸ� �����Ѵ�.
	String strFileName = "";
	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "station_" + tdt.FormatString("yyyymmddhhmmss") + ".std";

//	Error = OHTMainForm->GetStationDataManager()->SaveDataToFile(strFileName);

    if(Error == NO_ERR)
    {
        ADD_LOG_EXTRAJOB("[MapMake] Station Data File Backup Complete...");
    }
    else
    {
        ADD_LOG_EXTRAJOB("[MapMake] Station Data File Backup Fail... Error:%d", Error);
    }

    return 0;
}
*/
//---------------------------------------------------------------------------

int ExtraJobThread::SaveMapData()
{
    // Map�� ����Ǿ����� MapMake�� ���� �� Map ������ �����Ѵ�.
	String strFileName = "";
    strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_MAP;

	OHTMainForm->GetMapDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetMap_STD());

	ADD_LOG_EXTRAJOB("[MapMake] Map File Update Complete...");

	return 0;
}
//---------------------------------------------------------------------------

int ExtraJobThread::SaveStationData()
{
    int Error = NO_ERR;

    // Map�� ����Ǿ����� MapMake�� ���� �� Map ������ �����Ѵ�.
	String strFileName = "";
    strFileName = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_STATION;

    // ������ �Ǹ� m_bDataModified flag�� �ڵ����� false�� ��
	Error = OHTMainForm->GetStationDataManager()->SaveDataToFile(strFileName, DATATYPE_STATION);

    if(Error == NO_ERR)
    {
        ADD_LOG_EXTRAJOB("[MapMake] Station Data File Update Complete...");
    }
    else
    {
        ADD_LOG_EXTRAJOB("[MapMake] Station Data File Update Fail... Error:%d", Error);
    }

    return 0;
}
//---------------------------------------------------------------------------

int ExtraJobThread::SaveTeachingData()
{
    // by zzang9un 2013.07.16 : Data ���� ���θ� Manager ���� ������ ������
    ///@todo ���� �ڵ� ó���ؾ� ��
	//���� ��� �κ� ����.  m_pMapMake_Status->bStartBackup_Map �������� ���� ???
	int Error = NO_ERR;
	String strOHTPath = "", strPath = "";

    strOHTPath = ExtractFileDir(Application->ExeName);
    strPath = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_TEACHING;
    //TEACHING ��������
	Error = OHTMainForm->GetTeachingDataManager()->SaveDataToFile(strPath, DATATYPE_TEACHING);
	if(Error == NO_ERR)
	{	
		ADD_LOG_EXTRAJOB("[MapMake] Teaching Data File Update Complete...");
	}
	else
	{
		ADD_LOG_EXTRAJOB("[MapMake] Teaching Data File Update Fail [%d]...",Error);
	}
    return Error;
    
}
//---------------------------------------------------------------------------

bool ExtraJobThread::IsTeachingDataModified()
{
	return OHTMainForm->GetTeachingDataManager()->m_bDataModified;
}
//---------------------------------------------------------------------------

bool ExtraJobThread::IsStationDataModified()
{
	return OHTMainForm->GetStationDataManager()->m_bDataModified;
}
//---------------------------------------------------------------------------

inline int ExtraJobThread::SwapUpdateData()
{
	int Error=0;

			try{
			StationDataManager* deleteStation =	OHTMainForm->GetStationDataManager();
			OHTMainForm->SetStationDataManager(m_pTempStationData);
			m_pTempStationData = NULL;
			delete deleteStation;


			TeachingDataManager* deleteTeaching = OHTMainForm->GetTeachingDataManager();
			OHTMainForm->SetTeachingDataManager(m_pTempTeachingData);
			m_pTempTeachingData = NULL;
			delete deleteTeaching;


			MAP* deleteMap = OHTMainForm->GetMap();
			OHTMainForm->SetMap(m_pTempMap);
			m_pTempMap = NULL;
			delete deleteMap;  //������ �޸�.

			MapDataManager* deleteMapData = OHTMainForm->GetMapDataManager();
			OHTMainForm->SetMapDataManager(m_pTempMapData);
			m_pTempMapData = NULL;
			delete deleteMapData;

			///STD Data
#if(OHT_NAME != OHT_NAME_P4_MASK)
			STD_CID* deleteCIDData = OHTMainForm->GetCID_STD();
			OHTMainForm->SetCID_STD(m_TempCIDData_STD);
			m_TempCIDData_STD = NULL;
			delete deleteCIDData;
#endif

			STD_Collision* deleteCollisionData = OHTMainForm->GetCollision_STD();
			OHTMainForm->SetCollision_STD(m_TempCollisionData_STD);
			m_TempCollisionData_STD = NULL;
			delete deleteCollisionData;

			STD_FlowControl* deleteFlowControlData = OHTMainForm->GetFlowControl_STD();
			OHTMainForm->SetFlowControl_STD(m_TempFlowControlData_STD);
			m_TempFlowControlData_STD = NULL;
			delete deleteFlowControlData;

			STD_MAP* deleteMapDataData = OHTMainForm->GetMap_STD();
			OHTMainForm->SetMap_STD(m_TempMapData_STD);
			m_TempMapData_STD = NULL;
			delete deleteMapDataData;

			STD_Station* deleteSTD_StationData = OHTMainForm->GetStation_STD();
			OHTMainForm->SetStation_STD(m_TempStationData_STD);
			m_TempStationData_STD = NULL;
			delete deleteSTD_StationData;

			STD_Teaching* deleteSTD_TeachingData = OHTMainForm->GetTeaching_STD();
			OHTMainForm->SetTeaching_STD(m_TempTeachingData_STD);
			m_TempTeachingData_STD = NULL;
			delete deleteSTD_TeachingData;

			STD_PathData* deletePathData = OHTMainForm->GetPathData_STD();
			OHTMainForm->SetPathData_STD(m_TempPathData_STD);
			m_TempPathData_STD = NULL;
			delete deletePathData;


#if(OHT_NAME != OHT_NAME_P4_MASK)
			CIDManager_STD* deleteCIDDataManagerData = OHTMainForm->GetCIDDataManager_STD();
			OHTMainForm->SetCIDDataManager_STD(m_TempCIDManager_STD);
			m_TempCIDManager_STD = NULL;
			delete deleteCIDDataManagerData;
#endif
			CollisionManager_STD* deleteCollisionManagerData = OHTMainForm->GetCollisionDataManager_STD();
			OHTMainForm->SetCollisionDataManager_STD(m_TempCollisionManager_STD);
			m_TempCollisionManager_STD = NULL;
			delete deleteCollisionManagerData;

			FlowControlManager_STD* deleteFlowControlManagerData = OHTMainForm->GetFlowControlDataManager_STD();
			OHTMainForm->SetFlowControlDataManager_STD(m_TempFlowControlManager_STD);
			m_TempFlowControlManager_STD = NULL;
			delete deleteFlowControlManagerData;

			MapDataManager_STD* deleteMapDataManagerData = OHTMainForm->GetMapDataManager_STD();
			OHTMainForm->SetapDataManager_STD(m_TempMapManager_STD);
			m_TempMapManager_STD = NULL;
			delete deleteMapDataManagerData;

			StationManager_STD* deleteStationManagerData = OHTMainForm->GetStationDataManager_STD();
			OHTMainForm->SetStationDataManager_STD(m_TempStationManager_STD);
			m_TempStationManager_STD = NULL;
			delete deleteStationManagerData;

			TeachingManager_STD* deleteTeachingManagerData = OHTMainForm->GetTeachingDataManager_STD();
			OHTMainForm->SetTeachingDataManager_STD(m_TempTeachingManager_STD);
			m_TempTeachingManager_STD = NULL;
			delete deleteTeachingManagerData;

			PathDataManager_STD* deletePathDataManagerData = OHTMainForm->GetPathDataManager_STD();
			OHTMainForm->SetPathDataManager_STD(m_TempPathDataManager_STD);
			m_TempPathDataManager_STD = NULL;
			delete deletePathDataManagerData;


			//autu Teaching ���� ���� �߰�
			MapMakerFileManager_STD* deleteMapMakerFileManagerData = OHTMainForm->GetMapMakerFileManager_STD();
			OHTMainForm->SetMapMakerFileManager_STD(m_TempMapMakerFileManager_STD);
			m_TempMapMakerFileManager_STD = NULL;
			delete deleteMapMakerFileManagerData;

			UpdateDataStep = UPDATE_STATE_END_UPDATE;

			}
			catch(...)
			{
              	ADD_LOG_EXTRAJOB("[Update] Update Exception!!!");
            }


//		}
//
//	}





    return Error;
}

/*
inline int ExtraJobThread::DeleteUpdateTempData()
{
	int Error=0;

	if(m_pTempStationData != NULL)
	{
		delete m_pTempStationData;
	}

	if(m_pTempTeachingData != NULL)
	{
		delete m_pTempTeachingData;
	}

	if(m_pTempMap != NULL)
	{
		delete m_pTempMap;
	}

	if(m_pTempMapData != NULL)
	{
		delete m_pTempMapData;
	}

	if(m_TempCIDData_STD != NULL)
	{
		delete m_TempCIDData_STD;
	}

	if(m_TempCollisionData_STD != NULL)
	{
		delete m_TempCollisionData_STD;
	}

	if(m_TempFlowControlData_STD != NULL)
	{
		delete m_TempFlowControlData_STD;
	}

	if(m_TempMapData_STD != NULL)
	{
		delete m_TempMapData_STD;
	}

	if(m_TempStationData_STD != NULL)
	{
		delete m_TempStationData_STD;
	}

	if(m_TempTeachingData_STD != NULL)
	{
		delete m_TempTeachingData_STD;
	}

	if(m_TempPathData_STD != NULL)
	{
		delete m_TempPathData_STD;
	}



	if(m_TempCIDManager_STD != NULL)
	{
		delete m_TempCIDManager_STD;
	}

	if(m_TempCollisionManager_STD != NULL)
	{
		delete m_TempCollisionManager_STD;
	}

	if(m_TempFlowControlManager_STD != NULL)
	{
		delete m_TempFlowControlManager_STD;
	}

	if(m_TempMapManager_STD != NULL)
	{
		delete m_TempMapManager_STD;
	}

	if(m_TempStationManager_STD != NULL)
	{
		delete m_TempStationManager_STD;
	}

	if(m_TempTeachingManager_STD != NULL)
	{
		delete m_TempTeachingManager_STD;
	}

	if(m_TempPathDataManager_STD != NULL)
	{
		delete m_TempPathDataManager_STD;
	}

    return Error;
}
*/
//---------------------------------------------------------------------------

/**
@brief   Update ������ ������ ������� ������Ʈ�� ����
@author  doori.shin
@date    2013.08.07
*/
//7.0 ���� ���� �����ϸ鼭 �޸� Swap
//8.0 ��ü ���Ϸ� ���� ���� �ѹ��� �޸� Swap
int ExtraJobThread::AutoUpdate(TSearchRec* pSchRec)
{
    int Error = 0;
	static unsigned char ErrorCount=0;
	static bool bSecondUpdate = false;
	String strFilePath = String(strOHTPath + STR_DATAFILE_UPDATE_PATH + pSchRec->Name);
	String strFileExt = "";
	String strOHTPath = "", strFileName = "", strFileNameDST = "";
	switch(UpdateDataStep)
    {
    //Step 1. File ��ȿ�� �˻�(Ÿ��, ũ��)
	case UPDATE_STATE_CHECK_FILE:
	if(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist)
	{
		switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
			//������Ʈ ������ ����
		case STS_INIT:
		case STS_ARRIVED:
		case STS_UNLOADED:
		case STS_LOADED:
		case STS_COMPLETE:
		case STS_ERROR:
		case STS_TURN_OFF:
		case STS_AUTORECOVERY:
		case STS_AUTORECOVERY_BLANKTRANS:
		case STS_AUTORECOVERY_DOUBLETRANS:
		case STS_AUTORECOVERY_OVERRUN:
			{
                int TestUpdate = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status; //AOHC-233 MapUpdate ���� ����
                ADD_LOG_EXTRAJOB("[AutoUpdate]: %d", TestUpdate);

					// MAPSET ���� ����� Assistant���� ���� ī�Ƿ� ��ü ����! ??
				if((m_pOHTMainStatus->DataUseFlag.fStation)
					|| (m_pOHTMainStatus->DataUseFlag.fMap)
					|| (m_pOHTMainStatus->DataUseFlag.fTeaching))
				{
					ADD_LOG_EXTRAJOB("[Update] Update is Blocked. Map : %d, Station : %d, Teaching : %d",
						m_pOHTMainStatus->DataUseFlag.fMap, m_pOHTMainStatus->DataUseFlag.fStation, m_pOHTMainStatus->DataUseFlag.fTeaching);
					Error = ERR_DATA_UPDATE_BLOCKED;
				}
				else
				{
					if(m_pOHTMainStatus->DataUseFlag.fSwapping == true)
					{
//						m_pOHTMainStatus->DataUseFlag.fSwapping = true;

						ADD_LOG_EXTRAJOB("[Update] (Step %d) Update file check and Backup", UPDATE_STATE_CHECK_FILE);
						gUpdateErrCnt = 0;
						//�������� ���
						//MAPSET ���ϵ��� ũ��� Assistant���� Ȯ���Ͽ� ������Ʈ ������ ��������.
						DeletebackupFile();
						BackupMapsetFiles();
						UpdateDataStep = UPDATE_STATE_FILE_LOAD;
						m_pOHTMainStatus->DataUseFlag.fReadySwapping = false;
					}


				}
			}
		}
	}
		//AOHC-233 MapUpdate ���� ����
        switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
        {
            case STS_UNLOADING:
            case STS_LOADING:
            {
                int TestUpdate = m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status;
                ADD_LOG_EXTRAJOB("[Load/Unload]: %d", TestUpdate);
                if((m_pOHTMainStatus->DataUseFlag.fStation)
					|| (m_pOHTMainStatus->DataUseFlag.fMap)
					|| (m_pOHTMainStatus->DataUseFlag.fTeaching))
				{
                    ADD_LOG_EXTRAJOB("[Update] Update is Blocked. Map : %d, Station : %d, Teaching : %d",
                    m_pOHTMainStatus->DataUseFlag.fMap, m_pOHTMainStatus->DataUseFlag.fStation, m_pOHTMainStatus->DataUseFlag.fTeaching);
					Error = ERR_DATA_UPDATE_BLOCKED;
                }
                else
				{
					if(m_pOHTMainStatus->DataUseFlag.fSwapping == true)
					{
//						m_pOHTMainStatus->DataUseFlag.fSwapping = true;

						ADD_LOG_EXTRAJOB("[Update] (Step %d) Update file check and Backup", UPDATE_STATE_CHECK_FILE);
						gUpdateErrCnt = 0;
						//�������� ���
						//MAPSET ���ϵ��� ũ��� Assistant���� Ȯ���Ͽ� ������Ʈ ������ ��������.
						DeletebackupFile();
						BackupMapsetFiles();
						UpdateDataStep = UPDATE_STATE_FILE_LOAD;
						m_pOHTMainStatus->DataUseFlag.fReadySwapping = false;
					}


				}

            }

        }

        m_pOHTMainStatus->DataUseFlag.fReadySwapping = true;
		//UpdateDataStep = UPDATE_STATE_FILE_COPY;
		break;

    //Step 2. Temp Buffer�� ������ �����͸� Loading
	case UPDATE_STATE_FILE_LOAD:
	{
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : Start", UPDATE_STATE_FILE_LOAD);

		//String strOHTPath = "", strFileName = "";
		strOHTPath = ExtractFileDir(Application->ExeName);
#if(OHT_NAME != OHT_NAME_P4_MASK)

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;

		m_TempCIDData_STD = new STD_CID();
		m_TempCIDManager_STD = new  CIDManager_STD();

		Error = m_TempCIDManager_STD->LoadMapFile(strFileName.c_str(),m_TempCIDData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 1 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
        {   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }
#endif
		m_TempCollisionData_STD = new STD_Collision();
		m_TempCollisionManager_STD = new  CollisionManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
		Error = m_TempCollisionManager_STD->LoadMapFile(strFileName.c_str(),m_TempCollisionData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 2 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
        {   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		m_TempFlowControlData_STD = new STD_FlowControl();
		m_TempFlowControlManager_STD = new  FlowControlManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
		Error = m_TempFlowControlManager_STD->LoadMapFile(strFileName.c_str(),m_TempFlowControlData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 3 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
        {   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		m_TempMapData_STD = new STD_MAP();
		m_TempMapManager_STD = new  MapDataManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
		Error = m_TempMapManager_STD->LoadMapFile(strFileName.c_str(),m_TempMapData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 4 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
        {   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		m_TempStationData_STD = new STD_Station();
		m_TempStationManager_STD = new  StationManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
		Error = m_TempStationManager_STD->LoadMapFile(strFileName.c_str(),m_TempStationData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 5 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
		{   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }

		m_TempTeachingData_STD = new STD_Teaching();
		m_TempTeachingManager_STD = new  TeachingManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
		Error = m_TempTeachingManager_STD->LoadMapFile(strFileName.c_str(),m_TempTeachingData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 6 E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
        {   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		m_TempPathData_STD = new STD_PathData();
		m_TempPathDataManager_STD = new  PathDataManager_STD();

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;
		Error = m_TempPathDataManager_STD->LoadMapFile(strFileName.c_str(),m_TempPathData_STD);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : 7 End E%d", UPDATE_STATE_FILE_LOAD, Error);
		Sleep(0);
		if(Error == NO_ERR)//Success
		{   //NOP
		}
		else//Fail
		{
			ErrorCount += 1;
		}

		//Node ���� Collision ���� ����ġ�� Error ó��
		if(m_TempCollisionData_STD->m_NodeNum != m_TempMapData_STD->m_NodeNum)
		{
			ErrorCount += 1;
		}
		//////////////////////////////////////
		//Map Maker File Load(Auto Teaching)//
		//////////////////////////////////////
		m_TempMapMakerFileManager_STD = new  MapMakerFileManager_STD();
		strOHTPath = ExtractFileDir(Application->ExeName);
		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MapMakerFile;
		m_TempMapMakerFileManager_STD->LoadDataFromFile(strFileName);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : MapMakerFile", UPDATE_STATE_FILE_LOAD);
		Sleep(0);

//		m_TempMapMakerFileManager_STD = new  MapMakerFileManager_STD();
//		strOHTPath = ExtractFileDir(Application->ExeName);
//		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_JobOrderFile;
//		m_TempMapMakerFileManager_STD->LoadDataFromFile(strFileName);
//		ADD_LOG_EXTRAJOB("[Update] (Step %d) Mapset File Load : MapMakerFile", UPDATE_STATE_FILE_LOAD);
//		Sleep(0);


		if(ErrorCount == 0)//Success
        {
            m_pTempStationData = new StationDataManager;
			m_pTempTeachingData = new TeachingDataManager;
			m_pTempMapData = new MapDataManager;
			m_pTempMap = new MAP;


			// Map Data
			UINT uiDataTmp = m_TempMapManager_STD->m_MapFileHeader.DataCount;
			if( (uiDataTmp <= NODE_COUNT_MAX ) && (uiDataTmp>0) )
			{
				//AOHC-285
#if(OHT_NAME == OHT_NAME_P4_MASK)
				Error = m_pTempMapData->LoadDataFromSTDMap(
						m_pTempMap,
						m_TempMapData_STD->m_NodeArray,
						m_TempCollisionData_STD->m_NodeArray,
						m_TempFlowControlData_STD->m_NodeArray,
						/*m_TempCIDData_STD->m_NodeArray,*/
						OHTMainForm->GetMapMakeProcess(),
						OHTMainForm->GetLineParam()->bUseOptionalNode,
						m_TempMapManager_STD->m_MapFileHeader.DataCount,
						m_TempFlowControlManager_STD->m_FlowControlFileHeader.DataCount
						/*,m_TempCIDManager_STD->m_CIDFileHeader.DataCount*/);
#else
				Error = m_pTempMapData->LoadDataFromSTDMap(
						m_pTempMap,
						m_TempMapData_STD->m_NodeArray,
						m_TempCollisionData_STD->m_NodeArray,
						m_TempFlowControlData_STD->m_NodeArray,
						m_TempCIDData_STD->m_NodeArray,
						OHTMainForm->GetMapMakeProcess(),
						OHTMainForm->GetLineParam()->bUseOptionalNode,
						m_TempMapManager_STD->m_MapFileHeader.DataCount,
						m_TempFlowControlManager_STD->m_FlowControlFileHeader.DataCount,
						m_TempCIDManager_STD->m_CIDFileHeader.DataCount);

#endif

				if(Error != NO_ERR)
				{
					ADD_LOG_EXTRAJOB("[Update] (Step %d) GetMapDataManager_STD LoadDataFromSTDMap Validation Error", UPDATE_STATE_FILE_LOAD);
					UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
					break;
				}
			}
			else
			{
				ADD_LOG_EXTRAJOB("[Update] (Step %d) GetMapDataManager_STD LoadDataFromSTDMap NODE_COUNT err :  %d", UPDATE_STATE_FILE_LOAD, uiDataTmp);
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				break;
			}
			ADD_LOG_EXTRAJOB("[Update] (Step %d) GetMapDataManager_STD LoadDataFromSTDMap : End %d", UPDATE_STATE_FILE_LOAD,Error);
			Sleep(0);
			// Station Data
			UINT uiDataTmp_Station = m_TempStationManager_STD->m_StationFileHeader.DataCount;
			if( (uiDataTmp_Station <= NODE_COUNT_MAX ) && (uiDataTmp_Station>0) )
			{
				Error = m_pTempStationData->LoadDataFromSTDMap(
						m_pTempMap,
						m_TempStationData_STD->m_NodeArray, 
						uiDataTmp_Station);
				if(Error != NO_ERR)
				{
					ADD_LOG_EXTRAJOB("[Update] (Step %d) GetStationDataManager_STD LoadDataFromSTDMap Validation Error", UPDATE_STATE_FILE_LOAD);
					UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
					break;
				}
			}
			else
			{
				ADD_LOG_EXTRAJOB("[Update] (Step %d) GetStationDataManager_STD LoadDataFromSTDMap NODE_COUNT err :  %d", UPDATE_STATE_FILE_LOAD, uiDataTmp_Station);
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				break;
			}
			ADD_LOG_EXTRAJOB("[Update] (Step %d) GetStationDataManager_STD LoadDataFromSTDMap : End %d", UPDATE_STATE_FILE_LOAD,Error);
			Sleep(0);

			// Teaching Data
			UINT uiDataTmp_Teaching = m_TempTeachingManager_STD->m_TeachingFileHeader.DataCount;

			if( (uiDataTmp_Teaching <= NODE_COUNT_MAX ) && (uiDataTmp_Teaching>0) )
			{
				Error = m_pTempTeachingData->LoadDataFromSTDMap(
						m_TempTeachingData_STD->m_NodeArray,
						uiDataTmp_Teaching);
				if(Error != NO_ERR)
				{
					ADD_LOG_EXTRAJOB("[Update] (Step %d) GetTeachingDataManager_STD LoadDataFromSTDMap Validation Error", UPDATE_STATE_FILE_LOAD);
					UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
					break;
				}
			}
			else
			{
				ADD_LOG_EXTRAJOB("[Update] (Step %d) GetTeachingDataManager_STD LoadDataFromSTDMap NODE_COUNT err :  %d", UPDATE_STATE_FILE_LOAD, uiDataTmp_Teaching);
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				break;
			}
			ADD_LOG_EXTRAJOB("[Update] (Step %d) GetTeachingDataManager_STD LoadDataFromSTDMap %d", UPDATE_STATE_FILE_LOAD,Error);
			Sleep(0);

			if(Error)//Fail
			{
				ADD_LOG_EXTRAJOB("[Update] (Step %d) End File Loading : Fail", UPDATE_STATE_FILE_LOAD);
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				break;
			}
	        else//Success
			{
	            ADD_LOG_EXTRAJOB("[Update] (Step %d) End File Loading : Success", UPDATE_STATE_FILE_LOAD);
				UpdateDataStep = UPDATE_STATE_FILE_COPY;
			}
        }
		else//Fail
		{
			ADD_LOG_EXTRAJOB("[Update] (Step %d) ERR_UPDATE_DATA_UPDATE_INVALID_FILE", UPDATE_STATE_FILE_LOAD);

			if(gUpdateErrCnt < 3)
			{
				gUpdateErrCnt += 1;
				Error = 0;
				ErrorCount = 0;  //AOHC-289
				UpdateDataStep = UPDATE_STATE_FILE_LOAD;
			}
			else
			{
				Error = UPDATE_STATE_ERR;
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				gUpdateErrCnt = 0;
				ErrorCount = 0;
			}
		}

	}
        break;

	//Step 3. �������Ͽ� ����� : Update -> MAPSET ������ ����
	// MAPSET ���� ����� Assistant���� ���� ī�Ƿ� ��ü ����! ??
	// OHT Status ������ �����Ͽ� ������ ����� �޴� ������(OHT->AS->FM,���Ϻ���->AS->OHT)
	// OHT Main ���� �����ϴ� �ͺ��� �ð��� �����.
	// ���� ���� �� Thread�� ������ ��ȯ�Ͽ� OCS ����� ���� �ϸ� ��.
	case UPDATE_STATE_FILE_COPY:
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File Copying", UPDATE_STATE_FILE_COPY);


		//������Ʈ ���� �������Ͽ� �����
		//Error = CopyFile(strFilePath.w_str(), String(strOHTPath+STR_DATAFILE_PATH+pSchRec->Name).w_str(), false);
#if 1
		strOHTPath = ExtractFileDir(Application->ExeName);
#if(OHT_NAME != OHT_NAME_P4_MASK)
		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_CID;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 1", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
        {   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }
#endif
		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_COLLISION;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 2", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
        {   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_FLOWCONTROL;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 3", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
        {   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_MAP;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 4", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
		{   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_STATION;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 5", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
        {   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_TEACHING;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 6", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
        {   //NOP
        }
		else//Fail
		{	ErrorCount += 1; }

 		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_PATHDATA;
		Error = CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File : 7", UPDATE_STATE_FILE_COPY);
		Sleep(0);
		if(Error)//Success
		{   //NOP
		}
		else//Fail
		{	ErrorCount += 1; }

		//MapMaker file  Copy
		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MapMakerFile;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_MapMakerFile;
		CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File Copy: MapMakerFile", UPDATE_STATE_FILE_COPY);
		Sleep(0);

		strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_JobOrderFile;
		strFileNameDST = String(strOHTPath + STR_DATAFILE_PATH_MAPSET) + STR_FILENAME_JobOrderFile;
		CopyFile(strFileName.w_str(), strFileNameDST.w_str(), false);
		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start File Copy: MapMakerFile", UPDATE_STATE_FILE_COPY);
		Sleep(0);


#endif
        if(ErrorCount == 0)//Success
        {
            ADD_LOG_EXTRAJOB("[Update] (Step %d) End File Copying", UPDATE_STATE_FILE_COPY);
			UpdateDataStep = UPDATE_STATE_DATA_SWAP;
            Error = 0;
        }
		else//Fail
		{
			ADD_LOG_EXTRAJOB("[Update] (Step %d) ERR_DATA_UPDATE_COPY_FAIL", UPDATE_STATE_FILE_COPY);

			if(gUpdateErrCnt < 3)
			{
				gUpdateErrCnt += 1;
				Error = 0;
				ErrorCount = 0;  //AOHC-289
				UpdateDataStep = UPDATE_STATE_FILE_COPY;
			}
			else
			{
				Error = UPDATE_STATE_ERR;
				UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
				DeleteMapsetFile();
				gUpdateErrCnt = 0;
				ErrorCount = 0;
			}
		}

        break;

    //Step 4. OHT Status üũ �� ������ �ΰ�
    case UPDATE_STATE_DATA_SWAP:
        //Mode üũ


		ADD_LOG_EXTRAJOB("[Update] (Step %d) Start Data Swapping ( %d Mode)", UPDATE_STATE_DATA_SWAP,m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode);
		Error = SwapUpdateData();
		//UpdateDataStep = UPDATE_STATE_END_UPDATE;

		if(bSecondUpdate == false)
		{
			bSecondUpdate = true;
			UpdateDataStep = UPDATE_STATE_FILE_LOAD;
		}
		else
		{
			UpdateDataStep = UPDATE_STATE_END_UPDATE;
			bSecondUpdate = false;
		}

		if(Error) break;

		ADD_LOG_EXTRAJOB("[Update] (Step %d) End Data Swapping ( %d Mode)", UPDATE_STATE_DATA_SWAP,m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Mode);


		break;
	//���� �۾������� ������ SwapUpdateData()�Լ� ������ ������...
	//Step 5. �۾�����
	case UPDATE_STATE_END_UPDATE:
	{
	    ADD_LOG_EXTRAJOB("[Update] (Step %d) Success to Update", UPDATE_STATE_END_UPDATE);
		//delete m_pFileInfo;


		UpdateDataStep = UPDATE_STATE_CHECK_FILE;
		DeleteMapsetFile();
		// MAPSET ���� ���� �� ������ ���
		// OCS�� ������ ���� ��ô ���� ���� (����)
		OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_SUCCESS;
		OHTMainForm->m_MapUpdateResult = NO_ERR;
		m_pOHTMainStatus->DataUseFlag.fSwapping = false;
		m_pOHTMainStatus->DataUseFlag.fReadySwapping = false;

	}
        break;
	case UPDATE_STATE_UPDATE_FAIL:
		switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
				//������Ʈ ������ ���¿� ������ ���¿��� �� ���� �߻� ��Ŵ
			case STS_INIT:
			case STS_ARRIVED:
			case STS_UNLOADED:
			case STS_LOADED:
			case STS_COMPLETE:
			case STS_ERROR:
			case STS_TURN_OFF:
			{
				//�� �˻�� Error �߻��ϸ� ���� ��� ó�� ���� ������ ����
				//�̰����� Error �߻�
				OHTMainForm->m_MapUpdateResult = ERR_MAPDATA_LOAD_FAIL;
				UpdateDataStep = UPDATE_STATE_CHECK_FILE;
				ADD_LOG_EXTRAJOB("[Update] (Step %d) UPDATE_STATE_UPDATE_FAIL (Manual Mode)", UPDATE_STATE_UPDATE_FAIL);
//				DeleteUpdateTempData();
				DeleteMapsetFile();

				m_pOHTMainStatus->DataUseFlag.fSwapping = false;
				m_pOHTMainStatus->DataUseFlag.fReadySwapping = false;
				
			}

		}
		break;
	//Default
    default:
        break;
    }

    if(Error)
    {
        ADD_LOG_EXTRAJOB("[Update] In Step %d, Error occured. [ErrCode : %d]", UpdateDataStep, Error);
        //ADD_LOG_EXTRAJOB("[Update] Be Deleted Update File and Started the First Step Again..");

        //delete m_pFileInfo;
		UpdateDataStep = UPDATE_STATE_UPDATE_FAIL;
		DeleteMapsetFile();
        //DeleteFile(strFilePath);

		// MAPSET ���� ���� �� ������ ���
		// OCS�� ������ ���� ��ô ���� ���� (����)
		OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_FAIL;
		m_pOHTMainStatus->DataUseFlag.fSwapping = false;
		m_pOHTMainStatus->DataUseFlag.fReadySwapping = false;
    }

	return Error;

}
//---------------------------------------------------------------------------


/**
@brief   Update ������ ������ ������� ������Ʈ�� ����
@author  doori.shin
@date    2013.08.07
*/
//7.0 ���� ���� �����ϸ鼭 �޸� Swap
//8.0 ��ü ���Ϸ� ���� ���� �ѹ��� �޸� Swap
int ExtraJobThread::AutoUpdateParam()
{
	int Error = 0;

	switch(UpdateDataStepParam)
    {
    //Step 1. File ��ȿ�� �˻�(Ÿ��, ũ��)
	case UPDATEPARAM_STATE_CHECK_FILE:
		if(m_pOHTMainStatus->CurCycle == CYCLE::CycleNotExist)
		{
			switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
			{
				//������Ʈ ������ ����
			case STS_INIT:
			case STS_ARRIVED:
			case STS_UNLOADED:
			case STS_LOADED:
			case STS_COMPLETE:
			case STS_ERROR:
			case STS_TURN_OFF:
			case STS_AUTORECOVERY:
			case STS_AUTORECOVERY_BLANKTRANS:
			case STS_AUTORECOVERY_DOUBLETRANS:
			case STS_AUTORECOVERY_OVERRUN:
				{
					if(m_pOHTMainStatus->DataUseFlag.fSwappingParam == true)
					{
						ADD_LOG_EXTRAJOB("[Update param] (Step %d) Update file check", UPDATEPARAM_STATE_CHECK_FILE);
						UpdateDataStepParam = UPDATEPARAM_STATE_FILE_COPY;
						m_pOHTMainStatus->DataUseFlag.fReadySwappingParam = false;
					}

				}
			}
		}

		 m_pOHTMainStatus->DataUseFlag.fReadySwappingParam = true;
	break;


	//Step 2. �������Ͽ� ����� : Update -> Datafiles ������ ����

	case UPDATEPARAM_STATE_FILE_COPY:
		ADD_LOG_EXTRAJOB("[Update param] (Step %d) Start File Copying", UPDATEPARAM_STATE_FILE_COPY);

		Error = CopyFile(m_strFileParam[m_nParamNum].w_str(), m_strFileParamDST[m_nParamNum].w_str(), false);

		ADD_LOG_EXTRAJOB("[Update param] (Step %d) Start File : 1", UPDATEPARAM_STATE_FILE_COPY);
		Sleep(0);

		for(int i = 0; i< 10; i++)
		{
			if(CheckFileCopyCompletedParamDST())
			{
				Error = NO_ERR;
                ADD_LOG_EXTRAJOB("[Update param] (Step %d) End File Copying Count:%d", UPDATEPARAM_STATE_FILE_COPY,i);
				break;
            }
			Sleep(10);
        }

		if(Error == NO_ERR)
        {
			UpdateDataStepParam = UPDATEPARAM_STATE_FILE_LOAD;
			Error = 0;
		}
		else//Fail
		{
			ADD_LOG_EXTRAJOB("[Update param] (Step %d) ERR_DATA_UPDATE_COPY_FAIL", UPDATEPARAM_STATE_FILE_COPY);
			Error = UPDATE_STATE_ERR;
			UpdateDataStepParam = UPDATE_STATE_UPDATE_FAIL;
		}

        break;

	//Step 2. Temp Buffer�� ������ �����͸� Loading
	case UPDATEPARAM_STATE_FILE_LOAD:
	{
		ADD_LOG_EXTRAJOB("[Update param] (Step %d) File Load : Start %d", UPDATEPARAM_STATE_FILE_LOAD,m_nParamNum);


		Error = m_pTaskControl->LoadParamIni(m_nParamNum);
		ADD_LOG_EXTRAJOB("[Update param] (Step %d) File Load : 1 E%d", UPDATEPARAM_STATE_FILE_LOAD, Error);
		Sleep(0);

		if(Error == NO_ERR)
        {
			ADD_LOG_EXTRAJOB("[Update param] (Step %d) End File Loading : Success", UPDATEPARAM_STATE_FILE_LOAD);
			UpdateDataStepParam = UPDATEPARAM_STATE_END_UPDATE;

		}
		else//Fail
		{
			ADD_LOG_EXTRAJOB("[Update param] (Step %d) ERR_UPDATE_DATA_UPDATE_INVALID_FILE", UPDATEPARAM_STATE_FILE_LOAD);
			Error = UPDATE_STATE_ERR;
			UpdateDataStepParam = UPDATEPARAM_STATE_UPDATE_FAIL;

		}

	}
	break;

     	//���� �۾������� ������ SwapUpdateData()�Լ� ������ ������...
	//Step 5. �۾�����

	case UPDATEPARAM_STATE_END_UPDATE:
	{
		ADD_LOG_EXTRAJOB("[Update param] (Step %d) Success to Update", UPDATEPARAM_STATE_END_UPDATE);
		//delete m_pFileInfo;


		UpdateDataStepParam = UPDATEPARAM_STATE_CHECK_FILE;
		DeleteParamsetFile(m_nParamNum);
		m_nParamNum = -1;
		OHTMainForm->m_ParamUpdateResult = NO_ERR;
		m_pOHTMainStatus->DataUseFlag.fSwappingParam = false;
		m_pOHTMainStatus->DataUseFlag.fReadySwappingParam = false;
		// MAPSET ���� ���� �� ������ ���
		// OCS�� ������ ���� ��ô ���� ���� (����)
//		OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_SUCCESS;

	}
        break;
	case UPDATEPARAM_STATE_UPDATE_FAIL:
		switch(m_pOHTMainStatus->StatusCommon.MSCmdID.Info.Status)
		{
				//������Ʈ ������ ���¿� ������ ���¿��� �� ���� �߻� ��Ŵ
			case STS_INIT:
			case STS_ARRIVED:
			case STS_UNLOADED:
			case STS_LOADED:
			case STS_COMPLETE:
			case STS_ERROR:
			case STS_TURN_OFF:
			{
				//�� �˻�� Error �߻��ϸ� ���� ��� ó�� ���� ������ ����
				//�̰����� Error �߻�
				OHTMainForm->m_ParamUpdateResult = ERR_PARADATA_LOAD_FAIL;
				UpdateDataStepParam = UPDATEPARAM_STATE_CHECK_FILE;
				ADD_LOG_EXTRAJOB("[Update] (Step %d) UPDATE_STATE_UPDATE_FAIL (Manual Mode)", UPDATEPARAM_STATE_UPDATE_FAIL);
				DeleteParamsetFile(m_nParamNum);
				m_nParamNum = -1;

				m_pOHTMainStatus->DataUseFlag.fSwappingParam = false;
				m_pOHTMainStatus->DataUseFlag.fReadySwappingParam = false;

			}

		}
		break;
	//Default
    default:
        break;
    }

    if(Error)
    {
		ADD_LOG_EXTRAJOB("[Update param] In Step %d, Error occured. [ErrCode : %d]", UpdateDataStepParam, Error);
        //ADD_LOG_EXTRAJOB("[Update] Be Deleted Update File and Started the First Step Again..");

        //delete m_pFileInfo;
		UpdateDataStepParam = UPDATEPARAM_STATE_UPDATE_FAIL;
        //DeleteFile(strFilePath);

		// MAPSET ���� ���� �� ������ ���
		// OCS�� ������ ���� ��ô ���� ���� (����)
//		OHTMainForm->g_MAPUpdateStatus = DATA_UPDATE_FAIL;

    }

	return Error;

}

/**
@brief   �������� ������ �ϷῩ�θ� �˾Ƴ�
@author  doori.shin
@date    2013.08.08
@return  true : �Ϸ�, false : ������
*/
bool ExtraJobThread::CheckFileCopyCompleted(String strFileName)
{
    String strFilePath = String(strOHTPath + STR_DATAFILE_UPDATE_PATH + strFileName);

    HANDLE hFile = CreateFile(strFilePath.w_str(),
                              GENERIC_READ,
                              0,    /// no share
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);


	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return true;

	}

	return false;
}

/**
@brief   �������� ������ �ϷῩ�θ� �˾Ƴ�
@author  sh17.jo
@date    2017.05.29
@return  true : �Ϸ�, false : ������
*/
bool ExtraJobThread::CheckFileCopyCompleted(void)
{
#if(OHT_NAME == OHT_NAME_P4_MASK)
	String strFile[6];
	UINT iResult=0;
	HANDLE hFile;

	strFile[0] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	strFile[1] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[2] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	strFile[3] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	strFile[4] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	strFile[5] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;

	for(int i = 0; i<6/*7*/; i++)
	{
		hFile = CreateFile(strFile[i].w_str(),
							 GENERIC_READ,
							 0,    /// no share
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL);

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		else
			iResult++;
	}
#else
	String strFile[7];
	UINT iResult=0;
	HANDLE hFile;

	strFile[0] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;
	strFile[1] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	strFile[2] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	strFile[3] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	strFile[4] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	strFile[5] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	strFile[6] = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;

	for(int i = 0; i<7; i++)
	{
        hFile = CreateFile(strFile[i].w_str(),
                             GENERIC_READ,
                             0,    /// no share
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
							 NULL);

        if (hFile != INVALID_HANDLE_VALUE)
		    CloseHandle(hFile);
		else
			iResult++;
	}
#endif
    if(iResult == 0)
    {
    	ADD_LOG_EXTRAJOB("file copy Completed");
		return true;
    }
	else
	{
		ADD_LOG_EXTRAJOB("file copying error");
		return false;
	}
}


/**
@brief   �������� ������ �ϷῩ�θ� �˾Ƴ�
@author  sh17.jo
@date    2017.05.29
@return  true : �Ϸ�, false : ������
*/
bool ExtraJobThread::CheckFileCopyCompletedParam(void)
{

	HANDLE hFile;

	for(int i = 0; i<6; i++)
	{
		hFile = CreateFile(m_strFileParam[i].w_str(),
                             GENERIC_READ,
                             0,    /// no share
                             NULL,
							 OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
							 NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			m_nParamNum = i;
			break;
		}
	}

	if(m_nParamNum != -1)
	{
		ADD_LOG_EXTRAJOB("file Param copy Completed");
		return true;
    }
	else
	{
		ADD_LOG_EXTRAJOB("file Param copying error");
		return false;
	}
}


bool ExtraJobThread::CheckFileCopyCompletedParamDST(void)
{

	HANDLE hFile;


		hFile = CreateFile(m_strFileParamDST[m_nParamNum].w_str(),
                             GENERIC_READ,
                             0,    /// no share
                             NULL,
							 OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
							 NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
            return true;

		}else
		{
			return false;
		}
}

//---------------------------------------------------------------------------
int ExtraJobThread::DeleteMapsetFile()
{
	String strOHTPath = "", strFileName = "";
	strOHTPath = ExtractFileDir(Application->ExeName);
	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;
	DeleteFile(strFileName);

	//MapMaker file ���� (Auto teaching)
	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MapMakerFile;
	DeleteFile(strFileName);

	strFileName = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_JobOrderFile;
	DeleteFile(strFileName);

}

void ExtraJobThread::DeleteParamsetFile(int nType)
{
	DeleteFile(m_strFileParam[nType]);
}

int ExtraJobThread::BackupMapsetFiles()
{
	TDateTime tdt;
	tdt = tdt.CurrentDateTime();

	// ��� ������ �ð� ������ ���ϸ� �����Ѵ�.
	String strFileName = "";
	UINT iResult=0;
	int Error = NO_ERR;
#if(OHT_NAME != OHT_NAME_P4_MASK)
	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "CID_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetCIDDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetCID_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}
#endif
	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "COLLISION_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetCollisionDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetCollision_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "FLOWCONTROL_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetFlowControlDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetFlowControl_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "MAPDATA_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetMapDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetMap_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "STATION_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetStationDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetStation_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "TEACHING_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetTeachingDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetTeaching_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_BACKUP_PATH) + "PATHDATA_" + tdt.FormatString("yyyymmddhhmmss") + ".dat";
	Error = OHTMainForm->GetPathDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetPathData_STD());
	Sleep(0);
	if( Error != NO_ERR)
	{	iResult+=1;	}

	if(iResult==0)
	{
		ADD_LOG_EXTRAJOB("[Update] BackupMapsetFiles Complete...");
	}
	else
	{
		ADD_LOG_EXTRAJOB("[Update] BackupMapsetFiles Fail... : %d",iResult);
	}

	return iResult;
}

/*
int ExtraJobThread::SaveMapsetFiles()
{
	// Map�� ����Ǿ����� MapMake�� ���� �� Map ������ �����Ѵ�.
	String strFileName = "";
	UINT iResult=0;
	int Error = NO_ERR;
	
	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_CID;
	Error = OHTMainForm->GetCIDDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetCID_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_COLLISION;
	Error = OHTMainForm->GetCollisionDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetCollision_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_FLOWCONTROL;
	Error = OHTMainForm->GetFlowControlDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetFlowControl_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}
	
	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_MAP;
	Error = OHTMainForm->GetMapDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetMap_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_STATION;
	Error = OHTMainForm->GetStationDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetStation_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_TEACHING;
	Error = OHTMainForm->GetTeachingDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetTeaching_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	strFileName = String(strOHTPath + STR_DATAFILE_PATH) + STR_FILENAME_PATHDATA;
	Error = OHTMainForm->GetPathDataManager_STD()->SaveMapFile(strFileName.w_str(), OHTMainForm->GetPathData_STD());
	if( Error != NO_ERR)
	{	iResult+=1;	}

	if(iResult==0)
	{
		ADD_LOG_EXTRAJOB("[Update] SaveMapsetFiles Complete...");
	}
	else
	{
		ADD_LOG_EXTRAJOB("[Update] SaveMapsetFiles Fail... : %d",iResult);
	}

	return iResult;
}
*/

//---------------------------------------------------------------------------
int ExtraJobThread::FindMapsetFiles()
{
	AnsiString strOHTPath = ExtractFileDir(Application->ExeName);

#if(OHT_NAME == OHT_NAME_P4_MASK)

	AnsiString strFile1 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	AnsiString strFile2 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	AnsiString strFile3 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	AnsiString strFile4 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	AnsiString strFile5 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	AnsiString strFile6 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;

	UINT iResult=0;

	if(PathFileExistsA(strFile1.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile2.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile3.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile4.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile5.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile6.c_str()))
	{	iResult+=1;	}


#else

	AnsiString strFile1 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_CID;
	AnsiString strFile2 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_COLLISION;
	AnsiString strFile3 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_FLOWCONTROL;
	AnsiString strFile4 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_MAP;
	AnsiString strFile5 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_STATION;
	AnsiString strFile6 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_TEACHING;
	AnsiString strFile7 = String(strOHTPath + STR_DATAFILE_UPDATE_PATH) + STR_FILENAME_PATHDATA;

	UINT iResult=0;

	if(PathFileExistsA(strFile1.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile2.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile3.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile4.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile5.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile6.c_str()))
	{	iResult+=1;	}

	if(PathFileExistsA(strFile7.c_str()))
	{	iResult+=1;	}
#endif

	return iResult;
}

int ExtraJobThread::FindParamsetFiles()
{

	UINT iResult = -1;


	for (int i = 0; i < 6; i++)
	{
      	if(PathFileExists(m_strFileParam[i].c_str()))
		{
			iResult=i;
			break;
		}

	}
	return iResult;
}

bool ExtraJobThread::DeletebackupFile()
{
   struct ffblk ffblk;
   int done;
//   char FileName[1024];
   AnsiString strFName;
   bool bR;

   AnsiString strLogPath = String(strOHTPath + STR_DATAFILE_BACKUP_PATH);


   strFName.sprintf("%s*.dat",strLogPath);

   done = findfirst(strFName.c_str(),&ffblk,0);
   while (!done)
   {


   	strFName.sprintf("%s%s", strLogPath, ffblk.ff_name);

	 try
	 {
		bR=DeleteFile(strFName);
		if(bR==false) //�б������� ���
		{
		   SetFileAttributesA(strFName.c_str(), FILE_ATTRIBUTE_NORMAL);
		   DeleteFile(strFName);
		}
	 }
	 catch (Exception &e)
	 {
	 }

	  done = findnext(&ffblk);
      Sleep(10);
   }
   return true;
}

SIZE_T ExtraJobThread::CheckMemoryInfo()
{
	DWORD processID = GetCurrentProcessId();
	SIZE_T dwWorkingSetSize = 0;

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
									PROCESS_VM_READ,
									FALSE, processID );
	if (hProcess == NULL)
		return dwWorkingSetSize;

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)))
	{
		dwWorkingSetSize = pmc.WorkingSetSize;
//		CPUNMEM_LOG("%08X", pmc.WorkingSetSize);
	}
	CloseHandle( hProcess );
	return dwWorkingSetSize;
}


#pragma package(smart_init)
