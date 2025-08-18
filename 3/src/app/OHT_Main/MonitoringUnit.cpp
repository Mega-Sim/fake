//---------------------------------------------------------------------------
/**
@file    MonitoringUnit.cpp
$Rev:: 602           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2013-07-28 0#$:  Date of last commit
@version Monitor Ver 1.0
@brief   MonitoringUnit Class CPP File
*/
#include "MonitoringUnit.h"
#include "PUtily.h"

#include "OHTMain.h"

//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
MONITORING::MONITORING()
{
   gm_Monitoringinfo = new MONITOR_DATA_INFO;
   memset(gm_Monitoringinfo, 0x00, sizeof(MONITOR_DATA_INFO));
   loadParameter();
   m_bCheckWinSetup = false;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING �Ҹ���
@author  puting
@date    2013.07.22
*/
MONITORING::~MONITORING()
{
	SaveParameter();
 	delete gm_Monitoringinfo;
}

//---------------------------------------------------------------------------
/**
@brief   �ֱ������� ������Ʈ �ϴ� �׸��� �̰����� �����ȴ�
@author  puting
@date    2013.07.22
*/
void MONITORING::MonitorUpdate(MONITOR_DATA_INFO *pMonitoringinfo)   //�����忡�� �����ߵ� ��..
{
	static	DWORD	dwUpdateTrigerTime	= timeGetTime();     //�Լ� ���� �ð�.
	DWORD CurrTime = timeGetTime();
	static int nLogNotSaveCount=0;

	if(CurrTime - dwUpdateTrigerTime > 200)
	{
		//�������α׷� �׸� üũ(����͸� �׸�)
		pMonitoringinfo->MonitorOperStatus.bIs_OHTInk = File_Find(L"C:\\\\Documents and Settings\\Administrator\\���� �޴�\\���α׷�\\�������α׷�\\*.*",L"OHT.exe");
		pMonitoringinfo->MonitorOperStatus.bIs_AssistantInk=File_Find(L"C:\\\\Documents and Settings\\Administrator\\���� �޴�\\���α׷�\\�������α׷�\\*.*",L"OHTAssistant.exe");
		pMonitoringinfo->MonitorOperStatus.bIs_VNCInk=File_Find(L"C:\\\\Documents and Settings\\Administrator\\���� �޴�\\���α׷�\\�������α׷�\\*.*",L"Run WinVNC");

		//V3 üũ
		if(FileExists(L"C:\\\\Program Files\\ESTsoft\\ALYac\\AYUpdate.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x01;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\ESTsoft\\ALYac\\AYUpdate.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\ESTsoft\\ALYac\\VP\\PZPatch.dat"), 16);
		}
		else if (FileExists(L"C:\\\\Program Files\\AhnLab\\V3Lite\\V3Light.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x02;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3Lite\\V3Light.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3Lite\\MUpdate2\\supdate.log"), 16);
		}
		else if (FileExists(L"C:\\\\Program Files\\AhnLab\\V3\\V3Pro32.exe"))
		{
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x03;
			memmove(pMonitoringinfo->MonitorOperStatus.szVirusVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\V3\\V3Pro32.exe"), 16);
			memmove(pMonitoringinfo->MonitorOperStatus.szDBVersion, Get_FileAccessTime(L"C:\\\\Program Files\\AhnLab\\Smart Update Utility\\supdate.log"), 16);
		}
		else
			pMonitoringinfo->MonitorOperStatus.Is_VirusCheckProgram = 0x00;

		//���̷��� üũ���α׷� ���࿩�� üũ
		pMonitoringinfo->MonitorOperStatus.bIsVirusCheckProgramExec = Find_IsVirusCheckProgramExec();

		//Check Log Alive, USB Alive(����͸��׸�)
		if(pMonitoringinfo->MonitorOperStatus.LogSaveMode==LOGMODE_DRIVE)
		{
			pMonitoringinfo->MonitorOperStatus.bIs_LogSave = Get_IsLogDriveAlive("D:\\LOG");

			if(pMonitoringinfo->MonitorOperStatus.bIs_LogSave == 0x01) //dead
			{
			   nLogNotSaveCount++;
			   if(nLogNotSaveCount>50)        // about 10sec
			  {
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x02;  // USB Fault
					if(nLogNotSaveCount ==1000000) nLogNotSaveCount=60;   //�����÷��� ����
			  }
			}
			else
			{
				nLogNotSaveCount = 0;

				if(pMonitoringinfo->MonitorOperStatus.IPCSysInfo.nfreeSpace < 10000)  // If USB free space is under 10MB, then Error occure.
				{
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x01;  //  fully used
				}
				else
				{
					pMonitoringinfo->MonitorOperStatus.bIs_USBAlive = 0x00;  //  normal
				}
			}
		}

		//�αװ���(����͸� �׸�)
		pMonitoringinfo->MonitorOperStatus.nLogCount = GetNum_LogFile(L"D:\\\\LOG\\*.*");

		//Assistant ���α׷� ����Ȯ��(����͸� �׸�)
		OHTMainForm->VerInfo.GetFileVersionInformation(String(Application->ExeName + STR_FILENAME_OHTASSISTANT), "FileVersion");
		pMonitoringinfo->MonitorOperStatus.nAssistantVer = OHTMainForm->VerInfo.Major * 1000000 + OHTMainForm->VerInfo.Minor * 10000
														 + OHTMainForm->VerInfo.Release * 100 + OHTMainForm->VerInfo.Build;

		dwUpdateTrigerTime = CurrTime;
	}

}

