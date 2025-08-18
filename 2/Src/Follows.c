#define FOLLOWS_GLOBALS

#include "all.h"


#pragma CODE_SECTION(".user_int");
unsigned short crc16_ccitt(const char *buf, int len)
{
	register int counter;
	register unsigned short crc = 0;
	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
	return crc;
}

#pragma CODE_SECTION(".user_int");
int CalBit(int *IO_arr, int BIT_Index, int BIT_Size)
{
	int IO_value = 0, temp = 0;
	int j = 0, Num = 0;
	int Array_size = sizeof(IO_arr) * 8;

	// IO 배열 위치 확인
	Num = BIT_Index / Array_size;
	// BIT 확인
	for(int i = 0; i < BIT_Size; i++)
	{
		temp = (IO_arr[Num] >> ((BIT_Index - (Num * 32)) + i)) & 0x1;
		IO_value += temp * pow(2,j);
		j++;
		// 배열 끝나면 다음 배열로
		if(((BIT_Index + i +1) % 32) == 0) Num++;
	}
	return IO_value;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: Get_Velocity_by_Area
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
double Get_Velocity_by_Area(void)
{
	OBS_Sensor_value = ((IO_Input[0] >> Model_OBS_DETECT_READ_BIT_SCALE) & 0xF);
	OHT_Sensor_value = ((IO_Input[1] >> Model_OHT_DETECT_READ_BIT_SCALE) & 0xF);

	OBS_Ignore_Distance = abs(OBS_Distance - pCBuf[0]->jtpos);

	if(Model_OBS_SENSOR_MAKER == HOKUYO)
	{		
		switch(OBS_Sensor_value)
		{
			case HOKUYO_OBS_SPEED_GRADE_8_BIT:
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;  

			case HOKUYO_OBS_SPEED_GRADE_7_BIT :
			if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
			{    
				if((OBS_Ignore_Distance < (Model_PULSETOMM*2000)) && (OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE))
				{				
					OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				}				
				else
				{				
					OBS_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;			
				}
			}
			else
			{
				if((OBS_Ignore_Distance < (Model_PULSETOMM*2000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
				{				
					OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				}				
				else
				{				
					OBS_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;			
				}
			}
			break;  

			case HOKUYO_OBS_SPEED_GRADE_5_BIT :
				if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*1000)) && (OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;			
					}		
				}
				else
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*1000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;			
					}	
				} 
				break;  

			case HOKUYO_OBS_SPEED_GRADE_3_BIT :
				OBS_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;			
				break;  

			case HOKUYO_OBS_SPEED_GRADE_STEP_OUT :
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;  

			default :
				OBS_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
				break;  
		}
	}		
	else
	{
		switch(OBS_Sensor_value)
		{
			case SICK_OBS_SPEED_GRADE_8_BIT:
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;  

			case SICK_OBS_SPEED_GRADE_7_BIT :
				if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*2500)) && (OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;			
					}
				}
				else
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*2500)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;			
					}
				}
				break;  

			case SICK_OBS_SPEED_GRADE_5_BIT :
				if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*1000)) && (OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;			
					}		
				}
				else
				{
					if((OBS_Ignore_Distance < (Model_PULSETOMM*1000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
					}				
					else
					{				
						OBS_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;			
					}	 
				}                     
				break;  

			case SICK_OBS_SPEED_GRADE_3_BIT :
				OBS_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;			
				break;  

			case SICK_OBS_SPEED_GRADE_STEP_OUT :
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;  

			default :
				OBS_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
				break;  
		}
	}

	if(PBS_Ignore_Flag == true)
	{		
		OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;		
	}		

	if(Model_OBS_SENSOR_MAKER == HOKUYO)
    {   
    	if(!((OBS_Sensor_value >> 3) & 0x1))
    	{
            OBS_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
    	}
	}

    switch(OHT_Sensor_value)
	{
        case OHT_SPEED_GRADE_8_BIT:
            OHT_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
            break;  
            
        case OHT_SPEED_GRADE_7_BIT :
            OHT_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;
            break;  

        case OHT_SPEED_GRADE_6_BIT :
            OHT_velocity_Level = Model_SPEED_GRADE_6_VELOCITY;
            break;  
            
        case OHT_SPEED_GRADE_5_BIT :
            OHT_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;
            break;  
            
        case OHT_SPEED_GRADE_4_BIT :
            OHT_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;
            break;  
	
		case OHT_SPEED_GRADE_3_BIT :
			OHT_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;
    		break;	

        default : 
            OHT_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
            break;  
    }

	if(!((OHT_Sensor_value >> 3) & 0x1))
	{
        OHT_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
	}

	if(Model_OHT_SENSOR_ENABLE == false)
	{
		OHT_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
	}

    if(OHT_velocity_Level <= OBS_velocity_Level) 
    {
	    Sensor_velocity_Level = OHT_velocity_Level;	
		
		if(Sensor_velocity_Level == Model_SPEED_GRADE_1_VELOCITY)
		{			
			AMC_Follow_Control_Status = STATUS_OBS_STOP;		
		}			
		else
		{	
			AMC_Follow_Control_Status = STATUS_NORMAL;				
		}		
    }
	else
    {
		AMC_Follow_Control_Status = STATUS_NORMAL;							    
		
		if(OBS_velocity_Level == Model_SPEED_GRADE_7_VELOCITY)
		{
			if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
			{
				if(OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE)
				{
					Sensor_velocity_Level = VELOCITY_0_5_MPS;				
				}	
				else
				{
					if((Model_OHT_ID == NEO_TRAY) || (Model_OHT_ID == NEO_SSD))
					{					
						Sensor_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;	
					}					
					else if((Model_OHT_ID == NEO_MAGAZINE) || (Model_OHT_ID == NEO_CASSETTE) || (Model_OHT_ID == FOSB_2WD) || (Model_OHT_ID == OHT_NAME_MASK))
					{					
						Sensor_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;	
					}					
					else
					{					
						Sensor_velocity_Level = Model_SPEED_GRADE_6_VELOCITY;			
					}					
				}
			}
			else
			{
				if((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE)&&(OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE))
				{
					Sensor_velocity_Level = VELOCITY_0_5_MPS;				
				}	
				else
				{
					Sensor_velocity_Level = Model_SPEED_GRADE_6_VELOCITY;							
				}
			}   
		}
		else if(OBS_velocity_Level == Model_SPEED_GRADE_5_VELOCITY)    		
		{
			if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
			{
				if(OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE)
				{
					Sensor_velocity_Level = VELOCITY_0_15_MPS;
				}	
				else
				{
					if((Model_OHT_ID == NEO_TRAY) || (Model_OHT_ID == NEO_SSD))
					{
						Sensor_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;	
					}					
					else if((Model_OHT_ID == NEO_MAGAZINE) || (Model_OHT_ID == NEO_CASSETTE) || (Model_OHT_ID == FOSB_2WD) || (Model_OHT_ID == OHT_NAME_MASK))
					{					
						Sensor_velocity_Level = Model_SPEED_GRADE_2_VELOCITY;	
					}					
					else
					{					
						Sensor_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;	
					}					
				}		
			}
			else
			{
				if((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE)&&(OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE))
				{
					Sensor_velocity_Level = VELOCITY_0_15_MPS;
				}	
				else
				{				
					Sensor_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;					
				}		
			}
		}
		else 
		{
			Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;	

			AMC_Follow_Control_Status = STATUS_PBS_STOP;							
		}		
    }    

	if(AMC_Pause_Control == true)
	{
		Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;	
		
		AMC_Follow_Control_Status = STATUS_OHT_PAUSED;
	}
	
	if((Servo_Off_type == 1) && (TorqueLimit_Response != 20))
	{
		if((TorqueLimit_Response == 10) && (Sensor_velocity_Level != Model_SPEED_GRADE_1_VELOCITY))
		{
			TorqueLimit_Request = 2;
		}
		
		Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;	
		AMC_Follow_Control_Status = STATUS_OHT_PAUSED;
	}
    	
	return Sensor_velocity_Level;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: GetBit
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetBit(int Source, int BitNum)       
{
    return (Source >> BitNum) & 0x01;	
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: GetPos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetPos(void)
{
    return (int)pCBuf[0]->jtpos;		
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: GetCurrentPos
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
int GetCurrentPos(int Axis)
{
    return (int)pCBuf[Axis]->jtpos;		
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: AxisCmdDo
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void AxisCmdDo(AMC_CMD *AmcCmd, int axis)
{
	volatile double FollowsACC_tmp;
	volatile double FollowsDCC_tmp;
	volatile double FollowsVEL_tmp;
	volatile int FollowsPOS_tmp;	
	volatile int nloop;
   	volatile int i;    

	for(nloop = 0 ; nloop < 3 ; nloop++)
	{
		Marking_Process_Flag[nloop] = false;						
		UseSmallAdd_For_Marking[nloop] = false;
		Marking_Process_Trigger_Flag[nloop] = false;
	}
	
	switch(AmcCmd->CmdAxis[axis].CmdType)
	{
		case AMC_CMD_AXIS_NONE : 
    		break;	

		case AMC_CMD_AXIS_RESET : 
			_dsp_amp_fault_reset(axis);
	    	break;

		case AMC_CMD_AXIS_AMP_ENABLE: 
			_dsp_set_amp_enable(axis, TRUE);
    		break;

		case AMC_CMD_AXIS_AMP_DISABLE:	
			_dsp_set_amp_enable(axis, FALSE);			
	    	break;

		case AMC_CMD_AXIS_MOVE_P:		
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			_dsp_move_p(axis, AmcCmd->CmdAxis[axis].Accel, AmcCmd->CmdAxis[axis].Speed);
			break;
			
		case AMC_CMD_AXIS_MOVE_N:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			_dsp_move_n(axis, AmcCmd->CmdAxis[axis].Accel, AmcCmd->CmdAxis[axis].Speed);
	    	break;

		case AMC_CMD_AXIS_MOVE_S:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			MoveS_Flag = 3;
			Follow_Enable_Flag = false;

			SoftMotionFlag = false;
			count_SoftMotion = 0;		

            SoftStopMotionFlag = false;
            count_Soft_StopMotion = 0;

            for(i=0; i<5 ; i++)
            {
                if(_dsp_move_s(axis, AmcCmd->CmdAxis[axis].Decel) == MMC_OK)
                {
                    break;
                }          
                else
                {
                    Delay(1);
                }
            }
            
            if(i>=5)
            {
            	forcedMoveStopCommand = true;
            	_dsp_move_s(axis, AmcCmd->CmdAxis[axis].Decel);  
            	forcedMoveStopCommand = false;            	          	
            } 
    	    break;

		case AMC_CMD_AXIS_MOVE_DS:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
			
			TriggerCtrlInfoParsing[axis].Cmd = AmcCmd->CmdAxis[axis].TriggerCtrlInfo;
			UseSmallAdd_For_Marking[axis] = TriggerCtrlInfoParsing[axis].TriggerInfo.UseSmallAdd;			

			JtPos_During_Marking[axis] = 0; 

            SoftMotionFlag = false;
			count_SoftMotion = 0;	

            SoftStopMotionFlag = false;
            count_Soft_StopMotion = 0;

			if(UseSmallAdd_For_Marking[axis] == true)
			{
				IoNumber_For_Marking[axis] = TriggerCtrlInfoParsing[axis].TriggerInfo.TriggerIONumber;
				IoType_For_Marking[axis] = TriggerCtrlInfoParsing[axis].TriggerInfo.IsTriggerOnRisingEdge;
				DistanceAddition_For_Marking[axis] = TriggerCtrlInfoParsing[axis].TriggerInfo.AdditionalDist;

				ACC_For_Marking[axis] = AmcCmd->CmdAxis[axis].Accel;
				DCC_For_Marking[axis] = AmcCmd->CmdAxis[axis].Decel;
				VEL_For_Marking[axis] = AmcCmd->CmdAxis[axis].Speed;
				POS_For_Marking[axis] = pCBuf[axis]->jtpos + DistanceAddition_For_Marking[axis];				
				
				DistanceLimit_For_Marking[axis] = AmcCmd->CmdAxis[axis].Distance;	

				if((DistanceLimit_For_Marking[axis]-pCBuf[axis]->jtpos) >= 0)
				{
					DirectionFlag_For_Marking[axis] = true;					
				}
				else
				{
					DirectionFlag_For_Marking[axis] = false;					
				}

				if(IoNumber_For_Marking[axis] < 32)
				{
					FlagIO_For_Marking[axis] = GetBit(IO_Input[0], IoNumber_For_Marking[axis]);
				}
				else
				{
					FlagIO_For_Marking[axis] = GetBit(IO_Input[1], IoNumber_For_Marking[axis]-32);
				}

				Follow_Enable_Flag = true;
				Marking_Process_Flag[axis] = true;			 
								
			    MEnqueue(MAxisQueue[axis], 5);  
			    MEnqueue(MAxisQueue[axis], ACC_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], DCC_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], VEL_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], DistanceAddition_For_Marking[axis]);  				
			    MEnqueue(MAxisQueue[axis], DistanceLimit_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], IoNumber_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], DirectionFlag_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], IoType_For_Marking[axis]);  
			    MEnqueue(MAxisQueue[axis], pCBuf[axis]->jtpos);  				
			    MEnqueue(MAxisQueue[axis], Count_200us_INT_4_Debug);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  
			    MEnqueue(MAxisQueue[axis], 0);  					
			}
			else
			{
				Marking_Process_Flag[axis] = false;

	            if(axis == 0)
	            {
		            Check_Front_Rear_servo_On();

					FollowsACC_tmp = AmcCmd->CmdAxis[axis].Accel; 
					FollowsDCC_tmp = AmcCmd->CmdAxis[axis].Decel;
					FollowsVEL_tmp = AmcCmd->CmdAxis[axis].Speed;
					FollowsPOS_tmp = AmcCmd->CmdAxis[axis].Distance;
					
					MoveDS_Flag = 5;		
					
					if(_dsp_move_ds(axis, FollowsACC_tmp, FollowsDCC_tmp, FollowsVEL_tmp, FollowsPOS_tmp) == MMC_OK)
					{
						Follow_Enable_Flag = FALSE;
						
						FollowsPOS = FollowsPOS_tmp;
						FollowsVEL = FollowsVEL_tmp;					
						FollowsACC = FollowsACC_tmp;
						FollowsDCC = FollowsDCC_tmp;

						FollowsDCC_CMD = FollowsDCC;

						FollowsPOS_Prev = FollowsPOS;					
						FollowsVEL_Prev = FollowsVEL;					
						FollowsACC_Prev = FollowsACC;
						FollowsDCC_Prev = FollowsDCC;
						
						Follow_Enable_Flag = TRUE;					
					}
				}
				else
				{
					MoveDS_Flag = 6;	
					
					_dsp_move_ds(axis, AmcCmd->CmdAxis[axis].Accel, AmcCmd->CmdAxis[axis].Decel, AmcCmd->CmdAxis[axis].Speed, AmcCmd->CmdAxis[axis].Distance);
				}					
			}	
    		break;

		case AMC_CMD_AXIS_MOVE_TIME_P:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			_dsp_move_pt(axis, (AmcCmd->CmdAxis[axis].Accel/SECTOMSEC), AmcCmd->CmdAxis[axis].Speed);
    		break;

		case AMC_CMD_AXIS_MOVE_TIME_N:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			_dsp_move_nt(axis, (AmcCmd->CmdAxis[axis].Accel/SECTOMSEC), AmcCmd->CmdAxis[axis].Speed);
    		break;

		case AMC_CMD_AXIS_MOVE_TIME_S:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;

			Follow_Enable_Flag = false;
			MoveS_Flag = 4;
			_dsp_move_st(axis, (AmcCmd->CmdAxis[axis].Decel/SECTOMSEC));
    		break;

		case AMC_CMD_AXIS_MOVE_TIME_DS:	
			if((axis == 0) && (SyncMotion.Flag != true) && (flag4AbortEventStop != 1))	SyncMotion.Flag = true;
		
			Marking_Process_Flag[axis] = false;
			
            if(axis == 0)
			{				
	            Check_Front_Rear_servo_On();

				FollowsACC_tmp = (AmcCmd->CmdAxis[axis].Accel/SECTOMSEC);
				FollowsDCC_tmp = (AmcCmd->CmdAxis[axis].Decel/SECTOMSEC);
				FollowsVEL_tmp = AmcCmd->CmdAxis[axis].Speed;
				FollowsPOS_tmp = AmcCmd->CmdAxis[axis].Distance;
				
				MoveDS_Flag = 7;									
				_dsp_move_dst(axis, FollowsACC_tmp, FollowsDCC_tmp, FollowsVEL_tmp, FollowsPOS_tmp);

				Follow_Enable_Flag = FALSE;
				
				FollowsPOS = FollowsPOS_tmp;
				FollowsVEL = FollowsVEL_tmp;					
				FollowsACC = SPEED_2_0_MPS2;
				FollowsDCC = SPEED_3_0_MPS2;

				FollowsDCC_CMD = FollowsDCC;

				FollowsPOS_Prev = FollowsPOS;					
				FollowsVEL_Prev = FollowsVEL;					
				FollowsACC_Prev = FollowsACC;
				FollowsDCC_Prev = FollowsDCC;
				
				Follow_Enable_Flag = TRUE;		
			}
			else
			{				
				MoveDS_Flag = 8;									
				_dsp_move_dst(axis, (AmcCmd->CmdAxis[axis].Accel/SECTOMSEC), (AmcCmd->CmdAxis[axis].Decel/SECTOMSEC), AmcCmd->CmdAxis[axis].Speed, AmcCmd->CmdAxis[axis].Distance);
			}				
			break;

		case AMC_CMD_AXIS_SET_HOME:
            __SetPosition(axis, 0);
	    	break;

		case AMC_CMD_AXIS_SET_OFFSET:	
		    break;

		case AMC_CMD_AXIS_SET_POS_LIMIT:
    		break;

		case AMC_CMD_AXIS_SET_NEG_LIMIT:
	    	break;

		case AMC_CMD_AXIS_RESET_LIMIT:	
    		break;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: Main_to_Dsp_Command
