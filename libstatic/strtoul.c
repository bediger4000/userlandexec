#include <libstatic.h>
unsigned long int
strtoul(const char *nptr, char **endptr, int base)
{
	unsigned long ret = 0;
	int i;

	for (i = 0; nptr[i]; ++i)
	{
		char digit = nptr[i];
		unsigned int value;
		if (digit <= '9')
		{
			value = '0';
		} else if (digit <= 'Z') {
			value = 'A' - 10;
		} else if (digit <= 'z') {
			value = 'a' - 10;
		}
		ret *= base;
		ret += (digit - value);
		if (endptr) *endptr = &(nptr[i]);
	}

	return ret;
}
