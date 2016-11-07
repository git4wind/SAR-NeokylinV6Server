/**
 * author:pc
 * time:2015-10-08
 * copyright:hba.cn
 * version:1.0.0
 * file:hbla_conf.c
 * desc:
 * 	read the conf.ini & set the global struct of g_hbla_conf_info
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "hbla_conf.h"
#include "hbla_log.h"

HBLA_CONF_INFO g_hbla_conf_info;	/*global info in conf.ini*/

static char hbla_conf_fileName[] = "conf/conf.ini";	/*config file name*/
static char hbla_data_fileName[] = "conf/hbla.db";	/*config file name*/
static char hbla_libs_fileName[] = "libs/";	/*config file name*/
static char hbla_skey_fileName[] = "conf/server_cert.pem";	/*config file name*/
static char hbla_ccert_fileName[] = "conf/client_crt.pem";	/*config file name*/
static char hbla_ckey_fileName[] = "conf/client_key.pem";	/*config file name*/

/**
 * author:pc
 * time:2015-10-08 16:00
 * desc:translate the level from integer of enum to char[]
 */
static int hbla_conf_getPaths(char* cpLocalPaths,char* cpConfPaths, 
				char *cpDataPaths, char * cpLibsPaths,
				char *cpSKeyPaths, char *cpCKeyPaths,
				char *cpCCertPaths)
{  
    char pidfile[64];  
    int bytes;  
    int fd;  
  
    sprintf(pidfile, "/proc/%d/cmdline", getpid());  
  
    fd = open(pidfile, O_RDONLY, 0);  
    bytes = read(fd, cpConfPaths, 256);  
    close(fd);  
    cpLocalPaths[255] = '\0';
	cpConfPaths[255] = '\0';
  
    char * p = &cpConfPaths[strlen(cpConfPaths)];  
    do   
    {  
        *p = '\0';  
        p--;  
    } while( '/' != *p );  
  
	/* set localPaths */
	memcpy(cpLocalPaths, cpConfPaths, strlen(cpConfPaths));
	p++;
	memcpy(p, hbla_conf_fileName, strlen(hbla_conf_fileName));
	/* set datapasths */
	memcpy(cpDataPaths, cpLocalPaths, strlen(cpLocalPaths));
	p = &cpDataPaths[strlen(cpDataPaths)];
	memcpy(p, hbla_data_fileName, strlen(hbla_data_fileName));
	/* set libspasths */
	memcpy(cpLibsPaths, cpLocalPaths, strlen(cpLocalPaths));
	p = &cpLibsPaths[strlen(cpLibsPaths)];
	memcpy(p, hbla_libs_fileName, strlen(hbla_libs_fileName));
	/* set serverkeyPaths */
	memcpy(cpSKeyPaths, cpLocalPaths, strlen(cpLocalPaths));
	p = &cpSKeyPaths[strlen(cpSKeyPaths)];
	memcpy(p, hbla_skey_fileName, strlen(hbla_skey_fileName));
	/* set clientKeyPaths */
	memcpy(cpCKeyPaths, cpLocalPaths, strlen(cpLocalPaths));
	p = &cpCKeyPaths[strlen(cpCKeyPaths)];
	memcpy(p, hbla_ckey_fileName, strlen(hbla_ckey_fileName));
	memcpy(cpCCertPaths, cpLocalPaths, strlen(cpLocalPaths));
	p = &cpCCertPaths[strlen(cpCCertPaths)];
	memcpy(p, hbla_ccert_fileName, strlen(hbla_ckey_fileName));

    return 0;  
} 

/*
 * author:pc
 * time:2015-10-08 16:28
 * desc:trim the string
 */
static int hbla_conf_lTrim(char *str)
{
	int i = 0;
	int j = 0;

	if(str == NULL)
		return 0;
	int len = strlen(str);

	if(0 >= len)
	{
		return 0;
	}

	for(i=0; i < len; i++)
	{
		if(' ' == str[i])
		{
			j++;
		}else
		{
			break;
		}
	}
	for(i=0; i < len-j;i++)
	{
		str[i] = str[i+j];
	}
	str[i] = '\0';

	return 1;
}

static int hbla_conf_rTrim(char *str)
{
	int i = 0;
	if(str == NULL)
		return 0;
	int len = strlen(str);

	if(0 >= len)
	{
		return 0;
	}
	i = len;
	while(' ' == str[--i]);
	str[i+1] = '\0';

	return 1;
}

static int hbla_conf_trim(char *str)
{
	if(hbla_conf_lTrim(str) && hbla_conf_rTrim(str))
		return 1;
	return 0;
}

/**
 * author:pc
 * time:2015-10-08 16:00
 * desc:translate the level from integer of enum to char[]
 */
