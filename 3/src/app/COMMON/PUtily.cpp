//---------------------------------------------------------------------------

#pragma hdrstop

#include "PUtily.h"
#include "Protocol_Define_Modify.h"

#pragma hdrstop
//---------------------------------------------------------------------------

//char�� wchar�� ����Ʈ
void CharToWChar( const char* pstrSrc, wchar_t pwstrDest[] )
{
        int nLen = ( int )strlen( pstrSrc ) + 1;
        mbstowcs( pwstrDest, pstrSrc, nLen );
}


//wchar�� char�� ����Ʈ
void WCharToChar(const wchar_t* pwstrSrc, char pstrDest[])
{
      int nLen = ( int )wcslen( pwstrSrc );
	  wcstombs( pstrDest, pwstrSrc, nLen + 1 );
}

/**
@brief   Char ���ڿ� �ڸ��� �Լ�.
@date    2013.03.14
@param   Char ���ڿ�, ���� ��ġ, ����
@return  �ڸ� ���ڿ� ����
*/
//	���װ� �־� �����ϱ�� ��.  shkim. 2015-11-18. ���ø޸𸮸� �����ϸ� �ȵ�!
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len)
{
	// char chSndData[64];
	char* chError = "\0";
	int nDataLen;
//	chSndData = (char *)malloc(sizeof(char)*64);
	nDataLen = strlen(chRevData);
	//1. Error  ó��
	if((nDataLen< StartPos)||((nDataLen - StartPos) < Len)||(Len==0))
	{
//		free(chSndData);
		return chError;
	}
	//2. char���ڿ��� ������ġ, ���̸�ŭ ����.
//	memset(chSndData, 0x00, 10);
	memmove(chSndData, &chRevData[StartPos], Len );

	return chSndData;
}

char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len, int ninitLen)
{
	// char chSndData[64];
	char* chError = "\0";
	int nDataLen;
//	chSndData = (char *)malloc(sizeof(char)*64);
	nDataLen = strlen(chRevData);
	//1. Error  ó��
	if((nDataLen< StartPos)||((nDataLen - StartPos) < Len)||(Len==0))
	{
//		free(chSndData);
		return chError;
	}
	//2. char���ڿ��� ������ġ, ���̸�ŭ ����.
	memset(chSndData, 0x00, ninitLen);

	memmove(chSndData, &chRevData[StartPos], Len );


	return chSndData;
}

/**
@brief   Char�� Int�� ��ȯ�ϴ� �Լ�
@date    2013.03.14
@param   Char ���ڿ�
@return  Int ����
*/
int CharToInt(char* chData)
{
	int nVal=0;
	char aVal[8];
	int nDataLen=0;
	//1. �ִ� ó�� ���ڿ� check!
	nDataLen= strlen(chData);
	if(nDataLen > 8)	return -1;

	strcpy(aVal, chData);
	nVal= atoi(aVal);

	return nVal;
}


/**
@brief   Char���ڿ� ��/�� ���� ����
@date    2013.03.14
@param   Char ���ڿ�
@return
*/
void trim(char *str)
{
	ltrim(str);
	rtrim(str);
}
//-----------------------------------------------------------------------------
void rtrim(char *str)
{
	int i=0;
	int len = strlen(str);
	int start = 0;
	int end = len;
	while((start < end) && (str[end-1] <= ' '))
	{
		end--;
	}

	if(start > end)
	{
		memset(str, '\0', len);
	}
	for(i = 0 ; i < end - start ; i++)
	{
		str[i] = str[i + start];
	}

	for( ; i < len ; i++)
	{
		str[i] = '\0';
	}
}
//-----------------------------------------------------------------------------
void ltrim(char *str)
{
	int i=0;
	int len = strlen(str);
	int start = 0;
	int end = len;
	while((start < len) && (str[start] <= ' '))
	{
		start++;
	}

	if(start > end)
	{
		memset(str, '\0', len);
	}
	for(i = 0 ; i < end - start ; i++)
	{
		str[i] = str[i + start];
	}

	for( ; i < len ; i++)
	{
		str[i] = '\0';
	}
}


/**
@brief   ���ڿ��� �˻��ϴ� �Լ�
@date    2013.03.14
@param   s1: �˻������ �Ǵ� ���ڿ�		s2:�˻�� �Ǵ� ���ڿ�
@return  char *
*/
char* my_strstr(const char *s1, const char *s2){
	int i; //s1�� s2�� ���� �� ���� ���� ���ڿ��� ������ ������ ����

	if(*s2=='\0')//�׳� ������ �ԷµǸ�..
		return (char *)s1;//�ҽ��� �Ǵ� s1�� �״�� ����

	else{
		for(;*s1;s1++){//�˻������ ������..
			if(*s1==*s2){//�˻����� ù���ڿ� �˻������ ���ڰ� ���� ������

				//�˻����� �˻�� ��ġ�ϴ� ���� ��ŭ i����
				for(i=1;*(s1+i)==*(s2+i);i++);

				if(i==my_strlen(s2))//i�� �˻����� ���̰� ������
					return (char *)s1;//������ s1�ּҰ��� ����
			}
		}
		return NULL;//��ġ�ϴ� ���� ������ NULL ��ȯ
	}
}
//------------------------------------------------------------------------------
/**
@brief   ���ڿ��� ���̸� ��ȯ�ϴ�  �Լ�
@date    2013.03.14
@param   s1: �˻� ���ڿ�
@return  ����(int)
*/
int my_strlen(const char *s1){
	int i;
	for(i=0;*s1;s1++, i++);//*s1�� ���� NULL(��)�� �� ������ i�� ����
	return i;
}


