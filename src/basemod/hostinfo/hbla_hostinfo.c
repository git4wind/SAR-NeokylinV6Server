/**
 * file hbla_hostinfo.h
 * author: dyk
 * desc: get the linux host info
 * 		change by pc 2015-10-20
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <utmp.h>
#include <paths.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <arpa/inet.h>

#include "hbla_hostinfo.h"
#include <hbla_log.h>

#if 0 
//delete by pc 2015-10-20
#define DIR_INSTALL  "/hbhsp/temp"
#define DIR_HOSTINFO  "/hbhsp/temp/hostinfo"
#endif 

#define FILE_HB_OS_INIT_TIME	"/tmp/hb_os_init_time.hbt"
#define FILE_HB_BIOS			"/tmp/hb_bios.hbt"
#define FILE_HB_BASEBOARD		"/tmp/hb_baseboard.hbt"
#define FILE_HB_HARDDISK		"/tmp/hb_harddisk.hbt"
#define FILE_HB_DISKSIZE		"/tmp/hb_disksize.hbt"

#define FILE_MEMINFO	"/proc/meminfo"
#define FILE_CPUINFO	"/proc/cpuinfo"
#define FILE_BOARDINFO	"/proc/boardinfo"

#ifndef min
#define min(x, y) ({ \
	const typeof(x) _x = (x); \
	const typeof(y) _y = (y); \
	(void) (&_x == &_y); \
	_x < _y ? _x : _y; })
#endif

/************************************************************************/
/* 删除字符串中指定字符 */
/************************************************************************/
static void del_char(char* str, char ch)
{
	char *p = str;
	char *q = str;
	while(*q)
	{
		if (*q !=ch)
			*p++ = *q;
		q++;
	}
	*p='\0';
}

/**
 * judge the file isExist
 */
static int file_exist(const char* cpFilePath)
{
	if (NULL == cpFilePath)
		return 0;
	if (0 == access(cpFilePath, F_OK))
		return 1;
	return 0;
}

/**
 * 判断目录是否存在 1=存在 0=不存在
 */
static int dir_exist(const char* cpDirPath)
{
	DIR* dir;
	if (NULL == cpDirPath)
		return 0;
	
	dir = opendir(cpDirPath);
	if (NULL == dir)
		return 0;
	closedir(dir);

	return 1;
}

/**
 * crate the dir by the dirPath
 */
static int createdir(const char* cpDirPath)  
{  
	char cpTemp[512] = {0};  
	strcpy(cpTemp,   cpDirPath);  
	int i;
	int len = strlen(cpTemp);
	if(cpTemp[len-1]!='/')
		strcat(cpTemp,   "/");
	len = strlen(cpTemp);

	for(i=1; i<len; i++)
	{
		if(cpTemp[i]=='/')
		{
			cpTemp[i] = 0;
			if(0 == dir_exist(cpTemp))  
			{
				if(mkdir(cpTemp, 0755) == -1)  
					return  0;
			}
			cpTemp[i] = '/';
		}  
	}
   
	return 1;  
}

/**
 * get the system install_time
 */
static void get_sys_install_time(char* pSysInstallTime)
{
	char cpTemp[512] = {0};
	char cpFileName[128] = {0};
	strcpy(cpFileName, FILE_HB_OS_INIT_TIME);
	system("passwd -S root|tee > /tmp/hb_os_init_time.hbt");
	FILE* fp = fopen(cpFileName,"r");  
	if(NULL == fp)
		return;
	
	fseek(fp,0,SEEK_SET);
	if((fgets(cpTemp,sizeof(cpTemp),fp))!=NULL); 
		cpTemp[strlen(cpTemp)-1] = '\0';
	fclose(fp);
	
	if (0 != strlen(cpTemp))
	{
		char* pTemp = NULL;
		pTemp = strstr(cpTemp, "root ");
		if (NULL != pTemp)
			memcpy(pSysInstallTime, pTemp+8, 10);
	}
}

/**
 * get the group name
 */
static void get_group_name(int iGid, char* cpGroupName, unsigned int nMaxSize)
{
	struct group *data;
	data = getgrgid(iGid);
	if(data != NULL)
		strncpy(cpGroupName,data->gr_name, min(nMaxSize, strlen(data->gr_name)));
}

