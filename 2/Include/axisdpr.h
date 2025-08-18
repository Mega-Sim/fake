#ifndef __AXISDPR_H
#define __AXISDPR_H

#ifdef AXISDPR_GLOBALS
#define AXISDPR_EXT
#else
#define AXISDPR_EXT extern
#endif

#include "axisdef.h"

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define	UINT    unsigned int
#define	ULONG   unsigned long
#define	USHORT  unsigned short

// DPRAM Map 
#define	CD_Command		    0 	    //  2 byte	short int
#define	CD_Len			    2	    //  2 byte	short int
#define	CD_Vel			    4	    // 32 byte	float[8]
#define	CD_Acc			    36	    // 16 byte	short int[8]
#define	CD_Axis			    52	    // 16 byte	short int[8]
#define	CD_Pos			    68 	    // 32 byte	float[8]
#define	CD_Ox			    100	    //  4 byte	float
#define	CD_Oy			    104	    //  4 byte	float
#define	CD_Angle		    108	    //  4 byte	float
#define	CD_L_Vel		    112	    //  4 byte	float
#define	CD_L_Acc		    116	    //  2 byte	short int
#define	CD_CirDir		    118     //  2 byte 	short dummy
#define	CD_L_Deg		    120	    //  4 byte	float
#define	CD_Io_outValue		124	    //  4 byte	int
#define	CD_Io_inValue		128	    //  4 byte	int
#define	CD_AnalogChannel1	132	    //  2 byte  short int
#define	CD_AnalogChannel2	134	    //  2 byte  short int
#define	CD_AnalogChannel3	136	    //  2 byte  short int
#define	CD_AnalogChannel4	138	    //  2 byte	short int
#define	CD_Delay		    140	    //  4 byte	int
#define	CD_AxisNum		    144	    //	2 byte	short int
#define	CD_Ver			    146	    //	2 byte	short int
#define	CD_Dcc			    148	    // 16 byte	short int[8]
#define	CD_Temp_Acc		    164	    //  2 byte  short int
#define	CD_Temp_Dcc		    166	    //  2 byte  short int
#define	CD_Temp_Vel		    168	    //  4 byte  int
#define	CD_Spl_Pos		    172	    // 16 byte	short int[8]

#if (defined(__AMC_SMD) || defined(__AMC_V70))  // 120831 2.9.8 SYK io�߰��� ����
#define	CD_Io_inValueR	    472	    //  4 byte	int
#define	CD_Io_inValue1_1	476	    //  4 byte	int
#define	CD_Io_inValue2_1	480	    //  4 byte	int
#define	CD_Io_outValue3	    484     //  4 byte	int
#define	CD_Io_inValue3	    488	    //  4 byte	int
#define	CD_Io_outValue4	    492	    //  4 byte	int
#define	CD_Io_inValue4	    496	    //  4 byte	int
#define	CD_Io_outValue5	    500	    //  4 byte	int
#define	CD_Io_inValue5	    504	    //  4 byte	int
#define	CD_Io_outValue6	    508	    //  4 byte	int
#define	CD_Io_inValue6	    512	    //  4 byte	int
#define	CD_Io_outValue7	    516	    //  4 byte	int
#define	CD_Io_inValue7	    520	    //  4 byte	int
#define	CD_Io_outValue8	    524	    //  4 byte	int
#define	CD_Io_inValue8	    528	    //  4 byte	int
#endif

#define	CD_Io_outValue2	    532	    //  4 byte	int 		
#define	CD_Io_inValue2	    536	    //  4 byte	int 		
#define	CD_Int2Pc		    1023	//  0x3ff

// Map ������ ������ ���ؼ�  0xb0001650 (0x1580+172)����
// 0xb000162c ~ 0xb0001794 ���� ����ִ°��� �̿��Ѵ�.
// 0xb0001640 ~ 0xb000174f ���� 256 + 16����Ʈ
//
// 0xb0001640 ~ 0xb0001643 : Total CRC-32
// 0xb0001644 ~ 0xb0001647 : Total Bytes
// 0xb0001648 ~ 0xb0001649 : Total 256����Ʈ ������ Block ����
// 0xb000164a ~ 0xb000164b : ���� ���۵� ����� ��ȣ, 0���� ����
// 0xb000164c			   : Data Valid, (PC->DSP)
// 0xb000164d			   : Read Complete, (DSP->PC)
// 0xb000164e			   : (����)
// 0xb000164f			   : (����)
// 0xb0001650 ~ 0xb000174f : ������ ���� ����
#define	MAP_DPRAM_ADDRESS	(char *) (0xb0001640)
#define	MAP_OFS_CRC4		0
#define	MAP_OFS_TOTLEN4		4		// ���� �Ϸ��� 0�� �� ������ ����, ���1�� �� ������ �������� PC�� �� �� �ֵ��� �Ѵ�.
#define	MAP_OFS_BLKLEN2		8
#define	MAP_OFS_BLKID2		10
#define	MAP_OFS_SAVEADDR4	12		// ���ſϷ��� ������ eeprom�� ���۹���
#define	MAP_OFS_DATA256		16

