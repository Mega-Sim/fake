/**
@file    HID.cpp
$Rev:: 679           $:  Revision of last commit
$Author:: ltw070     $:  Author of last commit
$Date:: 2013-03-11 1#$:  Date of last commit
@version OHT 7.0
@brief   HID Class CPP File
*/

#include "HID_VII_ZR.h"

#include "OHTMain.h"

using namespace std;

#define HID_LOG(...)    WRITE_LOG_ON_THIS_FILE("D:\\log\\HIDRawData.log", __VA_ARGS__)
#define ADD_FDC_LOG(...)			writeFDCUnit(pFDCUnit, __VA_ARGS__)

/**
@brief   HID ������
@author   puting
@date
@param   pHWCommon : HWCommon Class�� ������
@note    Input, Ouput�� ����ϱ� ���� HWSet�� Pointer�� ������
*/
HID_VII_ZR::HID_VII_ZR(int ComPortNum): Virtual_HID()
{
	int nError =0;

	//1.ũ��Ƽ�� ���� ����
	InitializeCriticalSection(&m_CS);

	//2. �ʱ�ȭ
	memset(&m_HIDData, 0x00, sizeof(m_HIDData));
	m_HIDCommPort=NULL;
	GetVer = false;
	BMS_Ver = false;     //�⺻�� ���� CP
	Ver_Cnt = 0;

    RegulatorCheck = false;
	//3. �ø��� Open
	nError = Open(ComPortNum);
	if(nError != NO_ERR)
	{
		 Close();
	 }
	 else
	 {
		// Open ���� �α� ���
	   //	MainForm->OHTStatus->Caption = "Open����";
	   //	 MainForm->Memo1->Lines->Add("OPEN ����");
	   //	 m_nHIDComFailCount=0;
	 }

	 m_pExecuteInfo = OHTMainForm->GetExecuteInfo();
	 m_pOHTMainStatus = OHTMainForm->GetOHTStatus();
	 pFDCUnit = getFDCUnit("D:\\log\\FDC.log", MAX_BYTE_SIZE_OF_MAIN_LOG);
}
/**
@brief   HID �Ҹ���
@author  zzang9un
@date    2012.10.13
*/
HID_VII_ZR::~HID_VII_ZR()
{
	//�Ҹ� �� �ݱ�
	DeleteCriticalSection(&m_CS);
	//delete m_HIDInfo;
	Close();
}



/**
@brief   �ø���� HID Data�� �о ���� �м��� �ϴ� �Լ�
@author  puting
@date    2012.11.07
@note    �̸� ���� Data�� �����´�. �� �������� �ð��� 200ms�̻��� �Ǿ�� �����Ͱ� ����Ǳ� ������ �� ���Ŀ� �����͸� �������� �ȴ�.
@return  ������ ��� 0, ���� �߻��� ��� Error Code ����
*/
int HID_VII_ZR::UpdateData(int nMoveInfo, int nPathInfo)
{
	static	DWORD	dwHIDTrigerTime	= timeGetTime();     //�Լ� ���� �ð�.
	static  int		nErrorCount	= 0;
//	static	bool	bHIDOnTriger	= false;
	bool	bHIDOnTriger	= false;

	UINT  nError = NO_ERROR;

	unsigned char chReadBuff[255]={0,};
//	char chReadBuff[255]={0,};
	char chSendBuff[255]={0,};

	int TempMoveinfo = nMoveInfo;
	int TempnPathInfo = nPathInfo;

	int nRtnByteNo = 0;

	//1. Open Check puting
	// HID�� ������ ���� �����̹Ƿ� ������ �ٷ� �����ؾ���.
	if(m_HIDCommPort == NULL)
		return ERR_HID_OPEN;

	HID_LOG("dwHIDTrigerTime = %d", OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime));    //HIDTrigerTime üũ

	//2. ������ �б�.
	//if( bHIDOnTriger==true)) //������ �ϳ� ������ ������ ����.
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime) > 200)
	{
		nRtnByteNo = Read(chReadBuff, chETX);
		HID_LOG(">>nErrorCount=%d, Recv Data Len = %d, chReadBuff = %s", nErrorCount, strlen(chReadBuff), chReadBuff);     //Recv �α� Ȯ�ο�&�� ������ Ÿ���� ����Ȯ��
		HID_LOG("Checksum=%02x, Last=%02x", chReadBuff[strlen(chReadBuff)-2], chReadBuff[strlen(chReadBuff)-1]);     //������ �̻��ϰ� ©���°� Ȯ�ο�
//		HID_LOG("Vcell Data = %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x",
//				chReadBuff[117],chReadBuff[118],chReadBuff[119],chReadBuff[120],chReadBuff[121],chReadBuff[122],chReadBuff[123],
//				chReadBuff[124],chReadBuff[125],chReadBuff[126],chReadBuff[127],chReadBuff[128],chReadBuff[129],chReadBuff[130],chReadBuff[131],
//				chReadBuff[132],chReadBuff[133],chReadBuff[134],chReadBuff[135],chReadBuff[136],chReadBuff[137],chReadBuff[138],chReadBuff[139],
//				chReadBuff[140],chReadBuff[141],chReadBuff[142],chReadBuff[143]);

		if(nRtnByteNo == 0)
		{
			nErrorCount++;
		}
		else
		{
			trim(chReadBuff);

			if(BMS_Ver == true)
			{
				ltrim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
			}
			else
			{
				trim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
//			for(int number = 116; number<strlen(chReadBuff); number++)
//			{
//				HID_LOG("%02x,", chReadBuff[number]);
//			}
				if(nBeforeLen-2 > strlen(chReadBuff)) chReadBuff[strlen(chReadBuff)] = recvChecksum;      //���ܿ� trim���� ������ checksum �߰�
				HID_LOG("AFTER Last Change LEN = %d",strlen(chReadBuff));
			}

		//1. ���� ������ �м�/ ����ü ����
			nError = AnalysisData(chReadBuff);

			if(nError != NO_ERROR)
				return nError;

			//*********�̰�쿡�� �����͸� ������ ����*************
			//�����͸� ���� ���, �������� �ʱ����� �ʱ�ȭ
			nErrorCount =0;
			//2. ������ ������.
			GetSendData(chSendBuff, nMoveInfo, nPathInfo);
		  //	memmove(chSendBuff,chWriteBuff, SEND_PK_LENGTH70+2);
			Write(chSendBuff);
			dwHIDTrigerTime	= timeGetTime();
			bHIDOnTriger = true;
		}
		// 5���̻�  ���� �����Ͱ� �������, �ٽ� �б� �õ�.
		if(nErrorCount > RETRY_HID_MAX_COUNT)
		{
				bHIDOnTriger= false;
				nErrorCount =0;
				return ERR_HID_READTIMEOUT;
		}
	}
