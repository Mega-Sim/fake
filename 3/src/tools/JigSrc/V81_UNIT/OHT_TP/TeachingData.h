//---------------------------------------------------------------------------
/**
@file    TeachingData.h
$Rev:: 477           $:  Revision of last commit
$Author:: zzang9un   $:  Author of last commit
$Date:: 2013-01-07 1#$:  Date of last commit
@version OHT 7.0
@brief   TeachingData Class�� ������ Header File
*/

#ifndef TeachingDataH
#define TeachingDataH

#include <map>
#include <utility>

#include "Def_OHT.h"
#include "Def_DataFile.h"
#include "Def_DataType.h"
//---------------------------------------------------------------------------

// by zzang9un 2013.07.19 : ��� �������� ������ ���� 1����Ʈ�� Pack
#pragma pack(push, 1) // 1byte�� Pack

// Error Code
#define ERR_TDATA_MAKEMAP       -1
#define ERR_TDATA_FILEOPEN      -1001
#define ERR_TDATA_FORMAT        -2000   ///< Data Format Error
#define ERR_TDATA_DUPLICATION   -2001   ///< Data �ߺ�
#define ERR_TDATA_INSERTFAIL    -2002   ///< Map Insert Fail
#define ERR_TDATA_DELETEFAIL    -2003   ///< Map Delete Fail
#define ERR_TDATA_UPDATEFAIL    -2004   ///< Map Update Fail
#define ERR_TDATA_NODATA        -2005   ///< Map Data ����
#define ERR_TDATA_SAVEFAIL      -2006   ///< Save ����

// by zzang9un 2013.07.18 : ��Ʈ �ʵ尡 ���Ե� �� ���� ������ ����
#define NUM_TEACHING_DATA_ELEMENT    16   ///< Station Data ����ü ���� ����

/**
@brief   Teaching Data ����ü
@author  zzang9un
@date    2013.07.18
*/
typedef struct TEACHING_DATA_
{
    UINT StationID;             ///< Station ID
    BYTE PortType;              ///< �۾� Port Type
    int  Hoist;                 ///< Hoist Teching ��(mm)
    int  Shift;                 ///< Shift Teching ��(mm)
    int  Rotate;                ///< Rotate Teching ��(radian)
    BYTE PIODirection;          ///< PIO ����(Left, Right)
    BYTE PIOTimeLevel;          ///< PIO Time Level
    BYTE LookDownLevel;         ///< LookDownLevel
	uTRANS_OPTION TransOption;   ///< ��Ÿ ������ �ɼ�
	UINT PIOID;             	///< RF PIO ID
	BYTE PIOCS;					///< RF PIO CS ��ȣ ����
	UINT Reserved1;             ///< Reserved 4Byte
	BYTE Reserved2;				///< Reserved 1Byte
} TEACHING_DATA;

#pragma pack(pop) // Pack ����

// Teaching Data�� ���� multimap ����
typedef std::multimap<UINT, TEACHING_DATA *> TEACHING_DATA_MMAP;
typedef TEACHING_DATA_MMAP::value_type TEACHING_DATA_VALUE;
typedef TEACHING_DATA_MMAP::iterator TEACHING_DATA_MMAP_ITER;
typedef std::pair<UINT, TEACHING_DATA *> TEACHING_DATA_PAIR;
typedef std::pair<TEACHING_DATA_MMAP_ITER, bool> TEACHING_DATA_MMAP_RESULT;
typedef std::pair<TEACHING_DATA_MMAP_ITER, TEACHING_DATA_MMAP_ITER> TEACHING_DATA_MMAP_RANGE;

/**
@class   TeachingDataManager
@date    2013.06.19
@author  zzang9un
@brief   Teaching Data�� �����ϴ� Class

         1. Teaching Data�� map(�ڷᱸ��)�� �����Ǿ� ����
         2. binary ������ ���ϸ� �о�� �� ����
*/
class TeachingDataManager
{
public:
    bool m_bDataLoaded;          ///< Data Load ���� flag
	bool m_bDataModified;      ///< �ӽ� Data Load ���� flag

	FILE_HEADER m_FileHeader;

public:
    TEACHING_DATA_MMAP m_TeachingData;   ///< Teaching Data �ڷᱸ��(map)

    TeachingDataManager();
    ~TeachingDataManager();

    // ���� ó�� �Լ�
    int LoadDataFromFile(String FileName);
	int SaveDataToFile(String FileName);
	FILE_HEADER* GetFileHeader();
	void SetFileHeader(FILE_HEADER *pHeader);

    // Map �ڷᱸ�� ���� �Լ�
    void Init();
    int GetCount();

    // Map �ڷᱸ�� �߰�, ������Ʈ, ����, �˻�  �Լ�
    int Insert(TEACHING_DATA *addStationData);
    int Update(TEACHING_DATA *updateSTD);
    int Delete(UINT StationID, BYTE PortType);
    TEACHING_DATA* Find(UINT StationID, BYTE PortType);
};

//---------------------------------------------------------------------------

#endif
