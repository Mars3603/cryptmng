
// keymngserverabsop.h

#pragma once 

#include "keymng_msg.h"

//keymngserver ������

#define		MngSvr_OK				0		//��ȷ
#define		MngSvr_ParamErr			301		//�������ʧ��
#define		MngSvr_NoNetPointErr	302		//�����ڴ��У�û���ҵ�������Ϣ
#define		MngSvr_NodeMaxCount		303		//�����ڴ��У������������
#define		MngSvr_CheckErr			304		//�����ڴ��У������������


//�������˳�ʼ��	
typedef struct _MngServer_Info
{
	char			serverId[12];	//�������˱��
	
	//���ݿ����ӳؾ��	
	char			dbuse[24]; //���ݿ��û���
	char			dbpasswd[24]; //���ݿ�����
	char			dbsid[24]; //���ݿ�sid
	int				dbpoolnum; //���ݿ�� ������
	//�ź������
	
	char			serverip[24];
	int 			serverport;
	
	//�����ڴ�������Ϣ
	int				maxnode; //��������� �ͻ���Ĭ��1��
	int 			shmkey;	 //�����ڴ�keyid ���������ڴ�ʱʹ��	 
	int 			shmhdl; //�����ڴ���	
}MngServer_Info;



class  KeyMngServerAbsOp
{
public:
	KeyMngServerAbsOp()
	{
		
	}
	
	virtual ~KeyMngServerAbsOp()
	{
		
	}
	
public:
	//��ʼ�������� ȫ�ֱ���
	virtual int MngServer_InitInfo(MngServer_Info *svrInfo) = 0;
	
	
	//����� ��ԿЭ��Ӧ������
	virtual int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	//int keymngserver_agree(MngServer_Info  *pmngServerInfo,   MsgKey_Req *pMsgKeyReq ,  unsigned char **pMsgKey_ResData, int *iMsgKey_ResDataLen);
	

	virtual int MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	
	virtual int MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	
	
};






