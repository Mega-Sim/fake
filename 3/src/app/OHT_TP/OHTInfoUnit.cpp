//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <IniFiles.hpp>

#pragma hdrstop

#include "Def_TP.h"
#include "OHTInfoUnit.h"
#include "MainUnit.h"
#include "Define.h"
#include "ConnectUnit.h"


TOHTCONNECTINFO *gm_OHTConnectInfo;
TOHTSTATUS* gm_OHTStatus;
//---------------------------------------------------------------------------
TOHTCONNECTINFO::TOHTCONNECTINFO()
{
   int i;
   for(i=0;i<1000;i++)
   {
      strOHTName[i]="";
      strOHTIP[i]="";
   }
   nCount=0;

   // added by sanghwa // 110207
   g_strLineName = "[Warning] Unknown Line";
   g_iLineColor = 0x000000FF;
   g_strTileName = "OHTLP5";
}
//---------------------------------------------------------------------------
TOHTCONNECTINFO::~TOHTCONNECTINFO()
{

}
//---------------------------------------------------------------------------
/**
@brief   OHT_STATUS�� ������
@author  puting
@date    2012.12.13
@param   OHT�� ��� Data�� ����(new)
*/
TOHTSTATUS::TOHTSTATUS()
{
   Init();
   IOInput=new TOHT_INPUT;
   IONumInput=new TOHT_INPUT;
   IOOutput=new TOHT_OUTPUT;

   Motor_Driving_F = new TOHT_MOTOR;
   Motor_Driving_R = new TOHT_MOTOR;
   Motor_Hoist = new TOHT_MOTOR;
   Motor_Shift = new TOHT_MOTOR;
   Motor_Rotate = new TOHT_MOTOR;

   CheckOnOff=new TOHT_CHECKON;
   CmdControl=new TOHT_CMDCONTROL;
   HIDData = new TOHT_HIDDATA;

   VIBData = new TOHT_VIBDATA;
}
//---------------------------------------------------------------------------
TOHTSTATUS::~TOHTSTATUS()
{
   delete IOInput;
   IOInput=NULL;

   delete IONumInput;
   IONumInput=NULL;

   delete IOOutput;
   IOOutput=NULL;

   delete Motor_Driving_F;
   Motor_Driving_F=NULL;

   delete Motor_Driving_R;
   Motor_Driving_R=NULL;

   delete Motor_Hoist;
   Motor_Hoist=NULL;

   delete Motor_Shift;
   Motor_Shift=NULL;

   delete Motor_Rotate;
   Motor_Rotate=NULL;

   delete CheckOnOff;
   CheckOnOff=NULL;

   delete CmdControl;
   CmdControl=NULL;

   delete HIDData;
   HIDData=NULL;

   delete VIBData;
   VIBData=NULL;
}
//---------------------------------------------------------------------------
/**
@brief   OHT_STATUS �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   OHT Status�� �ʱ�ȭ
*/
void TOHTSTATUS::Init()
{
   nSndCMDID = 0;
   nTPCommandType = 0;

   m_OHTStatus.Data.CurNodeID = 0;

   nCmdCount = 0;
   nTransCount = 0;

   nDetectLevel = 0;

   // by zzang9un 2012.11.26 : Mode, Status ���� Ÿ�� ����
   m_OHTStatus.Data.MSCmdID.Info.Mode = MODE_MANUAL;
   m_OHTStatus.Data.MSCmdID.Info.Status = STS_INIT;
   
   PreStatus = STS_INIT;

   strErrorString="";
   strPMACRespose="";
   strMapDist="";
   strPMACResponse="NODATA";
   strDownPercent="0";
   bReceiveStatus=true;
   nTransLongMode=TRMODE_INIT;
   nProgram_Mode=0;
   strNetworkDrive = STR_NETWORKDRIVE;
   strLogworkDrive = STR_LOGDRIVE;

   strObsDist="0";
   strDetectDist="0";
   g_nIsOHTBoot=FROMHDD;
   strOHTVer="";
   strOHTType="";
   strLineInfo="[Warning] Unknown Line";
   strOHTNum="";
   nSTBDire=-1;
   nIsRotateExist=-1;
   //nIsRFPIORS23=-1;
//   nIsRFPIO6Digit=-1;
   nIsRFPIOType=-1;

   bSendMapData=false;

   bIs_Hand_Origin=false;
   bIs_Shutter_Origin=false;
   bIs_Steering_Origin=false;
   bIs_Rotate_Origin=false;

   strLogSaveMode="0";
   strLogSavePath = STR_PATH_LOG;
   strAPSignal = "0";
   nIs_DirectLogSave=1;

   nIsUBGSenUse=1;
   nUBGDetectLevel=0;
   nIsPauseSet=0;

}
//---------------------------------------------------------------------------
TOHT_HIDDATA::TOHT_HIDDATA()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_HIDDATA::~TOHT_HIDDATA()
{

}
//---------------------------------------------------------------------------
/**
@brief   HID Data �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   HID Data�� �ʱ�ȭ
*/
void TOHT_HIDDATA::Init()
{
   nErec=0;
   nEdc=0;
   nIdc=0;
   nKw=0;

   nErrBit=0;
   strErrString="000";
   nFuseOpen=0;
   nOverTemperature=0;
   nEdcOverVoltage=0;
   nEdcUnderVoltage=0;
   nIdcOverCurrent=0;
   nErecOverVoltage=0;
   nErecUnderVoltage=0;
   nErrStorage       = 0;
   nOverOutput_KW    = 0;

   strTemperature[0]="0.0";
   strTemperature[1]="0.0";

}

