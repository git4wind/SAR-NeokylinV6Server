objects = hbla_hostinfo.o
inc  = -I ../../includes/
all:$(objects)
	gcc -shared $^ -g -o libhbla_hostinfo.so -fPIC
	cp -f libhbla_hostinfo.so ../../libs/
	cp -f hbla_hostinfo.h ../../includes/
	cp -f libhbla_hostinfo.so /usr/lib/
$(objects):%.o:%.c
	gcc -g -c $< -o $@ $(inc) -fPIC
clean:
	rm -f $(objects) libhbla_hostinfo.so ../../libs/libhbla_hostinfo.so ../../includes/hbla_hostinfo.h
