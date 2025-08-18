//---------------------------------------------------------------------------
#include <map>
#include <iostream>
#include "PathSearch.h"
#include "ErrorCode.h"
#include "MAP.h"
#include "OHTMain.h"
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#define ADD_LOG(...)    WRITE_LOG_UNIT(pLogPathSearch, __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)
#define ADD_MD_LOG(...)    WRITE_LOG_UNIT(pLogMDetail, __VA_ARGS__)  //AOHC-253 Monitoring �׸� �߰� 2

using namespace std;
//---------------------------------------------------------------------------
/**
@brief   Main Thread�� Execute Thread ���̿� ���޵� Driving�� Define
@author  ehoto
@date    2012.11.12
*/
int VisitLinkCheckArray[1000000][2];
int VisitNodeCheckArray[1000000];

typedef struct PATH_SEARCH_NODE_INFO_
{
	UINT IndexNum;				 ///< Path Index
	UINT SQID;					///< ������ ID : Passpermit �뵵.
	UINT NID;              	 ///< Node ID
	UINT NextNID;           ///< Next Node ID		: ���� ��� ���� ã�� �� ���
	UINT SubNID;			///������ ������ Node ID : ����㰡�� ���� �κ�
	UINT PathIndex;		///< Next Path ID Index	: ���� Path ���� ã�� �� ���
	PATH_STEERING_INFO		PreSteering;         ///<������ ������ ���� �κ�.
	PATH_DIVERGENCE_INFO    DivInfo;			///< �б� ����

	double dTotalDistance;      //ó�� ���ۺ��� ���� Path���� ���� �Ÿ�
	double dPathDistance;       //���� �н��� �Ÿ�

} PATH_SEARCH_NODE_INFO;


PathSearch::PathSearch()
{
	pLogPathSearch = getLogUnit("D:\\log\\PathSearch.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
	pLogMDetail = getLogUnit("D:\\log\\MonitoringDetail.log", MAX_BYTE_SIZE_OF_MAIN_LOG);

	InitializeCriticalSection(&m_CS);
	m_PathList = OHTMainForm->GetPathList();

	m_pParamSet = new PATHSEARCH_PARAM_SET();
	m_pUBGParamSet = new PATHSEARCH_UBG_CHECK_PARAM_SET();
	m_pNodeLanCutSet = new PATHSEARCH_NODE_LANCUT_PARAM_SET();

	loadParameterRecovery("DataFiles\\PathSearch.ini", m_pParamSet);
	loadUBGCheckParam("DataFiles\\UBGCheckNodes.dat", m_pUBGParamSet);

	m_SquenceIndex = 1;
	m_pDrivingControl = NULL;
	m_pTmpParamSet = NULL;

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	m_defualtparam = OHTMainForm->GetDefaultParam();
	m_EndNodeCnt = 0;

	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	m_bFindTheWay = false;
	m_ParamChehckError = NO_ERR;
	bCheckPathParam=false;
	bUseAVS = m_defualtparam->bUseAVS;
	dwPathSearchTime = timeGetTime();

}
PathSearch::~PathSearch()
{

	InitPathList();
//	delete m_PathList;
	DeleteCriticalSection(&m_CS);

//	saveParameter("DataFiles\\PathSearch.ini", m_pParamSet);


	delete m_pParamSet;
	delete m_pUBGParamSet;
	delete m_pNodeLanCutSet;
}
/**
@brief   List�� �ʱ�ȭ�ϴ� �Լ�
@author  puting
@date    2015.11.16
@note    List�� CMD_DATA ����ü�� �����ͷ� �����Ǿ� ����.
*/
void PathSearch::InitPathList()
{
	// List�� ���� �ش� ������ delete ��
	int nCount = m_PathList->getCount();
	for(int i = 0; i < nCount; i++)
	{
		EnterCriticalSection(&m_CS);

        try
        {
			delete (EXECUTE_DRIVING_INFO *)m_PathList->popFront();
        }
        __finally
		{
			LeaveCriticalSection(&m_CS);
        }
    }

    // ����Ʈ�� Clear�Ѵ�.
	EnterCriticalSection(&m_CS);
    try
    {
		m_PathList->clear();
    }
    __finally
    {
		LeaveCriticalSection(&m_CS);
    }

}
//---------------------------------------------------------------------------

/**
@brief   List�� Command�� �߰��ϴ� �Լ�
@author  puting
@date    2015.11.16
@param   pData : Add�Ϸ��� void ������
*/
void PathSearch::AddPath(void *pData)
{
	EnterCriticalSection(&m_CS);
	try
	{
		m_PathList->insertBack(pData);
	}
	__finally
	{
		LeaveCriticalSection(&m_CS);
	}
}

/**
@brief   List�� ù��° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2015.11.16
@return  List�� ù��° Item�� �����͸� �����Ѵ�.

void* PathSearch::ReferFirstPath()
{
	void *pListItem = NULL;

	pListItem = m_PathList->referFront();

	return pListItem;
}
*/


/**
@brief   List�� Index Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2013.10.28
@return  List�� index Item�� �����͸� �����Ѵ�.
*/
void* PathSearch::ReferPath(UINT index)
{
    void *pListItem = NULL;

	pListItem = m_PathList->refer(index);

    return pListItem;
}


/**
@brief   List�� ��������° Item�� �����ϴ� ����Ʈ�� �����ϴ� �Լ�
@author  puting
@date    2016.01.19
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
*/
void* PathSearch::ReferBackPath()
{
	void *pListItem = NULL;

	pListItem = m_PathList->referBack();

	return pListItem;
}

//---------------------------------------------------------------------------

/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  puting
@date    2015.11.16
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* PathSearch::PopBackPath()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_PathList->popBack();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
/**
@brief   List�� ù��° Item�� Pop�ϴ� �Լ�
@author  puting
@date    2015.11.16
@return  List�� ù��° Item�� �����͸� �����Ѵ�.
@note    Pop�� Item�� ����Ʈ���� �����ȴ�.
*/
void* PathSearch::PopFirstPath()
{
    void *pListItem = NULL;

	CS_TaskList->Acquire();
    try
    {
		pListItem = m_PathList->popFront();
    }
    __finally
    {
		CS_TaskList->Release();
    }

    return pListItem;
}
//---------------------------------------------------------------------------

/**
@brief   List�� count�� �����ϴ� �Լ�
@author  puting
@date    2015.11.16
@return  List�� count�� �����Ѵ�.
*/
int PathSearch::GetPathCount()
{
	return m_PathList->getCount();
}

/**
@brief   ����㰡 �ֱ� ����
@author  puting
@date    2016.11.26
@return  List�� count�� �����Ѵ�.
*/
int PathSearch::GetPassPermitPriod()
{
	return m_pParamSet->nPassPermitPeriod;
}

/**
@brief   ����㰡 �ֱ� ����
@author  puting
@date    2016.11.26
@return  List�� count�� �����Ѵ�.
*/
int PathSearch::GetPassPermitRollbackTime()
{
	return m_pParamSet->nPassPermitRollbackTime;
}

/**
@brief   ����㰡 �ֱ� ����
@author  puting
@date    2016.11.26
@return  List�� count�� �����Ѵ�.
*/
int PathSearch::GetPassPermitRollbackFinshTime()
{
	return m_pParamSet->nPassPermitRollbackFinshTime;
}

/**
@brief   ����㰡 �ݳ� ���� ����
@author  puting
@date    2018.4.9
@return  List�� count�� �����Ѵ�.
*/
double PathSearch::GetPassPermitRollbackMargin()
{
	return m_pParamSet->dPassPermitRollbackMargin;
}

//---------------------------------------------------------------------------
/**
@brief   Map �� Node ID�� �Է��Ͽ� NODE TYPE�� �޴� �Լ�
@author  ���¿�
@date    2013-04-02
@param   pMap : ����ϴ� Map
@param   uNID :  Node ID
@return  NODE TYPE
*/
bool PathSearch::getNodeType(MAP *pMap, UINT uNID, NODE_TYPE* pNodeType)
{
	bool bSuccess = true;
	
	NODE_INFO	*pNODE_INFO = NULL; // Node ����
	NODE_TYPE NodeType;
	int		nNodeIndex			= 0;		// Node�� Index

	nNodeIndex = pMap->FindNodeIndex(uNID);

	if(nNodeIndex==INDEX_NONE)
	{
		bSuccess=false;
		return bSuccess;
	}

	pNODE_INFO = &(pMap->m_NodeArray[nNodeIndex]);

	if(pNODE_INFO->PrePathIndex[0]!=INDEX_NONE && pNODE_INFO->PrePathIndex[1]!=INDEX_NONE &&
	   pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE )
	{
		NodeType = MERGE_BRANCH_POINT;
	}
	else if(pNODE_INFO->PrePathIndex[0]!=INDEX_NONE && pNODE_INFO->PrePathIndex[1]!=INDEX_NONE)
	{
		NodeType = MERGE_POINT;
	}
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)
	{
		NodeType = BRANCH_POINT;
	}
	else
	{
		NodeType = NORMAL_POINT;
	}

	*pNodeType = NodeType;
	
	return bSuccess;
	
}


/**
@brief   Station �ִ� Path�� ���ռ��� Ȯ���ϴ� �Լ�
@author  ���¿�
@date    2013-04-02
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@return  Error Code

int	PathSearch::GetPreSteering(MAP *pMap, UINT StopNode, UINT PreSteeringNode, PATH_STEERING_INFO *pPreSteeringInfo)
{
	MAP	*pTmpMap			= pMap;				// ����ϴ� Map
	int	nStopNode			= StopNode;			// ���� ���
	int	nPreSteeringNode	= PreSteeringNode;	// �� ���
	int	nTmpError			= 0;
	int	nCount				= 0;
	int	nCountOfList		= 0;

	LList 						DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO		*pEXECUTE_DRIVING_INFO	= NULL;	// List�� �����ϴ� ����ü
	EXECUTE_DRIVING_ADD_INFO	AdditionalInfo;
	PATH_STEERING_INFO			PathSteeringInfo = PATH_STEERING_NONE;

	memset(&AdditionalInfo, 0, sizeof(EXECUTE_DRIVING_ADD_INFO));

	//1 1. Path Search �ǽ�
	nTmpError		= SearchDrivingPath(pTmpMap, nStopNode, nPreSteeringNode, AdditionalInfo, &DRIVING_INFO_List);
	nCountOfList	= DRIVING_INFO_List.getCount();

	//1 2. ���� Ȯ��
	if(nTmpError == NO_ERR)
	{
		nCount = 0;

		for(int n=0; n<nCountOfList; n++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.Steering == PATH_STEERING_NONE)
			{
				nCount++;
				delete pEXECUTE_DRIVING_INFO;

				continue;
			}
			else
			{
				PathSteeringInfo = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
				delete pEXECUTE_DRIVING_INFO;

				break;
			}
		}

		//2	��� �Է�
//		pPreSteeringInfo->NodeCountFromStopNode	= nCount;
//		pPreSteeringInfo->PathSteeringInfo		= PathSteeringInfo;
		*pPreSteeringInfo = PathSteeringInfo;
	}

	nCountOfList = DRIVING_INFO_List.getCount();

	//1 3. ���� �Ҹ�
	for(int n=0; n<nCountOfList; n++)
	{
		pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
		delete pEXECUTE_DRIVING_INFO;
	}

	return nTmpError;
}
*/
bool PathSearch::CheckTurnOffOfPathInfo(MAP *pMap,UINT nCurNode)
{
	bool bReturn = false;
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< ��� ��� Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

	switch(pNODE_INFO->InOutInfo)
	{
		case SLOPE_NTYPE:
		case LIFTPRE_NTYPE_LEFT:
		case LIFTPRE_NTYPE_RIGHT:
		case LIFTIN_NTYPE_LEFT:
		case LIFTIN_NTYPE_RIGHT:
		case LIFTOUT_NTYPE:
		case MTL_SEL_ON_LEFT:
		case MTL_SEL_ON_RIGHT:
		case MTL_STOP_LEFT:
		case MTL_STOP_RIGTH:
		case AUTODOOR_LEFT:
		case AUTODOOR_RIGHT:
         	bReturn = true;
		break;

	}

	return  bReturn;
}
bool PathSearch::CheckEmergencyPathInfo(MAP *pMap,UINT nCurNode)
{
	bool nReturn = false;
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< ��� ��� Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

	switch(pNODE_INFO->InOutInfo)
	{
		case EmergencyNodeMTL_NTYPE:
			nReturn = true;
		break;

	}

	return  nReturn;
}

int PathSearch::CheckDeviceOfPathInfo(MAP *pMap,UINT nCurNode)
{
	int nReturn = 0;
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	nNodeIndex = pTmpMap->FindNodeIndex(nCurNode);   ///< ��� ��� Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

	switch(pNODE_INFO->InOutInfo)
	{
		case SLOPE_NTYPE:
			nReturn = 1;
		break;
		case LIFTPRE_NTYPE_LEFT:
		case LIFTPRE_NTYPE_RIGHT:
		case LIFTIN_NTYPE_LEFT:
		case LIFTIN_NTYPE_RIGHT:
		case LIFTOUT_NTYPE:
        	nReturn = 2;
		break;
		case MTL_SEL_ON_LEFT:
		case MTL_SEL_ON_RIGHT:
		case MTL_STOP_LEFT:
		case MTL_STOP_RIGTH:
		case AUTODOOR_LEFT:
		case AUTODOOR_RIGHT:
			nReturn = 3;
		break;

	}

	return  nReturn;
}
PATH_DIVERGENCE_INFO PathSearch::GetStationDivInfo(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

  	PATH_DIVERGENCE_INFO nTmpreturn = PATH_DIVERGENCE_NONE;
//	UINT	CmdID				= CmdGoData->CmdID;  //���� ID
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

//
	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	///������ Station�ΰ�쿡 ���� ��ó�� �κ�

		//�������� ��� �θ��尡 ���´�.

		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����

					if(pPATH_INFO->EndNID ==nNextNode)
					{
                     	if(n == 0)
							nTmpreturn = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTmpreturn = PATH_DIVERGENCE_RIGHT;    //������ �б�

						break;
                    }
				}
			}
		}
		else
		{
			ADD_LOG("[GetStationDivInfo] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
		}

        ADD_LOG("[GetStationDivInfo:FromNode %d, EndNode %d  %d",nFromNode, nNextNode, nTmpreturn);

		return nTmpreturn;
}

bool PathSearch::GetIsNBrench(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

	bool nTmpreturn = false;
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map
	//
	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����


	///������ Station�ΰ�쿡 ���� ��ó�� �κ�

		//�������� ��� �θ��尡 ���´�.
		ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		if(nNodeIndex != INDEX_NONE)
		{
			if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) && (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// ���� ��忡 Path ������� ���� ���
			{
				for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����

					if(pPATH_INFO->EndNID ==nNextNode)
					{
						if(pPATH_INFO->PathBranch ==  0x02)
						{
							nTmpreturn =true;
							break;
						}
					}
				}
			}
			else
			{
				ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
				, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			}
		}

		ADD_LOG("[CheckNextNode:FromNode %d, Return %d",nFromNode,nTmpreturn);

		return nTmpreturn;

}


bool PathSearch::GetIsNBrenchMapMake(MAP *pTmpMap, UINT nPreNode, UINT nNextNode)
{
	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO		= NULL;	// Path ����
	int	 nPathIndex;
	bool nTmpreturn = false;
	int  nPreNodeIndex	= pTmpMap->FindNodeIndex(nPreNode);
	int  nNodeIndex = pTmpMap->FindNodeIndex(nNextNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

	//�շ��� ��, �б� ��, �Ϲݰ
	 ADD_LOG("[PATHSEARCH_Start_NextNode,nNodeIndex] %d %d %d %d",nNextNode,nNodeIndex,pNODE_INFO->NextPathIndex[0],pNODE_INFO->NextPathIndex[1]);
	if(nNodeIndex != INDEX_NONE)
	{
		//�б�
		if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) && (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// ���� ��忡 Path ������� ���� ���
		{

			return true;
		}
		//�շ�, �Ϲ�
		else if(pNODE_INFO->NextPathIndex[0] != INDEX_NONE)
		{

			int NewNextNodeIndex = pNODE_INFO->NextPathIndex[0];
#if(USE_CHANGENODE == 1)
			BYTE UBGRegion = pTmpMap->m_PathArray[NewNextNodeIndex].UBGRegion;
#else
			BYTE UBGRegion = (pTmpMap->m_PathArray[NewNextNodeIndex].PathDirection & 0x04);
#endif
			BYTE Steering = pTmpMap->m_PathArray[NewNextNodeIndex].Steering;

			//�б�����
			if(Steering !=0x00 && UBGRegion == 0x01)
				return true;

			//�
			if(UBGRegion !=0x01)
				return true;


        }
	}
	//�շ��� ��, �б� ��, � ��
	if(nPreNodeIndex!= INDEX_NONE)
	{

		nPathIndex  = pTmpMap->GetPathIndex(nPreNode, nNextNode);
#if(USE_CHANGENODE == 1)
		BYTE UBGRegion = pTmpMap->m_PathArray[nPathIndex].UBGRegion;
#else
        BYTE UBGRegion = (pTmpMap->m_PathArray[nPathIndex].PathDirection & 0x04);
#endif
		BYTE Steering = pTmpMap->m_PathArray[nPathIndex].Steering;

		//�б�����
		if(Steering != 0x00 && UBGRegion == 0x01)
			return true;

		//�
		if(UBGRegion != 0x01)
			return true;


	}



	ADD_LOG("[CheckNextNode:nNextNode %d, Return %d", nNextNode, nTmpreturn);

	return nTmpreturn;

}

PATH_STEERING_INFO PathSearch::GetSteeringInfo(MAP *pMap, UINT nFromNode, UINT nNextNode)
{

	PATH_STEERING_INFO nTmpreturn = PATH_STEERING_NONE;
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map
	//
	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����


	///������ Station�ΰ�쿡 ���� ��ó�� �κ�

		//�������� ��� �θ��尡 ���´�.
		ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
		nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		if((pNODE_INFO->NextPathIndex[0]!=INDEX_NONE) || (pNODE_INFO->NextPathIndex[1]!=INDEX_NONE))// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����

					if(pPATH_INFO->EndNID ==nNextNode)
					{
						nTmpreturn = pPATH_INFO->Steering;
						break;
					}
				}
			}
		}

		ADD_LOG("[CheckNextNode] FromNode %d, Return %d",nFromNode,nTmpreturn);

		return nTmpreturn;

}

int	PathSearch::GetNextNode(MAP *pMap, UINT nFromNode, UINT nPathDiv)
{

   	int nTmpreturn = -1;
//	UINT	CmdID				= CmdGoData->CmdID;  //���� ID
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

//
	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index

//
	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����

	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	///������ Station�ΰ�쿡 ���� ��ó�� �κ�

	//�������� ��� �θ��尡 ���´�.
	ADD_LOG("[PATHSEARCH_Start_Station_FromNode] %d",nFromNode);
	nNodeIndex = pTmpMap->FindNodeIndex(nFromNode);   ///< ��� ��� Index
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

	if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����

				if(nPathDiv !=0x00)
				{
					if(((nPathDiv ==0x01) && (n==0)) ||
					   ((nPathDiv ==0x02) && (n==1)))
					   {
							nTmpreturn  =  pPATH_INFO->EndNID;

					   }
					else if(nPathDiv==0x03)
					{

							nTmpreturn  =  pPATH_INFO->EndNID;
					}

				}
			}
		}
	}
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
			{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
					nTmpreturn  =  pPATH_INFO->EndNID;

			}
		}
	}
	else
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
		, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
	}

	ADD_LOG("[CheckNextNode:FromNode %d, EndNode %d",nFromNode,nTmpreturn);

	return nTmpreturn;

}

/**
@brief   Station �ִ� Path�� ���ռ��� Ȯ���ϴ� �Լ�
@author  ���¿�
@date    2013-03-20
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@return  Error Code
*/
int PathSearch::CheckPathOfStation(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo)
{
	int	nTmpError	= NO_ERR;
	
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map
	UINT	uFromNode			= FromNode;			// ���� ���
	UINT	uToNode				= ToNode;			// �� ���

	int		nFromNodeIndex		= pTmpMap->FindNodeIndex(uFromNode);	// FromNode�� Index
	int		nToNodeIndex		= pTmpMap->FindNodeIndex(uToNode);		// ToNode�� Index
	UINT	nPathIndex			= 0;		// Path�� Index

//	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)
	
	NODE_INFO	*pNODE_INFO	= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	//1 0. Station ������ ���� ��츸 ���� ó��
	if(AdditionalInfo.StationType==MARK_TYPE_NODE)
	{	
		nTmpError	= NO_ERR;
		return nTmpError;
	}

	//1 1. ������� ������� ������ ���� ��� / �� ��尡 ���� ���
	if( nFromNodeIndex	==	-1	|| 
		nToNodeIndex	==	-1	||
		uFromNode		== uToNode 	)
	{
		nTmpError	= ERR_PATHSEARCH_FAIL;     //���� �߻� �ʿ�
		ADD_LOG("[ERR_PATHSEARCH_FAIL_1] %d:%d:%d:%d", nFromNodeIndex, nToNodeIndex, uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	//1 2. Link�� 1���� �ƴ� ���(FromNode ���� ��忡 ToNode ���� ���� Ȯ��)
	pNODE_INFO	= &(pMap->m_NodeArray[nFromNodeIndex]);
	nTmpError	= ERR_PATHSEARCH_FAIL;	// ���� ���� ��� NO_ERR�� ����
	for(int i=0; i<2;i++)
	{
		nPathIndex	= pNODE_INFO->NextPathIndex[i];
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO	= &(pMap->m_PathArray[nPathIndex]);
			if(pPATH_INFO->EndNID==uToNode)
			{
				nTmpError	= NO_ERR;
				break;
			}
		}
	}
	if(nTmpError!=NO_ERR)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_2] %d", pNODE_INFO->NID);
		ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
	}
	
	if(nTmpError	!= NO_ERR)
	{
		return nTmpError;
	}

	//1 3. Station������ �Ÿ��� Link ������ �Ÿ����� Ŭ���
	if(	AdditionalInfo.DistToStation > (pPATH_INFO->Distance+MARGIN_OF_DIST_TO_STATION) ||
		AdditionalInfo.DistToStation < 0)
	{
		nTmpError	= ERR_PATHSEARCH_FAIL;   //���� �߻� �ʿ�
		ADD_LOG("[ERR_PATHSEARCH_FAIL_3] %3.1lf:%3.1lf"
			, AdditionalInfo.DistToStation
			, pPATH_INFO->Distance);
		ADD_FDC_LOG("ERR/%d/5/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
	}

    return nTmpError;

}

/**
@brief   Driving Tag PathSearch Map Search �Լ�
@author  ehoto
@date    2012-11-01
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code

int PathSearch::SearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	MAP		*pTmpMap			= pMap;				// ����ϴ� Map
	UINT	uFromNode			= FromNode;			// ���� ���
	UINT	uToNode				= ToNode;			// �� ���
	LList	*pResultAddList	= DRIVING_INFO_List;	// ������ ���� ����� �߰��ϱ� ���� List

	bool	bArrived			= false;	// Map Seach�� ������ ���
	UINT	uPathSearchIndex	= 0;		// PathSearch List�� Index
	UINT	uListInsertIndex	= 0;		// PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		// PathSearch List ���� ���� ��

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index
	UINT	uTmpToNode			= 0;		// Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		// Path ������ ���� Ȯ�� ��
	double	dTmpTotalDistance	= 0.0;	// ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;
	
	NODE_INFO	*pNODE_INFO	= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����
	
	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü

	LList PathSearchList;									// PathSearch�� ���� List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node ������ ã�� ���� Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. �ʱ� ���� : ----------------------------------------------------------
	//   0) ���� Node�� Path Ȯ��
	//   1)�� Node List�� �߰�
	//   �� �������� ã���� ������ ó���� ���������� ����� ����
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
//	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;     //�����߻��ʿ�
		
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}
	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	uTmpToNode = uToNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node ���� ���� : ���� ���
	pPATH_SEARCH_NODE_INFO->NID = uTmpToNode;
	pPATH_SEARCH_NODE_INFO->NextNID = 0;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List �� ����
	mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;		// map �� ����
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ ���۳�尡 �߰ߵǸ� ����
	ADD_LOG("[PATHSEARCH_Step 1-2]");

	while(bArrived==false)	// ���� Node ���� ��� bArrived �� true�� �ٲ���
	{
		pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
		uPathSearchIndex++;

		dtmpPathSearchListCnt = PathSearchList.getCount();
		if(dtmpPathSearchListCnt > dMaxPathSearchListCnt) 
		{
			dMaxPathSearchListCnt = dtmpPathSearchListCnt;
			
			if((dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
			{
				nTmpError = ERR_PATHSEARCH_FAIL;
				ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				break;
			}
		}
		
		// ���۳�� ���� Ȯ��
		if(pPATH_SEARCH_NODE_INFO != NULL)
		{
			if(pPATH_SEARCH_NODE_INFO->NID == uFromNode)
			{
				bArrived = true;
				break;
			}
		}

		if(pPATH_SEARCH_NODE_INFO != NULL)	// mapPathSearchNode ��  uTmpToNode �� PathSearch Node������ ������
		{
			dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
		}
		else
		{
			nTmpError = -1; // PathSearch ����
			break;
		}

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;
		nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

		for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
		{
			nPathIndex = pNODE_INFO->PrePathIndex[n];		// Path Index ȹ��
			if(nPathIndex!=-1)
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path ����
				dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link ���� ȹ�� : �̵� �ð� (�Ÿ�/�ӵ�)
				uTmpFromNode = pPATH_INFO->StartNID;            // ���� Node ID ȹ��

				IterPos = mapPathSearchNode.find(uTmpFromNode);
				if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
				{
					// PathSearch Node ���� ���� : ���� ���
					pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
					pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
					pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
					pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
					pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

					//�߰��ϴ� ��ƾ
					pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
					if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// �Ÿ��� ���� �� ���
					{
						PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
						mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;
					}
					else	// �߰��� ���� �Ǵ� ���
					{
						uListInsertIndex = 0;
						uTmpListCount = PathSearchList.getCount();
						while(uListInsertIndex<uTmpListCount)
						{
							pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
							if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
							{
								PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
								mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;
								uListInsertIndex = uTmpListCount;	// while �� ����
							}
							else
							{
								uListInsertIndex++;
							}
						}	// while
					}
				}
				else	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
				{
					pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
					if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // ���� ���ų� Ŭ ��� �ձ�-->skip
					{
						// PathSearch Node ���� ���� : �߰��� ����
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						// ����Ʈ���� �߰� �� ���� �� ���� ��ƾ
						uListInsertIndex = 0;
						uTmpListCount = PathSearchList.getCount();
						while(uListInsertIndex<uTmpListCount)
						{
							pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
							if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
							{
								// ���ο� �� �߰�
								PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
								// ���� �Ÿ� �� ���� �� ����
								uTmpListCount = PathSearchList.getCount();
								uListInsertIndex++;
								while(uListInsertIndex<uTmpListCount)
								{
									pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
									if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
									{
										PathSearchList.pop(uListInsertIndex); 	// List ���� ����
										mapPathSearchNode.erase(IterPos);
										mapPathSearchNode[uTmpFromNode]=pPATH_SEARCH_NODE_INFO;
										delete pTempPATH_SEARCH_NODE_INFO;		// ���� �Ҹ�
									}
									uListInsertIndex = uTmpListCount;	// while �� ����
								}
							}
							else
							{
								uListInsertIndex++;
							}
						}	// while
					}
				}
			}	// if(nPathIndex!=-1)
		}	// for(int n=0; n<2; n++)
	}

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Start Node ���� ���� Node ã�ư��鼭 Driving List ���� ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpFromNode);
		
        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ� 
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) || 
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ� 
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpFromNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NextNID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		if((pTmpMap->tPassBoxNodeList != NULL) && (pOHTMainStatusTmp->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL))
		{
			for(int j=0 ; j < pTmpMap->tPassBoxNodeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tPassBoxNodeList->Strings[j];

				if((uTmpFromNode == StrToIntDef(StrTmp, 0)) && ((uTmpFromNode != FromNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxOut)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#1] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}

				if((pPATH_SEARCH_NODE_INFO->NextNID == StrToIntDef(StrTmp, 0)) && ((StrToIntDef(StrTmp, 0) != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxIn)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#2] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/2/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
	   //	pEXECUTE_DRIVING_INFO->AddInfo					= AdditionalInfo;

		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake �� ��� Map Make �ӵ� �ݿ� : ��ġ �� ó�� ����� ���� �߰� ��� �ʿ�
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

//		Route_ADD_LOG("[PathSearch] Node:%X~%X",pEXECUTE_DRIVING_INFO->StartNID, pEXECUTE_DRIVING_INFO->EndNID);
		pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);

		uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
		if(uTmpFromNode == uToNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}	
	}
	
	// 4. Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}
*/
/**
@brief   ChangPoint ������ Search �Լ�
@author  puting
@date    2016-07-02
@param   StartNode : ���� Node
@param   DefaultDiv : �б� ���� ����
@return  0 ����, �������� ����.
*/
bool PathSearch::CheckSearchChangePoint(int ChangeNodeID, UINT &ChangeSqNum, double &ChangeNodeDistance)
{
	bool bArrived = false;
	int Index = 0;
	bool bCheckFinish = false;
	EXECUTE_DRIVING_INFO 	*pPathListInfo = NULL;

	// 1) Path List ���� Ȯ��
	for(Index =0; Index< GetPathCount(); Index++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(Index);

		if(pPathListInfo !=NULL)
		{
			if(pPathListInfo->BasicInfo.StartNID ==  ChangeNodeID)
			{
				bArrived = true;
				break;
			}
		}
	}

	if(Index ==(GetPathCount() -1))  bCheckFinish = true;

	if(bArrived == true)
	{

		ChangeSqNum = pPathListInfo->SQNum;
		ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->BasicInfo.dLength;
		ADD_LOG("[CheckSearchChangePoint] %d %f:%f--------",ChangeNodeID, pPathListInfo->ChangeEndNodeDistance,pPathListInfo->BasicInfo.dLength);

		if(bCheckFinish ==true)
		{
			if(pPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
			{
				ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
				ADD_LOG("[CheckSearchChangePoint %d:%d]",Index,ChangeNodeID);
			}
		}
	}
	else
	{
		//������ ��ġ���� Ȯ���ϴ� �κ�
		if(pPathListInfo !=NULL)
		{
			if(pPathListInfo->BasicInfo.EndNID ==  ChangeNodeID)
			{
				bArrived = true;
				ChangeSqNum = pPathListInfo->SQNum;
				ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance;
				ADD_LOG("[CheckSearchChangePoint22] %d:%f--------",ChangeNodeID,pPathListInfo->ChangeEndNodeDistance);
			}
		}
    }


	return bArrived;
}
/**
@brief   ���� �б� Search �Լ�
@author  puting
@date    2016-06-19  , 2018.03.10 �켱 �ʺ��� ������.
@param   pMap : ����ϴ� Map
@param   StartNode : ���� Node
@param   DefaultDiv : �б� ���� ����
@return  0 ����, �������� ����.
*/
int PathSearch::CheckSearchDifferntPoint(MAP *pMap, PARSING_GO_INFO* CmdGoData, UINT &ChangeSqNum, int &ChangeNodeID, double &ChangeNodeDistance, int &nExtraPath, BYTE &chNackToAckType, UINT nPreTargetStation)
{
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	UINT	CmdID				= CmdGoData->CmdID;  //���� ID
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	//���Ž���� �ؾߵ� ���۰� �� ��ġ
	UINT	uFromNode			= CmdGoData->PathData_StartNodeID;			// ���� ���
	UINT	uToNode				= CmdGoData->PathData_EndNodeID;		    // �� ���


	bool	bArrived			= false;	// Map Seach�� ������ ���

	int		nPathListIndex	   	= 0;		// ���ϰ����ϴ� ��� ��ġ.

	UINT	uPathSearchIndex	= 0;		// PathSearch List�� Index
	UINT	uTmpListCount		= 0;		// PathSearch List ���� ���� ��

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index


	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����

	UINT	uTmpToNode			= 0;		// Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		// Path ������ ���� Ȯ�� ��

	double	dTmpTotalDistance	= 0.0;	// ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)


	//�⺻ �б� ���� // �߿��� �κ�. 0 : ���� 1: ������
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit üũ��
	int     uCheckExtraNum =0;
	int     nLastExtraNID = 0;

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����


	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü

	EXECUTE_DRIVING_INFO 	*pPathListInfo              = NULL;
	EXECUTE_DRIVING_INFO 	*pTempPathListInfo          = NULL;
	LList PathSearchList;									// PathSearch�� ���� List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node ������ ã�� ���� Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;



	//------------- 1. ��ó�� �κ�------------//
	// 1. �ʱ� ���� : ----------------------------------------------------------
	//   0) ���� Node�� Path Ȯ��
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;

	}

	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d : Dir :%d]",uFromNode,uToNode,bDefaultPathDirection);



