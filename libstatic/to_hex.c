#include <stdio.h>
#include <stdlib.h>

void
to_hex(char *p, unsigned long n)
{
	int i;
	for (i = 0; i < 16; ++i)
	{
		char x = ((n >> 60)  & 0xf);
		if (x < (char)10)
			*p++ = x + '0';
		else
			*p++ = (x - 10) + 'a';
		n <<= 4;
	}
	*p = '\0';
}

int
main(int ac, char **av)
{
	char buffer[256];
	unsigned long x = strtoul(av[1], NULL, 0x10);

	to_hex(buffer, x);

	printf("0x%lx converts to: \"%s\"\n", x, buffer);

	return 0;
}
