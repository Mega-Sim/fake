//---------------------------------------------------------------------------

#ifndef AUTOTEACHINGH
#define AUTOTEACHINGH
//---------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <winsock2.h>
#include "VirtualModuleControl.h"
#include "VirtualDrivingControl.h"

#include "HWSet.h"
#include "HWCommon.h"
#include "ExecuteInfo.h"
#include "Def_TPCommand.h"

#include "LogHandler.h" // Log �ۼ� ����

//#include "STD_MapMakerFile.h"


#define MAX_BIG_PACKET_SIZE  8192

#define OT		"Origin Teaching"		// zero position set. reset command. no response exist. ver 1.0 only

#define PT		"Free Teaching"			// Shift�� �̵� (Step �߰��� �����Ұ�.)

#define FT			"Focus Teaching"		// move1 -> FT Send
#define FT_EQ		"EFocus Teaching"		// move1 -> FT Send
#define FT_STB		"SFocus Teaching"		// move1 -> FT Send
#define FT_STK		"STKFocus Teaching"		// move1 -> FT Send

#define AT			"Auto Teaching"			// slide(Y), move(X). no (Z)
#define AT_EQ		"EAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STB_L		"SLAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STB_R		"SRAuto Teaching"			// slide(Y), move(X). no (Z)
#define AT_STK		"STKAuto Teaching"			// slide(Y), move(X). no (Z)

//#define STB_L_SHIFT_DIS 	-455.0
#define STB_L_HOIST_DIS 	108.5

//#define STB_R_SHIFT_DIS 	445.0
#define STB_R_HOIST_DIS 	108.5

#define ZT		"Z-Axis Teaching"
#define ST		"Status"
#define RST		"Reset"
#define BGE		"Begin"
#define END		"End"

#define HOIST_DOWN	0	// �Ʒ� ��������
#define HOIST_UP	1   // �� ��������

#define HOIST_DOWN_H2TP_AT         1 //ORIGIN -> TPOINT
#define HOIST_UP_TP2H_AT           2 //TPOINT -> ORIGIN
#define HOIST_MOVE_TP_AT           3 //TPOINT -> TPOINT_MODIFY
//#define HOIST_UP_TP2H_AT_STB       4 //TPOINT -> ORIGIN

#define SHIFTROTATE_H2TP_AT     	  1 //ORIGIN -> TPOINT
#define SHIFTROTATE_TP2H_AT           2 //TPOINT -> ORIGIN
#define SHIFTROTATE_MOVE_TP_AT        3 //TPOINT -> TPOINT_MODIFY

#define CMD_RETRY_ST        20000 //TPOINT -> TPOINT_MODIFY
#define CMD_RETRY_AT        15000 //TPOINT -> TPOINT_MODIFY
#define CMD_RETRY        	3000  //TPOINT -> TPOINT_MODIFY

#define DEFAULT_OFFSET        		30.0 //Default Offset �Ÿ�
#define DEFAULT_OFFSET_EQ        	20.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_HOIST  		10.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_STB_HOIST  	1.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_SHIFT_L  	0.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_SHIFT_R  	0.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_SHIFT_EQ  	0.0 //Default Offset �Ÿ�
#define VISION_OFFSET_SHIFT  		0.0 //Default Offset �Ÿ�
#define TEACHING_OFFSET_HOIST_AT  20.0 //soming : AUTOTEACHING Hoist Default Offset �Ÿ�



typedef enum SIM_RUN_CMD_
{
		CMD_INT 	 = 0,
		CMD_RESPONSE 	,	// ������ ���ƿ��� (�������� �Ѿ�� �Ǵ��� State Machine�� �ؾ� �Ѵ�)   C
		CMD_READY		,	// Start from Step1
//		CMD_TIMEOUT		,
		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
}SIM_RUN_CMD;

typedef enum RESPONCE_TYPE_
{
		RESPONCE_READY 	 	= 100,
		RESPONCE_BUSY 	 	= 101,
		RESPONCE_ERROR	 	= 109,
		RESPONCE_COMPLETE 	= 110,	// Start from Step1

		STATUS_M_READY  = 200,
		STATUS_M_BUSY   = 201,
		STATUS_V_ERROR  = 208,
		STATUS_O_ERROR  = 209,
}RESPONCE_TYPE;



