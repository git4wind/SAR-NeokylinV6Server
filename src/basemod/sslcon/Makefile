objects = hbla_ssl.o hbla_sslcon.o
libs =  -L ../../libs/ -ldl -lpthread -lssl -lcrypto
all:$(objects)
	gcc -shared $^ -g -o libhbla_sslcon.so $(libs) -fPIC
	cp -f libhbla_sslcon.so ../../libs/
	cp -f hbla_sslcon.h ../../includes/
	cp -f libhbla_sslcon.so /usr/lib/
$(objects):%.o:%.c
	gcc -g -c $< -o $@ $(libs) -fPIC

clean:
	rm -f $(objects) libhbla_sslcon.so ../../libs/libhbla_sslcon.so ../../includes/hala_sslcon.h