// Brief	: DPRAM 통해서 들어온 명령 처리
//            명령 처리 부분과 리턴 부분으로 나뉘어진다.
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Main_to_Dsp_Command(char *Command, int Size, char *ReturnMessage, int *MessageLength)
{
    int ax;
    int Io1_out=0, Io2_out=0, Io3_out=0, Io4_out=0, Io5_out=0, Io6_out=0, Io7_out=0, Io8_out=0;    
    unsigned char DetectOHT = 0, DetectOBS = 0, tmpOBSIO = 0, tmpOHTIO = 0;
	
	AMC_CMD *AmcCmd = (AMC_CMD *)&Command[0];		
	AMC_DATA *Amc = (AMC_DATA *)ReturnMessage;  
	
    int temp_crc = AmcCmd->crc;
    AmcCmd->crc = 0;
    
    if(temp_crc == crc16_ccitt((char *)AmcCmd, sizeof(AMC_CMD)))
    {
            // 명령 처리
        if(GetBit(AmcCmd->Cmd, 20))
    	{
    		UBG_Setting_flag = 1;
    	}
    	else
    	{
    		UBG_Setting_flag = 0;
    	}
    	if(GetBit(AmcCmd->Cmd, 0))            
    	{
    		Io1_out = AmcCmd->Output[0];
    		Io2_out = AmcCmd->Output[1];
    		Io3_out = AmcCmd->Output[2];		
    		Io4_out = AmcCmd->Output[3];
    		
    		tmpOBSIO = (((Io2_out & Model_OBS_DETECT_WRITE_BIT_MASK)>>Model_OBS_DETECT_WRITE_BIT_SCALE) & 0xf);
    		tmpOHTIO = (((Io2_out & Model_OHT_DETECT_WRITE_BIT_MASK)>>Model_OHT_DETECT_WRITE_BIT_SCALE) & 0xf);

            if((Io4_out & 0x8000) == FALSE)
            {
                if((Model_OHT_ID == NEO_SSD_TRAY) || (Model_OHT_ID == OHT_NAME_MODULETRAY))
                {
                    Io2_out = ((Io2_out & (~Model_OBS_DETECT_WRITE_BIT_MASK)) | (OBS_Detect_AreaSetting<<Model_OBS_DETECT_WRITE_BIT_SCALE));
                }                
                else
                {
                    Io2_out = ((Io2_out & (~Model_OHT_DETECT_WRITE_BIT_MASK)) | (OHT_Detect_AreaSetting_For_Curve<<Model_OHT_DETECT_WRITE_BIT_SCALE));
                    Io2_out = ((Io2_out & (~Model_OBS_DETECT_WRITE_BIT_MASK)) | (OBS_Detect_AreaSetting<<Model_OBS_DETECT_WRITE_BIT_SCALE));
                }
            }
            else
            {
    			if(tmpOBSIO != OBS_Detect_AreaSetting)
    			{
    				OBS_Detect_AreaSetting = tmpOBSIO;
    			}
    			if((UBG_Setting_flag) && (tmpOHTIO != OHT_Detect_AreaSetting_For_Curve))
    			{
    				OHT_Detect_AreaSetting_For_Curve = tmpOHTIO;
    				OHT_Detect_AreaSetting = tmpOHTIO;
    			}
            }
            
            __SetIoOutData_256(Io1_out, Io2_out, Io3_out, Io4_out, Io5_out, Io6_out, Io7_out, Io8_out);
        }   

    	if(GetBit(AmcCmd->Cmd, 1))        
    	{
    		NodeCount = 0;
    		
    		if(NodeCountState[3].SensorStatus)  
    		{
    			PreCheckCount = 1;
    		}
    		else
    	    {
    			PreCheckCount = 0;
    		}

    		// initiailize
    		PositionOfDrivingTag = 0; 		
     		PositionOfTransTagFirst = 0; 
     		PositionOfTransTagFinal = 0; 
     		PositionOfPreCheck = 0;
    			
    		State_Init();        

        	SoftMotionFlag = false;
        	count_SoftMotion = 0;	

            SoftStopMotionFlag = false;
            count_Soft_StopMotion = 0;

    		flag4AbortEventStop = 0;
    		count4AbortEventStop = 0;
    		firstExecute4AbortEventStop = 0;    	
    	}	

    	if(GetBit(AmcCmd->Cmd, 2))
    	{
    		AxisCmdDo(AmcCmd, 0); 
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 3))
    	{
    		AxisCmdDo(AmcCmd, 1); 
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 4))
    	{
    		AxisCmdDo(AmcCmd, 2); 
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 5))
    	{
    		AxisCmdDo(AmcCmd, 3); 
    	}	

    	if(GetBit(AmcCmd->Cmd, 16))   
    	{
    		Follows_MoveDS_Flag = false;
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 17))   
    	{
    		Follows_MoveDS_Flag = true;
    	}		

        if(GetBit(AmcCmd->Cmd, 6))
        {
    		if((AmcCmd->DetectInfo.FollowCommandType == DETECT_INFO_TYPE_ONLY_UBG) || (AmcCmd->DetectInfo.FollowCommandType == DETECT_INFO_TYPE_BOTH))
    		{			
    			if((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE))
    			{
    				DetectOHT = (unsigned char)(AmcCmd->DetectInfo.Type & 0x0f);
    				DetectOBS = (unsigned char)((AmcCmd->DetectInfo.Type>>4) & 0x0f);

    		        switch(DetectOBS)
    		        {
    		            case OBS_DETECT_CMD_PATH_DIRECTION_NO_USE :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NO_USE;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_NORMAL :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_LEFT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_BOTH_VALUE;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_RIGHT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_LEFT_VALUE;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_SHORT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RIGHT_VALUE;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_BOTH :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED1;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_NARROW :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED2;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED1 :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED3;
    		                break;
    						
    		            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED2 :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED4;
    		                break;
    						
    		            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED3 :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED5;
    		                break;
    						
    		            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED4 :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED6;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_N_LEFT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED7;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_N_RIGHT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED8;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED9;
    		                break;

    		            case OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED10;
    		                break;						
    						
    		            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED5 :
    		                tmp_OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RESERVED11;
    		                break;					
    		        }
    					
    		        switch(DetectOHT)
    		        {
    		            case OHT_DETECT_CMD_PATH_DIRECTION_NO_USE :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NO_USE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_NORMAL :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NORMAL_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_LEFT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_LEFT_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_RIGHT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RIGHT_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_SHORT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_SHORT_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_BOTH :
    		                OHT_Detect_AreaSetting = OHT_DETECT_BOTH_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_NARROW :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NARROW_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL :
    		                OHT_Detect_AreaSetting = OHT_DETECT_HILL_VALUE;
    		                break;
    						
    		            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED2 :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RESERVED2;
    		                break;
    						
    		            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED3 :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RESERVED3;
    		                break;
    						
    		            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED4 :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RESERVED4;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NL_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NR_VALUE;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_DOUBLE_LEFT;
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_DOUBLE_RIGHT;
    		                break;						
    						
    		            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED5 :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RESERVED5;
    		                break;					
    		        }
    			}
    			else
    			{
    				switch(AmcCmd->DetectInfo.Type)
    		        {
    		            case OHT_DETECT_CMD_PATH_DIRECTION_NO_USE :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NO_USE;
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_NORMAL :
    		                OHT_Detect_AreaSetting = OHT_DETECT_NORMAL_VALUE;
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_LEFT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_LEFT_VALUE;            
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_RIGHT :
    		                OHT_Detect_AreaSetting = OHT_DETECT_RIGHT_VALUE;            
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_SHORT : 
    		                OHT_Detect_AreaSetting = OHT_DETECT_SHORT_VALUE;            
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_BOTH : 
    		                OHT_Detect_AreaSetting = OHT_DETECT_BOTH_VALUE;            
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_NARROW :  
    		                OHT_Detect_AreaSetting = OHT_DETECT_NARROW_VALUE;            
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT :  
    		                OHT_Detect_AreaSetting = OHT_DETECT_NL_VALUE;            
    		                break;	       					
    	                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT : 
    		                OHT_Detect_AreaSetting = OHT_DETECT_NR_VALUE;            
    		                break;

    		            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT : 
    		                OHT_Detect_AreaSetting = OHT_DETECT_DOUBLE_LEFT;            
    		                break;
    		                
    		            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT :  
    		                OHT_Detect_AreaSetting = OHT_DETECT_DOUBLE_RIGHT;            
    		                break;	                
    		        }			
    			}			
    	        
    	       	OHT_Detect_ChangeNodeValue = AmcCmd->DetectInfo.NodeCount;

    			if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
    			{  
    				if( AmcCmd->DetectInfo.Type == OHT_DETECT_CMD_PATH_DIRECTION_NORMAL)
    				{			
    					OBS_Distance = AmcCmd->DetectInfo.DistSumOfStraightPath; 
    				}
    			}
    			else
    			{
    				if(( AmcCmd->DetectInfo.Type == OHT_DETECT_CMD_PATH_DIRECTION_NORMAL)||( AmcCmd->DetectInfo.Type == OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL))
    				{			
    					OBS_Distance = AmcCmd->DetectInfo.DistSumOfStraightPath; 
    				}
    			}

    			Follows_MoveDS_Flag = AmcCmd->DetectInfo.UseFollowControlFlag; 	
    			
    			if(Follows_MoveDS_Flag == true)
    			{
    				OHT_Detect_AreaSetting_Flag = true;
    			}
    			else
    			{			
    				if(OHT_Detect_AreaSetting == OHT_DETECT_NO_USE)
    				{
    					OHT_Detect_AreaSetting_Flag2 = true;			
    				}
    				else
    				{
    					OHT_Detect_AreaSetting_Flag2 = false;							
    				}
    				
    				OHT_Detect_AreaSetting_Flag = false;			
    			}							
    		}
    		
    		if((AmcCmd->DetectInfo.FollowCommandType == DETECT_INFO_TYPE_ONLY_PBS) || (AmcCmd->DetectInfo.FollowCommandType == DETECT_INFO_TYPE_BOTH))
    		{
    			OHT_PBS_IgnoreChangeNodeValue = AmcCmd->DetectInfo.NodeCount;
    			OHT_PBS_IgnoreFlag = AmcCmd->DetectInfo.PBSIgnoreFlag;
    			OHT_PBS_IgnoreSettingFlag = true;					
    		}			
    		
        }

    	if(GetBit(AmcCmd->Cmd, 7))    // Change Gear Ratio
    	{
    		if(TorqueLimit_Request == 1)
    		{
    			TorqueLimit_Request = 0;
    			TorqueLimit_Response = 10;			
    		}
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 8))    // Set Limit Of Dcc
    	{
    		if(TorqueLimit_Request == 2)
    		{
    			TorqueLimit_Request = 0;
    			TorqueLimit_Response = 20;			
    		}		
    	}

    	if(GetBit(AmcCmd->Cmd, 9))   
    	{
    	}

    	if(GetBit(AmcCmd->Cmd, 10))   
    	{
    		PauseBy = AmcCmd->PauseInfo.Commander;
    		PauseFlag = AmcCmd->PauseInfo.Pause;
    	}

    	if(GetBit(AmcCmd->Cmd, 11))   
    	{
    		Path_Slope_Status = 1;
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 12))   
    	{
    		Path_Slope_Status = 0;
    	}

    	if(GetBit(AmcCmd->Cmd, 13))   
    	{
    		Final_Arrival_Flag = 1;
    	}
    	
    	if(GetBit(AmcCmd->Cmd, 14))   
    	{
    		Final_Arrival_Flag = 0;
    	}	

    	if(GetBit(AmcCmd->Cmd, 15))   
    	{
    		if((NodeCount == 0) && ((pCBuf[0]->jtpos/Model_PULSETOMM) <= forcedNodeCountDistLimit))
    		{
    			NodeCount = 1;  
       
    			if(FlagFirstNode == 1)
    			{
    				PreCheckCount = 1;
    				FlagFirstNode = 0;             
       			}
    		}
    	}
    }

	

	// AMC LibData
	for(ax=0; ax<AMC_AXIS; ax++)
	{
        Amc->AmpEnable[ax] = !pCBuf[ax]->g_on;
        Amc->FollowingError[ax] = (int)pCBuf[ax]->error_z0;
        Amc->InPosition[ax] = pCBuf[ax]->in_pos_flag;
        Amc->Position[ax] = pCBuf[ax]->jtpos;
        Amc->Running[ax] = _dsp_in_motion(ax);
        Amc->Source[ax] = pCBuf[ax]->axis_source;
        Amc->Velocity[ax] = pCBuf[ax]->velfbz0*1000*5;
        Amc->State[ax] = pCBuf[ax]->event_st;
		Amc->PositionOfSmallAddCheck[ax] = JtPos_During_Marking[ax];
        Amc->TargetVel[ax] = pCBuf[ax]->dtBox;
        Amc->TargetPos[ax] = Motion_Flag[ax];
        if(ax==1)
        	Amc->TargetPos[ax] = FollowsDCC_Prev;
        Amc->FinalPos[ax] = pCBuf[ax]->dProfFinalPos;
        if(ax==1)
            Amc->AxisFlag1[1] = 0;
        else
            Amc->AxisFlag1[ax] = currentSpeedMode[ax];
        
        Amc->AxisFlag2[ax] = 0;
	}
	
	if(Model_OHT_Service)
	{
		//Amc->AxisFlag2[0] = LitzCableErrorStatus;
		//Amc->AxisFlag2[1] = LitzCableWarningStatus;
		Amc->AxisFlag2[0] = 0x00;
		for(int i = (sizeof(Amc->AxisFlag2[0]) * 8); i > 0; i--)
		{
			Amc->AxisFlag2[0] |= LitzCableStatus[i-1] << (i-1);
			// Status 주기통신 전달 후 AMC 자체 Clear
			if(LitzCableStatus[i-1] == true)
			{
				LitzCableStatus[i-1] = false;
				LitzCableStatusClear[i-1] = false;
			}
		}
	}
	
	Amc->AxisFlag2[2] = Follows_MoveDS_Flag;
	Amc->AxisFlag2[3] = TorqueLimit_Request;

	Amc->Output[0] = pGBuf->user_io_out;
	Amc->Output[1] = pGBuf->user_io_out2;
	Amc->Output[2] = pGBuf->user_io_out3;	
	Amc->Output[3] = pGBuf->user_io_out4;		

	Amc->VoltageMonitor[0] = VCC_Moniter;
	Amc->VoltageMonitor[1] = VCC_Moniter_2;
	
	Amc->Input[0] = IO_Input[0];
	Amc->Input[1] = IO_Input[1];
	Amc->Input[2] = IO_Input[2];	
	Amc->Input[3] = IO_Input[3];		

	Amc->PauseState = AMC_Follow_Control_Status;
	
	// Follows Data
	Amc->NodeCheckData.NodeCount = NodeCount;
	Amc->NodeCheckData.NodeCountOfPreCheck = PreCheckCount;
	
	Amc->NodeCheckData.PositionOfDrivingTag = PositionOfDrivingTag;
	Amc->NodeCheckData.PositionOfTransTagFirst = PositionOfTransTagFirst;
	Amc->NodeCheckData.PositionOfTransTagFinal = PositionOfTransTagFinal;
	Amc->NodeCheckData.PositionOfDrivingTagPreCheck =  PositionOfPreCheck;

    Amc->AxisFlag1[1] = crc16_ccitt((char *)Amc, sizeof(AMC_DATA));
 
	*MessageLength = sizeof(AMC_DATA);		
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: State_Init
// Brief	: 최초 초기화
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void State_Init(void)
{
	int i, DrivingSensorValue;	

	NodeCountState[0].SensorNum = Model_DRIVINGFRONTNUM;	
	NodeCountState[1].SensorNum = Model_TRANSFRONT;		
	NodeCountState[2].SensorNum = Model_TRANSREAR;		
	NodeCountState[3].SensorNum = Model_DRIVINGPRECHECK;	

	for(i=0; i<MONITORING_SENSOR_NUM; i++)
	{
		if((i==0) || (i==3))
		{			
			DrivingSensorValue = GetBit(IO_Input[1], NodeCountState[i].SensorNum - 32);
			
			if(DrivingSensorValue == 0)
			{
				NodeCountState[i].LowerLimit = GetPos() - (Model_PULSETOMM*100);
				NodeCountState[i].UpperLimit = GetPos() + (Model_PULSETOMM*Model_NODE_COUNT_IGNORE_UPPER_LIMIT_INIT);
			}				
			else
			{
				NodeCountState[i].LowerLimit = GetPos() - (Model_PULSETOMM*5);
				NodeCountState[i].UpperLimit = GetPos() + (Model_PULSETOMM*5);
			}				
		}			
		else
		{
			NodeCountState[i].LowerLimit = GetPos() - (Model_PULSETOMM*5);
			NodeCountState[i].UpperLimit = GetPos() + (Model_PULSETOMM*5);
		}			
	
		NodeCountState[i].OldSensorValue = GetBit(IO_Input[1], NodeCountState[i].SensorNum - 32);	
		NodeCountState[i].State	= 0;
		
		NodeCountState[i].SensorStatus = !NodeCountState[i].OldSensorValue;	
		NodeCountState[i].LastEdgePnt = GetPos();
	}

	FlagFirstNode = 1;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: State_WaitEdge
// Brief	: 각 센서의 엣지 검출을 기다린다, StateNum : 0
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void State_WaitEdge(int SensorNum)
{
	int CurrentSensorValue, CurrentPos;
        
	CurrentSensorValue = GetBit(IO_Input[1], NodeCountState[SensorNum].SensorNum - 32);
	
	CurrentPos = GetPos();

	if(NodeCountState[SensorNum].OldSensorValue != CurrentSensorValue) 
	{
		if(CurrentSensorValue == 0)                     
		{
			if(!((NodeCountState[SensorNum].LowerLimit < CurrentPos) && (CurrentPos < NodeCountState[SensorNum].UpperLimit)))   // 이전에 지정해둔 리밋에 있지 않으면
			{
				NodeCountState[SensorNum].EdgePos = CurrentPos;
				NodeCountState[SensorNum].KeepEdgeCounter = 0;
				NodeCountState[SensorNum].State = 1;    
			}
		}
    }
    
	NodeCountState[SensorNum].OldSensorValue = CurrentSensorValue;
	NodeCountState[SensorNum].SensorStatus = !NodeCountState[SensorNum].OldSensorValue;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: State_KeepEdge
// Brief	: 검출 된 엣지, StateNum : 1
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void State_KeepEdge(int SensorNum)
{
	int CurrentSensorValue;
	
    CurrentSensorValue = GetBit(IO_Input[1], NodeCountState[SensorNum].SensorNum - 32);

	if(NodeCountState[SensorNum].OldSensorValue == CurrentSensorValue)          						// 계속 유지 된다면
	{
		// 적당한 선에서 Saturation 시켜준다.
		if(NodeCountState[SensorNum].KeepEdgeCounter < (Model_FILTER_4_NODECNT*2))
		{
			NodeCountState[SensorNum].KeepEdgeCounter++;				        						// 카운트를 늘려주고
		}

		if(NodeCountState[SensorNum].KeepEdgeCounter >= Model_FILTER_4_NODECNT)	        						// 유지되어 특정카운트까지 간다면
		{
			NodeCountState[SensorNum].LowerLimit = (-100*Model_PULSETOMM) + NodeCountState[SensorNum].EdgePos;  // 앞쪽 리밋은 언제나 같음.
			
			if(NodeCountState[SensorNum].SensorNum == Model_DRIVINGFRONTNUM)	        						// 엣지로 검출되 었을때, 리밋 조정하고, 노드카운트 업데이트
			{
				NodeCountState[SensorNum].UpperLimit = (Model_NODE_COUNT_IGNORE_UPPER_LIMIT*Model_PULSETOMM) + NodeCountState[SensorNum].EdgePos;
				PositionOfDrivingTag = NodeCountState[SensorNum].EdgePos;
				
				if(NodeCountState[SensorNum].LastEdgePnt < NodeCountState[SensorNum].EdgePos)			// 노드 카운트 업데이트 뒤로 가는 경우도 고려함.
			    {
					NodeCount++;
				}
				
				if(FlagFirstNode == 1)
				{
					if(NodeCount == 1)
					{
						PreCheckCount = 1;
					}
					FlagFirstNode = 0;
				}

				NodeCountState[SensorNum].LastEdgePnt = NodeCountState[SensorNum].EdgePos;				// 6.  마지막 포인트 업데이트.
			}
			else if(NodeCountState[SensorNum].SensorNum == Model_TRANSFRONT)                               	// 엣지로 검출되 었을때, 리밋 조정하고, 노드카운트 업데이트
			{
				NodeCountState[SensorNum].UpperLimit = (280 * Model_PULSETOMM) + NodeCountState[SensorNum].EdgePos;				
				PositionOfTransTagFirst = NodeCountState[SensorNum].EdgePos;

				NodeCountState[SensorNum].LastEdgePnt = NodeCountState[SensorNum].EdgePos;				// 6.  마지막 포인트 업데이트.
			}
			else if(NodeCountState[SensorNum].SensorNum == Model_TRANSREAR)                                 	// 엣지로 검출된 후속조치.
			{
				NodeCountState[SensorNum].UpperLimit = (105 * Model_PULSETOMM) + NodeCountState[SensorNum].EdgePos;		
				PositionOfTransTagFinal = NodeCountState[SensorNum].EdgePos;
				
				NodeCountState[SensorNum].LastEdgePnt = NodeCountState[SensorNum].EdgePos;				// 6.  마지막 포인트 업데이트.
			}
			else if(NodeCountState[SensorNum].SensorNum == Model_DRIVINGPRECHECK)                           	// 엣지로 검출된 후속조치.
			{
				NodeCountState[SensorNum].UpperLimit = (Model_NODE_COUNT_IGNORE_UPPER_LIMIT * Model_PULSETOMM) + NodeCountState[SensorNum].EdgePos;		
				PositionOfPreCheck = NodeCountState[SensorNum].EdgePos;

				if(NodeCountState[SensorNum].LastEdgePnt < NodeCountState[SensorNum].EdgePos)		    //  5. 노드 카운트 업데이트 뒤로 가는 경우도 고려함.
				{
					PreCheckCount++;
                }
					
				if(FlagFirstNode == 1)
				{
					if(PreCheckCount == -1 )
				    {
						NodeCount = -2;
					}
					FlagFirstNode = 0;
				}
			}
			
			NodeCountState[SensorNum].SensorStatus = 1;     // Sensor On
			NodeCountState[SensorNum].State = 0;            // 7. Trans To State_WaitEdge
		}
	}
	else
	{
        NodeCountState[SensorNum].KeepEdgeCounter = 0;
		NodeCountState[SensorNum].SensorStatus = 0;	        // Sensor Off
		NodeCountState[SensorNum].State = 0;                // Trans To State_WaitEdge
	}

	NodeCountState[SensorNum].OldSensorValue = CurrentSensorValue;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: Check_Rear_servo_Off
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Check_Rear_servo_Off(void)
{
	int ServoState1=false, ServoState2=false;
	
	if((_dsp_axis_done(AXIS_F_DRIVING) == TRUE) && (_dsp_axis_done(AXIS_R_DRIVING) == TRUE))
	{
		if(++AMC_Stop_count >= Model_AXIS_STOP_COUNT_LIMIT)
		{
			AMC_Stop_count = 0;								
			
			if(Path_Slope_Status == 0)
			{
				_dsp_get_amp_enable(AXIS_R_DRIVING, &ServoState1);		
			
				if(ServoState1 == TRUE)
				{
					_dsp_set_amp_enable(AXIS_R_DRIVING, FALSE);
				}

				_dsp_get_amp_enable(AXIS_R_DRIVING, &ServoState2);		
			
				if(ServoState2 == TRUE)
				{
					_dsp_set_amp_enable(AXIS_R_DRIVING, FALSE);
				}
				
				TorqueLimit_Request = 0;
				Servo_Off_type = 0;
			}
			else
			{
				TorqueLimit_Request = 1;
				Servo_Off_type = 1;
			}
		
			Rear_servo_Off_flag = TRUE;
		}
	}
	else
	{
		AMC_Stop_count = 0;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: Check_Front_Rear_servo_On
// Brief	: 
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Check_Front_Rear_servo_On(void)
{
	int ServoStateF = FALSE;
	int ServoStateR = FALSE;	

	_dsp_get_amp_enable(AXIS_F_DRIVING, &ServoStateF);		
	
	if(ServoStateF == FALSE)
	{
		_dsp_set_amp_enable(AXIS_F_DRIVING, TRUE);
	}

	_dsp_get_amp_enable(AXIS_R_DRIVING, &ServoStateR);		
	
	if(ServoStateR == FALSE)
	{
		_dsp_set_amp_enable(AXIS_R_DRIVING, TRUE);
	}

	AMC_Stop_count = 0;	
	Rear_servo_Off_flag = FALSE;	
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function	: Follows1ms
// Brief	: 1ms 처리  Sequence 
//            1. 노드카운트 처리 부분
//            2. 와치독 처리부분
// Input	:
// Return	:
// History	:
// Date		:
////////////////////////////////////////////////////////////////////////
void Follows1ms(void)
{
	static int Init = 0;
	int ax, i, Io2_out = 0;
	int	ServoStateR1=FALSE, ServoStateR2=FALSE;
	double cruise_speed = 0.0;
    double cruise_speed_margin = 0.0;
    double tmpCurrentVelocity = 0.0;

	Debug_Flag = 0;
	MoveCondition = 0x0000;
    
	if(Model_OHT_Service)	// PatrolVHL 확인 변수
	{
		for(i = 0; i < PatrolVHLIOCnt; i++)
		{
			if((LitzCableStatusClear[i] == false) && (PreLitzCableStatus[i] == 1) && (CalBit(IO_Input, LitzCableIO[i], 1) == 0))
			{
				LitzCableStatusClear[i] = true;
				LitzCableStatus[i] = true;
			}
		}
		for(i = 0; i < PatrolVHLIOCnt; i++)
		{
			PreLitzCableStatus[i] = CalBit(IO_Input, LitzCableIO[i], 1);
		}
	}
    
    if((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE))
    {
        CurrentSpeed4NodeFilter = (double)((pCBuf[0]->velfbz0 * 5)/113.4);
        
        if(CurrentSpeed4NodeFilter < 2.0)
        {
            Model_FILTER_4_NODECNT = 5;
        }
        else if(CurrentSpeed4NodeFilter <= 3.3)
        {
            Model_FILTER_4_NODECNT = 3;
        }
        else
        {
            Model_FILTER_4_NODECNT = 2;
        }
    }        
		
	if(Init == 0)
	{
		State_Init();
		Init = 1;
	}
	NodeCountState[0].SensorNum = Model_DRIVINGFRONTNUM;	
	NodeCountState[1].SensorNum = Model_TRANSFRONT;		
	NodeCountState[2].SensorNum = Model_TRANSREAR;		
	NodeCountState[3].SensorNum = Model_DRIVINGPRECHECK;

	for(i=0 ; i<MONITORING_SENSOR_NUM ; i++)
	{
		switch(NodeCountState[i].State)
		{
			case 0 :
				State_WaitEdge(i);
    			break;

			case 1 :
				State_KeepEdge(i);
	    		break;
		}
    }

    if(((NodeCount >= OHT_Detect_ChangeNodeValue) && (OHT_Detect_AreaSetting_Flag == true)) || (OHT_Detect_AreaSetting_Flag2 == true))
	{
		if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
		{
			if(Model_OBS_SENSOR_MAKER == HOKUYO)
			{			
				if(OHT_Detect_AreaSetting == OHT_DETECT_NORMAL_VALUE)
				{
					OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;
				}			
				else if(OHT_Detect_AreaSetting == OHT_DETECT_LEFT_VALUE)
				{
					OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_LEFT_VALUE;
				}			
				else if(OHT_Detect_AreaSetting == OHT_DETECT_RIGHT_VALUE)
				{
					OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RIGHT_VALUE;
				}			
				else
				{			
					OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_BOTH_VALUE;
				}				
			}			
			else
			{
				if(OHT_Detect_AreaSetting == OHT_DETECT_NORMAL_VALUE)
				{
					OBS_Detect_AreaSetting = SICK_OBS_DETECT_NORMAL_VALUE;
				}			
				else if(OHT_Detect_AreaSetting == OHT_DETECT_LEFT_VALUE)
				{
					OBS_Detect_AreaSetting = SICK_OBS_DETECT_LEFT_VALUE;
				}			
				else if(OHT_Detect_AreaSetting == OHT_DETECT_RIGHT_VALUE)
				{
					OBS_Detect_AreaSetting = SICK_OBS_DETECT_RIGHT_VALUE;
				}			
				else
				{			
					OBS_Detect_AreaSetting = SICK_OBS_DETECT_BOTH_VALUE;
				}				
			}
		}
		else
		{
			OBS_Detect_AreaSetting = tmp_OBS_Detect_AreaSetting;
		}	

		if((OHT_Detect_AreaSetting == OHT_DETECT_NR_VALUE) || (OHT_Detect_AreaSetting == OHT_DETECT_NL_VALUE))
		{
			OHT_DETECT_N_Distance = pCBuf[0]->jtpos;
			OHT_DETECT_N_VALUE_Flag = true;
		}

		OHT_Detect_AreaSetting_For_Curve = OHT_Detect_AreaSetting;        
		OHT_Detect_AreaSetting_Flag = false;
		OHT_Detect_AreaSetting_Flag2 = false;
	
        if((Model_OHT_ID == NEO_SSD_TRAY) || (Model_OHT_ID == OHT_NAME_MODULETRAY))
        {
            Io2_out = ((pGBuf->user_io_out2 & (~Model_OBS_DETECT_WRITE_BIT_MASK)) | (OBS_Detect_AreaSetting<<Model_OBS_DETECT_WRITE_BIT_SCALE));
        }
        else 
        {
            Io2_out = ((pGBuf->user_io_out2 & (~Model_OHT_DETECT_WRITE_BIT_MASK)) | (OHT_Detect_AreaSetting_For_Curve<<Model_OHT_DETECT_WRITE_BIT_SCALE));
            Io2_out = ((Io2_out & (~Model_OBS_DETECT_WRITE_BIT_MASK)) | (OBS_Detect_AreaSetting<<Model_OBS_DETECT_WRITE_BIT_SCALE));
        }        

       	__SetIoOutData_256(pGBuf->user_io_out, Io2_out, pGBuf->user_io_out3, pGBuf->user_io_out4, pGBuf->user_io_out5, pGBuf->user_io_out6, pGBuf->user_io_out7, pGBuf->user_io_out8);		       		        
	}
	
 	if((OHT_DETECT_N_VALUE_Flag == true) && ((abs(pCBuf[0]->jtpos - OHT_DETECT_N_Distance)/Model_PULSETOMM) >= OHT_DETECT_N_CHANGE_OFFST))
	{
		OHT_DETECT_N_VALUE_Flag = false;
		
        Io2_out = ((pGBuf->user_io_out2 & (~Model_OHT_DETECT_WRITE_BIT_MASK)) | (OHT_DETECT_SHORT_VALUE<<Model_OHT_DETECT_WRITE_BIT_SCALE));
        
        OHT_Detect_AreaSetting_For_Curve = OHT_DETECT_SHORT_VALUE;
        
        __SetIoOutData_256(pGBuf->user_io_out, Io2_out, pGBuf->user_io_out3, pGBuf->user_io_out4, pGBuf->user_io_out5, pGBuf->user_io_out6, pGBuf->user_io_out7, pGBuf->user_io_out8);		        
	}

    if((NodeCount >= OHT_PBS_IgnoreChangeNodeValue) && OHT_PBS_IgnoreSettingFlag)
	{
		PBS_Ignore_Flag = OHT_PBS_IgnoreFlag;
		OHT_PBS_IgnoreSettingFlag = false;
	}

	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{
		if(gIsrSt[ax].bHaltEvent)
		{
			Debug_Flag = 1;
			return;
		}
	}

	for(ax=0 ; ax<JPC_AXIS ; ax++)
	{		
		if(Marking_Process_Flag[ax] == true)
		{
			if(IoNumber_For_Marking[ax] < 32)
			{
				if((GetBit(IO_Input[0], IoNumber_For_Marking[ax]) == IoType_For_Marking[ax]) && (FlagIO_For_Marking[ax] != IoType_For_Marking[ax]))
				{
					JtPos_During_Marking[ax] = (pCBuf[ax]->jtpos==0 ? 1 : pCBuf[ax]->jtpos);			
					Marking_Process_Flag[ax] = false;			
					Marking_Process_Trigger_Flag[ax] = true;
				}
				else
				{
					JtPos_During_Marking[ax] = 0;
				}		
				FlagIO_For_Marking[ax] = GetBit(IO_Input[0], IoNumber_For_Marking[ax]);
			}
			else
			{
				if((GetBit(IO_Input[1], IoNumber_For_Marking[ax]-32) == IoType_For_Marking[ax]) && (FlagIO_For_Marking[ax] != IoType_For_Marking[ax]))
				{
					JtPos_During_Marking[ax] = (pCBuf[ax]->jtpos==0 ? 1 : pCBuf[ax]->jtpos);			
					Marking_Process_Flag[ax] = false;						
					Marking_Process_Trigger_Flag[ax] = true;					
				}
				else
				{
					JtPos_During_Marking[ax] = 0;
				}		
				
				FlagIO_For_Marking[ax] = GetBit(IO_Input[1], IoNumber_For_Marking[ax]-32);
			}

			if(Marking_Process_Flag[ax] == true)
			{
				if(DirectionFlag_For_Marking[ax] == true)
				{					
					if(((double)(pCBuf[ax]->dtBox * FREQ) < ((Target_Velocity_by_Sensor*90)/100)) || (!Follows_MoveDS_Flag))
					{						
						POS_For_Marking[ax] = pCBuf[ax]->jtpos + DistanceAddition_For_Marking[ax];
					}						
					
					POS_For_Marking_Final[ax] = pCBuf[ax]->jtpos + DistanceAddition_For_Marking[ax];						

					if(POS_For_Marking[ax] <= pCBuf[ax]->dPosCmdRaw)
					{
						POS_For_Marking[ax] = pCBuf[ax]->dPosCmdRaw;
					}						
					
					if(DistanceLimit_For_Marking[ax] <= POS_For_Marking[ax])
					{				
						POS_For_Marking[ax] = DistanceLimit_For_Marking[ax];
						Marking_Process_Flag[ax] = false;
					}

					if(POS_For_Marking_Final[ax] <= pCBuf[ax]->dPosCmdRaw)
					{
						POS_For_Marking_Final[ax] = pCBuf[ax]->dPosCmdRaw;
					}						
					
					if(DistanceLimit_For_Marking[ax] <= POS_For_Marking_Final[ax])
					{				
						POS_For_Marking_Final[ax] = DistanceLimit_For_Marking[ax];
						Marking_Process_Flag[ax] = false;
					}					
				}
				else
				{
					POS_For_Marking[ax] = pCBuf[ax]->jtpos - DistanceAddition_For_Marking[ax];

					if(POS_For_Marking[ax] >= pCBuf[ax]->dPosCmdRaw)
					{
						POS_For_Marking[ax] = pCBuf[ax]->dPosCmdRaw;
					}						
					
					if(DistanceLimit_For_Marking[ax] >= POS_For_Marking[ax])
					{				
						POS_For_Marking[ax] = DistanceLimit_For_Marking[ax];
						Marking_Process_Flag[ax] = false;
					}
				}

				if(!Follows_MoveDS_Flag)
				{
					MoveDS_Flag = 9;	
					
					_dsp_move_ds(ax, ACC_For_Marking[ax], DCC_For_Marking[ax], VEL_For_Marking[ax], POS_For_Marking[ax]);
				}
				else
				{
					if(ax==0)
					{					
						FollowsACC = ACC_For_Marking[ax];
						FollowsDCC = DCC_For_Marking[ax];

						FollowsDCC_CMD = FollowsDCC;

						FollowsVEL = VEL_For_Marking[ax];
						FollowsPOS = POS_For_Marking[ax];
					}					
					else
					{					
						MoveDS_Flag = 10;	
						
						_dsp_move_ds(ax, ACC_For_Marking[ax], DCC_For_Marking[ax], VEL_For_Marking[ax], POS_For_Marking[ax]);
					}					
				}					
			}				
		}		
	}		

	if(!Follows_MoveDS_Flag)   
	{
		Debug_Flag = 2;
        return;
	}

	if(Rear_servo_Off_flag == FALSE)
	{		
		Check_Rear_servo_Off();
	}		
	
    Target_Velocity_by_Sensor = Get_Velocity_by_Area();
    
    tmpCurrentVelocity = (double)(((pCBuf[0]->dtBox*FREQ)*3.5)/VELOCITY_3_5_MPS);

	if(PauseFlag)
	{
		if(PauseBy == PAUSED_BY_OCS)
		{				
			AMC_Pause_Control = true;
		}				
		else if(PauseBy == PAUSED_BY_TP)
		{
			if((AMC_Follow_Control_Status == STATUS_PBS_STOP) || (AMC_Follow_Control_Status == STATUS_OBS_STOP))
			{					
				AMC_Pause_Control = true;
			}	
			else if(AMC_Pause_Control != true)
			{
				PauseFlag = PAUSED_NONE;
			}
		}
	}			
	else
	{
		AMC_Pause_Control = false;		
	}    

	if(Target_Velocity_by_Sensor != Model_SPEED_GRADE_8_VELOCITY)
	{
		if(Path_Slope_Status == 1)
		{
	    	FollowsDCC = Model_FOLLOWSDCC;	//SPEED_3_0_MPS2;
	    	Model_MOVE_S_DCC_BY_SENSOR_NORMAL = Model_FOLLOWSDCC;	//SPEED_3_0_MPS2;
	    }
	    else
	    {
	    	FollowsDCC = Model_FOLLOWSDCC;    	    
	    	Model_MOVE_S_DCC_BY_SENSOR_NORMAL = Model_FOLLOWSDCC;
	    }
        
		if(((double)(pCBuf[0]->dtBox*FREQ)) < ((Model_SPEED_GRADE_5_VELOCITY*90)/100))		
		{			
			if((double)(pCBuf[0]->dtBox*FREQ) < ((Target_Velocity_by_Sensor*90)/100))
			{			
				FollowsDCC = FollowsDCC_CMD;				
			}				
		}					
	}		    

	// Dynamic ACC
	if((currentSpeedMode[0] == ACC_MODE) && (tmpCurrentVelocity > ACC_CHANGE_SPEED3))
	{
		tmpFollowsACC = DYNAMIC_ACC_4th;
	}
	else if((currentSpeedMode[0] == ACC_MODE)
	&& ((tmpCurrentVelocity > ACC_CHANGE_SPEED2)
	&& (tmpCurrentVelocity <= ACC_CHANGE_SPEED3)))
	{
		tmpFollowsACC = DYNAMIC_ACC_3th;
	}
	else if((currentSpeedMode[0] == ACC_MODE) 
	&& ((tmpCurrentVelocity > ACC_CHANGE_SPEED1) 
	&& (tmpCurrentVelocity <= ACC_CHANGE_SPEED2)))
	{
		tmpFollowsACC = DYNAMIC_ACC_2nd;
	}
	else 
	{
		// 전방에 OHT가 없는 경우
		if(Target_Velocity_by_Sensor == Model_SPEED_GRADE_8_VELOCITY)
			tmpFollowsACC = FollowsACC;
		// 전방에 OHT가 있는 경우
		else
			tmpFollowsACC = DYNAMIC_ACC_1st;
	}

	//근접제어용 가속도 계산
	if(FollowsVEL >= Target_Velocity_by_Sensor)
	{
		Target_velocity_cmd = Target_Velocity_by_Sensor;
        if(Target_Velocity_by_Sensor <= Model_SPEED_GRADE_3_VELOCITY)
        {
            tmpFollowsACC = (double)(Model_MOVE_DS_ACC_BY_SENSOR_CURVE/2);
        }
    }
    else    	    
    {
		Target_velocity_cmd = FollowsVEL;
    }    
	
    if(Target_velocity_cmd == Model_SPEED_GRADE_1_VELOCITY)
    {
    	Debug_Flag = 20;

    	SoftMotionFlag = false;
        count_SoftMotion = 0;

        SoftStopMotionFlag = false;
        count_Soft_StopMotion = 0;
        
	    if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
	    {
            if(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)
    		{
                if((FollowsVEL_Prev != Target_velocity_cmd) || (FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_NORMAL))
                {
    				MoveS_Flag = 5;            
    				//MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
    				MoveCondition += (((FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_NORMAL) & 0x01) * 16);
    				MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
    				//MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

    				if(_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_NORMAL)==MMC_OK)
    				{            
    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
    	                FollowsVEL_Prev = Target_velocity_cmd;	
    	                
               			followsMoveSFailCnt = 0;			
    				}				
    				else
    				{    	  
    					followsMoveSFailCnt++;
     					
    					if(followsMoveSFailCnt>3)
    					{
    						forcedMoveStopCommand = true;
    						_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_NORMAL);
    						forcedMoveStopCommand = false;
    						
	    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
    		                FollowsVEL_Prev = Target_velocity_cmd;			    						    						
    		                
    		                followsMoveSFailCnt = 0;
    					}
    				}					
                }
                else
                {
                	Debug_Flag = 6;
                }
    		}
    		else
    		{
                if((FollowsVEL_Prev != Target_velocity_cmd) || (FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_CURVE))
                {
       				MoveS_Flag = 6;            
       				//MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
       				MoveCondition += (((FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_CURVE) & 0x01) * 16);
       				MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
       				//MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

                    if(_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_CURVE) == MMC_OK)
    				{
    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_CURVE;
    	                FollowsVEL_Prev = Target_velocity_cmd;				
    	                
    	                followsMoveSFailCnt = 0;
    				}					
    				else
    				{    	  
    					followsMoveSFailCnt++;
     					
    					if(followsMoveSFailCnt>3)
    					{
    						forcedMoveStopCommand = true;
    						_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_CURVE);
    						forcedMoveStopCommand = false;
    						
	    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_CURVE;
    		                FollowsVEL_Prev = Target_velocity_cmd;			    						    						
    		                
    		                followsMoveSFailCnt = 0;
    					}
    				}				
                }
                else
                {
                	Debug_Flag = 7;
                }
    		}
        }
        else
        {
            if((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE))
    		{
                if((FollowsVEL_Prev != Target_velocity_cmd) || (FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_NORMAL))
                {
    				MoveS_Flag = 7;
    				//MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
    				MoveCondition += (((FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_NORMAL) & 0x01) * 16);
    				MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
    				//MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);
    				
                    if(_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_NORMAL)==MMC_OK)
    				{            
    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
    	                FollowsVEL_Prev = Target_velocity_cmd;		
    	                
						followsMoveSFailCnt = 0;    	                				
    				}					
    				else
    				{    	  
    					followsMoveSFailCnt++;
     					
    					if(followsMoveSFailCnt>3)
    					{
    						forcedMoveStopCommand = true;
    						_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_NORMAL);
    						forcedMoveStopCommand = false;
    						
	    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
    		                FollowsVEL_Prev = Target_velocity_cmd;			    						    						
    		                
    		                followsMoveSFailCnt = 0;
    					}
    				}
                }
                else
                {
                	Debug_Flag = 8;
                }
    		}
    		else
    		{
                if((FollowsVEL_Prev != Target_velocity_cmd) || (FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_CURVE))
                {
       				MoveS_Flag = 8;            
       				//MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
       				MoveCondition += (((FollowsDCC_Prev != Model_MOVE_S_DCC_BY_SENSOR_CURVE) & 0x01) * 16);
       				MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
       				//MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

                    if(_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_CURVE) == MMC_OK)
    				{
    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_CURVE;
    	                FollowsVEL_Prev = Target_velocity_cmd;				
    	                
    	                followsMoveSFailCnt = 0;
    				}					
    				else
    				{    	  
    					followsMoveSFailCnt++;
     					
    					if(followsMoveSFailCnt>3)
    					{
    						forcedMoveStopCommand = true;
    						_dsp_move_s(0, Model_MOVE_S_DCC_BY_SENSOR_CURVE);
    						forcedMoveStopCommand = false;
    						
	    	                FollowsDCC_Prev = Model_MOVE_S_DCC_BY_SENSOR_CURVE;
    		                FollowsVEL_Prev = Target_velocity_cmd;			    						    						
    		                
    		                followsMoveSFailCnt = 0;
    					}
    				}
                }
                else
                {
                	Debug_Flag = 9;
                }
    		}
        }
	}
	else
	{
		Debug_Flag = 30;

		followsMoveSFailCnt = 0;
		
		if(Rear_servo_Off_flag)
		{
			if(Servo_Off_type == 1)
			{
				if(TorqueLimit_Response == 20)
				{
					Check_Front_Rear_servo_On();
					
					MoveDS_Wait_Count_After_Servo_On = 0;					
					MoveDS_Wait_Count_After_Servo_On_Flag = TRUE;
			
					Rear_servo_Off_flag = false;
					
					Servo_Off_type = 0;
					TorqueLimit_Response = 0;

					_dsp_get_amp_enable(AXIS_R_DRIVING, &ServoStateR1);	
					
					if(ServoStateR1 == FALSE)
					{
						Check_Front_Rear_servo_On();
					}					
				}
				else
				{
					Debug_Flag = 3;				
	            	return;
				}
			}
			else
			{
				Check_Front_Rear_servo_On();
				
				Servo_Off_type = 0;
				
				MoveDS_Wait_Count_After_Servo_On = 0;					
				MoveDS_Wait_Count_After_Servo_On_Flag = TRUE;
				
				Rear_servo_Off_flag = false;

				_dsp_get_amp_enable(AXIS_R_DRIVING, &ServoStateR2);		
				
				if(ServoStateR2 == FALSE)
				{
					Check_Front_Rear_servo_On();
				}				
			}
		}

		if(MoveDS_Wait_Count_After_Servo_On_Flag)
		{
			if(MoveDS_Wait_Count_After_Servo_On <= Model_MOVE_DS_RESTART_COUNT_LIMIT_AFTER_SERVO_ON)
			{
				MoveDS_Wait_Count_After_Servo_On++;			
				AMC_Stop_count = 0;	
				Debug_Flag = 4;								
				return;
			}
			else
			{
				AMC_Stop_count = 0;	
				MoveDS_Wait_Count_After_Servo_On_Flag = FALSE;
			}
		}
		
#if EN_CRUISE_MODE		
		cruise_speed = (double)(pCBuf[0]->dtBox*FREQ);
        cruise_speed_margin = cruise_speed * 1.03;	
        
		if(((((double)(Target_velocity_cmd - cruise_speed)*3.5)/VELOCITY_3_5_MPS) > 0.5) && (cruise_speed > 0))
        {     
// 근접제어로 감속 중 가속하는 경우 -> 1sec 등속 모드 실행
#if 1
			//if((SoftMotionFlag == false) && (cruise_speed_margin >= FollowsVEL_Prev))
            if((SoftMotionFlag == false) && (cruise_speed_margin >= Pre_Target_Velocity_by_Sensor))
            {
                if((currentSpeedMode[0] == DCC_MODE) || ((currentSpeedMode[0] == CRUISE_MODE) && (cruise_mode_cnt < Cruise_Mode_Offset) && (cruise_mode_set != true)))
#else
// 주행 중 감속 중 가속하는 경우, 등속 약 400msec 중 가속하는 경우 -> 1sec 등속 모드 실행
            if(SoftMotionFlag == false)
            {
                if((currentSpeedMode[0] == DCC_MODE) || ((currentSpeedMode[0] == CRUISE_MODE) && (cruise_mode_cnt < Cruise_Mode_Offset) && (cruise_mode_set != true)))
#endif
                {
    	            if((Follow_Enable_Flag) && ((FollowsACC_Prev != tmpFollowsACC) || (FollowsDCC_Prev != FollowsDCC) || (FollowsVEL_Prev != Target_velocity_cmd) || (FollowsPOS_Prev != FollowsPOS)))
    	            {
    	                if(currentSpeedMode[0] == CRUISE_MODE)
    	            	{
                            // 200us 단위 mm 단위로 변환
    	            		nCruiseTime =  (Cruise_Mode_Offset - cruise_mode_cnt)/5;	
    	            	}
    	            	else
    	            	{
    	            		nCruiseTime = Cruise_Mode_Offset/5;
    	            	}
    	            	
    	            	if(nCruiseTime > 0)
    	            	{
    					    MEnqueue(MAxisQueue[0], 6);  
    					    MEnqueue(MAxisQueue[0], 0);
    					    MEnqueue(MAxisQueue[0], tmpFollowsACC);  
    					    MEnqueue(MAxisQueue[0], FollowsDCC);  
    					    MEnqueue(MAxisQueue[0], Target_velocity_cmd); 				
    					    MEnqueue(MAxisQueue[0], cruise_speed); 
    					    MEnqueue(MAxisQueue[0], currentSpeedMode[0]);
    					    MEnqueue(MAxisQueue[0], FollowsPOS);  
    					    MEnqueue(MAxisQueue[0], Count_200us_INT_4_Debug);
    					    MEnqueue(MAxisQueue[0], cruise_mode_cnt);
    					    MEnqueue(MAxisQueue[0], cruise_count[0]);
    					    MEnqueue(MAxisQueue[0], acc_count[0]);  
    					    MEnqueue(MAxisQueue[0], dcc_count[0]);  
    					    MEnqueue(MAxisQueue[0], Pre_Target_Velocity_by_Sensor);  
    					    MEnqueue(MAxisQueue[0], nCruiseTime);  
    					    MEnqueue(MAxisQueue[0], 0);  
    					    MEnqueue(MAxisQueue[0], 0);  
    					    MEnqueue(MAxisQueue[0], 0);		   
    	                    MEnqueue(MAxisQueue[0], 0);	
    					    MEnqueue(MAxisQueue[0], 0);    	                    
    	
    	                   	MoveDS_Flag = 15;				
    	                   	MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
    	                   	MoveCondition += (((FollowsDCC_Prev != FollowsDCC) & 0x01) * 16);
    	                   	MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
    	                   	MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

    			            if(_dsp_move_ds(0, tmpFollowsACC, FollowsDCC, cruise_speed, FollowsPOS) == MMC_OK)
    						{
    				            SoftMotionFlag = true;			            
    				            FollowsACC_Prev = tmpFollowsACC;
    				            FollowsDCC_Prev = FollowsDCC;
    				            FollowsVEL_Prev = cruise_speed;
    				            FollowsPOS_Prev = FollowsPOS;	
    	                        cruise_mode_set = true;
    				        }
    				        else
    				        {
    		                	SoftMotionFlag = false;
    				        }
    				        count_SoftMotion = 0;	
    					}
    					else
    					{
    						SoftMotionFlag = false;
    						count_SoftMotion = 0;
    						cruise_mode_set = true;
    					}		        
    			    }
    	            else
    	            {
    	            	Debug_Flag = 15;
    	            }
    	        }   
            }
        }
        else
        {
        	SoftMotionFlag = false;
        	count_SoftMotion = 0;
        }

        //Pre_Target_Velocity_by_Sensor = Target_Velocity_by_Sensor;

        if(SoftMotionFlag == true)
        {
            count_SoftMotion++;
            if(count_SoftMotion >= nCruiseTime)
            {
                SoftMotionFlag = false;
                cruise_mode_cnt = 0;
            }
			Debug_Flag = 5;								            
            return;
        }

        // check speed limit
        if((SoftStopMotionFlag == false) && (currentSpeedMode[0] == STOP_MODE) && (stop_mode_cnt < Stop_Mode_Offset) && (Pre_Target_Velocity_by_Sensor == Model_SPEED_GRADE_1_VELOCITY))
        {
            // 200us -> 1ms
            nStopTime =  (Stop_Mode_Offset - stop_mode_cnt)/5;
            if(nStopTime > 0)
            {
                SoftStopMotionFlag = true;
                count_Soft_StopMotion = 0;  
            }
            else
            {
                SoftStopMotionFlag = false; 
            }
        }

        // excute speed limit
        if(SoftStopMotionFlag)
        {
            count_Soft_StopMotion++;
            if(count_Soft_StopMotion < nStopTime)
            {
                // limit speed
                Target_velocity_cmd = VELOCITY_0_2_MPS;     
            }
            else
            {
                count_Soft_StopMotion = 0;
                SoftStopMotionFlag = false;
            }
            
        }
#endif        

        if(!((Model_OHT_ID == OHT_STD_01) || (Model_OHT_ID == OHT_NAME_STD_V80_SERVICE)))	
	    { 
            if((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE) || (Marking_Process_Flag[0] == true))
        	{
        	    if(Final_Arrival_Flag == 1)
        		{
        	        if(((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) < Model_DIST_ARRIVED_LEVEL1) && ((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) >= Model_DIST_ARRIVED_LEVEL2))
        	        {
        	        	if(Target_velocity_cmd >= Model_SPEED_GRADE_5_VELOCITY)
        				{
        	       			Target_velocity_cmd = Model_SPEED_GRADE_5_VELOCITY;            		
        	   			}
        	        }            	
        	        else if((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) < Model_DIST_ARRIVED_LEVEL2)
        	        {
        	        	if(Target_velocity_cmd >= Model_SPEED_GRADE_4_VELOCITY)
        	        	{
        	       			Target_velocity_cmd = Model_SPEED_GRADE_4_VELOCITY;            		
        	   			}
        	        }   		
        		}				
        		
                if((Follow_Enable_Flag) && ((FollowsACC_Prev != tmpFollowsACC) || (FollowsDCC_Prev != FollowsDCC) || (FollowsVEL_Prev != Target_velocity_cmd) || (FollowsPOS_Prev != FollowsPOS)))
                {
                	int tempAcc, tempDcc, tempVel, tempPos;
                	tempAcc = tmpFollowsACC;
                	tempDcc = FollowsDCC;
                	tempVel = Target_velocity_cmd;
                	tempPos = FollowsPOS;
                	
                    MoveDS_Flag = 11;				
                    MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
                    MoveCondition += (((FollowsDCC_Prev != FollowsDCC) & 0x01) * 16);
                    MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
                    MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

                    if(_dsp_move_ds(0, tmpFollowsACC, FollowsDCC, Target_velocity_cmd, FollowsPOS)==MMC_OK)
        			{
        	            FollowsACC_Prev = tempAcc;
        	            FollowsDCC_Prev = tempDcc;
            	        FollowsVEL_Prev = tempVel;
        	            FollowsPOS_Prev = tempPos;				
        			}					
                }
                else
                {
                	Debug_Flag = 11;
                }
        	}
        	else
        	{
                if((Follow_Enable_Flag) && ((FollowsACC_Prev != Model_MOVE_DS_ACC_BY_SENSOR_CURVE) || (FollowsDCC_Prev != Model_MOVE_DS_DCC_BY_SENSOR_CURVE) || (FollowsVEL_Prev != Target_velocity_cmd) || (FollowsPOS_Prev != FollowsPOS)))
                {
                	int tempAcc, tempDcc, tempVel, tempPos;
                	tempAcc = Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
                	tempDcc = Model_MOVE_DS_DCC_BY_SENSOR_CURVE;
                	tempVel = Target_velocity_cmd;
                	tempPos = FollowsPOS;
                	
                    MoveDS_Flag = 12;
                    MoveCondition = ((FollowsACC_Prev != Model_MOVE_DS_ACC_BY_SENSOR_CURVE) & 0x01);
                    MoveCondition += (((FollowsDCC_Prev != Model_MOVE_DS_DCC_BY_SENSOR_CURVE) & 0x01) * 16);
                    MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
                    MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);
                    
                    if(_dsp_move_ds(0, Model_MOVE_DS_ACC_BY_SENSOR_CURVE, Model_MOVE_DS_DCC_BY_SENSOR_CURVE, Target_velocity_cmd, FollowsPOS)==MMC_OK)			
        		    {
        	            FollowsACC_Prev = tempAcc;
        	            FollowsDCC_Prev = tempDcc;
        	            FollowsVEL_Prev = tempVel;
        	            FollowsPOS_Prev = tempPos;				
        			}					
                }
                else
                {
                	Debug_Flag = 12;
                }
        	}
         }
         else
         {
            if((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)
            		|| (OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)
					|| (Marking_Process_Flag[0] == true))
            {
                if(Final_Arrival_Flag == 1)
            	{
            	    if(((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) < Model_DIST_ARRIVED_LEVEL1) && ((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) >= Model_DIST_ARRIVED_LEVEL2))
            	    {
           	            if(Target_velocity_cmd >= Model_SPEED_GRADE_5_VELOCITY)
    				    {
            	       	    Target_velocity_cmd = Model_SPEED_GRADE_5_VELOCITY;            		
            	   		}
            	    }            	
            	    else if((abs(FollowsPOS - pCBuf[0]->jtpos)/Model_PULSETOMM) < Model_DIST_ARRIVED_LEVEL2)
            	    {
            	        if(Target_velocity_cmd >= Model_SPEED_GRADE_4_VELOCITY)
            	        {
            	       	    Target_velocity_cmd = Model_SPEED_GRADE_4_VELOCITY;            		
            	   		}
            	    }   		
            	}				
            		
                if((Follow_Enable_Flag) && ((FollowsACC_Prev != tmpFollowsACC) || (FollowsDCC_Prev != FollowsDCC) || (FollowsVEL_Prev != Target_velocity_cmd) || (FollowsPOS_Prev != FollowsPOS)))
                {
                	int tempAcc, tempDcc, tempVel, tempPos;
                	tempAcc = tmpFollowsACC;
                	tempDcc = FollowsDCC;
                	tempVel = Target_velocity_cmd;
                	tempPos = FollowsPOS;
                	
                    MoveDS_Flag = 13;		
                    MoveCondition = ((FollowsACC_Prev != tmpFollowsACC) & 0x01);
                    MoveCondition += (((FollowsDCC_Prev != FollowsDCC) & 0x01) * 16);
                    MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
                    MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

                    if(_dsp_move_ds(0, tmpFollowsACC, FollowsDCC, Target_velocity_cmd, FollowsPOS)==MMC_OK)
            	    {
            	        FollowsACC_Prev = tempAcc;
    	                FollowsDCC_Prev = tempDcc;
        	            FollowsVEL_Prev = tempVel;
    	                FollowsPOS_Prev = tempPos;				
    				}			   				
                }
                else
                {
                	Debug_Flag = 13;
                }
    		}
    		else
    		{
                if((Follow_Enable_Flag) && ((FollowsACC_Prev != Model_MOVE_DS_ACC_BY_SENSOR_CURVE) || (FollowsDCC_Prev != Model_MOVE_DS_DCC_BY_SENSOR_CURVE) || (FollowsVEL_Prev != Target_velocity_cmd) || (FollowsPOS_Prev != FollowsPOS)))
                {
                	int tempAcc, tempDcc, tempVel, tempPos;
                	tempAcc = Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
                	tempDcc = Model_MOVE_DS_DCC_BY_SENSOR_CURVE;
                	tempVel = Target_velocity_cmd;
                	tempPos = FollowsPOS;
                	
                	MoveDS_Flag = 14;			
                	MoveCondition = ((FollowsACC_Prev != Model_MOVE_DS_ACC_BY_SENSOR_CURVE) & 0x01);
                	MoveCondition += (((FollowsDCC_Prev != Model_MOVE_DS_DCC_BY_SENSOR_CURVE) & 0x01) * 16);
                	MoveCondition += (((FollowsVEL_Prev != Target_velocity_cmd) & 0x01) * 16 * 16);
                	MoveCondition += (((FollowsPOS_Prev != FollowsPOS) & 0x01) * 16 * 16 * 16);

                    if(_dsp_move_ds(0, Model_MOVE_DS_ACC_BY_SENSOR_CURVE, Model_MOVE_DS_DCC_BY_SENSOR_CURVE, Target_velocity_cmd, FollowsPOS)==MMC_OK)			
    				{
    	                FollowsACC_Prev = tempAcc;
    	                FollowsDCC_Prev = tempDcc;
    	                FollowsVEL_Prev = tempVel;
    	                FollowsPOS_Prev = tempPos;				
    				}
                }
                else
                {
                	Debug_Flag = 14;
                }
    		}
        }
    }
    Pre_Target_Velocity_by_Sensor = Target_Velocity_by_Sensor;
}

