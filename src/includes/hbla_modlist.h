/**
 * author:pc
 * time:2015-11-07 13:55
 * copyright: hba.cn
 * desc: crate a list for modules
 */
#ifndef _HBLA_MODLIST_H_
#define _HBLA_MODLIST_H_

typedef struct _tagModuleNode{
	char cpPath[256];
	void *handle;
	MODULEINFO mi;
	struct _tagModuleNode * pNext;
}MODULENODE;

typedef struct _tagModuleList{
	MODULENODE * pHead;
	MODULENODE * pFoot;
	unsigned int count;
}MODULELIST;

/**
 * get the modules
 */
extern int hbla_modlist_createModList(const char * cpPath);

/**
 * get the list of modules
 */
extern MODULELIST * hbla_modlist_getModList();

/**
 * call the init & attach of modules
 */
extern int hbla_modlist_initModule(const BASEINFO *pBi, const void *sendMsg,
				const void *setRule, const void *getRule);

/**
 * call the run of modules
 */
extern int hbla_modlist_runModule();

/**
 * call the deatch of modules
 */
extern int hbla_modlist_deatchModule();

/**
 * dispose the rule of policy to modules
 */
extern int hbla_modlist_disposePolicy(void *pData, unsigned short sCNo, unsigned char cMNo);
#endif
