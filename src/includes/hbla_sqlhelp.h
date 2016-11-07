/**
 * author:pc
 * date:2015-10-15 19:22
 * desc:
 * 		input the functions which based on sqlcipher to 
 * 		policy && rules && mesg && bulidNo and go on.
 */
#ifndef _HALA_SQLHELP_H_
#define _HALA_SQLHELP_H_

/**
 * initilize the sqlcipher
 */
int hbla_sqlhelp_init(const char *path, const char *pwd);

/**
 * get the buildNo
 */
int hbla_sqlhelp_getBuildNo();

/**
 * set the buildNo
 */
int hbla_sqlhelp_setBuildNo(int buildNo);

/**
 * get the Policy time
 */
char * hbla_sqlhelp_getPolicyTime();

/**
 * set the Policy time
 */
int hbla_sqlhelp_setPolicyTime(const char* pPolicyTime);

/**
 * get the clientID
 */
char * hbla_sqlhelp_getClientID();

/**
 * set the clientID
 */
int hbla_sqlhelp_setClientID(const char *pClientID);

/**
 * get the policyRule
 */
int hbla_sqlhelp_getPolicyRule(char cMNo,void *pData, int *len, int *state);
/**
 * set the policyRule
 */
int hbla_sqlhelp_setPolicyRule(char cMNo, void *pData, int len,int state);

/**
 * read a msgAuidt
 */
int hbla_sqlhelp_readMsg(void *pData, int *len, int *cMNo, int *sCNo);

/**
 * save a msgaudit
 */
int hbla_sqlhelp_writeMsg(void *pData, int len, int cMNo, int sCNo);

/**
 * set Msg NewState by oldState
 */
int hbla_sqlhelp_setMsgStateByState(int oldState, int newState);

/**
 * delete the sendd msgs
 */
int hbla_sqlhelp_delMsgByState();

/**
 * write the person in charge
 */
int hbla_sqlhelp_setPerson(const char* pPerson);

/**
 * read the person in charge
 */
char * hbla_sqlhelp_getPerson();

/**
 * write the department
 */
int hbla_sqlhelp_setDepartment(const char* pDepartment);

/**
 * read the department
 */
char * hbla_sqlhelp_getDepartment();

/**
 * wirte the unit
 */
int hbla_sqlhelp_setUnit(const char* pUnit);

/**
 * read the unit
 */
char * hbla_sqlhelp_getUnit();

/**
 * get the SIGNStatus
 */
int hbla_sqlhelp_getIsSign();

/**
 * set the buildNo
 */
int hbla_sqlhelp_setIsSign(int isSign);

/**
 * set the policyRule state
 */
int hbla_sqlhelp_setPolicyRule_state(char cMNo, char cState);

/**
 * get the policyRule state
 */
int hbla_sqlhelp_getPolicyRule_state(char cMNo);
#endif
