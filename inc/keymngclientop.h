
// keymngclientop.h

#pragma once


#define		MngClt_OK				0			//正确
#define		MngClt_ParamErr			301			//输入参数失败
#define		MngClt_NoNetPointErr	302		//共享内存中，没有找到网点信息
#define		MngClt_NodeMaxCount		303			//共享内存中，超过最大网点

typedef struct _MngClient_Info
{
	char			clientId[12];	//客户端编号
	char			AuthCode[16];	//认证码
	char			serverId[12];	//服务器端编号
	
	char			serverip[32];
	int 			serverport;
	
	int				maxnode; //最大网点数 客户端默认1个
	int 			shmkey;	 //共享内存keyid 创建共享内存时使用	 
	int 			shmhdl; //共享内存句柄	
}MngClient_Info;


class KeyMngClientOp
{
public:
	//初始化客户端 全局变量
	 int MngClient_InitInfo(MngClient_Info *pCltInfo);
	
	 int MngClient_Quit(MngClient_Info *pCltInfo);
	
	 int MngClient_Agree(MngClient_Info *pCltInfo);
	
	 int MngClient_Check(MngClient_Info *pCltInfo);
	
	 int MngClient_Revoke(MngClient_Info *pCltInfo);
	
	 int MngClient_view(MngClient_Info *pCltInfo);	
};








