#include <libstatic/libstatic.h>
#include <libstatic/crt.h>

void c_main(int ac, char **av, char **env)
{
	print_string(1, "Hello, world!\n");
	linux_exit(3);
}
