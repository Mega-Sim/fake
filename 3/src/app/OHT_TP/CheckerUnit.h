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


// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_SensorCheck
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		// ### (1) E-STOP �� �����ּ���.
		CheckPressESTOP,
		// ### (2) E-STOP �� �����ϰ� Error�� �������ּ���.
		CheckResetESTOP,
		// ### (3) Bumper�� �����ּ���.
		CheckPressBumper,
		// ### (4) Error�� �������ּ���.
		CheckResetBumper,
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};

// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_Oscilation
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		// ### (1) Hoist�� �����ּ���.
		CheckHoistDown,
		// ### (2) Oscilation �� �����ּ���.
		CheckOscilation,
		// ### (3) OScilation Error�� Ȯ�����ּ���.
		CheckError,
		// ### (4) Error�� �������ּ���.
		CheckResetError,
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};
// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_LookDown
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		// ### (1) Hoist�� �����ּ���.
		CheckAreaSelect,
		// ### (2) Oscilation �� �����ּ���.
		CheckLookDown,
		// ### (3) OScilation Error�� Ȯ�����ּ���.
		CheckError,
		// ### (4) Error�� �������ּ���.
		CheckResetError,
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};
// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_Trans
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		// ### (1) Hoist�� �����ּ���.
		CheckLoadUnloadCmd,
		// ### (2) Oscilation �� �����ּ���.
		CheckError,
		// ### (4) Error�� �������ּ���.
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};
       // ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_STBTrans
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		// ### (1) Hoist�� �����ּ���.
		CheckError,
		// ### (4) Error�� �������ּ���.
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};
// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_FoupDetect
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		CheckFoupDetect,
		CheckUnload,
		CheckResetError,
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};
// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
namespace Sim_Step_FoupCover
{
	enum Reserved
	{
		// (0) Not work
		None		= 0,
		SensorCheckStart,
		CheckFoupCover,
		CheckResetError,
		SensorCheckComplete,		// (28) Cid Check Complete

	};

};