// �׽�Ʈ�� Ŭ���̾�Ʈ (OHT)
#define AUTO_TEACH_MY_SERVER_PORT  1201	// ���� Listen �� ��Ʈ��ȣ
#define AUTO_TEACH_SEND_PORT	   1200	// ������ ����ϴ� ��Ʈ��ȣ

typedef struct
{
	char cCommand[256];
} AUTO_TEACH_PACKET;


typedef struct
{
	double dEQ_Shift;
	double dEQ_Hoist;
	double dSTB_Shift;
	double dSTB_Hoist;
	double dSTB_R_Shift_Offset;
	double dSTB_L_Shift_Offset;
	int nSTB_R_Driving_Offset;
	int nSTB_L_Driving_Offset;
	unsigned char AutoTeachingIP[20];
}AUTO_TEACH_OFFSET;

// Auto Teaching �۾� STEP ���� Define
// --------------------------------------------------------------------------
typedef enum AUTO_TEACHING_STEP_
{


// ���� �ʱ���ġ, Shift �ʱ���ġ, Hoist �ʱ���ġ���� ������ ���� ��.

	AUTO_TEACHING_READY                  = 0,      //CheckAutoTeachningReady() : ���� Foup ����ִ��� Ȯ��, AutoTeaching Jig�� ������� Ȯ�� ��
												   //���� ��� �ʱ�ȭ �ܰ�.
	AUTO_TEACHING_CHECK_BEGIN				,

	AUTO_TEACHING_MOVE_INIT_SHIFTROTATE       			,     //���� ��ġ �̵� Check(�⺻ Map ���� ����, �������� ��� �ֱ�)
	AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE      ,
	AUTO_TEACHING_MOVE_INIT_DOWN_HOIST      			,
	AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST	    ,        // ƼĪ��ġ���� 20mm ������ �̵�.


	//1.---------���� ��ġ �غ� Ȯ�� �� �̵�
//	AUTO_TEACHING_P_TEACHING_CMD			,	// Free Teaching 1ȸ (FT) ---AT�� ���
//	AUTO_TEACHING_P_TEACHING_WAIT			,
//
//	AUTO_TEACHING_MOVE_FREE_CMD				,   // OHT �̵�    --�߽ɷ� ��ġ ����. �ٽ� Ȩ���� �̵� �Ұ��, �ش� ��ġ�� �ٷ� �̵�.
//	AUTO_TEACHING_MOVE_FREE_WAIT			,

	//1.---------Focus Ȯ�� �� �̵�
	AUTO_TEACHING_F_TEACHING_CMD			,	// Focus Teaching 1ȸ (FT) ---AT�� ���
	AUTO_TEACHING_F_TEACHING_WAIT			,

	AUTO_TEACHING_MOVE_FOCUS_CMD			,   // OHT �̵�    --��Ŀ�� ��ġ ����. �ٽ� Ȩ���� �̵� �Ұ��, �ش� ��ġ�� �ٷ� �̵�.
	AUTO_TEACHING_MOVE_FOCUS_WAIT			,

///////////////////////////////////////////////////////////
	//2. --------------AT �ݺ� ���� ---����-------------------//
///////////////////////////////////////////////////////////
    AUTO_TEACHING_TEACHING_CMD				,   // Auto Teaching �ݺ� (AT)   ---AT�� ���
	AUTO_TEACHING_TEACHING_WAIT				,	// ƼĪ ������� �Ѿ����, ������������ ������ Ȯ���ϰ�,
												// �����ȿ� ������ AUTO_TEACHING_COMPLETE�� �̵��Ѵ�.

	//STB��
	AUTO_TEACHING_MOVE_UP_HOIST_STB      			,      //Home���� �̵�
	AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB		    ,

	//------------������ �̵� ----------//   Limit �Ÿ� ���� �Ѱ�.. �ʱ� ��ġ�� ����(+- ����.)
	AUTO_TEACHING_MOVE_X_POS_CMD			,   // OHT �̵�   ( ���� õõ�� �����ӵ� ���� )
//	AUTO_TEACHING_MOVE_X_POS_WAIT			,

	//Shift �� �̵�
	AUTO_TEACHING_MOVE_Y_POS_CMD			,
	AUTO_TEACHING_MOVE_Y_POS_WAIT			,   //�̵� �� ���� ����⿡ ���� ���Žð� ���� ��� ���� �ݺ� ���� �� ��.

	AUTO_TEACHING_MOVE_DOWN_HOIST_STB      			,      //Home���� �̵�
	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB		    ,

//	//------------Teaching��ġ �̵�(������ �̵� ������, Teaching ��ġ�� �ٷ� �̵� ----//
//    AUTO_TEACHING_MOVE_T_POINT_CMD	    ,
//	AUTO_TEACHING_MOVE_T_POINT_WAIT	        ,
///////////////////////////////////////////////////////////
///---------------AT �ݺ� ���� ---��------------------//
///////////////////////////////////////////////////////////

	//3. ---------Hoist �� �̵�
    AUTO_TEACHING_Z_TEACHING_CMD			,	// Z�� ƼĪ�� �ѹ��� �ϰ� �Ѿ��   ---AT�� ���
	AUTO_TEACHING_Z_TEACHING_WAIT			,

		//STB��
	AUTO_TEACHING_MOVE_UP_HOIST_STB_Z      			,      //Home���� �̵�
	AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z		    ,

		//Shift �� �̵�
	AUTO_TEACHING_MOVE_Y_POS_CMD_Z			,
	AUTO_TEACHING_MOVE_Y_POS_WAIT_Z			,   //�̵� �� ���� ����⿡ ���� ���Žð� ���� ��� ���� �ݺ� ���� �� ��.

	AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z      			,      //Home���� �̵�
//	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB_Z		    ,

//	AUTO_TEACHING_HOIST_MOVE_CMD			,      // OHT �̵�
//	AUTO_TEACHING_HOIST_MOVE_WAIT			,

	AUTO_TEACHING_CHECK_PUSH_HAND			,

	AUTO_TEACHING_SAVE_CURRENT_POS_CMD		,



	AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN			   	, //soming :Hoist ���Ʒ� seq ���� ��, Hand OPEN ����

	AUTO_TEACHING_MOVE_COMP_UP_HOIST     			,      //Home���� �̵�
	AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST	    ,
   /*
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST             ,     //soming 10�ܰ�
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_FAST     ,
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_SLOW          ,
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_SLOW   ,
	AUTO_TEACHING_U_CHECK_ARRIVE_TEACHING_POINT  ,
	AUTO_TEACHING_U_GRIP_FOUP                    ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_SLOW           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_SLOW     ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_FAST           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_FAST     ,
    */
	AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE       			,
	AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE      ,
	AUTO_TEACHING_CHECK_ANTIDROP			,

	AUTO_TEACHING_COMPLETE					,

} AUTO_TEACHING_STEP;


