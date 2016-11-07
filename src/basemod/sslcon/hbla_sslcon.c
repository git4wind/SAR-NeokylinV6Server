/**
 * author:pc
 * copyRight:hba.cn
 * time:2015-10-10
 * desc:init ssl connections,use openssl in static libriary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "hbla_ssl.h"
#include "hbla_sslcon.h"

/*
 * static info for connect
 */
static const char * hbla_sslcon_cpClientKey;
static const char * hbla_sslcon_cpClientCert;
static const char * hbla_sslcon_cpServerPubKey;
static const char * hbla_sslcon_cpCenterAddr;
static int hbla_sslcon_iCenterPort;
static char hbla_sslcon_cPNo;
static char hbla_sslcon_cFNo;
/*
 * static pthread_t
 */
static pthread_t hbla_sslcon_pHandlerRun;
static pthread_mutex_t sendLock;
static pthread_mutex_t recvLock;
static pthread_mutex_t linkLock;
/**
 * keeplive times
 */
static time_t recv_time;

/**
 * a pthread to ensure the sock is ok
 */
static void * hbla_sslcon_run(void * noting)
{
	int ret;
	static time_t now_time;
run:
	while(!hbla_ssl_isConnected())
	{
		recv_time = time(NULL);
		pthread_mutex_lock(&linkLock);
		ret = hbla_ssl_connect(hbla_sslcon_cpClientKey,
						hbla_sslcon_cpClientCert,
						hbla_sslcon_cpServerPubKey,
						hbla_sslcon_cpCenterAddr,
					   	hbla_sslcon_iCenterPort);
		pthread_mutex_unlock(&linkLock);
		if(ret == 0)
			break;
		sleep(1);	
	}
	//to make sure the connection keepAlives;
	
	while(hbla_ssl_isConnected())
	{
		now_time = time(NULL);
		if(15 <= (now_time - recv_time))
		{
			pthread_mutex_lock(&linkLock);
			hbla_ssl_disConnect();
			pthread_mutex_unlock(&linkLock);
			break;
		}
		static char buf[3] = "ok";
		hbla_sslcon_send(0x70,0x7700,buf,3);
		sleep(5);
	}
	
	goto run;

	return NULL;
}

/**
 * initilize the cPNo & cFNo
 */
void hbla_sslcon_init(unsigned char cPNo,unsigned char cFNo)
{
	hbla_sslcon_cPNo = cPNo;
	hbla_sslcon_cFNo = cFNo;
	
	//set the mutex for translation tunle
	pthread_mutex_init(&sendLock, NULL);
	pthread_mutex_init(&recvLock, NULL);
	pthread_mutex_init(&linkLock, NULL);

}

/**
 * judge the sock is connected
 */
int hbla_sslcon_isConnected()
{
	static int isLink;
	pthread_mutex_lock(&linkLock);
	isLink = hbla_ssl_isConnected();
	pthread_mutex_unlock(&linkLock);
	return isLink;
}

/**
 * connect to center
 */
int hbla_sslcon_connect(const char * cpClientPriKey,const char * cpClientCert,const char * cpServerPubKey,
				const char * cpCenterAddr, int iCenterPort)
{
	int ret;
	hbla_sslcon_cpClientKey = cpClientPriKey;
	hbla_sslcon_cpClientCert = cpClientCert;
	hbla_sslcon_cpServerPubKey = cpServerPubKey;
	hbla_sslcon_cpCenterAddr = cpCenterAddr;
	hbla_sslcon_iCenterPort = iCenterPort;

	ret = pthread_create(&hbla_sslcon_pHandlerRun, NULL, 
					&hbla_sslcon_run, NULL);
	if(ret != 0)
		return -1;
	return 0;
}

/**
 * send the baseframe&framebuf to center
 * return:
 * 		-1 	parms errors
 * 		-2 	send sock error
 * 		-3 	malloc error
 * 		0 	success
 */
int hbla_sslcon_send(unsigned char cMNo, unsigned short sComNo,
				const char *buf, unsigned int size)
{
	unsigned int allLen;
	int ret;
	BASEFRAME * pBaseFrame;
	if(buf == NULL || size < 1)
		return -1;

	allLen = size + HBLA_SSLCON_BASEFRAME_LENGTH;
	pBaseFrame = (BASEFRAME *)malloc(allLen * sizeof(char));
	memset(pBaseFrame, 0, sizeof(char)*allLen);
	if(NULL == pBaseFrame)
		return -3;
	pBaseFrame->sHCode = HBLA_SSLCON_HCODE;
	pBaseFrame->cVer = HBLA_SSLCON_VER;
	pBaseFrame->cPNo = hbla_sslcon_cPNo;
	pBaseFrame->iFLength = htonl(allLen);
	pBaseFrame->cFNo = hbla_sslcon_cFNo;
	pBaseFrame->cMNo = cMNo;
	pBaseFrame->sComNo = sComNo;
	memcpy(&(pBaseFrame->cFBf), buf, size);

	pthread_mutex_lock(&sendLock);
	ret = hbla_ssl_write((char *)pBaseFrame, allLen);
	pthread_mutex_unlock(&sendLock);
	free(pBaseFrame);

	if(ret)
		return -2;

	return 0;	
}

/**
 * recv the baseframe from center
 * return retBak -1 error
 * 		0 success
 */
BASEFRAME* hbla_sslcon_recv(int *retBak)
{
	char bufTem[4] = {0};
	unsigned int iFLength = 0;
	static BASEFRAME *pBaseFrame;
	int ret;
	int readSocks;
	static int sock;

	pthread_mutex_lock(&recvLock);
	
	while(1){
		ret = hbla_ssl_read(bufTem,1);
		if(ret)
			goto err;
		if(bufTem[0] != 0x63)
			continue;
		ret = hbla_ssl_read(bufTem,1);
		if(ret)
			goto err;
		if(bufTem[0] == 0x70)
			break;
	}
	ret = hbla_ssl_read(bufTem,2);
	if(ret)
		goto err;
	if(bufTem[0] != HBLA_SSLCON_VER)
		goto err1;
	ret = hbla_ssl_read(bufTem,4);
	if(ret)
		goto err;
	iFLength = *(unsigned int *)bufTem;
	if(iFLength < 12)
		goto err1;
	if(pBaseFrame != NULL){
		free(pBaseFrame);
		pBaseFrame = NULL;
	}
	pBaseFrame = (BASEFRAME *)malloc(iFLength * sizeof(char));
	if(pBaseFrame == NULL)
		goto err1;
	memset(pBaseFrame, 0, iFLength * sizeof(char));
	ret = hbla_ssl_read((char *)&(pBaseFrame->cFNo),iFLength - 8);
	if(ret)
		goto err;
	pBaseFrame->sHCode = 0x7063;
	pBaseFrame->cVer = 0x01;
	pBaseFrame->cPNo = 0x01;
	pBaseFrame->iFLength = iFLength;
	pthread_mutex_unlock(&recvLock);
	
	recv_time = time(NULL);
	retBak = 0;
	return pBaseFrame;

err:
	*retBak = -1;
	pthread_mutex_unlock(&recvLock);
	return NULL;
err1:
	*retBak = -1;
	pthread_mutex_unlock(&recvLock);
	return NULL;
}