// ------------- �׽�Ʈ �ùķ��̼� ���� ���� -------------
class TUnitChecker : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *btnSMPSCheck;
	TGroupBox *grpSteer;
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
	TGroupBox *grpShiftHome;
	TGroupBox *GroupBox1;
	TPanel *Panel2;
	TShape *Shp_ShiftHome;
	TLabel *Label3;
	TMemo *mmoShiftHome;
	TStringGrid *strngrdShiftHome;
	TGroupBox *GroupBox2;
	TGroupBox *GroupBox3;
	TPanel *Panel1;
	TMemo *mmoHoist;
	TStringGrid *strngrdHoist;
	TTimer *tmrSimulationShift;
	TTimer *tmrSimulationHoist;
	TButton *btnShiftHomeStart;
	TButton *btnShiftHomeStop;
	TButton *btnHoistStart;
	TButton *btnHoistStop;
	TGroupBox *grpEStop;
	TPanel *pnlEStop;
	TShape *shpEStop;
	TShape *shpEStopDone;
	TLabel *lblEStopDone;
	TButton *btnEStopStart;
	TButton *btnEStopEnd;
	TLabel *lblEStop;
	TLabel *lblBumperPress;
	TShape *shpBumperPress;
	TTimer *tmrEStop;
	TGroupBox *grpTextZone;
	TShape *shpTotalPopup;
	TLabel *lblTextPopup;
	TLabel *lblTimeLimit;
	TLabel *lblLimitTime;
	TGroupBox *grpCheckAll;
	TCheckBox *chkRailTest;
	TLabel *lblCurrentTime;
	TStringGrid *strngrdEStop1;
	TButton *btnStart;
	TButton *btnStop;
	TGroupBox *grpOscilation;
	TShape *shpOscilationDone;
	TLabel *lblOscilationDone;
	TPanel *pnlOscilationDone;
	TShape *shpOscilation;
	TLabel *lblOsclation;
	TButton *btnOscilationStart;
	TButton *btnOsclationEnd;
	TStringGrid *strngrdOscilation;
	TTimer *tmrOscilation;
	TGroupBox *grpLookDown;
	TShape *shpLookDownDone;
	TLabel *lblLookDownDone;
	TPanel *pnlLookDown;
	TShape *shpLookDown;
	TLabel *lblLookDown;
	TButton *btnLookDownStart;
	TButton *btnLookDownEnd;
	TStringGrid *strngrdLookDown;
	TLabel *Label1;
	TShape *ShpHoist;
	TEdit *edtHoistStandard;
	TLabel *lbl1;
	TTimer *tmrLookDown;
	TGroupBox *grpSTBDetect;
	TShape *shpSTBDoubleDone;
	TLabel *lblSTBDoubleDone;
	TPanel *pnl1STBDetect;
	TButton *btnSTBNoCarrier;
	TButton *btnSTBDoubleCarrier;
	TStringGrid *strngrdSTBDetect;
	TLabel *lblSTBNoDone;
	TShape *shpSTBNoDone;
	TButton *btnSTBDetectEnd;
	TTimer *tmrSTBNo;
	TTimer *tmrSTBDouble;
	TButton *btnClose;
	TCheckBox *chkSTBDirection;
	TGroupBox *grpEQPort;
	TShape *shpEQNoDone;
	TShape *shpEQDoubleDone;
	TLabel *lblEQDoubleDone;
	TLabel *lblEQNoDone;
	TPanel *pnlEQ;
	TButton *btnEQNo;
	TButton *btnEQDone;
	TButton *btnEQStop;
	TStringGrid *strngrdEQ;
	TTimer *tmrEQNo;
	TTimer *tmrEQDouble;
	TGroupBox *grpFoup;
	TShape *shpFoupDetectDone;
	TLabel *lblFoupDetectDone;
	TPanel *pnlFoup;
	TShape *shpFoupDetect;
	TLabel *lblFoupDetect;
	TButton *btnFoupDetectStart;
	TButton *btnFoupDetectStop;
	TStringGrid *strngrdFoupDetect;
	TTimer *tmrFoupDetect;
	TGroupBox *grpFoupCover;
	TShape *shpFoupCoverDone;
	TLabel *lblFoupCoverDone;
	TPanel *pnlFoupCover;
	TShape *shpFoupCover;
	TLabel *lblFoupCover;
	TButton *btnFoupCoverStart;
	TButton *btnFoupCoverStop;
	TStringGrid *strngrdFoupCover;
	TTimer *tmrFoupCover;
	TCheckBox *chkTest;
	TButton *btnUBGOBS;
	TShape *shpSTBRDetect;
	TLabel *lblSTBRDetect;
	TLabel *lblSTBLDetect;
	TShape *shpSTBLDetect;
	TButton *btnCIDCheck;
	TLabel *lblYSCode;
	TEdit *edtYSCode;
	TLabel *lblOscilationTime;
	TLabel *lblLookDownTime;
	TLabel *lblVHLName;
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
	void __fastcall btnEStopStartClick(TObject *Sender);
	void __fastcall btnEStopEndCick(TObject *Sender);
	void __fastcall btnOscilationStartClick(TObject *Sender);
	void __fastcall btnOsclationEndClick(TObject *Sender);
	void __fastcall tmrOscilationTimer(TObject *Sender);
	void __fastcall tmrEStopTimer(TObject *Sender);
	void __fastcall btnLookDownStartClick(TObject *Sender);
	void __fastcall btnLookDownEndClick(TObject *Sender);
	void __fastcall tmrLookDownTimer(TObject *Sender);
	void __fastcall btnSTBNoCarrierClick(TObject *Sender);
	void __fastcall btnSTBDoubleCarrierClick(TObject *Sender);
	void __fastcall btnSTBDetectEndClick(TObject *Sender);
	void __fastcall tmrSTBNoTimer(TObject *Sender);
	void __fastcall tmrSTBDoubleTimer(TObject *Sender);
	void __fastcall btnEQNoClick(TObject *Sender);
	void __fastcall btnEQDoneClick(TObject *Sender);
	void __fastcall btnEQStopClick(TObject *Sender);
	void __fastcall tmrEQNoTimer(TObject *Sender);
	void __fastcall tmrEQDoubleTimer(TObject *Sender);
	void __fastcall btnFoupDetectStartClick(TObject *Sender);
	void __fastcall tmrFoupDetectTimer(TObject *Sender);
	void __fastcall btnFoupDetectStopClick(TObject *Sender);
	void __fastcall tmrFoupCoverTimer(TObject *Sender);
	void __fastcall btnFoupCoverStartClick(TObject *Sender);
	void __fastcall btnFoupCoverStopClick(TObject *Sender);
	void __fastcall btnUBGOBSClick(TObject *Sender);
	void __fastcall btnCIDCheckClick(TObject *Sender);
private:	// User declarations
	void StatusMachine(int iCmd);
	void Log(UnicodeString strLog, int Type =0);
	void LastTime(DWORD dwTmpTime, DWORD dwRefTime, bool bUse = false);
	void Pass(int iIndex, bool bPass);

	void MaxDistShiftHoist(int iIndex, int MaxDist, bool IsPass = false, bool IsHoist =false);

	void StatusMachineShift(int iCmd);
	void ShiftINCMove(int MoveDist,bool bLeft);

	void StatusMachineHoist(int iCmd);
	void HoistINCMove(int MoveDist);

	void StatusMachineEStop(int iCmd);
	void StatusMachineOscilation(int iCmd);
	void StatusMachineLookDown(int iCmd);

	void StatusMachineSTBNo(int iCmd);
	void StatusMachineSTBDouble(int iCmd);
	void StatusMachineEQNo(int iCmd);
	void StatusMachineEQDouble(int iCmd);

	void StatusMachineFoupDetect(int iCmd);
	void StatusMachineFoupCover(int iCmd);


	void OACISDataSendTP2OHT(short type, short typeNum,int Data1,int Data2=0,int Data3=0,int Data4=0);
	void OACISLocalLog(String strLog);

	void PassOrNot(TStringGrid* a, int iIndex, bool bPass = false);
	void PassOrNot(TStringGrid* a, int iIndex, bool bPass, double dSec);

	int tmpTesting;
public:		// User declarations
	__fastcall TUnitChecker(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TUnitChecker *UnitChecker;
//---------------------------------------------------------------------------
#endif
