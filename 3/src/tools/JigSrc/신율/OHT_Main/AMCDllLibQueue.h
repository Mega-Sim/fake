// ---------------------------------------------------------------------------
/**
@file    AMCDllLibQueue.h
@version OHT 7.0
@brief   AMCDllLibQueue Class Header File
*/

#ifndef AMCDllLibQueueH
#define AMCDllLibQueueH

#include <windows.h>
#include "ErrorCode.h"
#include "Def_MotionControler.h"
#include "AMC_Protocol.h"
#include "List.h"

#include "LogHandler.h"	// Log �ۼ� ����

// Additional Protocol for AMCDllLib.cpp
#if(USE_CHANGENODE == 1)
typedef struct _CMD_DATA_UNION
{
	int	Output[4];		///< Output ����		(SetOutBit)
	CMD_AXIS	CmdAxis;		///< �� ��� ����		(�� �̵� ���)
	DETECT_INFO	DetectInfo;	///< ���� ���� Type ������ ���� ����
	PAUSE_INFO PauseInfo;
	OPTIONAL_NODE_INFO OptNodeInfo;
//	int	DecelLimitOfDriving;	///< ���� ���� Limit �������ӵ�(Cts/s2)
}CMD_DATA_UNION;
#else
typedef union _CMD_DATA_UNION
{
	int	Output[4];		///< Output ����		(SetOutBit)
	CMD_AXIS	CmdAxis;		///< �� ��� ����		(�� �̵� ���)
	DETECT_INFO	DetectInfo;	///< ���� ���� Type ������ ���� ����
	PAUSE_INFO PauseInfo;
//	int	DecelLimitOfDriving;	///< ���� ���� Limit �������ӵ�(Cts/s2)
}CMD_DATA_UNION;
#endif


// Additional Protocol for AMCDllLib.cpp
typedef struct _QUEUEING_AMC_CMD
{
	CMD_ANALYSIS_UNION	CmdAnalysisUnion;	///< Union�� Type�� Ȯ���ϴ� �Լ�
	CMD_DATA_UNION		DataUnion;			///< ����� ����ü --> AMCDllLib ���ο��� Buffer�� �޸� ������ �������� ���۵�

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6��  8.1���� �������Ҷ��� �̺κ��� �ּ�ó��
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}QUEUEING_AMC_CMD;


//-----------------------------------------------------------
/**
 @class   AMCDllLibQueue
 @date    2012.10.25
 @author  ehoto
 @brief   AMCDllLibQueue ���� Class
 @note    AMC �ֱ� ��� ������ �����ϴ� Queue
 */
class AMCDllLibQueue
{
public:
	AMCDllLibQueue();	///< ������
	~AMCDllLibQueue();	///< �Ҹ���

	bool init(void);	///<List�� ���� Count�� �ʱ�ȭ
	bool putData(QUEUEING_AMC_CMD QueuingAmcCmd, int *nError);		///< User�� Buffer�� ������ �Է��� �� ���
	bool makeData(AMC_CMD* pAmcCmd, int *nError);	///< Buffer ������ �������� AMC�� ���� ����� ���� �� ���
	int GetReportError();
	
private:
	CRITICAL_SECTION	m_CS;		///< Buffer�� Count�� �а� �� ��
	LList	m_LList;	///< ����� �����ϴ� List
//	int 	m_nCount;	///< List�� �� �ִ� ����� ����
	int	m_nError;	///< ����: ���� ���� ��Ȳ
	int	m_nReportError;	///< ����: ���� ���� ��Ȳ

};
// ---------------------------------------------------------------------------
#endif
