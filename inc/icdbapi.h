#ifndef _IC_DBApi_H_
#define _IC_DBApi_H_


#ifdef  __cplusplus
extern "C" {
#endif


#ifndef ICDBHandle
#define ICDBHandle			void*
#endif

#ifndef ICDBCursor
#define ICDBCursor			void*
#endif

#ifndef ICDBLob
#define ICDBLob				void*
#endif


#define ICDBBLOB			0
#define ICDBCLOB			1
#define ICDBNCLOB			2

#define MAXBLOBDATALEN		1024


#ifndef ICDBField
typedef struct _ICDBField
{
	char*	cont;
	int		contLen;
}ICDBField;
#endif

#ifndef ICDBRow
typedef struct _ICDBRow
{
	ICDBField*	field;
	int			fieldCount;
}ICDBRow;
#endif


/*实际使用的Level(如果[1]=0或1(debug)，[2]=0或2(info)，[3]=0或3(warning)，[4]=0或4(error)),为0则表示不写相应的日志*/
//extern int  IC_DB_DTUseLevel[5];
extern int  DTORADBLevel[5];


/********************************************************/
/*程 序 名：	IC_DBApi_PoolInit						*/
/*功能描述：	数据库连接池初始化						*/
/*参数说明：	bounds		连接池容量					*/
/*				dbName		数据库名称					*/
/*				dbUser		数据库用户					*/
/*				dbPswd		数据库用户口令				*/
/********************************************************/
int IC_DBApi_PoolInit(int bounds, char* dbName, char* dbUser, char* dbPswd);

// SECMNG/SCEMNG@orcl


/********************************************************/
/*程 序 名：	IC_DBApi_ConnGet						*/
/*功能描述：	从连接池获取数据库连接					*/
/*参数说明：	handle		数据库连接句柄				*/
/*				sTimeout	超时时间(秒)				*/
/*				nsTimeout	超时时间(毫秒)				*/
/********************************************************/
int IC_DBApi_ConnGet(ICDBHandle* handle, int sTimeout, int nsTimeout);


/********************************************************/
/*程 序 名：	IC_DBApi_ConnFree						*/
/*功能描述：	释放数据库连接到连接池					*/
/*参数说明：	handle		数据库连接句柄				*/
/*				validFlag	数据库连接有效性		1	*/
/********************************************************/
int IC_DBApi_ConnFree(ICDBHandle handle, int validFlag);


/********************************************************/
/*程 序 名：	IC_DBApi_PoolFree						*/
/*功能描述：	释放数据库连接池						*/
/*参数说明：											*/
/********************************************************/
int IC_DBApi_PoolFree();


/********************************************************/
/*程 序 名：	IC_DBApi_ExecNSelSql					*/
/*功能描述：	执行单个非select数据库语句(不包含事务)	*/
/*参数说明：	handle		数据库连接句柄				*/
/*				sql			数据库语句					*/
/********************************************************/
int IC_DBApi_ExecNSelSql(ICDBHandle handle, char* sql);


/********************************************************/
/*程 序 名：	IC_DBApi_ExecNSelSqls					*/
/*功能描述：	执行多个非select数据库语句(包含事务)	*/
/*参数说明：	handle		数据库连接句柄				*/
/*				sqls		数据库语句(以NULL结束)		*/
/********************************************************/
int IC_DBApi_ExecNSelSqls(ICDBHandle handle, char* sqls[]);


/********************************************************/
/*程 序 名：    IC_DBApi_BeginTran                     */
/*功能描述：    数据库事务开始                          */
/*参数说明：    handle      数据库连接句柄              */
/********************************************************/
int IC_DBApi_BeginTran(ICDBHandle handle);


/********************************************************/
/*程 序 名：	IC_DBApi_Rollback						*/
/*功能描述：	数据库事务回退							*/
/*参数说明：	handle		数据库连接句柄				*/
/********************************************************/
int IC_DBApi_Rollback(ICDBHandle handle);


/********************************************************/
/*程 序 名：	IC_DBApi_Commit						*/
/*功能描述：	数据库事务提交							*/
/*参数说明：	handle		数据库连接句柄				*/
/********************************************************/
int IC_DBApi_Commit(ICDBHandle handle);


/********************************************************/
/*程 序 名：	IC_DBApi_ExecSelSql					*/
/*功能描述：	执行select数据库语句(返回单条记录)		*/
/*参数说明：	handle		数据库连接句柄				*/
/*				sql			数据库语句					*/
/*				row			返回的记录					*/
/********************************************************/
int IC_DBApi_ExecSelSql(ICDBHandle handle, char* sql, ICDBRow* row);


/********************************************************/
/*程 序 名：    IC_DBApi_ExecSelSqlMR                  */
/*功能描述：    执行select数据库语句(返回多条记录)      */
/*参数说明：    handle      数据库连接句柄              */
/*              sql         数据库语句                  */
/*              beginNo     返回记录的开始条数          */
/*              count       返回记录的条数              */
/*				rows		返回的记录					*/
/*              allRecNum   符合条件的记录总数          */
/********************************************************/
int IC_DBApi_ExecSelSqlMR(ICDBHandle handle, char* sql, int beginNo, int count, ICDBRow* rows, int* allRecNum);


/********************************************************/
/*程 序 名：    IC_DBApi_OpenCursor                    */
/*功能描述：    打开游标								*/
/*参数说明：    handle      数据库连接句柄              */
/*              sql         数据库语句                  */
/*              fieldCount  字段个数	                */
/*              pCursor     游标指针			        */
/********************************************************/
int IC_DBApi_OpenCursor(ICDBHandle handle, char* sql, int fieldCount, ICDBCursor* pCursor);


/********************************************************/
/*程 序 名：    IC_DBApi_FetchByCursor                 */
/*功能描述：    通过游标获取数据						*/
/*参数说明：    handle      数据库连接句柄              */
/*              cursor      游标				       	*/
/*				row			返回的记录					*/
/********************************************************/
int IC_DBApi_FetchByCursor(ICDBHandle handle, ICDBCursor cursor, ICDBRow* row);


/********************************************************/
/*程 序 名：    IC_DBApi_CloseCursor                   */
/*功能描述：    关闭游标								*/
/*参数说明：    handle      数据库连接句柄              */
/*              pCursor     游标指针			        */
/********************************************************/
int IC_DBApi_CloseCursor(ICDBHandle handle, ICDBCursor* pCursor);

/********************************************************/
/*程 序 名：    IC_DBApi_Pem2Der			            */
/*功能描述：    PEM编码转DER编码						*/
/*参数说明：    pemData     PEM数据              		*/
/*              pemDataLen  PEM数据长度			        */
/*              derData     DER数据			        	*/
/*              derDataLen  DER数据长度			        */
/********************************************************/
int IC_DBApi_Pem2Der(char* pemData, int pemDataLen, unsigned char* derData, int* derDataLen);


/********************************************************/
/*程 序 名：    IC_DBApi_Der2Pem			            */
/*功能描述：    PEM编码转DER编码						*/
/*参数说明：    derData     DER数据              		*/
/*              derDataLen  DER数据长度			        */
/*              pemData     PEM数据			        	*/
/*              pemDataLen  PEM数据长度			        */
/********************************************************/
int IC_DBApi_Der2Pem(unsigned char* derData, int derDataLen, char* pemData, int* pemDataLen);


/********************************************************/
/*程 序 名：    IC_DBApi_GetDBTime			            */
/*功能描述：    获取数据库时间							*/
/*参数说明：    handle      数据库连接句柄              */
/*              dbTime   	数据库时间             		*/
/********************************************************/
int IC_DBApi_GetDBTime(ICDBHandle handle, char* dbTime);




#define IC_DB_BASE_ERR						17000

#define IC_DB_OK							0
#define IC_DB_NODATA_AFFECT					100

#define IC_DB_PARAM_ERR						IC_DB_BASE_ERR+1		//SQLDA初始化错误
#define IC_DB_SQLDAINIT_ERR					IC_DB_BASE_ERR+1		//SQLDA初始化错误
#define IC_DB_SQLRELEASE_ERR				IC_DB_BASE_ERR+2		//提交事务并释放连接语句错误
#define IC_DB_SQLALTERDATE_ERR				IC_DB_BASE_ERR+3		//修改连接的时间格式语句错误
#define IC_DB_SQLCONNECT_ERR				IC_DB_BASE_ERR+4		//连接数据库语句错误
#define IC_DB_PTHREAD_MUTEX_INIT_ERR		IC_DB_BASE_ERR+5		//初始化互斥错误
#define IC_DB_PTHREAD_COND_INIT_ERR			IC_DB_BASE_ERR+6		//初始化条件错误
#define IC_DB_SQLENABLETHREADS_ERR			IC_DB_BASE_ERR+7		//设置使用线程语句错误
#define IC_DB_MALLOC_ERR					IC_DB_BASE_ERR+8		//分配内存错误
#define IC_DB_CONTEXTALLOCATE_ERR			IC_DB_BASE_ERR+9		//为连接句柄分配内存语句错误
#define IC_DB_CONNECT_ERR					IC_DB_BASE_ERR+10	//数据库连接错误
#define IC_DB_PTHREAD_MUTEX_LOCK_ERR		IC_DB_BASE_ERR+11	//获取互斥锁错误
#define IC_DB_ALLCONN_NOTVALID_ERR			IC_DB_BASE_ERR+12	//连接池所有连接都是无效连接错误
#define IC_DB_CONDTIMEOUT_ERR				IC_DB_BASE_ERR+13	//获取连接超时错误
#define IC_DB_PTHREAD_COND_TIMEDWAIT_ERR	IC_DB_BASE_ERR+14	//线程条件超时等待函数错误
#define IC_DB_POOLTERMINATED_ERR			IC_DB_BASE_ERR+15	//连接池已经终止错误
#define IC_DB_GETCOND_VALIDISZERO_ERR		IC_DB_BASE_ERR+16	//获取条件后有效连接仍未0错误
#define IC_DB_PTHREAD_MUTEX_UNLOCK_ERR		IC_DB_BASE_ERR+17	//释放互斥锁错误
#define IC_DB_VALIDBOUNDS_ERR				IC_DB_BASE_ERR+18	//有效连接数大于最大容量错误
#define IC_DB_NVALIDBOUNDS_ERR				IC_DB_BASE_ERR+19	//无效连接数大于最大容量错误
#define IC_DB_POOL_WAKEALL_ERR				IC_DB_BASE_ERR+20	//唤醒所有连接的阻塞状态错误
#define IC_DB_SQLEXECIMMEDIAT_ERR			IC_DB_BASE_ERR+21	//立即执行SQL语句错误
#define IC_DB_COMMIT_ERR					IC_DB_BASE_ERR+22	//事务提交语句错误
#define IC_DB_ROLLBACK_ERR					IC_DB_BASE_ERR+23	//事务回退语句错误
#define IC_DB_PREPARESTMT_ERR				IC_DB_BASE_ERR+24	//准备查询语句错误
#define IC_DB_DESCRIBESELECT_ERR			IC_DB_BASE_ERR+25	//填充SQLDA语句错误
#define IC_DB_FIELDCOUNT_ERR				IC_DB_BASE_ERR+26	//输入的字段数目错误
#define IC_DB_FIELDNULL_ERR					IC_DB_BASE_ERR+27	//输入的字段为空错误
#define IC_DB_SETROWDATAMEMORY_ERR			IC_DB_BASE_ERR+28	//为字段设置存贮空间错误
#define IC_DB_DECLARECURSOR_ERR				IC_DB_BASE_ERR+29	//定义游标语句错误
#define IC_DB_OPENCURSOR_ERR				IC_DB_BASE_ERR+30	//打开游标语句错误
#define IC_DB_FETCH_ERR						IC_DB_BASE_ERR+31	//通过游标获取数据语句错误
#define IC_DB_CLOSECURSOR_ERR				IC_DB_BASE_ERR+32	//关闭游标语句错误
#define IC_DB_INPUT_ERR						IC_DB_BASE_ERR+33	//输入条件错误
#define IC_DB_GETROWCOUNT_ERR				IC_DB_BASE_ERR+34	//获取符合条件的记录总数错误
#define IC_DB_LOBDESCRIBE_ERR				IC_DB_BASE_ERR+35	//获取原有LOB数据属性语句错误
#define IC_DB_LOBWRITE_ERR					IC_DB_BASE_ERR+36	//往LOB字段写数据语句错误
#define IC_DB_ALLOCATELOB_ERR				IC_DB_BASE_ERR+37	//分配BLOB语句错误
#define IC_DB_LOBREAD_ERR					IC_DB_BASE_ERR+38	//从LOB字段读数据语句错误
#define IC_DB_LOBFREE_ERR					IC_DB_BASE_ERR+39	//释放LOB语句错误
#define IC_DB_LOBCLOSE_ERR					IC_DB_BASE_ERR+40	//关闭LOB语句错误
#define IC_DB_LOBNOTOPEN_ERR				IC_DB_BASE_ERR+41	//Lob没有打开错误
#define IC_DB_LOBOPEN_ERR					IC_DB_BASE_ERR+42	//打开LOB错误
#define IC_DB_SQLEXECBEGINWORK_ERR			IC_DB_BASE_ERR+43	//开始事务错误
#define IC_DB_SQLGETTIME_ERR				IC_DB_BASE_ERR+44	//获取数据库时间错误
/*#define 				IC_DB_BASE_ERR+		//错误
#define 				IC_DB_BASE_ERR+45	//错误
#define 				IC_DB_BASE_ERR+46	//错误
#define 				IC_DB_BASE_ERR+47	//错误
#define 				IC_DB_BASE_ERR+48	//错误
#define 				IC_DB_BASE_ERR+49	//错误*/



#define IC_DB_CONNECTION_ERR				-3113					//与数据库的连接错误
#define IC_DB_CONNECTION2_ERR				-3114					//与数据库的连接错误


#ifdef __cplusplus
}
#endif


#endif
