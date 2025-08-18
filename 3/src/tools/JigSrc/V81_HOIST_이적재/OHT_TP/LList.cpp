// #include "stdafx.h"
#include "LList.h"

using namespace std;

/**
 @brief LList Class default ������

 ����, ������ ���� �ʱ�ȭ
 @author  ehoto
 @date    2012.11.12
 */
LList::LList()
{
    m_nCount = 0;
}

/**
 @brief LList Class default �Ҹ���

 @author  ehoto
 @date    2012.11.12
 */
LList::~LList()
{

}
// Insert : ���� �� ��� ---------------------------------------------------------------------------

/**
 @brief   LList �ڿ� �����ϴ� �Լ�

 ���� Ÿ���� ������ ����Ʈ�� �ڿ� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : �߰��ϴ� ������ ������ ( void* )
 */
void LList::insertBack(void* pCmd)
{
    m_LList.push_back(pCmd);
    m_nCount++;
}

/**
 @brief   LList �տ� �����ϴ� �Լ�

 ���� Ÿ���� ������ ����Ʈ�� �տ� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : �߰��ϴ� ������ ������ ( void* )
 */
void LList::insertFront(void* pCmd)
{
    m_LList.push_front(pCmd);
    m_nCount++;
}

/**
 @brief   LList ���� ��ġ�� �����ϴ� �Լ�

 ���� Ÿ���� ������ ����Ʈ�� ������ ��ġ�� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @param   pCmd : �߰��ϴ� ������ ������ ( void* )
 @param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
 @return  ���� ���� Return (0: ����, �׿�: ������)
 */
int LList::insert(void* pCmd, UINT nIndex)
{
    if(m_nCount <= 0)
        return -1;
    ///<����ó��
    if(nIndex >= m_nCount)
        return -1;
    ///<����ó��

    list<void*>::iterator iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    m_LList.insert(iterPos, pCmd);
    m_nCount++;

    return 0;
}

// Refer : ������ ��� (���ڸ� ������ ����) ---------------------------------------------------------------------------
/**
 @brief   LList �� ���� ���ڸ� �����ϴ� �Լ�

 ����Ʈ �������� �ִ� ���� Ÿ���� ������ �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::referBack()
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��

    iterPos = m_LList.end();
    iterPos--;
    pTemp = (void*)(*iterPos);

    return pTemp;
}

/**
 @brief   LList �� ���� ���ڸ� �����ϴ� �Լ�

 ����Ʈ �� �п� �ִ� ���� Ÿ���� ������ �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::referFront(void)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��

    iterPos = m_LList.begin();
    pTemp = (void*)(*iterPos);

    return pTemp;
}

/**
 @brief   LList ���� ��ġ�� ���ڸ� �����ϴ� �Լ�

 ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::refer(UINT nIndex)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��
    if(nIndex >= m_nCount)
        return NULL;
    ///<����ó��

    iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    pTemp = (void*)(*iterPos);

    return pTemp;
}

// Pop : ���ڸ� List���� ���� ---------------------------------------------------------------------------
/**
 @brief  LList �� �ڿ��� ���ڸ� ������ �Լ�

 ����Ʈ �������� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::popBack()
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��

    iterPos = m_LList.end();
    iterPos--;
    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.pop_back();

    return pTemp;
}

/**
 @brief   LList �� �տ��� ���ڸ� ������ �Լ�

 ����Ʈ �� �п� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::popFront(void)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��

    iterPos = m_LList.begin();
    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.pop_front();

    return pTemp;
}

/**
 @brief   LList ���� ��ġ���� ���ڸ� ������ �Լ�

 ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
 @author  ehoto
 @date    2012.11.12
 @param   nIndex : �߰��ϴ� ��ġ (0: ��ó��, N-1: �� ������)
 @return  �Էµ� ���� ������ ������ ( void* )
 */
void* LList::pop(UINT nIndex)
{
    void* pTemp = NULL;
    list<void*>::iterator iterPos;

    if(m_nCount <= 0)
        return NULL;
    ///<����ó��
    if(nIndex >= m_nCount)
        return NULL;
    ///<����ó��

    iterPos = m_LList.begin();
    for(UINT i = 0; i < nIndex; i++)
    {
        iterPos++;
    }

    pTemp = (void*)(*iterPos);

    m_nCount--;
    m_LList.erase(iterPos);

    return pTemp;
}

// ---------------------------------------------------------------------------

/**
 @brief   LList ��ü�� �����ϴ� �Լ�

 ����Ʈ�� ������ ��ġ�� �ִ� ���� Ÿ���� ������ ��ȯ�ϰ� List���� �����Ѵ�.
 [����] ���� ���ڰ� �ڵ� �Ҹ��Ű�� ����
 clear ȣ���ϱ� ���� ���� ���ڰ� �Ҹ� �ʿ� �� �� �Ҹ��� ��
 �׷��� ���� ��� �޸� ���� �߻� ���ɼ� ����
 @author  ehoto
 @date    2012.11.12
 */
void LList::clear(void)
{
    m_LList.clear();
    m_nCount = 0;
}
// ---------------------------------------------------------------------------

/**
 @brief   LList�� �ִ� ���� ������ ������ return�ϴ� �Լ�

 ����Ʈ�� �ִ� ���� ������ ������ return�ϴ� �Լ�.
 @author  ehoto
 @date    2012.11.12
 @return  �Էµ� ���� ������ ���� (0: ����, 1~: �� �ִ� ������ ����)
 */
int LList::getCount()
{
    return m_nCount;
}
