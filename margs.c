#include <libstatic/libstatic.h>

#include <libstatic/crt.h>

void c_main(int ac, char **av, char **env)
{
	int i;

	for (i = 0; i < ac; ++i)
	{
		print_long(1, (unsigned long)i);
		print_string(1, "	");
		print_string(1, "\"");
		print_string(1, av[i]);
		print_string(1, "\"");
		print_string(1, "\r\n");
	}

	linux_exit(13);
}

