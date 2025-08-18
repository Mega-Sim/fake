//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include "AutoTeaching.h"
#include "OHTMain.h"

#define ADD_LOG(...)        WRITE_LOG_UNIT(pLogAutoTeaching, __VA_ARGS__)
//---------------------------------------------------------------------------

/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
AutoTeaching::AutoTeaching()
{
	m_pVirtualDrivingControl    = OHTMainForm->GetDrivingControl();
	m_pHWCommon         = OHTMainForm->GetHWCommon();
	m_pHWSet            = OHTMainForm->GetHWSet();

	m_pExecuteInfo = OHTMainForm->GetExecuteInfo();

	//Jig ��� �κ� �ʱ�ȭ
	bListenMode = false;
	ServerSock = NULL;


	SocketInit();

	iCmd =  CMD_INT;

	fHoist = 0.0;
	fSlide = 0.0;
	fDrive = 0.0;

	dHoist_Offset = 0.0;
	fSlide_Offset = 0.0;
	fDrive_Offset = 0.0;

	Total_fDrive_Offset = 0.0;

	dSTB_R_Offset = 0.0;
	dSTB_L_Offset = 0.0;
	nResult = 0;

	dwAutoTeachingResponseTime = timeGetTime();

	bSTBTeaching = false;

	memset(cATIpAddr, 0x00, sizeof(cATIpAddr));

	Load_AutoTeaching_Param();

//	dSTB_R_Offset =  defualtOffset.dSTB_R_Shift_Offset;
//	dSTB_L_Offset =  defualtOffset.dSTB_L_Shift_Offset;

	pLogAutoTeaching = getLogUnit("D:\\log\\AutoTeaching.log", 1024*2);

}

/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
AutoTeaching::~AutoTeaching()
{
   SocketClose();
}

int AutoTeaching::SocketInit()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
	else
		return 0;
}
/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
void AutoTeaching::SocketClose()
{
	WSACleanup();
	bListenMode = false;
	memset(cATIpAddr, 0x00, sizeof(cATIpAddr));
}
// bind�� �õ��ϰ�, �����ϸ� true�� �����Ѵ�
//����Ǿ� �մ� ���, ������ ���� ������.
/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
int AutoTeaching::ServerBind(int iPort, char* cATAddr)
{
	int retval = NO_ERR;


	if (bListenMode == true)
	{
		return retval;
    }

	// socket()
	ServerSock = socket(AF_INET, SOCK_DGRAM, 0);
	if(ServerSock == INVALID_SOCKET)
		return -1;

	// ����ŷ
	u_long on = 1;
	retval = ioctlsocket(ServerSock, FIONBIO, &on);
	if(retval == SOCKET_ERROR)
	{
		ServerClose();
		return -2;
	}


	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(iPort);
	retval = bind(ServerSock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR)
	{
		ServerClose();
		return -2;
	}

	bListenMode = true;		// ������ ���� ���� ����
	memmove(cATIpAddr, cATAddr, strlen(cATAddr)); //Return Value

	SetAutoTeachingJigResponse();

	dHoist_Offset = 0.0;
	fSlide_Offset = 0.0;
	fDrive_Offset = 0.0;

	Total_fDrive_Offset = 0.0;


	return retval;
}
/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
bool AutoTeaching::IsConnect()
{
  	return  bListenMode;
}

/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
bool AutoTeaching::IsCheckIPCompare(char *strIP)
{
	bool breturn = false;

	if(memcmp(&(defualtOffset.AutoTeachingIP), strIP, strlen(defualtOffset.AutoTeachingIP)) == 0)
	 	breturn = true;

	return  breturn;
}
/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
int  AutoTeaching::ServerClose()
{

	dHoist_Offset = 0.0;
	fSlide_Offset = 0.0;
	fDrive_Offset = 0.0;

	Total_fDrive_Offset = 0.0;


	closesocket(ServerSock);
	bListenMode = false;

	return 0;
}
/**
@brief   AutoTeaching
@author  puting
@date    2016.12.19
@note
*/
int AutoTeaching::UnicastSendMessage(char* cIpAddr, int iPort, char* buf, int len)
{
	int retval = NO_ERR;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET)
	{
		ADD_LOG("Broadcast Sender - Invalid_Socket");
    	return -99;
	}
	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	// remoteaddr.sin_addr.s_addr = inet_addr(REMOTEIP);
	remoteaddr.sin_addr.s_addr = inet_addr(cIpAddr);
	remoteaddr.sin_port = htons(iPort);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;

	retval = sendto(sock, buf, len, 0,
		(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));

	if(retval == SOCKET_ERROR){
		return -1;
	}


	closesocket(sock);


	return retval;
}

