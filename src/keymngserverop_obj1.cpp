#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <pthread.h>

#include "keymnglog.h"

#include "keymngserverabsop.h"
#include "keymngserverop_obj1.h"
#include "poolsocket.h"
#include "keymng_msg.h"
#include "myipc_shm.h"

#include "keymng_shmop.h"
#include "icdbapi.h"
#include "keymng_dbop.h"

//读配置文件获取数据库的连接信息 user pw sid
//从数据库中 获取服务器的配置参数

int KeyMngServerOp_OBJ1::MngServer_InitInfo(MngServer_Info *svrInfo)
{
	int 			ret  = 0;
	//读配置信息 获取  user pw sid
	
	strcpy(svrInfo->serverId, "0001");
	strcpy(svrInfo->dbuse, "SECMNG");
	strcpy(svrInfo->dbpasswd, "SECMNG");
	strcpy(svrInfo->dbsid, "orcl");
	svrInfo->dbpoolnum = 5;  //数据库连接池的个数
	
	strcpy(svrInfo->serverip, "127.0.0.1");
	svrInfo->serverport = 8001;
	
	svrInfo->maxnode = 20;
	svrInfo->shmkey = 0x0001;
	
	//初始化共享内存
	ret =  KeyMng_ShmOp::KeyMng_ShmInit(svrInfo->shmkey, svrInfo->maxnode, &svrInfo->shmhdl);
	if (ret != 0)
	{
		printf("func KeyMng_ShmInit() err:%d \n", ret);
		return ret;
	}
	
	//初始化数据库连接池
	ret = IC_DBApi_PoolInit(svrInfo->dbpoolnum , svrInfo->dbsid, svrInfo->dbuse, svrInfo->dbpasswd);
	if (ret != 0)
	{
		printf("func IC_DBApi_PoolInit() err:%d \n", ret);
		return ret;
	}
	printf("系统初始化 数据库连接池 ok\n");
	
	
	return ret;	
}

//static int g_keynum =  101;
//服务器端 应答流程
int KeyMngServerOp_OBJ1::MngServer_Agree(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen)
{
	int 				i = 0, ret = 0;
	//组织 应答报文
	MsgKey_Res			msgKeyRes;
	ICDBHandle			handle = NULL;
	int 				tmpkeyid = 0;
	
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func MngServer_Agree() Begin ");
	
	memset(&msgKeyRes, 0, sizeof(MsgKey_Res) );
	
	msgKeyRes.rv = 0;
	strcpy(msgKeyRes.clientId, msgkeyReq->clientId);
	strcpy(msgKeyRes.serverId, svrInfo->serverId);
	
	
	//获取连接
	ret =  IC_DBApi_ConnGet(&handle, 0, 0);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func IC_DBApi_ConnGet() err ");
		return ret;
	}
	
	//开事物
	ret = IC_DBApi_BeginTran(handle);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func IC_DBApi_BeginTran() err ");
		goto End;
	}
	
	//从数据库中获取密钥序列号
	ret =  KeyMngDBOp::KeyMngsvr_DBOp_GenKeyID(handle, &tmpkeyid);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func KeyMngsvr_DBOp_GenKeyID() err ");
		goto End;
	}
	
	msgKeyRes.seckeyid = tmpkeyid; //赋给应答报文
	for (i=0; i<64; i++)
	{
		msgKeyRes.r2[i] = 'a' + i;
	}
	
	//编码应答报文
	ret = MsgEncode(&msgKeyRes, ID_MsgKey_Res, outData, datalen);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func MsgEncode() err ");
		goto End;
	}
	
	//协商密钥
	NodeSHMInfo  nodeSHMInfo;
	memset(&nodeSHMInfo, 0, sizeof(NodeSHMInfo));
	nodeSHMInfo.status = 0;
	strcpy(nodeSHMInfo.clientId, msgkeyReq->clientId);
	strcpy(nodeSHMInfo.serverId, msgkeyReq->serverId);
	nodeSHMInfo.seckeyid = msgKeyRes.seckeyid;
	
	//r1{abc} r2{123}
	//a1b2c3d4
	for (i=0; i<64; i++)
	{
		nodeSHMInfo.seckey[2*i] = msgkeyReq->r1[i];
		nodeSHMInfo.seckey[2*i + 1] = msgKeyRes.r2[i];
	}

	//写服务器的共享内存
	ret =  KeyMng_ShmOp::KeyMng_ShmWrite(svrInfo->shmhdl, svrInfo->maxnode, &nodeSHMInfo);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func KeyMng_ShmWrite() err:%d \n", ret);
		goto End;
	}
	
	//写数据库
	ret =  KeyMngDBOp::KeyMngsvr_DBOp_WriteSecKey(handle, &nodeSHMInfo);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func KeyMngsvr_DBOp_WriteSecKey() err:%d \n", ret);
		goto End;
	}
	 
	 
End:
	if  (ret == 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"事务提交");
		IC_DBApi_Commit(handle); //提交事物
	}
	else
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"回滚....");
		IC_DBApi_Rollback(handle); //回退
	}
	
	if (ret == IC_DB_CONNECT_ERR)
	{
		IC_DBApi_ConnFree(handle, 0); //需要断链修复
	}
	else
	{
		IC_DBApi_ConnFree(handle, 1); 	 //正常连接
	}
	
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func MngServer_Agree() End ");
	return ret;
}


