//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Def_TP.h"
//---------------------------------------------------------------------------
// -------------------------------------------
// ���� ���� ���ڿ� ��ȯ �Լ�
// -------------------------------------------

/**
@brief   Mode ������ String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2013.01.22
@param   mode : Mode
@return  Mode�� ��ȯ�� ���ڿ�
*/
String GetStringMode(BYTE Mode)
{
    String strRtnMode = "";

    switch(Mode)
    {
    case MODE_AUTO:
        strRtnMode = "Auto";
        break;

    case MODE_MANUAL:
        strRtnMode = "Manual";
        break;

    default:
        strRtnMode = "Unknown";
        break;
    }

    return strRtnMode;
}

/**
@brief   Status ������ String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2013.01.22
@param   status : Status ��
@return  Status�� ��ȯ�� ���ڿ�
*/
String GetStringStatus(BYTE Status)
{
    String strRtnStatus = "";

    switch(Status)
    {
    case STS_INIT:
        strRtnStatus = "Initialized";
        break;

    case STS_GOING:
        strRtnStatus = "Going";
        break;

    case STS_ARRIVED:
        strRtnStatus = "Arrived";
        break;

    case STS_UNLOADING:
        strRtnStatus = "Unloading";
        break;

    case STS_UNLOADED:
        strRtnStatus = "Unloaded";
        break;

    case STS_LOADING:
        strRtnStatus = "Loading";
        break;

    case STS_LOADED:
        strRtnStatus = "Loaded";
        break;

    case STS_AUTOPOSITION:
        strRtnStatus = "Auto Position";
        break;

    case STS_RUNNING:
        strRtnStatus = "Running";
        break;

    case STS_COMPLETE:
        strRtnStatus = "Complete";
        break;    

    case STS_AUTORECOVERY:
        strRtnStatus = "AutoRecovery";
        break;

    case STS_ERROR:
        strRtnStatus = "Error";
		break;

	case STS_AUTO_TAUGHT:
		strRtnStatus = "Taught";
		break;
        
    default:
        strRtnStatus = "Unknown";
        break;
    }

    return strRtnStatus;
}

/**
@brief   OHTVer ������ String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2013.01.22
@param   OHTVer : OHT Version
@return  OHTVer�� ��ȯ�� ���ڿ�
*/
String GetStringOHTVer(BYTE OHTVer)
{
    String strRtnOHTVer = "";

    switch(OHTVer)
    {
    case OHT_VERSION_47:
        strRtnOHTVer = "Ver 4.7";
        break;

    case OHT_VERSION_60:
        strRtnOHTVer = "Ver 6.0";
        break;

    case OHT_VERSION_65:
        strRtnOHTVer = "Ver 6.5";
        break;

    case OHT_VERSION_70:
        strRtnOHTVer = "Ver 7.0";
		break;

	case OHT_VERSION_71:
		strRtnOHTVer = "Ver 7.1";
		break;

	case OHT_VERSION_72:
		strRtnOHTVer = "Ver 7.2";
		break;
	case OHT_VERSION_80:
		strRtnOHTVer = "Ver 8.0";
		break;
	case OHT_VERSION_81:
		strRtnOHTVer = "Ver 8.1";
		break;
	case OHT_VERSION_85:
		strRtnOHTVer = "Ver 8.5";
		break;
    default:
        strRtnOHTVer = "Unknown";
        break;
    }

    return strRtnOHTVer;
}

