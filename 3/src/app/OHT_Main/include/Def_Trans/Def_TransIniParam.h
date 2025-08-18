#ifndef Def_TransIniParamH
#define Def_TransIniParamH

#include <stdio.h>
#include "AxisParam.h"		// PROPOTINAL_AXIS_PARAM, THREE_LINEAR_EQUATION_AXIS_PARAM
#include "Def_Variables.h"

#define CARRIER_OFFSET_MAX_COUNT	4
#define DEFAULT_SPECIAL_EQ_PARAM 30 //P1L default �ݿ�

/**
@brief	���� �ݼ۹��� ���� Ofsset ó���� ���� ����
@author  ehoto
@date	 2013.08.05
*/
typedef struct CARRIER_OFFSET_
{
	double dHosit;
	double dShift;
}CARRIER_OFFSET;

/**
@brief	 Reticle ���� Port�� ���� Ofsset ó���� ���� ����
@author  ehoto
@date	 2013.08.05
*/
typedef struct
{
	double dPortOffset_Shift;
	double dPortOffset_Hoist;
	int    nPortOffset_Rotate;
}RETICLE_TEACH_OFFSET;

typedef struct TRANS_PARAM_
{
   double dOffset_MoveUp_SpdnOffset;  ///< Hoist Up �� ���ӱ��� Offset
   double dOffset_MoveDn_up_SpdnOffset;  ///< Hoist Down �� ���� ���ӱ��� Offset
   double dOffset_MoveDn_dn_SpdnOffset;  ///< Hoist Down �� ���� ���ӱ��� Offset
   
   int nTrans_Time;  ///< Trans �� Timeover �ð�
   int nLookdownSen_Detect_Time;  ///< Look Down ���� Detect Timeover �ð�
   int nOscillationCheckDist;   ///< 
   int nTransSound;  ///< ������ Sound On, Off

   int nTransStep;  ///< ������ �ܰ�
   int nTransType;  ///< ������ Ÿ��(Load �������, Unload �������)
   int nEqType;  ///< �� ������ ��Ʈ Ÿ��
   int nSTBDirection; //STB ���� (0:EQ,STK 1:RIGHT 2:LEFT)
   int nPIOTimeoverLevel;  ///< ��Ʈ �� EQ PIO Timeover ����

   DWORD dwLREQ_STTime;  ///< Load REQ ���� �ð�
   DWORD dwLookdn_STTime;  ///< LookDown ���� ���� ���� �ð�
   DWORD dwTotalTrans_STTime; ///< �� ������ �ð�
   
   double dHoist_TeachingPos;  ///< Hoist ƼĪ ��
   double dShift_TeachingPos;  ///< Shift ƼĪ ��
   double dRotate_TeachingPos;  ///< Rotate ƼĪ ��
   int nRotate_Pos; //(��� X)
   
   double dCmdHoistPos;  ///< ��� ���� �� Hoist ��� ��ġ(Hoist�� ���� ����� �� ��)
   double dCmdHoistStartPos;  ///< ��� ���� �� Hoist ���� ��ġ
   double dCmdHoistSpeed;  ///< ��� ���� �� Hoist ��� �ӵ�
   double dCmdFinal_HoistPos; ///< ���� Hoist�� ƼĪ ��� ��
   double dCmdShiftPos;  ///< ��� ���� �� Shift ��� ��ġ
   double dCmdRotatePos;  ///< ��� ���� �� Rotate ��� ��ġ

   double dHoistLevel;  ///< Hoist �� �ӵ� ���� ����
   double dShiftLevel;  ///< Shift �� �ӵ� ���� ����

   int nErrCount_GripAbNormalON;  ///< Gripper Sensor �� ������������ On �Ǿ����� �� Ȯ�� Ƚ��
   int nErrCount_GripAbNormalOFF;  ///< Gripper Sensor �� ������������ Off �Ǿ����� �� Ȯ�� Ƚ��

   bool bIsSTBTypeTrans;  ///< STB ���� ������
   bool bIsSTKTypeTrans;  ///< STK ���� ������
}TRANS_PARAM;


