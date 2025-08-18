#pragma once

#include "stdafx.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region  MAPDATA.dat ���� �Ľ� �ڵ�




#define NODE_COUNT_MAX        (999999) //�ִ� ��� �� 999999
#define VESION_STR_LENGTH			(80)

// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack
typedef struct Node_Info_STD_Map
{
	UINT NodeID;                        ///< ���� Node Id
	BYTE NodeType;                      ///< Node�� ����
										//   0x1: Lift In, 0x2: Lift out 0x3: MTL In, 0x4 : MTL Middle, 0x5:MTL out
	UINT LeftNodeID;                    ///< (����) ����� Node Id
	UINT LeftNodeDistance;              ///< (����) ����� Node������ �Ÿ�(mm)
	WORD LeftNodeSpeed;                 ///< (����) ����� Node������ �̵� �ӵ�(mm/s)
	BYTE LeftSteering;                  ///< (����) ����� Point������ Steer ����
	BYTE LeftBranch;                    ///< (����) ����� Point������ �б�����(�Ϲݺб�,N�б�)
	UINT RightNodeID;                   ///< (������) ����� Node Id
	UINT RightNodeDistance;             ///< (������) ����� Node������ �Ÿ�(mm)
	WORD RightNodeSpeed;                ///< (������) ����� Node������ �̵� �ӵ�(mm/s)
	BYTE RightSteering;                 ///< (������) ����� Point������ Steer ����
	BYTE RightBranch;                   ///< (������) ����� Point������ �б�����(�Ϲݺб�,N�б�)
										//	UINT ResetNodeMain;                 ///< ���_�㰡_Reset Node ID(Main)
										//	UINT ResetNodeSub;                  ///< ���_�㰡_Reset Node ID(Sub.)
										//	UINT ResetNodeRedundance1;          ///< ���_�㰡_Reset Node ID(Redundance 1)
										//	UINT ResetNodeRedundance2;          ///< ���_�㰡_Reset Node ID(Redundance 2)
	UINT Reserved_1;                    ///< Reserved_1
	UINT Reserved_2;                    ///< Reserved_2
	UINT Reserved_3;                    ///< Reserved_3

	Node_Info_STD_Map() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_MAP;
//stuct size : 57Byte
#pragma pack(pop) // Pack ����

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
	BYTE VersionString[VESION_STR_LENGTH];           ///< ���������� ���ڿ� - ��>����, �����, ���� ��Ī
	UINT DataCount;                   ///< Data ��ü ��

	FILE_HEADER_STD() { ZeroMemory(this, sizeof(*this)); }
}FILE_HEADER_STD;


static const unsigned short crc16tab[256] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

unsigned short crc16_ccitt(const char *buf, int len)
{
	register int counter;
	register unsigned short crc = 0;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *(char *)buf++) & 0x00FF];
	return crc;
}


/**
@brief   Big-Endian�� Little-Endian���� ��ȯ���ִ� �Լ�
@author  zzang9un
@date    2012.11.01
@param   objp : Convert�� Data�� �ּ�
@tparam  Type : template parameter
*/
template <class Type>
inline void ConvertEndian(Type *objp)
{
	unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
	std::reverse(memp, memp + sizeof(Type));
}


