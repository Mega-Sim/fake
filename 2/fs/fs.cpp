#include "6xsystem.h"
#include "filesystem.h"
#include "axisdpr.h"
#include "../Appl-include/amc_fs.h"
#include "i28f128x16.h"
#include "system.h"

#include "crc32.h"


static FILECMD sFileCmd;
static FILE_DESC m_sfd = {"", 0, NULL};
static FILECMD_ACK	sFileCmdAck;





// 아래는 내부적으로 이용되는 함수들임.
void FormatSystemData();
SYSTEM_DATA* GetSystemData();
BOOL WriteSystemData(const SYSTEM_DATA* pData);
void InitializeFileSystemFunction();
//11.11.18 syk//  함수 원형 변경
int WriteFile(FILE_DESC *pfd);		
//void WriteFile(FILE_DESC *pfd);



void DumpSysData();
void DumpFileTbl();
void DumpFat();




void PushAck(FILECMD_ACK *pAck)
{
	int nSize = 16;
	char *ptr = (char *) pAck;
	
	if (strlen((const char *)pAck->uszCmd)) nSize += strlen((const char *)pAck->uszCmd);
	pAck->usPackLen = nSize;
	for (int i = 0; i < nSize; i ++)
		WriteChannel(MAP_DPRAM_ADDRESS, i, *ptr ++);
}




INT _dsp_fs_files()
{
	return GetNumberOfFile();
}

void _dsp_fs_getfilename(int ofs, char *pdstn)
{
	GetFileName(ofs, pdstn, 50);
}

UINT _dsp_fs_getfilesize(int ofs, ULONG *plen)
{
	char szname[100];
	_dsp_fs_getfilename(ofs, szname);
	return _dsp_fs_getfilenamesize(szname, NULL);
}
	
UINT _dsp_fs_getfilenamesize(char *pszname, ULONG *plen)
{
	HANDLE hfile = FileOpen(pszname, FILE_MODE_READ);
	UINT uilen = FileGetSize(hfile);
	FileClose(hfile);
	if (plen != NULL) *plen = uilen;
	return uilen;
}

void _dsp_fs_getsysteminfo(SYSTEM_DATA *p)
{
	memcpy(p, GetSystemData(), sizeof(SYSTEM_DATA));
}

HANDLE _dsp_fs_open(char *pszname)
{
    return FileOpen(pszname, FILE_MODE_READ);
}
void _dsp_fs_close(HANDLE hfile)
{
	FileClose(hfile);
}
INT _dsp_fs_read(HANDLE hfile, void *p, UINT nread)
{
	return FileRead(hfile, p, nread);
}
void _dsp_fs_makefile(char *psz, void *p, int len)
{
	_dsp_fs_deletefile(psz);

	FILE_DESC fd;
	fd.m_nSize = len;
	fd.m_pBuffer = (unsigned char *) p;
	strcpy(fd.m_sName, psz);	
	WriteFile(&fd);
}


void FlashWriteEnable(int ben)
{
	MEM(McASP1_PFUNC) |= 0x20000000; 		// ACLKR를 I/O 모드로 설정
	MEM(McASP1_PDIR) |= 0x20000000; 		// ACLKR를 Output으로 설정
	if (ben)
		MEM(McASP1_PDOUT) |= 0x20000000; 		// ACLKR를 High로 출력
	else
		MEM(McASP1_PDOUT) &= ~0x20000000; 		// ACLKR를 High로 출력
}


void _dsp_fs_setbootfile(char *pszName)
{
	SYSTEM_DATA sdat;
	memcpy(&sdat, GetSystemData(), sizeof(sdat));
	strcpy(sdat.m_sRunFileName, pszName);
	sdat.m_nCPUType = 0x01;
	
	FlashWriteEnable(1);
	WriteSystemData(&sdat);
	FlashWriteEnable(0);
}


