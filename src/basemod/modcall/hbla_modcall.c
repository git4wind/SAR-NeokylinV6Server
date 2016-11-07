/*
 * author:pc
 * time:2015-11-07
 * desc:load the dynamic Link Libary for the modules
 */
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <hbla_log.h>

/**
 * load a dynamic link libary
 */
int hbla_modcall_loadDl(char *cpFile,void **handle)
{
	if(cpFile == NULL)
		return -1;
	*handle = dlopen(cpFile,RTLD_LAZY);
	if(*handle == NULL)
		return -2;
	return 0;
}

/**
 * close a handle of dynamic link libary
 */
int hbla_modcall_closeDl(void **handle)
{
	if(*handle == NULL)
		return -1;

	dlclose(*handle);
	*handle == NULL;

	return 0;
}

/**
 * load a fun
 */
int hbla_modcall_loadFun(void *handle, void **func
				,const char *cpFuncName)
{
	if(handle == NULL)
		return -1;
	*func = (void *)dlsym(handle, cpFuncName);
	if(*func == NULL)
		return -2;
	return 0;
}
