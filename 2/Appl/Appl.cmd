-heap 0xc00000
-stack 0x40000

MEMORY
{
	SDRAM : o=0X80040000	l=0x0fc0000
	SDRAM2:	o=0x81000000	l=0x1000000
}

SECTIONS
{
	.vectors	>	SDRAM
	.text		>	SDRAM
	.cio		>	SDRAM
	.far		>	SDRAM
	.cinit		>	SDRAM
	.const		>	SDRAM
	.data		>	SDRAM
	.switch		>	SDRAM
	.bss		>	SDRAM
	.system		>	SDRAM
	.stack		>	SDRAM
}
