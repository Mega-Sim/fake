#ifndef		__AMC_DEF_H
#define		__AMC_DEF_H

#include <string.h>

#include "amc_type.h"
#include "amc_hardware.h"

enum {CIR_CCW = 0, CIR_CW = 1};

#define		EEPROM_BOOTPARAM_MAGIC_NO		0x9abcdef0
#define		EEPROM_BOOTPARAM_START_OFS		0X0000
#define		EEPROM_MAP_START_OFS			0X2000


#include "axisdef.h"


int ReadBootParam(UBOOTPARA *pBParam);
void SaveBootParam(UBOOTPARA *pBParam);
void InitBootParam(UBOOTPARA *pb);

int LoadLongBlockFromEeprom(
	UINT uiEepromAddr,
	char *psMap, 		// �� ������ ������ ������
	UINT *puiCrc, 		// EEPROM���� ���� CRC32
	UINT *puiLen, 		// EEPROM���� ���� ���� ���̹���Ʈ
	int nReadBytes		// �о���� ���� ũ�� ����Ʈ
);
void SaveLongBlockToEeprom();
void UploadParamToIPC();
void DnloadParamFromIPC();
void RunConfigUploadToIPC();
void RunConfigSaveFromIPC();




#endif