//	//------------- 2. �ֿ� ó���κ�------------//
//	// 1) ���� ��ġ���� Ȯ��
	for(int i=0; i< GetPathCount(); i++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(i);
		ADD_LOG("[PATHSEARCH Start ID]%d ExtraID:%d %d",pPathListInfo->BasicInfo.StartNID, CmdGoData->PathData_ExtraID[uCheckExtraNum],CmdGoData->PathData_ExtraNum);

		ChangeSqNum = pPathListInfo->SQNum;  //�ʱⰪ ����

		/*
		if((CmdGoData->PathData_ExtraID[uCheckExtraNum] == pPathListInfo->BasicInfo.StartNID) &&
			(pPathListInfo->BasicInfo.StartNID !=  uFromNode))
		{

			for(int j=0; j<CmdGoData->PathData_ExtraNum; j++)
			{
				ADD_LOG("[Error_ExtraID:%d %d",CmdGoData->PathData_ExtraID[j],j);
			}

			return -1;         //������ ���� ������ �ȵǿ�.

		}
         */

		if(pPathListInfo->BasicInfo.StartNID ==  uFromNode)
		{
			nPathListIndex = i;
			bArrived = true;
			break;
		}
	}

	//������ġ�� ��ã���� ���� ó��. (�۾����� Nack)

	if(bArrived == false)
	{
		return -1;
	}

//	if((uFromNode == uToNode) &&
//	  (bArrived == true))
//	{
//		ChangeSqNum = pPathListInfo->SQNum;
//		ChangeNodeID = pPathListInfo->BasicInfo.StartNID;
//		ChangeNodeDistance = pPathListInfo->ChangeEndNodeDistance - pPathListInfo->BasicInfo.dLength;
//		return nTmpError;
//	}

//	if(bArrived ==false)
//		return ERR_STD_INNER_PATH_FAIL; // PathSearch ����

	// 3) ���� Node��ġ List�� ����
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	bArrived = false;

	while(bArrived==false)	// ���� Node ���� ��� bArrived �� true�� �ٲ���
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 ���� Node�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 �Ÿ� ���� Ȯ�� �ʿ�
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch ����
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

		}

		////< (2) List�� Max Count Check!!     puting -----------------------------------------------------------���Ļ����� �������� �Ǵ�.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if(dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax)
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
            ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		////< (4) ���� ��尡 �ƹ��� PathIndex�� ���� ��� ���� ó�� �߰�
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;

		}

		///<(6) Next Node(Path) ������ � ���� Ȯ���ϴ� �κ�  puting 20151107
 		////< (6)-1 �б��� ���,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			  ////< (6-1) �⺻�� �������� Ȥ���ϴ� �κ�  puting 20151107
			if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
			{
				ADD_LOG("[PathData_ExtraID %d",CmdGoData->PathData_ExtraID[uCheckExtraNum]);
				bCheckDefaultPathDirection = true;  //�⺻�� �б� �� ���
				uCheckExtraNum++;
				nExtraPath++;

				nLastExtraNID = pNODE_INFO->NID;

				if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
				{
					nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
					ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			};

			/////< (6-3) �б⿡�� ���ߵ� Path ���� Ȯ��

			////Map �κ� ���� �ʿ�.     Ȯ�� �ʿ� ������ �ݴ��� ��� ����.
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// ���� Path ���� ��ŭ Ȯ��
			{

				if(bCheckDefaultPathDirection == true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //���� �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //���� �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}

				}
			}

                  //�������� 0, ������ 1

			bCheckDefaultPathDirection = false;
		}

		////< (6-3) �бⰡ �ƴѰ��,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				//nTempDivInfo =  PATH_DIVERGENCE_NONE;
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) ����� Next Path(Node) ���� Ȯ�� �� ����
		////<(7)-1 ����� Path�� ���� �ϴ� ���
		if(nPathIndex!=INDEX_NONE)
		{
					////<(7)-1 Next Path ����
			uPathSearchIndex++;

			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����


			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //����� ���� ���


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//����ġ�� �ʿ����.


			// PathSearch Node ���� ���� : ���� ���
			pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
			pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
			pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

		   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
//				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
			pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;

			pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

			pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

			pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //������ ���õ� ���� �߰�. puting 20160119

			///puting �߰��ؾߵǴ� ����/ �ʱ�Ÿ��� Target�� ���� �Ÿ��� ����.
			//  Step 1.Target�� station�� ��� �Ÿ��� �߰��Ұ�.
			pPATH_SEARCH_NODE_INFO->dTotalDistance = (double)dTmpPathDistance + (double)dTmpTotalDistance;
			pPATH_SEARCH_NODE_INFO->dPathDistance  = (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

			//�߰��ϴ� ��ƾ
			PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

			pTempPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(nPathListIndex);
			nPathListIndex++;



			///< (3)-1 ������ ��� ���� Ȯ��  //�����翡 ������ �ڵ� ���� �ʿ� puting  20180311
			//ª�� ���
		  if(nPathListIndex != GetPathCount())
		   {
				 //ª�� ���
				 if(pPATH_SEARCH_NODE_INFO->NID == uToNode)
				 {

					  ADD_LOG("[PATHSEARCH_Diff_Short %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
					  ChangeSqNum = pTempPathListInfo->SQNum;
					  ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
					  ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
					  chNackToAckType = JobChageNackType::CycleChage_Type1;

					  if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
					  {
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
					  }

					  if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
					  {
							nExtraPath--;
					  }

					  bArrived = true;
					  break;

				  }
				  //��ΰ� ����Ǵ� ���
				  else if(pPATH_SEARCH_NODE_INFO->NextNID != pTempPathListInfo->BasicInfo.EndNID)
				 {
						ADD_LOG("[PATHSEARCH_Diff_Mid %d:%d %06f %06f]"
						,pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID,pTempPathListInfo->ChangeEndNodeDistance,pTempPathListInfo->BasicInfo.dLength);
			//					ChangeSqNum = pTempPathListInfo->SQNum;
						ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
						ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
						chNackToAckType = JobChageNackType::CycleChage_Type1_1;

						if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
						{
							nExtraPath--;
						}

						bArrived = true;
						break;
				}
		   }
		   else
		   {

				//ª�� ���(������ Tag�� �����ϳ� �ٸ��б�� ���°��.)
				if((pPATH_SEARCH_NODE_INFO->NextNID != pTempPathListInfo->BasicInfo.EndNID)
				  || ((CmdGoData->StopStationID !=0)&&
					 (CmdGoData->StationData_ParentNodeID == pTempPathListInfo->BasicInfo.StartNID) &&
					 (CmdGoData->StationData_NextNodeID != pTempPathListInfo->BasicInfo.EndNID)))
				{
					ChangeSqNum = pTempPathListInfo->SQNum;
					ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
					ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;
					chNackToAckType = JobChageNackType::CycleChage_Type1;

					ADD_LOG("[PATHSEARCH_Diff_Short_2 Ne%d:Cha%d:End %d IsStat:%d %f %d]",
						pPATH_SEARCH_NODE_INFO->NextNID,
						ChangeNodeID,
						pTempPathListInfo->BasicInfo.EndNID,
						pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
						pTempPathListInfo->ChangeEndNodeDistance,
						pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);


					if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
					{
						ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
					}

					if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
					{
					   nExtraPath--;
					}

				}
				//�����ϰų� �� ���
				else
				{


					 ADD_LOG("[PATHSEARCH_Diff_End Ne%d:Cha%d:End %d IsStat:%d %f %d]",
					 pPATH_SEARCH_NODE_INFO->NextNID,
					 ChangeNodeID,
					 pTempPathListInfo->BasicInfo.EndNID,
					 pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
						pTempPathListInfo->ChangeEndNodeDistance,
					 pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);


					 ChangeSqNum = pTempPathListInfo->SQNum;
					 ChangeNodeID = pTempPathListInfo->BasicInfo.EndNID;
					 ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance;

					 //�����ϰų� ª�� ���
					 // (pPATH_SEARCH_NODE_INFO->NextNID == pTempPathListInfo->BasicInfo.EndNID)
					  if(pPATH_SEARCH_NODE_INFO->NID == uToNode)
					  {
					  		ChangeSqNum = pTempPathListInfo->SQNum;
							ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - pTempPathListInfo->BasicInfo.dLength;

							chNackToAckType = JobChageNackType::CycleChage_Type1;

							if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
							{
							//Start�� End�� �����Ҷ��� �ѹ��� ������.
								if(CmdGoData->PathData_StartNodeID == CmdGoData->PathData_EndNodeID)
								{
										chNackToAckType = JobChageNackType::CycleChage_Type2;//�����翡 ������ �ڵ� ���� �ʿ� puting
								}

								//���濡 �ٷ� �̵��� ��츸 nack���� ack�� ó��.
								if((CmdGoData->StopStationID !=0) &&
								   ((CmdGoData->StationData_RunDist -pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist) >200.0))
								  {
										chNackToAckType = JobChageNackType::CycleChage_Type2;//�����翡 ������ �ڵ� ���� �ʿ� puting
								  }

								 if(nPreTargetStation == CmdGoData->StopStationID)
								 {
										chNackToAckType = JobChageNackType::CycleChage_Type2;//�����翡 ������ �ڵ� ���� �ʿ� puting
								 }

								  //����������, ������ Tag Next Node��ġ�� ���� ������ Ȯ�� �Ұ�.(������ ������ Tag, �ٸ� ������ Tag�� ���� �з��Ұ�)
								  ADD_LOG("[PATHSEARCH_Diff_End_1 %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
								  ChangeSqNum = pTempPathListInfo->SQNum;
								  ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
								  ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;

							 }

							 if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
							  {
							   nExtraPath--;
							  }

						}

					//������ ���, ������ ���� �۾����� �켱 �۾����� Nack ó���Ұ�.
					// (pPATH_SEARCH_NODE_INFO->NextNID == pTempPathListInfo->BasicInfo.EndNID)
						 else if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
							 (pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == false))
					   {
							 //�ش����̽��� �Ϲ����ΰ��� ����.
							ADD_LOG("[PATHSEARCH_Diff_End_2 %d:%d]",pPATH_SEARCH_NODE_INFO->NextNID,pTempPathListInfo->BasicInfo.EndNID);
							ChangeSqNum = pTempPathListInfo->SQNum;
							ChangeNodeID = pTempPathListInfo->BasicInfo.EndNID;
							ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance;
							chNackToAckType = JobChageNackType::CycleChage_Type1_1;

					   }
					   else //�հ��
					   {
							 //�������ΰ�� ã�³�带 ������� �����Ұ� �θ���� �ű���� ¥���� �ٽ� �̾���ߵ�.
							chNackToAckType = JobChageNackType::CycleChage_Type2;//�����翡 ������ �ڵ� ���� �ʿ� puting
							if(pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd == true)
							{
								ChangeNodeID = pTempPathListInfo->BasicInfo.StartNID;
								ChangeNodeDistance = pTempPathListInfo->ChangeEndNodeDistance - (double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist;
								ADD_LOG("[PATHSEARCH_Diff_End_1_3 Ne%d:Cha%d:End %d IsStat:%d %f %d]",
										pPATH_SEARCH_NODE_INFO->NextNID,
										ChangeNodeID,
										pTempPathListInfo->BasicInfo.EndNID,
										pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
										pTempPathListInfo->ChangeEndNodeDistance,
										pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);

								if (nLastExtraNID == pPATH_SEARCH_NODE_INFO->NID)
								{
									nExtraPath--;
								}

							}

						   ADD_LOG("[PATHSEARCH_Diff_End_1-4 Ne%d:Cha%d:End %d IsStat:%d %f %f]",
							pPATH_SEARCH_NODE_INFO->NextNID,
							ChangeNodeID,
							pTempPathListInfo->BasicInfo.EndNID,
							pTempPathListInfo->AddInfo.ProcessingGoInfo.IsStationEnd,
							pTempPathListInfo->ChangeEndNodeDistance,
							(double)pTempPathListInfo->AddInfo.ProcessingGoInfo.StationData_EndRunDist);
						}
				}

				bArrived = true;
				break;
		   }

			ChangeSqNum = pTempPathListInfo->SQNum;  //������ �ѹ� ����
		}

		if(nTmpError!=NO_ERR) break;


		nPathIndex = INDEX_NONE;

	 }  //while()

	 //3. PathSerchlist ����
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{

		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();

		ADD_LOG("[PATHSEARCH_Diff_ %d:%d:%d:%d",uTmpListCount,uIndex, pTempPATH_SEARCH_NODE_INFO->SQID,pTempPATH_SEARCH_NODE_INFO->NID);
		delete pTempPATH_SEARCH_NODE_INFO;
	}

      return nTmpError;
}

/**
@brief   Driving Tag Map Search �Լ�
@author  puting
@date    2015-10-28
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/

int PathSearch::StandardSearchPassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, UINT &uCheckExtraNum,bool bIsFoup,double PathExtra,bool bIgnorOverrun,bool bNoonechek)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	UINT	CmdID				= CmdGoData->CmdID;  //���� ID
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	//���Ž���� �ؾߵ� ���۰� �� ��ġ
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// ���� ���
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;		    // �� ���

	LList	*pResultAddList		= DRIVING_INFO_List;	// ������ ���� ����� �߰��ϱ� ���� Cycle List

	bool	bArrived			= false;	// Map Seach�� ������ ���

	UINT	uPathSearchIndex	= 0;		// PathSearch List�� Index
//	UINT	uListInsertIndex	= 0;		// PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		// PathSearch List ���� ���� ��

	UINT 	uSequenceIndex      = 0;      // ������ Number
	UINT 	uCompSequenceIndex  = 0;      // �ӽ� ������ Number
	UINT 	uFirstSequenceIndex  = 0;      // �ӽ� ������ Number

  //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node�� Index
	int     nNextNodeIndex      = 0;        //NextNode�� Index
	int		nPathIndex			= 0;		// Path�� Index
	int		nSubPathIndex	 	= 0;		// Path�� Index

	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����


	UINT	uTmpToNode			= 0;		// Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		// Path ������ ���� Ȯ�� ��
//	UINT	uTmpNode			= 0;		// �ӽ� ���� Ȯ�� ��

	double	dTmpTotalDistance	= 0.0;	// ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)
	double  dStopNodeMarGin     = 0.0;
	double  dTempMaxSpeed       = 0.0;  //����㰡�� ���� Max �ӵ�

	//�⺻ �б� ���� // �߿��� �κ�. 0 : ���� 1: ������
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit üũ��

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path ������ �̿��� ����㰡 Sub������ ȹ��

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü
	EXECUTE_DRIVING_INFO	*pTempEXECUTE_DRIVING_INFO	= NULL;	// ������ ���õ� ����
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst�� �����ϴ� ����ü
	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü


	LList PathSearchList;									// PathSearch�� ���� List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node ������ ã�� ���� Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	EXECUTE_DRIVING_ADD_INFO TempAdditionalInfo;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};
  //	PATH_STEERING_INFO		TempPreSteering = STEERING_NONE;		///< ������ ���� Default puting 20160119

	String StrTmp = "";

//    UINT									DivCount = 0;
//	UINT									TempDivCount = 0; 			 							//�������Ŭ �������� �б� ����
//	CYCLEDRIVING_CHANGE_INFO	  			TempDivInfo[MAX_LIST_COUNT_FOR_CHANGE] ={0,};	 	   //�������Ŭ �������� �б� ���� (Max 10�� ����, ���� ��ȹ���� ���� �ʿ�.)
	bool bIsTargetStationoPassPermitOpen = false; //������ ��ġ�� ����㰡�� �ȹް� �̵��ϴ� ���


	int EndCount = 0;
	//------------- 1. ��ó�� �κ�------------//
	// 1. �ʱ� ���� : ----------------------------------------------------------
	//   0) ���� Node�� Path Ȯ��
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		m_bFindTheWay = false;
		return ERR_PATHSEARCH_FAIL;

	}

	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d]",uFromNode,uToNode);

	////<(7)-1 Next Path ����
	///Target�� Station�ΰ�쿡 ���� ��ó�� �κ�
	if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
	{
		//�������� ��� �θ��尡 ���´�.

		bIsTargetStationoPassPermitOpen =  CmdGoData->bIsTargetStationoPassPermitOpen;  //puting

		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_EndNodeID);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		ADD_LOG("[PATHSEARCH_End_Station_5] %d %d %d",CmdGoData->PathData_EndNodeID, CmdGoData->bIsTargetStationoPassPermitOpen,bIgnorOverrun);
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist;
                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[PATHSEARCH_End_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
            }

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_53] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			m_bFindTheWay = false;
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 2]");


	//------------- 2. �ֿ� ó���κ�------------//

	// 3) ���� Node��ġ List�� ����
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	uSequenceIndex = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ ���۳�尡 �߰ߵǸ� ����

	while(bArrived==false)	// ���� Node ���� ��� bArrived �� true�� �ٲ���
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 ���� Node�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 �Ÿ� ���� Ȯ�� �ʿ�
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch ����
				//break;
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			 dTmpTotalDistance = PathExtra;  //----------------------------�ʱ� �� ���� �Է����ٰ�.

		}

		////< (2) List�� Max Count Check!!     puting -----------------------------------------------------------���Ļ����� �������� �Ǵ�.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if((dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
		{
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		////< (4) ���� ��尡 �ƹ��� PathIndex�� ���� ��� ���� ó�� �߰�
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		   //	return nTmpError;
		}


		///<(6) Next Node(Path) ������ � ���� Ȯ���ϴ� �κ�  puting 20151107
 		////< (6)-1 �б��� ���,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			  ////< (6-1) �⺻�� �������� Ȥ���ϴ� �κ�  puting 20151107
			if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
			{
				bCheckDefaultPathDirection = true;  //�⺻�� �б� �� ���
				uCheckExtraNum++;

				if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
				{
					nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
					ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				}
			}

			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %d %X:%X  :%d %d  NodeIndex:%d"  ,pNODE_INFO->NID
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],bCheckDefaultPathDirection,bDefaultPathDirection , nNodeIndex);
			////< ( 6-2) �б⿡ ���� ������ ������. �۾�����

//			TempDivInfo[DivCount].ChangeNodePoint = pNODE_INFO->NID;
//            TempDivInfo[DivCount].ChangeNodeType = 0x01;
//			TempDivInfo[DivCount].ChangeNodeDistance = dTmpTotalDistance;
//			DivCount++;

//			if(DivCount >m_pParamSet->nMapSearchMax)
//			{
//				ADD_LOG("[TempDivInfo] Max Over!!");
//				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
//            }
			/////< (6-3) �б⿡�� ���ߵ� Path ���� Ȯ��

			////Map �κ� ���� �ʿ�.     Ȯ�� �ʿ� ������ �ݴ��� ��� ����.
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// ���� Path ���� ��ŭ Ȯ��
			{

				if(bCheckDefaultPathDirection ==true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //������ �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //����㰡 Sub�κ��� ���� Check

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //���� �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
//
//						pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
//
//						ADD_LOG("[bDefaultPathDirection %d][n : %d, Direction %d]", bDefaultPathDirection , nTempIndex, pPATH_INFO->PathDirection);
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //����㰡 Sub�κ��� ���� Check
				}


			}
             	 //������ �̵��� �б��� ���, �������� �б������� ����. ��Ŭ���� ó�� 20160422
			if((CmdGoData->PathData_EndNodeID == pNODE_INFO->NID) &&
				(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)&& ((uPathSearchIndex !=0) || (bNoonechek ==true)))     //AOHC-301
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.EndNodeID)
				{
					int Temp =0;
					Temp = nPathIndex;
					nPathIndex = nSubPathIndex;
					nSubPathIndex = Temp;

					if(nTempDivInfo == PATH_DIVERGENCE_LEFT)
						nTempDivInfo = PATH_DIVERGENCE_RIGHT;
					else
						nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�


					ADD_LOG("Station Target End %d %d %d",nPathIndex, nSubPathIndex,nTempDivInfo);
				}
			}
			else
			{
				ADD_LOG("StopNode Normal");
			}


			bCheckDefaultPathDirection = false;
		}

		////< (6-3) �бⰡ �ƴѰ��,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
//			nTempDivInfo =  PATH_DIVERGENCE_NONE;
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) ����� Next Path(Node) ���� Ȯ�� �� ����
		////<(7)-1 ����� Path�� ���� �ϴ� ���
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
			if(nSubPathIndex !=0)
				pSubPATH_INFO = &(pTmpMap->m_PathArray[nSubPathIndex]);		// ����� Path ����

			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //����� ���� ���



		   //����㰡 ���� Ȯ���ϴ� �κ�
			int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_INFO->EndNID,pFlowMap);


			/// (5) ��������� ��� sequenceIndex �����ϴ� �κ�  puting------------------------------------------------------------------------------------------ MapData ���� �ʿ�.7
			if((uPathSearchIndex==0)||
				(FlowControlIndex != INDEX_NONE))
//				(pNODE_INFO->TagType ==STOP_NTYPE)||(pNODE_INFO->TagType ==COMMON_NTYPE))
			 {
    			 if(uPathSearchIndex==0)
                 {
                	uSequenceIndex = GetIncreaseSeqenceNumber();
                 }
                 else
                 {
                    if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_INFO->EndNID) ||
					(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_INFO->EndNID))
                    {
                        uSequenceIndex = GetIncreaseSeqenceNumber();
                    }
                    else
                    {
        			 	uSequenceIndex = GetSeqenceNumber();
                    }
                 }

			   //	ADD_LOG("[STOP/COMMONSequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);

			 }else
			 {
			 	uSequenceIndex = GetSeqenceNumber();
			   //	ADD_LOG("[SequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);
			 }
			 if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)
			 {
				ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d : %d]", uTmpFromNode, uTmpToNode, uSequenceIndex, nPathIndex);
			 }

			 if(uPathSearchIndex==0)
				uFirstSequenceIndex  = uSequenceIndex;

			////<(7)-1 Next Path ����
			uPathSearchIndex++;     //uPathSearchIndex �� ���Ͽ� ������ ���� �ֹǷ� ���� ���� �Ұ�


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//����ġ�� �ʿ����.

//			IterPos = mapPathSearchNode.find(uTmpToNode);
//			// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������ ����ó��.
//			if(IterPos == mapPathSearchNode.end())
//			{
				// PathSearch Node ���� ���� : ���� ���
				pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
				pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
				pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

			   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType = 
				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
				pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
				if(nSubPathIndex !=0)
					pPATH_SEARCH_NODE_INFO->SubNID = pSubPATH_INFO->EndNID;
				else
					pPATH_SEARCH_NODE_INFO->SubNID = 0;
				pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

				pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

				pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //������ ���õ� ���� �߰�. puting 20160119

				if((uPathSearchIndex ==1)&& (GetPathCount()!=0))
				{
					pTempEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *) ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering  = pPATH_INFO->Steering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  nTempDivInfo;
				}

				///puting �߰��ؾߵǴ� ����/ �ʱ�Ÿ��� Target�� ���� �Ÿ��� ����.
				//  Step 1.Target�� station�� ��� �Ÿ��� �߰��Ұ�.
				pPATH_SEARCH_NODE_INFO->dTotalDistance = (double)dTmpPathDistance + (double)dTmpTotalDistance;
				pPATH_SEARCH_NODE_INFO->dPathDistance  = (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

				//�߰��ϴ� ��ƾ
				PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

				///< (3)-1 ������ ��� ���� Ȯ��

				//���Ž���� ��츸 ī��Ʈ++
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode && m_bFindTheWay)
				{
					EndCount++;
					ADD_LOG("FindTheWay: End Count - %d", EndCount);
				}

				//���Ž���ϰ�� m_EndNodeCnt ��ŭ EndCount++ �϶� break
				//�ʱ���̵���� m_EndNodeCnt =0, EndCount = 0, ������� �����ڸ��� break
				//CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID �ǹ� -> Ÿ�� �θ��� == ������ ��尡 ������ �Ǵ� (�׻� ��� �ʿ�)
				//uPathSearchIndex !=1 �� ���Ž�� �ʱ� ���� ���Ͽ� �߰�   AOHC-301
				if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode && m_EndNodeCnt == EndCount) &&
				   ((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||

				   ((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(((uPathSearchIndex !=1) || (bNoonechek ==true)) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID)))))   //AOHC-301
				{
					m_EndNodeCnt = 0;
					EndCount = 0;

					ADD_LOG("[PATHSEARCH_Stepll %06f %d]",pPATH_SEARCH_NODE_INFO->dTotalDistance, AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
					if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						pPATH_SEARCH_NODE_INFO->dTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;


					bArrived = true;
					break;
				}

		}

		if(nTmpError!=NO_ERR) break;


		nPathIndex = 0;
		nSubPathIndex =0;


	 }  //while()


    ADD_LOG("[PATHSEARCH_Step 3]");
	///----------------3. ��ó�� �κ� ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	try{
	bArrived		= false;
	uPathSearchIndex= 0;

	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
	uCompSequenceIndex = 0;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
	}catch(...)
	{
		m_EndNodeCnt = 0;
		m_bFindTheWay = false;
		ADD_LOG("[ERR_PATHSEARCH_Exception111111111]");
    }



	///<(2) ���ִ� Path�� �ϳ��� ���� ��ó�� �۾�
	if( nTmpError == NO_ERR)
	{

		for(int PathIndex =0; PathIndex < uTmpListCount; PathIndex++)
		{
			uPathSearchIndex++; //���ִ� Path�� Pop Conut��!!

			if(nTmpError!=NO_ERR) break;

			try
			{
//			///< (2)-1 ��ó�� �� ���� Ȯ�� ����
			pPATH_SEARCH_NODE_INFO =(PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();

			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				delete pPATH_SEARCH_NODE_INFO;
				break;
			}
			}catch(...)
			   ADD_LOG("[ERR_PATHSEARCH_Exception22222222222]");

			//����/��������Ŭ�� �ƴҶ� Ȯ���ϴ� �κ�
			if((CmdGoData->CmdCycle != CYCLE::CycleNotExist) && (CmdGoData->CmdCycle != CYCLE::TakeOut))
			{
				if((CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NID) ==true) ||
				   (CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NextNID) ==true))
				   {
						nTmpError = ERR_LAYOUT_PATH_LIMIT;
						break;
				   }
			}

			//(4)  EXECUTE_CYCLEDRIVING_INFO��  List ���� ---------------
			//    (������ ID�� ������ �Ͼ�� �������� Ȯ�� �� ��. )
		 
			if((uCompSequenceIndex != pPATH_SEARCH_NODE_INFO->SQID) ||
				(uTmpListCount == uPathSearchIndex))  //Target Node �� ���, ������ Ndoe
			{
				 //������ ID ���� ���
				 uCompSequenceIndex = pPATH_SEARCH_NODE_INFO->SQID;

				 nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
				 pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

				 dStopNodeMarGin = (double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)) + (double)GetMarginOfSpeedDist((double)MAXSPEED(1500,pPATH_INFO->Speed), (m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount));// + PathExtra;  //puting path Extra ���� 20160727

				 if(dStopNodeMarGin > m_pParamSet->dPassPermitMaxMargin) dStopNodeMarGin = m_pParamSet->dPassPermitMaxMargin;

				//(4)-1���� ��ġ�� ���
				if(PathExtra != 0.0)
				{
					 PathExtra = 0.0;
				}

				ADD_LOG("[dStopNodeMarGin] dStopNodeMarGin : %f, %f %f, VHLACCEL:%f,  dTempMaxSpeed:%f Speed %f, %d %d",
						dStopNodeMarGin,
						(double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)),
						 (double)GetMarginOfSpeedDist((double)pPATH_INFO->Speed,(m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount)),
						 (double)VHLACCEL, (double)dTempMaxSpeed,(double)pPATH_INFO->Speed, m_pParamSet->nPassPermitPeriod, m_pParamSet->nPassPermitCount);

				if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount != uPathSearchIndex))
				{


					try{
					TempAdditionalInfo.DistToStation = 0.0;
					TempAdditionalInfo.StationType = MARK_TYPE_NODE;
					TempAdditionalInfo.FinalPortType = MARK_TYPE_NODE;
					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;


					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					pCYCLE_DRIVING_INFO    	= 	new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00, sizeof(PKT_CYCLECMDDATA));

					// 1-1�⺻���� ���
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					//��� LINK�� ���� ����㰡 ��û ����
					//���� Node�� ���, ������ �̵��� ����㰡 ������ ���� ����㰡 ��û �Ұ�.
					//excute info�� �̵� ������ �־��ִ� �������� ���.



					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint					=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType           	 	=  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	 	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance            		=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;      //Stop ����Ʈ ��ġ
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 				= CYCLENODETYPE_START2STOP; //Default�� ����
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum 	= (unsigned short) uCompSequenceIndex;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 0;

                	//�б����� �߰�.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					// 2-3 �۾������� ���� �б� ����
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
//
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].caDivNodePoint,TempDivInfo[TempDivCount].DivNodeDistance);
//					}

					ADD_LOG("[PassPermit_Start_NotMatch_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{


						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }

									 }
								}

							}
						 else
						 {
							if(pNODE_INFO->TagType == COMMON_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
							else if(pNODE_INFO->TagType == STOP_NTYPE)
                                pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;
                         }

					}
					else
					{
                      	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;

						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

                    }

					/*
					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
                      pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;

					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
                    }

                    */
//					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
//					{
//                    	ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDistance:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

//						for(int i=0; i< 3; i++)
//						{
//							if (HIGH_BYTE == 0)
//							 {
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
//							 }
//							 else
//							 {
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
//								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
//							 }
//
//						}
//					}
					 /*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }

						}
					}
                     */

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					}catch(...)
						ADD_LOG("[ERR_PATHSEARCH_Exception333333333]");

				}

				// (4)-1���� ��ġ�� ���, �н��� �ϳ��ۿ� ���� ���.
				else if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 0;
					memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					pCYCLE_DRIVING_INFO    					= new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
					// 1-1�⺻���� ���
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					//��� LINK�� ���� ����㰡 ��û ����
					//���� Node�� ���, ������ �̵��� ����㰡 ������ ���� ����㰡 ��û �Ұ�.
					//excute info�� �̵� ������ �־��ִ� �������� ���.

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_START2TARGET; //Default�� ����
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;

   					//�б����� �߰�.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 0;

				   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

				   					// 2-3 �۾������� ���� �б� ����
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//                    }
					ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

					//Reset �Ÿ��� �ֱ�. puting
					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}

									 }
								}

							}
						 else
						 {
							if(pNODE_INFO->TagType == COMMON_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
							else if(pNODE_INFO->TagType == STOP_NTYPE)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;
                         }

					}
					else
					{
                      	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;

						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

					}





				   /*
					if((pNODE_INFO->TagType == STOP_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					}
					else
					{
                       	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = 0;
                    	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
                    }


					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{
						ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDistance:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }

						}
					}
					*/
				   /*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }

						}
					}
                    */
					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint = pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
												bIsTargetStationoPassPermitOpen);


					//�������� ��� ����ó��.  puting 20160113
					if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
					{
						if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
							{
								if(bIgnorOverrun == true)
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

									TempAdditionalInfo.bIgnolOverRun = true;
                                }
								else if(bIsTargetStationoPassPermitOpen ==true)
								{
									//safety Level �� ���
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;
								}
								else
								{
									//�Ϲ����� ���
                                	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

								}
							}

							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
					}

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);

					//������ ��ġ�� Ȯ�εǸ� ������
					if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID))))

					{
						bArrived = true;

					}
					else if(!m_bFindTheWay)
					{
						nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
						ADD_LOG("[ERR_PATHSEARCH_FAIL_8]");
						ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
									ERR_PATHSEARCH_FAIL,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}


				}
				//(4)-2 ������ ��ġ �� ���.(�н��� �ϳ� �̻��� ���)
				else if((uPathSearchIndex!=FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 10;
					memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

					//���� ȹ��
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

                   	uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
                      	///<Step1. ���� ����㰡 ������ ���� ���� �߰�=================================================================================
//						pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
                                               //������尡 Reset�� ��� ����㰡 ó��.
						if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
                             	pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint   	=  pPATH_SEARCH_NODE_INFO->NID;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;

											/// ������ �����Ͽ� ������ ���� ����
								pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
								pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
								pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;

								ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
								pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

								nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															 bIsTargetStationoPassPermitOpen);

                                //���� ����Ŭ ���� ���
								pCYCLE_DRIVING_INFO    			   = new PKT_CYCLECMDDATA;

								memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
								// 1-1�⺻���� ���
								pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
								pCYCLE_DRIVING_INFO->CmdID = CmdID;
								pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
								pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
								pCYCLE_DRIVING_INFO->IsListF2B  = false;
								pCYCLE_DRIVING_INFO->Isreturn = false;

								//��� LINK�� ���� ����㰡 ��û ����
								//���� Node�� ���, ������ �̵��� ����㰡 ������ ���� ����㰡 ��û �Ұ�.
								//excute info�� �̵� ������ �־��ִ� �������� ���.

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

								//�б����� �߰�.  puting 20170312
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;
									   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_STOP2STOP; //Default�� ����
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
							   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

								ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

								//Reset �Ÿ��� �ֱ�. puting

								if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 10;
								//����㰡 �Ÿ��� ����
								if((pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance)
								  && (pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance))
								  {
										double TempPreDist  = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;
										double TempDist  = pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;

										if( TempDist > TempPreDist)
										{
											ADD_LOG("[PassPermit MOD] Pre %6.11f/%6.11f/%6.11f/%6.11f", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance;
										}
								  }

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint		 =  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;
								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;
								ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
                                        }

									 }
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint=  pPATH_SEARCH_NODE_INFO->NextNID;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


								if(uCompSequenceIndex == uFirstSequenceIndex)
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
								}
								else
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
								}

								ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

								nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															bIsTargetStationoPassPermitOpen);


								//�������� ��� ����ó��.  puting 20160113
								if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
								{
									if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
										{
											if(bIgnorOverrun == true)
											{
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                            	TempAdditionalInfo.bIgnolOverRun = true;
											}
											else if(bIsTargetStationoPassPermitOpen ==true)
											{
												//safety Level �� ���
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;

											}else
											{
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
												pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

											}
										}

										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
								}




								pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);

							}else
							{
                                	pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


									if(uCompSequenceIndex == uFirstSequenceIndex)
									{
										pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
									}
									else
									{
										pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
									}

									ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
									pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

									nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
															pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
															 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
															 bIsTargetStationoPassPermitOpen);

									//�������� ��� ����ó��.  puting 20160113
									if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
									{
										if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
											{
												if(bIgnorOverrun == true)
												{
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                                	TempAdditionalInfo.bIgnolOverRun = true;
												}
												else if(bIsTargetStationoPassPermitOpen ==true)
												{
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;


												}else
												{
                                                    pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
												}
											}

											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
									}
                            }




					}else
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

		//					if(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance ==0.0)
		//						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					 //	ADD_LOG("[PassPermit_End] EndND:%d, NType:%d", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
												 bIsTargetStationoPassPermitOpen);

						//�������� ��� ����ó��.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									if(bIgnorOverrun == true)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;

										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

                                    	TempAdditionalInfo.bIgnolOverRun = true;

									}
									else if(bIsTargetStationoPassPermitOpen ==true)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = true;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;

									}else
									{
                                        pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
									}
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}
                    }


					/*
					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{

						pCYCLE_DRIVING_INFO    			   = new PKT_CYCLECMDDATA;

						memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
						// 1-1�⺻���� ���
						pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
						pCYCLE_DRIVING_INFO->CmdID = CmdID;
						pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
						pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
						pCYCLE_DRIVING_INFO->IsListF2B  = false;
						pCYCLE_DRIVING_INFO->Isreturn = false;

						//��� LINK�� ���� ����㰡 ��û ����
						//���� Node�� ���, ������ �̵��� ����㰡 ������ ���� ����㰡 ��û �Ұ�.
						//excute info�� �̵� ������ �־��ִ� �������� ���.

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - ((double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode) - (double)dStopNodeMarGin;

                       					// 2-3 �۾������� ���� �б� ����
//						if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//						{
//
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////							ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//						}
					   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_STOP2STOP; //Default�� ����
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
					   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

						ADD_LOG("[PassPermit_Start_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

						//Reset �Ÿ��� �ֱ�. puting

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint		 =  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;

						ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }

						}


						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint=  pPATH_SEARCH_NODE_INFO->NextNID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
													pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
													&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);


						//�������� ��� ����ó��.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}


						pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);


					}
					else
					{

						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NextNID;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

		//					if(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance ==0.0)
		//						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - pParamSet->dPassPermitOpenMargin;
					 //	ADD_LOG("[PassPermit_End] EndND:%d, NType:%d", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


						if(uCompSequenceIndex == uFirstSequenceIndex)
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
						}
						else
						{
							pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
						}

						ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

						nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);

						//�������� ��� ����ó��.  puting 20160113
						if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
						{
							if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == RESET_NTYPE) || (pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType == COMMON_NTYPE))
								{
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;
								}

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = AdditionalInfo.ProcessingGoInfo.OriginEndNodeID;
						}

					}



                     */
					//��������������  ����㰡�� ���� ������ �ؾߵ��� �Ǵ��ϴ� �κ� �߰�
					//�����ʿ��� ������ ����㰡 ��û�� ������ �����, ����㰡 ������ �ʿ��� ���, ������.


					//������ ��ġ�� Ȯ�εǸ� ������


					if((pPATH_SEARCH_NODE_INFO->NextNID == uToNode) &&
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false) ||
						((AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) &&(CmdGoData->PathData_EndNodeID ==pPATH_SEARCH_NODE_INFO->NID))))

					{
						bArrived = true;
					
					}
					else if (!m_bFindTheWay)
					{
						nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
						ADD_LOG("[ERR_PATHSEARCH_FAIL_8]");
						ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
									ERR_PATHSEARCH_FAIL,
									m_pOHTMainStatus->StatusCommon.CurNodeID,
									m_pExecuteInfo->ExecutePositionInfo.NextNode,
									m_pOHTMainStatus->StatusCommon.CurNodeOffset,
									m_pOHTMainStatus->StatusCommon.ExistFoup,
									m_pOHTMainStatus->StatusCommon.StopStationID);
					}
				
				}
				//(4)-3�߰� ���� ����� ���
				else
				{
//					if(dStopNodeMarGin < 0) dStopNodeMarGin = 10;

					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;

					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					///<Step1. ���� ����㰡 ������ ���� ���� �߰�=================================================================================
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

					/// ������ �����Ͽ� ������ ���� ����
              		pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;


					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;


					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
												pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
												 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);

				

					///<Step2. ���� ����㰡 ������ ���� ���� ����=================================================================================
					pCYCLE_DRIVING_INFO    						= new PKT_CYCLECMDDATA;
					memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
					// 2-1�⺻���� ���
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					// 2-2 ������� ���� ���
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType 		  	=  pNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance 		  	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance 		  	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			=  CYCLENODETYPE_STOP2STOP; //Default�� ����

					//�б����� �߰�.  puting 20170312
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.Division = pPATH_SEARCH_NODE_INFO->DivInfo;

					if(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance < 0) pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = 10;
					// 2-3 �۾������� ���� �б� ����
