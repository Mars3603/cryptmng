
// keymngserverabsop.h

#pragma once 

#include "keymng_msg.h"

//keymngserver 错误码

#define		MngSvr_OK				0		//正确
#define		MngSvr_ParamErr			301		//输入参数失败
#define		MngSvr_NoNetPointErr	302		//共享内存中，没有找到网点信息
#define		MngSvr_NodeMaxCount		303		//共享内存中，超过最大网点
#define		MngSvr_CheckErr			304		//共享内存中，超过最大网点


//服务器端初始化	
typedef struct _MngServer_Info
{
	char			serverId[12];	//服务器端编号
	
	//数据库连接池句柄	
	char			dbuse[24]; //数据库用户名
	char			dbpasswd[24]; //数据库密码
	char			dbsid[24]; //数据库sid
	int				dbpoolnum; //数据库池 连接数
	//信号量句柄
	
	char			serverip[24];
	int 			serverport;
	
	//共享内存配置信息
	int				maxnode; //最大网点树 客户端默认1个
	int 			shmkey;	 //共享内存keyid 创建共享内存时使用	 
	int 			shmhdl; //共享内存句柄	
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
	//初始化服务器 全局变量
	virtual int MngServer_InitInfo(MngServer_Info *svrInfo) = 0;
	
	
	//服务端 密钥协商应答流程
	virtual int MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	//int keymngserver_agree(MngServer_Info  *pmngServerInfo,   MsgKey_Req *pMsgKeyReq ,  unsigned char **pMsgKey_ResData, int *iMsgKey_ResDataLen);
	

	virtual int MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	
	virtual int MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen) = 0;
	
	
};






