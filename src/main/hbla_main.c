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
#include "hbla_auth.h"

/** global info */
static HBLA_CONF_INFO *g_confinfo;
static HOSTINFO * g_hostinfo;
static BASEINFO * g_pBi;

static int g_isAuth;
#define DBPASS "pclvmm"


/**
 * send the msg
 */
static int hbla_main_sendMsg(char cMNo, unsigned short sCNo, void *pData, int len)
{
	int ret;
	if(pData == NULL)
		return -1;
	if(g_isAuth)
	{
		ret = hbla_sslcon_send(cMNo, sCNo, (char *)pData, len);
		if(ret == 0)
			return 0;
	}
	ret = hbla_sqlhelp_writeMsg(pData, len, cMNo, sCNo);
	if(ret != 0)
	{
		return -2;
	}
	return 0;
}

/**
 * send the local info pthread
 */
static void hbla_main_sendLocalMsg()
{
	int ret;
	int len;
	int cMNo;
	int sCNo;
	static char pData[4096];

	while(1)
	{
		if(g_isAuth)
		{
			ret = hbla_sqlhelp_readMsg(pData,&len, &cMNo, &sCNo);
			if(ret != 0)
			{
				usleep(100);
				continue;
			}
			ret = hbla_sslcon_send(cMNo, sCNo, (char *)pData, len);
			if(ret == 0)
				hbla_sqlhelp_delMsgByState();
			//hbla_sqlhelp_setMsgStateByState(1,0); 2016 3 7 with y
		}
			usleep(100);
	}
}

/**
 * dispose the frame
 */
static int hbla_main_dispose(BASEFRAME * pBf)
{
	int ret;
	switch(pBf->cFNo)
	{
	case FNO_CENTER:
		if(pBf->cMNo == AUTHMODULE)
		{
			if(pBf->sComNo == AUTHSIGN)
			{
			//TODO add the dispose for sign
			}else if(pBf->sComNo == AUTHGETHOSTINFO)
			{
				ret = hbla_sslcon_send(AUTHMODULE, AUTHSETHOSTINFO, (char *)g_hostinfo, sizeof(HOSTINFO));
			}
		}else
		{
			hbla_modlist_disposePolicy((void *)&pBf->cFBf, pBf->sComNo, pBf->cMNo);
		}
		break;
	default:
		break;
	}
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
	//TODO
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
	ret = hbla_sqlhelp_init(g_confinfo->cpDataPaths, DBPASS);
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
	/** load theModules */
	ret = hbla_modlist_createModList(g_confinfo->cpLibsPaths);
	if(0 != ret)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"load the moduls error!\nexit\n");
		return -1;
	}
	/** init theModule*/
	hbla_modlist_initModule(g_pBi, (void *)hbla_main_sendMsg, 
					(void *)hbla_sqlhelp_setPolicyRule,
					(void *)hbla_sqlhelp_getPolicyRule);
	/** run theModule*/
	hbla_modlist_runModule();
	
	/* run the sendLocalMsg */
	pthread_t pid;
	pthread_attr_t attr;
	memset(&pid, 0, sizeof(pthread_t));
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&pid, &attr, (void *)hbla_main_sendLocalMsg, NULL);
	if(0 != ret)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR,"Pthread_create() of sendLocalMsg failed\nexit\n");
		return -1;
	}
	pthread_attr_destroy(&attr);
	/* initialize the connected */
	hbla_sslcon_init(PNO,FNO_SC_LINUX);
	/* crate a pthread for connecting the center */
	ret = hbla_sslcon_connect(g_confinfo->cpCKeyPaths,
					g_confinfo->cpCCertPaths, 
					g_confinfo->cpSKeyPaths,
					g_confinfo->cpCenterIp,
					g_confinfo->iCenterPort);
	while(1)
	{
		if(hbla_sslcon_isConnected())
		{
			BASEFRAME * pBf;
			if(0 == g_isAuth)
			{
				/* authentication */
				AUTHFRAME *af = hbla_auth_getAuth(g_hostinfo,g_confinfo);		
				ret = hbla_sslcon_send(AUTHMODULE, AUTHINFO, (char *)af, sizeof(AUTHFRAME));
				if(ret != 0)
					continue;
				pBf = hbla_sslcon_recv(&ret);

				if(pBf == NULL)
					continue;
				if(pBf->cFNo == FNO_CENTER && 
					pBf->cMNo == AUTHMODULE &&
					pBf->sComNo == AUTHRETURN)
				{
					AUTHRESULT * ar = (AUTHRESULT *)&(pBf->cFBf);
					if(ar->iSize == 12)
					{
						if(ar->iResult != 1)
							return 0;
						//TODO dispose the ar->iStatus;
						g_isAuth = 1;
						continue;
					}
				}
				return 0;
			}
			pBf = hbla_sslcon_recv(&ret);
			if(pBf == NULL)
				continue;
			printf("sHCode = %d,cVer = %d,cPNo = %d, iFlength = %d,cFNo = %d,cMNo = %d,sComNo = %d,cFBf = %s\n", pBf->sHCode,pBf->cVer, pBf->cPNo, pBf->iFLength, pBf->cFNo, pBf->cMNo, pBf->sComNo,&pBf->cFBf);
			COMMONRULEHEAD *pcom = (COMMONRULEHEAD*)&pBf->cFBf;
            if (pcom->nInfoCode == 3 /*||pcom->nInfoCode == 0*/ || pcom->nInfoCode == 1 || pcom->nInfoCode == 2){
				printf("nSize = %d, nSubCommand = %d, nAlarmLevel = %d, nModuleID = %d, nInfoCode = %d, nExtRuleSize = %d, ExtBuf = %s\n", pcom->nSize, pcom->nSubCommand, pcom->nAlarmLevel, pcom->nModuleID, pcom->nInfoCode, pcom->nExtRuleSize, pcom->ExtBuf);      
            }       
			hbla_main_dispose(pBf);
		}else{
			g_isAuth = 0;
		}
	}

	return 0;
}