class STD_MAP
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_MAP *m_NodeArray; ///< Node ���� �迭 ������
	FILE_HEADER_STD MapFileHeader;

	STD_MAP()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_MAP()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_MAP[NodeNum];
		}
	}
	bool LoadMapFile_MapData(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		
		bool MapFileLoaded = false;
		bool Error = false;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&MapFileHeader, 0x00, sizeof(MapFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_MapFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&MapFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&MapFileHeader.ModelID);
			ConvertEndian(&MapFileHeader.Version);
			ConvertEndian(&MapFileHeader.DataCount);
			ConvertEndian(&MapFileHeader.DataType);

			if ((MapFileHeader.DataCount > 0) && (MapFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				InitMap(MapFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = MapFileHeader.DataCount * sizeof(NODE_INFO_STD_MAP);

				fs.read((char *)m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						Error = false;
						return Error;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Map Data File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < m_NodeNum; i++)
				{
					ConvertEndian(&m_NodeArray[i].NodeID);
					ConvertEndian(&m_NodeArray[i].LeftNodeID);
					ConvertEndian(&m_NodeArray[i].LeftNodeDistance);
					ConvertEndian(&m_NodeArray[i].LeftNodeSpeed);
					ConvertEndian(&m_NodeArray[i].RightNodeID);
					ConvertEndian(&m_NodeArray[i].RightNodeDistance);
					ConvertEndian(&m_NodeArray[i].RightNodeSpeed);
					i = i; //debug break point �뵵 ���� ����  
				}
				MapFileLoaded = true; // Map File Loaded flag ����
			}
		}
		else
		{
			Error = true;
		}

		fs.close();
		return true;
	}

	bool Save_asText(CString filename)
	{
		// OCSLayout���Ͽ� ���ʿ� �ٿ��� MapData ������ ����Ѵ�. [MAP_DATA] �� ����
		CStdioFile layoutFile;
		if (layoutFile.Open(filename, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		// map data ���
		for (int i = 0; i < m_NodeNum; i++)
		{
			NODE_INFO_STD_MAP *p = &m_NodeArray[i];

			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->NodeID,
				p->NodeType,
				p->LeftNodeID,
				p->LeftNodeDistance,
				p->LeftNodeSpeed,
				p->LeftSteering,
				p->LeftBranch,
				p->RightNodeID,
				p->RightNodeDistance,
				p->RightNodeSpeed,
				p->RightSteering,
				p->RightBranch
			);

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}
};


#pragma endregion

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region CID.dat ���� �Ľ� �ڵ�



// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack


/**
@brief   CID.DAT�� data ���� ����ü
@author  KJD
@date    2016.03.30
*/
typedef struct Node_Info_STD_CID
{
	BYTE LayoutType;                    ///< �շ�����, ����, MTL, Lift, �߰�(Interface), Pass Door, Fire Door
										//UINT LayoutID;                    // UI ǥ�ÿ� ID
	BYTE LayoutID[6];                   ///< UI ǥ�ÿ� ID : CID001, CID002...
	UINT ResetNode;                     ///< Layout �� Reset Node
	UINT StopNode1;                     ///< Layout �� Stop Node 1
	UINT StopNode2;                     ///< Layout �� Stop Node 2
	BYTE ResetNodeType;                 ///< Layout �� Reset Node Type
	BYTE CIDDirection1;                 ///< Stop Node 1 �� Reset Node������ CID-O ����
	UINT CommStartNode1_1;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 1
	UINT CommStartNode1_2;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 2
	UINT CommStartNode1_3;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 3
	UINT CommStartNode1_4;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 4
	UINT CommStartNode1_5;              ///< Stop Node 1 �� Reset Node������ ��� ����(CID-O On) Node 5
	INT  CommStartNode1_Offset;         ///< CommStartNode1_1 ���� CID���� �������� ������ �Ÿ�(mm, �⺻��0, ���ۼ��� ���谪 ����)
	BYTE CIDDirection2;                 ///< Stop Node 2 �� Reset Node������ CID-O ����
	UINT CommStartNode2_1;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 1
	UINT CommStartNode2_2;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 2
	UINT CommStartNode2_3;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 3
	UINT CommStartNode2_4;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 4
	UINT CommStartNode2_5;              ///< Stop Node 2 �� Reset Node������ ��� ����(CID-O On) Node 5
	INT  CommStartNode2_Offset;         ///< CommStartNode2_1 ���� CID���� �������� ������ �Ÿ�(mm, �⺻��0, ���ۼ��� ���谪 ����)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2

	Node_Info_STD_CID() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_CID;
//stuct size : 79Byte
#pragma pack(pop) // Pack ����

class STD_CID
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_CID *m_NodeArray; ///< Node ���� �迭 ������

	//std::map<int, int> m_CIDMap;

	STD_CID()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;


		// CIDMap �ʱ�ȭ
		//m_CIDMap.clear();
	}
	~STD_CID()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;


		//m_CIDMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_CID[NodeNum];
		}

		//m_CIDMap.clear();
	}
	bool LoadMapFile_CID(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		FILE_HEADER_STD m_CIDFileHeader; ///< Map File Header

										 //bool Error = false;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_CIDFileHeader, 0x00, sizeof(m_CIDFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));

			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_CIDFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_CIDFileHeader.ModelID);
			ConvertEndian(&m_CIDFileHeader.Version);
			ConvertEndian(&m_CIDFileHeader.DataCount);
			ConvertEndian(&m_CIDFileHeader.DataType);

			if ((m_CIDFileHeader.DataCount > 0) && (m_CIDFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				InitMap(m_CIDFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = m_CIDFileHeader.DataCount * sizeof(NODE_INFO_STD_CID);

				fs.read((char *)m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] CID File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < m_NodeNum; i++)
				{
					//ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
					ConvertEndian(&m_NodeArray[i].ResetNode);
					ConvertEndian(&m_NodeArray[i].StopNode1);
					ConvertEndian(&m_NodeArray[i].StopNode2);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_1);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_2);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_3);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_4);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_5);
					ConvertEndian(&m_NodeArray[i].CommStartNode1_Offset);

					ConvertEndian(&m_NodeArray[i].CommStartNode2_1);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_2);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_3);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_4);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_5);
					ConvertEndian(&m_NodeArray[i].CommStartNode2_Offset);
					i = i; //debug break point �뵵 ���� ����  
				}
				//m_MapFileLoaded = true; // Map File Loaded flag ����
			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString path)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(path, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data ���
		CString strLine;
		for (int i = 0; i < m_NodeNum; i++)
		{
			Node_Info_STD_CID *p = &m_NodeArray[i];

			strLine.Format("%d/%c%c%c%c%c%c/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->LayoutType,
				p->LayoutID[0], p->LayoutID[1], p->LayoutID[2], p->LayoutID[3], p->LayoutID[4], p->LayoutID[5],
				p->ResetNode,
				p->StopNode1,
				p->StopNode2,
				p->ResetNodeType,
				p->CIDDirection1,
				p->CommStartNode1_1
				, p->CommStartNode1_2
				, p->CommStartNode1_3
				, p->CommStartNode1_4
				, p->CommStartNode1_5
				, p->CommStartNode1_Offset
				, p->CIDDirection2
				, p->CommStartNode2_1
				, p->CommStartNode2_2
				, p->CommStartNode2_3
				, p->CommStartNode2_4
				, p->CommStartNode2_5
				, p->CommStartNode2_Offset
				, p->Reserved1
				, p->Reserved2
			);

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}

};


