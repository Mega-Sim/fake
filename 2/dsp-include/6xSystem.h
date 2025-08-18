#ifndef __6XSYSTEM_H__
#define __6XSYSTEM_H__

typedef unsigned int		BOOL;
typedef unsigned int		UINT;
typedef unsigned int		DWORD;
typedef unsigned int		HANDLE;
typedef const char *		LPCTSTR;
typedef char *				LPSTR;
typedef unsigned char*		LPBYTE;
typedef unsigned char 		BYTE;
typedef	unsigned short		UINT2;
typedef	short				INT2;


#define TRUE				1
#define FALSE				0
#define NULL				0

#define INFINITE			0

typedef struct  {int nWidth; int nHeight; unsigned char *pFrame;}IMAGE, *IMAGEID;
typedef struct	{int left,top,right,bottom;}RECT,*RECTID;
typedef struct	{int x,y;}POINT, *POINTID;

void SystemInit();

#ifdef NS6713
static inline int 		GetTicCount()				{return (*((volatile int *)(0x1940008)));}
static inline float 	msec(unsigned int clk)		{return ((float)(clk)*(1000./50000000));}
static inline float 	microsec(unsigned int clk)	{return ((float)(clk)*(1000000./50000000));}
#elif NS6712
static inline int 		GetTicCount()				{return (*((volatile int *)(0x1940008)));}
static inline float 	msec(unsigned int clk)		{return ((float)(clk)*(1000./37500000));}
static inline float 	microsec(unsigned int clk)	{return ((float)(clk)*(1000000./37500000));}
#else
static inline int 		GetTicCount()				{return (*((volatile int *)(0x1940008)));}
static inline float 	msec(unsigned int clk)		{return ((float)(clk)*(1000./37500000));}
static inline float 	microsec(unsigned int clk)	{return ((float)(clk)*(1000000./37500000));}
#endif

void Delay(int ms);
void MicroDelay(int us);
void Beep(int msec);
void Sign(BOOL bOn);

#endif
