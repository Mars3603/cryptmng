
#pragma once 

#include "keymng_shmop.h"

class KeyMngDBOp
{
public:
	static int KeyMngsvr_DBOp_GenKeyID(void *dbhdl, int *keyid);
	static int KeyMngsvr_DBOp_WriteSecKey(void *dbhdl, NodeSHMInfo *pNodeInfo); 
	static int KeyMngsvr_DBOp_upKeyStatus(void *dbdl,int seckeyid);
	
};