typedef struct TRANS_USE_PART_
{
    bool EQPIO;            ///< EQ PIO ��� ����
    bool GripperSen;       ///< Gripper Sensor ��� ����
	bool OscillationSen;   ///< Ocillation Sensor ��� ����
    bool HandPushSen;      ///< HandPush Sensor ��� ����
    bool LookdownSen;      ///< Look Down Sensor ���� Ȯ�� ����
    bool ShutterControl;   ///< Shutter ���� ���� ����
    bool HandSensor;       ///< Hand Sensor ��� ����
    bool HoistHomeSen;       ///< Hoist Align Sensor ��� ����
    bool STBSen;           ///< STB Sensor ��� ����
    bool UTBSen;           ///< UTB Sensor ��� ����
    bool UTBFoupCheck;     ///< UTB Unload�ÿ� ƼĪPoint���� �̵��ؼ� Foup�� üũ ����
    bool IgnorePIOError;   ///< PIO Error ���� ����
    bool TransSound;       ///< ������ Sound ��� ����
    bool ObsDetectSound;   ///< ��ֹ� ���� Sound ��� ����
	bool OhtDetectSound;   ///< ���� ���� Sound ��� ����
//	bool bLookdown_Detected;//??
	bool DrivingSound;       ///< ���� Sound ��� ����
	int  EQPIOType;
	bool OscillationSenPause;
	bool TransEQSound;         ///< ������ ��Ʈ�� Sound ��� ����
	bool TransSTKSound;        ///< ������ ��Ʈ�� Sound ��� ����
	bool TransSTBSound;        ///< ������ ��Ʈ�� Sound ��� ����
#if(OHT_NAME == OHT_NAME_P4_MASK)	//mgchoi. 2022.08.23. P4 MaskOHT��
	bool HandLoadUnlPositionSen;
#endif
	bool bDualStageStbPIO; // 2�� STB�� ���� PIO ��� ����
	bool bPstbPIO;			///<PSTB�� PIO ��� ����
}TRANS_USE_PART;



