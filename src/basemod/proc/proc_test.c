#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "readproc.h"
#include "sysinfo.h"


int main()
{
	/*PROCTAB* proc = openproc(PROC_FILLMEM 
		| PROC_FILLCOM
		| PROC_FILLENV
		| PROC_FILLUSR
		| PROC_FILLGRP
		| PROC_FILLWCHAN
		| PROC_FILLARG
		| PROC_FILLSTAT 
		| PROC_FILLSTATUS);

	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));
	while (readproc(proc, &proc_info) != NULL) {
		 printf("%20s:\t%5ld\t%5lld\t%5lld\n",
	         proc_info.cmd, proc_info.resident,
        	 proc_info.utime, proc_info.stime);
	}
	closeproc(proc);*/
	PROCTAB* proc = openproc( PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
 
	 proc_t proc_info;
 
 // zero out the allocated proc_info memory
   memset(&proc_info, 0, sizeof(proc_info));
    
 	while (readproc(proc, &proc_info) != NULL) {
		
	printf("%-10d %-10d ", proc_info.tid, proc_info.ppid);
		if (proc_info.cmdline != NULL) {
            	//print full cmd line if available
			printf("%s\n", *proc_info.cmdline);
		} else {
                    // if no cmd line use executable filename 
                      printf("[%s]\n", proc_info.cmd);
                        }                         
	
	}
                           
        closeproc(proc);
	return 1;
}
