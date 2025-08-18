#pragma once
#include <iostream>

// Author : SHCHO 2019.03
// NumericSystem Class  
// 10������ N������ �ٲ㼭 Byte ���� ���ϼ� �ִ�.
// Pure C++ Code
// ��� C++ �÷������� ��� ����
//
/* USAGE # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

	�����ô� ������, �������� 90�������� ��ȯ
	90���� Table�� Ű���忡�� �Է��Ҽ� �ִ� ���ڷθ� ��. ��� -, %, Z, / ��� ����� ����
	�ƽ�Ű�ڵ尪���� ���� �ʴ� ������ ������ ����, ���� ���ڵ� ��Ŀ� ���� ���� ������ �ֱ� ����
	0�� ���� �����Ҽ��� ȿ���� ���� -> 0/0/0/0 �� ���� 0�� �����ϸ� Z4 �� ��ȯ��
	���� ū���ϼ��� ȿ���� ����. INT_MAX ���� 5�� �̳� ǥ������.  2147483647 -> w#)DB
	
	������ ��
	16������ ���ڿ��� ���� ���ԵǾ������� �ȵ�.


 ��� ����

char orgData[] = "/1111/-371/-2504/0/1/1910889/1913072/0/-1/1100/250000/250000/0/0/30801920/4320880/0/-869269219/-2005090179/8388736/9/9/1820851/1834943/0/1787929/329/510/1199/1199/0/0/0/4000/4000/0/0";
//char p[] = "/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0/0";
char szNSysData[4096];
char szDecData[4096];
memset(szNSysData, 0, sizeof(szNSysData));
memset(szDecData, 0, sizeof(szDecData));

// �α� ���ڵ�
NumericSystem::EncodingLog(orgData, szNSysData, sizeof(szNSysData));

// �α� ���ڵ�
NumericSystem::DecodingLog(szNSysData, szDecData, sizeof(szDecData));

// ���� ���� ���ڵ� -> ���ڵ� ��Ų ���� �����ؾ���. nCmp ���� 0�� ��
int nCmp = strcmp(szDecData, orgData);

// szNSysData ��� : "/b</-3/-q\"/0/1/2y,i/2zh~/0/-1/b\\/s:g/s:g/Z2/5yS7/0/-bV#f[/-q_-cG/aD:H/9/9/2oN4/2qeR/0/2k%4/3O/5J/c;/c;/Z3/H1/Z2"
*/


