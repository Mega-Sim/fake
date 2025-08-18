//---------------------------------------------------------------------------

#ifndef MTLPIOH
#define MTLPIOH
//---------------------------------------------------------------------------

#include "HWCommon.h"

class MTLPIO
{
private:
	HWCommon *m_pHWCommon;

public:

	MTLPIO(HWCommon *pHWCommon);
	~MTLPIO();

	int PIOEnable();
	int AllPIOOff();
	int PIODisable(bool bOutClear);

//	bool IsPBAVBLOn();
//	bool IsESOn();

	bool IsGoSignalOn();

	//5�� PIO
	bool IsTakeOutOkOn_LR();
	bool IsTakeOutNGOn_UR();
	bool IsMoveOKOn_RD();
	bool IsFrontMoveOn_HA();
	bool IsMTLAutoOn();

	// 8�� PIO �߰� ���� puting
//	bool IsAutoMode_PIO8();    //�̻��
	//����
	bool IsMove_Back_PIO8();
	bool IsMove_Front_PIO8();
	bool IsMoveOK_PIO8();

	//������ & ��ȸ��
	bool IsTakeOutNG_PIO8();   //���� �Ұ�
	bool IsTakeOutOK_PIO8();   //��������
	bool IsTakeInComplete_PIO8();   //���� �Ϸ� ??

    //�°���
	bool IsTakeOutComplete_PIO8();
//	bool IsTakeInReq_Rail_2_PIO8();    // �̻��
	bool IsTakeInReq_Rail_1_PIO8();
	//////////////////////////////////


	int OHTInpositionOnOff_TR(bool bOnOff);
	int TakeoutReqOnOff_CS1(bool bOnOff);
	int TakeInNGOnOff(bool bOnOff);
	int TakeInOKOnOff_VA(bool bOnOff);

//	int Check_PB_AVBL_OffTime(int nPeriodTime);
//	int Check_ESOffTime(int nPeriodTime);
	int Check_GoOffTime(int nPeriodTime);
};

#define PASSBOX_PIO_CHECK_GO_SIGNAL_WAIT_TIME				60000
#define PASSBOX_PIO_CHECK_ES_SIGNAL_WAIT_TIME				60000
#define PASSBOX_PIO_CHECK_PB_AVBL_SIGNAL_WAIT_TIME			60000
#define PASSBOX_PIO_CHECK_CLOSE_ON_SIGNAL_TA1				60000
#define PASSBOX_PIO_CHECK_BUSY_ON_SIGNAL_TA2				60000
#define PASSBOX_PIO_CHECK_CLOSE_OFF_SIGNAL_TA3				60000
#define PASSBOX_PIO_CHECK_BUSY_OFF_SIGNAL_TA4				60000
#define PASSBOX_PIO_CHECK_OPEN_ON_WAIT_TIME					60000
#define PASSBOX_PIO_CHECK_ABNORMAL_SIGNAL_WAIT_TIME			60000

#define PASSBOX_PIO_CHECK_GO_SIGNAL_CHECK_TIME				200
#define PASSBOX_PIO_CHECK_ES_SIGNAL_CHECK_TIME				200
#define PASSBOX_PIO_CHECK_PB_AVBL_SIGNAL_CHECK_TIME			200

#endif