//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////
////						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//                    }
				 //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
                    //�������� ���� �κ� �߰�.


					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)uCompSequenceIndex;
				   //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

					ADD_LOG("[PassPermit_Mid] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);

					//Reset NTYpe�� ���� puting
	//				if((pNODE_INFO->TagType == RESET_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
	//				{
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;

	//				}
	//				else if(pNODE_INFO->TagType == STOP_NTYPE)
	//				{
	//					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
	//				}


					//����㰡 �Ÿ��� ����
					if((pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance)
					  && (pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance > pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance))
					  {
							double TempPreDist  = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;
							double TempDist  = pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance -  pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance;

							if( TempDist > TempPreDist)
							{
								ADD_LOG("[PassPermit MOD] Pre %6.11f/%6.11f/%6.11f/%6.11f", pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance, pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance = pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance;
							}
					  }


					int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

					if(FlowControlIndex != INDEX_NONE)
					{
						 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
							(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
							{
								if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
								else
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

								ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
								for(int i=0; i< 3; i++)
								{
									if (HIGH_BYTE == 0)
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}
									 }
									 else
									 {
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
										if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										}else
										{
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
											pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										}

									 }
								}

							}
						 else
						 {
							ADD_LOG("���ܻ�Ȳ�߻� Path ����㰡");
						 }

					}




					 /*
					if((pNODE_INFO->TagType == STOP_NTYPE) ||(pNODE_INFO->TagType == COMMON_NTYPE))
					{

						ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d, StartDist:%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pPATH_SEARCH_NODE_INFO->SubNID >> (i * 8);
							 }
						}
					}
                     */
					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					
				}


                dTempMaxSpeed = 0.0;  //Max Speed �ʱ�ȭ
			}else
			{
				try{

					// 2-3 �۾������� ���� �б� ����
					TempAdditionalInfo.ProcessingGoInfo.IsStationEnd = false;

					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
					nNodeIndex = pTmpMap->FindNodeIndex(pPATH_SEARCH_NODE_INFO->NID);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

//					if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//					{
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodePPoint = TempDivInfo[TempDivCount].DivNodePPoint;
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivInfo[DivCount].DivNodeDistance = TempDivInfo[TempDivCount].DivNodeDistance;
////						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.DivCount++;
////						ADD_LOG("[PassPermit_DIV_Point] SQnum %d StartND:%d,DivNodeDistance %f",uCompSequenceIndex,TempDivInfo[TempDivCount].DivNodePPoint,TempDivInfo[TempDivCount].DivNodeDistance);
//					}

					/// ������ �����Ͽ� ������ ���� ����
              		pTempEXECUTE_DRIVING_INFO =(EXECUTE_DRIVING_INFO*)ReferBackPath();
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreSteering =  pPATH_SEARCH_NODE_INFO->PreSteering;
					pTempEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo =  pPATH_SEARCH_NODE_INFO->DivInfo;

					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
                    	}catch(...)
						 ADD_LOG("[ERR_PATHSEARCH_Exception77777777]");

            }

			//MAP MAKER �κ� ���Ŀ� �����ϱ� PUTING
			/*
			// MapMake �� ��� Map Make �ӵ� �ݿ� : ��ġ �� ó�� ����� ���� �߰� ��� �ʿ�
			if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
				(pCYCLECMDDATA_INFO->EXE_Driving_Info.BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pCYCLECMDDATA_INFO->EXE_Driving_Info.BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
			}

			Route_ADD_LOG("[PathSearch] Node:%X~%X",pEXECUTE_DRIVING_INFO->StartNID, pEXECUTE_DRIVING_INFO->EndNID);
			///< ������� �־���
			pResultAddList->insertBack((void*)pCYCLECMDDATA_INFO);
			*/

			//ó������ �ϳ��� Path ������ ��.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		   //	ADD_LOG("[ExcuteDrivingInfo: %d", nPathIndex);
			// (3) EXECUTE_DRIVING_INFO�� PathSearcing List ���� ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


			//Excute Info�� ����� Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType      	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
			pEXECUTE_DRIVING_INFO->BasicInfo.bChargeEnable	= pPATH_INFO->ChargeEnable;   //eco test 
			//�ӽ�
//			if(pPATH_INFO->PathDirection == PATH_DIRECTION_SHORT)
//				 pPATH_INFO->PathDirection  = PATH_DIRECTION_SPECIAL_R;
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
            		//Map Maker ���� �ӵ�
			if((AdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)  &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
				ADD_LOG("[Map Maker PathSearch...]");

			}else if((bIsFoup ==false) && (m_pParamSet->nIsCurveSpeedUp == true) &&( bUseAVS == false))
			{
#if(USE_CHANGENODE == 1)
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion;
#else
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.Direction & 0x0F;
#endif
				//N�б�, ������ �ƴҰ�쿡 ����ӵ� ���� : ������� -300����
				if((nTempFoupCoverDectect ==PATH_DIRECTION_LEFT) || (nTempFoupCoverDectect ==PATH_DIRECTION_RIGHT))
				{

					if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 0.75) && (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0)))//Short Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec
					else if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 1.0)&& (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0))) // Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec

					ADD_LOG("[IsFoup :Speed Down %f]",pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed);
				}

			}

			memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &TempAdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

			AddPath(pEXECUTE_DRIVING_INFO);


			if((pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTIN_NTYPE_LEFT) || (pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTIN_NTYPE_RIGHT) || (pEXECUTE_DRIVING_INFO->BasicInfo.NodeType == LIFTOUT_NTYPE))
				dTempMaxSpeed = pPATH_INFO->Speed;
			else
				dTempMaxSpeed = MAXSPEED(pPATH_INFO->Speed,dTempMaxSpeed); // mm/s

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;

			//�۾��������� ���� Ȯ�� �κ�
//			if(pPATH_SEARCH_NODE_INFO->NID == TempDivInfo[TempDivCount].ChangeNodePoint)
//            	TempDivCount++;

			delete pPATH_SEARCH_NODE_INFO;
		}
	}
	ADD_LOG("[PATHSEARCH_Step 4]");
	m_bFindTheWay = false;
	m_EndNodeCnt = 0;
   //	if(uTmpFromNode !=  uTmpFromNode)  nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
	///< Error �߻��� ��� List ���� �ϴ� �κ�
	if(nTmpError != NO_ERR)
	{
		//1. Path List ���� ����
		InitPathList();

		//2. Cycle Lis ���� ����
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. �ӽ� Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

	ChangePathVelocity();

	return nTmpError;
}


/**
@brief   ������ �θ��忡�� ������ ��忡�� �Ǵ� ������ ���� �̵��� ��� Tag Map Search �Լ�
@author  puting
@date    2015-12-04
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/
//AOHC-195 ������ ���� �۾�(����2)  ->  bIgnorOverrun ��
int PathSearch::StandardSearchOnlyOnePassPermit(MAP *pMap, STD_FlowControl *pFlowMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List, bool bcheckTarget, PKT_CYCLECMDDATA *FinalCycleData, UINT &uCheckExtraNum, double &dTempDis, UINT FinalSeqNum,PASSPERMIT_COMMON TempPassPermitData,bool bIsFoup, bool bIgnorOverrun,bool bturnoff)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;				// ���� ���� �ʱ�ȭ

	UINT	CmdID				= CmdGoData->CmdID; 	//���� ID
	MAP		*pTmpMap			= pMap;					// ����ϴ� Map

	//���Ž���� �ؾߵ� ���۰� �� ��ġ
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// ���� ���
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;			// �� ���
	bool	bPassPermit			= false;	// Map Seach�� ������ ���

	LList	*pResultAddList		= DRIVING_INFO_List;	// ������ ���� ����� �߰��ϱ� ���� Cycle List

	bool	bArrived			= false;	// Map Seach�� ������ ���

	UINT	uPathSearchIndex	= 0;		// PathSearch List�� Index
//	UINT	uListInsertIndex	= 0;		// PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		// PathSearch List ���� ���� ��

	UINT 	uSequenceIndex      = 0;      // ������ Number
	UINT 	uCompSequenceIndex  = 0;      // �ӽ� ������ Number

 //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node�� Index
	int		nPathIndex			= 0;		// Path�� Index
	int		nSubPathIndex	  	= 0;		// Path�� ����㰡�� ���� Index

	PATH_DIVERGENCE_INFO     nTempDivInfo         = PATH_DIVERGENCE_NONE;

	UINT	uTmpToNode			= 0;		// Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		// Path ������ ���� Ȯ�� ��
//	UINT	uTmpNode			= 0;		// �ӽ� ���� Ȯ�� ��

	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)
	double  dStopNodeMarGin     = 0.0;
	double  dTmpBetweenDistance = 0.0;

	//�⺻ �б� ���� // �߿��� �κ�. 0 : ���� 1: ������
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

//	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit üũ��

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path ������ �̿��� ����㰡 Sub������ ȹ��

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst�� �����ϴ� ����ü
//	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü


	LList PathSearchList;									// PathSearch�� ���� List
//	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	// Node ������ ã�� ���� Map
//	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	bool bIsStartStationPassPermit = false;     //��� ��ġ�� ����㰡�� �ް� �̵��ϴ� ���
	bool bIsTargetStationoPassPermitOpen = false; //������ ��ġ�� ����㰡�� �ȹް� �̵��ϴ� ���


//	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	//------------- 1. ��ó�� �κ�------------//

	// 1. �ʱ� ���� : ----------------------------------------------------------
	//   0) ���� Node�� Path Ȯ��
//	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
//	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
//	||	(uFromNode == uToNode)
//	)
//	{
//
//		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
//			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
//		return ERR_PATHSEARCH_FAIL;
//	}

	//  1) Station�� Path Ȯ��   //�뵵 ?  puting
	/*
	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}
	*/

	///������ Station�ΰ�쿡 ���� ��ó�� �κ�
	if(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true)
	{
		//�������� ��� �θ��尡 ���´�.
		ADD_LOG("[PATHSEARCH_Start_Station_5] %d",CmdGoData->PathData_StartNodeID);
		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_StartNodeID);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
					   // bDefaultPathDirection = n;
						//AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
					}

					if(AdditionalInfo.ProcessingGoInfo.PathDiv !=0x00)
					{
						if(((AdditionalInfo.ProcessingGoInfo.PathDiv ==0x01) && (n==0)) ||
						   ((AdditionalInfo.ProcessingGoInfo.PathDiv ==0x02) && (n==1)))
						   {
						   	   //	bDefaultPathDirection = n;
								if(AdditionalInfo.ProcessingGoInfo.EndNodeID==0)
									AdditionalInfo.ProcessingGoInfo.EndNodeID = pPATH_INFO->EndNID;
								  //	TempEndID  =  pPATH_INFO->EndNID;
									uToNode =  pPATH_INFO->EndNID;
						   }
						else if(AdditionalInfo.ProcessingGoInfo.PathDiv ==0x03)
						{
							if(AdditionalInfo.ProcessingGoInfo.EndNodeID==0)
									AdditionalInfo.ProcessingGoInfo.EndNodeID = pPATH_INFO->EndNID;
                           //	TempEndID  =  pPATH_INFO->EndNID;
							uToNode =  pPATH_INFO->EndNID;
                        }

                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode < 0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[PATHSEARCH_Start_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_55] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}

	////<(7)-1 Next Path ����
	///Target�� Station�ΰ�쿡 ���� ��ó�� �κ�
	if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
	{
		//�������� ��� �θ��尡 ���´�.

		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_EndNodeID);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		ADD_LOG("[PATHSEARCH_End_Station_5] %d",CmdGoData->PathData_EndNodeID);
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.EndNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist;
					}
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[PATHSEARCH_End_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_56] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}



	if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) ||
	   (AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false))
	 {
		bPassPermit = true;     //����㰡���� ó�� �ʿ�.

	 }
	 else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
	 {
     	dTmpBetweenDistance =  AdditionalInfo.ProcessingGoInfo.StationData_EndRunDist - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
	 }


     //puting ������� ������ Tag�� ���� �߰��κ� 20170803
      if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true) ||
	   (AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
	   {
			bIsTargetStationoPassPermitOpen =  CmdGoData->bIsTargetStationoPassPermitOpen;
       }
	//------------- 2. �ֿ� ó���κ�------------//

	// 3) ���� Node��ġ List�� ����
	uPathSearchIndex = 0;
    uSequenceIndex = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ ���۳�尡 �߰ߵǸ� ����


	////< (1) first Node Check!!
 	uTmpFromNode = uFromNode;
	nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


	////< (4) ���� ��尡 �ƹ��� PathIndex�� ���� ��� ���� ó�� �߰�
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
		ADD_LOG("[ERR_PATHSEARCH_FAIL_57] %X:%X"
		, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

//	if((pNODE_INFO->TagType ==STOP_NTYPE)||(pNODE_INFO->TagType ==COMMON_NTYPE))


	///<(6) Next Node(Path) ������ � ���� Ȯ���ϴ� �κ�  puting 20151107
	////< (6)-1 �б��� ���,
	if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		  ////< (6-1) �⺻�� �������� Ȥ���ϴ� �κ�  puting 20151107
		if(CmdGoData->PathData_ExtraID[uCheckExtraNum]==pNODE_INFO->NID)
		{
			bCheckDefaultPathDirection = true;  //�⺻�� �б� �� ���
			uCheckExtraNum++;

			ADD_LOG("[�б�ܹ��� %d %d DefDir %d]", CmdGoData->PathData_ExtraID[uCheckExtraNum] ,pNODE_INFO->NID,bDefaultPathDirection);

			if(uCheckExtraNum > CmdGoData->PathData_ExtraNum)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}
		}
		/////< (6-2) �б⿡�� ���ߵ� Path ���� Ȯ��

		////Map �κ� ���� �ʿ�.
		for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// ���� Path ���� ��ŭ Ȯ��
			{

				if(bCheckDefaultPathDirection ==true)
				{
					if(bDefaultPathDirection != nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //������ �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //����㰡 Sub�κ��� ���� Check

				}
				else
				{
					if(bDefaultPathDirection == nTempIndex)
					{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //���� �б�

						nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
//
//						pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
//
//						ADD_LOG("[bDefaultPathDirection %d][n : %d, Direction %d]", bDefaultPathDirection , nTempIndex, pPATH_INFO->PathDirection);
					}
					else
						nSubPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];  //����㰡 Sub�κ��� ���� Check
				}


			}
             	 //������ �̵��� �б��� ���, �������� �б������� ����. ��Ŭ���� ó�� 20160422
			if((CmdGoData->PathData_EndNodeID == pNODE_INFO->NID) &&
				(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
			{
				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.EndNodeID)
				{
					int Temp =0;
					Temp = nPathIndex;
					nPathIndex = nSubPathIndex;
					nSubPathIndex = Temp;

					if(nTempDivInfo == PATH_DIVERGENCE_LEFT)
						nTempDivInfo = PATH_DIVERGENCE_RIGHT;
					else
						nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�

					ADD_LOG("Station Target End %d %d %d ",nPathIndex, nSubPathIndex,nTempDivInfo);
				}
			}
			//������ �̵��� �б��� ���, �������� �б������� ����. �������� ó���ؾߵ�
//			else if((CmdGoData->PathData_StartNodeID == pNODE_INFO->NID) &&
//				(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true))
//			{
//				pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
//				if(pPATH_INFO->EndNID != AdditionalInfo.ProcessingGoInfo.StartNodeID)
//				{
//					int Temp =0;
//					Temp = nPathIndex;
//					nPathIndex = nSubPathIndex;
//					nSubPathIndex = Temp;
//
//					ADD_LOG("Station Target Start");
//				}
//			}

//		bCheckDefaultPathDirection = false;
	}

	////< (6-3) �бⰡ �ƴѰ��,
	else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
//		nTempDivInfo =  PATH_DIVERGENCE_NONE;
		for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
		{
			if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				nPathIndex = pNODE_INFO->NextPathIndex[n];
		}

	}

	///< (7) ����� Next Path(Node) ���� Ȯ�� �� ����
	////<(7)-1 ����� Path�� ���� �ϴ� ���
	if(nPathIndex!=-1)
	{

		////<(7)-1 Next Path ����
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
		if(nSubPathIndex !=0)
			pSubPATH_INFO = &(pTmpMap->m_PathArray[nSubPathIndex]);		// ����� Path ����

		//����ġ�� �ʿ����.
		dTmpPathDistance = (double)pPATH_INFO->Distance;
		uTmpToNode = pPATH_INFO->EndNID; //����� ���� ���

		int FlowControlIndex =pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_INFO->EndNID,pFlowMap);

		if(FinalSeqNum !=0)
		   uSequenceIndex =  FinalSeqNum;
		else
		{
			if(FlowControlIndex != INDEX_NONE)
			{
				if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_INFO->EndNID) ||
					(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_INFO->EndNID))
					{
                    	uSequenceIndex = GetSeqenceNumber();//7102;
					}
					else
						uSequenceIndex = GetIncreaseSeqenceNumber();//7102;

			}else
				uSequenceIndex = GetIncreaseSeqenceNumber();//7102;

		}

		// PathSearch Node ���� ���� : ���� ���
		pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
		pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
		pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

	   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
		pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
		pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
		if(nSubPathIndex !=0)
			pPATH_SEARCH_NODE_INFO->SubNID = pSubPATH_INFO->EndNID;
		else
			pPATH_SEARCH_NODE_INFO->SubNID = 0;
		pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;

		pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;

		ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d]", uTmpFromNode, uTmpToNode, uSequenceIndex);
		///puting �߰��ؾߵǴ� ����/ �ʱ�Ÿ��� Target�� ���� �Ÿ��� ����.
		//  Step 1.Target�� station�� ��� �Ÿ��� �߰��Ұ�.
		if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
		{
			 pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpBetweenDistance;
			 pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
		}
		else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==false) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true))
		{
			if(bcheckTarget ==false)
			{
				pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;
				pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
			}
			else
			{
				ADD_LOG("[PATHSEARCH_Stepll %06f %06f %d %d]",pPATH_SEARCH_NODE_INFO->dTotalDistance, pPATH_SEARCH_NODE_INFO->dPathDistance,FinalCycleData->EXE_CycleDriving_Info.EndDistance, dTempDis);
				pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_EndRunDistOfNextNode;
				pPATH_SEARCH_NODE_INFO->dTotalDistance =  (double)pPATH_SEARCH_NODE_INFO->dPathDistance  + (double)FinalCycleData->EXE_CycleDriving_Info.EndDistance  + dTempDis;
			}

		}
		else if((AdditionalInfo.ProcessingGoInfo.IsStationStart ==true) && (AdditionalInfo.ProcessingGoInfo.IsStationEnd ==false))
		{
			pPATH_SEARCH_NODE_INFO->dPathDistance = (double)dTmpPathDistance - (double)AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
			pPATH_SEARCH_NODE_INFO->dTotalDistance =  pPATH_SEARCH_NODE_INFO->dPathDistance  + dTempDis;
		}

		ADD_LOG("[PathSearch_Check][dTotalDistance:%f/IsStationStart:%d/IsStationEnd%d]", pPATH_SEARCH_NODE_INFO->dTotalDistance, AdditionalInfo.ProcessingGoInfo.IsStationStart, AdditionalInfo.ProcessingGoInfo.IsStationEnd);
		//�߰��ϴ� ��ƾ
		PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
		//mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

		if(pPATH_SEARCH_NODE_INFO->NextNID != uToNode)
		{
			ADD_LOG("[PathSearch_Check][Same Node]");
			nTmpError = ERR_PATHSEARCH_FAIL;
			ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		}


	}	// if(nPathIndex!=-1)





	///----------------3. ��ó�� �κ� ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	bArrived		= false;
	uPathSearchIndex= 0;
	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
    uCompSequenceIndex = 0;

	///<(1) ������ �ܿ��� Path ������ ������ ���� ó��. (�������� ó��?) ���Ŀ� ����ó�� ���� ��.
