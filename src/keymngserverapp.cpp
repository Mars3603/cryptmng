
#include <iostream>

using namespace std;

#include "keymngserverapp.h"
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
#include "keymng_dbop.h"
#include "icdbapi.h"


KeyMngServerApp::KeyMngServerApp()
{
	
}

KeyMngServerApp::~KeyMngServerApp()
{
	
}

int KeyMngServerApp::setServerOp(KeyMngServerAbsOp *pKeyMngServerOp)
{
	m_pKeyMngServerOp = pKeyMngServerOp;	
	return 0;
}

int KeyMngServerApp::setInfo(MngServer_Info *pMngServerInfo)
{
	m_pMngServerInfo = pMngServerInfo;
	return 0;
}

int	g_EndTag = 0 ;

//
void mysighandler_t(int arg)
 {
 	printf("server_server_server_server_func mysighandler_t() begin arg:%d\n", arg);
 	g_EndTag = 1;
 	
 	printf("server_server_server_server_func mysighandler_t() end\n");
 	
 	return ;
 }
 
int KeyMngServerApp::init()
{
	int 		ret = 0;
	//安装一个信号处理函数
	signal(SIGUSR1, mysighandler_t); //告诉linux内核 当有人向我发送用户自定义信号1的时候 请你调用myserver_server_server_func api函数
	signal(SIGPIPE, SIG_IGN); //告诉linux内核 当有管道破裂时候 ，请你不要在发送SIGPIPE 让1.exe死掉 请你忽略
	
	memset(m_pMngServerInfo, 0, sizeof(MngServer_Info));
	
	
	//初始化服务器 全局变量
	ret = m_pKeyMngServerOp->MngServer_InitInfo(m_pMngServerInfo);
	if (ret != 0)
	{
		printf("server_server_server_func MngServer_InitInfo() err:%d \n", ret);
		return ret;
	}
	
	//函数声明
	//服务器端初始化
	ret =  sckServer_init(m_pMngServerInfo->serverport, &m_listenfd);  //应该把listenfd 变成app类的属性
	if (ret != 0)
	{
		printf("server_server_server_func sckServer_init() err:%d \n", ret);
		return ret;	
	}
	
	return ret;
}




struct ThreadInfo
{
	KeyMngServerApp 	*keyMngServerApp;
	int 				connfd;
};

//业务线程

//1 收客户端的请求报文 
//2 解析客户端的请求报文
//3 cmdType 123
//4 组织应答报文 编码应答报文
//5 回发应答报文

