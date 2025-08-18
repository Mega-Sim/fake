#ifndef		__AMC_FSYSTEM_H
#define		__AMC_FSYSTEM_H

#include "amc_type.h"


#include <string.h>

#define		FSCMD_FILENUM		0x0100		// ������ ����
#define		FSCMD_FILENAME		0x0101		// ������ �̸�
#define		FSCMD_FILESIZE		0x0102		// ������ ũ��
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

// ����� ��ȯ�̳�, ������ ��ȯ�� ���ؼ� ���ǵ� ����ü.
// 2007.12.11, ckyu
typedef struct
{
    UINT	uiCrc32;	// ���۵� ��ü �������� crc32
    UINT	uiTotLen;	// ���۵� ��ü �������� ����Ʈ��
    USHORT	usBlkID;	// ���� �̿��� ���� ID
    USHORT	usCmd16;	// ���ϸ�ɿ� �̿��ϱ����� �ʵ�
    UCHAR	ucIndex;	// ���� ������ ���� �뵵.
    UCHAR	ucData;		// 0:cmd, 1:upload-data, 2:dnload-data
    USHORT	usPackLen;	// filler, 0 always
    
    UCHAR 	uszFileName[50];


	// ����� ���ϸ��� ����Ʈ���� �����Ѵ�.
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
	USHORT	usBytes;	// ��ȿ�� ����Ʈ��
	UCHAR	uszData[256];
} FILEBODY, *LPFILEBODY;

typedef struct
{
    UINT	uiCrc32;	// �ڿ� ���۵Ǵ� 50����Ʈ�� ���� crc32
    UINT	uiTotLen;		// ����ũ�⸦ �ø��� �̿�.
    UINT	uiValidBytes;	// ��ȿ�� ����Ʈ��
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





// I/F �Լ���
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
