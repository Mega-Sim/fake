/**
@file    HwMask.cpp
@version OHT 7.0
@brief   HwMask Class CPP File
*/

#include "HwMask.h"

/**
 @brief   HwMask Class ������
 @author  //
 @date    //
 @return  //
 */
HwMask::HwMask(int nCountOfInput, int nCountOfOutput, int nCountOfAxis)
{
	m_nCountOfInput		=	nCountOfInput;
	m_nCountOfOutput	=	nCountOfOutput;
	m_nCountOfAxis		=	nCountOfAxis;	

	m_bMakeTraceLog		=	false;
	
	m_aInput	= new MASK_INPUT	[m_nCountOfInput		];	
	m_aOutput	= new MASK_OUTPUT	[m_nCountOfOutput	];	
	m_aAxis		= new MASK_AXIS		[m_nCountOfAxis		];		
}

/**
 @brief   HwMask Class �Ҹ���
 @author  //
 @date    //
 @return  //
 */
HwMask::~HwMask() 
{
	delete [] m_aInput	;
	delete [] m_aOutput	;
	delete [] m_aAxis	;
}

/**
 @brief		Input Mask�� �����ִ� �Լ�
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Input Mask�� ��ȣ
 @return	�ش� Mask�� ���� ��� Mask�� ������ ��ȯ, �ƴ� ��� NULL ��ȯ
 */
MASK_INPUT*		HwMask::getMaskOfInput	(int nNum)
{
	MASK_INPUT	*pMask = NULL;
	if(nNum<m_nCountOfInput)
	{
		pMask = &m_aInput[nNum];
	}
	return pMask;
}

/**
 @brief		Output Mask�� �����ִ� �Լ�
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Output Mask�� ��ȣ
 @return	�ش� Mask�� ���� ��� Mask�� ������ ��ȯ, �ƴ� ��� NULL ��ȯ
 */
MASK_OUTPUT*		HwMask::getMaskOfOutput	(int nNum)
{
	MASK_OUTPUT	*pMask = NULL;
	if(nNum<m_nCountOfOutput)
	{
		pMask = &m_aOutput[nNum];
	}
	return pMask;
}

/**
 @brief		Axis Mask�� �����ִ� �Լ�
 @author	ehoto
 @date		2013.02.16
 @param		nNum : Axis Mask�� ��ȣ
 @return	�ش� Mask�� ���� ��� Mask�� ������ ��ȯ, �ƴ� ��� NULL ��ȯ
 */
MASK_AXIS*		HwMask::getMaskOfAxis	(int nNum)
{
	MASK_AXIS	*pMask = NULL;
	if(nNum<m_nCountOfAxis)
	{
		pMask = &m_aAxis[nNum];
	}
	return pMask;
}


/**
 @brief	Trace Log ������ ��û�ϴ� �Լ�
 @author	���¿�
 @date	2013.03.05
 */
void	HwMask::requsetToMakeTraceLog()
{
	m_bMakeTraceLog = true;
}

