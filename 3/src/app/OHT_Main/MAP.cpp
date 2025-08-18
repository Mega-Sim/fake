/**
@file    MAP.cpp
@version OHT 7.0
@brief   MAP�� ���õ� Class ��� �Լ��� ������ cpp ����
*/

#pragma hdrstop

#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include "MAP.h"
#include "OHTMain.h"
#include "ErrorCode.h"

#define ADD_LOG_MAPMAKE_STD(...)        WRITE_LOG_UNIT(pLogMapMakeSTD, __VA_ARGS__)
LogUnit *pLogMapMakeSTD; ///< Map Make STD���� �α� ����

using namespace std;
//---------------------------------------------------------------------------

/**
@brief   MAP Class default ������ 

         ����, ������ ���� �ʱ�ȭ
@author  zzang9un
@date    2012.11.09
*/
MAP::MAP()
{
    m_PathNum = 0;
    m_NodeNum = 0;

	m_FlowcontrolCheckResult = NO_ERR;
	
    m_PathArray = NULL;
    m_NodeArray = NULL;
	m_FlowCheckArray = NULL;
    
    // PathMap, NodeMap �ʱ�ȭ
    m_MakeRightNodeMap.clear();
    m_NodeMap.clear();
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	m_CIDStartMap_1.clear();
	m_CIDStartMap_2.clear();

	m_FlowControl.clear();

	tNodeDisalbeList = new TStringList;
	tPassBoxNodeList = new TStringList;

	ReadNodeDisableFile();
	ReadPassBoxNodeFile();

	pLogMapMakeSTD = getLogUnit("D:\\log\\MapMakeSTD.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
}
//---------------------------------------------------------------------------

/**
@brief   MAP Class �Ҹ���.

         ���� �Ҵ�� �迭, ������ ������ delete�Ѵ�.
@author  
@date    2012.11.09
@note    
*/
MAP::~MAP()
{
    if(m_PathArray != NULL)
        delete [] m_PathArray;

    if(m_NodeArray != NULL)
        delete [] m_NodeArray;

	if(m_FlowCheckArray != NULL)
		delete [] m_FlowCheckArray;

    m_MakeRightNodeMap.clear();
    m_NodeMap.clear(); 
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	
	if(tNodeDisalbeList != NULL)
		delete tNodeDisalbeList;

	if(tPassBoxNodeList != NULL)
		delete tPassBoxNodeList;
	
}
//---------------------------------------------------------------------------

/**
@brief   Node ����ü�� Node ID�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author  zzang9un
@date    2012.11.09
*/
void MAP::MakeNodeMap()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
        return;

	m_NodeMap.clear(); // �ű� ���Ͽ� ��尡 �� ������� ������ �ȵ�. update�� m_NodeMap.clear �ʿ�. 
    for(int i = 0; i < m_NodeNum; i++)
    {
        // Key�� Node ID, Value�� �迭 Index
        m_NodeMap[m_NodeArray[i].NID] = i;
    }   
}
//---------------------------------------------------------------------------

/**
@brief   Node ����ü�� Right Node ID�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
         Collision �������� m_PathArray�� Offset ���� ã������.
@author  KJD
@date    2016.06.01
*/
void MAP::MakeRightNodeMapFromPathArray()
{
    if((m_NodeNum == 0) || (m_NodeArray == NULL))
        return;

    for(int i = 0; i < (m_PathNum-m_NodeNum); i++)
    {
        // Key�� Right Node ID, Value�� �迭 Index
		m_MakeRightNodeMap[m_PathArray[m_NodeNum+i].EndNID] = i;
    }   
}

//---------------------------------------------------------------------------
/**
@brief   Teaching ����ü�� Station ID�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author  KJD
@date    2016.06.08
*/
void MAP::MakeTeachingMap(NODE_INFO_STD_TEACHING *m_NodeArray_Teach, UINT DataNum)
{
    if(DataNum == 0)
        return;

    for(int i = 0; i < DataNum; i++)
    {
        // Key�� Station ID, Value�� �迭 Index
        m_TeachingMap[m_NodeArray_Teach[i].StationID] = i;
    }   
}
//---------------------------------------------------------------------------
/**
@brief   CID ����ü�� Reset, Stop node ID�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author  KJD
@date    2016.07.28
*/
#if(OHT_NAME != OHT_NAME_P4_MASK)
void MAP::MakeCIDMap(NODE_INFO_STD_CID * m_NodeArray_CID, UINT DataNum)
{
    if(DataNum == 0)
        return;

    for(int i = 0; i < DataNum; i++)
    {
        // Key�� CID Reset ID, Value�� �迭 Index
        m_CIDResetMap[m_NodeArray_CID[i].ResetNode] = i;
        m_CIDStopMap_1[m_NodeArray_CID[i].StopNode1] = i;
		m_CIDStopMap_2[m_NodeArray_CID[i].StopNode2] = i;

		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_1] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_2] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_3] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_4] = i;
		m_CIDStartMap_1[m_NodeArray_CID[i].CommStartNode1_5] = i;

		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_1] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_2] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_3] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_4] = i;
		m_CIDStartMap_2[m_NodeArray_CID[i].CommStartNode2_5] = i;

	}
}
#endif
/**
@brief   Flowcontrol ����ü�� Reset, Stop node ID�� map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author   puting
@date    2016.07.28
*/
void MAP::MakeFlowControlMap(NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum)
{
	map<int, int>::iterator FindIter;
    bool nbreak = false;
	int nTempStopNode = 0;
	int StopResetCNT = 0;
	unsigned long long StopResetNode = 0;

	if(DataNum == 0)
		return;

	m_FlowControl_StopReset.clear();

	for(int i = 0; i < DataNum; i++)
	{
		// Key�� CID Reset ID, Value�� �迭 Index
        nTempStopNode =  m_FlowArray[i].StopNodeID;
		//4�� Stop Node �ߺ��Ȱſ� ���� ó�� �κ�.
		for(int j = 0; j <4; j++)
		{
			nTempStopNode = nTempStopNode + 1000000;

			FindIter = m_FlowControl.find(nTempStopNode);

            if(FindIter == m_FlowControl.end())
			{
				// Node�� �� ã�� ���
				m_FlowControl[nTempStopNode] = i;
				break;
			}
		}
		//Flow Control ��� �շ��� �˻縦 ���� Map �����̳� ����
		StopResetNode = (unsigned long long)m_FlowArray[i].StopNodeID << 32 | (unsigned long long)m_FlowArray[i].ResetNodeMain;
		m_FlowControl_StopReset[StopResetNode]=StopResetCNT++;
//		if(m_FlowArray[i].StopNodeID == 1010)
//		ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset %6d, %6d, %6d",m_FlowArray[i].StopNodeID, m_FlowArray[i].ResetNodeMain, StopResetCNT);
		
		if(m_FlowArray[i].ResetNodeSub != 0)
		{
			StopResetNode = (unsigned long long)m_FlowArray[i].StopNodeID << 32 | (unsigned long long)m_FlowArray[i].ResetNodeSub;
			m_FlowControl_StopReset[StopResetNode]=StopResetCNT++;
//			if(m_FlowArray[i].StopNodeID == 1010)
//			ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset %6d, %6d, %6d",m_FlowArray[i].StopNodeID, m_FlowArray[i].ResetNodeSub, StopResetCNT);
		}
		//sleep(10);
	}
	ADD_LOG_MAPMAKE_STD("m_FlowControl_StopReset StopResetCNT : %d",StopResetCNT);
}
//---------------------------------------------------------------------------
/**
@brief   �շ��� ����㰡 ���� �˻�
@author  KJD
@date    2016.09.01
*/
int MAP::CheckFlowControlData(unsigned long long StopResetNode)
{
    int ret = 0;
	ret = FindIndex_FlowControl_StopReset(StopResetNode);
	if(ret == INDEX_NONE)
	{
		return -1;
	}
	else
	{
		return NO_ERR;
	}
}
//---------------------------------------------------------------------------


