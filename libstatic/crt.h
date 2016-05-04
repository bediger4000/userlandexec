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
