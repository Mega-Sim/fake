#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#define FILE_MODE_CREATE	1
#define FILE_MODE_READ		2
#define FILE_MODE_WRITE		3

BOOL 	InitFileSystem();   //���� �ý����� �ʱ�ȭ �Ѵ�. ���� �ý����� ����Ϸ��� �� �Լ��� �ݵ�� �����Ͽ��� �Ѵ�.

//������ ������ ���� �ٸ��Լ��� �̿��Ͻÿ�.
HANDLE 	FileOpen(const char * filename, unsigned int openmode);

BOOL 	IsEndOfFile(HANDLE hFile);
BOOL 	FileWrite(HANDLE hFile, const void *buf, unsigned int size);
int 	FileRead(HANDLE hFile, void *buf, unsigned int size);
BOOL 	FileClose(HANDLE hFile);
BOOL	FileClose_NoAct(HANDLE hFile);  // 11.11.18, syk �Լ� �߰�
unsigned long FileGetSize(HANDLE hFile);

// ����� ������ ������ �����Ѵ�.
int 	GetNumberOfFile();

//����� ������ �̸��� ���´� �Լ�.
BOOL 	GetFileName(int nIndex, char* sFileName, int nLength);



// ����ڴ� �Ʒ� �Լ��� �̿����� ����, �ٸ��̸��� �Լ��� �̿��Ͻÿ�.
BOOL 	FileDelete(const char *filename);	
BOOL 	FormatFileSystem();


#endif
