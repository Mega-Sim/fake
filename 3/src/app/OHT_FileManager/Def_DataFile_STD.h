/**
@file    Def_DataFile_STD.h
@brief   Standard Map Data File�� �������� ����Ǵ� Header ����
*/

#ifndef Def_DataFile_STD
#define Def_DataFile_STD

#include <IniFiles.hpp>
#include <iostream>
#include <fstream>
#include <locale.h> // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�
#include <Def_DataType.h>

#define ERR_FILTER_FORMAT_REVISION      -100  //Define ���� ������ ��� ��ȣġ�� �ߺ����� �����߻�
#define ERR_FILTER_FORMAT_UPDATETIME    -101
#define ERR_FILTER_FORMAT_COUNT         -102
#define ERR_DATA_SIZE                   -110
#define ERR_DATA_LIMIT_REVISION         -200
#define ERR_DATA_LIMIT_UPDATETIME       -201
#define ERR_DATA_LIMIT_COUNT            -202

#if(OHT_NAME == OHT_NAME_P4_MASK)
#define TOTAL_MAPSET_FILES (6)
#else
#define TOTAL_MAPSET_FILES (7)
#endif

#define NODE_COUNT_MAX        (50000) //�ִ� ��� �� 10000 ??  <<--���� �Ķ���ͷ� ������ 
                                      //�ƴϸ� �ƿ� ū�� ���� ���� �Ѿ�� ���� �ȵǴ°� ����
#define FileTypeCID				(1)
#define FileTypeCollision		(2)
#define FileTypeFlowControl		(3)
#define FileTypeLookDown		(4)
#define FileTypeMAP				(5)
#define FileTypeStation			(6)
#define FileTypeTeaching		(7)
#define FileTypeJobOrder	    (8)
#define FileTypeMapMakerFile	(9)


// -------------------------------------------
// Map ���� ���� �ڵ�
// -------------------------------------------
#define ERR_MAP_FILEOPENFAIL        (-1000)
#define ERR_MAP_FILESAVEFAIL        (-2000)
#define ERR_MAP_HEADERINVALID       (-3000)
#define ERR_MAP_FILEINVALID         (-4000)

#if 0 // Def_OHT.h �� ����
// -------------------------------------------
// Map ���� ��� �� ���� �̸�
// -------------------------------------------
#define STR_DATAFILE_PATH           L"\\DataFiles\\MapSet\\"
#define STR_DATAFILE_BACKUP_PATH    L"\\DataFiles\\Backup\\"
#define STR_FILENAME_CID            L"CID.DAT"
#define STR_FILENAME_COLLISION      L"COLLISION.DAT"
#define STR_FILENAME_FLOWCONTROL    L"FLOWCONTROL.DAT"
#define STR_FILENAME_LOOKDOWN       L"LOOKDOWN.DAT"
#define STR_FILENAME_MAP            L"MAPDATA.DAT"
#define STR_FILENAME_STATION        L"STATION.DAT"
#define STR_FILENAME_TEACHING       L"TEACHING.DAT"
#endif
//---------------------------------------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack

/**
@brief   Data File Header ����ü
@author  KJD
@date    2016.3.30
@note    ǥ��ȭ �� �ش��� ���� �ϹǷ� �������� ���
         ���� ��� ����� "HEADER_INFO_STD_�����̸�" ����ü ���
*/
typedef struct FILE_HEADER_STD
{
	UINT ModelID;                     ///< ���� ���� OHT �� ����   ex) ���� ���� ���� ID
	UINT Version;                     //< bit���� Major(8bits), Minor(12bits), Revision(12bits)��  �Ҵ�
                                      // ex) ���������� ���� 98.12.3456�� ��� 
                                      //     98 �� 2������ "110 0010",  Hex�� 0x62
                                      //     12 �� 2������ "1100", Hex��   0xC
                                      //     3456 �� 2������ "1101 1000  0000", Hex�� 0xD80
                                      //     ���������� ���� Hex�� 0x6200CD80�� �Ǹ�   10�����δ� 1,644,219,776�� ��
	UINT DataType;                    ///< Data ���� �� �ɼ�				  
                                      //  0 bit : ����/��� Ȯ�� bit
                                      //  ������ MAP�̸� ON, ���� MAP�̸� OFF.
                                      //  Project.tgz�� ������ ������ ��  bit�� OFF(����)   ���� MAP�� ������ ��쿡�� 
                                      //  bit�� ON(������)�� �Ѵ�.
                                      //  1 bit : �Ķ���� ������bit
                                      //  ��ü�� ���� ƼĪ�̳� ���� �н��� �ǽ����� ��� ON�� �Ѵ�.
                                      //  StationEditor�� �о���� ���, bit�� OFF�� �Ѵ�.
	BYTE VersionString[80];           ///< ���������� ���ڿ� - ��>����, �����, ���� ��Ī
	UINT DataCount;                   ///< Data ��ü ��
}FILE_HEADER_STD;

#pragma pack(pop) // 1byte�� Pack
//---------------------------------------------------------------------------

/**
@brief   ���Ͽ��� ���� Header�� �Ľ��Ͽ� �����ϴ� �Լ�
@author  KJD
@date    2016.3.30
@param   pHeader : �˻��� ���� Header ����ü
@return  Parsing�� File Header ����ü�� �����Ѵ�. ???
*/
FILE_HEADER_STD ParsingFileHeader_STD(FILE_HEADER_STD* pHeader)
{
    FILE_HEADER_STD RtnHeader;
    memset(&RtnHeader, 0x00, sizeof(RtnHeader));

    //int Error = NO_ERR;

    // ����� �ùٸ��� Ȯ��
	if((pHeader->DataCount <= 0) || (pHeader->DataCount > 10000))
	{
		//Error = ERR_DATA_LIMIT_COUNT;
		RtnHeader.DataCount = NULL;
	}
	else
	{
		RtnHeader.ModelID = pHeader->ModelID;
		RtnHeader.Version = pHeader->Version;
		RtnHeader.DataCount = pHeader->DataCount;
		memcpy(RtnHeader.VersionString,pHeader->VersionString,sizeof(pHeader->VersionString));
	}

	//Todo : �߰����� ��� �Ǵ� ���� Ȯ��

    return RtnHeader;
}

//---------------------------------------------------------------------------
#endif

