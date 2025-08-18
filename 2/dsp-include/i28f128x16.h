#ifndef __I28F128X16_H__
#define __I28F128X16_H__

#define FLASH_START_ADDR	0x90000000
#define FLASH_SIZE			0x01000000
#define FLASH_BLOCK_SIZE	0x00020000
#define NUM_OF_BLOCK		128
#define BANK_SIZE 			0x00200000
#define NUM_OF_BANK 		8

void FLASH_Init();
BOOL FLASH_Erase(unsigned int addr);
BOOL FLASH_EraseBlock(int Block);
BOOL FLASH_Write(unsigned int addr,const void *src,int bytecnt);
void FLASH_Read(unsigned int addr, void *dest, int bytecnt);
void* FLASH_GetPhysicalAddr(unsigned int addr);
void* FLASH_BeginRead(unsigned int addr);
void ControlFLASH(int bank,int ctrl);

#endif

