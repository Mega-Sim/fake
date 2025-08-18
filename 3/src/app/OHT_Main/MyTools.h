//---------------------------------------------------------------------------

#ifndef MyToolsH
#define MyToolsH


#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>


#include <vcl.h>
#include <sstream>
#include <iostream>
#include <string>

#include <vector>
#include <string>



// ***************************************
// MyTools
//
// All Codes Written by SHCHO
//
// to Support on C++ Builder
//
// From 2018.04 ~
//
//
//
//

#define RLOG_UBG_CHK		"D:\\log\\UBGPatternComp"
#define MAIN_EXIT_LOG        "D:\\log\\OHTTerminate"
#define IPC_STAT  			"D:\\log\\IPCVoltTemp"
#define UBG_ABNORMAL_CHECK_LOG        "D:\\log\\UBGAbnormalCheck"
#define OBS_AREA_LOG                  "D:\\log\\OBSAreaCheck"


#define OACIS_LOG_STEER           	  "C:\\User\\OHT\\DataFiles\\OACIS\\Steering.dat"
#define OACIS_LOG_VHLDETECT           "C:\\User\\OHT\\DataFiles\\OACIS\\VHLDetect.dat"
#define OACIS_LOG_OBSDETECT           "C:\\User\\OHT\\DataFiles\\OACIS\\OBSDetect.dat"
#define OACIS_LOG_SHIFTHOME           "C:\\User\\OHT\\DataFiles\\OACIS\\ShiftHome.dat"
#define OACIS_LOG_HOISTHOME           "C:\\User\\OHT\\DataFiles\\OACIS\\HoistHome.dat"
#define OACIS_LOG_SMPSVOLTAGE         "C:\\User\\OHT\\DataFiles\\OACIS\\SMPSVoltage.dat"
#define OACIS_LOG_ALL					"C:\\User\\OHT\\DataFiles\\OACIS\\UnitCheck.dat"


// Rapid Log . ȣ��Ǵ� ��� �����. Queuing ���� ����
// pFilePath_withoutExt : Ȯ���ڸ� ������ ���ϰ��. Ȯ���� .log�� �˾Ƽ� ����.
// bTime : �α׿� �ð��� ������� ����.
// bTimeOnFileName : �α����Ͽ� ��¥�ͽð����� ������ ����.
// 		true �ϰ�� ex) TestLog �� �൵ TestLog2018072316.log ���Ͽ� �����

// �ش� �Լ� file write/close �ϴ� �κп��� Hang �߻��Ͽ� ���� �߻� (AMOHTV80F-1293)
// Auto ���� ���� ��Ȳ���� �ش� �Լ� ������� �ʵ��� �����Ͽ���

// void RapidLog(char *pFilePath_withoutExt, char *szBuf, bool bTime = true, bool bTimeOnFileName = true);

// OACISLog �Լ��� RapidLog �Լ��� ������ �����̳�, Auto ���� �߿� ������� �����Ƿ�, �������� ����.

void OACISLog(char *pFilePath_withoutExt, char *szBuf, bool bTime = true);
char* OACISType(int TypeNum);
char* OACISResult(int Result);

//void TRACE(char* szMessage, ...);          //�̻��

//bool GET_BIT(void* pMem, UINT nBit);                 //�̻��
//void SET_BIT(void* pMem, UINT nBit, bool bSet);      //�̻��


//void __fastcall SplitString(AnsiString Original, AnsiString Delim, TStrings *List);       //�̻��
// shcho
//  StringToHexStream
// ex) [in: ABCDE]   [seperator: ',']   use0x : ���̻��̿� 0x ������ ����
//     [outout: 65,66,67,68,69]
//bool StringToHexStream(char *in, AnsiString *pOut, char seperator, bool bUse0x);
bool StringToHexStream(char *in, char *pOut, int nOutSize, char seperator, bool bUse0x);
// shcho
//bool __fastcall HexToInt(char *szHex, int *pRet);       //�̻��


int GetThisCount_inString(char *str);
int GetThisCount_inString(AnsiString *str);


// SHCHO 2019 : FindStringPos. Pure C++ (�����Ϸ�)
// pSrc = "She is beautiful", pFind = "beau"�� �ָ� 7�� ����. ������ -1
// ���ڿ� ���� ���̴� 1024
int FindStringPos(char *pSrc, char *pFind);

// SHCHO 2019 GetThisValue  - Pure C++ (�����Ϸ�)
//  ���ڿ����� Ư�� ���� �о
// ex)
// Px.1=5000.999���� Px1.=�� �ָ� 5000.999�� �о�� int�� Ȥ�� double �Ѵ� ��ȯ��Ŵ
bool GetThisValue(char *pSrc, char *chFind, int *pnVal = NULL, double *pdVal = NULL);


bool IsDigit(char *sz, int nSize);
int CountOfTokens(char *sz, int nSize);
bool IsIpAddress(char *szAddr, int nSize);


// SHCHO
// StrReplace
// pStr���� from�� ã�Ƽ�, to�� �ٲ۴�
bool StrReplace(std::string* pStr, const std::string& from, const std::string& to);

