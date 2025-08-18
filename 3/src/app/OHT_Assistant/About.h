/**
@file    About.h
$Rev:: 771           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-28 1#$:  Date of last commit
@version OHT 7.0
@brief   ���α׷� ���� ǥ�ø� ���� Form Header ����
*/

//----------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>
//----------------------------------------------------------------------------
#define FILENAME_UPDATE	L"\\Assistant_Update.txt"
//----------------------------------------------------------------------------

/**
@class   TAboutBox
@date    2013.04.03
@author  zzang9un
@brief   Program ���� ǥ�ø� ���� AboutBox
@note    ���α׷� ������ ����, ������Ʈ ������ update.txt ������ Ȱ��
@warning ������Ʈ ���� ������ �������ϰ� ������ ��ġ�� ��ġ�ϵ��� �Ѵ�.
*/
class TAboutBox : public TForm
{
__published:
	TPanel *panProgramInfo;
	TImage *ProgramIcon;
	TLabel *ProgramName;
	TLabel *Version;
	TLabel *Copyright;
	TLabel *CompanyName;
	TButton *OKButton;
	TPageControl *pgcAbout;
	TTabSheet *tabInfo;
	TTabSheet *tabUpdate;
	TRichEdit *redtUpdate;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);

private:
	String __fastcall VersionInfo(const String &sQuery); // ���� ���� �������� �Լ�

public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif    
