objects = sslcon_test.o
sLib = ../libhbla_sslcon.a 
#../../libs/libcrypto.a ../../libs/libssl.a
dlib = ../../libs/libssl.a ../../libs/libcrypto.a -L ../
slibs = -ldl -lpthread
dlibs = -ldl -lpthread -lbla_sslcon 
inc = -I ../

all:$(objects)
	gcc -g $^  -o test -ldl -lpthread -lssl -lcrypto -lhbla_sslcon

$(objects):%.o:%.c
	gcc -c -g $< -o $@ $(inc)
clean:
	rm -f *.o test
