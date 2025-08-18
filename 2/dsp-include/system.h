#ifndef __SYSTEM_H__
#define __SYSTEM_H__

// System parameters
//#define CPUCLK 150000000  			// 6.7 nsec
//#define SYSCLK  (CPUCLK/4)   		// 26.7 nsec


// DMA channel
#define IMAGEIN_CHAN		10					// line by line
#define OVERLAY_CHAN		INT_HSYNC_DISP		// line by line
#define OVERLAY_CHAN_E		13					// even field
#define OVERLAY_CHAN_O		8					// odd field
#define IMAGEOUT_CHAN		14					// line by line
#define IMAGEOUT_CHAN_E		15					// even field
#define IMAGEOUT_CHAN_O		9					// odd field

// FFCTRL bit definitions
#define FFRR	1		// frame FIFO(buffer) read reset
#define IFWR	0x10	// image FIFO write reset

/* field values
#define EDMA_OPT_FS_NO                    (0)
#define EDMA_OPT_FS_YES                   (1)
#define EDMA_OPT_LINK_NO                  (0)
#define EDMA_OPT_LINK_YES                 (1<<1)
#define EDMA_OPT_TCC(x)                	  ((UINT)(x)<<16)
#define EDMA_OPT_TCINT_NO                 (0)
#define EDMA_OPT_TCINT_YES                (1<<20)
#define EDMA_OPT_DUM_NONE                 (0)
#define EDMA_OPT_DUM_INC                  (1<<21)
#define EDMA_OPT_DUM_DEC                  (2<<21)
#define EDMA_OPT_DUM_IDX                  (3<<21)
#define EDMA_OPT_2DD_NO                   (0)
#define EDMA_OPT_2DD_YES                  (1<<23)
#define EDMA_OPT_SUM_NONE                 (0)
#define EDMA_OPT_SUM_INC                  (1<<24)
#define EDMA_OPT_SUM_DEC                  (2<<24)
#define EDMA_OPT_SUM_IDX                  (3<<24)
#define EDMA_OPT_2DS_NO                   (0)
#define EDMA_OPT_2DS_YES                  (1<<26)
#define EDMA_OPT_ESIZE_32BIT              (0)
#define EDMA_OPT_ESIZE_16BIT              (1<<27)
#define EDMA_OPT_ESIZE_8BIT               (2<<27)
#define EDMA_OPT_PRI_HIGH                 (1<<29)
#define EDMA_OPT_PRI_LOW                  (2<<29)

*/

// interrupt mask bits
#define INT_EXT4		4
#define INT_EXT5		5
#define INT_EXT6		6
#define INT_EXT7		7
#define INT_EDMA		8
#define INT_SDRAM		9
#define INT_TIMER0		14
#define INT_TIMER1		15

#define EXT_INT_GRABBER	0x100  // Interrupt for Grabber(0,1,2,3)

#define MEM(addr)		(*((volatile unsigned int *)(addr)))	// memory-mapped register or IO (32 bits)
#define MEM16(addr)	(*((volatile unsigned short *)(addr)))	// memory-mapped register or IO (16 bits)
#define MEM8(addr)	(*((volatile unsigned char *)(addr)))	// memory-mapped register or IO (8 bits)

/*Interrupt*/
#define INT_LAN				INT_EXT4
#define INT_UART0			INT_EXT6
#define INT_EXTEND			INT_EXT7

/* Define Memory Address Allocation */
#define CONTROL			MEM8(0xa0000000)	// global control register (CE1 control (ROM/flash))  ---> Not Used
#define FMCTRL			MEM8(0xa0000001)	// flash memory control
#define BDCTRLIN		MEM8(0xa0000004)	// board control input [option board] (2bits)
#define BDLED			MEM8(0xa0000006)	// board led [option board] ( 2bits )

#define CAMCTRL			MEM8(0xa0001000)	// camera control
#define TRGCTRL			MEM16(0xa0001004)	// trigger control ( 63.5u * val )
#define SCCBCTRL		MEM8(0xa000100c)	// SCCB control

#define PIOCTRL			MEM8(0xa0005000)	// output enable, interrupt enable
#define OUTPORT			MEM8(0xa0005004)	// output port (4bits)
#define INPORT			MEM8(0xa0005008)	// input port (4bits)
#define INPINTR			MEM8(0xa000500a)	// input interrupt (4bits)
#define INPINTRMASK		MEM8(0xa000500c)	// input interrupt mask (4bits)
#define INPINTRCLR		MEM8(0xa000500e)	// input interrupt clear (4bits)

#define FFCTRL			MEM8(0xa0006000)	// fifo control
#define SYNCSTAT		MEM8(0xa0006001)	// ad_hysnc(1), ad_vysnc(2), da_hsync(0x10), da_vsync(0x20)