typedef struct TRANS_DEFINE_
{
#if(OHT_NAME == OHT_NAME_P4_MASK)
	int nTracker_Delay_Time;  ///< Tracker ���� �ð�

	double dHoist_Positive_Limit_OnWait;  ///< ��� �� Hoist ���� Positive Limit
	double dHoist_Negative_Limit_OnWait;  ///< ��� �� Hoist ���� Negative Limit
	double dShift_Positive_Limit_OnWait;  ///< ��� �� Shift ���� Positive Limit
	double dShift_Negative_Limit_OnWait;  ///< ��� �� Shift ���� Negative Limit
	double dHoist_Abnormal_Move_Speed_Limit;  ///< Hoist ���� �̵� �ӵ� Limit
	double dShift_Abnormal_Move_Speed_Limit;  ///< Shift ���� �̵� �ӵ� Limit

	double dHoist_MoveLimit_P;  ///< Hoist ���� Positive �̵� Limit
	double dHoist_MoveLimit_N;  ///< Hoist ���� Negative �̵� Limit
	double dShift_MoveLimit_P;  ///< Shift ���� Positive �̵� Limit
	double dShift_MoveLimit_N;  ///< Shift ���� Negative �̵� Limit
	double dDist_Hoist_Home_Margin;  ///< Hoist ���� ���� ���� �Ÿ�
	double dDist_Shift_Home_Margin;  ///< Shift ���� ���� ���� �Ÿ�

	int nUpstate_SpeedDown_Offset;  ///< Hoist Up �� ���ӱ��� Offset
	int nDnState_UP_SpeedDown_Offset;  ///< Hoist Down �� ���� ���ӱ��� Offset
	int nDnState_DN_SpeedDown_Offset;  ///< Hoist Down �� ���� ���ӱ��� Offset

	int nUpstate_SpeedDown_Offset_Load;  ///< Hoist Up �� ���ӱ��� Offset
	int nDnState_UP_SpeedDown_Offset_Load;  ///< Hoist Down �� ���� ���ӱ��� Offset
	int nDnState_DN_SpeedDown_Offset_Load;  ///< Hoist Down �� ���� ���ӱ��� Offset

	float dHoist_SmallAdd_Dist;
	float dHoist_SmallAdd_Dist_UTB; 	//2025-01-20. mgchoi [AMOHTV80F-1644]
	float dHoist_SmallAdd_Dist_For_UnloadPosition;
	float dHoist_SmallAdd_Antidrop_DownDist;

	float dHoist_SmallAdd_Antidrop_DownDist_RSP150;
	float dHoist_SmallAdd_Antidrop_DownDist_RSP200;
	float dHoist_SmallAdd_Antidrop_DownDist_EUV;

#endif
   int nDnState_UP_SpeedDown_Offset_SpecialEQ;  ///< Hoist Up �� ���� ���ӱ��� Offset
   int nDnState_UP_SpeedDown_Percent_SpecialEQ;  ///< Hoist Up �� ���� ���ӱ��� Offset  0~100

   int nLookdn_Detect_Timeover;  ///< Look Down ���� Detect Timeover �ð�
   double dLookdn_Detect_Check_Dist;  ///< Look Down ���� Detect Check �Ÿ�
   int nTrans_Timeover;  ///< Trans �� Timeover �ð�
   double dOscillation_Check_Dist;  ///<
//   double dHoist_MovInpos_Limit;  ///< ??

   double dShift_Fast_Move_Dist;  ///< Shift �� ��� �̵� �Ÿ�
   double dOscillation_Ignore_Dist;  ///< Oscillation ���� üũ ���� �Ÿ�
   double dFast_Move_Limit;  ///< Hoist ���� ��� �̵� ���� Limit
   double dFast_Move_Limit_Ver85h;

   int nSTBUTBSensorOFFTime;  ///< STB, UTB Sensor �� Off �Ǵµ� �ɸ��� �ð�
   int nSTBUTBSensorONTime;  ///< STB, UTB Sensor �� Off �Ǵµ� �ɸ��� �ð�
   int nRotateManualMoveTimeOver;
//   int nRailsupport_Homing_TimeOut;  ///< Rail support �� Homing �� �����ð�?

//   double dShift_Speed_Ratio;  ///< Shift �� �ӵ� ��

//   double dShift_AccTime;  ///< Shift �� ���� �ð�

//   double dHoist_Max_Speed;  ///< Hoist �� �ְ� �ӵ�
//
//   double dHoist_Pos_Limit;  ///< Hoist �� Positive Software Limit
//   double dHoist_Neg_Limit;  ///< Hoist �� Negative Software Limit
//   double dShift_Pos_Limit;  ///< Shift �� Positive Software Limit
//   double dShift_Neg_Limit;  ///< Shift �� Negative Software Limit

//   double dInpos_Limit;
//   double dShift_Pos_Limit_ForWait;  ///< ��� �� Shift ���� Positive Limit
//   double dShift_Neg_Limit_ForWait;  ///< ��� �� Shift ���� Negative Limit
//   double dHoist_Pos_Limit_TransOffset;  ///< ������ �� Hoist Limit ���� Offset
//
//   double dHoist_Origin_Limit;  ///< Hoist ���� Limit

// 2016-11-28, Add by Labeler, Rotate ���� Timeout ���� �߰�
//   int nRotate_Exec_Timeout;
//   int nDnState_DN_SpeedDown_Speed_Load;  ///< Hoist Down �� ���� ���ӱ��� �ӵ� ����(0 ~ 100%)

//   double dShift_Speed_Ratio_50Down_Load;
//   double dShift_Speed_Ratio_50Up_Load;
//   double dShift_Speed_Ratio_50Down_UnLoad;
//   double dShift_Speed_Ratio_50Up_UnLoad;

//   double dHandPushRetryPos;


//	int nHoistMoveUp_HomeOffset_Load;  ///< Hoist Up �� ���� Offset
//	int nHoistMoveUp_TeachingOffset_Load;  ///< Hoist Up �� ���� ���ӱ��� Offset
//	int nHoistMoveDown_TeachingOffset_Load;  ///< Hoist Down �� ���� ���ӱ��� Offset

//	int nHoistMoveUp_HomeOffset_Unload;  ///< Hoist Up �� ���� Offset
//	int nHoistMoveUp_TeachingOffset_Unload;  ///< Hoist Up �� ���� ���ӱ��� Offset
//	int nHoistMoveDown_TeachingOffset_Unload;  ///< Hoist Down �� ���� ���ӱ��� Offset

//	int nHoistMoveUp_HomeOffset_STB_Load;  ///< Hoist Up �� ���� Offset_STB
//	int nHoistMoveUp_TeachingOffset_STB_Load;  ///< Hoist Up �� ���� ���ӱ��� Offset_STB
//	int nHoistMoveDown_TeachingOffset_STB_Load;  ///< Hoist Down �� ���� ���ӱ��� Offset_STB

//	int nHoistMoveUp_HomeOffset_STB_Unload;  ///< Hoist Up �� ���� Offset_STB
//	int nHoistMoveUp_TeachingOffset_STB_Unload;  ///< Hoist Up �� ���� ���ӱ��� Offset_STB
//	int nHoistMoveDown_TeachingOffset_STB_Unload;  ///< Hoist Down �� ���� ���ӱ��� Offset_STB

}TRANS_DEFINE;

