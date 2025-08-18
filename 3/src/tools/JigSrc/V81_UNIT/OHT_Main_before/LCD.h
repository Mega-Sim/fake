//---------------------------------------------------------------------------

#ifndef LCDH
#define LCDH

#include "HWCommon.h"
#include "CommHandler.h"
#include "ErrorCode.h"

//---------------------------------------------------------------------------

/* Packet ����
|STX  | Header | Length | Data | CRC16 | ETX |
|1Byte| 1Byte  | 1Byte  | ���� | 2Byte |1Byte|
*/

#define LCD_STX		0x02

//Header
#define LCD_Function	0x20
#define LCD_Text		0x40
#define LCD_BMP			0x60


//Data Function
#define LCD_BackgroundColor		0x21
#define LCD_ReturnADC			0X23
#define LCD_Clear				0x24

//Data Text
#define LCD_ReturnVersion		0x25
#define LCD_FWReboot			0x1B
#define LCD_TextXY				0x41
#define LCD_TextColor			0x42
#define LCD_TextFont			0x43


#define LCD_ETX		0x03


//DisPlay step
#define LCD_Satus_Compare		0
#define LCD_First_Print			1
#define LCD_First_Delay			2
#define LCD_Second_Print		3
#define LCD_Second_Delay		4



//VHL ACTION
#define LCD_ACTION_NOMAL				0 //����� ���� ����
#define LCD_ACTION_UBGSTOP				1
#define LCD_ACTION_OBSSTOP				2
#define LCD_ACTION_BZSTOP				3
#define LCD_ACTION_DEPOSIT				4 //FOUP�� ������ ����
#define LCD_ACTION_ACQUIRE				5 //Foup�� ������ ���� ����
#define LCD_ACTION_OHTOFF				6
#define LCD_ACTION_COMFAIL				7
#define LCD_ACTION_TPPAUSE				8
#define LCD_ACTION_CIDINTERLOCK			9 //CID Interlock
#define LCD_ACTION_CIDDECEL				10//CID Decel

/**
@class   LCD
@date    2017.12.12
@author  kyh
@brief   LCD ���� Class
*/
class LCD
{
private:
	Comm_Port *m_pCommPort;

	bool bConnect;
	int m_nComPortNum;
	int m_nBaudRate;

public:
	LCD(HWCommon *pHWCommon);
	~LCD();

	bool IsConnect();
	int AnalysisData();

	int OpenCom();
	int CloseCom();
	int ReOpenCom();

	int GetID(unsigned char* pReadBuff);
	int SendTriger(unsigned char* ackData, int length);
	WORD crc16(WORD crc, BYTE *ptr,WORD size);
};

typedef struct LCD_DATA_FLAG_
{
	unsigned int PreLCDData;
	unsigned int LCDData;		//Error �߻� ���� �Ǵ� ��� Ȯ��
	unsigned int preLCDNode;
	unsigned int LCDNode;		//������ Station, Node
	unsigned int LCDError;
	int PreLCDAction;
	int LCDAction;				//���� ���� ó��

	DWORD dwIntervalOfLCD;
	int LCDStatus;				//LCD ǥ�� ����

	int LCDOHTMainOff;			//Main ����

	int LCDCommfail;			//Commfail �߻�
	bool LCDCommfail_WaitTimeCheckFlag;
	DWORD LCDCommfail_WaitTime;
}LCD_DATA_FLAG;

#endif
