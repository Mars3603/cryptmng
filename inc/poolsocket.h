#ifndef _poolsocket_H_
#define _poolsocket_H_

#ifdef __cplusplus
extern "C"
{
#endif

//错误码定义  
#define Sck_Ok             	0
#define Sck_BaseErr   		3000

#define Sck_ErrParam                	(Sck_BaseErr+1)
#define Sck_ErrTimeOut                	(Sck_BaseErr+2)
#define Sck_ErrPeerClosed               (Sck_BaseErr+3)
#define Sck_ErrMalloc			   		(Sck_BaseErr+4)

#define Sck_Err_Pool_CreateConn				(Sck_BaseErr+20)  //创建连接池 （没有达到最大连接数）
#define Sck_Err_Pool_terminated				(Sck_BaseErr+21) //已终止
#define Sck_Err_Pool_GetConn_ValidIsZero	(Sck_BaseErr+22) //有效连接数是零
#define Sck_Err_Pool_HaveExist				(Sck_BaseErr+22) //连接已经在池中
#define Sck_Err_Pool_ValidBounds			(Sck_BaseErr+22) //有效连接数目超过了最大连接数

//客户端 初始化
int sckClient_init();

//客户端 连接服务器
int sckClient_connect(char *ip, int port, int connecttime, int *connfd);

//客户端 关闭和服务端的连接
int sckClient_closeconn(int connfd);

//客户端 发送报文
int sckClient_send(int connfd, int sendtime, unsigned char *data, int datalen);

//客户端 接受报文
int sckClient_rev(int connfd, int revtime, unsigned char **out, int *outlen); //1

//客户端 释放内存
int sck_FreeMem(void **buf);
//客户端 释放
int sckClient_destroy();


typedef struct _SCKClitPoolParam
{
	char 	serverip[64];
	int 	serverport;
	int 	bounds; //池容量
	int 	connecttime;
	int 	sendtime;
	int 	revtime;
}SCKClitPoolParam;


//客户端 socket池初始化
int sckCltPool_init(void **handle, SCKClitPoolParam *param);

//客户端 socket池 获取一条连接 
int sckCltPool_getConnet(void *handle, int *connfd);

//客户端 socket池 发送数据 
int sckCltPool_send(void *handle, int  connfd,  unsigned char *data, int datalen);

//客户端 socket池 接受数据
int sckCltPool_rev(void *handle, int  connfd, unsigned char **out, int *outlen); //1

//客户端 socket池 把连接放回 socket池中 
int sckCltPool_putConnet(void *handle, int connfd, int validFlag); //0正常 1

//客户端 socket池 销毁连接
int sckCltPool_destroy(void *handle);



//函数声明
//服务器端初始化
int sckServer_init(int port, int *listenfd);

int sckServer_accept(int listenfd, int timeout, int *connfd);
//服务器端发送报文
int sckServer_send(int connfd, int timeout, unsigned char *data, int datalen);
//服务器端端接受报文
int sckServer_rev(int  connfd, int timeout, unsigned char **out, int *outlen); //1

int sckServer_close(int connfd);

//服务器端环境释放 
int sckServer_destroy();



#ifdef __cplusplus
}
#endif


#endif