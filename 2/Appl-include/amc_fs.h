#ifndef		__AMC_FSYSTEM_H
#define		__AMC_FSYSTEM_H

#include "amc_type.h"


#include <string.h>

#define		FSCMD_FILENUM		0x0100		// 파일의 갯수
#define		FSCMD_FILENAME		0x0101		// 파일의 이름
#define		FSCMD_FILESIZE		0x0102		// 파일의 크기
#define		FSCMD_CPUTYPE_READ	0x0000
#define		FSCMD_CPUTYPE_WRITE	0x0001
#define		FSCMD_FILE_UPLOAD	0x0110
#define		FSCMD_FILE_DNLOAD	0x0111
#define		FSCMD_BL_DNLOAD		0x0112
#define		FSCMD_FS_FORMAT		0x0200
#define		FSCMD_DEL_FILE		0x0202
#define		FSCMD_SET_BOOT		0x0203
#define		FSCMD_GET_SYSTEM	0x0300

#define		FSCMD_DUMP_SYSDATA	0xff00
#define		FSCMD_DUMP_FILETBL	0xff01
#define		FSCMD_DUMP_FAT		0xff02



typedef		BYTE		UCHAR;

// 명령의 교환이나, 파일의 교환을 위해서 정의된 구조체.
// 2007.12.11, ckyu
typedef struct
{
    UINT	uiCrc32;	// 전송될 전체 데이터의 crc32
    UINT	uiTotLen;	// 전송될 전체 데이터의 바이트수
    USHORT	usBlkID;	// 현재 이용할 블럭의 ID
    USHORT	usCmd16;	// 단일명령에 이용하기위한 필드
    UCHAR	ucIndex;	// 순서 지정을 위한 용도.
    UCHAR	ucData;		// 0:cmd, 1:upload-data, 2:dnload-data
    USHORT	usPackLen;	// filler, 0 always
    
    UCHAR 	uszFileName[50];


	// 저장된 파일명의 바이트수를 리턴한다.
	int SetFilename(char *pname)
	{
		int len = strlen(pname);
		if (len >= sizeof(uszFileName)) len = sizeof(uszFileName) - 1;
		strncpy((char *) uszFileName, pname, len);
		uszFileName[len] = '0';
		return len;
	}
} FILECMD, *LPFILECMD;

typedef struct
{
	UINT	uiCrc32;
	USHORT	usBytes;	// 유효한 바이트수
	UCHAR	uszData[256];
} FILEBODY, *LPFILEBODY;

typedef struct
{
    UINT	uiCrc32;	// 뒤에 전송되는 50바이트에 대한 crc32
    UINT	uiTotLen;		// 파일크기를 올릴때 이용.
    UINT	uiValidBytes;	// 유효한 바이트수
    USHORT	usFiller;
    USHORT	usPackLen;
    UCHAR	uszCmd[50];

	void GetFileName(char *pdstn)
	{
		int len = uiTotLen - 16;
		memcpy(pdstn, uszCmd, len);
		pdstn[len] = '\0';
	}
	int SetFileName(char *psrc)
	{
		int len = strlen(psrc);
		if (len >= sizeof(uszCmd)) len = sizeof(uszCmd) - 1;
		strncpy((char *) uszCmd, psrc, len);
		uszCmd[len] = '\0';
		return len;
	}
} FILECMD_ACK, *LPFILECMD_ACK;


typedef struct
{
	char m_sName[60];
	unsigned int m_nSize;
	unsigned char *m_pBuffer;
} FILE_DESC;




typedef struct _SYSTEM_DATA
{
	int m_nCPUType;   //0:6712, 1:6713
	char m_sRunFileName[48];
}SYSTEM_DATA;





// I/F 함수들
// 2009.1.9, ckyu
int		_dsp_fs_files();
void	_dsp_fs_getfilename(int ofs, char *pdstn);
unsigned int _dsp_fs_getfilesize(int ofs, unsigned long *plen);
unsigned int _dsp_fs_getfilenamesize(char *pszname, unsigned long *plen);
BOOL	_dsp_fs_deletefile(char *pszname);
void	_dsp_fs_setbootfile(char *pszname);
void	_dsp_fs_format();
void	_dsp_fs_getsysteminfo(SYSTEM_DATA *p);
HANDLE	_dsp_fs_open(char *pszname);
void	_dsp_fs_close(HANDLE hfile);
int		_dsp_fs_read(HANDLE hfile, void *p, unsigned int nread);
void	_dsp_fs_makefile(char *pszname, void *p, unsigned int nwrite);










#endif