#pragma endregion




// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack
typedef struct Node_Info_STD_Collision
{
	UINT NodeID;                        ///< ���� Node Id
	UINT LeftNodeID;                    ///< (����) ����� Node Id
	BYTE LeftUBGRegion;                 ///< (����) ����� Point������ ������������ ����
	BYTE LeftObstacleRegion;            ///< (����) ����� Point������ ��ֹ��������� ����
	BYTE LeftObstacleStatus;            ///< (����) ��ֹ��������� ��� ���� (0: �̻��, 1: ���)
										//	UINT Reserved1;
	BYTE L_Reserved1;
	BYTE L_Reserved2;
	BYTE L_Reserved3;
	BYTE L_Reserved4;
	UINT RightNodeID;                   ///< (������) ����� Node Id
	BYTE RightUBGRegion;                ///< (������) ����� Point������ ������������ ����
	BYTE RightObstacleRegion;           ///< (������) ����� Point������ ��ֹ��������� ����
	BYTE RightObstacleStatus;           ///< (������) ��ֹ��������� ��� ���� (0: �̻��, 1: ���)
										//	UINT Reserved2;
	BYTE R_Reserved1;
	BYTE R_Reserved2;
	BYTE R_Reserved3;
	BYTE R_Reserved4;
	Node_Info_STD_Collision() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_COLLISION;
#pragma pack(pop) // Pack ����



class STD_Collision
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_COLLISION *m_NodeArray; ///< Node ���� �迭 ������

	//std::map<int, int> m_CollisionMap;
	void MakeCollisionMap_STD()
	{
		if ((m_NodeNum == 0) || (m_NodeArray == NULL))
			return;

		//for (int i = 0; i < m_NodeNum; i++)
		//{
		//	// Key�� Path ID, Value�� �迭 Index
		//	m_CollisionMap[m_NodeArray[i].NodeID] = i;
		//}
	}

	STD_Collision()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;

		// CollisionMap �ʱ�ȭ
		//m_CollisionMap.clear();
	}
	~STD_Collision()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;

		//m_CollisionMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_COLLISION[NodeNum];
		}

		//m_CollisionMap.clear();
	}

};

class CollisionManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag
	STD_Collision Mapdata;
