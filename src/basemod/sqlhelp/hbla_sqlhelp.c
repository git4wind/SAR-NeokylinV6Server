/**
 * author:pc
 * date:2015-10-16
 * desc:
 * 		set the sqlhelp for get update time && policyTime
 * 		buildNo && policy rules && audit msg
 */ 
#include <stdio.h>
#include <stdlib.h>
#include "hbla_sql.h"
#include "hbla_sqlhelp.h"

/**
 * initilize the sqlcipher
 */
int hbla_sqlhelp_init(const char *path, const char *pwd)
{
	return hbla_sql_init(path,pwd);
}

/**
 * initilize the sqlite
 */
int hbla_sqlhelp_single_init(const char *path)
{
	return hbla_sql_signle_init(path);
}

/**
 * get the buildNo
 */
int hbla_sqlhelp_getBuildNo()
{
	char pNo[24];
	int ret;
	ret = hbla_sql_selectStr("select buildNo from baseInfo;", pNo);
	if(ret != 0 || pNo == NULL)
		return 0;
	return atoi(pNo);
}

/**
 * set the buildNo
 */
int hbla_sqlhelp_setBuildNo(int buildNo)
{
	char sql[128];
	sprintf(sql,"update baseInfo set buildNo = %d;",buildNo);
	return hbla_sql_exec(sql);
}

/**
 * get the Policy time
 */
char * hbla_sqlhelp_getPolicyTime()
{
	static char policyTime[24];
	int ret;
	
	ret = hbla_sql_selectStr("select policyTime from baseInfo;", policyTime);
	if(ret != 0)
		return NULL;
	policyTime[24] = '\0';

	return policyTime;
}

/**
 * set the Policy time
 */
int hbla_sqlhelp_setPolicyTime(const char* pPolicyTime)
{
	char sql[128];
	
	if(pPolicyTime == NULL)
		return -1;
	sprintf(sql,"update baseInfo set policyTime = '%s';", pPolicyTime);
	return hbla_sql_exec(sql);
}

/**
 * get the clientID
 */
char * hbla_sqlhelp_getClientID()
{
	static char clientID[32];
	int ret;

	ret = hbla_sql_selectStr("select clientID from baseInfo;", clientID);
	if(0 != ret)
		return NULL;
	clientID[32] = '\0';
	return clientID;
}

/**
 * set the clientID
 */
int hbla_sqlhelp_setClientID(const char *pClientID)
{
	char sql[128];

	if(NULL == pClientID)
		return -1;
	sprintf(sql, "update baseInfo set clientID = '%s';", pClientID);
	
	return hbla_sql_exec(sql);
}

/**
 * get the policyRule state
 */
int hbla_sqlhelp_getPolicyRule_state(char cMNo)
{
	char sql[128];
	int ret;
	char State[8] = {'0'};

	if(cMNo < 0)
		return -1;
	sprintf(sql, "select state from policyRule where moduleNO = %d;", cMNo);
	ret = hbla_sql_selectStr(sql, State);
	if(0 != ret)
		return -1;
	return State[0] - '0';
}

/**
 * set the policyRule state
 */
int hbla_sqlhelp_setPolicyRule_state(char cMNo, char cState)
{
	char sql[128];
	int ret;
	if(cMNo < 0)
		return -1;
	sprintf(sql,"update policyRule set state = %d where moduleNo = %d",cState, cMNo);
	hbla_sql_exec(sql);
	return 0;
}

/**
 * get the policyRule
 */
int hbla_sqlhelp_getPolicyRule(char cMNo,void *pData, int *len, int *state)
{
	char sql[128];
	int ret;
	char State[8]={0};

	if(pData == NULL)
		return -1;
	sprintf(sql, "select state from policyRule where moduleNO = %d;", cMNo);
	ret = hbla_sql_selectStr(sql, State);
	if(0 != ret)
		return -1;
	if(State[0] == 0)
	{
		sprintf(sql,"insert into policyRule values(%d, NULL, 0);", cMNo);
		hbla_sql_exec(sql);
		*(char *)pData = '\0';
		*len = 0;
		*state = 0;
		return 0;
	}
	*state = atoi(State);
	sprintf(sql, "select rule from policyRule where moduleNO = %d and state = 1;",cMNo);
	hbla_sql_selectBind(sql, pData, len);
	return 0;
}

/**
 * set the policyRule
 */
int hbla_sqlhelp_setPolicyRule(char cMNo, void *pData, int len,int state)
{
	char sql[128];
	int ret;
	if(pData == NULL)
	{
		sprintf(sql, "update policyRule set rule = NULL, state = %d where moduleNO = %d;", state, cMNo);
		ret = hbla_sql_exec(sql);
		if(0 != ret)
			return -1;
		return 0;
	}
	sprintf(sql, "update policyRule set state = %d where moduleNO = %d;", state, cMNo);
	ret = hbla_sql_exec(sql);
	if(0 != ret)
		return -1;
	sprintf(sql, "update policyRule set rule = ? where moduleNO = %d;", cMNo);
	ret = hbla_sql_execBind(sql, pData, len);
	if(0 != ret)
		return -1;
	return 0;
}

