/* $Id: elfauxv.c,v 1.2 2014/02/12 17:48:03 bediger Exp $ */
#include <libstatic/libstatic.h>
#include <elf.h>

#include <libstatic/crt.h>

typedef unsigned long uint64_t;
#define BUFSIZ 1024
char *printable_aux_type(uint64_t a_val);

void
print_data(
	int i, void *auxvp,
	int a_type, unsigned long a_val
)
{
		print_long(1, (unsigned long)i);
		print_string(1, " ");
		print_hex(1, (unsigned long)auxvp);
		print_string(1, " a_type ");
		print_long(1, (unsigned long)a_type);
		print_string(1, ", ");
		print_string(1, printable_aux_type(a_type));
		print_string(1, ": a_un.a_val ");
		print_hex(1, a_val);
		print_string(1, "\n");

/*
		printf("%d %p a_type %lu, %s: a_un.a_val 0x%lx\n",
			i, auxvp,
			auxvp->a_type, printable_aux_type(auxvp->a_type),
			auxvp->a_un.a_val);
*/
}

char *
printable_aux_type(uint64_t a_val)
{
	char *r = "Unknown";
	switch (a_val)
	{
	case AT_NULL:      r = "End of vector"; break;
	case AT_IGNORE:      r = "Entry should be ignored"; break;
	case AT_EXECFD:      r = "File descriptor of program"; break;
	case AT_PHDR:      r = "Program headers for program"; break;
	case AT_PHENT:      r = "Size of program header entry"; break;
	case AT_PHNUM:      r = "Number of program headers"; break;
	case AT_PAGESZ:      r = "System page size"; break;
	case AT_BASE:      r = "Base address of interpreter"; break;
	case AT_FLAGS:      r = "Flags"; break;
	case AT_ENTRY:      r = "Entry point of program"; break;
	case AT_NOTELF:     r = "Program is not ELF"; break;
	case AT_UID:     r = "Real uid"; break;
	case AT_EUID:     r = "Effective uid"; break;
	case AT_GID:     r = "Real gid"; break;
	case AT_EGID:     r = "Effective gid"; break;
	case AT_CLKTCK:     r = "Frequency of times()"; break;
	case AT_PLATFORM:     r = "String identifying platform. "; break;
	case AT_HWCAP:     r = "Machine dependent hints about processor capabilities. "; break;
	case AT_FPUCW:     r = "Used FPU control word. "; break;
	case AT_DCACHEBSIZE:     r = "Data cache block size. "; break;
	case AT_ICACHEBSIZE:     r = "Instruction cache block size. "; break;
	case AT_UCACHEBSIZE:     r = "Unified cache block size. "; break;
	case AT_IGNOREPPC:     r = "Entry should be ignored. "; break;
	case AT_SECURE:     r = "Boolean, was exec setuid-like? "; break;
/* The following ifdefs exist because apparently glibc starting
 * defining new auxillary types */
#ifdef AT_BASE_PLATFORM
	case AT_BASE_PLATFORM:    r= "String identifying real platforms."; break;
#endif
#ifdef AT_RANDOM
	case AT_RANDOM:     r = "Address of 16 random bytes. "; break;
#endif
#ifdef AT_EXECFN
	case AT_EXECFN:     r = "Filename of executable. "; break;
#endif
	case AT_SYSINFO:    r = "Address of VDSO"; break;
	case AT_SYSINFO_EHDR:  r = "AT_SYSINFO_EHDR"; break;
#ifdef AT_L1I_CACHESHAPE
	case AT_L1I_CACHESHAPE: r = "AT_L1I_CACHESHAPE"; break;
	case AT_L1D_CACHESHAPE: r = "AT_L1D_CACHESHAPE"; break;
	case AT_L2_CACHESHAPE:  r = "AT_L2_CACHESHAPE"; break;
	case AT_L3_CACHESHAPE:  r = "AT_L3_CACHESHAPE"; break;
#endif
	}
	return r;
}

void
print_maps(void)
{
	char rbuf[BUFSIZ];
	int fd, cc;

	fd = linux_open("/proc/self/maps", 0, 0);
	while (0 < (cc = linux_read(fd, rbuf, sizeof(rbuf))))
		linux_write(1, rbuf, cc);
	linux_close(fd);
}

void
c_main(int ac, char **av, char **envp)
{
	int i;
	unsigned long *p;
	Elf64_auxv_t *auxvp;

	print_maps();

	print_string(1, "ac ");
	print_long(1, ac);
	print_string(1, "\n");

	print_string(1, "av[0] holds ");
	print_hex(1, (unsigned long)av[0]);
	print_string(1, "\n");

	print_string(1, "envp[0] holds ");
	print_hex(1, (unsigned long)envp[0]);
	print_string(1, "\n");

	p = (unsigned long *)&envp[0];

	while (*p != 0)
		++p;

	print_string(1, "First NULL 8-byte word at ");
	print_hex(1, (unsigned long)p);
	print_string(1, "\n");


	for (i = 0,
		auxvp = (Elf64_auxv_t *)(p + 1);
		auxvp->a_type != AT_NULL;
		++auxvp, ++i
	) {
		char *str;

		print_data(i, auxvp, auxvp->a_type, auxvp->a_un.a_val);

		switch (auxvp->a_type)
		{
		case AT_PLATFORM:
#ifdef AT_EXECFN
		case AT_EXECFN:
#endif
			str = (char *)auxvp->a_un.a_val;
			print_string(1, "\t\"");
			print_string(1, str);
			print_string(1, "\"\n");
			break;
		}
	}
	
	linux_exit(0);
}