/**
 * get the os infomations
 */
static int get_os_info(PSYSTEMINFO pSystemInfo)
{
	char cpTemp[512] = {0};

	FILE* fp = fopen("/etc/system-release","r");
	if (NULL == fp)
		return 0;
	if((fgets(cpTemp, sizeof(cpTemp), fp)) != NULL) 
		cpTemp[strlen(cpTemp)-1] = '\0';		
	fclose(fp);

	if (0 != strlen(cpTemp))
	{
		strncpy(pSystemInfo->cpOSVerInfo, cpTemp, min(sizeof(pSystemInfo->cpOSVerInfo), strlen(cpTemp)));
		pSystemInfo->cpOSVerInfo[sizeof(pSystemInfo->cpOSVerInfo)-1] = 0;
	}

	return 1;
}

/**
 * get the scaled
 */
static unsigned long
get_scaled(const char *buffer, const char *key)
{
	const char    *ptr;
	char          *next;
	unsigned long value = 0;

	if ((ptr = strstr(buffer, key)))
	{
		ptr += strlen(key);
		value = strtoul(ptr, &next, 0);
		if (strchr(next, 'k'))
			value *= 1024;
		else if (strchr(next, 'M'))
			value *= 1024 * 1024;
	}

	return value;
}

/**
 * get meminfo
 */
void get_mem_info(PMEMORYINFO pMemInfo)
{
	char buffer [BUFSIZ] = {0};
	int fd = 0;
	int len = 0;
	unsigned int nTotal = 0;

	fd = open (FILE_MEMINFO, 0);
	if (fd < 0)
		return;
	
	len = read (fd, buffer, BUFSIZ-1);
	if (len < 0)
		return;
	close (fd);
	
	buffer [len] = '\0';
	nTotal  = get_scaled(buffer, "MemTotal:");
	if (0 != nTotal)
		sprintf(pMemInfo->cpMemTotal, "%d MB", nTotal/1024/1000);
	/*
	 * 	pMemInfo->used   = get_scaled(buffer, "Active:");
	 *  pMemInfo->free   = get_scaled(buffer, "MemFree:");
	 * 	pMemInfo->shared = get_scaled(buffer, "Mapped:");
	 * 	pMemInfo->buffer = get_scaled(buffer, "Buffers:");
	 * 	pMemInfo->cached = get_scaled(buffer, "Cached:");
	 * 	pMemInfo->user = pMemInfo->total - pMemInfo->free - pMemInfo->cached - pMemInfo->buffer;
	 */
	//pMemInfo->flags = sysdeps_mem;
}

/**
 * get the cpu info
 */
int get_cpu_info(PCPUINFO pCpuInfo)
{
		char cpTemp[1024] = {0};
		char cpModelName[128] = {0};
		char cpVendor[64] = {0};
		int iNum = 0;
		char *ptr;
		FILE* fp = fopen(FILE_CPUINFO,"r");

		if (NULL == fp)
			return 0;
		while((fgets(cpTemp, sizeof(cpTemp), fp))!=NULL) 
		{
			ptr = strstr(cpTemp,"processor");
			if(ptr != NULL)
				iNum++;
			ptr = strstr(cpTemp,"system type");
			if(ptr != NULL)
			{
				ptr = strstr(cpTemp,": ");
				strncpy(cpVendor,ptr+2,strlen(ptr)-2);
				cpVendor[strlen(ptr)-3] = '\0';
			}
			ptr = strstr(cpTemp,"model name");
			if(ptr != NULL)
			{
				ptr = strstr(cpTemp,": ");
				strncpy(cpModelName,ptr+2,strlen(ptr)-2);
				cpModelName[strlen(ptr)-3] = '\0';
			}

			if (0 == strlen(pCpuInfo->cpVendor))
			{
				ptr = strstr(cpTemp,"vendor_id");
				if(ptr != NULL)
				{
					ptr = strstr(cpTemp,": ");
					strncpy(cpVendor,ptr+2,strlen(ptr)-2);
					cpVendor[strlen(ptr)-3] = '\0';
				}
			}
		}
		fclose(fp);

		if (0 != strlen(cpVendor))
		{
			if (NULL != strstr(cpVendor, "loongson"))
			{
				strcpy(pCpuInfo->cpVendor, "loongson");
			}else if (NULL != strstr(cpVendor, "GenuineIntel"))
			{
				strcpy(pCpuInfo->cpVendor, "Intel");
			}else if (NULL != strstr(cpVendor, "AuthenticAMD"))
			{
				strcpy(pCpuInfo->cpVendor, "AMD");
			}else
			{
				strcpy(pCpuInfo->cpVendor, cpVendor);
				pCpuInfo->cpVendor[sizeof(pCpuInfo->cpVendor)-1] = '\0';
			}
		}

		if ((0 != iNum) && (0 != strlen(cpModelName)))
		{
			if (1 == iNum)
			{
				strcpy(pCpuInfo->cpProcessorName, cpModelName);
			}else
			{
				sprintf(pCpuInfo->cpProcessorName, "%dx %s", iNum, cpModelName);
			}

			if (0 == strlen(cpVendor))
			{
				if (NULL != strstr(cpModelName, "loongson"))
				{
					strcpy(pCpuInfo->cpVendor, "loongson");
				}else if (NULL != strstr(cpModelName, "Intel"))
				{
					strcpy(pCpuInfo->cpVendor, "Intel");
				}else if (NULL != strstr(cpModelName, "AMD"))
				{
					strcpy(pCpuInfo->cpVendor, "AMD");
				}
			}
		}
	return 1;
}