/**
@brief   MAP Class ������ 

         Path�� Node ������ �̿��Ͽ� ���� �����Ѵ�.
@author  zzang9un
@date    2012.11.09
@param   PathNum : Path ����
@param   NodeNum : Node ����
*/
void MAP::InitMap(int PathNum, int NodeNum)
{
    m_PathNum = PathNum;
    m_NodeNum = NodeNum;
    
    // ���� �����ϱ� �� Null������ Ȯ�� �� ����
    if(m_PathArray != NULL)
    {
        // NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		delete [] m_PathArray;
		m_PathArray = new PATH_INFO[PathNum];
    }
    else
		m_PathArray = new PATH_INFO[PathNum];
    
    if(m_NodeArray != NULL)
    {
        // NULL�� �ƴ� ���� delete �Ŀ� ���� ����
        delete [] m_NodeArray;
        m_NodeArray = new NODE_INFO[NodeNum];
    }
    else
        m_NodeArray = new NODE_INFO[NodeNum];

	if(m_FlowCheckArray != NULL)
	{
		// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
		delete [] m_FlowCheckArray;
		m_FlowCheckArray = new FLOW_INFO[NodeNum];
	}
	else
		m_FlowCheckArray = new FLOW_INFO[NodeNum];


    m_MakeRightNodeMap.clear();
    m_NodeMap.clear();
	m_TeachingMap.clear();
	m_LookDownMap.clear();
	m_CIDResetMap.clear();
	m_CIDStopMap_1.clear();
	m_CIDStopMap_2.clear();
	m_CIDStartMap_1.clear();
	m_CIDStartMap_2.clear();

	m_FlowControl.clear();
   
}
//---------------------------------------------------------------------------

/**
@brief   map �����̳ʷ� ��ȯ��Ű�� �Լ�.
@author  zzang9un
@date    2012.11.09
*/
int MAP::MakeMap(MAP *pMapdata,
				NODE_INFO_STD_MAP * m_NodeArray_Map,
				UINT NodeNum_Map)
{
int ret = 0;
ADD_LOG_MAPMAKE_STD("MakeMap Step1");
	MakeNodeMap();
ADD_LOG_MAPMAKE_STD("MakeMap Step2");
	MakeRightNodeMapFromPathArray();
ADD_LOG_MAPMAKE_STD("MakeMap Step3");
	// Next Path�� Index�� ���� ���� �ִ� �Լ�
	ret = MakePathIndex(pMapdata,
						m_NodeArray_Map,
						NodeNum_Map);
ADD_LOG_MAPMAKE_STD("MakeMap Step4");
	return ret;
}
//---------------------------------------------------------------------------