// 2011.8.1, ckyu => 2.8.04, 2011.10.13
// Alive Check�� ���ؼ� Ư�� �����V�� �����Ѵ�.
// ��� ��巹�� 0xb000163e~f (offset:0x23e(574,575))
// 0x23e : 0x9A�� �� ���´�.
// 0x23f : (0x23e) ^ 0xff => 0x65
#define	ALIVE_CHK_ADDR1		0xb000163e
#define	ALIVE_CHK_ADDR2		0xb000163f
#define	ALIVE_CHK_BYTE1		(0x9A)
#define	ALIVE_CHK_BYTE2		(ALIVE_CHK_BYTE1 ^ 0xff)

#define	MAP_OFS_EEPROM		0x2000  // 0x2000 ~ 0x7fff(24KB�� ���� ���忡 �̿��Ѵ�. 2006.12.7)

// 2007.1.10, ckyu
// PC�� ���� ��Ʈ���� �ޱ����� �̿��ϴ� �����V
#define	STRING_OFS_LEN			0
#define	STRING_OFS_CHKSUM		1
#define	STRING_OFS_RESERVED		2
#define	STRING_OFS_ACKLEN		3
#define	STRING_OFS_ACKCHKSUM	4
#define	STRING_OFS_ACKERRNO		5
#define	STRING_OFS_BODY			6

// 2007.2.27, ckyu
// EEPROM ������ ������ ���� �̿��ϴ� �����V
#define	EDUMP_OFS_CRC4			0		// ������ ��������ü�� crc32 ��
#define	EDUMP_OFS_TOTLEN4		4		// ������ ��������ü ����Ʈ��
#define	EDUMP_OFS_BLKLEN2		8		// ���� ��(256����Ʈ����)�� ����
#define	EDUMP_OFS_BLKID2		10		// ������� ����, BLKID=0�� ó�� ������ ���� �غ�
#define	EDUMP_OFS_SAVEADDR4		12		// ������ EEPROM�� ��巹��
#define	EDUMP_OFS_DATABLOCK		16

// ������ ������ �����Ѵ�.
// Upload from DSP
// 1. (Start Address, Len) -> to DSP
// 2. (crc4, blklen) <- from DSP
// 3. blklen ��ŭ blkid�� 1..blklen���� �����Ͽ� DSP�� ȣ��
// 4. ���� �ش��ϴ� �����͸� �����Ͽ� �����Ѵ�.
// 5. �����͸� ��� ������ �� CRC����Ͽ� �������� ���Ѵ�.
// 6. ����

// Downloading to DSP
// 1. (Save address, TotLen, CRC, BlkLen, BlkID=0) -> to DSP
// 2. BlkID=1..BlkLen, �����ͼ��� -> to DSP
// 3. DSP������ �� ������ TotLen=0, ������ ������ TotLen=-1�� �����Ͽ� �����Ѵ�.  

#define	AD_Board_Exist			    0
#define	AD_In_Pos_Flag			    2
#define	AD_In_Sequence			    3		// 2011.9.30, in_sequence
#define	AD_AxisSource			    4
#define	AD_AxisState			    6
#define	AD_Vel           		    8		// com RPM, 2007.12.18
#define	AD_ActVel				    10		// act RPM�� ����., 2007.12.18
#define	AD_Frames_Q				    12
#define	AD_Frames_Interpolation	    13
#define	AD_CharType1			    14		// 2011.9.30, __SetErrorLimit(),__GetErrorLimit(), _dsp_get_servo
#define	AD_CharType2			    15
#define	AD_IntType1				    16		// 2011.9.30, P Gain, Pos & Vel Loop, frames_left()
#define	AD_IntType2				    18		// 2011.9.30, I Gain, Pos & Vel Loop
#define	AD_IntType3				    20		// 2011.9.30, D Gain, Pos & Vel Loop
#define	AD_IntType4				    22		// 2011.9.30, F Gain, Pos & Vel Loop
#define	AD_IntType5				    24		// 2011.9.30, ILimit, Pos & Vel Loop
#define	AD_LongType1			    26		// 2011.9.30, __SetPosition, __GetCurPos, __GetPos, __SetErrorLimit,__GetErrorLimit,_dsp_dwell
#define	AD_FloatType1			    30		// 2011.9.30, _dsp_get_error, _dsp_set_in_position
#define	AD_LongType2			    34		// com RPS, 2007.12.18
#define	AD_FloatType2			    38		// act RPS�� ����., 2007.12.18

