/**
@file    HwMask.h
@version OHT 7.0
@brief   HwMask Class Header File
*/

#ifndef HwMaskH
#define HwMaskH

#include <stdio.h>

template <typename T>
struct	MASK
{
	bool	bUse;		///< Use/NoUse ����
	T		Value;		///< On/Off ����
	
	MASK()
	{
		bUse	= false;
		Value	= 0;
	}
};

typedef MASK<bool>	MASK_INPUT;
typedef MASK<bool>	MASK_OUTPUT;

typedef	struct	MASK_AXIS_
{
	MASK<double>	Position;		///< ��ġ ����
	MASK<bool>		AmpEnable;		///< AMP Eable ����
	MASK<bool>		Running;		///< ���ۿ��� ����
	MASK<int>		ErrorCode;		///< ���� �ڵ�
    // 2016-12-08, Add by Labeler, Rotate ���� ��� Origin ���� �ʿ�� ���� �߰�
    MASK<int>       Origin;         ///< Motor Origin ����
	MASK<int>	Speed;
//	MASK<double>	Speed;


}MASK_AXIS;

/**
@class   HwMask
@date
@author  ehoto
@brief   HW�� Masking �ϴ� ����ü Ŭ����
*/

class HwMask
{
private:
	unsigned int	m_nCountOfInput;
	unsigned int	m_nCountOfOutput;
	unsigned int	m_nCountOfAxis;
	
public:
	/// I/O ���� ���� ----------------------------------
	bool	m_bMakeTraceLog;

	/// I/O ���� ���� ----------------------------------
	MASK_INPUT	*m_aInput;	///< Input  Mask�� ���� �迭 ������
	MASK_OUTPUT	*m_aOutput;	///< Output Mask�� ���� �迭 ������
	MASK_AXIS	*m_aAxis;	///< Axis   Mask�� ���� �迭 ������

	/// ������ & �Ҹ��� --------------------------------
	HwMask(int nCountOfInput, int nCountOfOutput, int nCountOfAxis);
	~HwMask();
	
	MASK_INPUT*		getMaskOfInput	(int nNum);	///< Input  Mask�� �����͸� �޾ƿ��� �Լ�
//	MASK_OUTPUT*	getMaskOfOutput	(int nNum);	///< Input  Mask�� �����͸� �޾ƿ��� �Լ�         //�̻��
	MASK_AXIS*		getMaskOfAxis	(int nNum);	///< Input  Mask�� �����͸� �޾ƿ��� �Լ�

//	void	requsetToMakeTraceLog();	///< Trace Log ������ ��û�ϴ� �Լ�                       //�̻��

};

#endif  //HwMaskH