// shcho
// char ���ڿ��� token���� �ɰ��� std::string ��Ʈ�� �迭�� ����(����)
// include  <vector>, <string> �ʿ�
std::vector<std::string> StrSplit(char *str, char token);

// SHCHO
// ���ڿ����� ���� ����
// ex) 1,2,3,4,5  ���ڿ����� �� ���� �����ؼ� vector ������ ����
std::vector<int> ExtractNumber_inString(std::string strInput, char token);


////////////////////////////////////////////////////////////////////////////////
// class CStepDelay
// �ܼ� Delay
////////////////////////////////////////////////////////////////////////////////
// ��� ��
// CStepDelay Delay1;
// .....
// case 120 : if (NextStepCOnditionOK) {Delay1.Clear();step++;}
//		break;
// .....
// case 121 : if (Delay1.Delay(100)) step++;
//		break;
//

class CStepDelay
{

protected :

	bool bScan;// 0 : TickCount���,  1 : Scan ����
	DWORD PreTickTime;
	inline DWORD GetCurTickTime(){return GetTickCount();}
	bool    bPaused;

public :
	bool Out;
	DWORD SetTime;// [ms] ���� �ð�
	DWORD CurTime;// [ms] ���� �ð� ( 0 -> �����ð� )

	static int ScanTime;// [ms]
public :

	//Functions.
	CStepDelay(bool _bScan=true)
	{
		bScan = _bScan;
		bPaused = false;
		SetTime = 0;
		Clear();
	}

	void Clear(void)
	{
		CurTime = 0;
		PreTickTime = GetCurTickTime();
		Out = false;
	}

	bool Delay(int _SetTime)
	{
		SetTime = _SetTime;
		DWORD CurTick = GetCurTickTime();

		if (bScan && !bPaused)CurTime+=ScanTime;
		else if(!bPaused)CurTime += CurTick - PreTickTime;

		if (CurTime>=SetTime)
		{
			CurTime = 0;
			Out = true;
		}
		else
		{
			Out = false;
		}
		PreTickTime = CurTick;
		return Out;
	}

	void Pause()
	{
		bPaused = true;
	}

	void Resume()
	{
		bPaused = false;
	}
};

////////////////////////////////////////////////////////////////////////////////
// class CStepOnDelayTimer
// �ܼ� Delay + Input���� �˻�
////////////////////////////////////////////////////////////////////////////////
class CStepTimer
{

public :
	bool Out;
	DWORD SetTime;// [ms] ���� �ð�
	DWORD CurTime;// [ms] ���� �ð� ( 0 -> �����ð� )

protected :
	bool bScan;// 0 : TickCount���,  1 : Scan ����
	DWORD PreTickTime;
	inline DWORD GetCurTickTime(){return GetTickCount();}
	DWORD   PauseTickTime;
	bool    bPaused;

public :

	//Functions.
	CStepTimer(bool _bScan=false)
	{
		bScan = _bScan;
		bPaused = false;
		SetTime = 0;
		PauseTickTime = 0;
		Clear();
	}

	void SetScan(bool _bScan)
	{
		bScan = _bScan;
	}
	void Clear(void)
	{
		OnDelay(false);
	}

	bool OnDelay(bool SeqInput, DWORD _SetTime)
	{
		SetTime = _SetTime;
		return OnDelay(SeqInput);
	}

	bool OnDelay(bool SeqInput)
	{

		DWORD CurTick = GetCurTickTime();
		if (SeqInput && !bPaused)
		{
			if (bScan)CurTime+=CStepDelay::ScanTime;
			else  CurTime += CurTick - PreTickTime;
			if (CurTime>=SetTime)
			{
				CurTime = SetTime;
				Out = true;
			}
			else
			{
				Out = false;
			}
		}
		else
		{
			CurTime = 0;
			Out = false;
		}
		PreTickTime = CurTick;
		return Out;
	}


	void Pause()
	{
		bPaused = true;
		//if (bPaused == false)
		//{
		//PauseTickTime = GetCurTickTime();
		//bPaused = true;
		//}
	}

	void Resume()
	{
		bPaused = false;
		//if (bPaused)
		//{
		//SetTime += GetCurTickTime() - PauseTickTime;
		//bPaused = false;
		//}
	}
};

class CStepClass
{

public:

	CStepClass(){ Step =0;ErrorStep = 0;StepComment = ""; bAllDone = false; nResult = nRetryCnt = 0;}
	int Step;
	AnsiString StepComment;
	int ErrorStep;
	CStepTimer TimeOver;
	CStepTimer StepDelay;

	int nRetryCnt;
    bool bAllDone;
    int nResult;
	
	void SetErrStep()	{ErrorStep = Step;}
};




// GetEternetInfo(..) �Լ����� ���̴� ��ü
typedef struct sEthernetInfo
{
	char Name[50];
	char IP[50];
	char SubnetMask[50];
	char Gateway[50];

	sEthernetInfo()	{	ZeroMemory(this, sizeof(*this));	}
} sEthernetInfo;