static char *hbla_conf_getString(char *title, char *key, char *def, char *filename)
{   
    FILE *fp;   
    char szLine[1024];  
    static char tmpstr[1024];  
    int rtnval;  
    int i = 0;   
    int flag = 0;   
    char *tmp;  
  
    if((fp = fopen(filename, "r")) == NULL)   
    {   
        printf("have   no   such   file \n");  
        return "";   
    }  
    while(!feof(fp))   
    {   
        rtnval = fgetc(fp);   
        if(rtnval == EOF)   
        {   
            break;   
        }   
        else   
        {   
            szLine[i++] = rtnval;   
        }   
        if(rtnval == '\n')   
        {   
            szLine[--i] = '\0';
            i = 0;   
            tmp = strchr(szLine, '=');   
  
            if(( tmp != NULL )&&(flag == 1))   
            {   
                if(strstr(szLine,key)!=NULL)   
                {   
                    if ('#' == szLine[0])  
                    {  
                    }  
                    else if ( '/' == szLine[0] && '/' == szLine[1] )  
                    {  
                          
                    }  
                    else  
                    {  
                        strcpy(tmpstr,tmp+1);   
                        fclose(fp);  
						if(hbla_conf_trim(tmpstr))
                        	return tmpstr;
						return NULL;
                    }  
                }   
            }  
            else   
            {   
                strcpy(tmpstr,"[");   
                strcat(tmpstr,title);   
                strcat(tmpstr,"]");  
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 )   
                {  
                    flag = 1;   
                }  
            }  
        }  
    }  
    fclose(fp);   

	return def;   
}

static int hbla_conf_getInt(char *title,char *key,int def,char *filename)  
{  
	char * temStr;
	temStr = hbla_conf_getString(title, key, NULL, filename);
	if(temStr == NULL)
		return def;
    return atoi(temStr);  
}

/**
 * author:pc
 * time:2015-10-08
 * desc: extern function to read the config info
 */
int hbla_conf_getConfInfo()
{
	char *cpTem;
	if(hbla_conf_getPaths(g_hbla_conf_info.cpLocalPaths, 
					g_hbla_conf_info.cpConfPaths,
					g_hbla_conf_info.cpDataPaths,
					g_hbla_conf_info.cpLibsPaths,
					g_hbla_conf_info.cpSKeyPaths,
					g_hbla_conf_info.cpCKeyPaths,
					g_hbla_conf_info.cpCCertPaths))
	{
		HBLA_LOGGER(HBLA_LOG_ERROR, "get paths error!");
		return -1;
	}
	HBLA_LOGGER(HBLA_LOG_INFO,"get localPaths = %s",g_hbla_conf_info.cpLocalPaths);
	HBLA_LOGGER(HBLA_LOG_INFO,"get ConfigPaths = %s",g_hbla_conf_info.cpConfPaths);
	//HBLA_LOGGER(HBLA_LOG_INFO,"get DBPaths = %s",g_hbla_conf_info.cpDataPaths);
	/* get center ip */
	cpTem = hbla_conf_getString("CONNECT", "CENTERIP", NULL, g_hbla_conf_info.cpConfPaths);
	if(cpTem == NULL)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR, "get center ip error!");
		return -1;
	}
	strncpy(g_hbla_conf_info.cpCenterIp, cpTem, strlen(cpTem));
	HBLA_LOGGER(HBLA_LOG_INFO,"get CenterAddr = %s",g_hbla_conf_info.cpCenterIp);
	/* get center Port */
	g_hbla_conf_info.iCenterPort = hbla_conf_getInt("CONNECT","CENTERPORT",45451,g_hbla_conf_info.cpConfPaths);
	HBLA_LOGGER(HBLA_LOG_INFO,"get CenterPort = %d",g_hbla_conf_info.iCenterPort);

	/* get deptName */
	cpTem = hbla_conf_getString("BASEINFO", "DEPTNAME", NULL, g_hbla_conf_info.cpConfPaths);
	if(cpTem == NULL)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR, "get deptName error!");
		return -1;
	}
	strncpy(g_hbla_conf_info.cpDeptName, cpTem, strlen(cpTem));
	HBLA_LOGGER(HBLA_LOG_INFO,"get deptName = %s",g_hbla_conf_info.cpDeptName);

	/* get userName */
	cpTem = hbla_conf_getString("BASEINFO", "USERNAME", NULL, g_hbla_conf_info.cpConfPaths);
	if(cpTem == NULL)
	{
		HBLA_LOGGER(HBLA_LOG_ERROR, "get userName error!");
		return -1;
	}
	strncpy(g_hbla_conf_info.cpUserName, cpTem, strlen(cpTem));
	HBLA_LOGGER(HBLA_LOG_INFO,"get userName = %s",g_hbla_conf_info.cpUserName);
	return 0;
}
static char conf_time[24];
/**
 * get the localTime
 */
char * hbla_conf_getLocalTime()
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep);
	memset(conf_time,0,24);
	sprintf(conf_time,"%d/%d/%d %d:%d:%d",
					(1900 +p->tm_year),
					(1 + p->tm_mon),
					p->tm_mday,
					p->tm_hour,
					p->tm_min,
					p->tm_sec);
	return conf_time;
}