// Auto Teaching �۾� STEP ���� Define
// --------------------------------------------------------------------------
typedef enum AUTO_TEACHING_NULL_TYPE_STEP_
{


// ���� �ʱ���ġ, Shift �ʱ���ġ, Hoist �ʱ���ġ���� ������ ���� ��.

	N_AUTO_TEACHING_READY                  = 0,      //CheckAutoTeachningReady() : ���� Foup ����ִ��� Ȯ��, AutoTeaching Jig�� ������� Ȯ�� ��
												   //���� ��� �ʱ�ȭ �ܰ�.
	N_AUTO_TEACHING_CHECK_BEGIN				,

	N_AUTO_TEACHING_MOVE_INIT_SHIFTROTATE       			,     //���� ��ġ �̵� Check(�⺻ Map ���� ����, �������� ��� �ֱ�)
	N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_SHIFTROTATE      ,
	N_AUTO_TEACHING_MOVE_INIT_DOWN_HOIST      			,
	N_AUTO_TEACHING_MOVE_INIT_CHECK_MOVE_DOWN_HOIST	    ,        // ƼĪ��ġ���� 20mm ������ �̵�.


	//1.---------���� ��ġ �غ� Ȯ�� �� �̵�
//	AUTO_TEACHING_P_TEACHING_CMD			,	// Free Teaching 1ȸ (FT) ---AT�� ���
//	AUTO_TEACHING_P_TEACHING_WAIT			,
//
//	AUTO_TEACHING_MOVE_FREE_CMD				,   // OHT �̵�    --�߽ɷ� ��ġ ����. �ٽ� Ȩ���� �̵� �Ұ��, �ش� ��ġ�� �ٷ� �̵�.
//	AUTO_TEACHING_MOVE_FREE_WAIT			,

	//1.---------Focus Ȯ�� �� �̵�
	N_AUTO_TEACHING_F_TEACHING_CMD			,	// Focus Teaching 1ȸ (FT) ---AT�� ���
	N_AUTO_TEACHING_F_TEACHING_WAIT			,

	N_AUTO_TEACHING_MOVE_FOCUS_CMD			,   // OHT �̵�    --��Ŀ�� ��ġ ����. �ٽ� Ȩ���� �̵� �Ұ��, �ش� ��ġ�� �ٷ� �̵�.
	N_AUTO_TEACHING_MOVE_FOCUS_WAIT			,

///////////////////////////////////////////////////////////
	//2. --------------AT �ݺ� ���� ---����-------------------//
///////////////////////////////////////////////////////////
	N_AUTO_TEACHING_TEACHING_CMD				,   // Auto Teaching �ݺ� (AT)   ---AT�� ���
	N_AUTO_TEACHING_TEACHING_WAIT				,	// ƼĪ ������� �Ѿ����, ������������ ������ Ȯ���ϰ�,
												// �����ȿ� ������ AUTO_TEACHING_COMPLETE�� �̵��Ѵ�.

	//STB��
	N_AUTO_TEACHING_MOVE_UP_HOIST_STB      			,      //Home���� �̵�
	N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB		    ,

	//------------������ �̵� ----------//   Limit �Ÿ� ���� �Ѱ�.. �ʱ� ��ġ�� ����(+- ����.)
	N_AUTO_TEACHING_MOVE_X_POS_CMD			,   // OHT �̵�   ( ���� õõ�� �����ӵ� ���� )
//	AUTO_TEACHING_MOVE_X_POS_WAIT			,

	//Shift �� �̵�
	N_AUTO_TEACHING_MOVE_Y_POS_CMD			,
	N_AUTO_TEACHING_MOVE_Y_POS_WAIT			,   //�̵� �� ���� ����⿡ ���� ���Žð� ���� ��� ���� �ݺ� ���� �� ��.

	N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB      			,      //Home���� �̵�
	N_AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB		    ,

//	//------------Teaching��ġ �̵�(������ �̵� ������, Teaching ��ġ�� �ٷ� �̵� ----//
//    AUTO_TEACHING_MOVE_T_POINT_CMD	    ,
//	AUTO_TEACHING_MOVE_T_POINT_WAIT	        ,
///////////////////////////////////////////////////////////
///---------------AT �ݺ� ���� ---��------------------//
///////////////////////////////////////////////////////////

	//3. ---------Hoist �� �̵�
	N_AUTO_TEACHING_Z_TEACHING_CMD			,	// Z�� ƼĪ�� �ѹ��� �ϰ� �Ѿ��   ---AT�� ���
	N_AUTO_TEACHING_Z_TEACHING_WAIT			,

		//STB��
	N_AUTO_TEACHING_MOVE_UP_HOIST_STB_Z      			,      //Home���� �̵�
	N_AUTO_TEACHING_MOVE_CHECK_UP_HOIST_STB_Z		    ,

		//Shift �� �̵�
	N_AUTO_TEACHING_MOVE_Y_POS_CMD_Z			,
	N_AUTO_TEACHING_MOVE_Y_POS_WAIT_Z			,   //�̵� �� ���� ����⿡ ���� ���Žð� ���� ��� ���� �ݺ� ���� �� ��.

	N_AUTO_TEACHING_MOVE_DOWN_HOIST_STB_Z      			,      //Home���� �̵�
//	AUTO_TEACHING_MOVE_CHECK_DOWN_HOIST_STB_Z		    ,

//	AUTO_TEACHING_HOIST_MOVE_CMD			,      // OHT �̵�
//	AUTO_TEACHING_HOIST_MOVE_WAIT			,

	N_AUTO_TEACHING_CHECK_PUSH_HAND			,

	N_AUTO_TEACHING_SAVE_CURRENT_POS_CMD		,



	N_AUTO_TEACHING_CHECK_HAND_CLOSE_OPEN			   	, //soming :Hoist ���Ʒ� seq ���� ��, Hand OPEN ����

	N_AUTO_TEACHING_MOVE_COMP_UP_HOIST     			,      //Home���� �̵�
	N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_UP_HOIST	    ,
   /*
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_FAST             ,     //soming 10�ܰ�
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_FAST     ,
	AUTO_TEACHING_U_MOVE_DOWN_HOIST_SLOW          ,
	AUTO_TEACHING_U_CHECK_MOVE_DOWN_HOIST_SLOW   ,
	AUTO_TEACHING_U_CHECK_ARRIVE_TEACHING_POINT  ,
	AUTO_TEACHING_U_GRIP_FOUP                    ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_SLOW           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_SLOW     ,
	AUTO_TEACHING_U_MOVE_UP_HOIST_FAST           ,
	AUTO_TEACHING_U_CHECK_MOVE_UP_HOIST_FAST     ,
    */
	N_AUTO_TEACHING_MOVE_COMP_HOME_SHIFTROTATE       			,
	N_AUTO_TEACHING_MOVE_COMP_CHECK_MOVE_HOME_SHIFTROTATE      ,
	N_AUTO_TEACHING_CHECK_ANTIDROP			,

	N_AUTO_TEACHING_COMPLETE					,

} AUTO_TEACHING_NULL_TYPE_STEP;