/**
 * get stst info
 */
static int get_system_info(PSYSTEMINFO pSystemInfo)
{
	struct utsname uBuf;
	if(uname(&uBuf) >= 0)
	{
		strcpy(pSystemInfo->cpComputerName, uBuf.nodename);//计算机名称
		sprintf(pSystemInfo->cpKernelInfo, "%s %s (%s)", uBuf.sysname, uBuf.release, uBuf.machine);//内核信息
		strncpy(pSystemInfo->cpCompiled, uBuf.version, min(sizeof(pSystemInfo->cpCompiled), strlen(uBuf.version)));//编译时间
	}

	uid_t userid;
	userid = getuid();

	struct passwd* pwd;
	pwd = getpwuid(userid);//获取当前用户
	if (NULL != pwd)
	{
		strncpy(pSystemInfo->cpUserName, pwd->pw_name, min(sizeof(pSystemInfo->cpUserName), strlen(pwd->pw_name)));
		get_group_name(pwd->pw_gid, pSystemInfo->cpGroupName, sizeof(pSystemInfo->cpGroupName));
	}

	get_os_info(pSystemInfo);
	get_sys_install_time(pSystemInfo->cpSysInstTime);//获取系统安装日期-非root没权限

	return 1;
}

/**
 * get board info
 */
