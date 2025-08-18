#ifndef Def_PathInfoH
#define Def_PathInfoH

#include <stdio.h>
#include "Def_Variables.h"

#define ID_NONE 0


typedef enum DETECTINFOTYPE_
{
	DETECT_INFO_TYPE_ONLY_UBG 		= 0,
	DETECT_INFO_TYPE_ONLY_PBS 		= 1,
	DETECT_INFO_TYPE_BOTH	 		= 2
}DETECTINFOTYPE;


/**
@brief   ���� H/W Define
@author  ���¿�
@date    2013.08.05
*/
typedef enum STEER_HW_INFO_POSITION_
{
	STEER_HW_INFO_POSITION_NONE		= 0,
	STEER_HW_INFO_POSITION_LEFT		= 1,
	STEER_HW_INFO_POSITION_RIGHT	= 2,
	STEER_HW_INFO_POSITION_BOTH		= 3
}STEER_HW_INFO_POSITION;

/**
@brief   ���� H/W ��� Define
@author  ���¿�
@date    2013.08.05
*/
typedef enum STEER_HW_INFO_CMD_
{
	STEER_HW_INFO_CMD_LEFT	= 0,
	STEER_HW_INFO_CMD_RIGHT	= 1
}STEER_HW_INFO_CMD;

//1	������  PATH_STEERING_INFO_�� ���� ���� 
/**
@brief   ���� Define
@author  ���¿�
@date    2012.11.12
*/
typedef enum STEERING_INFO_
{
	STEERING_NONE			= 0,	///< Steering ���� ����
	STEERING_LEFT			= 1,	///< Left Steering
	STEERING_RIGHT			= 2,	///< Right Steering
	STEERING_N_BRANCH_L2R	= 3,	///< N�б� Left to Right
	STEERING_N_BRANCH_R2L	= 4,	///< N�б� Right to Left
	STEERING_MOVING			= 5		///< Steering �̵� ��
}STEERING_INFO;

/**
@brief   Path�� ���� Define
@author  ���¿�
@date    2012.11.12
*/
typedef enum PATH_STEERING_INFO_
{
	PATH_STEERING_NONE			= STEERING_NONE,			///< Steering ���� ����
	PATH_STEERING_LEFT			= STEERING_LEFT,			///< Left Steering
	PATH_STEERING_RIGHT			= STEERING_RIGHT,			///< Right Steering
	PATH_STEERING_N_BRANCH_L2R	= STEERING_N_BRANCH_L2R,	///< N�б� Left to Right
	PATH_STEERING_N_BRANCH_R2L	= STEERING_N_BRANCH_R2L,	///< N�б� Right to Left
}PATH_STEERING_INFO;

///**
//@brief   Path�� �б� Define
//@author  ���¿�
//@date    2016.11.09
//*/
//typedef enum PATH_DIV_INFO_
//{
//	PATH_STEERING_NONE			= 0,			///< Steering ���� ����
//	PATH_STEERING_LEFT			= 1,			///< Left Steering
//	PATH_STEERING_RIGHT			= 2,			///< Right Steering
//}PATH_DIV_INFO;

/**
@brief   Path�� ���� Define
@author  ���¿�
@date    2012.11.12

  ����, ��ֹ� ���� ������ ��� ��...

 */
typedef enum PATH_DIRECTION_INFO_
{
	PATH_DIRECTION_NO_USE	= 0,	///< �̻��: ���� ����
	PATH_DIRECTION_NORMAL	= 1,	///< ���� ����: ���� N
	PATH_DIRECTION_LEFT		= 2,	///< ����: ���� L
	PATH_DIRECTION_RIGHT	= 3,	///< ������: ���� R
	PATH_DIRECTION_SHORT	= 4,	///< N�б� ���: ���� S
	PATH_DIRECTION_BOTH		= 5,	///< ª�� S���� �: ���� B
	PATH_DIRECTION_NARROW	= 6,		///< ���� ������ 900mm �����϶��� ª�� S���� �: ���� V
	PATH_DIRECTION_HILL		= 7,		///< Slope ����: ���� V
	PATH_DIRECTION_SPECIAL_L = 11,		///< Ư�� �б�(N�б�)
	PATH_DIRECTION_SPECIAL_R = 12,		///< Ư�� �б�(N�б�)
	PATH_DIRECTION_SPECIAL_C_L = 13,		///< Ư�� �б�(4ä�� �)
	PATH_DIRECTION_SPECIAL_C_R = 14,		///< Ư�� �б�(4ä�� �)
	//validation �߰��� ���� �߰��� ��ȣ ������� �����ؾ� ��.
}PATH_DIRECTION_INFO;