#else
	if(OHTUTILITY::PreventOverflowGetTimeMs(dwHIDTrigerTime) > 300)
	{
		nRtnByteNo = Read(chReadBuff, chETX);
		HID_LOG(">>nErrorCount=%d, Recv Data Len = %d, chReadBuff = %s", nErrorCount, strlen(chReadBuff), chReadBuff);     //Recv �α� Ȯ�ο�&�� ������ Ÿ���� ����Ȯ��

		if(nRtnByteNo == 0)
		{
			nErrorCount++;
		}
		else
		{
			int nBeforeLen = 0;
			unsigned char recvChecksum = chReadBuff[strlen(chReadBuff)-2];
			nBeforeLen = strlen(chReadBuff);

			if(BMS_Ver == true)
			{
				ltrim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
			}
			else
			{
				trim(chReadBuff);
				HID_LOG("AFTER TRIM LEN = %d",strlen(chReadBuff));
//			for(int number = 116; number<strlen(chReadBuff); number++)
//			{
//				HID_LOG("%02x,", chReadBuff[number]);
//			}
				if(nBeforeLen-2 > strlen(chReadBuff)) chReadBuff[strlen(chReadBuff)] = recvChecksum;      //���ܿ� trim���� ������ checksum �߰�
				HID_LOG("AFTER Last Change LEN = %d",strlen(chReadBuff));
			}

			HID_LOG("chReadBuff = %s", chReadBuff);

			if(RegulatorCheck == true && GetVer == false)  //regulator ���� �ޱ�
			{
				if((GetRcvDataCheckSum(chReadBuff, 8)!=true)&&chReadBuff[0]!='g')
				{
					HID_LOG("Error �߻�chReadBuff[0] = %c", chReadBuff[0]);
					return RERTY_HID_SEND_DATA;
				}
				else
				{
					GetVer = true;       //regulator ���� �������� �Ϸ�
					HID_LOG("First Recv Data length = %d", strlen(chReadBuff));
					HID_LOG("Recv Data = %s", chReadBuff);
					if(chReadBuff[1] == 'e')
						BMS_Ver = true;

					HID_LOG("By Receive Data GetVer=%d, BMS_Ver=%d", GetVer, BMS_Ver);
					nErrorCount = 0;
				}
			}
			else
			{
				//1. ���� ������ �м�/ ����ü ����
				nError = AnalysisData(chReadBuff);

				if(nError != NO_ERROR)
					return nError;

				//*********�̰�쿡�� �����͸� ������ ����*************
				//�����͸� ���� ���, �������� �ʱ����� �ʱ�ȭ
				nErrorCount =0;
				//2. ������ ������.
				HID_LOG("GetSend Check 1");
				GetSendData(chSendBuff, nMoveInfo, nPathInfo);
				HID_LOG("Send = %s", chSendBuff);         //������ �ð� Ȯ�ΰ� ������ Ȯ��
				nError = Write(chSendBuff);
				dwHIDTrigerTime	= timeGetTime();
				bHIDOnTriger = true;
				//**************������ �κ��� retry�̰ų� ����*********
			}
		}
		// 5���̻�  ���� �����Ͱ� �������, �ٽ� �б� �õ�.
		if(nErrorCount > RETRY_HID_MAX_COUNT)
		{
				HID_LOG("!!Read Again!!");
				bHIDOnTriger= false;
				nErrorCount =0;
				return ERR_HID_READTIMEOUT;
		}
	}
#endif
	//�ʱ� ���۶� ������ ������ ����. Ȥ�� �����͸� �����ؼ� ������ ��� �ٽ� �õ� �ϱ� ����

	if(bHIDOnTriger == false)
	{
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
    	GetSendData(chSendBuff, nMoveInfo, nPathInfo);
		HID_LOG("Send = %s", chSendBuff);
#else
		if(RegulatorCheck == true && GetVer == false)
		{
			if(Ver_Cnt > 4)      //5�� �̻� �õ��ߴµ� ���������� ���չ�������
			{
				HID_LOG("Set Regulator Version ����CP");
				GetVer = true;
			}
			else
			{
				SendFirstData(chSendBuff);
				Ver_Cnt++;     //regulator ���� �������� SendData ���� Ƚ��
				HID_LOG("Regulator Version Cehck Ƚ�� : %d", Ver_Cnt);
			}
		}
		else
		{
			//2. ������ ������.
			HID_LOG("GetSend Check ------ 2");
			GetSendData(chSendBuff, nMoveInfo, nPathInfo);
			HID_LOG("Send = %s", chSendBuff);             //������ �ð� Ȯ�ΰ� ������ Ȯ��
		}
#endif
		nError = Write(chSendBuff);
		dwHIDTrigerTime	= timeGetTime();

		return RERTY_HID_SEND_DATA;
	}

	return nError;
}

//ó�� ������ �� Ÿ�� Regulator ���� �������� �Լ�   - sseung
void HID_VII_ZR::SendFirstData(char* SendData)
{
	int Error;

	SendData[0] = chSTX;
	SendData[1] = 'v';
	SendData[2] = '0';
	SendData[3] = '0';   //���δ� 0x30�̶� recv�� �ȿ�
	SendData[4] = 'v';
	SendData[5] = chETX;

	HID_LOG("First Send = %s, Len=%d", SendData, strlen(SendData));
//	HID_LOG("First Send = %c,%c,%c,%c,%c", chSendBuff[0],chSendBuff[1],chSendBuff[2],chSendBuff[4],chSendBuff[5]);
}

/**
@brief   HID�κ��� Read�� Data�� �м��ϴ� �Լ�
@date    2012.11.07
@author  puting
@param   strRcvData : HID�κ��� Read�� ���ڿ�
@param   nTmpError : ���� ���� ����(?)
@return  ������ ��� 0, ���� �߻��� ��� Error Code ����
@warning StrToIntDef �Լ��� Builder������ �����ϵ�
*/
int HID_VII_ZR::AnalysisData(char* chRcvData)
{
	int nError = 0;
	char Substr[10] = {0,};
	bool bV85Protocol = false;
	static int nSerialCommCheck = 0;
	int nRecvLen = 0;

	  //Form1->Memo1->Lines->Add(chRcvData);
	//1. data ���ռ� Ȯ��(����)
//	int lens = strlen(chRcvData);
//	if(lens!=RCV_PK_LENGTH70)                           //HID Data ���� ����  6.5�� ���
//   		return ERR_DEFFER_HID_RCV_DATA;      //���� HID ������ ���̰� �ٸ� ���

	//ũ��Ƽ�ü��� ����
	EnterCriticalSection(&m_CS);
	nRecvLen = strlen(chRcvData);
	HID_LOG("Recv Data length = %d, nSerialCommCheck = %d", nRecvLen, nSerialCommCheck);   //recv data ���̷� v85 ��Ʈ Ÿ���� Ȯ��

#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(nRecvLen > RCV_PK_PROTOCOL_LENSTD)
		bV85Protocol = true;

	//2.s�ĺ��� �� CheckSum Ȯ��
//	HID_LOG("HID Data : %s",chRcvData);
	if(bV85Protocol == false)
	{
		if((GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70-1)!=true)&&(chRcvData[0]!='s'))
		{
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //���� �����Ͱ� �̻��� ���.
		}
		else
		{
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
	//		m_HIDData.WarningCode 				= CharToInt(SubChar(chRcvData,23,2));
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Reserve2  				= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
		}
	}
	else
	{
		//recv ũ�⵵ �ٸ�.. RCV_PK_LENGTH70
		if((GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH70-1)!=true)&&(chRcvData[0]!='s'))
		{
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //���� �����Ͱ� �̻��� ���.
		}
		else
		{
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Reserve2  				= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
		}
	}