/**
@brief   Node ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  zzang9un
@date    2012.11.09
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndex(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_NodeMap.find(NodeID);

    if(FindIter != m_NodeMap.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}

/**
@brief	 NextNode ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date	 2017.9.01
@param	 NodeID : ã���� �ϴ� NextNode ID
@return  NextNode ID�� ��Ī�Ǵ� �迭 index
@todo	 
*/
int MAP::FindNextIndex(int NodeID)
{
	map<int, int>::iterator FindIter;

	FindIter = m_NextNode.find(NodeID);

	if(FindIter != m_NextNode.end())
	{
		// Node�� ã�� ���
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
/**
@brief	 NextNode ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date	 2017.9.01
@param	 NodeID : ã���� �ϴ� NextNode ID
@return  NextNode ID�� ��Ī�Ǵ� �迭 index
@todo	 
*/
int MAP::FindIndex_FlowControl_StopReset(unsigned long long NodeID)
{
	map<unsigned long long, int>::iterator FindIter;

	FindIter = m_FlowControl_StopReset.find(NodeID);

	if(FindIter != m_FlowControl_StopReset.end())
	{
		// Node�� ã�� ���
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
/**
@brief	 NextNode ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date	 2017.9.01
@param	 NodeID : ã���� �ϴ� NextNode ID
@return  NextNode ID�� ��Ī�Ǵ� �迭 index
@todo	 

int MAP::FindIndex_RailMerge_StopReset(unsigned long long NodeID)
{
	map<unsigned long long, int>::iterator FindIter;

	FindIter = m_RailMerge_StopReset.find(NodeID);

	if(FindIter != m_RailMerge_StopReset.end())
	{
		// Node�� ã�� ���
		return FindIter->second;
	}	 
	else
	{
		return INDEX_NONE;
	}		 
}
*/



//---------------------------------------------------------------------------
/**
@brief   Right Node ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.06.01
@param   NodeID : ã���� �ϴ� Right Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindRightNodeIndex(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_MakeRightNodeMap.find(NodeID);

    if(FindIter != m_MakeRightNodeMap.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}

//---------------------------------------------------------------------------
/**
@brief   Station ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.06.08
@param   NodeID : ã���� �ϴ� Station ID
@return  Station ID�� ��Ī�Ǵ� �迭 index

int MAP::FindStationIndexFromTeachingMap(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_TeachingMap.find(NodeID);

    if(FindIter != m_TeachingMap.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
*/
//---------------------------------------------------------------------------
/**
@brief   Station ID�� ��Ī�Ǵ� Path �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.06.08
@param   NodeID : ã���� �ϴ� Station ID
@return  Station ID�� ��Ī�Ǵ� �迭 index

int MAP::FindStationIndexFromLookDownMap(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_LookDownMap.find(NodeID);

    if(FindIter != m_LookDownMap.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
*/


//---------------------------------------------------------------------------

/**
@brief   Reset Node ID�� ��Ī�Ǵ� CID �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.07.28
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromCIDReset(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDResetMap.find(NodeID);

    if(FindIter != m_CIDResetMap.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------
/**
@brief   StopNode1 ID�� ��Ī�Ǵ� CID �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.07.28
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromCIDStop_1(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStopMap_1.find(NodeID);

    if(FindIter != m_CIDStopMap_1.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------
/**
@brief   StopNode2 ID�� ��Ī�Ǵ� CID �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.07.28
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromCIDStop_2(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStopMap_2.find(NodeID);

    if(FindIter != m_CIDStopMap_2.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }    
    else
    {
        return INDEX_NONE;
    }        
}
//---------------------------------------------------------------------------



/**
@brief   Node�� Next Path �ε����� Make�ϴ� �Լ�
         MAPDATA�� Next�� �����Ͽ� �ε��� �ϴ� ������ ����
         MAPDATA�� �θ� ��� �߰��� ������ ������� �־� �߰��ϱ� ����.
@author  KJD
@date    2016.05.31
@return  ������ ��� 0, ������ �߻��� ��� ���� �ڵ� ����
*/
#if 1
int MAP::MakePathIndex(MAP *pMapdata, NODE_INFO_STD_MAP * m_NodeArray_Map, UINT NodeNum_Map)
{
	int tmpResetLH;
	int tmpResetRH;
	int RailMergeCNT=0;
	int ret = 0;
	int cnt = 0;
	int FlowCnt;
	unsigned long long StopResetNode = 0; 

	m_NextNode.clear();
	m_FlowcontrolCheckResult = NO_ERR;

	UINT tmpArray[1024] = {0,};

	// MTL STOP �� �������� ������ �迭�� ����
	// MTL ������� �迭 �� Node�� �������� �����ϴٸ� Flage true
	for(int i=0 ; i < NodeNum_Map ; i++ )
	{
		m_FlowCheckArray[i].Flowflag = false;     //�ʱ�ȭ

		if((m_NodeArray_Map[i].NodeType == MTL_STOP_LEFT) || (m_NodeArray_Map[i].NodeType == MTL_STOP_RIGTH))
		{
			tmpArray[cnt++] = m_NodeArray_Map[i].LeftNodeID;
		}
	}

	for(int j=0 ; j < cnt ; j++ )
	{
		FlowCnt = 0;

		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			if(FlowCnt >= 2)
			{
				break;
			}

			if((m_NodeArray_Map[i].LeftNodeID == tmpArray[j]) && (m_FlowCheckArray[i].Flowflag == false))
			{
				m_FlowCheckArray[i].Flowflag = true;
				FlowCnt++;
			}
		}
	}

	for(int i=0 ; i < cnt ; i++ )
	{
		ADD_LOG_MAPMAKE_STD("MTL STOP LeftNode List : %d",tmpArray[i]);
	}


	for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//if( m_NodeArray_Map[i]->LeftNodeID != 0 ) // ��尡 �����. �ǹ̻� �־�� �ϳ� �׻� ���̹Ƿ� ���Ǻ� ����.
			//m_NodeArray_Map[i]->LeftNodeID�� ��δ�
			//NodeIndex�� PathIndex�� �����ϹǷ� NodeIndex���� �迭������ �˾ƿ�.

			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Map[i].NodeID);

           	if( m_NodeArray_Map[i].LeftNodeID != 0 ) // ��尡 �����.
				m_NodeArray[i].NextPathIndex[0] = tmp; // ���� Path Index
			else
                m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;

			if( m_NodeArray_Map[i].RightNodeID != 0 ) // ��尡 �����.
			{
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Map[i].RightNodeID);
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Map[i].RightNodeID )
				{
					m_NodeArray[i].NextPathIndex[1] = NodeNum_Map+tmp; //���� Path Index
				}
				else
				{
					m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
				}

			}
			else
			{
				m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
			}

			
			//����㰡 ���� ������ ���� ������ ����
			//�շ��� ã�� - NextNode�� �ߺ��� ��� 
			//������ ����
			if(m_NodeArray_Map[i].LeftNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetLH = FindNextIndex(m_NodeArray_Map[i].LeftNodeID);
				if(tmpResetLH == INDEX_NONE)// Path�� ���� ���
				{
					m_NextNode[m_NodeArray_Map[i].LeftNodeID] = i;
				}
				else// NextNode Map�� �̹� �����ϴ� ��� �շ������� ���.
				{
					//�շ��� ã�� �� Flowcontrol���� �˻�.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].LeftNodeID;//stop, reset �������� 8byte key ���
					//m_RailMerge_StopReset[StopResetNode++] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData1 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].LeftNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
			if(m_NodeArray_Map[i].RightNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetRH = FindNextIndex(m_NodeArray_Map[i].RightNodeID);
				if(tmpResetRH == INDEX_NONE)
				{
					m_NextNode[m_NodeArray_Map[i].RightNodeID]=NodeNum_Map+i;
				}
				else// NextNode Map�� �̹� �����ϴ� ��� �շ������� ���.
				{
					//�շ��� ã�� �� Flowcontrol���� �˻�.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].RightNodeID;//stop, reset �������� 8byte key ���
					//m_RailMerge_StopReset[StopResetNode] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData2 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].RightNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
		}
	m_NextNode.clear();
	for(int i=NodeNum_Map-1 ; i > 0 ; i-- )
		{			
			//����㰡 ���� ������ ���� ������ ����
			//�շ��� ã�� - NextNode�� �ߺ��� ��� 
			//������ ����
			if(m_NodeArray_Map[i].LeftNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetLH = FindNextIndex(m_NodeArray_Map[i].LeftNodeID);
				if(tmpResetLH == INDEX_NONE)// 
				{
					m_NextNode[m_NodeArray_Map[i].LeftNodeID] = i;
				}
				else// NextNode Map�� �̹� �����ϴ� ��� �շ������� ���.
				{
					//�շ��� ã�� �� Flowcontrol���� �˻�.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].LeftNodeID;//stop, reset �������� 8byte key ���
					//m_RailMerge_StopReset[StopResetNode++] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData1 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].LeftNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
			if(m_NodeArray_Map[i].RightNodeID != 0 && m_NodeArray_Map[i].NodeType != LIFTIN_NTYPE_LEFT && m_NodeArray_Map[i].NodeType !=LIFTIN_NTYPE_RIGHT && m_FlowCheckArray[i].Flowflag == false)
			{
				tmpResetRH = FindNextIndex(m_NodeArray_Map[i].RightNodeID);
				if(tmpResetRH == INDEX_NONE)
				{
					m_NextNode[m_NodeArray_Map[i].RightNodeID]=NodeNum_Map+i;
				}
				else// NextNode Map�� �̹� �����ϴ� ��� �շ������� ���.
				{
					//�շ��� ã�� �� Flowcontrol���� �˻�.
					StopResetNode = (unsigned long long)m_NodeArray_Map[i].NodeID<<32 | (unsigned long long)m_NodeArray_Map[i].RightNodeID;//stop, reset �������� 8byte key ���
					//m_RailMerge_StopReset[StopResetNode] = RailMergeCNT++;
					ret = CheckFlowControlData(StopResetNode);
					if(ret == -1)
					{
						ADD_LOG_MAPMAKE_STD("CheckFlowControlData2 err: %d, %d, %d",m_NodeArray_Map[i].NodeID,m_NodeArray_Map[i].RightNodeID, ret);
						m_FlowcontrolCheckResult = 0xFA;//fail
					}
				}
			}
		}

	return m_FlowcontrolCheckResult;
}


#else
/**
@brief   Node�� Pre, Next Path �ε����� Make�ϴ� �Լ�
@author  zzang9un
@date    2012.11.30
@return  ������ ��� 0, ������ �߻��� ��� ���� �ڵ� ����
*/

int MAP::MakePathIndex(NODE_INFO_STD_MAP * m_NodeArray_Map)
{
    bool bPreFind = false;
    bool bNextFind = false;
    int PreFindCnt = 0;
    int NextFindCnt = 0;
    
    for(int i = 0; i < m_NodeNum; i++)
    {
        for(int j = 0; j < m_PathNum; j++)
        {
			// Pre Path �˻�
			/*
            if(m_NodeArray[i].NID == m_PathArray[j].EndNID)
            {
                if(bPreFind)
                {
                    // ���� ó���� ���� �߰�
                    if(PreFindCnt >= 3)
                        return -1;
                    
                    // by zzang9un 2012.11.30 : �ѹ� �� �˻��� �� ���
                    m_NodeArray[i].PrePathIndex[PreFindCnt] = j;
                    PreFindCnt++;
                }
                else
                {
                    // by zzang9un 2012.11.30 : ���� �˻��� ���
                    m_NodeArray[i].PrePathIndex[PreFindCnt] = j;
                    PreFindCnt++;
                    bPreFind = true;
                }
            }
             */
            // Next Path �˻�
            if(m_NodeArray[i].NID == m_PathArray[j].StartNID)
            {
                if(bNextFind)
                {
                    // ���� ó���� ���� �߰�
                    if(NextFindCnt >= 3)
                        return -1;

                    // by zzang9un 2012.11.30 : �ѹ� �� �˻��� �� ���
                    m_NodeArray[i].NextPathIndex[NextFindCnt] = j;
                    NextFindCnt++;  // by zzang9un 2013.08.02 : ���Ծ ���� �߰�
                }
                else
                {
                    // by zzang9un 2012.11.30 : ���� �˻��� ���
                    m_NodeArray[i].NextPathIndex[NextFindCnt] = j;
                    NextFindCnt++;
                    bNextFind= true;
                }
            }
        }

        // ��� �� ���� �� ó���ؾ� �ϴ� �κ�

		// Pre Path üũ
		/*
        if(bPreFind == true)
        {
            // by zzang9un 2013.08.02 : ����� Path�� ���� ��쵵 ����ؾ� ��
            switch(PreFindCnt)
            {
            case 0: // ����� Pre Path�� ���� ���
                m_NodeArray[i].PrePathIndex[0] = INDEX_NONE;
                m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
                break;

            case 1: // ����� Pre Path�� 1���� �ִ� ���
                // 1���� ã�� ��� ������ ���� 0xFFFFFFFF�� �ʱ�ȭ    
                m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
                break;

            case 2: // ����� Pre Path�� 2�� �ִ� ���
                // 2���� ���� �ִ� ���� dist�� ���� ���� ���� ������ ����    
                ComparePathDistance(m_NodeArray[i].PrePathIndex);
                break;    
            }
        }
        else
        {
            // by zzang9un 2013.08.02 : ���� ������(0, 1 ��� ����)
            // ã�� ���� ��쿡�� ��� 0xFFFFFFFF�� �ʱ�ȭ
            m_NodeArray[i].PrePathIndex[0] = INDEX_NONE;
            m_NodeArray[i].PrePathIndex[1] = INDEX_NONE;
        }
         */
        // Next Path üũ
        if(bNextFind == true)
        {
            // by zzang9un 2013.08.02 : ����� Path�� ���� ��쵵 ����ؾ� ��
            switch(NextFindCnt)
            {
            case 0: // ����� Next Path�� ���� ���
                m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;
                m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
                break;

            case 1: // ����� Next Path�� 1���� �ִ� ���
                // 1���� ã�� ��� ������ ���� 0xFFFFFFFF�� �ʱ�ȭ
                m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
                break;

            case 2: // ����� Next Path�� 2�� �ִ� ���
                // 2���� ���� �ִ� ���� dist�� ���� ���� ���� ������ ����    
                ComparePathDistance(m_NodeArray[i].NextPathIndex);
                break;    
            }            
        }
        else
        {
            // by zzang9un 2013.08.02 : ���� ������(0, 1 ��� ����)
            // ã�� ���� ��쿡�� ��� 0xFFFFFFFF�� �ʱ�ȭ
            m_NodeArray[i].NextPathIndex[0] = INDEX_NONE;
            m_NodeArray[i].NextPathIndex[1] = INDEX_NONE;
        }

        // for�� ���� �����ϱ� �� ���� �ʱ�ȭ
		PreFindCnt = 0;
        bPreFind = false;

        NextFindCnt = 0;
        bNextFind = false;
    }
}
#endif
//---------------------------------------------------------------------------

/**
@brief   ���� �ٸ� �� Path�� �Ÿ��� ���Ͽ� swap�ϴ� �Լ�
@author  zzang9un
@date    2012.11.30
*/
void MAP::ComparePathDistance(UINT *PathIndex)
{
    // Dist�� ª�� Path�� Index�� 0 index�� �ֵ��� üũ
	// �������� :
#if(USE_CHANGENODE == 1)
	if((m_PathArray[PathIndex[1]].UBGRegion == PATH_DIRECTION_LEFT) ||
	   (m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_RIGHT) ||
	   (m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_NARROW) ||
//		((m_PathArray[PathIndex[0]].StartNID == 217) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_RIGHT)) ||
	   ((m_PathArray[PathIndex[0]].UBGRegion == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_N_BRANCH_R2L)) ||
	   ((m_PathArray[PathIndex[1]].UBGRegion == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[1]].Steering == PATH_STEERING_N_BRANCH_L2R)))

#else
	if((m_PathArray[PathIndex[1]].PathDirection == PATH_DIRECTION_LEFT) ||
	   (m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_RIGHT) ||
	   (m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_NARROW) ||
//		((m_PathArray[PathIndex[0]].StartNID == 217) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_RIGHT)) ||
	   ((m_PathArray[PathIndex[0]].PathDirection == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[0]].Steering == PATH_STEERING_N_BRANCH_R2L)) ||
	   ((m_PathArray[PathIndex[1]].PathDirection == PATH_DIRECTION_SHORT) && (m_PathArray[PathIndex[1]].Steering == PATH_STEERING_N_BRANCH_L2R)))
#endif
	{
		swap(PathIndex[0], PathIndex[1]);
		ADD_LOG_MAPMAKE_STD("ComparePathDistance");

	}

}
//---------------------------------------------------------------------------

/**
@brief   ���� Node�� �� Node�� �̿��Ͽ� �ش� Path����ü�� Index�� �����Ѵ�.
@author  zzang9un
@date    2012.11.30
@param   StartNID : ���� Node ID
@param   EndNID : �� Node ID
@return  ã�� ��� �ش� Path �迭�� Index, ã�� ���� ��� INDEX_NONE�� ����
*/
int MAP::GetPathIndex(int StartNID, int EndNID)
{
    int RtnPathIndex = INDEX_NONE;

    UINT StartNodeIndex = 0; // ���� Node�� index
    UINT EndNodeIndex = 0; // �� Node�� Index

    // �� Node�� �迭 Index�� �����´�.
	StartNodeIndex = FindNodeIndex(StartNID);
	//EndNodeIndex = FindNodeIndex(EndNID);

	PATH_INFO  pPATH_INFO;

	if(StartNodeIndex == INDEX_NONE)
	{
    	return -1;
    }

    // ���۰� ���� ����� Path�� ã�� Index�� �����Ѵ�.
    for(int i = 0; i < 2; i++)
    {

		if(m_NodeArray[StartNodeIndex].NextPathIndex[i] != INDEX_NONE)
		{
			// ã�� ��� index�� �����ϰ� for���� break
			RtnPathIndex = m_NodeArray[StartNodeIndex].NextPathIndex[i];
			pPATH_INFO = m_PathArray[RtnPathIndex];		// ����� Path ����

			if(pPATH_INFO.EndNID ==EndNID)
				break;
			else
             	RtnPathIndex = INDEX_NONE;
		}

    }

    return RtnPathIndex;
}

/**
@brief   ���� �ȵǴ� node �о�´�.
@author  ks4423.kim
@date    2014.12.16
@param   
@return  ã�� ��� �ش� Path �迭�� Index, ã�� ���� ��� INDEX_NONE�� ����
*/
int MAP::ReadPassBoxNodeFile(void)
{
    int nError = 0;
	String strOHTPath = "", strFileName = "";

	strOHTPath = ExtractFileDir(Application->ExeName);
    strFileName = String(strOHTPath + STR_DATAFILE_PATH) + NAME_PASS_BOX_NODE;

	if ((strFileName.Length() == 0) || (!FileExists(strFileName)))
    {
		nError = ERR_READ_PASS_BOX_NODE_FILE;
	}

    if (nError == NO_ERR)
    {
	   tPassBoxNodeList->LoadFromFile(strFileName);
	}
	
	return nError;
}

/**
@brief   ���� �ȵǴ� node �о�´�.
@author  ks4423.kim
@date    2014.12.16
@param   
@return  ã�� ��� �ش� Path �迭�� Index, ã�� ���� ��� INDEX_NONE�� ����
*/
int MAP::ReadNodeDisableFile(void)
{
    int nError = 0;
	String strOHTPath = "", strFileName = "";

	strOHTPath = ExtractFileDir(Application->ExeName);
    strFileName = String(strOHTPath + STR_DATAFILE_PATH) + NAME_NODE_DISABLE;

	if ((strFileName.Length() == 0) || (!FileExists(strFileName)))
    {
		nError = ERR_READ_NODE_DISABLE_FILE;
	}

    if (nError == NO_ERR)
    {
	   tNodeDisalbeList->LoadFromFile(strFileName);
	}
	
	return nError;
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� ����Ʈ ������

         ���� �ʱ�ȭ
@author  zzang9un
@date    2013.07.30
*/
MapDataManager::MapDataManager()
{
    m_MapFileLoaded = false;

    memset(&m_MapFileHeader, 0x00, sizeof(MAP_FILE_HEADER));
}
//---------------------------------------------------------------------------

/**
@brief   MapDatamanager Ŭ���� �Ҹ���

         FileStream ���� �޸� ����
@author  zzang9un
@date    2012.11.11
*/
MapDataManager::~MapDataManager()
{
    m_MapFileLoaded = false;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data�� File���� �ҷ����� �Լ�
@author  zzang9un
@date    2013.07.30
@param   FileName : Map Data�� ����� �����̸�
@param   pMapdata : Map Data�� ����� ����ü ������
@return  ������ ��� 0�� ����, ������ ��� ���� �ڵ� ����
@todo    ���� �ڵ� ���� �ʿ�
*/
int MapDataManager::LoadMapFile(TCHAR *FileName, MAP *pMapdata)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    int Error = NO_ERR;

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::in | ios::binary);

    memset(&m_MapFileHeader, 0x00, sizeof(m_MapFileHeader));

    if(fs.is_open())
    {
        int PathSize = 0; // Path Data �� size
        int NodeSize = 0; // Node Data �� size

        // Step 2. Read File Header Information
        MAP_FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
        fs.read((char *)&FileHeader, sizeof(MAP_FILE_HEADER));

        // Step 3. Header �Ľ�
        m_MapFileHeader = ParsingMapFileHeader(&FileHeader);

        if(m_MapFileHeader.PathNum > 0 && m_MapFileHeader.NodeNum > 0)
        {
            // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
            pMapdata->InitMap(m_MapFileHeader.PathNum, m_MapFileHeader.NodeNum);

            // Step 5. Path, Node Size��ŭ Read
            PathSize = m_MapFileHeader.PathNum * sizeof(PATH_INFO);
            NodeSize = m_MapFileHeader.NodeNum * sizeof(NODE_INFO);

			fs.read((char *)pMapdata->m_PathArray, PathSize);
			fs.read((char *)pMapdata->m_NodeArray, NodeSize);

            // Step 5. Map Data�� map �����̳ʷ� make
			//pMapdata->MakeMap(m_NodeArray_Map);

            m_MapFileLoaded = true; // Map File Loaded flag ����
		}
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
	}

    fs.close();
    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Map Data�� File�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.30
@param   FileName : Map Data�� ������ ���� �̸�
@param   pMapdata : ������ Map Data ����ü ������
@return  ������ ��� 0�� ����, ������ ��� ���� �ڵ� ����
@todo    ���� �ڵ� ���� �ʿ�
*/
int MapDataManager::SaveMapFile(TCHAR *FileName, MAP *pMapdata)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

    int Error = NO_ERR;

    ofstream fs;

    // Step 1. File Open
    fs.open(FileName, ios::out | ios::binary);

    if(fs.is_open())
    {
        // Step 2. Write File Header Information

        if(pMapdata->m_PathNum == 0 || pMapdata->m_NodeNum == 0)
            Error = ERR_MAP_FILESAVEFAIL;

        // Step 3. Make Header
        String strTime;
        strTime = Now().FormatString("yyyymmddhhnnss");

        m_MapFileHeader.Revision = 0; // Revision�� 0���� ������
        m_MapFileHeader.UpdateTime = StrToInt64("0x" + strTime);
        m_MapFileHeader.PathNum = pMapdata->m_PathNum;    // Path ����
        m_MapFileHeader.NodeNum = pMapdata->m_NodeNum;    // Node ����

        MAP_FILE_HEADER FileHeader;
        memset(&FileHeader, 0x00, sizeof(FileHeader));
        
        if(Error == NO_ERR)
            FileHeader = MakeMapFileHeader(&m_MapFileHeader);

        if(FileHeader.PathNum > 0 && FileHeader.NodeNum > 0)
        {
            fs.write((char *)&FileHeader, sizeof(FileHeader));

            // step3. Path ���� write
            int PathSize = pMapdata->m_PathNum * sizeof(PATH_INFO);
            fs.write((char *)pMapdata->m_PathArray, PathSize);

            // step4. Node ���� write
            int NodeSize = pMapdata->m_NodeNum * sizeof(NODE_INFO);
            fs.write((char *)pMapdata->m_NodeArray, NodeSize);
        }
    }
    else
    {
        Error = ERR_MAP_FILEOPENFAIL;
    }

    fs.close();

    return Error;    
}
//---------------------------------------------------------------------------

/**
@brief   Map File Load�� ���¸� �����ϴ� �Լ�
@author  zzang9un
@date    2012.11.16
@return  Map File�� Load�� ��� true, �׷��� ���� ��� false�� ����

bool MapDataManager::GetMapLoaded()
{
	return m_MapFileLoaded;
}
*/
//---------------------------------------------------------------------------

/**
@brief   Map File Version �����ϴ� �Լ�
@author  puting
@date    2014.06.15
@return  Map File�� Version ����

unsigned int MapDataManager::GetMapVersion()
{
	return m_MapFileHeader.Revision;
}
*/
//---------------------------------------------------------------------------

/**
@brief   STD Map���κ��� 7.0 Map Path �� ������ ���ϴ� �Լ�
@author  KJD
@date    2016.04.22
@param   TBD
@return  ������ ��� Path ����, �׷��� ���� ��� 0
*/
UINT FindPathTotalCount(NODE_INFO_STD_MAP *m_NodeArray_Map,UINT DataNum_Map)
{
	UINT PathNum = 0;
	UINT RightNodeNum = 0;

	for( int i=0 ; i< DataNum_Map ; i++ )
	{
	  if(m_NodeArray_Map[i].RightNodeID != 0 )
	  {
	  	RightNodeNum += 1;
	  }
	}
	PathNum = DataNum_Map + RightNodeNum;
	if(PathNum>(NODE_COUNT_MAX*2)) //path �ִ� ũ��� ����� 2����� �Ҵ�
	{
		PathNum = 0;
	}

	return PathNum;
}
//---------------------------------------------------------------------------
/**
@brief   STD Map���κ��� 7.0 Map Data�� Load�ϴ� �Լ�
@author  KJD
@date    2016.04.22
@param   TBD
@return  ������ ��� 0, �׷��� ���� ��� ���� �ڵ� ����
*/
#if(OHT_NAME == OHT_NAME_P4_MASK)
int MapDataManager::LoadDataFromSTDMap(MAP *pMapdata,
						   NODE_INFO_STD_MAP * m_NodeArray_Map,
						   NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
						   NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   /*NODE_INFO_STD_CID * m_NodeArray_CID,*/
						   MapMakeProcess* pMapMakeProcess,
						   bool bUseoptionalNode,
						   UINT NodeNum_Map, UINT DataNum_Flow, /*UINT DataNum_CID,*/ bool bAdjust)
#else
int MapDataManager::LoadDataFromSTDMap(MAP *pMapdata,
                           NODE_INFO_STD_MAP * m_NodeArray_Map,
                           NODE_INFO_STD_COLLISION * m_NodeArray_Collision,
                           NODE_INFO_STD_FLOWCONTROL * m_NodeArray_Flow,
						   NODE_INFO_STD_CID * m_NodeArray_CID,
						   MapMakeProcess* pMapMakeProcess,
						   bool bUseoptionalNode,
						   UINT NodeNum_Map, UINT DataNum_Flow, UINT DataNum_CID, bool bAdjust)
#endif
{
	int Error = NO_ERR;
	int ret = 0;
//	DWORD	TimeOfTest;
	UINT PathNum = 0; // ����� �� ������ ������ �ִ� ����

	int PathSize = 0; // Path Data �� size
	int NodeSize = 0; // Node Data �� size
	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Start");
	PathNum = FindPathTotalCount(m_NodeArray_Map,NodeNum_Map);

	if(pMapMakeProcess->GetCount() !=0)
		pMapMakeProcess->InitMapMakeProcessMemory();

	//STD Map �ڷḦ �̿��Ͽ� Path ���� ����
	if( (PathNum >= NodeNum_Map) && (NodeNum_Map > 0) )
	{
		// Step 1. ���Ͽ��� Load�� ���뿡 ���� Init
		pMapdata->InitMap(PathNum, NodeNum_Map);
		ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step1");
		// Step 2. ��� ���� ����
		// copy NODE_INFO
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//pMapdata->m_cpNode[0]
			pMapdata->m_NodeArray[i].NID =  m_NodeArray_Map[i].NodeID;
			pMapdata->m_NodeArray[i].InOutInfo = m_NodeArray_Map[i].NodeType;
			pMapdata->m_NodeArray[i].TagType = NORMAL_NTYPE;   // TagType 1�� : Normal Node�� �Է�
//			pMapdata->m_NodeArray[i].CollisionInfo = 0;
			if(pMapdata->m_NodeArray[i].InOutInfo != NONE_TYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != SLOPE_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTPRE_NTYPE_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTPRE_NTYPE_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTIN_NTYPE_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTIN_NTYPE_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != LIFTOUT_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_SEL_ON_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_SEL_ON_RIGHT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_STOP_LEFT &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_STOP_RIGTH &&
			   pMapdata->m_NodeArray[i].InOutInfo != MTL_AREA&&
			   pMapdata->m_NodeArray[i].InOutInfo != AUTODOOR_LEFT&&
			   pMapdata->m_NodeArray[i].InOutInfo != AUTODOOR_RIGHT&&
			   pMapdata->m_NodeArray[i].InOutInfo != FDConnectedType &&
			   pMapdata->m_NodeArray[i].InOutInfo != FDDisconnectedType&&
			   pMapdata->m_NodeArray[i].InOutInfo != EmergencyNodeMTL_NTYPE &&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_TAG &&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_DISTANCE&&
			   pMapdata->m_NodeArray[i].InOutInfo != OPT_COMBO)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step1 Error NodeType : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

             //AOHC-285
			if(bUseoptionalNode==false)
			{
				if((pMapdata->m_NodeArray[i].InOutInfo == OPT_TAG) ||
				   (pMapdata->m_NodeArray[i].InOutInfo == OPT_DISTANCE) ||
				   (pMapdata->m_NodeArray[i].InOutInfo == OPT_COMBO))
				   {
						ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap OptionalNode : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
						Error = ERR_MAPDATA_LOAD_FAIL;
						break;
				   }
			}
			// AOHC-409 FDConnectedType,  FDDisconnectedType �� ��� �÷ο���Ʈ�� ������ ���� �߻�
			if( pMapdata->m_NodeArray[i].InOutInfo == FDConnectedType || pMapdata->m_NodeArray[i].InOutInfo == FDDisconnectedType )
			{
			   int tmp;
			   tmp = pMapdata->FireDoorCheck(pMapdata->m_NodeArray[i].NID, m_NodeArray_Flow, DataNum_Flow);
			   if(tmp!=NO_ERR)
			   {
					ADD_LOG_MAPMAKE_STD("FireDoorTypeMismatch : %d, %d",pMapdata->m_NodeArray[i].NID, pMapdata->m_NodeArray[i].InOutInfo);
					Error = ERR_MAPDATA_LOAD_FAIL;
					break;

			   }
			}

			Sleep(0);
		}
 ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2");
		// Step 3. ��� ���� ����
		// Step 3.1 ���� �б� �Ǵ� ����(������ �бⰡ ���� ����)��� ����
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			pMapdata->m_PathArray[i].ID = i+PATHINDEX_OFFSET;       //10
			pMapdata->m_PathArray[i].StartNID = m_NodeArray_Map[i].NodeID;
			pMapdata->m_PathArray[i].EndNID = m_NodeArray_Map[i].LeftNodeID;
			pMapdata->m_PathArray[i].Speed = m_NodeArray_Map[i].LeftNodeSpeed;
			pMapdata->m_PathArray[i].Steering = m_NodeArray_Map[i].LeftSteering;
			pMapdata->m_PathArray[i].Distance = m_NodeArray_Map[i].LeftNodeDistance;
#if(USE_CHANGENODE == 1)
			pMapdata->m_PathArray[i].UBGRegion= 0;  //�������ܿ��� ���� : COLLISION.DAT ���
			pMapdata->m_PathArray[i].ObstacleRegion = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
#else
			pMapdata->m_PathArray[i].PathDirection = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
#endif
			pMapdata->m_PathArray[i].PathBranch = m_NodeArray_Map[i].LeftBranch;
			if(pMapdata->m_PathArray[i].Steering > 2)//N�б� ó�� �� ����
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[i].StartNID,pMapdata->m_PathArray[i].Steering);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			if( pMapdata->m_PathArray[i].PathBranch == 2 )// N�б�
			{
            	pMapdata->m_PathArray[i].Steering += 2;//STEERING_N_BRANCH_L2R or STEERING_N_BRANCH_R2L
            	if(pMapdata->m_PathArray[i].Steering != STEERING_N_BRANCH_L2R )
            	{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].Steering);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
			}
			pMapdata->m_PathArray[i].ObstacleIgnore = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
			//COLLISION.DAT�� �ִ� ��� ������ ��ġ ���θ� Ȯ�� �� �� �����Ƿ�, ��� �˻��Ͽ� ������ �Է��� �Ǿ���.

			if( m_NodeArray_Map[i].RightNodeID != 0 ) // ������ �б� ��� ���� ���� Ȯ��
			{
				pMapdata->m_PathArray[i].RailBranch = PATH_DIVERGENCE_LEFT; // ������ ��尡 ������ ���� ���� ���ʺб⸦ ����.
			}
			else
			{
				pMapdata->m_PathArray[i].RailBranch = PATH_DIVERGENCE_NONE; // ������ ��尡 ������ ����.
			}
			if(pMapdata->m_PathArray[i].Speed < 500 || pMapdata->m_PathArray[i].Speed > 8000)			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Speed : %d, %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].Speed);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}			
			if(pMapdata->m_PathArray[i].PathBranch > 2)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error PathBranch : %d, %d", pMapdata->m_PathArray[i].StartNID, pMapdata->m_PathArray[i].PathBranch);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			pMapdata->m_PathArray[i].ChargeEnable = m_NodeArray_Map[i].ChargeEnable;

			if(m_NodeArray_Map[i].ChargeEnable != 0)
			{ADD_LOG_MAPMAKE_STD("%d Left Node Charge:%d, Path Charge:%d", pMapdata->m_PathArray[i].StartNID, m_NodeArray_Map[i].ChargeEnable, pMapdata->m_PathArray[i].ChargeEnable);}

			if(bAdjust == true)
			{
				if(m_NodeArray_Map[i].IsDefaultMap.LOW_DefaultMapInfo == 1)
				{
					MAMMAKEPROCESS_LINK_INFO *updateSTD;
					updateSTD= new MAMMAKEPROCESS_LINK_INFO;
					updateSTD->LinkNID = m_NodeArray_Map[i].NodeID;
					updateSTD->IsCheck = 0x00;
					pMapMakeProcess->Update(updateSTD);
				}
            }

