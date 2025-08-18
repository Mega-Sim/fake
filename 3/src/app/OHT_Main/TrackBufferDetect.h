//---------------------------------------------------------------------------
/**
@file    TrackBufferDetect.h
@version OHT 7.0
@brief   TransStopTag Class Header File
*/

#ifndef TrackBufferDetectH
#define TrackBufferDetectH


#include "HWCommon.h"

/**
@class   TrackBufferDetect
@date    2012.10.22
@author  LSJ
@brief   TrackBufferDetect ���� Class
@note    STB, UTB Buffer�� �ݼ۹� ������ Ȯ���ϱ� ���� �Լ� ����
*/
class TrackBufferDetect
{
private:
	HWCommon *m_pHWCommon;
	
public:

	TrackBufferDetect(HWCommon *pHWCommon);
	~TrackBufferDetect();

	bool IsLeftSTBFoupDetect();
	bool IsRightSTBFoupDetect();
	bool IsUTBFoupDetect();

};
//---------------------------------------------------------------------------
#endif  //TrackBufferDetectH