#define ETHERNET_ADDR	0xA0010000	// W3100A
#define GRABFIFO_ADDR	0xB0000000  // 8-bits FIFO ( read only )
#define DISPFIFO_ADDR	0xB0080000	// 8-bits FIFO ( write only )

/* Define Timer0 Registers */
#define TIMER0_CTL		0x1940000	/* timer0 control reg.       */
#define TIMER0_PRD		0x1940004	/* timer0 period reg.        */
#define TIMER0_CNT		0x1940008	/* timer0 counter reg.       */

/* Define Timer1 Registers */
#define TIMER1_CTL		0x1980000	/* timer1 control reg.       */
#define TIMER1_PRD		0x1980004	/* timer1 period reg.        */
#define TIMER1_CNT		0x1980008	/* timer1 counter reg.       */

/* Define QDMA Memory Mapped Registers */
#define QDMA_PARAMS		0x02000000	/* QDMA options register     */
#define QDMA_S_PARAMS	0x02000020	/* QDMA options register     */

// register control macro function
#define SetTimerPeriod(id,period) 	MEM(TIMER0_PRD+0x40000*(id))=period
#define SetTimerCount(id,count)		MEM(TIMER0_CNT+0x40000*(id))=count
#define StartTimer(id) 				MEM(TIMER0_CTL+0x40000*(id))|=0xc0
#define StartTimerPulse(id, p) 				MEM(TIMER0_CTL+0x40000*(id))|=p
#define StopTimer(id) 				MEM(TIMER0_CTL+0x40000*(id))&=~0xc0
#define GetTimerCount(id) 			(MEM(TIMER0_CNT+0x40000*(id)))
#define inport()						INPORT
#define outport(val)					OUTPORT=(val)

/* Define EMIF Registers  */
#define EMIF_GBLCTL		MEM(0x1800000)	/* EMIF global control		*/
#define EMIF_CE0CTL		MEM(0x1800008)	/* EMIF CE0 control			*/
#define EMIF_CE1CTL		MEM(0x1800004)	/* EMIF CE1 control			*/
#define EMIF_CE2CTL		MEM(0x1800010)	/* EMIF CE2 control			*/
#define EMIF_CE3CTL		MEM(0x1800014)	/* EMIF CE3 control			*/

#define EMIF_SDCTL		MEM(0x1800018)	/* EMIF SDRAM control		*/
#define EMIF_SDTIM		MEM(0x180001c)	/* EMIF SDRM refresh period	*/
#define EMIF_SDEXT		MEM(0x1800020)	/* EMIF SDRAM extension		*/
 

// CE space control field
#define WRITE_SETUP(n)	((n)<<28)
#define WRITE_STROBE(n)	((n)<<22)
#define WRITE_HOLD(n)	((n)<<20)
#define READ_SETUP(n)	((n)<<16)
#define READ_STROBE(n)	((n)<<8)
#define READ_HOLD(n)	(n)
#define TA_A(n)			((n)<<14)
#define MTYPE_8A 		0
#define MTYPE_16A 		(1<<4)
#define MTYPE_32A 		(2<<4)

/* Define Timer bits */
#define FUNC			(1)
#define DATOUT			(1<<2)

/* Define Multichannel Buffered Serial Port 0 Registers */
#define McBSP0_SPCR		0x018C0008	/* serial port control register 	*/
#define McBSP0_PCR		0x018C0024	/* pin control register 	*/

/* Define Multichannel Buffered Serial Port 0 Registers */
#define McBSP1_SPCR		0x01900008	/* serial port control register 	*/
#define McBSP1_PCR		0x01900024	/* pin control register 	*/

/* Define McBSP bits */
#define XIOEN			(1<<13)
#define RIOEN			(1<<12)
#define FSXM			(1<<11)
#define FSRM			(1<<10)
#define CLKXM			(1<<9)
#define CLKRM			(1<<8)
#define DX_STAT			(1<<5)
#define DR_STAT			(1<<4)
#define FSXP			(1<<3)
#define FSRP			(1<<2)
#define CLKXP			(1<<1)
#define CLKRP			(1)

/* Define Multichannel Buffered Serial Port 0 Registers */
#define McASP1_PFUNC	0x01B50010	/* Pin Function Reg	*/
#define McASP1_PDIR		0x01B50014	/* pin Direction register 	*/
#define McASP1_PDOUT	0x01B50018	/* pin Direction register 	*/


void (*SetVect(int irq, void (*func)()))();
void EnableInterrupt(int bit,int enable);
void (*SetDMAVect(int index, void (*func)()))();

extern volatile int DmaStat;			// Dma status (running flag)

int TestChkSum(unsigned char *p, int bytes, unsigned char chksum);

void InitSystem(BOOL bBootLoader);
unsigned char GenChkSum(unsigned char *p, int bytes);

#endif