static int get_board_info_ex(PHOSTINFO pHostInfo)
{
	char* pTemp = NULL;
	char cpTemp[512] = {0};
	char cpTempBuf[512] = {0};
	
	FILE* fp = fopen(FILE_BOARDINFO,"r");  
	if (NULL == fp)
	{
		return 0;
	}
								
	while((fgets(cpTemp,sizeof(cpTemp),fp))!=NULL)
	{
		cpTemp[strlen(cpTemp)-1] = '\0';

		pTemp = strstr(cpTemp, "Vendor");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			if (NULL != pTemp)
			{
				memset(cpTempBuf, 0, sizeof(cpTempBuf));
				strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
				cpTempBuf[strlen(pTemp)-3] = '\0';
				strncpy(pHostInfo->BiosInfo.cpVendor, cpTempBuf, min(sizeof(pHostInfo->BiosInfo.cpVendor), strlen(cpTempBuf)));
				pHostInfo->BiosInfo.cpVendor[sizeof(pHostInfo->BiosInfo.cpVendor)-1] = 0;
			}			
		}

		pTemp = strstr(cpTemp, "Version");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			if (NULL != pTemp)
			{
				memset(cpTempBuf, 0, sizeof(cpTempBuf));
				strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
				cpTempBuf[strlen(pTemp)-3] = '\0';
				strncpy(pHostInfo->BiosInfo.cpVersion, cpTempBuf, min(sizeof(pHostInfo->BiosInfo.cpVersion), strlen(cpTempBuf)));
				pHostInfo->BiosInfo.cpVersion[sizeof(pHostInfo->BiosInfo.cpVersion)-1] = 0;
			}
		}

		pTemp = strstr(cpTemp, "Release date");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			if (NULL != pTemp)
			{
				memset(cpTempBuf, 0, sizeof(cpTempBuf));
				strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
				cpTempBuf[strlen(pTemp)-3] = '\0';
				strncpy(pHostInfo->BiosInfo.cpDate, cpTempBuf, min(sizeof(pHostInfo->BiosInfo.cpDate), strlen(cpTempBuf)));
				pHostInfo->BiosInfo.cpDate[sizeof(pHostInfo->BiosInfo.cpDate)-1] = 0;
			}
		}

		pTemp = strstr(cpTemp, "Manufacturer");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			if (NULL != pTemp)
			{
				memset(cpTempBuf, 0, sizeof(cpTempBuf));
				strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
				cpTempBuf[strlen(pTemp)-2] = '\0';
				strncpy(pHostInfo->BoardInfo.cpVendor, cpTempBuf, min(sizeof(pHostInfo->BoardInfo.cpVendor), strlen(cpTempBuf)));
				pHostInfo->BoardInfo.cpVendor[sizeof(pHostInfo->BoardInfo.cpVendor)-1] = 0;
			}
		}

		pTemp = strstr(cpTemp, "Board name");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			if (NULL != pTemp)
			{
				memset(cpTempBuf, 0, sizeof(cpTempBuf));
				strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
				cpTempBuf[strlen(pTemp)-2] = '\0';
				strncpy(pHostInfo->BoardInfo.cpName, cpTempBuf, min(sizeof(pHostInfo->BoardInfo.cpName), strlen(cpTempBuf)));
				pHostInfo->BoardInfo.cpName[sizeof(pHostInfo->BoardInfo.cpName)-1] = 0;
			}
			break;
		}
	}

	fclose(fp);
	return 1;
}

/**
 * get bios info
 */
static int get_bios_info(PBIOSINFO pBiosInfo)
{
		char* pTemp = NULL;
		char cpTemp[512] = {0};
		char cpTempBuf[512] = {0};

		system("dmidecode -t bios>/tmp/hb_bios.hbt");
		FILE* fp = fopen(FILE_HB_BIOS,"r");  
		if (NULL == fp)
			return 0;

		while((fgets(cpTemp,sizeof(cpTemp),fp))!=NULL)
		{
			cpTemp[strlen(cpTemp)-1] = '\0';
			pTemp = strstr(cpTemp,"Vendor");

			if(NULL != pTemp)
			{
				pTemp = strstr(cpTemp, ": ");
				if (NULL != pTemp)
				{
					memset(cpTempBuf, 0, sizeof(cpTempBuf));
					strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
					cpTempBuf[strlen(pTemp)-2] = '\0';
					strncpy(pBiosInfo->cpVendor, cpTempBuf, min(sizeof(pBiosInfo->cpVendor), strlen(cpTempBuf)));
					pBiosInfo->cpVendor[sizeof(pBiosInfo->cpVendor)-1] = 0;
				}			
			}

			pTemp =strstr(cpTemp,"Version");
			if(NULL != pTemp)
			{
				pTemp = strstr(cpTemp, ": ");

				if (NULL != pTemp)
				{
					memset(cpTempBuf, 0, sizeof(cpTempBuf));
					strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
					cpTempBuf[strlen(pTemp)-2] = '\0';
					strncpy(pBiosInfo->cpVersion, cpTempBuf, min(sizeof(pBiosInfo->cpVersion), strlen(cpTempBuf)));
					pBiosInfo->cpVersion[sizeof(pBiosInfo->cpVersion)-1] = 0;
				}
			}

			pTemp = strstr(cpTemp,"Release Date");
			if(NULL != pTemp)
			{
				pTemp = strstr(cpTemp, ": ");

				if (NULL != pTemp)
				{
					memset(cpTempBuf, 0, sizeof(cpTempBuf));
					strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
					cpTempBuf[strlen(pTemp)-2] = '\0';
					strncpy(pBiosInfo->cpDate, cpTempBuf, min(sizeof(pBiosInfo->cpDate), strlen(cpTempBuf)));

					pBiosInfo->cpDate[sizeof(pBiosInfo->cpDate)-1] = 0;
				}

				break;
			}
		} 

		fclose(fp);
		return 1;
}

