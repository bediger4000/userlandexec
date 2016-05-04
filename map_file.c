#include <libstatic/libstatic.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <elf.h>
#include <ulexec.h>

void *
map_file(char *file_to_map)
{
	struct stat sb;
	void *mapped;

	if (0 > linux_stat(file_to_map, &sb))
	{
		error_msg("map_file stat() failed ");
		linux_exit(1);
	}

	mapped = linux_mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	if (mapped == (void *)-1)
	{
		error_msg("map_file mmap() failed ");
		linux_exit(1);
	}

	copy_in(file_to_map, mapped);

	return mapped;
}

void
copy_in(char *filename, void *address)
{
	int fd, cc;
	off_t offset = 0;
	char buf[1024];

	if (0 > (fd = linux_open(filename, 0, 0)))
	{
		error_msg("opening dynamically-loaded file failed");
		linux_exit(2);
	}

	while (0 < (cc = linux_read(fd, buf, sizeof(buf))))
	{
		memcpy((address + offset), buf, cc);
		offset += cc;
	}

	linux_close(fd);
}
