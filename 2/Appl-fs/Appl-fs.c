#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "amc_type.h"
#include "callback.h"
#include "watchdog.h"
#include "amc_app.h"
#include "amc_kernel.h"
#include "amc_fs.h"

#include <string.h>

void Appl_Body_Main();
void Appl_Prev_Main();




#define		TEST_PROMPT		"\r\nAMC-fs%"



void Appl_Int10()
{
}


void __App_First()
{
	__dsplib_set_callback(CALLBACK_PREV_MAIN_LOOP, Appl_Prev_Main);
	__dsplib_set_callback(CALLBACK_BODY_MAIN_LOOP, Appl_Body_Main);
	__dsplib_set_callback(CALLBACK_INT10, Appl_Int10);
	
	InitializeFileSystemFunction();
}


void PutPrompt()
{
	SERIAL_WriteBytes(TEST_PROMPT, strlen(TEST_PROMPT));
}

void Appl_Prev_Main()
{
	PutPrompt();
}

void DoCmd(char *pszCmd, int ofs);

static char szCmd[100];
static char nszCmdOfs = 0;
void Appl_Body_Main()
{
    unsigned char szstr[100];
    
    int nRcv = SERIAL_ReadBytes(szstr, 100, 0);
    if (nRcv < 1) return;
    
    for (int i = 0; i < nRcv; i ++)
    {
    	unsigned char ch = szstr[i];
    	
		if (ch == '\r')
		{
			SERIAL_WriteBytes("\r\n", 2);
			szCmd[nszCmdOfs] = '\0';
		    DoCmd(szCmd, nszCmdOfs);
		    nszCmdOfs = 0;
		    SERIAL_WriteBytes("\r\n", 2);
		    PutPrompt();
		} else if (ch == '\n');
		else {
			if (ch == '\b')
			{
			    if (1 <= nszCmdOfs)
			    {
			        szCmd[--nszCmdOfs] = ch;
			    	SERIAL_WriteBytes(&ch, 1);		// echo back
			    	SERIAL_WriteBytes(" ", 1);
			    	SERIAL_WriteBytes(&ch, 1);		// echo back
			    } else 
			    {
			    	ch = 0x07;
			    	SERIAL_WriteBytes(&ch, 1);
			    }
			} else
			{
				szCmd[nszCmdOfs ++] = ch;
				if (nszCmdOfs >= 100)
				{
					nszCmdOfs = 0;
					SERIAL_WriteBytes("(!!!)\r\n", 7);
				}
		    	else SERIAL_WriteBytes(&ch, 1);		// echo back
			}
		}
    }
}

// 남은 갯수를 리턴한다.
int RemoveChar(void *psz, unsigned char ch)
{
    int rd, wr, len;
    char *p = (char *) psz;
    
    rd = wr = 0;
    len = strlen(p);
    for (rd = 0; rd < len; rd ++)
    {
    	if (p[rd] == ch) continue;
    	p[wr++] = p[rd];
    }
    p[wr] = '\0';
    return wr;
}

/*
	CMD's

	"ls"	: show file list
	"rm f.n"	: delete the file 'f.n'
	"mk test"	: test file creation

*/

void Do_ls();
void Do_rm(char *pszParam1);
void Do_cpu();
void Do_mk(char *pszParam1);
void Do_boot(char *pszParam1);

void PutHelp(char *psz)
{
	SERIAL_WriteBytes(psz, strlen(psz));
	SERIAL_WriteBytes("\r\n", 2);
}
void Do_help()
{
	PutHelp("'ls'\t저장되어있는 파일목록을 출력한다.");
	PutHelp("'rm f.n'\tf.n 파일을 삭제한다.");
	PutHelp("'cpu'\tCPU type을 출력한다. 보통'1'");
	PutHelp("'mk f.n'\tf.n파일을 생성하고 테스트문자를 써 넣는다.");
	PutHelp("'boot f.n'\t지정한 파일로 이후 부팅한다. f.n이 없으면 파일명을 출력한다.");
}

