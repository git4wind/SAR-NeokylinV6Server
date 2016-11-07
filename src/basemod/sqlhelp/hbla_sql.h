/**
 * author:pc
 * date:2015-10-15 15:36
 * desc:
 * 		Package the sqlite to use
 */
#ifndef _HBLA_SQL_H_
#define _HBLA_SQL_H_

/**
 * set the database path and password
 */
int hbla_sql_init(const char *path,const char *pwd);

/**
 * exec insert && update which not need return callbak
 */
int hbla_sql_exec(const char *sql);

/**
 * exec the select which need return callbak
 */
int hbla_sql_selectStr(const char *sql, char *pData);

/**
 * exec the selct which need a bind info
 */
int hbla_sql_selectBind(const char *sql, void *pData, int *len);

/**
 * update & insert the bind info 
 */
int hbla_sql_execBind(const char *sql, void *pData, int len);
#endif