#define	max(a,b)            (((a)>(b)) ? (a) : (b))
#define	min(a,b)            (((a)<(b)) ? (a) : (b))

#define DPRAMTEST_VALUE1    0xac
#define	DPRAMTEST_VALUE2    0x53

#define DIV_LENGTH          64

typedef struct 
{
	UINT		uiCrc32;
	UINT		uiTotLen;
	UINT		uiBlkLen;
} MAP_MAIN_HEADER;

typedef struct 
{
	unsigned int ulStartAddress;
	unsigned int ulTotLen;
	unsigned int uiBlkLen;
	unsigned int ulCrc;
} EDUMP_HDR;

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
AXISDPR_EXT char *m_pcaBuff;
AXISDPR_EXT MAP_MAIN_HEADER mmHdr;
AXISDPR_EXT unsigned int uiEepromSaveAddr;
AXISDPR_EXT EDUMP_HDR eDumpHdr;
AXISDPR_EXT char *peDumpBody;
AXISDPR_EXT char check_pccmd;

AXISDPR_EXT int int_change;
AXISDPR_EXT unsigned int gbNewCmdArrived;
AXISDPR_EXT int gnNewCmdIndex;
AXISDPR_EXT int gnNewCmdAxis;

AXISDPR_EXT FUNCPOINT Service[MAX_COMM_NUM+1];
AXISDPR_EXT volatile float dcleanVoltage;

// Dynamic ACC Speed
AXISDPR_EXT volatile int ACC_CHANGE_SPEED1;
AXISDPR_EXT volatile int ACC_CHANGE_SPEED2;
AXISDPR_EXT volatile int ACC_CHANGE_SPEED3;

AXISDPR_EXT	char b2wdFlag;	// 200601 yjchoe, 2wd Select Flag

AXISDPR_EXT volatile int	PatrolVHLIOCnt;

#if (defined(__AMC_SMD) || defined(__AMC_V70))
AXISDPR_EXT int in_mask[8];     // 120831 2.9.8 SYK
AXISDPR_EXT int out_mask[8];	// 120831 2.9.8 SYK
#endif

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
AXISDPR_EXT void Put_CleanVoltage(void);
AXISDPR_EXT void InitServiceFunction();
AXISDPR_EXT void GetAMCData70();
AXISDPR_EXT interrupt void Dpr_Int( void );
AXISDPR_EXT void DoCmdIfExist();
AXISDPR_EXT void NoOperation(void);
AXISDPR_EXT void SPL_Line_Move(void);
AXISDPR_EXT void SPL_Arc_Move(void);
AXISDPR_EXT void SPL_Move( void );
AXISDPR_EXT void TraceUpdate_1(void);
AXISDPR_EXT void TraceUpdate(void);
AXISDPR_EXT void GET_Position(void);
AXISDPR_EXT void PUT_Curr_Pos(void);
AXISDPR_EXT void PUT_Position(void);
AXISDPR_EXT void PUT_Error_Pls(void);
AXISDPR_EXT void GET_Error_Limit(void);
AXISDPR_EXT void PUT_Error_Limit(void);
#if (defined(__AMC_SMD) || defined(__AMC_V70))
AXISDPR_EXT void SetIoOutput(void);     
AXISDPR_EXT void SetIOCount(void);	      
AXISDPR_EXT void GetIoOutput(void);     
AXISDPR_EXT void GetIOCount(void);      
AXISDPR_EXT void make_io_mask();       
AXISDPR_EXT void ReloadServopackAll();
#endif
AXISDPR_EXT void SET_IO_Bit(void);
AXISDPR_EXT void RESET_IO_Bit(void);
AXISDPR_EXT void Servo_On(void);
AXISDPR_EXT void Servo_Off(void);
#if defined(__AMC_V70)
AXISDPR_EXT void SetSystemMoniEnable(void);
AXISDPR_EXT void SetSystemMoniDisable(void);
AXISDPR_EXT void SetMoniteringThresholdPercent(void);
AXISDPR_EXT void GetMoniteringThresholdPercent(void);
AXISDPR_EXT void GetSystemMoniValue(void);
AXISDPR_EXT void GetSystempStatus(void);
AXISDPR_EXT void GetSystemMoniEnable(void);

