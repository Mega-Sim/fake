//---------------------------------------------------------------------------

#ifndef IOMapUnitH
#define IOMapUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <IniFiles.hpp>

#include "../Common/Def_DataType.h"
//---------------------------------------------------------------------------
// Grid Default ��
#define INPUTGRID_COL           4   ///< Input Grid ��
#define INPUTGRID_ROW           64  ///< Input Grid ��
#define OUTPUTGRID_COL          3   ///< Output Grid ��
#define OUTPUTGRID_ROW          64  ///< Output Grid ��
#define CHECKONOFF_COL          3   ///< CheckONOFF Grid ��
#define CHECKONOFF_ROW          34  ///< CheckONOFF Grid ��
#define MOTORGRID_COL           3   ///< Motor Grid ��
#define MOTORGRID_ROW           40  ///< Motor Grid ��

#define COL_INPUTNAME           1   ///< Input name�� ��µǴ� ���� Index
#define COL_INPUTONOFF          2   ///< Input ON/OFF�� ��µǴ� ���� Index
#define COL_INPUTDATA           3   ///< Input �������� ��µǴ� ���� Index

#define COL_OUTPUTNAME          1   ///< Input name�� ��µǴ� ���� Index
#define COL_OUTPUTDATA          2   ///< Input �������� ��µǴ� ���� Index

// Ini ���� Key ��
#define INI_KEY_INPUT           L"INPUT"
#define INI_KEY_OUTPUT          L"OUTPUT"
//---------------------------------------------------------------------------

/**
@class   TIOMapForm
@date    2013.02.18
@author  zzang9un
@brief   IOMap�� �����ִ� Form
*/
class TIOMapForm : public TForm
{
__published:	// IDE-managed Components
    TPopupMenu *popupCheckONOFF;
    TMenuItem *mniSetON;
    TMenuItem *mniSetOFF;
    TPopupMenu *popupOutput;
    TMenuItem *mniSetTrue;
    TMenuItem *mniSetFalse;
    TPageControl *pgcIOMap;
    TTabSheet *tabInput;
    TStringGrid *strGridInput;
    TTabSheet *tabOutput;
    TStringGrid *strGridOutput;
    TTabSheet *tabCheckONOFF;
    TStringGrid *strgridUseOnOff;
    TTabSheet *tabMotor;
    TStringGrid *strGridMotor;
   void __fastcall strGridInputDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall strGridOutputDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall strgridUseOnOffDrawCell(TObject *Sender, int ACol,
          int ARow, TRect &Rect, TGridDrawState State);
   void __fastcall popupCheckONOFFPopup(TObject *Sender);
   void __fastcall strgridUseOnOffClick(TObject *Sender);
   void __fastcall mniSetTrueClick(TObject *Sender);
   void __fastcall mniSetOFFClick(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall strGridMotorDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
   void __fastcall strGridOutputClick(TObject *Sender);
   void __fastcall popupOutputPopup(TObject *Sender);
   void __fastcall mniSetFalseClick(TObject *Sender);
   void __fastcall mniSetONClick(TObject *Sender);

private:	// User declarations
	TIniFile *IOMapIni; ///< IOMap.ini ������ �ҷ����� ���� ����
	IO_MAP_FIELD	*m_IOMapField;
	int iInputNumCount;
	int iOutputNumCount;

public:		// User declarations
   __fastcall TIOMapForm(TComponent* Owner);
   void IO_Display();
   void SendUseONOFF(int, int);
	void InitIOMap(IO_MAP_FIELD*);
};

//---------------------------------------------------------------------------
extern PACKAGE TIOMapForm *IOMapForm;
//---------------------------------------------------------------------------
#endif





