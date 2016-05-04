#include <libstatic/libstatic.h>
#include <elf.h>
#include <sys/mman.h>

#include <ulexec.h>

#define ALLOCATE(size)  \
      linux_mmap(0, (size), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)

/* call with argc as positive value for main's argv,
 * call with argc == 0 for env. */
struct saved_block *
save_argv(int argc, char **argv)
{
	struct saved_block *r = NULL;
	int i, len;
	char *str;

	if (argc > 0)
		for (i = 0, len = 0; i < argc; ++i)
			len += strlen(argv[i]) + 1;
	else {
		argc = 0;
		char **p = argv;
		while (*p)
		{
			len += strlen(*p) + 1;
			++p;  /* move past ASCII Nul */
			++argc;
		}
	}

	r = ALLOCATE(sizeof(*r));
	r->size = len;
	r->cnt = argc;
	r->block = ALLOCATE(len);

	/* Do it this way because the values of argv[] may not actually
	 * exist as contiguous strings.  We will make them contiguous. */
	for (i = 0, str = r->block; i < argc; i++)
	{
		int j;
		for (j = 0; argv[i][j]; ++j)
			str[j] = argv[i][j];
		str[j] = '\0';
		str += (j + 1);
	}

	return r;
}

void
release_args(struct saved_block *args)
{
	linux_munmap((void *)args->block, args->size);
	linux_munmap((void *)args, sizeof(*args));
}


struct saved_block *
save_elfauxv(char **envp)
{
	struct saved_block *r;
	unsigned long *p;
	int cnt;
	Elf64_auxv_t *q;

	p = (unsigned long *)envp;
	while (*p != 0)
		++p;

	++p; /* skip null word after env */

	for (cnt = 0, q = (Elf64_auxv_t *)p; q->a_type != AT_NULL; ++q)
		++cnt;

	++cnt; /* The AT_NULL final entry */

	r = ALLOCATE(sizeof(*r));
	r->size = sizeof(*q) * cnt;
	r->cnt = cnt;
	r->block = ALLOCATE(r->size);
	memcpy((void *)r->block, (void *)p, r->size);

	return r;
}

/* Returns value for %rsp, the new "bottom of the stack */
void *
stack_setup(
	struct saved_block *args,
	struct saved_block *envp,
    struct saved_block *auxvp,
	Elf64_Ehdr *ehdr,
	Elf64_Ehdr *ldso
)
{
	Elf64_auxv_t	*aux, *excfn = NULL;
	char **av, **ev;
	char	*addr, *str, *rsp;
	unsigned long *ptr;
	int	  i, j;
	char newstack[16384];

	/* Align new stack. */
	rsp = (char *)ALIGN(((unsigned long)&newstack[150]), 16);

	/* 
	 * After returning from
	 * stack_setup(), don't do anything that uses the call stack: that
	 * will roach this newly-constructed stack.
	 */

	ptr = (unsigned long *)rsp;

	*ptr++ = args->cnt; /* set argc */
	av = (char **)ptr;
	ptr += args->cnt;  /* skip over argv[] */
	*ptr++ = 0;

	ev = (char **)ptr;
	ptr += envp->cnt;  /* skip over envp[] */
	*ptr++ = 0;

	aux = (Elf64_auxv_t *)ptr;

	ptr = (unsigned long *)ROUNDUP((unsigned long)ptr + auxvp->size, sizeof(unsigned long));
	
	/* copy ELF auxilliary vector table */
	addr =  (char *)aux;
	for (j = 0; j < auxvp->size; ++j)
		addr[j] = auxvp->block[j];

	/* Fix up a few entries: kernel will have set up the AUXV
	 * for the user-land exec program, mapped in at a low address.
	 * need to fix up a few AUXV entries for the "real" program. */
	for (i = 0; i < auxvp->cnt; ++i)
	{
		switch (aux[i].a_type)
		{
		case AT_PHDR:  aux[i].a_un.a_val = (unsigned long)((char *)ehdr + ehdr->e_phoff); break;
		case AT_PHNUM: aux[i].a_un.a_val = ehdr->e_phnum; break;
		case AT_BASE:  aux[i].a_un.a_val = (unsigned long)ldso; break;
		case AT_ENTRY: aux[i].a_un.a_val = (unsigned long)ehdr->e_entry; break;
#ifdef AT_EXECFN
		case AT_EXECFN: excfn = &(aux[i]); break;
#endif
		}
	}

	*ptr++ = 0;

	/* Copy argv strings onto stack */
	addr =  (char *)ptr;
	str = args->block;

	for (i = 0; i < args->cnt; ++i)
	{
		av[i] = addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;  /* ASCII Nul */
	}

	ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	*ptr = 0;

	/* Copy envp strings onto stack */
	addr =  (char *)ptr;
	str = envp->block;

	for (i = 0; i < envp->cnt; ++i)
	{
		ev[i] = addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;  /* ASCII Nul */
	}

	ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	*ptr = 0;

	/* Executable name at top of stack */
	if (excfn)
	{
		addr =  (char *)ptr;
		str = args->block;
		excfn->a_un.a_val = (unsigned long)addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;  /* ASCII Nul */

		ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	}

	release_args(args);
	release_args(envp);
	release_args(auxvp);

	return ((void *)rsp);
}
