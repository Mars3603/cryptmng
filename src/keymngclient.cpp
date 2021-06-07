

#include <iostream>

using namespace std;

#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <pthread.h>

#include "keymnglog.h"
#include "keymngclientapp.h"
#include "keymngclientop.h"

/* 
#include "poolsocket.h"

#include "keymng_msg.h"
*/

int main()
{
	int 		ret = 0;
	cout << "hello keymngclient...\n";	
	KeyMngClientApp    	keyMngClientApp;
	
	KeyMngClientOp		keyMngClientOp;
	
	MngClient_Info 		mngClientInfo;
	
	
	//把业务流类对象 注入到 应用程序框架中
	keyMngClientApp.setKeyMngClientOp(&keyMngClientOp);
	keyMngClientApp.setMngClientInfo(&mngClientInfo);
	
	ret = keyMngClientApp.init();
	if (ret != 0)
	{
		printf("keymngclient init err:%d \n", ret);	
		return ret;
	}
	printf("keymngclient init ok \n");	
	
	keyMngClientApp.run();
	keyMngClientApp.exit();
	
	
					
	return 0;
}

  