#if(USE_CHANGENODE == 1)
			// for change node (jhoun.lim)
			pMapdata->m_PathArray[i].OptDistance = 0;
			pMapdata->m_PathArray[i].OptDistanceAfter = 0;
			pMapdata->m_PathArray[i].OptObstacleRegion = 0;
			pMapdata->m_PathArray[i].OptUBGRegion = 0;
#endif
			Sleep(0);

		}
		// Step 3.2 ������ �бⰡ �ִ� ��� ��� ����
		int tmp = 0;//������ ��尡 �ִ� ��쿡�� ������Ű�� ���� ����
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			if( m_NodeArray_Map[i].RightNodeID != 0 ) // ������ �б� ��� ���� ���� Ȯ��
			{
				pMapdata->m_PathArray[NodeNum_Map+tmp].ID = NodeNum_Map+tmp+1;
				pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID = m_NodeArray_Map[i].NodeID;
				pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID = m_NodeArray_Map[i].RightNodeID;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Speed = m_NodeArray_Map[i].RightNodeSpeed;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Steering = m_NodeArray_Map[i].RightSteering;
				pMapdata->m_PathArray[NodeNum_Map+tmp].Distance = m_NodeArray_Map[i].RightNodeDistance;
#if(USE_CHANGENODE == 1)
				pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion= 0;	//�������ܿ��� ���� : COLLISION.DAT ���
				pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
#else
				pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
#endif
				pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch = m_NodeArray_Map[i].RightBranch;
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].Steering > 2)//N�б� ó�� �� ����
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Steering);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch == 2 )// N�б�
				{
					pMapdata->m_PathArray[NodeNum_Map+tmp].Steering += 2;//STEERING_N_BRANCH_L2R or STEERING_N_BRANCH_R2L
					if(pMapdata->m_PathArray[NodeNum_Map+tmp].Steering != STEERING_N_BRANCH_R2L )
	            	{
						ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Steering : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Steering);
						Error = ERR_MAPDATA_LOAD_FAIL;
					}
				}
				pMapdata->m_PathArray[NodeNum_Map+tmp].RailBranch = PATH_DIVERGENCE_RIGHT;  // ������ ����̹Ƿ� �����ʺб⸦ ����.
				pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = 0;  //�������ܿ��� ���� : COLLISION.DAT ���
				//COLLISION.DAT�� �ִ� ��� ������ ��ġ ���θ� Ȯ�� �� �� �����Ƿ�, ��� �˻��Ͽ� ������ �Է��� �Ǿ���.
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].Speed<500 || pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 8000)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error Speed : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch > 2)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step2 Error PathBranch : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				pMapdata->m_PathArray[i].ChargeEnable = m_NodeArray_Map[i].ChargeEnable;

				if(m_NodeArray_Map[i].ChargeEnable != 0)
				{ADD_LOG_MAPMAKE_STD("%d Right Node Charge : %d", pMapdata->m_PathArray[i].StartNID, m_NodeArray_Map[i].ChargeEnable);}

				if(bAdjust == true)
				{
					if(m_NodeArray_Map[i].IsDefaultMap.HIGH_DefaultMapInfo == 1)
					{
						MAMMAKEPROCESS_LINK_INFO *updateSTD;
						updateSTD= new MAMMAKEPROCESS_LINK_INFO;
						updateSTD->LinkNID = 1000000+ m_NodeArray_Map[i].NodeID;
						updateSTD->IsCheck = 0x00;
						pMapMakeProcess->Update(updateSTD);
					}
				}
