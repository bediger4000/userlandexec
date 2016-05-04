#include <libstatic.h>

int
print_long(int fd, unsigned long i)
{
	char i_buff[32];
	int l, r;
	to_decimal(i, i_buff);
	for (l = 0; i_buff[l]; ++l);
	r = linux_write(fd, i_buff, l);
	return r;
}

int
print_hex(int fd, unsigned long i)
{
	char i_buff[64];
	int l, r;
	to_hex(i, i_buff);
	for (l = 0; i_buff[l]; ++l);
	r = linux_write(fd, i_buff, l);
	return r;
}

int
print_string(int fd, char *s)
{
	int i, r;
	for (i = 0; s[i]; ++i);
	r = linux_write(fd, s, i);
	return r;
}

int
to_decimal(unsigned long x, char *p)
{
	int count = 0;

	if (x == 0)
		*p++ ='0';
	else {
		unsigned long q, r, b;
		int f = 0;
		b = 10000000000000000000U;

		do {
			q = x/b;
			if (q || f)
			{
				*p++ = ('0' + q);
				++count;
				f = 1;
				x = x%b;
			}
			b /= 10;
		} while (b > 0);

	}

	*p = '\0';

	return count;
}

int
to_hex(unsigned long n, char *p)
{
	int i;
	int count = 0;
	for (i = 0; i < 16; ++i)
	{
		char x = ((n >> 60)  & 0xf);
		if (x < (char)10)
			*p++ = x + '0';
		else
			*p++ = (x - 10) + 'a';
		++count;
		n <<= 4;
	}
	*p = '\0';
	return count;
}
