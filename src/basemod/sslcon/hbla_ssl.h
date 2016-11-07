/**
 * author:pc
 * copyRight:hba.cn
 * time:2015-09-21
 * desc:init ssl connections,use openssl in static libriary.
 */
#ifndef _HALA_SSL_H_
#define _HALA_SSL_H_

/**
 * connect to server
 * input: 
 * 		ccpIP is the serverIP/name, iPort is the server Port
 * return:
 * 		0 : success!
 * 		-1: ccpIP error
 * 		-2: bio error
 * 		-3: ssl error
 * 		-4: conenction error
 * 		-5: handshake error
 * 		-6: certificate error
 * 		 1: args error
 * 		 2: init error
 */

extern int hbla_ssl_connect(const char * cpClientPriKey,const char * cpClientCert, const char * cpServerPubKey,
				const char * cpCenterAddr,int iCenterPort);

/**
 * read from ssl
 * input:
 * 		buf:malloc by upside
 * 		size:read lengths
 */
extern int hbla_ssl_read(char *buf,unsigned int len);

/**
 * write to ssl
 * input:
 * 		buf:malloc by upside
 * 		size:read lengths
 */
extern int hbla_ssl_write(char *buf,unsigned int size);

/**
 * judge the sock is ok
 * return:
 * 		0 is unconnected
 * 		1 is connected
 */
extern int hbla_ssl_isConnected();
/**
 * disConnect the link
 */
void hbla_ssl_disConnect();
/**
 * get the sock
 */
int hbla_ssl_getSock();
#endif
