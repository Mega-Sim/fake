#ifndef __AMC_H
#define __AMC_H

#include "6xSystem.h"
#include "filesystem.h"
#include "SerialPort.h"
#include "../Appl-include/amc_filter.h"
#include "../Appl-include/amc_otf.h"

#define	MUST_ENTER_CRITICAL_SECTION
#define	MUST_LEAVE_CRITICAL_SECTION

#define	MUST_NOP

#define	DISABLE_COMM_INTERRUPT		
#define	ENABLE_COMM_INTERRUPT		

#define	DISABLE_1MS_INTERRUPT
#define	DISABLE_5MS_INTERRUPT
#define	ENABLE_1MS_INTERRUPT
#define	ENABLE_5MS_INTERRUPT

#define	DISABLE_GLOBAL_INTERRUPT
#define	ENABLE_GLOBAL_INTERRUPT

#define	CLEAR_PENDED_DPRAM_INTERRUPT

#define	API

#define	MMC_OK  0
#define	API

enum { CMD_MOVE_P = 1, CMD_MOVE_N, CMD_MOVE_S, CMD_MOVE_DS, CMD_MOVE_DC, CMD_MOVE_DDS, CMD_MOVE_DDDS };

typedef struct
{
	char mode;	// 0 : pulse/sec^2, 1 : pulse/sec
	char cmd;	// 1 : move_p, 
				// 2 : move_n
				// 3 : move_s
				// 4 : move_ds
				// 5 : move_dc
				// 6 : move_dds
				// 7 : move_ddds
	short int ax;
	float a[5];
	float v[5];
	float m[5];
} MOVE_X_PARAM;

#endif