#else
	if(nRecvLen == (RCV_PK_LENGTH_CP+1) && (chRcvData[37]=='0' && chRcvData[38]=='8' && chRcvData[39]=='5'))  //���̰� �����ϰ� Data�� 85 �����϶�
		bV85Protocol = true;
	else if(nRecvLen != (RCV_PK_LENGTH70+1) && nRecvLen != (RCV_PK_LENGTH_ECO+1))  //2.s�ĺ��� �� CheckSum Ȯ��
	{
		HID_LOG("Data Length Diff");
		nSerialCommCheck++;
		if(nSerialCommCheck > 3)
		{
			nSerialCommCheck = 0;
			memset(&m_HIDData, 0x00, sizeof(m_HIDData));
			nError = ERR_HID_READTIMEOUT;   //���� �����Ͱ� �̻��� ���.
		}

		LeaveCriticalSection(&m_CS);
		return nError;
	}

	if(RegulatorCheck)     //ECO�� ���� CP�� ��
	{
		if(BMS_Ver && (nRecvLen!=(RCV_PK_LENGTH_BMS+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_BMS-1)!=true) || (chRcvData[0]!='s')))
		{
			HID_LOG("ECO ver. ");
			nSerialCommCheck++;
		}
		else if(!BMS_Ver && bV85Protocol==true && (nRecvLen!=(RCV_PK_LENGTH_CP+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_CP)!=true) || (chRcvData[0]!='s')))  //regulator ver.85�̸� 81ver checksum ���
		{
			HID_LOG("85ver. ");
			nSerialCommCheck++;
		}
		else if(!BMS_Ver && bV85Protocol==false && (nRecvLen!=(RCV_PK_LENGTH70+1) || GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70)!=true || chRcvData[0]!='s'))  //RCV_PK_LENGTH70 regulaotr ������ �°� ���� �ʿ�
		{
			HID_LOG("���� CP ver.");
			nSerialCommCheck++;
		}
		else
		{
			if(BMS_Ver)
			{
				m_HIDData.InputStatus				= chRcvData[1];
				m_HIDData.BMSStatus					= chRcvData[2];
				m_HIDData.BatteryStatus				= chRcvData[3];
				m_HIDData.ExCharge					= chRcvData[4];
				m_HIDData.ErrorCode					= CharToInt(SubChar(chRcvData,Substr,5,3,sizeof(Substr)));
				m_HIDData.ErrorCode					= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
				m_HIDData.Iac						= CharToInt(SubChar(chRcvData,Substr,8,3,sizeof(Substr)));
				m_HIDData.BMS_Edc					= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
				m_HIDData.BMS_Iout_Sign 			= chRcvData[14];
				m_HIDData.BMS_Iout					= CharToInt(SubChar(chRcvData,Substr,15,3,sizeof(Substr)));
				m_HIDData.BVoltage					= CharToInt(SubChar(chRcvData,Substr,18,4,sizeof(Substr)));

				m_HIDData.BIout_Sign 				= chRcvData[22];
				m_HIDData.BCurrent					= CharToInt(SubChar(chRcvData,Substr,23,5,sizeof(Substr)));
				m_HIDData.ConverterEfficiency		= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
				m_HIDData.PickupTemperature			= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
				m_HIDData.HeatsinkTemperature		= CharToInt(SubChar(chRcvData,Substr,34,2,sizeof(Substr)));
				m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,36,2,sizeof(Substr)));
				m_HIDData.ECap2Temperature			= CharToInt(SubChar(chRcvData,Substr,38,2,sizeof(Substr)));
				m_HIDData.ECapRoundTemperature		= CharToInt(SubChar(chRcvData,Substr,40,2,sizeof(Substr)));
				m_HIDData.CTransTemperature			= CharToInt(SubChar(chRcvData,Substr,42,3,sizeof(Substr)));
				m_HIDData.CMosFETPriTemperature		= CharToInt(SubChar(chRcvData,Substr,45,3,sizeof(Substr)));

				m_HIDData.CMOsFETSecTemperature		= CharToInt(SubChar(chRcvData,Substr,48,3,sizeof(Substr)));
				m_HIDData.Voltage5					= CharToInt(SubChar(chRcvData,Substr,51,2,sizeof(Substr)));
				m_HIDData.Current5					= CharToInt(SubChar(chRcvData,Substr,53,2,sizeof(Substr)));
				m_HIDData.Voltage24					= CharToInt(SubChar(chRcvData,Substr,55,3,sizeof(Substr)));
				m_HIDData.Current24					= CharToInt(SubChar(chRcvData,Substr,58,3,sizeof(Substr)));
				m_HIDData.Voltage27					= CharToInt(SubChar(chRcvData,Substr,61,3,sizeof(Substr)));
				m_HIDData.Current27					= CharToInt(SubChar(chRcvData,Substr,64,2,sizeof(Substr)));
				m_HIDData.Regulator_ver				= CharToInt(SubChar(chRcvData,Substr,66,3,sizeof(Substr)));
				m_HIDData.Fimware					= CharToInt(SubChar(chRcvData,Substr,69,3,sizeof(Substr)));
				m_HIDData.Soc_per					= CharToInt(SubChar(chRcvData,Substr,72,3,sizeof(Substr)));

				m_HIDData.Soc  						= CharToInt(SubChar(chRcvData,Substr,75,5,sizeof(Substr)));
				m_HIDData.Soh  						= CharToInt(SubChar(chRcvData,Substr,80,3,sizeof(Substr)));
				m_HIDData.BMS_Voltage				= CharToInt(SubChar(chRcvData,Substr,83,4,sizeof(Substr)));
				m_HIDData.Ref						= CharToInt(SubChar(chRcvData,Substr,87,3,sizeof(Substr)));
				m_HIDData.Current_Sign				= chRcvData[90];
				m_HIDData.Current					= CharToInt(SubChar(chRcvData,Substr,91,5,sizeof(Substr)));
				m_HIDData.Max_Temp					= CharToInt(SubChar(chRcvData,Substr,98,3,sizeof(Substr)));
				m_HIDData.Min_Temp					= CharToInt(SubChar(chRcvData,Substr,99,5,sizeof(Substr)));
				m_HIDData.PCBTemperature			= CharToInt(SubChar(chRcvData,Substr,102,5,sizeof(Substr)));
				m_HIDData.StartDate					= CharToInt(SubChar(chRcvData,Substr,105,6,sizeof(Substr)));

				m_HIDData.TotalTime					= SubChar(chRcvData,Substr,111,8,sizeof(Substr)); // Hex�� ���
				m_HIDData.TotalIac					= CharToInt(SubChar(chRcvData,Substr,119,7,sizeof(Substr)));
				m_HIDData.TotalIout					= CharToInt(SubChar(chRcvData,Substr,126,7,sizeof(Substr)));

				for(int i=0; i<11; i++){
					m_HIDData.CellVoltage[i]					= CharToInt(SubChar(chRcvData,Substr,133+(i*4),4,sizeof(Substr)));
				}
			}
			else
			{
				m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
				m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
				m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
				m_HIDData.Iout_sign 		   		= chRcvData[10];
				m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
				m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
				m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
				m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
				m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
				m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
				m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
				m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
				m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
				m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
				m_HIDData.Regulator_ver  			= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
				m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
				m_HIDData.Voltage12                 = CharToInt(SubChar(chRcvData,Substr,44,3,sizeof(Substr)));
				m_HIDData.Voltage5                  = CharToInt(SubChar(chRcvData,Substr,47,3,sizeof(Substr)));
				m_HIDData.Voltage12_Sign            = chRcvData[50];
				m_HIDData.Voltage12_Minus           = CharToInt(SubChar(chRcvData,Substr,51,3,sizeof(Substr)));
				m_HIDData.Voltage24                 = CharToInt(SubChar(chRcvData,Substr,54,3,sizeof(Substr)));
				m_HIDData.Voltage27                 = CharToInt(SubChar(chRcvData,Substr,57,3,sizeof(Substr)));
				m_HIDData.Current5                  = CharToInt(SubChar(chRcvData,Substr,60,3,sizeof(Substr)));
				m_HIDData.Current24                 = CharToInt(SubChar(chRcvData,Substr,63,3,sizeof(Substr)));
				m_HIDData.Current27                 = CharToInt(SubChar(chRcvData,Substr,66,3,sizeof(Substr)));
				m_HIDData.Current12                 = CharToInt(SubChar(chRcvData,Substr,69,3,sizeof(Substr)));
				m_HIDData.Current12_Minus           = CharToInt(SubChar(chRcvData,Substr,72,3,sizeof(Substr)));
				m_HIDData.SMPS_ErrorCode            = CharToInt(SubChar(chRcvData,Substr,75,2,sizeof(Substr)));
				m_HIDData.SMPS_Firmware             = CharToInt(SubChar(chRcvData,Substr,77,3,sizeof(Substr)));
				m_HIDData.Reserve2                  = CharToInt(SubChar(chRcvData,Substr,80,2,sizeof(Substr)));
				m_HIDData.Reserve3                  = CharToInt(SubChar(chRcvData,Substr,82,2,sizeof(Substr)));
			}
			nSerialCommCheck = 0;

			if(BMS_Ver)
			{
				// BMS ���� �Ŀ� ��� ����  ���� sg.lee 25.06.26
				switch (m_HIDData.BatteryStatus)
				{
					// ����
					case 'A': // ���� ��
					case 'B': // ���� ��
					case 'C': // �� �뷱�� ���� ��
					case 'D': // ���͸� SOC�� 15% ���Ϸ� ������ ��� ���� ��û
					case 'E': //���͸� SOC�� 10% ���� �� �˶� �߻� => 10% ���� 200s ���� �� ������ ���� ���� OFF
						break;

					case 'F':
						HID_LOG("!Warning! Cell �� ���� ���� 0.5V �̻�");
						break;

					case 'G':
						HID_LOG("!ERROR! CHARGE DISABLE");
						break;
					case 'H':
						HID_LOG("!ERROR! DISCHARGE DISABLE");
						break;

					case 'I':
						HID_LOG("!ERROR! OVER CURRENT");
						break;

					case 'J':
						HID_LOG("!ERROR! OVER TEMPERATURE");
						break;

					case 'K':
						HID_LOG("!ERROR! ���� �� Cell �� ���� ���� 0.55V �̻�");
						break;

					case 'L':
						HID_LOG("!ERROR! ���� �� Cell �� ���� ���� 0.55V �̻�");
						break;

					case 'M':
						HID_LOG("!ERROR! EMERGENCY!!!"); // Fault Error ����
						break;
					
					default:
						break;
				}

				/*
				if(m_HIDData.Fault_Code == NO_ERR)
				{
					//���͸� ������ �� Error ��ġ�� ������ ��� ���� �߻�
					if(m_HIDData.CD == 'C')
					{
						if(m_HIDData.Max >= 3600)
						{
							HID_LOG("!ERROR! �� ������ �߻�. ���� �� ����: %d", m_HIDData.Max);
							nError = ERR_CELL_OVERVOLTAGE;
						}

						if(m_HIDData.Bms >= 287)   //�Ҽ������� �޾ƿ��� ���� -> �Ҽ��� ����
						{
							HID_LOG("!ERROR! Pack ���� ������ �߻�. ���� �ִ� ����: %d", m_HIDData.Bms);
							nError = ERR_PACK_OVERVOLTAGE;
						}

						if(m_HIDData.Current_Sign == '+')
						{
							if(m_HIDData.Current >= 33)  //33 = 3.3, ������ ����ÿ� �Ҽ��� ǥ��X
							{
								HID_LOG("!ERROR! ���͸� ������  �߻�. ���� ���͸� ����: %d", m_HIDData.Current);
								nError = ERR_CHARGE_OVER;
							}
						}
					}
					else if(m_HIDData.CD == 'D')
					{
						if(m_HIDData.Bms <= 260)     //�Ҽ������� �޾ƿ��� ���� -> �Ҽ��� ����
						{
							HID_LOG("!ERROR! Pack ���� ������ �߻�. ���� �ּ� ����: %d", m_HIDData.Bms);
							nError = ERR_PACK_UNDERVOLTAGE;
						}

						if(m_HIDData.Current_Sign == '-')
						{
							if(m_HIDData.Current >= 121)  //������ �ڸ��� �Ѿ -> �ڸ��� �߰�
							{
								HID_LOG("!ERROR! ���͸� ������  �߻�. ���� ���͸� ����: %d", m_HIDData.Current);
								nError = ERR_DISCHARGE_OVER;
							}
						}
					}

					if(m_HIDData.Min <= 3170)
					{
						HID_LOG("!ERROR! �� ������ �߻�. ���� �� ����: %d", m_HIDData.Min);
						nError = ERR_CELL_UNDERVOLTAGE;
					}

					if(m_HIDData.Max_Temp_Sign == '+')
					{
						if(m_HIDData.Max_Temp >= 40)
						{
							HID_LOG("!ERROR! �� ���� �߻�. ���� �� �µ�: %d", m_HIDData.Max_Temp);
							nError = ERR_CELL_OVERTEMP;
						}
					}
				}
				else
				{
					HID_LOG("Battery Error Code: %d", m_HIDData.Fault_Code);
//					m_HIDData.ErrorCode = ERR_HID_WRITE;
				}
				*/
			}

			if(nError != NO_ERR)
			{
				m_HIDData.ErrorCode = nError;
				return 0; //������ �߻���Ű�� ���ַ����� ���� ���� ���� �ʿ�
			}
		}
	}
	else if(bV85Protocol == false)
	{
		if((GetRcvDataCheckSum(chRcvData, RCV_PK_LENGTH70)!=true) || (chRcvData[0]!='s'))
		{
			nSerialCommCheck++;
		}
		else
		{
	//	   HID_LOG("RawData:%s", chRcvData);
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)));
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
	//		m_HIDData.WarningCode 				= CharToInt(SubChar(chRcvData,23,2));
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Reserve2  				= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
			nSerialCommCheck = 0;
		}
	}
	else
	{
		HID_LOG("chRcvData[0] = %c", chRcvData[0]); //0�� �迭�� �� ���ִ���
		//recv ũ�⵵ �ٸ�.. RCV_PK_LENGTH70
		if(nRecvLen!=(RCV_PK_LENGTH_CP+1) || (GetRcvDataCheckSum_V85(chRcvData, RCV_PK_LENGTH_CP)!=true) || (chRcvData[0]!='s'))
		{
			nSerialCommCheck++;
		}
		else
		{
			m_HIDData.Edc 						= CharToInt(SubChar(chRcvData,Substr,1,3,sizeof(Substr)));;
			m_HIDData.Ecap 						= CharToInt(SubChar(chRcvData,Substr,4,3,sizeof(Substr)));
			m_HIDData.Eout  					= CharToInt(SubChar(chRcvData,Substr,7,3,sizeof(Substr)));
			m_HIDData.Iout_sign 		   		= chRcvData[10];
			m_HIDData.Iout 						= CharToInt(SubChar(chRcvData,Substr,11,3,sizeof(Substr)))
			m_HIDData.Iac 						= CharToInt(SubChar(chRcvData,Substr,14,3,sizeof(Substr)));
			m_HIDData.ErrorCode  			    = CharToInt(SubChar(chRcvData,Substr,17,2,sizeof(Substr)));
			m_HIDData.ErrorCode 				= ErrorConvert(m_HIDData.ErrorCode);    //puting  2013.11.12
			m_HIDData.PickupTemperature 		= CharToInt(SubChar(chRcvData,Substr,19,3,sizeof(Substr)));
			m_HIDData.IGBTTemperature			= CharToInt(SubChar(chRcvData,Substr,22,3,sizeof(Substr)));
			m_HIDData.ECap1Temperature			= CharToInt(SubChar(chRcvData,Substr,25,3,sizeof(Substr)));
			m_HIDData.ECap2Temperature 			= CharToInt(SubChar(chRcvData,Substr,28,3,sizeof(Substr)));
			m_HIDData.ECapRoundTemperature 		= CharToInt(SubChar(chRcvData,Substr,31,3,sizeof(Substr)));
			m_HIDData.Reserve1  				= CharToInt(SubChar(chRcvData,Substr,34,3,sizeof(Substr)));
			m_HIDData.Regulator_ver  			= CharToInt(SubChar(chRcvData,Substr,37,3,sizeof(Substr)));
			m_HIDData.Fimware  				 	= CharToInt(SubChar(chRcvData,Substr,40,4,sizeof(Substr)));
			nSerialCommCheck = 0;
		}
    }

	if(nSerialCommCheck > 3)
	{
		nSerialCommCheck = 0;
		memset(&m_HIDData, 0x00, sizeof(m_HIDData));
		nError = ERR_HID_READTIMEOUT;   //���� �����Ͱ� �̻��� ���.
	}
	//Recv Data ��¹�
	if(nSerialCommCheck == 0)
	{
		if(BMS_Ver)
		{
			HID_LOG("BMS Recv Data = %c,%c,%c,%c,%d,%d,%d,%c,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c,%d,%d,%d,%d,%d,%s,%d,%d",
					m_HIDData.InputStatus, m_HIDData.BMSStatus, m_HIDData.BatteryStatus, m_HIDData.ExCharge, m_HIDData.ErrorCode,
					m_HIDData.Iac, m_HIDData.BMS_Edc, m_HIDData.BMS_Iout_Sign, m_HIDData.BMS_Iout, m_HIDData.BVoltage, m_HIDData.BIout_Sign, m_HIDData.BCurrent,
					m_HIDData.ConverterEfficiency, m_HIDData.PickupTemperature, m_HIDData.HeatsinkTemperature, m_HIDData.ECap1Temperature, m_HIDData.ECap2Temperature, m_HIDData.ECapRoundTemperature,
					m_HIDData.CTransTemperature, m_HIDData.CMosFETPriTemperature, m_HIDData.CMOsFETSecTemperature,
					m_HIDData.Voltage5, m_HIDData.Current5, m_HIDData.Voltage24, m_HIDData.Current24, m_HIDData.Voltage27, m_HIDData.Current27,
					m_HIDData.Regulator_ver, m_HIDData.Fimware, m_HIDData.Soc_per, m_HIDData.Soc, m_HIDData.Soh, m_HIDData.BMS_Voltage, m_HIDData.Ref, m_HIDData.Current_Sign, m_HIDData.Current,
					m_HIDData.Max_Temp, m_HIDData.Min_Temp, m_HIDData.PCBTemperature, m_HIDData.StartDate, m_HIDData.TotalTime, m_HIDData.TotalIac, m_HIDData.TotalIout);

			HID_LOG("BMS Vcell Data = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.CellVoltage[0], m_HIDData.CellVoltage[1], m_HIDData.CellVoltage[2], m_HIDData.CellVoltage[3], m_HIDData.CellVoltage[4],
				m_HIDData.CellVoltage[5], m_HIDData.CellVoltage[6], m_HIDData.CellVoltage[7], m_HIDData.CellVoltage[8], m_HIDData.CellVoltage[9],
				m_HIDData.CellVoltage[10]);
		}
		else if(m_HIDData.Regulator_ver == 85)    //regulator ver85�� ��
		{
			HID_LOG("ver.85 CP Recv Data = %d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.Edc,m_HIDData.Ecap,m_HIDData.Eout,m_HIDData.Iout_sign,m_HIDData.Iout,m_HIDData.Iac,m_HIDData.ErrorCode,m_HIDData.PickupTemperature,
				m_HIDData.IGBTTemperature,m_HIDData.ECap1Temperature,m_HIDData.ECap2Temperature,m_HIDData.ECapRoundTemperature,m_HIDData.Regulator_ver,m_HIDData.Reserve1,
				m_HIDData.Fimware,m_HIDData.Voltage12,m_HIDData.Voltage5,m_HIDData.Voltage12_Sign,m_HIDData.Voltage12_Minus,m_HIDData.Voltage24,m_HIDData.Voltage27,
				m_HIDData.Current5,m_HIDData.Current24,m_HIDData.Current27,m_HIDData.Current12,m_HIDData.Current12_Minus,m_HIDData.SMPS_ErrorCode,
				m_HIDData.SMPS_Firmware,m_HIDData.Reserve2,m_HIDData.Reserve3);
		}
		else
		{
			HID_LOG("ver.81 CP Recv Data = %d,%d,%d,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				m_HIDData.Edc,m_HIDData.Ecap,m_HIDData.Eout,m_HIDData.Iout_sign,m_HIDData.Iout,m_HIDData.Iac,m_HIDData.ErrorCode,m_HIDData.PickupTemperature,
				m_HIDData.IGBTTemperature,m_HIDData.ECap1Temperature,m_HIDData.ECap2Temperature,m_HIDData.ECapRoundTemperature,m_HIDData.Regulator_ver,m_HIDData.Reserve1,
				m_HIDData.Fimware);
		}
	}

	//2023.01.03 sseung �������� �ý��� Regultor Version ����
	if(OHTMainForm->Version_Info.Regulator_ver == "" && !(chRcvData[40]=='0' && chRcvData[41]=='0' && chRcvData[42]=='0' && chRcvData[43]=='0'))
	{
		OHTMainForm->Version_Info.Regulator_ver.sprintf("%c.%c.%c.%c", chRcvData[40], chRcvData[41], chRcvData[42], chRcvData[43]);
        OHTMainForm->Version_Info.Regulator_Date.sprintf(OHTMainForm->strVersion_Date.c_str());
		HID_LOG("RegulatorFW_ver = %s", OHTMainForm->Version_Info.Regulator_ver);
	}
