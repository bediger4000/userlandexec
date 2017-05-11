extern long errno;

#define NULL 0

int  linux_write(int fd, const void *data, unsigned long len);
int  linux_read(int fd, char *buffer, unsigned long bufferlen);
int  linux_open(const char *pathname, unsigned long flags, unsigned long mode);
int linux_close(int fd);
int linux_stat(const char *filename, void *buf);
void linux_exit(int code);
int  print_long(int fd, unsigned long i);
int  print_hex(int fd, unsigned long i);
int  print_string(int fd, char *s);
int  to_decimal(unsigned long x, char *p);
int  to_hex(unsigned long x, char *p);
void *linux_mmap(void *start, unsigned long length, int prot, int flags, int fd, unsigned long offset);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
int linux_munmap(void *start, unsigned long length);
int linux_mprotect(void *addr, unsigned long len, int prot);
unsigned long file_size(char *filename);
int linux_getpagesize(void);
void linux_brk(unsigned long addr);


unsigned long strlen(const char *s);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *str, char *substr);
void *memcpy(void *dest, const void *src, unsigned long n);
char *strcat(char *dest, const char *src);

#define PGSZ 0x1000