//	if(GetPathCount() !=0)
//	{
//		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
//    }

	///<(2) ���ִ� Path�� �ϳ��� ���� ��ó�� �۾�
	if( nTmpError == NO_ERR)
    {


			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[ERR_PATHSEARCH_FAIL_OnlyOne_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);

			}

			//����/��������Ŭ�� �ƴҶ� Ȯ���ϴ� �κ�
			if((CmdGoData->CmdCycle != CYCLE::CycleNotExist) && (CmdGoData->CmdCycle != CYCLE::TakeOut))
			{
				if((CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NID) ==true) ||
				   (CheckEmergencyPathInfo(pTmpMap,pPATH_SEARCH_NODE_INFO->NextNID) ==true))
				   {
						nTmpError = ERR_LAYOUT_PATH_LIMIT;
				   }
			}
			//ó������ �ϳ��� Path ������ ��.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

			// (3) EXECUTE_DRIVING_INFO�� PathSearcing List ���� ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			//Excute Info�� ����� Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= uTmpFromNode;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType     	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
			//AOHC-195 ������ ���� �۾�(����2)
			if(bIgnorOverrun == true)
			{
				AdditionalInfo.bIgnolOverRun = true;
			}

			pEXECUTE_DRIVING_INFO->AddInfo					= AdditionalInfo;


			//Map Maker ���� �ӵ�
			if((AdditionalInfo.CmdType == EXECUTE_DRIVING_CMD_MAPMAKE)  &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
			{
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
				ADD_LOG("[Map Maker PathSearch...]");

			}else if((bIsFoup ==false) && (m_pParamSet->nIsCurveSpeedUp == true) && (bUseAVS == false))
			{
#if(USE_CHANGENODE == 1)
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion;
#else
				int nTempFoupCoverDectect = pEXECUTE_DRIVING_INFO->BasicInfo.Direction & 0x0F;
#endif
				//N�б�, ������ �ƴҰ�쿡 ����ӵ� ���� : ������� -300����
				if((nTempFoupCoverDectect ==PATH_DIRECTION_LEFT) || (nTempFoupCoverDectect ==PATH_DIRECTION_RIGHT))
				{

					if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 0.75) && (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0)))//Short Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsShoutCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec
					else if((pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed	== 1.0)&& (pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed < (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0))) // Curve
						pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(m_pParamSet->nIsCurveSpeedUpValue/1000.0);			// mm/s -> mm/msec

					ADD_LOG("[IsFoup :Speed Down %f]",pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed);
				}

			}

			AddPath(pEXECUTE_DRIVING_INFO);

			dStopNodeMarGin = (double)GetSpeedDownDist((double)VHLACCEL,(double)MAXSPEED(1500,pPATH_INFO->Speed)) +  (double)GetMarginOfSpeedDist((double)MAXSPEED(1500,pPATH_INFO->Speed),(m_pParamSet->nPassPermitPeriod*m_pParamSet->nPassPermitCount));

			ADD_LOG("[Only dStopNodeMarGin] dStopNodeMarGin %f ,VHLACCEL:%f, dTempMaxSpeed:%f  %d %d",dStopNodeMarGin, (double)VHLACCEL, (double)pPATH_INFO->Speed, m_pParamSet->nPassPermitPeriod, m_pParamSet->nPassPermitCount);
//			double k = sqrt(sqrt((double)GetSpeedDownDist((double)VHLACCEL,(double)pPATH_INFO->Speed)));

			//(4)  EXECUTE_CYCLEDRIVING_INFO��  List ���� ---------------
			//    (������ ID�� ������ �Ͼ�� �������� Ȯ�� �� ��. )

			//(4)-1���� ��ġ�� ���


			// (4)-1���� ��ġ�� ���, �н��� �ϳ��ۿ� ���� ���.

				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
				nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
				pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

				uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
				nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
				pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


				pCYCLE_DRIVING_INFO    					= new PKT_CYCLECMDDATA;

				memset(pCYCLE_DRIVING_INFO, 0x00,sizeof(PKT_CYCLECMDDATA));
				// 1-1�⺻���� ���
				pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
				pCYCLE_DRIVING_INFO->CmdID = CmdID;
				pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
				pCYCLE_DRIVING_INFO->IsNoExcuteCycle = false;
				pCYCLE_DRIVING_INFO->IsListF2B  = false;
				pCYCLE_DRIVING_INFO->Isreturn = false;

				//��� LINK�� ���� ����㰡 ��û ����
				//���� Node�� ���, ������ �̵��� ����㰡 ������ ���� ����㰡 ��û �Ұ�.
				//excute info�� �̵� ������ �־��ִ� �������� ���.

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint				=  pPATH_SEARCH_NODE_INFO->NID;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType            =  pNODE_INFO->TagType;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance           	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - (double)pPATH_SEARCH_NODE_INFO->dPathDistance - (double)dStopNodeMarGin;

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 			= CYCLENODETYPE_START2TARGET; //Default�� ����
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum =  (unsigned short)pPATH_SEARCH_NODE_INFO->SQID;
				//YCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.SQNum =  0;

				if(AdditionalInfo.ProcessingGoInfo.IsStationEnd ==true)
				{
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = pPATH_SEARCH_NODE_INFO->NID; // pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.AddInfo.TargetStationID = AdditionalInfo.StationID;
				}
				else
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint = pPATH_SEARCH_NODE_INFO->NextNID; // pPATH_SEARCH_NODE_INFO->NextNID;

				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint = pPATH_SEARCH_NODE_INFO->NextNID; // pPATH_SEARCH_NODE_INFO->NextNID;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNextNODE_INFO->TagType;
				pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;

              	dTempDis += (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;


				ADD_LOG("[PassPermit_Start_OnlyOne_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);




				ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d StartDist:%06f", pPATH_SEARCH_NODE_INFO->SQID, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);


				int FlowControlIndex = pTmpMap->FindNodeIndexFromFlowControl(pNODE_INFO->NID,pPATH_SEARCH_NODE_INFO->NextNID,pFlowMap);

				if(FlowControlIndex != INDEX_NONE)
				{
					 if((pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID) ||
						(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub ==pPATH_SEARCH_NODE_INFO->NextNID))
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint = pPATH_SEARCH_NODE_INFO->NextNID;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =   (double)pPATH_SEARCH_NODE_INFO->dTotalDistance - m_pParamSet->dPassPermitOpenMargin;


							if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMainDefaultValue;
							else
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance -= pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSubDefaultValue;

							ADD_LOG("[����㰡 ����] ������ Num:%d, StopNode:%d,ResetNodeMain:%d,ResetNodeSub:%d Start Distance%06f", uCompSequenceIndex, pPATH_SEARCH_NODE_INFO->NID , pPATH_SEARCH_NODE_INFO->NextNID, pPATH_SEARCH_NODE_INFO->SubNID,pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance);
							for(int i=0; i< 3; i++)
							{
								if (HIGH_BYTE == 0)
								 {
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
									if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
									}else
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[2-i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
									}
								 }
								 else
								 {
									pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
									if(pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain ==pPATH_SEARCH_NODE_INFO->NextNID)
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
									}else
									{
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Main[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeSub >> (i * 8);
										pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.ResetNodeID_Sub[i] = pFlowMap->m_NodeArray[FlowControlIndex].ResetNodeMain >> (i * 8);
									}

								 }
							}

							bPassPermit = true;
							bIsStartStationPassPermit =true;
							/*
							//������峻 ������ �±׿��� �̵��ϴ� ���
							if(TempPassPermitData.SQNum ==0)
							{
								bPassPermit = true;
								bIsStartStationPassPermit =true;
							}
							else
							{
								if(memcmp(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData, &TempPassPermitData, sizeof(PASSPERMIT_COMMON)) != 0)
								{
									nTmpError = ERR_STD_ST_PASSPERMIT_FAIL;
								}
							}
							*/

						}
					 else
					 {
						if(pNODE_INFO->TagType == COMMON_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
						else if(pNODE_INFO->TagType == STOP_NTYPE)
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

						if(bcheckTarget ==false)
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  0;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
						}
						else
						{
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  pPATH_SEARCH_NODE_INFO->NID;
							pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
						}

					 }

				}else
				{
					if(pNODE_INFO->TagType == COMMON_NTYPE)
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = RESET_NTYPE;
					else if(pNODE_INFO->TagType == STOP_NTYPE)
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType = NORMAL_NTYPE;

					if(bcheckTarget ==false)
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  0;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance = 0.0;
					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetPoint =  pPATH_SEARCH_NODE_INFO->NID;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance =  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
					}
				}

					/*
					if((pNODE_INFO->TagType == RESET_NTYPE) || (pNODE_INFO->TagType == COMMON_NTYPE))
					{
					   //memmove(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData,&FinalCycleData->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));
					   //emmove(&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData,&FinalCycleData->EXE_CycleDriving_Info.CurLinkPassPermitData, sizeof(PASSPERMIT_COMMON));

						for(int i=0; i< 3; i++)
						{
							if (HIGH_BYTE == 0)
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[2-i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[2-i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
							 else
							 {
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.StopNodeID[i] = pPATH_SEARCH_NODE_INFO->NID >> (i * 8);
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PreLinkPassPermitData.ResetNodeID_Main[i] = pPATH_SEARCH_NODE_INFO->NextNID >> (i * 8);
							 }
								//	pCYCLE_DRIVING_INFO->PassPermitData.ResetNodeID_Main = pPATH_SEARCH_NODE_INFO->NextNID;
								///puting Reset Sub�κ� ��� ������ ����...
							//	pCYCLE_DRIVING_INFO->PassPermitData.ResetNodeID_Sub =0;

						}

					}
					*/

					ADD_LOG("[OnlyPassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d %d",
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
						bPassPermit);

                     //
					if((bPassPermit) && (nTmpError == NO_ERR))
					{
						nTmpError = CheckOnlyOnePassPermit(pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
														pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
														pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
														&pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info,
														AdditionalInfo.ProcessingGoInfo.IsStationStart,
														AdditionalInfo.ProcessingGoInfo.IsStationEnd,
														bIsStartStationPassPermit,
														bIsTargetStationoPassPermitOpen);

							//AOHC-195 ������ ���� �۾�(����2)
							if(bIgnorOverrun == true)
							{
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

//								AdditionalInfo.bIgnolOverRun = true;
							}
							else if((pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.ResetNodeDistance < 0) && (bturnoff==true)) // �⵿ Off���� ������ġ�� ��� ���� �ݳ� ��ġ�� �շ�����ġ�� ���
							{
								ADD_LOG("�⵿Off && ResetNodeDistance < 0 ���� In");
                             	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = true;
								pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
                            }
					}
					else
					{
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;
						pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					  //	pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsPreLinkPPOpenPoint = false;
					}


					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
	}


	if(nTmpError != NO_ERR)
	{
		//1. Path List ���� ����
		InitPathList();

		//2. Cycle Lis ���� ����
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. �ӽ� Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}


	return nTmpError;
}

int PathSearch::CheckOnlyOnePassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool IsStartStation,bool IsEndStation,bool bStartStationPassPermit,bool bIsTargetStationoPassPermitOpen)	///< PathSearch ��� Return
{
	int nError = NO_ERR;

	if(bStartStationPassPermit ==false)  //1. ����㰡 ��û���� �ʴ� ����  (Safety Level ������ ��������)
	{
		if((IsStartStation ==  true) && (IsEndStation ==false))   //1) ������ -> �������� �̵�
		{
		  if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

			}
		}
		else if((IsStartStation ==  false) && (IsEndStation ==true)) //2) ���� -> ���������� �̵�
		{
		   if(bIsTargetStationoPassPermitOpen == true)  //�ݳ��� �ʿ��� ���.
		   {
               if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

				}
		   }
		   else        //�ݳ��� �ʿ����� �ʴ� ���
		   {
		        if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

				}
           }

		}
		else  //3) ������ ->������� �̵��ϴ� ��� �� ����->����(���̽� ����),
		{
			if((IsStartStation ==  true) && (IsEndStation ==true) && (bIsTargetStationoPassPermitOpen == true))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
		}
	}
	else //2. ����㰡�� �޾ƾߵǴ� ���̽�
	{
		if((IsStartStation ==  true) && (IsEndStation ==false))    //1) ������ -> �������� �̵�
		{
		  if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
			}
			else
			{
				nError = ERR_STD_ST_PASSPERMIT_FAIL;

			}
		}
		//2) ���� -> ������, ������ -> ������� �̵�
		else if(((IsStartStation ==  false) && (IsEndStation ==true))
				||((IsStartStation ==  true) && (IsEndStation ==true)))
		{
			 if(bIsTargetStationoPassPermitOpen == true)    // Ÿ���� �ݳ��ΰ��
			 {
				if((StartType == STOP_NTYPE) && (EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = true;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else
				{
					nError = ERR_STD_ST_PASSPERMIT_FAIL;
				}
			 }
			 else
			 {
			 	if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else
				{
					nError = ERR_STD_ST_PASSPERMIT_FAIL;
				}
			 }

		}
	}

	 return nError;
}

