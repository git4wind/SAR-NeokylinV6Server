/**
 * author:pc
 * copyRight:hba.cn
 * time:2015-09-21
 * desc:init ssl connections,use openssl in static libriary.
 */
#ifndef _HALA_SSLCON_H_
#define _HALA_SSLCON_H_

/**
 * all tranlate by the baseFrame
 */
typedef struct tag_baseFrame{
	unsigned short sHCode;	//define it to 0x7063
	unsigned char cVer;		//the version is 0x01
	unsigned char cPNo;		//the product is Overall audit,define it to 0x01
	unsigned int iFLength;	//the size of baseFrame
	unsigned char cFNo;		//this is Linux Auidt Client,define it to 0x04
	unsigned char cMNo;		//the Module No
	unsigned short sComNo;	//the Command No
	unsigned char cFBf;		//the extInfo
}BASEFRAME;

#define HBLA_SSLCON_BASEFRAME_LENGTH 12
#define HBLA_SSLCON_VER 0x01
#define HBLA_SSLCON_HCODE 0x7063

extern void hbla_sslcon_init(unsigned char cPNo,unsigned char cFNo);
/**
 * connect to center
 */
extern int hbla_sslcon_connect(const char * cpClientPriKey, 
				const char * cpClientCert,
				const char * cpServerPubKey,const char * cpCenterAddr,
			   	int iCenterPort);
/**
 * judge the sock is connected
 */
extern int hbla_sslcon_isConnected();

/**
 * send the baseframe&framebuf to center
 * return:
 * 		-1 	parms errors
 * 		-2 	send sock error
 * 		-3 	malloc error
 * 		0 	success
 */
extern int hbla_sslcon_send(unsigned char cMNo, unsigned short sComNo,
				const char *buf, unsigned int size);
/**
 * recv a basedrame from center
 */
extern BASEFRAME* hbla_sslcon_recv(int *retBak);
/**
 * byte alignmen in one byte
 */
//#pragma pack(1)

#endif

