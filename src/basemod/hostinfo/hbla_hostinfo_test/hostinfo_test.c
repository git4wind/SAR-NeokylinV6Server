#include <stdio.h>
#include <stdlib.h>
#include <hbla_hostinfo.h>

static HOSTINFO * g_host;

int main(int argc, char **argv)
{
	char * pID;
	char * pMD5;

	g_host = hbla_hostinfo_get();
	
	pID = hbla_hostinfo_getClientID(g_host);
	pMD5 = hbla_hostinfo_getCliInfoMD5(g_host);
	printf("id = %s, md5 = %s\n",pID, pMD5);
	printf("len=%d\n",sizeof(HOSTINFO));
	return 0;
}