public:
	FILE_HEADER_STD m_CollisionFileHeader; ///< Map File Header
										   //HEADER_INFO_STD_MAP

	CollisionManager_STD()
	{
		m_MapFileLoaded = false;
		memset(&m_CollisionFileHeader, 0x00, sizeof(FILE_HEADER_STD));
	}
	~CollisionManager_STD()
	{
		m_MapFileLoaded = false;
	}

	bool LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_CollisionFileHeader, 0x00, sizeof(m_CollisionFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_CollisionFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_CollisionFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_CollisionFileHeader.ModelID);
			ConvertEndian(&m_CollisionFileHeader.Version);
			ConvertEndian(&m_CollisionFileHeader.DataCount);
			ConvertEndian(&m_CollisionFileHeader.DataType);

			if ((m_CollisionFileHeader.DataCount > 0) && (m_CollisionFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				Mapdata.InitMap(m_CollisionFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = m_CollisionFileHeader.DataCount * sizeof(NODE_INFO_STD_COLLISION);

				fs.read((char *)Mapdata.m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						//ADD_LOG("[CRC16_ERR] Collision File: cCRC16_header: 0x%x, cCRC16_file: 0x%x", cCRC16_header, cCRC16_file);
						//Error = ERR_MAP_FILEOPENFAIL;
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Collision File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < Mapdata.m_NodeNum; i++)
				{
					ConvertEndian(&Mapdata.m_NodeArray[i].NodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].LeftNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].RightNodeID);
					i = i; //debug break point �뵵 ���� ����  
				}
				m_MapFileLoaded = true; // Map File Loaded flag ����
			}
		}
		else
		{
			return false;
			//Error = ERR_MAP_FILEOPENFAIL;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data ���
		CString strLine;
		for (int i = 0; i <Mapdata.m_NodeNum; i++)
		{
			Node_Info_STD_Collision *p = &Mapdata.m_NodeArray[i];

			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d\n",
				p->NodeID, p->LeftNodeID, p->LeftUBGRegion, p->LeftObstacleRegion, p->LeftObstacleStatus
				, p->RightNodeID, p->RightUBGRegion, p->RightObstacleRegion, p->RightObstacleStatus);
				

			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	}

};


// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack

typedef struct DEFAULT_MAPINFO_
{
	BYTE LOW_DefaultMapInfo : 4;    ///< ���� ��Ʈ(Left, Staion, Teaching ����)
	BYTE HIGH_DefaultMapInfo : 4;    ///< ���� ��Ʈ(Right ����)

	DEFAULT_MAPINFO_() { ZeroMemory(this, sizeof(*this)); }
} DEFAULT_MAPINFO;


typedef struct Node_Info_STD_Station
{
	UINT StationID;                     ///< Station Id
	BYTE StationType;                   ///< Station Type
	UINT ParentNodeID;                  ///< Station�� ���� �ִ� Link�� from Node Id
	UINT NextNodeID;                    ///< Station�� ���� �ִ� Link�� to Node Id
	WORD Offset;                        ///< Parent Node�κ����� �Ÿ�(mm)
	BYTE Safety_Level;                  ///< ������峻 ������ �ݳ� ���� �Ǵ� �÷���
	BYTE Reserved1;         			///< Reserved 1
	BYTE Reserved2;         			///< Reserved 2
	BYTE Reserved3;         			///< Reserved 3
	BYTE Reserved4;                     ///< Reserved 4
	BYTE Reserved5;                     ///< Reserved 5
	BYTE Reserved6;                     ///< Reserved 6
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low����Ұ�.

	Node_Info_STD_Station() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_STATION;
//stuct size : 23Byte

#pragma pack(pop) // Pack ����

class STD_Station
{
private:


public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_STATION *m_NodeArray; ///< Node ���� �迭 ������

	//std::map<int, int> m_StationMap;

	STD_Station()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;

		// StationMap �ʱ�ȭ
		//m_StationMap.clear();
	}
	~STD_Station()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
		//m_StationMap.clear();
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_STATION[NodeNum];
		}

		//m_StationMap.clear();
	}

};

class StationManager_STD
{
private:
	bool m_MapFileLoaded; ///< Map File Load Flag

public:
	FILE_HEADER_STD m_StationFileHeader; ///< Map File Header
										 //HEADER_INFO_STD_MAP
	STD_Station Mapdata;

	StationManager_STD()
	{
		m_MapFileLoaded = false;

		memset(&m_StationFileHeader, 0x00, sizeof(FILE_HEADER_STD));
	}
	~StationManager_STD()
	{
		m_MapFileLoaded = false;
	}

