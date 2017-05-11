void *map_file(char *filename, unsigned long *sz);
void error_msg(char *msg);
void print_address(char *phrase, void *address);
void *load_elf(char *mapped, int anywhere, Elf64_Ehdr **elf, Elf64_Ehdr **ldso);
void unmap(char *progname);
void copy_in(char *progname, void *address);

#define ROUNDUP(x, y)   ((((x)+((y)-1))/(y))*(y))
#define ALIGN(k, v) (((k)+((v)-1))&(~((v)-1)))
#define ALIGNDOWN(k, v) ((unsigned long)(k)&(~((unsigned long)(v)-1)))

struct saved_block {
	int size;
	int cnt;
	char *block;
};

void release_args(struct saved_block *args);
struct saved_block *save_elfauxv(char **envp);
struct saved_block *save_argv(int argc, char **argv);
void *stack_setup(
    struct saved_block *args,
    struct saved_block *envp,
    struct saved_block *auxvp,
    Elf64_Ehdr *ehdr,
    Elf64_Ehdr *ldso
);



