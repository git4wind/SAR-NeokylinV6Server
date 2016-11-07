/*
 * author:pc
 * date:2015-10-15 15:36
 * desc:
 * 		Package the sqlite to use
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sqlite3.h>

static sqlite3 *db;
static char cpDBPaths[256];
static char cpDBPwd[32];
static pthread_mutex_t execLock;

/**
 * set the database path and password
 */
int hbla_sql_init(const char *path,const char *pwd)
{
	int ret;
	if(path == NULL || pwd == NULL)
		return -1;
	if(db != NULL)
		return 0;
	memcpy(cpDBPaths, path, 256);
	memcpy(cpDBPwd, pwd, 32);
	pthread_mutex_init(&execLock, NULL);

	ret = sqlite3_open(cpDBPaths,&db);
	if(ret != SQLITE_OK)
	{
		if(db != NULL)
			sqlite3_close(db);
		return -1;
	}
	sqlite3_key(db, cpDBPwd, strlen(cpDBPwd));
	
	//pthread_mutex_init(&execLock, NULL); 2016 3 7 with y
	return 0;
}

/**
 * set the database path and password
 */
int hbla_sql_signle_init(const char *path)
{
	int ret;
	if(path == NULL)
		return -1;
	if(db != NULL)
		return 0;
	memcpy(cpDBPaths, path, 256);
	pthread_mutex_init(&execLock, NULL);

	ret = sqlite3_open(cpDBPaths,&db);
	if(ret != SQLITE_OK)
	{
		if(db != NULL)
			sqlite3_close(db);
		return -1;
	}
	
	pthread_mutex_init(&execLock, NULL);
	return 0;
}

/**
 * exec insert && update which not need return callbak
 */
int hbla_sql_exec(const char *sql)
{
	char *err;
	int ret;
	if(sql == NULL)
		return -1;
	pthread_mutex_lock(&execLock);
	ret = sqlite3_exec(db, sql, NULL, NULL, &err);
	if(ret != SQLITE_OK)
	{
		fputs(err, stderr);
		pthread_mutex_unlock(&execLock);
		return -1;
	}
	pthread_mutex_unlock(&execLock);
	return 0;
}

static int hbla_sql_selectfun(void *p, int argc, char **argv, char **argvv)
{
	if(p == NULL)
		return -1;
	if(argv[0] != NULL)
		memcpy(p, argv[0], strlen(argv[0]));
	return 0;
}

/**
 * exec the select which need return callbak
 */
int hbla_sql_selectStr(const char *sql, char *pData)
{
	char *err;
	int ret;
	if(pData == NULL)
		return -1;

	pthread_mutex_lock(&execLock);
   	ret = sqlite3_exec(db, sql, hbla_sql_selectfun, pData, &err);
	if(ret != SQLITE_OK)
	{
		fputs(err, stderr);
		pthread_mutex_unlock(&execLock);
		return -1;
	}
	pthread_mutex_unlock(&execLock);
	return 0;
}

/**
 * exec the selct which need a bind info
 */
int hbla_sql_selectBind(const char *sql, void *pData, int *len)
{
	sqlite3_stmt *stmt;
	char *temData;

	if(NULL == sql || NULL == pData)
		return -1;
	
	pthread_mutex_lock(&execLock);
	sqlite3_prepare(db, sql, -1, &stmt, 0);
	sqlite3_step(stmt);
	temData = (char *)sqlite3_column_blob(stmt, 0);
	*len = sqlite3_column_bytes(stmt, 0);
	memcpy(pData, temData, *len);
	sqlite3_finalize(stmt);
	pthread_mutex_unlock(&execLock);

	return 0;
}

/**
 * exec the select which need a bind info v2
 */
int hbla_sql_selectBindMsg(const char *sql, void *pData, int *len, 
				int *cMNo, int *sCNo)
{
	sqlite3_stmt *stmt;
	char *temData;

	if(NULL == sql || NULL == pData)
		return -1;
	
	pthread_mutex_lock(&execLock);
	sqlite3_prepare(db, sql, -1, &stmt, 0);
	sqlite3_step(stmt);
	temData = (char *)sqlite3_column_blob(stmt, 0);
	*len = sqlite3_column_bytes(stmt, 0);
	*cMNo = sqlite3_column_int(stmt, 1);
	*sCNo = sqlite3_column_int(stmt, 2);
	memcpy(pData, temData, *len);
	sqlite3_finalize(stmt);
	pthread_mutex_unlock(&execLock);

	return 0;
}

/**
 * update & insert the bind info 
 */
int hbla_sql_execBind(const char *sql, void *pData, int len)
{
	int ret;
	sqlite3_stmt *stmt;

	if(NULL == pData || len < 1)
		return -1;
	
	pthread_mutex_lock(&execLock);
	sqlite3_prepare(db, sql, -1, &stmt, NULL);
	sqlite3_bind_blob(stmt, 1, pData, len, NULL);
	ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	pthread_mutex_unlock(&execLock);
	if(ret != SQLITE_DONE)
		return -1;

	return 0;
}

/**
 * update & insert the bind info v2
 */
int hbla_sql_execBindMsg(const char *sql, void *pData, int len,int cMNo, int sCNo)
{
	int ret;
	sqlite3_stmt *stmt;

	if(NULL == pData || len < 1)
		return -1;
	
	pthread_mutex_lock(&execLock);
	sqlite3_prepare(db, sql, -1, &stmt, NULL);
	sqlite3_bind_blob(stmt, 1, pData, len, NULL);
	sqlite3_bind_int(stmt, 2, cMNo);
	sqlite3_bind_int(stmt, 3, sCNo);
	ret = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	pthread_mutex_unlock(&execLock);
	if(ret != SQLITE_DONE)
		return -1;

	return 0;
}
