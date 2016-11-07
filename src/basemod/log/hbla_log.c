/**
 * author:pc
 * time:2015-09-17
 * copyright:hba.cn
 * version:1.0.0
 * desc:
 * 		1.This is used for hbaudit(In linux) to printf the logger
 * 		export the function hb_ha_logger.
 * 		2.LEVEL is OFF,DEBUG,INFO,WARN,ERROR,FATAL,ALL
 * 		3.DEBUG < INFO < WARN < ERROR < FATAL
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "hbla_log.h"

static char hbla_log_levstr[][6]={"OFF","DEBUG","INFO","WARN","ERROR","FATAL","ALL"};

/**
 * author:pc
 * time:2015-09-17 16:00
 * desc:get localtime
 */
static char * hbla_log_gettime()
{
	time_t t;
	char *cBuf;
	time(&t);
	cBuf = ctime(&t);
	cBuf[strlen(cBuf) - 1] = '\0';
	return cBuf;
}

/**
 * author:pc
 * time:2015-09-17 16:00
 * desc:translate the level from integer of enum to char[]
 */
static char * hbla_log_getlevel(int level)
{
	if(level < HBLA_LOG_OFF || level > HBLA_LOG_ALL)
		return "NOTING";
	
	return hbla_log_levstr[level];
}

/**
 * author:pc
 * time:2015-09-17 15:43
 * parm:
 * 		level is hbla_log_level,suggests use HB_LA_LOG_INFO,
 * 		HB_LA_LOGGER_ERROR
 * 		loginfo is the message for loginfo
 */
void hbla_log_logger(int level, char *filename,
					int line,const  char *fun,char *fmt,...)
{
	va_list args;
	char buf[512] = {0};
	
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
		
	printf("{HB_LinuxAudit}-%s[%s]-%s:%d:%s\t%s\n",
					hbla_log_gettime(),
					hbla_log_getlevel(level),
					filename,
					line,
					fun,
					buf);
}
