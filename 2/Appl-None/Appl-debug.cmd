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
	.pinit		>	SDRAM
	.cinit		>	SDRAM
	.const		>	SDRAM
	.data		>	SDRAM
	.switch		>	SDRAM
	.bss		>	SDRAM
	.system		>	SDRAM
	.stack		>	SDRAM
	.user		>	SDRAM
	.user_dpr	>	SDRAM
	.user_int	>	SDRAM
	.user_ctrl	>	SDRAM
	.user_main	>	SDRAM
	.user_dbgcmd	>	SDRAM
	.user_dbgmsg	>	SDRAM
}

