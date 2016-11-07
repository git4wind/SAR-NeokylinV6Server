/**
 * author:pc
 * time:2015-10-08
 * copyright:hba.cn
 * version:1.0.0
 * file:hbla_conf.h
 * desc:
 * 	read the conf.ini & set the global struct of g_hbla_conf_info
 */
#ifndef _HALA_CONF_H_
#define _HALA_CONF_H_

typedef struct hbla_conf_info{
	char cpLocalPaths[256];	//the process's path in the local machine;
	char cpConfPaths[256];	//the conffile path;
	char cpDataPaths[256];	//the databasefile path;
	char cpLibsPaths[256];
	char cpSKeyPaths[256];	//the server publickey path;
	char cpCKeyPaths[256];	//the client privateKey path;
	char cpCCertPaths[256];	//the client privateKey path;
	char cpCenterIp[64];	//the hostip or hostname of the center;
	int iCenterPort;		//the listening port of center,default is 45451;
	char cpDeptName[256];
	char cpUserName[256];
	//char cpUnitName;
	//TODO
}HBLA_CONF_INFO;

extern HBLA_CONF_INFO g_hbla_conf_info;
extern int hbla_conf_getConfInfo();

extern char * hbla_conf_getLocalTime();
#endif