TOHT_VIBDATA::TOHT_VIBDATA()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_VIBDATA::~TOHT_VIBDATA()
{

}

void TOHT_VIBDATA::Init()
{
   x = y = z = 0;
}

//---------------------------------------------------------------------------
TOHT_CMDCONTROL::TOHT_CMDCONTROL()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_CMDCONTROL::~TOHT_CMDCONTROL()
{
}
//---------------------------------------------------------------------------
void TOHT_CMDCONTROL::Init()
{
//   bSendCMD=false;
//   nSendCMDID=0;
}

//---------------------------------------------------------------------------
TOHT_INPUT::TOHT_INPUT()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_INPUT::~TOHT_INPUT()
{
}
//---------------------------------------------------------------------------
/**
@brief   OHT_INPUT �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   OHT�� Input Data�� �ʱ�ȭ
*/
void TOHT_INPUT::Init()
{
    for(int i=0; i<3; i++)
    {
        nAMCInput[i]=0;
	}

}
//---------------------------------------------------------------------------
TOHT_OUTPUT::TOHT_OUTPUT()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_OUTPUT::~TOHT_OUTPUT()
{
}
//---------------------------------------------------------------------------
/**
@brief   OHT_OUTPUT �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   OHT�� Output Data�� �ʱ�ȭ
*/
void TOHT_OUTPUT::Init()
{
     for(int i=0; i<3; i++){
        nAMCOutput[i]=0;
   }

}
//---------------------------------------------------------------------------
TOHT_MOTOR::TOHT_MOTOR()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_MOTOR::~TOHT_MOTOR()
{
}
//---------------------------------------------------------------------------
/**
@brief   OHT_MOTOR Info �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   OHT Motor Info�� �ʱ�ȭ
*/
void TOHT_MOTOR::Init()
{
   dPosition_cts=0.0;
   dPosition_mm=0.0;
   dVelocity_ctspermsec=0.0;
   dVelocity_mpersec=0.0;
   dVelocity_rpm=0.0;
   nError=0;
   nErrorCode=0;
   nAxis_Running=0;  //1:running��, 0:����
   bIs_ServoON=false;
   bIs_Origin=false; //rotate��(coolmuscle)�� ���
   nAxis_state_code=0;
   nAxis_source_code=0;
}
//---------------------------------------------------------------------------
TOHT_CHECKON::TOHT_CHECKON()
{
   Init();
}
//---------------------------------------------------------------------------
TOHT_CHECKON::~TOHT_CHECKON()
{
}
//---------------------------------------------------------------------------
/**
@brief   OHT_USE_DATA �ʱ�ȭ
@author  puting
@date    2012.12.13
@param   OHT Use Data�� �ʱ�ȭ
*/
void TOHT_CHECKON::Init()
{

   Usedata.Data = 0x00000000;

   bUse_BCR=false;
   bUse_PBSSensor=false;
   bUse_SoundAlarm=false;
   bUse_LampIndi=false;
   bUse_ShutterControl=false;
   bUse_EQPIO=false;

   bUse_STBSensor=false;
   bUse_HandLimitSensorCheck=false;
   bUse_HandControl=false;
   bUse_SteeringControl=false;
   bUse_UTBSensor=false;
   bUse_LookDownSensor=false;
   bUse_HandPushSensorCheck=false;
   bUse_GripperSen=false;

   bUse_HoistAlignSen=false;
   bUse_OscillationSen=false;
   bUse_ErrSound=false;
   bUse_DrivingSound=false;
   bUse_TransSound=false;
   bUse_ObsDetectSound=false;
   bUse_OhtDetectSound=false;
   bUse_ObsIgnoreSound=false;
   bUse_IgnorePIOError=false; //�������� pio���� �߻��� �����ϰ� ������ �Ϸ�
   bUse_HoistHomePos_PIOError=false; //pio���� �߻��� 2����� ��� ���� �߻�..
   bUse_NextMotion_34Axis=false; //Next command ��ɹ޾����� 3,4���� ���� �������� ���� �Ǵ�..
   bUse_LocSave_Trans_ing=false; //�������� Hand Push�� off�϶� ������ ƼĪ���� �������� ����
   bUse_PushAbnormalOnCheck=false;  //�������� ƼĪ ��ġ �̿��� �������� Hand Push Sensor �� On �Ǵ� ���� Ȯ������ ����
   bUse_UTBFoupCheck=false; //UTB Unload �� Hand�� ƼĪ ��ġ���� �������� Hand Close�� �Ͽ� Foup�� �ִ����� Ȯ������ ����
   bUse_InnerFoupDetectSen=false; //���� Foup �������� ��뿩��
   bUse_TransEQSound=false;
   bUse_TransSTKSound=false;
   bUse_TransSTBSound=false;
}
//---------------------------------------------------------------------------
void TOHTCONNECTINFO::ConnectStart()
{
   int nError=0, i;
   String strTmpFile;
   String strTmpPath;

   strTmpPath = ExtractFilePath(Application->ExeName)+"Remote.ini";
   nError = FileExistCheck(strTmpPath,strTmpFile);
   if(!nError)
   {
      strTmpFile = ExtractFilePath(Application->ExeName)+strTmpFile;
      nCount=0;
	  nError = RemoteFileCheck(strTmpFile);

      if(nError)
      {
         ConnectForm->ConnectName->Items->Add("No File");
      }
	  else
      {
		 ConnectForm->ConnectName->Items->Clear();
		 for(i=0;i<nCount;i++)
		 {
			ConnectForm->ConnectName->Items->Add(strOHTName[i]);
		 }
	  }
   }
   else
   {
	  /*
	  ������ ������ ������ no File�θ� ǥ��������, �ڵ����� ����Ʈ ������ �����ϵ��� ������
	  ConnectForm->ConnectName->Items->Add("No File");
	  */

	  MakeRemoteIniFile(strTmpPath);
	  nError = RemoteFileCheck(strTmpPath);

	  ConnectForm->ConnectName->Items->Clear();
	  for(i=0;i<nCount;i++)
	  {
		  ConnectForm->ConnectName->Items->Add(strOHTName[i]);
	  }
   }

   ConnectForm->ConnectName->ItemIndex = 0;
   nIndex = ConnectForm->ConnectName->ItemIndex;
   ConnectForm->ConnectIP->Text = strOHTIP[0];

   // display name and color for someplace(line)
   MainForm->panLineName->Caption = g_strLineName;
   MainForm->panLineName->Color = (TColor)g_iLineColor;
}

