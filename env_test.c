#include <libstatic/libstatic.h>

int main(int ac, char **av, char **env);

/*
 * This is the real entry point.  It passes the initial stack into
 * the C entry point.
 *
 * %rdi - holds value of argc
 * %rsi - holds value of argv, a pointer
 * %rdx - holds value of env, a pointer
 *
 * env = argv + 8*(argc+1)
 */
asm (
	".text\n"
	".global _start\n"
        ".type _start,@function\n"
        "_start:\n\t"
        "movq (%rsp),%rdi\n\t"
        "movq %rsp,%rsi\n\t"
		"addq $8,%rsi\n\t"
		"movq %rdi, %rax\n\t"
		"addq $1,%rax\n\t"
		"movq $8,%rdx\n\n"
		"mul %rdx\n\t"
		"addq %rsi,%rax\n\t"
		"movq %rax, %rdx\n\t"
        "jmp c_main"
	);

void
c_main(int ac, char **av, char **env)
{
	int r = main(ac, av, env);
	linux_exit(r);
}

int
main(int ac, char **av, char **env)
{
	char buffer[32];
	int i;
/*
	if (env == NULL)
		linux_exit(1);
*/

	print_string(1, "argc holds ");
	print_long(1, ac);
	print_string(1, "\n");

	print_string(1, "argv holds ");
	to_hex((unsigned long)av, buffer);
	print_string(1, buffer);
	print_string(1, "\n");

	print_string(1, "env holds ");
	to_hex((unsigned long)env, buffer);
	print_string(1, buffer);
	print_string(1, "\n");

	print_string(1, "\nEnvironment:\n\t");
	for (i = 0; env[i]; ++i)
	{
		print_string(1, env[i]);
		print_string(1, "\n\t");
	}
	return 0;
}
