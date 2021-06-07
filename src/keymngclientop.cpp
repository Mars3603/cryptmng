

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <pthread.h>


#include "keymng_msg.h"
#include "myipc_shm.h"
#include "poolsocket.h"
#include "keymnglog.h"
#include "keymngclientop.h"
#include "keymng_shmop.h"  //网点密钥

int KeyMngClientOp::MngClient_InitInfo(MngClient_Info *pCltInfo)
{
	
	int 			ret = 0;
	//读配置文件
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], 0,"func main() MngClient_InitInfo() begin");
	
	strcpy(pCltInfo->clientId, "1111");
	strcpy(pCltInfo->AuthCode, "1111");
	strcpy(pCltInfo->serverId, "0001");
	strcpy(pCltInfo->serverip, "127.0.0.1");
	pCltInfo->serverport = 8001;
	pCltInfo->maxnode = 1;
	pCltInfo->shmkey = 0x1111;
	pCltInfo->shmhdl = 0;
	
	
	//客户端初始化共享内存
	ret = KeyMng_ShmOp::KeyMng_ShmInit(pCltInfo->shmkey, pCltInfo->maxnode, &pCltInfo->shmhdl);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func main() KeyMng_ShmInit() err");
		return ret;
	}
	
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], 0,"func main() MngClient_InitInfo() end");

	return 0;
}

/* 	
	//密钥协商
	1 组织请求报文(结构体)
	2 报文编码
	3 发送报文
	4 接受报文
	4-2 解码应答报文
	5 协商密钥共享内存
*/	

int KeyMngClientOp::MngClient_Agree(MngClient_Info *pCltInfo)
{

	int 					ret = 0, i = 0 , mytime =3;
	
	MsgKey_Req				msgKeyReq;
	
	//报文的编码结果
	unsigned char			*outData = NULL;
	int						outLen = 0;
	
	
	int 					connfd = 0;
	
	
	//应答报文 数据
	unsigned char			*pMsgKey_ResData = NULL;
	int						iMsgKey_ResDataLen = 0;
	
	MsgKey_Res				*pMsgKeyRes = NULL;
	int 					iMsgKeyReaType = 0;		
	
	//1组织请求报文
	memset(&msgKeyReq, 0, sizeof(MsgKey_Req));
	msgKeyReq.cmdType = KeyMng_NEWorUPDATE;
	strcpy(msgKeyReq.clientId, pCltInfo->clientId);
	strcpy(msgKeyReq.AuthCode, pCltInfo->AuthCode);
	strcpy(msgKeyReq.serverId, pCltInfo->serverId);
	
	//产生随机数
	for (i=0; i<64; i++)
	{
		msgKeyReq.r1[i] = 'a' + i;
	}
	
	//2 报文编码
	ret = MsgEncode(&msgKeyReq,  ID_MsgKey_Req, &outData, &outLen); //free1
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func MsgEncode() err:%d \n", ret);
		goto End;
	}
	
	
	//3 发送报文
	
	//客户端 初始化
	ret =  sckClient_init();
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func sckClient_init() err:%d \n", ret);
		goto End;
	}
	
	//客户端 连接服务器
	ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, mytime, &connfd);
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func sckClient_connect() err:%d serverip:%s, serverport:%d", ret, pCltInfo->serverip, pCltInfo->serverport);
		goto End;
	}
	
	//客户端 发送报文
	ret = sckClient_send( connfd, mytime, outData, outLen);
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func sckClient_send() err:%d \n", ret);
		goto End;
	}
	
	//客户端 接受报文
	ret = sckClient_rev(connfd, mytime, &pMsgKey_ResData, &iMsgKey_ResDataLen); //free 2
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func sckClient_rev() err:%d \n", ret);
		goto End;
	}
	
	//4-2 解码应答报文
	ret = MsgDecode(pMsgKey_ResData, iMsgKey_ResDataLen, (void **)&pMsgKeyRes, &iMsgKeyReaType); //free
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func MsgDecode() err:%d \n", ret);
		goto End;
	}
	
	if (pMsgKeyRes->rv == 0)
	{
		printf("服务器端应答流程  seckeyid:%d ok\n", pMsgKeyRes->seckeyid);	
	}
	else
	{
		printf("服务器端应答流程 err:%d\n", pMsgKeyRes->rv);		
	}

	//将网点密钥信息写共享内存， 网点共享内存结构体
	NodeSHMInfo  nodeSHMInfo;
	memset(&nodeSHMInfo, 0, sizeof(NodeSHMInfo));
	nodeSHMInfo.status = 0;
	strcpy(nodeSHMInfo.clientId, msgKeyReq.clientId);
	strcpy(nodeSHMInfo.serverId, msgKeyReq.serverId);
	nodeSHMInfo.seckeyid = pMsgKeyRes->seckeyid;
	
	//r1{abc} r2{123}
	//a1b2c3d4
	for (i=0; i<64; i++)
	{
		nodeSHMInfo.seckey[2*i] = msgKeyReq.r1[i];
		nodeSHMInfo.seckey[2*i + 1] = pMsgKeyRes->r2[i];
	}

	//写客户端的共享内存
	ret = KeyMng_ShmOp::KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &nodeSHMInfo);
	if (ret != 0)
	{
		//KeyMng_Log(__FILE__, __LINE__,KeyMngLevel[1], ret,"func KeyMng_ShmWrite() err:%d \n", ret);
		goto End;
	}
