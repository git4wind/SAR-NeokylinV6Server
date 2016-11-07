/**
 * file hbla_hostinfo.h
 * author: dyk
 * desc: get the linux host info
 * 		change by pc 2015-10-20
 */
#ifndef _HBLA_HOSTINFO_H_
#define _HBLA_HOSTINFO_H_


//系统信息
typedef struct tagSystemInfo{
	char cpOSVerInfo[128];		//操作系统
	char cpCompiled[64];		//编译时间
	char cpKernelInfo[128];		//内核信息
	char cpSysInstTime[24];		//系统安装时间
	char cpComputerName[64];	//计算机名称	
	char cpUserName[64];		//用户名
	char cpGroupName[64];		//用户所属域名
}SYSTEMINFO,*PSYSTEMINFO;

//BIOS信息
typedef struct tagBiosInfo{
	char  cpDate[32];			//BIOS日期 
	char  cpVendor[128];		//BIOS厂商
	char  cpVersion[64];		//BIOS版本
}BIOSINFO, *PBIOSINFO;

//硬盘信息
#define HARDID_NUM		64
typedef struct tagHardDiskInfo{
	char cpModelModel[HARDID_NUM];	//硬盘型号
	char cpSerial[HARDID_NUM];			//硬盘序列号
	char cpHardDiskSize[HARDID_NUM];	//硬盘大小
}HARDDISKINFO,*PHARDDISKINFO;

//CPU信息
typedef struct tagCPUInfo{
	char cpVendor[64];				//生产商
	char cpProcessorName[128];		//处理器名称
}CPUINFO,*PCPUINFO;

//主板信息
typedef struct tagBoardInfo{
	char  cpName[128];			//主板名称
	char  cpVendor[128];		//主板厂商
}BOARDINFO, *PBOARDINFO;

//内存信息
#define MEM_LEN	64
typedef struct tagMemoryInfo{
	char cpMemTotal[MEM_LEN];	//内存总大小
}MEMORYINFO,*PMEMORYINFO;

/* interface info add by pc 2015-10-20*/
typedef struct tagInterfaceInfo{
	char cpInterName[64];
	char cpIp[24];
	char cpBroadIp[24];
	char cpNetMask[24];
	char cpInterMac[24];
	int iMtu;
}INTERFACEINFO;

//主机信息扩展结构
typedef struct _tagHostInfo{
	unsigned int nSize;			//_tagHostInfo 大小
	SYSTEMINFO	SystemInfo;		//系统信息
	BIOSINFO BiosInfo;			//BIOS信息
	HARDDISKINFO HarddiskInfo;	//硬盘信息
	MEMORYINFO	MemoryInfo;		//内存信息
	CPUINFO CpuInfo;			//CPU信息
	BOARDINFO BoardInfo;		//主板信息
	/* interface add by pc 2015-10-20 default MAX interface nulber is six*/
	int interfaceNum;
	INTERFACEINFO ifInfo[6];
}HOSTINFO,*PHOSTINFO;

/**
 * get the hostinfo
 */
extern HOSTINFO *hbla_hostinfo_get();
extern int get_board_info(PBOARDINFO pBoardInfo);
extern int get_hard_disk_info(PHARDDISKINFO pHardDiskInfo);
extern int get_cpu_info(PCPUINFO pCpuInfo);
extern void get_mem_info(PMEMORYINFO pMemInfo);
/**
 * get hostId -- clientID
 */
char *hbla_hostinfo_getClientID(HOSTINFO * hi);
/**
 * get hostifnoMd5 -- get clientinfoMD5 in char
 */
char * hbla_hostinfo_getCliInfoMD5(HOSTINFO *hi);

/**
 * get clientIp
 */
char * hbla_hostinfo_getClientIP();
/**
 * get hostName
 */
char * hbla_hostinfo_getHostName();
#endif
