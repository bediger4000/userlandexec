#include <libstatic.h>

unsigned long
strlen(const char *s)
{
	unsigned long r = 0;
	for (; s && *s; ++s, ++r);
	return r;
}

char *strchr(const char *s, int c)
{
	char *r = NULL;

	for (; s && *s; ++s)
	{
		if (*s == c)
		{
			r = (char *)s;
			break;
		}
	}
	return r;
}

char *
strcat(char *dest, const char *src)
{
	if (dest && src)
	{
		char *p = dest;
		while (*p)
			++p;

		for (; *src; ++p, ++src)
			*p = *src;
	}

	return dest;
}
char *strstr(const char *str, char *substr)
{
	char *r = NULL;
	int substrl = strlen(substr);
	int strl = strlen(str);

	if (substrl < strl)
	{
		int i;

		for (i = 0; i <= strl - substrl; ++i)
		{
			char *p = (char *)&str[i];
			int j;

			for (j = 0; j < substrl; ++j)
			{
				if (p[j] != substr[j])
					break;
			}

			if (j == substrl)
			{
				r = p;
				break;
			}
		}
	} else if (substrl == strl) {
		int i;
		char *p = (char *)&str[0];
		for (i = 0; i < substrl; ++i)
		{
			if (p[i] != substr[i])
				break;
		}
		if (i == substrl)
			r = p;
	}

	return r;
}