int PathSearch::CheckPassPermit(CYCLENODEINFO CycleNodeInfo, PASSPERMIT_NODE_TYPE StartType, PASSPERMIT_NODE_TYPE EndType, EXECUTE_CYCLEDRIVING_INFO *pPassPermitInfo,bool bIsTargetStationoPassPermitOpen)	///< PathSearch ��� Return
{
			int nError =NO_ERR;
			if(m_pDrivingControl == NULL) m_pDrivingControl = OHTMainForm->GetDrivingControl();
			int nMTLTakeInPIOStep = m_pDrivingControl->m_nMTLTakeInPIOStep; //MTL Auto TakeIn ���� ����� ����ó����
			/*
			  ����ó�� ���� �߰�----puting
			  1. Reset ���Ŀ� Reset Node�� ���� ���
			  2. Normal Node ���Ŀ� Reset Node�� ���� ���
			  3. Stop Node ���Ŀ� Normal Node�� ���� ���
			*/
				/*
				Normal -> Stop							Normal -> Normal
				Stop -> Stop		Stop -> Reset		Stop -> Noraml			Stop -> Comon
				Reset -> Stop							Reset -> Noraml
				Common -> Stop		Common -> Reset		Common -> Normal		Common -> Comon
				*/
			///////----------------------------puting ���Ͽ��⼭ ���� ����~~~ �Ϸ�

			//bIsTargetStationoPassPermitOpen //Ÿ�� �ݳ� ���� �������϶� ������ ��ġ�� ������ ��츸 true�� ó���Ұ�.  IsCurLinkPPOpenPoint = false ->   IsCurLinkPPOpenPoint = true
			if(CycleNodeInfo ==  CYCLENODETYPE_START2STOP)
			{
				//���۰� �߰����� ���,
				   if((StartType == NORMAL_NTYPE) &&(EndType==STOP_NTYPE))
					{

						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}else if((StartType == RESET_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}else if((StartType == COMMON_NTYPE) &&((EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE)))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}else if((StartType == STOP_NTYPE) &&((EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE)))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else
						nError = ERR_STD_ST_PASSPERMIT_FAIL;
			}
				//�߰����� �߰��� ���,
			else if(CycleNodeInfo ==  CYCLENODETYPE_STOP2STOP)
			{
				if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==STOP_NTYPE)||(EndType==COMMON_NTYPE))
				{
					pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
					pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//					pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//					pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_BEFORE_NEXTLINK_PASSPERMIT_RESPONSE;

					pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
					pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//					pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = true;    	//���� Link�� ����㰡 ���� ��û ����
					pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
				}
				else
					nError = ERR_STD_MD_PASSPERMIT_FAIL;
			}
			else if(CycleNodeInfo ==  CYCLENODETYPE_STOP2TARGET)
			{
					//�߰����� �������� ���,
					if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
						//pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;        puting 20160113

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
						//pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;        puting 20160113
//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if(((StartType == STOP_NTYPE) ||(StartType == COMMON_NTYPE)) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_AFTER_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else
						nError = ERR_STD_ED_PASSPERMIT_FAIL;
			}
			else if(CycleNodeInfo ==  CYCLENODETYPE_START2TARGET)
			{
				//���ۿ��� �������� ���,

					if((StartType == NORMAL_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_MEANINGLESS;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}
					else if((StartType == RESET_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;//true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}
					else if((StartType == COMMON_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

					}else if((StartType == STOP_NTYPE) &&(EndType==STOP_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
//						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
					  	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == STOP_NTYPE) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == NORMAL_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_MEANINGLESS;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
  //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == RESET_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;//true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
					 	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==NORMAL_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
					  	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==RESET_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
                    	pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}
					else if((StartType == COMMON_NTYPE) &&(EndType==COMMON_NTYPE))
					{
						pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_RESPONSE;
						pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;

//						pPassPermitInfo->D_Step = CYCLESTEP_DRIVING_BEFORE_PRELINK_PASSPERMIT_OPEN_RESPONSE;
//						pPassPermitInfo->D_NextStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

						pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = true;       //���� Link�� ����㰡 ��û����
						pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = true;    //���� Link�� ����㰡 ���� ��û ����
 //						pPassPermitInfo->PassPermitInfo.IsNextLinkPPPoint = false;    	//���� Link�� ����㰡 ���� ��û ����
						pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)
					}

					else
						nError = ERR_STD_ETC_PASSPERMIT_FAIL;
			}
			else
						nError = ERR_STD_ETC_PASSPERMIT_FAIL;

			if(MTL_TakeIn_CTRL_STEP_INIT <= nMTLTakeInPIOStep && nMTLTakeInPIOStep < MTL_TakeIn_CTRL_STEP_COMPLETE)
			{
				pPassPermitInfo->D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
				pPassPermitInfo->D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;

				pPassPermitInfo->PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
				pPassPermitInfo->PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
				pPassPermitInfo->PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

				ADD_LOG("MTL Auto TakeIn �߿��� MTL �շ��� ����㰡 �̻��, nMTLTakeInPIOStep = %d", nMTLTakeInPIOStep);
			}
			return nError;
}


/**
@brief   Driving Tag Map Back Search �Լ�
@author  puting
@date    2016-10-22
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/
int PathSearch::StandardSearchPassBack(MAP *pMap, PARSING_GO_INFO* CmdGoData, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	UINT	CmdID				= CmdGoData->CmdID;  //���� ID
	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	//���Ž���� �ؾߵ� ���۰� �� ��ġ
	UINT	uFromNode			= AdditionalInfo.ProcessingGoInfo.StartNodeID;			// ���� ���
	UINT	uToNode				= AdditionalInfo.ProcessingGoInfo.EndNodeID;		    //  Next Node

	LList	*pResultAddList		= DRIVING_INFO_List;	// ������ ���� ����� �߰��ϱ� ���� Cycle List

	bool	bArrived			= false;	// Map Seach�� ������ ���

	UINT	uPathSearchIndex	= 0;		// PathSearch List�� Index
	UINT	uTmpListCount		= 0;		// PathSearch List ���� ���� ��

	UINT 	uSequenceIndex      = 0;      // ������ Number
	UINT 	uCompSequenceIndex  = 0;      // �ӽ� ������ Number
	UINT 	uFirstSequenceIndex  = 0;      // �ӽ� ������ Number

  //	UINT	uCheckExtraNum		 = 0;

	int		nNodeIndex			= 0;		// Node�� Index
	int     nNextNodeIndex      = 0;        //NextNode�� Index
	int		nPathIndex			= 0;		// Path�� Index
	int		nSubPathIndex	 	= 0;		// Path�� Index

	PATH_DIVERGENCE_INFO     nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����


	UINT	uTmpToNode			= 0;		// Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		// Path ������ ���� Ȯ�� ��
//	UINT	uTmpNode			= 0;		// �ӽ� ���� Ȯ�� ��

	double	dTmpTotalDistance	= 0.0;	// ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	// Path�� Link ����ġ(�Ÿ�/�ӵ�)
	double  dStopNodeMarGin     = 0.0;
	double  dTempMaxSpeed       = 0.0;  //����㰡�� ���� Max �ӵ�

	//�⺻ �б� ���� // �߿��� �κ�. 0 : ���� 1: ������
	int	bDefaultPathDirection = (int)CmdGoData->PathData_Def_Direction;
	bool	bCheckDefaultPathDirection = false;

	int		dtmpPathSearchListCnt = 0; //Pass Permit Limit üũ��

	NODE_INFO	*pNODE_INFO		= NULL;	// Node ����
	NODE_INFO	*pNextNODE_INFO	= NULL;	// NextNode ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����
	PATH_INFO	*pSubPATH_INFO	= NULL;	// Path ������ �̿��� ����㰡 Sub������ ȹ��

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü


	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü
	EXECUTE_DRIVING_INFO	*pTempEXECUTE_DRIVING_INFO	= NULL;	// ������ ���õ� ����
	PKT_CYCLECMDDATA  		*pCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü
//	PKT_CYCLECMDDATA		*pNextCYCLE_DRIVING_INFO= NULL; // CycleLIst�� �����ϴ� ����ü
	PKT_CYCLECMDDATA  		*pPreCYCLE_DRIVING_INFO 	= NULL;	// CycleLIst�� �����ϴ� ����ü

	LList PathSearchList;									// PathSearch�� ���� List


	EXECUTE_DRIVING_ADD_INFO TempAdditionalInfo;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
//	NODE_TYPE DrivingEndNodeType = NORMAL_POINT;
	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";


	//------------- 1. ��ó�� �κ�------------//
	// 1. �ʱ� ���� : ----------------------------------------------------------
	//   0) ���� Node�� Path Ȯ��
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
  //	||	(uFromNode == uToNode)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;

	}

	//  1) Station�� Path Ȯ��   //�뵵 ?  puting


	// 2) ���� Node�� Path ������ ���� ���

	///������ Station�ΰ�쿡 ���� ��ó�� �κ�
	if(AdditionalInfo.ProcessingGoInfo.IsStationStart ==true)
	{
		//�������� ��� �θ��尡 ���´�.
		ADD_LOG("[PATHSEARCH_Start_Station_5] %d",CmdGoData->PathData_StartNodeID);
		nNodeIndex = pTmpMap->FindNodeIndex(CmdGoData->PathData_StartNodeID);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);  ///< ��� ��� ����

		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
				{
					nPathIndex = pNODE_INFO->NextPathIndex[n];
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����
					if(pPATH_INFO->EndNID == AdditionalInfo.ProcessingGoInfo.StartNodeID)
					{
						AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode =  pPATH_INFO->Distance - AdditionalInfo.ProcessingGoInfo.StationData_StartRunDist;
                    }
				}
			}

			if(AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode <0)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[PATHSEARCH_Start_Station_Fail] %d",AdditionalInfo.ProcessingGoInfo.StationData_StartRunDistOfNextNode);
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
            }

		}
		else
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_52] %d:%d"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			return nTmpError;
		}
	}



	ADD_LOG("[PATHSEARCH_Step 1 from%d To %d]",uFromNode,uToNode);



	//------------- 2. �ֿ� ó���κ�------------//

	// 3) ���� Node��ġ List�� ����
	uTmpFromNode = uFromNode;
	uPathSearchIndex = 0;
	uSequenceIndex = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ ���۳�尡 �߰ߵǸ� ����

	while(bArrived==false)	// ���� Node ���� ��� bArrived �� true�� �ٲ���
	{
		nTempDivInfo        = PATH_DIVERGENCE_NONE;         //�б�����
		////< (1) first Node Check!!
		if(uPathSearchIndex !=0)
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex-1);
			////< (1)-1 ���� Node�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			}
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


			///< (1)-2 Node Info Check!!
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{

				///(3)-2 �Ÿ� ���� Ȯ�� �ʿ�
//				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;

			}
			else
			{
				nTmpError = ERR_STD_INNER_PATH_FAIL; // PathSearch ����
				//break;
			}

		}
		else
		{
			//uTmpFromNode = uTmpFromNode;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����
		}

		////< (2) List�� Max Count Check!!     puting -----------------------------------------------------------���Ļ����� �������� �Ǵ�.
		dtmpPathSearchListCnt = PathSearchList.getCount();
		if((dtmpPathSearchListCnt > m_pParamSet->nMapSearchMax) && (AdditionalInfo.FinalNode == 0))
			{
				nTmpError = ERR_PATHSEARCH_FAIL;
				ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dtmpPathSearchListCnt , m_pParamSet->nMapSearchMax);
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
				break;
			}

		////< (4) ���� ��尡 �ƹ��� PathIndex�� ���� ��� ���� ó�� �߰�
		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_54] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1]);
			ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		   //	return nTmpError;
		}


		///<(6) Next Node(Path) ������ � ���� Ȯ���ϴ� �κ�  puting 20151107
 		////< (6)-1 �б��� ���,
		if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE && pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			////< (6-1) �⺻�� �������� Ȥ���ϴ� �κ�  puting 20151107
			for(int nTempIndex=0; nTempIndex<2; nTempIndex++)// ���� Path ���� ��ŭ Ȯ��
			{

				if(bDefaultPathDirection == nTempIndex)
				{
						if(nTempIndex == 0)
							nTempDivInfo = PATH_DIVERGENCE_LEFT;    //���� �б�
						else
							nTempDivInfo = PATH_DIVERGENCE_RIGHT;    //���� �б�

					nPathIndex = pNODE_INFO->NextPathIndex[nTempIndex];
				}

			}
		}

		////< (6-3) �бⰡ �ƴѰ��,
		else if(pNODE_INFO->NextPathIndex[0]!=INDEX_NONE || pNODE_INFO->NextPathIndex[1]!=INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
//			nTempDivInfo =  PATH_DIVERGENCE_NONE;
			for(int n=0; n<2; n++)// ���� Path ���� ��ŭ Ȯ��
			{
				if(pNODE_INFO->NextPathIndex[n]!=INDEX_NONE)
					nPathIndex = pNODE_INFO->NextPathIndex[n];
			}

		}

		///< (7) ����� Next Path(Node) ���� Ȯ�� �� ����
		////<(7)-1 ����� Path�� ���� �ϴ� ���
		if(nPathIndex!=INDEX_NONE)
		{
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// ����� Path ����

			dTmpPathDistance = (double)pPATH_INFO->Distance;
			uTmpToNode = pPATH_INFO->EndNID; //����� ���� ���

			ADD_LOG("[PathSearch_Check][uTmpFromNode:%d/uTmpToNode:%d/uSequenceIndex%d]", uTmpFromNode, uTmpToNode, uSequenceIndex);

		   //����㰡 ���� Ȯ���ϴ� �κ�  //����
			/// (5) ��������� ��� sequenceIndex �����ϴ� �κ�  puting------------------------------------------------------------------------------------------ MapData ���� �ʿ�.7
			if(uPathSearchIndex==0)
			 {
                	uSequenceIndex = GetIncreaseSeqenceNumber();
				//ADD_LOG("[BACKSequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);

			 }
			 else
			 {
			 	uSequenceIndex = GetSeqenceNumber();
			   //	ADD_LOG("[SequenceIndexUpCheck][SequenceIndex:%d/NodeType:%d]", uSequenceIndex , pNODE_INFO->TagType);
			 }

			 if(uPathSearchIndex==0)
				uFirstSequenceIndex  = uSequenceIndex;

			////<(7)-1 Next Path ����
			uPathSearchIndex++;


			if(uPathSearchIndex >MAX_PATHINDEX)
			{
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			}

			//����ġ�� �ʿ����.

//			IterPos = mapPathSearchNode.find(uTmpToNode);
//			// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������ ����ó��.
//			if(IterPos == mapPathSearchNode.end())
//			{
				// PathSearch Node ���� ���� : ���� ���
				pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
				pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
				pPATH_SEARCH_NODE_INFO->IndexNum = uPathSearchIndex;

			   //	pPATH_SEARCH_NODE_INFO->PassPermit_NType =
				pPATH_SEARCH_NODE_INFO->SQID =  uSequenceIndex;
				pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
				pPATH_SEARCH_NODE_INFO->SubNID = 0;
				pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
				pPATH_SEARCH_NODE_INFO->DivInfo = nTempDivInfo;
				pPATH_SEARCH_NODE_INFO->PreSteering = pPATH_INFO->Steering;     //������ ���õ� ���� �߰�. puting 20160119

				///puting �߰��ؾߵǴ� ����/ �ʱ�Ÿ��� Target�� ���� �Ÿ��� ����.
				//  Step 1.Target�� station�� ��� �Ÿ��� �߰��Ұ�.
				pPATH_SEARCH_NODE_INFO->dTotalDistance = 0;  //��ó������ �����Ұ�. �ڷ� �ִ°� ������.
				pPATH_SEARCH_NODE_INFO->dPathDistance  = - (double)dTmpPathDistance;
//				ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

				//�߰��ϴ� ��ƾ
				PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
//				mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;

				///< (3)-1 ������ ��� ���� Ȯ��
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
//					ADD_LOG("[PATHSEARCH_Stepll %06f]",pPATH_SEARCH_NODE_INFO->dTotalDistance);

					bArrived = true;
					break;
				}
//			}
//			else
//			{
//				nTmpError = ERR_PATHSEARCH_FAIL;
//			}

		}	// if(nPathIndex!=-1)

		if(nTmpError!=NO_ERR) break;


		nPathIndex = 0;
		nSubPathIndex =0;

	 }  //while()


    ADD_LOG("[PATHSEARCH_Step 3]");
	///----------------3. ��ó�� �κ� ---------------------//
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	try{
	bArrived		= false;
	uPathSearchIndex= 0;

	uTmpListCount	= PathSearchList.getCount();
	uTmpFromNode	= uFromNode;
	uCompSequenceIndex = 0;
	dTmpTotalDistance = 0.0;

	memmove(&TempAdditionalInfo, &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
	}catch(...)
	  ADD_LOG("[ERR_PATHSEARCH_Exception111111111]");


	///<(2) ���ִ� Path�� �ϳ��� ���� ��ó�� �۾�
	if( nTmpError == NO_ERR)
	{

		for(int PathIndex =0; PathIndex < uTmpListCount; PathIndex++)
		{
			uPathSearchIndex++; //���ִ� Path�� Pop Conut��!!

			if(nTmpError!=NO_ERR) break;

			try
			{
//			///< (2)-1 ��ó�� �� ���� Ȯ�� ����
			pPATH_SEARCH_NODE_INFO =(PATH_SEARCH_NODE_INFO*)PathSearchList.popBack();


			dTmpTotalDistance += pPATH_SEARCH_NODE_INFO->dPathDistance;


			if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
				(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false))
			{
				// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
				nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
				ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
				ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
                delete pPATH_SEARCH_NODE_INFO;
				break;
			}
			}catch(...)
			   ADD_LOG("[ERR_PATHSEARCH_Exception22222222222]");



			//(4)  EXECUTE_CYCLEDRIVING_INFO��  List ���� ---------------
			//    (������ ID�� ������ �Ͼ�� �������� Ȯ�� �� ��. )

			if((uCompSequenceIndex != pPATH_SEARCH_NODE_INFO->SQID) ||
				(uTmpListCount == uPathSearchIndex))  //Target Node �� ���, ������ Ndoe
			{
				 //������ ID ���� ���
				 uCompSequenceIndex = pPATH_SEARCH_NODE_INFO->SQID;

				 dStopNodeMarGin = 0;  // ������ ����㰡 ��û�� ����.

				if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount != uPathSearchIndex))
				{
					try{
					TempAdditionalInfo.DistToStation = 0.0;
					TempAdditionalInfo.StationType = MARK_TYPE_TAG_STATION_BACK;
					TempAdditionalInfo.FinalPortType = MARK_TYPE_TAG_STATION_BACK;


					uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����


					pCYCLE_DRIVING_INFO    	= 	new PKT_CYCLECMDDATA;

					memset(pCYCLE_DRIVING_INFO, 0x00, sizeof(PKT_CYCLECMDDATA));

					// 1-1�⺻���� ���
					pCYCLE_DRIVING_INFO->CycleType = CYCLETYPE_DRIVING;
					pCYCLE_DRIVING_INFO->CmdID = CmdID;
					pCYCLE_DRIVING_INFO->IsExcuteInfo = false;
					pCYCLE_DRIVING_INFO->IsNoExcuteCycle = true;
					pCYCLE_DRIVING_INFO->IsListF2B  = false;
					pCYCLE_DRIVING_INFO->Isreturn = false;

					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint					=  pPATH_SEARCH_NODE_INFO->NextNID;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType           	 	=  pNextNODE_INFO->TagType;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartDistance           	 	=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StopDistance            		=  (double)pPATH_SEARCH_NODE_INFO->dTotalDistance;      //Stop ����Ʈ ��ġ
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo 				= CYCLENODETYPE_START2STOP; //Default�� ����
					pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CurLinkPassPermitData.SQNum 	= (unsigned short) uCompSequenceIndex;


					ADD_LOG("[PassPermit_Start_NotMatch_ListCount] StartND:%d, NType:%d", pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint, pCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType);

				

					pResultAddList->insertBack((void*)pCYCLE_DRIVING_INFO);
					}catch(...)
						ADD_LOG("[ERR_PATHSEARCH_Exception33]");

				}

				// (4)-1���� ��ġ�� ���, �н��� �ϳ��ۿ� ���� ���.
				else if((uPathSearchIndex==FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
				   // �������� �н��� ������ �ϳ� �̻���.


				}
				//(4)-2 ������ ��ġ �� ���.(�н��� �ϳ� �̻��� ���)
				else if((uPathSearchIndex!=FIRST_INDEX)&&(uTmpListCount == uPathSearchIndex))
				{
					TempAdditionalInfo.DistToStation = AdditionalInfo.DistToStation;
					TempAdditionalInfo.StationType = AdditionalInfo.StationType;
					TempAdditionalInfo.FinalPortType = AdditionalInfo.FinalPortType;
					//���� ȹ��
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();

                   	uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

					uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
					nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
					pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����



					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint =  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint =  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType =  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance = (double)pPATH_SEARCH_NODE_INFO->dTotalDistance + dTmpTotalDistance;


					if(uCompSequenceIndex == uFirstSequenceIndex)
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_START2TARGET;
					}
					else
					{
						pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo = CYCLENODETYPE_STOP2TARGET;
					}

					ADD_LOG("[PassPermit] CycleInfo %d StartID %d, NType %d EndND:%d, NType:%d",
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartPoint,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint,
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType);


					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPStep = CYCLESTEP_DRIVING_MEANINGLESS;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.D_PPOStep = NEXTCYCLESTEP_DRIVING_MEANINGLESS;//NEXTCYCLESTEP_DRIVING_BEFORE_CURLINK_PASSPERMIT_OPEN_RESPONSE;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPPoint = false;       //���� Link�� ����㰡 ��û����
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsCurLinkPPOpenPoint = false;    //���� Link�� ����㰡 ���� ��û ����
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.PassPermitInfo.IsTargetStationPPOpenPoint = false;  	//������ ��ġ�� Station�ΰ�� ����㰡 �ݳ� ������ ���� �÷��� (����Ʈ�� false)

//					nTmpError = CheckPassPermit(pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.CycleNodeInfo,
//											pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.StartNodeType,
//											pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType,
//											 &pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info);
				}
				//(4)-3�߰� ���� ����� ���
				else
				{

				    //������ �ϳ��� ����Ŭ�� ������.

				}


                dTempMaxSpeed = 0.0;  //Max Speed �ʱ�ȭ
			}else
			{

													// 2-3 �۾������� ���� �б� ����
					pPreCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->referBack();
					nNodeIndex = pTmpMap->FindNodeIndex(pPATH_SEARCH_NODE_INFO->NID);	// Node Index ȹ��
					pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

				try{
					// 2-3 �۾������� ���� �б� ����
					TempAdditionalInfo.DistToStation = AdditionalInfo.DistToStation;
					TempAdditionalInfo.StationType = AdditionalInfo.StationType;
					TempAdditionalInfo.FinalPortType = AdditionalInfo.FinalPortType;

					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.FinalPoint 		=  pPATH_SEARCH_NODE_INFO->NID;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndNodeType 	=  pNODE_INFO->TagType;
					pPreCYCLE_DRIVING_INFO->EXE_CycleDriving_Info.EndDistance 	= (double)pPATH_SEARCH_NODE_INFO->dTotalDistance + dTmpTotalDistance;
				}catch(...)
						 ADD_LOG("[ERR_PATHSEARCH_Exception77777777]");

            }

			//ó������ �ϳ��� Path ������ ��.
			nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
			pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		  //	ADD_LOG("[ExcuteDrivingInfo: %d", nPathIndex);
			// (3) EXECUTE_DRIVING_INFO�� PathSearcing List ���� ---------------
			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			uTmpToNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpToNode);	// Node Index ȹ��
			pNextNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);		// Node ����

			//Excute Info�� ����� Data
			pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
			memset(pEXECUTE_DRIVING_INFO, 0x00,sizeof(EXECUTE_DRIVING_INFO));

			pEXECUTE_DRIVING_INFO->PathIndexNum				= pPATH_SEARCH_NODE_INFO->IndexNum;
			pEXECUTE_DRIVING_INFO->CmdID					= CmdID;
			pEXECUTE_DRIVING_INFO->SQNum					= pPATH_SEARCH_NODE_INFO->SQID;
			pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance    = pPATH_SEARCH_NODE_INFO->dTotalDistance;
            pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= pPATH_SEARCH_NODE_INFO->NextNID;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartType		= pNODE_INFO->TagType;
//			pEXECUTE_DRIVING_INFO->BasicInfo.DeviceType     = pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NodeType      	= pNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.NextNodeType   = pNextNODE_INFO->InOutInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo        = pPATH_SEARCH_NODE_INFO->DivInfo;
			pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
			pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)0.4;			// mm/s -> mm/msec AOHC-367
			pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
			pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
			pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;

			// for change node (jhoun.lim)
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
			pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;

#else
			pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
			pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= STEERING_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.PreDivInfo     = PATH_DIVERGENCE_NONE;
			pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
#endif
			//Map Maker ���� �ӵ�

			memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &TempAdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));

			AddPath(pEXECUTE_DRIVING_INFO);



			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NID;
			delete pPATH_SEARCH_NODE_INFO;

		}
	}
     ADD_LOG("[PATHSEARCH_Step 4]");
   //	if(uTmpFromNode !=  uTmpFromNode)  nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
	///< Error �߻��� ��� List ���� �ϴ� �κ�
	if(nTmpError != NO_ERR)
	{
		//1. Path List ���� ����
		InitPathList();

		//2. Cycle Lis ���� ����
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pCYCLE_DRIVING_INFO = (PKT_CYCLECMDDATA*)pResultAddList->popFront();
			delete pCYCLE_DRIVING_INFO;
		}
	}

	// 4. �ӽ� Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}


	return nTmpError;
}


/**
 @brief   ��� ����
 @author  soo583
 @date    2013.07.11
 @param   szFilePath(���� ���)
 @return  ���� �� true / ���� �� false
 */
bool PathSearch::loadParameterRecovery(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
{
    bool bSuccess = true;
    int nTmpValue = 0;
    int nResult = 0;
    double dTmpValue = 0.0;
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

       	pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		if(StrToIntA(strTempValue) <= StrToIntA(pParamSet->Version))
			bSuccess = false;
		else
		{
			memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
			memset(strTempValue, 0, sizeof(strTempValue));
		}

		// 1.
		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH", "MapSearchMax", MAX_LIST_COUNT_FOR_PRESTEER, 5000, 15000,&nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nMapSearchMax = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitPeriod", 90, 0, 200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitPeriod = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermit_Count ", 7, 1, 10, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitCount = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitOpen_Margin", MARGIN_OF_DIST_TO_RESETNODE, 100.0, 500.0,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitOpenMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitRollbackTime", 200, 100, 300, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitRollbackTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PASSPERMIT","PassPermitRollbackFinshTime", 400, 350, 800, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nPassPermitRollbackFinshTime = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH", "IsCurveSpeedUp", 1,0,3, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsCurveSpeedUp = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH","IsShoutCurveSpeedUpValue", 1000, 750, 1000, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsShoutCurveSpeedUpValue = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadIntegerMinMaxVerification("PATHSEARCH","IsCurveSpeedUpValue", 1000, 800, 1200, &nTmpValue);
			if(bCheckParam)
			{
				pParamSet->nIsCurveSpeedUpValue = nTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitMax_Margin", 10000.0, 5000.0, 20000.0,&dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitMaxMargin = dTmpValue;
			}
			else
			{
				bSuccess = false;
			}

		}

		if(bSuccess)
		{
			bCheckParam = pIniFile->ReadDoubleMinMaxVerification("PASSPERMIT", "PassPermitRollbacking_Margin", 200.0, 0.0, 300.0,  &dTmpValue);
			if(bCheckParam)
			{
				pParamSet->dPassPermitRollbackMargin = dTmpValue;
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
		bSuccess = false;
	}

	//AOHC-358 �Ķ���� ���� ���� ��, ���� �߻�
	if(bSuccess ==false)
	{
	  //	SOM_LOG ("loadparameterRecovery");
		bCheckPathParam = true;
		bCheckParam = loadParameter("DataFiles\\PathSearch.ini", m_pParamSet);
		if(bCheckParam == false)

			OHTMainForm->m_CheckParamResult = ERR_PARADATA_LOAD_FAIL;
		else
			OHTMainForm->m_CheckParamResult = NO_ERR;

	}



	return bSuccess;
}
/**
 @brief   ��� ����
 @author  soo583
 @date    2013.07.11
 @param   szFilePath(���� ���)
 @return  ���� �� true / ���� �� false
 */
bool PathSearch::loadParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
{
    bool bSuccess = false;
    int nTmpValue = 0;
    int nResult = 0;
    double dTmpValue = 0.0;
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
	}
	//AOHC-358 �Ķ���� ���� ���� ��, ���� �߻�
	 if(pIniFile == NULL|| bCheckPathParam == true)
	{
		strcpy(pParamSet->Version, "00");

		pParamSet->nMapSearchMax  = MAX_LIST_COUNT_FOR_PRESTEER;
		pParamSet->nIsCurveSpeedUp = 1;
		pParamSet->nIsShoutCurveSpeedUpValue = 1000;
		pParamSet->nIsCurveSpeedUpValue = 1000;

		pParamSet->nPassPermitPeriod = 90;
		pParamSet->nPassPermitCount  = 8;
		pParamSet->dPassPermitOpenMargin = MARGIN_OF_DIST_TO_RESETNODE;
		pParamSet->dPassPermitRollbackMargin = 250.0;
		pParamSet->nPassPermitRollbackFinshTime  = 350.0;
		pParamSet->dPassPermitMaxMargin  = 10000.0;
		pParamSet->dPassPermitRollbackMargin = 200.0;

		bSuccess = false;
		bCheckPathParam = false ;
		ADD_MD_LOG("Pathsearch Param Loading Fail");
		 return bSuccess;
	}
	else if(pIniFile != NULL)
	{

		memset(&pParamSet->Version,0x00, sizeof(pParamSet->Version));
		pIniFile->ReadString("Version","IniVersion", "00", strTempValue);
		memmove(&pParamSet->Version, strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		// 1. HOIST PARAM
		pIniFile->ReadInteger("PATHSEARCH", "MapSearchMax", MAX_LIST_COUNT_FOR_PRESTEER, &nTmpValue);
		pParamSet->nMapSearchMax = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitPeriod", 90, 0, 200, &nTmpValue);
		pParamSet->nPassPermitPeriod = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermit_Count ", 8, 1, 10, &nTmpValue);
		pParamSet->nPassPermitCount = nTmpValue;

		pIniFile->ReadDouble("PASSPERMIT", "PassPermitOpen_Margin", MARGIN_OF_DIST_TO_RESETNODE, &dTmpValue);
		pParamSet->dPassPermitOpenMargin = dTmpValue;


		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitRollbackTime", 200, 100, 300, &nTmpValue);
		pParamSet->nPassPermitRollbackTime = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PASSPERMIT","PassPermitRollbackFinshTime", 350, 340, 800, &nTmpValue);
		pParamSet->nPassPermitRollbackFinshTime = nTmpValue;

		pIniFile->ReadInteger("PATHSEARCH", "IsCurveSpeedUp", 1, &nTmpValue);
		pParamSet->nIsCurveSpeedUp = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PATHSEARCH","IsShoutCurveSpeedUpValue", 1000, 750, 1000, &nTmpValue);
		pParamSet->nIsShoutCurveSpeedUpValue = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("PATHSEARCH","IsCurveSpeedUpValue", 1000, 800, 1200, &nTmpValue);
		pParamSet->nIsCurveSpeedUpValue = nTmpValue;

		pIniFile->ReadDouble("PASSPERMIT", "PassPermitMax_Margin", 10000.0, &dTmpValue);
		pParamSet->dPassPermitMaxMargin = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("PASSPERMIT", "PassPermitRollbacking_Margin", 200.0, 0.0, 300.0,  &dTmpValue);
		pParamSet->dPassPermitRollbackMargin = dTmpValue;

		delete pIniFile;
		bSuccess = true;

	}


	return bSuccess;
}

bool PathSearch::saveParameter(char* szFilePath, PATHSEARCH_PARAM_SET* pParamSet)
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
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "MapSearchMax", pParamSet->nMapSearchMax);

		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitPeriod", pParamSet->nPassPermitPeriod);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermit_Count", pParamSet->nPassPermitCount);
		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitOpen_Margin", pParamSet->dPassPermitOpenMargin);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitRollbackTime", pParamSet->nPassPermitRollbackTime);
		m_lWriteIniFile->WriteInteger("PASSPERMIT", "PassPermitRollbackFinshTime", pParamSet->nPassPermitRollbackFinshTime);

		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsCurveSpeedUp", pParamSet->nIsCurveSpeedUp);
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsShoutCurveSpeedUpValue", pParamSet->nIsShoutCurveSpeedUpValue);
		m_lWriteIniFile->WriteInteger("PATHSEARCH", "IsCurveSpeedUpValue", pParamSet->nIsCurveSpeedUpValue);

		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitMax_Margin", pParamSet->dPassPermitMaxMargin);

		m_lWriteIniFile->WriteDouble("PASSPERMIT", "PassPermitRollbacking_Margin", pParamSet->dPassPermitRollbackMargin);


		delete m_lWriteIniFile;
		bResult = true;


	}

    return bResult;
}
/**
@brief   �Ķ���͸� ������Ʈ �ϴ� �Լ�
@author  ���¿�
@date    2013.7.19
@return �����ϸ� true ��ȯ, List�� ����� ���� �߰� ���ϸ� false ��ȯ
*/
bool PathSearch::updateParameter()
{
	PATHSEARCH_PARAM_SET* pTmpParamSet;

    if(m_pTmpParamSet!=NULL)
    {
        pTmpParamSet = m_pParamSet;
        m_pParamSet = m_pTmpParamSet;
        m_pTmpParamSet = NULL;
        delete pTmpParamSet;
        return true;
    }
    else
    {
        return false;
    }
}
/**
@brief    UBG Monitoring.ini �Ķ���� Load
@author  soming
@date    2018.10.15
@ ������常 ���� ������ �о  m_pUBGParamSet �� ���� �ִ´�
@return �����ϸ� true ��ȯ, List�� ����� ���� �߰� ���ϸ� false ��ȯ
*/
bool PathSearch::loadUBGCheckParam(char* szFilePath, PATHSEARCH_UBG_CHECK_PARAM_SET* pUBGParamSet)
{

	std::string tmp;
	std::ifstream tmpFile(szFilePath);

	// �ʱ�ȭ
	memset(pUBGParamSet->ArrNode, 0 ,sizeof(pUBGParamSet->ArrNode));
	pUBGParamSet->nNodeCnt = 0;

	if(tmpFile.is_open() == false)
		return false;

	int nCurNodeIdx = 0;

	// �������� �д´�.
	// ���ٿ� �Ѱ� ��尡 �ִ�
	while (std::getline(tmpFile, tmp))
	{
		char szLine[62];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());

		int nNode = atoi(szLine);

		if(nCurNodeIdx >= MAX_JIKJIN_NODE_CHECK_CNT-1)
			break;
		if((nNode <= 0) || (nNode > 99999999))
			continue;

		pUBGParamSet->ArrNode[nCurNodeIdx++] = nNode;

	}

	 pUBGParamSet->nNodeCnt = nCurNodeIdx;
    tmpFile.close();

	return true;
}

/**
@brief    UBG Monitoring.ini �Ķ���� Load
@author  soming
@date    2018.10.15
@ ������常 ���� ������ �о  m_pUBGParamSet �� ���� �ִ´�
@return �����ϸ� true ��ȯ, List�� ����� ���� �߰� ���ϸ� false ��ȯ
*/
bool PathSearch::loadNodeLanCutCheckParam(char* szFilePath, PATHSEARCH_NODE_LANCUT_PARAM_SET* pNodeLanCutSet)
{
	std::string tmp;
	std::ifstream tmpFile(szFilePath);

	// �ʱ�ȭ
	memset(pNodeLanCutSet->ArrNode, 0 ,sizeof(pNodeLanCutSet->ArrNode));
	pNodeLanCutSet->nNodeCnt = 0;

	if(tmpFile.is_open() == false)
		return false;

	int nCurNodeIdx = 0;

	// �������� �д´�.
	// ���ٿ� �Ѱ� ��尡 �ִ�
	while (std::getline(tmpFile, tmp))
	{
		char szLine[62];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());

		int nNode = atoi(szLine);

		if(nCurNodeIdx >= MAX_JIKJIN_NODE_CHECK_CNT-1)
			break;
		if((nNode <= 0) || (nNode > 99999999))
			continue;

		pNodeLanCutSet->ArrNode[nCurNodeIdx++] = nNode;

	}

	pNodeLanCutSet->nNodeCnt = nCurNodeIdx;
    tmpFile.close();

	return true;
}