//���°� ���ŵ� �ش� �κп��� ó�� ���� ��.
//���� ������Ʈ �ϴ� �κ�
int AutoTeaching::UpdateATData()
{
	int nError = NO_ERR;

	int iReturn = 0, iLen = 0, iClientPort = 0;
	char cBuffer[256] ={0,};
    char szTemp[256]={0,};
	//�����ð����� ������ ����� ���� ��� ���¿�û ����ٰ�.

	iReturn = ServerListen(cBuffer, iLen, iClientPort);

	String strReceive = cBuffer;
//	strReceive.sprintf(L"%s",cBuffer);

	// ���� ���� ��
	if (iReturn > 0)
	{
		ADD_LOG("RecvData : %s(%s)  Len:%d", cBuffer,strReceive, iReturn);



  		String strATData;
		String strATDriveData;
		String strATHoistData;
		String strATShiftData;
		String strATRotateData;
		int iATLength, iAT2Length, iAT3Length, iAT4Length;
		strATData = strReceive;
		strATData.Trim();


		// Drive Direction :
		iATLength  = strReceive.Length();
		iAT2Length = strReceive.Pos(",");
		strATDriveData = strReceive.SubString(1, (iAT2Length - 1));
		fDrive = StrToFloat(strATDriveData);
		if(fabs(fDrive) > 999.0)
		{
			fDrive = 0.0;  //0���� ����°Ը´���???
		}

		// Slide Direction :
		strReceive  = strReceive.Delete(1, iAT2Length);
		iATLength  = strReceive.Length();
		iAT3Length = strReceive.Pos(",");
		strATShiftData = strReceive.SubString(1, (iAT3Length - 1));
		fSlide = StrToFloat(strATShiftData);
		if(fabs(fSlide) > 999.0)
		{
			fSlide = 0.0;  //0���� ����°Ը´���???
		}


		// Hoist Direction :
		strReceive  = strReceive.Delete(1, iAT3Length);
		iATLength  = strReceive.Length();
		iAT4Length = strReceive.Pos(",");
		strATHoistData = strReceive.SubString(1, (iAT4Length - 1));
		fHoist = StrToFloat(strATHoistData);
		if(fabs(fHoist) > 999.0)
		{
			fHoist = 0.0;  //0���� ����°Ը´���???
		}

		// Result :
		strReceive  = strReceive.Delete(1, iAT4Length);
		iATLength  = strReceive.Length();
		nResult = StrToInt(strReceive.c_str());

		ADD_LOG("parsData : D:%f, S:%f, H:%f Re:%d", fDrive,fSlide, fHoist, nResult);

		if(nResult == RESPONCE_COMPLETE)
			iCmd = CMD_RESPONSE;
		else if(nResult == STATUS_M_READY)
			iCmd = CMD_READY;
		else if(nResult == RESPONCE_READY)
			iCmd = CMD_READY;

		if((nResult != RESPONCE_ERROR)&& (nResult != STATUS_V_ERROR) && (nResult != STATUS_O_ERROR))
			SetAutoTeachingJigResponse();

	}

	if(OHTUTILITY::PreventOverflowGetTimeMs(dwAutoTeachingResponseTime) > 60000)
	{
     	nError = ERR_AUTOTEACHING_TIMEOUT;
    }

	return nError;

}
// ���Ͽ� ���� ���� �ֳ� Ȯ���ϰ�, ������ ���� ����Ʈ���� �����Ѵ�.
// ���� : UDP�̹Ƿ�, �ѹ��� �ϳ��� ��Ŷ�� �����ش�.
//        ���� �ѹ� �о��� �ؼ�, ��� ������ �������� �ƴϴ�. (���۰� �ִ�)
int  AutoTeaching::ServerListen(char* buf, int& iLen, int& iClientPort)
{
	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval = NO_ERR;
	char tmpA[256];
	//char buf[MAX_PACKET_SIZE+1];

	// ������ �ޱ�
	addrlen = sizeof(clientaddr);
	retval = recvfrom(ServerSock, buf, MAX_BIG_PACKET_SIZE, 0,
		(SOCKADDR *)&clientaddr, &addrlen);
	if(retval == SOCKET_ERROR){
		iLen = 0; 	// ������ byte���� 0�Դϴ�.
		return -2;
	}
	else
	{
		iLen = retval;
	}

	// �������� ���� ������ ����Ҷ� ����ϰ� ǥ�õǵ��� ��
   	buf[retval] = '\0';
	sprintf(tmpA, "%s", inet_ntoa(clientaddr.sin_addr));
	iClientPort = ntohs(clientaddr.sin_port);

	return retval;
}
int AutoTeaching::AutoTeaching_SendCommand(char* Buf)
{
	int retval = NO_ERR;
    	// string strMsg;
	AUTO_TEACH_PACKET	SEND_PACKET;

	int iPort = AUTO_TEACH_SEND_PORT;
	int len = strlen(Buf);

	ADD_LOG("AutoTeaching_SendCommand:%s: Port:%d", cATIpAddr,iPort);

	if (len  > 256)
    	return -1;

	memset(&SEND_PACKET.cCommand, '\0', sizeof(AUTO_TEACH_PACKET));
	memmove(&SEND_PACKET.cCommand, Buf, len);
	SEND_PACKET.cCommand[len+1] = '\0';

	ADD_LOG("SendCom : %s", SEND_PACKET.cCommand);
	UnicastSendMessage(cATIpAddr, iPort, (char*)&SEND_PACKET, len);

	return retval;
}

