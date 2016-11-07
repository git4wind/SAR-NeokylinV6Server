objects = hbla_conf.o

all:$(objects)
	ar -rsv libhbla_conf.a $< hbla_conf.h
	cp -f libhbla_conf.a ../../libs/
	cp -f hbla_conf.h ../../includes/
$(objects):%.o:%.c
	gcc -g -c $(ISDEBUG) $< -o $@  -I ../../includes/

clean:
	rm -f hbla_conf.o libhbla_conf.a ../../libs/libhbla_conf.a ../../includes/hbla_conf.h
