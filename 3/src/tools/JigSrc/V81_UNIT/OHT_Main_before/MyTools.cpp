//---------------------------------------------------------------------------


#pragma hdrstop
#include <stdio.h>
#include "MyTools.h"
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>


//---------------------------------------------------------------------------

#pragma package(smart_init)

bool gCS = false;    // global ciritcal section
int CStepDelay::ScanTime = 0;

void TRACE(char* szMessage, ...)
{
     char szDebugMsg[4096];
     va_list va;
     va_start(va, szMessage);
     vsprintf(szDebugMsg, szMessage, (va_list)va);
     va_end(va);
     OutputDebugStringA(szDebugMsg);

}



// shcho : String Split
// Exam
/*
   TStringList *Tokens = new TStringList;
   SplitString("0x30,0x50,0x33,0x32", ",", Tokens);
   for(int x = 0; x < Tokens->Count; x++)
       ShowMessage(Tokens->Strings[x]);
   delete Tokens;
*/
void __fastcall SplitString(AnsiString Original, AnsiString Delim, TStrings *List)
{
        int iPos;

        while( true )
        {
            iPos = Original.Pos(Delim);
            if( iPos == 0 )
                break;

            List->Append(Original.SubString(1, iPos-1));
            Original.Delete(1, iPos + Delim.Length());
        }

        if( !Original.IsEmpty() )
            List->Append(Original);
}



// shcho
//  StringToHexStream
// ex) [in: ABCDE]   [seperator: ',']   use0x : ���̻��̿� 0x ������ ����
//     [outout: 65,66,67,68,69]
bool StringToHexStream(char *in, char *pOut, int nOutSize, char seperator, bool bAsHex)
{
     if(in == NULL)         return false;

     char szBuf[4096];

     int nLen = strlen(in);
     if(nLen <= 0 || nLen >= sizeof(szBuf)) //over flow ����
      return false;


     int nSum = 0;
     for(int i=0; i<nLen ; i++)
     {
        char str1[10]={0,},  str2[10]={0,};
		if(bAsHex)
			sprintf(str1, "0x%x", in[i]);//str1.sprintf("0x%x", in[i]);
        else
			sprintf(str1, "%d", in[i]);

        if(i != nLen-1)
            sprintf(str2, "%s%c", str1, seperator);  //str2.sprintf("%s%c", str1.c_str(), seperator);
        strcpy(szBuf+nSum, str2);

        nSum += strlen(str2);
        if(nSum >= 4096 || nSum >= nOutSize)
            break;  //overflow ����

        //*pOut += str2;
     }

     strcpy(pOut, szBuf);
     return true;
}

// shcho
bool __fastcall HexToInt(char *szHex, int *pRet)
{
/*
#include <sstream>                  <<--- these includes needed for string stream
#include <iostream>
*/
    if(szHex == NULL)
        return false;

    if(strlen(szHex) <= 0)
        return false;


    unsigned int x;
    std::stringstream ss;
    ss << std::hex << szHex;
    ss >> x;

    if(pRet)
      *pRet = x;

    return true;
}

int GetThisCount_inString(char *str)
{
    if(str == NULL) return 0;

    int nCnt = 0;
    for(int i=0; i< strlen(str) ; i++)
    {
        if(str[i] == ',')
            nCnt++;
    }

    return nCnt;
}

int GetThisCount_inString(AnsiString *str)
{
   return GetThisCount_inString(str->c_str());

}

// SHCHO 2019 : FindStringPos. Pure C++
// pSrc = "She is beautiful", pFind = "beau"�� �ָ� 7�� ����. ������ -1
// ���ڿ� ���� ���̴� 1024
int FindStringPos(char *pSrc, char *pFind)
{
	if (pSrc == NULL || pFind == NULL)
		return -1;

	int nLen = strlen(pSrc);
	int nFindLen = strlen(pFind);

	//
	if (nLen > 1024 || nFindLen > 1024 || nFindLen > nLen || nLen == 0 || nFindLen == 0)
		return -1;

	int nMatchCnt = 0;
	int nStartPos = -1;
	for (int i=0; i < nLen ; i++)
	{
		char cO = pSrc[i];
		char cF = pFind[nMatchCnt];
		if (cO == cF)
		{
			if (nMatchCnt == 0)
				nStartPos = i;	// ������ġ

			if (nMatchCnt == nFindLen-1)	// ��� ����
				return nStartPos;

			nMatchCnt++;
		}
		else
		{
			nMatchCnt = 0;
			nStartPos = -1;
		}
	}
	return -1;
}