/**
@brief   OHT type ������ String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2013.01.22
@param   OHTType : OHT Type
@return  Status�� ��ȯ�� ���ڿ�
*/
String GetStringOHTType(BYTE OHTType)
{
    String strRtnOHTType = "";

    switch(OHTType)
    {
    case OHT_TYPE_FOUP:
        strRtnOHTType = "FOUP";
        break;

    case OHT_TYPE_RETICLE:
        strRtnOHTType = "RETICLE";
        break;

    case OHT_TYPE_EUV:
        strRtnOHTType = "EUV";
        break;

    case OHT_TYPE_FOSB:
        strRtnOHTType = "FOSB";
        break;

    case OHT_TYPE_SERVICE:
        strRtnOHTType = "SERVICE";
        break;

    case OHT_TYPE_MZ0:
        strRtnOHTType = "MZ0";
        break;

    case OHT_TYPE_CS0:
        strRtnOHTType = "CS0";
        break;

    case OHT_TYPE_MZ90:
        strRtnOHTType = "MZ90";
        break;

    case OHT_TYPE_CS90:
        strRtnOHTType = "CS90";
        break;

    case OHT_TYPE_MZRT:
        strRtnOHTType = "MZRT";
        break;

	case OHT_TYPE_CSRT:
		strRtnOHTType = "CSRT";
		break;
	case OHT_TYPE_FOUP_S:
		strRtnOHTType = "FOUP_S";
		break;

    default:
        strRtnOHTType = "Unknown";
        break;
    }

    return strRtnOHTType;
}
//---------------------------------------------------------------------------

/**
@brief   ���ڷε� Port Type�� Byte �ڷ������� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2012.12.06
@param   strPortType : ��ȯ�ؾ� �� ���ڿ�
@return  ��ȯ�� Port Type
*/
BYTE StrToPortType(String strPortType)
{
    if(strPortType == "EQ")
        return PORT_TYPE_EQ;
    else if(strPortType == "EQ_X")
        return PORT_TYPE_EQ_X;
    else if(strPortType == "STOCKER")
        return PORT_TYPE_STOCKER;
    else if(strPortType == "LOADER")
        return PORT_TYPE_LOADER;
    else if(strPortType == "STB_L")
        return PORT_TYPE_STB_L;
    else if(strPortType == "STB_R")
        return PORT_TYPE_STB_R;
    else if(strPortType == "UTB")
        return PORT_TYPE_UTB;
    else
        return 0;
}
//---------------------------------------------------------------------------

/**
@brief   Port Type�� String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2012.12.06
@param   PortType : ��ȯ�ؾ� �� Port Type
@return  ��ȯ�� Port Type String
*/
String PortTypeToStr(BYTE PortType)
{
    String strRtn;

    // Port Type�� String���� ��ȯ
    switch(PortType)
    {
    case PORT_TYPE_EQ:
        strRtn = "EQ";
        break;

    case PORT_TYPE_EQ_X:
        strRtn = "EQ_X";
        break;

    case PORT_TYPE_STOCKER:
        strRtn = "STOCKER";
        break;

    case PORT_TYPE_LOADER:
        strRtn = "LOADER";
        break;

    case PORT_TYPE_STB_L:
        strRtn = "STB_L";
        break;

    case PORT_TYPE_STB_R:
        strRtn = "STB_R";
        break;

    case PORT_TYPE_UTB:
        strRtn = "UTB";
        break;

    default:
        strRtn = "";
        break;
    }

    return strRtn;
}
//---------------------------------------------------------------------------

/**
@brief   Tag Type�� String���� ��ȯ�ϴ� �Լ�
@author  zzang9un
@date    2013.07.23
@param   TagType : ��ȯ�ؾ� �� Tag Type
@return  ��ȯ�� Tag Type String
*/
String TagTypeToStr(BYTE TagType)
{
    String strRtn;

    // Port Type�� String���� ��ȯ
    switch(TagType)
    {
    case TAG_TYPE_NONE:
        strRtn = "Driving";
        break;

    case TAG_TYPE_STOP:
        strRtn = "Stop";
        break;

    case TAG_TYPE_QR_LEFT:
        strRtn = "QR_Left";
        break;

    case TAG_TYPE_QR_RIGHT:
        strRtn = "QR_Right";
        break;

    default:
        strRtn = "";
        break;
    }

    return strRtn;
}


#pragma package(smart_init)