#endif
	LeaveCriticalSection(&m_CS);    //ũ��Ƽ�ü��� ����

	return nError;
}
/**
@brief   HIDData ����ü ��ȯ�ϴ� �Լ�.
@date    2013.03.14
@author  puting
@param
@return  HIDDATA ����ü ����
*/
HID_DATA HID_VII_ZR::GetHIDData()
{
	HID_DATA Temp_HIDData;

	EnterCriticalSection(&m_CS);
	Temp_HIDData = m_HIDData;
	LeaveCriticalSection(&m_CS);

	return Temp_HIDData;
}

char HID_VII_ZR::GetSendData_Type(int nType)
{
	char nChar ='D';

	switch(nType)
	{
		 case 0:
			nChar = 'D'; //���
		 break;
		 case 1:
			nChar = 'A'; //����
		 break;

		 case 2:
			nChar = 'C';  //����
		 break;
		 case 3:
			nChar = 'B'; //���
		 break;
	}

	return nChar;
}

char HID_VII_ZR::GetSendData_Path(int nType)
{
	char nChar ='N';

	switch(nType)
	{
		 case 0:
			nChar = 'N';   //�Ұ�
		 break;

		 case 1:
			nChar = 'E';    //����
		 break;

		 case 2:
			nChar = 'F';    //�
		 break;

		 case 3:
			nChar = 'G';     //����
		 break;
	}

	return nChar;
}

