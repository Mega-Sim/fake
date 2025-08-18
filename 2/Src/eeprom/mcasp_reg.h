#ifndef		__MCASP_REG_H
#define		__MCASP_REG_H



#define		MCASP0_BASE		(0x01b4c000)
#define		MCASP1_BASE		(0x01b50000)

#define _REG32(addr) (*((volatile unsigned int *)(addr)))

#define		DEVCFG		_REG32(0X019C0200)

// for interrupt selector
#define		MUXH		_REG32(0x019c0000)
#define		MUXL		_REG32(0x019c0004)


#define		McPID0		_REG32(0x01b4c000)
#define		McPID1		_REG32(0x01b50000)

#define		McPWRDEMU0	_REG32(0x01b4c004)
#define		McPWRDEMU1	_REG32(0x01b50004)

#define		McPFUNC0	_REG32(0x01b4c010)
#define		McPFUNC1	_REG32(0x01b50010)

#define		McPDIR0		_REG32(0x01b4c014)
#define		McPDIR1		_REG32(0x01b50014)

#define		McPDOUT0	_REG32(0x01b4c018)
#define		McPDOUT1	_REG32(0x01b50018)

#define		McPDIN0		_REG32(0x01b4c01c)
#define		McPDIN1		_REG32(0x01b5001c)

#define		McPDCLR0	_REG32(0x01b4c020)
#define		McPDCLR1	_REG32(0x01b50020)

#define		McGBLCTL0	_REG32(0x01b4c044)
#define		McGBLCTL1	_REG32(0x01b50044)

#define		McAMUTE0	_REG32(0x01b4c048)
#define		McAMUTE1	_REG32(0x01b50048)

#define		McDLBCTL0	_REG32(0x01b4c04C)
#define		McDLBCTL1	_REG32(0x01b5004C)

#define		McDITCTL0	_REG32(0x01b4c050)
#define		McDITCTL1	_REG32(0x01b50050)

#define		McRGBLCTL0	_REG32(0x01b4c060)
#define		McRGBLCTL1	_REG32(0x01b50060)

#define		McRMASK0	_REG32(0x01b4c064)
#define		McRMASK1	_REG32(0x01b50064)

#define		McRFMT0		_REG32(0x01b4c068)
#define		McRFMT1		_REG32(0x01b50068)

#define		McAFSRCTL0	_REG32(0x01b4c06c)
#define		McAFSRCTL1	_REG32(0x01b5006c)

#define		McACLKRCTL0	_REG32(0x01b4c070)
#define		McACLKRCTL1	_REG32(0x01b50070)

#define		McAHCLKRCTL0	_REG32(0x01b4c074)
#define		McAHCLKRCTL1	_REG32(0x01b50074)

#define		McRTDM0		_REG32(0x01b4c078)
#define		McRTDM1		_REG32(0x01b50078)

#define		McRINTCTL0		_REG32(0x01b4c07c)
#define		McRINTCTL1		_REG32(0x01b5007c)

#define		McRSTAT0		_REG32(0x01b4c080)
#define		McRSTAT1		_REG32(0x01b50080)

#define		McRSLOT0		_REG32(0x01b4c084)
#define		McRSLOT1		_REG32(0x01b50084)

#define		McRCLKCHK0		_REG32(0x01b4c088)
#define		McRCLKCHK1		_REG32(0x01b50088)

#define		McREVTCTL0		_REG32(0x01b4c08c)
#define		McREVTCTL1		_REG32(0x01b5008c)

#define		McXGBLCTL0		_REG32(0x01b4c0a0)
#define		McXGBLCTL1		_REG32(0x01b500a0)

#define		McXMASK0		_REG32(0x01b4c0a4)
#define		McXMASK1		_REG32(0x01b500a4)

#define		McXFMT0			_REG32(0x01b4c0a8)
#define		McXFMT1			_REG32(0x01b500a8)

#define		McAFSXCTL0		_REG32(0x01b4c0ac)
#define		McAFSXCTL1		_REG32(0x01b500ac)