int AutoTeaching::ATDrivingMove(BYTE PortType)
{
	int nError = NO_ERR;

	MANUAL_DRIVING_CMD_INFO Info;

    //��ü �̵��� ���� �Ѱ� ..
	Total_fDrive_Offset += fDrive_Offset;

	//STB���� Test/���� �ʿ�.(EQ�� ���Ѿ��� Display �� Log ����)
	if((PortType == PORT_TYPE_STB_L) || (PortType == PORT_TYPE_STB_R))
	{
		if(fabs(Total_fDrive_Offset > 10.0))   //�θ��� ���� �Ÿ� �� QR mark ����. �߰� Tset ���� ��, �Ÿ����� �ּҷ� �������� �������� �����Ұ�.
		{
			ADD_LOG("[STB]Driving Limit Over: Total:%f (offset:%f)", Total_fDrive_Offset, fDrive_Offset);
			nError  = ERR_AUTOTEACHING_DRIVING_LIMIT;
		}
	}else
	{
        if(fabs(Total_fDrive_Offset > 30.0))   //�θ��� ���� �Ÿ� �� QR mark ����. �߰� Tset ���� ��, �Ÿ����� �ּҷ� �������� �������� �����Ұ�.
		{
			ADD_LOG("[EQ]Driving Limit Over: Total:%f (offset:%f)", Total_fDrive_Offset, fDrive_Offset);
			nError  = ERR_AUTOTEACHING_DRIVING_LIMIT;
		}
    }

	if(nError ==NO_ERR)
	{
		Info.CmdType = MANUAL_DRIVING_CMD_FORWARD;
		Info.dAccel = 0.0001;
		Info.dDecel = 0.0001;
		Info.dSpeed = 0.1;
		Info.dDist = fDrive_Offset;  //offset��

		nError = m_pVirtualDrivingControl->AutoTaeching_Driving(Info);
	}

	return nError;
}

int AutoTeaching::ATDrivingMoveCheck(bool &bCheck)
{

	return -1;

}


