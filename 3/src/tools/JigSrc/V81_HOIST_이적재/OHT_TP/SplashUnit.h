/**
@file    SplashUnit.h
$Rev:: 735           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-22 1#$:  Date of last commit
@version OHT 7.0
@brief   OHT TP ���α׷� �ʱ�ȭ���� ����ϴ� SplashForm Header File
*/

#ifndef SplashUnitH
#define SplashUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TSplashForm : public TForm
{
__published:	// IDE-managed Components
    TImage *imgOHT;
    TTimer *timerSplash;
    TPanel *panSplash;
    TProgressBar *pbSplash;
    TRichEdit *memoSplash;
    TPageControl *pgcSplash;
    TTabSheet *tabLogin;
    TTabSheet *tabStatus;
    TButton *btnCancel;
    TButton *btnLogin;
    TEdit *edtID;
    TEdit *edtPassword;
    TEdit *edtDataServerIPAddr;
    TLabel *lbID;
    TLabel *lbIP;
    TLabel *lbPW;
    void __fastcall timerSplashTimer(TObject *Sender);
    void __fastcall btnLoginClick(TObject *Sender);
    void __fastcall imgOHTClick(TObject *Sender);

private:	// User declarations
    String m_strID;                 ///< ID(����� ����)
    String m_strPassword;           ///< Password(��й�ȣ)
    String m_strDataServerIPAddr;   ///< Data Server IP �ּ�

public:		// User declarations
    __fastcall TSplashForm(TComponent* Owner);
    TTextAttributes *__fastcall CurrText(void);
    void PrintMemo(TColor TmpFontColor, String strDisplayData);
    bool IsAbailableIP(String strIPAddr);
};
//---------------------------------------------------------------------------
extern PACKAGE TSplashForm *SplashForm;
//---------------------------------------------------------------------------
#endif
