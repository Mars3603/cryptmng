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


/*ʵ��ʹ�õ�Level(���[1]=0��1(debug)��[2]=0��2(info)��[3]=0��3(warning)��[4]=0��4(error)),Ϊ0���ʾ��д��Ӧ����־*/
//extern int  IC_DB_DTUseLevel[5];
extern int  DTORADBLevel[5];


/********************************************************/
/*�� �� ����	IC_DBApi_PoolInit						*/
/*����������	���ݿ����ӳس�ʼ��						*/
/*����˵����	bounds		���ӳ�����					*/
/*				dbName		���ݿ�����					*/
/*				dbUser		���ݿ��û�					*/
/*				dbPswd		���ݿ��û�����				*/
/********************************************************/
int IC_DBApi_PoolInit(int bounds, char* dbName, char* dbUser, char* dbPswd);

// SECMNG/SCEMNG@orcl


/********************************************************/
/*�� �� ����	IC_DBApi_ConnGet						*/
/*����������	�����ӳػ�ȡ���ݿ�����					*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/*				sTimeout	��ʱʱ��(��)				*/
/*				nsTimeout	��ʱʱ��(����)				*/
/********************************************************/
int IC_DBApi_ConnGet(ICDBHandle* handle, int sTimeout, int nsTimeout);


/********************************************************/
/*�� �� ����	IC_DBApi_ConnFree						*/
/*����������	�ͷ����ݿ����ӵ����ӳ�					*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/*				validFlag	���ݿ�������Ч��		1	*/
/********************************************************/
int IC_DBApi_ConnFree(ICDBHandle handle, int validFlag);


/********************************************************/
/*�� �� ����	IC_DBApi_PoolFree						*/
/*����������	�ͷ����ݿ����ӳ�						*/
/*����˵����											*/
/********************************************************/
int IC_DBApi_PoolFree();


/********************************************************/
/*�� �� ����	IC_DBApi_ExecNSelSql					*/
/*����������	ִ�е�����select���ݿ����(����������)	*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/*				sql			���ݿ����					*/
/********************************************************/
int IC_DBApi_ExecNSelSql(ICDBHandle handle, char* sql);


/********************************************************/
/*�� �� ����	IC_DBApi_ExecNSelSqls					*/
/*����������	ִ�ж����select���ݿ����(��������)	*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/*				sqls		���ݿ����(��NULL����)		*/
/********************************************************/
int IC_DBApi_ExecNSelSqls(ICDBHandle handle, char* sqls[]);


/********************************************************/
/*�� �� ����    IC_DBApi_BeginTran                     */
/*����������    ���ݿ�����ʼ                          */
/*����˵����    handle      ���ݿ����Ӿ��              */
/********************************************************/
int IC_DBApi_BeginTran(ICDBHandle handle);


/********************************************************/
/*�� �� ����	IC_DBApi_Rollback						*/
/*����������	���ݿ��������							*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/********************************************************/
int IC_DBApi_Rollback(ICDBHandle handle);


/********************************************************/
/*�� �� ����	IC_DBApi_Commit						*/
/*����������	���ݿ������ύ							*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/********************************************************/
int IC_DBApi_Commit(ICDBHandle handle);


/********************************************************/
/*�� �� ����	IC_DBApi_ExecSelSql					*/
/*����������	ִ��select���ݿ����(���ص�����¼)		*/
/*����˵����	handle		���ݿ����Ӿ��				*/
/*				sql			���ݿ����					*/
/*				row			���صļ�¼					*/
/********************************************************/
int IC_DBApi_ExecSelSql(ICDBHandle handle, char* sql, ICDBRow* row);


/********************************************************/
/*�� �� ����    IC_DBApi_ExecSelSqlMR                  */
/*����������    ִ��select���ݿ����(���ض�����¼)      */
/*����˵����    handle      ���ݿ����Ӿ��              */
/*              sql         ���ݿ����                  */
/*              beginNo     ���ؼ�¼�Ŀ�ʼ����          */
/*              count       ���ؼ�¼������              */
/*				rows		���صļ�¼					*/
/*              allRecNum   ���������ļ�¼����          */
/********************************************************/
int IC_DBApi_ExecSelSqlMR(ICDBHandle handle, char* sql, int beginNo, int count, ICDBRow* rows, int* allRecNum);