int AutoTeaching::ATSave_NullType_Data(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing,char* cATAddr)
{
	int Error = NO_ERR;

	double TempHoistOffset = 0.0;
	double TempShiftOffset = 0.0;

// Update�� Teaching Data�� Find�Ѵ�.
	TEACHING_DATA *teachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, PortType);

	//ƼĪ Data�� ���� ���.
	if(Error == NO_ERR)
	{
		if (teachingData == NULL)
		{
			 Error = ERR_AUTOTEACHING_NULLTYPE_NO_DATA;   //�߰�
		}
		//ƼĪ Data�� ���� ��� Update
		else
		{
			// �̹������ϴ� ���� ���� ������Ʈ�Ѵ�.
			if(PortType == PORT_TYPE_STB_L)
			{
				defualtOffset.dSTB_L_Shift_Offset = ((double)(teachingData->Shift - (int)m_pExecuteInfo->TransDifference.dShift)/10) - dSTeacing;  //���� ��ġ
				defualtOffset.nSTB_L_Driving_Offset = -(int)dDTeaching; //���̸� �÷��ٰ�.
			}
			else if(PortType == PORT_TYPE_STB_R)
			{
				defualtOffset.dSTB_R_Shift_Offset = ((double)(teachingData->Shift - (int)m_pExecuteInfo->TransDifference.dShift)/10) - dSTeacing;  //���� ��ġ
				defualtOffset.nSTB_R_Driving_Offset = -(int)dDTeaching; //���̸� �÷��ٰ�.
			}
			else
			{
              	Error = ERR_AUTOTEACHING_NULLTYPE_NO_DATA;   //�߰�
			}

			memmove(&(defualtOffset.AutoTeachingIP), cATAddr,strlen(cATAddr));

			if(fabs(dDTeaching) > 5.0)         //Driving Limit �Ÿ��� �߰� ��ü������ +-10mm���� ��� ���� �߻�.
				 Error = ERR_AUTOTEACHING_DRIVING_LIMIT;

			if(Error == NO_ERR)
			{
				Error = CheckSTBShiftLimitError(PortType,(double)(teachingData->Shift/10));
			}
			if(Error == NO_ERR)
			{
				Error = CheckSTBHoistLimitError(PortType,(double)(teachingData->Hoist/10));
			}

			if(Error == NO_ERR)
			{
				if((((double)(teachingData->Shift - (int)m_pExecuteInfo->TransDifference.dShift)/10) - dSTeacing) > 5.0)
					Error = ERR_SHIFT_OUT_OF_P_LIMIT;
 				else if((((double)(teachingData->Shift - (int)m_pExecuteInfo->TransDifference.dShift)/10) - dSTeacing) < -5.0)
					Error = ERR_SHIFT_OUT_OF_N_LIMIT;
			}

			if (Error == NO_ERR)
			{
				// by zzang9un 2013.07.23 : �α� ����
				ADD_LOG(
				"[Data][Teaching][Update] StationID:%06d, PortType:%02X, Hoist:%d(), Shift:%d(), Rotate:%d, STB Offset:%d(%f) %f %d %f %d",
				teachingData->StationID, teachingData->PortType,
				teachingData->Hoist,
				teachingData->Shift,
				teachingData->Rotate,
				teachingData->STBOffSet,
				dDTeaching,
				defualtOffset.dSTB_L_Shift_Offset,
				defualtOffset.nSTB_L_Driving_Offset,
				defualtOffset.dSTB_R_Shift_Offset,
				defualtOffset.nSTB_R_Driving_Offset);



				Save_AutoTeaching_Param_NullType(PortType);
			}

		}
	}
	else
	{
     	Error = ERR_AUTOTEACHING_NULLTYPE_NO_DATA;   //�߰�
    }

	if (Error)
	{
			// ����ó��
			ADD_LOG("[Data][Teaching save] FailTeachingData,%d",Error);
	}

		/// ���� Offset�� ���Ϸ� ����.
    return Error;
}
int AutoTeaching::ATSave_Current_Pos(UINT CurStationID, BYTE PortType, double dDTeaching, double dHTeacing, double dSTeacing)
{
	int Error = NO_ERR;

	double TempDrivingOffset = 0.0;
	double TempHoistOffset = 0.0;
	double TempShiftOffset = 0.0;

	// Update�� Teaching Data�� Find�Ѵ�.
	TEACHING_DATA *teachingData = OHTMainForm->GetTeachingDataManager()->Find(CurStationID, PortType);

	MapMakerFileManager_STD *MapMakerFile = OHTMainForm->GetMapMakerFileManager_STD();

	if((PortType == PORT_TYPE_STB_L) || (PortType == PORT_TYPE_STB_R))
	{

		TempHoistOffset = TEACHING_OFFSET_STB_HOIST + defualtOffset.dSTB_Hoist;

		if(PortType == PORT_TYPE_STB_L)
		{
			TempShiftOffset = defualtOffset.dSTB_L_Shift_Offset;
			TempDrivingOffset = dDTeaching+defualtOffset.nSTB_L_Driving_Offset;
		}
		else
		{
			TempShiftOffset = defualtOffset.dSTB_R_Shift_Offset;
			TempDrivingOffset = dDTeaching+defualtOffset.nSTB_R_Driving_Offset;
		}

	}
	else
	{
		TempShiftOffset = defualtOffset.dEQ_Shift;
		TempHoistOffset = TEACHING_OFFSET_HOIST+ defualtOffset.dEQ_Hoist;//EQ Port Jig ����
	}

	//ƼĪ Data�� ���� ���.
	if(Error == NO_ERR)
	{
		if (teachingData == NULL)
		{
				// ã�� ���Ѱ��, �� ���Ӱ� �߰��ؾ��ϴ°��
				teachingData = new TEACHING_DATA;
				memset(teachingData, 0, sizeof(TEACHING_DATA));

				teachingData->StationID = CurStationID;
				teachingData->PortType = PortType;

				// by zzang9un 2013.08.30 : Teaching Offset ����
				//teachingData->Hoist = m_StatusOHT2TP.Hoist.Position +	(int)m_pExecuteInfo->TransDifference.dHoist;
				//teachingData->Shift = m_StatusOHT2TP.Shift.Position + 	(int)m_pExecuteInfo->TransDifference.dShift;
				//teachingData->Rotate = m_StatusOHT2TP.Rotate.Position;
				//OHT main���� ���� Hoist, Shift, Rotate���� Offset�ݿ��Ͽ� ����
				teachingData->Hoist = (dHTeacing +TempHoistOffset)*10  +	(int)m_pExecuteInfo->TransDifference.dHoist;  //���� ��ġ
				teachingData->Shift = (dSTeacing+TempShiftOffset)*10  +	(int)m_pExecuteInfo->TransDifference.dShift;  //���� ��ġ
				teachingData->Rotate = 0;

				teachingData->STBOffSet = (int)TempDrivingOffset; //���̸� �÷��ٰ�.


				teachingData->PIODirection =  1; //Left
				teachingData->PIOTimeLevel = 1; //Default
				teachingData->LookDownLevel = 1; //default
				teachingData->PIOID = CurStationID;
				teachingData->PIOType = 1; //RF Default;
				teachingData->PIOChannel = 0;

				//ƼĪ���� Ȯ�� �÷���
				teachingData->IsDefaultMap.LOW_DefaultMapInfo = 1;

				if(fabs(teachingData->STBOffSet) > 10.0)         //Driving Limit �Ÿ��� �߰� ��ü������ +-10mm���� ��� ���� �߻�.
					 Error = ERR_AUTOTEACHING_DRIVING_LIMIT;

				if(Error == NO_ERR)
				{
					Error = CheckSTBShiftLimitError(PortType,(double)(teachingData->Shift/10));
				}
				if(Error == NO_ERR)
				{
					Error = CheckSTBHoistLimitError(PortType,(double)(teachingData->Hoist/10));
				}

				if (Error == NO_ERR)
				{
					//���Ϸ� �����Ұ�.
					// by zzang9un 2013.07.23 : �α� ����
					ADD_LOG("[Data][Teaching][Insert] StationID:%06d, PortType:%02X, Hoist:%d(), Shift:%d(), Rotate:%d, STB Offset:%d",
						teachingData->StationID, teachingData->PortType,
						teachingData->Hoist,
						teachingData->Shift,
						teachingData->Rotate,
						teachingData->STBOffSet);

					Error = OHTMainForm->GetTeachingDataManager()->Insert(teachingData);
				}

				if (Error == NO_ERR)
					Save_AutoTeaching_DrvingData(CurStationID,PortType,(int)teachingData->STBOffSet);


			}

			//ƼĪ Data�� ���� ��� Update
			else
			{
				// �̹������ϴ� ���� ���� ������Ʈ�Ѵ�.
				teachingData->StationID = CurStationID;
				teachingData->PortType = PortType;

//				if(teachingData->IsDefaultMap.LOW_DefaultMapInfo == 4)
//				{
//					ADD_LOG("[Data][Teaching save] No DefaultMap %d",teachingData->IsDefaultMap);
//					return ERR_UPDATE_DATA_UPDATE_INVALID_FILE;
//				}

				teachingData->Hoist = (dHTeacing + TempHoistOffset)*10  +	(int)m_pExecuteInfo->TransDifference.dHoist;  //���� ��ġ
				teachingData->Shift = (dSTeacing + TempShiftOffset)*10  +	(int)m_pExecuteInfo->TransDifference.dShift;  //���� ��ġ

				teachingData->Rotate = 0;

				//ƼĪ���� Ȯ�� �÷���
				teachingData->IsDefaultMap.LOW_DefaultMapInfo  = 1;

				teachingData->STBOffSet = (int)TempDrivingOffset; //���̸� �÷��ٰ�.

				//soming
				 if(PortType == PORT_TYPE_STB_L)
				{
					  ADD_LOG("����� Shift ��: %d, %d",teachingData->Shift,teachingData->STBL_Shift);
					  teachingData->Shift = teachingData->Shift + teachingData->STBL_Shift;
				}

				if(fabs(teachingData->STBOffSet) > 10.0)         //Driving Limit �Ÿ��� �߰� ��ü������ +-10mm���� ��� ���� �߻�.
					 Error = ERR_AUTOTEACHING_DRIVING_LIMIT;

				if(Error == NO_ERR)
				{
					Error = CheckSTBShiftLimitError(PortType,(double)(teachingData->Shift/10));
				}
				if(Error == NO_ERR)
				{
					Error = CheckSTBHoistLimitError(PortType,(double)(teachingData->Hoist/10));
				}

				if (Error == NO_ERR)
				{
					// by zzang9un 2013.07.23 : �α� ����
					ADD_LOG(
					"[Data][Teaching][Update] StationID:%06d, PortType:%02X, Hoist:%d(), Shift:%d(), Rotate:%d, STB Offset:%d(+%f)",
					teachingData->StationID, teachingData->PortType,
					teachingData->Hoist,
					teachingData->Shift,
					teachingData->Rotate,
					teachingData->STBOffSet,
					TempDrivingOffset);



					Error = OHTMainForm->GetTeachingDataManager()->TeachingUpdate(teachingData);
				}

				if (Error == NO_ERR)
					Save_AutoTeaching_DrvingData(CurStationID,PortType,(int)teachingData->STBOffSet);
			}
	}

		//Map Maker file ���� ������ �׳� ������.
	if (Error == NO_ERR)
	{
		MapMakerFile->SucessesFindUpdate('T',CurStationID);//����� EQ, STB�� �� �ϳ��� Tag�� �ִٴ� �������� ������.
	}
	if (Error)
	{
			// ����ó��
			ADD_LOG("[Data][Teaching save] FailTeachingData,%d",Error);
	}

		/// ���� Offset�� ���Ϸ� ����.


	return Error;
}