/*
  : TCP or UDP ��� �κ� �߰�
  : �����̵����� �κ� �߰� .
  : �������� ���õ� �κ� Ȯ��.
*/


class AutoTeaching
{
private:
	HWSet 					*m_pHWSet;
    HWCommon 				*m_pHWCommon;
	VirtualDrivingControl	*m_pVirtualDrivingControl;

	EXECUTE_INFO *m_pExecuteInfo;


    LogUnit *pLogAutoTeaching; ///< Map Make STD���� �α� ����


    AUTO_TEACH_OFFSET defualtOffset;
	DWORD dwAutoTeachingResponseTime;
	//--------Jig�� ��źκ�
	SOCKET ServerSock;
	bool bListenMode;
	char cATIpAddr[20];

	int SocketInit(void);
	void SocketClose();


	int UnicastSendMessage(char* cIpAddr, int iPort, char* buf, int len);
	int ServerListen(char* buf, int& iLen, int& iClientPort);

public:

	AutoTeaching();
	~AutoTeaching();


	//�ӽ� ����
	int iCmd;

	double fHoist;
	double fSlide;
	double fDrive;


	double dHoist_Offset;
	double fSlide_Offset;
	double fDrive_Offset;
	double Total_fDrive_Offset;


	double dSTB_R_Offset;
	double dSTB_L_Offset;

