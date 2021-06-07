

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
#include "keymngclientapp.h"

KeyMngClientApp::KeyMngClientApp()
{
}

KeyMngClientApp::~KeyMngClientApp()
{
}
	
	
int KeyMngClientApp::setKeyMngClientOp(KeyMngClientOp *keyMngClientOp)
{
	m_keyMngClientOp =  keyMngClientOp;
	return 0;
}	


int KeyMngClientApp::setMngClientInfo(MngClient_Info *mngClientInfo)
{
	m_MngClientInfo =  mngClientInfo;
	return 0;
}	


int KeyMngClientApp::init()
{
	m_keyMngClientOp->MngClient_InitInfo(m_MngClientInfo);
	return 0;
}

void KeyMngClientApp::Usage()
{
    
    system("clear");    
	printf("\t\t********************************************\t\t\n");
	printf("\t\t**********  数据传输加密平台  * *************\t\t\n");
	printf("\t\t*********************************************\t\t\n");
	printf("\t\t************   作者：Mars    ****************\t\t\n");
	printf("\t\t********   创作时间：2021/5/18     **********\t\t\n");
	printf("\t\t*********************************************\t\t\n");

	printf("\n");
	printf("\t\t****************操作提示*********************\n");
	printf("\t\t\t*   密钥协商：agree\n");
	printf("\t\t\t*   密钥校验：check\n");
	printf("\t\t\t*   密钥注销：revoke\n");
	printf("\t\t\t*   退出系统：exit\n");
	printf("\t\t*********************************************\n");

     
}

int KeyMngClientApp::run()
{
	int ret = 0;
	char command[20]={0};
	Usage();
	
	while (1)
	{	
		printf("[Mars@crypt_system]#");
    	scanf("%s", command);  //
   	    
   	    if(!strcmp("agree",command)){
   	    	ret=m_keyMngClientOp->MngClient_Agree(m_MngClientInfo);
   	    }
   	    else if(!strcmp("check",command)){
   	    	ret = m_keyMngClientOp->MngClient_Check(m_MngClientInfo);
   	    }
   	    else if(!strcmp("revoke",command)){
   	    	ret = m_keyMngClientOp->MngClient_Revoke(m_MngClientInfo);
   	    }
   	    else if(!strcmp("exit",command)){
   	    	return 0;
   	    }
   	    else {
			printf("不符合要求的输入\n");
		}
		
		if (ret)
		{
			printf("\n!!!!!!!!!!!!!!!!!!!!ERROR!!!!!!!!!!!!!!!!!!!!");
			printf("\n错误码是：%x\n", ret);
		}
		else
		{
			printf("\n!!!!!!!!!!!!!!!!!!!!SUCCESS!!!!!!!!!!!!!!!!!!!!\n");
		}	
		getchar();
	}
	return 0;
}

int KeyMngClientApp::exit()
{
	return 0;
}
	