void AutoTeaching::SetAutoTeachingJigResponse()
{
	dwAutoTeachingResponseTime = timeGetTime();
}
DWORD AutoTeaching::GetAutoTeachingJigResponse()
{
	return dwAutoTeachingResponseTime;
}

int AutoTeaching::Load_AutoTeaching_Param()
{
	int nResult = 0;
	int nTmpValue = 0;
	double dTmpValue = 0.0;
	char strTempValue[30]= {0,};

	memset(&defualtOffset,000, sizeof(AUTO_TEACH_OFFSET));

	LIniFile *pIniFile;
    pIniFile = NULL;
	pIniFile = new LIniFile("DataFiles\\AutoTeachingData.txt");

    if(!pIniFile->GetIniFileExist())
    {
        delete pIniFile;
        pIniFile = NULL;
    }

	if(pIniFile != NULL)
	{

		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","EQSHIT",0.0, -20.0, 20.0 , &dTmpValue);
		defualtOffset.dEQ_Shift = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","EQHOIST",0.0, -20.0, 20.0 , &dTmpValue);
		defualtOffset.dEQ_Hoist = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","STBSHIT",0.0, -20.0, 20.0 , &dTmpValue);
		defualtOffset.dSTB_Shift = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","STBHOIST",0.0, -20.0, 20.0 , &dTmpValue);
		defualtOffset.dSTB_Hoist = dTmpValue;


		//���������� ���� �����͵�
		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","STBRSHIFTOFFSET",TEACHING_OFFSET_SHIFT_R, -30.0, 30.0 , &dTmpValue);
		defualtOffset.dSTB_R_Shift_Offset = dTmpValue;

		pIniFile->ReadDoubleMinMaxCheck("DEFAULT","STBLSHIFTOFFSET",TEACHING_OFFSET_SHIFT_L, -30.0, 30.0 , &dTmpValue);
		defualtOffset.dSTB_L_Shift_Offset = dTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("DEFAULT","STBRDRIVINGOFFSET",TEACHING_OFFSET_SHIFT_R, -5, 5 , &nTmpValue);
		defualtOffset.nSTB_R_Driving_Offset = nTmpValue;

		pIniFile->ReadIntegerMinMaxCheck("DEFAULT","STBLDRIVINGOFFSET",TEACHING_OFFSET_SHIFT_L, -5, 5 , &nTmpValue);
		defualtOffset.nSTB_L_Driving_Offset = nTmpValue;


		//2018.01.04 puting ����ƼĪ ���� IP ȹ��
		pIniFile->ReadString("DEFAULT","IpAddress", "192.168.0.1", strTempValue);
		memmove(&(defualtOffset.AutoTeachingIP), strTempValue, strlen(strTempValue)); //Return Value
		memset(strTempValue, 0, sizeof(strTempValue));

		delete pIniFile;
	}

	return  nResult;
 }