/********************************************************/
/*�� �� ����    IC_DBApi_OpenCursor                    */
/*����������    ���α�								*/
/*����˵����    handle      ���ݿ����Ӿ��              */
/*              sql         ���ݿ����                  */
/*              fieldCount  �ֶθ���	                */
/*              pCursor     �α�ָ��			        */
/********************************************************/
int IC_DBApi_OpenCursor(ICDBHandle handle, char* sql, int fieldCount, ICDBCursor* pCursor);


/********************************************************/
/*�� �� ����    IC_DBApi_FetchByCursor                 */
/*����������    ͨ���α��ȡ����						*/
/*����˵����    handle      ���ݿ����Ӿ��              */
/*              cursor      �α�				       	*/
/*				row			���صļ�¼					*/
/********************************************************/
int IC_DBApi_FetchByCursor(ICDBHandle handle, ICDBCursor cursor, ICDBRow* row);


/********************************************************/
/*�� �� ����    IC_DBApi_CloseCursor                   */
/*����������    �ر��α�								*/
/*����˵����    handle      ���ݿ����Ӿ��              */
/*              pCursor     �α�ָ��			        */
/********************************************************/
int IC_DBApi_CloseCursor(ICDBHandle handle, ICDBCursor* pCursor);

/********************************************************/
/*�� �� ����    IC_DBApi_Pem2Der			            */
/*����������    PEM����תDER����						*/
/*����˵����    pemData     PEM����              		*/
/*              pemDataLen  PEM���ݳ���			        */
/*              derData     DER����			        	*/
/*              derDataLen  DER���ݳ���			        */
/********************************************************/
int IC_DBApi_Pem2Der(char* pemData, int pemDataLen, unsigned char* derData, int* derDataLen);


/********************************************************/
/*�� �� ����    IC_DBApi_Der2Pem			            */
/*����������    PEM����תDER����						*/
/*����˵����    derData     DER����              		*/
/*              derDataLen  DER���ݳ���			        */
/*              pemData     PEM����			        	*/
/*              pemDataLen  PEM���ݳ���			        */
/********************************************************/
int IC_DBApi_Der2Pem(unsigned char* derData, int derDataLen, char* pemData, int* pemDataLen);


/********************************************************/
/*�� �� ����    IC_DBApi_GetDBTime			            */
/*����������    ��ȡ���ݿ�ʱ��							*/
/*����˵����    handle      ���ݿ����Ӿ��              */
/*              dbTime   	���ݿ�ʱ��             		*/
/********************************************************/
int IC_DBApi_GetDBTime(ICDBHandle handle, char* dbTime);




#define IC_DB_BASE_ERR						17000

#define IC_DB_OK							0
#define IC_DB_NODATA_AFFECT					100

