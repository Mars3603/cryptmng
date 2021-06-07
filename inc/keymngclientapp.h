

#include <iostream>

using namespace std;

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>

#include "keymnglog.h"
#include "keymngclientop.h"


class KeyMngClientApp
{
public:
	KeyMngClientApp();
	~KeyMngClientApp();
	
public:
	int setKeyMngClientOp(KeyMngClientOp *keyMngClientOp);
	int setMngClientInfo(MngClient_Info *mngClientInfo);
	void Usage();
	int init();
	int run();
	int exit();	
	
public:	
	KeyMngClientOp		*m_keyMngClientOp;
	MngClient_Info 		*m_MngClientInfo;
};