int AutoTeaching::Save_AutoTeaching_Param_NullType(BYTE PortType)
{
	int nResult = 0;
	int nTmpValue = 0;
	double dTmpValue = 0.0;
	char strTempValue[30]= {0,};

//	memset(&defualtOffset,000, sizeof(AUTO_TEACH_OFFSET));

	LIniFile *m_lWriteIniFile;
	m_lWriteIniFile = NULL;
	m_lWriteIniFile = new LIniFile("DataFiles\\AutoTeachingData.txt");

	if(!m_lWriteIniFile->GetIniFileExist())
    {
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
    }

	if(m_lWriteIniFile != NULL)
	{

		m_lWriteIniFile->WriteString("DEFAULT","IpAddress", defualtOffset.AutoTeachingIP);

		if(PortType == PORT_TYPE_STB_L)
		{
			m_lWriteIniFile->WriteDouble("DEFAULT","STBLSHIFTOFFSET", defualtOffset.dSTB_L_Shift_Offset);
			m_lWriteIniFile->WriteInteger("DEFAULT","STBLDRIVINGOFFSET", defualtOffset.nSTB_L_Driving_Offset);

		}else
		{
           	m_lWriteIniFile->WriteDouble("DEFAULT","STBRSHIFTOFFSET", defualtOffset.dSTB_R_Shift_Offset );
			m_lWriteIniFile->WriteInteger("DEFAULT","STBRDRIVINGOFFSET", defualtOffset.nSTB_R_Driving_Offset);
        }

		delete m_lWriteIniFile;
	}

	return  nResult;
 }


