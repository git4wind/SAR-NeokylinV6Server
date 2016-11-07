#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"


int main(void){
	sqlite3 *db;
	int ret = sqlite3_open("hbla.db", &db);
	printf("ret = %d\n", ret);
//	ret = sqlite3_key(db, "pclvmm", 6);
//	printf("ret = %d\n", ret);
	ret = sqlite3_rekey(db, NULL, 0);
	printf("ret = %d\n", ret);
	ret = sqlite3_close(db);
	printf("ret = %d\n", ret);
	return 0;
}
