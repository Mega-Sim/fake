//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH

#include <stdio.h>
//#include <ScktComp.hpp>
#include <vcl.h>

#include "Def_OHT.h"
#include "../Common/Def_DataType.h"

#define STRINGIFY(x)               #x // ���� �̸��� ���ڿ��� ��ȯ

//---------------------------------------------------------------------------
class OHTUTILITY
{
 public :
   OHTUTILITY();
   ~OHTUTILITY();

   static void CheckDirectory();
//   static void Check_DeleteLogFile(AnsiString, int);   //�̻��
//   static int DeleteFile_SizeZero(AnsiString);         //�̻��
   static int Get_IsLogDriveAlive(AnsiString);
   static int GetNum_LogFile(void);
//   static int File_Delete_All(AnsiString);             //�̻��

	static bool Check_DDrive();
	static bool Check_LogFolder();

	static AnsiString Get_FileAccessTime(AnsiString);
	static int Check_VirusDirectory(void);
	static unsigned long PreventOverflowGetTimeMs(unsigned long precalc);

	static int GetDiskInfo(DISK_INFO* pDiskInfo, char cDrive);
	static void CompareVariables(AnsiString name, double val1, double val2);
};

//---------------------------------------------------------------------------

enum TimeUnit { Second, Minute, Hour, Day, Week };

class PeriodicCounter {
private:
    int nCount;                   // count ����
    TimeUnit timeUnit;            // �ð� ����
    TDateTime checkTime;          // ���� ���� �ð�
	TDateTime durationTime;		  // ���� �ð�
    bool bAdjustFirst;            // ���� �ֱ��� �ð� ���� ����

    TDateTime getDuration(int nMultiplier) const; // ������ ���� �ð� ��ȯ
	TDateTime getNextPeriod(bool bInit = false) const; // ���� ���� ���� ��ȯ

    void reset(bool bInit = false); // reset count, time
    void resetIfNeeded(); // ���� �ð��� ���� �ð��� �ʰ��ߴ��� Ȯ�� �� ����

public:
    PeriodicCounter(TimeUnit unit, int multiplier = 1, bool bAdjustFirst = false); // ������: �ʱ� �ð� ���� + count �ʱ�ȭ

    int increment(); // count ����

    int getCount(); // ���� count ��ȸ

    void resetCountOnly(); // reset count (Error �߻� ���� �ʱ�ȭ �뵵)

    bool updateAndCheckThreshold(int nThreshold, int *nCnt); // count ���� �� �Ѱ�ġ ���� ���� Ȯ��
};
//---------------------------------------------------------------------------

#endif
