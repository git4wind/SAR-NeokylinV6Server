/**
 * author:pc
 * date:2015-10-16
 * desc:
 * 		test the sqlhelp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hbla_sqlhelp.h>

#define PATH "./hbla.db"
#define PATH_SINGLE "./hbla_single.db"
#define PWD "pclvmm"

#define NEWCLIENTID "123456testtest"

typedef struct tag_test{
	char name[24];
	int age;
	int bud;
}S_T;


int main(int argc, char **argv)
{
	int ret;
	int buidNo;
	char * cpClientID;
	S_T st;
	
	//ret = hbla_sqlhelp_init(PATH, PWD);
	ret = hbla_sqlhelp_single_init(PATH_SINGLE);
	if(0 != ret)
		return 0;

	buidNo = hbla_sqlhelp_getBuildNo();
	printf("get buildNo1 = %d\n", buidNo);
	hbla_sqlhelp_setBuildNo(buidNo + 1);
	buidNo = hbla_sqlhelp_getBuildNo();
	printf("get buildNo2 = %d\n", buidNo);

	cpClientID = hbla_sqlhelp_getClientID();
	printf("get clientID1 = %s\n", cpClientID ? cpClientID:"NULL");
	hbla_sqlhelp_setClientID(NEWCLIENTID);
	cpClientID = hbla_sqlhelp_getClientID();
	printf("get clientID2 = %s\n", cpClientID ? cpClientID:"NULL");

	int len;
	int state;
	S_T st_tem;
	
	ret = hbla_sqlhelp_getPolicyRule(0x02, &st_tem, &len, &state);
	if(ret == 0 || len == 0)
	{
		printf("no moduleRule!\n");
		strncpy(st.name, "pclvmm", 7);
		st.bud = 17;
		ret = hbla_sqlhelp_setPolicyRule(0x02, &st, sizeof(st), 1);
		ret = hbla_sqlhelp_getPolicyRule(0x02, &st_tem, &len, &state);
		if(ret == 0 || len != 0 )
		{
			printf("st_name = %s\t st_bud = %d\n",st_tem.name, st_tem.bud);
		}
	}else if(ret == 0 && len != 0)
	{
		printf("st_name = %s\t st_bud = %d\n",st_tem.name, st_tem.bud);
	}

	strncpy(st.name, "pclvmm", 7);
	st.bud = 17;
	int mno,cno;

	char Data[2046];
	ret = hbla_sqlhelp_readMsg(&Data, &len, &mno, &cno);
	if(len == 0){
		printf("no Msg\n");
	}else{
		S_T * stem = (S_T *)Data;
		printf("st_name = %s\t st_bud = %d mno=%d cno=%d \n",stem->name, stem->bud, mno, cno);
	}
	ret = hbla_sqlhelp_writeMsg(&st, sizeof(st), 1, 2);
	if(ret != 0)
		return -1;

	ret = hbla_sqlhelp_readMsg(&Data, &len, &mno, &cno);
	if(len == 0){
		printf("no Msg\n");
	}else{
		S_T * stem = (S_T *)Data;
		printf("st_name = %s\t st_bud = %d mno=%d cno=%d \n",stem->name, stem->bud, mno, cno);
	}

	ret = hbla_sqlhelp_readMsg(&Data, &len, &mno, &cno);
	if(len == 0){
		printf("no Msg\n");
	}else{
		S_T * stem = (S_T *)Data;
		printf("st_name = %s\t st_bud = %d mno=%d cno=%d \n",stem->name, stem->bud, mno, cno);
	}
	hbla_sqlhelp_setMsgStateByState(1, 0);
	
	ret = hbla_sqlhelp_readMsg(&Data, &len, &mno, &cno);
	if(len == 0){
		printf("no Msg\n");
	}else{
		S_T * stem = (S_T *)Data;
		printf("st_name = %s\t st_bud = %d mno=%d cno=%d \n",stem->name, stem->bud, mno, cno);
	}

	hbla_sqlhelp_delMsgByState();

	ret = hbla_sqlhelp_readMsg(&Data, &len, &mno, &cno);
	if(len == 0){
		printf("no Msg\n");
	}else{
		S_T * stem = (S_T *)Data;
		printf("st_name = %s\t st_bud = %d mno=%d cno=%d \n",stem->name, stem->bud, mno, cno);
	}

	return 0;
}