End:
		
	if (outData )   			MsgMemFree((void **) &outData, 0); // 
	if (pMsgKey_ResData)  		sck_FreeMem(( void ** )&pMsgKey_ResData); //
	if (pMsgKeyRes )   			MsgMemFree((void **) &pMsgKeyRes, iMsgKeyReaType); // 	
	
	//客户端 关闭和服务端的连接
 	sckClient_closeconn(connfd);
	//客户端 释放内存
	
	//客户端 释放
	sckClient_destroy();
	
	return ret;

}

//读共享内存  获取网点密钥
//组织请求报文  把网点密钥的前8个字节 
//发送给服务器
//接受服务器应答
//rv == 0;
int  KeyMngClientOp::MngClient_Check(MngClient_Info *pCltInfo)
{
	NodeSHMInfo			nodeSHMInfo;
	MsgKey_Req			msgKeyReq; 
	
	int connfd=0;
	int 					ret = 0,  mytime =3;
	//报文的编码结果
	unsigned char			*outData = NULL;	
	int						outLen = 0;
	
	//应答报文 数据
	unsigned char			*pMsgKey_ResData = NULL;
	int						iMsgKey_ResDataLen = 0;
	
	MsgKey_Res				*pMsgKeyRes = NULL; 
	int 					iMsgKeyReaType = 0;		

	LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[1], ret, "func MngClient_Check() Begin ");
    
    memset(&msgKeyReq, 0, sizeof(MsgKey_Req));
    
    memset(&nodeSHMInfo, 0, sizeof(nodeSHMInfo));
    
	ret =  KeyMng_ShmOp::KeyMng_ShmRead(pCltInfo->shmhdl,pCltInfo->clientId,pCltInfo->serverId,pCltInfo->maxnode, &nodeSHMInfo);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "func KeyMng_ShmRead() err:%d \n", ret);
		goto End;
	}
	
	//1组织请求报文
	msgKeyReq.cmdType = KeyMng_Check;
	strcpy(msgKeyReq.clientId, pCltInfo->clientId);
	strcpy(msgKeyReq.AuthCode, pCltInfo->AuthCode);
	strcpy(msgKeyReq.serverId, pCltInfo->serverId);
	memcpy(msgKeyReq.r1, nodeSHMInfo.seckey, 8);
	
	ret = MsgEncode(&msgKeyReq, ID_MsgKey_Req, &outData, &outLen); //free1
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "client func MsgEncode() err:%d \n", ret);
		goto End;
	}
	
	//初始化连接
	ret =  sckClient_init();
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "MngClient_Check sckClient_init() err:%d \n", ret);
		goto End;
	}
	
	//客户端 连接服务器
	ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, mytime, &connfd);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "MngClient_Check sckClient_connect() err:%d \n", ret);
		goto End;
	}
	
	ret = sckServer_send(connfd, mytime, outData, outLen);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Check sckServer_send() err:%d \n", ret);
		goto End;
	}
	
	//客户端 接受报文
	ret = sckClient_rev(connfd, mytime, &pMsgKey_ResData, &iMsgKey_ResDataLen); //free 2
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Check sckClient_rev() err:%d \n", ret);
		goto End;
	}
	//4-2 解码应答报文
	ret = MsgDecode(pMsgKey_ResData, iMsgKey_ResDataLen, (void **)&pMsgKeyRes, &iMsgKeyReaType); //free
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Check MsgDecode() err:%d \n", ret);
		goto End;
	}
	
	//printf("密钥校验\n");
	if (pMsgKeyRes->rv == 0)
	{
		printf("秘钥校验成功..........\n");
	}
	else
	{
		printf("秘钥校验失败......rv=%d\n",pMsgKeyRes->rv);		
	}
    
	