void DoCmd(char *pszCmd, int cmdofs)
{
	if (strcmp(pszCmd, "?") == 0 || strcmp(pszCmd, "help") == 0) Do_help();
    else if (strcmp(pszCmd, "ls") == 0) Do_ls();
    else if (strncmp(pszCmd, "rm ", 3) == 0) Do_rm(strchr(pszCmd, ' ') + 1);
    else if (strcmp(pszCmd, "cpu") == 0) Do_cpu();
    else if (strncmp(pszCmd, "mk ", 3) == 0) Do_mk(strchr(pszCmd, ' ') + 1);
    else if (strncmp(pszCmd, "boot", 4) == 0) Do_boot(strchr(pszCmd, '=') + 0);
    
}

void Do_boot(char *pszParam1)
{
    char fn[100];
    
    RemoveChar(pszParam1, ' ');
    if (pszParam1[0] != '=') return;
    
    if (pszParam1[1] == '?')
    {
    	// boot 파일명을 출력한다.
    	sprintf(fn, "Boot: '%s'\r\n", GetSystemData()->m_sRunFileName);
    	SERIAL_WriteBytes(fn, strlen(fn));
    } else if (pszParam1[1] == '\0')
    {
    } else {
        sscanf(pszParam1+1, "%s", fn);
    	ChangeBootFile(fn);
    }
}
        
#define		TEST_STRING		"file testing string. 2008.1.2"
void Do_mk(char *pszParam1)
{
	FILE_DESC m_sfd;
	int len = strlen(TEST_STRING);
	
    if (strlen(pszParam1) < 1) return;
    
    strcpy(m_sfd.m_sName, pszParam1);
    m_sfd.m_nSize = len;
    m_sfd.m_pBuffer = (UCHAR *) malloc(len);
    memcpy(m_sfd.m_pBuffer, TEST_STRING, len);
    
    WriteFile(&m_sfd);
    free(m_sfd.m_pBuffer);
}

void Do_cpu()
{
	char str[100];
	sprintf(str, "CPU type is '%d'", GetSystemData()->m_nCPUType);
	SERIAL_WriteBytes(str, strlen(str));
}

void Do_ls()
{
	HANDLE hFile;
	char fname[100];
	SERIAL_WriteBytes((unsigned char *) "\r\n", 2);
	
	int nfile = GetNumberOfFile();
	for (int i = 0; i < nfile; i ++)
	{
	    if (GetFileName(i, fname+1, 100) == TRUE) 
	    {
	    	RemoveChar(fname+1, ' ');
	    	if (strcmp(GetSystemData()->m_sRunFileName, fname+1) == 0) fname[0] = '*';
	    	else fname[0] = ' ';
	    	SERIAL_WriteBytes(fname, strlen(fname));
	    	SERIAL_WriteBytes("\t", 1);
	    	
	    	hFile = FileOpen(fname, FILE_MODE_READ);
	    	if (hFile)
	    	{
	    		sprintf(fname, "%10d\r\n", FileGetSize(hFile));
	    		FileClose(hFile);
	    	} else sprintf(fname, "\r\n");
	    	
    		SERIAL_WriteBytes(fname, strlen(fname));
	    }
	}
	sprintf(fname, "\r\nTotal %d files\r\n", nfile);
	SERIAL_WriteBytes(fname, strlen(fname));
}

void Do_rm(char *pszParam1)
{
	char fname[100];
	char str[100];
	if (strlen(pszParam1))
	{
		sscanf(pszParam1, "%s", fname);
		sprintf(str, "File '%s' delete...", fname);
		if (DeleteFile(fname) == TRUE)
			sprintf(str+strlen(str), "success");
		else
			sprintf(str + strlen(str), "fail");
		SERIAL_WriteBytes(str, strlen(str));
	}
    SERIAL_WriteBytes((unsigned char *) "\r\n", 2);
}












