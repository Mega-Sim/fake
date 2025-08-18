/**
@file    Def_DataFile.h
@brief   Data File�� �������� ����Ǵ� Header ����
*/

#ifndef Def_DataFileH
#define Def_DataFileH

#include <IniFiles.hpp>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

#define FILTER_REVISION     0xF1000000          ///< Revision Filter
#define FILTER_UPDATETIME   0xF200000000000000  ///< UpdateTime Filter
#define FILTER_COUNT        0xF3000000          ///< Data Count Filter
#define MASK_FILTER_32      0xFF000000          ///< Header Fiter ������ ���� Mask(4byte��)
#define MASK_FILTER_64      0xFF00000000000000  ///< Header Fiter ������ ���� Mask(8byte��)
#define MASK_VALUE_32       0x00FFFFFF          ///< 
#define MASK_VALUE_64       0x00FFFFFFFFFFFFFF  ///< 

#define ERR_FILTER_FORMAT_REVISION      -100
#define ERR_FILTER_FORMAT_UPDATETIME    -101
#define ERR_FILTER_FORMAT_COUNT         -102
#define ERR_DATA_SIZE                   -110
#define ERR_FILE_OPEN                   -120
#define ERR_DATA_LIMIT_REVISION         -200
#define ERR_DATA_LIMIT_UPDATETIME       -201
#define ERR_DATA_LIMIT_COUNT            -202
#define NO_ERR                          0
//---------------------------------------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack

/**
@brief   Data File Header ����ü
@author  doori.shin
@date    2013.07.26
@note    ���� �� ��Ʈ�� ���͸� ���� ����ξ�� �Ѵ�.
*/
typedef struct FILE_HEADER_
{
    UINT Revision;      ///< Data Revision
    UINT64 UpdateTime;  ///< File Update Time
    UINT NumOfData;     ///< The Number of Data
}FILE_HEADER;

/**
@brief   Map Data File Header ����ü
@author  zzang9un
@date    2013.07.27
*/
typedef struct MAP_FILE_HEADER_
{
    UINT Revision;      ///< Data Revision
    UINT64 UpdateTime;  ///< File Update Time
    UINT PathNum;       ///< The Number of Data
    UINT NodeNum;       ///< The Number of Data
} MAP_FILE_HEADER;

#pragma pack(pop) // 1byte�� Pack
//---------------------------------------------------------------------------

/**
@brief   ���Ͽ��� ���� Header�� �Ľ��Ͽ� �����ϴ� �Լ�
@author  zzang9un
@date    2013.08.01
@param   pHeader : �˻��� ���� Header ����ü
@return  Parsing�� File Header ����ü�� �����Ѵ�.
*/
FILE_HEADER ParsingFileHeader(FILE_HEADER* pHeader)
{
    FILE_HEADER RtnHeader;
    memset(&RtnHeader, 0x00, sizeof(RtnHeader));
    
    int Error = NO_ERR;

    // ����� �ùٸ��� Ȯ��
    if((pHeader->Revision & MASK_FILTER_32) != FILTER_REVISION) 
        Error = ERR_FILTER_FORMAT_REVISION;

    if((pHeader->UpdateTime & MASK_FILTER_64) != FILTER_UPDATETIME) 
        Error = ERR_FILTER_FORMAT_UPDATETIME;

    if((pHeader->NumOfData & MASK_FILTER_32)!= FILTER_COUNT) 
        Error = ERR_FILTER_FORMAT_REVISION;

    // �Ǿ��� �� ��Ʈ�� 0���� �ٲ�
    if(Error == NO_ERR)
    {
        RtnHeader.Revision = pHeader->Revision & MASK_VALUE_32;    
        RtnHeader.UpdateTime = pHeader->UpdateTime & MASK_VALUE_64;
        RtnHeader.NumOfData = pHeader->NumOfData & MASK_VALUE_32;
    }
    
    return RtnHeader;
}
//---------------------------------------------------------------------------

/**
@brief   Map ���Ͽ��� ���� Header�� �Ľ��Ͽ� �����ϴ� �Լ�
@author  zzang9un
@date    2013.08.01
@param   pHeader : �˻��� ���� Header ����ü
@return  Parsing�� Map File Header ����ü�� �����Ѵ�.
*/
MAP_FILE_HEADER ParsingMapFileHeader(MAP_FILE_HEADER *pHeader)
{
    MAP_FILE_HEADER RtnHeader;
    memset(&RtnHeader, 0x00, sizeof(RtnHeader));
    
    int Error = NO_ERR;

    // ����� �ùٸ��� Ȯ��
    if((pHeader->Revision & MASK_FILTER_32) != FILTER_REVISION) 
        Error = ERR_FILTER_FORMAT_REVISION;

    if((pHeader->UpdateTime & MASK_FILTER_64) != FILTER_UPDATETIME) 
        Error = ERR_FILTER_FORMAT_UPDATETIME;

    if((pHeader->PathNum & MASK_FILTER_32)!= FILTER_COUNT) 
        Error = ERR_FILTER_FORMAT_REVISION;

    if((pHeader->NodeNum & MASK_FILTER_32)!= FILTER_COUNT) 
        Error = ERR_FILTER_FORMAT_REVISION;

    // �Ǿ��� �� ��Ʈ�� 0���� �ٲ�
    if(Error == NO_ERR)
    {
        RtnHeader.Revision = pHeader->Revision & MASK_VALUE_32;    
        RtnHeader.UpdateTime = pHeader->UpdateTime & MASK_VALUE_64;
        RtnHeader.PathNum = pHeader->PathNum & MASK_VALUE_32;
        RtnHeader.NodeNum = pHeader->NodeNum & MASK_VALUE_32;
    }
    
    return RtnHeader;
}
//---------------------------------------------------------------------------

