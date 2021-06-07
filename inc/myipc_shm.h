

// myipc_shm.h
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//共享内存错误码
#define		MYIPC_OK				0		//正确
#define		MYIPC_ParamErr			301		//输入参数失败
#define		MYIPC_NotEXISTErr		302		//共享内存不存在错误
#define		MYIPC_CreateErr			303		//创建共享内存错误

class MyIpc_ShmHelper
{
public:
	//创建共享内存 若共享内存不存在，则创建
	static int IPC_CreatShm(int key, int shmsize, int *shmhdl);
	
	//打开共享内存 若共享内存不存在，返回错误
	static int IPC_OpenShm(int key, int shmsize, int *shmhdl);
	
	
	/***********************************************************************
	描述：    创建共享内存 通过种子文件
	参数说明：    shmname  [in]  是共享内存名,系统中唯一标志
	            shmsize  [in]  是要创建的共享内存的大小；
	            shmhdl   [out] 共享内存的句柄.
	回值：      返回0函数执行成功；非0返回错误码
	************************************************************************/
	static int IPC_CreatShmBySeedName(char *shmname, int shmsize, int *shmhdl);
	
	/***********************************************************************
	描述：    关联共享内存
	参数说明：    shmhdl	[in]  共享的句柄
	            mapaddr [out] 共享内存首地址
	回值：      返回0函数执行成功；非0返回错误码
	************************************************************************/
	static int IPC_MapShm(int shmhdl,void **mapaddr);
	
	/***********************************************************************
	描述：    取消共享内存关联
	参数说明：    unmapaddr   [in] 共享内存首地址
	回值：      返回0函数执行成功；非0返回错误码
	************************************************************************/
	static int IPC_UnMapShm(void *unmapaddr);
	
	/***********************************************************************
	描述：    删除共享内存
	参数说明：    shmhdl	[in]  共享的句柄
	回值：      返回0函数执行成功；非0返回错误码
	************************************************************************/
	static int IPC_DelShm(int shmhdl);
	
public:
	static int shmflag;
	static int shmkey;
};








