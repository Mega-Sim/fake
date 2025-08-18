#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#define FILE_MODE_CREATE	1
#define FILE_MODE_READ		2
#define FILE_MODE_WRITE		3

BOOL 	InitFileSystem();   //파일 시스템을 초기화 한다. 파일 시스템을 사용하려면 이 함수를 반드시 실행하여야 한다.

//파일을 생성할 때는 다른함수를 이용하시오.
HANDLE 	FileOpen(const char * filename, unsigned int openmode);

BOOL 	IsEndOfFile(HANDLE hFile);
BOOL 	FileWrite(HANDLE hFile, const void *buf, unsigned int size);
int 	FileRead(HANDLE hFile, void *buf, unsigned int size);
BOOL 	FileClose(HANDLE hFile);
BOOL	FileClose_NoAct(HANDLE hFile);  // 11.11.18, syk 함수 추가
unsigned long FileGetSize(HANDLE hFile);

// 저장된 파일의 갯수를 리턴한다.
int 	GetNumberOfFile();

//저장된 파일의 이름을 얻어온는 함수.
BOOL 	GetFileName(int nIndex, char* sFileName, int nLength);



// 사용자는 아래 함수를 이용하지 말고, 다른이름의 함수를 이용하시오.
BOOL 	FileDelete(const char *filename);	
BOOL 	FormatFileSystem();


#endif