/**
@brief   Driving PathSearch �Լ�, ���� PathSearch �Լ��� ���, Target Node ���� From Node�� Search �ϳ�, �ش� �Լ��� ���, From Node ���� Target Node Ž��
@author  Labeler
@date    2016-12-22
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPath(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	MAP		*pTmpMap			= pMap;				        // ����ϴ� Map
	UINT	uFromNode			= FromNode;			        // ���� ���
	UINT	uToNode				= ToNode;			        // �� ���
	LList	*pResultAddList	= DRIVING_INFO_List;	        // ������ ���� ����� �߰��ϱ� ���� List

	bool	bArrived			= false;	                // Map Seach�� ������ ���
	UINT	uPathSearchIndex	= 0;		                // PathSearch List�� Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		                // PathSearch List ���� ���� ��

	int		nNodeIndex			= 0;		                // Node�� Index
	int		nPathIndex			= 0;		                // Path�� Index
	UINT	uTmpToNode			= 0;		                // Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		                // Path ������ ���� Ȯ�� ��
	double	dTmpTotalDistance	= 0.0;	                    // ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	                    // Path�� Link ����ġ(�Ÿ�/�ӵ�)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path ����

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü

	LList PathSearchList;									// PathSearch�� ���� List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node ������ ã�� ���� Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. Path Search �� ���� Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return ERR_PATHSEARCH_FAIL;
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		ADD_FDC_LOG("ERR/%d/6/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node ���� ���� : ���� ���
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List �� ����
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map �� ����
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ Ÿ�ٳ�尡 �߰ߵǸ� ����
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node ���� ��� bArrived �� true�� �ٲ���
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}

			// Ÿ�ٳ�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode ��  uTmpToNode �� PathSearch Node������ ������
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch ����
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node ����

			for(int n=0; n<2; n++)                              // Next Path ���� ��ŭ Ȯ��
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index ȹ��
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path ����
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link ���� ȹ�� : �̵� �ð� (�Ÿ�/�ӵ�)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID ȹ��

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						// PathSearch Node ���� ���� : ���� ���
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//�߰��ϴ� ��ƾ
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// �Ÿ��� ���� �� ���
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// �߰��� ���� �Ǵ� ���
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while �� ����
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // ���� ���ų� Ŭ ��� �ձ�-->skip
						{
							// PathSearch Node ���� ���� : �߰��� ����
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// ����Ʈ���� �߰� �� ���� �� ���� ��ƾ
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// ���ο� �� �߰�
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// ���� �Ÿ� �� ���� �� ����
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List ���� ����
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// ���� �Ҹ�
										}
										uListInsertIndex = uTmpListCount;	// while �� ����
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node ���� ���� Node ã�ư��鼭 Driving List ���� ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpToNode);

        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			ADD_FDC_LOG("ERR/%d/7/5/%06d/%06d/%d/%d/%06d",
						ERR_PATHSEARCH_FAIL,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/3/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		if((pTmpMap->tPassBoxNodeList != NULL) && (pOHTMainStatusTmp->StatusCommon.MSCmdID.Info.Mode == MODE_MANUAL))
		{
			for(int j=0 ; j < pTmpMap->tPassBoxNodeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tPassBoxNodeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) && ((uTmpToNode != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxOut)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#1] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpToNode, pPATH_SEARCH_NODE_INFO->NID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}

				if((pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)) && ((StrToIntDef(StrTmp, 0) != ToNode) || (AdditionalInfo.FinalPortType != GO_FINAL_PORT_TYPE_PassBoxIn)))
				{
					ADD_LOG("[ERR_NODE_DISABLE_BY_PASSBOX_#2] %d:%d:%d:%d:%d:%d", FromNode, ToNode, StrToIntDef(StrTmp, 0), uTmpToNode, pPATH_SEARCH_NODE_INFO->NID,AdditionalInfo.FinalPortType);
					nTmpError = ERR_NODE_DISABLE;
					ADD_FDC_LOG("ERR/%d/4/5/%06d/%06d/%d/%d/%06d",
						ERR_NODE_DISABLE,
						m_pOHTMainStatus->StatusCommon.CurNodeID,
						m_pExecuteInfo->ExecutePositionInfo.NextNode,
						m_pOHTMainStatus->StatusCommon.CurNodeOffset,
						m_pOHTMainStatus->StatusCommon.ExistFoup,
						m_pOHTMainStatus->StatusCommon.StopStationID);
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake �� ��� Map Make �ӵ� �ݿ� : ��ġ �� ó�� ����� ���� �߰� ��� �ʿ�
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}

	// 4. Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}




/**
@brief   Driving PathSearch �Լ�, ���� PathSearch �Լ��� ���, From Node ���� Target Node Ž��, �б⿡���� Stop
@author  puitng
@date    2019-09-02
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPathMapMake(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List,int &nTargetNode)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	MAP		*pTmpMap			= pMap;				        // ����ϴ� Map
	UINT	uFromNode			= FromNode;			        // ���� ���
	UINT	uToNode				= ToNode;			        // �� ���
	LList	*pResultAddList	= DRIVING_INFO_List;	        // ������ ���� ����� �߰��ϱ� ���� List

	bool	bArrived			= false;	                // Map Seach�� ������ ���
	UINT	uPathSearchIndex	= 0;		                // PathSearch List�� Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		                // PathSearch List ���� ���� ��

	int		nNodeIndex			= 0;		                // Node�� Index
	int		nPathIndex			= 0;		                // Path�� Index
	UINT	uTmpToNode			= 0;		                // Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		                // Path ������ ���� Ȯ�� ��
	double	dTmpTotalDistance	= 0.0;	                    // ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	                    // Path�� Link ����ġ(�Ÿ�/�ӵ�)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path ����

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü

	LList PathSearchList;									// PathSearch�� ���� List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node ������ ã�� ���� Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	nTargetNode = ToNode;

	// 1. Path Search �� ���� Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		return ERR_PATHSEARCH_FAIL;
	}


	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node ���� ���� : ���� ���
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List �� ����
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map �� ����
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ Ÿ�ٳ�尡 �߰ߵǸ� ����
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node ���� ��� bArrived �� true�� �ٲ���
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					break;
				}
			}

			// Ÿ�ٳ�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

//			// �б��� ���� Ȯ��
//			if(pPATH_SEARCH_NODE_INFO != NULL)
//			{
//				if(uPathSearchIndex >1)
//				{
//					ADD_LOG("[PATHSEARCH_Step 1-2-3: %d %d]", uPathSearchIndex, pPATH_SEARCH_NODE_INFO->NextNID);
//				  if(GetIsNBrenchMapMake(pMap, pPATH_SEARCH_NODE_INFO->NextNID))
//				  {
//					bArrived = true;
//					uToNode = pPATH_SEARCH_NODE_INFO->NextNID;
//					nTargetNode = pPATH_SEARCH_NODE_INFO->NextNID;
//					break;
//                  }
//                }
//
//				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
//				{
//					bArrived = true;
//					break;
//				}
//			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode ��  uTmpToNode �� PathSearch Node������ ������
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch ����
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node ����

			for(int n=0; n<2; n++)                              // Next Path ���� ��ŭ Ȯ��
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index ȹ��
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path ����
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link ���� ȹ�� : �̵� �ð� (�Ÿ�/�ӵ�)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID ȹ��

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						// PathSearch Node ���� ���� : ���� ���
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//�߰��ϴ� ��ƾ
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// �Ÿ��� ���� �� ���
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// �߰��� ���� �Ǵ� ���
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while �� ����
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // ���� ���ų� Ŭ ��� �ձ�-->skip
						{
							// PathSearch Node ���� ���� : �߰��� ����
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// ����Ʈ���� �߰� �� ���� �� ���� ��ƾ
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// ���ο� �� �߰�
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// ���� �Ÿ� �� ���� �� ����
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List ���� ����
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// ���� �Ҹ�
										}
										uListInsertIndex = uTmpListCount;	// while �� ����
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node ���� ���� Node ã�ư��鼭 Driving List ���� ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
		IterPos = mapPathSearchNode.find(uTmpToNode);

		if( IterPos != mapPathSearchNode.end() )
		{
			pPATH_SEARCH_NODE_INFO = IterPos->second;
		}
		else
        {
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					break;
				}
			}
		}

		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake �� ��� Map Make �ӵ� �ݿ� : ��ġ �� ó�� ����� ���� �߰� ��� �ʿ�
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		//��������
		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		IterPos = mapPathSearchNode.find(uTmpToNode);
		if( IterPos != mapPathSearchNode.end() )
		{
			pPATH_SEARCH_NODE_INFO = IterPos->second;
		}
		else
        {
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
		}

		UINT uTmpFromNode 	= pPATH_SEARCH_NODE_INFO->NID;

		//�бⱸ�� Ȯ�� �ڿ������� Ȯ��
		if((uTmpToNode !=uToNode) && (uTmpToNode != uFromNode))
		{
			  if(GetIsNBrenchMapMake(pMap, uTmpFromNode, uTmpToNode))
			  {

				   ADD_LOG("[PATHSEARCH_Step 1-2-3: %d]", uTmpToNode);
				   nTargetNode = uTmpToNode;

			  }
		}


		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}else
	{
		if(nTargetNode !=uToNode)
		{
		 	uTmpListCount = pResultAddList->getCount();
			for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
			{
				pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popBack();
				delete pEXECUTE_DRIVING_INFO;

				if(pEXECUTE_DRIVING_INFO->BasicInfo.StartNID  ==nTargetNode)
                  	break;
			}
		}
	}

	// 4. Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}


/**
@brief   Driving PathSearch �Լ�, ���� PathSearch �Լ��� ���, Target Node ���� From Node�� Search �ϳ�, �ش� �Լ��� ���, From Node ���� Target Node Ž��
@author  puting
@date    2016-12-22
@param   pMap : ����ϴ� Map
@param   FromNode : ���� Node
@param   ToNode : Target Node
@param   AdditionalInfo : ���� ��� �߰� ����
@param   DRIVING_INFO_List : ���� ��� �߰� List
@return  Error Code
*/
int PathSearch::FrontSearchDrivingPathAutoMoveTest(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	// 0. �� �ʱ�ȭ ------------------------------------------------------------
	int nTmpError				= NO_ERR;					// ���� ���� �ʱ�ȭ

	MAP		*pTmpMap			= pMap;				        // ����ϴ� Map
	UINT	uFromNode			= FromNode;			        // ���� ���
	UINT	uToNode				= ToNode;			        // �� ���
	LList	*pResultAddList	= DRIVING_INFO_List;	        // ������ ���� ����� �߰��ϱ� ���� List

	bool	bArrived			= false;	                // Map Seach�� ������ ���
	UINT	uPathSearchIndex	= 0;		                // PathSearch List�� Index
	UINT	uListInsertIndex	= 0;		                // PathSearch List ���Խ� ���Ǵ� Index
	UINT	uTmpListCount		= 0;		                // PathSearch List ���� ���� ��

	int		nNodeIndex			= 0;		                // Node�� Index
	int		nPathIndex			= 0;		                // Path�� Index
	UINT	uTmpToNode			= 0;		                // Path ������ ���� Ȯ�� ��
	UINT	uTmpFromNode		= 0;		                // Path ������ ���� Ȯ�� ��
	double	dTmpTotalDistance	= 0.0;	                    // ���� Node������ �� ����ġ(�Ÿ�/�ӵ�)
	double	dTmpPathDistance	= 0.0;	                    // Path�� Link ����ġ(�Ÿ�/�ӵ�)

	int		dMaxPathSearchListCnt = 0;
	int		dtmpPathSearchListCnt = 0;

	NODE_INFO	*pNODE_INFO	= NULL;	                        // Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	                        // Path ����

	PATH_SEARCH_NODE_INFO	*pPATH_SEARCH_NODE_INFO		= NULL;	// Path Search�� ���� Node ����ü
	PATH_SEARCH_NODE_INFO	*pTempPATH_SEARCH_NODE_INFO	= NULL;	// Path Search�� ���� Node ����ü
	EXECUTE_DRIVING_INFO	*pEXECUTE_DRIVING_INFO		= NULL;	// ExcuteThread�� �����ϴ� ����ü

	LList PathSearchList;									// PathSearch�� ���� List
	map<int, PATH_SEARCH_NODE_INFO*> mapPathSearchNode;	    // Node ������ ã�� ���� Map
	map<int, PATH_SEARCH_NODE_INFO*>::iterator IterPos;

	NODE_TYPE aNodeType[2] = {NORMAL_POINT,};

	String StrTmp = "";

	OHTMainStatus *pOHTMainStatusTmp = OHTMainForm->GetOHTStatus();

	// 1. Path Search �� ���� Check
	ADD_LOG("[PATHSEARCH_Step 1]");
	if(	(pTmpMap->FindNodeIndex(uFromNode)	==	-1)
	||	(pTmpMap->FindNodeIndex(uToNode)	==	-1)
	)
	{
		ADD_LOG("[ERR_PATHSEARCH_FAIL_4] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(uFromNode), pTmpMap->FindNodeIndex(uToNode), uFromNode, uToNode);
		return ERR_PATHSEARCH_FAIL;
	}

	if(AdditionalInfo.StationType!=MARK_TYPE_NODE)
	{
		nTmpError = CheckPathOfStation(pTmpMap, uFromNode, uToNode, AdditionalInfo);
		if(nTmpError!=NO_ERR)
		{
			return nTmpError;
		}
	}

	ADD_LOG("[PATHSEARCH_Step 1-1]");
	nNodeIndex = pTmpMap->FindNodeIndex(uFromNode);
	pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);
	if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
	{
		nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
		ADD_LOG("[ERR_PATHSEARCH_FAIL_51] %X:%X"
			, pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[0]);
		return nTmpError;
	}

	uTmpFromNode = uFromNode;
	pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;	// PathSearch Node ���� ���� : ���� ���
	pPATH_SEARCH_NODE_INFO->NID = 0;
	pPATH_SEARCH_NODE_INFO->NextNID = uTmpFromNode;
	pPATH_SEARCH_NODE_INFO->PathIndex = INDEX_NONE;
	pPATH_SEARCH_NODE_INFO->dTotalDistance = 0.0;

	PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);	// List �� ����
	mapPathSearchNode[uTmpFromNode] = pPATH_SEARCH_NODE_INFO;		// map �� ����
	uPathSearchIndex = 0;
	dMaxPathSearchListCnt = 0;

	// 2. Index ���������� ������Ű�鼭 Node �߰� ------------------------------
	//   1) ���� ��ġ�� Index�� �������Ѱ��鼭 ���� ������ PathSarchList�� �߰���
	//     . PathSearchList�� �߰� �� ���� �Ÿ� ����ġ �ݿ��Ͽ� ����� ������ ����
	//     . �ߺ��Ǵ� Node �� ���� ��� �Ÿ� ����ġ�� ����� ���� ����, �� ���� ����
	//   2) ���� ��忡 ���� Path�� ���� ��ŭ Ÿ�ٳ�尡 �߰ߵǸ� ����
	ADD_LOG("[PATHSEARCH_Step 1-2]");


	try{
		while(bArrived==false)	                // Target Node ���� ��� bArrived �� true�� �ٲ���
		{
			pPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uPathSearchIndex);
			uPathSearchIndex++;

			dtmpPathSearchListCnt = PathSearchList.getCount();
			if(dtmpPathSearchListCnt > dMaxPathSearchListCnt)
			{
				dMaxPathSearchListCnt = dtmpPathSearchListCnt;

				if(dMaxPathSearchListCnt > m_pParamSet->nMapSearchMax)
				{
					nTmpError = ERR_PATHSEARCH_FAIL;
					ADD_LOG("[ERR_PATHSEARCH_FAIL, List Count Over!!!], [%d/%d]", dMaxPathSearchListCnt, m_pParamSet->nMapSearchMax);
					break;
				}
			}

			// Ÿ�ٳ�� ���� Ȯ��
			if(pPATH_SEARCH_NODE_INFO != NULL)
			{
				if(pPATH_SEARCH_NODE_INFO->NextNID == uToNode)
				{
					bArrived = true;
					break;
				}
			}

			if(pPATH_SEARCH_NODE_INFO != NULL)	                // mapPathSearchNode ��  uTmpToNode �� PathSearch Node������ ������
			{
				dTmpTotalDistance = pPATH_SEARCH_NODE_INFO->dTotalDistance;
			}
			else
			{
				nTmpError = -1;                                 // PathSearch ����
				break;
			}

			uTmpFromNode = pPATH_SEARCH_NODE_INFO->NextNID;
			nNodeIndex = pTmpMap->FindNodeIndex(uTmpFromNode);	// Node Index ȹ��
			if(nNodeIndex == -1) continue;
			if(m_pNodeLanCutSet != NULL)
			{
				if(m_pNodeLanCutSet->ExistsCheck(uTmpFromNode))
				{
                 	continue;
				}
			}

			pNODE_INFO = &(pTmpMap->m_NodeArray[nNodeIndex]);	// Node ����

			for(int n=0; n<2; n++)                              // Next Path ���� ��ŭ Ȯ��
			{
				nPathIndex = pNODE_INFO->NextPathIndex[n];		// Path Index ȹ��
			   //	if((nPathIndex!=0) && (nPathIndex!=-1))
			   if(nPathIndex!=-1)
				{
					pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);		// Path ����
					dTmpPathDistance = (double)pPATH_INFO->Distance / (double)pPATH_INFO->Speed;	// Link ���� ȹ�� : �̵� �ð� (�Ÿ�/�ӵ�)
					uTmpToNode = pPATH_INFO->EndNID;            // Next Node ID ȹ��

					IterPos = mapPathSearchNode.find(uTmpToNode);
					if(IterPos == mapPathSearchNode.end())	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						// PathSearch Node ���� ���� : ���� ���
						pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
						pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
						pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
						pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
						pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

						//�߰��ϴ� ��ƾ
						pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.referBack();
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance <= pPATH_SEARCH_NODE_INFO->dTotalDistance)	// �Ÿ��� ���� �� ���
						{
							PathSearchList.insertBack((void*)pPATH_SEARCH_NODE_INFO);
							mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
						}
						else	// �߰��� ���� �Ǵ� ���
						{
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									mapPathSearchNode[uTmpToNode] = pPATH_SEARCH_NODE_INFO;
									uListInsertIndex = uTmpListCount;	// while �� ����
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
					else	// mapPathSearchNode ��  uTmpFromNode �� PathSearch Node������ ������
					{
						pTempPATH_SEARCH_NODE_INFO = (*IterPos).second;
						if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > dTmpTotalDistance + dTmpPathDistance) // ���� ���ų� Ŭ ��� �ձ�-->skip
						{
							// PathSearch Node ���� ���� : �߰��� ����
							pPATH_SEARCH_NODE_INFO = new PATH_SEARCH_NODE_INFO;
							pPATH_SEARCH_NODE_INFO->NID = uTmpFromNode;
							pPATH_SEARCH_NODE_INFO->NextNID = uTmpToNode;
							pPATH_SEARCH_NODE_INFO->PathIndex = nPathIndex;
							pPATH_SEARCH_NODE_INFO->dTotalDistance = dTmpTotalDistance + dTmpPathDistance;

							// ����Ʈ���� �߰� �� ���� �� ���� ��ƾ
							uListInsertIndex = 0;
							uTmpListCount = PathSearchList.getCount();
							while(uListInsertIndex<uTmpListCount)
							{
								pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
								if(pTempPATH_SEARCH_NODE_INFO->dTotalDistance > pPATH_SEARCH_NODE_INFO->dTotalDistance)
								{
									// ���ο� �� �߰�
									PathSearchList.insert((void*)pPATH_SEARCH_NODE_INFO, uListInsertIndex);
									// ���� �Ÿ� �� ���� �� ����
									uTmpListCount = PathSearchList.getCount();
									uListInsertIndex++;
									while(uListInsertIndex<uTmpListCount)
									{
										pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.refer(uListInsertIndex);
										if(pTempPATH_SEARCH_NODE_INFO==(*IterPos).second)
										{
											PathSearchList.pop(uListInsertIndex); 	// List ���� ����
											mapPathSearchNode.erase(IterPos);
											mapPathSearchNode[uTmpToNode]=pPATH_SEARCH_NODE_INFO;
											delete pTempPATH_SEARCH_NODE_INFO;		// ���� �Ҹ�
										}
										uListInsertIndex = uTmpListCount;	// while �� ����
									}
								}
								else
								{
									uListInsertIndex++;
								}
							}
						}
					}
				}
			}
		}
	}catch(...)
	{
        return 9999;
    }

	ADD_LOG("[PATHSEARCH_Step 1-3]");
	// 3. Target Node ���� ���� Node ã�ư��鼭 Driving List ���� ---------------
	//   - pResultAddList ( EXECUTE_DRIVING_INFO_List ) �� �߰�
	bArrived		= false;
	uTmpListCount	= PathSearchList.getCount();
	uTmpToNode		= uToNode;

	while(bArrived==false && nTmpError == NO_ERR)
	{
        IterPos = mapPathSearchNode.find(uTmpToNode);

        if( IterPos != mapPathSearchNode.end() )
        {
            pPATH_SEARCH_NODE_INFO = IterPos->second;
        }
        else
        {
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_6]");
			break;
        }

		if((getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NID, aNodeType)==false) ||
			(getNodeType(pTmpMap, pPATH_SEARCH_NODE_INFO->NextNID, (aNodeType+1))==false)	)
		{
        	// �Ʒ��� ���� �� Ȯ���� 0% �� : Path Search�� �Ϸ� �Ǹ� �Ʒ� ���� �߻� Ȯ�� ����. �������� ������ �ſ�� ���� �ڵ�
			nTmpError = ERR_PATHSEARCH_FAIL; // PathSearch ����
			ADD_LOG("[ERR_PATHSEARCH_FAIL_7]");
			break;
		}

		if(pTmpMap->tNodeDisalbeList != NULL)
		{
			for(int j=0 ; j < pTmpMap->tNodeDisalbeList->Count ; j++)
			{
				StrTmp = "0x" + pTmpMap->tNodeDisalbeList->Strings[j];

				if((uTmpToNode == StrToIntDef(StrTmp, 0)) || (pPATH_SEARCH_NODE_INFO->NID == StrToIntDef(StrTmp, 0)))
				{
					ADD_LOG("[ERR_NODE_DISABLE] %d:%d:%d", StrToIntDef(StrTmp, 0), uTmpFromNode, pPATH_SEARCH_NODE_INFO->NextNID);
					nTmpError = ERR_NODE_DISABLE;
					break;
				}
			}
		}


		nPathIndex = pPATH_SEARCH_NODE_INFO->PathIndex;
		pPATH_INFO = &(pTmpMap->m_PathArray[nPathIndex]);

		pEXECUTE_DRIVING_INFO							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID		= pPATH_SEARCH_NODE_INFO->NID;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID			= uTmpToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType		= aNodeType[0];
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType		= aNodeType[1];
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed			= (double)(pPATH_INFO->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength		= (double)(pPATH_INFO->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering		= (PATH_STEERING_INFO)(pPATH_INFO->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion		= (PATH_DIRECTION_INFO)(pPATH_INFO->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion = (PATH_OBS_DIRECTION_INFO)(pPATH_INFO->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;

		// for change node (jhoun.lim)
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance = pPATH_INFO->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter = pPATH_INFO->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion = (PATH_DIRECTION_INFO)pPATH_INFO->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion = (PATH_OBS_DIRECTION_INFO)pPATH_INFO->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction		= (PATH_DIRECTION_INFO)(pPATH_INFO->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering	= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag = pPATH_INFO->ObstacleIgnore;
        // 2016-12-22, Add by Labeler, �б� ���� �߰�
        pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo = pPATH_INFO->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));
		// MapMake �� ��� Map Make �ӵ� �ݿ� : ��ġ �� ó�� ����� ���� �߰� ��� �ʿ�
		if(	(AdditionalInfo.CmdType==EXECUTE_DRIVING_CMD_MAPMAKE) &&
			(pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed>(double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0)) )
		{
			pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed = (double)(AdditionalInfo.SpeedLimitOfMapMake/1000.0);
		}

		pResultAddList->insertFront((void*)pEXECUTE_DRIVING_INFO);

		uTmpToNode = pPATH_SEARCH_NODE_INFO->NID;

		if(uTmpToNode == uFromNode)
		{
			bArrived = true;
		}
	}

	if(nTmpError != NO_ERR)
	{
		uTmpListCount = pResultAddList->getCount();
		for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)pResultAddList->popFront();
			delete pEXECUTE_DRIVING_INFO;
		}
	}

	// 4. Path Search List �Ҹ� ------------------------------------------------
	//   1) ���ڵ� delete
	//   2) List �Ҹ�
	uTmpListCount = PathSearchList.getCount();
	for(UINT uIndex=0; uIndex<uTmpListCount; uIndex++)
	{
		pTempPATH_SEARCH_NODE_INFO = (PATH_SEARCH_NODE_INFO*)PathSearchList.popFront();
		delete pTempPATH_SEARCH_NODE_INFO;
	}

    ADD_LOG("[PATHSEARCH_Step 1-4]");
	return nTmpError;
}

/**
@brief   ���� ��θ� �⺻ �б�(����) �� �������� �̵��ؾ� �ϴ� ��� Ž�� �Լ�
@author  Labeler
@date    2016-12-22
@return �����ϸ� NO_ERR ��ȯ, ���� �� Error ��ȯ
*/
int PathSearch::SearchExceptPathForV80(MAP * pMap, UINT FromNode, UINT ToNode, EXCEPT_PATH_LIST_V80 *pPath,int &TargetNode, BYTE MapMakeBrench)
{
	int nError = NO_ERR;
	int nCount = 0;

	int nTempTargetNode = 0;
    LList DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO	= NULL;	                    // List�� �����ϴ� ����ü
    EXECUTE_DRIVING_ADD_INFO DrivingAdditionalInfo;                         // ���� �߰� ����

    memset(&DrivingAdditionalInfo, 0, sizeof(DrivingAdditionalInfo));       // �߰� ���� ���� �ʱ�ȭ

    DrivingAdditionalInfo.CmdType = EXECUTE_DRIVING_CMD_GO; 				// ��� Ÿ��
    DrivingAdditionalInfo.FinalNode = ToNode; 								// ���� ���� Ÿ�� ���
    DrivingAdditionalInfo.StationType = TAG_TYPE_NONE; 						// ��� Type
    DrivingAdditionalInfo.FinalPortType = GO_FINAL_PORT_TYPE_NONE;

	loadNodeLanCutCheckParam("DataFiles\\LanCutNodes.dat", m_pNodeLanCutSet);

	if(MapMakeBrench == 0x00)
	{
		nError = FrontSearchDrivingPath(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List);
    }
	else if(MapMakeBrench == 0x01)
	{
		nError = FrontSearchDrivingPathMapMake(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List,nTempTargetNode);
		TargetNode = nTempTargetNode;
	}
	else if(MapMakeBrench == 0x02)
	{
		nError = FrontSearchDrivingPathAutoMoveTest(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List);
    	//�̵��׽�Ʈ
	}
	else if(MapMakeBrench == 0x10)      //HJH Test Code
	{
		if(!FindTheWay(pMap, FromNode, ToNode, DrivingAdditionalInfo ,&DRIVING_INFO_List))
		{
			nError = ERR_PATHSEARCH_FAIL;
		}

	}

    if(nError == NO_ERR)
    {
        nCount = DRIVING_INFO_List.getCount();

		for(int i  = 0 ; i < nCount ; i++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo == PATH_DIVERGENCE_RIGHT)
			{
				ADD_LOG("ExceptNode :%d",pEXECUTE_DRIVING_INFO->BasicInfo.StartNID);
				pPath->ExceptNode[pPath->nCnt] = pEXECUTE_DRIVING_INFO->BasicInfo.StartNID;
				pPath->nCnt++;
			}
			delete pEXECUTE_DRIVING_INFO;
		}
    }
    return nError;
}
int PathSearch::SearchExceptPathForV80_AutoSearch(MAP * pMap,  PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ PACKET_BODY, EXCEPT_PATH_LIST_V80 *pPath)
{
	int nError = NO_ERR;
	int nCount = 0;

	int nTempTargetNode = 0;
    LList DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO 	*pEXECUTE_DRIVING_INFO	= NULL;	                // List�� �����ϴ� ����ü
	EXECUTE_DRIVING_ADD_INFO 	DrivingAdditionalInfo;                      // ���� �߰� ����

	memset(&DrivingAdditionalInfo, 0, sizeof(DrivingAdditionalInfo));       // �߰� ���� ���� �ʱ�ȭ

	DrivingAdditionalInfo.CmdType 		= EXECUTE_DRIVING_CMD_GO; 				// ��� Ÿ��
	DrivingAdditionalInfo.FinalNode 	= PACKET_BODY.EndID; 					// ���� ���� Ÿ�� ���
	DrivingAdditionalInfo.StationType	= TAG_TYPE_NONE; 						// ��� Type
    DrivingAdditionalInfo.FinalPortType = GO_FINAL_PORT_TYPE_NONE;

	if(!RouteCheckNode(pMap, PACKET_BODY, DrivingAdditionalInfo, &DRIVING_INFO_List))
	{
    	nError = ERR_PATHSEARCH_FAIL;
	}


    if(nError == NO_ERR)
    {
        nCount = DRIVING_INFO_List.getCount();

		for(int i  = 0 ; i < nCount ; i++)
		{
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO*)DRIVING_INFO_List.popFront();
			if(pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo == PATH_DIVERGENCE_RIGHT)
			{
				ADD_LOG("ExceptNode :%d",pEXECUTE_DRIVING_INFO->BasicInfo.StartNID);
				pPath->ExceptNode[pPath->nCnt] = pEXECUTE_DRIVING_INFO->BasicInfo.StartNID;
				pPath->nCnt++;
			}
			delete pEXECUTE_DRIVING_INFO;
		}
    }
    return nError;
}

int PathSearch::GetIncreaseSQNumber()
{
	return GetIncreaseSeqenceNumber();
}

/**
@brief   Node�� Link Index ���� Ȯ���ϴ� �Լ�
@author  puting
@date    2017.08.16
@return  �̻������� ������. �̻� false, �̻� true
*/
bool PathSearch::CheckNodeAndLinkIndex(MAP * pMap, UINT FromNode, UINT ToNode)
{
	bool breturn = false;

	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	int		nFromNodeIndex	  = 0;		// FromNode�� Index
	int     nToNodeIndex      = 0;      // ToNode�� Index

	NODE_INFO	*pNODE_INFO			= NULL;	// Node ����
	NODE_INFO	*pEndNODE_INFO		= NULL;	// Node ����
	//1) Node Index Check �κ�
	if(	(pTmpMap->FindNodeIndex(FromNode)	==	INDEX_NONE)
	||	(pTmpMap->FindNodeIndex(ToNode)	==	INDEX_NONE)
	)
	{
		ADD_LOG("[Node:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(FromNode), pTmpMap->FindNodeIndex(ToNode), FromNode, ToNode);

		return true;

	}

	try
	{
	//2) Link Index Check �κ�
		nFromNodeIndex = pTmpMap->FindNodeIndex(FromNode);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nFromNodeIndex]);  ///< ��� ��� ����

		nToNodeIndex = pTmpMap->FindNodeIndex(ToNode);   ///< ��� ��� Index
		pEndNODE_INFO = &(pTmpMap->m_NodeArray[nToNodeIndex]);  ///< ��� ��� ����

		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			ADD_LOG("[Link From:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			,pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],FromNode,nFromNodeIndex);

			breturn = true;
		}
		else if(pEndNODE_INFO->NextPathIndex[0]==INDEX_NONE && pEndNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			ADD_LOG("[Link From:CheckNodeAndLinkIndex] %d:%d:%d:%d"
			,pEndNODE_INFO->NextPathIndex[0], pEndNODE_INFO->NextPathIndex[1],ToNode,nToNodeIndex);

			breturn = true;
		}
	}catch(...)
       	ADD_LOG("[CheckNodeAndLinkIndex] Exception");

    return breturn;
}


/**
@brief   End Staion�� ���� Ȯ���ϴ� �Լ�
@author  puting
@date    2017.08.16
@return  �̻������� ������. �̻� false, �̻� true
*/
bool PathSearch::CheckEndStationOffset(MAP * pMap, UINT ParentNode, UINT NextNode, int EndStationOffset)
{

	bool breturn = false;

	MAP		*pTmpMap			= pMap;				// ����ϴ� Map

	int		nFromNodeIndex	  = 0;		// FromNode�� Index
	int     nToNodeIndex      = 0;      // ToNode�� Index

	UINT	nPathIndex			= 0;		// Path�� Index

	NODE_INFO	*pNODE_INFO			= NULL;	// Node ����
	PATH_INFO	*pPATH_INFO	= NULL;	// Path ����

	//1) Node Index Check �κ�
	if(	(pTmpMap->FindNodeIndex(ParentNode)	==	INDEX_NONE)
	||	(pTmpMap->FindNodeIndex(NextNode)	==	INDEX_NONE)
	)
	{
		ADD_LOG("[Node:CheckEndStationOffset] %d:%d:%d:%d"
			, pTmpMap->FindNodeIndex(ParentNode), pTmpMap->FindNodeIndex(NextNode), ParentNode, NextNode);

		return true;

	}
	try
	{
		//2) Link Index Check �κ�
		nFromNodeIndex = pTmpMap->FindNodeIndex(ParentNode);   ///< ��� ��� Index
		pNODE_INFO = &(pTmpMap->m_NodeArray[nFromNodeIndex]);  ///< ��� ��� ����


		if(pNODE_INFO->NextPathIndex[0]==INDEX_NONE && pNODE_INFO->NextPathIndex[1]==INDEX_NONE)// ���� ��忡 Path ������� ���� ���
		{
			ADD_LOG("[Link From:CheckEndStationOffset] %d:%d:%d:%d"
			,pNODE_INFO->NextPathIndex[0], pNODE_INFO->NextPathIndex[1],ParentNode,nFromNodeIndex);

			return true;
		}

		//3) NexeNode Ȯ���ϴ� �κ�
		breturn = true; //ã�� ��� false�� ��ȯ
		for(int i=0; i<2;i++)
		{
			nPathIndex	= pNODE_INFO->NextPathIndex[i];
			if(nPathIndex!=INDEX_NONE)
			{
				pPATH_INFO	= &(pMap->m_PathArray[nPathIndex]);
				if(pPATH_INFO->EndNID==NextNode)
				{
					breturn	= false;
					break;
				}
			}
		}


		//3) Station Offset �� Ȯ���ϴ� �κ�
		if(breturn ==false)
		{
				//1 3. Station������ �Ÿ��� Link ������ �Ÿ����� Ŭ���
			if(	EndStationOffset > pPATH_INFO->Distance ||
				EndStationOffset < 0)
			{
				breturn = true;
				ADD_LOG("[StationOffset:CheckEndStationOffset] %d:%d"
					, EndStationOffset
					, pPATH_INFO->Distance);
			}
		}
    }catch(...)
		ADD_LOG("[CheckNodeAndLinkIndex] Exception");

	return breturn;
}

int PathSearch::ChangePathVelocity(void)
{
	/*
		�⺻������ ���ӵ� N������ �Ѱ��� �ӵ��� MAP�� �����Ǿ� �ϸ�
		���ܷ� CID�� ���� 3.3 ���� MAP �ӵ��� ���ѵǾ� �ִ� ��찡 �����Ƿ�
		�ش� ������ ��� ����� ��.
		CID�� ���� �ӵ� ��ȭ ���� ���� ���� �Ϸ� �� ������ �ӵ� ���
	*/
	int nError = NO_ERR;
	//LList *m_PathList = OHTMainForm->GetPathList();

	int cntPath = GetPathCount();
	int pathIndex;
	int tempIndex;

	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO_TEMP = NULL;

	PATH_STEERING_INFO StartSteering;	// ù��°����
	PATH_STEERING_INFO EndSteering;   // �ι�°����

	double dDefaultDecel;
	double dDefaultAccel;
	double dVelNew;
	double dVelOri;
	double dVelTemp;

	double dVelBefore;
	double dVelAfter;

	double dVelIn;
	double dVelOut;
	double dDisSum; // ���⿡�� ���� ������� ���� �Ÿ�
	int nNodeCnt;

	bool bVelChangeSkipFlag;

	if(cntPath == 1)
	{
		//ADD_LOG("[Test] cntPath = 1");
		return nError; // Pathserch ��� link �ϳ��� ��� skip
	}
	if(m_pDrivingControl == NULL) m_pDrivingControl = OHTMainForm->GetDrivingControl();

	if(m_pDrivingControl != NULL){

		dDefaultAccel = m_pDrivingControl->GetDefaultAccel()*1000;
		dDefaultDecel = m_pDrivingControl->GetDefaultDecel()*1000;
	}
	else{
		dDefaultAccel = DEFAULT_ACCEL;
		dDefaultDecel = DEFAULT_DECEL;
	}

	dDisSum = 0;
	nNodeCnt = 0;
	dVelBefore = 0;

	// 1. ������ N - N - R - N - N - N - L - N - N �� �� R�� L �� ���� ���� ���̿��� N ������ ���� �۾�
	// 2. N �������� �ӵ��� ���ӵ��� ���� ��� skip (CID�� ���� �ӵ� ���� ��)

	for(pathIndex=0; pathIndex < cntPath - 2; pathIndex++) // // AOHC-230 Map �ӵ� ���� E23xx ����
	{
		pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex);
		dVelAfter  = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;
		if(pathIndex == 0) StartSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;

		if(pEXECUTE_DRIVING_INFO->BasicInfo.Steering == STEERING_NONE)
		{
			if(nNodeCnt == 0) // N���� ���� ����
			{
				dVelIn = dVelBefore;
			}

			if(pathIndex == 0) // pathsearch ó��
			{
				dDisSum = pEXECUTE_DRIVING_INFO->ChangeEndNodeDistance;
				dVelIn = dVelBefore = 0;  // 1��° node�� ��� ���� �����Ƿ� �ӵ� 0
				//dVelIn = dVelBefore = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;  // 1��° node�� �ӵ� 0���� ���Ϸ��� ���ڵ� ���
			}
			else
			{
				dDisSum += pEXECUTE_DRIVING_INFO->BasicInfo.dLength;
				dVelBefore = dVelAfter;
			}

			if(pathIndex != cntPath - 3) { // AOHC-230 Map �ӵ� ���� E23xx ����
				nNodeCnt++; // N���� count ����
				continue; // ������ node�� �ƴ� ���
			}
		}

		EndSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
		dVelOut = dVelAfter; // dVelBefore <  dVelAfter ? dVelBefore :	dVelAfter;
         		// add by jhoun.lim
		if(pathIndex == cntPath - 3 && pEXECUTE_DRIVING_INFO->BasicInfo.Steering == STEERING_NONE)
		{
			// get next path
			pEXECUTE_DRIVING_INFO = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
			EndSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;
			dVelOut  = pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed;
			if(EndSteering != STEERING_NONE)
			{
				pathIndex++;
				nNodeCnt++;
			}
		}

		//==========================================================================================
		// CID, SLOPE, LIFT �� ���� �ǵ����� �ӵ� ���� � ����
		// ���ӵ� N�������� �ӵ��� ���ϴ� �κ��� �ִ��� Ȯ��
		// �ٲ�� ������ ���� ���� Skip
		// N������ 2�� �̻��� ��� �� ����
		bVelChangeSkipFlag = false;
		if(nNodeCnt >= MIN_CNT_OF_VEL_CHANGE)
		{
			if(EndSteering == STEERING_NONE) tempIndex = LAST_INDEX_OF_PATH; // pathsearch�� ������ node�� ���
			else tempIndex = LAST_INDEX_OF_NORMAL_LINK;

			if((0 <= (pathIndex - tempIndex)) && (cntPath > (pathIndex - tempIndex)) && (cntPath > (pathIndex - nNodeCnt))){ // ����ڵ�
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex++);
				dVelTemp = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

				for(; tempIndex<=nNodeCnt; tempIndex++){
					pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex);
					dVelOri  = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;
					if(dVelOri != dVelTemp) {
						//ADD_LOG("[Test] VelChange = true");
						bVelChangeSkipFlag = true;
						break;
					}
				}
			}
		}
		//==========================================================================================


		//==========================================================================================
		// IN, OUT �ӵ�, ���ӵ� N���� �Ÿ�, ���� ���� ���θ� ���� �ӵ� ��� �� ���� �ӵ� ����
		if((nNodeCnt != 0 || EndSteering ==STEERING_NONE) && (bVelChangeSkipFlag == false))// if(���� ���� �ƴ� ��� || ������ link �� ��� && ���ӵ� N�������� �ӵ� ��ȭ ���� ���)
		{
			if(((StartSteering == STEERING_LEFT) && (EndSteering == STEERING_RIGHT)) ||
				((StartSteering == STEERING_LEFT) && (EndSteering == STEERING_N_BRANCH_R2L)) ||
				((StartSteering == STEERING_N_BRANCH_L2R) && (EndSteering == STEERING_LEFT)) ||
				((StartSteering == STEERING_N_BRANCH_L2R) && (EndSteering == STEERING_N_BRANCH_L2R)) ||
				((StartSteering == STEERING_RIGHT) && (EndSteering == STEERING_LEFT)) ||
				((StartSteering == STEERING_RIGHT) && (EndSteering == STEERING_N_BRANCH_L2R)) ||
				((StartSteering == STEERING_N_BRANCH_R2L) && (EndSteering == STEERING_RIGHT)) ||
				((StartSteering == STEERING_N_BRANCH_R2L) && (EndSteering == STEERING_N_BRANCH_R2L)))
			{
				// ����� �ӵ� ���, ���� �ٸ� ���
				dVelNew = GetNewVelocity(dVelIn, dVelOut, dDefaultAccel, dDefaultDecel, dDisSum, true);
			}
			else 
			{
				// ����� �ӵ� ���, ���� ���� ���
				dVelNew = GetNewVelocity(dVelIn, dVelOut, dDefaultAccel, dDefaultDecel, dDisSum, false);
			}

			if(EndSteering == STEERING_NONE) tempIndex = LAST_INDEX_OF_PATH; // pathsearch�� ������ node�� ��� �ӵ� ���濡 ����
			else tempIndex = LAST_INDEX_OF_NORMAL_LINK;
			for(; tempIndex<=nNodeCnt; tempIndex++){
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex - tempIndex);
				dVelOri  = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;
				//pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelNew;

				if(dVelNew < dVelOri) {
					//ADD_LOG("[MapVelOptim] Map speed change, NodeID:%d, AsIs:%.2f, ToBe:%.2f", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
					//	pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed, dVelNew);
					pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelNew; // ����� �ӵ� ����
				}
				else{
					ADD_LOG("[MVO]Map speed change x/N:%d,MapS:%.2f,CalS:%.2f", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
						pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed, dVelNew);
					break;
				}
			}
			if(dVelNew < dVelOri) {
				ADD_LOG("[MVO] Map speed change/N:%d,AsIs:%.2f,ToBe:%.2f,nNC:%d", pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.StartNID,
					dVelOri, dVelNew, nNodeCnt);
			}
		}

		// �Ÿ�, Node count, �ʱ�ȭ
		dDisSum = 0;
		nNodeCnt = 0;
		dVelBefore = dVelAfter;
		StartSteering = pEXECUTE_DRIVING_INFO->BasicInfo.Steering;

		// N�б⸦ ������ �Ŀ��� ������ �ݴ�� �ٲ�Ƿ� StartSteering�� ����
		if(StartSteering == STEERING_N_BRANCH_L2R) StartSteering = STEERING_RIGHT;
		else if(StartSteering == STEERING_N_BRANCH_R2L) StartSteering = STEERING_LEFT;
	}

	// AOHC-230 Map �ӵ� ���� E23xx ����
	// cntPath - 1, cntPath ���Ͽ� ���� �� ���
	if ((EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1) != NULL) //��� �ڵ�
	{
		double dVelBeforLast = 0;
		double dVelLast = 0;

		pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
		dVelBeforLast = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

		pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1);
		dVelLast = pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed;

		if (dVelBeforLast != 0 && dVelLast != 0) //��� �ڵ�
		{
			if (dVelBeforLast > dVelLast)
			{
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath - 2);
				pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelLast;
			}
			else
			{
				pEXECUTE_DRIVING_INFO_TEMP = (EXECUTE_DRIVING_INFO *)m_PathList->refer(cntPath-1);
				pEXECUTE_DRIVING_INFO_TEMP->BasicInfo.dSpeed = dVelBeforLast;
			}
		}
	}
	return nError;
}