/**
 * get board info
 */

int get_board_info(PBOARDINFO pBoardInfo)
{
		char* pTemp = NULL;
		char cpTemp[512] = {0};
		char cpTempBuf[512] = {0};

		system("dmidecode -t baseboard>/tmp/hb_baseboard.hbt");
		FILE* fp = fopen(FILE_HB_BASEBOARD,"r");  
		if (NULL == fp)
			return 0;
		
		while((fgets(cpTemp, sizeof(cpTemp), fp))!=NULL)
		{
			cpTemp[strlen(cpTemp)-1] = '\0';

			pTemp =strstr(cpTemp,"Manufacturer");
			if(NULL != pTemp)
			{
				pTemp = strstr(cpTemp, ": ");

				if (NULL != pTemp)
				{
					memset(cpTempBuf, 0, sizeof(cpTempBuf));
					strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
					cpTempBuf[strlen(pTemp)-2] = '\0';
					strncpy(pBoardInfo->cpVendor, cpTempBuf, min(sizeof(pBoardInfo->cpVendor), strlen(cpTempBuf)));
					pBoardInfo->cpVendor[sizeof(pBoardInfo->cpVendor)-1] = 0;
				}
			}

			pTemp =strstr(cpTemp,"Product Name");
			if(NULL != pTemp)
			{
				pTemp = strstr(cpTemp, ": ");
				if (NULL != pTemp)
				{
					memset(cpTempBuf, 0, sizeof(cpTempBuf));
					strncpy(cpTempBuf,pTemp+2,strlen(pTemp)-2);
					cpTempBuf[strlen(pTemp)-2] = '\0';
					strncpy(pBoardInfo->cpName, cpTempBuf, min(sizeof(pBoardInfo->cpName), strlen(cpTempBuf)));
					pBoardInfo->cpName[sizeof(pBoardInfo->cpName)-1] = 0;
				}
				break;
			}
		} 
		fclose(fp);

		return 1;
}

/**
 * get disk Size
 */

static int get_hard_disk_size(PHARDDISKINFO pHardDiskInfo)
{
	FILE* fp = NULL;
	char* pTemp = NULL;
	char* pTempBuf = NULL;
	char cpTemp[512] = {0};
	char cpTemBuf[512] = {0};
	int iLen = 0;

	system("fdisk -l>/tmp/hb_disksize.hbt");
	fp = fopen(FILE_HB_DISKSIZE,"r");
	if (NULL == fp)
		return 0;

	while((fgets(cpTemp, sizeof(cpTemp), fp))!=NULL)
	{
		cpTemp[strlen(cpTemp)-1] = '\0';
		pTemp = strstr(cpTemp,"/dev/sda:");
		if (NULL == pTemp)
		{
			pTemp = strstr(cpTemp,"/dev/hda:");
		}

		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ": ");
			pTempBuf = strstr(cpTemp, ",");

			if ((NULL != pTemp) && (NULL != pTempBuf))
			{
				iLen = strlen(pTemp)-strlen(pTempBuf)-2;
				if (iLen > 0)
				{
					memset(cpTemBuf, 0, sizeof(cpTemBuf));
					strncpy(cpTemBuf,pTemp+2,iLen);
					cpTemBuf[sizeof(cpTemBuf)-1] = 0;
					strncpy(pHardDiskInfo->cpHardDiskSize, cpTemBuf, min(sizeof(pHardDiskInfo->cpHardDiskSize), strlen(cpTemBuf)));
					pHardDiskInfo->cpHardDiskSize[sizeof(pHardDiskInfo->cpHardDiskSize)-1] = 0;
				}				
			}
			break;
		}
	} 
	fclose(fp);

	return 1;
}

/**
 * get hard info
 */
