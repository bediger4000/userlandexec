#include <sys/syscall.h>
#include <libstatic.h>

int
linux_open(const char *pathname, unsigned long flags, unsigned long mode)
{

	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_open),
		      "D" (pathname), "S" (flags), "d" (mode) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int) ret;
}