#if(USE_CHANGENODE == 1)
				// for change node (jhoun.lim)
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistanceAfter = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = 0;
				pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = 0;
#endif
				tmp += 1;
			}
			Sleep(0);
		}
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step3");
		// Step 4. Map Data�� map �����̳ʷ� make
		pMapdata->MakeFlowControlMap(m_NodeArray_Flow, DataNum_Flow);
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step4");
		ret = pMapdata->MakeMap(pMapdata,m_NodeArray_Map,NodeNum_Map);
		if(ret != NO_ERR)
		{
			ADD_LOG_MAPMAKE_STD("m_FlowcontrolCheckResult : fail..");
			Error = ERR_MAPDATA_LOAD_FAIL;
		}
#if(OHT_NAME != OHT_NAME_P4_MASK)
		pMapdata->MakeCIDMap(m_NodeArray_CID, DataNum_CID);
#endif
		

		// Step 5, 6�� FindNodeIndex, FindRightNodeIndex�� ����ϰ��� MakeMap ������ ������.

		// Step 5. TagType make(�б� �Ǵ� �շ������ Tag���� N:1, S:2, R:3, C:4)
		// Step 5.1 TagType 2�� : Stop Node�� �Է�
		for(int i=0 ; i < DataNum_Flow ; i++ )
		{
			int tmp;
			tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].StopNodeID);
			if(tmp != INDEX_NONE)
			{
			if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = STOP_NTYPE;   // TagType 2�� : Stop Node�� �Է�
			}
			else
			{
				//NOP
				}
			}
			Sleep(0);
		}

		// Step 5.2 TagType 3�� : Reset Node, Common Node �Է�
		for(int i=0 ; i < DataNum_Flow ; i++ )
		{
			//if( m_NodeArray_Flow->ResetNodeMain != 0 ) // ��尡 �����. �ǹ̻� �־�� �ϳ� �׻� ���̹Ƿ� ���Ǻ� ����.
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].ResetNodeMain);
			if(tmp != INDEX_NONE)
			{
			if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = RESET_NTYPE;   // TagType 3�� : Reset Node�� �Է�
			}
			else if( pMapdata->m_NodeArray[tmp].TagType == STOP_NTYPE )
			{
				pMapdata->m_NodeArray[tmp].TagType = COMMON_NTYPE;   // TagType 3�� : Common Node�� �Է�
			}
			}
			if( m_NodeArray_Flow[i].ResetNodeSub != 0 ) // ��尡 �����.
			{
				int tmp = pMapdata->FindNodeIndex(m_NodeArray_Flow[i].ResetNodeSub);
				if(tmp != INDEX_NONE)
				{
				if( pMapdata->m_NodeArray[tmp].TagType == NORMAL_NTYPE )
				{
					pMapdata->m_NodeArray[tmp].TagType = RESET_NTYPE;   // TagType 3�� : Reset Node�� �Է�
				}
				else if( pMapdata->m_NodeArray[tmp].TagType == STOP_NTYPE )
				{
					pMapdata->m_NodeArray[tmp].TagType = COMMON_NTYPE;   // TagType 3�� : Common Node�� �Է�
					}
				}
			}
			Sleep(0);
		}
 ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5");
		// Step 6. m_PathArray�� COLLISION.DAT ���� mapping
		for(int i=0 ; i < NodeNum_Map ; i++ )
		{
			//if( m_NodeArray_Collision[i]->LeftNodeID != 0 ) // ��尡 �����. �ǹ̻� �־�� �ϳ� �׻� ���̹Ƿ� ���Ǻ� ����.
			//m_NodeArray_Collision[i]->LeftNodeID�� ��δ�
			//NodeIndex�� PathIndex�� �����ϹǷ� NodeIndex���� �迭������ �˾ƿ�.
#if(USE_CHANGENODE == 1)
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Collision[i].NodeID);
			if( pMapdata->m_PathArray[tmp].UBGRegion != 0 || pMapdata->m_PathArray[tmp].ObstacleRegion != 0)//UBG ���� 0�� �ƴϸ� ��� �ߺ�
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[tmp].StartNID);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			pMapdata->m_PathArray[tmp].UBGRegion = m_NodeArray_Collision[i].LeftUBGRegion;
			pMapdata->m_PathArray[tmp].ObstacleRegion = m_NodeArray_Collision[i].LeftObstacleRegion;
			pMapdata->m_PathArray[tmp].ObstacleIgnore = m_NodeArray_Collision[i].LeftObstacleStatus;

            //puting optionalNode ��� ����
			pMapdata->m_PathArray[tmp].OptUBGRegion = m_NodeArray_Collision[i].LeftUBGRegion;
			pMapdata->m_PathArray[tmp].OptObstacleRegion = m_NodeArray_Collision[i].LeftObstacleRegion;


			if(pMapdata->m_PathArray[tmp].UBGRegion == 0 && pMapdata->m_PathArray[tmp].ObstacleRegion == 0)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[tmp].StartNID,pMapdata->m_PathArray[tmp].UBGRegion );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			//���� ���� �߰��� ���� 4��Ʈ������ �� ó��
			if((pMapdata->m_PathArray[tmp].UBGRegion != PATH_DIRECTION_NORMAL && pMapdata->m_PathArray[tmp].UBGRegion != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[tmp].Speed > 1300 )// ������ N, Slope�� �ƴѵ� �ӵ��� 1.3m/s �̻��� ���
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, 0x%x, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].UBGRegion, pMapdata->m_PathArray[tmp].Speed );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			// for change node (jhoun.lim)
			pMapdata->m_PathArray[tmp].calcOptDistance(m_NodeArray_Map[tmp].LeftNodeOptRatio);
			if(pMapdata->m_PathArray[tmp].OptDistance > 0)
			{
				pMapdata->m_PathArray[tmp].OptUBGRegion = m_NodeArray_Collision[i].LeftOptUBGRegion;
				pMapdata->m_PathArray[tmp].OptObstacleRegion = m_NodeArray_Collision[i].LeftOptObstacleRegion;
				pMapdata->m_PathArray[tmp].calcOptDistanceAfter(m_NodeArray_Map[tmp].LeftNodeOptRatio);
                if(pMapdata->m_PathArray[tmp].OptDistanceAfter < 0)
                {
                    Error = ERR_MAPDATA_LOAD_FAIL;
                }

			}else if(pMapdata->m_PathArray[tmp].OptDistance < 0)
            {
                Error = ERR_MAPDATA_LOAD_FAIL;
            }

			if( m_NodeArray_Collision[i].RightNodeID != 0 ) // ��尡 �����.
			{
				int tmp2 = tmp;
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Collision[i].RightNodeID);

				if( pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != 0 || pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion != 0)//UBG ���� 0�� �ƴϸ� ��� �ߺ�
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Collision[i].RightNodeID )
				{
					pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion = m_NodeArray_Collision[i].RightUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion = m_NodeArray_Collision[i].RightObstacleRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = m_NodeArray_Collision[i].RightObstacleStatus;

					//puting optionalNode ��� ����
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = m_NodeArray_Collision[i].RightUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = m_NodeArray_Collision[i].RightObstacleRegion;
				}

				if(pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion == 0 && pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleRegion == 0)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion );
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				//���� ���� �߰��� ���� 4��Ʈ������ �� ó��
				if((pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != PATH_DIRECTION_NORMAL && pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 1300 )// ������ N, Slope�� �ƴѵ� �ӵ��� 1.3m/s �̻��� ���
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].UBGRegion, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}

				// for change node (jhoun.lim)
				pMapdata->m_PathArray[NodeNum_Map+tmp].calcOptDistance(m_NodeArray_Map[tmp2].RightNodeOptRatio);
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance > 0)
				{

					pMapdata->m_PathArray[NodeNum_Map+tmp].OptUBGRegion = m_NodeArray_Collision[i].RightOptUBGRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].OptObstacleRegion = m_NodeArray_Collision[i].RightOptObstacleRegion;
					pMapdata->m_PathArray[NodeNum_Map+tmp].calcOptDistanceAfter(m_NodeArray_Map[tmp2].RightNodeOptRatio);
					if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistanceAfter< 0)
                    {
                        Error = ERR_MAPDATA_LOAD_FAIL;
                    }

				}
				else if(pMapdata->m_PathArray[NodeNum_Map+tmp].OptDistance < 0)
                {
                    Error = ERR_MAPDATA_LOAD_FAIL;
                }
			}