/**
@brief   HID 2������ ���� Data�� ��ȯ�ϴ� �Լ�
@date    2013.03.14
@author  puting
@param
@return  Send Data�� ��ȯ(16�ڸ�)
*/
void HID_VII_ZR::GetSendData(char *SendData,int nMoveInfo, int nPathInfo)
{
//	char SendData[20] ={0,};
	BYTE wCRCData;
	struct date today;
	struct time currenttime;
    int tmep;
	getdate(&today);
	gettime(&currenttime);

	//2023.12.08 sseung ECO �����, ���� ���� Ȯ�� �� �Է�
	bChargeEnable = m_pExecuteInfo->bChargeEnable;
	bError = m_pExecuteInfo->bErrorStatus;


	SendData[0] = chSTX;

	SendData[1] = 'S';

	//OHTSection Data
	SendData[2] = GetSendData_Type(nMoveInfo); //����

	//OHTTraveling Data
	SendData[3] =GetSendData_Path(nPathInfo); //����

	//OHT Time Stamp Year Data
	SendData[4] = (today.da_year /1000)+ 0x30;  //2013
	SendData[5] = ((today.da_year /100)%10)+ 0x30;  //2013
	SendData[6] = ((today.da_year /10)%10)+ 0x30;  //2013
	SendData[7] = (today.da_year % 10)+ 0x30 ;

	//OHT Time Stamp Month data
	tmep = today.da_mon;
	SendData[8] = (tmep / 10) + 0x30;
	SendData[9] = (tmep % 10) +0x30;

	//OHT Time Stamp Date Data
	tmep = today.da_day;
	SendData[10] = (tmep / 10) +0x30;
	SendData[11] = (tmep % 10) +0x30;

	//OHT Time Stamp Hour Data
	tmep = currenttime.ti_hour;
	SendData[12] = (tmep / 10) +0x30;
	SendData[13] = (tmep % 10) +0x30;

	//OHT Time Stamp Minute Data
	tmep = currenttime.ti_min;
	SendData[14] = (tmep / 10) +0x30;
	SendData[15] = (tmep % 10) +0x30;

	//OHT Time Stamp Second Data
	tmep = currenttime.ti_sec;
	SendData[16] = (tmep / 10) +0x30;
	SendData[17] = (tmep % 10) +0x30;

	if(BMS_Ver)         //���� ����� ���� ����,����� ���� ǥ�ø� ���� ���� �߰�
	{
		SendData[18] = 'R';

		if((bError == 0 && bChargeEnable == false && m_HIDData.Soc > 20) || m_HIDData.Soc > 80)    //���� ���� ���� Node�̰� ���͸� �ܷ��� 20% �ʰ��϶� �Ǵ� ���͸� �ܷ��� 80% �̻��� �� �������
		{
			SendData[18] = 'S';
		}
		
		SendData[19] = 'O'; // �ܺ� ���� ��� �߰� �ʿ� sg.lee 25.06.26

		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70);

		SendData[20] = wCRCData;
		SendData[21] = chETX;

		HID_LOG("HID bChargeEnable=%d, bError=%d", bChargeEnable, bError);
		if(bChargeEnable == false && (m_HIDData.Soc < 20 || bError != 0))  //���͸� 20%���� �Ǵ� �����߻��� ������ ���� Ȯ�ο�
			HID_LOG("����� ���� ������ Battery:%d", m_HIDData.Soc);

		HID_LOG("����͸� ������: %d/%d/%d/%c/%d/%d/%d/%d/%d/%d", m_HIDData.Soc,(100-m_HIDData.Soc), m_pExecuteInfo->ExecutePositionInfo.CurrNode,
				m_HIDData.Current_Sign, m_HIDData.Current, m_HIDData.Max, m_HIDData.Min, m_HIDData.Max_Temp, m_HIDData.Min_Temp, m_HIDData.Soh);
	}
	else
	{
		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);

		SendData[18] = wCRCData;
		SendData[19] = chETX;
	}

	//V80
