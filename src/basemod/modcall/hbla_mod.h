/**
 * author:pc
 * time:2015-11-06 15:04
 * right:copyright hba.cn
 * desc:the module for linux hostAudit
 */
#ifndef _HBLA_MOD_H_
#define _HBLA_MOD_H_

/**
 * the callbake functions
 */
typedef int(*sendMsg)(char cMNo, unsigned short sCNo,void *pData, int len);
typedef int(*setRule)(char cMNo, void *pData, int len, int state);
typedef int(*getRule)(char cMNo, void *pData, int *len, int *state);

/**
 * hostBaseInfo
 */
typedef struct _tagBaseInfo{
	const char *cpHostIp;
	const char *cpHostId;
	const char *cpHostName;
	const char *cpUserName;
	const char *cpDepName;
}BASEINFO;

/**
 * One module info
 */
typedef struct _tagModuleInfo{
	char cpName[64];
	unsigned int cMNo;
	int (*init)(const BASEINFO * pBi, const void *sendMsg,const  void *setRule, const void *getRule, const void *pMi);
	int (*setPolicy)(unsigned short sCNo,void *pData);
	int (*attach)();
	int (*run)();
	int (*deatch)();
}MODULEINFO;


/**
 * set the Baseinfo for modules
 */
int hbla_mod_setBaseInfo(const char *hostIp,
				const char *hostId,
				const char *hostName,
				const char *userName,
				const char *depName);


/**
 * get the BaseInfo
 */
BASEINFO * hbla_mod_getBaseInfo();

#endif