	bool LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		//int Error = NO_ERR;
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_StationFileHeader, 0x00, sizeof(m_StationFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_StationFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_StationFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_StationFileHeader.ModelID);
			ConvertEndian(&m_StationFileHeader.Version);
			ConvertEndian(&m_StationFileHeader.DataCount);
			ConvertEndian(&m_StationFileHeader.DataType);

			if ((m_StationFileHeader.DataCount > 0) && (m_StationFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				Mapdata.InitMap(m_StationFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = m_StationFileHeader.DataCount * sizeof(NODE_INFO_STD_STATION);

				fs.read((char *)Mapdata.m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						//ADD_LOG("[CRC16_ERR] Station File: cCRC16_header: 0x%x, cCRC16_file: 0x%x", cCRC16_header, cCRC16_file);
						//Error = ERR_MAP_FILEOPENFAIL;
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Station File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < Mapdata.m_NodeNum; i++)
				{
					ConvertEndian(&Mapdata.m_NodeArray[i].StationID);
					ConvertEndian(&Mapdata.m_NodeArray[i].ParentNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].NextNodeID);
					ConvertEndian(&Mapdata.m_NodeArray[i].Offset);
					i = i; //debug break point �뵵 ���� ����  
				}

				m_MapFileLoaded = true; // Map File Loaded flag ����
			}
		}
		else
		{
			return false;
			//Error = ERR_MAP_FILEOPENFAIL;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile layoutFile;
		if (layoutFile.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		// cid data ���
		CString strLine;
		for (int i = 0; i <Mapdata.m_NodeNum; i++)
		{
			NODE_INFO_STD_STATION *p = &Mapdata.m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d\n", p->StationID, p->StationType, p->ParentNodeID, p->NextNodeID, p->Offset, p->Safety_Level);
			layoutFile.WriteString(strLine);
		}
		layoutFile.Close();
		return true;
	
	}
	//int SaveMapFile(TCHAR *FileName, STD_Station *pMapdata);
	FILE_HEADER_STD* GetFileHeader(){ return &m_StationFileHeader; }
	void SetFileHeader(FILE_HEADER_STD *pHeader){ memcpy(&m_StationFileHeader, pHeader, sizeof(FILE_HEADER_STD)); }

};


#pragma pack(push, 1) // 1byte�� Pack

typedef struct Node_Info_STD_FlowControl
{
	UINT StopNodeID;                    ///< Stop Node Id
	UINT ResetNodeMain;                 ///< Reset Node ID(Main) - �շ��� ��� Main���� �Է�"
	BYTE BZStopMain;                    ///< BZ Stop ����
	int ResetNodeMainDefaultValue;     ///< Reset Node ID(Main)�� ���� ��� Default ��
	UINT ResetNodeMainDefaultRatio;     ///< Reset Node ID(Main)�� ���� ��� Default �� * (0 ~ 200%)
	UINT ResetNodeSub;                  ///< Reset Node ID(Sub.) - �б��� ��� Main�� Sub.�� ��� �Է�"
	BYTE BZStopSub;                     ///< BZ Stop ����
	int ResetNodeSubDefaultValue;      ///< Reset Node ID(Sub.)�� ���� ��� Default ��
	UINT ResetNodeSubDefaultRatio;      ///< Reset Node ID(Sub.)�� ���� ��� Default �� * (0 ~ 200%)
	int ResetNodeRedundance1;          ///< Reset Node ID(Redundance 1)
	BYTE BZStopRedundance1;             ///< BZ Stop ����
	int ResetNodeRedundance1DefaultValue;        ///< Reset Node ID(Redundance 1)�� ���� ��� Default ��
	UINT ResetNodeRedundance1DefaultRatio;        ///< Reset Node ID(Redundance 1)�� ���� ��� Default �� * (0 ~ 200%)
												  //UINT ResetNodeRedundance2;          ///< Reset Node ID(Redundance 2)
												  //BYTE BZStopRedundance2;             ///< BZ Stop ����
												  //UINT ResetNodeRedundance2DefaultValue;        ///< Reset Node ID(Redundance 2)�� ���� ��� Default ��
												  //UINT ResetNodeRedundance2DefaultRatio;        ///< Reset Node ID(Redundance 2)�� ���� ��� Default �� * (0 ~ 200%)
	UINT Reserved1;                     ///< Reserved_1
	UINT Reserved2;                     ///< Reserved_2
	UINT Reserved3;                     ///< Reserved_3
	UINT Reserved4;                     ///< Reserved_4

	Node_Info_STD_FlowControl() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_FLOWCONTROL;
//stuct size : 72Byte

#pragma pack(pop) // Pack ����


class STD_FlowControl
{
private:


public:
	int m_NodeNum; ///< Node ����
	NODE_INFO_STD_FLOWCONTROL *m_NodeArray; ///< Node ���� �迭 ������
	
	STD_FlowControl() 
	{
		m_NodeNum = 0;
		m_NodeArray = NULL;
	}

	~STD_FlowControl()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_FLOWCONTROL[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_FLOWCONTROL[NodeNum];
		}
	}

	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		FILE_HEADER_STD FlowControlFileHeader; ///< Map File Header

		STD_FlowControl *pMapdata = this;
		
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&FlowControlFileHeader, 0x00, sizeof(FlowControlFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_FlowControlFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&FlowControlFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&FlowControlFileHeader.ModelID);
			ConvertEndian(&FlowControlFileHeader.Version);
			ConvertEndian(&FlowControlFileHeader.DataCount);
			ConvertEndian(&FlowControlFileHeader.DataType);


			if ((FlowControlFileHeader.DataCount > 0) && (FlowControlFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				pMapdata->InitMap(FlowControlFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = FlowControlFileHeader.DataCount * sizeof(NODE_INFO_STD_FLOWCONTROL);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					return false;
					//ADD_LOG("[EOF_ERR] FlowControl File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].StopNodeID);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMain);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMainDefaultValue);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeMainDefaultRatio);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSub);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSubDefaultValue);
					ConvertEndian(&pMapdata->m_NodeArray[i].ResetNodeSubDefaultRatio);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved1);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved2);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved3);
					ConvertEndian(&pMapdata->m_NodeArray[i].Reserved4);
					i = i; //debug break point �뵵 ���� ����  
				}
				//m_MapFileLoaded = true; // Map File Loaded flag ����
			}
		}
		else
		{
			//Error = ERR_MAP_FILEOPENFAIL;
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			NODE_INFO_STD_FLOWCONTROL *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n", p->StopNodeID, p->ResetNodeMain, p->ResetNodeMainDefaultValue, p->ResetNodeMainDefaultRatio, p->ResetNodeSub, p->ResetNodeSubDefaultValue
				, p->ResetNodeSubDefaultRatio
				, p->ResetNodeRedundance1
				, p->BZStopRedundance1
				, p->ResetNodeRedundance1DefaultValue
				, p->ResetNodeRedundance1DefaultRatio);

			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};