#define		McACLKXCTL0		_REG32(0x01b4c0b0)
#define		McACLKXCTL1		_REG32(0x01b500b0)

#define		McAHCLKXCTL0	_REG32(0x01b4c0b4)
#define		McAHCLKXCTL1	_REG32(0x01b500b4)


#define		McXTDM0			_REG32(0x01b4c0b8)
#define		McXTDM1			_REG32(0x01b500b8)

#define		McXINTCTL0		_REG32(0x01b4c0bc)
#define		McXINTCTL1		_REG32(0x01b500bc)

#define		McXSTAT0		_REG32(0x01b4c0c0)
#define		McXSTAT1		_REG32(0x01b500c0)

#define		McXSLOT0		_REG32(0x01b4c0c4)
#define		McXSLOT1		_REG32(0x01b500c4)

#define		McXCLKCHK0		_REG32(0x01b4c0c8)
#define		McXCLKCHK1		_REG32(0x01b500c8)

#define		McXEVTCTL0		_REG32(0x01b4c0cc)
#define		McXEVTCTL1		_REG32(0x01b500cc)

#define		McSRCTL00		_REG32(0x01b4c180)
#define		McSRCTL10		_REG32(0x01b4c184)
#define		McSRCTL20		_REG32(0x01b4c188)
#define		McSRCTL30		_REG32(0x01b4c18c)
#define		McSRCTL40		_REG32(0x01b4c190)
#define		McSRCTL50		_REG32(0x01b4c194)
#define		McSRCTL60		_REG32(0x01b4c198)
#define		McSRCTL70		_REG32(0x01b4c19c)
#define		McSRCTL01		_REG32(0x01b50180)
#define		McSRCTL11		_REG32(0x01b50184)
#define		McSRCTL21		_REG32(0x01b50188)
#define		McSRCTL31		_REG32(0x01b5018c)
#define		McSRCTL41		_REG32(0x01b50190)
#define		McSRCTL51		_REG32(0x01b50194)
#define		McSRCTL61		_REG32(0x01b50198)
#define		McSRCTL71		_REG32(0x01b5019c)

#define		McXBUF00		_REG32(0x01b4c200)
#define		McXBUF10		_REG32(0x01b4c204)
#define		McXBUF20		_REG32(0x01b4c208)
#define		McXBUF30		_REG32(0x01b4c20c)
#define		McXBUF40		_REG32(0x01b4c210)
#define		McXBUF50		_REG32(0x01b4c214)
#define		McXBUF60		_REG32(0x01b4c218)
#define		McXBUF70		_REG32(0x01b4c21c)
#define		McXBUF01		_REG32(0x01b50200)
#define		McXBUF11		_REG32(0x01b50204)
#define		McXBUF21		_REG32(0x01b50208)
#define		McXBUF31		_REG32(0x01b5020c)
#define		McXBUF41		_REG32(0x01b50210)
#define		McXBUF51		_REG32(0x01b50214)
#define		McXBUF61		_REG32(0x01b50218)
#define		McXBUF71		_REG32(0x01b5021c)

#define		McRBUF00		_REG32(0x01b4c280)
#define		McRBUF10		_REG32(0x01b4c284)
#define		McRBUF20		_REG32(0x01b4c288)
#define		McRBUF30		_REG32(0x01b4c28c)
#define		McRBUF40		_REG32(0x01b4c290)
#define		McRBUF50		_REG32(0x01b4c294)
#define		McRBUF60		_REG32(0x01b4c298)
#define		McRBUF70		_REG32(0x01b4c29c)
#define		McRBUF01		_REG32(0x01b50280)
#define		McRBUF11		_REG32(0x01b50284)
#define		McRBUF21		_REG32(0x01b50288)
#define		McRBUF31		_REG32(0x01b5028c)
#define		McRBUF41		_REG32(0x01b50290)
#define		McRBUF51		_REG32(0x01b50294)
#define		McRBUF61		_REG32(0x01b50298)
#define		McRBUF71		_REG32(0x01b5029c)



#endif