#else
			int tmp = pMapdata->FindNodeIndex(m_NodeArray_Collision[i].NodeID);
			if( pMapdata->m_PathArray[tmp].PathDirection != 0)//UBG ���� 0�� �ƴϸ� ��� �ߺ�
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[tmp].StartNID);
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			

			pMapdata->m_PathArray[tmp].PathDirection = m_NodeArray_Collision[i].LeftUBGRegion | (m_NodeArray_Collision[i].LeftObstacleRegion << 4);

			pMapdata->m_PathArray[tmp].ObstacleIgnore = m_NodeArray_Collision[i].LeftObstacleStatus;

			if(pMapdata->m_PathArray[tmp].PathDirection == 0)
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[tmp].StartNID,pMapdata->m_PathArray[tmp].PathDirection );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}
			//���� ���� ������� ����
			//if(pMapdata->m_PathArray[tmp].PathBranch == 2 && pMapdata->m_PathArray[tmp].PathDirection != PATH_DIRECTION_SHORT) // N�б��ε� ������ S�� �ƴ� ���
			//{
			//	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error N Branch Pattern : %d, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].PathDirection);
			//	Error = ERR_MAPDATA_LOAD_FAIL;
			//}
			//���� ���� �߰��� ���� 4��Ʈ������ �� ó��
			if(((pMapdata->m_PathArray[tmp].PathDirection & 0x0F) != PATH_DIRECTION_NORMAL && (pMapdata->m_PathArray[tmp].PathDirection & 0x0F) != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[tmp].Speed > 1300 )// ������ N, Slope�� �ƴѵ� �ӵ��� 1.3m/s �̻��� ���
			{
				ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, 0x%x, %d", pMapdata->m_PathArray[tmp].StartNID, pMapdata->m_PathArray[tmp].PathDirection, pMapdata->m_PathArray[tmp].Speed );
				Error = ERR_MAPDATA_LOAD_FAIL;
			}

			if( m_NodeArray_Collision[i].RightNodeID != 0 ) // ��尡 �����.
			{
				tmp = pMapdata->FindRightNodeIndex(m_NodeArray_Collision[i].RightNodeID);
				
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection != 0)//UBG ���� 0�� �ƴϸ� ��� �ߺ�
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error Node ID Not Unique : %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				
				if( pMapdata->m_PathArray[NodeNum_Map+tmp].EndNID == m_NodeArray_Collision[i].RightNodeID )
				{

					pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection = m_NodeArray_Collision[i].RightUBGRegion | (m_NodeArray_Collision[i].RightObstacleRegion << 4);
					pMapdata->m_PathArray[NodeNum_Map+tmp].ObstacleIgnore = m_NodeArray_Collision[i].RightObstacleStatus;
				}
				
				if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection == 0)
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection );
					Error = ERR_MAPDATA_LOAD_FAIL;
				}
				//���� ���� ������� ����
				//if(pMapdata->m_PathArray[NodeNum_Map+tmp].PathBranch == 2 && pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection != PATH_DIRECTION_SHORT) // N�б��ε� ������ S�� �ƴ� ���
				//{
				//	ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error N Branch Pattern : %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection);
				//	Error = ERR_MAPDATA_LOAD_FAIL;
				//}
				//���� ���� �߰��� ���� 4��Ʈ������ �� ó��
				if(((pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection & 0x0F) != PATH_DIRECTION_NORMAL && (pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection & 0x0F) != PATH_DIRECTION_HILL) && pMapdata->m_PathArray[NodeNum_Map+tmp].Speed > 1300 ) // ������ N, Slope�� �ƴѵ� �ӵ��� 1.3m/s �̻��� ���
				{
					ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step5 Error PathDirection Speed : %d, %d, %d", pMapdata->m_PathArray[NodeNum_Map+tmp].StartNID, pMapdata->m_PathArray[NodeNum_Map+tmp].PathDirection, pMapdata->m_PathArray[NodeNum_Map+tmp].Speed);
					Error = ERR_MAPDATA_LOAD_FAIL;
				}


			}