//	wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);
//
//	SendData[18] = wCRCData;


//   HID_LOG("RawSendData:%s", SendData);
//	return SendData;

}

void HID_VII_ZR::GetSendData(char *SendData,int nMoveInfo, int nPathInfo, bool errorDataFlag)
{
//	char SendData[20] ={0,};
	BYTE wCRCData;
	struct date today;
	struct time currenttime;
    int tmep;
	getdate(&today);
	gettime(&currenttime);

	SendData[0] = chSTX;

	if(errorDataFlag) SendData[1] = 'E';
	else SendData[1] = 'S';

	//OHTSection Data
	SendData[2] = GetSendData_Type(nMoveInfo); //����

	//OHTTraveling Data
	SendData[3] =GetSendData_Path(nPathInfo); //����

	//OHT Time Stamp Year Data
	SendData[4] = (today.da_year /1000)+ 0x30;  //2013
	SendData[5] = ((today.da_year /100)%10)+ 0x30;  //2013
	SendData[6] = ((today.da_year /10)%10)+ 0x30;  //2013
	SendData[7] = (today.da_year % 10)+ 0x30 ;

	//OHT Time Stamp Month data
	tmep = today.da_mon;
	SendData[8] = (tmep / 10) + 0x30;
	SendData[9] = (tmep % 10) +0x30;

	//OHT Time Stamp Date Data
	tmep = today.da_day;
	SendData[10] = (tmep / 10) +0x30;
	SendData[11] = (tmep % 10) +0x30;

	//OHT Time Stamp Hour Data
	tmep = currenttime.ti_hour;
	SendData[12] = (tmep / 10) +0x30;
	SendData[13] = (tmep % 10) +0x30;

	//OHT Time Stamp Minute Data
	tmep = currenttime.ti_min;
	SendData[14] = (tmep / 10) +0x30;
	SendData[15] = (tmep % 10) +0x30;

	//OHT Time Stamp Second Data
	tmep = currenttime.ti_sec;
	SendData[16] = (tmep / 10) +0x30;
	SendData[17] = (tmep % 10) +0x30;

	if(BMS_Ver)         //���� ����� ���� ����,����� ���� ǥ�ø� ���� ���� �߰�
	{
		SendData[18] = 'R';

		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70);

		SendData[19] = wCRCData;
		SendData[20] = chETX;
	}
	else
	{
		wCRCData = CheckSUMData(&SendData[1], SEND_PK_LENGTH70 -1);

		SendData[18] = wCRCData;
		SendData[19] = chETX;
	}
}

