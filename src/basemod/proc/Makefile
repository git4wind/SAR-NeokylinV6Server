object = alloc.o  devname.o  escape.o  ksym.o  pwcache.o  readproc.o  sig.o  slab.o  sysinfo.o  version.o  whattime.o
header = alloc.h  devname.h  escape.h  pwcache.h  readproc.h  sig.h  slab.h  sysinfo.h  version.h  whattime.h
object_test=alloc.o  devname.o  escape.o  ksym.o  pwcache.o  readproc.o  sig.o  slab.o  sysinfo.o  version.o  whattime.o proc_test.o
%.o : %.c
	gcc -g -c -o $@ $<

lib:$(object)
	ar -rsv libhbla_proc.a $<
	cp -f libhbla_proc.a ../../libs/
	cp -f *.h ../../includes/
	
test:$(object_test)
	gcc -g $(object_test) -o proc_test

clean:
	rm proc_test *.o libhbla_proc.a ../../libs/libhbla_proc.a