int get_hard_disk_info(PHARDDISKINFO pHardDiskInfo)
{
	FILE* fp = NULL;
	char* pTemp = NULL;
	char cpTemp[512] = {0};
	char cpTemBuf[512] = {0};

	system("hdparm -I /dev/sda|grep \"Number:\">/tmp/hb_harddisk.hbt");
	fp = fopen(FILE_HB_HARDDISK,"r");  
	if (NULL == fp)
	{
		system("hdparm -I /dev/hda|grep \"Number:\">/tmp/hb_harddisk.hbt");
		fp = fopen(FILE_HB_HARDDISK,"r");  
		if (NULL == fp)
		return 0;
	}
	
	while((fgets(cpTemp, sizeof(cpTemp), fp))!=NULL)
	{
		cpTemp[strlen(cpTemp)-1] = '\0';

		pTemp =strstr(cpTemp,"Model Number");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ":       ");

			if (NULL != pTemp)
			{
				memset(cpTemBuf, 0, sizeof(cpTemBuf));
				strncpy(cpTemBuf,pTemp+8,strlen(pTemp)-8);
				cpTemBuf[strlen(pTemp)-3] = '\0';
				del_char(cpTemBuf, ' ');
				strncpy(pHardDiskInfo->cpModelModel, cpTemBuf, min(sizeof(pHardDiskInfo->cpModelModel), strlen(cpTemBuf)));
				pHardDiskInfo->cpModelModel[sizeof(pHardDiskInfo->cpModelModel)-1] = 0;
			}			
		}

		pTemp = strstr(cpTemp,"Serial Number");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, ":      ");

			if(NULL != pTemp)
			{
				memset(cpTemBuf, 0, sizeof(cpTemBuf));
				strncpy(cpTemBuf,pTemp+7,strlen(pTemp)-7);
				cpTemBuf[strlen(pTemp)-3] = '\0';
				strncpy(pHardDiskInfo->cpSerial, cpTemBuf, min(sizeof(pHardDiskInfo->cpSerial), strlen(cpTemBuf)));
				pHardDiskInfo->cpSerial[sizeof(pHardDiskInfo->cpSerial)-1] = 0;
			}			

			break;
		}
	}

	fclose(fp);
	/** get serial function 2*/	
	if(strlen(pHardDiskInfo->cpSerial) > 2)
		return 1;

	system("udevadm info --query=property --name=/dev/sda |grep ID_MODEL > /tmp/hb_harddisk.hbt");
	fp = fopen(FILE_HB_HARDDISK,"r");  
	if (NULL == fp)
	{
		system("udevadm info --query=property --name=/dev/hda |grep ID_MODEL > /tmp/hb_harddisk.hbt");
		fp = fopen(FILE_HB_HARDDISK,"r");  
		if (NULL == fp)
		return 0;
	}
	
	while((fgets(cpTemp, sizeof(cpTemp), fp))!=NULL)
	{
		cpTemp[strlen(cpTemp)-1] = '\0';

		pTemp =strstr(cpTemp,"ID_MODEL=");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, "=");

			if (NULL != pTemp)
			{
				memset(cpTemBuf, 0, sizeof(cpTemBuf));
				strncpy(cpTemBuf,pTemp+1,strlen(pTemp)-1);
				cpTemBuf[strlen(pTemp)-1] = '\0';
				del_char(cpTemBuf, ' ');
				strncpy(pHardDiskInfo->cpModelModel, cpTemBuf, min(sizeof(pHardDiskInfo->cpModelModel), strlen(cpTemBuf)));
				pHardDiskInfo->cpModelModel[sizeof(pHardDiskInfo->cpModelModel)-1] = 0;
			}			
		}

		pTemp = strstr(cpTemp,"ID_MODEL_ENC=");
		if(NULL != pTemp)
		{
			pTemp = strstr(cpTemp, "=");

			if(NULL != pTemp)
			{
				memset(cpTemBuf, 0, sizeof(cpTemBuf));
				strncpy(cpTemBuf,pTemp+1,strlen(pTemp)-1);
				cpTemBuf[strlen(pTemp)-1] = '\0';
				strncpy(pHardDiskInfo->cpSerial, cpTemBuf, min(sizeof(pHardDiskInfo->cpSerial), strlen(cpTemBuf)));
				pHardDiskInfo->cpSerial[sizeof(pHardDiskInfo->cpSerial)-1] = 0;
			}			

			break;
		}
	}

	fclose(fp);

	return 1;
}

/**
 * get the macinfo
 */
