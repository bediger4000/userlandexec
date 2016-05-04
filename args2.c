/* $Id: args2.c,v 1.2 2014/02/12 17:48:03 bediger Exp $ */
#include <stdio.h>
int
main(int ac, char **av)
{
	int i;
	printf("argc at %p\n", &ac);
	printf("argv[0] at %p\n", &av[0]);
	printf("argc %d\n", ac);
	for (i = 0; i < ac; ++i)
		printf("%d: \"%s\"\n", i, av[i]);
	return 0;
}