// -------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack


typedef struct Node_Info_STD_Teaching
{
	UINT StationID;                     ///< Station Id
	BYTE PortType;                      ///< ������ Port ���� ex) EQ, STB-L, STB-R, UTB"
	int Hoist;                         ///< ������ Hoist �̵��Ÿ�(1/10mm)
	int Shift;                         ///< ������ Shift �̵��Ÿ�(1/10mm)
	int Rotate;                        ///< ������ Rotate �̵��Ÿ�(radian)
	BYTE PIOType;                       ///< 0:IR, 1:RF, 2:Hybrid 5.0GHz
	BYTE PIODirection;                  ///< PIO ����(��, ��)
	UINT PIOID;                         ///< RF-PIO Setting ID ����
	UINT PIOChannel;                    ///< RF-PIO Setting Channel ����
	BYTE PIOTimeLevel;                  ///< PIO Time out ���� �ð� ����
	BYTE AutoRecovery;                  ///< AutoRecovery
	BYTE HandDetectEQ;                  ///< HandDetectEQ
	int STBOffSet;                     ///< STB Off Set �̵��Ÿ�(1/10mm, �⺻�� 0)
	BYTE LookDown;                      ///< Station ������ �� Lookdown Area ����(0~16, �⺻�� 1)
	UINT Reserved1;                     ///< Reserved 1
	BYTE Reserved2;                     ///< Reserved 2
	BYTE Reserved3;                     ///< Reserved 4
	char STBL_Shift;                     ///< STBL_Shift ƼĪ ���� 3mm�߰� soming
	DEFAULT_MAPINFO IsDefaultMap;       ///< Low����Ұ�.

	Node_Info_STD_Teaching() { ZeroMemory(this, sizeof(*this)); }
} NODE_INFO_STD_TEACHING;
//stuct size : 34Byte

#pragma pack(pop) // Pack ����


class STD_Teaching
{
public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_TEACHING *m_NodeArray; ///< Node ���� �迭 ������

	STD_Teaching()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_Teaching()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_TEACHING[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_TEACHING[NodeNum];
		}
	}

	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		FILE_HEADER_STD m_TeachingFileHeader; ///< Map File Header
		STD_Teaching *pMapdata = this;
		
		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&m_TeachingFileHeader, 0x00, sizeof(m_TeachingFileHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_TeachingFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&m_TeachingFileHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&m_TeachingFileHeader.ModelID);
			ConvertEndian(&m_TeachingFileHeader.Version);
			ConvertEndian(&m_TeachingFileHeader.DataCount);
			ConvertEndian(&m_TeachingFileHeader.DataType);

			if ((m_TeachingFileHeader.DataCount > 0) && (m_TeachingFileHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				pMapdata->InitMap(m_TeachingFileHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = m_TeachingFileHeader.DataCount * sizeof(NODE_INFO_STD_TEACHING);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();
						return false;
					}
				}
				else
				{
					return false;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].StationID);
					ConvertEndian(&pMapdata->m_NodeArray[i].PIOID);
					ConvertEndian(&pMapdata->m_NodeArray[i].Hoist);
					ConvertEndian(&pMapdata->m_NodeArray[i].Shift);
					ConvertEndian(&pMapdata->m_NodeArray[i].Rotate);
					ConvertEndian(&pMapdata->m_NodeArray[i].PIOChannel);
					ConvertEndian(&pMapdata->m_NodeArray[i].STBOffSet);

					//i = i; //debug break point �뵵 ���� ����  
				}
				
			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			NODE_INFO_STD_TEACHING *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n"
				, p->StationID
				, p->PortType
				, p->Hoist
				, p->Shift
				, p->Rotate
				, p->PIOType
				, p->PIODirection
				, p->PIOID
				, p->PIOChannel
				, p->PIOTimeLevel
				, p->AutoRecovery
				, p->HandDetectEQ
				, p->STBOffSet
				, p->LookDown
				, p->STBL_Shift
			);
		
			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};
//---------------------------------------------------------------------------
#pragma pack(push, 1) // 1byte�� Pack
typedef struct Node_Info_STD_PathData
{
	BYTE LayoutType;                    ///< Layout Type(1: MTL, 2: Lifter)
	UINT LayoutID;                      ///< Layout ID
	UINT LeftNodeIDIn;                  ///< MTL(����), Lifter(�Ʒ�) ���� Node Id
	UINT LeftNodeIDMiddle;              ///< MTL(����), Lifter(�Ʒ�) �߰� Node Id
	UINT LeftNodeIDOut;                 ///< MTL(����) ���� Node Id
	BYTE LeftDriveDirection;            ///< ���� ���� ������ ǥ��(0: ����, 1: ����)
	UINT RightNodeIDIn;                 ///< MTL(������), Lifter(��) ���� Node Id
	UINT RightNodeIDMiddle;             ///< MTL(������), Lifter(��) �߰� Node Id
	UINT RightNodeIDOut;                ///< MTL(������) ���� Node Id
	BYTE RightDriveDirection;           ///< ���� ���� ������ ǥ��(0: ����, 1: ����)
	UINT Reserved1;                     ///< Reserved 1
	UINT Reserved2;                     ///< Reserved 2

	Node_Info_STD_PathData() { ZeroMemory(this, sizeof(*this)); }
	
} NODE_INFO_STD_PATHDATA;
#pragma pack(pop) // Pack ����

class STD_PathData
{
public:
	int m_NodeNum; ///< Node ����


	NODE_INFO_STD_PATHDATA *m_NodeArray; ///< Node ���� �迭 ������

	STD_PathData()
	{
		m_NodeNum = 0;

		m_NodeArray = NULL;
	}
	~STD_PathData()
	{
		if (m_NodeArray != NULL)
			delete[] m_NodeArray;
	}

