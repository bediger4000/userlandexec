CC = gcc
CFLAGS = -I. -g -Wall -Wextra -std=gnu99 -nostdlib -fPIC

all: dyn_unmap_run hw env_test margs args2 elfauxv elfauxv_dynamic \
	example ulexec.so mycat

mycat: mycat.c libstatic/libstatic.h libstatic/crt.h libstatic/libstatic.a
	gcc -I. -g -Wall -std=gnu99 -nostdlib -c  mycat.c
	gcc -I. -g -std=gnu99 -nostdlib \
		mycat.o -o mycat  -Llibstatic -lstatic

dyn_unmap_run: dyn_unmap_run.c load_elf.o map_file.o stack_fix.o ulexec.h libstatic/libstatic.h libstatic/crt.h libstatic/libstatic.a
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC   -c  dyn_unmap_run.c
	gcc -I. -g -std=gnu99 -nostdlib \
		dyn_unmap_run.o load_elf.o map_file.o stack_fix.o -o dyn_unmap_run -Llibstatic -lstatic

ulexec.so: ulexec.c load_elf.o map_file.o stack_fix.o ulexec.h  unmap.o \
		libstatic/libstatic.h libstatic/crt.h libstatic/libstatic.a
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC   -c  ulexec.c
	gcc -fPIC -shared -I. -g -std=gnu99 -nostdlib \
		ulexec.o load_elf.o map_file.o unmap.o stack_fix.o -o ulexec.so -Llibstatic -lstatic

env_test: env_test.o libstatic/libstatic.a
	gcc -g -I. -std=gnu99 -nostdlib -fPIC \
		env_test.o -o env_test \
		-Llibstatic -lstatic
	chmod ugo-x env_test

places: places.o libstatic/libstatic.a
	gcc -g -I. -std=gnu99 -nostdlib -fPIC \
		places.o -o places \
		-Llibstatic -lstatic
	chmod ugo-x places

margs: margs.o libstatic/libstatic.a
	gcc -g -I. -std=gnu99 -nostdlib -fPIC \
		margs.o -o margs \
		-Llibstatic -lstatic
	chmod ugo-x margs

hw: hw.c libstatic/libstatic.a libstatic/crt.h
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC   -c hw.c
	gcc -I. -g -std=gnu99 -nostdlib \
		hw.o -o hw -Llibstatic -lstatic
	chmod ugo-x hw

raw: raw.c libstatic/libstatic.a 
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC -c raw.c
	gcc -I. -g -std=gnu99 -nostdlib raw.o -o raw -Llibstatic -lstatic

globaltest: globaltest.c libstatic/libstatic.a libstatic/crt.h
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC   -c globaltest.c
	gcc -I. -g -std=gnu99 -nostdlib \
		globaltest.o -o globaltest -Llibstatic -lstatic
	chmod ugo-x globaltest

global2: global2.c libstatic/libstatic.a libstatic/crt.h
	gcc -I. -g -Wall -std=gnu99 -nostdlib -fPIC   -c global2.c
	gcc -I. -g -std=gnu99 -nostdlib \
		global2.o -o global2 -Llibstatic -lstatic
	chmod ugo-x global2

elfauxv: elfauxv.o libstatic/libstatic.a
	gcc -I. -g -std=gnu99 -nostdlib \
		elfauxv.o -o elfauxv -Llibstatic -lstatic
	chmod ugo-x elfauxv

args2: args2.c
	gcc -I. -g -Wall -Wextra -o args2 args2.c
	chmod ugo-x args2

example: example.c
	gcc -I. -g -Wall -Wextra -o example example.c -ldl

elfauxv_dynamic: elfauxv_dynamic.c
	gcc -I. -g -Wall -Wextra -o elfauxv_dynamic elfauxv_dynamic.c
	chmod ugo-x elfauxv_dynamic

libstatic/libstatic.a:
	cd libstatic; make

load_elf.o: load_elf.c ulexec.h libstatic/libstatic.h
map_file.o: map_file.c ulexec.h libstatic/libstatic.h
unmap.o: unmap.c ulexec.h libstatic/libstatic.h
places.o: places.c ulexec.h libstatic/libstatic.h
elfauxv.o: elfauxv.c ulexec.h libstatic/libstatic.h
stack_fix.o: stack_fix.c ulexec.h libstatic/libstatic.h

clean:
	-rm -rf *.o *.a *core 
	-rm -rf margs hw args2 env_test dyn_unmap_run static_dyn_load_run
	-rm -rf elfauxv elfauxv_dynamic hw2 example ulexec.so
	cd libstatic; make clean
