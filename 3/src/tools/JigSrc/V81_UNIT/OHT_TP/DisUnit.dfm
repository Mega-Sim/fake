object DisForm: TDisForm
  Left = 506
  Top = 180
  Width = 314
  Height = 670
  Caption = 'Display Form'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 298
    Height = 616
    Align = alClient
    BorderStyle = bsNone
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = '����ü'
    Font.Style = []
    ImeName = '�ѱ��� �Է� �ý��� (IME 2000)'
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object SaveDialog1: TSaveDialog
    Left = 96
    Top = 48
  end
  object MainMenu1: TMainMenu
    Left = 176
    Top = 48
    object Save1: TMenuItem
      Caption = 'Save'
      OnClick = Save1Click
    end
  end
end