/**
@brief   Path�� ���� Define
@author  ���¿�
@date    2012.11.12

  ����, ��ֹ� ���� ������ ��� ��...

 */
typedef enum PATH_OBS_DIRECTION_INFO_
{
	PATH_OBS_DIRECTION_NO_USE	= 0,	///< �̻��: ���� ����
	PATH_OBS_DIRECTION_NORMAL	= 1,	///< ���� ����: ���� N
	PATH_OBS_DIRECTION_LEFT		= 2,	///< ����: ���� L
	PATH_OBS_DIRECTION_RIGHT	= 3,	///< ������: ���� R
	PATH_OBS_DIRECTION_SHORT	= 4,	///< N�б� ���: ���� S
	PATH_OBS_DIRECTION_BOTH		= 5,	///< ª�� S���� �: ���� B
	PATH_OBS_DIRECTION_NARROW	= 6,		///< ���� ������ 900mm �����϶��� ª�� S���� �: ���� V
	PATH_OBS_DIRECTION_STB_L 	= 7,		///< Ư�� �б�(N�б�)
	PATH_OBS_DIRECTION_STB_R 	= 8,		///< Ư�� �б�(N�б�)
	PATH_OBS_DIRECTION_NBreanch_L = 9,		///< Ư�� �б�(4ä�� �)
	PATH_OBS_DIRECTION_NBreanch_R = 10,		///< Ư�� �б�(4ä�� �)
	PATH_OBS_DIRECTION_UTrun_L = 11,		///< Ư�� �б�(4ä�� �)
	PATH_OBS_DIRECTION_UTrun_R = 12,		///< Ư�� �б�(4ä�� �)
	PATH_OBS_DIRECTION_Bay = 13,		///< Ư�� �б�(4ä�� �)
	//validation �߰��� ���� �߰��� ��ȣ ������� �����ؾ� ��.
}PATH_OBS_DIRECTION_INFO;

/**
@brief   Path�� �б� ���� Define
@author  ���¿�
@date    2016.11.12
*/
typedef enum PATH_DIVERGENCE_INFO_
{
	PATH_DIVERGENCE_NONE	= 0,   	///< �б� ����
	PATH_DIVERGENCE_LEFT	= 1,	///< ���ʺб�
	PATH_DIVERGENCE_RIGHT	= 2		///< �����ʺб�

}PATH_DIVERGENCE_INFO;

///**
//@brief   Path�� ����  Define
//@author  ���¿�
//@date    2012.11.12
//*/
//typedef enum _SLOPE_NODE_INFO_
//{
//	NODE_INIT		= 0,	///< �ʱⰪ
//	NODE_NORMAL		= 1,   	///< �Ϲ� ���
//	NODE_SLOPE		= 2   	///< ������ ���
//
//}SLOPE_NODE_INFO;



/**
@brief   ���� Path�� ���¿� ���� Node�� Type
@author  puting
@date    2015.11.09
*/
typedef enum PASSPERMIT_NODE_TYPE_
{
	NORMAL_NTYPE		= 1,		///< NORMAL_POINT = �Ϲ�
	STOP_NTYPE			= 2,		///< ���� Node
	RESET_NTYPE			= 3,		///< RESET Node
	COMMON_NTYPE		= 4		///< ���� Node
}PASSPERMIT_NODE_TYPE;

