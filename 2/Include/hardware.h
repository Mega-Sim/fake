#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef HARDWARE_GLOBALS
#define HARDWARE_EXT
#else
#define HARDWARE_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define _REG32(addr)    (*((volatile unsigned int *)(addr)))

#define	CCFG    _REG32(0X01840000)
#define MUXH    _REG32(0x019c0000)	// Interrupt Multiplexer High
#define MUXL    _REG32(0x019c0004)	// Interrupt Multiplexer Low 

#define EXTPOL	_REG32(0x019c0008)
#define	GPEN	_REG32(0x01b00000)
#define	GPVAL	_REG32(0X01B00008)
#define	GPDIR	_REG32(0X01B00004)
#define	GPPOL	_REG32(0X01B00024)

#define	INTR_1MS_PULSE_ADDR		((char *)0xa00000a0)

#define DAC_BASE        0x8000

#define	AMC_VERSION     "\r\nAMC Board Firmware Version "

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
HARDWARE_EXT int FlashType;
HARDWARE_EXT volatile int enc_sign;
HARDWARE_EXT int sen_output;	
#if (defined(__AMC_SMD) || defined(__AMC_V70)) 
HARDWARE_EXT unsigned int ui_CurPos[4];
#endif
HARDWARE_EXT char* pBootPara;
HARDWARE_EXT volatile int *MOTOR_DIRECTION_CONTROL;
HARDWARE_EXT int* ASIC_COUNT[MMC_AXIS];
HARDWARE_EXT int* DA_PORT_ADDR[MMC_AXIS];
HARDWARE_EXT int* CLK_OUT_ADDR[MMC_AXIS];
HARDWARE_EXT int* CLK_IN_ADDR[MMC_AXIS];
HARDWARE_EXT char* CommDPRAM;
HARDWARE_EXT char* AxisDPRAM[MMC_AXIS];
HARDWARE_EXT int* pDownloadDPRAM;
HARDWARE_EXT char* Ack2Pc;
HARDWARE_EXT char* Int2Pc;
HARDWARE_EXT char* AxisInfo;
HARDWARE_EXT volatile int* ptr_user_oport;
HARDWARE_EXT volatile int* ptr_user_oport2;
HARDWARE_EXT volatile int* ptr_user_oport3;
HARDWARE_EXT volatile int* ptr_user_oport4;
#if (defined(__AMC_SMD) || defined(__AMC_V70))
HARDWARE_EXT volatile int* ptr_user_oport5;
HARDWARE_EXT volatile int* ptr_user_oport6;
HARDWARE_EXT volatile int* ptr_user_oport7;
HARDWARE_EXT volatile int* ptr_user_oport8;
HARDWARE_EXT volatile int* ptr_user_oport9;
HARDWARE_EXT volatile int* ptr_user_oport10;
HARDWARE_EXT volatile int* ptr_user_oport11;
HARDWARE_EXT volatile int* ptr_user_oport12;
HARDWARE_EXT volatile int* ptr_user_oport13;
HARDWARE_EXT volatile int* ptr_user_oport14;
HARDWARE_EXT volatile int* ptr_user_oport15;
HARDWARE_EXT volatile int* ptr_user_oport16;
#endif
HARDWARE_EXT volatile int* ptr_user_iport;
HARDWARE_EXT volatile int* ptr_user_iport2;
HARDWARE_EXT volatile int* ptr_user_iport3;
HARDWARE_EXT volatile int* ptr_user_iport4;
#if (defined(__AMC_SMD) || defined(__AMC_V70))
HARDWARE_EXT volatile int* ptr_user_iport5;
HARDWARE_EXT volatile int* ptr_user_iport6;
HARDWARE_EXT volatile int* ptr_user_iport7;
HARDWARE_EXT volatile int* ptr_user_iport8;
HARDWARE_EXT volatile int* ptr_user_iport9;
HARDWARE_EXT volatile int* ptr_user_iport10;
HARDWARE_EXT volatile int* ptr_user_iport11;
HARDWARE_EXT volatile int* ptr_user_iport12;
HARDWARE_EXT volatile int* ptr_user_iport13;
HARDWARE_EXT volatile int* ptr_user_iport14;
HARDWARE_EXT volatile int* ptr_user_iport15;
HARDWARE_EXT volatile int* ptr_user_iport16;
#endif
#if defined(__AMC_V70)
HARDWARE_EXT volatile UINT2* ptr_power_12p;
HARDWARE_EXT volatile UINT2* ptr_power_12m;
HARDWARE_EXT volatile UINT2* ptr_power_5p;
HARDWARE_EXT volatile UINT2* ptr_power_3p3;
HARDWARE_EXT volatile UINT2* ptr_power_cutoff_12p;
HARDWARE_EXT volatile UINT2* ptr_power_cutoff_12m;
HARDWARE_EXT volatile UINT2* ptr_power_cutoff_5p;
HARDWARE_EXT volatile UINT2* ptr_power_cutoff_3p3;
HARDWARE_EXT volatile UINT2* ptr_Moni_R_Dac0;
HARDWARE_EXT volatile UINT2* ptr_Moni_R_Dac1;
HARDWARE_EXT volatile UINT2* ptr_Moni_R_Dac2;
HARDWARE_EXT volatile UINT2* ptr_Moni_R_Dac3;
HARDWARE_EXT volatile char* ptr_cutoff_enable;
HARDWARE_EXT volatile char* ptr_cutoff_status;
HARDWARE_EXT volatile UINT2* ptr_Power_MinMaxPeak_3p3;
HARDWARE_EXT volatile UINT2* ptr_Power_MinMaxPeak_5p;
HARDWARE_EXT volatile UINT2* ptr_Power_MinMaxPeak_12p;
HARDWARE_EXT volatile UINT2* ptr_Power_MinMaxPeak_12m;
HARDWARE_EXT volatile UINT2* ptr_WdStopDecelRatio_Ax0;
HARDWARE_EXT volatile UINT2* ptr_WdStopDecelRatio_Ax1;
HARDWARE_EXT volatile UINT2* ptr_WdStopDecelRatio_Ax2;
HARDWARE_EXT volatile UINT2* ptr_WdStopDecelRatio_Ax3;
#endif
HARDWARE_EXT volatile int* ptr_sys_sctrl;
HARDWARE_EXT volatile int* ptr_sys_sstat;
HARDWARE_EXT volatile int* ptr_sys_sabs;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
HARDWARE_EXT void InitHardwareParam(void);
HARDWARE_EXT void InitHardware(void);
HARDWARE_EXT int __ReadEncoder(int ax);
HARDWARE_EXT void InitMotorParameters();
HARDWARE_EXT void ResetEncCounter(void);
HARDWARE_EXT void ControlIdle(void);
HARDWARE_EXT int ClearDPRInt(void);
HARDWARE_EXT void ResetPulseOut(void);
HARDWARE_EXT void InitPIDGain(void);
HARDWARE_EXT void InitDA(void);
HARDWARE_EXT void InitPosition(void);
HARDWARE_EXT void SetTimer( void );
HARDWARE_EXT void SelectFlashROM( void );
HARDWARE_EXT void ServoOn( int ax );
HARDWARE_EXT void ServoOff( int ax );
HARDWARE_EXT int ReadCommand(void);
HARDWARE_EXT int ReadCommandAxis( void );
HARDWARE_EXT int EchoCommand( int x );
HARDWARE_EXT int GetEncoderHwDirection(int ax);
HARDWARE_EXT int SetEncoderHwDirection(int ax, int cwccw);
HARDWARE_EXT int _dsp_get_encoder_direction(int ax);
HARDWARE_EXT int ReadUserIn( void );
HARDWARE_EXT void _dsp_get_io64(int *pdat);
#if defined(__AMC_29x)      // 120831 2.9.8 SYK 64BIT  
HARDWARE_EXT int __get_bit(int bit, int *pdat);
HARDWARE_EXT int _dsp_get_bit(int bitno, pINT chk_err);             // 120228 syk
HARDWARE_EXT int _dsp_get_outbit(int bitno, pINT chk_err);          // 120228 syk
#endif
#if (defined(__AMC_SMD) || defined(__AMC_V70))      // 256BIT
HARDWARE_EXT int __get_bit(int bit, int *pdat);
HARDWARE_EXT int _dsp_get_bit(int bitno, pINT chk_err);				// 120831 2.9.8 SYK  64BIT  --> 256BIT
HARDWARE_EXT int _dsp_get_outbit(int bitno, pINT chk_err);			// 120831 2.9.8 SYK  64BIT  --> 256BIT
HARDWARE_EXT void _dsp_get_io_input(int *pdat);
#endif
HARDWARE_EXT void WriteEncoder(int ax, int pos);
HARDWARE_EXT char WriteFlashMemory( char * flash_address, char data );
HARDWARE_EXT int ReadServoAlarm( int nzax );
HARDWARE_EXT int ReadServoSRdy( int nzax );
HARDWARE_EXT int ReadSensor( int ax );
HARDWARE_EXT void WriteDAPort( int ax, int val );
HARDWARE_EXT void WritePulseCommand( int ax, int value, unsigned int attr, int r1, int r2, int upperCounter );
HARDWARE_EXT int ReadInPos( int ax );
HARDWARE_EXT int GetAxisStatus(int ax);
HARDWARE_EXT char ReadChannel1( char *addr, int offset);
HARDWARE_EXT short ReadChannel2( char *addr, int offset);
HARDWARE_EXT int ReadChannel(char *addr, int offset);
HARDWARE_EXT void WriteChannel1(char *addr, int offset, char data);
HARDWARE_EXT void WriteChannel2(char *addr, int offset, short data);
HARDWARE_EXT void WriteChannel(char *addr, int offset, int data);
HARDWARE_EXT int ReadMem1(char *addr,int offset);
HARDWARE_EXT int ReadMem2(char *addr,int offset);
HARDWARE_EXT int ReadMem(char *addr,int offset);
HARDWARE_EXT void WriteMem1(volatile char *addr, int offset, int data);
HARDWARE_EXT void WriteMem2(volatile char *addr, int offset, int data);
HARDWARE_EXT void WriteMem(volatile char *addr, int offset, int data);
HARDWARE_EXT void __SetIoOutData( int ld1, int ld2 );
HARDWARE_EXT int _dsp_set_io64(int port, int *pval);
#if defined(__AMC_29x)
HARDWARE_EXT void __SetIoBit(int npos);
HARDWARE_EXT void __ResetIoBit(int npos);
#endif
#if (defined(__AMC_SMD) || defined(__AMC_V70))  // 120818 2.9.8 syk
HARDWARE_EXT void __SetIoBit(int npos);
HARDWARE_EXT void __ResetIoBit(int npos);
HARDWARE_EXT void __SetIoCount(int ld1, int ld2);           // 120831 2.9.8 SYK  INPUT/OUTPUT
HARDWARE_EXT void __SetIoOutData_256( int ld1, int ld2, int ld3, int ld4, int ld5, int ld6, int ld7, int ld8);	// 120831 2.9.8 SYK 256BIT OUTPUT
HARDWARE_EXT int _dsp_set_io_output(int port, int *pval);   // 120831 2.9.8 SYK DSP함수 : 256BIT OUTPUT
HARDWARE_EXT int _dsp_set_io_count(int in_c, int out_c);    // 120831 2.9.8 SYK DSP함수 : INPUT/OUTPUT
#endif
HARDWARE_EXT void _ReadServopackPosition(int ax, int nValidEncBitNo, int bEnAlready);
HARDWARE_EXT void ResetEncoderTiming(int nValidEncBitNo);
#if (defined(__AMC_SMD) || defined(__AMC_V70))  // 20121204 SYK 2.11.2
HARDWARE_EXT int _dsp_reload_servopack_all();
HARDWARE_EXT void _ReadServopackALLPosition(int ax, int nValidEncBitNo);
HARDWARE_EXT void _ReloadServopackTiming(int nValidEncBitNo);
#endif

#endif

