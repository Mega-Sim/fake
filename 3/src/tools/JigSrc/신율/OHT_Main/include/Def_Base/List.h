//---------------------------------------------------------------------------
/**
@file    List.h
@version OHT 7.0
@brief   MAP�� ���õ� Class, ����ü�� ������ Header ����
*/

//#include "stdafx.h"

#ifndef ListH
#define ListH

#include <list>
#include <algorithm>

typedef unsigned int        UINT;   ///< 4byte �ڷ���(unsigned int)

/**
@class   List
@date    20      12.11.12
@author  ehoto
@brief   OHT�� ���Ǵ� Customȭ�� List class
*/
template <typename T>
class List
{
private:
	std::list<T> m_StlList;		///< ���ο��� ���Ǵ� STL List
	UINT m_nCount;				///< List ���ο� �� �ִ� ���� ����

public:
	List();
	~List();

	void insertBack(T Value);			///< List �ڿ� �߰�
	void insertFront(T Value);			///< List �տ� �߰�
	int insert(T Value, UINT nIndex);	///< List ������ ��ġ�� �߰� (ó���� 0, �������� N-1)

	T referBack(void);		///< List �ڿ� �߰�
	T referFront(void);		///< List �ڿ� �߰�
	T refer(UINT nIndex);	///< List �ڿ� �߰�

	T popFront(void);		///< List �ڿ� �߰�
	T popBack(void);		///< List �ڿ� �߰�
	T pop(UINT nIndex);		///< List �ڿ� �߰�

	UINT find(T Value);

	void clear(void);		///< List Clear�ϴ� �Լ� ~ [����] ���� ���ڰ� �ڵ� �Ҹ��Ű�� ����

	UINT getCount();		///< List ���� ���� �޾ƿ��� �Լ�
	UINT getCountsize();
};

/**
@brief List Class default ������

         ����, ������ ���� �ʱ�ȭ
@author  ehoto
@date    2012.11.12
*/
template <typename T>
List<T>::List()
{
	m_nCount=0;
}

/**
@brief List Class default �Ҹ���

@author  ehoto
@date    2012.11.12
*/
template <typename T>
List<T>::~List()
{

}
// Insert : ���� �� ��� ---------------------------------------------------------------------------

/**
@brief   List �ڿ� �����ϴ� �Լ�

         ���� Ÿ���� ������ ����Ʈ�� �ڿ� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@param   Value : �߰��ϴ� ������ ������ ( T )
*/
template <typename T>
void List<T>::insertBack(T Value)
{
	m_StlList.push_back(Value);
	m_nCount++;
}

/**
@brief   List �տ� �����ϴ� �Լ�

         ���� Ÿ���� ������ ����Ʈ�� �տ� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@param   Value : �߰��ϴ� ������ ������ ( T )
*/
template <typename T>
void List<T>::insertFront(T Value)
{
	m_StlList.push_front(Value);
	m_nCount++;
}

/**
@brief   List ���� ��ġ�� �����ϴ� �Լ�

         ���� Ÿ���� ������ ����Ʈ�� ������ ��ġ�� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@param   Value : �߰��ϴ� ������ ������ ( T )
@param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
@return  ���� ���� Return (0: ����, �׿�: ������)
*/
template <typename T>
int List<T>::insert(T Value, UINT nIndex)
{
	if(m_nCount==0) return -1;		///<����ó��
	if(nIndex>=m_nCount) return -1;	///<����ó��

	std::list<T>::iterator iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	m_StlList.insert(iterPos, Value);
	m_nCount++;

	return 0;
}

// Refer : ������ ��� (���ڸ� ������ ����) ---------------------------------------------------------------------------
/**
@brief   List �� ���� ���ڸ� �����ϴ� �Լ�

         ����Ʈ �������� �ִ� ���� Ÿ���� ������ �����Ѵ�.
@author  ehoto
@date    2012.11.12
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::referBack()
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<����ó��

	iterPos = m_StlList.end();
	--iterPos;
	pTemp = (*iterPos);

	return pTemp;
}

/**
@brief   List �� ���� ���ڸ� �����ϴ� �Լ�

         ����Ʈ �� �п� �ִ� ���� Ÿ���� ������ �����Ѵ�.
@author  ehoto
@date    2012.11.12
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::referFront(void)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<����ó��

	iterPos = m_StlList.begin();
	pTemp = (*iterPos);

	return pTemp;
}

/**
@brief   List ���� ��ġ�� ���ڸ� �����ϴ� �Լ�

         ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ �����Ѵ�.
@author  ehoto
@date    2012.11.12
@param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::refer(UINT nIndex)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;		///<����ó��
	if(nIndex>=m_nCount) return NULL;	///<����ó��

	iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	pTemp = (*iterPos);

	return pTemp;
}

// Pop : ���ڸ� List���� ���� ---------------------------------------------------------------------------
/**
@brief  List �� �ڿ��� ���ڸ� ������ �Լ�

         ����Ʈ �������� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::popBack()
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<����ó��

	iterPos = m_StlList.end();
	--iterPos;
	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.pop_back();

	return pTemp;
}
/**
@brief   List �� �տ��� ���ڸ� ������ �Լ�

         ����Ʈ �� �п� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::popFront(void)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;	///<����ó��

	iterPos = m_StlList.begin();
	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.pop_front();

	return pTemp;
}

/**
@brief   List ���� ��ġ���� ���ڸ� ������ �Լ�

         ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
@author  ehoto
@date    2012.11.12
@param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
@return  �Էµ� ���� ������ ������ ( T )
*/
template <typename T>
T List<T>::pop(UINT nIndex)
{
	T pTemp = NULL;
	std::list<T>::iterator iterPos;

	if(m_nCount==0) return NULL;		///<����ó��
	if(nIndex>=m_nCount) return NULL;	///<����ó��

	iterPos = m_StlList.begin();
	for(UINT i=0; i<nIndex; i++)
	{
		++iterPos;
	}

	pTemp = (*iterPos);

	m_nCount--;
	m_StlList.erase(iterPos);

	return pTemp;
}

//---------------------------------------------------------------------------

/**
@brief   List ��ü�� �����ϴ� �Լ�

         ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
		 [����] ���� ���ڰ� �ڵ� �Ҹ��Ű�� ����
		 clear ȣ���ϱ� ���� ���� ���ڰ� �Ҹ� �ʿ� �� �� �Ҹ��� ��
		 �׷��� ���� ��� �޸� ���� �߻� ���ɼ� ����
@author  ehoto
@date    2012.11.12
*/
template <typename T>
void List<T>::clear(void)
{
	m_StlList.clear();
	m_nCount = 0;
}
//---------------------------------------------------------------------------

/**
@brief   LList�� �ִ� ���� ������ ������ return�ϴ� �Լ�

         ����Ʈ�� �ִ� ���� ������ ������ return�ϴ� �Լ�.
@author  ehoto
@date    2012.11.12
@return  �Էµ� ���� ������ ���� (0: ����, 1~: �� �ִ� ������ ����)
*/
template <typename T>
UINT List<T>::getCount()
{
	return m_nCount;
}

template <typename T>
UINT List<T>::getCountsize()
{
	return m_StlList.size();
}

template <typename T>
UINT List<T>::find(T Value)
{

   int index=0;

   std::list<T>::iterator iterPos;

   iterPos = m_StlList.begin();

	for(UINT i=0; i<m_nCount; i++)
	{
		if((*iterPos) == Value) break;
		++iterPos;
		index++;
	}

	if(m_nCount== index)
	{
		index =-1;
	}

	return index;
}


typedef List<void*> LList;


#endif
