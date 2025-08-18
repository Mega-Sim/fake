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
	char *psMap, 		// 맵 정보를 저장할 포인터
	UINT *puiCrc, 		// EEPROM에서 읽은 CRC32
	UINT *puiLen, 		// EEPROM에서 읽은 멥의 길이바이트
	int nReadBytes		// 읽어들일 맵의 크기 바이트
);
void SaveLongBlockToEeprom();
void UploadParamToIPC();
void DnloadParamFromIPC();
void RunConfigUploadToIPC();
void RunConfigSaveFromIPC();




#endif

