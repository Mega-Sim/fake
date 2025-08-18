-heap 0xc00000
-stack 0x40000

MEMORY
{
	IRAM :  o=0x00000400	l=0x0003fc00
	SDRAM : o=0X80040000	l=0x0fc0000
	SDRAM2:	o=0x81000000	l=0x1000000
}


SECTIONS
{
	.vectors	>	SDRAM
	.text		>	SDRAM
	.cio		>	SDRAM
	.far		>	SDRAM
	.cinit		>	IRAM
	.const		>	IRAM
	.data		>	IRAM
	.switch		>	IRAM
	.bss		>	IRAM
	.system		>	IRAM
	.stack		>	SDRAM
	.user		>	IRAM
	.user_dpr	>	IRAM
	.user_int	>	IRAM
	.user_main	>	IRAM
	.user_dbgcmd	>	IRAM
	.user_dbgmsg	>	IRAM
}

