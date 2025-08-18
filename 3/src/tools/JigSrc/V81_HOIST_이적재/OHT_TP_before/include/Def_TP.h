/**
@file    Def_TP.h
$Rev:: 735           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-03-22 1#$:  Date of last commit
@version OHT 7.0
@brief   TP ���α׷��� ���Ǵ� ���, �Լ� ����
*/

#ifndef Def_TPH
#define Def_TPH
//---------------------------------------------------------------------------
#include "Def_OHT.h"
#include "Def_Protocol.h"
#include "Def_TPCommand.h"
#include "Def_DataType.h"

#define MODE_KOREAN         1
#define LANGUAGE_MODE       MODE_KOREAN

#define LOG_DELETE_DAY		10			// ������ �α� ���� ���� ��¥

// -------------------------------------------
// �ʱ�ȭ ���� ���� : Splash ǥ�ÿ�
// -------------------------------------------
typedef enum INIT_STATE_
{
    STATE_NONE       = 0,    ///< State Node
    STATE_CONNECTCHECK,      ///< Data Server Connect üũ
    STATE_DATAVERREQUEST,    ///< Data ���� ��û
    STATE_DATAVERCHECK,      ///< Data ���� üũ
    STATE_STATIONSYNC,       ///< Station Map Data �ٿ�ε�
    STATE_STATIONSYNCDONE,   ///< Station Map Data �ٿ�ε� �Ϸ�
    STATE_STATIONSYNCFAIL,   ///< Station Map Data �ٿ�ε� Fail
    STATE_TEACHINGSYNC,      ///< Teaching Data �ٿ�ε�
    STATE_TEACHINGSYNCDONE,  ///< Teaching Data �ٿ�ε� �Ϸ�
    STATE_TEACHINGSYNCFAIL,  ///< Teaching Data �ٿ�ε� Fail
    STATE_DATASYNCCOMPLETE,  ///< Data Sync �Ϸ�� ����
    STATE_DATASYNCFAIL,      ///< Data Sync Fail�� ����  
    STATE_CONNECTFAIL,       ///< Data Sever Connect ����
    STATE_END,               ///< �ʱ�ȭ �۾� ����
    STATE_MAX                ///< State Max
} INIT_STATE;

// -------------------------------------------
// TColor ���� ���� define
// -------------------------------------------
#define COLOR_AQUA          clAqua
#define COLOR_BLACK         clBlack
#define COLOR_BLUE          clBlue
#define COLOR_CREAM         clCream
#define COLOR_DARKGRAY      clDkGray
#define COLOR_FUCHSIA       clFuchsia
#define COLOR_GRAY          clGray
#define COLOR_GREEN         clGreen
#define COLOR_LIME          clLime
#define COLOR_LIGHTGRAY     clLtGray
#define COLOR_MAROON        clMaroon
#define COLOR_MEDIUMGRAY    clMedGray
#define COLOR_MINTGREEN     clMoneyGreen
#define COLOR_NAVYBLUE      clNavy
#define COLOR_OLIVE         clOlive
#define COLOR_PURPLE        clPurple
#define COLOR_RED           clRed
#define COLOR_SILVER        clSilver
#define COLOR_SKYBLUE       clSkyBlue
#define COLOR_WHITE         clWhite
#define COLOR_YELLOW        clYellow

// -------------------------------------------
// ���� ���� Define
// -------------------------------------------
#define STR_NETWORKDRIVE            L"Z:"       ///< ��Ʈ��ũ ����̺� �̸�
#define STR_LOGDRIVE            	L"Y:"


// by zzang9un 2013.09.13 : Data File ����
#define DATAFILE_MAP                0 ///< Map Data 
#define DATAFILE_STATION            1 ///< Station Data
#define DATAFILE_TEACHING           2 ///< Teaching Data
#define DATAFILE_AMCOUTFILE         3 ///< AMC Out File
#define DATAFILE_PARAM_DRIVING      4 ///< Parameter(Driving) Data
#define DATAFILE_PARAM_TRANS        5 ///< Parameter(Trans) Data
#define DATAFILE_NODEDISABLE        6 ///< Node Disable Data
#define DATAFILE_USERSELECT         7 ///< User Select File
#define DATAFILE_ASSISTANT          8 ///< Assistant Program
#define DATAFILE_LOG                9 ///< Log

#define FILENAME_IOMAP L"IOMap.ini"

// -------------------------------------------
// ���� ���� ���ڿ� ��ȯ �Լ�
// -------------------------------------------
String GetStringMode(BYTE Mode);
String GetStringStatus(BYTE Status);
String GetStringOHTVer(BYTE OHTVer);
String GetStringOHTType(BYTE OHTType);
BYTE StrToPortType(String strPortType);
String PortTypeToStr(BYTE PortType);
String TagTypeToStr(BYTE TagType);

//---------------------------------------------------------------------------
#endif