void TOHTCONNECTINFO::MakeRemoteIniFile(AnsiString strIniFilePath)
{
/*
	TIniFile* m_IniFile;
	m_IniFile = new TIniFile(strIniFilePath);

	m_IniFile->WriteString("TARGET","MainUpdate"	,"N");

	delete m_IniFile;    */

	FILE *stream;

	try
	{
		stream = fopen(strIniFilePath.c_str(), "w+");

		fprintf(stream,"%s", "LineName=[Warning] Unknown Line\n");
		fprintf(stream,"%s", "LineColor=4259584\n");
		fprintf(stream,"%s", "Title=OHT_TP5\n");
		fprintf(stream,"%s", "Local=127.0.0.1\n");
		fprintf(stream,"%s", "OHT01=192.168.100.1\n");

		if(stream)
			fclose(stream);
		stream = NULL;
	}
	catch(...)
	{
		if(stream)
			fclose(stream);
		stream = NULL;
	}
}

//---------------------------------------------------------------------------
int TOHTCONNECTINFO::FileExistCheck(String strTmpPath, String &strTmpFileName)
{
   TSearchRec sr;
   int nFindFlag=0;

   if(FindFirst(strTmpPath, faArchive|faReadOnly|faSysFile|faHidden, sr)==0)
   {
    //���丮�� �����ϰ� ���ϸ��� �˻��Ѵ�
      if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
      {
      //�˻��� ������ �����̸�
         if(!((sr.Name == ".") || (sr.Name == "..")))
         {
            strTmpFileName = sr.Name;
            RemoteFileName=strTmpFileName;
            FindClose(sr);
            return 0;
         }
      }
      while(FindNext(sr) == 0)
      {
         if(sr.Attr == faArchive||faReadOnly||faSysFile||faHidden)
         {
            if((sr.Name == ".") || (sr.Name == "..")) continue;
            strTmpFileName = sr.Name;
            RemoteFileName=strTmpFileName;
            nFindFlag=1;
            break;
         }
         nFindFlag=0;
      }
   }
   FindClose(sr);
   if(nFindFlag==1) return NO_ERR;
   else return ERR_FILE_NO_EXIST;

}
//---------------------------------------------------------------------------
int TOHTCONNECTINFO::RemoteFileCheck(String strTmpFileName)
{
   FILE* fp;
   TCHAR chBuffer[256];
   String strTmpBuffer;
   int Pos=0;
   int nError=0;

   if (strTmpFileName.Length() <= 0) return ERR_FILE_NAME;

   fp = _wfopen(strTmpFileName.w_str(), L"r");
   if (fp == NULL)
   {
      fclose(fp);
	  return ERR_FILE_OPEN;
   }

   while (feof(fp) == false)
   {
      try
      {
         fgetws(chBuffer, 255, fp);
      }
      catch (Exception &e)
      {
         fclose(fp);
		 return ERR_FILE_OPEN;
      }
   }
   fclose(fp);

   fp = _wfopen(strTmpFileName.w_str(), L"r");
   if (fp == NULL)
   {
      fclose(fp);
	  return ERR_FILE_OPEN;
   }
 
   while (feof(fp) == false)
   {
      swprintf(chBuffer, L"");
      try
      {
         fgetws(chBuffer, 255, fp);
      }
      catch (Exception &e)
      {
         fclose(fp);
		 return ERR_FILE_OPEN;;
	  }
      strTmpBuffer = chBuffer;
      strTmpBuffer.TrimRight();
      while ((Pos = strTmpBuffer.Pos(L"\n")) > 0)
         strTmpBuffer.Delete(Pos, 1);
      if (strTmpBuffer.Length() == 0)  continue;

      Pos=strTmpBuffer.Pos("=");
      if(Pos>0)
      {
         String tmpLeftStr  = strTmpBuffer.SubString(1,Pos-1);
         String tmpRightStr = strTmpBuffer.SubString(Pos+1, strTmpBuffer.Length());

         if(tmpLeftStr.Pos(L"LineName") > 0)
         {
            g_strLineName = tmpRightStr;
         }
         else if(tmpLeftStr.Pos(L"LineColor") > 0)
         {
            g_iLineColor = tmpRightStr.ToInt();
         }
         else if(tmpLeftStr.Pos(L"Title") > 0)
         {
            g_strTileName = tmpRightStr;
            Application->Title = g_strTileName;
            MainForm->g_strTitle = Application->Title;
         }
         else
         {
            nCount++;
            strOHTName[nCount-1] = tmpLeftStr;
            strOHTIP[nCount-1]   = tmpRightStr;
         }
      }
   }
   fclose(fp);
   return nError;
}
//---------------------------------------------------------------------------
bool TOHTCONNECTINFO::RemoteFileSave()
{
   String strTmpFileName=RemoteFileName; //�ʱ⿡ �о�Դ� ���� �����..
   int i;
   FILE *fp;

   strTmpFileName = ExtractFilePath(Application->ExeName) + strTmpFileName;
   //if(fp != NULL) fclose(fp);
   fp = _wfopen(strTmpFileName.w_str(), L"w"); //w:���..

   if (fp != NULL)
   {
      fwprintf(fp, L"LineName=%s\n", g_strLineName.w_str());
      fwprintf(fp, L"LineColor=%d\n", g_iLineColor );
      fwprintf(fp, L"Title=%s\n", g_strTileName );

      try
      {
         for(i=0;i<nCount;i++)
         {
            fwprintf(fp, L"%s=%s\n",strOHTName[i],strOHTIP[i]);
         }
         fflush(fp);
      }
      catch (Exception &e)
      {
         fclose(fp);
         fp = NULL;
         return false;
      }
      fclose(fp);
      return true;
   }
   fclose(fp);
   return false;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
