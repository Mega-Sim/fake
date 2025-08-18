//---------------------------------------------------------------------------

#ifndef NetworkSignallibH
#define NetworkSignallibH
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "NetworkAdapterControl.h"
#include "../Common/PUtily.h"
#include "../Common/Def_DataType.h"

#include "LogHandler.h"	// Log �ۼ� ����
#include "Def_DefaultIniParam.h"
#pragma comment (lib, "NetworkAdapterControl.lib")
 //������ OHT cpp�� USELIB("NetworkAdapterControl.lib")�� ���� ���̺귯�� ��� �㰡. puting

#define ERR_NETWORKAPDATER_INIT		777
#define ERR_NETWORKAPDATER_NOTOPEN	888

#define IPC_EZUNE_1st_Length 40
#define IPC_EZUNE_2nd_Length 39

//#define MAX_PATH	1024


 /**
@brief   NetworkSinal ���̺귯��
@author  puting
@date    2013.03.26
*/
class NetworkSinalLib
{
private:
	LogUnit *pLogWirelessNet;    				  ///< pLogWirelessNet �α� ������ ���� ����
	bool	m_bNETWORKReady;						   ///< NetWorkAdapter ��� ������ ����
	int     m_nNetworkAdapterIndex;    				   ///< NetWorkAdapter Index
	INetConnectInfo m_Info;
	DEFAULT_PARAM_SET *m_defualtparam;
	bool	m_bDisconnectFlag;						   ///< Nerwork Adapter Disable ���� Flag
	bool	_getDisconnectFlag()
	{
		return this->m_bDisconnectFlag;
	}
	void 	_setDisconnectFlag(bool _flag)
	{
		EnterCriticalSection(&m_FlagCS);
		m_bDisconnectFlag = _flag;
		LeaveCriticalSection(&m_FlagCS);
	}
	CRITICAL_SECTION m_FlagCS;

public:
	int NETWORKInit(void);             				   ///< NetWork �ʱ�ȭ
	void NETWORKClose(void);             				   ///< NetWork ����
	__property bool DisconnectFlag = {write = m_bDisconnectFlag, read = m_bDisconnectFlag};


public:
	NetworkSinalLib();
	~NetworkSinalLib();

	int Network_Adapter_Enable(void);       		  ///< Network Enable
	int NetWork_Adapter_Disable(void);                ///< Network Disable
	int Get_WLanInfo(APSIGNAL_DATA* APSignalData);    ///< Lan Info ���

};
#endif
