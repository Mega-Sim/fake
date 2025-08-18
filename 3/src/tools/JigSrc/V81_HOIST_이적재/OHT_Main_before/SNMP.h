//---------------------------------------------------------------------------

#ifndef SNMPH
#define SNMPH
//---------------------------------------------------------------------------
#define MAX_PACKET 1024
#define MIB_CNT 3
#define MAX_MAC 18
#define MAC_SSID 50

enum MIB_TYPE
{
	MIB_NONE,
	MIB_RSSI,
	MIB_MAC,
	MIB_SSID
};

class SNMP
{
	// ��� ����
private:
protected:
public:

	// ��� �Լ�
private:
protected:
public:
	SNMP()
	{

	}
	~SNMP()
	{

	}
    /* SNMP ��Ŷ ���� */
	int MakeSnmp(unsigned char *packet, char *mibstring, int *snmplength);

	/* SNMP ���� ������ �м� (Type = INT��)*/
	int IntParse(unsigned char *response);

	/* SNMP ���� ������ �м� (Type = CHAR��)*/
	int CharParse(unsigned char *response, unsigned char *result);

	/* ���� MIB Data �м� (Type = CHAR��)*/
	int MibParsing(unsigned char *mibpacket, char *mibstring, int *miblength);
};
#endif
