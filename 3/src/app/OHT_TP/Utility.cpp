// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Utility.h"
#include <stdio.h>
#include <dos.h>
#include <Filectrl.hpp>
#include "ConnectUnit.h"

// ---------------------------------------------------------------------------
TOHTLOGFILE::TOHTLOGFILE()
{
}

// ---------------------------------------------------------------------------
TOHTLOGFILE::~TOHTLOGFILE()
{
}

// ---------------------------------------------------------------------------
bool TOHTLOGFILE::WriteLogFile(String strTmpFileName, String strTmpLogData)
{
    struct date today;
    struct time currenttime;
    String strTmpFullName, strTPPath;
    FILE *fp;
    String strTmpData = strTmpLogData.TrimRight();

    getdate(&today);
    gettime(&currenttime);

    strTPPath = ExtractFileDir(Application->ExeName);
    // ȭ���̸��� ����
    strTmpFullName.sprintf(L"%s\\Log\\%s%04d%02d%02d%02d.log", strTPPath, strTmpFileName, today.da_year, today.da_mon, today.da_day,
        currenttime.ti_hour);
    // if(fp != NULL) fclose(fp);
    fp = _wfopen(strTmpFullName.w_str(), L"a");

    if(fp != NULL)
    {
        try
        {
            fwprintf(fp, L"%02d:%02d:%02d.%02d %s\n", currenttime.ti_hour, currenttime.ti_min, currenttime.ti_sec, currenttime.ti_hund,
                strTmpData);
            fflush(fp);
        }
        catch(Exception &e)
        {
            fclose(fp);
            fp = NULL;
            return false;
        }
        fclose(fp);
        return true;
    }
    fclose(fp);
    return false;
}

// ---------------------------------------------------------------------------
// 2002.11.27 JYB
// ���糯¥/�ð����Ͽ� �Ϸ��� ���� �ð����� Log File�� Check�Ͽ� Log File����..
// ---------------------------------------------------------------------------
void TOHTLOGFILE::DeleteLogFile()
{
    String strFullLogFile, strMonDay;
    bool bYun;
    struct date today;
    struct time currenttime;
    int nYear, nMon, nDay;
    int nSData;

    getdate(&today);
    gettime(&currenttime);

    strMonDay.sprintf(L"%02d%02d", today.da_mon, today.da_day);

    if(strMonDay == "0101") // 1��1���̸� �����⵵ 12��31�� LogFileã��..
    {
        nYear = today.da_year - 1;
        nMon = 12;
        nDay = 31;
    }
    else if(strMonDay == "0301")
    {
        nYear = today.da_year;
        nMon = 02;
        // ������
        if(!(today.da_year % 4) && today.da_year % 100 || !(today.da_year % 400))
            nDay = 29;
        else
            nDay = 28;
    }
    else if((strMonDay == "0201") || (strMonDay == "0401") || (strMonDay == "0601") || (strMonDay == "0801") || (strMonDay == "0901") ||
        (strMonDay == "1101"))
    {
        nYear = today.da_year;
        nMon = today.da_mon - 1;
        nDay = 31;
    }
    else if((strMonDay == "0501") || (strMonDay == "0701") || (strMonDay == "1001") || (strMonDay == "1201"))
    {
        nYear = today.da_year;
        nMon = today.da_mon - 1;
        nDay = 30;
    }
    else
    {
        nYear = today.da_year;
        nMon = today.da_mon;
        nDay = today.da_day - 1;
    }
    strFullLogFile.sprintf(L"%04d%02d%02d%02d", nYear, nMon, nDay, currenttime.ti_hour);
    nSData = strFullLogFile.ToInt();

    FileExistCheck(".\\Log\\*.log", nSData);
}

// ---------------------------------------------------------------------------
void TOHTLOGFILE::FileExistCheck(String strTmpPath, int nNowData)
{
    TSearchRec sr;
    String strTmpFileName;

    if(FindFirst(strTmpPath, faArchive | faReadOnly | faSysFile | faHidden, sr) == 0)
    {
        // ���丮�� �����ϰ� ���ϸ��� �˻��Ѵ�
        if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
        {
            // �˻��� ������ �����̸�
            if(!((sr.Name == ".") || (sr.Name == "..")))
            {
                strTmpFileName = sr.Name;
                Check_DeleteLogFile(strTmpFileName, nNowData);
            }
        }
        while(FindNext(sr) == 0)
        {
            if(sr.Attr == faArchive || faReadOnly || faSysFile || faHidden)
            {
                if((sr.Name == ".") || (sr.Name == ".."))
                    continue;
                strTmpFileName = sr.Name;
                Check_DeleteLogFile(strTmpFileName, nNowData);
            }
        }
    }
    FindClose(sr);
}

// ---------------------------------------------------------------------------
void TOHTLOGFILE::Check_DeleteLogFile(String strTmpFile, int nNowData)
{
    int nPos, nPreData;
    String strTmpS;

    nPos = strTmpFile.Pos(".log");
    strTmpS = strTmpFile.SubString(nPos - 10, 10);
    nPreData = strTmpS.ToInt();

    if(nPreData < nNowData)
        DeleteFile(".\\Log\\" + strTmpFile);
}

// ---------------------------------------------------------------------------
void TOHTLOGFILE::CheckDirectory()
{
    if(!DirectoryExists(".\\Log"))
    {
        CreateDir(".\\Log");
    }
}

// ---------------------------------------------------------------------------
void Snooze(float numSec)
{
    MSG msg;
    DWORD timeout = (numSec * 1000UL) + GetCurrentTime();
    while(GetCurrentTime() < timeout)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                PostQuitMessage(0);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
// ---------------------------------------------------------------------------

#pragma package(smart_init)
