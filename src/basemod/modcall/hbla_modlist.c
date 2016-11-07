/**
 * author:pc
 * time:2015-11-07 13:48
 * copyright:hba.cn
 * desc:create a list for modules
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <hbla_log.h>
#include "hbla_mod.h"
#include "hbla_modcall.h"
#include "hbla_modlist.h"

static char modPath[256];
static MODULELIST ml;


/**
 * create a ModuleNode;
 */
static MODULENODE * hbla_modlist_createNode()
{
	MODULENODE *pMn = (MODULENODE *)malloc(sizeof(MODULENODE) * sizeof(char));
	if(pMn == NULL)
		HBLA_LOGGER(HBLA_LOG_ERROR,"con't create a MODULENODE");
	memset(pMn, 0, sizeof(MODULENODE));
	return pMn;
}

/**
 * free a ModuleNode
 */
static void hbla_modlist_freeNode(MODULENODE * pMn)
{
	if(pMn != NULL)
	{
		free(pMn);
		pMn = NULL;
	}
}

/**
 * insert a node to the ModuleNode;
 */
static int hbla_modlist_insert(MODULENODE * pMn)
{
	if(pMn == NULL)
		return -1;
	if(ml.pFoot == NULL || ml.pHead == NULL)
	{
		ml.pHead = pMn;
		ml.pFoot = pMn;
		ml.count = 1;
		return 0;
	}

	ml.pFoot->pNext= pMn;
	ml.pFoot = pMn;
	ml.count++;
	pMn->pNext = NULL;
	return 0;
}

/**
 * get a ModuleNode from modlist
 */
static MODULENODE * hbla_modlist_getNodeByMNo(char cMNo)
{
	MODULENODE * pMn_t;
	pMn_t = ml.pHead;
	while(pMn_t != NULL)
	{
		if(pMn_t->mi.cMNo == cMNo)
			return pMn_t;
		pMn_t = pMn_t->pNext;
	}
	return NULL;
}

/**
 * delete a ModuleNode from the ModuleList
 */
static int hbla_modlist_deleteNodeByNo(char cMNo)
{
	MODULENODE * pMn_t = NULL;
	MODULENODE * pMn_t1 = NULL;
	pMn_t = ml.pHead;
	pMn_t1 = pMn_t;
	while(pMn_t != NULL)
	{
		if(pMn_t->mi.cMNo == cMNo)
		{
			pMn_t1 = pMn_t->pNext;
			ml.count--;
			hbla_modlist_freeNode(pMn_t);
			pMn_t = pMn_t1;
			continue;
		}
		pMn_t = pMn_t->pNext;
	}
	return 0;
}

/**
 * judge the file is a soFile
 */
static int hbla_modlist_isSoFile(const char *cpPath)
{
	int len;

	if(cpPath == NULL)
		return -1;
	
	len = strlen(cpPath);
	if(len < 4)
		return -1;
	if(((cpPath[len-1] == 'o') || (cpPath[len-1] == 'O'))
		&& ((cpPath[len-2] == 's') || (cpPath[len-2]) == 'S')
		&& (cpPath[len -3] == '.'))
	{
		return 1;
	}

	return 0;
}

/**
 * get the modules
 */
int hbla_modlist_createModList(const char * cpPath)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	int ret;
	MODULENODE * moTem;

	if(cpPath == NULL)
		return -1;
	strncpy(modPath,cpPath,256);
	if((dp = opendir(modPath)) == NULL){
		HBLA_LOGGER(HBLA_LOG_ERROR,"the dynmic link libray path is error!");
		return -1;
	}
	//chdir(modePath);
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name, &statbuf);
		//if(S_ISDIR(statbuf.st_mode))
		//	continue;
		ret = hbla_modlist_isSoFile(entry->d_name);
		if(ret == 1)
		{
			moTem = hbla_modlist_createNode();
			sprintf(moTem->cpPath,"%s/%s",modPath,entry->d_name);
			ret = hbla_modcall_loadDl(moTem->cpPath, &moTem->handle);
			if(ret != 0)
			{
				HBLA_LOGGER(HBLA_LOG_ERROR,"load dynmic link libary %s error!",moTem->cpPath);
				hbla_modlist_freeNode(moTem);
				continue;
			}
			ret = hbla_modcall_loadFun(moTem->handle, (void *)&((moTem->mi.init)),"init");
			if(ret != 0)
			{
				HBLA_LOGGER(HBLA_LOG_ERROR,"load dynmic link libary %s funs init error!",moTem->cpPath);
				hbla_modlist_freeNode(moTem);
				continue;
			}
			hbla_modlist_insert(moTem);
		}
	}
	closedir(dp);
	return 0;
}

/**
 * get the list of modules
 */
MODULELIST * hbla_modlist_getModList()
{
	return &ml;
}

/**
 * call the init & attach of modules
 */
int hbla_modlist_initModule(const BASEINFO *pBi, const void *sendMsg,
				const void *setRule, const void *getRule)
{
	if((pBi == NULL) || (sendMsg == NULL)
		|| (setRule == NULL) || (getRule == NULL))
	{
		return -1;
	}

	MODULENODE * pTem;
	int ret;
	pTem = ml.pHead;
	while(pTem)
	{
		if(pTem->mi.init != NULL)
		{
			pTem->mi.init(pBi, sendMsg, setRule, getRule, (void *)&(pTem->mi));
			HBLA_LOGGER(HBLA_LOG_INFO,"call The init of %s",pTem->cpPath);
		}
		if(pTem->mi.attach != NULL)
		{
			pTem->mi.attach();
			HBLA_LOGGER(HBLA_LOG_INFO,"call The attach of %s",pTem->mi.cpName);
		}
		pTem = pTem->pNext;
	}

	return 0;
}

/**
 * call the run of modules
 */
int hbla_modlist_runModule()
{
	MODULENODE * pTem;
	pTem = ml.pHead;
	while(pTem)
	{
		if(pTem->mi.run != NULL)
		{
			pTem->mi.run();
			HBLA_LOGGER(HBLA_LOG_INFO,"call The run of %s",pTem->mi.cpName);
		}
		pTem = pTem->pNext;
	}
}

/**
 * call the deatch of modules
 */
int hbla_modlist_deatchModule()
{
	MODULENODE * pTem;
	pTem = ml.pHead;
	while(pTem)
	{
		if(pTem->mi.deatch != NULL)
		{
			pTem->mi.deatch();
			HBLA_LOGGER(HBLA_LOG_INFO,"call The deatch of %s",pTem->mi.cpName);
		}
		pTem = pTem->pNext;
	}
}

/**
 * dispose the rule of policy to modules
 */
//TODO
int hbla_modlist_disposePolicy(void *pData, unsigned short sCNo,unsigned char cMNo)
{
	if(pData == NULL)
		return -1;

	MODULENODE * pTem;
	pTem = hbla_modlist_getNodeByMNo(cMNo);
	if(pTem == NULL)
		return -2;
	if(pTem->mi.setPolicy != NULL)
	{
		pTem->mi.setPolicy(sCNo, pData);
		HBLA_LOGGER(HBLA_LOG_INFO,"call The setPolicy of %s",pTem->mi.cpName);
	}

	return 0;
}