// �߰��� on 2018.05.23 --> ���� ����͸� ���� �� ���� ���� & WatchDog �߻� �� Decel Stop Ratio ��� �߰�.
AXISDPR_EXT void ResetPeakHoldValue(void);	// 200
AXISDPR_EXT void GetMinPeakHoldValue(void);	// 201  --> 3.3V, 5V, +12V, -12V ������ ���� ���ŵ� �ּ� Peak Hold Voltage, raw, dac �� ��ȯ
AXISDPR_EXT void GetMaxPeakHoldValue(void);	// 202  --> 3.3V, 5V, +12V, -12V ������ ���� ���ŵ� �ִ� Peak Hold Voltage, raw, dac �� ��ȯ
AXISDPR_EXT void SetDecelOnWD(void);		// 203  --> Ratio = 0 �� �� �ش� ��� ��� ����(Disable)
AXISDPR_EXT void GetDecelOnWD(void);		// 204 --> �ະ�� ������ Wd Decel Ratio ��ȯ.
AXISDPR_EXT void SetDecelOnWD_By_AMC(int ax, float fDecelOnWd);
AXISDPR_EXT void ResetPeakHoldValue_By_AMC(void);

#endif
AXISDPR_EXT void Alarm_Reset(void);
AXISDPR_EXT void Alarm_Set(void);
AXISDPR_EXT void Clear_Status(void);
AXISDPR_EXT void Put_Axis_Stat(void);
AXISDPR_EXT void Put_Axis_Source(void);
AXISDPR_EXT void Get_Dac_Offset(void);
AXISDPR_EXT void Put_Dac_Offset(void);
AXISDPR_EXT void Get_Frames_Clear(void);
AXISDPR_EXT void Get_Amp_On_Level(void);
AXISDPR_EXT void Put_Amp_On_Level(void);
AXISDPR_EXT void Get_SW_Upper_Limit(void);
AXISDPR_EXT void Put_SW_Upper_Limit(void);
AXISDPR_EXT void Get_SW_Lower_Limit(void);
AXISDPR_EXT void Put_SW_Lower_Limit(void);
AXISDPR_EXT void Put_IO_Out_Data(void);
AXISDPR_EXT void Get_SyncMap(void);
AXISDPR_EXT void Get_SyncOn(void);
AXISDPR_EXT void Get_SyncOff(void);		
AXISDPR_EXT void Get_SyncGain(void);
AXISDPR_EXT void Put_SyncGain(void);
AXISDPR_EXT void Put_Encoder(void);
AXISDPR_EXT void Put_SyncEncoder(void);
AXISDPR_EXT void Set_Oht_Model_id(void);
AXISDPR_EXT void Get_Oht_Model_id(void);
AXISDPR_EXT void Exist_Check(void);
AXISDPR_EXT float IEEE2TMS(int x);
AXISDPR_EXT int TMS2IEEE(float x);
AXISDPR_EXT void Copy_Para_2_BootPara(void);
AXISDPR_EXT void Copy_BootPara_2_Para(void);
AXISDPR_EXT int __GetInt(int ofs);
AXISDPR_EXT short __GetShort(int ofs);
AXISDPR_EXT unsigned char __GetChar(int ofs);
AXISDPR_EXT void __PutInt(int ofs, int nval);
AXISDPR_EXT void __PutShort(int ofs, short sn);
AXISDPR_EXT void __PutChar(int ofs, char ch);
AXISDPR_EXT int ReadBootParam(UBOOTPARA *pBParam);
AXISDPR_EXT void SaveBootParam(UBOOTPARA *pBParam);
AXISDPR_EXT void InitBootParam(UBOOTPARA *pb);
AXISDPR_EXT UINT SetBootParaCrc(UBOOTPARA *pbp);
AXISDPR_EXT void SaveLongBlockToEeprom();
AXISDPR_EXT int _SaveLongBlockToEeprom();
AXISDPR_EXT int LoadLongBlockFromEeprom(UINT uiEepromAddr,char *psMap, UINT *puiCrc, UINT *puiLen, int nReadBytes);
AXISDPR_EXT void RunConfigUploadToIPC();
AXISDPR_EXT void RunConfigSaveFromIPC();
AXISDPR_EXT void UploadParamToIPC();
AXISDPR_EXT void DnloadParamFromIPC();
AXISDPR_EXT void WatchdogOperations();
AXISDPR_EXT void GetMovexCmd();
AXISDPR_EXT void _dsp_clear_amc_error();
AXISDPR_EXT void _dsp_get_error_status();
AXISDPR_EXT void AMCStatusGet();
AXISDPR_EXT void InitDbgCmdBuff(int ax);
AXISDPR_EXT void PushDbgCmdBuff(int ax, int cmd, float acc, float dcc, float vel, int pos);
AXISDPR_EXT void move_PushDbgCmdBuff(int ax, int cmd, float acc, float dcc, float vel, int pos);
AXISDPR_EXT void DPRAMTest();
AXISDPR_EXT void ReloadEncoderPos();
AXISDPR_EXT void Put_Vel_Curve(void);	 
AXISDPR_EXT void Get_Vel_Curve(void);
AXISDPR_EXT void Put_Actvel_Margin(void);
AXISDPR_EXT void Get_Actvel_Margin(void);	 
AXISDPR_EXT void AMCStatusGet2();

#endif