double PathSearch::GetNewVelocity(double dVelIn, double dVelOut, double dDefaultAccel, double dDefaultDecel, double dDistanceSum, bool bSteeringChange)
{
	double dDistanceDecel = 0.0;
	
	double dVelCalculated;
	double distance = dDistanceSum / 1000;

	double dStopTime;
	double dSpeedDownTime;

	double dDistanceAccel = 0.0;

	double defaultAccelDistance;
	double defaultAccelTime;

	// steering ��ȭ�� �ִ� ���

	/******************************************************************************
	1. ���� ���� �ð� >= ���� ���� �ð� (���� �Ұ�, ��� �ð� = ���� ���� �ð�) 
	  �� ���� �ӵ� = ���� �ӵ� ���Ϸ� ��� (Case1)
	2. ���� ���� �ð� <= ���� ���� �ð� (���� ����, ���� �ð� + ��� �ð� = ���� ���� �ð�)
	 1) (���� �ӵ� < 3.3m/s) && (���� �ӵ� < 3.3m/s) �� ���� ���� �ð��� ���� ���� �ð����� �ϴ� �ִ� �ӵ��� ��� (Case2-1)
	 2) (���� �ӵ� < 3.3m/s) && (���� �ӵ� > 3.3m/s) �� Case2�� ����, 3.3m/s �̻���� ���ӵ� 1.0 ��� (Case2-2) 
	 3) (���� �ӵ� > 3.3m/s) �� Case2�� ����, ���ӵ� 1.0 ��� (Case2-3) 
	3. ���� ���� �ð� > ���� ���� �ð� (���� ����, ��� �ð� = ��� �ð�) 
	 1) (���� �ӵ� < 3.3m/s) && (���� �ӵ� < 3.3m/s) �� ���� �ӵ� ��� (Case3-1)
	 2) (���� �ӵ� < 3.3m/s) && (���� �ӵ� > 3.3m/s) �� Case5�� ����, 3.3m/s �̻���� ���ӵ� 1.0 ��� (Case3-2)
	 3) (���� �ӵ� > 3.3m/s) �� Case5�� ����, ���ӵ� 1.0 ��� (Case3-3)
	******************************************************************************/
	if(bSteeringChange == true){

		dStopTime = dVelIn/dDefaultDecel;
		///dDistanceDecel = GET_DECEL_DIST(dVelIn, 0, dDefaultDecel);
		double dMinStrChangeDis = ((dStopTime+TIME_OF_STEERING_CHANGE+TIME_OF_STEERING_CHANGE)* dVelIn)/2;  // ��ӽð��� ���� �Ǵ��� Ȯ��

		if(dMinStrChangeDis > distance) 
		{
			// Steering change �� ���� ���� �ʴ� ��� Vin�� ���� (Vin == Vmax, steering change time == ��ӽð�)
			// ���� ���� ���Լӵ� �����Ÿ� ��ٸ��� ���
			ADD_LOG("[�ӵ�����] ���⺯��1");
			dVelCalculated = abs((-1.0*dDefaultDecel*(TIME_OF_STEERING_CHANGE))+(dDefaultDecel*(
			sqrt(((TIME_OF_STEERING_CHANGE)*(TIME_OF_STEERING_CHANGE))+(2.0*distance/dDefaultDecel)))));
		}
		else
		{
			if(dVelIn >= HIGH_SPEED_LIMIT_VEL) // ���� �ӵ��� 3.3 �̻��� ���
			{	
				// Vmax�� ���ϴ� ��
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
				dSpeedDownTime = TIME_OF_CONST_VEL + ((dVelCalculated - dVelIn) /HIGH_SPEED_ACCEL);

				// Vmax�� Steering change time�� ���� �ش�
				if(dSpeedDownTime <= TIME_OF_STEERING_CHANGE)
				{
					ADD_LOG("[�ӵ�����] ���⺯��2-3");
					dVelCalculated = calcSpeed_2(dVelIn, distance, TIME_OF_STEERING_CHANGE, HIGH_SPEED_ACCEL, dDefaultDecel);
				}
				else{
					ADD_LOG("[�ӵ�����] ���⺯��3-3");
				}
			}
			else
			{
				// Vmax�� ���ϴ� ��
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
				dSpeedDownTime = TIME_OF_CONST_VEL + ((dVelCalculated - dVelIn) /dDefaultAccel);

				if(dSpeedDownTime <= TIME_OF_STEERING_CHANGE)
				{
					dVelCalculated = calcSpeed_2(dVelIn, distance, TIME_OF_STEERING_CHANGE, dDefaultAccel, dDefaultDecel);

					if(dVelCalculated > HIGH_SPEED_LIMIT_VEL)
					{
						ADD_LOG("[�ӵ�����] ���⺯��2-2");
						//defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel);
						defaultAccelTime = (HIGH_SPEED_LIMIT_VEL-dVelIn)/dDefaultAccel;
						defaultAccelDistance = ((dVelIn + HIGH_SPEED_LIMIT_VEL) * defaultAccelTime) / 2;

						defaultAccelTime = TIME_OF_STEERING_CHANGE - defaultAccelTime; // Tstr(streering ���� �ð�) - T3.3(3.3���� �ð�)
						dVelCalculated = calcSpeed_2(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, defaultAccelTime, HIGH_SPEED_ACCEL, dDefaultDecel);
						
					}
					else if(dVelCalculated <= HIGH_SPEED_LIMIT_VEL)
					{
						// Vmax ���� ���ӵǴ� ������ Sterring change time���� ª�� �ð��� ��� ����� Sterring change time ��ŭ �ϰԲ� Vmax�� ����
						ADD_LOG("[�ӵ�����] ���⺯��2-1");
						//dVelCalculated = (sqrt((TIME_OF_STEERING_CHANGE+dVelIn/dDefaultAccel)*(TIME_OF_STEERING_CHANGE+dVelIn/dDefaultAccel)+
						//(1/dDefaultDecel-1/dDefaultAccel)*(dVelIn*dVelIn/dDefaultAccel+2*distance))
						//-TIME_OF_STEERING_CHANGE-dVelIn/dDefaultAccel)/(1/dDefaultDecel-1/dDefaultAccel);
					}

				}

				else if(dVelCalculated > HIGH_SPEED_LIMIT_VEL && dSpeedDownTime > TIME_OF_STEERING_CHANGE)
				{	
					// 3.3m/s ���Ĵ� ���ӵ� 1.0�� ����ϹǷ� 3.3m/s�� �����ϴ� �Ÿ���ŭ �� �� ���ӵ� 1.0���� �ٽ� ���
					ADD_LOG("[�ӵ�����] ���⺯��3-2");
					defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel); // default accel (2.0)���� 3.3m/s ���� ���� �ϴµ������� �Ÿ�
					dVelCalculated = calcSpeed(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);

				}

				else{
					//�Ʒ� ������ �ش� -> ���� �� �״�� ���
					//if(dVelCalculated <= HIGH_SPEED_LIMIT_VEL && dSpeedDownTime > TIME_OF_STEERING_CHANGE)
					ADD_LOG("[�ӵ�����] ���⺯��3-1");
				}
			}
			
		}

	}
	// steering ��ȭ�� ���� ���
	else{
		/******************************************************************************
		����
		1) ���� �ӵ� < ���� �ӵ� && ���� �Ÿ� >= ���� �Ÿ�
		  �� ���� �ӵ� = ���� �ӵ�
		2) ���� �ӵ� < ���� �ӵ� && ���� �Ÿ� < ���� �Ÿ�
		  �� ���� �ӵ� = �Ÿ��� ���� ����(���� �ӵ� or ���� �ӵ� +a)
		3) ���� �ӵ� > ���� �ӵ� && ���� �Ÿ� >= ���� �Ÿ�
		  �� ���� �ӵ� = ���� �ӵ�
		4) ���� �ӵ� > ���� �ӵ� && ���� �Ÿ� < ���� �Ÿ�
		  �� ���� �ӵ� =  �Ÿ��� ���� ����(���� �ӵ� or ���� �ӵ� + a)
		5) ���� �ӵ� = ���� �ӵ�
		  �� ���� �ӵ� = �Ÿ��� ���� ����
		******************************************************************************/

		if(dVelIn < dVelOut) dDistanceAccel = GET_ACCEL_DIST(dVelIn, dVelOut, dDefaultAccel);
		if(dVelIn > dVelOut) dDistanceDecel = GET_DECEL_DIST(dVelIn, dVelOut, dDefaultDecel);

		if((dVelIn < dVelOut) && (dDistanceAccel >= distance)){ // case 1
			dVelCalculated = dVelOut;
			ADD_LOG("[�ӵ�����] ����̺��� 1");
		}
		else if((dVelIn < dVelOut) && (dDistanceAccel < distance)){ // case 2
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelOut* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelOut; // ���� ���� �� �ӵ��� out �ӵ��� ���(10%)�ϸ� out �ӵ� ����
			ADD_LOG("[�ӵ�����] ����̺��� 2");
		}
		else if((dVelIn > dVelOut) && (dDistanceDecel >= distance)){ // case 3
			dVelCalculated = dVelIn;
			ADD_LOG("[�ӵ�����] ����̺��� 3");
		}
		else if((dVelIn > dVelOut) && (dDistanceDecel < distance)){ // case 4
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelIn* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelIn; // ���� ���� �� �ӵ��� in �ӵ��� ���(10%)�ϸ� out �ӵ� ����
			ADD_LOG("[�ӵ�����] ����̺��� 4");
		}
		else if(dVelIn == dVelOut){ // case 5
			dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, dDefaultAccel, dDefaultDecel);
			if(dVelCalculated < dVelOut* VELOCITY_INCREASE_MARGIN) dVelCalculated = dVelOut;
			ADD_LOG("[�ӵ�����] ����̺��� 5");
		}
		else{
			//error;
			dVelCalculated = DEFAULT_VEL;
		}
		
		if(dVelCalculated > HIGH_SPEED_LIMIT_VEL)
		{	
			if(dVelIn > HIGH_SPEED_LIMIT_VEL){
				dVelCalculated = calcSpeed(dVelIn, distance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
				ADD_LOG("[�ӵ�����] ����̺��� 6");
			}
			else{
				defaultAccelDistance = (HIGH_SPEED_LIMIT_VEL*HIGH_SPEED_LIMIT_VEL - dVelIn*dVelIn)/(2.0*dDefaultAccel); // default accel (2.0)���� 3.3m/s ���� ���� �ϴµ������� �Ÿ�
				dVelCalculated = calcSpeed(HIGH_SPEED_LIMIT_VEL, distance-defaultAccelDistance, TIME_OF_CONST_VEL, HIGH_SPEED_ACCEL, dDefaultDecel);
                ADD_LOG("[�ӵ�����] ����̺��� 7");
			}
		}
	}
	

	if(dVelCalculated <= 0.7) dVelCalculated = 0.7; // �ּ� �ӵ�

	//ADD_LOG("[Test] �ӵ����� �Լ�");
	return dVelCalculated;
}


// ��� �ð� ����  (constTime)
double PathSearch::calcSpeed(double velIn, double distance, double constTime, double accel, double decel)
{
	// ���� ���� �Ǵ�
	// 1. ���� �Ÿ� ���� �ȵǸ� velOut = velIn
	double decelDist = (velIn*velIn) / (2.0*decel);
	if(distance < decelDist)
	{
		return velIn;
	}

	// 2. ��ӽð��� ������ �� ������ velOut = velIn
    double constDist = velIn * constTime;
	if (distance < decelDist + constDist)
	{
		return velIn;
	}

	// A(X^2) + 2B(X) - C = 0
	// X = (-B + sqrt(B^2 + AC)) / A
	double A = 1.0/decel+1.0/accel;
	double B = constTime;
	double C = velIn*velIn/accel + 2*distance;

	double velOut = (sqrt(B*B+A*C)- B) / A;
	return velOut;
}


// ���� Ÿ�̹� ����. (spdDownTime)
double PathSearch::calcSpeed_2(double velIn, double distance, double spdDownTime, double accel, double decel)
{
	// ���� ���� �Ǵ�
	// 1. ���� �Ÿ� ���� �ȵǸ� velOut = velIn
	double decelDist = (velIn*velIn) / (2.0*decel);
	if(distance < decelDist)
	{
		return velIn;
	}

	// 2. ���� ������ ������ �� ������ velOut = velIn
    double constDist = velIn * spdDownTime;
	if (distance < decelDist + constDist)
	{
		return velIn;
	}

	// A(X^2) + 2B(X) - C = 0
	// X = (-B + sqrt(B^2 + AC)) / A
	double A = 1.0/decel - 1.0/accel;
	double B = spdDownTime + velIn/accel;
	double C = velIn*velIn/accel + 2*distance;

	double velOut;
	if(A == 0) // 2BX = C (2���� ���� ����)
		velOut =  C / (2.0 * B);
	else
		velOut = (sqrt(B*B+A*C)- B) / A;

	return velOut;
}
bool PathSearch::CheckSteering4Deccel(PARSING_GO_INFO *GetGOInfo,int ChangeNodeID)
{
	int cntPath = GetPathCount();
	int cntSteer = 0;
	int pathIndex =0;
	int CheChangeNode =ChangeNodeID;
	UINT	uFromNode			= GetGOInfo->PathData_StartNodeID;			// ���� ���
	UINT	uToNode				= GetGOInfo->PathData_EndNodeID;		    // �� ���
	int		nPathListIndex	   	= 0;		// ���ϰ����ϴ� ��� ��ġ.

	EXECUTE_DRIVING_INFO 	*pPathListInfo  = NULL;
   //	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO = NULL;
  //	EXECUTE_DRIVING_INFO *pEXECUTE_DRIVING_INFO_TEMP = NULL;

	for(int i=0; i< cntPath; i++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO*) ReferPath(i);
	 //	ADD_LOG("[PATHSEARCH Start ID]%d ExtraID:%d %d",pPathListInfo->BasicInfo.StartNID, CmdGoData->PathData_ExtraID[uCheckExtraNum],CmdGoData->PathData_ExtraNum);

	 //	ChangeSqNum = pPathListInfo->SQNum;  //�ʱⰪ ����

		if(pPathListInfo->BasicInfo.StartNID ==  uFromNode)
		{
			nPathListIndex = i;
//				 ADD_SOM_LOG("StartNID %d, uFromNode = %d ,nPathListIndex:%d ",pPathListInfo->BasicInfo.StartNID, uFromNode,i ) ;
		  //	bArrived = true;
			break;
		}
	}

//	ADD_SOM_LOG("cntPath = %d",cntPath );
	for(pathIndex= nPathListIndex; pathIndex < cntPath ; pathIndex++)
	{
		pPathListInfo = (EXECUTE_DRIVING_INFO *)m_PathList->refer(pathIndex);

		if(pPathListInfo->BasicInfo.Steering == STEERING_NONE)
		{
			// ���� NONE �� ��� ++;
//			 ADD_SOM_LOG("Steering = %d",pPathListInfo->BasicInfo.Steering ) ;
//			 ADD_SOM_LOG("node %d", pPathListInfo->BasicInfo.StartNID);
			 cntSteer ++;



		}
		else
		{  //���� NONE�� �ƴѰ� �ϳ��� ������ return �Ͽ� ���ӵ� fix
		 //	cntSteer = 0;
//			ADD_SOM_LOG("diff status node %d ,cntPath:%d, cntSteer:%d", pPathListInfo->BasicInfo.StartNID,cntPath,cntSteer);
			return false;
		}

		if(pPathListInfo->BasicInfo.EndNID == CheChangeNode)        //ChangeNode ������ ���� üũ ��
		{
//			ADD_SOM_LOG("End %d, change node %d", pPathListInfo->BasicInfo.EndNID,CheChangeNode);
			break;
		 }

	}
	//Path�� NONE �� �ִ� ��� Ȯ��
//	if(cntPath != 0)
//	{
//		  ADD_SOM_LOG("same status cntPath:%d, cntSteer:%d,pathIndex:%d",cntPath,cntSteer,pathIndex);
		  return true;
//	}

}

//AOHC-302 �ʱ�� ������ �ڵ�Ž��
bool PathSearch::FindTheWay(MAP *pMap, UINT FromNode, UINT ToNode, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	//�ʱ�� �̵�, ����ó���� ���� ����
	m_bFindTheWay = true;
	m_EndNodeCnt = 0;

	//����/���п��� ����
	bool bSuccess = true;

	//while�� ī��Ʈ
	int nCnt = 0;

	//����, �����
	int nStartNode = FromNode;
	int nEndNode = ToNode;

	//������ Ȯ�ο� ����, ����Ʈ
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node ����
	PATH_INFO	*pPathInfo	= NULL;	  // Path ����
	LaneCutSt	*TempLanCut = NULL;   // LaneCut ����

	//��� ���⿡ ���Ǵ� ����Ʈ
	LList	*pResultAddList	= DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO  *pEXECUTE_DRIVING_INFO = NULL;

	//�˰��� ���Ǵ� ����

	int nIndex = 0;                 //���� ������ ��� ���� Cnt(�ߺ� ����)
	int nVisitCnt = 0;              //���� Visit�� ��ũ ���� Cnt(�ߺ� ����)
	int nMaxVisitCnt = 0;      	 	//�ִ� Visit�� ��ũ ���� Cnt
	int nRouteListSize = 0;     	//����Ʈ�� ���� ������ ��

	int Node;						//���� ���
	int NodeArray[250];        		//������ ��� ���� ����

	std::list <RouteSt> RouteList;	//�˰��� ���Ǵ� ����Ʈ
	RouteSt RouteTemp;				//����Ʈ�� ���Ǵ� ����ü

	//Initailization
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad())
	{
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//���۳��, ����� �������� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)
	||	(pMap->FindNodeIndex(nEndNode)	==	-1)	)
	{
		ADD_LOG("No Map Info, Start: %d, End: %d", nStartNode, nEndNode);
		return false;
	}

	//���� ��ũ�� �� �������� Ȯ��, ��ũ�� �ƴ� �� Fail
	if(AdditionalInfo.StationType != MARK_TYPE_NODE)
	{
		if(CheckPathOfStation(pMap, nStartNode, nEndNode, AdditionalInfo)!= NO_ERR)
		{
			ADD_LOG("No Node Mark");
			return false;
		}
	}

	//	���� ��� ���� ���� ��� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop�� ������ �� ��� Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	while(idx.getCount() != 0)
	{
		//ī��Ʈ �̻� �� ����
		if(nCnt > 50000)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}

		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode  ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;

		for(int i = 0; i<nCntChk;i++)
		{
			if(VisitLinkCheckArray[Node][i]==0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitCnt++;
			}

			if(nTempNextNode[i]!=-1)
				idx.insertBack(nTempNextNode[i]);
		}
		nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//����Ʈ �ʱ�ȭ
	RouteList.clear();

	//ó�� ����Ʈ�� ���� ������ ����
	RouteTemp.index = 0;
	RouteTemp.VisitCnt = 0;
	RouteTemp.indexNode = nStartNode;

	ZeroMemory(&RouteTemp.NodeArray, sizeof(RouteTemp.NodeArray));
	RouteTemp.NodeArray[RouteTemp.index++] = nStartNode;


	//����Ʈ iteratior, ���� ����Ʈ ����
	nCnt = 0;
	list<RouteSt>::iterator iterRouteSt;
	RouteList.push_back(RouteTemp);


	//�˰���  ����
	while(!RouteList.empty())
	{
		//����Ʈ�� �� �յ����͸� POP
		nRouteListSize = RouteList.size();
		iterRouteSt = RouteList.begin();
		nIndex = iterRouteSt->index;
		nVisitCnt = iterRouteSt->VisitCnt;
		Node = iterRouteSt->indexNode;
		memmove(&NodeArray,  &iterRouteSt->NodeArray, sizeof(iterRouteSt->NodeArray));
		RouteList.pop_front();

		//���� ȸ�� �ּ�ȭ
		if(nMaxVisitCnt <= nVisitCnt)
		{
			nMaxVisitCnt = nVisitCnt;
		}
		else
		{
			continue;
		}

		//��������, ���Ž���Ϸ�
		if(nVisitCnt == VisitCnt && Node == nEndNode)
		{
			int NodeSum = nIndex;
			int FromNode;
			int ToNode;
			int nPathIndex;
			ADD_LOG("index: %d", NodeSum);

			for(int i=0; i<NodeSum-1; i++)
			{
				FromNode   = NodeArray[i];
				ToNode 	   = NodeArray[i+1];

				if(ToNode == nEndNode)
					m_EndNodeCnt++;

				nPathIndex	= pMap->GetPathIndex(FromNode, ToNode);
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);

				ADD_LOG("Link: %d-%d, Steer: %d", FromNode, ToNode,	pPathInfo->RailBranch);

				pEXECUTE_DRIVING_INFO	 = new EXECUTE_DRIVING_INFO;
				pEXECUTE_DRIVING_INFO->BasicInfo.StartNID			= FromNode;
				pEXECUTE_DRIVING_INFO->BasicInfo.EndNID				= ToNode;
				pEXECUTE_DRIVING_INFO->BasicInfo.StartNType			= 0;
				pEXECUTE_DRIVING_INFO->BasicInfo.EndNType			= 0;
				pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed				= (double)(pPathInfo->Speed/1000.0);			// mm/s -> mm/msec
				pEXECUTE_DRIVING_INFO->BasicInfo.dLength			= (double)(pPathInfo->Distance);		// UINT --> double
				pEXECUTE_DRIVING_INFO->BasicInfo.Steering			= (PATH_STEERING_INFO)(pPathInfo->Steering);
		#if(USE_CHANGENODE == 1)
				pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion			= (PATH_DIRECTION_INFO)(pPathInfo->UBGRegion);
				pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion 	= (PATH_OBS_DIRECTION_INFO)(pPathInfo->ObstacleRegion);
				pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
				pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag 	= pPathInfo->ObstacleIgnore;

				pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;

				pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance		= pPathInfo->OptDistance;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter	= pPathInfo->OptDistanceAfter;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion 		= (PATH_DIRECTION_INFO)pPathInfo->OptUBGRegion;
				pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion	= (PATH_OBS_DIRECTION_INFO)pPathInfo->OptObstacleRegion;
		#else
				pEXECUTE_DRIVING_INFO->BasicInfo.Direction			= (PATH_DIRECTION_INFO)(pPathInfo->PathDirection);
				pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
				pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag		= pPathInfo->ObstacleIgnore;
				pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;
		#endif
				memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));


				pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);
			}
			break;
		}
		//��������, Index�� 250 �̻��� ��, ����
		else if(nIndex >= 250)
		{
			bSuccess = false;
			break;

		}
		//�� �� ����, Ž�� ��� ����


		//������ üũ
		int NodeIndex = pMap->FindNodeIndex(Node);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;
			}
			else
			{
				nTempNextNode[i] = -1;
			}
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
				else
				{
					break;
				}
			}
		}

		//��� �� üũ
		int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;

		//��� ����ŭ ����Ʈ�� �״´�.
		for(int i = 0; i < nCntChk; i++)
		{
			//������ �ʱ�ȭ, ����ġ��
			ZeroMemory(&RouteTemp, sizeof(RouteTemp));
			RouteTemp.index = nIndex;
			RouteTemp.VisitCnt = nVisitCnt;
			RouteTemp.indexNode = nTempNextNode[i];
			memmove(&RouteTemp.NodeArray, &NodeArray, sizeof(NodeArray));
			RouteTemp.NodeArray[RouteTemp.index++] = nTempNextNode[i];

			//�Դ� ������� Ȯ���ϴ� �Լ�
			Fn_VisitCheck(&RouteTemp);

			//����Ʈ�� �߰�
			RouteList.push_back(RouteTemp);
		}

		nCnt++;
	}
	ADD_LOG("End Count: %d", m_EndNodeCnt);

	//����Ʈ �ʱ�ȭ
	while(!RouteList.empty())
	{
		RouteList.pop_front();
	}
	RouteList.clear();
	LaneCutList.clear();

	return bSuccess;
}

bool PathSearch::RouteCheckNode(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_MOVE_REQ PACKET_BODY, EXECUTE_DRIVING_ADD_INFO AdditionalInfo, LList *DRIVING_INFO_List)
{
	//�ʱ�� �̵�, ����ó���� ���� ����
	m_bFindTheWay = true;
	m_EndNodeCnt = 0;

	//����/���п��� ����
	bool bSuccess = true;

	//���۳��,  �����
	int nStartNode  		=   PACKET_BODY.StartID;
	int nEndNode 			=	PACKET_BODY.EndID;
	int nNodeCnt 			= 	PACKET_BODY.RouteCount;
	int RouteList[50] 		= 	{0,};
	memset(RouteList,  	0,						sizeof(RouteList));
	memcpy(RouteList, 	PACKET_BODY.RouteList, 	sizeof(RouteList));

	NODE_INFO	*pNodeInfo	= NULL;	  // Node ����
	PATH_INFO	*pPathInfo	= NULL;	  // Path ����
	LaneCutSt	*TempLanCut = NULL;   // LaneCut ����

	//��� ���⿡ ���Ǵ� ����Ʈ
	LList	*pResultAddList	= DRIVING_INFO_List;
	EXECUTE_DRIVING_INFO  *pEXECUTE_DRIVING_INFO = NULL;


	//���۳��, ����� �������� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	if((pMap->FindNodeIndex(nStartNode) ==	-1)
		||	(pMap->FindNodeIndex(nEndNode)	== -1) )
	{
		ADD_LOG("[RouteCheckNode]No Map Info, Start: %d, End: %d", nStartNode, nEndNode);
		return false;
	}


	//���� ��ũ�� �� �������� Ȯ��, ��ũ�� �ƴ� �� Fail
	if(AdditionalInfo.StationType != MARK_TYPE_NODE)
	{
		if(CheckPathOfStation(pMap, nStartNode, nEndNode, AdditionalInfo)!= NO_ERR)
		{
			ADD_LOG("[RouteCheckNode]No Node Mark");
			return false;
		}
	}

	//���� ��� ���� ���� ��� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);

	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("[RouteCheckNode]Next Node Index None");
		return false;
	}

	ADD_LOG("[RouteCheckNode]index: %d", nNodeCnt);

	for(int i=0; i<nNodeCnt-1; i++)
	{
		int FromNode   = RouteList[i];
		int ToNode 	   = RouteList[i+1];

		if(ToNode == nEndNode)
			m_EndNodeCnt++;

		int nPathIndex = pMap->GetPathIndex(FromNode, ToNode);
		if(nPathIndex == INDEX_NONE)
		{
			ADD_LOG("[RouteCheckNode]Path Abnormal");
			return false;
		}
		pPathInfo = &(pMap->m_PathArray[nPathIndex]);

		ADD_LOG("[RouteCheckNode]Link: %d-%d, Steer: %d", FromNode, ToNode,	pPathInfo->RailBranch);

		pEXECUTE_DRIVING_INFO	 							= new EXECUTE_DRIVING_INFO;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNID			= FromNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNID				= ToNode;
		pEXECUTE_DRIVING_INFO->BasicInfo.StartNType			= 0;
		pEXECUTE_DRIVING_INFO->BasicInfo.EndNType			= 0;
		pEXECUTE_DRIVING_INFO->BasicInfo.dSpeed				= (double)(pPathInfo->Speed/1000.0);			// mm/s -> mm/msec
		pEXECUTE_DRIVING_INFO->BasicInfo.dLength			= (double)(pPathInfo->Distance);		// UINT --> double
		pEXECUTE_DRIVING_INFO->BasicInfo.Steering			= (PATH_STEERING_INFO)(pPathInfo->Steering);
#if(USE_CHANGENODE == 1)
		pEXECUTE_DRIVING_INFO->BasicInfo.UBGRegion			= (PATH_DIRECTION_INFO)(pPathInfo->UBGRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.ObstacleRegion 	= (PATH_OBS_DIRECTION_INFO)(pPathInfo->ObstacleRegion);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag 	= pPathInfo->ObstacleIgnore;

		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;

		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistance		= pPathInfo->OptDistance;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptDistanceAfter	= pPathInfo->OptDistanceAfter;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptUBGRegion 		= (PATH_DIRECTION_INFO)pPathInfo->OptUBGRegion;
		pEXECUTE_DRIVING_INFO->BasicInfo.OptObstacleRegion	= (PATH_OBS_DIRECTION_INFO)pPathInfo->OptObstacleRegion;
#else
		pEXECUTE_DRIVING_INFO->BasicInfo.Direction			= (PATH_DIRECTION_INFO)(pPathInfo->PathDirection);
		pEXECUTE_DRIVING_INFO->BasicInfo.PreSteering		= PATH_STEERING_NONE;
		pEXECUTE_DRIVING_INFO->BasicInfo.nPBSIgnoreFlag		= pPathInfo->ObstacleIgnore;
		pEXECUTE_DRIVING_INFO->BasicInfo.DivInfo 			= pPathInfo->RailBranch;
#endif
		memmove(&(pEXECUTE_DRIVING_INFO->AddInfo), &AdditionalInfo, sizeof(EXECUTE_DRIVING_ADD_INFO));


		pResultAddList->insertBack((void*)pEXECUTE_DRIVING_INFO);
	}


	ADD_LOG("End Count: %d", m_EndNodeCnt);

	return bSuccess;
}
//AOHC-302 �ʱ�� ������ �ڵ�Ž��
void PathSearch::Fn_VisitCheck(RouteSt* pRouteSt)
{
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	int cnt = pRouteSt->index;
	int VisitCnt = 0;
	int pre_node, next_node;

	for(int i = 0; i<cnt-1; i++)
	{
		pre_node = pRouteSt->NodeArray[i];
		next_node = pRouteSt->NodeArray[i+1];

		if(VisitLinkCheckArray[pre_node][0] == 0)
		{
			 VisitCnt++;
			 VisitLinkCheckArray[pre_node][0] = next_node;
		}
		else
		{
			if(VisitLinkCheckArray[pre_node][0] != next_node)
			{
				if(VisitLinkCheckArray[pre_node][1] == 0)
				{
					VisitCnt++;
					VisitLinkCheckArray[pre_node][1] = next_node;
				}
			}
		}
	}
	pRouteSt->VisitCnt = VisitCnt;
}