static int get_ifinfo(HOSTINFO * hi)
{
	static ifNums;
	struct ifreq ifr;
	struct ifconf ifc;
	char buf[2048];
	int success = 0;
		
	ifNums = 0;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        printf("socket error\n");
        return -1;
    }
					 
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
       printf("ioctl error\n");
       return -1;
  }
								 
   struct ifreq* it = ifc.ifc_req;
   const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
   char szMac[64];
   int count = 0;
   unsigned long nIP, nNetmask, nBroadIP;
   for (; it != end; ++it) 
   {
	   strcpy(ifr.ifr_name, it->ifr_name);
	   if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) 
	   {
		   if (! (ifr.ifr_flags & IFF_LOOPBACK)) 
		   { 
			   if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) 
			   {
			 		count ++ ;		
					unsigned char * ptr ;
					ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
					/** get macaddr */
					snprintf(hi->ifInfo[ifNums].cpInterMac,24,"%02X:%02X:%02X:%02X:%02X:%02X",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
					/* get interfaceName */
					strcpy(hi->ifInfo[ifNums].cpInterName,ifr.ifr_name);
					/* get ip */
					if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
						nIP = 0;
					else
						nIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
				    strcpy(hi->ifInfo[ifNums].cpIp,(char *)inet_ntoa(*(struct in_addr*)&nIP));
					/* get boradIp*/
					if(ioctl(sock, SIOCGIFBRDADDR, &ifr) < 0)
						nBroadIP = 0;
				    else
						nBroadIP = *(unsigned long*)&ifr.ifr_broadaddr.sa_data[2];
					strcpy(hi->ifInfo[ifNums].cpBroadIp,(char *)inet_ntoa(*(struct in_addr*)&nBroadIP));
					/* get MaskAddr */
					if(ioctl(sock, SIOCGIFNETMASK, &ifr) < 0)
						nNetmask = 0;
					else
						nNetmask = *(unsigned long*)&ifr.ifr_netmask.sa_data[2];
					strncpy(hi->ifInfo[ifNums].cpNetMask,(char *)inet_ntoa(*(struct in_addr*)&nNetmask),24);
					/* get MTU	*/
					if(ioctl(sock, SIOCGIFMTU, &ifr) < 0)
						hi->ifInfo[ifNums].iMtu = 0;
				  	else
						hi->ifInfo[ifNums].iMtu = ifr.ifr_mtu;
					hi->interfaceNum = ++ifNums;
			   }
		   }
	   }else{
			   return -1;
	   }
   }
	
}

static HOSTINFO HostInfo;
/**
 * get hostInfo
 */
