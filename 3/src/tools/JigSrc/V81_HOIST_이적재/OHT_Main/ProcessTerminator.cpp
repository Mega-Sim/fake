//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProcessTerminator.h"

//---------------------------------------------------------------------------
ProcessTerminator::ProcessTerminator()
{

}

ProcessTerminator& ProcessTerminator::getInstance()
{
	if(instance == NULL)
	{
		instance = new ProcessTerminator;
	}

	return *instance;
}

/**
@brief		Process�� PID�� �˾Ƴ�
@author  	doori.shin
@date    	2013.12.31
@param		procName : ����� �ϴ� ���μ��� �̸�
@param		option : FP_FIRST_PID - ���� �̸��� ���μ����� �ִٸ� ���� ó���� ã�°��� PID�� ����(Basic)
					 FP_LAST_PID - ���� �̸��� ���μ����� �ִٸ� ���� �������� ã�°��� PID�� ����
@return		ã�� ���μ����� PID�� ����, ���ų� �����̸� -1����
*/
DWORD ProcessTerminator::FindPID(WCHAR* procName, int option)
{
	//���μ������ ��������
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	DWORD pid=0;

	PROCESSENTRY32 pe32 = {0,};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//ù ���μ����� ���ٸ� ����
	if(!Process32First(hProcessSnap, &pe32))
	{
		printf("Process32First Error!\n");
		CloseHandle(hProcessSnap);
		return -1;
	}

	//�ش� �̸��� process�˻�, PID���
	do{
		//printf("%25LS\t\t%d\n", pe32.szExeFile, pe32.th32ProcessID);
		//���μ��� �̸� ��
		if(!wcscmp(pe32.szExeFile, procName))
		{
			pid = pe32.th32ProcessID;

			if(option == FP_FIRST_PID) break;
		}
	}while(Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return pid;
}

 /**
@brief		���μ����� �����Ŵ
@author  	doori.shin
@date    	2013.12.31
@param		dwPID : �����ų ���μ����� ID
@param		dwTimeout : ���μ����� ����� ������ ��ٷ��� �ð�
@return		dwRet : ������¸� ����
			TA_FAILED - ���ῡ �����ϰų� �ش� ���μ����� ����
			TA_SUCCESS_CLEAN - ���μ����� Timeout�ð����� �����
			TA_SUCCESS_KILL - Timeout�Ǿ� ������ ������
*/
DWORD WINAPI ProcessTerminator::TerminateApp( DWORD dwPID, DWORD dwTimeout)
{
    HANDLE   hProc ;
    DWORD   dwRet ;

    hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE,
        dwPID);

    if(hProc == NULL)
    {
		return TA_FAILED ;
    }

    //PID�� �´� ���μ����� WM_CLOSE�޽����� ������
    EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID) ;

    //�����Ϸ��� ���μ����� timeout �ð� ���� ����Ǿ��ٸ� CLEAN, �ƴϸ� ���������Ų��.
    if(WaitForSingleObject(hProc, dwTimeout)!=WAIT_OBJECT_0)
		dwRet=(TerminateProcess(hProc,0)?TA_SUCCESS_KILL:TA_FAILED);
    else
		dwRet = TA_SUCCESS_CLEAN ;

    CloseHandle(hProc) ;

    return dwRet ;
}

/**
@brief		���μ������� �޽����� ����
@author  	doori.shin
@date    	2013.12.31
*/
BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
    DWORD dwID ;

    GetWindowThreadProcessId(hwnd, &dwID) ;

    if(dwID == (DWORD)lParam)
    {
		PostMessage(hwnd, WM_CLOSE, 0, 0) ;
	}

	return TRUE ;
}

#pragma package(smart_init)
