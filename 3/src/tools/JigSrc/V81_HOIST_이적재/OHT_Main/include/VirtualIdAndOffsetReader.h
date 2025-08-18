/**
@file    VirtualIdAndOffsetReader.h
@version OHT 7.0
@brief   VirtualIdAndOffsetReader Class Header File
*/

#ifndef VirtualIdAndOffsetReaderH
#define VirtualIdAndOffsetReaderH

#include "VirtualIdReader.h"

/**
@class   VirtualIdAndOffsetReader
@date    
@author  
@brief   VirtualIdAndOffsetReader ���� Class
*/
class VirtualIdAndOffsetReader: public VirtualIdReader
{
private:
	
public:
    VirtualIdAndOffsetReader(){m_Type = ID_READER_TYPE_ID_AND_OFFSET;}
    virtual ~VirtualIdAndOffsetReader(){;}		///< ���� �Ҹ���
    
	//virtual	int SetResultPointer(int* pnReadedID, double* pdReadedOffset) = 0;	///< Read Buffer �� �� �ڵ����� ���� �޴� ���� ����
	virtual double	GetLatestOffset() = 0;

};

#endif  // VirtualIdAndOffsetReader