HOSTINFO *hbla_hostinfo_get()
{
	memset(&HostInfo, 0, sizeof(HOSTINFO));
	HostInfo.nSize = sizeof(HOSTINFO);

#if 0
//delete by pc 2015-10-20
	if (0 == dir_exist(DIR_INSTALL))
	{
		createdir(DIR_INSTALL);
	}
	if (0 == dir_exist(DIR_HOSTINFO))
	{
		createdir(DIR_HOSTINFO);
	}
#endif 

	get_system_info(&HostInfo.SystemInfo);
	get_mem_info(&HostInfo.MemoryInfo);
	get_cpu_info(&HostInfo.CpuInfo);
	if (1 != get_board_info_ex(&HostInfo))
	{
		get_bios_info(&HostInfo.BiosInfo);
		get_board_info(&HostInfo.BoardInfo);
	}
	get_hard_disk_info(&HostInfo.HarddiskInfo);
	get_hard_disk_size(&HostInfo.HarddiskInfo);
	get_ifinfo(&HostInfo);
	HBLA_LOGGER(HBLA_LOG_INFO, "System:\n cpOSVerInfo=[%s]\n cpKernelInfo=[%s]\n cpCompiled=[%s]\n cpComputerName=[%s]\n cpUserName=[%s]\n "
					"cpSysInstTime=[%s]\n cpGroupName=[%s]\n cpMemTotal=[%s]\n cpProcessorName=[%s]\n cpVendor=[%s]\n",
					HostInfo.SystemInfo.cpOSVerInfo, HostInfo.SystemInfo.cpKernelInfo, HostInfo.SystemInfo.cpCompiled, HostInfo.SystemInfo.cpComputerName, 
					HostInfo.SystemInfo.cpUserName, HostInfo.SystemInfo.cpSysInstTime,	HostInfo.SystemInfo.cpGroupName, HostInfo.MemoryInfo.cpMemTotal,
					HostInfo.CpuInfo.cpProcessorName, HostInfo.CpuInfo.cpVendor);
	HBLA_LOGGER(HBLA_LOG_INFO, "BIOS:\n cpVendor=[%s]\n cpVersion=[%s]\n cpDate=[%s]\n", HostInfo.BiosInfo.cpVendor, HostInfo.BiosInfo.cpVersion, HostInfo.BiosInfo.cpDate);
	HBLA_LOGGER(HBLA_LOG_INFO, "Board:\n cpName=[%s]\n cpVendor=[%s]\n", HostInfo.BoardInfo.cpName, HostInfo.BoardInfo.cpVendor);
	HBLA_LOGGER(HBLA_LOG_INFO, "Hard:\n cpModelModel=[%s]\n cpSerial=[%s]\n cpHardDiskSize=[%s]\n", HostInfo.HarddiskInfo.cpModelModel, HostInfo.HarddiskInfo.cpSerial, HostInfo.HarddiskInfo.cpHardDiskSize);
	int i = 0;
	for(i; i < HostInfo.interfaceNum && i < 6; i++)
	{
		HBLA_LOGGER(HBLA_LOG_INFO, "INTERFACE:\n ifnum:%d\n cpInterName=%s\n cpIp=%s\n cpBroadIp=%s\n cpNetMask=%s\n cpInterMac=%s\n iMtu=%d",i,
						HostInfo.ifInfo[i].cpInterName,
						HostInfo.ifInfo[i].cpIp,
						HostInfo.ifInfo[i].cpBroadIp,
						HostInfo.ifInfo[i].cpNetMask,
						HostInfo.ifInfo[i].cpInterMac,
						HostInfo.ifInfo[i].iMtu);
	}
	return &HostInfo;
}

static char cpClientID[32];
/**
 * get hostId -- clientID
 */
char *hbla_hostinfo_getClientID(HOSTINFO * hi)
{
	int iFillNum = 0;
	int i, iLen;

	if(hi == NULL)
		return NULL;

	if(strlen(hi->ifInfo[0].cpInterMac) > 16)
	{
		for(i = 0; i < 17; i ++)
		{
			if((i%3) == 2)
				continue;
			cpClientID[iFillNum++] = hi->ifInfo[0].cpInterMac[i];
		}	
	}

	iLen = strlen(hi->HarddiskInfo.cpSerial);
	if(iLen > 0)
	{
		for(i = 0; i < iLen && iFillNum < 31; i++)
		{
			char tem = hi->HarddiskInfo.cpSerial[i];
			if((tem >= '0' && tem <= '9') || 
				(tem <= 'Z' && tem >= 'A') ||
				(tem <= 'z' && tem >= 'a'))
			{
				cpClientID[iFillNum++] = tem;
			}
		}
	}

	while(iFillNum < 31)
	{
		cpClientID[iFillNum++] = 'h';
	}

	return cpClientID;
}

static char cpCliInfoMD5[32];
/**
 * get hostifnoMd5 -- get clientinfoMD5 in char
 */
char * hbla_hostinfo_getCliInfoMD5(HOSTINFO *hi)
{
	MD5_CTX ctx;
	unsigned char outmd[16];
	int i;
	int iFillNum = 0;

	if(NULL == hi)
		return NULL;
	MD5_Init(&ctx);
	MD5_Update(&ctx, hi, sizeof(HOSTINFO));
	MD5_Final(outmd,&ctx);
	for(i = 0; i < 16; i++)
	{
		sprintf(cpCliInfoMD5 + iFillNum,"%02x",outmd[i]);
		iFillNum += 2;
	}
	
	return cpCliInfoMD5;
}

/**
 * get clientIp
 */
char * hbla_hostinfo_getClientIP()
{
	if(HostInfo.interfaceNum != 0)
		return HostInfo.ifInfo[0].cpIp;
	return NULL;
}

/**
 * get hostName
 */
char * hbla_hostinfo_getHostName()
{
	return HostInfo.SystemInfo.cpComputerName;
}
