all:sqlhelp_test.c
	gcc sqlhelp_test.c -g -o sqltest ../libhbla_sqlhelp.a ../../../libs/libsqlcipher.a ../../../libs/libcrypto.a -I ../../../includes/ -ldl -lpthread
clean:
	rm -f sqltest
