#include <sys/syscall.h>
#include <libstatic.h>

int
linux_write(int fd, const void *data, unsigned long len)
{

	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_write),
		      "D" (fd), "S" (data), "d" (len) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int)ret;
}
