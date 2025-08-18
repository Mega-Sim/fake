// MapExtract.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "MapExtract.h"
#include <iostream>
#include <fstream>
#include "MapFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ������ ���� ���α׷� ��ü�Դϴ�.

CWinApp theApp;

using namespace std;

CString GetWorkDir()
{
	static CString strWorkingDirectory;

	if (strWorkingDirectory == "")
	{
		TCHAR szModuleFileName[_MAX_PATH];
		::GetModuleFileName(::AfxGetInstanceHandle(), szModuleFileName, _MAX_PATH);

		TCHAR szDrive[_MAX_DRIVE], szPath[_MAX_PATH];
		//_splitpath(szModuleFileName, szDrive, szPath, NULL, NULL);
		_splitpath_s(szModuleFileName, szDrive, _MAX_DRIVE, szPath, _MAX_PATH, NULL, 0, NULL, 0);

		strWorkingDirectory.Format("%s%s", szDrive, szPath);
	}

	return strWorkingDirectory;
}

int _FileAge(CString szFileName)
{
	HANDLE  Handle;
	WIN32_FIND_DATA FindData;
	FILETIME LocalFileTime;
	WORD wTime = 0, wDate = 0;


	Handle = FindFirstFile(LPCTSTR(szFileName), &FindData);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		FindClose(Handle);
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			FileTimeToLocalFileTime(&FindData.ftLastWriteTime, &LocalFileTime);
			if (FileTimeToDosDateTime(&LocalFileTime, &wDate, &wTime))
				return MAKELONG(wTime, wDate);
		}
	}

	return -1;
}

BOOL FileExists(CString szFileName)
{
	return (_FileAge(szFileName) != -1) ? TRUE : FALSE;
}

CString gStringFormat(LPCTSTR fmtstr, ...)
{
	TCHAR buffer[4096];
	ZeroMemory(buffer, sizeof(buffer));
	try
	{
		va_list argptr;
		va_start(argptr, fmtstr);
		_vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), fmtstr, argptr);
		va_end(argptr);
	}
	catch (...)
	{
		//...
	}

	CString str;
	str.SetString(buffer);
	return str;
}

// Ư�� ���μ��� �ƱԸ�Ʈ ���� ����
// ShellExecute ���� ���� ����, �����ų ���μ����� ����������� ����� �� �־�, ����� ó�� ����.
// �� ���� ��ΰ��� ������ commandLine�� ������� ���, ��ο� ������ �����ϸ� ������ �ȵ� �� ���ɼ��� ����
bool RunProcess_Infinite(CString strProcess, CString commandLine, bool bShowHide, int *pRetVal)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { 0 };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = bShowHide ? SW_SHOW : SW_HIDE;

	CString strCmd = strProcess + " "+ commandLine;
	if (CreateProcess(NULL, strCmd.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == false)
		return false;
	
	// Wait until child process exits.
	while (true)
	{
		Sleep(1);

		if (WaitForSingleObject(pi.hProcess, 1) != WAIT_TIMEOUT)
			break;
	}

	DWORD exitCode;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	if(pRetVal)
		*pRetVal = exitCode;

	return true;
}

void GetCommandLineArg(CStringArray* _out_pStrArr)
{
	LPWSTR* pStr = NULL;
	int iCnt = 0;
	pStr = CommandLineToArgvW(GetCommandLineW(), &iCnt);

	try
	{
		for (int i = 1; i<iCnt; i++)
		{
			_out_pStrArr->Add(CString(pStr[i]));
		}
	}
	catch (...) {}
}
// Files & Directory
CString _ExtractFilePath(CString szFilePath)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char buf[_MAX_PATH];
	CString szVal;


	ASSERT(szFilePath);

	strcpy_s(path_buffer, szFilePath);

	_splitpath_s(path_buffer, drive, dir, fname, ext);

	memset(buf, '\0', sizeof(buf));
	strcat_s(buf, drive);
	strcat_s(buf, dir);

	szVal = CString(buf);

	return szVal;
}