bool MONITORING::loadParameter()
{
	bool bRetrun = false;
	double dTmpValue = 0.0;
	int    nTmpValue = 0;

	LIniFile *pIniFile;
	pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\MonitorSaveData.ini");

	if(!pIniFile->GetIniFileExist())
	{
		delete pIniFile;
		pIniFile = NULL;
	}

	if(pIniFile != NULL)
	{
		//2	(1)DRIVING_AXIS_PARAM
		//1.IniFile Read
		pIniFile->ReadDouble("Position","Driving", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_DrivingPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Hoist", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_HoistPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Shift", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_ShiftPosition = dTmpValue; //Return Value
		pIniFile->ReadDouble("Position","Rotate", 0.0, &dTmpValue);
		gm_Monitoringinfo->MonitorMovementData.dSum_RotatePosition = dTmpValue; //Return Value

		pIniFile->ReadInteger("MoveCount","Steer", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_SteerMoveCount = nTmpValue; //Return Value
		pIniFile->ReadInteger("MoveCount","Hand", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_HandMoveCount = nTmpValue; //Return Value
		pIniFile->ReadInteger("MoveCount","Shutter", 0, &nTmpValue);
		gm_Monitoringinfo->MonitorMovementData.nSum_ShutterMoveCount = nTmpValue; //Return Value


		delete pIniFile;
		bRetrun = true;
	}

	return bRetrun;

}

bool MONITORING::SaveParameter()
{
	bool bResult = false;

	LIniFile *m_lWriteIniFile;
	m_lWriteIniFile = NULL;
	m_lWriteIniFile = new LIniFile("DataFiles\\MonitorSaveData.ini");


	if(!m_lWriteIniFile->GetIniFileExist())
	{
		delete m_lWriteIniFile;
     	m_lWriteIniFile = NULL;
	}

	if(m_lWriteIniFile != NULL)
	{
		m_lWriteIniFile->DeleteIniFile();

		//1. [DRIVING_AXIS_PARAM]
		m_lWriteIniFile->WriteDouble("Position","Driving", gm_Monitoringinfo->MonitorMovementData.dSum_DrivingPosition);
		m_lWriteIniFile->WriteDouble("Position","Hoist", gm_Monitoringinfo->MonitorMovementData.dSum_HoistPosition);
		m_lWriteIniFile->WriteDouble("Position","Shift", gm_Monitoringinfo->MonitorMovementData.dSum_ShiftPosition);
		m_lWriteIniFile->WriteDouble("Position","Rotate", gm_Monitoringinfo->MonitorMovementData.dSum_RotatePosition);

		m_lWriteIniFile->WriteInteger("MoveCount","Steer", gm_Monitoringinfo->MonitorMovementData.nSum_SteerMoveCount);
		m_lWriteIniFile->WriteInteger("MoveCount","Hand", gm_Monitoringinfo->MonitorMovementData.nSum_HandMoveCount);
		m_lWriteIniFile->WriteInteger("MoveCount","Shutter", gm_Monitoringinfo->MonitorMovementData.nSum_ShutterMoveCount);

		delete m_lWriteIniFile;
		bResult = true;

	}

	return bResult;
}

//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
@remark  ����� �ѹ��� ���� �޾ƿ��� �׸��� �̰��� ����
*/
int MONITORING::MonitorInitData(MONITOR_OPERATION_STATUS *pOperationStatus)
{
	int nError=0;

	//FBWF ���� üũ
	pOperationStatus->bIsFBWF = Get_Dos_Cmd_Result(L"fbwfmgr");

	//�α� Save mode üũ
	/*
	AnsiString strOHTPath, strIniFileName;
	strOHTPath=ExtractFileDir(Application->ExeName);
	strIniFileName=AnsiString(strOHTPath+STR_DATAFILE_PATH)+"OHTPARAM.ini";
	TIniFile *TimeSetIni = new TIniFile (strIniFileName);
	pOperationStatus->LogSaveMode = TimeSetIni->ReadInteger("LOG","nIs_LogSaveMode",1);
	delete TimeSetIni;
	*/
    pOperationStatus->LogSaveMode = 2; //�ӽ÷� fixed

	//OHT, AMC ���� üũ
	pOperationStatus->nOHTVer = OHTMainForm->GetOHTProgramVersion().Info.Major * 1000000
												  + OHTMainForm->GetOHTProgramVersion().Info.Minor * 10000
												  + OHTMainForm->GetOHTProgramVersion().Info.Release * 100
												  + OHTMainForm->GetOHTProgramVersion().Info.Build;
	pOperationStatus->nAMCVer = OHTMainForm->GetHWSet()->GetAMCVersion();

    m_bCheckWinSetup = true;

   return nError;
}

//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
int MONITORING::Is_USBAlive(void)
{
   int nIsAlive=0;
    nIsAlive = Get_IsLogDriveAlive("D:\\LOG");

   return nIsAlive;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
int MONITORING::Get_IsLogDriveAlive(String strDrvName)
{
   TCHAR DrvRoot[4] = L"A:\\";
   int Drv;
   TCHAR VolName[MAX_PATH], FileSysName[10];
   DWORD VolSerial;
   bool bFind=false;
   AnsiString strTmpPath;
   AnsiString strFileFormat="";

   int nPos = strDrvName.Pos(":");
   if(nPos==0) return 0;

   strTmpPath = strDrvName.SubString(1, nPos-1);

   DWORD Drives = GetLogicalDrives();
   for(Drv=0; Drv<26; Drv++)
   {
      if((Drives >> Drv) & 0x00000001)
      {
         if((char)(Drv + 'A') == strTmpPath[1])
         {
            bFind=true;
            break;
         }
      }
   }

   if(bFind==false) return 0;

   DrvRoot[0] += Drv;
   GetVolumeInformation(DrvRoot, VolName, sizeof(VolName), &VolSerial, NULL, NULL, FileSysName, sizeof(FileSysName));

   strFileFormat=FileSysName;
   if((strFileFormat=="FAT32")||(strFileFormat=="NTFS")) return 0;
   else return 1;

}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
//���μ��� ���������� ã�� �����ϱ�

int MONITORING::Kill_VirusCheckProgram(void)
{
   int nError=0;

   HANDLE IdProcess, hToken;
   PROCESSENTRY32 Pren32 = {0} ;
   Pren32.dwSize = sizeof(PROCESSENTRY32);
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(hSnapshot == INVALID_HANDLE_VALUE) return 1;
   if(!Process32First(hSnapshot, &Pren32)) return 1;

   AnsiString sProcessName;
   do
   {
      sProcessName=Pren32.szExeFile;
      if((sProcessName=="V3LTray.exe")||(sProcessName=="V3LSvc.exe")||(sProcessName=="AYAgent.aye")||(sProcessName=="AYServiceNT.aye"))
      {
         if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
         {
			SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);

            if((IdProcess=OpenProcess(PROCESS_ALL_ACCESS,false,Pren32.th32ProcessID))!=NULL)
            {
               TerminateProcess(IdProcess,-1);
               CloseHandle(IdProcess);
            }
            SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);
            CloseHandle(hToken);
         }
	  }
   }while(Process32Next(hSnapshot, &Pren32));

   CloseHandle(hSnapshot);

   return nError;
}
*/
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
BYTE MONITORING::Find_IsVirusCheckProgramExec(void)
{
   int bFindCount=0;

   PROCESSENTRY32 Pren32 = {0} ;
   Pren32.dwSize = sizeof(PROCESSENTRY32);
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(hSnapshot == INVALID_HANDLE_VALUE) return bFindCount;
   if(!Process32First(hSnapshot, &Pren32)) return bFindCount;

   AnsiString sProcessName;
   do
   {
      sProcessName=Pren32.szExeFile;
      if((sProcessName=="V3LTray.exe")||(sProcessName=="V3LSvc.exe")||(sProcessName=="AYAgent.aye")||(sProcessName=="AYServiceNT.aye"))
      {
         bFindCount++;
      }
   }while(Process32Next(hSnapshot, &Pren32));

   CloseHandle(hSnapshot);

   return bFindCount;

}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
bool MONITORING::File_Find(String strTmpPath, String strPosData)
{
    bool bFind=false;
   TSearchRec sr;
   String strTmpFileName;

   if(FindFirst(strTmpPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //���丮�� �����ϰ� ���ϸ��� �˻��Ѵ�
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //�˻��� ������ �����̸�
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            strTmpFileName=sr.Name;
            if(strTmpFileName.Pos(strPosData)!=0) bFind=true;
         }
      }
	  if(bFind==false)
      {
         while(FindNext(sr) == 0)
         {
            if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
            {
               if((sr.Name == ".") || (sr.Name == "..")) continue;
               strTmpFileName=sr.Name;
			   if(strTmpFileName.Pos(strPosData)!=0)
               {
                  bFind=true;
                  break;
               }
            }
         }
      }
   }
   FindClose(sr);

   return bFind;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
char* MONITORING::Get_FileAccessTime(String strFileName)
{
   TSearchRec sr;
   TDateTime fDate;
   String strDate="";
   strDate.sprintf(L"%04d%02d%02d%02d%02d",0,0,0,0,0);

   if(FindFirst(strFileName, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
	//���丮�� �����ϰ� ���ϸ��� �˻��Ѵ�
	  if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
	  {
	  //�˻��� ������ �����̸�
		 if(!((sr.Name == ".") || (sr.Name == "..")))
		 {
			fDate = FileDateToDateTime(sr.Time);
			strDate = FormatDateTime("yyyymmddhhmm",fDate);
		 }
	  }
   }
   FindClose(sr);

   return WCharToChar(strDate.t_str());
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
//HANDLE hToken:��ū �ڵ�, LPCTSTR Privilege:Ȱ��/��Ȱ��ȭ�� ����, BOOL bEnablePrivilege:���� Ȱ��ȭ ����

BOOL MONITORING::SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege)
{
   TOKEN_PRIVILEGES tp;
   LUID luid;
   TOKEN_PRIVILEGES tpPrevious;
   DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);

   if(!LookupPrivilegeValue( NULL, Privilege, &luid )) return FALSE;

   // ������ ���� ���� ���
   tp.PrivilegeCount   = 1;
   tp.Privileges[0].Luid   = luid;
   tp.Privileges[0].Attributes = 0;

   AdjustTokenPrivileges(
                         hToken,
                         FALSE,
                         &tp,
                         sizeof(TOKEN_PRIVILEGES),
                         &tpPrevious,
                         &cbPrevious
                        );

   if(GetLastError() != ERROR_SUCCESS) return FALSE;

   // ������ ���� ������ ���� ���� �����ϱ�
   tpPrevious.PrivilegeCount       = 1;
   tpPrevious.Privileges[0].Luid   = luid;

   if(bEnablePrivilege)
   {
      tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
   }
   else
   {
      tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
      tpPrevious.Privileges[0].Attributes);
   }

   AdjustTokenPrivileges(
                         hToken,
                         FALSE,
                         &tpPrevious,
                         cbPrevious,
                         NULL,
                         NULL
                        );

   if(GetLastError() != ERROR_SUCCESS) return FALSE;

   return TRUE;
}
*/
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
int MONITORING::GetNum_LogFile(String Path)
{
   TSearchRec sr;
   int nCount=0;

   if(FindFirst(Path, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //���丮�� �����ϰ� ���ϸ��� �˻��Ѵ�
	  if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //�˻��� ������ �����̸�

		 if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            nCount++;
         }
      }
      while(FindNext(sr) == 0)
      {
         if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
         {
            if((sr.Name == ".") || (sr.Name == "..")) continue;
            nCount++;
         }
      }
   }
   FindClose(sr);

   return nCount;
}
//---------------------------------------------------------------------------
/**
@brief   MONITORING ������
@author  puting
@date    2013.07.22
*/
BYTE MONITORING::Get_Dos_Cmd_Result(TCHAR* strDosCmd)
{
   bool                    bflag;
   HANDLE                  hIwrite, hIread;
   HANDLE                  hOwrite, hOread;
   HANDLE                  hEwrite, hEread;
   SECURITY_ATTRIBUTES     sa;
   STARTUPINFO            siParam;
   PROCESS_INFORMATION     piParam;
   DWORD                   dwExitCode;

   char buffer[512];
   DWORD BytesRead;
   AnsiString strData;

   // Pipe Parameter Set
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = true;

  // memoDosCmd->Clear();            //puting


   // ���ϸӽ� ������ ����  // Input File
   bflag = CreatePipe(&hIread, &hIwrite, &sa, 0);
   if (!bflag)
   {
	 // memoDosCmd->Lines->Add("ERROR:Fail to open pipe");  puting
	  CloseHandle(hIread);
	  CloseHandle(hIwrite);
	  return false;
   }

   // ���ϸӽ� ������ ����  // output File
   bflag = CreatePipe(&hOread, &hOwrite, &sa, 0);
   if (!bflag)
   {
   //   memoDosCmd->Lines->Add("ERROR:Fail to open pipe");  puting
      CloseHandle(hIread);
      CloseHandle(hIwrite);
      CloseHandle(hOread);
      CloseHandle(hOwrite);
	  return false;
   }

   // ���ϸӽ� ������ ���� // Error File
   bflag = CreatePipe(&hEread, &hEwrite, &sa, 0);
   if (!bflag)
   {
	 // memoDosCmd->Lines->Add("ERROR:Fail to open pipe");    //puting
      CloseHandle(hIread);
      CloseHandle(hIwrite);
      CloseHandle(hOread);
      CloseHandle(hOwrite);
      CloseHandle(hEread);
	  CloseHandle(hEwrite);
	  return false;
   }

   // �ʱ�ȭ
   FillMemory(&siParam, sizeof(STARTUPINFO), 0);
   FillMemory(&piParam, sizeof(PROCESS_INFORMATION), 0);

   // Param ����
   siParam.cb          = sizeof(STARTUPINFO);
   siParam.dwFlags     = STARTF_USESTDHANDLES;
   siParam.wShowWindow = SW_HIDE;
   siParam.hStdInput = hIread;   // ǥ���Է�(stdin)
   siParam.hStdOutput = hOwrite;  // ǥ�����(stdout) �����̷���
   siParam.hStdError = hEwrite;   // ǥ�ؿ���(stderr) �����̷���

   hIread = CreateFile(L"input.d",            //ǥ���Է¿� input.d ���� ����
                       GENERIC_READ,
                       FILE_SHARE_READ,
					   &sa,
                       OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);

   bflag = CreateProcess(NULL, strDosCmd, NULL, NULL, true, DETACHED_PROCESS, NULL, NULL, &siParam, &piParam);
   WaitForInputIdle(GetCurrentProcess(), INFINITE);

   if(!bflag)
   {
     CloseHandle(hIread);
     CloseHandle(hIwrite);
     CloseHandle(hOread);
     CloseHandle(hOwrite);
     CloseHandle(hEread);
     CloseHandle(hEwrite);
	// memoDosCmd->Lines->Add("ERROR:Fail to create process");
	 return false;
   }

   // ���μ����� ����ɶ����� �����
   if(piParam.hProcess)
   {
      dwExitCode = STILL_ACTIVE;

      while(dwExitCode == STILL_ACTIVE)
      {
         WaitForSingleObject(piParam.hProcess, INFINITE);
         GetExitCodeProcess(piParam.hProcess, &dwExitCode);
         CloseHandle(piParam.hProcess);
         CloseHandle(piParam.hThread);
		// UpdateWindow(Handle);
	  }
   }

   CloseHandle(hIread);
   CloseHandle(hOwrite);
   CloseHandle(hEwrite);


   while(ReadFile(hOread, buffer, sizeof(buffer)-1, &BytesRead, NULL) && BytesRead)
   {
	buffer[BytesRead] = '\0';
   }

   CloseHandle(hOread);
   CloseHandle(hEread);
   CloseHandle(hIwrite);

   ///���÷��̺κ�  �� IsFBWF����

	if(wcscmp(strDosCmd, L"fbwfmgr")==0)
   {
	  int i;

	  TStringList* tmpResult = new TStringList;
	  tmpResult->Add(buffer);

	  for(i=0; i<tmpResult->Count; i++)
	  {
		 strData = tmpResult->Strings[i];

		 if(strData.Pos("filter state: enabled") != 0)
		 {
			delete tmpResult;
			OHTMainForm->memoMain->Lines->Add(L"FBWF Enable");
			return true;
		 }
		 else if(strData.Pos("filter state: disabled") != 0)
		 {
	        delete tmpResult;
			OHTMainForm->memoMain->Lines->Add(L"FBWF Disable");
			return false;
		 }
	  }

	  delete tmpResult;
   }

   return false;
}
//---------------------------------------------------------------------------

bool MONITORING::IsWinSetupLoaded()
{
    return m_bCheckWinSetup;
}