/**
@brief   HID 2������ ���� Data�� CheckSum�� ����� �Լ�
@date    2013.03.14
@author  puting
@param
@return  BYTE ����
*/
BYTE HID_VII_ZR::CheckSUMData(char* SendData, int Len)
{
	WORD    wCalcValue = 0, wBuff = 0;
	int	i = 0;

	for(i = 0; i < Len; i++)    //checksumData�� 1~17���� 1~18������ Ȯ��
	{
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		wBuff = SendData[i] & 0xFF;
#else
		wBuff = SendData[i] & 0xFF;      //210609 ���� ����
#endif
		wCalcValue += wBuff;
	}
//	HID_LOG("wCalcValue=%06x, wCalcValue& 0x00FF=%06x, wCalcValue& 0xFF=%06x", wCalcValue, wCalcValue & 0x00FF, wCalcValue& 0xFF);
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	return wCalcValue & 0x00FF;
#else
	return wCalcValue & 0xFF;
#endif
}

/**
@brief   HID 2�������� ���� Data�� CheckSum�� �������� Ȯ���ϴ�
@date    2013.03.14
@author  puting
@param
@return  ������ ��� true, �������� ��� false ����
*/
bool  HID_VII_ZR::GetRcvDataCheckSum(char* RcvData, int Len)
{
 	WORD    wCalcValue = 0, wBuff = 0;
	int	i = 0;

	for(i = 0; i < Len; i++)
	{
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
		wBuff = RcvData[i] & 0x00FF;
#else
		wBuff = RcvData[i] & 0xFF;
#endif
		wCalcValue += wBuff;
	}
//	if(RcvData[Len] == (char)(wCalcValue & 0x00FF))        //if(RcvData[RCV_PK_LENGTH70-1] == (char)(wCalcValue & 0x00FF))
	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c, wCalcValue=%04x", Len, RcvData[0], RcvData[Len], (wCalcValue & 0xFF));
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	if(RcvData[RCV_PK_LENGTH70-1] == (char)(wCalcValue & 0x00FF))
		return true;
#else
	if(RcvData[Len] == (char)(wCalcValue & 0xFF))
		return true;
#endif
	else
		return false;
}

/**
@brief   HID 2������ ���� Data�� CheckSum�� ����� �Լ�
@date    2013.03.14
@author  puting
@param
@return  BYTE ����
*/
/*
char HID_VII_ZR::CheckSUMData_V85(char* SendData, int Len, int Num)
{
	WORD    wCalcValue = 0, wBuff = 0;
	char Tmep[10] = {0,};

	int	i = 0;

	for(i = 0; i < Len; i++)
	{
		wBuff = SendData[i] & 0x00FF;

		wCalcValue += wBuff;

	}

	if(Num ==1)
		sprintf(Tmep, "%x", (wCalcValue & 0x00F0)>>4);
	else
		sprintf(Tmep, "%x", (wCalcValue & 0x000F));

	return Tmep[0];
}
*/
bool  HID_VII_ZR::GetRcvDataCheckSum_V85(char* RcvData, int Len)
{
	WORD    wCalcValue = 0, wBuff = 0;
	char Tmep[10] = {0,};
	bool breturn = false;

	char Check1, Check2;
	int	i = 0;

	for(i = 0; i < Len; i++)
#if((OHT_NAME == OHT_NAME_STD_V80)||(OHT_NAME == OHT_NAME_STD_V81)||(OHT_NAME == OHT_NAME_STD_V81S)||(OHT_NAME == OHT_NAME_STD_V85)||(OHT_NAME == OHT_NAME_STD_V80_RETICLE))
	{
		wBuff = RcvData[i] & 0x00FF;
		wCalcValue += wBuff;
	}
	sprintf(Tmep, "%x", (wCalcValue & 0x00F0)>>4);
	Check1 = Tmep[0];
	sprintf(Tmep, "%x", (wCalcValue & 0x000F));
	Check2 = Tmep[0];

	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c", Len, RcvData[0], RcvData[Len]);
	if((RcvData[76] == Check1) && (RcvData[77] == Check2))
		return true;
#else
	{
		wBuff = RcvData[i] & 0xFF;
		wCalcValue += wBuff;
	}

	sprintf(Tmep, "%x", (wCalcValue & 0xF0)>>4);
	Check1 = Tmep[0];
	sprintf(Tmep, "%x", (wCalcValue & 0x0F));
	Check2 = Tmep[0];

	HID_LOG("Len=%d, RcvData[0]=%c, RcvData[Len]=%c, wCalcValue=%c", Len, RcvData[0], RcvData[Len], (char)(wCalcValue & 0xFF));  //wCalcValue=%04x
	if(RcvData[Len] == (char)(wCalcValue & 0xFF))
		return true;
#endif
	else
		return false;
}

