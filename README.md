# userlandexec
## userland exec for Linux x86_64

This code emulates an `exec()` system call. That is, it reads an ELF format file,
and loads it into memory at the correct address. It then starts the newly-loaded
executable to running.

All this is usually done by the Linux kernel, so some bizarre things go on.
For starters, the userland exec unmaps the currently-executing ELF file,
so as to be able to put the new ELF file's contents in the right place
in memory.

This code works with 64-bit Linux ELF files, compiled with GCC and linked against glibc.
Other C runtimes (Musl libc, for example) cause errors mysterious to me.

### Building

Run `make` - that should compile `example` and `ulexec.so`. Once you've
done that, you can try it out: 

`./example  ./ulexec.so  /usr/bin/cat /proc/self/maps`

### Fun

`./example  ./ulexec.so  `./example  ./ulexec.so  `./example  ./ulexec.so  /usr/bin/cat /proc/self/maps`

Yes! You can have it overlay itself with another copy of itself.
