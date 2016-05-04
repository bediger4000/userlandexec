#include <sys/syscall.h>
#include <libstatic.h>
int
linux_munmap(void *start, unsigned long length)
{

	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_munmap),
		      "D" (start), "S" (length) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int)ret;
}
