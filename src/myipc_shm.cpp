
#define	_OS_LINUX_

#if defined _OS_LINUX_
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include "myipc_shm.h" 

#endif

#include "myipc_shm.h"

int MyIpc_ShmHelper::shmflag = 0;
int MyIpc_ShmHelper::shmkey;


//创建共享内存 若共享内存不存在，则创建 若存在使用原来的
int MyIpc_ShmHelper::IPC_CreatShm(int key, int shmsize, int *shmhdl)
{
	int		tmpshmhdl = 0;
	int 	ret = 0;
	 //	创建共享内存 
	 //	若共享内存不存在则创建 
	 //	若共享内存已存在使用原来的
    tmpshmhdl = shmget(key, shmsize, IPC_CREAT|0666);
    if (tmpshmhdl == -1)			//创建失败
    {
    	ret = MYIPC_ParamErr;
    	printf("func shmget() err :%d ", ret);
    	return ret;
    }
    *shmhdl = tmpshmhdl;
	return ret;
}

//打开共享内存 若共享内存不存在，返回错误
//参数 无意义 可填写0
int MyIpc_ShmHelper::IPC_OpenShm(int key, int shmsize, int *shmhdl)
{
	int		tmpshmhdl = 0;
	int 	ret = 0;
	 //	创建共享内存 
	 //	若共享内存不存在则创建 
	 //	若共享内存已存在使用原来的
    tmpshmhdl = shmget(key, 0, 0);
    if (tmpshmhdl == -1)			//打开失败
    {
    	ret = MYIPC_NotEXISTErr;
    	//printf("func shmget() err :%d ", ret);
    	return ret;
    }
    *shmhdl = tmpshmhdl;
	return ret;
	
}

/***********************************************************************
  功能描述：    创建共享内存
  参数说明：    shmname  [in]  是共享内存名,系统中唯一标志
                shmsize  [in]  是要创建的共享内存的大小；
                shmhdl   [out] 共享内存的句柄.
  返回值：      返回0函数执行成功；非0返回错误码
************************************************************************/
int MyIpc_ShmHelper::IPC_CreatShmBySeedName(char *shmseedfile, int shmsize, int *shmhdl)
{
    if(shmflag == 0)			//判断接口中共享内存key是否已经存在
    {
    	shmkey = ftok(shmseedfile, 'c');
    	if (shmkey == -1)
    	{
    		perror("ftok");
    		return -1;
    	}
    	    
    	shmflag = 1;
    }
    
    //创建共享内存
    *shmhdl = shmget(shmkey,shmsize,IPC_CREAT|0666);
    if (*shmhdl == -1)			//创建失败
		return -2;
	return 0;

}
/***********************************************************************
  功能描述：    关联共享内存
  参数说明：    shmhdl	[in]  共享的句柄
                mapaddr [out] 共享内存首地址
  返回值：      返回0函数执行成功；非0返回错误码
************************************************************************/
int  MyIpc_ShmHelper::IPC_MapShm(int  shmhdl, void  **mapaddr)
{
    void *tempptr = NULL;

    //连接共享内存
    tempptr = (void *)shmat(shmhdl,0,SHM_RND);
    
    //C++的类型转换 指针 转换 变量 
  	//int myTmp =  reinterpret_cast<int>(tempptr);
  	long myTmp =  reinterpret_cast<long>(tempptr);
    
    if (myTmp == -1)		//共享内存连接失败
		return -1;
    *mapaddr = tempptr;			//导出共享内存首指针

    return 0;
}
/***********************************************************************
  功能描述：    取消共享内存关联
  参数说明：    unmapaddr   [in] 共享内存首地址
  返回值：      返回0函数执行成功；非0返回错误码
************************************************************************/
int MyIpc_ShmHelper::IPC_UnMapShm(void *unmapaddr)
{
    int  rv;
    //取消连接共享内存 
    rv = shmdt((char *)unmapaddr);
    if (rv == -1)			//取消连接失败
		return -1;

    return 0;
}
/***********************************************************************
  功能描述：    删除共享内存
  参数说明：    shmhdl	[in]  共享的句柄
  返回值：      返回0函数执行成功；非0返回错误码
************************************************************************/
int MyIpc_ShmHelper::IPC_DelShm(int shmhdl)
{
    int  rv;
    //删除共享内存
    rv = shmctl(shmhdl,IPC_RMID,NULL);
    if(rv < 0)				//删除共享内存失败
		return -1;
    return 0;
}
