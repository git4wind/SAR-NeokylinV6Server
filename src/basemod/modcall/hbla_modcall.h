/**
 * author:pc
 * time:2015-11-06 14:48
 * desc:load the dynamic Link Libary for the modules
 */
#ifndef _HBLA_MODCALL_H_
#define _HBLA_MODCALL_H_

/**
 * load a dynamic link libary
 */
int hbla_modcall_loadDl(char *cpFile,void **handle);

/**
 * close a handle of dynamic link libary
 */
int hbla_modcall_closeDl(void **handle);

/**
 * load a fun
 */
int hbla_modcall_loadFun(void *handle, void **func
				,const char *cpFuncName);

#endif
