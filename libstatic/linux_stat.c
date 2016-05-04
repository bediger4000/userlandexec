#include <libstatic.h>
//       int stat(const char *path, struct stat *buf);

int
linux_stat(const char *path, void *buf)
{
	long ret;
	asm volatile ("syscall" :
		"=a" (ret) :
		"a" (4), "D" (path), "S" (buf) :
		"memory"
	);
	if (ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	return (int)ret;
}