//AOHC-302 �ʱ�� ������ �ڵ�Ž��
bool PathSearch::LaneCutLoad()
{
	std::string tmp;
	std::ifstream tmpFile("DataFiles\\LaneCutList.ini");

	int PreNodeID = 0;
	int PreNode = 0;
	int NextNode = 0;

	LaneCutSt* pLanCutSt;
	LaneCutList.clear();

	while(LaneCutList.getCount()>0)
	{
		pLanCutSt = (LaneCutSt*)LaneCutList.popFront();
		delete pLanCutSt;
	}

	if(tmpFile.is_open() == false)
		return false;

	//���پ� �д� �κ�
	while (std::getline(tmpFile, tmp))
	{
		char szLine[32];
		memset(szLine, 0, sizeof(szLine));
		sprintf(szLine, "%s", tmp.c_str());
		for(int i=0; szLine[i]!=0; i++)
		{
			if(szLine[i] == '_') szLine[i] = ' ';
		}
			int n = sscanf(szLine, "%d %d",
					&PreNode,
					&NextNode
					);

		pLanCutSt = new LaneCutSt;
		pLanCutSt->PreNode = PreNode;
		pLanCutSt->NextNode = NextNode;

		LaneCutList.insertBack((void*)pLanCutSt);

	}
	tmpFile.close();

	return true;
}

bool PathSearch::LaneCutLoad(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ PACKET_BODY)
{
	std::string tmp;

	int PreNodeID = 0;
	int PreNode = 0;
	int NextNode = 0;
	int LaneCutArray[20][2];

	LaneCutSt* pLanCutSt;
	LaneCutList.clear();

	while(LaneCutList.getCount()>0)
	{
		pLanCutSt = (LaneCutSt*)LaneCutList.popFront();
		delete pLanCutSt;
	}

	memcpy(&LaneCutArray, &PACKET_BODY.LaneCutList, sizeof(PACKET_BODY.LaneCutList));

	//���پ� �д� �κ�
	for(int i = 0; i<20; i++)
	{
		NODE_INFO	*pNodeInfo	= NULL;
		PATH_INFO	*pPathInfo	= NULL;	  // Path ����

		bool bResult = false;
		PreNode = LaneCutArray[i][0];
		NextNode = LaneCutArray[i][1];

		if(PreNode == 0 && NextNode == 0)
			continue;
		else if(PreNode == 0 && NextNode != 0)
			return false;
		else if(PreNode!=0 && NextNode == 0)
			return false;

		if(	(pMap->FindNodeIndex(PreNode) == -1)
		||	(pMap->FindNodeIndex(NextNode)	==	-1)	)
		{
			ADD_LOG("No Map Info, PreNode: %d, Nextnode: %d", PreNode, NextNode);
			return false;
		}

		int NodeIndex = pMap->FindNodeIndex(PreNode);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				if(NextNode == pPathInfo->EndNID)
                	bResult = true;
			}
		}

		if(!bResult)
		{
			ADD_LOG("LaneCutList Information Abnormal, preNode: %d, NextNode: %d", PreNode, NextNode);
			return false;
		}

		pLanCutSt = new LaneCutSt;
		pLanCutSt->PreNode = PreNode;
		pLanCutSt->NextNode = NextNode;

		LaneCutList.insertBack((void*)pLanCutSt);
	}

	return true;

}
bool PathSearch::NodeDrivingTest(MAP *pMap, UINT FromNode, int nLinkSetting)
{
	//����/���п��� ����
	bool bSuccess = false;

	//while�� ī��Ʈ
	int nCnt = 0;

	//���۳��
	int nStartNode = FromNode;

	//������ Ȯ�ο� ����, ����Ʈ
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitLinkCnt = 0;
	int VisitNodeCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node ����
	PATH_INFO	*pPathInfo	= NULL;	  // Path ����
	LaneCutSt	*TempLanCut = NULL;   // LaneCut ����


	//�˰��� ���Ǵ� ����
	int nLinkCnt = 0;               //�ε��� �ϳ� ���� ���� ��ũ ����
	int nIndex = 0;                 //���� �ε�����
	int nIndexNode = 0;				//���� ���
	int nVisitLinkCnt = 0;          //���� Visit�� ��ũ ���� Cnt(�ߺ� ����)
	int nVisitLinkMaxCnt = 0;      	//�ִ� Visit�� ��ũ ���� Cnt
	int nVisitNodeCnt = 0;          //���� Visit�� ��� ���� Cnt(�ߺ� ����)
	int nVisitNodeMaxCnt = 0;      	//�ִ� Visit�� ��� ���� Cnt
	int NodeArray[500];
	int StopOverArray[100];

	int nListSize = 0;     			//����Ʈ�� ���� ������ ��
	std::list <DrivingTestSt> DrivingTestList;	//�˰��� ���Ǵ� ����Ʈ
	DrivingTestSt DrivingTestTemp;				//����Ʈ�� ���Ǵ� ����ü

	//Initailization
	ZeroMemory(&VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	ZeroMemory(&VisitNodeCheckArray, sizeof(VisitNodeCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad())
	{
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//���۳�� �������� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)  )
	{
		ADD_LOG("No Map Info, Start: %d", nStartNode);
		return false;
	}

	//	���� ��� ���� ���� ��� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop�� ������ �� ��� Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	VisitNodeCnt = 1;
	VisitNodeCheckArray[nStartNode] = 1;

	while(idx.getCount() != 0)
	{
		//ī��Ʈ �̻� �� ����
		if(nCnt > 500)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode == nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;
		int preVisitLinkCnt = VisitLinkCnt;

		for(int i = 0; i<nCntChk; i++)
		{
			if(VisitLinkCheckArray[Node][i] == 0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitLinkCnt++;
			}

			if(nTempNextNode[i]!=-1)
			{
				idx.insertBack(nTempNextNode[i]);
				if(VisitNodeCheckArray[nTempNextNode[i]] == 0)
				{
					VisitNodeCheckArray[nTempNextNode[i]] = 1;
					VisitNodeCnt++;
				}
			}

		}
		if(preVisitLinkCnt == VisitLinkCnt)
			nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//���༺������(node)
	bool bNodeCheck = false;
	bool bAllCheck = false;

	//����Ʈ �ʱ�ȭ
	DrivingTestList.clear();

	//ó�� ����Ʈ�� ���� ������ ����
	DrivingTestTemp.index = 1;
	DrivingTestTemp.indexNode = nStartNode;
	DrivingTestTemp.VisitLinkCnt = 0;
	DrivingTestTemp.VisitNodeCnt = 0;
	ZeroMemory(&DrivingTestTemp.NodeArray, sizeof(DrivingTestTemp.NodeArray));
	ZeroMemory(&DrivingTestTemp.StopOverArray, sizeof(DrivingTestTemp.StopOverArray));

	DrivingTestTemp.NodeArray[0] = nStartNode;
	DrivingTestTemp.StopOverArray[0] = 0;

	//����Ʈ iteratior, ���� ����Ʈ ����
	nCnt = 0;
	list<DrivingTestSt>::iterator iterDrivingTestSt;
	DrivingTestList.push_back(DrivingTestTemp);


	//�˰���  ����
	while(!DrivingTestList.empty())
	{
		//����Ʈ�� �� �յ����͸� POP..
		nListSize = DrivingTestList.size();
		iterDrivingTestSt = DrivingTestList.begin();
		nIndex = iterDrivingTestSt->index;
		nIndexNode = iterDrivingTestSt->indexNode;
		nVisitLinkCnt = iterDrivingTestSt->VisitLinkCnt;
		nVisitNodeCnt = iterDrivingTestSt->VisitNodeCnt;

		memset(&NodeArray, 0, sizeof(NodeArray));
		memset(&StopOverArray, 0, sizeof(StopOverArray));
		memmove(&NodeArray,  iterDrivingTestSt->NodeArray, sizeof(iterDrivingTestSt->NodeArray));
		memmove(&StopOverArray,  iterDrivingTestSt->StopOverArray, sizeof(iterDrivingTestSt->StopOverArray));

		DrivingTestList.pop_front();


		//���üũ��.. ����ȸ�� �ּ�ȭ
		if(!bNodeCheck)
		{
			if(nVisitNodeMaxCnt <= nVisitNodeCnt)
			{
				nVisitNodeMaxCnt = nVisitNodeCnt;
			}
			else
			{
				continue;
			}
		}

		//��������1 - ���Ž��(�������) �Ϸ�
		if(VisitNodeCnt == nVisitNodeCnt && !bNodeCheck)
		{
			bNodeCheck = true;
			DrivingTestList.clear();
		}
		//��������2 - ���Ž�� �Ϸ� ��, ������������ ����
		else if(bNodeCheck && !bAllCheck)
		{
			//��������2 - ������������ ���ͿϷ�
			if(nIndexNode == nStartNode)
				bAllCheck = true;

		}
		//��������, Index�� 250 �̻��� ��, ����
		else if(nIndex >= 500 || nVisitNodeCnt >= 100)
		{
			bSuccess = false;
			break;

		}

		//Ž���� ��� ����Ϸ��������  Ȯ��.
		if(bAllCheck)
		{
			FILE* fp;
			fp = fopen("DataFiles\\NavigationRoute.txt", "w");

			if(fp == NULL) return false;
			int S_Index = 0;

			int n = 0;
			for(int i = 0; i<nIndex; i++)
			{
                n++;
				if(i!= 0 && NodeArray[i] == StopOverArray[S_Index] && n>nLinkSetting-1)
				{
					fprintf(fp, "%d\n", NodeArray[i]);
					S_Index++;
					n = 0;
					if(i == nIndex-1)
						break;
				}

				fprintf(fp, "%d ", NodeArray[i]);
			}
			fclose(fp);
			bSuccess = true;
			break;
		}


		// �� ���� ��� �Ʒ� Ž������ ����.


		//������ üũ
		int NodeIndex = pMap->FindNodeIndex(nIndexNode);
		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i<2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;
			}
			else
			{
				nTempNextNode[i] = -1;
			}
		}
		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
			if(TempLanCut->PreNode == nIndexNode)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode ==  nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
				else
				{
					break;
				}
			}
		}

		//��� �� üũ
		int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;


		//��� ����ŭ ����Ʈ�� �״´� -> �ٽ� ����
		for(int i = 0; i < nCntChk; i++)
		{
			int nextNode = nTempNextNode[i];
			memset(&DrivingTestTemp, 0, sizeof(DrivingTestTemp));

			//������ ����
			DrivingTestTemp.index = nIndex;
			DrivingTestTemp.VisitLinkCnt = nVisitLinkCnt;
			DrivingTestTemp.VisitNodeCnt = nVisitNodeCnt;
			memcpy(&DrivingTestTemp.NodeArray, &NodeArray ,sizeof(DrivingTestTemp.NodeArray));
			memcpy(&DrivingTestTemp.StopOverArray, &StopOverArray ,sizeof(DrivingTestTemp.StopOverArray));

			DrivingTestTemp.indexNode = nTempNextNode[i];
			DrivingTestTemp.NodeArray[DrivingTestTemp.index++] = nextNode;


			if(!bNodeCheck)
			{
				//�� �������� ��ũ������ ���� �̻��� ��, ���üũ ����
				VisitNodeCheck(&DrivingTestTemp,nLinkSetting);

			}


			//����Ʈ�� �߰�
			DrivingTestList.push_back(DrivingTestTemp);
		}

		nCnt++;
	}

	//����Ʈ �ʱ�ȭ
	while(!DrivingTestList.empty())
	{
		DrivingTestList.pop_front();
	}
	DrivingTestList.clear();
	LaneCutList.clear();

	return bSuccess;
}
bool PathSearch::NodeDrivingTest(MAP *pMap, PROTOCOL_BODY_AUTO_DRIVING_TEST_ROUTE_REQ PACKET_BODY)
{

//����/���п��� ����
	bool bSuccess = false;

	//while�� ī��Ʈ
	int nCnt = 0;

	//�ʿ��� �ֿ����� �Ľ�
	int nStartNode = PACKET_BODY.StartID;
	int nLinkSetting = PACKET_BODY.LinkCount;

	//������ Ȯ�ο� ����, ����Ʈ
	int nTempPreNode  = 0;
	int nTempNextNode[2] = {0,};
	int VisitLinkCnt = 0;
	int VisitNodeCnt = 0;
	int nCrossRoad = 0;

	List <int> idx;
	NODE_INFO	*pNodeInfo	= NULL;	  // Node ����
	PATH_INFO	*pPathInfo	= NULL;	  // Path ����
	LaneCutSt	*TempLanCut = NULL;   // LaneCut ����

	//�˰��� ���Ǵ� ����
	int nLinkCnt = 0;               //�ε��� �ϳ� ���� ���� ��ũ ����
	int nIndex = 0;                 //���� �ε�����
	int nIndexNode = 0;				//���� ���
	int nVisitLinkCnt = 0;          //���� Visit�� ��ũ ���� Cnt(�ߺ� ����)
	int nVisitLinkMaxCnt = 0;      	//�ִ� Visit�� ��ũ ���� Cnt
	int nVisitNodeCnt = 0;          //���� Visit�� ��� ���� Cnt(�ߺ� ����)
	int nVisitNodeMaxCnt = 0;      	//�ִ� Visit�� ��� ���� Cnt
	int NodeArray[500];
	int StopOverArray[100];

	int nListSize = 0;     						//����Ʈ�� ���� ������ ��
	std::list <DrivingTestSt> DrivingTestList;	//�˰��� ���Ǵ� ����Ʈ
	DrivingTestSt DrivingTestTemp;				//����Ʈ�� ���Ǵ� ����ü

	//Initailization
	ZeroMemory(&VisitLinkCheckArray, sizeof(VisitLinkCheckArray));
	ZeroMemory(&VisitNodeCheckArray, sizeof(VisitNodeCheckArray));

	//LaneCutLoad
	if(!LaneCutLoad(pMap, PACKET_BODY))
	{
		ADD_LOG("LaneCutList Abnormal");
		return false;
	}

	//1. Route Search Ready : Information Check, Interlock

	//���۳�� �������� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	if(	(pMap->FindNodeIndex(nStartNode) ==	-1)  )
	{
		ADD_LOG("No Map Info, Start: %d", nStartNode);
		return false;
	}

	//	���� ��� ���� ���� ��� ������ �ִ��� Ȯ��, ������ ���� �� Fail
	int nNodeIndex = pMap->FindNodeIndex(nStartNode);
	pNodeInfo = &(pMap->m_NodeArray[nNodeIndex]);
	if(pNodeInfo->NextPathIndex[0] == INDEX_NONE && pNodeInfo->NextPathIndex[1] == INDEX_NONE)
	{
		ADD_LOG("Next Node Index None");
		return false;
	}


	//2. Route Teaching : Closed loop�� ������ �� ��� Teaching
	idx.clear();
	idx.insertBack(nStartNode);
	VisitNodeCnt = 1;
	VisitNodeCheckArray[nStartNode] = 1;

	while(idx.getCount() != 0)
	{
		//ī��Ʈ �̻� �� ����
		if(nCnt > 500)
		{
			break;
		}
		int Node = idx.popFront();
		int NodeIndex = pMap->FindNodeIndex(Node);

		pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

		for(int i = 0; i < 2; i++)
		{
			int nPathIndex = pNodeInfo->NextPathIndex[i];

			if(nPathIndex != -1)
			{
				pPathInfo = &(pMap->m_PathArray[nPathIndex]);
				nTempNextNode[i] = pPathInfo->EndNID;

			}
			else
			{
				nTempNextNode[i] = -1;
            }
		}

		int nCount = LaneCutList.getCount();
		for(int i = 0; i < nCount; i++)
		{
			TempLanCut =(LaneCutSt*) LaneCutList.refer(i);
			if(TempLanCut->PreNode == Node)
			{
				if(TempLanCut->NextNode ==  nTempNextNode[0])
				{
					nTempNextNode[0] = nTempNextNode[1];
					nTempNextNode[1] = -1;
				}
				else if(TempLanCut->NextNode == nTempNextNode[1])
				{
					nTempNextNode[1] = -1;
				}
			}

		}

		int nCntChk = nTempNextNode[1] < 0 ? 1:2;
		int preVisitLinkCnt = VisitLinkCnt;

		for(int i = 0; i<nCntChk; i++)
		{
			if(VisitLinkCheckArray[Node][i] == 0)
			{
				ADD_LOG("Visit:%d-%d ", Node,nTempNextNode[i]);
				VisitLinkCheckArray[Node][i] = 1;
				VisitLinkCnt++;
			}

			if(nTempNextNode[i]!=-1)
			{
				idx.insertBack(nTempNextNode[i]);
				if(VisitNodeCheckArray[nTempNextNode[i]] == 0)
				{
					VisitNodeCheckArray[nTempNextNode[i]] = 1;
					VisitNodeCnt++;
				}
			}

		}
		if(preVisitLinkCnt == VisitLinkCnt)
			nCnt++;
	}
	idx.clear();


	//3. Algorithm

	//���༺������(node)
	bool bNodeCheck = false;
	bool bAllCheck = false;

	//����Ʈ �ʱ�ȭ
	DrivingTestList.clear();

	//ó�� ����Ʈ�� ���� ������ ����
	DrivingTestTemp.index = 1;
	DrivingTestTemp.indexNode = nStartNode;
	DrivingTestTemp.VisitLinkCnt = 0;
	DrivingTestTemp.VisitNodeCnt = 0;
	ZeroMemory(&DrivingTestTemp.NodeArray, sizeof(DrivingTestTemp.NodeArray));
	ZeroMemory(&DrivingTestTemp.StopOverArray, sizeof(DrivingTestTemp.StopOverArray));

	DrivingTestTemp.NodeArray[0] = nStartNode;
	DrivingTestTemp.StopOverArray[0] = 0;

	//����Ʈ iteratior, ���� ����Ʈ ����
	nCnt = 0;
	list<DrivingTestSt>::iterator iterDrivingTestSt;
	DrivingTestList.push_back(DrivingTestTemp);


	try
	{
		dwPathSearchTime = timeGetTime();
		//�˰���  ����
		while(!DrivingTestList.empty())
		{
			//�ð��ʰ� Ȯ�� 2.5�� �̻�� Fail ó��
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwPathSearchTime) > 2500)
					break;

			//����Ʈ�� ����� 20�� �̻��� ��� Fail ó�� 60�� �̻��� ��� SIZE FULL
			nListSize = DrivingTestList.size();
			if(nListSize > 200000)
				break;

			//����Ʈ�� �� �յ����͸� POP..
			iterDrivingTestSt = DrivingTestList.begin();
			nIndex = iterDrivingTestSt->index;
			nIndexNode = iterDrivingTestSt->indexNode;
			nVisitLinkCnt = iterDrivingTestSt->VisitLinkCnt;
			nVisitNodeCnt = iterDrivingTestSt->VisitNodeCnt;

			memset(&NodeArray, 0, sizeof(NodeArray));
			memset(&StopOverArray, 0, sizeof(StopOverArray));
			memmove(&NodeArray,  iterDrivingTestSt->NodeArray, sizeof(iterDrivingTestSt->NodeArray));
			memmove(&StopOverArray,  iterDrivingTestSt->StopOverArray, sizeof(iterDrivingTestSt->StopOverArray));

			DrivingTestList.pop_front();



			//���üũ��.. ����ȸ�� �ּ�ȭ
			if(!bNodeCheck)
			{
				if(nVisitNodeMaxCnt <= nVisitNodeCnt)
				{
					nVisitNodeMaxCnt = nVisitNodeCnt;
				}
				else
				{
					continue;
				}
			}

			//��������1 - ���Ž��(�������) �Ϸ�
			if(VisitNodeCnt == nVisitNodeCnt && !bNodeCheck)
			{
				bNodeCheck = true;
				if(nIndexNode == nStartNode)
					bAllCheck = true;
				DrivingTestList.clear();
			}
			//��������2 - ���Ž�� �Ϸ� ��, ������������ ����
			else if(bNodeCheck && !bAllCheck)
			{
				//��������2 - ������������ ���ͿϷ�
				if(nIndexNode == nStartNode)
					bAllCheck = true;

			}
			//��������, Index�� 250 �̻��� ��, ����
			else if(nIndex >= 500 || nVisitNodeCnt >= 100)
			{
				bSuccess = false;
				break;

			}

			//Ž���� ��� ����Ϸ��������  Ȯ��.
			if(bAllCheck)
			{
				FILE* fp;
				fp = fopen("DataFiles\\NavigationRoute.txt", "w");

				if(fp == NULL) return false;
				int S_Index = 0;

				int n = 0;
				int PreNode = 0;
				int NodeIndex = 0;
				double DistSum = 0;
				for(int i = 0; i<nIndex; i++)
				{
					// Link �Ÿ� ���
					if(PreNode!=0)
					{
						NodeIndex = pMap->FindNodeIndex(PreNode);
						pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

						//�Ÿ����
						for(int j = 0; j<2; j++)
						{
							int nPathIndex = pNodeInfo->NextPathIndex[j];
							if(nPathIndex != -1)
							{
								pPathInfo = &(pMap->m_PathArray[nPathIndex]);
								if(NodeArray[i] == pPathInfo->EndNID)
								{
									DistSum += (double)pPathInfo->Distance;
								}
							}
						}
					}
					n++;
					//������ 1���� �ڸ��� �κ�
					if(i!= 0 && NodeArray[i] == StopOverArray[S_Index] && n>nLinkSetting-1)
					{
						fprintf(fp, "%d %.0f\n", NodeArray[i], DistSum);
						ADD_LOG("EndID:%d, DistSum:%.0f ", NodeArray[i],DistSum);
						S_Index++;
						PreNode = 0;
						DistSum = 0;
						n = 0;
					}

					// ��� 1���� ��� �κ�
					if(i < nIndex-1)
					{
					   fprintf(fp, "%d ", NodeArray[i]);
					   PreNode = NodeArray[i];
					}
					else
					{
						fprintf(fp, "%d %.0f\n", NodeArray[i], DistSum);
						ADD_LOG("EndID:%d, DistSum:%.0f ", NodeArray[i],DistSum);
					}


				}
				fclose(fp);
				bSuccess = true;
				break;
			}


			// �� ���� ��� �Ʒ� Ž������ ����.


			//������ üũ
			int NodeIndex = pMap->FindNodeIndex(nIndexNode);
			int BlockCheck = 0;
			pNodeInfo =  &(pMap->m_NodeArray[NodeIndex]);

			for(int i = 0; i<2; i++)
			{
				int nPathIndex = pNodeInfo->NextPathIndex[i];

				if(nPathIndex != -1)
				{
					pPathInfo = &(pMap->m_PathArray[nPathIndex]);
					nTempNextNode[i] = pPathInfo->EndNID;
				}
				else
				{
					BlockCheck++;
					nTempNextNode[i] = -1;
				}
			}

			//�����ִ� ���
			if(BlockCheck >=2)
				break;

			int nCount = LaneCutList.getCount();
			for(int i = 0; i < nCount; i++)
			{
				LaneCutSt* TempLanCut = (LaneCutSt*)LaneCutList.refer(i);
				if(TempLanCut->PreNode == nIndexNode)
				{
					if(TempLanCut->NextNode ==  nTempNextNode[0])
					{
						nTempNextNode[0] = nTempNextNode[1];
						nTempNextNode[1] = -1;
					}
					else if(TempLanCut->NextNode ==  nTempNextNode[1])
					{
						nTempNextNode[1] = -1;
					}
					else
					{
						break;
					}
				}
			}

			//��� �� üũ
			int nCntChk = nTempNextNode[1] < 0 ? 1 : 2;


			//��� ����ŭ ����Ʈ�� �״´� -> �ٽ� ����
			for(int i = 0; i < nCntChk; i++)
			{
				int nextNode = nTempNextNode[i];
				memset(&DrivingTestTemp, 0, sizeof(DrivingTestTemp));


				//������ ����
					DrivingTestTemp.index = nIndex;
					DrivingTestTemp.VisitLinkCnt = nVisitLinkCnt;
					DrivingTestTemp.VisitNodeCnt = nVisitNodeCnt;
					memcpy(&DrivingTestTemp.NodeArray, &NodeArray ,sizeof(DrivingTestTemp.NodeArray));
					memcpy(&DrivingTestTemp.StopOverArray, &StopOverArray ,sizeof(DrivingTestTemp.StopOverArray));

					DrivingTestTemp.indexNode = nTempNextNode[i];
					DrivingTestTemp.NodeArray[DrivingTestTemp.index++] = nextNode;




				if(!bNodeCheck)
				{
					//�� �������� ��ũ������ ���� �̻��� ��, ���üũ ����
					VisitNodeCheck(&DrivingTestTemp,nLinkSetting);

				}

				//����Ʈ�� �߰�
				DrivingTestList.push_back(DrivingTestTemp);
			}

			nCnt++;
		}
	}
	catch(...)
	{
		ADD_LOG("Abnormal Case");
	}

	//����Ʈ �ʱ�ȭ
	while(!DrivingTestList.empty())
	{
		DrivingTestList.pop_front();
	}
	DrivingTestList.clear();
	LaneCutList.clear();

	return bSuccess;
}

bool PathSearch::VisitLinkCheck(DrivingTestSt* pDrivingTestSt, int VisitLinkCnt)
{
	ZeroMemory(VisitLinkCheckArray, sizeof(VisitLinkCheckArray));

	int cnt = pDrivingTestSt->index;
	int VisitCnt = 0;
	int pre_node, next_node;
	bool bSuccess = false;

	for(int i = 0; i<cnt-1; i++)
	{
		pre_node = pDrivingTestSt->NodeArray[i];
		next_node = pDrivingTestSt->NodeArray[i+1];

		if(VisitLinkCheckArray[pre_node][0] == 0)
		{
			 VisitCnt++;
			 VisitLinkCheckArray[pre_node][0] = next_node;
		}
		else
		{
			if(VisitLinkCheckArray[pre_node][0] != next_node)
			{
				if(VisitLinkCheckArray[pre_node][1] == 0)
				{
					VisitCnt++;
					VisitLinkCheckArray[pre_node][1] = next_node;
				}
			}
		}
	}

	pDrivingTestSt->VisitLinkCnt = VisitCnt;

	if(VisitCnt == VisitLinkCnt)
		bSuccess = true;

	return	bSuccess;
}


bool PathSearch::VisitNodeCheck(DrivingTestSt* pDrivingTestSt, int nLinkSetting)
{
	bool bSuccess = true;
	int VisitCnt = 0;
	int CurrentNode;
	int nextNode = pDrivingTestSt->indexNode;
	int cnt = pDrivingTestSt->VisitNodeCnt;
	int index = pDrivingTestSt->index - 1;
	int StopIndex =  pDrivingTestSt->VisitNodeCnt-1;
	int CntCheck = 0;

	for(int i = 0; i<cnt; i++)
	{
		CurrentNode = pDrivingTestSt->StopOverArray[i];

		if(CurrentNode == nextNode)
		{
			bSuccess = false;

			break;
		}
	}

	if(bSuccess)
	{
    	if(StopIndex >= 0)
		{
			for(int i = index; i>=0; i--)
			{
				CntCheck++;
				if(pDrivingTestSt->NodeArray[i] == 	pDrivingTestSt->StopOverArray[StopIndex])
				{
					break;
				}
			}
		}
		else
		{
			CntCheck = index + 1;
		}
	}

	if(bSuccess && CntCheck > nLinkSetting)
	{
		pDrivingTestSt->StopOverArray[pDrivingTestSt->VisitNodeCnt++] = nextNode;
	}


	return bSuccess;
}