int AutoTeaching::Save_AutoTeaching_DrvingData(UINT CurStationID, BYTE PortType, int dDriving)
{
	bool bResult = false;

	char strStationID[20] = {0,};
	char strPortType[20] = {0,};

	sprintf(strStationID, "%u",CurStationID);
	sprintf(strPortType, "%d",PortType);


	ADD_LOG("[Data][AutoTeaching][Driving]Station %d, Driving %d",CurStationID, dDriving);

	LIniFile *m_lWriteIniFile;
	m_lWriteIniFile = NULL;
	m_lWriteIniFile = new LIniFile("DataFiles\\AutoTeachingData.txt");




	if(!m_lWriteIniFile->GetIniFileExist())
	{
		delete m_lWriteIniFile;
		m_lWriteIniFile = NULL;
	}

	if(m_lWriteIniFile != NULL)
	{
		m_lWriteIniFile->WriteInteger(strStationID,strPortType, (int)dDriving);

		delete m_lWriteIniFile;
		bResult = true;
	}

	return bResult;
}

void AutoTeaching::QRDrvingTrigger(BYTE PortType)
{
	if(PortType == PORT_TYPE_STB_L)
	{
		m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(true);

    }
	else if(PortType == PORT_TYPE_STB_R)
	{
		m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(true);
	}
}

