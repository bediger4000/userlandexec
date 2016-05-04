#include <sys/syscall.h>
#include <libstatic.h>
int
linux_read(int fd, char *buffer, unsigned long bufferlen)
{

	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_read),
		      "D" (fd), "S" (buffer), "d" (bufferlen) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int)ret;
}
