//---------------------------------------------------------------------------
/**
@file    NetworkSignallib.cpp
$Rev:: 467           $:  Revision of last commit
$Author:: puting   $:  Author of last commit
$Date:: 2013-03-26 1#$:  Date of last commit
@version OHT ver
@brief   NetworkSignallib Class �ҽ� File
*/

#include "NetworkSignallib.h"
#include "OHTMain.h"

//#define WirelessNetwork_LOG(...)	WRITE_LOG_ON_THIS_FILE("D:\\log\\WirelessNet.log", __VA_ARGS__)
//����
#define WirelessNetwork_LOG(...)   WRITE_LOG_UNIT(pLogWirelessNet, __VA_ARGS__)


#define ERRORNetConnectInfo 8
#define ConnectAP			0
#define INetInfoPointNull 	-1
#define DisconnectAP 		-2
#define INIT_STATUS			-3
//---------------------------------------------------------------------------
/**
@brief   NETWORK Library ������
@author  puting
@date    2013.03.25
*/
NetworkSinalLib::NetworkSinalLib()
{
	pLogWirelessNet = getLogUnit("D:\\log\\WirelessNet.log",MAX_BYTE_SIZE_OF_MAIN_LOG);
	//NETWORKInit();
	//DisconnectFlag = false;
	m_bNETWORKReady = false;
	m_bDisconnectFlag = false;
	m_nNetworkAdapterIndex = -1;
	memset(&m_Info, 0x00, sizeof(INetConnectInfo));
	m_defualtparam = OHTMainForm->GetDefaultParam();
}
/**
@brief   NETWORK Library �Ҹ���
@author  puting
@date    2013.03.25
*/
NetworkSinalLib::~NetworkSinalLib()
{
	/// @todo	���� �ʿ信 ���� Sync Gain�����ϴ� ��� �߰�
	NETWORKClose();


}
/**
@brief   NETWORK �ʱ�ȭ �Լ�
@author  puting
@date    2013.03.25
@return  nError
*/
int NetworkSinalLib::NETWORKInit(void)
{
	int nError =0;
	int iNumberOfDevice = 0;
	char* strDevName="";
	LPWSTR lpDevName;

	INetInfo* pInfo;
	m_nNetworkAdapterIndex =-1;
	m_bNETWORKReady = false;

	iNumberOfDevice = NetworkAdapterControl_Initialize();

	WirelessNetwork_LOG("Network Device Num : %d",  iNumberOfDevice);

	if(iNumberOfDevice < 0)
	{
		m_nNetworkAdapterIndex=-1;
		m_bNETWORKReady = false;
		DisconnectFlag = true;
		nError = ERR_NETWORKAPDATER_INIT;
	}
	else
	{
	  for(int i = 0; i < iNumberOfDevice; i++)
	  {
		pInfo = NetworkAdapterControl_GetINetInfo(i);
		lpDevName = pInfo->szDevName;

		int nLen = wcslen(lpDevName);
		strDevName = (char*)malloc(sizeof(char) * nLen);

		WCharToChar(lpDevName, strDevName);

		WirelessNetwork_LOG("Net Name : %s", strDevName);

#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
		if(my_strstr(strDevName, "Wireless")!=NULL)
		{
			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
#else
        // FXE-3000 ���� Client�� ��� Ethernet ��Ʈ�� ����
		// FXE-3000 Ethernet ��Ʈ �̸� : Realtek PCIe FE Family Controller
		// IOT Ethernet ��Ʈ �̸� : Realtek RTL8139 Family PCI Fast Ethernet NIC
		AnsiString strTmp;
		strTmp = strDevName;
		strTmp = strTmp.UpperCase();
		WirelessNetwork_LOG("Net Name Upper : %s, %d", strTmp, sizeof(strTmp) );
#if((OHT_NAME == OHT_NAME_STD_V85S)||(OHT_NAME == OHT_NAME_STD_V82)||(OHT_NAME == OHT_NAME_STD_V86)||(OHT_NAME == OHT_NAME_P4_MASK))
		if((m_defualtparam->IPCType == IPC_EZUNE_1st)&&(strTmp.AnsiPos("INTEL(R) I211 GIGABIT NETWORK CONNECTION") != 0)&& (strlen(strDevName) == IPC_EZUNE_1st_Length/*40*/) )  // ���� IPC ������ī�� ��Ī :Intel(R) I211 Gigabit Network Connection
		{
			WirelessNetwork_LOG("Controller found!, i : %d", i);
			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
		else if((m_defualtparam->IPCType == IPC_EZUNE_2nd)&&(strTmp.AnsiPos("INTEL(R) ETHERNET CONTROLLER (3) I225-V") != 0)&& (strlen(strDevName) == IPC_EZUNE_2nd_Length/*39*/))    // �ű� IPC ������ī�� ��Ī :Intel(R) Ethernet Controller (3) I225-V
		{
			WirelessNetwork_LOG("Controller found!, i : %d", i);

			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
		else if((m_defualtparam->IPCType == IPC_SOM6869)&&(strTmp.AnsiPos("CONNECTION") != 0))    // som 6869
		{
			WirelessNetwork_LOG("Controller found!, i : %d", i);

			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
		else if((m_defualtparam->IPCType == IPC_SOM4466)&&(strTmp.AnsiPos("CONTROLLER") != 0))     //�ű�, ���� ���� IPC, som6869 �������� �𸣴� ��� som4466���� ����
		{

			WirelessNetwork_LOG("Controller found!, i : %d", i);

			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
#else
		if(strTmp.AnsiPos("CONTROLLER") != 0)
		// if(my_strstr(strDevName, "Controller")!=NULL)
		{
        	WirelessNetwork_LOG("Controller found!");

			// 2016.12.19 sh17.jo ������Ʈ �ν� ���� Ȯ��
			// ���� ��Ʈ�� 2�� �νĵǹǷ� 2���� ���ؼ� Enable �ʿ� (���� ����)
			m_nNetworkAdapterIndex = i;
			m_bNETWORKReady = true;
			free(strDevName);
			break;
		}
#endif
#endif
      free(strDevName);
	  }

	  if(m_bNETWORKReady==true)
	  {
		nError = Network_Adapter_Enable();
		DisconnectFlag = false;
	  }
	  else
		nError = ERR_NETWORKAPDATER_INIT;
	}

#if(WirelessClient_TYPE == XR5_WIRELESS_CLIENT)
	if(nError==0)
	{
		WirelessNetwork_LOG("Wireless NetworkAdapter Init Succeced!");
	}
	else
	{
		WirelessNetwork_LOG("Wireless NetworkAdapter Init Failed.. ERR_NETWORKAPDATER_INIT");
	}
#else
	if(nError==0)
	{
		WirelessNetwork_LOG("Wire NetworkAdapter Init Succeced!");
	}
	else
	{
		WirelessNetwork_LOG("Wire NetworkAdapter Init Failed.. ERR_NETWORKAPDATER_INIT");
	}
#endif
	return nError;
}
//----------------------------------------------------------------------------
/**
@brief   NETWORK ���� �Լ�
@date    2013.03.26
@param   m_bNETWORKReady
@return
*/
void NetworkSinalLib::NETWORKClose(void)
{
	m_bNETWORKReady = false;
	m_nNetworkAdapterIndex = -1;
	NetWork_Adapter_Disable();
	NetworkAdapterControl_DeInitialize();
}
//----------------------------------------------------------------------------
/**
@brief   Network_Adapter_Enable �Լ�
@date    2013.03.26
@param   g_nNetworkAdapterIndex
@return  nError
*/
int NetworkSinalLib::Network_Adapter_Enable(void)
{
   int nError=0;

   if(m_nNetworkAdapterIndex!=-1)
   {
	  nError = NetworkAdapterControl_ControlAdaptor(m_nNetworkAdapterIndex, TRUE);
   }

   return nError;

}
//----------------------------------------------------------------------------
/**
@brief   NetWork_Adapter_Disable �Լ�
@date    2013.03.26
@param   g_nNetworkAdapterIndex
@return  nError
*/
int NetworkSinalLib::NetWork_Adapter_Disable(void)
{
   int nError=0;

   if(m_nNetworkAdapterIndex!=-1)
   {
	  nError = NetworkAdapterControl_ControlAdaptor(m_nNetworkAdapterIndex, FALSE);
   }

   return nError;

}
//------------------------------------------------------------------------------
/**
@brief   WLanInfo ��� �Լ�
@date    2013.03.26
@param   MaxAddress, APSignal, APChannel, APSiganlLevel, SSID
@return  �����̸� true, ���� �߻� �� false ��ȯ
*/
int NetworkSinalLib::Get_WLanInfo(APSIGNAL_DATA* APSignalData)
{
	int nWirelessStatus = INIT_STATUS;
	memset(&m_Info, 0x00, sizeof(INetConnectInfo));
	nWirelessStatus = NetworkAdapterControl_GetINetConnectInfo(m_nNetworkAdapterIndex, &m_Info);
	if(nWirelessStatus == ConnectAP)
   {
		//����ü�� SSID Data ����
		strcpy(APSignalData->szSSID, m_Info.szSSID);
		trim(APSignalData->szSSID);

		//����ü�� APMaxAddress ����
		memmove(&APSignalData->szAPMacAddress, m_Info.cAPMAC, sizeof(m_Info.cAPMAC));

		//����ü�� APä�� �� �ñ׳�  ����
		APSignalData->nAPSignal = m_Info.iRssi;
		APSignalData->nChannel = m_Info.iChannel;
		APSignalData->nAPSignalLevel  = m_Info.iRssi;

		if(m_Info.iRssi > -40)
			APSignalData->nAPSignalLevel = -40;
		else if(m_Info.iRssi < -90)
			APSignalData->nAPSignalLevel = -90;

		//����ü�� AP�ñ׳η���  ����
		APSignalData->nAPSignalLevel = (int)(((double)(APSignalData->nAPSignalLevel - (-90))) / ((-40) - (-90)) * 10 + 0.5);
		return NO_ERR;
   }
   else if(nWirelessStatus == DisconnectAP)
   {
		//����ü�� SSID Data ����
		// SSID : Null

		//����ü�� APMaxAddress ����
		// Mac Address : 00-00-00-00-00-00

		//����ü�� APä�� �� �ñ׳�  ����
		APSignalData->nChannel = 0;
		APSignalData->nAPSignalLevel  = 0;
		return NO_ERR;
   }
   else
		return ERRORNetConnectInfo;
}
//------------------------------------------------------------------------------\