	bool bSTBTeaching;
	int nResult;
	/////////
	///------ Jig�� ��� �κ�
	int ServerBind(int iPort, char* cIpAddr);
	int ServerClose();
	bool IsConnect(); //��� �����޴��� Ȯ��.

	bool IsCheckIPCompare(char *strIP); //��� �����޴��� Ȯ��.

	void SetAutoTeachingJigResponse();
	DWORD GetAutoTeachingJigResponse();


	//// ----Auto Teaching ���� �κ� --���& Data Ȯ��
	int AutoTeaching_SendCommand(char* Buf);  //FT, AT ��
	int UpdateATData();  //��� ����� �����忡�� �����Ұ�.

	int CheckSTBShiftLimitError(int nPortType, double Shift);
	int CheckSTBHoistLimitError(int nPortType, double Hoist);

	int ATSave_Current_Pos(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing);

	int ATSave_NullType_Data(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing,char* cATAddr);

	int Save_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType, int dDriving);
	int Load_AutoTeaching_Param();

	int Save_AutoTeaching_Param_NullType(BYTE PortType);

	//// ----AUto Teaching ���� �κ� --�̵����
	int ATDrivingMove(BYTE PortType);
	int ATDrivingMoveCheck(bool &bCheck);

	void QRDrvingTrigger(BYTE PortType);
	bool QRDataCheck(BYTE PortType, int nTargetID);
	
};


#endif

