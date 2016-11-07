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
#ifndef _HBLA_LOG_H_
#define _HBLA_LOG_H_

#define HBLA_LOGGER(level,format,args...) hbla_log_logger(level,__FILE__,__LINE__,__func__,format,##args)

void hbla_log_logger(int level, char *filename,
					int line, const char *fun, char *fmt,...);

enum hbla_log_level{
	HBLA_LOG_OFF = 0,
	HBLA_LOG_DEBUG = 1,
	HBLA_LOG_INFO = 2,
	HBLA_LOG_WARN = 3,
	HBLA_LOG_ERROR = 4,
	HBLA_LOG_FATAL = 5,
	HBLA_LOG_ALL = 6
};

#endif