/**
 * set Msg state by id
 */
int hbla_sqlhelp_setMsgStateById(int id, int state)
{
	char sql[128];
	int ret;
	
	sprintf(sql, "update auditMsg set state = %d where id = %d", state, id);
	ret = hbla_sql_exec(sql);
	if(0 != ret)
		return -1;
	return 0;
}

/**
 * delete the sendd msgs
 */
int hbla_sqlhelp_delMsgByState()
{
	char sql[128];
	int ret;
	
	sprintf(sql, "delete from auditMsg where state = 1;");
	ret = hbla_sql_exec(sql);
	if(0 != ret)
		return -1;
	return 0;

}
/**
 * set Msg NewState by oldState
 */
int hbla_sqlhelp_setMsgStateByState(int oldState, int newState)
{
	char sql[128];
	int ret;
	
	sprintf(sql, "update auditMsg set state = %d where state = %d", newState, oldState);
	ret = hbla_sql_exec(sql);
	if(0 != ret)
		return -1;
	return 0;
}

/**
 * save a msgaudit
 */
int hbla_sqlhelp_writeMsg(void *pData, int len, int cMNo, int sCNo)
{
	char sql[128];
	int ret;

	if(pData == NULL || len < 1)
		return -1;
	sprintf(sql, "insert into auditMsg values(NULL, ?, ?, ?, 0);");
	ret = hbla_sql_execBindMsg(sql, pData, len, cMNo, sCNo);
	if(0 != ret)
		return -1;

	return 0;
}

/**
 * read a msgAuidt
 */
int hbla_sqlhelp_readMsg(void *pData, int *len, int *cMNo, int *sCNo)
{
	char sql[128];
	int ret;

	sprintf(sql, "select msg,mno,cno from auditMsg where state = 0 order by id limit 1;");
	ret = hbla_sql_selectBindMsg(sql, pData, len, cMNo, sCNo);
	if(0 != ret)
		return -1;
	if(*len == 0)
		return 0;
	sprintf(sql, "update auditMsg set state = 1 where id in (select id from auditMsg where state = 0 order by id limit 1);");
	ret = hbla_sql_exec(sql);
	if(0 != ret)
		return -1;

	return 0;
}

/**
 * write the person in charge
 */
int hbla_sqlhelp_setPerson(const char* pPerson)
{
	char sql[256];
	
	if(pPerson == NULL)
		return -1;
	sprintf(sql,"update baseInfo set person = '%s';", pPerson);
	return hbla_sql_exec(sql);
}

/**
 * read the person in charge
 */
char * hbla_sqlhelp_getPerson()
{
	static char person[128];
	int ret;
	
	ret = hbla_sql_selectStr("select person from baseInfo;", person);
	if(ret != 0)
		return NULL;
	person[128] = '\0';

	return person;
}

/**
 * write the department
 */
int hbla_sqlhelp_setDepartment(const char* pDepartment)
{
	char sql[256];
	
	if(pDepartment == NULL)
		return -1;
	sprintf(sql,"update baseInfo set department = '%s';", pDepartment);
	return hbla_sql_exec(sql);
}

/**
 * read the department
 */
char * hbla_sqlhelp_getDepartment()
{
	static char department[128];
	int ret;
	
	ret = hbla_sql_selectStr("select department from baseInfo;", department);
	if(ret != 0)
		return NULL;
	department[128] = '\0';

	return department;
}

/**
 * wirte the unit
 */
int hbla_sqlhelp_setUnit(const char* pUnit)
{
	char sql[256];
	
	if(pUnit == NULL)
		return -1;
	sprintf(sql,"update baseInfo set unit = '%s';", pUnit);
	return hbla_sql_exec(sql);
}

/**
 * read the unit
 */
char * hbla_sqlhelp_getUnit()
{
	static char unit[128];
	int ret;
	
	ret = hbla_sql_selectStr("select unit from baseInfo;", unit);
	if(ret != 0)
		return NULL;
	unit[128] = '\0';

	return unit;
}

/**
 * get the SIGNStatus
 */
int hbla_sqlhelp_getIsSign()
{
	char pNo[24];
	int ret;
	ret = hbla_sql_selectStr("select isSign from baseInfo;", pNo);
	if(ret != 0 || pNo == NULL)
		return 0;
	return atoi(pNo);
}

/**
 * set the buildNo
 */
int hbla_sqlhelp_setIsSign(int isSign)
{
	char sql[128];
	sprintf(sql,"update baseInfo set isSign = %d;", isSign);
	return hbla_sql_exec(sql);
}
