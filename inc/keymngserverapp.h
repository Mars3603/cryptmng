
#pragma once 

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
#include "poolsocket.h"
#include "keymngserverabsop.h"

class KeyMngServerApp
{
	friend void *mystart_routine (void *arg);
public:
	KeyMngServerApp();
	~KeyMngServerApp();
	
public:
	int setServerOp(KeyMngServerAbsOp *pKeyMngServerOp);
	int setInfo(MngServer_Info *pMngServerInfo);	
public:
	int init();
	int run();
	int exit();
	
private:
	KeyMngServerAbsOp *m_pKeyMngServerOp;
	MngServer_Info *m_pMngServerInfo;
	int 				m_listenfd ;
	
};

