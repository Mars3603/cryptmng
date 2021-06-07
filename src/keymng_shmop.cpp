
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "keymnglog.h"
#include "keymng_shmop.h"
#include "myipc_shm.h"

#include "keymngclientop.h"


//服务器或者客户端初始化共享内存
//初始化时 先判断共享内存是否存在
//若 存在  使用就的
//若 不存在  创建共享内存
int KeyMng_ShmOp::KeyMng_ShmInit(int key, int maxnodenum, int *shmhdl)
{
	int 		ret = 0;
	//打开共享内存
	ret = MyIpc_ShmHelper::IPC_OpenShm(key, maxnodenum*sizeof(NodeSHMInfo), shmhdl);
	if (ret == MYIPC_NotEXISTErr)
	{
		printf("系统 检测到共享内存 不存在 将要创建共享内存\n");
		
		ret = MyIpc_ShmHelper::IPC_CreatShm(key, maxnodenum*sizeof(NodeSHMInfo), shmhdl);
		if (ret != 0)
		{
			printf("系统 创建共享内存失败 err:%d \n", ret);
			return ret;	
		}
		else
		{
			printf("系统 创建共享内存ok\n");
		}
		
	
		void *mapaddr = NULL;
		ret = MyIpc_ShmHelper::IPC_MapShm(*shmhdl, &mapaddr);
		if (ret != 0)
		{
			printf("func  IPC_MapShm()  err:%d \n", ret);
			return ret;
		}
		
		memset(mapaddr, 0, maxnodenum*sizeof(NodeSHMInfo));
		
		printf("系统 初始化共享内存 ok\n");
		
		MyIpc_ShmHelper::IPC_UnMapShm(mapaddr);
	
	}
	else if (ret == 0)
	{
		printf("系统检测到共享内存已经存在 使用旧的共享内存\n");
		return ret;	
	}
	else if (ret != 0)
	{
		
		printf("func IPC_OpenShm() err:%d \n", ret);
		return ret;
	}
	return 0;	
}

//写网点密钥
//1 若网点 已经存在 则覆盖
//2 若网点 不存在 找一个空的位置 写入
int KeyMng_ShmOp::KeyMng_ShmWrite(int shmhdl, int maxnodenum, NodeSHMInfo *pNodeInfo)
{
	int				ret = 0, i = 0;
	NodeSHMInfo  	tmpNodeInfo; //空结点
	NodeSHMInfo		*pNode = NULL;
	
	void 			*mapaddr = NULL;
	
	
	memset(&tmpNodeInfo, 0, sizeof(NodeSHMInfo));
	
	//连接共享内存
	ret = MyIpc_ShmHelper::IPC_MapShm(shmhdl, &mapaddr);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"IPC_MapShm() Err");
		return ret;
	}
	
	
	//1 若网点 已经存在 则覆盖
	for (i=0; i<maxnodenum; i++)
	{
		pNode = reinterpret_cast<NodeSHMInfo *>(reinterpret_cast<long>(mapaddr)+sizeof(NodeSHMInfo)*i ); 
		if ( strcmp(pNode->clientId,pNodeInfo->clientId) == 0 &&
			 strcmp(pNode->serverId,pNodeInfo->serverId) == 0 )
		{
			LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[3], ret,"系统检测到网点密钥已经存在 覆盖clientId:%s serverid:%s", pNodeInfo->clientId,pNodeInfo->serverId);
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			goto End;
		}
	}
	
	//2 若网点 不存在 找一个空的位置 写入
	for (i=0; i<maxnodenum; i++)
	{
		//pNode = mapaddr + sizeof(NodeSHMInfo)*i;
		pNode = reinterpret_cast<NodeSHMInfo *>(reinterpret_cast<long>(mapaddr)+sizeof(NodeSHMInfo)*i ); 
		if ( memcmp(pNode ,&tmpNodeInfo, sizeof(NodeSHMInfo) ) == 0)
		{
			LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[3], ret,"系统检测到空节点 写入网点信息clientId:%s serverid:%s", pNodeInfo->clientId,pNodeInfo->serverId);
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			goto End;
		}
	}
	
	//
	if (i == maxnodenum)
	{
		ret = 102;
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"系统检测到网点节点已满错误 clientId:%s serverid:%s", pNodeInfo->clientId,pNodeInfo->serverId);
		goto End;
	}
	
End:
		
	MyIpc_ShmHelper::IPC_UnMapShm((void **)&mapaddr); //取消连接
	return ret;
}


int KeyMng_ShmOp::KeyMng_ShmRead(int shmhdl, char *clientId, char *serverId,  int maxnodenum, NodeSHMInfo *pNodeInfo)
{
	int				ret = 0, i = 0;
	NodeSHMInfo  	tmpNodeInfo; //空结点
	NodeSHMInfo		*pNode = NULL;
	
	void 			*mapaddr = NULL;
	
	memset(&tmpNodeInfo, 0, sizeof(NodeSHMInfo));
	
	//连接共享内存
	ret = MyIpc_ShmHelper::IPC_MapShm(shmhdl, &mapaddr);
	if (ret != 0)
	{
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"IPC_MapShm() Err");
		return ret;
	}
	
	
	//2 若网点 不存在 找一个空的位置 写入
	for (i=0; i<maxnodenum; i++)
	{
		//pNode = mapaddr + sizeof(NodeSHMInfo)*i;
		pNode = reinterpret_cast<NodeSHMInfo *>(reinterpret_cast<long>(mapaddr)+sizeof(NodeSHMInfo)*i ); 
		if ( strcmp(pNode->clientId, clientId) == 0 &&
			 strcmp(pNode->serverId, serverId) == 0 )
		{
			LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[3], ret,"系统检测到空节点 写入网点信息clientId:%s serverid:%s", pNodeInfo->clientId,pNodeInfo->serverId);
			memcpy(pNodeInfo, pNode,  sizeof(NodeSHMInfo));
			goto End;
		}
	}
	
	//
	if (i == maxnodenum)
	{
		ret = 102;
		LogHelper::KeyMng_Log(__FILE__, __LINE__,LogHelper::KeyMngLevel[4], ret,"系统 没有找到网点节点错误 clientId:%s serverid:%s", pNodeInfo->clientId,pNodeInfo->serverId);
		goto End;
	}
	
End:
		
	MyIpc_ShmHelper::IPC_UnMapShm((void **)&mapaddr); //取消连接
	return ret;
	
}
