/**
@file    VirtualIdReader.h
@version OHT 7.0
@brief   VirtualIdReader Class Header File
*/

#ifndef VirtualIdReaderH
#define VirtualIdReaderH

#include <stdarg.h>
#include <stdio.h>

typedef enum ID_READER_TYPE_
{
	ID_READER_TYPE_ID_ONLY			= 0,
	ID_READER_TYPE_ID_AND_OFFSET
}ID_READER_TYPE;


/**
@class   VirtualIdReader
@date    
@author  
@brief   VirtualIdReader ���� Class
*/
class VirtualIdReader
{
private:

protected:
	ID_READER_TYPE m_Type;
	
public:
    VirtualIdReader(){m_Type = ID_READER_TYPE_ID_ONLY;}
    virtual ~VirtualIdReader(){;}		///< ���� �Ҹ���
    
//	virtual	int SetResultPointer(int* pnResultID) = 0;	///< Read Buffer �� �� �ڵ����� ���� �޴� ���� ����
	virtual int SetResultPointer(int* pnResultID, ...) = 0;	///< Read Buffer �� �� �ڵ����� ���� �޴� ���� ����
	
	virtual int SendTriggerOn() = 0;	///< Trigger On ��ȣ�� ���� : ��� ���·� ID �߰��ϸ� ����
	virtual int SendTriggerOff() = 0;	///< Trigger Off ��ȣ�� ����: ��� ���� ����
	virtual int ReadBuffer() = 0;		///< ���� Data�� Ȯ����
	virtual int	GetLatestID() = 0;		///< ���ŵ� ID�� ����
	
	virtual int Enable() = 0;
	virtual void Disable() = 0;
	virtual int ReStart() = 0;

};

#endif  // VirtualIdReader

