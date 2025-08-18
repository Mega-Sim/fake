#ifndef		__CTS_EEPROM_H
#define		__CTS_EEPROM_H

//
// 24FC1025 (1Mbits) 를 대상으로 짜여진 코드임.
// 2007.6.21, ckyu
//


#define		EEPROM_NO_MORE_DATA		1
#define		EEPROM_MORE_DATA		0

#define		EEPROM_PAGE_SIZE		128

void Init_eeprom();

void WriteEepromChar(unsigned usAddr0_12, unsigned char ucValue);
void WriteEepromShort(unsigned usAddr0_12, unsigned short ucValue);
void WriteEepromInt(unsigned usAddr0_12, unsigned int ucValue);
unsigned char ReadEepromChar(unsigned usAddr0_12);


int WriteEepromString(unsigned int usAddr, char *ps, int nBytes);
int ReadEepromString(unsigned int usAddr, char *ps, int nRead);


void WriteEepromChar(unsigned int usAddr0_14, unsigned char ucValue);
void WriteEepromShort(unsigned int usAddr0_14, unsigned short usValue);
void WriteEepromInt(unsigned int usAddr0_14, unsigned int uiValue);
unsigned char ReadEepromChar(unsigned int usAddr0_14);
unsigned short ReadEepromShort(unsigned int usAddr0_14);
unsigned int ReadEepromInt(unsigned int usAddr0_14);

#endif

