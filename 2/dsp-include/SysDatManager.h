#ifndef __SYSDATMANAGER_H__
#define __SYSDATMANAGER_H__

typedef struct _SYSTEM_DATA
{
	int m_nCPUType;   //0:6712, 1:6713
	char m_sRunFileName[48];
}SYSTEM_DATA;

void FormatSystemData();
SYSTEM_DATA* GetSystemData();
BOOL WriteSystemData(const SYSTEM_DATA* pData);

#endif