// SHCHO 2018.05.29
// ���α׷� ����
// �� ���������� �ִ� �������� extract.dat ������ �ְ� ���� �ȿ� ������ zip ��ΰ� ����־�� �Ѵ�.
// �׷� �� ���α׷��� ������ ������ 7zip�� �̿��ؼ� Ǯ��, ���̳ʸ��� MapData.dat ������ ��� ����ü�� �°� �Ľ��Ѵ�
// �Ľ��� ������ OcsLayout.dat ���Ͽ� �ؽ�Ʈ�� �ٿ� �ִ´�.
// �׷� ���������� �� ���α׷��� ȣ���� ���μ����� �ؽ�Ʈ ������ OcsLayout.dat ���ϸ� �ε��ϸ�
// ��� �������� �� �� �ִ�
int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // MFC�� �ʱ�ȭ�մϴ�. �ʱ�ȭ���� ���� ��� ������ �μ��մϴ�.
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
            wprintf(L"�ɰ��� ����: MFC�� �ʱ�ȭ���� ���߽��ϴ�.\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: ���� ���α׷��� ������ ���⿡�� �ڵ��մϴ�.
			// �ƱԸ�Ʈ���� zip ��� �˾ƿ���
			CStringArray args;
			GetCommandLineArg(&args);
			//for (int n = 0; n < args.GetCount(); n++)
			//	AfxMessageBox(args.GetAt(n));
			bool DEBUGGING = false;

			if (args.GetCount() < 1 && !DEBUGGING)
				return -31;

			
			CString zipPath;
			
			if(!DEBUGGING)
				zipPath = args.GetAt(0);		// gStringFormat("%s\\Map.zip", gStrWorkDir);
			else
				zipPath = "C:\\.oht_easy_view_temporary\\22\\aa.zip";

			if (FileExists(zipPath) == false)
				return -30;

			
			CString strWorkDir = _ExtractFilePath(zipPath);
			// �ӽ� ���丮 ����
			CreateDirectory(strWorkDir, NULL);
			SetFileAttributes(strWorkDir, FILE_ATTRIBUTE_HIDDEN);

			CString strSubDir = "vehicle_map\\1";	// �� �׷� 1�� �켱

			// Folder Clearing
			CString strLayout_in					= gStringFormat("%sOcsLayout.dat", strWorkDir);
			CString strMapDataPath_in			= gStringFormat("%s%s\\MAPDATA.DAT", strWorkDir, strSubDir);
			CString strMapDataPath_out		= gStringFormat("%sMAPDATA.TXT", strWorkDir);
			CString strCIDPath_in					= gStringFormat("%s%s\\CID.DAT", strWorkDir, strSubDir);
			CString strCIDPath_out				= gStringFormat("%sCID.TXT", strWorkDir);
			CString strCollisionPath_in			= gStringFormat("%s%s\\COLLISION.DAT", strWorkDir, strSubDir);
			CString strCollisionPath_out			= gStringFormat("%sCOLLISION.TXT", strWorkDir);
			CString strStationPath_in				= gStringFormat("%s%s\\STATION.DAT", strWorkDir, strSubDir);
			CString strStationPath_out			= gStringFormat("%sSTATION.TXT", strWorkDir);
			CString strFlowControlPath_in = gStringFormat("%s%s\\FLOWCONTROL.DAT", strWorkDir, strSubDir);
			CString strFlowControlPath_out = gStringFormat("%sFLOWCONTROL.TXT", strWorkDir);
			CString strTeachingPath_in = gStringFormat("%s%s\\TEACHING.DAT", strWorkDir, strSubDir);
			CString strTeachingPath_out = gStringFormat("%sTEACHING.TXT", strWorkDir);
			CString strPathData_in = gStringFormat("%s%s\\PATHDATA.DAT", strWorkDir, strSubDir);
			CString strPathData_out = gStringFormat("%sPATHDATA.TXT", strWorkDir);
			
			CPtrArray pathArr;
			pathArr.Add(&strLayout_in);
			pathArr.Add(&strMapDataPath_in);
			pathArr.Add(&strMapDataPath_out);
			pathArr.Add(&strCIDPath_in);
			pathArr.Add(&strCIDPath_out);
			pathArr.Add(&strCollisionPath_in);
			pathArr.Add(&strCollisionPath_out);
			pathArr.Add(&strCollisionPath_in);
			pathArr.Add(&strStationPath_in);
			pathArr.Add(&strStationPath_out);
			pathArr.Add(&strFlowControlPath_in);
			pathArr.Add(&strFlowControlPath_out);
			pathArr.Add(&strTeachingPath_in);
			pathArr.Add(&strTeachingPath_out);
			pathArr.Add(&strPathData_in);
			pathArr.Add(&strPathData_out);


			for (int i = 0; i < pathArr.GetCount(); i++)
			{
				CString *pStr = (CString*)pathArr.GetAt(i);
				DeleteFile(*pStr);
			}
			
			// 7zip�� �̿��ؼ� OcsLayout�� Mapdata �ΰ� ���ϸ� ���� ��ŷ ������ Ǯ�ڴٴ�
			CString cmd;
			//cmd.Format("e %s -o%s OcsLayout.dat MAPDATA.DAT CID.DAT COLLISION.DAT STATION.DAT FLOWCONTROL.DAT TEACHING.DAT-r -y", zipPath, gStrWorkDir);
			//cmd.Format("e %s -o%s -r -y", zipPath, strWorkDir);	
			cmd.Format("x %s -o%s -y", zipPath, strWorkDir);	//cmd.Format("e %s -o%s OcsLayout.dat MAPDATA.DAT CID.DAT COLLISION.DAT STATION.DAT FLOWCONTROL.DAT TEACHING.DAT-r -y", zipPath, gStrWorkDir);

			if (RunProcess_Infinite("7z.exe", cmd, false, NULL) == false)
				return -40;
			
			// dat ���� ���� ��Ǯ���� �ִ��� Ȯ��
			for (int i = 0; i < pathArr.GetCount(); i++)
			{
				CString *pStr = (CString*)pathArr.GetAt(i);
				if (pStr->Find(".DAT") >= 0)
				{
					if (!FileExists(*pStr))
						return -50 - i;
				}
			}

			// MapData.dat ������ �ε��� �� // txt�� save �Ѵ�
			STD_MAP mapData;
			if (mapData.LoadMapFile_MapData(strMapDataPath_in) == false)				return -70;
			if (mapData.Save_asText(strMapDataPath_out) == false)				return -71;

			// CID.dat ������ �ε��Ѵ� , �� txt save
			STD_CID cidData;
			if (cidData.LoadMapFile_CID(strCIDPath_in) == false)				return -71;
			if (cidData.Save_asTxt(strCIDPath_out) == false)				return -71;

			// collision.dat ������ �ε��Ѵ� , �� txt save
			CollisionManager_STD collision;
			if (collision.LoadMapFile(strCollisionPath_in) == false)				return -72;
			if (collision.Save_asTxt(strCollisionPath_out) == false)				return -72;

			//// staion.dat ������ �ε��Ѵ� , �� txt save
			StationManager_STD station;
			if (station.LoadMapFile(strStationPath_in) == false)				return -73;
			if (station.Save_asTxt(strStationPath_out) == false)				return -73;

			//// flowcontrol.dat ������ �ε��Ѵ� , �� txt save
			STD_FlowControl flowControl;
			if (flowControl.LoadMapFile(strFlowControlPath_in) == false)				return -73;
			if (flowControl.Save_asTxt(strFlowControlPath_out) == false)				return -73;

			//// teaching.dat ������ �ε��Ѵ� , �� txt save
			STD_Teaching teaching;
			if (teaching.LoadMapFile(strTeachingPath_in) == false)				return -74;
			if (teaching.Save_asTxt(strTeachingPath_out) == false)				return -74;

			STD_PathData pathData;
			if (pathData.LoadMapFile(strPathData_in) == false)				return -75;
			if (pathData.Save_asTxt(strPathData_out) == false)				return -75;

        }
    }
    else
    {
        // TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
        wprintf(L"�ɰ��� ����: GetModuleHandle ����\n");
        nRetCode = 1;
    }

    return nRetCode;
}