#endif
			
			Sleep(0);
		}
		

		m_MapFileLoaded = true; // Map File Loaded flag ����
	}
	else
	{
		m_MapFileLoaded = false; // Map File Loaded flag ����
	}
ADD_LOG_MAPMAKE_STD("LoadDataFromSTDMap Step6 END");

	return Error;
}


//---------------------------------------------------------------------------

/**
@brief   StartNode1_1~5 ID�� ��Ī�Ǵ� CID �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.08.18
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromCIDStart_1(int NodeID)
{
    map<int, int>::iterator FindIter;

	FindIter = m_CIDStartMap_1.find(NodeID);

    if(FindIter != m_CIDStartMap_1.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
    }
    else
    {
        return INDEX_NONE;
    }
}

//---------------------------------------------------------------------------

/**
@brief   StartNode2_1~5 ID�� ��Ī�Ǵ� CID �迭�� index�� �����ϴ� �Լ�.
@author  KJD
@date    2016.08.18
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromCIDStart_2(int NodeID)
{
    map<int, int>::iterator FindIter;

    FindIter = m_CIDStartMap_2.find(NodeID);

    if(FindIter != m_CIDStartMap_2.end())
    {
        // Node�� ã�� ���
        return FindIter->second;
	}
	else
	{
		return INDEX_NONE;
	}
}

/**
@brief   FlowControl�� ��Ī�Ǵ� Flowcontrol �迭�� index�� �����ϴ� �Լ�.
@author  puting
@date    2016.08.18
@param   NodeID : ã���� �ϴ� Node ID
@return  Node ID�� ��Ī�Ǵ� �迭 index
@todo    ���� �ڵ� ������
*/
int MAP::FindNodeIndexFromFlowControl(int NodeID, int NextNodeID, STD_FlowControl *pFlowMap)
{
	map<int, int>::iterator FindIter;

	int nInedex = INDEX_NONE;
	int nTempStopNode = NodeID;

	for(int j = 0; j <4; j++)
	{
		nTempStopNode = nTempStopNode + 1000000;

		FindIter = m_FlowControl.find(nTempStopNode);

		if(FindIter != m_FlowControl.end())
		{
			// Node�� ã�� ���   ȥ���ؼ�����ϴ� ��춧���� ����
			if((pFlowMap->m_NodeArray[FindIter->second].ResetNodeMain ==NextNodeID) ||
				(pFlowMap->m_NodeArray[FindIter->second].ResetNodeSub ==NextNodeID))
				{
					nInedex = FindIter->second;
					break;
				}
		}
	}

	return nInedex;
}