// SHCHO 2019 GetThisValue  - Pure C++
//  ���ڿ����� Ư�� ���� �о
// ex)
// Px.1=-5000.156���� =�� �ָ� 5000�� �о
bool GetThisValue(char *pSrc, char *chFind, int *pnVal, double *pdVal)
{
	int nLen = strlen(pSrc);
	int nPos = -1;

	nPos = FindStringPos(pSrc, chFind);

	if (nPos < 0)
		return false;

	nPos += strlen(chFind);

	char szVal[64];
	memset(szVal, 0, sizeof(szVal));
	int nAddPos = 0;
	bool bDot = false;
	// �ϳ��� �˻��ϸ鼭 ���� or ��ȣ�� �ƴ� char �� ���ö� ���� reading �Ѵ�
	for (int i = nPos; i<nLen; i++)
	{
		if (sizeof(szVal) - 1 < nAddPos)     // over flow ����
			return false;

		char c = pSrc[i];
		if (c >= '0' && c <= '9')
		{
			szVal[nAddPos++] = c;
		}
		else if (nAddPos == 0 && c == '-')	// ù���ڴ� ���� ��ȣ�� �ü��� �ִ�
		{
			szVal[nAddPos++] = c;
		}
		else if (bDot == false && c == '.')  // �Ҽ����� �ѹ��� �Ⱦ������� �ѹ��� ���ϼ��ִ�
		{
			bDot = true;
			szVal[nAddPos++] = c;
		}
		else
			break;
	}


	if (nAddPos == 0)	// ���� ���� ����
		return false;

	szVal[nAddPos] = '\0';		// null char

	if (pnVal)
	{
		if (szVal[0] == '.')
			return false;
		*pnVal = atoi(szVal);
	}

	if (pdVal)
		*pdVal = atof(szVal);


	return TRUE;
}

// 2017.07.19 SHCHO  �������� �ƴ��� üũ���ִ� �Լ�
// ex) sz�� 1234 �ϰ�� true
// ex) sz�� 123a4 �� ��� false
bool IsDigit(char *sz, int nSize)
{
	if(nSize == 0 || sz == NULL)
		return false;

	for(int i=0; i<nSize ; i++)
	{
		if(sz[i] < '0' || sz[i] > '9')
			return false;
	}

	return true;
}

// 2017.07.19 SHCHO ���ڿ����� char tok �� ������ ����� ����
// ex 255.255.255.255 ���� .�� ������ 3
int CountOfTokens(char *sz, char tok, int nSize)
{
	if(sz == NULL)
		return 0;

	int count =0;

	for(int i=0;i<nSize;i++)
	{
		if(sz[i] == tok)
			count++;
	}

	return count;
}