void WriteBootLoader(FILE_DESC *pfd)
{
	UCHAR * temp;
	temp = (unsigned char*)malloc(0x20000);
	memset(temp,0xff,0x20000);
	memcpy(temp,pfd->m_pBuffer,pfd->m_nSize);

	FlashWriteEnable(1);

	FLASH_Read(0x00010000, temp+0x10000, 0x10000);//Backup FPGA Program
	FLASH_EraseBlock(0);
	FLASH_Write(0x00000000, temp, 0x20000);

	FlashWriteEnable(0);

	free(temp);
}

//11.11.18 syk//  write 성공 여부 리턴 값 필요
//원인 : 사이즈 제한 및 write 공간 부족 알림
//해결 : 함수 원형 변경
//1 : write 성공
//2 : a.out의 파일사이즈가 크다.
//3 : flash에 write공간 부족.
//void WriteFile(FILE_DESC *pfd)
int WriteFile(FILE_DESC *pfd)
{
	HANDLE FileHandle;
	
	FlashWriteEnable(1);

	FileHandle = FileOpen(pfd->m_sName, FILE_MODE_CREATE);
//11.11.18 syk//  사이즈 제한
//원인 : file handler의 m_pcontents[1000][4080] 사이즈임.
//해결 : 사이즈가 이보다 크면 write 미실행.
	if(pfd->m_nSize > 4080*1000)   // 사이즈 체크
	{
		FileClose_NoAct(FileHandle); 
		FlashWriteEnable(0);
		return 2;
	}

	if(!FileWrite(FileHandle, pfd->m_pBuffer, pfd->m_nSize)) // Handler에 써진 사이즈 체크
	{
		FileClose_NoAct(FileHandle); 
		FlashWriteEnable(0);	
		return 2;
	}

	if(!FileClose(FileHandle)) // flash 남아있는 공간 체크
	{
		FlashWriteEnable(0);
		return 3;
	}
    
	FlashWriteEnable(0);
	return 1;
}

BOOL _dsp_fs_deletefile(char *pszFileName)
{
	BOOL bf;
	FlashWriteEnable(1);

	bf = FileDelete(pszFileName);
	
	FlashWriteEnable(0);
	return bf;
}


void InitializeFileSystemFunction()
{
	FMCTRL=2;//FRP;					// write-protect and normal flash	
	FLASH_Init();		// init flash control ports
	ControlFLASH(0,2);//FRP);	
}

void _dsp_fs_format()
{
	FlashWriteEnable(1);
	FormatFileSystem();
	FormatSystemData();
	FlashWriteEnable(0);
}







// 이 함수는 파일시스템과 관련된 동작을 수행하기위한 용도임.
// 2007.12.11, ckyu

