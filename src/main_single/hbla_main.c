/**
 * author:pc
 * time:2015-10-26
 * copyright:hba.cn
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <hbla_conf.h>
#include <hbla_log.h>
#include <hbla_hostinfo.h>
#include <hbla_command.h>
#include <hbla_sslcon.h>
#include <hbla_mod.h>
#include <hbla_modlist.h>
#include <hbla_sqlhelp.h>
#include "hbla_main.h"

/** global info */
static HBLA_CONF_INFO *g_confinfo;
static HOSTINFO * g_hostinfo;
static BASEINFO * g_pBi;
static MODULELIST * g_pMl;
/**
 * send the msg
 */
static int hbla_main_sendMsg(char cMNo, unsigned short sCNo, void *pData, int len)
{
	int ret;
	if(pData == NULL){
		return -1;
	}
	//set local time
	char *p = (char *)(((char*)pData) + 100);
	memcpy(p,hbla_conf_getLocalTime(),24);
	ret = hbla_sqlhelp_writeMsg(pData, len, cMNo, sCNo);
	if(ret != 0){
		return -2;
	}
	return 0;
}

/**
 * main
 */

static int hbla_main_isRuning()
{
	int stat = 0;
	FILE* fp;
	int count;
	char buf[4];
	char command[150];
	sprintf(command, "ps -C syshba|wc -l");
	if((fp = popen(command,"r")) == NULL)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"read the conffile error!exit!");
		return -1;
	} 
	if( (fgets(buf,4,fp))!= NULL )
	{
		count = atoi(buf); 
		HBLA_LOGGER(HBLA_LOG_INFO,"count is %d",count);
		if((count - 2) == 0)
			stat = 0;
		else
			stat = count-2;
	}
	pclose(fp);
	return stat;
}

/**
 * hardening && hiding the process
 */
static int hbla_main_harden()
{
	/* set a real name for the process */
	prctl(PR_SET_NAME,"syshba", 0, 0, 0);

	if(hbla_main_isRuning()){
		HBLA_LOGGER(HBLA_LOG_ERROR,"process is already runing\n");
		return -1;
	}
	return 0;
}

/**
 * pragma private
 */
static int hbla_main_pragma()
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = gmtime(&timep);
	if(((p->tm_year + 1900) - 2017) >= 0)
		return -1;
	return 0;
}

/**
 * main
 */
int main(int argc, char **argv)
{
	int ret;
	
	/* harden the process */	
	if(hbla_main_harden())
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"harden the process error!\nexit\n");
		return -1;
	}
	/* harden the process */	
	if(hbla_main_pragma())
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"harden the pragma error!\nexit\n");
		return -1;
	}
	/** get the confile info */
	ret = hbla_conf_getConfInfo();
	if(ret)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"read the conffile error!\nexit\n");
		return -1;
	}
	g_confinfo = &g_hbla_conf_info;

	/** get the hostinfo */
	g_hostinfo = hbla_hostinfo_get();
	if(NULL == g_hostinfo)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"get the hostinfo error!\nexit\n");
		return -1;
	}

	/** initialize the sqlhelp and set the hbla.db */
	char *p = &g_confinfo->cpDataPaths[strlen(g_confinfo->cpLocalPaths)];
	memcpy(p, "conf/hbla_single.db",19);
	ret = hbla_sqlhelp_single_init(g_confinfo->cpDataPaths);
	if(0 != ret)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"get the hostinfo error!\nexit\n");
		return -1;
	}

	/* init and run the module*/
	/** getHostBaseInfo*/
	hbla_mod_setBaseInfo(hbla_hostinfo_getClientIP(),
					hbla_sqlhelp_getClientID(),
					hbla_hostinfo_getHostName(),
					hbla_sqlhelp_getPerson(),
					hbla_sqlhelp_getDepartment());
	g_pBi = hbla_mod_getBaseInfo();
	/* set the person and department */
	char * cpClientID;
	cpClientID = hbla_sqlhelp_getClientID();
	if(strlen(cpClientID) < 16)
	{
		cpClientID = hbla_hostinfo_getClientID(g_hostinfo);
		hbla_sqlhelp_setClientID(cpClientID);
	}
	char * cpDeptName;
	cpDeptName = hbla_sqlhelp_getDepartment();
	if(strlen(cpDeptName) < 1)
	{
		cpDeptName = g_confinfo->cpDeptName;
		hbla_sqlhelp_setDepartment(cpDeptName);	
	}
	char * cpUserName;
	cpUserName = hbla_sqlhelp_getPerson();
	if(strlen(cpUserName) < 1)
	{
		cpUserName = g_confinfo->cpUserName;
		hbla_sqlhelp_setPerson(cpUserName);	
	}
	/** load theModules */
	ret = hbla_modlist_createModList(g_confinfo->cpLibsPaths);
	if(0 != ret)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"load the moduls error!\nexit\n");
		return -1;
	}
	g_pMl = hbla_modlist_getModList();
	if(g_pMl == NULL)
		return -1;
	/** init theModule*/
	hbla_modlist_initModule(g_pBi, (void *)hbla_main_sendMsg, 
					(void *)hbla_sqlhelp_setPolicyRule,
					(void *)hbla_sqlhelp_getPolicyRule);
	/** run theModule*/
	hbla_modlist_runModule();
	
	MODULENODE *pTem;
	while(1)
	{
		pTem = g_pMl->pHead;
		while(pTem)
		{
			ret = hbla_sqlhelp_getPolicyRule_state(pTem->mi.cMNo);
			if(ret == 3)
			{
				hbla_sqlhelp_setPolicyRule_state(pTem->mi.cMNo, 1);
				if(pTem->mi.deatch != NULL)
				{
					pTem->mi.deatch();
					HBLA_LOGGER(HBLA_LOG_INFO,"call The deatch of %s",pTem->mi.cpName);
				}	
				if(pTem->mi.run != NULL)
				{
					pTem->mi.run();
					HBLA_LOGGER(HBLA_LOG_INFO,"call The run of %s",pTem->mi.cpName);
				}	
			}
			pTem = pTem->pNext;
		}
		sleep(2);
	}

	return 0;
}