//AOHC-294
typedef struct TRANS_PROFILEDETAIL_
{
	//���� ��������
	int nProfile1_DN_SpeedDown_Offset_Load;  	 	// Hoist Down �� ����    		���ӱ��� Offset
	int nProfile1_DN_TeachingSpeedRate_Load;  	 	// Hoist Down �� ����    		���ӱ��� Velocity
	int nProfile1_DN_SpeedDown_Offset_Unload; 	 	// Hoist Down �� ����    		���ӱ��� Offset
	int nProfile1_DN_TeachingSpeedRate_Unload;	 	// Hoist Down �� ����    		���ӱ��� Velocity
	int nProfile1_UP_SpeedDown_Offset_Unload;	 	// Hoist Up   �� ƼĪ��ġ����   ���ӱ��� Offset
	int nProfile1_UP_TeachingSpeedRate_Unload;      // Hoist Up   �� ƼĪ��ġ����   ���ӱ��� Velocity
	int nProfile1_UP_SpeedDown_Home_Offset_Unload;  // Hoist Up   �� Ȩ��ġ����     ���ӱ��� Offset

	//���� ��������
	int nProfile2_HMD_TeachingOffset_Load;         // Hoist Down �� ����    		���ӱ��� Offset
	int nProfile2_HMD_TeachingSpeedRate_Load;      // Hoist Down �� ����    		���ӱ��� Velocity
	int nProfile2_HMD_TeachingOffset_Unload;       // Hoist Down �� ����    		���ӱ��� Offset
	int nProfile2_HMD_TeachingSpeedRate_Unload;    // Hoist Down �� ����    		���ӱ��� Velocity
	int nProfile2_HMU_TeachingOffset_Unload;       // Hoist Up   �� ƼĪ ��ġ����   ���ӱ��� Offset
	int nProfile2_HMU_TeachingSpeedRate_Unload;    // Hoist Up   �� ƼĪ ��ġ����   ���ӱ��� Velocity
	int nProfile2_HMU_TeachingOffset_Home_Unload;  // Hoist Up   �� Ȩ ��ġ����     ���ӱ��� Offset

	//STB ��������
	int nProfile2_HMD_TeachingOffset_STB_Load;
	int nProfile2_HMD_TeachingOffset_STB_Unload;


}TRANS_PROFILEDETAIL;

/**
@brief	 ���� ���氨�� �Ķ����
@author  ehoto
@date	 2013.07.08
*/
struct TRANS_PARAM_SET
{
	THREE_LINEAR_EQUATION_AXIS_PARAM	HoistAxis;			///< ȣ�̽�Ʈ�� ���� ����
	PROPOTINAL_AXIS_PARAM				ShiftAxis;			///< ����Ʈ�� ���� ����
	
	TRANS_PARAM		TransParam;
	TRANS_USE_PART	TransUsePart;
	TRANS_DEFINE	TransDefine;

//	CARRIER_OFFSET	CarrierOffset[CARRIER_OFFSET_MAX_COUNT];	///< �ݼ۹��� ���� Offset; �켱 FOUP�� 0, MAC�� 1�� ��� --> ������� ���� ����

//	int		CarrierType;
	char	Version[8];
};



#endif
