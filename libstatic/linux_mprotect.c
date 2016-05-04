#include <sys/syscall.h>
#include <libstatic.h>

int
linux_mprotect(void *addr, unsigned long len, int prot)
{

	long ret;
	asm volatile ("syscall" : "=a" (ret) : "a" (__NR_mprotect),
		      "D" (addr), "S" (len), "d" (prot) :
		      "cc", "memory", "rcx",
		      "r8", "r9", "r10", "r11" );
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int) ret;
}
