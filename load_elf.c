#include <libstatic/libstatic.h>
#include <elf.h>
#include <sys/mman.h>

#include <ulexec.h>

#define ROUNDUP(x, y)   ((((x)+((y)-1))/(y))*(y))
#define ALIGN(k, v) (((k)+((v)-1))&(~((v)-1)))
#define ALIGNDOWN(k, v) ((unsigned long)(k)&(~((unsigned long)(v)-1)))

void *memcopy(void *dest, const void *src, unsigned long n);

void *
load_elf(char *mapped, int anywhere, Elf64_Ehdr **elf_ehdr, Elf64_Ehdr **ldso_ehdr)
{
	Elf64_Ehdr *hdr;
	Elf64_Phdr *pdr, *interp = NULL;
	int i;
	void *text_segment = NULL;
	void *entry_point = NULL;
	unsigned long initial_vaddr = 0;
	unsigned long brk_addr = 0;
	char buf[128];
	unsigned int mapflags = MAP_PRIVATE|MAP_ANONYMOUS;

	if (!anywhere)
		mapflags |= MAP_FIXED;

	/* Just addresses in mapped-in file. */
	hdr = (Elf64_Ehdr *)mapped;
	pdr = (Elf64_Phdr *)(mapped + hdr->e_phoff);

	entry_point = (void *)hdr->e_entry;

	for (i = 0; i < hdr->e_phnum; ++i, ++pdr)
	{
		unsigned int protflags = 0;
		unsigned long map_addr = 0, rounded_len, k;
		unsigned long unaligned_map_addr = 0;
		void *segment;

		if (pdr->p_type == 0x03)  /* PT_INTERP */
		{
			interp = pdr;
			continue;
		}

		if (pdr->p_type != PT_LOAD)  /* Segment not "loadable" */
			continue;

		if (text_segment != 0 && anywhere)
		{
			unaligned_map_addr
				= (unsigned long)text_segment
				+ ((unsigned long)pdr->p_vaddr - (unsigned long)initial_vaddr)
			;
			map_addr = ALIGNDOWN((unsigned long)unaligned_map_addr, 0x1000);
			mapflags |= MAP_FIXED;
		} else if (!anywhere) {
			map_addr = ALIGNDOWN(pdr->p_vaddr, 0x1000);
		} else {
			map_addr = 0UL;
		}

		if (!anywhere && initial_vaddr == 0)
			initial_vaddr = pdr->p_vaddr;

		/* mmap() freaks out if you give it a non-multiple of pagesize */
		rounded_len = (unsigned long)pdr->p_memsz + ((unsigned long)pdr->p_vaddr % 0x1000);
		rounded_len = ROUNDUP(rounded_len, 0x1000);

		segment = linux_mmap(
			(void *)map_addr,
			rounded_len,
			PROT_WRITE, mapflags, -1, 0
		);

		if (segment == (void *) -1)
		{
			print_string(1, "Failed to mmap() ");
			to_hex(pdr->p_memsz, buf);
			print_string(1, buf);
			print_string(1, " bytes at 0x");
			to_hex(map_addr, buf);
			print_string(1, buf);
			print_string(1, "\n");
			linux_exit(3);
		}

		memcopy(
			!anywhere? (void *)pdr->p_vaddr:
			(void *)((unsigned long)segment + ((unsigned long)pdr->p_vaddr % 0x1000)),
			mapped + pdr->p_offset,
			pdr->p_filesz
		);

		if (!text_segment)
		{
			*elf_ehdr = segment;
			text_segment = segment;
			initial_vaddr = pdr->p_vaddr;
			if (anywhere)
				entry_point = (void *)((unsigned long)entry_point
					- (unsigned long)pdr->p_vaddr
					+ (unsigned long)text_segment);
		}


		if (pdr->p_flags & PF_R)
			protflags |= PROT_READ;
		if (pdr->p_flags & PF_W)
			protflags |= PROT_WRITE;
		if (pdr->p_flags & PF_X)
			protflags |= PROT_EXEC;

		linux_mprotect(segment, rounded_len, protflags);

		k = pdr->p_vaddr + pdr->p_memsz;
		if (k > brk_addr) brk_addr = k;
	}

	if (interp)
	{
		Elf64_Ehdr *junk_ehdr = NULL;
		entry_point = load_elf(map_file(&(((char *)mapped)[interp->p_offset])), 1, ldso_ehdr, &junk_ehdr);
	}

	if (!anywhere)
		linux_brk(ROUNDUP(brk_addr, 0x1000));

	return (void *)entry_point;
}

void *
memcopy(void *dest, const void *src, unsigned long n)
{
    unsigned long i;
    unsigned char *d = (unsigned char *)dest;
    unsigned char *s = (unsigned char *)src;

/*
	char buf[64];

	print_string(1, "load_elf() copying 0x");
	to_hex(n, buf);
	print_string(1, buf);
	print_string(1, " bytes from 0x");
	to_hex((unsigned long)src, buf);
	print_string(1, buf);
	print_string(1, " to 0x");
	to_hex((unsigned long)dest, buf);
	print_string(1, buf);
	print_string(1, "\n");
	linux_read(0, buf, 1);
*/

    for (i = 0; i < n; ++i)
        d[i] = s[i];

    return dest;
}
