/*
 * author:pc
 * time:2015-11-07 13:26
 * right:hba.cn
 * desc: the module for linux hostAudit
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hbla_log.h>
#include "hbla_mod.h"

static BASEINFO bi;

/**
 * set the Baseinfo for modules
 */
int hbla_mod_setBaseInfo(const char *hostIp,
				const char *hostId,
				const char *hostName,
				const char *userName,
				const char *depName)
{
	if((hostIp == NULL) || (hostId == NULL)
		|| (hostName == NULL) || (userName == NULL)
	   	|| (depName == NULL))
		return -1;
	bi.cpHostIp = hostIp;
	bi.cpHostId = hostId;
	bi.cpHostName = hostName;
	bi.cpUserName = userName;
	bi.cpDepName = depName;
	return 0;
}

/**
 * get the BaseInfo
 */
BASEINFO * hbla_mod_getBaseInfo()
{
	return &bi;
}


