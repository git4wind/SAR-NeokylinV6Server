#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hbla_mod.h>
#include <hbla_modlist.h>
#include <hbla_command.h>
#include <hbla_sqlhelp.h>
/*add by modules*/
#include "../../../modules/fileMonitor/hbla_fileMonitor.h"
/*endadd*/
static MODULELIST *pMl;
static BASEINFO *pBi;

static int hbla_main_sendMsg(char cMNo, unsigned short sCNo, void *pData, int len)
{
	if(pData == NULL)
		return -1;
	printf("MNO = %d, CNO = %d, len = %d\n",cMNo, sCNo, len);
	COMMONMSGHEAD * pcm = (COMMONMSGHEAD *)pData;
	/*add by modules*/
	FILEMSG * pfm = (FILEMSG *)&(pcm->ExtBuf);
	printf("nSize = %d nFileType = %d nOperType = %d file + %s\n",
					pfm->nSize, 
					pfm->nFileType,
					pfm->nOperationType,
					pfm->fileOrDirName);
	/*endadd*/
	return 0;
}

int main(int argc, char** argv)
{
	int ret;

	/* getHostBaseInfo*/
	hbla_mod_setBaseInfo("192.168.0.100",
					 "1234561234561234561234561234561",
					 "hostName",
					 "testUserName",
					 "testDepartMent");
	pBi = hbla_mod_getBaseInfo();

	/* init the sqlhelp*/
	ret = hbla_sqlhelp_init("./conf/hbla.db","pclvmm");
	if(ret != 0)
		return -1;
	/* load the Modules*/
	ret = hbla_modlist_createModList("./libs");
	pMl = hbla_modlist_getModList();
	
	/* init the Modules */
	hbla_modlist_initModule(pBi, (void *)hbla_main_sendMsg, (void *)hbla_sqlhelp_setPolicyRule,
					(void *)hbla_sqlhelp_getPolicyRule);
	hbla_modlist_runModule();
	
	sleep(10);
	/* setPolciy */
	/*add by modules*/
	COMMONRULEHEAD * pcr = (COMMONRULEHEAD *)malloc(sizeof(COMMONRULEHEAD) + sizeof(FILERULE) + 254 + 256 );
	if(pcr == NULL)
		return -1;
	pcr->nSize = sizeof(COMMONRULEHEAD) + sizeof(FILERULE) + 254 + 256;
	pcr->nAlarmLevel = 1;
	/* beigin */
	pcr->nInfoCode = 1;
	pcr->nExtRuleSize = 0;
	hbla_modlist_disposePolicy((void *)pcr, MODULE_FILE);
	/*data*/
	pcr->nInfoCode = 2;
	pcr->nExtRuleSize = sizeof(FILERULE) + 255 + 256;
	FILERULE * pfr = (FILERULE *)(pcr->ExtBuf);
	pfr->nSize = sizeof(FILERULE) + 255 + 256;
	pfr->iAlarmLevel = 1;
	pfr->isActive = 1;
	pfr->isMrs = 1;
	pfr->num = 2;
	strncpy(pfr->paths,"/home/pclvmm/test/",256);
	strncpy((char *)pfr->paths + 256,"/home/pclvmm/test2/",256);
	hbla_modlist_disposePolicy((void *)pcr, MODULE_FILE);
	/*end*/
	pcr->nInfoCode = 3;
	pcr->nExtRuleSize = 0;
	hbla_modlist_disposePolicy((void *)pcr, MODULE_FILE);
	/*endadd*/

	while(1);
	return 0;
}
