objects = hbla_log.o

all:$(objects)
	ar -rsv libhbla_log.a $< hbla_log.h
	cp -f libhbla_log.a ../../libs/
	cp -f hbla_log.h ../../includes/
$(objects):%.o:%.c
	gcc -c -fPIC $< -o $@

clean:
	rm -f hbla_log.o libhbla_log.a ../../libs/libhbla_log.a ../../includes/hbla_log.h
