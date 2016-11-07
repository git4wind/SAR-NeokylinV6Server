objects = hbla_sql.o hbla_sqlhelp.o 
libs = -L ../../libs/ -sqlcipher -lpthread
inc = -I ../../includes/
all:$(objects)
	ar -rsv libhbla_sqlhelp.a $(objects) hbla_sql.h hbla_sqlhelp.h
	cp -f libhbla_sqlhelp.a ../../libs/
	cp -f hbla_sqlhelp.h ../../includes/
$(objects):%.o:%.c
	gcc -c -g $^  -o $@ $(inc) -fPIC
clean:
	rm -f *.o *.a ../../libs/libhbla_sqlhelp.a ../../includes/hbla_sqlhelp.h