/**
@brief   File�� ������ File Header�� Make���ִ� �Լ�
@author  zzang9un
@date    2013.08.01
@param   pHeader : �������
@return  Make�� FILE_HEADER ����ü
*/
FILE_HEADER MakeFileHeader(FILE_HEADER* pHeader)
{
    int Error = 0;
    
    FILE_HEADER RtnFileHeader;
    memset(&RtnFileHeader, 0x00, sizeof(RtnFileHeader));

    if((pHeader->Revision & MASK_FILTER_32) != 0) 
        Error = ERR_DATA_LIMIT_REVISION;
    
    if((pHeader->UpdateTime & MASK_FILTER_64) != 0) 
        Error = ERR_DATA_LIMIT_UPDATETIME;
    
    if((pHeader->NumOfData & MASK_FILTER_32) != 0)
        Error = ERR_DATA_LIMIT_COUNT;

    if(Error == NO_ERR)
    {
        RtnFileHeader.Revision = pHeader->Revision | FILTER_REVISION;
        RtnFileHeader.UpdateTime = pHeader->UpdateTime | FILTER_UPDATETIME;
        RtnFileHeader.NumOfData = pHeader->NumOfData | FILTER_COUNT;
    }

    return RtnFileHeader;
}
//---------------------------------------------------------------------------

/**
@brief   Map File�� ������ File Header�� Make���ִ� �Լ�
@author  zzang9un
@date    2013.08.01
@param   pHeader : �������
@return  Make�� MAP_FILE_HEADER ����ü
*/
MAP_FILE_HEADER MakeMapFileHeader(MAP_FILE_HEADER *pHeader)
{
    int Error = 0;
    
    MAP_FILE_HEADER RtnFileHeader;
    memset(&RtnFileHeader, 0x00, sizeof(RtnFileHeader));

    if((pHeader->Revision & MASK_FILTER_32) != 0) 
        Error = ERR_DATA_LIMIT_REVISION;
    
    if((pHeader->UpdateTime & MASK_FILTER_64) != 0) 
        Error = ERR_DATA_LIMIT_UPDATETIME;
    
    if((pHeader->PathNum & MASK_FILTER_32) != 0)
        Error = ERR_DATA_LIMIT_COUNT;

    if((pHeader->NodeNum & MASK_FILTER_32) != 0)
        Error = ERR_DATA_LIMIT_COUNT;

    if(Error == NO_ERR)
    {
        RtnFileHeader.Revision = pHeader->Revision | FILTER_REVISION;
        RtnFileHeader.UpdateTime = pHeader->UpdateTime | FILTER_UPDATETIME;
        RtnFileHeader.PathNum = pHeader->PathNum | FILTER_COUNT;
        RtnFileHeader.NodeNum = pHeader->NodeNum | FILTER_COUNT;
    }

    return RtnFileHeader;
}
//---------------------------------------------------------------------------

/**
@brief   File�� Header�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.30
@param   FileName : ���� �̸�(��� ����)
@param   pHeader : FILE_HEADER ����ü ������
@return  �����ڵ�
*/
int GetFileHeader(String FileName, FILE_HEADER *pHeader)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
    
    int Error = NO_ERR;

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName.w_str(), ios::in | ios::binary);

    if(fs.is_open())
    {
        fs.read((char *)pHeader, sizeof(FILE_HEADER));

        *pHeader = ParsingFileHeader(pHeader);
    }
    else
    {
        Error = ERR_FILE_OPEN;
    }

    fs.close();

    return Error;
}
//---------------------------------------------------------------------------

/**
@brief   Map File�� Header�� �����ϴ� �Լ�
@author  zzang9un
@date    2013.07.30
@param   FileName : ���� �̸�(��� ����)
@param   pHeader : FILE_HEADER ����ü ������
@return  �����ڵ�
*/
int GetMapFileHeader(String FileName, MAP_FILE_HEADER *pHeader)
{
    setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
    
    int Error = NO_ERR;

    ifstream fs;

    // Step 1. File Open
    fs.open(FileName.w_str(), ios::in | ios::binary);

    if(fs.is_open())
    {
        fs.read((char *)pHeader, sizeof(MAP_FILE_HEADER));

        *pHeader = ParsingMapFileHeader(pHeader);
    }
    else
    {
        Error = ERR_FILE_OPEN;
    }

    fs.close();

    return Error;
}

/**
@brief   Ini ������ ������ ���´�
@author  doori.shin
@date    2013.07.30
@param   FileName : ���� �̸�(��� ����)
@param   UpdateTime : ������¥�� �޾ƿ�
@return  Revision : �������� �޾ƿ�
*/
int GetIniFileInfo(String FileName, String* UpdateTime, UINT* Revision)
{
	int Error = NO_ERR;
	TDateTime dt;

	TIniFile *IniFile = new TIniFile(FileName);
	*Revision = IniFile->ReadInteger(L"COMMON", L"Version", 0);

	if(*Revision==0) return Error = 1;

	dt = FileDateToDateTime(FileAge(FileName));
	UpdateTime->sprintf(L"%s", FormatDateTime("yyyy.mm.dd hh:mm:ss", dt));

	return Error;
}
//---------------------------------------------------------------------------
#endif
