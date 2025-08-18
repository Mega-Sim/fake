#ifndef __VERSIONINFO
#define __VERSIONINFO

// ���α׷��� �������� ���

//typedef struct tagVS_FIXEDFILEINFO
//{
//	DWORD   dwSignature;            /* e.g. 0xfeef04bd */
//	DWORD   dwStrucVersion;         /* e.g. 0x00000042 = "0.42" */
//	DWORD   dwFileVersionMS;        /* e.g. 0x00030075 = "3.75" */
//	DWORD   dwFileVersionLS;        /* e.g. 0x00000031 = "0.31" */
//	DWORD   dwProductVersionMS;     /* e.g. 0x00030010 = "3.10" */
//	DWORD   dwProductVersionLS;     /* e.g. 0x00000031 = "0.31" */
//	DWORD   dwFileFlagsMask;        /* = 0x3F for version "0.42" */
//	DWORD   dwFileFlags;            /* e.g. VFF_DEBUG | VFF_PRERELEASE */
//	DWORD   dwFileOS;               /* e.g. VOS_DOS_WINDOWS16 */
//	DWORD   dwFileType;             /* e.g. VFT_DRIVER */
//	DWORD   dwFileSubtype;          /* e.g. VFT2_DRV_KEYBOARD */
//	DWORD   dwFileDateMS;           /* e.g. 0 */
//	DWORD   dwFileDateLS;           /* e.g. 0 */
//} VS_FIXEDFILEINFO;

/**
@class   TVersionInfo
@author  ���¼�
@brief   �ڽ��� ���� ���α׷��� ���� ������ �ٸ� ���α׷��� ���� ������ ������ Ŭ����
@note    �ڱ���� ������ VersionString() �Լ� ���
@note    �ٸ� ���α׷��� ���� ���� ������ GetFileVersionInformation("Project1.exe", "CompanyName") �Լ� ��
*/
class TVersionInfo
{
private:
	// �ڽ��� �������� �о� ���̱�.
	//
	void ReadVersion()
	{
		HRSRC hVersion = FindResource(HInstance, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
		if (hVersion)
		{
			HGLOBAL hResVersion = LoadResource(HInstance, hVersion);
			if (hResVersion)
			{
				void *pVersionInfo = LockResource(hResVersion);
				if (pVersionInfo)
				{
					WORD InfoSize;
					memmove(&InfoSize, pVersionInfo, sizeof(WORD));

					byte *pVersionData = new byte[InfoSize];
					memmove(pVersionData, pVersionInfo, InfoSize);

					void *pFixedFileInfo;
					UINT Len = 0;
					VerQueryValue(pVersionData, L"\\", &pFixedFileInfo, &Len);
					memmove(&Data, pFixedFileInfo, sizeof(VS_FIXEDFILEINFO));
					delete[] pVersionData;
				}
			}
			UnlockResource(hResVersion);
			FreeResource(hResVersion);
		}
	}
	WORD GetMajor()
	{
		return HIWORD(Data.dwProductVersionMS);
	}
	WORD GetMinor()
	{
		return LOWORD(Data.dwProductVersionMS);
	}
	WORD GetRelease()
	{
		return HIWORD(Data.dwProductVersionLS);
	}
	WORD GetBuild()
	{
		return LOWORD(Data.dwProductVersionLS);
	}

public:
	VS_FIXEDFILEINFO	Data;
	__property  WORD Build   = { read=GetBuild };
	__property  WORD Major   = { read=GetMajor };
	__property  WORD Minor   = { read=GetMinor };
	__property  WORD Release = { read=GetRelease };

public:
	TVersionInfo()
	{
		ZeroMemory(this, sizeof(*this));
		ReadVersion();
	}

	String VersionString()
	{
		return String().sprintf(L"[Ver%u.%u.%u.%u]", Major, Minor, Release, Build);
	}

	// ������ ������ ���������� ��� �´�.
	/*  query List
		- CompanyName
		- FileDescription
		- FileVersion
		- InternalName
		- LegalCopyright
		- LegalTrademarks
		- ProductName
		- ProductVersion
	*/
	static String 	GetFileVersionInformation(String filename, String query)
	{
		if (query.IsEmpty())
			return "";

		DWORD dwHandle;
		DWORD dwVersionInfoSize = GetFileVersionInfoSize(filename.w_str(), &dwHandle);
		if (dwVersionInfoSize == 0)
			return "";

		void *pFileInfo = (void*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwVersionInfoSize);

		GetFileVersionInfo(filename.w_str(), dwHandle, dwVersionInfoSize, pFileInfo);
		void *ptr;
		UINT uLength;
		VerQueryValue(pFileInfo, TEXT("\\VarFileInfo\\Translation"), &ptr, &uLength);

		WORD *id = (WORD*) ptr;
		String str = "\\StringFileInfo\\" + IntToHex(id[0], 4) + IntToHex(id[1], 4) + "\\" + query;

		VerQueryValue(pFileInfo, str.w_str(), &ptr, &uLength);
		if (uLength == 0)
			return "";

		String info = (char *)ptr;
		HeapFree(GetProcessHeap(), 0, pFileInfo);

		return info;
	}
};

//2022.11.03 sseung ���� ���� �ý��� ���� ������ ���� �߰�
typedef struct VERSION_INFO_
{
	bool nGetVer;
	AnsiString Main_ver;
	AnsiString Main_Date;
	AnsiString Assistant_ver;
	AnsiString Assistant_Date;
	AnsiString FM_ver;
	AnsiString FM_Date;
	AnsiString AMC_ver;
	AnsiString AMC_Date;
	AnsiString AMC_Dll_ver;
	AnsiString AMC_Dll_Date;

	AnsiString CID_O_ver;
	AnsiString CID_O_DATE;
	AnsiString Regulator_ver;
	AnsiString Regulator_Date;
	AnsiString LCD_ver;
	AnsiString LCD_Date;
	AnsiString EQPIO_ver;
	AnsiString EQPIO_Date;
	AnsiString BCR_ver;
	AnsiString BCR_Date;
	AnsiString QR_ver;
	AnsiString QR_Date;

	AnsiString RSA_Driving_Rear_ver;
	AnsiString RSA_Driving_Rear_Date;
	AnsiString RSA_Driving_Front_ver;
	AnsiString RSA_Driving_Front_Date;
	AnsiString RSA_Hoist_ver;
	AnsiString RSA_Hoist_Date;
	AnsiString RSA_Slide_ver;
	AnsiString RSA_Slide_Date;
	AnsiString Fine_Slave_Rear_ver;
	AnsiString Fine_Slave_Rear_Date;
	AnsiString Fine_Slave_Front_ver;
	AnsiString Fine_Slave_Front_Date;
}VERSION_INFO;

#endif

