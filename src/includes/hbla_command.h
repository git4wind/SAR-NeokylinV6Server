/**
 * author:pc
 * time:2015-11-02
 * desc:
 * 	define all command for linuxAuditClient
 */
#ifndef _HBLA_COMMAND_H_
#define _HBLA_COMMAND_H_

/* product number default is 0x01*/
#define PNO			0x01	

/* frame number, differentiate the frame where from */
#define	FNO_CENTER	  	0x01	/* center server */ 
#define FNO_WINSERVER		0x03	/* windows serverAudit client */
#define FNO_LINUXSERVER		0x05	/* linux serverAudit client */
#define FNO_SC_WIN              0x0A    /*windows service consolidation*/
#define FNO_SC_LINUX            0x0B    /*Linux service consolidation*/

/*
 * module for keepalive
 */
#define KEEPLIVE  0x70

/**
 *  module for authentication 
 */
#define AUTHMODULE		0x01

/* command for authentication */
#define AUTHINFO		0x0001	/* C--S */
#define AUTHRETURN		0x0002	/* S--C */
#define AUTHSIGN		0x0004	/* S--C */
#define AUTHGETHOSTINFO		0x0008	/* S--C */
#define AUTHSETHOSTINFO 	0x0010	/* C--S */
#define AUTHCONFIRM      0x0020    /*C--S*/
#define AUTHCLEARNRULE   0x0040   /*C--S*/

/** infoCode */
#define INFO_BEGIN	0x01
#define INFO_DATA	0x02
#define INFO_END	0x03

/*--------------------------------------*/
/*	commonInfo							*/
/*--------------------------------------*/
typedef struct _tagCommonRuleHead
{
	unsigned int nSize;			
	unsigned int nSubCommand;	
	unsigned int nAlarmLevel;			
	unsigned int nModuleID;		
	unsigned int nInfoCode;				
	unsigned int nExtRuleSize;	
	char ExtBuf[1];				
}COMMONRULEHEAD,*PCOMMONRULEHEAD;

typedef struct _tagCommonMsgHead
{
	unsigned int nSize;			
	char cpClientID[32];		
	char cpUserName[64];		
	char cpTimeLocal[24];
	unsigned int nAlarmLevel;			
	unsigned int nExtMsgSize;	
	char ExtBuf[1];				
}COMMONMSGHEAD, *PCOMMONMSGHEAD;

typedef struct _tagCommonList
{
	unsigned int nSize;
	unsigned int iInfoCode;
	char ExtBuf[1];
}COMMONLIST, *PCOMMONLIST;

/**
 * module for fileMonitor
 */
#define MODULE_FILE		0x02

/* command for fileMonitor */
#define FILE_POLICY		0x0001	/* S--C	*/
#define FILE_MSG		0x0002	/* C--S	*/

/**
 * module for login
 */
#define MODULE_LOGIN		0x03
/* command for login */
#define LOGIN_POLICY		0x0001	/* S--C */
#define LOGIN_MSG		0x0002	/* C--S */


/**
 * module for process
 */
#define MODULE_PROCESS		0x04
/* command for process */
#define PROCESS_POLICY		0x0001	/* S--C */
#define PROCESS_MSG		0x0002	/* C--S */

/**
 * module for integrity(完整性)
 */
#define MODULE_INTEGRITY    0x05
/* command for integrity */
#define INTEGRITY_POLICY		0x0001	/* S--C */
#define INTEGRITY_MSG		0x0002	/* C--S */
	
/**
 * module for network
 */
#define MODULE_NETWORK		0x06
/* command for netowrk */
#define NETWORK_POLICY		0x0001	/* S--C */
#define NETWORK_MSG		0x0002	/* C--S */

/**
 * module for hardconf
 */
#define MODULE_HARD		0x07
/* command for hardconf */
#define HARD_POLICY		0x0001	/* S--C */
#define HARD_MSG		0x0002	/* C--S */

/**
 * module for disk
 */
#define MODULE_DISK		0x08
/* command for disk */
#define DISK_POLICY		0x0001	/* S--C */
#define DISK_MSG		0x0002	/* C--S */

/** 
 * module for share
 */
#define MODULE_SHARE		0x09
/* commnad for share */
#define SHARE_POLICY		0x0001	/* S--C */
#define SHARE_MSG		0x0002	/* C--S	*/

/** 
 * module for service 
 */
#define MODULE_SERVICE		0x010
/* command for service */
#define SERVICE_POLICY		0x0001	/* S--C */
#define SERVICE_MSG		0x0002	/* C--S	*/
#define SERVICE_GET_LIST	0X0003	/* S--C */
#define SERVICE_PUT_LIST	0x0004	/* C--S */
#define SERVICE_GET_OPR		0x0005	/* S--C */
#define SERVICE_PUT_OPR		0x0006	/* C--S */
/**
 * module for config
 */
#define MODULE_CONFIG		0x011
/*	command for config */
#define CONFIG_POLICY	0x0001	/* S--C */
#define CONFIG_MSG		0x0002	/* C--S	*/
#define CONFIG_GET_LIST	0X0003	/* S--C */
#define CONFIG_PUT_LIST	0x0004	/* C--S */
#define CONFIG_SET_ENV  0x0005  /* S--C */

/*
 * module for user
*/
#define MODULE_USER		0x012
/* command for user*/
#define USER_POLICY		0x0001    /*S--C*/
#define USER_MSG		0x0002	  /*C--S*/	

/*
 * module for usb storage
*/
#define MODULE_USBSTORAGE	0x013
/* command for usbstorage*/
#define USBSTORAGE_POLICY    0x0001 /*S--C*/
#define USBSTORAGE_MSG       0x0002 /*C--S*/ 

/**
 * module for Soft
 */
#define MODULE_SOFT		0x14
/* command for soft */
#define SOFT_POLICY		0x0001	/* S--C	*/
#define SOFT_MSG		0x0002	/* C--S */
#define SOFTCTL_GET_LIST	0x0003	/* S--C	*/
#define SOFTCTL_PUT_LIST	0x0004	/* C--S	*/

#endif
