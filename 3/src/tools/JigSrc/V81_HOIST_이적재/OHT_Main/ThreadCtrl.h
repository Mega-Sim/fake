#ifndef ThreadCtrlH
#define ThreadCtrlH

//#include <iostream>
//#include <tchar.h>
#include <windows.h>
#include <process.h>

/*
THREAD_PRIORITY_TIME_CRITICAL 
THREAD_PRIORITY_HIGHEST 
THREAD_PRIORITY_ABOVE_NORMAL 
THREAD_PRIORITY_NORMAL 
THREAD_PRIORITY_BELOW_NORMAL 
THREAD_PRIORITY_LOWEST 
THREAD_PRIORITY_IDLE 
*/
 

#pragma comment(lib, "Kernel32.lib")

//using namespace std;
//// static �Լ��� ���� Critical section
//static int nThreadCtrlCount = 0;
//static CRITICAL_SECTION	ThreadCtrl_CS;

class ThreadCtrl
{
private:
	CRITICAL_SECTION	m_CS;
	HANDLE	m_hThread;
	bool	m_bOnThreading; ///< RealThreadProc �� ȣ�� ���� �Ǿ��� �� �ڵ����� true ȣ��, �Ϸ� �Ǿ��� �� false�� ���� �Ѵ�.

	static UINT WINAPI ThreadProc(LPVOID lpParam);
	void ThreadProcCtrl(void);
	void endThread(void);

protected:
//	virtual UINT WINAPI RealThreadProc(void)=0;
	bool	m_bThreadBreaker;	///< Thread�� �극��ũ ��ų �� ���
	virtual void RealThreadProc(void)=0;
	
	//2014.04.15 doori.shin Thread check�� ���� ����
	int m_Tick;
	static const int MAX_TICK = 1023;
public:
	ThreadCtrl(void);
	virtual ~ThreadCtrl(void);
	
	void beginThread(void);
//	void suspendThread(void);	// Thread�� �������� ���� �ҷ� �߻� ���ɼ� �־� ������
//	void resumeThread(void);	// Thread�� �������� ���� �ҷ� �߻� ���ɼ� �־� ������
	void setPriority(int nPriority);
	void getPriority();
	bool isOnThreading();

	void qiutRealThreadProc();	/// m_bThreadBreaker �� true�� ����� �ִ� �Լ�
	HANDLE GetTreadHandle();
	//2014.04.15 doori.shin Thread check�� ���� �Լ�
	int getTick();
};

#endif
