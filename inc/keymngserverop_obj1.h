

#pragma once 

#include "keymng_msg.h"
#include "keymngserverabsop.h"

class  KeyMngServerOp_OBJ1 : public KeyMngServerAbsOp  
{
public:
	//��ʼ�������� ȫ�ֱ���
	virtual int MngServer_InitInfo(MngServer_Info *svrInfo);
	
	//����� ��ԿЭ��Ӧ������
	virtual int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
	
	virtual int MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
	
	virtual int MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen);
    
};






