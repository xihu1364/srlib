/************************************************************************
* FileName : NetDefine.h 
* Author   : xihu
* Version  : 1.0
* Info     : 2015/01/20
************************************************************************/
#ifndef __NETDEFINE_H__
#define __NETDEFINE_H__

#pragma pack(push) 
#pragma pack(1)

/************************************************************************
*						��Ϣ��������
************************************************************************/
#define OP_COMPRESS		0x01
#define OP_UNCOMPRESS	0x02
#define OP_COMPLETE		0x04
#define OP_IMCOMPLETE	0x08

/************************************************************************
*						��Ϣ����
************************************************************************/

typedef struct MSGPACKETHEAD
{
	unsigned short	usMsgLen;		// ���ĳ���
	unsigned short	nMsgCmd;		// ��Ϣ���� ���ɰ���65535�����
	unsigned char	nMsgDataType;	// OP_COMPRESSΪѹ����OP_UNCOMPRESSΪ��ѹ����OP_COMPLETE��������OP_IMCOMPLETE�ְ�
	unsigned int	nSessionId;		// �ỰID
	unsigned int	nMsgDataLen;	// ��Ϣ���ʵ�ʳ���
	unsigned int	nReserved;		// ��չ
}MsgPakcetHead,*PMsgPacketHead;

const int MSG_HEAD_LEN = sizeof(MSGPACKETHEAD);
const int MSG_MAX_LEN  = 8192;
const int MSG_DATA_LEN = MSG_MAX_LEN - MSG_HEAD_LEN;

struct IMsgPacket
{
	MSGPACKETHEAD	msgHead;
	char			msgData[MSG_DATA_LEN];

	IMsgPacket()
	{
		memset(this,0x00,sizeof(IMsgPacket));
	}
};

const int MSG_KEEP_ALIVE = 1;


#define PACKET_MAX MSG_MAX_LEN

enum E_IO_TYPE
{
	IO_ON_CREATE	= 0,
	IO_ON_ACCEPT	= 1,
	IO_ON_CONNECT	= 2,
	IO_ON_DISCONN	= 3,
	IO_ON_RECV		= 4,
	IO_SEND			= 5,
	IO_ON_SEND		= 6,
	IO_ERROR		= 7,
};

typedef struct IOMSG
{
	SOCKET		_sServer;
	SOCKET		_sClient;
	int			_nIoType;
	int			_nTick;
	unsigned int _nResult;
	char*		_pszBuf;
	int			_nLen;
	IOMSG() : _sServer(INVALID_SOCKET),_sClient(INVALID_SOCKET),_pszBuf(NULL),_nLen(0){}
}IoMsg,*PIoMsg;

typedef struct MSGDATA
{
	unsigned int nDataLen;
	unsigned int nCmd;
	char* pMsgData;
	MSGDATA()
	{
		memset(this,0x00,sizeof(MSGDATA));
	}
	void Clear()
	{
		memset(this,0x00,sizeof(MSGDATA));
	}
}MsgData,*PMsgData;


#pragma pack(pop)

#endif