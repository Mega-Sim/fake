//---------------------------------------------------------------------------
/**
@file    VirtualDetect.h
@version OHT 7.0
@brief   Detect Class Header File
*/

#ifndef VirtualDetectH
#define VirtualDetectH

typedef enum DETECT_AREA_TYPE_
{
	DETECT_AREA_NO_USE	= 0,	// �̻��: ���� ����
	DETECT_AREA_NORMAL	= 1,	// ���� ����: ���� N
	DETECT_AREA_LEFT	= 2,	// ����: ���� L
	DETECT_AREA_RIGHT	= 3,	// ������: ���� R
	DETECT_AREA_SHORT	= 4,	// N�б� ���: ���� S
	DETECT_AREA_BOTH	= 5,	// ª�� S���� �: ���� B
	DETECT_AREA_NARROW	= 6		// ���� ������ 900mm �����϶��� ª�� S���� �: ���� V
}DETECT_AREA_TYPE;

/**
@brief   AREA ���� Define
@author  ehoto
@date    2012.11.12
*/
typedef enum DETECT_STATUS_
{
	DETECT_NONE			= 0,		///< ������ �� ����
	DETECT_AND_MOVE		= 1,		///< �����Ǿ��� �׿� ���� �̵� ���� �ʿ�
	DETECT_AND_STOP		= 2,		///< �����Ǿ� ���� �ʿ�
	DETECT_UNIT_FAIL	= 3 	///< ���� ��ġ �̻�
//	DETECT_UNIT_STEPOUT	= 4		///< ���� ��ġ Ż�� 
}DETECT_STATUS;

/**
@class   VirtualDetect
@date    2013.01.22
@author  ���¿�
@brief   Detect Sensor ���� Class
@note    Detect Sensor�� Area�� �����ϰ� �����ϱ� ���� �Լ� ����
*/
class VirtualDetect
{
private:

public:

	VirtualDetect(){;}
	~VirtualDetect(){;}

	virtual	int				Enable()						= 0;	// Ȱ��ȭ(���� ON)
	virtual	int				Disable()						= 0;	// ��Ȱ��ȭ(���� OFF)

	virtual	DETECT_STATUS	GetStatus()						= 0;	// ���¸� �޾ƿ��� �Լ�
	virtual	void			SetArea(DETECT_AREA_TYPE Type)	= 0;	// ������ �����ϴ� �Լ�
	virtual	double			GetMovableDist()				= 0;	// �̵� ������ �Ÿ��� �޾ƿ��� �Լ�
};

#endif  //VirtualDetectH