#define IC_DB_PARAM_ERR						IC_DB_BASE_ERR+1		//SQLDA��ʼ������
#define IC_DB_SQLDAINIT_ERR					IC_DB_BASE_ERR+1		//SQLDA��ʼ������
#define IC_DB_SQLRELEASE_ERR				IC_DB_BASE_ERR+2		//�ύ�����ͷ�����������
#define IC_DB_SQLALTERDATE_ERR				IC_DB_BASE_ERR+3		//�޸����ӵ�ʱ���ʽ������
#define IC_DB_SQLCONNECT_ERR				IC_DB_BASE_ERR+4		//�������ݿ�������
#define IC_DB_PTHREAD_MUTEX_INIT_ERR		IC_DB_BASE_ERR+5		//��ʼ���������
#define IC_DB_PTHREAD_COND_INIT_ERR			IC_DB_BASE_ERR+6		//��ʼ����������
#define IC_DB_SQLENABLETHREADS_ERR			IC_DB_BASE_ERR+7		//����ʹ���߳�������
#define IC_DB_MALLOC_ERR					IC_DB_BASE_ERR+8		//�����ڴ����
#define IC_DB_CONTEXTALLOCATE_ERR			IC_DB_BASE_ERR+9		//Ϊ���Ӿ�������ڴ�������
#define IC_DB_CONNECT_ERR					IC_DB_BASE_ERR+10	//���ݿ����Ӵ���
#define IC_DB_PTHREAD_MUTEX_LOCK_ERR		IC_DB_BASE_ERR+11	//��ȡ����������
#define IC_DB_ALLCONN_NOTVALID_ERR			IC_DB_BASE_ERR+12	//���ӳ��������Ӷ�����Ч���Ӵ���
#define IC_DB_CONDTIMEOUT_ERR				IC_DB_BASE_ERR+13	//��ȡ���ӳ�ʱ����
#define IC_DB_PTHREAD_COND_TIMEDWAIT_ERR	IC_DB_BASE_ERR+14	//�߳�������ʱ�ȴ���������
#define IC_DB_POOLTERMINATED_ERR			IC_DB_BASE_ERR+15	//���ӳ��Ѿ���ֹ����
#define IC_DB_GETCOND_VALIDISZERO_ERR		IC_DB_BASE_ERR+16	//��ȡ��������Ч������δ0����
#define IC_DB_PTHREAD_MUTEX_UNLOCK_ERR		IC_DB_BASE_ERR+17	//�ͷŻ���������
#define IC_DB_VALIDBOUNDS_ERR				IC_DB_BASE_ERR+18	//��Ч���������������������
#define IC_DB_NVALIDBOUNDS_ERR				IC_DB_BASE_ERR+19	//��Ч���������������������
#define IC_DB_POOL_WAKEALL_ERR				IC_DB_BASE_ERR+20	//�����������ӵ�����״̬����
#define IC_DB_SQLEXECIMMEDIAT_ERR			IC_DB_BASE_ERR+21	//����ִ��SQL������
#define IC_DB_COMMIT_ERR					IC_DB_BASE_ERR+22	//�����ύ������
#define IC_DB_ROLLBACK_ERR					IC_DB_BASE_ERR+23	//�������������
#define IC_DB_PREPARESTMT_ERR				IC_DB_BASE_ERR+24	//׼����ѯ������
#define IC_DB_DESCRIBESELECT_ERR			IC_DB_BASE_ERR+25	//���SQLDA������
#define IC_DB_FIELDCOUNT_ERR				IC_DB_BASE_ERR+26	//������ֶ���Ŀ����
#define IC_DB_FIELDNULL_ERR					IC_DB_BASE_ERR+27	//������ֶ�Ϊ�մ���
#define IC_DB_SETROWDATAMEMORY_ERR			IC_DB_BASE_ERR+28	//Ϊ�ֶ����ô����ռ����
#define IC_DB_DECLARECURSOR_ERR				IC_DB_BASE_ERR+29	//�����α�������
#define IC_DB_OPENCURSOR_ERR				IC_DB_BASE_ERR+30	//���α�������
#define IC_DB_FETCH_ERR						IC_DB_BASE_ERR+31	//ͨ���α��ȡ����������
#define IC_DB_CLOSECURSOR_ERR				IC_DB_BASE_ERR+32	//�ر��α�������
#define IC_DB_INPUT_ERR						IC_DB_BASE_ERR+33	//������������
#define IC_DB_GETROWCOUNT_ERR				IC_DB_BASE_ERR+34	//��ȡ���������ļ�¼��������
#define IC_DB_LOBDESCRIBE_ERR				IC_DB_BASE_ERR+35	//��ȡԭ��LOB��������������
#define IC_DB_LOBWRITE_ERR					IC_DB_BASE_ERR+36	//��LOB�ֶ�д����������
#define IC_DB_ALLOCATELOB_ERR				IC_DB_BASE_ERR+37	//����BLOB������
#define IC_DB_LOBREAD_ERR					IC_DB_BASE_ERR+38	//��LOB�ֶζ�����������
#define IC_DB_LOBFREE_ERR					IC_DB_BASE_ERR+39	//�ͷ�LOB������
#define IC_DB_LOBCLOSE_ERR					IC_DB_BASE_ERR+40	//�ر�LOB������
#define IC_DB_LOBNOTOPEN_ERR				IC_DB_BASE_ERR+41	//Lobû�д򿪴���
#define IC_DB_LOBOPEN_ERR					IC_DB_BASE_ERR+42	//��LOB����
#define IC_DB_SQLEXECBEGINWORK_ERR			IC_DB_BASE_ERR+43	//��ʼ�������
#define IC_DB_SQLGETTIME_ERR				IC_DB_BASE_ERR+44	//��ȡ���ݿ�ʱ�����
/*#define 				IC_DB_BASE_ERR+		//����
#define 				IC_DB_BASE_ERR+45	//����
#define 				IC_DB_BASE_ERR+46	//����
#define 				IC_DB_BASE_ERR+47	//����
#define 				IC_DB_BASE_ERR+48	//����
#define 				IC_DB_BASE_ERR+49	//����*/



#define IC_DB_CONNECTION_ERR				-3113					//�����ݿ�����Ӵ���
#define IC_DB_CONNECTION2_ERR				-3114					//�����ݿ�����Ӵ���


#ifdef __cplusplus
}
#endif


#endif
