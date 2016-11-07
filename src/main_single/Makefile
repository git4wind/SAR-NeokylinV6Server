objects = hbla_main.o
inc = -I ../includes/
libs = -L ../libs/ -lhbla_log -lhbla_conf -lhbla_hostinfo -lhbla_mod  -lhbla_sqlhelp -lsqlcipher -pthread -lcrypto -ldl
ISDEBUG = -g

all:$(objects)
	gcc $(objects) $(ISDEBUG) -o hbla $(libs) $(inc)

$(objects):%.o:%.c
	gcc -c $(ISDEBUG) $< -o $@ $(inc)

clean:
	rm -f hbla_main.o hbla