bool AutoTeaching::QRDataCheck(BYTE PortType, int nTargetID)
{
	bool breturn = false;

	int nID =0;
	double dOffset = 0.0;

	if(PortType == PORT_TYPE_STB_L)
	{
		if(m_pHWSet->m_pLeftQRR->GetIDnOffset(&nID, &dOffset)!=NO_ERR)
		{
			nID = 0;
			dOffset = 0.0;
		}

		if(m_pHWSet->m_pLeftQRR->GetStaticQRID() == nTargetID)
		{
			Total_fDrive_Offset = -m_pHWSet->m_pLeftQRR->GetStaticQROffset();
			breturn = true;
		}

		m_pHWSet->m_pLeftQRR->SetStaticQRDataTrigger(false);

	}
	else if(PortType == PORT_TYPE_STB_R)
	{
		if(m_pHWSet->m_pRightQRR->GetIDnOffset(&nID, &dOffset)!=NO_ERR)
		{
			nID = 0;
			dOffset = 0.0;
		}

		if(m_pHWSet->m_pRightQRR->GetStaticQRID() == nTargetID)
		{
			Total_fDrive_Offset = -m_pHWSet->m_pRightQRR->GetStaticQROffset();
			breturn = true;
		}

		m_pHWSet->m_pRightQRR->SetStaticQRDataTrigger(false);
	}

	ADD_LOG("[Data][AutoTeaching][QRDataCheck]Port:%d,TargetID:%d,QRID:%d(%d),Offset:%f(%f)",
			PortType,nTargetID, m_pHWSet->m_pLeftQRR->GetStaticQRID(),nID,m_pHWSet->m_pRightQRR->GetStaticQROffset(),dOffset);

	return breturn;

}

int AutoTeaching::CheckSTBShiftLimitError(int nPortType,double Shift)
{
	int Error = NO_ERR;

	if(nPortType == PORT_TYPE_STB_R)
	{
		if(Shift > SHIFT_MOVELIMIT_P)
		{
			ADD_LOG("[RSTB]Shift Out Of P Limit");
			Error = ERR_SHIFT_OUT_OF_P_LIMIT;
		}

		if(Shift < SHIFT_MOVELIMIT_N_MIN_STB_R)
		{
			ADD_LOG("[RSTB]Shift Out Of N Limit");
			Error = ERR_SHIFT_OUT_OF_N_LIMIT;
		}

	}
	else if(nPortType == PORT_TYPE_STB_L)
	{
		if(Shift< SHIFT_MOVELIMIT_N)
		{
			ADD_LOG("[LSTB]Shift Out Of N Limit");
			Error = ERR_SHIFT_OUT_OF_N_LIMIT;
		}
		if(Shift > SHIFT_MOVELIMIT_N_MIN_STB_L)
		{
			ADD_LOG("[LSTB]Shift Out Of P Limit");
			Error = ERR_SHIFT_OUT_OF_P_LIMIT;
        }

	}
	return Error;
}

int AutoTeaching::CheckSTBHoistLimitError(int nPortType, double Hoist)
{
	int Error = NO_ERR;
	if((nPortType == PORT_TYPE_STB_R) || (nPortType == PORT_TYPE_STB_L))
	{
		if(Hoist  > HOIST_MOVELIMIT_P_MAX_STB)  	//soming STB Hoist Limit �� �߰�
		{
			ADD_LOG("[AutoTeaching]Hoist Out Of P Limit");
			Error = ERR_HOIST_OUT_OF_P_LIMIT;
        }


		if(Hoist  < HOIST_MOVELIMIT_N_MIN_STB)  	//soming STB Hoist Limit �� �߰�
		{
			ADD_LOG("[AutoTeaching]Hoist Out Of N Limit");
			Error = ERR_HOIST_OUT_OF_N_LIMIT;
        }

	}

	return Error;
}