void FSExchange()
{
	int i;
	char * pMapStartBlock = MAP_DPRAM_ADDRESS;
	char szStr[200];
	HANDLE hFile;
	UINT uiSize;
	SYSTEM_DATA stSystemData;
	
	sFileCmd.uiCrc32 = ReadChannel(pMapStartBlock, 0);
	sFileCmd.uiTotLen = ReadChannel(pMapStartBlock, 4);
	sFileCmd.usBlkID = ReadChannel2(pMapStartBlock, 8);
	sFileCmd.usCmd16 = ReadChannel2(pMapStartBlock, 10);
	sFileCmd.ucIndex = ReadChannel1(pMapStartBlock, 12);
	sFileCmd.ucData = ReadChannel1(pMapStartBlock, 13);
	sFileCmd.usPackLen = ReadChannel2(pMapStartBlock, 14);
	
	memset(&sFileCmdAck, 0, sizeof(sFileCmdAck));
	if (sFileCmd.ucData == 0)
	{
		// 명령일 경우만 파일명을 가져온다.
		if (sFileCmd.usPackLen > 16)
		{
			for (i = 0; i < sFileCmd.usPackLen - 16; i ++)
			    sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, i + 16);
			sFileCmd.uszFileName[i] = 0;
		}
		UINT uiCrc = _crc32_get_crc((char*) &sFileCmd.uiTotLen, sFileCmd.usPackLen - 4);
		if (uiCrc != sFileCmd.uiCrc32) 
		{
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			return;
		}
		
		// 연속동작해야하는 명령과 한번에 끝나는 명령을 구분한다.
		switch (sFileCmd.usCmd16)
		{
		case FSCMD_FILENUM://0x0100:	// 파일의 갯수 리턴하기
			sFileCmdAck.uiTotLen = GetNumberOfFile();
			PushAck(&sFileCmdAck);
		    return;
		case FSCMD_FILENAME:	// 파일이름 가져오기
			if(GetFileName(sFileCmd.ucIndex, szStr+100, 50)==TRUE)
				strcpy((char *) sFileCmdAck.uszCmd, szStr+100);
			else
				strcpy((char *) sFileCmdAck.uszCmd, "");
			PushAck(&sFileCmdAck);
			return;
		case FSCMD_FILESIZE:	// 파일크기 가져오기
			if (sFileCmd.usPackLen <= 16)
				// 파일명이 없는경우
				GetFileName(sFileCmd.ucIndex, szStr, 100);
			else
				strcpy(szStr, (char *) sFileCmd.uszFileName);
			{
				hFile = FileOpen(szStr, FILE_MODE_READ);
//				sprintf(sFileCmdAck.uszCmd, "[0102>%d]", FileGetSize(hFile));
				sFileCmdAck.uiTotLen = FileGetSize(hFile);
				FileClose(hFile);
			}
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_CPUTYPE_READ://0x0000:	// CPU type 읽기
			sFileCmdAck.uiTotLen = GetSystemData()->m_nCPUType;
//			sprintf(sFileCmdAck.uszCmd, "[0000>%d]", GetSystemData()->m_nCPUType);
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_CPUTYPE_WRITE://0x0001:	// CPU type 쓰기
			memcpy(&stSystemData, GetSystemData(), sizeof(SYSTEM_DATA));
			stSystemData.m_nCPUType = sFileCmd.ucIndex;
			FlashWriteEnable(1);
			WriteSystemData(&stSystemData);
			FlashWriteEnable(0);
			sFileCmdAck.uiValidBytes = sFileCmd.ucIndex;
			PushAck(&sFileCmdAck);
			return;
		case FSCMD_FS_FORMAT://0x0200:	// 파일시스템 포멧
			_dsp_fs_format();
			
			sprintf(szStr, "File system format!\r\n");
			SERIAL_WriteBytes(szStr, strlen(szStr));
			
			sFileCmdAck.uiValidBytes = 1;
			PushAck(&sFileCmdAck);
			return;
		case FSCMD_DEL_FILE://0x0202:	// 파일삭제
			_dsp_fs_deletefile((char *) sFileCmd.uszFileName);
			sFileCmdAck.uiValidBytes = 1;
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_SET_BOOT://0x0203:	// 부트파일 설정
			_dsp_fs_setbootfile((char*) sFileCmd.uszFileName);
			
			sprintf(szStr, "Set Boot: '%s'\r\n", sFileCmd.uszFileName);
			SERIAL_WriteBytes(szStr, strlen(szStr));
			
			sFileCmdAck.uiValidBytes = 1;
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_GET_SYSTEM://0x0300:	// 시스템정보 얻기
			memcpy(&stSystemData, GetSystemData(), sizeof(SYSTEM_DATA));
			sFileCmdAck.uiValidBytes = stSystemData.m_nCPUType;
			sFileCmdAck.SetFileName( stSystemData.m_sRunFileName);
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_FILE_UPLOAD://0x0110:	// 파일내용 upload
			strcpy(m_sfd.m_sName, (char*) sFileCmd.uszFileName);
			hFile = FileOpen(m_sfd.m_sName, FILE_MODE_READ);
			
			if (hFile == NULL) uiSize = 0;
			else {
				uiSize = FileGetSize(hFile);
				if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
				m_sfd.m_nSize = uiSize;
				m_sfd.m_pBuffer = (unsigned char*)malloc(uiSize);
		        FileRead(hFile, m_sfd.m_pBuffer, uiSize);
				FileClose(hFile);
				
				printf("File upload: @ 0x%p, size=%d\n", m_sfd.m_pBuffer, m_sfd.m_nSize);
			}
			
			// 크기를 올려 보내면, 다음부터는 크기와 블럭ID를 내려보낸다.
			sFileCmdAck.uiTotLen = uiSize;
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_FILE_DNLOAD://0x0111:	// 파일 저장하기
			for (i = 0; i < sFileCmd.usPackLen - 16; i ++)
				sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, 16 + i);
			sFileCmd.uszFileName[i] = 0;
			
			strcpy((char *) m_sfd.m_sName, (char *) sFileCmd.uszFileName);
			m_sfd.m_nSize = sFileCmd.uiTotLen;
	
			if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
			m_sfd.m_pBuffer = (UCHAR *) malloc(m_sfd.m_nSize);

			sprintf(szStr, "File download: @ 0x%p, name=%s, size=%d\r\n", m_sfd.m_pBuffer, m_sfd.m_sName, m_sfd.m_nSize);
			SERIAL_WriteBytes(szStr, strlen(szStr));

			sFileCmdAck.uiValidBytes = 1;
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_BL_DNLOAD://0x0112:	// 부트로더 내려보내기
			for (i = 0; i < sizeof(sFileCmd.uszFileName); i ++)
				sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, 16 + i);
			sFileCmd.uszFileName[i] = 0;
			
			strcpy((char *) m_sfd.m_sName, (char *) sFileCmd.uszFileName);
			m_sfd.m_nSize = sFileCmd.uiTotLen;
	
			if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
			m_sfd.m_pBuffer = (UCHAR *) malloc(m_sfd.m_nSize);
			printf("BL-File download: @ 0x%p, size=%d\n", m_sfd.m_pBuffer, m_sfd.m_nSize);

			sFileCmdAck.uiValidBytes = 1;
			PushAck(&sFileCmdAck);
			return;
			
		case FSCMD_DUMP_SYSDATA:
			DumpSysData();
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			return;		

		case FSCMD_DUMP_FILETBL:
			DumpFileTbl();
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			return;		

		case FSCMD_DUMP_FAT:
			DumpFat();
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			return;		

		default:
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			return;		
		}
	}
	
	// for file upload 
	if (sFileCmd.ucData == 1 && sFileCmd.uiTotLen != 0)
	{
		//  IPC로 파일의 내용을 올린다.
		UINT uiOfs = sFileCmd.usBlkID * 256;
		int nLen = min(sFileCmd.uiTotLen - uiOfs, 256);
		if ((sFileCmd.usBlkID % 20) == 0)
		{
			sprintf(szStr, "(0x%p) : Blk=%d, Read=%d, Ofs=%d\r\n", &m_sfd.m_pBuffer[uiOfs], sFileCmd.usBlkID, nLen, uiOfs);
			SERIAL_WriteBytes(szStr, strlen(szStr));
		}

		sFileCmdAck.usPackLen = 16;	// 헤더의 길이
		sFileCmdAck.uiTotLen = nLen;	// 데이터의 바이트수
		sFileCmdAck.uiCrc32 = _crc32_get_crc((char *) &m_sfd.m_pBuffer[uiOfs], nLen);
		sFileCmdAck.uiValidBytes = 1;

		PushAck(&sFileCmdAck);
		for (int i = 0; i < nLen; i ++)
			WriteChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i, m_sfd.m_pBuffer[uiOfs + i]);
	
	}
	// for file download
	else if (sFileCmd.ucData == 2 && sFileCmd.uiTotLen != 0)
	{
		// IPC에서 파일의 내용을 내려보낸다.
		// 모든 패킷마다 응답을 보낸다.
		UINT uiOfs = sFileCmd.usBlkID * 256;
		int nLen = min(sFileCmd.uiTotLen - uiOfs, 256);
		UINT uicrc = _crc32_get_crc(pMapStartBlock + EDUMP_OFS_DATABLOCK, nLen);
		if (sFileCmd.uiCrc32 != uicrc)
		{
			sFileCmdAck.uiValidBytes = 0;
			PushAck(&sFileCmdAck);
			
			sprintf(szStr, "Fail: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
			SERIAL_WriteBytes(szStr, strlen(szStr));
			
			free(m_sfd.m_pBuffer);
			m_sfd.m_pBuffer = NULL;
		} else 
		{
			// crc 가 일치한 경우
			if ((sFileCmd.usBlkID % 20) == 0)
			{
//				sprintf(szStr, "Sucd(0x%p): Tot=%d, blkid=%d, Ofs=%d, len=%d\r\n", &m_sfd.m_pBuffer[uiOfs], sFileCmd.uiTotLen, sFileCmd.usBlkID, uiOfs, nLen);
//				SERIAL_WriteBytes(szStr, strlen(szStr));
			}
			
			if (m_sfd.m_pBuffer)
			{
				memcpy(&m_sfd.m_pBuffer[uiOfs], pMapStartBlock + EDUMP_OFS_DATABLOCK, nLen);
				sFileCmdAck.uiValidBytes = 1;
			} else {
				sFileCmdAck.uiValidBytes = 0;
			}

			
			if (uiOfs + nLen >= sFileCmd.uiTotLen)
			{
				
				// 성공이므로 그에 맞는 동작을 수행한다.
				// 즉 부트로더 쓰기 또는 파일 쓰기등...
				if (sFileCmd.usCmd16 == FSCMD_FILE_DNLOAD) 
				{
					//11.11.18 syk//  writefile 실행 후 리턴값 ack , usFiller 사용안하는 듯하여 사용함.
					sFileCmdAck.usFiller = WriteFile(&m_sfd);
					if(sFileCmdAck.usFiller ==1)  	sprintf(szStr, "FileWrite: End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
					if(sFileCmdAck.usFiller ==2)	sprintf(szStr, "ERROR(Appl File Size Over): End: Tot=%d\r\n", sFileCmd.uiTotLen);
					if(sFileCmdAck.usFiller ==3)	sprintf(szStr, "ERROR(Lack of Space): End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);									
				}
				else if (sFileCmd.usCmd16 == FSCMD_BL_DNLOAD)
				{
					WriteBootLoader(&m_sfd);
					//11.11.18 syk//  writefile 실행 후 리턴값 ack , usFiller 사용안하는 듯하여 사용함.					
					sFileCmdAck.usFiller =1;
					sprintf(szStr, "Boot FileWrite: End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
				}
				SERIAL_WriteBytes(szStr, strlen(szStr));
	
				free(m_sfd.m_pBuffer);
				m_sfd.m_pBuffer = NULL;
			}
			PushAck(&sFileCmdAck);	
		}
	}
}




void DumpSysData()
{
	char str[50];
	SYSTEM_DATA sdat;
	memcpy(&sdat, GetSystemData(), sizeof(SYSTEM_DATA));
	unsigned char *puc = (unsigned char *) &sdat;
	
	sprintf(str, "%s():\r\n", "DumpSysData");
	SERIAL_WriteBytes(str, strlen(str));
	for (int i = 0; i < sizeof(SYSTEM_DATA); i ++) 
	{
		sprintf(str, "%02X ", *puc ++);
		SERIAL_WriteBytes(str, strlen(str));
	}
	SERIAL_WriteBytes("\r\n", 2);
}

void DumpFileTbl()
{
}

void DumpFat()
{
}


