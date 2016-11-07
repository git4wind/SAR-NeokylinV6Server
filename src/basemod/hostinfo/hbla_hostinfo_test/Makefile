objects = hostinfo_test.o
inc = -I ../

all:$(objects)
	gcc -g $^ -o test -ldl -lhbla_hostinfo -L ../../../libs/ -lhbla_log -lssl -lcrypto

$(objects):%.o:%.c
	gcc -c -g $< -o $@ $(inc)

clean:
	rm -f *.o test
