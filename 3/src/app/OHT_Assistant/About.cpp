/**
@file    About.cpp
$Rev:: 771           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT 7.0
@brief   ���α׷� ���� ǥ�ø� ���� Form CPP ����
*/

// ---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
// ---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox;

// ---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent *AOwner)
	:
	TForm(AOwner)
{

}
// ---------------------------------------------------------------------

void __fastcall TAboutBox::FormShow(TObject *Sender)
{
	// ���α׷� ������ ���� ���̵��� ����
	pgcAbout->ActivePageIndex = 0;

	// Form�� ������ �� ������Ʈ ������ ���Ϸκ��� �о��
	String FileName;
	FileName = ExtractFileDir(Application->ExeName) + FILENAME_UPDATE;

	redtUpdate->Lines->LoadFromFile(FileName);
}
// ---------------------------------------------------------------------------

/**
@brief   ���������� ���ڿ��� ��ȯ���ִ� �Լ�

         Query�� ���� ���ڿ� �� �ϳ��� �ؾ� �Ѵ�.
         "CompanyName",
		"FileDescription",
		"FileVersion",
		"InternalName",
		"LegalCopyright",
		"LegalTradeMarks",
		"OriginalFileName",
		"ProductName",
		"ProductVersion",
		"Comments"
@param   sQuery : ���� ����(query�� String)
@return  sQuery�� ���� ���� ������ String���� return
*/
String __fastcall TAboutBox::VersionInfo(const String &sQuery)
{
	DWORD dwHandle = 0, dwVersionInfoSize;
	UINT uLength;
	LPVOID pFileInfo, ptr;
	String sOut; // ���ϵ� ���� ����.

	String filename = Application->ExeName;

	dwVersionInfoSize = GetFileVersionInfoSize(filename.w_str(), &dwHandle);

	pFileInfo = (LPVOID) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwVersionInfoSize);

	GetFileVersionInfo(filename.w_str(), dwHandle, dwVersionInfoSize, pFileInfo);
	VerQueryValue(pFileInfo, TEXT("\\VarFileInfo\\Translation"), &ptr, &uLength);

	WORD *id = (WORD*) ptr;
	String szString = "\\StringFileInfo\\" + IntToHex(id[0], 4) + IntToHex(id[1], 4) + "\\" + sQuery;

	VerQueryValue(pFileInfo, szString.w_str(), &ptr, &uLength);
	sOut = String((TCHAR *) ptr);
	HeapFree(GetProcessHeap(), 0, pFileInfo);
	return sOut;
}
// ---------------------------------------------------------------------------

void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
    // by zzang9un 2013.04.03 : ���α׷� ������ ���´�.
	ProgramName->Caption = VersionInfo("FileDescription");
	Version->Caption = "Version : " + VersionInfo("FileVersion");
	Copyright->Caption = VersionInfo("LegalCopyright");
	CompanyName->Caption = VersionInfo("CompanyName");
}
//---------------------------------------------------------------------------

