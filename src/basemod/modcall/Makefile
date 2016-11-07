objects = hbla_mod.o hbla_modlist.o hbla_modcall.o

all:$(objects)
	ar -rsv libhbla_mod.a $(objects)
	cp -f libhbla_mod.a ../../libs/
	cp -f hbla_mod.h ../../includes/
	cp -f hbla_modlist.h ../../includes/
$(objects):%.o:%.c
	gcc -c -g $< -o $@ -I ../../includes/

clean:
	rm -f $(objects) libhbla_mod.a ../../libs/libhbla_mod.a ../../includes/hbla_mod.h ../../includes/hbla_modlist.h

