// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include "ErrorSearchUnit.h"
#include "Define.h"
#include "Def_OHT.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorSearchForm *ErrorSearchForm;

// ---------------------------------------------------------------------------
/**
@brief   ErrSearchForm ������
@author  ji7412.lee
@date    2013.02.18
*/
__fastcall TErrorSearchForm::TErrorSearchForm(TComponent* Owner) : TForm(Owner)
{
    m_pListErrNum = new TStringList;
    m_pListErrName = new TStringList;
}
// ---------------------------------------------------------------------------

/**
@brief   ErrCode �׸��� �˻��ϰ� ���ִ� �Լ�
@author  ji7412.lee
@date    2013.02.18
*/
void __fastcall TErrorSearchForm::btnSearchClick(TObject *Sender)
{
    int nErrCode;
    String strDescription;
    nErrCode = StrToIntDef(edtErrorCode->Text.Trim(), 0);
    // strDescription = edtDescritpion->Text.Trim();
    FileSearchingRead(nErrCode, strDescription);
}
// ---------------------------------------------------------------------------

/**
 @brief   ErrCode int������ Open�ϴ� �Լ�
 @author  ji7412.lee
 @date    2013.02.18
 */
void __fastcall TErrorSearchForm::btnOpenClick(TObject *Sender)
{
    String strTmpPath = ExtractFileDir(Application->ExeName) + FILENAME_ERRORCODE;
    ShellExecute(Handle, L"open", strTmpPath.w_str(), NULL, NULL, SW_SHOW);
}
// ---------------------------------------------------------------------------

/**
 @brief   ErrCode int������ Save�ϴ� �Լ�
 @author  ji7412.lee
 @date    2013.02.18
 */
void __fastcall TErrorSearchForm::btnSaveClick(TObject *Sender)
{
    //
}
// ---------------------------------------------------------------------------

/**
@brief   ErrCode StrGrid �ʱ�ȭ �Լ�
@author  zzang9un
@date    2013.07.26
@param   RowCount : ���� �ڵ� ����
*/
void TErrorSearchForm::InitErrorCodeStrGrid(int RowCount)
{
    strngrdErrorCode->Color = TColor(RGB(255, 230, 204));
    strngrdErrorCode->FixedColor = TColor(RGB(255, 128, 0));

    strngrdErrorCode->ColCount = ERRCODE_STRGRID_COL;
    strngrdErrorCode->RowCount = RowCount + 1;
    strngrdErrorCode->Cells[1][0] = "ErrCode";
    strngrdErrorCode->Cells[2][0] = "Description";

    strngrdErrorCode->ColWidths[0] = 30;
    strngrdErrorCode->ColWidths[1] = 60;
    strngrdErrorCode->ColWidths[2] = 490;
}

/**
 @brief   ini���Ͽ� �˻��� Ű���带 Grid�� ���
 @author  ji7412.lee
 @date    2013.02.18
 */
void TErrorSearchForm::FileSearchingRead(int nErrCode, String strDescription)
{
    int nCount = 1;
    String sErrCode, sDescrition;
    if(nErrCode <= 0 && strDescription.IsEmpty())
        return;
    strngrdErrorCode->RowCount = 2;

    sErrCode.sprintf(L"E%04d", nErrCode);
    strngrdErrorCode->Cells[0][1] = nCount;
    strngrdErrorCode->Cells[1][1] = sErrCode;
    strngrdErrorCode->Cells[2][1] = m_pErrorIni->ReadString("ErrorCode", sErrCode, "");
}
// ---------------------------------------------------------------------------

/**
@brief   Error Code�� ��ϵ� INI������ Read�ϴ� �Լ�
@author  zzang9un
@date    2013.07.26
*/
void TErrorSearchForm::ReadErrorCode()
{
    String strFileName;
    String sErrCode;
    
    strFileName = ExtractFileDir(Application->ExeName) + STR_PATH_DATAFILES + FILENAME_ERRORCODE;

    // ���� ���� ���� �Ǵ�
    if(!FileExists(strFileName))
		return;
    
    m_pErrorIni = new TIniFile(strFileName);

    // Key(���� ��ȣ) �о���� 
    m_pErrorIni->ReadSection("ErrorCode", m_pListErrNum);

    // Value(���� �̸�) �о����
    for(int i = 0; i < m_pListErrNum->Count; i++)
    {
        m_pListErrName->Add(m_pErrorIni->ReadString("ErrorCode", m_pListErrNum->Strings[i],""));
    }

    // ���� ������ �ٸ� ��� ���� �̻����� ���� ó��
    if(m_pListErrNum->Count != m_pListErrName->Count)
    {
        Application->MessageBox(L"�����̻�", L"Error", MB_OK);    
        return;
    }

    // Grid �ʱ�ȭ
    InitErrorCodeStrGrid(m_pListErrNum->Count);
    
}

/**
 @brief   ini���Ͽ� ��ü Data�� �ٽ� ���
 @author  ji7412.lee
 @date    2013.02.18
 */
void __fastcall TErrorSearchForm::btnTotalSearchClick(TObject *Sender)
{
    ReadErrorCode();
    DisplayErrorCode();
}
// ---------------------------------------------------------------------------

/**
 @brief   Editâ�� ErrorCode�� ���� ������ �ڵ� Ž���ϴ� �Լ�
 @author  zzang9un
 @date    2013.02.18
 */
void __fastcall TErrorSearchForm::edtErrorCodeChange(TObject *Sender)
{
    int nErrCode;
    String strDescription;
    nErrCode = StrToIntDef(edtErrorCode->Text.Trim(), 0);
    // strDescription = edtDescritpion->Text.Trim();
    FileSearchingRead(nErrCode, strDescription);
}

//---------------------------------------------------------------------------
void __fastcall TErrorSearchForm::FormShow(TObject *Sender)
{
    ReadErrorCode(); 
    DisplayErrorCode();
}
//---------------------------------------------------------------------------

void __fastcall TErrorSearchForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    delete m_pListErrNum;
    delete m_pListErrName;
}
//---------------------------------------------------------------------------

void TErrorSearchForm::DisplayErrorCode()
{
    // Grid �ʱ�ȭ
    InitErrorCodeStrGrid(m_pListErrNum->Count);

    for(int i = 0; i < m_pListErrNum->Count; i++)
    {
        strngrdErrorCode->Cells[0][i+1] = i+1;
        strngrdErrorCode->Cells[1][i+1] = m_pListErrNum->Strings[i];
        strngrdErrorCode->Cells[2][i+1] = m_pListErrName->Strings[i];
    }
}


