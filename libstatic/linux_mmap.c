/*
void *
linux_mmap(
	void *start,
	unsigned long length,
	int prot,
	int flags,
	int fd,
	unsigned long offset
)
*/
asm (
    ".text\n"
    ".global linux_mmap\n"
        ".type linux_mmap,@function\n"
        "linux_mmap:\n\t"
		"mov    %rcx,%r10\n\t"
		"mov    $0x9,%eax\n\t"
		"syscall \n\t"
		"cmp    $0xfffffffffffff001,%rax\n\t"
		"jae    .Lx1\n\t"
		".Lx2:\n\t"
		"retq   \n\t"
		".Lx1:\n\t"
		"mov    2623486(%rip),%rcx\n\t"
		"xor    %edx,%edx\n\t"
		"sub    %rax,%rdx\n\t"
		"mov    %edx,%fs:(%rcx)\n\t"
		"or     $0xffffffffffffffff,%rax\n\t"
		"jmp    .Lx2\n"
    );
