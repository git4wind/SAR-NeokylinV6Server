objects = hbla_main.o hbla_auth.o
inc = -I ../includes/
libs = -L ../libs/ -lhbla_log -lhbla_conf -lhbla_mod -lhbla_hostinfo  -lcrypto -lhbla_sslcon -lhbla_sqlhelp -lsqlcipher -pthread -lusb
ISDEBUG = -g
all:$(objects)
	gcc $(objects) $(ISDEBUG) -o hbla $(libs) $(inc) -ldl

$(objects):%.o:%.c
	gcc -c $(ISDEBUG) $< -o $@ $(inc) $(libs) -ldl

clean:
	rm -f hbla_main.o hbla
