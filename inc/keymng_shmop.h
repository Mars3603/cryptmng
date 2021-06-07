

// keymng_shmop.h

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




//��������Կ��Ϣд�����ڴ棬 ���㹲���ڴ�ṹ��
typedef struct _NodeSHMInfo
{	
	int 			status;			//��Կ״̬ 0-��Ч 1��Ч
	char			clientId[12];	//�ͻ���id
	char			serverId[12];	//��������id	
	int				seckeyid;		//�Գ���Կid
	unsigned char	seckey[128];	//�Գ���Կ //hash1 hash256 md5
}NodeSHMInfo;

		
class KeyMng_ShmOp
{
public:
	static int KeyMng_ShmInit(int key, int maxnodenum, int *shmhdl);
	static int KeyMng_ShmWrite(int shmhdl, int maxnodenum, NodeSHMInfo *pNodeInfo);
	static int KeyMng_ShmRead(int shmhdl, char *clientId, char *serverId,  int maxnodenum, NodeSHMInfo *pNodeInfo);
};










