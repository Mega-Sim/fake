//---------------------------------------------------------------------------

#ifndef JigThreadH
#define JigThreadH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include "ThreadCtrl.h"

#include "LogHandler.h"
#include "LogThread.h"
#include "MyTools.h"
#include "OHTMainStatus.h"
#include "DrivingControl.h"

// 2019.07.05 Jig Test �Ķ����

typedef struct JigParam__
{
	// Hoist Jig Test param
	double dHoist_TeachingPos; // Hoist �Ʒ��� ���� �� mm
//	double dHoist_HomePos;	   // Hoist ������ ���ߴ� �� mm

	int nHoist_Interval_ms;       // 1ȸ �׽�Ʈ �� ���� interval time
	int nHoist_CycleCnt;		  // ��ȸ �׽�Ʈ ����
	int nHoist_CurrentCnt;		  // ���� ī��Ʈ.

	// Slide Jig Test Param
	double dSlide_SlideTeachingPos;	// Slide �¿������� ���� �� mm
	double dSlide_Acc;
	double dSlide_Vel;

	int nSlide_Interval_ms;       // 1ȸ �׽�Ʈ �� ���� interval time
	int nSlide_CycleCnt;		  // ��ȸ �׽�Ʈ ����
	int nSlide_CurrentCnt;		  // ���� ī��Ʈ.

	// Rotate Jig test Param
	int nRotate_Interval_ms;       // 1ȸ �׽�Ʈ �� ���� interval time
	int nRotate_CycleCnt;          // ��ȸ �׽�Ʈ ����
	int nRotate_CurrentCnt;        // ���� ī��Ʈ.

	// Hand Jig Test Param
	int nHand_Interval_ms;
	int nHand_CycleCnt;
	int nHand_CurrentCnt;

	// Shutter Jig Test Param
	int nShutter_Interval_ms;           // 1ȸ �׽�Ʈ �� ���� interval time
	int nShutter_CycleCnt;				// ��ȸ �׽�Ʈ ����
	int nShutter_CurrentCnt;			// ���� ī��Ʈ.


    JigParam()	{  ZeroMemory(this, sizeof(*this));	}

}JigParam;

enum JIGUNIT
{
    NONE = 0,
	HOIST,
	SLIDE,
	ROTATE,
	HAND,
	SHUTTER

};

enum JIGSTEP
{
	INITIALIZATION = 0,
	LONGRUN,
	ABNORMAL
};


class JigThread : public ThreadCtrl
{
private:

    CRITICAL_SECTION	m_CS;
    LogUnit* pLogJig; // Jig Log ���� ����
	int m_nError;
	int nError;
	int m_nHoistError;
	int m_nSlideError;
	bool bRunning;
	DrivingControl	*m_pDrivingControl;
	DRIVING_HW_INFO_SET JigHwInfo;

protected:

	void RealThreadProc(void);


public:
	JigThread();
	~JigThread();

	void ExecuteHoistSteps();   //Hoist Excute
	void ExecuteSlideSteps();   //Slide Excute
	void ExecuteShutterSteps(); //Shutter Excute
	void ExecuteRotateSteps();  //Rotate Excute
	void ExecuteHandSteps();    //Hand Excute

	int GetJigError();  // JIG Thread���� �����߻����� ��, Main thread���� Error ó���� �ϱ����� �Լ�
	int GetMainError(int nError); 	 //Main Error �߻��� ó��
	int GetShutterError(int nError); //Shutter Error �߻��� ó��
	int GetHoistError(int nError);	 //Hoist Error �߻��� ó��
	int GetSlideError(int nError);	 //Slide Error �߻��� ó��
	int GetRotateError(int nError);	 //Rotate Error �߻��� ó��
	int GetHandError(int nError);	 //Hand Error �߻��� ó��


	// Unit �� Class ����
	CStepClass m_StepJig_Hoist;    //Hoist Step
	CStepClass m_StepJig_Slide;    //Slide Step
	CStepClass m_StepJig_Rotate;   //Shutter Step
	CStepClass m_StepJig_Hand;     //Rotate Step
	CStepClass m_StepJig_Shutter;  //Hand Step

	JigParam m_JigParam;


	int Err_cnt;
	int AlarmReset_cnt;

	int nJogVal;

	SYSTEMTIME CurTime;

	OHTMainStatus *m_pMainStatus;

};
#endif
