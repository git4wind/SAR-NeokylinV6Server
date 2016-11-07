/**
 * author:pc
 * time:2015-10-29 10:48
 * desc:
 * 	authorize to center
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hbla_hostinfo.h>
#include <hbla_sqlhelp.h>
#include <hbla_conf.h>
#include "hbla_auth.h"

static AUTHFRAME authframe;

/**
 * get the authFrame of the client
 */
AUTHFRAME* hbla_auth_getAuth(HOSTINFO *hi,HBLA_CONF_INFO *ci)
{
	if(NULL == hi)
		return NULL;

	authframe.iSize = 436;
	authframe.iBuildNO = hbla_sqlhelp_getBuildNo();
	strncpy(authframe.cpPolicyDate, hbla_sqlhelp_getPolicyTime(), 24);
	char * cpClientID;
	cpClientID = hbla_sqlhelp_getClientID();
	if(strlen(cpClientID) < 16)
	{
		cpClientID = hbla_hostinfo_getClientID(hi);
		hbla_sqlhelp_setClientID(cpClientID);
	}
	strncpy(authframe.cpClientID, cpClientID, 32);
	strncpy(authframe.cpCliInfoMD5, hbla_hostinfo_getCliInfoMD5(hi), 32);
	authframe.iIsSign = hbla_sqlhelp_getIsSign();
	char * cpDeptName;
	cpDeptName = hbla_sqlhelp_getDepartment();
	if(strlen(cpDeptName) < 1)
	{
		cpDeptName = ci->cpDeptName;
		hbla_sqlhelp_setDepartment(cpDeptName);	
	}
	strncpy(authframe.cpDepartmentName, cpDeptName,128);
	char * cpUserName;
	cpUserName = hbla_sqlhelp_getPerson();
	if(strlen(cpUserName) < 1)
	{
		cpUserName = ci->cpUserName;
		hbla_sqlhelp_setPerson(cpUserName);	
	}
	strncpy(authframe.cpUserName, cpUserName,128);
	//TODO add the dispose for deptName
	//strncpy(authframe.cpDepartmentName, hbla_sqlhelp_getDepartment(), 128);
	//strncpy(authframe.cpUserName, hbla_sqlhelp_getPerson(), 128);
	char * cpIp = hbla_hostinfo_getClientIP();
	if(cpIp != NULL)
		strncpy(authframe.cpHostIp, cpIp, 16);
	strncpy(authframe.cpHostName, hbla_hostinfo_getHostName(), 64);

	return &authframe;
}
