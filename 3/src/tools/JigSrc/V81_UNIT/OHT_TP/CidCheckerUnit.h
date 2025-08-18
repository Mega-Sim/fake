//---------------------------------------------------------------------------

#ifndef CidCheckerUnitH
#define CidCheckerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

// ------------- �׽�Ʈ �ùķ��̼� ���� Ŀ�ǵ� -------------
namespace Sim_Cmd
{
	enum Reserved
	{
		TEST_NONE					= 0, // �׽�Ʈ �������� �ƴ�
		TEST_START					= 1, // �׽�Ʈ ����. Status Machine�� �ʱ�ȭ
		TEST_GOING				    = 2, // �׽�Ʈ�� ������ ����
		TEST_PAUSE				    = 3, // �Ͻ�����
		TEST_STOP					= 4,
		// ���� : �� �κп� 01234 ������ �� ������ �����Ϸ��� 8������ �ؼ��� ����!
	};
};

// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL �ܺκ��͸��� �����ϰ�, CID-O-S ������ �����̴��� Ȯ���ϰ�, �׽�Ʈ ��ġ�� �̵��� OK��ư�� ���� �ּ���
		BatteryAndMove,
	    BatteryAndMove_Confirm,	// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

	    // ### (2) �ܺ� ���͸��� �����ϰ� OK��ư�� ���� �ּ���
	    BatteryCut,
	    BatteryCut_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

	    // (3) O6(Operating Mode) On
	    OperatingModeOnCheck,
	    OperatingModeOnCheck_Sleep,

		// ### (4) O1(Left Select) On
		LeftCidLampOn,
		LeftCidLampOn_Sleep,
		LeftCidLampOn_Confirm,	// ���� CID-O-S Sensor�� ���� ���Խ��ϱ�? (�׸�����)

	    InputInterlockOnCheck,	// (5) O3(�շ�����) On
	    InputInterlockOnCheck_Sleep,
	    InputInterlockOffCheck,	// (6) O3(�շ�����) Off
	    InputInterlockOffCheck_Sleep,
	    
		CidPassAckOn,			// (7) O4(����Ϸ�) On
		CidPassAckOn_Sleep,
		CidPassAckOff,			// (8) O4(����Ϸ�) Off
		CidPassAckOff_Sleep,
		
		CidAbnormalOn_1,			// (9) O5(Abnormal) On
		CidAbnormalOn_1_Sleep,
		CidAbnormalOff_1,			// (10) O5(Abnormal) Off
		CidAbnormalOff_1_Sleep,

		CidLeftSelectOff,		// (11) O1(Left Select) Off
		CidLeftSelectOff_Sleep,
		CidLightCheck1,			// (12) �ø��� ��� 'M' �Է�
		// CidLightCheck1_Sleep,
		CidLightCheck1_Confirm,

#if 1
		CidCommonTagOn,			// (13) O7(Common Tag) On
		CidCommonTagOff,		// (14) O7(Common Tag) Off
#else
		CidCommonTagOff,		// (13) O7(Common Tag) Off
		CidCommonTagOn,			// (14) O7(Common Tag) On
#endif
		CidLeftSelOn,			// (15) O1(Left Select) On
		CidLeftSelOn_Sleep,

		CidLeftSelOff_And_OpModeOff,// (16) O1(Left Select) & O6(Operating Mode) Off
		CidLeftSelOn_And_Off_1,	// (17) O1(Left Select) On & Off
		CidLeftSelOn_And_Off_1_Sleep,
		CidLeftSelOn_And_Off_2,
		CidLeftSelOn_And_Off_2_Sleep,
		CidOpModeOn,			// (18) O6(Operating Mode) On

		// ### (19) O2(Right Select) On
		CidRightSelectOn,
		CidRightSelectOn_Confirm,// Left CID-O-S Sensor�� ���� ���Խ��ϱ�? (�׸�����)

		CidInterlockOn,			// (20) O3(�շ�����) On
		CidInterlockOn_Sleep,
		CidInterlockOff,		// (21) O3(�շ�����) Off
		CidInterlockOff_Sleep,
		CidPassOkOn,			// (22) O4(����Ϸ�) On
		CidPassOkOn_Sleep,
		CidPassOkOff,			// (23) O4(����Ϸ�) Off
		CidPassOkOff_Sleep,
		CidAbnormalOn_2,			// (24) O5(Abnormal) On
		CidAbnormalOn_2_Sleep,
		CidAbnormalOff_2,			// (25) O5(Abnormal) Off
		CidAbnormalOff_2_Sleep,

		// (26) O2(Left Select) Off & O6(Operating Mode) Off
		CidLeftSelOn_And_OpModeOff,
		CidLeftSelOn_And_OpModeOff_Sleep,

		CidLightCheck2,			// (27) �ø��� ��� 'M' �Է�
		// CidLightCheck2_Sleep,
		CidLightCheck2_Confirm,
	    CidCheckComplete,		// (28) Cid Check Complete
	};
};