// 1��Ʈ�� 1�̰� �������� 0�̸�, ������ ���� ���������
// ��쿡 ����, ���� ������ �޺��ڽ� �ε��� (0����)�� ��ȯ�Ѵ�

// case 1 => hex value input - combo index ouuput
int Get_CboItemIndex(unsigned char var)
{
	switch(var)
	{
		case 0x01	:	return 0;
		case 0x02	:	return 1;
		case 0x04	:	return 2;
		case 0x08	:	return 3;
		case 0x10	:	return 4;
		case 0x20	:	return 5;
		case 0x40	:	return 6;
		case 0x80	:	return 7;

		default:
			ShowMessage(L"GetIndex_UC - error!!");
			return -1;
   }

}

// case 2 => combo index input - hex value output
unsigned char CboIndex_ToHex(int var)
{
	switch(var)
	{
		case 0	: return 0x01;
		case 1 	: return 0x02;
		case 2	: return 0x04;
		case 3	: return 0x08;
		case 4 	: return 0x10;
		case 5	: return 0x20;
		case 6	: return 0x40;
		case 7	: return 0x80;

		default:
			ShowMessage(L"GetIndex_UC - error!!");
			return 0;
   }

}

// ���� ID�� �ڵ����� �����Ѵ�
unsigned int GetOrderID(void)
{
	static int iOrder = 0;

	iOrder++;	// ������ 0���� �����ص� ���� ID�� 1���� �������� ��

	if (iOrder > 10000)
	{
		iOrder = 1;
	}

	return iOrder;
}

// unsigned int ���� ������ 3����Ʈ�� unsigned char������ ��ȯ�Ѵ�
void Int2Uchar3(unsigned int iInput,
				unsigned char& cHigh, unsigned char& cMid, unsigned char& cLow)
{
	unsigned int iTmp;
	iTmp = iInput & 0xFF0000;
	cHigh = iTmp >> 16;
	//cHigh = (iInput & 0xFF0000) >> 16;

	iTmp =  iInput & 0xFF00;
	cMid  = iTmp  >>  8;

	cLow  = iInput & 0xFF;
}

// 3����Ʈ�� unsigned char���� ������ int�� �����Ѵ�
int Char2Int(unsigned char cHigh, unsigned char cMid, unsigned char cLow)
{
	return ((cHigh * 256 * 256) + (cMid * 256) + cLow);
}


AnsiString Char2BinString(unsigned char cChar)
{
	AnsiString strTmp;

	if((cChar & bit_7) > 0)
		strTmp = "1";
	else
		strTmp = "0";

	if((cChar & bit_6) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_5) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_4) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_3) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_2) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_1) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	if((cChar & bit_0) > 0)
		strTmp += "1";
	else
		strTmp += "0";

	return strTmp;
}

unsigned long PreventOverflowGetTimeMs(unsigned long precalc)
{
	double tmpPrecalc = (double)precalc;
	double postcalc = (double)timeGetTime();

	if(tmpPrecalc > postcalc) // �����ð��̹Ƿ�, ������ ���� �ð������� ���� ���� �ð����� ������ �����÷ο�� ���´�.
	{
		tmpPrecalc = tmpPrecalc - (double)0x100000000;
	}

	return (unsigned long)(postcalc - tmpPrecalc);
}

void ExecProgram(String sFileName)
{
	HINSTANCE hResult;
    int nResult;
	String strFileName;
	String strRes;

	strFileName = ".\\" + sFileName;

	hResult = ShellExecute(NULL, L"open", strFileName.c_str(), NULL, NULL, SW_SHOW);
	nResult = (int)hResult;
	switch(nResult)
    {
    case 0:
		strRes = "ERROR : The operating system is out of memory or resources.";
        break;
    case ERROR_FILE_NOT_FOUND:
        strRes = "ERROR : The specified file was not found.";
        break;
	case ERROR_PATH_NOT_FOUND:
        strRes = "ERROR : The specified path was not found.";
        break;
	case ERROR_BAD_FORMAT:
        strRes = "ERROR : The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).";
        break;
	case SE_ERR_ACCESSDENIED:
        strRes = "ERROR : The operating system denied access to the specified file.";
        break;
    case SE_ERR_ASSOCINCOMPLETE:
        strRes = "ERROR : The filename association is incomplete or invalid.";
        break;
    case SE_ERR_DDEBUSY:
        strRes = "ERROR : The DDE transaction could not be completed because other DDE transactions were being processed.";
        break;
    case SE_ERR_DDEFAIL:
        strRes = "ERROR : The DDE transaction failed.";
        break;
    case SE_ERR_DDETIMEOUT:
		strRes = "ERROR : The DDE transaction could not be completed because the request timed out.";
        break;
    case SE_ERR_DLLNOTFOUND:
        strRes = "ERROR : The specified dynamic-link library was not found.";
        break;
    case SE_ERR_NOASSOC:
        strRes = "ERROR : There is no application associated with the given filename extension.";
        break;
    case SE_ERR_OOM:
        strRes = "ERROR : There was not enough memory to complete the operation.";
        break;
    case SE_ERR_SHARE:
        strRes = "ERROR : A sharing violation occurred.";
		break;
    default:
		strRes = "OHT Execute ... complete..";
		break;
    }
}