// SHCHO 2018.10.17  GetEternetInfo(...)
// �� �̴���ī�庰�� ip, subnet, gateway�� �˾Ƴ���.
// ipconfig�� �Ľ��Ͽ� �˾Ƴ� (�ѱ�, ���� ������ ��� �׽�Ʈ �Ϸ�)
// ��ī�尡 �������� ��� windows APIs �� ���ؼ� �̴��� ������ ��°� ���� ������
//
// argement : arrEternetInfo : ��ī�� ��ü �迭 �ּҸ� ����. ���� ��ü
//          nArrCnt       : �Է� �迭 ����
// return : ã�Ƴ� �̴��� ī�� ����
int GetEternetInfo(sEthernetInfo *arrEternetInfo, int nArrCnt);




//2018.10.15 SHCHO : TP�� � ���� �Ǿ����� �������ִ� Ŭ����
#define MAX_TP_CONNECTION	10
#define TP_DISCON_TIME_LIMIT_MS		1000
class MyTPAddrMan
{
	//
	AnsiString 	ArrIPAddr[MAX_TP_CONNECTION];
	DWORD 		LastAddTick[MAX_TP_CONNECTION];
	AnsiString	LastReqAddr;
	int m_nCount;

public:


	MyTPAddrMan(){m_nCount = 0;memset(LastAddTick, 0x00, sizeof(LastAddTick));}

	bool Add(AnsiString ip)
	{
		Refresh();
		if(ip.Length() < 5)
			return false;

		LastReqAddr = ip;

		DWORD curTick = GetTickCount();

		for(int i=0; i<m_nCount ; i++)
		{
			if(ArrIPAddr[i] == ip)
			{
				LastAddTick[i] = curTick;  //tick ����
				return true;
            }
		}
		if(m_nCount == MAX_TP_CONNECTION)
			return false;

		// ������� �Դٴ� ���� �ߺ��Ǵ� ip ����. ���� ���ӵ� tp�̴�.
		ArrIPAddr[m_nCount] = ip;
		LastAddTick[m_nCount] = curTick;
		m_nCount++;

		return true;

	}

	int GetCount()
   {
		Refresh();
		return m_nCount;

   }

   bool bIsFirstTP()
   {
		if(m_nCount <= 0)
			return false;

		return LastReqAddr == ArrIPAddr[0];
   }

private:

	void Refresh()
	{
		if(m_nCount == 0)
			return;

		DWORD curTick = GetTickCount();

		for(int i=m_nCount-1 ; i>=0 ; i--)
		{
			// �ð� �ʰ��� IP ����
			if(curTick - LastAddTick[i] > TP_DISCON_TIME_LIMIT_MS)
			{
				ArrIPAddr[i] = "";
				LastAddTick[i] = 0;
				// �ϳ� ���������ϱ� �迭 ������ ������
				for(int k=i; k < m_nCount-1 ; k++)
				{
					ArrIPAddr[k] = ArrIPAddr[k+1];
					LastAddTick[k] = LastAddTick[k+1];
				}
				m_nCount--;
			}
		}

		//for debugging
		//for(int i=0; i<m_nCount; i++)
		//	TRACE("TP[%d] : %s", i+1, ArrIPAddr[i]);

	}
};

// Created by SHCHO 2018.10.24 : GetTickCount()�� ����Ͽ� Ư���κ� ����ð��� �����ϰ��� �Ҷ�
// �ҽ��ڵ尡 �������� ���µ�, �̸� �������� ���ϰ� ���� ����ϰ� �ϱ� ����

// ��� ��

/*
	TimeChk::Start(0);	// id = 0
	QuickSort(...);
	TimeChk::End(0);

	TimeChk::Start(1);	// id = 1
	BubleSort(...);
	TimeChk::End(1);

	printf("Quick=%d, Bubble=%d", TimeChk::Get(0), TimeChk::Get(1));
*/

// ������Ͽ� ����
class TimeChk
{
public:

	const static int MaxEventCnt = 30;	// �ִ� �̺�Ʈ

	// �������̽�
	void Start(int nEventID);
	void End(int nEventID);
	DWORD Get(int nEventID);	// msec
	void Init(int nEventID = -1);		// nEventID == -1  ��ü Ÿ�̸� �ʱ�ȭ, else �ش� Ÿ�̸� �ʱ�ȭ

	TimeChk();
private:

	DWORD nArrStart[MaxEventCnt];
	DWORD nArrEnd[MaxEventCnt];


};

class SWTimer
{
public:
	SWTimer();

	void Start();

	DWORD GetElipseTime(DWORD nCurT);

private:
	DWORD m_nStartT;
	bool m_ChekEnable;
};



class DTimer
{
	private:
		DWORD dwStartTick;
		bool bAtFirstEnable;

	public:

		DTimer();

		bool Enable(bool bEnable, DWORD dwTime_ms = 0);

		void Init();

};

class DStep
{
public:
	DStep(void);

	int ErrStep;
	int RetryCnt;
	int Result;
	bool bAllDone;
	int TimeOut_ms;
	DTimer Timer;
	DTimer TimeOver;

	int Step;
};

//---------------------------------------------------------------------------
#endif


