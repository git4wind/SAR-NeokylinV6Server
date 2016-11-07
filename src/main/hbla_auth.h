/**
 * author:pc
 * time:2015-10-29 10:48
 * desc:
 * 	authorize to center
 */
#ifndef _HBLA_AUTH_H_
#define _HBLA_AUTH_H_

typedef struct _tagAuthFrame{
	unsigned int iSize;
	char cpClientID[32];
	char cpCliInfoMD5[32];
	unsigned int iBuildNO;
	char cpPolicyDate[24];
	int iIsSign;
	char cpUserName[128];
	char cpDepartmentName[128];
	char cpHostIp[16];
	char cpHostName[64];
}AUTHFRAME;

typedef struct _tagAuthResult{
	unsigned int iSize;
	unsigned int iResult;
	unsigned int iStatus;
}AUTHRESULT;

/**
 * get the authFrame of the client
 */
AUTHFRAME* hbla_auth_getAuth(HOSTINFO *hi, HBLA_CONF_INFO *ci);

#endif
