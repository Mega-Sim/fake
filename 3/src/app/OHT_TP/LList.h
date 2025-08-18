//---------------------------------------------------------------------------
/**
@file    LList.h
$Rev:: 467           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-03 1#$:  Date of last commit
@version OHT 7.0
@brief   MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

#ifndef LListH
#define LListH

#include <list>

typedef unsigned int        UINT;   ///< 4byte �ڷ���(unsigned int)

/**
@class   LList
@date    20      12.11.12
@author  ehoto
@brief   OHT�� ���Ǵ� Customȭ�� List class
*/
class LList
{
private:
	std::list<void*> m_LList;	///< ���ο��� ���Ǵ� STL List
	UINT m_nCount;				///< List ���ο� �� �ִ� ���� ����

public:
	LList();
	~LList();

	void insertBack(void* pCmd);		///< List �ڿ� �߰�
	void insertFront(void* pCmd);		///< List �տ� �߰�
	int insert(void* pCmd, UINT nIndex);	///< List ������ ��ġ�� �߰� (ó���� 0, �������� N-1)

	void* referBack(void);		///< List �ڿ� �߰�
	void* referFront(void);		///< List �ڿ� �߰�
	void* refer(UINT nIndex);	///< List �ڿ� �߰�

	void* popFront(void);		///< List �ڿ� �߰�
	void* popBack(void);		///< List �ڿ� �߰�
	void* pop(UINT nIndex);		///< List �ڿ� �߰�

	void clear(void);			///< List Clear�ϴ� �Լ� ~ [����] ���� ���ڰ� �ڵ� �Ҹ��Ű�� ����

	int getCount();				///< List ���� ���� �޾ƿ��� �Լ�
};

#endif