void *mystart_routine (void *arg)
{
	
	int 					ret = 0;
	
	int 					connfd   = 0;
	int 					mytimeout = 3;
	
	//接受的客户端的请求报文 编码以后的结果
	unsigned char 			*out = NULL;
	int 					outlen = 0;
		
		
	//客户端请求报文数据结构
	MsgKey_Req				*pMsgKeyReq = NULL;
	int 					iMsgKeyReq = 0;
	
	//服务器应答报文 编码以后的结果
	unsigned char 			*pMsgKey_ResData = NULL;
	int 					iMsgKey_ResDataLen = 0;
	
	
	ThreadInfo 		*tmpThreadInfo = reinterpret_cast<ThreadInfo*>(arg);
	
	KeyMngServerAbsOp 	*pKeyMngServerOp = NULL;
	
	//方法1: 在提供一个get接口
	//方法2: 把私有属性改成public
	//方法3: 在全局函数mystart_routine中,想使用 KeyMngServerApp类的私有属性(m_pKeyMngServerOp);
	//可以把全局函数声明成 KeyMngServerApp类的好朋友
	pKeyMngServerOp = tmpThreadInfo->keyMngServerApp->m_pKeyMngServerOp;
	connfd = tmpThreadInfo->connfd;
	
	
	while (1)
	{
		if (g_EndTag ==1)
		{
			break;
		}
		out = NULL;
		outlen = 0;
		
		//客户端请求报文数据结构
		pMsgKeyReq = NULL;
		iMsgKeyReq = 0;
		
		//服务器应答报文 编码以后的结果
		pMsgKey_ResData = NULL;
		iMsgKey_ResDataLen = 0;
	
		
		//服务器端端接受报文
		//异常情况 1对方已经关闭 2 出错 3 超时
		ret =  sckServer_rev(connfd, mytimeout, &out, &outlen); //1 sck_FreeMem((void ** )&out);
		if (ret == Sck_ErrPeerClosed)
		{
			printf("服务端监测出 客户端已经关闭\n");
			break;	
		}
		else if (ret == Sck_ErrTimeOut)
		{
			goto End;	
		}
		else if (ret != 0)
		{
			printf("server_server_server_func sckServer_rev() err:%d \n", ret);
			break;
		}
	

		//2 解析客户端的请求报文
		ret = MsgDecode(out, outlen, (void **)&pMsgKeyReq, &iMsgKeyReq); //2  MsgMemFree((void **) &pMsgKeyReq, iMsgKeyReq);
		if (ret != 0)
		{
			printf("server_server_server_func MsgDecode() err:%d \n", ret);
			goto End;
		}
         
        // 验证报文合法性
        
		//3 cmdType 123
		switch (pMsgKeyReq->cmdType)
		{
		case KeyMng_NEWorUPDATE:
			//密钥协商应答
			ret = pKeyMngServerOp->MngServer_Agree(tmpThreadInfo->keyMngServerApp->m_pMngServerInfo, pMsgKeyReq, &pMsgKey_ResData, &iMsgKey_ResDataLen); //3 MsgMemFree( (void **) &pMsgKey_ResData, 0);
			break;
		case KeyMng_Check:
			//密钥校验
			printf("秘钥校验");
			ret = pKeyMngServerOp->MngServer_Check(tmpThreadInfo->keyMngServerApp->m_pMngServerInfo, pMsgKeyReq, &pMsgKey_ResData, &iMsgKey_ResDataLen); 
			break;
		case KeyMng_Revoke:
			ret = pKeyMngServerOp->MngServer_Revoke(tmpThreadInfo->keyMngServerApp->m_pMngServerInfo, pMsgKeyReq, &pMsgKey_ResData, &iMsgKey_ResDataLen);
			break;
		default:
			ret = MngSvr_ParamErr;
			break;		
			
		}
		if  (ret != 0)
		{
			printf("服务器端业务流处理失败 err:%d .\n",ret);
			goto End;	
		}
	
		//服务器端发送报文
		ret = sckServer_send(connfd, mytimeout, pMsgKey_ResData, iMsgKey_ResDataLen);
		if (ret == Sck_ErrPeerClosed)
		{
			printf("服务端监测出 客户端已经关闭\n");
			break;	
		}
		else if (ret == Sck_ErrTimeOut)
		{
			goto End;	
		}
		else if (ret != 0)
		{
			sck_FreeMem((void ** )&out);
			printf("server_server_server_server_func sckServer_rev() err:%d \n", ret);
			break;
		}
	
	End:
			if (out) 				sck_FreeMem((void ** )&out);
			if (pMsgKeyReq)			MsgMemFree((void **) &pMsgKeyReq, iMsgKeyReq);
			if (pMsgKey_ResData)	MsgMemFree( (void **) &pMsgKey_ResData, 0);
	}		
	
	if (out) 				sck_FreeMem((void ** )&out);
	if (pMsgKeyReq)			MsgMemFree((void **) &pMsgKeyReq, iMsgKeyReq);
	if (pMsgKey_ResData)	MsgMemFree( (void **) &pMsgKey_ResData, 0);

	
	
	//sckServer_close(connfd);
	
	
	delete tmpThreadInfo;
	return NULL;	
}


int KeyMngServerApp::run()
{
	int 		ret = 0;
	int 		connfd = 0;
	int 		mytime = 3;
	pthread_t	pid;
	
	while (1)
	{
		if (g_EndTag == 1)
		{
			break;
		}
		ret = sckServer_accept(m_listenfd, mytime, &connfd);
		if (ret == Sck_ErrTimeOut)
		{
			//printf("server_server_server_server_func sckServer_accept() 检测到内核中没有连接 \n ");
			continue;
		}
		
		//C++里面 使用业务类对象/连接/配置信息
		
		ThreadInfo  *pThreadInfo = new ThreadInfo;
		pThreadInfo->keyMngServerApp = this; //把app类的对象 传入到线程体中
		pThreadInfo->connfd = connfd;

       pthread_create(&pid,  NULL, mystart_routine, (void *)pThreadInfo);

	}
	
	
	return 0;
}

int KeyMngServerApp::exit()
{
	sleep(2);
	
	//释放资源
	sckClient_closeconn(m_listenfd);
	//服务器端环境释放 
	sckServer_destroy();
	
	//释放数据库连接池
	IC_DBApi_PoolFree();

	printf("keymngserver优雅退出\n");
	return 0;
}

	
