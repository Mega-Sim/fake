#ifndef Def_PathSearchIniParamH
#define Def_PathSearchIniParamH

#include <stdio.h>
#include "MinMaxCtrl.h"	//MIN_MAX_VALUE


/**
@brief	Path Search �� ����㰡 ���� �Ķ���� 
@date    2016.11.26
@author  ���¿�
*/
struct PATHSEARCH_PARAM_SET
{
	int		nMapSearchMax;
	
	int		nPassPermitPeriod;      ///< ����㰡 ��û �ֱ� 
	int		nPassPermitCount;		///< ����㰡 ��û �Ÿ� ����
	double	dPassPermitOpenMargin;	///< ����㰡 ���� �Ÿ� ����
	int     nPassPermitRollbackTime;
	int     nPassPermitRollbackFinshTime;
	double	dPassPermitMaxMargin;	///< ����㰡 ���� �Ÿ� ����
	double	dPassPermitRollbackMargin;	///< ����㰡 ���� �Ÿ� ����
	int     nIsCurveSpeedUp;    //���ϰ� ���� ���, ��� �ӵ� 0.7m/s�� ���� ����

	int 	nIsShoutCurveSpeedUpValue;
	int 	nIsCurveSpeedUpValue;

	char 	Version[8];
};

/**
@brief	 UBG Check ���� ��� �Ķ���� �޾ƿ���
@author  soming
@date	 2018.10.15
*/
#define MAX_JIKJIN_NODE_CHECK_CNT	2000

struct PATHSEARCH_UBG_CHECK_PARAM_SET
{
	int  ArrNode[MAX_JIKJIN_NODE_CHECK_CNT];
	int  nNodeCnt;

	PATHSEARCH_UBG_CHECK_PARAM_SET()
	{	ZeroMemory(this, sizeof(*this));
	}


	bool ExistsCheck(int nNode)
	{

		for(int i=0 ; i<nNodeCnt ; i++)
		{
			if(ArrNode[i] == nNode)
				return true;
		}
		return false;
	}


};

struct PATHSEARCH_NODE_LANCUT_PARAM_SET
{
	int  ArrNode[MAX_JIKJIN_NODE_CHECK_CNT];
	int  nNodeCnt;

	PATHSEARCH_NODE_LANCUT_PARAM_SET()
	{	ZeroMemory(this, sizeof(*this));
	}


	bool ExistsCheck(int nNode)
	{

		for(int i=0 ; i<nNodeCnt ; i++)
		{
			if(ArrNode[i] == nNode)
				return true;
		}
		return false;
	}


};


#endif
