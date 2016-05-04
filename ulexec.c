#include <libstatic/libstatic.h>

#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ulexec.h>

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

void
ulexec(int ac, char **av, char **env)
{
	char *file_to_map = av[3];
	char *file_to_unmap = av[2];
	int how_to_map = 0;
	void *mapped;
	void *entry_point;
	struct stat sb;
	Elf64_Ehdr *elf_ehdr, *ldso_ehdr;
	Elf64_Phdr *phdr;
	struct saved_block *argvb, *envb, *elfauxvb;
	int trim_args, i;
	void *stack_bottom;

	file_to_map = av[2];
	file_to_unmap = av[0];
	how_to_map = 0;
	trim_args = 2;

	if (file_to_unmap)
		unmap(file_to_unmap);

	mapped = map_file(file_to_map);
	elf_ehdr = (Elf64_Ehdr *)mapped;

	phdr = (Elf64_Phdr *)((unsigned long)elf_ehdr + elf_ehdr->e_phoff);

	for (i = 0; i < elf_ehdr->e_phnum; ++i)
		if (phdr[i].p_type == PT_LOAD && phdr[i].p_vaddr == 0)
		{
			how_to_map = 1;  /* map it anywhere, like ld.so, or PIC code. */
			break;
		}

	entry_point = load_elf(mapped, how_to_map, &elf_ehdr, &ldso_ehdr);

	linux_munmap(mapped, sb.st_size);

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
