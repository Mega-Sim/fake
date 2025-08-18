/**
@file    CmdManager.h
@version OHT 7.0
@brief   CmdManager Class�� ������ Header File
*/

#ifndef CmdManagerH
#define CmdManagerH
//---------------------------------------------------------------------------

#include "../Common/Protocol_Define_Modify.h"

#include "../Common/Def_DataType.h"
#include "../Common/Def_Protocol.h"
#include "../Common/Def_TPCommand.h"
//#include "OHT_TLV.h"
#include "List.h"
#include "OHTMainStatus.h"
#include "LogHandler.h"
#include "Def_HW.h"
#include "ExecuteInfo.h"
#include "Def_DefaultIniParam.h"

// by zzang9un 2013.12.05 : ���� ���� �Ÿ��� SW Limit�� ���� �ʵ��� �ϱ� ���� ����
#define	DRIVING_DISTANCE_LIMIT          800000 // ���� �Ÿ� ���� 800 meter
//---------------------------------------------------------------------------
/**
@class   CmdManager
@date    2012.10.30
@author  zzang9un
@brief   OHT Command Manager
*/
class CmdManager
{
private:
//    LList       *m_CmdList; ///< Main Command List


	LList       *m_TaskList; ///< Main Command List
	LList       *m_ResponseList; ///< Reseponse Command List
	LList		*m_CycleList;    ///< Cycle Command List

//	LList		*m_pPassOpenList;    	 ///< Pass Open Command List    puting 2016 03 15
//	LList		*m_pPassPermitList;     ///< Pass Permit Command List    puting 2016 05 31


    // by zzang9un 2013.04.09 : Quick Cmd ó���� ���� �߰�
	EXECUTE_INFO *m_pExecuteInfo;

    // by zzang9un 2013.08.16 : Log ���� ����
	LogUnit *pLogUnit;      ///< TaskControl �α� ������ ���� ����

    // by zzang9un 2013.10.03 : OCS ������ ���� ������ ���� �߰�

	// by zzang9un 2013.11.11 : ���� ��� ������ ���� ������ ���� �߰�

//	DEFAULT_PARAM_SET *m_defualtparam;
public:
	CmdManager();  // constructor
	~CmdManager(); // destructor
    OHTMainStatus *m_MainStatus;  ///< Main Status Command
    // List ���� �Լ�
//    void InitCmdList();
//    int DeleteFirstCmd();
//    int DeleteCmd(UINT Index);
//    void AddCmd(void *pData);
//    void* ReferFirstCmd();
//    void* PopFirstCmd();
//	int GetCmdCount();


	void InitTaskList();
//	int DeleteFirstTask();                      //�̻��
	int DeleteTask(UINT Index);
	void AddTask(void *pData);
	void AddTask(void *pData,int Index);
	void* ReferFirstTask();
	void* ReferTask(UINT index);
	void* PopFirstTask();
	int GetTaskCount();


	void InitOCSResponseList();
//    int DeleteFirstOCSResponse();             //�̻��
	int DeleteOCSResponse(UINT Index);
	void AddOCSResponse(void *pData);
//    void* ReferFirstOCSResponse();            //�̻��
	void* PopFirstOCSResponse();
    int GetOCSResponseCount();

	void InitCycleList();
//	int DeleteFirstCycle();                     //�̻��
//	int DeleteCycle(UINT Index);                //�̻��
	void AddCycle(void *pData);
	void* ReferCycle(UINT index);
//	void* ReferFirstCycle();                    //�̻��
//	void* PopFirstCycle();                      //�̻��
	void* PopBackCycle();
	int GetCycleCount();

	int DeleteBackCycle(); //puting �۾���ҿ� ���� ����κ�

	//Pass Open ���� �߰� ���� puting 2016 03 15
//	void InitPassOpenList();
//	int DeleteFirstPassOpen();
//	int DeletePassOpen(UINT Index);
//	void AddPassOpen(void *pData);
//	void* ReferPassOpen(UINT index);
//	void* ReferFirstPassOpen();
//	void* PopFirstPassOpen();
//	int GetPassOpenCount();
//
//	//Pass Permit ���� �߰� ���� puting 2016 03 15
//	void InitPassPermitList();
//	int DeleteFirstPassPermit();
//	int DeletePassPermit(UINT Index);
//	void AddPassPermit(void *pData);
//	void* ReferPassPermit(UINT index);
//	void* ReferFirstPassPermit();
//	void* ReferBackPassPermit();
//	void* PopFirstPassPermit();
//	void* PopBackPassPermit();
//	int GetPassPermitCount();

	void MakeTaskList_Standard(PKT_TASKCMDDATA *pCmdPktData);


	void MakeTaskList_Standard_Del_JobOrder();

    // by zzang9un 2013.04.09 : Quick Command�� Main���� �����ϴ� �Լ�
	void SendQuickCmd2Main(QUICK_CMD QuickCmd);

	// 2013.10.16 ji7412.lee : ���� ������ �������� Ȯ���ϴ� �Լ�
    int  Is_Executable(void);
    int  Is_Executable(char* ExMessage, ...);
};

#endif