const char TABLE_CHAR90[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', '`', '~', '!', '@', '#', '$', '^', '&', '*', '(', ')', '_', '=', '+', '\\', '|', '[', '{', ']', '}', ';', ':', '\'', '\"', ',', '<', '.', '>', '?' };

class NumericSystem
{
public:
	
	static int TableChar90(char sz)
	{
		switch (sz)
		{
		case '0':	return 0;
		case '1':	return 1;
		case '2':	return 2;
		case '3':	return 3;
		case '4':	return 4;
		case '5':	return 5;
		case '6':	return 6;
		case '7':	return 7;
		case '8':	return 8;
		case '9':	return 9;
		case 'a': return 10;
		case 'b': return 11;
		case 'c': return 12;
		case 'd': return 13;
		case 'e': return 14;
		case 'f': return 15;
		case 'g': return 16;
		case 'h': return 17;
		case 'i': return 18;
		case 'j': return 19;
		case 'k': return 20;
		case 'l': return 21;
		case 'm': return 22;
		case 'n': return 23;
		case 'o': return 24;
		case 'p': return 25;
		case 'q': return 26;
		case 'r': return 27;
		case 's': return 28;
		case 't': return 29;
		case 'u': return 30;
		case 'v': return 31;
		case 'w': return 32;
		case 'x': return 33;
		case 'y': return 34;
		case 'z': return 35;
		case 'A': return 36;
		case 'B': return 37;
		case 'C': return 38;
		case 'D': return 39;
		case 'E': return 40;
		case 'F': return 41;
		case 'G': return 42;
		case 'H': return 43;
		case 'I': return 44;
		case 'J': return 45;
		case 'K': return 46;
		case 'L': return 47;
		case 'M': return 48;
		case 'N': return 49;
		case 'O': return 50;
		case 'P': return 51;
		case 'Q': return 52;
		case 'R': return 53;
		case 'S': return 54;
		case 'T': return 55;
		case 'U': return 56;
		case 'V': return 57;
		case 'W': return 58;
		case 'X': return 59;
		case 'Y': return 60;
		case '`': return 61;
		case '~': return 62;
		case '!': return 63;
		case '@': return 64;
		case '#': return 65;
		case '$': return 66;
		case '^': return 67;
		case '&': return 68;
		case '*': return 69;
		case '(': return 70;
		case ')': return 71;
		case '_': return 72;
		case '=': return 73;
		case '+': return 74;
		case '\\': return 75;
		case '|': return 76;
		case '[': return 77;
		case '{': return 78;
		case ']': return 79;
		case '}': return 80;
		case ';': return 81;
		case ':': return 82;
		case '\'': return 83;
		case '\"': return 84;
		case ',': return 85;
		case '<': return 86;
		case '.': return 87;
		case '>': return 88;
		case '?': return 89;
		}
		return 0;
	}
	
	// N������ 10 ������
	static int N2Dec(char *pData, int nSize)
	{
		int nSystem = sizeof(TABLE_CHAR90);
		int nSum = 0;
		bool bMinus = false;
		for (int i = 0; i < nSize; i++)
		{
			char sz = pData[i];
			if (sz == '-' && i == 0)
			{
				bMinus = true;
				continue;
			}
			int nPow = nSize - i - 1;
			long nVal = TableChar90(sz);
			nSum += nVal * pow(nSystem, nPow);
		}
		return nSum * (bMinus ? (-1) : 1);
	}

	// ���� ��ȯ 10���� -> N����
	static void Dec2N(const int dec, char *pOut, int nBufSize)
	{
		int n = -100;

		memset(pOut, 0, nBufSize);

		int nArrMod[100];    // �������� ������ �迭
		memset(nArrMod, 0, sizeof(nArrMod));
		int nSys = sizeof(TABLE_CHAR90);    // �� ����

		bool bMinus = dec < 0;
		int nVal = abs(dec);    // ���� ��
		int nMod = 0;    // ������
		int nPos = 0;    // �迭 �ε���

						 // 4������ ��� 4���� ���� �������� ũ��, ������
		while (nVal >= nSys)
		{
			int nMod = nVal % nSys;
			nVal = nVal / nSys;


			nArrMod[nPos] = nMod;    // �迭�� ����
			nPos++;
		}

		// ������ ���� �迭 �� �ڿ� �ִ´�
		nArrMod[nPos++] = nVal;

		// ������ �迭�� �ݴ�� ���鼭 pOut�� TABLE �ڵ�ǥ ��� �ִ´�
		int nI = 0;
		for (int i = nPos - 1; i >= 0; i--)
		{
			if (bMinus && nI == 0)
				pOut[nI++] = '-';	// ����ó��

			int nSysIdx = nArrMod[i];
			pOut[nI++] = TABLE_CHAR90[nSysIdx];
		}

		pOut[nI] = '\0';    // ������ �ι���
	}

	// SHCHO 2019 : FindStringPos. Pure C++
	// pSrc = "She is beautiful", pFind = "beau"�� �ָ� 7�� ����. ������ -1
	// ���ڿ� ���� ���̴� 1024
	static int FindStringPos(char *pSrc, char *pFind)
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
		for (int i = 0; i < nLen; i++)
		{
			char cO = pSrc[i];
			char cF = pFind[nMatchCnt];
			if (cO == cF)
			{
				if (nMatchCnt == 0)
					nStartPos = i;	// ������ġ

				if (nMatchCnt == nFindLen - 1)	// ��� ����
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
	// Px.1=5000���� =�� �ָ� 5000�� �о
	static int GetThisValue(char *pSrc, char *chFind, int *pnVal, double *pdVal)
	{
		int nLen = strlen(pSrc);
		int nPos = -1;

		nPos = FindStringPos(pSrc, chFind);

		if (nPos < 0)
			return -1;

		nPos += strlen(chFind);

		char szVal[64];
		memset(szVal, 0, sizeof(szVal));
		int nAddPos = 0;
		bool bDot = false;
		// �ϳ��� �˻��ϸ鼭 ���� or ��ȣ�� �ƴ� char �� ���ö� ���� reading �Ѵ�
		for (int i = nPos; i < nLen; i++)
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


		//if (nAddPos == 0)	// ���� ���� ����
		//	return nPos;

		szVal[nAddPos] = '\0';		// null char

		if (pnVal)
		{
			if (szVal[0] == '.')
				return false;
			*pnVal = atoi(szVal);
		}

		if (pdVal)
			*pdVal = atof(szVal);


		return nPos;
	}

	static int GetThisValueString(char *pSrc, char chSpliter, char *szOutBuf, int nOutBufSize)
	{
		int nLen = strlen(pSrc);
		int nPos = -1;
		char s[2];
		sprintf(s, "%c", chSpliter);
		nPos = FindStringPos(pSrc, s);

		if (nPos < 0)
			return -1;

		nPos += 1;

		char szVal[64];
		memset(szVal, 0, sizeof(szVal));
		int nAddPos = 0;
		bool bDot = false;
		// �ϳ��� �˻��ϸ鼭 ���� spliter�� ���ö����� �˻�
		for (int i = nPos; i < nLen; i++)
		{
			if (sizeof(szVal) - 1 < nAddPos)     // over flow ����
				return -1;

			char c = pSrc[i];
			if (c == chSpliter)
				break;

			szVal[nAddPos++] = c;
		}


		//if (nAddPos == 0)	// ���� ���� ����
		//	return nPos;

		szVal[nAddPos] = '\0';		// null char
		strcpy(szOutBuf, szVal);

		return nPos;
	}

	// /1111/-371/-2504/0/1/1910889/1913072/0/-1/1100/250000/250000/0/0/30801920/4320880/0/-869269219/-2005090179/8388736/9/9/1820851/1834943/0/1787929/329/510/1199/1199/0/0/0/4000/4000/0/0
	static bool EncodingLog(char *pSrc, char *pOut, int nOutSize)
	{
		int nLen = strlen(pSrc);
		int nOffsetPos = 0;
		static const int ARR_SIZE = 1024;
		int nArrData[ARR_SIZE];
		int nArrIdx = 0;

		// [/]������ �������� �����͸� �Ľ��ؼ� int �� �迭�� �����Ѵ�
		while (true)
		{
			int nVal = 0;

			int pos = GetThisValue(pSrc + nOffsetPos, "/", &nVal, NULL);
			if (pos < 0)
				break;

			if (nArrIdx >= ARR_SIZE)	// buffer overflow
				return false;

			nArrData[nArrIdx++] = nVal;

			nOffsetPos += pos + 1;
			if (nLen <= nOffsetPos)
				break;
		}

		// ����� int �迭�� ���� ��ȯ�Ͽ� char����
		char tmpBuf[64];
		memset(tmpBuf, 0, sizeof(tmpBuf));

		char newLogLine[4096];
		memset(newLogLine, 0, sizeof(newLogLine));

		for (int i = 0; i < nArrIdx; i++)
		{
			// 0�� ��� �����ϴ��� ����
			int nZeroCnt = 0;
			while (i < nArrIdx)
			{
				if (nArrData[i] != 0)
					break;
				nZeroCnt++;
				i++;
			}

			// 0�� �ѹ� �ʰ��ϰ� �����ϸ� Z�� ġȯ�Ѵ�. �Ѱ��� ġȯ����. �Ѱ��� ġȯ�ϴ°� ������ ����
			if (nZeroCnt == 1)
				i--;

			else if (nZeroCnt > 1)
			{
				Dec2N(nZeroCnt, tmpBuf, sizeof(tmpBuf));
				if (strlen(tmpBuf) >= 1)
				{
					strcat(newLogLine, "/Z");	//Z�� <- �̷���.
					strcat(newLogLine, tmpBuf);
					i--;
					continue;
				}
			}

			Dec2N(nArrData[i], tmpBuf, sizeof(tmpBuf));
			strcat(newLogLine, "/");
			strcat(newLogLine, tmpBuf);
		}
		int nNewLen = strlen(newLogLine);
		if (nOutSize < nNewLen)
			return false;

		strcpy(pOut, newLogLine);
		return true;
	}

	static bool DecodingLog(char *pSrc, char *pOut, int nOutSize)
	{
		int nLen = strlen(pSrc);
		int nOffsetPos = 0;
		static const int ARR_SIZE = 1024;
		int nArrData[ARR_SIZE];
		int nArrIdx = 0;

		// [/]������ �������� �����͸� �Ľ��ؼ� int �� �迭�� �����Ѵ�
		while (true)
		{
			char buf[64];
			memset(buf, 0, sizeof(buf));
			int pos = GetThisValueString(pSrc + nOffsetPos, '/', buf, sizeof(buf));
			if (pos < 0)
				break;

			if (nArrIdx >= ARR_SIZE)	// buffer overflow
				return false;

			//Zb ���� ���°�� Z�� 0�� �����Ѵٴ� ����, b�� ��ȯ�� 11�̴ϱ�, 0�� 11�� �����ϰ� �������
			if (strlen(buf) > 1)
			{
				if (buf[0] == 'Z')
				{
					char bufZero[64];
					memset(bufZero, 0, sizeof(bufZero));
					strcpy(bufZero, buf + 1);
					int nZeroCnt = N2Dec(bufZero, strlen(bufZero));
					for (int y = 0; y < nZeroCnt; y++)
					{
						nArrData[nArrIdx++] = 0;
					}
					nOffsetPos += pos + 1;
					if (nLen <= nOffsetPos)
						break;
					continue;
				}
			}

			int nVal = 0;

			// ���� ��ȯ�Ͽ� int �迭�� ����
			nVal = N2Dec(buf, strlen(buf));
			nArrData[nArrIdx++] = nVal;

			nOffsetPos += pos + 1;
			if (nLen <= nOffsetPos)
				break;
		}


		char newLogLine[4096];
		memset(newLogLine, 0, sizeof(newLogLine));

		for (int i = 0; i < nArrIdx; i++)
		{
			char tmpp[64];
			sprintf(tmpp, "/%d", nArrData[i]);

			int nLen = strlen(newLogLine) + strlen(tmpp) + 1;
			if (nLen > sizeof(newLogLine))
				break;		// buffer over flow case

			strcat(newLogLine, tmpp);
		}


		if (nOutSize < strlen(newLogLine))
			return false;

		strcpy(pOut, newLogLine);
		return true;
	}

};
