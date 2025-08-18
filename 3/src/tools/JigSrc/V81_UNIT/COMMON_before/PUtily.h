//---------------------------------------------------------------------------

#ifndef PUtilyH
#define PUtilyH
//---------------------------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <vcl.h>
#include <iostream>
#include <algorithm>

wchar_t* CharToWChar(const char* pstrSrc);  //char�� wchar�� ����Ʈ
char* WCharToChar(const wchar_t* pwstrSrc); //wchar�� char��  ����Ʈ
char	HexToChar(char ch);
int		HexToDec(TCHAR* caBin);
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len);
char* SubChar(char* chRevData, char* chSndData, int StartPos, int Len, int ninitLen);
int CharToInt(char* chData);

void trim(char *);
void rtrim(char *);
void ltrim(char *);

char* my_strstr(const char *s1, const char *s2);   ///< s1���� s2���ڿ� �˻�
int my_strlen(const char *s1);                     ///< ���ڿ� ���� ��ȯ

// shkim
int Get_CboItemIndex(unsigned char var);
unsigned char CboIndex_ToHex(int var);
unsigned int GetOrderID(void);

void Int2Uchar3(unsigned int iInput, unsigned char& cHigh, unsigned char& cMid, unsigned char& cLow);
int Char2Int(unsigned char cHigh, unsigned char cMid, unsigned char cLow);
AnsiString Char2BinString(unsigned char cChar);




#endif