//服务器端 读自己的共享内存
//和客户端的数据 进行比较
//ok -- 0   err
int KeyMngServerOp_OBJ1::MngServer_Check(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen)
{
	int					 i = 0, ret = 0;
     
	NodeSHMInfo			nodeInfo;
	MsgKey_Res			msgKeyRes;
    
	LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[1], ret, "func MngServer_Check() Begin ");

	memset(&msgKeyRes, 0, sizeof(MsgKey_Res));

	msgKeyRes.rv = 0;
	strcpy(msgKeyRes.clientId, msgkeyReq->clientId);
	strcpy(msgKeyRes.serverId, svrInfo->serverId);

	//解析获取数据
	//读取共享内存
	/*读网点密钥
	int KeyMng_ShmRead(int shmhdl, char *clientId, char *serverId, int maxnodenum, NodeSHMInfo *pNodeInfo)*/
	
	memset(&nodeInfo, 0, sizeof(nodeInfo));
	ret =  KeyMng_ShmOp::KeyMng_ShmRead(svrInfo->shmhdl,msgkeyReq->clientId,svrInfo->serverId,svrInfo->maxnode, &nodeInfo);
	if (ret != 0)
	{  
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "func KeyMng_ShmRead() err:%d \n", ret);
		goto End;
	}
	
	//客户端只传来8个字节
   if (memcmp(msgkeyReq->r1, nodeInfo.seckey, 8) == 0) 
	{
		msgKeyRes.rv = 0;
	}
	else
	{
		msgKeyRes.rv = -1; //任意非零值即可
	}
	
	ret = MsgEncode(&msgKeyRes, ID_MsgKey_Res, outData, datalen);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "func MsgEncode() err ");
		goto End;
	}
	 
End:
	return ret;	
}



// 解析报文（已完成）
// 修改共享内存状态
// 修改数据库
// 组织应答报文，发送应答报文
int KeyMngServerOp_OBJ1::MngServer_Revoke(MngServer_Info *svrInfo, MsgKey_Req *msgkeyReq, unsigned char **outData, int *datalen)
{
	int 				ret = 0;
	//组织 应答报文
	MsgKey_Res			msgKeyRes;
	ICDBHandle			handle = NULL;
	int 				seckeyid=0;
	
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func MngServer_Revoke() Begin ");
	
	memset(&msgKeyRes, 0, sizeof(MsgKey_Res) );
	
	//组织共享内存结构体
	NodeSHMInfo  nodeSHMInfo;
	NodeSHMInfo  key_nodeSHMInfo;
	
	memset(&key_nodeSHMInfo, 0, sizeof(NodeSHMInfo));
	// 获取seckeyid
	ret =  KeyMng_ShmOp::KeyMng_ShmRead(svrInfo->shmhdl,msgkeyReq->clientId,svrInfo->serverId,svrInfo->maxnode, &key_nodeSHMInfo);
	if (ret != 0)
	{   
		LogHelper::KeyMng_Log(__FILE__, __LINE__, LogHelper::KeyMngLevel[4], ret, "func KeyMng_ShmRead() err:%d \n", ret);
		//goto End;
		return ret;
	}
	
	memset(&nodeSHMInfo, 0, sizeof(NodeSHMInfo));
	nodeSHMInfo.status = 1;  //1: 无效  0: 有效
 	strcpy(nodeSHMInfo.clientId, msgkeyReq->clientId);
	strcpy(nodeSHMInfo.serverId, msgkeyReq->serverId);
	nodeSHMInfo.seckeyid=key_nodeSHMInfo.seckeyid;
	
	//写服务器的共享内存
	ret =  KeyMng_ShmOp::KeyMng_ShmWrite(svrInfo->shmhdl, svrInfo->maxnode, &nodeSHMInfo);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func KeyMng_ShmWrite() err:%d \n", ret);
		goto End;
	} 
	//修改数据库
	//获取连接
	ret =  IC_DBApi_ConnGet(&handle, 0, 0);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func IC_DBApi_ConnGet() err ");
		return ret;
	}
	
	//开事物
	ret = IC_DBApi_BeginTran(handle);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func IC_DBApi_BeginTran() err ");
		goto End;
	}
	
	seckeyid=key_nodeSHMInfo.seckeyid;
	
	ret =  KeyMngDBOp::KeyMngsvr_DBOp_upKeyStatus(handle,seckeyid);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func KeyMngsvr_DBOp_upKeyStatus() err ");
		goto End;
	}
	
	 
	msgKeyRes.rv = 0;
	strcpy(msgKeyRes.clientId, msgkeyReq->clientId);
	strcpy(msgKeyRes.serverId, svrInfo->serverId);
	
	 
	ret = MsgEncode(&msgKeyRes, ID_MsgKey_Res, outData, datalen);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"func MsgEncode() err ");
		goto End;
	}
	 
	// 业务流中实现报文的发送 
	 
End:
	if  (ret == 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"事务提交");
		IC_DBApi_Commit(handle); //提交事物
	}
	else
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"回滚....");
		IC_DBApi_Rollback(handle); //回退
	}
	
	if (ret == IC_DB_CONNECT_ERR)
	{
		IC_DBApi_ConnFree(handle, 0); //需要断链修复
	}
	else
	{
		IC_DBApi_ConnFree(handle, 1); 	 //正常连接
	}
	
	LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[1], ret,"func MngServer_Agree() End ");
	return ret;
	
}
