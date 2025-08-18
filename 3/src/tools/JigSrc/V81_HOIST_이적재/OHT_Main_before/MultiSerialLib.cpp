//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MultiSerialLib.h"
#include "Def_OHT.h"
#include "ErrorCode.h"
#include "OHTMain.h"

#define AMC_MS_INTERRUPT            5
#define AMC_MS_RETRY				5
#define  NAME_SERIAL_CFG            "SerialConfig.cfg"



//---------------------------------------------------------------------------
HINSTANCE MultiSerialLibrary::R_RunTimeLink()
{
//   HINSTANCE g_hSerialLib;
	String strOHTPath = "", strTmpFileName = "";

    strOHTPath = ExtractFileDir(Application->ExeName);
    strTmpFileName = AnsiString(strOHTPath + STR_DATAFILE_PATH) + "amcms2.dll";

    // Multi Serial DLL ���� Load
    g_hSerialLib = LoadLibrary(strTmpFileName.c_str());

   if(g_hSerialLib == NULL)
   {
      Sleep(1000);
	  g_hSerialLib = LoadLibrary(strTmpFileName.c_str());
	  if(g_hSerialLib == NULL) 
		return NULL;	
   }

      DeviceSerialOpen        = (OPENSERIALDEVICE)             GetProcAddress(g_hSerialLib, "amcms_initialize");
      DeviceSerialClose       = (CLOSESERIALDEVICE)            GetProcAddress(g_hSerialLib, "amcms_release");
      DeviceSerialLoadSetting = (LOADSETTINGSERIALDEVICE)      GetProcAddress(g_hSerialLib, "amcms_load_setting");
/*
      DeviceSerialOpen        : Hardware ������ Serial Port Open
      DeviceSerialClose       : Port Close
      DeviceSerialLoadSetting : ��ü Port�� ���� Serial ��� �� setting...
*/

      if(!DeviceSerialOpen || !DeviceSerialClose || !DeviceSerialLoadSetting)
      {
        return NULL;
      }
      else
      {
         return g_hSerialLib;
      }

}
//---------------------------------------------------------------------------
MultiSerialLibrary::MultiSerialLibrary()
{
    Init();
}
//---------------------------------------------------------------------------
/**
@brief   Multi Serial Library�� �ʱ�ȭ�ϴ� �Լ�
@date    2012.12.03
@return  ���� �ڵ�
@warning �α׸� ����ų� ������ �ϳ� �� �������� ���� ��� OHT �ʱ�ȭ�� �ȵ�
@warning ������ ���� �ľ� ����
*/
int MultiSerialLibrary::Init()
{
    int nError = 0;

    g_hSerialLib = NULL;      //handlea
    g_bSerialDriverOpen = false; //device open flag

	if(!g_hSerialLib)
        g_hSerialLib = R_RunTimeLink();
    
    if(g_hSerialLib == NULL) 
        nError = ERR_SERIAL_DLL_INIT;

    if((nError == NO_ERR) && (!g_bSerialDriverOpen))
    {
        nError = SerialSettingFileLoad();

        if(nError == NO_ERR)
        {
			int nRetValue = 0;

			for(int i=0; i < AMC_MS_RETRY; i++)
			{
              	nRetValue = DeviceSerialOpen(AMC_MS_INTERRUPT);     // Serial �� ����� PC interrupt No : 5.. ���� ����...
				if(nRetValue == 0)
				{
				  g_bSerialDriverOpen = true;
				  break;
				}
				else
					g_bSerialDriverOpen = false; // ��� return int�� bool�� ����...

				Sleep(2000);
            }
			///@todo AMC Init ���� Ȯ�� �߰� �ʿ�
            //if((!g_bSerialDriverOpen)&&(gm_OHTStatus->bAMCInitialized==true))
             if(!g_bSerialDriverOpen)
                    nError = ERR_SERIAL_DLL_INIT;
        }
    }

// by zzang9un 2012.12.03 : ������ �� �� ����
#ifdef USE_LOG_MEMO
    if(nError == NO_ERR)
    {
        ADD_MEMO_T("Serial Dll Load... OK!..\r\n");
    }
    else
    {
        ADD_MEMO_T("Serial Dll Load... Fail!..\r\n");
    }
#endif /* USE_LOG_MEMO */    

    return nError;
}
//---------------------------------------------------------------------------
MultiSerialLibrary::~MultiSerialLibrary()
{
   if(g_bSerialDriverOpen)
   {
	  DeviceSerialClose();
   }
   if(g_hSerialLib)
   {
	  FreeLibrary(g_hSerialLib);
      g_hSerialLib=NULL;
   }
   g_bSerialDriverOpen = false;
}
//---------------------------------------------------------------------------
int MultiSerialLibrary::SerialSettingFileLoad(void)
{
   int nError=0;
   AnsiString strTmpFileName = "";
   AnsiString strOHTPath=".";

   strOHTPath=ExtractFileDir(Application->ExeName);
   strTmpFileName = AnsiString(strOHTPath+STR_DATAFILE_PATH) + NAME_SERIAL_CFG;

   if(strTmpFileName.Length() <= 0) return ERR_SERIAL_DLL_FILE_NAME;

   if(!FileExists(strTmpFileName)) return  ERR_SERIAL_DLL_FILE_NO_EXIST;

   DeviceSerialLoadSetting(strTmpFileName.c_str());

   return nError;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
