/**
@brief   ���� Path�� ���¿� ���� Node�� Type
@author  puting
@date    2015.11.09
*/
typedef enum DEVICE_NODE_TYPE_
{
/*
	NONE_TYPE		= 0,		///< None = �Ϲ�
	LIFTIN_NTYPE 	= 1,		///< lift In Node
	LIFTOUT_NTYPE  	= 2,		///< Lift Out Node
	MTLIN_NTYPE		= 3,		///< MTL In Node
	MTL_NTYPE   	= 4,		///< MTL ���� Node
	MTLOUT_NTYPE   	= 5,		///< MTL Out Node
	SLOPE_NTYPE   	= 6			///< Slope
*/

	NONE_TYPE			= 0x00,		///< None = �Ϲ�
	SLOPE_NTYPE   		= 0x01,	 	///< Slope

	// for change node (jhoun.lim)
	OPT_TAG				= 0x05,		///< Optional Node (Tag-type)
	OPT_DISTANCE		= 0x06,		///< Optional Node (Distance-type)
	OPT_COMBO			= 0x07,		///< Optional Node (Combo-type)

	LIFTPRE_NTYPE_LEFT 	= 0x10,		///< lift In Node
	LIFTPRE_NTYPE_RIGHT	= 0x11,		///< lift In Node
	LIFTIN_NTYPE_LEFT 	= 0x12,		///< lift In Node
	LIFTIN_NTYPE_RIGHT 	= 0x13,		///< lift In Node
	LIFTOUT_NTYPE  		= 0x14,		///< Lift Out Node

	MTL_SEL_ON_LEFT		= 0x20,		///< MTL In Node
	MTL_SEL_ON_RIGHT	= 0x21,		///< MTL In Node
	MTL_STOP_LEFT		= 0x22,		///< MTL In Node
	MTL_STOP_RIGTH		= 0x23,		///< MTL In Node

	INIT_NTYPE			= 0xFF,		///< None = �Ϲ�


}DEVICE_NODE_TYPE;



/**
@brief   ���� Path�� ���¿� ���� Node�� Type
@author  ���¿�
@date    2012.11.12
*/
typedef enum NODE_TYPE_
{
	NORMAL_POINT		= 0,		///< NORMAL_POINT = �Ϲ�
	BRANCH_POINT		= 1,		///< BRANCH_POINT = �б�
	MERGE_POINT			= 2,		///< MERGE_POINT = �շ�
	MERGE_BRANCH_POINT	= 3			///< �շ�, �б�
}NODE_TYPE;

/**
@brief   Mark�ϴ� ������ Type
@author  ���¿�
@date    2012.11.12
*/
typedef enum MARK_TYPE_
{
	MARK_TYPE_NODE				= 0,		///< DRIVING_TAG = ���� TAG
	MARK_TYPE_TAG_STATION		= 1,		///< TRANS_POINT_TAG = ������ TAG
	MARK_TYPE_QR_STATION_LEFT	= 2,		///< TRANS_POINT_QR_LEFT = ������ QR TAG Left
	MARK_TYPE_QR_STATION_RIGHT	= 3, 		///< TRANS_POINT_QR_RIGHT = ������ QR TAG Right
	MARK_TYPE_OFFSET  	 		= 4,
	MARK_TYPE_TAG_STATION_BACK	= 5			///< TRANS_POINT_TAG = ������ TAG
}MARK_TYPE;

/**
@brief   Mark�ϴ� Sensor Maker Type
@author  puting
@date    2016.02.03
*/
typedef enum MAKER_TYPE_
{
	MAKER_TYPE_KEYENCE	   	= 0,	///< Ű�ؽ��� ����
	MAKER_TYPE_OMRON 		= 1,		///< �ȷл� ����
	MAKER_TYPE_DM50 		= 2,		///< �ڵ�н��� ����
	MAKER_TYPE_MS	 		= 3		///< �ȷл� ����

}MAKER_TYPE;

#endif