End:
	if (outData )   			MsgMemFree((void **) &outData, 0); // 
	if (pMsgKey_ResData)  		sck_FreeMem(( void ** )&pMsgKey_ResData); //
	if (pMsgKeyRes )   			MsgMemFree((void **) &pMsgKeyRes, iMsgKeyReaType); // 	
		
	//客户端 关闭和服务端的连接
	sckClient_closeconn(connfd);
	//客户端 释放内存
	
	//客户端 释放
	sckClient_destroy();
	return ret;
}



//组织密钥报销报文
//发送密钥报销报文
// ------  等待服务器回发
//修改共享内存状态
int  KeyMngClientOp::MngClient_Revoke(MngClient_Info *pCltInfo)
{

	int 					ret = 0 , mytime =3;
	
	MsgKey_Req				msgKeyReq;
	
	//报文的编码结果
	unsigned char			*outData = NULL;
	int						outLen = 0;
	
	int 					connfd = 0;
	
	
	//应答报文 数据
	unsigned char			*pMsgKey_ResData = NULL;
	int						iMsgKey_ResDataLen = 0;
	
	MsgKey_Res				*pMsgKeyRes = NULL;
	int 					iMsgKeyReaType = 0;		
	
	//1组织秘钥注销报文
	memset(&msgKeyReq, 0, sizeof(MsgKey_Req));
	msgKeyReq.cmdType = KeyMng_Revoke;
	strcpy(msgKeyReq.clientId, pCltInfo->clientId);
	strcpy(msgKeyReq.AuthCode, pCltInfo->AuthCode);
	strcpy(msgKeyReq.serverId, pCltInfo->serverId);
	
	
	//2 报文编码
	ret = MsgEncode(&msgKeyReq,  ID_MsgKey_Req, &outData, &outLen); //free1
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func MsgEncode() err:%d \n", ret);
		goto End;
	}
	
	//3 发送报文
	
	//客户端 初始化
	ret =  sckClient_init();
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke sckClient_init() err:%d \n", ret);
		goto End;
	}
	
	//客户端 连接服务器
	ret = sckClient_connect(pCltInfo->serverip, pCltInfo->serverport, mytime, &connfd);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke sckClient_connect() err:%d \n", ret);
		goto End;
	}
	
	//客户端 发送报文
	ret = sckClient_send( connfd, mytime, outData, outLen);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke sckClient_send() err:%d \n", ret);
		goto End;
	}

	//客户端 接受报文
	ret = sckClient_rev(connfd, mytime, &pMsgKey_ResData, &iMsgKey_ResDataLen); //free 2
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke sckClient_rev() err:%d \n", ret);
		goto End;
	}
	
	//4-2 解码应答报文
	ret = MsgDecode(pMsgKey_ResData, iMsgKey_ResDataLen, (void **)&pMsgKeyRes, &iMsgKeyReaType); //free
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke MsgDecode() err:%d \n", ret);
		goto End;
	}

	if (pMsgKeyRes->rv == 0)
	{
		printf("秘钥注销成功  seckeyid:%d ok\n", pMsgKeyRes->seckeyid);	
		
		//将网点密钥信息写共享内存， 网点共享内存结构体
		NodeSHMInfo  nodeSHMInfo;
		memset(&nodeSHMInfo, 0, sizeof(NodeSHMInfo));
		nodeSHMInfo.status = 1;
		strcpy(nodeSHMInfo.clientId, msgKeyReq.clientId);
		strcpy(nodeSHMInfo.serverId, msgKeyReq.serverId);

		//写客户端的共享内存
		ret = KeyMng_ShmOp::KeyMng_ShmWrite(pCltInfo->shmhdl, pCltInfo->maxnode, &nodeSHMInfo);
		if (ret != 0)
		{
			LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret,"MngClient_Revoke KeyMng_ShmWrite() err:%d \n", ret);
			goto End;
		}
	
	}
	else
	{
		printf("秘钥注销失败\n");		
	}

	
End:
		
	if (outData )   			MsgMemFree((void **) &outData, 0); // 
	if (pMsgKey_ResData)  		sck_FreeMem(( void ** )&pMsgKey_ResData); //
	if (pMsgKeyRes )   			MsgMemFree((void **) &pMsgKeyRes, iMsgKeyReaType); // 	
	
	//客户端 关闭和服务端的连接
 	sckClient_closeconn(connfd);
	//客户端 释放内存
	
	//客户端 释放
	sckClient_destroy();
	
	return ret;

}
