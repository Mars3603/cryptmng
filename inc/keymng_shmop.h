

// keymng_shmop.h

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




//将网点密钥信息写共享内存， 网点共享内存结构体
typedef struct _NodeSHMInfo
{	
	int 			status;			//密钥状态 0-有效 1无效
	char			clientId[12];	//客户端id
	char			serverId[12];	//服务器端id	
	int				seckeyid;		//对称密钥id
	unsigned char	seckey[128];	//对称密钥 //hash1 hash256 md5
}NodeSHMInfo;

		
class KeyMng_ShmOp
{
public:
	static int KeyMng_ShmInit(int key, int maxnodenum, int *shmhdl);
	static int KeyMng_ShmWrite(int shmhdl, int maxnodenum, NodeSHMInfo *pNodeInfo);
	static int KeyMng_ShmRead(int shmhdl, char *clientId, char *serverId,  int maxnodenum, NodeSHMInfo *pNodeInfo);
};