UINT MAP::calReOptRatio(double ChangeOptRatio)         // STB Ȯ����
{
   double ChangeOptRatioCal;
   ChangeOptRatioCal=((int)(ChangeOptRatio*pow(10.0,4)))/pow(10.0,4);   // �Ҽ��� �ݤ����� �ڸ���
   UINT ChangeOptCal, OptCalResult;
   int RatioDigit[10];

   ChangeOptCal	= UINT(ChangeOptRatioCal*10000);
   ADD_LOG_MAPMAKE_STD("ChangeOptRatio : %f, ChangeOptRatioCal :%f, ChangeOptCal:%x %d",ChangeOptRatio,ChangeOptRatioCal,ChangeOptCal, ChangeOptCal );
	for(int i = 0; i<4 ;i++)         // 4�ڸ��� ������ ������ (�켱 Optional 1������ ��� ����)
	{
	   RatioDigit[i] = (ChangeOptCal % 10) & 0x000F;
	   ChangeOptCal /= 10;
	}
   ADD_LOG_MAPMAKE_STD("calReOptRatio : %x", (( RatioDigit[0]  | (RatioDigit[1]<<4)  |(RatioDigit[2]<<8) |(RatioDigit[3]<<12) ) & 0x0000FFFF ) << 16);

   return  (( RatioDigit[0]  | (RatioDigit[1]<<4)  |(RatioDigit[2]<<8) |(RatioDigit[3]<<12) ) & 0x0000FFFF ) << 16 ;


}

int MAP::FireDoorCheck(UINT NodeID, NODE_INFO_STD_FLOWCONTROL * m_FlowArray, UINT DataNum)
{
	int Error = NO_ERR;
	int nTempStopNode= 0;
	int nCnt = 0;

	for(int i=0;i<DataNum;i++)
	{
		nTempStopNode =  m_FlowArray[i].StopNodeID;
		if(NodeID == nTempStopNode)
		{
			ADD_LOG_MAPMAKE_STD("FireDoorCheck : %d %d", NodeID, nTempStopNode);
			return NO_ERR;
		}
		else
		{
			nCnt++; // Flowcontol�� StopNodeID�� �� ���ϴ� NodeID �ٸ� ��� Cnt ++�ؼ� DataNum-1�̶� ������ Flowconrol���ٰ� �Ǵ�

			if(nCnt == (DataNum - 1))
			{
				ADD_LOG_MAPMAKE_STD(" nCnt: %d, DataNum : %d", nCnt, DataNum);
				  return -1;
			}

		}
	}
}
#pragma package(smart_init)
