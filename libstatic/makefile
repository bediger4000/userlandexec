CC = gcc
CFLAGS = -fPIC -nostdlib -I.

OBJS = linux_close.o linux_exit.o linux_open.o linux_read.o linux_write.o \
	linux_mmap.o linux_munmap.o linux_stat.o memcpy.o \
	print.o errno.o strtoul.o file_size.o string.o linux_mprotect.o \
	linux_sbrk.o

all: libstatic.a

libstatic.a: $(OBJS)
	ar rcs libstatic.a $(OBJS)

linux_close.o: linux_close.c libstatic.h
linux_exit.o: linux_exit.c libstatic.h
linux_open.o: linux_open.c libstatic.h
linux_read.o: linux_read.c libstatic.h
linux_write.o: linux_write.c libstatic.h
linux_stat.o: linux_stat.c libstatic.h
linux_sbrk.o: linux_sbrk.c libstatic.h
linux_mmap.o: linux_mmap.c 
linux_mprotect.o: linux_mprotect.c  libstatic.h
linux_munmap.o: linux_munmap.c libstatic.h
print.o: print.c libstatic.h
errno.o: errno.c libstatic.h
strtoul.o: strtoul.c libstatic.h
file_size.o: file_size.c libstatic.h
string.o: string.c libstatic.h
memcpy.o: memcpy.c libstatic.h

clean:
	-rm -rf *.o *.a *core
