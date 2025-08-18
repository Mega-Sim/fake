#ifndef AMC_ProtocolH
#define AMC_ProtocolH

#include <stdio.h>
#include "Def_MotionControler.h"
#include "../crc16.h"

#define USE_CRC_CHECK

// -------------------------------------------
//1	Receive Data
// -------------------------------------------

// AMC Data To Get
typedef struct _AMC_DATA
{
	// Input/Output Data
	volatile int	Input[4];						///< InPut ����
	volatile int	Output[4];					///< Output ����

	// Axis Data
	volatile int	Source[4];					///< �ະ source
	volatile int	State[4];						///< �ະ state
	volatile char	Running[4];					///< �ະ �����̴� �� Ȯ��
	volatile char	InPosition[4];				///< �ະ ���� �Ϸ� Ȯ��
	volatile char	AmpEnable[4];					///< �ະ ���� ��� ���� Ȯ��(Servo On, ~~)
	volatile int	Position[4];					///< �ະ ��ġ (Cts)
	volatile int	Velocity[4];					///< �ະ �ӵ� (Cts/sec)
	volatile int	FollowingError[4];			///< �ະ ���� (Cts)
	volatile int	PositionOfSmallAddCheck[4];	///< Small Add ���� �� ���� ���� ��ġ�� ( �̰���,�̻�� �� 0.0 / ���� ��ġ�� 0�� ��� 1�� ���� )  (Cts)

	// AMC Log
	volatile int	TargetVel[4];					///< Sample �� �ӵ� Reffernce
	volatile int	TargetPos[4];					///< Sample �� ��ġ
	volatile int	FinalPos[4];					///< ������ ��ġ

	// 2018.07.26
	// AxisFlag1[1] �� CRC�÷��׷� ����ο��� ������
	volatile int    AxisFlag1[4];

	volatile int	AxisFlag2[4];
#if 1	// ���� �߰� �� AMC �̻� �������� �켱 ����
	volatile int	VolatageMonitor[2];
#endif	
	volatile int 	PauseState;
	NODE_CHECK_DATA NodeCheckData;				///< OHTv7 �� ���� �߰� ����

#ifdef USE_CRC_CHECK	// ����ο��� CRC ���� �ɶ� ifdef ���� ���� ********************


	// ���� ����ü�� CRC �� ���ؼ� Crc16 �� ����
	void MakeCRC16()
	{
		int *pnCrc16 = (int*)&AxisFlag1[1];    // CRC ������
		*pnCrc16 = 0;  // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ �� ���¿��� crc �� ����
		*pnCrc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC üũ
	// ���� ����ü�� Crc16 ����, ���� CRC���� ���Ͽ� Validation Check
	bool CrcCheck()
	{
		int *pnCrc16 = (int*)&AxisFlag1[1];   // CRC ������
		int OrgVal = *pnCrc16;	// CRC ���� �� ����
		*pnCrc16 = 0;   // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ ������ crc�� ����
		unsigned short CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		*pnCrc16 = OrgVal;		// CRC �� ����

		if(OrgVal != CalcVal)    // ���� �ٸ��� error
			return false;

		return true;
	}

	#endif
	
}AMC_DATA;
// -------------------------------------------


// -------------------------------------------
//1	Send Data
// -------------------------------------------

#if(USE_CHANGENODE == 1)
// AMC CMD Structure
typedef struct _AMC_CMD
{
	volatile int	Cmd;			///< bit ���� ��� : CMD_TYPE bitfield ����
	volatile int	Output[4];	///< Output ����
	CMD_AXIS		CmdAxis[4];	///< �� ��� ����
	DETECT_INFO		DetectInfo;	///< ���� ���� Type ������ ���� ����
	PAUSE_INFO		PauseInfo;

	OPTIONAL_NODE_INFO OptNodeInfo;

	#ifdef USE_CRC_CHECK	// ����ο��� CRC ���� �ɶ� ifdef ���� ���� ********************

	volatile int 	Crc16;	///< CRC Check

	// ���� ����ü�� CRC �� ���ؼ� Crc16 �� ����
	void MakeCRC16()
	{
		Crc16 = 0;  // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ �� ���¿��� crc �� ����
		Crc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC üũ
	// ���� ����ü�� Crc16 ����, ���� CRC���� ���Ͽ� Validation Check
	bool CrcCheck()
	{
		int OrgVal = Crc16;	// CRC ���� ���� ����
		Crc16 = 0;   // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ ������ crc�� ����
		int CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		Crc16 = OrgVal;		// CRC �� ����
		if(OrgVal != CalcVal)    // ���� �ٸ��� error
			return false;

		return true;
	}
	#endif

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6��  8.1���� �������Ҷ��� �̺κ��� �ּ�ó��
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}AMC_CMD;
#else
typedef struct _AMC_CMD
{
	volatile int	Cmd;			///< bit ���� ��� : CMD_TYPE bitfield ����
	volatile int	Output[4];	///< Output ����
	CMD_AXIS		CmdAxis[4];	///< �� ��� ����
	DETECT_INFO		DetectInfo;	///< ���� ���� Type ������ ���� ����
	PAUSE_INFO		PauseInfo;

	#ifdef USE_CRC_CHECK	// ����ο��� CRC ���� �ɶ� ifdef ���� ���� ********************

	volatile int 	Crc16;	///< CRC Check

	// ���� ����ü�� CRC �� ���ؼ� Crc16 �� ����
	void MakeCRC16()
	{
		Crc16 = 0;  // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ �� ���¿��� crc �� ����
		Crc16 = crc16_ccitt((const char*)this, sizeof(*this));
	}

	// CRC üũ
	// ���� ����ü�� Crc16 ����, ���� CRC���� ���Ͽ� Validation Check
	bool CrcCheck()
	{
		int OrgVal = Crc16;	// CRC ���� ���� ����
		Crc16 = 0;   // CRC �� �ʱ�ȭ

		// CRC�� �ʱ�ȭ ������ crc�� ����
		int CalcVal = crc16_ccitt((const char*)this, sizeof(*this));
		Crc16 = OrgVal;		// CRC �� ����
		if(OrgVal != CalcVal)    // ���� �ٸ��� error
			return false;

		return true;
	}
	#endif

#if((OHT_NAME == OHT_NAME_STD_V81) || (OHT_NAME == OHT_NAME_STD_V86))  //timy v8.2 8.6��  8.1���� �������Ҷ��� �̺κ��� �ּ�ó��
	int m_nSound;
	int m_nReserved_cmd1;
	int m_nReserved_cmd2;
#else
#endif

}AMC_CMD;
#endif

#endif
