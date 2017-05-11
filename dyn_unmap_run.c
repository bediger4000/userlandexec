#include <libstatic/libstatic.h>

#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ulexec.h>

#include <libstatic/crt.h>

#define JMP_ADDR(x) asm("\tjmp  *%0\n" :: "r" (x))
#define SET_STACK(x) asm("\tmovq %0, %%rsp\n" :: "r"(x))

void
print_maps(void)
{
	char rbuf[1024];
	int fd, cc;

	fd = linux_open("/proc/self/maps", 0, 0);
	while (0 < (cc = linux_read(fd, rbuf, sizeof(rbuf))))
		linux_write(1, rbuf, cc);
	linux_close(fd);
}

void c_main(int ac, char **av, char **env)
{
	char *file_to_map = av[3];
	char *file_to_unmap = av[2];
	int how_to_map = 0;
	void *mapped;
	void *entry_point;
	unsigned long dummy;
	Elf64_Ehdr *elf_ehdr, *ldso_ehdr;
	struct saved_block *argvb, *envb, *elfauxvb;
	int trim_args;
	void *stack_bottom;
	unsigned long empty[32768];

	empty[0] = 1;

	if (NULL == strstr(av[1], "dyn_unmap_run"))
	{
		file_to_map = av[1];
		file_to_unmap = NULL;
		how_to_map = 1;
		trim_args = 1;
	} else {
		file_to_map = av[2];
		file_to_unmap = av[0];
		how_to_map = 0;
		trim_args = 2;
	}

	if (file_to_unmap)
		unmap(file_to_unmap);

	mapped = map_file(file_to_map, &dummy);
	elf_ehdr = (Elf64_Ehdr *)mapped;

	entry_point = load_elf(mapped, how_to_map, &elf_ehdr, &ldso_ehdr);

	linux_munmap(mapped, dummy);

	argvb = save_argv(ac - trim_args, &av[trim_args]);
	envb = save_argv(0, env);
	elfauxvb = save_elfauxv(env);

	stack_bottom = stack_setup(argvb, envb, elfauxvb, elf_ehdr, ldso_ehdr);

	SET_STACK(stack_bottom);
	JMP_ADDR(entry_point);
}

void
error_msg(char *msg)
{
	char buf[32];
	print_string(1, msg);
	print_string(1, " ");
	to_decimal(errno, buf);
	print_string(1, buf);
	print_string(1, "\n");
}

void
print_address(char *phrase, void *address)
{
	char buf[256];
	to_hex((unsigned long)address, buf);
	print_string(1, phrase);
	print_string(1, " 0x");
	print_string(1, buf);
	print_string(1, "\n");
}

void
unmap(char *progname)
{
	char buf[1024], *p;
	char rbuf[2048];
	int cc, fd;

	fd = linux_open("/proc/self/maps", 0, 0);

	p = &buf[0];

	while (0 < (cc = linux_read(fd, rbuf, sizeof(rbuf))))
	{
		int i;

		for (i = 0; i < cc; ++i)
		{
			int c = rbuf[i];

			if ('\n' != c)
				*p++ = c;
			else {
				*p = '\0';
				/* When a line from /proc/self/maps shows up as having been
				 * mapped in from this running program, ld.so or libc, unmap it.
				 * This will keep the exec'd program's address space a lot
				 * cleaner.  But even a 32-bit address space can hold 2 copies
				 * of glibc without ill effects, so you don't really have to
				 * munmap() anything other than the program calling ul_exec() */
				if (strstr(buf, progname) || strstr(buf, "libdl") || strstr(buf, "/usr/lib/ld-")
					|| strstr(buf, "/lib64/ld-") || strstr(buf, "libc"))
				{
					char *u;
					char *first, *second;
					unsigned long low, high;

					u = strchr(buf, ' ');
					*u = '\0';

					first = buf;

					second = strchr(first, '-');
					*second = '\0';
					++second;

					low = strtoul(first, NULL, 0x10);
					high = strtoul(second, NULL, 0x10);

					linux_munmap((void *)low, high-low);
				}

				p = &buf[0];
			}
		}
	}

	linux_close(fd);
}

/*
void *
memcpy(void *dest, const void *src, unsigned long n)
{
	unsigned long i;
	unsigned char *d = (unsigned char *)dest;
	unsigned char *s = (unsigned char *)src;

	for (i = 0; i < n; ++i)
		d[i] = s[i];

	return dest;
}
*/
