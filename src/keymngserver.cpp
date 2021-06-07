
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

#include "keymngserverapp.h" 
#include "keymngserverabsop.h"
#include "keymngserverop_obj1.h"


#define INIT_DAEMON if(fork() >0) exit(1);setsid();
	
int main()
{
	cout << "keymngserver hello...\n";
	
	INIT_DAEMON
	
	KeyMngServerApp   	keyMngServerApp;
	                                           
	KeyMngServerAbsOp  * keyMngServerOp = new KeyMngServerOp_OBJ1;
	MngServer_Info		mngServer_Info;
	
	
	keyMngServerApp.setServerOp(keyMngServerOp);
	keyMngServerApp.setInfo(&mngServer_Info);
	
	
	keyMngServerApp.init();
	keyMngServerApp.run();   //====>run(&keyMngServerApp);
	keyMngServerApp.exit();
	
	delete keyMngServerOp;
	return 0;
}