// shcho
// char ���ڿ��� token���� �ɰ��� std::string ��Ʈ�� �迭�� ����(����)
// include  <vector>, <string> �ʿ�
std::vector<std::string> StrSplit(char *str, char token)
{
	std::vector<std::string> result;

	do
	{
		const char *begin = str;

		while(*str != token && *str)
			str++;

		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}

// SHCHO
// StrReplace
// pStr���� from�� ã�Ƽ�, to�� �ٲ۴�
bool StrReplace(std::string* pStr, const std::string& from, const std::string& to)
{
	size_t start_pos = pStr->find(from);
	if(start_pos == std::string::npos)
		return false;

	while(true)
	{
		pStr->replace(start_pos, from.length(), to);
		start_pos = pStr->find(from);
		if(start_pos == std::string::npos)
			break;

	}

    return true;
}

// SHCHO
// ���ڿ����� ���� ����
// ex) 1,2,3,4,5  ���ڿ����� �� ���� �����ؼ� vector ������ ����
std::vector<int> ExtractNumber_inString(std::string strInput, char token)
{
	std::vector<int> arrInt;

	std::vector<std::string> arrStr = StrSplit((char*)strInput.c_str(), token);
	int nArrSize = arrStr.size();

	for(int i=0 ; i<nArrSize ; i++)
	{
		char *pStr = (char*)arrStr[i].c_str();
		int nLen = arrStr[i].length();

		if(IsDigit(pStr, nLen) == false)
			continue;

		int nVal = atoi(pStr);
		arrInt.push_back(nVal);
	}

	return arrInt;
}

// 2018.07.19 SHCHO : IsIpAddress
// ���ڿ��� ��ȿ�� IPv4 �ּ����� �˻�. 0.0.0.0 ~ 255.255.255.255 ���� ���� �ִ� �������� ������
bool IsIpAddress(char *szAddr, int nSize)
{
	try
	{
		if(szAddr == NULL)
			return false;

		if(nSize < 7 || nSize > 15)	//0.0.0.0 ~ 255.255.255.255
			return false;

		// ��Ʈ�� �迭(����) �� ���´�
		std::vector<std::string> arr = StrSplit(szAddr, '.');

		int nDotCnt = CountOfTokens(szAddr, '.', nSize);

		int nCnt = arr.size();

		if(nCnt != 4)  // ������ 4��������
			return false;

		if(strcmp("0.0.0.0", szAddr) == 0)	//0.0.0.0 �� ��ȿ���� ����
			return false;

		for(int i=0; i<nCnt ; i++)
		{
			char *pStr = (char*)arr[i].c_str();
			int nLen =  arr[i].length();

			if(IsDigit(pStr, nLen) == false)
				return false;

			int nVal = atoi(pStr);
			if(nVal < 0 || nVal > 255)
				return false;
		}
	}
	catch(...)
	{
		return false;
	}

	return true;
}

// Rapid Log . ȣ��Ǵ� ��� �����. Queuing ���� ����
// pFilePath_withoutExt : Ȯ���ڸ� ������ ���ϰ��. Ȯ���� .log�� �˾Ƽ� ����.
// bTime : �α׿� �ð��� ������� ����.
// bTimeOnFileName : �α����Ͽ� ��¥�ͽð����� ������ ����.
// 		true �ϰ�� ex) TestLog �� �൵ TestLog2018072316.log ���Ͽ� �����

void RapidLog(char *pFilePath_withoutExt, char *pStr, bool bTime, bool bTimeOnFileName)
{
    FILE *fp = NULL;
	try
	{
		SYSTEMTIME CurTime;
		GetLocalTime(&CurTime);

		char szFilePath[512];
		ZeroMemory(szFilePath, sizeof(szFilePath));

		if(bTimeOnFileName)   	sprintf(szFilePath, "%s%d%02d%02d%02d.log", pFilePath_withoutExt, CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour);
		else             		sprintf(szFilePath, "%s.log", pFilePath_withoutExt);

		fp = fopen(szFilePath, "a");    // write+append

		if(fp == NULL)
			return;

		if(bTime)
		{
			fprintf(fp, "[%02d:%02d:%02d.%03d] ",
						CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds);
		}
		fwrite(pStr, 1, strlen(pStr), fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);

	}
	catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}
}

