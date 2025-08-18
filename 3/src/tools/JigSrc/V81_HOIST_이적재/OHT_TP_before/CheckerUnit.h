//---------------------------------------------------------------------------

#ifndef CheckerUnitH
#define CheckerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
// ------------- �׽�Ʈ �ùķ��̼� ���� Ŀ�ǵ� -------------
namespace Sim_Cmd_ST
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
namespace Sim_Step_ST
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL �ܺκ��͸��� �����ϰ�, CID-O-S ������ �����̴��� Ȯ���ϰ�, �׽�Ʈ ��ġ�� �̵��� OK��ư�� ���� �ּ���
		StartSteeringMove,

		// ### (2)
	    StartSteeringFRight,
		StartSteeringFRight_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		// (3)
		StartSteeringRRight,
		StartSteeringRRight_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		// ### (4) O1(Left Select) On
		StartSteeringFLeft,
		StartSteeringFLeft_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		StartSteeringRLeft,
		StartSteeringRLeft_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		SteeringCheckComplete,		// (28) Cid Check Complete

	};

};


// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_Shift
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL �ܺκ��͸��� �����ϰ�, CID-O-S ������ �����̴��� Ȯ���ϰ�, �׽�Ʈ ��ġ�� �̵��� OK��ư�� ���� �ּ���
		StartShiftMove,

		// ### (2)
		StartShiftRight,
		StartShiftRight_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		StartShiftHome1,
		StartShiftHome1_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		// ### (4) O1(Left Select) On
		StartShiftLeft,
		StartShiftLeft_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		StartShiftHome2,
		StartShiftHome2_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		ShiftCheckComplete,		// (28) Cid Check Complete

	};

};


// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_Hoist
{
	enum Reserved
	{
	    // (0) Not work
		None		= 0,

	    // ### (1) VHL �ܺκ��͸��� �����ϰ�, CID-O-S ������ �����̴��� Ȯ���ϰ�, �׽�Ʈ ��ġ�� �̵��� OK��ư�� ���� �ּ���
		StartHoistMove,

		// ### (2)
		StartHoistDown,
		StartHoistDown_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		StartHoistHome,
		StartHoistHome_Confirm,		// CID-O-S ������ ���ڰŸ��� �־����ϱ�? (�׸�����)

		HoistCheckComplete,		// (28) Cid Check Complete

	};

};
class TUnitChecker : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *btnSMPSCheck;
	TGroupBox *grpSteer;
	TButton *btnStart;
	TButton *btnStop;
	TGroupBox *grpSteeringIO;
	TPanel *Panel18;
	TShape *RStrRightLt;
	TShape *FStrRightLt;
	TLabel *Label17;
	TLabel *Label18;
	TPanel *Panel19;
	TShape *RStrLeftLt;
	TShape *FStrLeftLt;
	TLabel *Label19;
	TLabel *Label20;
	TMemo *mmoMemo;
	TStringGrid *strngrd1;
	TTimer *tmrIOUI;
	TTimer *tmrSimulation;
	TButton *btnClose;
	TGroupBox *grpShiftHome;
	TGroupBox *GroupBox1;
	TPanel *Panel2;
	TShape *Shp_ShiftHome;
	TLabel *Label3;
	TMemo *mmoShiftHome;
	TButton *btnShiftHomeStart;
	TButton *btnShiftHomeStop;
	TStringGrid *strngrdShiftHome;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TPanel *Panel1;
	TShape *ShpHoist;
	TLabel *Label1;
	TMemo *mmoHoist;
	TButton *btnHoistStart;
	TButton *btnHoistStop;
	TStringGrid *strngrdHoist;
	TTimer *tmrSimulationShift;
	TTimer *tmrSimulationHoist;
	TEdit *edtHoistStandard;
	TLabel *Label2;
	void __fastcall tmrIOUITimer(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrSimulationTimer(TObject *Sender);
	void __fastcall btnSMPSCheckClick(TObject *Sender);
	void __fastcall btnShiftHomeStopClick(TObject *Sender);
	void __fastcall tmrSimulationShiftTimer(TObject *Sender);
	void __fastcall btnShiftHomeStartClick(TObject *Sender);
	void __fastcall btnHoistStopClick(TObject *Sender);
	void __fastcall tmrSimulationHoistTimer(TObject *Sender);
	void __fastcall btnHoistStartClick(TObject *Sender);
private:	// User declarations
	void StatusMachine(int iCmd);
	void Log(UnicodeString strLog, int Type =0);
	void Pass(int iIndex, bool bPass);

	void MaxDistShiftHoist(int iIndex, int MaxDist, bool IsPass = false, bool IsHoist =false);

	void StatusMachineShift(int iCmd);
	void ShiftINCMove(int MoveDist,bool bLeft);

	void StatusMachineHoist(int iCmd);
	void HoistINCMove(int MoveDist);

	void OACISDataSendTP2OHT(short type, short typeNum,int Data1,int Data2=0,int Data3=0,int Data4=0);
public:		// User declarations
	__fastcall TUnitChecker(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUnitChecker *UnitChecker;
//---------------------------------------------------------------------------
#endif
