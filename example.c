/* $Id: example.c,v 1.1 2014/02/13 02:58:26 bediger Exp $ */
#include <stdio.h>   /* BUFSIZ, snprintf(), fopen(), fread(), etc */
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>    /* strtoul() */

int
main(int ac, char **av, char **env)
{
	void (*ul_exec)(int, char **, char **);
	void *libhdl;
	char *execd;

	execd = av[1];

	/* The flags passed to dlopen() actually make a difference.
	 * RTLD_NOW - "all undefined symbols in the library are resolved before
	 * 	      dlopen()  returns".
	 * RTLD_DEEPBIND - "a self-contained library will use its own symbols in
	 *        preference to global symbols with the same name".
	 *
	 * Since ul_exec() does an munmap() on libc.so and libdl.so, we don't
	 * want the ul_exec() code to reference anything in either library.
	 */

	libhdl = dlopen(execd, RTLD_NOW|RTLD_DEEPBIND);
	if (!libhdl)
	{
		fprintf(stderr, "Problem with loading SO: %s\n",
			dlerror());
		exit(1);
	}

	ul_exec = (void (*)(int, char **, char **))dlsym(libhdl, "ulexec");

	if (!ul_exec)
		fprintf(stderr, "Problem loading symbol: %s\n", dlerror());
	else
		ul_exec(ac, av, env);

	return 0;
}