	void InitMap(int NodeNum)
	{
		m_NodeNum = NodeNum;

		// ���� �����ϱ� �� Null������ Ȯ�� �� ����
		if (m_NodeArray != NULL)
		{
			// NULL�� �ƴ� ���� delete �Ŀ� ���� ����
			// �� ������ ���Ž� ���
			delete[] m_NodeArray;
			m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
		}
		else
		{
			//NodeNum Max������ �׽�Ʈ �ʿ� ??
			m_NodeArray = new NODE_INFO_STD_PATHDATA[NodeNum];
		}
	}
	int LoadMapFile(CString FileName)
	{
		setlocale(LC_ALL, "Korean"); // ��θ� �ѱ��� �ִ� ��츦 ���� �߰�

		FILE_HEADER_STD pathDataHeader; ///< Map File Header
		STD_PathData *pMapdata = this;

		unsigned short cCRC16_file;
		unsigned short cCRC16_header;
		unsigned char cEOF[3];

		ifstream fs;

		// Step 1. File Open
		fs.open(FileName, ios::in | ios::binary);

		if (fs.is_open())
		{
			memset(&pathDataHeader, 0x00, sizeof(pathDataHeader));

			// Step 2. Read File Header Information
			FILE_HEADER_STD FileHeader;
			memset(&FileHeader, 0x00, sizeof(FileHeader));

			fs.read((char *)&FileHeader, sizeof(FILE_HEADER_STD));


			// Step 3. Header �Ľ�  --> ���� �Ľ� �ʿ����.
			//m_CIDFileHeader = ParsingMapFileHeader_STD(&FileHeader);
			memcpy(&pathDataHeader, &FileHeader, sizeof(FILE_HEADER_STD));
			ConvertEndian(&pathDataHeader.ModelID);
			ConvertEndian(&pathDataHeader.Version);
			ConvertEndian(&pathDataHeader.DataCount);
			ConvertEndian(&pathDataHeader.DataType);

			if ((pathDataHeader.DataCount > 0) && (pathDataHeader.DataCount < NODE_COUNT_MAX))
			{
				int NodeSize = 0; // Node Data �� size
								  // Step 4. ���Ͽ��� Load�� ���뿡 ���� Init
				pMapdata->InitMap(pathDataHeader.DataCount);

				// Step 5. Node Size��ŭ Read
				NodeSize = pathDataHeader.DataCount * sizeof(NODE_INFO_STD_PATHDATA);

				fs.read((char *)pMapdata->m_NodeArray, NodeSize);
				fs.read((char *)&cCRC16_file, sizeof(cCRC16_file));
				fs.read((char *)&cEOF, sizeof(cEOF));
				ConvertEndian(&cCRC16_file);
				cCRC16_header = crc16_ccitt((const char *)&FileHeader, sizeof(FILE_HEADER_STD));
				if (cEOF[0] == 'E' && cEOF[1] == 'O' && cEOF[2] == 'F') //EOF �� ������ ���� �� ������ ����
				{
					if (cCRC16_header != cCRC16_file)
					{
						fs.close();					
						return false;
					}
				}
				else
				{
					//ADD_LOG("[EOF_ERR] Path Data File");
					//�� ���� ���� �� �ּ� �κ� ����
					//Error = ERR_MAP_FILEOPENFAIL;
					//fs.close();
					//return Error;
				}

				for (int i = 0; i < pMapdata->m_NodeNum; i++)
				{
					ConvertEndian(&pMapdata->m_NodeArray[i].LayoutID);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDIn);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDMiddle);
					ConvertEndian(&pMapdata->m_NodeArray[i].LeftNodeIDOut);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDIn);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDMiddle);
					ConvertEndian(&pMapdata->m_NodeArray[i].RightNodeIDOut);

					//i = i; //debug break point �뵵 ���� ����  
				}

			}
		}
		else
		{
			return false;
		}

		fs.close();
		return true;
	}
	bool Save_asTxt(CString FileName)
	{
		CStdioFile file;
		if (file.Open(FileName, CFile::modeReadWrite | CFile::modeCreate) == false)
			return false;

		CString strLine;
		for (int i = 0; i <m_NodeNum; i++)
		{
			// [PathData] 
			// LayoutType(1 or 2) / LayoutID / LeftNodeIDIn/ LeftNodeIDMiddle/ LeftNodeIDOut/ LeftDriveDir /
			// RightNodeIDIn/ RightNodeIDMiddle/ RightNodeIDOut/ RightDriveDir / Resered1 / Reserved2

			NODE_INFO_STD_PATHDATA *p = &m_NodeArray[i];
			strLine.Format("%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d\n"
				, p->LayoutType
				, p->LayoutID
				, p->LeftNodeIDIn
				, p->LeftNodeIDMiddle
				, p->LeftNodeIDOut
				, p->LeftDriveDirection
				, p->RightNodeIDIn
				, p->RightNodeIDMiddle
				, p->RightNodeIDOut
				, p->RightDriveDirection
				, p->Reserved1
				, p->Reserved2
			);
			file.WriteString(strLine);
		}
		file.Close();
		return true;

	}
};