//---------------------------------------------------------------------------

#ifndef ChkExecUnitH
#define ChkExecUnitH
//---------------------------------------------------------------------------
#define HOIST_HOME_CHECKDIST_FOR_HANDPUSHON     10.0 ///< Hoist Home�� ��� Hand Push ������ On�� ��� üũ �̵��Ÿ�(mm)
#define HOIST_HOME_CHECKDIST_FOR_HANDPUSHOFF    15.0 ///< Hoist Home�� ��� Hand Push ������ On�� ��� üũ �̵��Ÿ�(mm)
//---------------------------------------------------------------------------
class TCHKOHT
{
 public :
   TCHKOHT();
   ~TCHKOHT();
   int  Is_Executable(void);
   int  Is_Executable(char* ExMessage, ...);
//   int  Is_Executable(int nCount, ...);
   void Warnning_Display(TColor, String);
   int  Axis_Hoist_HOME(void);
   int  Axis_Shift_HOME(void);
   int  Axis_Rotate_HOME(void);
   int  Axis_Shift_Ratate_HOME(void);
   int  Is_MoveAxis_Hoist(void);

   int  Is_MoveAxis_Shift(bool);
   int  Is_MoveAxis_Rotate(void);
   int  Check_HandLimitSensor(void);

   int Axis_Hoist_HOME_With_Smalladd(void);
};
//---------------------------------------------------------------------------
extern TCHKOHT TOHTCheck;
#endif