//---------------------------------------------------------------------------
class TfrmCidChecker : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *strGridCheckList;
	TButton *btnStart;
	TButton *btnClose;
	TTimer *tmrSimulation;
	TGroupBox *GroupBox1;
	TShape *shpCID_Link_Compt;
	TLabel *Label2;
	TLabel *Label8;
	TShape *shpCID_Interlock;
	TLabel *Label54;
	TShape *shpCID_Pass_OK;
	TLabel *Label28;
	TShape *shpCID_Status;
	TLabel *Label44;
	TShape *shpCID_Reserved_1;
	TLabel *Label58;
	TShape *shpCID_Reserved_2;
	TGroupBox *GroupBox2;
	TShape *shpOut_LeftSel;
	TShape *shpOut_RightSel;
	TShape *shpOut_OccReq;
	TShape *shpOut_Passed;
	TShape *shpOut_Manual;
	TShape *shpOut_Operation;
	TShape *shpOut_Rev1;
	TShape *shpOut_Rev2;
	TLabel *Label1;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TLabel *Label10;
	TGroupBox *GroupBox4;
	TButton *btnCidLeftSel;
	TButton *btnCidRightSel;
	TButton *btnCidOccupReq;
	TButton *btnCidPassCompt;
	TButton *btnCidManual;
	TButton *btnCidOperation;
	TButton *btnCidRev1;
	TButton *btnCidRev2;
	TButton *btnCidLeftSel_Off;
	TButton *btnCidRightSel_Off;
	TButton *btnCidOccupReq_Off;
	TButton *btnCidPassCompt_Off;
	TButton *btnCidManual_Off;
	TButton *btnCidOperation_Off;
	TButton *btnCidRev1_Off;
	TButton *btnCidRev2_Off;
	TTimer *tmrCidDisplay;
	TImage *Image1;
	TGroupBox *GroupBox3;
	TMemo *mmInspectLog;
	TButton *btnLogClear;
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall tmrCidDisplayTimer(TObject *Sender);
	void __fastcall btnCidLeftSelClick(TObject *Sender);
	void __fastcall btnCidLeftSel_OffClick(TObject *Sender);
	void __fastcall btnCidRightSelClick(TObject *Sender);
	void __fastcall btnCidRightSel_OffClick(TObject *Sender);
	void __fastcall btnCidOccupReqClick(TObject *Sender);
	void __fastcall btnCidOccupReq_OffClick(TObject *Sender);
	void __fastcall btnCidPassComptClick(TObject *Sender);
	void __fastcall btnCidPassCompt_OffClick(TObject *Sender);
	void __fastcall btnCidManualClick(TObject *Sender);
	void __fastcall btnCidManual_OffClick(TObject *Sender);
	void __fastcall btnCidOperationClick(TObject *Sender);
	void __fastcall btnCidOperation_OffClick(TObject *Sender);
	void __fastcall btnCidRev1Click(TObject *Sender);
	void __fastcall btnCidRev1_OffClick(TObject *Sender);
	void __fastcall btnCidRev2Click(TObject *Sender);
	void __fastcall btnCidRev2_OffClick(TObject *Sender);
	void __fastcall tmrSimulationTimer(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnLogClearClick(TObject *Sender);

private:	// User declarations
	void StatusMachine(int iCmd);
	void Log(UnicodeString strLog);
	void ReadCidInput(int &I1, int &I2, int &I3, int &I4);
	void ReadCidOutput(int &O1, int &O2, int &O3, int &O4, int &O5, int &O6, int &O7, int &O8);
	void SetCidOutput(int iCidNo, int iOnOff);
	void Pass(int iIndex, bool bPass);

public:		// User declarations
	__fastcall TfrmCidChecker(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmCidChecker *frmCidChecker;
//---------------------------------------------------------------------------
#endif