void OACISLog(char *pFilePath_withoutExt, char *pStr, bool bTime)
{
    FILE *fp = NULL;
	try
	{
		SYSTEMTIME CurTime;
		GetLocalTime(&CurTime);

		char szFilePath[512];
		ZeroMemory(szFilePath, sizeof(szFilePath));

		sprintf(szFilePath, "%s", pFilePath_withoutExt);

		fp = fopen(szFilePath, "a");    // write+append

		if(fp == NULL)
			return;

		if(bTime)
		{
			fprintf(fp, "[%04d%02d%02d %02d:%02d:%02d]",
						CurTime.wYear,CurTime.wMonth,CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
		}
		fwrite(pStr, 1, strlen(pStr), fp);
		fwrite("\n", 1, 1, fp);
		fclose(fp);

	}
	catch(...)
	{
		if(fp != NULL)
			fclose(fp);
	}
}

char* OACISType(int TypeNum)
{
  static char strType[1024] = {0,};
  memset(strType,0x00, sizeof(strType));
  switch(TypeNum)
  {
	  case 0:
	  strcpy(strType,"Home");
//	  sprintf(strType,"Home");

	  break;
	  case 1:
	  strcpy(strType,"IO");
	  break;
	  case 2:
	  strcpy(strType,"Voltage");

	  case 3:
	  strcpy(strType,"SetOrigin");
	  break;
  }
  return strType;
}

char* OACISResult(int Result)
{
  static char strType1[1024] = {0,};

  memset(strType1,0x00, sizeof(strType1));

  switch(Result)
  {
	  case 0:
	  strcpy(strType1,"NONE");
	  break;
	  case 1:
	  strcpy(strType1,"OK");

	  break;
	  case 2:
	  strcpy(strType1,"NG");
	  break;
  }
  return strType1;
}

void SET_BIT(void* pMem, UINT nBit, bool bSet)
{
	BYTE* pByte = (BYTE*)pMem + (UINT)(nBit/8);
	nBit = nBit % 8;

	if (bSet)
	{
		*pByte |= (1 << nBit);
	}
	else
	{
		*pByte &= ~(1 << nBit);
	}
}

bool GET_BIT(void* pMem, UINT nBit)
{
	BYTE* pByte = (BYTE*)pMem + (UINT)(nBit/8);
	nBit = nBit % 8;

	return (*pByte >> nBit) & 1;
}

// SHCHO 2018.10.17  GetEternetInfo(...)
// �� �̴���ī�庰�� ip, subnet, gateway�� �˾Ƴ���.
// ipconfig�� �Ľ��Ͽ� �˾Ƴ� (�ѱ�, ���� ������ ��� �׽�Ʈ �Ϸ�)
// ��ī�尡 �������� ��� windows APIs �� ���ؼ� �̴��� ������ ��°� ���� ������
//
// argement : arrEternetInfo : ��ī�� ��ü �迭 �ּҸ� ����. ���� ��ü
//          nArrCnt       : �Է� �迭 ����
// return : ã�Ƴ� �̴��� ī�� ����
int GetEternetInfo(sEthernetInfo *arrEternetInfo, int nArrCnt)
{
	const char tmpFilePath[] = "C:\\.network.tmp";
	DeleteFileA(tmpFilePath);   // ������ �ӽ÷� �����Ȱ� Ȥ�� �ִٸ� ����

	char szCmd[100];
	sprintf(szCmd, "ipconfig >> %s", tmpFilePath);   // ipconfig ���� �ӽ� ���� ����


   	system(szCmd); // ���� Ŀ�ǵ� IPCONFIG ����
	// �Ʒ� GetLocaleInfo ������� ���õ� ���� �˾Ƴ� �� �ִµ�
	// ������ ��ġ ������ ���������� �˼� ��� �����
	//TCHAR loc[80];
	//GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, loc, 80*sizeof(TCHAR));
	//GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, loc, 80*sizeof(TCHAR));

	//char szLog[80];
	//wcstombs(szLog, loc, sizeof(szLog));
	bool bKorean = false;	//strcmp(szLog, "Korean") == 0;

	// �� sectino�� Key �̸�����. ex) IPV4�ּ�, ����ݸ���ũ, �⺻����Ʈ����
	char szKeyName_Ethernet[50];
	char szKeyName_IP[50];
	char szKeyName_Subnet[50];
	char szKeyName_Gateway[50];

     // ������ ��� �˾Ƴ���. IPCONFIG ���� ���� ��� �Ǵ�
	{
		std::string tmp;
		std::ifstream tmpFile(tmpFilePath);
		while (std::getline(tmpFile, tmp))
		{
			int nPos = tmp.find("Windows IP ����");	// ������  Windows IP Configuration �̶�� ����
			if(nPos >=0)
			{
				bKorean = true;
				break;
			}
		}
        tmpFile.close();
		sprintf(szKeyName_Ethernet, bKorean ? "�̴��� �����" : "Ethernet adapter");
		sprintf(szKeyName_IP, bKorean ? "IPv4 �ּ�" : "IP Address");
		sprintf(szKeyName_Subnet, bKorean ? "����� ����ũ" : "Subnet Mask");
		sprintf(szKeyName_Gateway, bKorean ? "�⺻ ����Ʈ����" : "Default Gateway");
	}

	std::string line;
	std::ifstream infile(tmpFilePath);

	int nCurCnt = -1;
	bool bKnowLanguage = false;
	bool bIsEnglish = false;
	bool bIsKorean = false;
	// parsing.. line by line
	while (std::getline(infile, line))
	{
		if(line.length() < 3)
			continue;

		int nPos = line.find(szKeyName_Ethernet);
		if(nPos >=0)
		{
			nCurCnt++;

			if(nCurCnt >= nArrCnt) // �Է� �迭 ���� �ʰ�
				return nCurCnt+1;

			// Ethernet adapter ���� ���� ����:
			// ���� ���ڿ����� ��ī�� �̸� "���� ���� ����" �� �����Ѵ�
			line.replace(0, nPos + strlen(szKeyName_Ethernet) + 1, "");
			if(line.find(":") >= 0)
				line.replace(line.find(":"), 1, "");

			strcpy(arrEternetInfo[nCurCnt].Name, line.c_str());
			continue;
        }
		if(nCurCnt < 0)
			continue;

		nPos = line.find(szKeyName_IP);
		if(nPos >= 0)
		{
			//���� ���� : IP Address. . . . . . . . . . . . : 100.100.100.100
			//�ѱ� ���� : IPv4 �ּ� . . . . . . . . . : 23.42.52.123
			//���� ���ڿ����� ip �ּҸ� �Ľ��Ѵ�

			int nColonPos = line.find(":", strlen(szKeyName_IP));
			if(nColonPos < strlen(szKeyName_IP))
				continue;	// �Ľ� �Ұ��� ip �ּ� ���

			line.replace(0, nColonPos, "");  // (:)�ݷ� ������ ���� ����
			while((int)(line.find(":")) >= 0)	// :(�ݷ�) ����
				line.replace(line.find(":"), 1, "");
			while((int)(line.find(" ")) >= 0)
				line.replace(line.find(" "), 1, ""); // ���� ����

			strcpy(arrEternetInfo[nCurCnt].IP, line.c_str());
			continue;
        }

		nPos = line.find(szKeyName_Subnet);
		if(nPos >= 0)
		{
			//���� ���� : Subnet Mask . . . . . . . . . . . : 255.255.0.0
			//�ѱ� ���� : ����� ����ũ . . . . . . . : 255.255.255.0
			//���� ���ڿ����� �ּҸ� �Ľ��Ѵ�

			int nColonPos = line.find(":", strlen(szKeyName_Subnet));
			if(nColonPos < strlen(szKeyName_Subnet))
				continue;	// �Ľ� �Ұ��� ip �ּ� ���

			line.replace(0, nColonPos, "");  // (:)�ݷ� ������ ���� ����
			while((int)(line.find(":")) >= 0)	// :(�ݷ�) ����
				line.replace(line.find(":"), 1, "");
			while((int)(line.find(" ")) >= 0)
				line.replace(line.find(" "), 1, ""); // ���� ����

			strcpy(arrEternetInfo[nCurCnt].SubnetMask, line.c_str());
			continue;
		}

		nPos = line.find(szKeyName_Gateway);
		if(nPos >= 0)
		{
			//���� ���� : Default Gateway . . . . . . . . . : 192.168.0.1
			//�ѱ� ���� : �⺻ ����Ʈ���� . . . . . . : 23.42.52.1
			//���� ���ڿ����� �ּҸ� �Ľ��Ѵ�

			int nColonPos = line.find(":", strlen(szKeyName_Gateway));
			if(nColonPos < strlen(szKeyName_Gateway))
				continue;	// �Ľ� �Ұ��� ip �ּ� ���

			line.replace(0, nColonPos, "");  // (:)�ݷ� ������ ���� ����
			while((int)(line.find(":")) >= 0)	// :(�ݷ�) ����
				line.replace(line.find(":"), 1, "");
			while((int)(line.find(" ")) >= 0)
				line.replace(line.find(" "), 1, ""); // ���� ����

			strcpy(arrEternetInfo[nCurCnt].Gateway, line.c_str());
			continue;
		}

	}
	infile.close();
	DeleteFileA(tmpFilePath);    // �ӽ÷� ������ ���� ����

	return nCurCnt + 1;
}

TimeChk::TimeChk()
{
	memset(nArrStart, 0, sizeof(nArrStart));
	memset(nArrEnd, 0, sizeof(nArrEnd));
};

void TimeChk::Start(int nEventID)
{
	if(nEventID >= MaxEventCnt && nEventID < 0)
		return;

	nArrStart[nEventID] = timeGetTime();
}

void TimeChk::End(int nEventID)
{
	if(nEventID >= MaxEventCnt && nEventID < 0)
		return;
	if(nArrStart[nEventID] == 0)
		return;

	nArrEnd[nEventID] = timeGetTime();
}

DWORD TimeChk::Get(int nEventID)	// msec.  watch: [return -1 == error]
{
	if(nEventID >= MaxEventCnt && nEventID < 0)
		return -1;
	if(nArrStart[nEventID] == 0)
		return -1;
	if(nArrEnd[nEventID] == 0)
		return -1;
	if(nArrEnd[nEventID] < nArrStart[nEventID])
		return -1;

	return nArrEnd[nEventID] - nArrStart[nEventID];
}
void TimeChk::Init(int nEventID)	// nEventID == -1  ��ü Ÿ�̸� �ʱ�ȭ, else �ش� Ÿ�̸� �ʱ�ȭ
{
	//ASSERT(nEventID < MaxEventCnt);

	if(nEventID < 0)
	{
		memset(nArrStart, 0, sizeof(nArrStart));
		memset(nArrEnd, 0, sizeof(nArrStart));
		return;
	}
	nArrStart[nEventID] = nArrEnd[nEventID] = 0;
}


DTimer::DTimer()
{
	Init();
}

bool DTimer::Enable(bool bEnable, DWORD dwTime_ms)
{
	// Disable�� ��� Ÿ�̸� �ʱ�ȭ
	if(bEnable == false)
	{	Init();
		return false;
	}

	// Enable�� ó�� �� ���� ���۽ð� ���
	if(bAtFirstEnable)
	{	bAtFirstEnable = false;
		dwStartTick = timeGetTime();
		return false;
	}

	//
	bool bOnTime = timeGetTime() - dwStartTick > dwTime_ms;
	return bOnTime;
}

void DTimer::Init()
{
	dwStartTick = 0;
	bAtFirstEnable = true;
}


DStep::DStep(void)
{
	Step = ErrStep = TimeOut_ms = RetryCnt = Result = 0;
	bAllDone = false;
}



SWTimer::SWTimer()
{
	  m_nStartT = 0;
	  m_ChekEnable = false;
}

	// �������̽�
void SWTimer::Start()
{
	  m_nStartT = timeGetTime();
 //	  m_ChekEnable = true;
}

DWORD SWTimer::GetElipseTime(DWORD nCurT)
{
 //		if(m_ChekEnable == false) return 0;

	  if(nCurT >= m_nStartT)
	  {
		return (nCurT -  m_nStartT);
	  }
	  else
	  { // overflow

		return ((DWORD_MAX - m_nStartT) + nCurT);
	  }
}