/**
@brief   HID 2������ ���� Data �� Error Code�� ���ο� �°� ��ȯ
@date    2013.11.12
@author  puting
@param
@return  �����ڵ�
*/
int HID_VII_ZR::ErrorConvert(int nerrorcode)
{
	int nErrorCode = 0;
	switch(nerrorcode)
	{
	case 1:
		nErrorCode = ERR_EDC_PEAKVOLTAGE;
		break;
	case 2:
		nErrorCode = ERR_EDC_OVERVOLTAGE;
		break;
	case 3:
		nErrorCode = ERR_EOUT_PEAKVOLTAGE;
		break;
	case 4:
		nErrorCode = ERR_EOUT_OVERVOLTAGE;
		break;
	case 5:
		nErrorCode = ERR_ECAP_PEAKVOLTAGE;
		break;
	case 6:
		nErrorCode = ERR_ECAP_OVERVOLTAGE;
		break;
	case 7:
		nErrorCode = ERR_IAC_PEAKCURRENT;
		break;
	case 8:
		nErrorCode = ERR_IAC_OVERCURRENT;
		break;
	case 9:
		nErrorCode = ERR_OUTPUT_PEAKCURRENT;
		break;
	case 10:
		nErrorCode = ERR_OUTPUT_OVERCURRENT;
		break;
	case 11:
		nErrorCode = ERR_OUTPUT_FUSE_OPEN;
		break;
	case 12:
		nErrorCode = ERR_ECAP_FUSE_OPEN;
		break;
	case 13:
		nErrorCode = ERR_PICKUP_PEAKTEMP;
		break;
	case 14:
		nErrorCode = ERR_IGBT_PEAKTEMP;
		break;
	case 15:
		nErrorCode = ERR_ECAP_PEAKTEMP;
		break;
	case 16:
		nErrorCode = ERR_GATE_DRIVER_FAULT;
		break;
	case 17:
		nErrorCode = ERR_HEATSINK_OVERTEMP;
		break;
	case 18:
		nErrorCode = ERR_PROTECT_BD_CABLE_OPEN;
		break;
	case 19:
		nErrorCode = ERR_OUTPUT_PEAKPOWER;
		break;
	case 20:
		nErrorCode = ERR_OUTPUT_OVERPOWER;
		break;
	case 22:
		nErrorCode = ERR_RES_CAP_PEAKTEMP1;
		break;
	case 23:
		nErrorCode = ERR_RES_CAP_PEAKTEMP2;
		break;
	case 24:
		nErrorCode = ERR_F_CAP_PEAKTEMP;
		break;
	case 25:
		nErrorCode = ERR_F_L_PEAKTEMP;
		break;
	case 26:
		nErrorCode = ERR_TB_PEAKTEMP;
		break;
	case 30:
		nErrorCode = ERR_ECO_OV;
		break;
	case 37:
		nErrorCode = ERR_HEATSINK_PEAKTEMP;
		break;
	case 51:
		nErrorCode = ERR_EDC_UNDERVOLTAGE;
		break;
	case 52:
		nErrorCode = ERR_EDC_LOWVOLTAGE;
		break;
	case 53:
		nErrorCode = ERR_EOUT_UNDERVOLTAGE;
		break;
	case 54:
		nErrorCode = ERR_EOUT_LOWVOLTAGE;
		break;
	case 55:
		nErrorCode = ERR_ECAP_UNDERVOLTAGE;
		break;
	case 56:
		nErrorCode = ERR_ECAP_LOWERVOLTAGE;
		break;
	case 57:
		nErrorCode = ERR_PICKUP_OVERTEMP;
		break;
	case 58:
		nErrorCode = ERR_IGBT_OVERTEMP;
		break;
	case 59:
	case 69:
	case 70:
	case 71:
	case 72:
	case 73:
	case 74:
		nErrorCode = ERR_ECAP_OVERTEMP;
		break;
	case 60:
		nErrorCode = ERR_PEAKCURRENT_WARN;
		break;
	case 62:
		nErrorCode = ERR_SEG_GEN_SIGNAL;
		break;
	case 63:
	case 90:
		nErrorCode = ERR_MULTI_SMPS_FAULT;
		break;
	case 64:
	case 65:
	case 75:
	case 76:
	case 77:
	case 78:
	case 79:
	case 80:
		nErrorCode = ERR_REGUL_OPEN;
		break;
	case 84:
		nErrorCode = ERR_CAP_BANK_DISCONNECT;
		break;
	default:
		nErrorCode = 0;
		break;
	}

	if(nErrorCode != NO_ERR)
	{
		ADD_FDC_LOG("ERR/%d/1/5/%06d/%06d/%d/%d/%06d",
					nErrorCode,
					m_pOHTMainStatus->StatusCommon.CurNodeID,
					m_pExecuteInfo->ExecutePositionInfo.NextNode,
					m_pOHTMainStatus->StatusCommon.CurNodeOffset,
					m_pOHTMainStatus->StatusCommon.ExistFoup,
					m_pOHTMainStatus->StatusCommon.StopStationID);
	}

	return nErrorCode;

}


// ���ַ����� ������ TP�� ���� �α� ������ �����ϸ�, �� �Լ��� ȣ��Ǿ� �α׸� �����Ѵ�
// �α� ������ �� 5�а� �ɸ���, �� ���� �ٸ����� �ϸ� �ȵȴ�.
#define HID_ERROR_NO_ERR				0				// ������ �������� �Ϸ�
#define HID_ERROR_SERIAL_CAN_T_OPEN 	ERR_HID_OPEN	// �ø�����Ʈ�� ���� ������
#define HID_ERROR_SERIAL_WRITE			ERR_HID_WRITE
#define HID_ERROR_SERIAL_TIMEOUT		333


int HID_VII_ZR::SaveErrorDataLog(void)
{
	char chSendBuff[255]={0,};
	volatile char chReadBuff[1024]={0,};
	char chDataEndBuff[8]={0,};
	int nError=HID_ERROR_NO_ERR;
	bool bErrorRequest = 1;
	ofstream mfsb;

	String strFileName = "D:\\log\\HIDLogDump"+Now().FormatString("yyyymmddhhnn")+".dat" ;
	mfsb.open(strFileName.c_str(),ios::out|ios::binary);

	if(mfsb.is_open()){
		//PurgeComm(m_HIDCommPort->m_hCOM , PURGE_TXCLEAR|PURGE_RXCLEAR);
		GetSendData(chSendBuff, 0, 0, bErrorRequest);
				  //	memmove(chSendBuff,chWriteBuff, SEND_PK_LENGTH70+2);
		nError = Write(chSendBuff);

		if(nError!=NO_ERR){
			OHTMainForm->memoMain->Lines->Add("Write fail...");
			return nError;
		}
	}

	Sleep(50);

	volatile char chBuff;
	DWORD		dwStartTime = timeGetTime();
	DWORD		dwEndTime;
	OHTMainForm->memoMain->Lines->Add("file open");
	int index = 0;

	while(1){

		if(m_HIDCommPort->ReadCommBlock((char*)&chBuff,1) == 0) {
			Sleep(10);

			// �о����� �ʾҴٸ� ���� �ð��� ���� ����
			dwEndTime = timeGetTime();
			//AOHC-288
			if (OHTUTILITY::PreventOverflowGetTimeMs(dwStartTime) > 3000) // �о����� ���� �ð��� 100ms �̻��̸� Ÿ�Ӿƿ�   //20191104 KDS
			{
				OHTMainForm->memoMain->Lines->Add("Time Out");
				nError = HID_ERROR_SERIAL_TIMEOUT;
				break;
			}
			else
				continue;
		}

		// �����Ͱ� �о����ٸ� ���ο� �ð��� ����
		dwStartTime = timeGetTime();

		chReadBuff[index++] = chBuff;



		memcpy(&chDataEndBuff[1], &chDataEndBuff[0], sizeof(chDataEndBuff)-1);
		chDataEndBuff[0] = chBuff;
		if(chDataEndBuff[7] == 'D' &&
		   chDataEndBuff[6] == 'A' &&
		   chDataEndBuff[5] == 'T' &&
		   chDataEndBuff[4] == 'A' &&
		   chDataEndBuff[3] == 'E' &&
		   chDataEndBuff[2] == 'N' &&
		   chDataEndBuff[1] == 'D' &&
		   chDataEndBuff[0] == chETX){
		   OHTMainForm->memoMain->Lines->Add("Write Success");
			mfsb.write((char*)chReadBuff, index);
			break;
		}
		if(index == 1024)
		{
			mfsb.write((char*)chReadBuff, 1024);
			index = 0;
		}
	}
	mfsb.close();
	// OHTMainForm->tmHID->Enabled = false;
	OHTMainForm->memoMain->Lines->Add("file close");

	return nError;
}

//CP ���� Regulator ����� �� - sseung
void HID_VII_ZR::SetRegulatorCheck(bool bUse)
{
	HID_LOG("RegulatorCheck=%d",bUse);
	RegulatorCheck = bUse;

	if(!RegulatorCheck)
		GetVer = true;     //regulator ���� Ȯ���� �ʿ� ������ true�� ��ȯ

	HID_LOG("RegulatorCheck=%d, GetVer=%d, BMS_Ver=%d",RegulatorCheck, GetVer, BMS_Ver);
}

/////////////////////////////////////////////////////////////////////////////////////
//���Ŀ� �߰������� �ʿ��Ѱ� ���� ���� Ŭ���� �� �Լ�ȭ�� ���� HID6.5/7.0�� ���� �з�

